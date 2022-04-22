#ifndef _GX_USBPAD_H_
#define _GX_USBPAD_H_

bool usbpad_init(uint8_t slots, bool skip_usb);
void usbpad_shutdown(bool skip_usb);
uint64_t usbpad_buttons(uint8_t pad_idx);
bool usbpad_avail(uint8_t index);
const char *usbpad_padname(uint8_t pad_idx);
const char *usbpad_label(uint8_t pad_idx, uint8_t b_idx);
bool usbpad_buttonavail(uint8_t pad_idx, uint8_t b_idx);
void usbpad_rumbleon(uint8_t pad_idx);
void usbpad_rumbleoff(uint8_t pad_idx);
int16_t usbpad_analog(uint8_t pad_idx, uint8_t a_idx);
#define usbpad_js_lx(a) usbpad_analog(a, 0)
#define usbpad_js_ly(a) usbpad_analog(a, 1)
#define usbpad_js_rx(a) usbpad_analog(a, 2)
#define usbpad_js_ry(a) usbpad_analog(a, 3)

#endif
