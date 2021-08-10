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

#ifndef CUSTOM_DEF
#define CUSTOM_DEF

#include <strings.h>

using std::string;

//Android #include "flmsg_dialog.h"

//Android make accessible from interface code
extern string edit_txt;
extern string html_form;
extern string html_view;
extern string custom_field;
extern string custombuffer;
//Android DEBUG
extern string debug_str;
//Android make available
extern void escape(string &s);
extern void unescape(string &s);

extern void update_customform();
extern void extract_fields();
extern void text_to_pairs();
extern void custom_viewer();
extern void assign_values(string &html);
extern void createCustomBuffer(string myHeader, int myReason);

#endif
