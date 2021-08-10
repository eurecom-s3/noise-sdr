// ----------------------------------------------------------------------------
// varicode.cxx  --  PSK31 Varicode
//
// Copyright (C) 2006
//		Dave Freese, W1HKJ
//
// This file is part of fldigi.  Adapted from code contained in gmfsk source code 
// distribution.
//
// Fldigi is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with fldigi.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------


//Android #include <config.h>

#include "pskvaricode.h"

// PSK31 Varicode for encoding

static const char *varicodetab1[] = {
	"1010101011",		/*   0 - <NUL>	*/
	"1011011011",		/*   1 - <SOH>	*/
	"1011101101",		/*   2 - <STX>	*/
	"1101110111",		/*   3 - <ETX>	*/
	"1011101011",		/*   4 - <EOT>	*/
	"1101011111",		/*   5 - <ENQ>	*/
	"1011101111",		/*   6 - <ACK>	*/
	"1011111101",		/*   7 - <BEL>	*/
	"1011111111",		/*   8 - <BS>	*/
	"11101111",		/*   9 - <TAB>	*/
	"11101",		/*  10 - <LF>	*/
	"1101101111",		/*  11 - <VT>	*/
	"1011011101",		/*  12 - <FF>	*/
	"11111",		/*  13 - <CR>	*/
	"1101110101",		/*  14 - <SO>	*/
	"1110101011",		/*  15 - <SI>	*/
	"1011110111",		/*  16 - <DLE>	*/
	"1011110101",		/*  17 - <DC1>	*/
	"1110101101",		/*  18 - <DC2>	*/
	"1110101111",		/*  19 - <DC3>	*/
	"1101011011",		/*  20 - <DC4>	*/
	"1101101011",		/*  21 - <NAK>	*/
	"1101101101",		/*  22 - <SYN>	*/
	"1101010111",		/*  23 - <ETB>	*/
	"1101111011",		/*  24 - <CAN>	*/
	"1101111101",		/*  25 - <EM>	*/
	"1110110111",		/*  26 - <SUB>	*/
	"1101010101",		/*  27 - <ESC>	*/
	"1101011101",		/*  28 - <FS>	*/
	"1110111011",		/*  29 - <GS>	*/
	"1011111011",		/*  30 - <RS>	*/
	"1101111111",		/*  31 - <US>	*/
	"1",			/*  32 - <SPC>	*/
	"111111111",		/*  33 - !	*/
	"101011111",		/*  34 - '"'	*/
	"111110101",		/*  35 - #	*/
	"111011011",		/*  36 - $	*/
	"1011010101",		/*  37 - %	*/
	"1010111011",		/*  38 - &	*/
	"101111111",		/*  39 - '	*/
	"11111011",		/*  40 - (	*/
	"11110111",		/*  41 - )	*/
	"101101111",		/*  42 - *	*/
	"111011111",		/*  43 - +	*/
	"1110101",		/*  44 - ,	*/
	"110101",		/*  45 - -	*/
	"1010111",		/*  46 - .	*/
	"110101111",		/*  47 - /	*/
	"10110111",		/*  48 - 0	*/
	"10111101",		/*  49 - 1	*/
	"11101101",		/*  50 - 2	*/
	"11111111",		/*  51 - 3	*/
	"101110111",		/*  52 - 4	*/
	"101011011",		/*  53 - 5	*/
	"101101011",		/*  54 - 6	*/
	"110101101",		/*  55 - 7	*/
	"110101011",		/*  56 - 8	*/
	"110110111",		/*  57 - 9	*/
	"11110101",		/*  58 - :	*/
	"110111101",		/*  59 - ;	*/
	"111101101",		/*  60 - <	*/
	"1010101",		/*  61 - =	*/
	"111010111",		/*  62 - >	*/
	"1010101111",		/*  63 - ?	*/
	"1010111101",		/*  64 - @	*/
	"1111101",		/*  65 - A	*/
	"11101011",		/*  66 - B	*/
	"10101101",		/*  67 - C	*/
	"10110101",		/*  68 - D	*/
	"1110111",		/*  69 - E	*/
	"11011011",		/*  70 - F	*/
	"11111101",		/*  71 - G	*/
	"101010101",		/*  72 - H	*/
	"1111111",		/*  73 - I	*/
	"111111101",		/*  74 - J	*/
	"101111101",		/*  75 - K	*/
	"11010111",		/*  76 - L	*/
	"10111011",		/*  77 - M	*/
	"11011101",		/*  78 - N	*/
	"10101011",		/*  79 - O	*/
	"11010101",		/*  80 - P	*/
	"111011101",		/*  81 - Q	*/
	"10101111",		/*  82 - R	*/
	"1101111",		/*  83 - S	*/
	"1101101",		/*  84 - T	*/
	"101010111",		/*  85 - U	*/
	"110110101",		/*  86 - V	*/
	"101011101",		/*  87 - W	*/
	"101110101",		/*  88 - X	*/
	"101111011",		/*  89 - Y	*/
	"1010101101",		/*  90 - Z	*/
	"111110111",		/*  91 - [	*/
	"111101111",		/*  92 - \	*/
	"111111011",		/*  93 - ]	*/
	"1010111111",		/*  94 - ^	*/
	"101101101",		/*  95 - _	*/
	"1011011111",		/*  96 - `	*/
	"1011",			/*  97 - a	*/
	"1011111",		/*  98 - b	*/
	"101111",		/*  99 - c	*/
	"101101",		/* 100 - d	*/
	"11",			/* 101 - e	*/
	"111101",		/* 102 - f	*/
	"1011011",		/* 103 - g	*/
	"101011",		/* 104 - h	*/
	"1101",			/* 105 - i	*/
	"111101011",		/* 106 - j	*/
	"10111111",		/* 107 - k	*/
	"11011",		/* 108 - l	*/
	"111011",		/* 109 - m	*/
	"1111",			/* 110 - n	*/
	"111",			/* 111 - o	*/
	"111111",		/* 112 - p	*/
	"110111111",		/* 113 - q	*/
	"10101",		/* 114 - r	*/
	"10111",		/* 115 - s	*/
	"101",			/* 116 - t	*/
	"110111",		/* 117 - u	*/
	"1111011",		/* 118 - v	*/
	"1101011",		/* 119 - w	*/
	"11011111",		/* 120 - x	*/
	"1011101",		/* 121 - y	*/
	"111010101",		/* 122 - z	*/
	"1010110111",		/* 123 - {	*/
	"110111011",		/* 124 - |	*/
	"1010110101",		/* 125 - }	*/
	"1011010111",		/* 126 - ~	*/
	"1110110101",		/* 127 - <DEL>	*/
	"1110111101",		/* 128 - 	*/
	"1110111111",		/* 129 - 	*/
	"1111010101",		/* 130 - 	*/
	"1111010111",		/* 131 - 	*/
	"1111011011",		/* 132 - 	*/
	"1111011101",		/* 133 - 	*/
	"1111011111",		/* 134 - 	*/
	"1111101011",		/* 135 - 	*/
	"1111101101",		/* 136 - 	*/
	"1111101111",		/* 137 - 	*/
	"1111110101",		/* 138 - 	*/
	"1111110111",		/* 139 - 	*/
	"1111111011",		/* 140 - 	*/
	"1111111101",		/* 141 - 	*/
	"1111111111",		/* 142 - 	*/
	"10101010101",		/* 143 - 	*/
	"10101010111",		/* 144 - 	*/
	"10101011011",		/* 145 - 	*/
	"10101011101",		/* 146 - 	*/
	"10101011111",		/* 147 - 	*/
	"10101101011",		/* 148 - 	*/
	"10101101101",		/* 149 - 	*/
	"10101101111",		/* 150 - 	*/
	"10101110101",		/* 151 - 	*/
	"10101110111",		/* 152 - 	*/
	"10101111011",		/* 153 - 	*/
	"10101111101",		/* 154 - 	*/
	"10101111111",		/* 155 - 	*/
	"10110101011",		/* 156 - 	*/
	"10110101101",		/* 157 - 	*/
	"10110101111",		/* 158 - 	*/
	"10110110101",		/* 159 - 	*/
	"10110110111",		/* 160 - �	*/
	"10110111011",		/* 161 - �	*/
	"10110111101",		/* 162 - �	*/
	"10110111111",		/* 163 - �	*/
	"10111010101",		/* 164 - �	*/
	"10111010111",		/* 165 - �	*/
	"10111011011",		/* 166 - �	*/
	"10111011101",		/* 167 - �	*/
	"10111011111",		/* 168 - �	*/
	"10111101011",		/* 169 - �	*/
	"10111101101",		/* 170 - �	*/
	"10111101111",		/* 171 - �	*/
	"10111110101",		/* 172 - �	*/
	"10111110111",		/* 173 - �	*/
	"10111111011",		/* 174 - �	*/
	"10111111101",		/* 175 - �	*/
	"10111111111",		/* 176 - �	*/
	"11010101011",		/* 177 - �	*/
	"11010101101",		/* 178 - �	*/
	"11010101111",		/* 179 - �	*/
	"11010110101",		/* 180 - �	*/
	"11010110111",		/* 181 - �	*/
	"11010111011",		/* 182 - �	*/
	"11010111101",		/* 183 - �	*/
	"11010111111",		/* 184 - �	*/
	"11011010101",		/* 185 - �	*/
	"11011010111",		/* 186 - �	*/
	"11011011011",		/* 187 - �	*/
	"11011011101",		/* 188 - �	*/
	"11011011111",		/* 189 - �	*/
	"11011101011",		/* 190 - �	*/
	"11011101101",		/* 191 - �	*/
	"11011101111",		/* 192 - �	*/
	"11011110101",		/* 193 - �	*/
	"11011110111",		/* 194 - �	*/
	"11011111011",		/* 195 - �	*/
	"11011111101",		/* 196 - �	*/
	"11011111111",		/* 197 - �	*/
	"11101010101",		/* 198 - �	*/
	"11101010111",		/* 199 - �	*/
	"11101011011",		/* 200 - �	*/
	"11101011101",		/* 201 - �	*/
	"11101011111",		/* 202 - �	*/
	"11101101011",		/* 203 - �	*/
	"11101101101",		/* 204 - �	*/
	"11101101111",		/* 205 - �	*/
	"11101110101",		/* 206 - �	*/
	"11101110111",		/* 207 - �	*/
	"11101111011",		/* 208 - �	*/
	"11101111101",		/* 209 - �	*/
	"11101111111",		/* 210 - �	*/
	"11110101011",		/* 211 - �	*/
	"11110101101",		/* 212 - �	*/
	"11110101111",		/* 213 - �	*/
	"11110110101",		/* 214 - �	*/
	"11110110111",		/* 215 - �	*/
	"11110111011",		/* 216 - �	*/
	"11110111101",		/* 217 - �	*/
	"11110111111",		/* 218 - �	*/
	"11111010101",		/* 219 - �	*/
	"11111010111",		/* 220 - �	*/
	"11111011011",		/* 221 - �	*/
	"11111011101",		/* 222 - �	*/
	"11111011111",		/* 223 - �	*/
	"11111101011",		/* 224 - �	*/
	"11111101101",		/* 225 - �	*/
	"11111101111",		/* 226 - �	*/
	"11111110101",		/* 227 - �	*/
	"11111110111",		/* 228 - �	*/
	"11111111011",		/* 229 - �	*/
	"11111111101",		/* 230 - �	*/
	"11111111111",		/* 231 - �	*/
	"101010101011",		/* 232 - �	*/
	"101010101101",		/* 233 - �	*/
	"101010101111",		/* 234 - �	*/
	"101010110101",		/* 235 - �	*/
	"101010110111",		/* 236 - �	*/
	"101010111011",		/* 237 - �	*/
	"101010111101",		/* 238 - �	*/
	"101010111111",		/* 239 - �	*/
	"101011010101",		/* 240 - �	*/
	"101011010111",		/* 241 - �	*/
	"101011011011",		/* 242 - �	*/
	"101011011101",		/* 243 - �	*/
	"101011011111",		/* 244 - �	*/
	"101011101011",		/* 245 - �	*/
	"101011101101",		/* 246 - �	*/
	"101011101111",		/* 247 - �	*/
	"101011110101",		/* 248 - �	*/
	"101011110111",		/* 249 - �	*/
	"101011111011",		/* 250 - �	*/
	"101011111101",		/* 251 - �	*/
	"101011111111",		/* 252 - �	*/
	"101101010101",		/* 253 - �	*/
	"101101010111",		/* 254 - �	*/
	"101101011011"		/* 255 - �	*/
};

