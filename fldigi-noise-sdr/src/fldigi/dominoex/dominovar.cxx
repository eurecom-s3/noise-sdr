/*
 *    dominovar.cxx  --  DominoEX Varicode
 *
 *    Copyright (C) 2001, 2002, 2003
 *      Tomi Manninen (oh2bns@sral.fi)
 *
 *    This file is part of fldigi.
 *
 *    Fldigi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    Fldigi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with fldigi.  If not, see <http://www.gnu.org/licenses/>.
 */

// Android #include <config.h>

#include "dominovar.h"

static unsigned char varicode[][3] = {
    /* Primary alphabet */
    {1, 15, 9},
    {1, 15, 10},
    {1, 15, 11},
    {1, 15, 12},
    {1, 15, 13},
    {1, 15, 14},
    {1, 15, 15},
    {2, 8, 8},
    {2, 12, 0},
    {2, 8, 9},
    {2, 8, 10},
    {2, 8, 11},
    {2, 8, 12},
    {2, 13, 0},
    {2, 8, 13},
    {2, 8, 14},
    {2, 8, 15},
    {2, 9, 8},
    {2, 9, 9},
    {2, 9, 10},
    {2, 9, 11},
    {2, 9, 12},
    {2, 9, 13},
    {2, 9, 14},
    {2, 9, 15},
    {2, 10, 8},
    {2, 10, 9},
    {2, 10, 10},
    {2, 10, 11},
    {2, 10, 12},
    {2, 10, 13},
    {2, 10, 14},
    {0, 0, 0},
    {7, 11, 0},
    {0, 8, 14},
    {0, 10, 11},
    {0, 9, 10},
    {0, 9, 9},
    {0, 8, 15},
    {7, 10, 0},
    {0, 8, 12},
    {0, 8, 11},
    {0, 9, 13},
    {0, 8, 8},
    {2, 11, 0},
    {7, 14, 0},
    {7, 13, 0},
    {0, 8, 9},
    {3, 15, 0},
    {4, 10, 0},
    {4, 15, 0},
    {5, 9, 0},
    {6, 8, 0},
    {5, 12, 0},
    {5, 14, 0},
    {6, 12, 0},
    {6, 11, 0},
    {6, 14, 0},
    {0, 8, 10},
    {0, 8, 13},
    {0, 10, 8},
    {7, 15, 0},
    {0, 9, 15},
    {7, 12, 0},
    {0, 9, 8},
    {3, 9, 0},
    {4, 14, 0},
    {3, 12, 0},
    {3, 14, 0},
    {3, 8, 0},
    {4, 12, 0},
    {5, 8, 0},
    {5, 10, 0},
    {3, 10, 0},
    {7, 8, 0},
    {6, 10, 0},
    {4, 11, 0},
    {4, 8, 0},
    {4, 13, 0},
    {3, 11, 0},
    {4, 9, 0},
    {6, 15, 0},
    {3, 13, 0},
    {2, 15, 0},
    {2, 14, 0},
    {5, 11, 0},
    {6, 13, 0},
    {5, 13, 0},
    {5, 15, 0},
    {6, 9, 0},
    {7, 9, 0},
    {0, 10, 14},
    {0, 10, 9},
    {0, 10, 15},
    {0, 10, 10},
    {0, 9, 12},
    {0, 9, 11},
    {4, 0, 0},
    {1, 11, 0},
    {0, 12, 0},
    {0, 11, 0},
    {1, 0, 0},
    {0, 15, 0},
    {1, 9, 0},
    {0, 10, 0},
    {5, 0, 0},
    {2, 10, 0},
    {1, 14, 0},
    {0, 9, 0},
    {0, 14, 0},
    {6, 0, 0},
    {3, 0, 0},
    {1, 8, 0},
    {2, 8, 0},
    {7, 0, 0},
    {0, 8, 0},
    {2, 0, 0},
    {0, 13, 0},
    {1, 13, 0},
    {1, 12, 0},
    {1, 15, 0},
    {1, 10, 0},
    {2, 9, 0},
    {0, 10, 12},
    {0, 9, 14},
    {0, 10, 13},
    {0, 11, 8},
    {2, 10, 15},
    {2, 11, 8},
    {2, 11, 9},
    {2, 11, 10},
    {2, 11, 11},
    {2, 11, 12},
    {2, 11, 13},
    {2, 11, 14},
    {2, 11, 15},
    {2, 12, 8},
    {2, 12, 9},
    {2, 12, 10},
    {2, 12, 11},
    {2, 12, 12},
    {2, 12, 13},
    {2, 12, 14},
    {2, 12, 15},
    {2, 13, 8},
    {2, 13, 9},
    {2, 13, 10},
    {2, 13, 11},
    {2, 13, 12},
    {2, 13, 13},
    {2, 13, 14},
    {2, 13, 15},
    {2, 14, 8},
    {2, 14, 9},
    {2, 14, 10},
    {2, 14, 11},
    {2, 14, 12},
    {2, 14, 13},
    {2, 14, 14},
    {2, 14, 15},
    {0, 11, 9},
    {0, 11, 10},
    {0, 11, 11},
    {0, 11, 12},
    {0, 11, 13},
    {0, 11, 14},
    {0, 11, 15},
    {0, 12, 8},
    {0, 12, 9},
    {0, 12, 10},
    {0, 12, 11},
    {0, 12, 12},
    {0, 12, 13},
    {0, 12, 14},
    {0, 12, 15},
    {0, 13, 8},
    {0, 13, 9},
    {0, 13, 10},
    {0, 13, 11},
    {0, 13, 12},
    {0, 13, 13},
    {0, 13, 14},
    {0, 13, 15},
    {0, 14, 8},
    {0, 14, 9},
    {0, 14, 10},
    {0, 14, 11},
    {0, 14, 12},
    {0, 14, 13},
    {0, 14, 14},
    {0, 14, 15},
    {0, 15, 8},
    {0, 15, 9},
    {0, 15, 10},
    {0, 15, 11},
    {0, 15, 12},
    {0, 15, 13},
    {0, 15, 14},
    {0, 15, 15},
    {1, 8, 8},
    {1, 8, 9},
    {1, 8, 10},
    {1, 8, 11},
    {1, 8, 12},
    {1, 8, 13},
    {1, 8, 14},
    {1, 8, 15},
    {1, 9, 8},
    {1, 9, 9},
    {1, 9, 10},
    {1, 9, 11},
    {1, 9, 12},
    {1, 9, 13},
    {1, 9, 14},
    {1, 9, 15},
    {1, 10, 8},
    {1, 10, 9},
    {1, 10, 10},
    {1, 10, 11},
    {1, 10, 12},
    {1, 10, 13},
    {1, 10, 14},
    {1, 10, 15},
    {1, 11, 8},
    {1, 11, 9},
    {1, 11, 10},
    {1, 11, 11},
    {1, 11, 12},
    {1, 11, 13},
    {1, 11, 14},
    {1, 11, 15},
    {1, 12, 8},
    {1, 12, 9},
    {1, 12, 10},
    {1, 12, 11},
    {1, 12, 12},
    {1, 12, 13},
    {1, 12, 14},
    {1, 12, 15},
    {1, 13, 8},
    {1, 13, 9},
    {1, 13, 10},
    {1, 13, 11},
    {1, 13, 12},
    {1, 13, 13},
    {1, 13, 14},
    {1, 13, 15},
    {1, 14, 8},
    {1, 14, 9},
    {1, 14, 10},
    {1, 14, 11},
    {1, 14, 12},
    {1, 14, 13},
    {1, 14, 14},
    {1, 14, 15},
    {1, 15, 8},

    /* Secondary alphabet */
    {6, 15, 9},
    {6, 15, 10},
    {6, 15, 11},
    {6, 15, 12},
    {6, 15, 13},
    {6, 15, 14},
    {6, 15, 15},
    {7, 8, 8},
    {4, 10, 12},
    {7, 8, 9},
    {7, 8, 10},
    {7, 8, 11},
    {7, 8, 12},
    {4, 10, 13},
    {7, 8, 13},
    {7, 8, 14},
    {7, 8, 15},
    {7, 9, 8},
    {7, 9, 9},
    {7, 9, 10},
    {7, 9, 11},
    {7, 9, 12},
    {7, 9, 13},
    {7, 9, 14},
    {7, 9, 15},
    {7, 10, 8},
    {7, 10, 9},
    {7, 10, 10},
    {7, 10, 11},
    {7, 10, 12},
    {7, 10, 13},
    {7, 10, 14},
    {3, 8, 8},
    {4, 15, 11},
    {5, 8, 14},
    {5, 10, 11},
    {5, 9, 10},
    {5, 9, 9},
    {5, 8, 15},
    {4, 15, 10},
    {5, 8, 12},
    {5, 8, 11},
    {5, 9, 13},
    {5, 8, 8},
    {4, 10, 11},
    {4, 15, 14},
    {4, 15, 13},
    {5, 8, 9},
    {4, 11, 15},
    {4, 12, 10},
    {4, 12, 15},
    {4, 13, 9},
    {4, 14, 8},
    {4, 13, 12},
    {4, 13, 14},
    {4, 14, 12},
    {4, 14, 11},
    {4, 14, 14},
    {5, 8, 10},
    {5, 8, 13},
    {5, 10, 8},
    {4, 15, 15},
    {5, 9, 15},
    {4, 15, 12},
    {5, 9, 8},
    {4, 11, 9},
    {4, 12, 14},
    {4, 11, 12},
    {4, 11, 14},
    {4, 11, 8},
    {4, 12, 12},
    {4, 13, 8},
    {4, 13, 10},
    {4, 11, 10},
    {4, 15, 8},
    {4, 14, 10},
    {4, 12, 11},
    {4, 12, 8},
    {4, 12, 13},
    {4, 11, 11},
    {4, 12, 9},
    {4, 14, 15},
    {4, 11, 13},
    {4, 10, 15},
    {4, 10, 14},
    {4, 13, 11},
    {4, 14, 13},
    {4, 13, 13},
    {4, 13, 15},
    {4, 14, 9},
    {4, 15, 9},
    {5, 10, 14},
    {5, 10, 9},
    {5, 10, 15},
    {5, 10, 10},
    {5, 9, 12},
    {5, 9, 11},
    {3, 8, 12},
    {4, 9, 11},
    {4, 8, 12},
    {4, 8, 11},
    {3, 8, 9},
    {4, 8, 15},
    {4, 9, 9},
    {4, 8, 10},
    {3, 8, 13},
    {4, 10, 10},
    {4, 9, 14},
    {4, 8, 9},
    {4, 8, 14},
    {3, 8, 14},
    {3, 8, 11},
    {4, 9, 8},
    {4, 10, 8},
    {3, 8, 15},
    {4, 8, 8},
    {3, 8, 10},
    {4, 8, 13},
    {4, 9, 13},
    {4, 9, 12},
    {4, 9, 15},
    {4, 9, 10},
    {4, 10, 9},
    {5, 10, 12},
    {5, 9, 14},
    {5, 10, 12},
    {5, 11, 8},
    {7, 10, 15},
    {7, 11, 8},
    {7, 11, 9},
    {7, 11, 10},
    {7, 11, 11},
    {7, 11, 12},
    {7, 11, 13},
    {7, 11, 14},
    {7, 11, 15},
    {7, 12, 8},
    {7, 12, 9},
    {7, 12, 10},
    {7, 12, 11},
    {7, 12, 12},
    {7, 12, 13},
    {7, 12, 14},
    {7, 12, 15},
    {7, 13, 8},
    {7, 13, 9},
    {7, 13, 10},
    {7, 13, 11},
    {7, 13, 12},
    {7, 13, 13},
    {7, 13, 14},
    {7, 13, 15},
    {7, 14, 8},
    {7, 14, 9},
    {7, 14, 10},
    {7, 14, 11},
    {7, 14, 12},
    {7, 14, 13},
    {7, 14, 14},
    {7, 14, 15},
    {5, 11, 9},
    {5, 11, 10},
    {5, 11, 11},
    {5, 11, 12},
    {5, 11, 13},
    {5, 11, 14},
    {5, 11, 15},
    {5, 12, 8},
    {5, 12, 9},
    {5, 12, 10},
    {5, 12, 11},
    {5, 12, 12},
    {5, 12, 13},
    {5, 12, 14},
    {5, 12, 15},
    {5, 13, 8},
    {5, 13, 9},
    {5, 13, 10},
    {5, 13, 11},
    {5, 13, 12},
    {5, 13, 13},
    {5, 13, 14},
    {5, 13, 15},
    {5, 14, 8},
    {5, 14, 9},
    {5, 14, 10},
    {5, 14, 11},
    {5, 14, 12},
    {5, 14, 13},
    {5, 14, 14},
    {5, 14, 15},
    {5, 15, 8},
    {5, 15, 9},
    {5, 15, 10},
    {5, 15, 11},
    {5, 15, 12},
    {5, 15, 13},
    {5, 15, 14},
    {5, 15, 15},
    {6, 8, 8},
    {6, 8, 9},
    {6, 8, 10},
    {6, 8, 11},
    {6, 8, 12},
    {6, 8, 13},
    {6, 8, 14},
    {6, 8, 15},
    {6, 9, 8},
    {6, 9, 9},
    {6, 9, 10},
    {6, 9, 11},
    {6, 9, 12},
    {6, 9, 13},
    {6, 9, 14},
    {6, 9, 15},
    {6, 10, 8},
    {6, 10, 9},
    {6, 10, 10},
    {6, 10, 11},
    {6, 10, 12},
    {6, 10, 13},
    {6, 10, 14},
    {6, 10, 15},
    {6, 11, 8},
    {6, 11, 9},
    {6, 11, 10},
    {6, 11, 11},
    {6, 11, 12},
    {6, 11, 13},
    {6, 11, 14},
    {6, 11, 15},
    {6, 12, 8},
    {6, 12, 9},
    {6, 12, 10},
    {6, 12, 11},
    {6, 12, 12},
    {6, 12, 13},
    {6, 12, 14},
    {6, 12, 15},
    {6, 13, 8},
    {6, 13, 9},
    {6, 13, 10},
    {6, 13, 11},
    {6, 13, 12},
    {6, 13, 13},
    {6, 13, 14},
    {6, 13, 15},
    {6, 14, 8},
    {6, 14, 9},
    {6, 14, 10},
    {6, 14, 11},
    {6, 14, 12},
    {6, 14, 13},
    {6, 14, 14},
    {6, 14, 15},
    {6, 15, 8},
};

