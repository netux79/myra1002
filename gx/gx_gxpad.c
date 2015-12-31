#include <gccore.h>
#include <ogc/pad.h>
#ifdef HW_RVL
#include <wiiuse/wpad.h>
#endif
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include "gx_gxpad.h"

#define GX_NO_BUTTON 0x0
#define GX_BUTTON_SET 15
#define GX_AXIS_SET 4
#define GX_ML_BSET 5
#define GX_MAX_NAME_LEN 32
#define GX_MAX_PADS 8

#ifdef HW_RVL
#define WPAD_EXP_GAMECUBE WPAD_EXP_CLASSIC + 1
#define WPAD_EXP_WIIMOTE WPAD_EXP_NONE
#else
#define WPAD_EXP_GAMECUBE 0
#define WPAD_EXP_WIIMOTE WPAD_EXP_GAMECUBE
#endif

typedef struct _gxpadsetup {
	char		name[GX_MAX_NAME_LEN];
	uint32_t	b_mask[GX_BUTTON_SET];
	uint8_t		b_label[GX_BUTTON_SET];
	uint8_t		num_analogs;
	uint8_t		type;
	void 		(*read_pad)(uint8_t pad_idx);
} gxpadsetup;

struct gxpad {
	const gxpadsetup *config;
	uint8_t			 p_slot;
	uint64_t		 b_state;
	int16_t		 	 a_state[GX_AXIS_SET];
};

#ifdef HW_RVL
struct gx_mldata {
	uint8_t	ml_buttons;
	int32_t	x, y;
};

static const uint32_t _gx_mlmask[GX_ML_BSET] = {WPAD_BUTTON_B, WPAD_BUTTON_1, WPAD_BUTTON_A,
												WPAD_BUTTON_PLUS, WPAD_BUTTON_MINUS};
#endif

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

