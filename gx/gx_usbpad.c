#include <malloc.h>
#include <gccore.h>
#include <string.h>
#include <unistd.h>
#include "gx_usbpad.h"

#define MAX_DEVICES 8
#define MAX_PADS 4
#define MAX_NAME_LEN 32
#define MIN_HID_SIZE 32
#define MAX_HID_SIZE 64

#define NUM_USB_CLASSES 2
#define STD_IN_ENDPOINT 0x81
#define DEVICE_NAME 0x02
#define LANGUAGE_EN 0x0409
#define ENABLE_CONTROLLER 0x01

#define BUTTON_SET 17
#define AXIS_SET 4

typedef struct _butsetup {
	uint8_t	offset;
	uint8_t	mask;
	uint8_t	label;
} butsetup;

typedef struct _axisetup {
	uint8_t		offset;
	uint8_t		type;
} axisetup;

typedef struct _padsetup {
	uint16_t    vid;
	uint16_t    pid;
	char        name[MAX_NAME_LEN];
	uint8_t     multipad;
	uint8_t     num_analogs;
	butsetup    buttons[BUTTON_SET];
	axisetup    analogs[AXIS_SET];
	void        (*set_operational)(void *data);
	void        (*rumble_pad)(uint8_t pad_idx, uint8_t action);
} padsetup;

struct usbpad {
	uint64_t			b_state;
	const padsetup		*config;
	int32_t				device_id;
	int32_t				fd;
	uint16_t			mpInSize;
	uint16_t			mpOutSize;
	int16_t				a_state[AXIS_SET];
	bool				reading;
	uint8_t				*hid_buffer;
	uint8_t				epInAddr;
	uint8_t				epOutAddr;
};

/* Include at this point so the structures are available to it */
#include "gx_padsetup.h"

/* Supported USB classes */
const uint8_t _USB_CLASS_XINPUT	= 0xFF;
const uint8_t _USB_CLASS_HID 	= 0x03;

static bool _inited = false;
static bool _pad_detected = false;
static bool _rem_cb = false; /* helps to know if it has just removed a pad from the usb iface. */
static uint8_t _usbslots = 0;
static struct usbpad *_pad_list[MAX_PADS]; /* just hold a pointer for each possible pad */
static lwp_t _pad_thread = LWP_THREAD_NULL;
static bool _pad_thread_running = false;
static bool _pad_thread_quit = false;

static void _free_pad(struct usbpad **pad) {
	if ((*pad)->hid_buffer) {
		free((*pad)->hid_buffer);
	}
	free((*pad));
	(*pad) = NULL;
}

static int32_t _rem_pad(struct usbpad *pad) {
	int32_t res;
	if (pad && pad->fd > 0)	{
		res = USB_CloseDevice(&pad->fd);
	}
	return res;
}

void usbpad_shutdown(bool skip_usb) {
	/* First kill the running thread */
	_pad_thread_quit = true;
	if (_pad_thread_running) {
		LWP_JoinThread(_pad_thread, NULL);
		_pad_thread_running = false;
	}

	uint8_t i;
	for (i = 0;i < _usbslots; i++)	{
		_rem_pad(_pad_list[i]);
	}

	if (!skip_usb) { /* In case other party is already handling usb setup */
		USB_Deinitialize();
	}
	_inited = false;
}

static int32_t _change_cb(int32_t result, void *usrdata) {
	if (!_rem_cb) {
		/* As it's not coming from the removal callback
		then we	detected a new device being inserted */
		_pad_detected = true;
	}
	else {
		_rem_cb = false;
	}

	uint8_t *hid_class = (uint8_t *)usrdata;

	/* Re-apply the callback for future connections changes with the correct class */
	USB_DeviceChangeNotifyAsync(*hid_class, _change_cb, (void *)hid_class);

	return result;
}

