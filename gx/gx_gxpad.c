#include <gccore.h>
#include <string.h>
#include <unistd.h>

#include "gx_gxpad.h"

#define GX_NO_BUTTON 0x0
#define WPAD_EXP_GAMECUBE WPAD_EXP_WIIBOARD + 1
#define WPAD_EXP_WIIMOTE WPAD_EXP_NONE

enum {
	GX_B = 0,
	GX_Y,		GX_1,
	GX_MINUS,
	GX_START,	GX_PLUS,
	GX_UP,
	GX_DOWN,
	GX_LEFT,
	GX_RIGHT,
	GX_A,
	GX_X,		GX_2,
	GX_L,		GX_Z,
	GX_R,		GX_C,
	GX_ZL,
	GX_ZR,
	GX_HOME,
	GX_NA,
};

enum {
	GX_NO_AXIS,
	GX_GC_STND,
	GX_GX_INVD,
	GX_A_WII_STND,
	GX_A_WII_INVD,
};

static const char* gxblabels[] = {
	"B",
	"Y",		"1",
	"Minus",
	"Start",	"Plus",
	"Up",
	"Down",
	"Left",
	"Right",
	"A",
	"X",		"2",
	"L",		"Z",
	"R",		"C",
	"ZL",
	"ZR",
	"Home",
	"N/A",
};

static const gxpadsetup valid_pad_config[] = {
			{"Gamecube Pad",
				{PAD_BUTTON_B, PAD_BUTTON_Y, PAD_TRIGGER_Z, PAD_BUTTON_START,
				PAD_BUTTON_UP, PAD_BUTTON_DOWN, PAD_BUTTON_LEFT, PAD_BUTTON_RIGHT,
				PAD_BUTTON_A, PAD_BUTTON_X, PAD_TRIGGER_L, PAD_TRIGGER_R,
				GX_NO_BUTTON, GX_NO_BUTTON, PAD_BUTTON_START | PAD_TRIGGER_Z},
				{GX_B, GX_1, GX_Z, GX_START, GX_UP, GX_DOWN, GX_LEFT, GX_RIGHT,
				GX_A, GX_X, GX_L, GX_R, GX_NA, GX_NA, GX_HOME},
				4, {GX_GC_STND, GX_GX_INVD, GX_GC_STND, GX_GX_INVD},
				WPAD_EXP_GAMECUBE},
			{"Wiimote",
				{WPAD_BUTTON_B, WPAD_BUTTON_1, WPAD_BUTTON_MINUS, WPAD_BUTTON_PLUS,
				WPAD_BUTTON_UP, WPAD_BUTTON_DOWN, WPAD_BUTTON_LEFT, WPAD_BUTTON_RIGHT,
				WPAD_BUTTON_A, WPAD_BUTTON_2, GX_NO_BUTTON, GX_NO_BUTTON,
				GX_NO_BUTTON, GX_NO_BUTTON, WPAD_BUTTON_HOME},
				{GX_B, GX_1, GX_MINUS, GX_PLUS, GX_UP, GX_DOWN, GX_LEFT, GX_RIGHT,
				GX_A, GX_2, GX_NA, GX_NA, GX_NA, GX_NA, GX_HOME},
				0, {GX_NO_AXIS, GX_NO_AXIS, GX_NO_AXIS, GX_NO_AXIS},
				WPAD_EXP_WIIMOTE},
			{"Wiimote + Nunchuk",
				{WPAD_BUTTON_B, WPAD_BUTTON_1, WPAD_BUTTON_MINUS, WPAD_BUTTON_PLUS,
				WPAD_BUTTON_UP, WPAD_BUTTON_DOWN, WPAD_BUTTON_LEFT, WPAD_BUTTON_RIGHT,
				WPAD_BUTTON_A, WPAD_BUTTON_2, WPAD_NUNCHUK_BUTTON_Z, WPAD_NUNCHUK_BUTTON_C,
				GX_NO_BUTTON, GX_NO_BUTTON, WPAD_BUTTON_HOME},
				{GX_B, GX_1, GX_MINUS, GX_PLUS, GX_UP, GX_DOWN, GX_LEFT, GX_RIGHT,
				GX_A, GX_2, GX_Z, GX_C, GX_NA, GX_NA, GX_HOME},
				2, {GX_A_WII_STND, GX_A_WII_STND, GX_NO_AXIS, GX_NO_AXIS},
				WPAD_EXP_NUNCHUK},
			{"Classic",
				{WPAD_CLASSIC_BUTTON_B, WPAD_CLASSIC_BUTTON_Y, WPAD_CLASSIC_BUTTON_MINUS, WPAD_CLASSIC_BUTTON_PLUS,
				WPAD_CLASSIC_BUTTON_UP, WPAD_CLASSIC_BUTTON_DOWN, WPAD_CLASSIC_BUTTON_LEFT, WPAD_CLASSIC_BUTTON_RIGHT,
				WPAD_CLASSIC_BUTTON_A, WPAD_CLASSIC_BUTTON_X, WPAD_CLASSIC_BUTTON_FULL_L, WPAD_CLASSIC_BUTTON_FULL_R,
				WPAD_CLASSIC_BUTTON_ZL, WPAD_CLASSIC_BUTTON_ZR, WPAD_CLASSIC_BUTTON_HOME},
				{GX_B, GX_Y, GX_MINUS, GX_PLUS, GX_UP, GX_DOWN, GX_LEFT, GX_RIGHT,
				GX_A, GX_X, GX_L, GX_R, GX_ZL, GX_ZR, GX_HOME},
				4, {GX_A_WII_STND, GX_A_WII_INVD, GX_A_WII_STND, GX_A_WII_INVD},
				WPAD_EXP_CLASSIC},
		};