static const char* gx_labels[] = {
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

static void _gx_read_gc(uint8_t pad_idx);
#ifdef HW_RVL
static void _gx_read_wiimote(uint8_t pad_idx);
static void _gx_read_classic(uint8_t pad_idx);
static void _gx_read_nunchuk(uint8_t pad_idx);
#endif

/* The order here is the same set in wpad.h as the expansions list,
 * as guitar hero is not used we take its place for GC controller */
static const gxpadsetup _gx_pad_config[] = {
#ifdef HW_RVL
	{"Wiimote",
		{WPAD_BUTTON_B, WPAD_BUTTON_1, WPAD_BUTTON_MINUS, WPAD_BUTTON_PLUS,
		WPAD_BUTTON_RIGHT, WPAD_BUTTON_LEFT, WPAD_BUTTON_UP, WPAD_BUTTON_DOWN,
		WPAD_BUTTON_A, WPAD_BUTTON_2, GX_NO_BUTTON, GX_NO_BUTTON,
		GX_NO_BUTTON, GX_NO_BUTTON, WPAD_BUTTON_HOME},
		{GX_B, GX_1, GX_MINUS, GX_PLUS, GX_UP, GX_DOWN, GX_LEFT, GX_RIGHT,
		GX_A, GX_2, GX_NA, GX_NA, GX_NA, GX_NA, GX_HOME},
		0, WPAD_EXP_WIIMOTE, _gx_read_wiimote},
	{"Wiimote + Nunchuk",
		{WPAD_BUTTON_B, WPAD_BUTTON_1, WPAD_BUTTON_MINUS, WPAD_BUTTON_PLUS,
		WPAD_BUTTON_UP, WPAD_BUTTON_DOWN, WPAD_BUTTON_LEFT, WPAD_BUTTON_RIGHT,
		WPAD_BUTTON_A, WPAD_BUTTON_2, WPAD_NUNCHUK_BUTTON_Z, WPAD_NUNCHUK_BUTTON_C,
		GX_NO_BUTTON, GX_NO_BUTTON, WPAD_BUTTON_HOME},
		{GX_B, GX_1, GX_MINUS, GX_PLUS, GX_UP, GX_DOWN, GX_LEFT, GX_RIGHT,
		GX_A, GX_2, GX_Z, GX_C, GX_NA, GX_NA, GX_HOME},
		2, WPAD_EXP_NUNCHUK, _gx_read_nunchuk},
	{"Classic",
		{WPAD_CLASSIC_BUTTON_B, WPAD_CLASSIC_BUTTON_Y, WPAD_CLASSIC_BUTTON_MINUS, WPAD_CLASSIC_BUTTON_PLUS,
		WPAD_CLASSIC_BUTTON_UP, WPAD_CLASSIC_BUTTON_DOWN, WPAD_CLASSIC_BUTTON_LEFT, WPAD_CLASSIC_BUTTON_RIGHT,
		WPAD_CLASSIC_BUTTON_A, WPAD_CLASSIC_BUTTON_X, WPAD_CLASSIC_BUTTON_FULL_L, WPAD_CLASSIC_BUTTON_FULL_R,
		WPAD_CLASSIC_BUTTON_ZL, WPAD_CLASSIC_BUTTON_ZR, WPAD_CLASSIC_BUTTON_HOME},
		{GX_B, GX_Y, GX_MINUS, GX_PLUS, GX_UP, GX_DOWN, GX_LEFT, GX_RIGHT,
		GX_A, GX_X, GX_L, GX_R, GX_ZL, GX_ZR, GX_HOME},
		4, WPAD_EXP_CLASSIC, _gx_read_classic},
#endif
	{"Gamecube Pad",
		{PAD_BUTTON_B, PAD_BUTTON_Y, PAD_TRIGGER_Z, PAD_BUTTON_START,
		PAD_BUTTON_UP, PAD_BUTTON_DOWN, PAD_BUTTON_LEFT, PAD_BUTTON_RIGHT,
		PAD_BUTTON_A, PAD_BUTTON_X, PAD_TRIGGER_L, PAD_TRIGGER_R,
		GX_NO_BUTTON, GX_NO_BUTTON, PAD_BUTTON_START | PAD_TRIGGER_Z},
		{GX_B, GX_Y, GX_Z, GX_START, GX_UP, GX_DOWN, GX_LEFT, GX_RIGHT,
		GX_A, GX_X, GX_L, GX_R, GX_NA, GX_NA, GX_HOME},
		4, WPAD_EXP_GAMECUBE, _gx_read_gc},
};

static bool _gx_inited = false;
static struct gxpad *_gx_list[GX_MAX_PADS]; /* just hold a pointer for each possible pad */
static struct gx_mldata _gx_mldata; /* mouse & lightgun data */
static lwp_t _gx_thread = LWP_THREAD_NULL;
static bool _gx_thread_running = false;
static bool _gx_thread_quit = false;
static PADStatus _gcpdata[PAD_CHANMAX]; /* Gamecube controllers data */

static void _gx_free_pad(uint8_t pad_idx) {
	if (_gx_list[pad_idx]) {
		free(_gx_list[pad_idx]);
		_gx_list[pad_idx] = NULL;
	}
}

static int8_t _gx_rem_pad(uint32_t type, int32_t p_slot) {
	int8_t i, pad_idx = -1;

	/* Look if the setup is currently active */
	for (i = 0; i < GX_MAX_PADS; i++) {
		if (_gx_list[i] &&
			_gx_list[i]->config->type == type &&
			_gx_list[i]->p_slot == p_slot) {
				pad_idx = i;
				break;
		}
	}

	if (pad_idx != -1) _gx_free_pad(pad_idx);

	return pad_idx;
}

void gxpad_shutdown(void) {
	/* First kill the running thread */
	_gx_thread_quit = true;
	if (_gx_thread_running) {
		LWP_JoinThread(_gx_thread, NULL);
		_gx_thread_running = false;
	}

	uint8_t i;
	for (i = 0;i < GX_MAX_PADS; i++) {
		_gx_free_pad(i);
	}

	_gx_inited = false;
}

static int8_t _gx_add_pad(uint32_t type, int32_t p_slot) {
	uint8_t i;

	/* Don allow any pad not defined  already in _gx_pad_config */
	if (type >= WPAD_EXP_WIIMOTE && type <= WPAD_EXP_GAMECUBE) {
		/* look for an empty slot to assign it */
		for (i = 0; i < GX_MAX_PADS; i++) {
			/* find an empty slot */
			if (_gx_list[i] == NULL) {
				/* alloc and setup the pad data in the slot */
				_gx_list[i] = malloc(sizeof(struct gxpad));
				/* if succesfully allocated */
				if (_gx_list[i]) {
					memset(_gx_list[i], 0, sizeof(struct gxpad));
					_gx_list[i]->config = &_gx_pad_config[type];
					_gx_list[i]->p_slot = p_slot;

					return i; /* return the slot which we added it */
				}
			}
		}
	}

    return -1; /* if this point is reached, no pad was added */
}

static void _gx_scan_devices(void) {
    uint8_t slot;
    uint32_t rpads = 0, ptype = 0;
    static bool g_lon[PAD_CHANMAX];
#ifdef HW_RVL
    uint32_t conn;
    static uint8_t w_pst[PAD_CHANMAX] = {WPAD_EXP_UNKNOWN, WPAD_EXP_UNKNOWN, WPAD_EXP_UNKNOWN, WPAD_EXP_UNKNOWN};
#endif
	/* First scan for GC controllers */
	PAD_Read(_gcpdata);
	for (slot = 0; slot < PAD_CHANMAX; ++slot) {
		switch (_gcpdata[slot].err) {
			case PAD_ERR_NONE:
			case PAD_ERR_NOT_READY:
			case PAD_ERR_TRANSFER:
				if (g_lon[slot] == false) {
					_gx_add_pad(WPAD_EXP_GAMECUBE, slot);
					g_lon[slot] = true;
				}
				break;
			case PAD_ERR_NO_CONTROLLER:
				rpads |= PAD_CHAN0_BIT >> slot;
				if (g_lon[slot] == true) {
					_gx_rem_pad(WPAD_EXP_GAMECUBE, slot);
					g_lon[slot] = false;
				}
				break;
		}
	}
	/* reset any disconnectd pad */
	if (rpads) PAD_Reset(rpads);

#ifdef HW_RVL
	/* now go for the wii controllers */
    WPAD_ReadPending(WPAD_CHAN_ALL, NULL);
	for (slot = 0; slot < PAD_CHANMAX; ++slot) {
		conn = WPAD_Probe(slot, &ptype);
		switch (conn) {
			case WPAD_ERR_NONE:
				if (w_pst[slot] == WPAD_EXP_UNKNOWN) {
					_gx_add_pad(ptype, slot);
					w_pst[slot] = ptype;
					break;
				} else if (w_pst[slot] == ptype)
					break;
				/* in case the pad is connected but a different type
				 * we fall thru to remove it and detect it later */
			case WPAD_ERR_NOT_READY:
			case WPAD_ERR_NO_CONTROLLER:
				if (w_pst[slot] != WPAD_EXP_UNKNOWN) {
					_gx_rem_pad(w_pst[slot], slot);
					w_pst[slot] = WPAD_EXP_UNKNOWN;
				}
				break;
		}
	}
#endif
}

static inline void _gx_get_mlinfo(uint32_t b) {
	uint8_t i;
	ir_t ir;
	/* Get the IR data from the wiimote */
	WPAD_IR(WPAD_CHAN_0, &ir);
	_gx_mldata.x = ir.x;
	_gx_mldata.y = ir.y;
	_gx_mldata.ml_buttons = 0; /* reset button state */
	for (i = 0; i < GX_ML_BSET; i++) {
		_gx_mldata.ml_buttons |= (b & _gx_mlmask[i]) ? (1 << i) : 0;
	}
	/* Small adjustment to match the RA buttons */
	_gx_mldata.ml_buttons = _gx_mldata.ml_buttons << 2;
}

static inline void _gx_get_buttons(struct gxpad *p, uint32_t b) {
	uint8_t i;
	uint32_t m;
	p->b_state = 0; /* clear button state */
	for (i = 0; i < GX_BUTTON_SET; i++) {
		m = p->config->b_mask[i];
		if (m > GX_NO_BUTTON) {
			p->b_state |= ((b & m) == m) ? (1ULL << i) : 0;
		}
	}
}

#define GX_JS_SIM(a) ((int16_t)a << 8)
#define GX_JS_SIM_INV(a) (-((int16_t)a << 8))
#define GX_JS_NOR(a) (((int16_t)a - 128) << 8)
#define GX_JS_NOR_INV(a) ((127 - (int16_t)a) << 8)
#define GX_JS_32(a) (((int16_t)a - 32) << 10)
#define GX_JS_32_INV(a) ((31 - (int16_t)a) << 10)
#define GX_JS_16(a) (((int16_t)a - 16) << 11)
#define GX_JS_16_INV(a) ((15 - (int16_t)a) << 11)

static void _gx_read_gc(uint8_t pad_idx) {
	struct gxpad *p = _gx_list[pad_idx];
	PADStatus gdata = _gcpdata[p->p_slot];

	/* Only process when the pad is perfectly ready */
	if (gdata.err == PAD_ERR_NONE) {
		/* BUTTONS */
		_gx_get_buttons(p, gdata.button);
		/* ANALOGS */
		p->a_state[0] = GX_JS_SIM(gdata.stickX);
		p->a_state[1] = GX_JS_SIM_INV(gdata.stickY);
		p->a_state[2] = GX_JS_SIM(gdata.substickX);
		p->a_state[3] = GX_JS_SIM_INV(gdata.substickY);
	}
}

#ifdef HW_RVL
static void _gx_read_wiimote(uint8_t pad_idx) {
	struct gxpad *p = _gx_list[pad_idx];
    WPADData *wdata = WPAD_Data(p->p_slot);

	/* BUTTONS */
	_gx_get_buttons(p, wdata->btns_h);

	/* MOUSE & LIGHTGUN
	 * Get data only from the first wiimote (port 0) */
	if (p->p_slot == WPAD_CHAN_0) _gx_get_mlinfo(wdata->btns_h);
}

static void _gx_read_nunchuk(uint8_t pad_idx) {
	struct gxpad *p = _gx_list[pad_idx];
    WPADData *wdata = WPAD_Data(p->p_slot);

	/* BUTTONS */
	_gx_get_buttons(p, wdata->btns_h);
	/* ANALOGS */
	expansion_t *e = &wdata->exp;
	p->a_state[0] = GX_JS_NOR(e->nunchuk.js.pos.x);
	p->a_state[1] = GX_JS_NOR_INV(e->nunchuk.js.pos.y);
}

static void _gx_read_classic(uint8_t pad_idx) {
	struct gxpad *p = _gx_list[pad_idx];
    WPADData *wdata = WPAD_Data(p->p_slot);

	/* BUTTONS */
	_gx_get_buttons(p, wdata->btns_h);
	/* ANALOGS */
	expansion_t *e = &wdata->exp;
	p->a_state[0] = GX_JS_32(e->classic.ljs.pos.x);
	p->a_state[1] = GX_JS_32_INV(e->classic.ljs.pos.y);
	p->a_state[2] = GX_JS_16(e->classic.rjs.pos.x);
	p->a_state[3] = GX_JS_16_INV(e->classic.rjs.pos.y);
}
#endif

static void * _gx_pad_polling(void *arg) {
	uint8_t i;

	while (!_gx_thread_quit) {

		/* Scan for any device change */
		_gx_scan_devices();

		/* read data from each available pad */
		for(i = 0; i < GX_MAX_PADS; i++) {
			if (_gx_list[i]) _gx_list[i]->config->read_pad(i);
		}

		/* wait 16.66 ms to process the available pads again */
		usleep(16666);
	}

	return NULL;
}

bool gxpad_init(void) {
	if (!_gx_inited) {

		/* Set to null the whole pad list */
		memset(_gx_list, 0, sizeof(_gx_list));
		PAD_Init();
#ifdef HW_RVL
		WPAD_Init();
		WPAD_SetVRes(0, 640, 480);
		WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR); /* data from wiimote 1 */
#endif
		if (!_gx_thread_running) {
			// start the polling thread
			_gx_thread_quit = false;
			if (LWP_CreateThread(&_gx_thread, _gx_pad_polling, NULL, NULL, 0, 65) < 0) {
				_gx_thread_running = false;
				gxpad_shutdown();
				return 0;
			}
			_gx_thread_running = true;
		}

		_gx_inited = true;
    }
    return _gx_inited;
}