// The same in a format more suitable for decoding.

static unsigned int varicodetab2[] = {
	0x2AB, 0x2DB, 0x2ED, 0x377, 0x2EB, 0x35F, 0x2EF, 0x2FD, 
	0x2FF, 0x0EF, 0x01D, 0x36F, 0x2DD, 0x01F, 0x375, 0x3AB, 
	0x2F7, 0x2F5, 0x3AD, 0x3AF, 0x35B, 0x36B, 0x36D, 0x357, 
	0x37B, 0x37D, 0x3B7, 0x355, 0x35D, 0x3BB, 0x2FB, 0x37F, 
	0x001, 0x1FF, 0x15F, 0x1F5, 0x1DB, 0x2D5, 0x2BB, 0x17F, 
	0x0FB, 0x0F7, 0x16F, 0x1DF, 0x075, 0x035, 0x057, 0x1AF, 
	0x0B7, 0x0BD, 0x0ED, 0x0FF, 0x177, 0x15B, 0x16B, 0x1AD, 
	0x1AB, 0x1B7, 0x0F5, 0x1BD, 0x1ED, 0x055, 0x1D7, 0x2AF, 
	0x2BD, 0x07D, 0x0EB, 0x0AD, 0x0B5, 0x077, 0x0DB, 0x0FD, 
	0x155, 0x07F, 0x1FD, 0x17D, 0x0D7, 0x0BB, 0x0DD, 0x0AB, 
	0x0D5, 0x1DD, 0x0AF, 0x06F, 0x06D, 0x157, 0x1B5, 0x15D, 
	0x175, 0x17B, 0x2AD, 0x1F7, 0x1EF, 0x1FB, 0x2BF, 0x16D, 
	0x2DF, 0x00B, 0x05F, 0x02F, 0x02D, 0x003, 0x03D, 0x05B, 
	0x02B, 0x00D, 0x1EB, 0x0BF, 0x01B, 0x03B, 0x00F, 0x007, 
	0x03F, 0x1BF, 0x015, 0x017, 0x005, 0x037, 0x07B, 0x06B, 
	0x0DF, 0x05D, 0x1D5, 0x2B7, 0x1BB, 0x2B5, 0x2D7, 0x3B5, 
	0x3BD, 0x3BF, 0x3D5, 0x3D7, 0x3DB, 0x3DD, 0x3DF, 0x3EB, 
	0x3ED, 0x3EF, 0x3F5, 0x3F7, 0x3FB, 0x3FD, 0x3FF, 0x555, 
	0x557, 0x55B, 0x55D, 0x55F, 0x56B, 0x56D, 0x56F, 0x575, 
	0x577, 0x57B, 0x57D, 0x57F, 0x5AB, 0x5AD, 0x5AF, 0x5B5, 
	0x5B7, 0x5BB, 0x5BD, 0x5BF, 0x5D5, 0x5D7, 0x5DB, 0x5DD, 
	0x5DF, 0x5EB, 0x5ED, 0x5EF, 0x5F5, 0x5F7, 0x5FB, 0x5FD, 
	0x5FF, 0x6AB, 0x6AD, 0x6AF, 0x6B5, 0x6B7, 0x6BB, 0x6BD, 
	0x6BF, 0x6D5, 0x6D7, 0x6DB, 0x6DD, 0x6DF, 0x6EB, 0x6ED, 
	0x6EF, 0x6F5, 0x6F7, 0x6FB, 0x6FD, 0x6FF, 0x755, 0x757, 
	0x75B, 0x75D, 0x75F, 0x76B, 0x76D, 0x76F, 0x775, 0x777, 
	0x77B, 0x77D, 0x77F, 0x7AB, 0x7AD, 0x7AF, 0x7B5, 0x7B7, 
	0x7BB, 0x7BD, 0x7BF, 0x7D5, 0x7D7, 0x7DB, 0x7DD, 0x7DF, 
	0x7EB, 0x7ED, 0x7EF, 0x7F5, 0x7F7, 0x7FB, 0x7FD, 0x7FF, 
	0xAAB, 0xAAD, 0xAAF, 0xAB5, 0xAB7, 0xABB, 0xABD, 0xABF, 
	0xAD5, 0xAD7, 0xADB, 0xADD, 0xADF, 0xAEB, 0xAED, 0xAEF, 
	0xAF5, 0xAF7, 0xAFB, 0xAFD, 0xAFF, 0xB55, 0xB57, 0xB5B
};

const char *psk_varicode_encode(unsigned char c)
{
	return varicodetab1[c];
}

int psk_varicode_decode(unsigned int symbol)
{
	for (int i = 0; i < 256; i++)
		if (symbol == varicodetab2[i])
			return i;
	return -1;
}


