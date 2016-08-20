/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2014 - Daniel De Matteis
 *
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

// Compile: gcc -o hq2x.so -shared hq2x.c -std=c99 -O3 -Wall -pedantic -fPIC

#include "softfilter.h"
#include <stdlib.h>

#ifdef RARCH_INTERNAL
#define softfilter_get_implementation hq2x_get_implementation
#define filter_data hq2x_filter_data
#endif

#define HQ2X_SCALE 2

#define MASK16_2 0x07E0
#define MASK16_13 0xF81F

#define  Ymask   0xFF0000
#define  Umask   0x00FF00
#define  Vmask   0x0000FF
#define  trY   0x300000
#define  trU   0x000700
#define  trV   0x000006

#define X2PIXEL00_0    *(dp) = w5;
#define X2PIXEL00_10   *(dp) = Interp01(w5, w1);
#define X2PIXEL00_11   *(dp) = Interp01(w5, w4);
#define X2PIXEL00_12   *(dp) = Interp01(w5, w2);
#define X2PIXEL00_20   *(dp) = Interp02(w5, w4, w2);
#define X2PIXEL00_21   *(dp) = Interp02(w5, w1, w2);
#define X2PIXEL00_22   *(dp) = Interp02(w5, w1, w4);
#define X2PIXEL00_60   *(dp) = Interp06(w5, w2, w4);
#define X2PIXEL00_61   *(dp) = Interp06(w5, w4, w2);
#define X2PIXEL00_70   *(dp) = Interp07(w5, w4, w2);
#define X2PIXEL00_90   *(dp) = Interp09(w5, w4, w2);
#define X2PIXEL00_100  *(dp) = Interp10(w5, w4, w2);

#define X2PIXEL01_0    *(dp + 1) = w5;
#define X2PIXEL01_10   *(dp + 1) = Interp01(w5, w3);
#define X2PIXEL01_11   *(dp + 1) = Interp01(w5, w2);
#define X2PIXEL01_12   *(dp + 1) = Interp01(w5, w6);
#define X2PIXEL01_20   *(dp + 1) = Interp02(w5, w2, w6);
#define X2PIXEL01_21   *(dp + 1) = Interp02(w5, w3, w6);
#define X2PIXEL01_22   *(dp + 1) = Interp02(w5, w3, w2);
#define X2PIXEL01_60   *(dp + 1) = Interp06(w5, w6, w2);
#define X2PIXEL01_61   *(dp + 1) = Interp06(w5, w2, w6);
#define X2PIXEL01_70   *(dp + 1) = Interp07(w5, w2, w6);
#define X2PIXEL01_90   *(dp + 1) = Interp09(w5, w2, w6);
#define X2PIXEL01_100  *(dp + 1) = Interp10(w5, w2, w6);

#define X2PIXEL10_0    *(dp + dst1line) = w5;
#define X2PIXEL10_10   *(dp + dst1line) = Interp01(w5, w7);
#define X2PIXEL10_11   *(dp + dst1line) = Interp01(w5, w8);
#define X2PIXEL10_12   *(dp + dst1line) = Interp01(w5, w4);
#define X2PIXEL10_20   *(dp + dst1line) = Interp02(w5, w8, w4);
#define X2PIXEL10_21   *(dp + dst1line) = Interp02(w5, w7, w4);
#define X2PIXEL10_22   *(dp + dst1line) = Interp02(w5, w7, w8);
#define X2PIXEL10_60   *(dp + dst1line) = Interp06(w5, w4, w8);
#define X2PIXEL10_61   *(dp + dst1line) = Interp06(w5, w8, w4);
#define X2PIXEL10_70   *(dp + dst1line) = Interp07(w5, w8, w4);
#define X2PIXEL10_90   *(dp + dst1line) = Interp09(w5, w8, w4);
#define X2PIXEL10_100   *(dp + dst1line) = Interp10(w5, w8, w4);

#define X2PIXEL11_0    *(dp + dst1line + 1) = w5;
#define X2PIXEL11_10   *(dp + dst1line + 1) = Interp01(w5, w9);
#define X2PIXEL11_11   *(dp + dst1line + 1) = Interp01(w5, w6);
#define X2PIXEL11_12   *(dp + dst1line + 1) = Interp01(w5, w8);
#define X2PIXEL11_20   *(dp + dst1line + 1) = Interp02(w5, w6, w8);
#define X2PIXEL11_21   *(dp + dst1line + 1) = Interp02(w5, w9, w8);
#define X2PIXEL11_22   *(dp + dst1line + 1) = Interp02(w5, w9, w6);
#define X2PIXEL11_60   *(dp + dst1line + 1) = Interp06(w5, w8, w6);
#define X2PIXEL11_61   *(dp + dst1line + 1) = Interp06(w5, w6, w8);
#define X2PIXEL11_70   *(dp + dst1line + 1) = Interp07(w5, w6, w8);
#define X2PIXEL11_90   *(dp + dst1line + 1) = Interp09(w5, w6, w8);
#define X2PIXEL11_100  *(dp + dst1line + 1) = Interp10(w5, w6, w8);

#define X3PIXEL00_1M   *(dp) = Interp01(w5, w1);
#define X3PIXEL00_1U   *(dp) = Interp01(w5, w2);
#define X3PIXEL00_1L   *(dp) = Interp01(w5, w4);
#define X3PIXEL00_2    *(dp) = Interp02(w5, w4, w2);
#define X3PIXEL00_4    *(dp) = Interp04(w5, w4, w2);
#define X3PIXEL00_5    *(dp) = Interp05(w4, w2);
#define X3PIXEL00_C    *(dp) = w5;

#define X3PIXEL01_1    *(dp + 1) = Interp01(w5, w2);
#define X3PIXEL01_3    *(dp + 1) = Interp03(w5, w2);
#define X3PIXEL01_6    *(dp + 1) = Interp01(w2, w5);
#define X3PIXEL01_C    *(dp + 1) = w5;

#define X3PIXEL02_1M   *(dp + 2) = Interp01(w5, w3);
#define X3PIXEL02_1U   *(dp + 2) = Interp01(w5, w2);
#define X3PIXEL02_1R   *(dp + 2) = Interp01(w5, w6);
#define X3PIXEL02_2    *(dp + 2) = Interp02(w5, w2, w6);
#define X3PIXEL02_4    *(dp + 2) = Interp04(w5, w2, w6);
#define X3PIXEL02_5    *(dp + 2) = Interp05(w2, w6);
#define X3PIXEL02_C    *(dp + 2) = w5;

#define X3PIXEL10_1    *(dp + dst1line) = Interp01(w5, w4);
#define X3PIXEL10_3    *(dp + dst1line) = Interp03(w5, w4);
#define X3PIXEL10_6    *(dp + dst1line) = Interp01(w4, w5);
#define X3PIXEL10_C    *(dp + dst1line) = w5;

#define X3PIXEL11      *(dp + dst1line + 1) = w5;

#define X3PIXEL12_1    *(dp + dst1line + 2) = Interp01(w5, w6);
#define X3PIXEL12_3    *(dp + dst1line + 2) = Interp03(w5, w6);
#define X3PIXEL12_6    *(dp + dst1line + 2) = Interp01(w6, w5);
#define X3PIXEL12_C    *(dp + dst1line + 2) = w5;

