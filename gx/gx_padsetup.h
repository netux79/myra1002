#ifndef _GX_PADSETUP_H_
#define _GX_PADSETUP_H_

#define MULTI_NOT_AVAIL 0
#define MULTI_PLAYER_1 1
#define MULTI_PLAYER_2 2

#define HAT_GET(a) (a & 0x0F) /*HAT MASK = 0x0F */
#define HAT_LEFT(a) (a == 0x05) || (a == 0x06) || (a == 0x07)
#define HAT_RIGHT(a) (a == 0x01) || (a == 0x02) || (a == 0x03)
#define HAT_UP(a) (a == 0x07) || (a == 0x00) || (a == 0x01)
#define HAT_DOWN(a) (a == 0x03) || (a == 0x04) || (a == 0x05)
#define HAT_MIN -32768
#define HAT_MAX 32767

#define NO_BUTTON {0,0,B_NA}
#define NO_AXIS {0,0}

enum {
	B_B = 0,	B_CROSS,
	B_Y,		B_SQUARE,
	B_SELECT,	B_Z,
	B_START,
	B_UP,
	B_DOWN,
	B_LEFT,
	B_RIGHT,
	B_A,	 	B_CIRCLE,
	B_X,		B_TRIANGLE,
	B_L,		B_L1,
	B_R,		B_R1,
	B_L2,
	B_R2,
	B_L3,
	B_R3,
	B_HOME,		B_PS,
	B_NA,
};

enum {
	A_STANDARD = 1,
	A_INVERTED,
	A_HAT_PSXH,
	A_HAT_PSXV,
};

static const char* blabels[] = {
	"B",		"Cross",
	"Y",		"Square",
	"Select",	"Z",
	"Start",
	"Up",
	"Down",
	"Left",
	"Right",
	"A",		"Circle",
	"X",		"Triangle",
	"L",		"L1",
	"R",		"R1",
	"L2",
	"R2",
	"L3",
	"R3",
	"Home",		"PS",
	"",
};

static void _ps3_rumble(uint8_t pad_idx, uint8_t action);

static const padsetup valid_pad_config[] = {
	{0x081F, 0xE401, "SNES USB Gamepad", MULTI_NOT_AVAIL, 2,
		{{5, 0x40, B_B}, {5, 0x80, B_Y}, {6, 0x10, B_SELECT}, {6, 0x20, B_START},
		NO_BUTTON, NO_BUTTON, NO_BUTTON, NO_BUTTON,
		{5, 0x20, B_A}, {5, 0x10, B_X}, {6, 0x01, B_L}, {6, 0x02, B_R},
		NO_BUTTON, NO_BUTTON, NO_BUTTON, NO_BUTTON,
		NO_BUTTON,},
		{{0, A_STANDARD}, {1, A_STANDARD}, NO_AXIS, NO_AXIS,
		NO_AXIS, NO_AXIS,}, NULL},
	{0x0079, 0x0011, "NES USB Gamepad", MULTI_NOT_AVAIL, 2,
		{{5, 0x10, B_B}, NO_BUTTON, {6, 0x10, B_SELECT}, {6, 0x20, B_START},
		NO_BUTTON, NO_BUTTON, NO_BUTTON, NO_BUTTON,
		{5, 0x20, B_A}, NO_BUTTON, NO_BUTTON, NO_BUTTON,
		NO_BUTTON, NO_BUTTON, NO_BUTTON, NO_BUTTON,
		NO_BUTTON,},
		{{3, A_STANDARD}, {4, A_STANDARD}, NO_AXIS, NO_AXIS,
		NO_AXIS, NO_AXIS,}, NULL},
	{0x054C, 0x0268, "Playstation 3 Gamepad", MULTI_NOT_AVAIL, 4,
		{{3, 0x40, B_CROSS}, {3, 0x80, B_SQUARE}, {2, 0x01, B_SELECT}, {2, 0x08, B_START},
		{2, 0x10, B_UP}, {2, 0x40, B_DOWN}, {2, 0x80, B_LEFT}, {2, 0x20, B_RIGHT},
		{3, 0x20, B_CIRCLE}, {3, 0x10, B_TRIANGLE}, {3, 0x04, B_L1}, {3, 0x08, B_R1},
		{3, 0x01, B_L2}, {3, 0x02, B_R2}, {2, 0x02, B_L3}, {2, 0x04, B_R3},
		{4, 0x01, B_PS},},
		{{6, A_STANDARD}, {7, A_STANDARD}, {8, A_STANDARD}, {9, A_STANDARD},
		NO_AXIS, NO_AXIS,}, _ps3_rumble},
	{0x0810, 0x0001, "PS2 Twin Adapter", MULTI_PLAYER_1, 6,
		{{5, 0x40, B_CROSS}, {5, 0x80, B_SQUARE}, {6, 0x10, B_SELECT}, {6, 0x20, B_START},
		NO_BUTTON, NO_BUTTON, NO_BUTTON, NO_BUTTON,
		{5, 0x20, B_CIRCLE}, {5, 0x10, B_TRIANGLE}, {6, 0x04, B_L1}, {6, 0x08, B_R1},
		{6, 0x01, B_L2}, {6, 0x02, B_R2}, {6, 0x40, B_L3}, {6, 0x80, B_R3},
		NO_BUTTON,},
		{{3, A_STANDARD}, {4, A_STANDARD}, {2, A_STANDARD}, {1, A_STANDARD},
		{5, A_HAT_PSXH}, {5, A_HAT_PSXV}}, NULL},
	{0x057E, 0x0337, "GC Adapter for Wii U", MULTI_NOT_AVAIL, 4,
		{{2, 0x02, B_B}, {2, 0x08, B_Y}, {3, 0x02, B_Z}, {3, 0x01, B_START},
		{2, 0x80, B_UP}, {2, 0x40, B_DOWN}, {2, 0x10, B_LEFT}, {2, 0x20, B_RIGHT},
		{2, 0x01, B_A}, {2, 0x04, B_X}, {3, 0x08, B_L}, {3, 0x04, B_R},
		NO_BUTTON, NO_BUTTON,	NO_BUTTON, NO_BUTTON,
		NO_BUTTON,},
		{{4, A_STANDARD}, {5, A_INVERTED}, {6, A_STANDARD}, {7, A_INVERTED},
		NO_AXIS, NO_AXIS,}, NULL},
};

#endif
