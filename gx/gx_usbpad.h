#ifndef _GX_USBPAD_H_
#define _GX_USBPAD_H_

#define BUTTON_SET 17
#define AXIS_SET 6
#define MAX_NAME_LEN 32

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
	uint16_t 	vid;
	uint16_t 	pid;
	char		name[MAX_NAME_LEN];
	uint8_t		multipad;
	uint8_t		num_analogs;
	butsetup	buttons[BUTTON_SET];
	axisetup	analogs[AXIS_SET];
} padsetup;

struct usbpad {
	int32_t			device_id;
	int32_t			fd;
	bool			reading;
	const padsetup	*config;
	uint8_t			*hid_buffer;
	uint64_t		b_state;
	int8_t			a_state[AXIS_SET];
	uint16_t		mpSize;
	uint8_t			epAddress;
};

bool usbpad_init(uint8_t slots, bool skip_usb);
void usbpad_shutdown(bool skip_usb);
uint64_t usbpad_buttons(uint8_t pad_idx);
bool usbpad_avail(uint8_t index);
const char *usbpad_padname(uint8_t pad_idx);
const char *usbpad_label(uint8_t pad_idx, uint8_t b_idx);
bool usbpad_buttonavail(uint8_t pad_idx, uint8_t b_idx);
int16_t usbpad_analog(uint8_t pad_idx, uint8_t a_idx);
#define usbpad_js_lx(a) usbpad_analog(a, 0)
#define usbpad_js_ly(a) usbpad_analog(a, 1)
#define usbpad_js_rx(a) usbpad_analog(a, 2)
#define usbpad_js_ry(a) usbpad_analog(a, 3)

#endif
