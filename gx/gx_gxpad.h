#ifndef _GX_GXPAD_H_
#define _GX_GXPAD_H_

#define BUTTON_SET 15
#define AXIS_SET 4
#define MAX_NAME_LEN 32

typedef struct _gxpadsetup {
	char		name[MAX_NAME_LEN];
	uint32_t	b_mask[BUTTON_SET];
	uint8_t		b_label[BUTTON_SET];
	uint8_t		num_analogs;
	uint8_t		type;
	void 		(*read_pad)(uint8_t pad_idx);
} gxpadsetup;

struct gxpad {
	const gxpadsetup *config;
	uint8_t			 p_slot;
	uint64_t		 b_state;
	int16_t		 	 a_state[AXIS_SET];
};

bool gxpad_init();
void gxpad_shutdown();
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

#endif
