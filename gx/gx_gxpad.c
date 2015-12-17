#include <gccore.h>
#include <string.h>
#include <unistd.h>

#include "gx_gxpad.h"

#define GX_NO_BUTTON 0x0
#define WPAD_EXP_GAMECUBE WPAD_EXP_CLASSIC + 1
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

static const char* blabels[] = {
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

static void _read_gc(uint8_t pad_idx);
static void _read_wiimote(uint8_t pad_idx);
static void _read_classic(uint8_t pad_idx);
static void _read_nunchuk(uint8_t pad_idx);
/* The order here is the same set in wpad.h as the expansions list,
 * as guitar hero is not used we take its place for GC controller */
static const gxpadsetup valid_pad_config[] = {
			{"Wiimote",
				{WPAD_BUTTON_B, WPAD_BUTTON_1, WPAD_BUTTON_MINUS, WPAD_BUTTON_PLUS,
				WPAD_BUTTON_LEFT, WPAD_BUTTON_RIGHT, WPAD_BUTTON_DOWN, WPAD_BUTTON_UP,
				WPAD_BUTTON_A, WPAD_BUTTON_2, GX_NO_BUTTON, GX_NO_BUTTON,
				GX_NO_BUTTON, GX_NO_BUTTON, WPAD_BUTTON_HOME},
				{GX_B, GX_1, GX_MINUS, GX_PLUS, GX_UP, GX_DOWN, GX_LEFT, GX_RIGHT,
				GX_A, GX_2, GX_NA, GX_NA, GX_NA, GX_NA, GX_HOME},
				0, WPAD_EXP_WIIMOTE, _read_wiimote},
			{"Wiimote + Nunchuk",
				{WPAD_BUTTON_B, WPAD_BUTTON_1, WPAD_BUTTON_MINUS, WPAD_BUTTON_PLUS,
				WPAD_BUTTON_UP, WPAD_BUTTON_DOWN, WPAD_BUTTON_LEFT, WPAD_BUTTON_RIGHT,
				WPAD_BUTTON_A, WPAD_BUTTON_2, WPAD_NUNCHUK_BUTTON_Z, WPAD_NUNCHUK_BUTTON_C,
				GX_NO_BUTTON, GX_NO_BUTTON, WPAD_BUTTON_HOME},
				{GX_B, GX_1, GX_MINUS, GX_PLUS, GX_UP, GX_DOWN, GX_LEFT, GX_RIGHT,
				GX_A, GX_2, GX_Z, GX_C, GX_NA, GX_NA, GX_HOME},
				2, WPAD_EXP_NUNCHUK, _read_nunchuk},
			{"Classic",
				{WPAD_CLASSIC_BUTTON_B, WPAD_CLASSIC_BUTTON_Y, WPAD_CLASSIC_BUTTON_MINUS, WPAD_CLASSIC_BUTTON_PLUS,
				WPAD_CLASSIC_BUTTON_UP, WPAD_CLASSIC_BUTTON_DOWN, WPAD_CLASSIC_BUTTON_LEFT, WPAD_CLASSIC_BUTTON_RIGHT,
				WPAD_CLASSIC_BUTTON_A, WPAD_CLASSIC_BUTTON_X, WPAD_CLASSIC_BUTTON_FULL_L, WPAD_CLASSIC_BUTTON_FULL_R,
				WPAD_CLASSIC_BUTTON_ZL, WPAD_CLASSIC_BUTTON_ZR, WPAD_CLASSIC_BUTTON_HOME},
				{GX_B, GX_Y, GX_MINUS, GX_PLUS, GX_UP, GX_DOWN, GX_LEFT, GX_RIGHT,
				GX_A, GX_X, GX_L, GX_R, GX_ZL, GX_ZR, GX_HOME},
				4, WPAD_EXP_CLASSIC, _read_classic},
			{"Gamecube Pad",
				{PAD_BUTTON_B, PAD_BUTTON_Y, PAD_TRIGGER_Z, PAD_BUTTON_START,
				PAD_BUTTON_UP, PAD_BUTTON_DOWN, PAD_BUTTON_LEFT, PAD_BUTTON_RIGHT,
				PAD_BUTTON_A, PAD_BUTTON_X, PAD_TRIGGER_L, PAD_TRIGGER_R,
				GX_NO_BUTTON, GX_NO_BUTTON, PAD_BUTTON_START | PAD_TRIGGER_Z},
				{GX_B, GX_1, GX_Z, GX_START, GX_UP, GX_DOWN, GX_LEFT, GX_RIGHT,
				GX_A, GX_X, GX_L, GX_R, GX_NA, GX_NA, GX_HOME},
				4, WPAD_EXP_GAMECUBE, _read_gc},
		};

#define GX_MAX_PADS 8

static bool _inited = false;
static bool _pad_detected = false;
static struct gxpad *_pad_list[GX_MAX_PADS]; /* just hold a pointer for each possible pad */
static lwp_t _pad_thread = LWP_THREAD_NULL;
static bool _pad_thread_running = false;
static bool _pad_thread_quit = false;
static PADStatus _gcpdata[PAD_CHANMAX];

static void _free_pad(uint8_t pad_idx) {
	if (_pad_list[pad_idx]) {
		free(_pad_list[pad_idx]);
		_pad_list[pad_idx] = NULL;
	}
}

static int8_t _rem_pad(uint32_t type, int32_t p_slot) {
	int8_t i, pad_idx -1;

	/* Look if the setup is currently active */
	for (i = 0; i<= GX_MAX_PADS; i++) {
		if (_pad_list[i] &&
			_pad_list[i]->config->type == type &&
			_pad_list[i]->p_slot == p_slot) {
				pad_idx = i;
				break;
		}
	}

	if (pad_idx != -1) _free_pad(pad_idx);

	return pad_idx;
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
		_free_pad(i);
	}

	_inited = false;
}