#define X3PIXEL20_1M   *(dp + dst1line + dst1line) = Interp01(w5, w7);
#define X3PIXEL20_1D   *(dp + dst1line + dst1line) = Interp01(w5, w8);
#define X3PIXEL20_1L   *(dp + dst1line + dst1line) = Interp01(w5, w4);
#define X3PIXEL20_2    *(dp + dst1line + dst1line) = Interp02(w5, w8, w4);
#define X3PIXEL20_4    *(dp + dst1line + dst1line) = Interp04(w5, w8, w4);
#define X3PIXEL20_5    *(dp + dst1line + dst1line) = Interp05(w8, w4);
#define X3PIXEL20_C    *(dp + dst1line + dst1line) = w5;

#define X3PIXEL21_1    *(dp + dst1line + dst1line + 1) = Interp01(w5, w8);
#define X3PIXEL21_3    *(dp + dst1line + dst1line + 1) = Interp03(w5, w8);
#define X3PIXEL21_6    *(dp + dst1line + dst1line + 1) = Interp01(w8, w5);
#define X3PIXEL21_C    *(dp + dst1line + dst1line + 1) = w5;

#define X3PIXEL22_1M   *(dp + dst1line + dst1line + 2) = Interp01(w5, w9);
#define X3PIXEL22_1D   *(dp + dst1line + dst1line + 2) = Interp01(w5, w8);
#define X3PIXEL22_1R   *(dp + dst1line + dst1line + 2) = Interp01(w5, w6);
#define X3PIXEL22_2    *(dp + dst1line + dst1line + 2) = Interp02(w5, w6, w8);
#define X3PIXEL22_4    *(dp + dst1line + dst1line + 2) = Interp04(w5, w6, w8);
#define X3PIXEL22_5    *(dp + dst1line + dst1line + 2) = Interp05(w6, w8);
#define X3PIXEL22_C    *(dp + dst1line + dst1line + 2) = w5;

#define X4PIXEL00_0    *(dp) = w5;
#define X4PIXEL00_11   *(dp) = Interp01(w5, w4);
#define X4PIXEL00_12   *(dp) = Interp01(w5, w2);
#define X4PIXEL00_20   *(dp) = Interp02(w5, w2, w4);
#define X4PIXEL00_50   *(dp) = Interp05(w2, w4);
#define X4PIXEL00_80   *(dp) = Interp08(w5, w1);
#define X4PIXEL00_81   *(dp) = Interp08(w5, w4);
#define X4PIXEL00_82   *(dp) = Interp08(w5, w2);

#define X4PIXEL01_0    *(dp + 1) = w5;
#define X4PIXEL01_10   *(dp + 1) = Interp01(w5, w1);
#define X4PIXEL01_12   *(dp + 1) = Interp01(w5, w2);
#define X4PIXEL01_14   *(dp + 1) = Interp01(w2, w5);
#define X4PIXEL01_21   *(dp + 1) = Interp02(w2, w5, w4);
#define X4PIXEL01_31   *(dp + 1) = Interp03(w5, w4);
#define X4PIXEL01_50   *(dp + 1) = Interp05(w2, w5);
#define X4PIXEL01_60   *(dp + 1) = Interp06(w5, w2, w4);
#define X4PIXEL01_61   *(dp + 1) = Interp06(w5, w2, w1);
#define X4PIXEL01_82   *(dp + 1) = Interp08(w5, w2);
#define X4PIXEL01_83   *(dp + 1) = Interp08(w2, w4);

#define X4PIXEL02_0    *(dp + 2) = w5;
#define X4PIXEL02_10   *(dp + 2) = Interp01(w5, w3);
#define X4PIXEL02_11   *(dp + 2) = Interp01(w5, w2);
#define X4PIXEL02_13   *(dp + 2) = Interp01(w2, w5);
#define X4PIXEL02_21   *(dp + 2) = Interp02(w2, w5, w6);
#define X4PIXEL02_32   *(dp + 2) = Interp03(w5, w6);
#define X4PIXEL02_50   *(dp + 2) = Interp05(w2, w5);
#define X4PIXEL02_60   *(dp + 2) = Interp06(w5, w2, w6);
#define X4PIXEL02_61   *(dp + 2) = Interp06(w5, w2, w3);
#define X4PIXEL02_81   *(dp + 2) = Interp08(w5, w2);
#define X4PIXEL02_83   *(dp + 2) = Interp08(w2, w6);

#define X4PIXEL03_0    *(dp + 3) = w5;
#define X4PIXEL03_11   *(dp + 3) = Interp01(w5, w2);
#define X4PIXEL03_12   *(dp + 3) = Interp01(w5, w6);
#define X4PIXEL03_20   *(dp + 3) = Interp02(w5, w2, w6);
#define X4PIXEL03_50   *(dp + 3) = Interp05(w2, w6);
#define X4PIXEL03_80   *(dp + 3) = Interp08(w5, w3);
#define X4PIXEL03_81   *(dp + 3) = Interp08(w5, w2);
#define X4PIXEL03_82   *(dp + 3) = Interp08(w5, w6);

#define X4PIXEL10_0    *(dp + dst1line) = w5;
#define X4PIXEL10_10   *(dp + dst1line) = Interp01(w5, w1);
#define X4PIXEL10_11   *(dp + dst1line) = Interp01(w5, w4);
#define X4PIXEL10_13   *(dp + dst1line) = Interp01(w4, w5);
#define X4PIXEL10_21   *(dp + dst1line) = Interp02(w4, w5, w2);
#define X4PIXEL10_32   *(dp + dst1line) = Interp03(w5, w2);
#define X4PIXEL10_50   *(dp + dst1line) = Interp05(w4, w5);
#define X4PIXEL10_60   *(dp + dst1line) = Interp06(w5, w4, w2);
#define X4PIXEL10_61   *(dp + dst1line) = Interp06(w5, w4, w1);
#define X4PIXEL10_81   *(dp + dst1line) = Interp08(w5, w4);
#define X4PIXEL10_83   *(dp + dst1line) = Interp08(w4, w2);

#define X4PIXEL11_0    *(dp + dst1line + 1) = w5;
#define X4PIXEL11_30   *(dp + dst1line + 1) = Interp03(w5, w1);
#define X4PIXEL11_31   *(dp + dst1line + 1) = Interp03(w5, w4);
#define X4PIXEL11_32   *(dp + dst1line + 1) = Interp03(w5, w2);
#define X4PIXEL11_70   *(dp + dst1line + 1) = Interp07(w5, w4, w2);

#define X4PIXEL12_0    *(dp + dst1line + 2) = w5;
#define X4PIXEL12_30   *(dp + dst1line + 2) = Interp03(w5, w3);
#define X4PIXEL12_31   *(dp + dst1line + 2) = Interp03(w5, w2);
#define X4PIXEL12_32   *(dp + dst1line + 2) = Interp03(w5, w6);
#define X4PIXEL12_70   *(dp + dst1line + 2) = Interp07(w5, w6, w2);

#define X4PIXEL13_0    *(dp + dst1line + 3) = w5;
#define X4PIXEL13_10   *(dp + dst1line + 3) = Interp01(w5, w3);
#define X4PIXEL13_12   *(dp + dst1line + 3) = Interp01(w5, w6);
#define X4PIXEL13_14   *(dp + dst1line + 3) = Interp01(w6, w5);
#define X4PIXEL13_21   *(dp + dst1line + 3) = Interp02(w6, w5, w2);
#define X4PIXEL13_31   *(dp + dst1line + 3) = Interp03(w5, w2);
#define X4PIXEL13_50   *(dp + dst1line + 3) = Interp05(w6, w5);
#define X4PIXEL13_60   *(dp + dst1line + 3) = Interp06(w5, w6, w2);
#define X4PIXEL13_61   *(dp + dst1line + 3) = Interp06(w5, w6, w3);
#define X4PIXEL13_82   *(dp + dst1line + 3) = Interp08(w5, w6);
#define X4PIXEL13_83   *(dp + dst1line + 3) = Interp08(w6, w2);

