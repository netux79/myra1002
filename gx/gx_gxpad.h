#ifndef _GX_GXPAD_H_
#define _GX_GXPAD_H_

bool gxpad_init(void);
void gxpad_shutdown(void);
uint64_t gxpad_buttons(uint8_t pad_idx);
bool gxpad_avail(uint8_t index);
const char *gxpad_padname(uint8_t pad_idx);
const char *gxpad_label(uint8_t pad_idx, uint8_t b_idx);
bool gxpad_buttonavail(uint8_t pad_idx, uint8_t b_idx);
uint8_t gxpad_nanalogs(uint8_t pad_idx);
int16_t gxpad_analog(uint8_t pad_idx, uint8_t a_idx);
#define gxpad_js_lx(a) gxpad_analog(a, 0)
#define gxpad_js_ly(a) gxpad_analog(a, 1)
#define gxpad_js_rx(a) gxpad_analog(a, 2)
#define gxpad_js_ry(a) gxpad_analog(a, 3)
uint8_t gxpad_mlbuttons(void);
bool gxpad_mlposvalid(void);
int32_t gxpad_mlposx(void);
int32_t gxpad_mlposy(void);
#endif