#define GX_MAX_PADS 8

static bool _inited = false;
static bool _pad_detected = false;
static struct gxpad *_pad_list[GX_MAX_PADS]; /* just hold a pointer for each possible pad */
static lwp_t _pad_thread = LWP_THREAD_NULL;
static bool _pad_thread_running = false;
static bool _pad_thread_quit = false;
static PADStatus _gcpdata[PAD_CHANMAX];

static int32_t _rem_pad(struct gxpad *pad) {
	free(pad);
	pad = NULL;
}

void gxpad_shutdown(void) {
	/* First kill the running thread */
	_pad_thread_quit = true;
	if (_pad_thread_running) {
		LWP_JoinThread(_pad_thread, NULL);
		_pad_thread_running = false;
	}

	uint8_t i;
	for (i = 0;i < GX_MAX_PADS; i++) {
		_rem_pad(_pad_list[i]);
	}

	_inited = false;
}

static int32_t _read_cb(int32_t result, void *usrdata) {
	struct gxpad *pad = (struct gxpad*)usrdata;
	uint8_t i;
	const butsetup *b;

	if (result) {
		/* Verify if the pad has multiple pad info in the package. If yes, only process the configured. */
		if (pad->config->multipad==0 || pad->config->multipad==pad->hid_buffer[0]) {
			pad->b_state = 0; /* clear button state */
			for (i = 0; i < BUTTON_SET; i++) {
				b = &pad->config->buttons[i];
				if (b->label < B_NA) {
					pad->b_state |= ((pad->hid_buffer[b->offset] & b->mask) == b->mask) ? (1ULL << i) : 0;
				}
			}

			for (i = 0; i < pad->config->num_analogs; i++) {
				pad->a_state[i] = (int8_t)pad->hid_buffer[pad->config->analogs[i].offset];
			}
		}
	}
	pad->reading = false;
	return result;
}

static bool _new_hid_device(int32_t id) {
    uint8_t i;
	for (i = 0; i < GX_MAX_PADS; ++i) {
        if (_pad_list[i] && _pad_list[i]->device_id == id) return false;
    }
    return true;
}

static int8_t _add_pad(int16_t cfg_idx, int32_t p_slot) {
	uint8_t i;

	/* look for an empty slot to assign it */
	for (i = 0; i < GX_MAX_PADS; i++)	{
		/* find an empty slot */
		if (_pad_list[i] == NULL) {
			/* alloc and setup the pad data in the slot */
			_pad_list[i] = malloc(sizeof(struct gxpad));
			/* if succesfully allocated */
			if (_pad_list[i]) {
				memset(_pad_list[i], 0, sizeof(struct gxpad));
				_pad_list[i]->config = &valid_pad_config[cfg_idx];
				_pad_list[i]->p_slot = p_slot;

				return i; /* return the slot which we added it */
			}
		}
	}

    return -1; /* if this point is reached, no pad was added */
}

static int8_t _scan_devices() {
    uint8_t slot;
    uint32_t _rpads = 0;
    uint32_t conn, ptype = 0;
    static bool g_lon[PAD_CHANMAX];
    static uint8_t w_pst[PAD_CHANMAX] = {WPAD_EXP_UNKNOWN, WPAD_EXP_UNKNOWN, WPAD_EXP_UNKNOWN, WPAD_EXP_UNKNOWN};

	/* First scan for GC controllers */
	PAD_Read(gcpdata);
	for (slot = 0; slot < PAD_CHANMAX; ++slot) {
		switch (gcpdata[slot].err) {
			case PAD_ERR_NONE:
			case PAD_ERR_NOT_READY:
			case PAD_ERR_TRANSFER:
				if (g_lon[slot] == false) {
					_add_pad(WPAD_EXP_GAMECUBE, slot);
					g_lon[slot] = true;
				}
				break;
			case PAD_ERR_NO_CONTROLLER:
				_rpads |= PAD_CHAN0_BIT >> slot;
				if (g_lon[slot] == true) {
					_rem_pad(WPAD_EXP_GAMECUBE, slot);
					g_lon[slot] = false;
				}
				break;
		}
	}
	/* reset any disconnectd pad */
	if _rpads PAD_Reset(_rpads);

	/* now go for the wii controllers */
    WPAD_ReadPending(WPAD_CHAN_ALL, NULL);
	for (slot = 0; slot < PAD_CHANMAX; ++slot) {
		_conn = WPAD_Probe(slot, &ptype);
		switch (_conn) {
			case WPAD_ERR_NONE:
				if (w_pst[slot] == WPAD_EXP_UNKNOWN) {
					_add_pad(ptype, slot);
					w_pst[slot] = ptype;
					break;
				}
				/* in case the pad is connected but a different type
				 * we want to remove the pad so we can detect it later */
			case WPAD_ERR_NOT_READY:
			case WPAD_ERR_NO_CONTROLLER:
				if (w_pst[slot] != WPAD_EXP_UNKNOWN) {
					_rem_pad(w_pst[slot], slot);
					w_pst[slot] = WPAD_EXP_UNKNOWN;
				}
				break;
		}
	}

    return 0;
}