uint64_t gxpad_buttons(uint8_t pad_idx) {
	if (pad_idx < GX_MAX_PADS && _gx_list[pad_idx]) {
		return _gx_list[pad_idx]->b_state;
	}

	return 0ULL;
};

const char *gxpad_label(uint8_t pad_idx, uint8_t b_idx) {
	if (pad_idx < GX_MAX_PADS && b_idx < GX_BUTTON_SET && _gx_list[pad_idx]) {
		return gx_labels[_gx_list[pad_idx]->config->b_label[b_idx]];
	}

	return gx_labels[GX_NA];
}

bool gxpad_buttonavail(uint8_t pad_idx, uint8_t b_idx) {
	if (pad_idx < GX_MAX_PADS && b_idx < GX_BUTTON_SET && _gx_list[pad_idx]) {
		return (_gx_list[pad_idx]->config->b_mask[b_idx] > GX_NO_BUTTON);
	}

	return false;
}

const char *gxpad_padname(uint8_t pad_idx) {
	if (pad_idx < GX_MAX_PADS && _gx_list[pad_idx]) {
		return _gx_list[pad_idx]->config->name;
	}

	return "No pad plugged in";
}

int16_t gxpad_analog(uint8_t pad_idx, uint8_t a_idx) {
	if (pad_idx < GX_MAX_PADS) {
		struct gxpad *pad = _gx_list[pad_idx];
		if (pad && a_idx < pad->config->num_analogs) {
			return pad->a_state[a_idx];
		}
	}

	return 0;
}

bool gxpad_avail(uint8_t pad_idx) {
	if (pad_idx < GX_MAX_PADS) {
		return (_gx_list[pad_idx]);
	}

	return false;
}

uint8_t gxpad_nanalogs(uint8_t pad_idx) {
	if (pad_idx < GX_MAX_PADS && _gx_list[pad_idx]) {
		return _gx_list[pad_idx]->config->num_analogs;
	}

	return 0;
}

uint8_t gxpad_mlbuttons(void) {
	return _gx_mldata.ml_buttons;
}


int32_t gxpad_mlposx(void) {
	return _gx_mldata.x;
}

int32_t gxpad_mlposy(void) {
	return _gx_mldata.y;
}
