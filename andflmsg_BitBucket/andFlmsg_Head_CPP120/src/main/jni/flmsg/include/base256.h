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

#include <stdio.h> 
#include <stdlib.h> 
#include <ctype.h> 

#include <string>

using namespace std;

class base256 {
#define LINELEN 64
private:
	string output;
	size_t iolen;
	size_t iocp;
	bool ateof;
	int linelength;
	void init();
	void escape(string &, bool encode = true);
	void addlf(string &);
	void remlf(string &);
public:
	base256() { init(); };
	~base256() {};
	string encode(string &in);
	string decode(string &in);
};