/*
 * The same in a format more suitable for decoding.
 * The index is the varicode symbol, being 1-3 nibbles, with no padding
 * (ie a single-symbol character uses bits 3:0 only). The value is the
 * ASCII character. Since continuation nibbles are required to have the
 * MSB set, significant portions of the table are unused and should not
 * be reached.
 */
static signed int varidecode[] = {
    32,  101, 116, 111, 97,  105, 110, 114, 115, 108, 104, 100, 99,  117, 109,
    102, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  112, 103, 121, 98,  119, 118,
    107, 120, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  113, 122, 106, 44,  8,
    13,  84,  83,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  69,  65,  73,  79,
    67,  82,  68,  48,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  77,  80,  49,
    76,  70,  78,  66,  50,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  71,  51,
    72,  85,  53,  87,  54,  88,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  52,
    89,  75,  56,  55,  86,  57,  81,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    74,  90,  39,  33,  63,  46,  45,  61,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  43,  47,  58,  41,  40,  59,  34,  38,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  64,  37,  36,  96,  95,  42,  124, 62,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  60,  92,  94,  35,  123, 125, 91,  93,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  126, 160, 161, 162, 163, 164, 165, 166, -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  167, 168, 169, 170, 171, 172, 173, 174, -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  175, 176, 177, 178, 179, 180, 181, 182, -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  183, 184, 185, 186, 187, 188, 189, 190,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  191, 192, 193, 194, 195, 196, 197,
    198, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  199, 200, 201, 202, 203, 204, 205, 206, -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  207, 208, 209, 210, 211, 212, 213, 214, -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  215, 216, 217, 218, 219, 220, 221, 222, -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  223, 224, 225, 226, 227, 228, 229, 230, -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  231, 232, 233, 234, 235, 236, 237, 238, -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  239, 240, 241, 242, 243, 244, 245, 246,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  247, 248, 249, 250, 251, 252, 253,
    254, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  255, 0,   1,   2,   3,   4,
    5,   6,   -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  7,   9,   10,  11,  12,  14,  15,  16,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  17,  18,  19,  20,  21,  22,  23,  24,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  25,  26,  27,  28,  29,  30,  31,  127, -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  128, 129, 130, 131, 132, 133, 134, 135, -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  136, 137, 138, 139, 140, 141, 142, 143,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  144, 145, 146, 147, 148, 149, 150,
    151, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  152, 153, 154, 155, 156, 157,
    158, 159, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  288, 357, 372, 367, 353, 361, 366, 370, -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  371, 364, 360, 356, 355, 373, 365, 358, -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  368, 359, 377, 354, 375, 374, 363, 376, -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  369, 378, 362, 300, 264, 269, 340, 339,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  325, 321, 329, 335, 323, 338, 324,
    304, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  333, 336, 305, 332, 326, 334,
    322, 306, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  327, 307, 328, 341, 309,
    343, 310, 344, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  308, 345, 331, 312,
    311, 342, 313, 337, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  330, 346, 295,
    289, 319, 302, 301, 317, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  299, 303, 314, 297, 296, 315, 290, 294, -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  320, 293, 292, 352, 351, 298, 380, 318,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  316, 348, 350, 291, 381, -1,  347,
    349, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  382, 416, 417, 418, 419, 420,
    421, 422, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  423, 424, 425, 426, 427,
    428, 429, 430, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  431, 432, 433, 434,
    435, 436, 437, 438, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  439, 440, 441,
    442, 443, 444, 445, 446, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  447, 448,
    449, 450, 451, 452, 453, 454, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  455, 456, 457, 458, 459, 460, 461, 462,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  463, 464, 465, 466, 467, 468, 469,
    470, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  471, 472, 473, 474, 475, 476,
    477, 478, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  479, 480, 481, 482, 483,
    484, 485, 486, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  487, 488, 489, 490,
    491, 492, 493, 494, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  495, 496, 497,
    498, 499, 500, 501, 502, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  503, 504,
    505, 506, 507, 508, 509, 510, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  511,
    256, 257, 258, 259, 260, 261, 262, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  263, 265, 266, 267, 268, 270, 271,
    272, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  273, 274, 275, 276, 277, 278,
    279, 280, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  281, 282, 283, 284, 285,
    286, 287, 383, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  384, 385, 386, 387,
    388, 389, 390, 391, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  392, 393, 394,
    395, 396, 397, 398, 399, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  400, 401,
    402, 403, 404, 405, 406, 407, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  408,
    409, 410, 411, 412, 413, 414, 415, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,
};

#include <stdio.h>

unsigned char *dominoex_varienc(unsigned char c, int secondary) {
  return varicode[c + ((secondary) ? 256 : 0)];
}

int dominoex_varidec(unsigned int symbol) {
  /* The caller is responsible for ensuring symbols are properly delineated. */
  return varidecode[symbol & 0xFFF];
}
