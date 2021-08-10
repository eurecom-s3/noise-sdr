// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of flmsg
//
// flrig is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// flrig is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include "base64.h"

void base64::init()
{
	iolen = 0;
	iocp = 0;
	ateof = false;
	linelength = 0;

// create the etable for encoding
	for (int i = 0; i < 9; i++) {
		etable[i] = 'A' + i;
		etable[i + 9] = 'J' + i;
		etable[26 + i] = 'a' + i;
		etable[26 + i + 9] = 'j' + i;
	}
	for (int i = 0; i < 8; i++) {
		etable[i + 18] = 'S' + i;
		etable[26 + i + 18] = 's' + i;
	}
	for (int i = 0; i < 10; i++)
		etable[52 + i] = '0' + i;
	etable[62] = '+';
	etable[63] = '/';
	
// create the dtable for decoding
	for (int i= 0; i < 255; i++)
		dtable[i] = 0x80;
	for (int i = 'A'; i <= 'I'; i++)
		dtable[i] = 0 + (i - 'A');
	for (int i = 'J'; i <= 'R'; i++)
		dtable[i] = 9 + (i - 'J');
	for (int i = 'S'; i <= 'Z'; i++)
		dtable[i] = 18 + (i - 'S');
	for (int i = 'a'; i <= 'i'; i++)
		dtable[i] = 26 + (i - 'a');
	for (int i = 'j'; i <= 'r'; i++)
		dtable[i] = 35 + (i - 'j');
	for (int i = 's'; i <= 'z'; i++)
		dtable[i] = 44 + (i - 's');
	for (int i = '0'; i <= '9'; i++)
		dtable[i] = 52 + (i - '0');
	dtable[(int)'+'] = 62;
	dtable[(int)'/'] = 63;
	dtable[(int)'='] = 0;
}

string base64::encode(string in)
{
	int n;
	byte igroup[3], ogroup[4];
	
	output = "\n";
	iocp = 0;
	ateof = false;
	if (crlf)
		linelength = 0;
	iolen = in.length();
	
	while (!ateof) {
		igroup[0] = igroup[1] = igroup[2] = 0;
		for (n = 0; n < 3; n++) {
			if (iocp == iolen) {
				ateof = true;
				break;
			} 
			igroup[n] = (byte)in[iocp];
			iocp++;
		}
 		if (n > 0) {
			ogroup[0] = etable[igroup[0] >> 2];
			ogroup[1] = etable[((igroup[0] & 3) << 4) | (igroup[1] >> 4)];
			ogroup[2] = etable[((igroup[1] & 0xF) << 2) | (igroup[2] >> 6)];
			ogroup[3] = etable[igroup[2] & 0x3F];
			if (n < 3) ogroup[3] = '=';
			if (n < 2) ogroup[2] = '=';

			for (int i = 0; i < 4; i++) {
				if (crlf)
					if (linelength >= LINELEN) {
						output += '\n';
						linelength = 0;
					}
				output += (byte)ogroup[i];
				if (crlf)
					linelength++;
			}
		}
	}
	if (crlf)
		output += '\n';

	return output;
}

string base64::decode(string in)
{
	int i;
	output = "";
	iocp = 0;
	iolen = in.length();
	byte c;
	
	while (iocp < iolen) {
		byte a[4], b[4], o[3];

		for (i = 0; i < 4; i++) {
			if (iocp == iolen) {
				output = "b64 file length error.\n";
				return output;
			}
			c = in[iocp++];
			while (c <= ' ') {
				if (iocp == iolen) {
					return output;
				}
				c = in[iocp++];
			}
			if (dtable[c] & 0x80) {
				output = "Illegal character in b64 file.\n";
				return output;
			}
			a[i] = c;
			b[i] = (byte)dtable[c];
		}
		o[0] = (b[0] << 2) | (b[1] >> 4);
		o[1] = (b[1] << 4) | (b[2] >> 2);
		o[2] = (b[2] << 6) | b[3];
		output += o[0];
		if (a[2] != '=') {
			output += o[1];
			if (a[3] != '=')
				output += o[2];
		}
	}
	return output;
}