#define X4PIXEL20_0    *(dp + dst1line + dst1line) = w5;
#define X4PIXEL20_10   *(dp + dst1line + dst1line) = Interp01(w5, w7);
#define X4PIXEL20_12   *(dp + dst1line + dst1line) = Interp01(w5, w4);
#define X4PIXEL20_14   *(dp + dst1line + dst1line) = Interp01(w4, w5);
#define X4PIXEL20_21   *(dp + dst1line + dst1line) = Interp02(w4, w5, w8);
#define X4PIXEL20_31   *(dp + dst1line + dst1line) = Interp03(w5, w8);
#define X4PIXEL20_50   *(dp + dst1line + dst1line) = Interp05(w4, w5);
#define X4PIXEL20_60   *(dp + dst1line + dst1line) = Interp06(w5, w4, w8);
#define X4PIXEL20_61   *(dp + dst1line + dst1line) = Interp06(w5, w4, w7);
#define X4PIXEL20_82   *(dp + dst1line + dst1line) = Interp08(w5, w4);
#define X4PIXEL20_83   *(dp + dst1line + dst1line) = Interp08(w4, w8);

#define X4PIXEL21_0    *(dp + dst1line + dst1line + 1) = w5;
#define X4PIXEL21_30   *(dp + dst1line + dst1line + 1) = Interp03(w5, w7);
#define X4PIXEL21_31   *(dp + dst1line + dst1line + 1) = Interp03(w5, w8);
#define X4PIXEL21_32   *(dp + dst1line + dst1line + 1) = Interp03(w5, w4);
#define X4PIXEL21_70   *(dp + dst1line + dst1line + 1) = Interp07(w5, w4, w8);

#define X4PIXEL22_0    *(dp + dst1line + dst1line + 2) = w5;
#define X4PIXEL22_30   *(dp + dst1line + dst1line + 2) = Interp03(w5, w9);
#define X4PIXEL22_31   *(dp + dst1line + dst1line + 2) = Interp03(w5, w6);
#define X4PIXEL22_32   *(dp + dst1line + dst1line + 2) = Interp03(w5, w8);
#define X4PIXEL22_70   *(dp + dst1line + dst1line + 2) = Interp07(w5, w6, w8);

#define X4PIXEL23_0    *(dp + dst1line + dst1line + 3) = w5;
#define X4PIXEL23_10   *(dp + dst1line + dst1line + 3) = Interp01(w5, w9);
#define X4PIXEL23_11   *(dp + dst1line + dst1line + 3) = Interp01(w5, w6);
#define X4PIXEL23_13   *(dp + dst1line + dst1line + 3) = Interp01(w6, w5);
#define X4PIXEL23_21   *(dp + dst1line + dst1line + 3) = Interp02(w6, w5, w8);
#define X4PIXEL23_32   *(dp + dst1line + dst1line + 3) = Interp03(w5, w8);
#define X4PIXEL23_50   *(dp + dst1line + dst1line + 3) = Interp05(w6, w5);
#define X4PIXEL23_60   *(dp + dst1line + dst1line + 3) = Interp06(w5, w6, w8);
#define X4PIXEL23_61   *(dp + dst1line + dst1line + 3) = Interp06(w5, w6, w9);
#define X4PIXEL23_81   *(dp + dst1line + dst1line + 3) = Interp08(w5, w6);
#define X4PIXEL23_83   *(dp + dst1line + dst1line + 3) = Interp08(w6, w8);

#define X4PIXEL30_0    *(dp + dst1line + dst1line + dst1line) = w5;
#define X4PIXEL30_11   *(dp + dst1line + dst1line + dst1line) = Interp01(w5, w8);
#define X4PIXEL30_12   *(dp + dst1line + dst1line + dst1line) = Interp01(w5, w4);
#define X4PIXEL30_20   *(dp + dst1line + dst1line + dst1line) = Interp02(w5, w8, w4);
#define X4PIXEL30_50   *(dp + dst1line + dst1line + dst1line) = Interp05(w8, w4);
#define X4PIXEL30_80   *(dp + dst1line + dst1line + dst1line) = Interp08(w5, w7);
#define X4PIXEL30_81   *(dp + dst1line + dst1line + dst1line) = Interp08(w5, w8);
#define X4PIXEL30_82   *(dp + dst1line + dst1line + dst1line) = Interp08(w5, w4);

#define X4PIXEL31_0    *(dp + dst1line + dst1line + dst1line + 1) = w5;
#define X4PIXEL31_10   *(dp + dst1line + dst1line + dst1line + 1) = Interp01(w5, w7);
#define X4PIXEL31_11   *(dp + dst1line + dst1line + dst1line + 1) = Interp01(w5, w8);
#define X4PIXEL31_13   *(dp + dst1line + dst1line + dst1line + 1) = Interp01(w8, w5);
#define X4PIXEL31_21   *(dp + dst1line + dst1line + dst1line + 1) = Interp02(w8, w5, w4);
#define X4PIXEL31_32   *(dp + dst1line + dst1line + dst1line + 1) = Interp03(w5, w4);
#define X4PIXEL31_50   *(dp + dst1line + dst1line + dst1line + 1) = Interp05(w8, w5);
#define X4PIXEL31_60   *(dp + dst1line + dst1line + dst1line + 1) = Interp06(w5, w8, w4);
#define X4PIXEL31_61   *(dp + dst1line + dst1line + dst1line + 1) = Interp06(w5, w8, w7);
#define X4PIXEL31_81   *(dp + dst1line + dst1line + dst1line + 1) = Interp08(w5, w8);
#define X4PIXEL31_83   *(dp + dst1line + dst1line + dst1line + 1) = Interp08(w8, w4);

#define X4PIXEL32_0    *(dp + dst1line + dst1line + dst1line + 2) = w5;
#define X4PIXEL32_10   *(dp + dst1line + dst1line + dst1line + 2) = Interp01(w5, w9);
#define X4PIXEL32_12   *(dp + dst1line + dst1line + dst1line + 2) = Interp01(w5, w8);
#define X4PIXEL32_14   *(dp + dst1line + dst1line + dst1line + 2) = Interp01(w8, w5);
#define X4PIXEL32_21   *(dp + dst1line + dst1line + dst1line + 2) = Interp02(w8, w5, w6);
#define X4PIXEL32_31   *(dp + dst1line + dst1line + dst1line + 2) = Interp03(w5, w6);
#define X4PIXEL32_50   *(dp + dst1line + dst1line + dst1line + 2) = Interp05(w8, w5);
#define X4PIXEL32_60   *(dp + dst1line + dst1line + dst1line + 2) = Interp06(w5, w8, w6);
#define X4PIXEL32_61   *(dp + dst1line + dst1line + dst1line + 2) = Interp06(w5, w8, w9);
#define X4PIXEL32_82   *(dp + dst1line + dst1line + dst1line + 2) = Interp08(w5, w8);
#define X4PIXEL32_83   *(dp + dst1line + dst1line + dst1line + 2) = Interp08(w8, w6);

#define X4PIXEL33_0    *(dp + dst1line + dst1line + dst1line + 3) = w5;
#define X4PIXEL33_11   *(dp + dst1line + dst1line + dst1line + 3) = Interp01(w5, w6);
#define X4PIXEL33_12   *(dp + dst1line + dst1line + dst1line + 3) = Interp01(w5, w8);
#define X4PIXEL33_20   *(dp + dst1line + dst1line + dst1line + 3) = Interp02(w5, w8, w6);
#define X4PIXEL33_50   *(dp + dst1line + dst1line + dst1line + 3) = Interp05(w8, w6);
#define X4PIXEL33_80   *(dp + dst1line + dst1line + dst1line + 3) = Interp08(w5, w9);
#define X4PIXEL33_81   *(dp + dst1line + dst1line + dst1line + 3) = Interp08(w5, w6);
#define X4PIXEL33_82   *(dp + dst1line + dst1line + dst1line + 3) = Interp08(w5, w8);