static int32_t _read_cb(int32_t result, void *usrdata) {
	struct usbpad *pad = (struct usbpad*)usrdata;
	uint8_t i;
	const butsetup *b;

	if (result > 0) {
		/* Verify if the pad has multiple pad info in the package. If yes, only process the configured. */
		if (pad->config->multipad == 0 || pad->config->multipad == pad->hid_buffer[0]) {
			pad->b_state = 0; /* clear button state */
			for (i = 0; i < BUTTON_SET; i++) {
				b = &pad->config->buttons[i];
				if (b->label < B_NA) {
					pad->b_state |= ((pad->hid_buffer[b->offset] & b->mask) == b->mask) ? (1ULL << i) : 0;
				}
			}

			for (i = 0; i < pad->config->num_analogs; i++) {
				uint8_t b = pad->config->analogs[i].offset;
				/* Assume here the analog uses 2 bytes as data. Switch order to use with MSB */
				pad->a_state[i] = (int16_t) (pad->hid_buffer[b] | pad->hid_buffer[b + 1] << 8);
			}
		}
	}
	pad->reading = false;
	return result;
}

static int32_t _removal_cb(int32_t result, void *usrdata) {
	struct usbpad **pad = (struct usbpad**)usrdata;

	if ((*pad)) _free_pad(pad);

	_rem_cb = true; /* inform we already pass thru the removal callback */
	return 0;
}

static bool _valid_pad_config(uint16_t vid, uint16_t pid, int16_t *cfg_idx) {
	int16_t i;
	for (i = 0; i < (sizeof(valid_pad_config) / sizeof(padsetup)); ++i) {
		if ((vid == valid_pad_config[i].vid) && (pid == valid_pad_config[i].pid)) {
			*cfg_idx = i;
			return true;
		}
	}

	*cfg_idx = -1;
	return false;
}

static void _ps3_set_operational(void *data) {
   struct usbpad *pad = (struct usbpad*)data;
	int32_t r;
	uint8_t ATTRIBUTE_ALIGN(32) buffer[4] = {0x42, 0x0c, 0x00, 0x00}; /* Special command to enable Sixaxis */
	/* Sometimes it fails so we should keep trying until success */
	do {
		r = USB_WriteCtrlMsg(pad->fd, USB_REQTYPE_INTERFACE_SET, USB_REQ_SETREPORT, (USB_REPTYPE_FEATURE<<8) | 0xf4, 0x0, sizeof(buffer), buffer);
	} while (r < 0);
}

static void _ps3_rumble(uint8_t pad_idx, uint8_t action) {
   uint8_t ATTRIBUTE_ALIGN(32) ps3Data[] =
   {
      0x01, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x27, 0x10, 0x00, 0x32, 
      0xFF, 0x27, 0x10, 0x00, 0x32, 0xFF, 0x27, 0x10, 0x00, 0x32, 0xFF, 0x27, 0x10, 0x00, 0x32, 0x00, 
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
      0x00,
   };
   struct usbpad *pad = _pad_list[pad_idx];

   ps3Data[3] = action == 1 ? 0xFF : 0x00;
   ps3Data[5] = action == 1 ? 0xFF : 0x00;
   
   USB_WriteIntrMsg(pad->fd, pad->epOutAddr, sizeof(ps3Data), ps3Data);
}

static void _gcwiiu_set_operational(void *data) {
   struct usbpad *pad = (struct usbpad*)data;
	int32_t r;
	uint8_t ATTRIBUTE_ALIGN(32) buffer[1] = {0x13}; /* Special command to enable reading */
	/* Sometimes it fails so we should keep trying until success */
	do {
		r = USB_WriteIntrMsg(pad->fd, pad->epOutAddr, sizeof(buffer), buffer);
	} while (r < 0);
}

static void _gcwiiu_rumble(uint8_t pad_idx, uint8_t action) {
   uint8_t ATTRIBUTE_ALIGN(32) gcData[5];
   struct usbpad *pad = _pad_list[pad_idx];

   /* action may contain the bits of which
    * control to turn on/off. We have 4
    * starting from right to left */
   gcData[0] = 0x11;
   gcData[1] = action & 1;
   /*gcData[2] = (action >> 1) & 1;
   gcData[3] = (action >> 2) & 1;
   gcData[4] = (action >> 3) & 1;*/

   USB_WriteIntrMsg(pad->fd, pad->epOutAddr, sizeof(gcData), gcData); 
}