static inline int32_t _read_pad(struct gxpad *pad) {
	return USB_ReadIntrMsgAsync(pad->fd, pad->epAddress, pad->mpSize, pad->hid_buffer, (usbcallback)_read_cb, pad);
}

static void * _pad_polling(void *arg) {
	uint8_t i;

	while (!_pad_thread_quit) {

		/* Scan for any device change */
		_scan_devices();

		/* read data for each available pad */
		for(i = 0; i < GX_MAX_PADS; i++) {
			if (_pad_list[i]) _read_pad(_pad_list[i]);
		}

		/* wait 16.66 ms to process the available pads again */
		usleep(16666);
	}

	return NULL;
}

bool gxpad_init(uint8_t slots, bool skip_usb) {
	if (!_inited) {

		if (!skip_usb) { /* In case other party is already handling usb setup */
			USB_Initialize();
		}

		/* don't allow more than the max possible pads number */
		GX_MAX_PADS = (slots > GX_MAX_PADS) || (slots <= 0) ? GX_MAX_PADS : slots;

		/* Set to null the whole pad list */
		memset(_pad_list, 0, sizeof(_pad_list));

		if (!_pad_thread_running) {
			// start the polling thread
			_pad_thread_quit = false;
			if (LWP_CreateThread(&_pad_thread, _pad_polling, NULL, NULL, 0, 65) < 0) {
				_pad_thread_running = false;
				gxpad_shutdown(skip_usb);
				return 0;
			}
			_pad_thread_running = true;
		}

		USB_DeviceChangeNotifyAsync(USB_CLASS_HID, _change_cb, NULL);
		_pad_detected = true;  /* try open any existing gxpad device */
		_inited = true;
    }
    return _inited;
}

uint64_t gxpad_buttons(uint8_t pad_idx) {
	if (pad_idx < GX_MAX_PADS && _pad_list[pad_idx]) {
		return _pad_list[pad_idx]->b_state;
	}

	return 0ULL;
};

const char *gxpad_label(uint8_t pad_idx, uint8_t b_idx) {
	if (pad_idx < GX_MAX_PADS && b_idx < BUTTON_SET && _pad_list[pad_idx]) {
		return blabels[_pad_list[pad_idx]->config->buttons[b_idx].label];
	}

	return blabels[B_NA];
}

bool gxpad_buttonavail(uint8_t pad_idx, uint8_t b_idx) {
	if (pad_idx < GX_MAX_PADS && b_idx < BUTTON_SET && _pad_list[pad_idx]) {
		return (_pad_list[pad_idx]->config->buttons[b_idx].mask > 0);
	}

	return false;
}

const char *gxpad_padname(uint8_t pad_idx) {
	if (pad_idx < GX_MAX_PADS && _pad_list[pad_idx]) {
		return _pad_list[pad_idx]->config->name;
	}

	return "No pad in USB slot";
}

static inline int16_t _normalize_a(int16_t a_value, uint8_t axis_type) {
	int16_t h;

	switch (axis_type) {
		case A_STANDARD:
			a_value = (a_value - 128) << 8; break;
		case A_INVERTED:
			a_value = (127 - a_value) << 8;	break;
		case A_HAT_PSXH:
			h = HAT_GET(a_value);
			a_value = HAT_LEFT(h) ? HAT_MIN: HAT_RIGHT(h) ? HAT_MAX: 0; break;
		case A_HAT_PSXV:
			h = HAT_GET(a_value);
			a_value = HAT_UP(h) ? HAT_MIN: HAT_DOWN(h) ? HAT_MAX: 0; break;
		default	:
			a_value = 0;
	}

	return a_value;
}

int16_t gxpad_analog(uint8_t pad_idx, uint8_t a_idx) {
	if (pad_idx < GX_MAX_PADS) {
		struct gxpad *pad = _pad_list[pad_idx];
		if (pad && a_idx < pad->config->num_analogs) {
			return _normalize_a(pad->a_state[a_idx], pad->config->analogs[a_idx].type);
		}
	}

	return 0;
}

bool gxpad_avail(uint8_t pad_idx) {
	if (pad_idx < GX_MAX_PADS) {
		return (_pad_list[pad_idx] != NULL);
	}

	return false;
}

uint8_t gxpad_nanalogs(uint8_t pad_idx) {
	if (pad_idx < GX_MAX_PADS && _pad_list[pad_idx]) {
		return _pad_list[pad_idx]->config->num_analogs;
	}

	return 0;
}