#define Absolute(c) \
(!(c & (1 << 31)) ? c : (~c + 1))

struct filter_data
{
   unsigned in_fmt;
};

static uint16_t *RGBtoYUV = NULL;

#define Interp01(c1, c2) \
(c1 == c2) ? c1 : \
(((((c1 & MASK16_2) *  3 + (c2 & MASK16_2)    ) >> 2) & MASK16_2) + \
 ((((c1 & MASK16_13) *  3 + (c2 & MASK16_13)    ) >> 2) & MASK16_13))

#define Interp02(c1, c2, c3) \
(((((c1 & MASK16_2) *  2 + (c2 & MASK16_2)     + (c3 & MASK16_2)    ) >> 2) & MASK16_2) + \
 ((((c1 & MASK16_13) *  2 + (c2 & MASK16_13)     + (c3 & MASK16_13)    ) >> 2) & MASK16_13))

#define Interp03(c1, c2) \
(c1 == c2) ? c1 : \
(((((c1 & MASK16_2) *  7 + (c2 & MASK16_2)    ) >> 3) & MASK16_2) + \
 ((((c1 & MASK16_13) *  7 + (c2 & MASK16_13)    ) >> 3) & MASK16_13))

#define Interp04(c1, c2, c3) \
(((((c1 & MASK16_2) *  2 + (c2 & MASK16_2) * 7 + (c3 & MASK16_2) * 7) >> 4) & MASK16_2) + \
 ((((c1 & MASK16_13) *  2 + (c2 & MASK16_13) * 7 + (c3 & MASK16_13) * 7) >> 4) & MASK16_13))

#define Interp05(c1, c2) \
(c1 == c2) ? c1 : \
(((((c1 & MASK16_2)      + (c2 & MASK16_2))     >> 1) & MASK16_2) + \
 ((((c1 & MASK16_13)      + (c2 & MASK16_13))     >> 1) & MASK16_13))

#define Interp06(c1, c2, c3) \
(((((c1 & MASK16_2) *  5 + (c2 & MASK16_2) * 2 + (c3 & MASK16_2)    ) >> 3) & MASK16_2) + \
 ((((c1 & MASK16_13) *  5 + (c2 & MASK16_13) * 2 + (c3 & MASK16_13)    ) >> 3) & MASK16_13))

#define Interp07(c1, c2, c3) \
(((((c1 & MASK16_2) *  6 + (c2 & MASK16_2)     + (c3 & MASK16_2)    ) >> 3) & MASK16_2) + \
 ((((c1 & MASK16_13) *  6 + (c2 & MASK16_13)     + (c3 & MASK16_13)    ) >> 3) & MASK16_13))

#define Interp08(c1, c2) \
(c1 == c2) ? c1 : \
(((((c1 & MASK16_2) *  5 + (c2 & MASK16_2) * 3) >> 3) & MASK16_2) + \
 ((((c1 & MASK16_13) *  5 + (c2 & MASK16_13) * 3) >> 3) & MASK16_13))

#define Interp09(c1, c2, c3) \
(((((c1 & MASK16_2) *  2 + (c2 & MASK16_2) * 3 + (c3 & MASK16_2) * 3) >> 3) & MASK16_2) + \
 ((((c1 & MASK16_13) *  2 + (c2 & MASK16_13) * 3 + (c3 & MASK16_13) * 3) >> 3) & MASK16_13))

#define Interp10(c1, c2, c3) \
(((((c1 & MASK16_2) * 14 + (c2 & MASK16_2)     + (c3 & MASK16_2)    ) >> 4) & MASK16_2) + \
 ((((c1 & MASK16_13) * 14 + (c2 & MASK16_13)     + (c3 & MASK16_13)    ) >> 4) & MASK16_13))

#define DECOMPOSE_PIXEL(PIX, R, G, B)   { (R) = (PIX) >> 11; (G) = ((PIX) >> 6) & 0x1f; (B) = (PIX) & 0x1f; }

static void InitLUTs (void)
{
   uint32_t   r, g, b, i;
   int      y, u, v;

   for (i = 0 ; i < (1 << 15) ; i++)
   {
      DECOMPOSE_PIXEL(i, r, g, b);
      r <<= 3;
      g <<= 3;
      b <<= 3;

      y = (int) ( 0.256788f * r + 0.504129f * g + 0.097906f * b + 0.5f) + 16;
      u = (int) (-0.148223f * r - 0.290993f * g + 0.439216f * b + 0.5f) + 128;
      v = (int) ( 0.439216f * r - 0.367788f * g - 0.071427f * b + 0.5f) + 128;

      RGBtoYUV[i] = (y << 16) + (u << 8) + v;
   }
}

static inline unsigned char Diff (int c1, int c2)
{
   return  (( abs((c1 & Ymask) - (c2 & Ymask)) > trY ) ||
            ( abs((c1 & Umask) - (c2 & Umask)) > trU ) ||
            ( abs((c1 & Vmask) - (c2 & Vmask)) > trV ) );
}