static void _xinput_set_operational(void *data) {
   struct usbpad *pad = (struct usbpad*)data;
	int32_t r;
	uint8_t ATTRIBUTE_ALIGN(32) buffer[3] = {0x01, 0x03, 0x06};

	/* Turn on led */
	do {
		r = USB_WriteIntrMsg(pad->fd, pad->epOutAddr, sizeof(buffer), buffer);
	} while (r < 0);
}

static void _xinput_rumble(uint8_t pad_idx, uint8_t action) {
   uint8_t ATTRIBUTE_ALIGN(32) xData[] = {0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   struct usbpad *pad = _pad_list[pad_idx];

   xData[3] = action == 1 ? 0x64 : 0x00;
   xData[4] = action == 1 ? 0x64 : 0x00;
   
   USB_WriteIntrMsg(pad->fd, pad->epOutAddr, sizeof(xData), xData);
}

static bool _new_hid_device(int32_t id) {
	uint8_t i;
	for (i = 0; i < _usbslots; ++i) {
		if (_pad_list[i] && _pad_list[i]->device_id == id) return false;
	}
	return true;
}

static int8_t _add_pad(int32_t deviceId, int32_t fd, int16_t cfgIdx, uint8_t epInAddr, uint16_t mpInSize, uint8_t epOutAddr, uint16_t mpOutSize) {
	uint8_t i;

	/* look for an empty slot to assign it */
	for (i = 0; i < _usbslots; i++)	{
		/* find an empty slot */
		if (_pad_list[i] == NULL) {
			/* alloc and setup the pad data in the slot */
			_pad_list[i] = malloc(sizeof(struct usbpad));
			/* if succesfully allocated */
			if (_pad_list[i]) {
				memset(_pad_list[i], 0, sizeof(struct usbpad));

				_pad_list[i]->device_id = deviceId;
				_pad_list[i]->fd = fd;
				_pad_list[i]->epInAddr = epInAddr;
				_pad_list[i]->mpInSize = mpInSize;
				_pad_list[i]->epOutAddr = epOutAddr;
				_pad_list[i]->mpOutSize = mpOutSize;
				_pad_list[i]->config = &valid_pad_config[cfgIdx];
				/* hid_buffer size needs to be multiple of 32 as it needs to be aligned to 32 bytes */
				_pad_list[i]->hid_buffer = (uint8_t *)memalign(32, (mpInSize > MIN_HID_SIZE) ? MAX_HID_SIZE : MIN_HID_SIZE);

				/* Get controller name from the device, we don't use it but some controllers expect the query to work */
				USB_GetAsciiString(_pad_list[i]->fd, DEVICE_NAME, LANGUAGE_EN, sizeof(_pad_list[i]->hid_buffer), _pad_list[i]->hid_buffer);

				/* Make the device operational */
				USB_SetConfiguration(_pad_list[i]->fd, ENABLE_CONTROLLER);

				/* extra stuff for special controllers */
				if (_pad_list[i]->config->set_operational)
					_pad_list[i]->config->set_operational(_pad_list[i]);

				USB_DeviceRemovalNotifyAsync(_pad_list[i]->fd, &_removal_cb, &_pad_list[i]);

				return i; /* return the slot which we added it */
			}
		}
	}

	return -1; /* if this point is reached, no pad was added */
}

static int8_t _get_valid_endpoints(int32_t fd, uint8_t *epInAddr, uint16_t *mpInSize, uint8_t *epOutAddr, uint16_t *mpOutSize) {
	uint32_t iConf, iInterface, iEp;
	usb_devdesc udd;
	usb_configurationdesc *ucd;
	usb_interfacedesc *uid;
	usb_endpointdesc *ued;
	int8_t retVal = -1;

	if (USB_GetDescriptors(fd, &udd) < 0) {
		return retVal; /* Error getting descriptors */
	}
	
	/* Look for valid Interrupt Input Endpoint; And optionally an Output
	 * Endpoint used for special commands in certain gamepads */
	for (iConf = 0; iConf < udd.bNumConfigurations; iConf++) {

		ucd = &udd.configurations[iConf];
		for (iInterface = 0; iInterface < ucd->bNumInterfaces; iInterface++) {

			/* Reset values on each Interface iteration */
			*epInAddr = *mpInSize = *epOutAddr = *mpOutSize = 0x00;

			uid = &ucd->interfaces[iInterface];
			for (iEp = 0; iEp < uid->bNumEndpoints; iEp++) {

				ued = &uid->endpoints[iEp];
				/* Only interested in Interrupt endpoints */
				if ((ued->bmAttributes & 0x03) == USB_ENDPOINT_INTERRUPT) {

					/* Only want IN 0x81 (and maybe 0x82) endpoint later... */
					if ((ued->bEndpointAddress & 0x80) == USB_ENDPOINT_IN && ued->bEndpointAddress == STD_IN_ENDPOINT) {
						*epInAddr = ued->bEndpointAddress;
						*mpInSize = ued->wMaxPacketSize;

					/* Any OUT endpoint  is OK */
					} else if ((ued->bEndpointAddress & 0x80) == USB_ENDPOINT_OUT) {
						*epOutAddr = ued->bEndpointAddress;
						*mpOutSize = ued->wMaxPacketSize;
					}
				}
			}
			
			/* Done if after searching the interface we found a valid endpoint */
			if (*epInAddr != 0x00) {
				retVal = 0;
				goto SEARCH_DONE;
			}
		}
	}

SEARCH_DONE:
	USB_FreeDescriptors(&udd);
	return retVal;
}

static int8_t _scan_devices() {
	uint8_t i, c, padAdded, devCount = 0;
	usb_device_entry dev_e[MAX_DEVICES];
	int16_t cfgIdx;
	int32_t fd = 0;
	uint8_t epInAddr;
	uint16_t mpInSize;
	uint8_t epOutAddr;
	uint16_t mpOutSize;

	/* Do the process 2 times: One for HID devices and a 2nd for XBOX360 class */
	for (c = 0; c < NUM_USB_CLASSES; c++) {

		if (USB_GetDeviceList(dev_e, MAX_DEVICES, c ? _USB_CLASS_XINPUT : _USB_CLASS_HID, &devCount) < 0) {
			return -1;
		}

		/* wait 250 milliseconds before start scanning */
		usleep(1000 * 250);

		for (i = 0; i < devCount; ++i) {

			if (!_new_hid_device(dev_e[i].device_id)) continue;
			if (!_valid_pad_config(dev_e[i].vid, dev_e[i].pid, &cfgIdx)) continue;
			if (USB_OpenDevice(dev_e[i].device_id, dev_e[i].vid, dev_e[i].pid, &fd) < 0) continue;

			/* Add the gamepad if valid endpoint(s) are found */
			if (_get_valid_endpoints(fd, &epInAddr, &mpInSize, &epOutAddr, &mpOutSize) == 0) {
				padAdded = _add_pad(dev_e[i].device_id, fd, cfgIdx, epInAddr, mpInSize, epOutAddr, mpOutSize);
			}

			/* Close device if not succesfully added */
			if (padAdded < 0) USB_CloseDevice(&fd);
		}
	}

	return 0;
}

static inline int32_t _read_pad(struct usbpad *pad) {
	return USB_ReadIntrMsgAsync(pad->fd, pad->epInAddr, pad->mpInSize, pad->hid_buffer, (usbcallback)_read_cb, pad);
}

static void * _pad_polling(void *arg) {
	uint8_t i;

	while (!_pad_thread_quit) {

		/* if a device is detected, try to add it. */
		if (_pad_detected) {
			/* As we are processing the detected device, we turn off the flag. */
			_pad_detected = false;
			_scan_devices(); /* Scan and add any new detected pad */
		}

		/* read data from each available pad */
		for(i = 0; i < _usbslots; i++)	{
			if (_pad_list[i] && !_pad_list[i]->reading) {
				_pad_list[i]->reading = true;
				_read_pad(_pad_list[i]);
			}
		}

		/* wait 8 ms to process the available pads again */
		usleep(8000);
	}

	return NULL;
}

bool usbpad_init(uint8_t slots, bool skip_usb) {
	if (!_inited) {

		if (!skip_usb) { /* In case other party is already handling usb setup */
			USB_Initialize();
		}

		/* don't allow more than the max possible pads number */
		_usbslots = (slots > MAX_PADS) || (slots <= 0) ? MAX_PADS : slots;

		/* Set to null the whole pad list */
		memset(_pad_list, 0, sizeof(_pad_list));

		if (!_pad_thread_running) {
			/* start the polling thread */
			_pad_thread_quit = false;
			if (LWP_CreateThread(&_pad_thread, _pad_polling, NULL, NULL, 0, 65) < 0) {
				_pad_thread_running = false;
				usbpad_shutdown(skip_usb);
				return 0;
			}
			_pad_thread_running = true;
		}

		/* Set callbacks to detect when a device is plugged/unplugged for each supported class */
		USB_DeviceChangeNotifyAsync(_USB_CLASS_HID, _change_cb, (void *)&_USB_CLASS_HID);
		USB_DeviceChangeNotifyAsync(_USB_CLASS_XINPUT, _change_cb, (void *)&_USB_CLASS_XINPUT);
		_pad_detected = true; /* try open any existing usbpad device */
		_inited = true;
	}
	return _inited;
}

uint64_t usbpad_buttons(uint8_t pad_idx) {
	if (pad_idx < _usbslots && _pad_list[pad_idx]) {
		return _pad_list[pad_idx]->b_state;
	}

	return 0ULL;
};

const char *usbpad_label(uint8_t pad_idx, uint8_t b_idx) {
	if (pad_idx < _usbslots && b_idx < BUTTON_SET && _pad_list[pad_idx]) {
		return blabels[_pad_list[pad_idx]->config->buttons[b_idx].label];
	}

	return blabels[B_NA];
}

bool usbpad_buttonavail(uint8_t pad_idx, uint8_t b_idx) {
	if (pad_idx < _usbslots && b_idx < BUTTON_SET && _pad_list[pad_idx]) {
		return (_pad_list[pad_idx]->config->buttons[b_idx].mask > 0);
	}

	return false;
}

const char *usbpad_padname(uint8_t pad_idx) {
	if (pad_idx < _usbslots && _pad_list[pad_idx]) {
		return _pad_list[pad_idx]->config->name;
	}

	return NULL;
}

static inline int16_t _normalize_a(int16_t a_value, uint8_t axis_type) {

	switch (axis_type) {
		case A_STANDARD:
			/* Only the first byte contains the data */
			a_value = ((a_value & 0x00FF) - 128) << 8; break;
		case A_INVERTED:
			/* Only the first byte contains the data */
			a_value = (127 - (a_value & 0x00FF)) << 8; break;
		case A_X360_STD:
			/* No changes needed */; break;
		case A_X360_INV:
			a_value = ~a_value; break;
		default :
			a_value = 0;
	}

	return a_value;
}

int16_t usbpad_analog(uint8_t pad_idx, uint8_t a_idx) {
	if (pad_idx < _usbslots) {
		struct usbpad *pad = _pad_list[pad_idx];
		if (pad && a_idx < pad->config->num_analogs) {
			return _normalize_a(pad->a_state[a_idx], pad->config->analogs[a_idx].type);
		}
	}

	return 0;
}

void usbpad_rumbleoff(uint8_t pad_idx) {
	if (pad_idx < _usbslots) {
		struct usbpad *pad = _pad_list[pad_idx];
		if (pad && pad->config->rumble_pad)
            pad->config->rumble_pad(pad_idx, 0);
	}
}

void usbpad_rumbleon(uint8_t pad_idx) {
	if (pad_idx < _usbslots) {
		struct usbpad *pad = _pad_list[pad_idx];
		if (pad && pad->config->rumble_pad)
            pad->config->rumble_pad(pad_idx, 1);
	}
}

bool usbpad_avail(uint8_t pad_idx) {
	if (pad_idx < _usbslots) {
		return (_pad_list[pad_idx] != NULL);
	}

	return false;
}

