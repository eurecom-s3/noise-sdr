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

#include <stdlib.h>
#include "base128.h"

//----------------------------------------------------------------------
// base 128 class
//----------------------------------------------------------------------

void base128::init()
{
	iolen = 0;
	iocp = 0;
	ateof = false;
	linelength = 0;
}

//----------------------------------------------------------------------
// fldigi uses some control codes to alter the program state
// several digital modems suppress some control and high bit set
// characters
//
// this function substitutes a two character sequence for the offending
// characters
//----------------------------------------------------------------------
void base128::escape(string &in, bool encode)
{
	string out;
	if (encode) {
		for( size_t i = 0; i < in.length(); i++) {
			switch ((in[i] & 0xFF)) {
				case ':'  : out.append("::"); break;
				case 0x00 : out.append(":0"); break;
				case 0x01 : out.append(":1"); break; // mt63
				case 0x02 : out.append(":2"); break;
				case 0x03 : out.append(":3"); break;
				case 0x04 : out.append(":4"); break;
				case 0x05 : out.append(":5"); break;
				case 0x06 : out.append(":6"); break;
				case 0x07 : out.append(":7"); break;
				case 0x08 : out.append(":8"); break;
				case 0x09 : out.append(":9"); break;
				case '\n' : out.append(":A"); break;
				case '\r' : out.append(":B"); break;
				case '^'  : out.append(":C"); break;
				case 0x7F : out.append(":D"); break;
				case 0xFF : out.append(":E"); break; // mt63
				default: out += in[i];
			}
		}
	} else {
		unsigned char ch = 0;
		for (size_t i = 0; i < in.length(); i++) {
			ch = in[i] & 0xFF;
			if (ch == ':') {
				i++;
				ch = in[i] & 0xFF;
				switch (ch) {
					case ':' : out += ':';  break;
					case '0' : out += ' ';  out[out.length() - 1] = 0x00; break;
					case '1' : out += 0x01; break;
					case '2' : out += 0x02; break;
					case '3' : out += 0x03; break;
					case '4' : out += 0x04; break;
					case '5' : out += 0x05; break;
					case '6' : out += 0x06; break;
					case '7' : out += 0x07; break;
					case '8' : out += 0x08; break;
					case '9' : out += 0x09; break;
					case 'A' : out += '\n'; break;
					case 'B' : out += '\r'; break;
					case 'C' : out += '^';  break;
					case 'D' : out += 0x7F; break;
					case 'E' : out += 0xFF; break;
				}
			} else out += ch;
		}
	}
	in = out;
}

void base128::addlf(string &in)
{
	string out;
	int len = 0;
	for (size_t n = 0; n < in.length(); n++) {
		if (len < LINELEN) {out += in[n]; len++;}
		else {out += '\n'; out += in[n]; len = 0;}
	}
	in.assign(out);
}

void base128::remlf(string &in)
{
	string out;
	for (size_t n = 0; n < in.length(); n++) {
		if (in[n] != '\n') out += in[n];
	}
	in.assign(out);
}

string base128::encode(string &in)
{
	size_t n;
	byte igroup[7], ogroup[8];
	char insize[20];
	snprintf(insize, sizeof(insize), "%d\n", in.length());

	output.assign(insize);
	iocp = 0;
	ateof = false;

	iolen = in.length();
	string temp;
	while (!ateof) {
		igroup[0] = igroup[1] = igroup[2] =
		igroup[3] = igroup[4] = igroup[5] = igroup[6] = 0;
		for (n = 0; n < 7; n++) {
			if (iocp == iolen) {
				ateof = true;
				break;
			} 
			igroup[n] = (byte)in[iocp];
			iocp++;
		}
		if (n > 0) {
			ogroup[0] =                              (igroup[0] >> 1) & 0x7F;
			ogroup[1] = ((igroup[0] << 6) & 0x40) | ((igroup[1] >> 2) & 0x3F);
			ogroup[2] = ((igroup[1] << 5) & 0x60) | ((igroup[2] >> 3) & 0x1F);
			ogroup[3] = ((igroup[2] << 4) & 0x70) | ((igroup[3] >> 4) & 0x0F);
			ogroup[4] = ((igroup[3] << 3) & 0x78) | ((igroup[4] >> 5) & 0x07);
			ogroup[5] = ((igroup[4] << 2) & 0x7C) | ((igroup[5] >> 6) & 0x03);
			ogroup[6] = ((igroup[5] << 1) & 0x7E) | ((igroup[6] >> 7) & 0x01);
			ogroup[7] =  (igroup[6]       & 0x7F);

			for (int i = 0; i < 8; i++) temp += (byte)ogroup[i];
		}
	}
	escape (temp);
	addlf(temp);
	output.append(temp);
	return output;
}

string base128::decode(string &in)
{
	int i;
	size_t nbr = 0;
	string temp = in;
	size_t p = temp.find("\n");
	if (p == string::npos)
		return "ERROR: b128 missing character count";
	sscanf(temp.substr(0, p).c_str(), "%d", &nbr);
	temp.erase(0, p+1);

	remlf(temp);
	escape(temp, false);

	output.clear();

	if (temp.length() % 8)
		return "ERROR: b128 file length error.\n";

	iocp = 0;
	iolen = temp.length();

	byte b[8], o[7];
	int k = 0;
	while (iocp < nbr) {

		for (i = 0; i < 7; i++) o[i] = 0;

		for (i = 0; i < 8; i++) b[i]  = temp[k + i];
		k += 8;

		o[0] = ((b[0] << 1) & 0xFE) | ((b[1] >> 6) & 0x01);
		o[1] = ((b[1] << 2) & 0xFC) | ((b[2] >> 5) & 0x03);
		o[2] = ((b[2] << 3) & 0xF8) | ((b[3] >> 4) & 0x07);
		o[3] = ((b[3] << 4) & 0xF0) | ((b[4] >> 3) & 0x0F);
		o[4] = ((b[4] << 5) & 0xE0) | ((b[5] >> 2) & 0x1F);
		o[5] = ((b[5] << 6) & 0xC0) | ((b[6] >> 1) & 0x3F);
		o[6] = ((b[6] << 7) & 0x80) |  (b[7]       & 0x7F);

		for (i = 0; i < 7; i++) {
			if (iocp++ < nbr)
				output += o[i];
		}
	}
	return output;
}