static void hq2x_16_rgb565 (int width, int height,
      uint16_t *sp, int src1line, uint16_t *dp, int dst1line)
{
   int   w1, w2, w3, w4, w5, w6, w7, w8, w9;

   uint32_t  pattern;
   int     l, y;

   while (height--)
   {
      sp--;

      w1 = *(sp - src1line);
      w4 = *(sp);
      w7 = *(sp + src1line);

      sp++;

      w2 = *(sp - src1line);
      w5 = *(sp);
      w8 = *(sp + src1line);

      for (l = width; l; l--)
      {
         sp++;

         w3 = *(sp - src1line);
         w6 = *(sp);
         w9 = *(sp + src1line);

         y = RGBtoYUV[w5];
         pattern = 0;

         if ((w1 != w5) && (Diff(y, RGBtoYUV[w1]))) pattern |= (1 << 0);
         if ((w2 != w5) && (Diff(y, RGBtoYUV[w2]))) pattern |= (1 << 1);
         if ((w3 != w5) && (Diff(y, RGBtoYUV[w3]))) pattern |= (1 << 2);
         if ((w4 != w5) && (Diff(y, RGBtoYUV[w4]))) pattern |= (1 << 3);
         if ((w6 != w5) && (Diff(y, RGBtoYUV[w6]))) pattern |= (1 << 4);
         if ((w7 != w5) && (Diff(y, RGBtoYUV[w7]))) pattern |= (1 << 5);
         if ((w8 != w5) && (Diff(y, RGBtoYUV[w8]))) pattern |= (1 << 6);
         if ((w9 != w5) && (Diff(y, RGBtoYUV[w9]))) pattern |= (1 << 7);

         switch (pattern)
         {
            case 0:
            case 1:
            case 4:
            case 32:
            case 128:
            case 5:
            case 132:
            case 160:
            case 33:
            case 129:
            case 36:
            case 133:
            case 164:
            case 161:
            case 37:
            case 165:
            {
               X2PIXEL00_20
               X2PIXEL01_20
               X2PIXEL10_20
               X2PIXEL11_20
               break;
            }
            case 2:
            case 34:
            case 130:
            case 162:
            {
               X2PIXEL00_22
               X2PIXEL01_21
               X2PIXEL10_20
               X2PIXEL11_20
               break;
            }
            case 16:
            case 17:
            case 48:
            case 49:
            {
               X2PIXEL00_20
               X2PIXEL01_22
               X2PIXEL10_20
               X2PIXEL11_21
               break;
            }
            case 64:
            case 65:
            case 68:
            case 69:
            {
               X2PIXEL00_20
               X2PIXEL01_20
               X2PIXEL10_21
               X2PIXEL11_22
               break;
            }
            case 8:
            case 12:
            case 136:
            case 140:
            {
               X2PIXEL00_21
               X2PIXEL01_20
               X2PIXEL10_22
               X2PIXEL11_20
               break;
            }
            case 3:
            case 35:
            case 131:
            case 163:
            {
               X2PIXEL00_11
               X2PIXEL01_21
               X2PIXEL10_20
               X2PIXEL11_20
               break;
            }
            case 6:
            case 38:
            case 134:
            case 166:
            {
               X2PIXEL00_22
               X2PIXEL01_12
               X2PIXEL10_20
               X2PIXEL11_20
               break;
            }
            case 20:
            case 21:
            case 52:
            case 53:
            {
               X2PIXEL00_20
               X2PIXEL01_11
               X2PIXEL10_20
               X2PIXEL11_21
               break;
            }
            case 144:
            case 145:
            case 176:
            case 177:
            {
               X2PIXEL00_20
               X2PIXEL01_22
               X2PIXEL10_20
               X2PIXEL11_12
               break;
            }
            case 192:
            case 193:
            case 196:
            case 197:
            {
               X2PIXEL00_20
               X2PIXEL01_20
               X2PIXEL10_21
               X2PIXEL11_11
               break;
            }
            case 96:
            case 97:
            case 100:
            case 101:
            {
               X2PIXEL00_20
               X2PIXEL01_20
               X2PIXEL10_12
               X2PIXEL11_22
               break;
            }
            case 40:
            case 44:
            case 168:
            case 172:
            {
               X2PIXEL00_21
               X2PIXEL01_20
               X2PIXEL10_11
               X2PIXEL11_20
               break;
            }
            case 9:
            case 13:
            case 137:
            case 141:
            {
               X2PIXEL00_12
               X2PIXEL01_20
               X2PIXEL10_22
               X2PIXEL11_20
               break;
            }
            case 18:
            case 50:
            {
               X2PIXEL00_22
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_10
               }
               else
               {
                  X2PIXEL01_20
               }
               X2PIXEL10_20
               X2PIXEL11_21
               break;
            }
            case 80:
            case 81:
            {
               X2PIXEL00_20
               X2PIXEL01_22
               X2PIXEL10_21
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_10
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 72:
            case 76:
            {
               X2PIXEL00_21
               X2PIXEL01_20
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_10
               }
               else
               {
                  X2PIXEL10_20
               }
               X2PIXEL11_22
               break;
            }
            case 10:
            case 138:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_10
               }
               else
               {
                  X2PIXEL00_20
               }
               X2PIXEL01_21
               X2PIXEL10_22
               X2PIXEL11_20
               break;
            }
            case 66:
            {
               X2PIXEL00_22
               X2PIXEL01_21
               X2PIXEL10_21
               X2PIXEL11_22
               break;
            }
            case 24:
            {
               X2PIXEL00_21
               X2PIXEL01_22
               X2PIXEL10_22
               X2PIXEL11_21
               break;
            }
            case 7:
            case 39:
            case 135:
            {
               X2PIXEL00_11
               X2PIXEL01_12
               X2PIXEL10_20
               X2PIXEL11_20
               break;
            }
            case 148:
            case 149:
            case 180:
            {
               X2PIXEL00_20
               X2PIXEL01_11
               X2PIXEL10_20
               X2PIXEL11_12
               break;
            }
            case 224:
            case 228:
            case 225:
            {
               X2PIXEL00_20
               X2PIXEL01_20
               X2PIXEL10_12
               X2PIXEL11_11
               break;
            }
            case 41:
            case 169:
            case 45:
            {
               X2PIXEL00_12
               X2PIXEL01_20
               X2PIXEL10_11
               X2PIXEL11_20
               break;
            }
            case 22:
            case 54:
            {
               X2PIXEL00_22
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_20
               }
               X2PIXEL10_20
               X2PIXEL11_21
               break;
            }
            case 208:
            case 209:
            {
               X2PIXEL00_20
               X2PIXEL01_22
               X2PIXEL10_21
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 104:
            case 108:
            {
               X2PIXEL00_21
               X2PIXEL01_20
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_20
               }
               X2PIXEL11_22
               break;
            }
            case 11:
            case 139:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_20
               }
               X2PIXEL01_21
               X2PIXEL10_22
               X2PIXEL11_20
               break;
            }
            case 19:
            case 51:
            {
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL00_11
                  X2PIXEL01_10
               }
               else
               {
                  X2PIXEL00_60
                  X2PIXEL01_90
               }
               X2PIXEL10_20
               X2PIXEL11_21
               break;
            }
            case 146:
            case 178:
            {
               X2PIXEL00_22
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_10
                  X2PIXEL11_12
               }
               else
               {
                  X2PIXEL01_90
                  X2PIXEL11_61
               }
               X2PIXEL10_20
               break;
            }
            case 84:
            case 85:
            {
               X2PIXEL00_20
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL01_11
                  X2PIXEL11_10
               }
               else
               {
                  X2PIXEL01_60
                  X2PIXEL11_90
               }
               X2PIXEL10_21
               break;
            }
            case 112:
            case 113:
            {
               X2PIXEL00_20
               X2PIXEL01_22
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL10_12
                  X2PIXEL11_10
               }
               else
               {
                  X2PIXEL10_61
                  X2PIXEL11_90
               }
               break;
            }
            case 200:
            case 204:
            {
               X2PIXEL00_21
               X2PIXEL01_20
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_10
                  X2PIXEL11_11
               }
               else
               {
                  X2PIXEL10_90
                  X2PIXEL11_60
               }
               break;
            }
            case 73:
            case 77:
            {
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL00_12
                  X2PIXEL10_10
               }
               else
               {
                  X2PIXEL00_61
                  X2PIXEL10_90
               }
               X2PIXEL01_20
               X2PIXEL11_22
               break;
            }
            case 42:
            case 170:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_10
                  X2PIXEL10_11
               }
               else
               {
                  X2PIXEL00_90
                  X2PIXEL10_60
               }
               X2PIXEL01_21
               X2PIXEL11_20
               break;
            }
            case 14:
            case 142:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_10
                  X2PIXEL01_12
               }
               else
               {
                  X2PIXEL00_90
                  X2PIXEL01_61
               }
               X2PIXEL10_22
               X2PIXEL11_20
               break;
            }
            case 67:
            {
               X2PIXEL00_11
               X2PIXEL01_21
               X2PIXEL10_21
               X2PIXEL11_22
               break;
            }
            case 70:
            {
               X2PIXEL00_22
               X2PIXEL01_12
               X2PIXEL10_21
               X2PIXEL11_22
               break;
            }
            case 28:
            {
               X2PIXEL00_21
               X2PIXEL01_11
               X2PIXEL10_22
               X2PIXEL11_21
               break;
            }
            case 152:
            {
               X2PIXEL00_21
               X2PIXEL01_22
               X2PIXEL10_22
               X2PIXEL11_12
               break;
            }
            case 194:
            {
               X2PIXEL00_22
               X2PIXEL01_21
               X2PIXEL10_21
               X2PIXEL11_11
               break;
            }
            case 98:
            {
               X2PIXEL00_22
               X2PIXEL01_21
               X2PIXEL10_12
               X2PIXEL11_22
               break;
            }
            case 56:
            {
               X2PIXEL00_21
               X2PIXEL01_22
               X2PIXEL10_11
               X2PIXEL11_21
               break;
            }
            case 25:
            {
               X2PIXEL00_12
               X2PIXEL01_22
               X2PIXEL10_22
               X2PIXEL11_21
               break;
            }
            case 26:
            case 31:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_20
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_20
               }
               X2PIXEL10_22
               X2PIXEL11_21
               break;
            }
            case 82:
            case 214:
            {
               X2PIXEL00_22
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_20
               }
               X2PIXEL10_21
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 88:
            case 248:
            {
               X2PIXEL00_21
               X2PIXEL01_22
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_20
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 74:
            case 107:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_20
               }
               X2PIXEL01_21
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_20
               }
               X2PIXEL11_22
               break;
            }
            case 27:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_20
               }
               X2PIXEL01_10
               X2PIXEL10_22
               X2PIXEL11_21
               break;
            }
            case 86:
            {
               X2PIXEL00_22
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_20
               }
               X2PIXEL10_21
               X2PIXEL11_10
               break;
            }
            case 216:
            {
               X2PIXEL00_21
               X2PIXEL01_22
               X2PIXEL10_10
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 106:
            {
               X2PIXEL00_10
               X2PIXEL01_21
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_20
               }
               X2PIXEL11_22
               break;
            }
            case 30:
            {
               X2PIXEL00_10
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_20
               }
               X2PIXEL10_22
               X2PIXEL11_21
               break;
            }
            case 210:
            {
               X2PIXEL00_22
               X2PIXEL01_10
               X2PIXEL10_21
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 120:
            {
               X2PIXEL00_21
               X2PIXEL01_22
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_20
               }
               X2PIXEL11_10
               break;
            }
            case 75:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_20
               }
               X2PIXEL01_21
               X2PIXEL10_10
               X2PIXEL11_22
               break;
            }
            case 29:
            {
               X2PIXEL00_12
               X2PIXEL01_11
               X2PIXEL10_22
               X2PIXEL11_21
               break;
            }
            case 198:
            {
               X2PIXEL00_22
               X2PIXEL01_12
               X2PIXEL10_21
               X2PIXEL11_11
               break;
            }
            case 184:
            {
               X2PIXEL00_21
               X2PIXEL01_22
               X2PIXEL10_11
               X2PIXEL11_12
               break;
            }
            case 99:
            {
               X2PIXEL00_11
               X2PIXEL01_21
               X2PIXEL10_12
               X2PIXEL11_22
               break;
            }
            case 57:
            {
               X2PIXEL00_12
               X2PIXEL01_22
               X2PIXEL10_11
               X2PIXEL11_21
               break;
            }
            case 71:
            {
               X2PIXEL00_11
               X2PIXEL01_12
               X2PIXEL10_21
               X2PIXEL11_22
               break;
            }
            case 156:
            {
               X2PIXEL00_21
               X2PIXEL01_11
               X2PIXEL10_22
               X2PIXEL11_12
               break;
            }
            case 226:
            {
               X2PIXEL00_22
               X2PIXEL01_21
               X2PIXEL10_12
               X2PIXEL11_11
               break;
            }
            case 60:
            {
               X2PIXEL00_21
               X2PIXEL01_11
               X2PIXEL10_11
               X2PIXEL11_21
               break;
            }
            case 195:
            {
               X2PIXEL00_11
               X2PIXEL01_21
               X2PIXEL10_21
               X2PIXEL11_11
               break;
            }
            case 102:
            {
               X2PIXEL00_22
               X2PIXEL01_12
               X2PIXEL10_12
               X2PIXEL11_22
               break;
            }
            case 153:
            {
               X2PIXEL00_12
               X2PIXEL01_22
               X2PIXEL10_22
               X2PIXEL11_12
               break;
            }
            case 58:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_10
               }
               else
               {
                  X2PIXEL00_70
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_10
               }
               else
               {
                  X2PIXEL01_70
               }
               X2PIXEL10_11
               X2PIXEL11_21
               break;
            }
            case 83:
            {
               X2PIXEL00_11
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_10
               }
               else
               {
                  X2PIXEL01_70
               }
               X2PIXEL10_21
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_10
               }
               else
               {
                  X2PIXEL11_70
               }
               break;
            }
            case 92:
            {
               X2PIXEL00_21
               X2PIXEL01_11
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_10
               }
               else
               {
                  X2PIXEL10_70
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_10
               }
               else
               {
                  X2PIXEL11_70
               }
               break;
            }
            case 202:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_10
               }
               else
               {
                  X2PIXEL00_70
               }
               X2PIXEL01_21
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_10
               }
               else
               {
                  X2PIXEL10_70
               }
               X2PIXEL11_11
               break;
            }
            case 78:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_10
               }
               else
               {
                  X2PIXEL00_70
               }
               X2PIXEL01_12
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_10
               }
               else
               {
                  X2PIXEL10_70
               }
               X2PIXEL11_22
               break;
            }
            case 154:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_10
               }
               else
               {
                  X2PIXEL00_70
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_10
               }
               else
               {
                  X2PIXEL01_70
               }
               X2PIXEL10_22
               X2PIXEL11_12
               break;
            }
            case 114:
            {
               X2PIXEL00_22
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_10
               }
               else
               {
                  X2PIXEL01_70
               }
               X2PIXEL10_12
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_10
               }
               else
               {
                  X2PIXEL11_70
               }
               break;
            }
            case 89:
            {
               X2PIXEL00_12
               X2PIXEL01_22
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_10
               }
               else
               {
                  X2PIXEL10_70
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_10
               }
               else
               {
                  X2PIXEL11_70
               }
               break;
            }
            case 90:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_10
               }
               else
               {
                  X2PIXEL00_70
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_10
               }
               else
               {
                  X2PIXEL01_70
               }
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_10
               }
               else
               {
                  X2PIXEL10_70
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_10
               }
               else
               {
                  X2PIXEL11_70
               }
               break;
            }
            case 55:
            case 23:
            {
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL00_11
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL00_60
                  X2PIXEL01_90
               }
               X2PIXEL10_20
               X2PIXEL11_21
               break;
            }
            case 182:
            case 150:
            {
               X2PIXEL00_22
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
                  X2PIXEL11_12
               }
               else
               {
                  X2PIXEL01_90
                  X2PIXEL11_61
               }
               X2PIXEL10_20
               break;
            }
            case 213:
            case 212:
            {
               X2PIXEL00_20
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL01_11
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL01_60
                  X2PIXEL11_90
               }
               X2PIXEL10_21
               break;
            }
            case 241:
            case 240:
            {
               X2PIXEL00_20
               X2PIXEL01_22
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL10_12
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL10_61
                  X2PIXEL11_90
               }
               break;
            }
            case 236:
            case 232:
            {
               X2PIXEL00_21
               X2PIXEL01_20
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
                  X2PIXEL11_11
               }
               else
               {
                  X2PIXEL10_90
                  X2PIXEL11_60
               }
               break;
            }
            case 109:
            case 105:
            {
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL00_12
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL00_61
                  X2PIXEL10_90
               }
               X2PIXEL01_20
               X2PIXEL11_22
               break;
            }
            case 171:
            case 43:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
                  X2PIXEL10_11
               }
               else
               {
                  X2PIXEL00_90
                  X2PIXEL10_60
               }
               X2PIXEL01_21
               X2PIXEL11_20
               break;
            }
            case 143:
            case 15:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
                  X2PIXEL01_12
               }
               else
               {
                  X2PIXEL00_90
                  X2PIXEL01_61
               }
               X2PIXEL10_22
               X2PIXEL11_20
               break;
            }
            case 124:
            {
               X2PIXEL00_21
               X2PIXEL01_11
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_20
               }
               X2PIXEL11_10
               break;
            }
            case 203:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_20
               }
               X2PIXEL01_21
               X2PIXEL10_10
               X2PIXEL11_11
               break;
            }
            case 62:
            {
               X2PIXEL00_10
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_20
               }
               X2PIXEL10_11
               X2PIXEL11_21
               break;
            }
            case 211:
            {
               X2PIXEL00_11
               X2PIXEL01_10
               X2PIXEL10_21
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 118:
            {
               X2PIXEL00_22
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_20
               }
               X2PIXEL10_12
               X2PIXEL11_10
               break;
            }
            case 217:
            {
               X2PIXEL00_12
               X2PIXEL01_22
               X2PIXEL10_10
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 110:
            {
               X2PIXEL00_10
               X2PIXEL01_12
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_20
               }
               X2PIXEL11_22
               break;
            }
            case 155:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_20
               }
               X2PIXEL01_10
               X2PIXEL10_22
               X2PIXEL11_12
               break;
            }
            case 188:
            {
               X2PIXEL00_21
               X2PIXEL01_11
               X2PIXEL10_11
               X2PIXEL11_12
               break;
            }
            case 185:
            {
               X2PIXEL00_12
               X2PIXEL01_22
               X2PIXEL10_11
               X2PIXEL11_12
               break;
            }
            case 61:
            {
               X2PIXEL00_12
               X2PIXEL01_11
               X2PIXEL10_11
               X2PIXEL11_21
               break;
            }
            case 157:
            {
               X2PIXEL00_12
               X2PIXEL01_11
               X2PIXEL10_22
               X2PIXEL11_12
               break;
            }
            case 103:
            {
               X2PIXEL00_11
               X2PIXEL01_12
               X2PIXEL10_12
               X2PIXEL11_22
               break;
            }
            case 227:
            {
               X2PIXEL00_11
               X2PIXEL01_21
               X2PIXEL10_12
               X2PIXEL11_11
               break;
            }
            case 230:
            {
               X2PIXEL00_22
               X2PIXEL01_12
               X2PIXEL10_12
               X2PIXEL11_11
               break;
            }
            case 199:
            {
               X2PIXEL00_11
               X2PIXEL01_12
               X2PIXEL10_21
               X2PIXEL11_11
               break;
            }
            case 220:
            {
               X2PIXEL00_21
               X2PIXEL01_11
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_10
               }
               else
               {
                  X2PIXEL10_70
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 158:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_10
               }
               else
               {
                  X2PIXEL00_70
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_20
               }
               X2PIXEL10_22
               X2PIXEL11_12
               break;
            }
            case 234:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_10
               }
               else
               {
                  X2PIXEL00_70
               }
               X2PIXEL01_21
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_20
               }
               X2PIXEL11_11
               break;
            }
            case 242:
            {
               X2PIXEL00_22
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_10
               }
               else
               {
                  X2PIXEL01_70
               }
               X2PIXEL10_12
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 59:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_20
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_10
               }
               else
               {
                  X2PIXEL01_70
               }
               X2PIXEL10_11
               X2PIXEL11_21
               break;
            }
            case 121:
            {
               X2PIXEL00_12
               X2PIXEL01_22
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_20
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_10
               }
               else
               {
                  X2PIXEL11_70
               }
               break;
            }
            case 87:
            {
               X2PIXEL00_11
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_20
               }
               X2PIXEL10_21
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_10
               }
               else
               {
                  X2PIXEL11_70
               }
               break;
            }
            case 79:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_20
               }
               X2PIXEL01_12
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_10
               }
               else
               {
                  X2PIXEL10_70
               }
               X2PIXEL11_22
               break;
            }
            case 122:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_10
               }
               else
               {
                  X2PIXEL00_70
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_10
               }
               else
               {
                  X2PIXEL01_70
               }
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_20
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_10
               }
               else
               {
                  X2PIXEL11_70
               }
               break;
            }
            case 94:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_10
               }
               else
               {
                  X2PIXEL00_70
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_20
               }
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_10
               }
               else
               {
                  X2PIXEL10_70
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_10
               }
               else
               {
                  X2PIXEL11_70
               }
               break;
            }
            case 218:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_10
               }
               else
               {
                  X2PIXEL00_70
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_10
               }
               else
               {
                  X2PIXEL01_70
               }
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_10
               }
               else
               {
                  X2PIXEL10_70
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 91:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_20
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_10
               }
               else
               {
                  X2PIXEL01_70
               }
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_10
               }
               else
               {
                  X2PIXEL10_70
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_10
               }
               else
               {
                  X2PIXEL11_70
               }
               break;
            }
            case 229:
            {
               X2PIXEL00_20
               X2PIXEL01_20
               X2PIXEL10_12
               X2PIXEL11_11
               break;
            }
            case 167:
            {
               X2PIXEL00_11
               X2PIXEL01_12
               X2PIXEL10_20
               X2PIXEL11_20
               break;
            }
            case 173:
            {
               X2PIXEL00_12
               X2PIXEL01_20
               X2PIXEL10_11
               X2PIXEL11_20
               break;
            }
            case 181:
            {
               X2PIXEL00_20
               X2PIXEL01_11
               X2PIXEL10_20
               X2PIXEL11_12
               break;
            }
            case 186:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_10
               }
               else
               {
                  X2PIXEL00_70
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_10
               }
               else
               {
                  X2PIXEL01_70
               }
               X2PIXEL10_11
               X2PIXEL11_12
               break;
            }
            case 115:
            {
               X2PIXEL00_11
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_10
               }
               else
               {
                  X2PIXEL01_70
               }
               X2PIXEL10_12
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_10
               }
               else
               {
                  X2PIXEL11_70
               }
               break;
            }
            case 93:
            {
               X2PIXEL00_12
               X2PIXEL01_11
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_10
               }
               else
               {
                  X2PIXEL10_70
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_10
               }
               else
               {
                  X2PIXEL11_70
               }
               break;
            }
            case 206:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_10
               }
               else
               {
                  X2PIXEL00_70
               }
               X2PIXEL01_12
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_10
               }
               else
               {
                  X2PIXEL10_70
               }
               X2PIXEL11_11
               break;
            }
            case 205:
            case 201:
            {
               X2PIXEL00_12
               X2PIXEL01_20
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_10
               }
               else
               {
                  X2PIXEL10_70
               }
               X2PIXEL11_11
               break;
            }
            case 174:
            case 46:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_10
               }
               else
               {
                  X2PIXEL00_70
               }
               X2PIXEL01_12
               X2PIXEL10_11
               X2PIXEL11_20
               break;
            }
            case 179:
            case 147:
            {
               X2PIXEL00_11
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_10
               }
               else
               {
                  X2PIXEL01_70
               }
               X2PIXEL10_20
               X2PIXEL11_12
               break;
            }
            case 117:
            case 116:
            {
               X2PIXEL00_20
               X2PIXEL01_11
               X2PIXEL10_12
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_10
               }
               else
               {
                  X2PIXEL11_70
               }
               break;
            }
            case 189:
            {
               X2PIXEL00_12
               X2PIXEL01_11
               X2PIXEL10_11
               X2PIXEL11_12
               break;
            }
            case 231:
            {
               X2PIXEL00_11
               X2PIXEL01_12
               X2PIXEL10_12
               X2PIXEL11_11
               break;
            }
            case 126:
            {
               X2PIXEL00_10
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_20
               }
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_20
               }
               X2PIXEL11_10
               break;
            }
            case 219:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_20
               }
               X2PIXEL01_10
               X2PIXEL10_10
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 125:
            {
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL00_12
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL00_61
                  X2PIXEL10_90
               }
               X2PIXEL01_11
               X2PIXEL11_10
               break;
            }
            case 221:
            {
               X2PIXEL00_12
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL01_11
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL01_60
                  X2PIXEL11_90
               }
               X2PIXEL10_10
               break;
            }
            case 207:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
                  X2PIXEL01_12
               }
               else
               {
                  X2PIXEL00_90
                  X2PIXEL01_61
               }
               X2PIXEL10_10
               X2PIXEL11_11
               break;
            }
            case 238:
            {
               X2PIXEL00_10
               X2PIXEL01_12
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
                  X2PIXEL11_11
               }
               else
               {
                  X2PIXEL10_90
                  X2PIXEL11_60
               }
               break;
            }
            case 190:
            {
               X2PIXEL00_10
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
                  X2PIXEL11_12
               }
               else
               {
                  X2PIXEL01_90
                  X2PIXEL11_61
               }
               X2PIXEL10_11
               break;
            }
            case 187:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
                  X2PIXEL10_11
               }
               else
               {
                  X2PIXEL00_90
                  X2PIXEL10_60
               }
               X2PIXEL01_10
               X2PIXEL11_12
               break;
            }
            case 243:
            {
               X2PIXEL00_11
               X2PIXEL01_10
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL10_12
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL10_61
                  X2PIXEL11_90
               }
               break;
            }
            case 119:
            {
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL00_11
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL00_60
                  X2PIXEL01_90
               }
               X2PIXEL10_12
               X2PIXEL11_10
               break;
            }
            case 237:
            case 233:
            {
               X2PIXEL00_12
               X2PIXEL01_20
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_100
               }
               X2PIXEL11_11
               break;
            }
            case 175:
            case 47:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_100
               }
               X2PIXEL01_12
               X2PIXEL10_11
               X2PIXEL11_20
               break;
            }
            case 183:
            case 151:
            {
               X2PIXEL00_11
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_100
               }
               X2PIXEL10_20
               X2PIXEL11_12
               break;
            }
            case 245:
            case 244:
            {
               X2PIXEL00_20
               X2PIXEL01_11
               X2PIXEL10_12
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_100
               }
               break;
            }
            case 250:
            {
               X2PIXEL00_10
               X2PIXEL01_10
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_20
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 123:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_20
               }
               X2PIXEL01_10
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_20
               }
               X2PIXEL11_10
               break;
            }
            case 95:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_20
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_20
               }
               X2PIXEL10_10
               X2PIXEL11_10
               break;
            }
            case 222:
            {
               X2PIXEL00_10
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_20
               }
               X2PIXEL10_10
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 252:
            {
               X2PIXEL00_21
               X2PIXEL01_11
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_20
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_100
               }
               break;
            }
            case 249:
            {
               X2PIXEL00_12
               X2PIXEL01_22
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_100
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 235:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_20
               }
               X2PIXEL01_21
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_100
               }
               X2PIXEL11_11
               break;
            }
            case 111:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_100
               }
               X2PIXEL01_12
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_20
               }
               X2PIXEL11_22
               break;
            }
            case 63:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_100
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_20
               }
               X2PIXEL10_11
               X2PIXEL11_21
               break;
            }
            case 159:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_20
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_100
               }
               X2PIXEL10_22
               X2PIXEL11_12
               break;
            }
            case 215:
            {
               X2PIXEL00_11
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_100
               }
               X2PIXEL10_21
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 246:
            {
               X2PIXEL00_22
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_20
               }
               X2PIXEL10_12
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_100
               }
               break;
            }
            case 254:
            {
               X2PIXEL00_10
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_20
               }
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_20
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_100
               }
               break;
            }
            case 253:
            {
               X2PIXEL00_12
               X2PIXEL01_11
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_100
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_100
               }
               break;
            }
            case 251:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_20
               }
               X2PIXEL01_10
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_100
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 239:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_100
               }
               X2PIXEL01_12
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_100
               }
               X2PIXEL11_11
               break;
            }
            case 127:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_100
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_20
               }
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_20
               }
               X2PIXEL11_10
               break;
            }
            case 191:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_100
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_100
               }
               X2PIXEL10_11
               X2PIXEL11_12
               break;
            }
            case 223:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_20
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_100
               }
               X2PIXEL10_10
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_20
               }
               break;
            }
            case 247:
            {
               X2PIXEL00_11
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_100
               }
               X2PIXEL10_12
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_100
               }
               break;
            }
            case 255:
            {
               if (Diff(RGBtoYUV[w4], RGBtoYUV[w2]))
               {
                  X2PIXEL00_0
               }
               else
               {
                  X2PIXEL00_100
               }
               if (Diff(RGBtoYUV[w2], RGBtoYUV[w6]))
               {
                  X2PIXEL01_0
               }
               else
               {
                  X2PIXEL01_100
               }
               if (Diff(RGBtoYUV[w8], RGBtoYUV[w4]))
               {
                  X2PIXEL10_0
               }
               else
               {
                  X2PIXEL10_100
               }
               if (Diff(RGBtoYUV[w6], RGBtoYUV[w8]))
               {
                  X2PIXEL11_0
               }
               else
               {
                  X2PIXEL11_100
               }
               break;
            }
         }

         w1 = w2; w4 = w5; w7 = w8;
         w2 = w3; w5 = w6; w8 = w9;

         dp += 2;
      }

      dp += (dst1line - width) * 2;
      sp += (src1line - width);
   }
}

