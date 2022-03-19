#ifndef _GX_PADSETUP_H_
#define _GX_PADSETUP_H_

#define MULTI_NOT_AVAIL 0
#define MULTI_PLAYER_1 1
#define MULTI_PLAYER_2 2

#define NO_BUTTON {0,0,B_NA}
#define NO_AXIS {0,0}

enum {
	B_B = 0,	B_CROSS,
	B_Y,		B_SQUARE,
	B_SELECT,	B_Z,		B_BACK,
	B_START,
	B_UP,
	B_DOWN,
	B_LEFT,
	B_RIGHT,
	B_A,	 	B_CIRCLE,
	B_X,		B_TRIANGLE,
	B_L,		B_L1,		B_LB,
	B_R,		B_R1,		B_RB,
	B_L2,					B_LT,	B_ZL,
	B_R2,					B_RT,	B_ZR,
	B_L3,					B_LSB,
	B_R3,					B_RSB,
	B_HOME,		B_PS,		B_XBOX,
	B_NA,
};

enum {
	A_STANDARD = 1,
	A_INVERTED,
	A_X360_STD,
	A_X360_INV,
};

static const char* blabels[] = {
	"B",		"Cross",
	"Y",		"Square",
	"Select",	"Z",		"Back",
	"Start",
	"Up",
	"Down",
	"Left",
	"Right",
	"A",		"Circle",
	"X",		"Triangle",
	"L",		"L1",		"LB",
	"R",		"R1",		"RB",
	"L2",					"LT",	"ZL",
	"R2",					"RT",	"ZR",
	"L3",					"LSB",
	"R3",					"RSB",
	"Home",		"PS",		"XBOX",
	"",
};

static void _ps3_set_operational(void *data);
static void _gcwiiu_set_operational(void *data);
static void _ps3_rumble(uint8_t pad_idx, uint8_t action);
static void _gcwiiu_rumble(uint8_t pad_idx, uint8_t action);
static void _xinput_set_operational(void *data);
static void _xinput_rumble(uint8_t pad_idx, uint8_t action);

