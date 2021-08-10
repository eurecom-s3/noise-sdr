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

#ifndef WRAP_H
#define WRAP_H

#include <string.h>
#include <string>

using namespace std;

enum {BASE64 = 1, BASE128, BASE256};
//Making sure it matches the Java declarations
//enum {BASE64, BASE128, BASE256};

extern void compress_maybe(string& input, bool file_transfer = false);
extern void decompress_maybe(string& input);

extern void export_wrapfile(string, string, string, bool with_ext);
//Android slight change
//extern bool import_wrapfile(string fname, string &efname, string &text);
extern bool import_wrapfile(string inputBuffer, string &efname, string &text);

extern void xfr_via_socket(string, string);

extern string wrap_outfilename;

extern void connect_to_fldigi();
extern void disconnect_from_fldigi();

#endif