static unsigned hq2x_generic_input_fmts(void)
{
   return SOFTFILTER_FMT_RGB565;
}

static unsigned hq2x_generic_output_fmts(unsigned input_fmts)
{
   return input_fmts;
}

static void hq2x_generic_destroy(void *data)
{
   struct filter_data *filt = (struct filter_data*)data;
   
   if(RGBtoYUV) 
      free(RGBtoYUV);
   free(filt);
}

static void *hq2x_generic_create(unsigned in_fmt)
{
   struct filter_data *filt = (struct filter_data*)calloc(1, sizeof(*filt));
   if (!filt)
      return NULL;

   filt->in_fmt  = in_fmt;

   RGBtoYUV = (uint16_t *)calloc(1 << 15, sizeof(RGBtoYUV));
   if (!RGBtoYUV)
   {
      free(filt);
      return NULL;
   }

   InitLUTs();
   
   return filt;
}

static void hq2x_generic_output(void *data, unsigned *out_width, unsigned *out_height,
      unsigned width, unsigned height)
{
   *out_width = width * HQ2X_SCALE;
   *out_height = height * HQ2X_SCALE;
}

static void hq2x_generic_render(void *data,
      void *output, size_t output_stride,
      const void *input, unsigned width, unsigned height, size_t input_stride)
{
   struct filter_data *filt = (struct filter_data*)data;

   if (filt->in_fmt == SOFTFILTER_FMT_RGB565)
      hq2x_16_rgb565(width, height,
         (uint16_t*)input, input_stride / SOFTFILTER_BPP_RGB565, 
         (uint16_t*)output, output_stride / SOFTFILTER_BPP_RGB565);
}

static const struct softfilter_implementation hq2x_generic = {
   hq2x_generic_input_fmts,
   hq2x_generic_output_fmts,

   hq2x_generic_create,
   hq2x_generic_destroy,

   hq2x_generic_output,
   hq2x_generic_render,
   "HQ2x",
};

const struct softfilter_implementation *softfilter_get_implementation(void)
{
   return &hq2x_generic;
}

#ifdef RARCH_INTERNAL
#undef softfilter_get_implementation
#undef filter_data
#endif