static const padsetup valid_pad_config[] = {
			/* SNES USB Gamepad */
		{0x081F, 0xE401, "SNES USB Gamepad", MULTI_NOT_AVAIL, 2,
			{{5, 0x40, B_B}, {5, 0x80, B_Y}, {6, 0x10, B_SELECT}, {6, 0x20, B_START},
			NO_BUTTON, NO_BUTTON, NO_BUTTON, NO_BUTTON,
			{5, 0x20, B_A}, {5, 0x10, B_X}, {6, 0x01, B_L}, {6, 0x02, B_R},
			NO_BUTTON, NO_BUTTON, NO_BUTTON, NO_BUTTON,
			NO_BUTTON,},
			{{0, A_STANDARD}, {1, A_STANDARD}, NO_AXIS, NO_AXIS},
			NULL, NULL},

			/* NES USB Gamepad */
		{0x0079, 0x0011, "NES USB Gamepad", MULTI_NOT_AVAIL, 2,
			{{5, 0x10, B_B}, NO_BUTTON, {6, 0x10, B_SELECT}, {6, 0x20, B_START},
			NO_BUTTON, NO_BUTTON, NO_BUTTON, NO_BUTTON,
			{5, 0x20, B_A}, NO_BUTTON, NO_BUTTON, NO_BUTTON,
			NO_BUTTON, NO_BUTTON, NO_BUTTON, NO_BUTTON,
			NO_BUTTON,},
			{{3, A_STANDARD}, {4, A_STANDARD}, NO_AXIS, NO_AXIS},
			NULL, NULL},

			/* Playstation 3 Controller */
		{0x054C, 0x0268, "Playstation 3 Controller", MULTI_NOT_AVAIL, 4,
			{{3, 0x40, B_CROSS}, {3, 0x80, B_SQUARE}, {2, 0x01, B_SELECT}, {2, 0x08, B_START},
			{2, 0x10, B_UP}, {2, 0x40, B_DOWN}, {2, 0x80, B_LEFT}, {2, 0x20, B_RIGHT},
			{3, 0x20, B_CIRCLE}, {3, 0x10, B_TRIANGLE}, {3, 0x04, B_L1}, {3, 0x08, B_R1},
			{3, 0x01, B_L2}, {3, 0x02, B_R2}, {2, 0x02, B_L3}, {2, 0x04, B_R3},
			{4, 0x01, B_PS},},
			{{6, A_STANDARD}, {7, A_STANDARD}, {8, A_STANDARD}, {9, A_STANDARD}},
			_ps3_set_operational, _ps3_rumble},

			/* PS2 Twin Adapter */
		{0x0810, 0x0001, "PS2 Twin Adapter", MULTI_PLAYER_1, 4,
			{{5, 0x40, B_CROSS}, {5, 0x80, B_SQUARE}, {6, 0x10, B_SELECT}, {6, 0x20, B_START},
			NO_BUTTON, NO_BUTTON, NO_BUTTON, NO_BUTTON,
			{5, 0x20, B_CIRCLE}, {5, 0x10, B_TRIANGLE}, {6, 0x04, B_L1}, {6, 0x08, B_R1},
			{6, 0x01, B_L2}, {6, 0x02, B_R2}, {6, 0x40, B_L3}, {6, 0x80, B_R3},
			NO_BUTTON,},
			{{3, A_STANDARD}, {4, A_STANDARD}, {2, A_STANDARD}, {1, A_STANDARD}},
			NULL, NULL},

			/* GC Adapter for Wii U */
		{0x057E, 0x0337, "GC Adapter for Wii U", MULTI_NOT_AVAIL, 4,
			{{2, 0x02, B_B}, {2, 0x08, B_Y}, {3, 0x02, B_Z}, {3, 0x01, B_START},
			{2, 0x80, B_UP}, {2, 0x40, B_DOWN}, {2, 0x10, B_LEFT}, {2, 0x20, B_RIGHT},
			{2, 0x01, B_A}, {2, 0x04, B_X}, {3, 0x08, B_L}, {3, 0x04, B_R},
			NO_BUTTON, NO_BUTTON,	NO_BUTTON, NO_BUTTON,
			NO_BUTTON,},
			{{4, A_STANDARD}, {5, A_INVERTED}, {6, A_STANDARD}, {7, A_INVERTED}},
			_gcwiiu_set_operational, _gcwiiu_rumble},

			/* Microsoft Xbox360 Controller */
		{0x045E, 0x028E, "Microsoft Xbox360 Controller", MULTI_NOT_AVAIL, 4,
			{{3, 0x10, B_A}, {3, 0x40, B_X}, {2, 0x20, B_BACK}, {2, 0x10, B_START},
			{2, 0x01, B_UP}, {2, 0x02, B_DOWN}, {2, 0x04, B_LEFT}, {2, 0x08, B_RIGHT},
			{3, 0x20, B_B}, {3, 0x80, B_Y}, {3, 0x01, B_LB}, {3, 0x02, B_RB},
			{4, 0xFF, B_LT}, {5, 0xFF, B_RT}, {2, 0x40, B_LSB}, {2, 0x80, B_RSB},
			{3, 0x04, B_XBOX},},
			{{6, A_X360_STD}, {8, A_X360_INV}, {10, A_X360_STD}, {12, A_X360_INV}},
			_xinput_set_operational, _xinput_rumble},

			/* SNES Classic USB Adapter */
		{0x11C0, 0x5500, "SNES Classic USB Adapter", MULTI_NOT_AVAIL, 0,
			{{0, 0x02, B_B}, {0, 0x01, B_Y}, {1, 0x01, B_SELECT}, {1, 0x02, B_START},
			{9, 0xFF, B_UP}, {10, 0xFF, B_DOWN}, {8, 0xFF, B_LEFT}, {7, 0xFF, B_RIGHT},
			{0, 0x04, B_A}, {0, 0x08, B_X}, {0, 0x10, B_L}, {0, 0x20, B_R},
			{0, 0x40, B_ZL}, {0, 0x80, B_ZR}, NO_BUTTON, NO_BUTTON,
			{1, 0x10, B_HOME},},
			{NO_AXIS, NO_AXIS, NO_AXIS, NO_AXIS},
			NULL, NULL},

			/* RetroFlag Controller */
		{0x0583, 0x2060, "RetroFlag Controller", MULTI_NOT_AVAIL, 2,
			{{2, 0x02, B_B}, {2, 0x08, B_Y}, {2, 0x40, B_SELECT}, {2, 0x80, B_START},
			NO_BUTTON, NO_BUTTON, NO_BUTTON, NO_BUTTON,
			{2, 0x01, B_A}, {2, 0x04, B_X}, {2, 0x10, B_L}, {2, 0x20, B_R},
			NO_BUTTON, NO_BUTTON, NO_BUTTON, NO_BUTTON,
			NO_BUTTON,},
			{{0, A_STANDARD}, {1, A_STANDARD}, NO_AXIS, NO_AXIS},
			NULL, NULL},

			/* Hyperkin N64 Adapter */
		{0x2E24, 0x0BFF, "Hyperkin N64 Adapter", MULTI_NOT_AVAIL, 4,
			{{0, 0x04, B_B}, NO_BUTTON, {0, 0x10, B_Z}, {1, 0x02, B_START},
			{1, 0x10, B_UP}, {1, 0x40, B_DOWN}, {1, 0x80, B_LEFT}, {1, 0x20, B_RIGHT},
			{0, 0x02, B_A}, NO_BUTTON, {0, 0x80, B_L}, {0, 0x20, B_R},
			NO_BUTTON, NO_BUTTON, NO_BUTTON, NO_BUTTON,
			NO_BUTTON,},
			{{2, A_STANDARD}, {3, A_STANDARD}, {5, A_STANDARD}, {4, A_STANDARD}},
			NULL, NULL},

			/* Mayflash GC Adapter */
		{0x0079, 0x1847, "Mayflash GC Adapter", MULTI_NOT_AVAIL, 4,
			{{0, 0x04, B_B}, {0, 0x08, B_Y}, {0, 0x80, B_Z}, {1, 0x02, B_START},
			{1, 0x10, B_UP}, {1, 0x40, B_DOWN}, {1, 0x80, B_LEFT}, {1, 0x20, B_RIGHT},
			{0, 0x02, B_A}, {0, 0x01, B_X}, {0, 0x10, B_L}, {0, 0x20, B_R},
			NO_BUTTON, NO_BUTTON, NO_BUTTON, NO_BUTTON,
			NO_BUTTON,},
			{{2, A_STANDARD}, {3, A_STANDARD}, {5, A_STANDARD}, {4, A_STANDARD}},
			NULL, NULL},
	};

#endif