static int8_t _add_pad(uint32_t type, int32_t p_slot) {
	uint8_t i;

	/* Don allow any pad not defined  already in valid_pad_config */
	if (type >= WPAD_EXP_WIIMOTE && type <= WPAD_EXP_GAMECUBE) {
		/* look for an empty slot to assign it */
		for (i = 0; i < GX_MAX_PADS; i++) {
			/* find an empty slot */
			if (_pad_list[i] == NULL) {
				/* alloc and setup the pad data in the slot */
				_pad_list[i] = malloc(sizeof(struct gxpad));
				/* if succesfully allocated */
				if (_pad_list[i]) {
					memset(_pad_list[i], 0, sizeof(struct gxpad));
					_pad_list[i]->config = &valid_pad_config[type];
					_pad_list[i]->p_slot = p_slot;

					return i; /* return the slot which we added it */
				}
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
				} else if (w_pst[slot] == ptype)
					break;
				/* in case the pad is connected but a different type
				 * we fall thru to remove it and detect it later */
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

static void _read_gc(uint8_t pad_idx) {
	uint8_t i;
	uint32_t b, m;
	struct gxpad *p = _pad_list[pad_idx];

	/* Only process when the pad is perfectly ready */
	if (_gcpdata[p->p_slot].err == PAD_ERR_NONE) {
		/* BUTTONS */
		b = _gcpdata[p->p_slot].button;
		p->b_state = 0; /* clear button state */
		for (i = 0; i < BUTTON_SET; i++) {
			m = p->config->b_mask[i];
			if (m > GX_NO_BUTTON) {
				p->b_state |= ((b & m) == m) ? (1ULL << i) : 0;
			}
		}
		/* ANALOGS */
		p->a_state[0] =   (int16_t)_gcpdata[p->p_slot].stickX << 8;
		p->a_state[1] = -((int16_t)_gcpdata[p->p_slot].stickY << 8);
		p->a_state[2] =   (int16_t)_gcpdata[p->p_slot].substickX << 8;
		p->a_state[3] = -((int16_t)_gcpdata[p->p_slot].substickX << 8);
	}
}

static void _read_wiimote(uint8_t pad_idx) {
	uint8_t i;
	uint32_t b, m;
	struct gxpad *p = _pad_list[pad_idx];
    WPADData *wdata = WPAD_Data(p->p_slot);

	/* BUTTONS */
    b = wdata->btns_h;
	p->b_state = 0; /* clear button state */
	for (i = 0; i < BUTTON_SET; i++) {
		m = p->config->b_mask[i];
		if (m > GX_NO_BUTTON) {
			p->b_state |= ((b & m) == m) ? (1ULL << i) : 0;
		}
	}
}

static void _read_nunchuk(uint8_t pad_idx) {
	uint8_t i;
	uint32_t b, m;
	struct gxpad *p = _pad_list[pad_idx];
    WPADData *wdata = WPAD_Data(p->p_slot);

	/* BUTTONS */
    b = wdata->btns_h;
	p->b_state = 0; /* clear button state */
	for (i = 0; i < BUTTON_SET; i++) {
		m = p->config->b_mask[i];
		if (m > GX_NO_BUTTON) {
			p->b_state |= ((b & m) == m) ? (1ULL << i) : 0;
		}
	}
	/* ANALOGS */
	expansion_t *e = &wdata->exp;
	p->a_state[0] = _get_wii_ah(e->nunchuk.js.mag, e->nunchuk.js.ang);
	p->a_state[1] = _get_wii_av(e->nunchuk.js.mag, e->nunchuk.js.ang);
}

static void _read_classic(uint8_t pad_idx) {
	uint8_t i;
	uint32_t b, m;
	struct gxpad *p = _pad_list[pad_idx];
    WPADData *wdata = WPAD_Data(p->p_slot);

	/* BUTTONS */
    b = wdata->btns_h;
	p->b_state = 0; /* clear button state */
	for (i = 0; i < BUTTON_SET; i++) {
		m = p->config->b_mask[i];
		if (m > GX_NO_BUTTON) {
			p->b_state |= ((b & m) == m) ? (1ULL << i) : 0;
		}
	}
	/* ANALOGS */
	expansion_t *e = &wdata->exp;
	p->a_state[0] = _get_wii_ah(e->classic.ljs.mag, e->classic.ljs.ang);
	p->a_state[1] = _get_wii_av(e->classic.ljs.mag, e->classic.ljs.ang);
	p->a_state[2] = _get_wii_ah(e->classic.rjs.mag, e->classic.rjs.ang);
	p->a_state[3] = _get_wii_av(e->classic.rjs.mag, e->classic.rjs.ang);
}

static void * _pad_polling(void *arg) {
	uint8_t i;

	while (!_pad_thread_quit) {

		/* Scan for any device change */
		_scan_devices();

		/* read data from each available pad */
		for(i = 0; i < GX_MAX_PADS; i++) {
			if (_pad_list[i]) _pad_list[i]->read_pad(i);
		}

		/* Add here the mouse data polling */

		/* wait 16.66 ms to process the available pads again */
		usleep(16666);
	}

	return NULL;
}

bool gxpad_init(void) {
	if (!_inited) {

		/* Set to null the whole pad list */
		memset(_pad_list, 0, sizeof(_pad_list));

		if (!_pad_thread_running) {
			// start the polling thread
			_pad_thread_quit = false;
			if (LWP_CreateThread(&_pad_thread, _pad_polling, NULL, NULL, 0, 65) < 0) {
				_pad_thread_running = false;
				gxpad_shutdown();
				return 0;
			}
			_pad_thread_running = true;
		}

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
		return blabels[_pad_list[pad_idx]->config->b_label[b_idx]];
	}

	return blabels[GX_NA];
}

bool gxpad_buttonavail(uint8_t pad_idx, uint8_t b_idx) {
	if (pad_idx < GX_MAX_PADS && b_idx < BUTTON_SET && _pad_list[pad_idx]) {
		return (_pad_list[pad_idx]->config->b_mask[b_idx] > GX_NO_BUTTON);
	}

	return false;
}

const char *gxpad_padname(uint8_t pad_idx) {
	if (pad_idx < GX_MAX_PADS && _pad_list[pad_idx]) {
		return _pad_list[pad_idx]->config->name;
	}

	return "No pad plugged in";
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
