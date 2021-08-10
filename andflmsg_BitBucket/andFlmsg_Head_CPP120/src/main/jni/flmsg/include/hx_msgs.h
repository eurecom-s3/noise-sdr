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

#ifndef hx_MSGS_H
#define hx_MSGS_H

//Android added maximum number of ARL messages for use in JAVA GUI
#define MAXHXMSGS 20


struct hx_TEXT { const char *sznbr; const char *text; const char *instruct;};

//Android fixed the size (quick fix for compile issue)
//extern hx_TEXT hx_list[];
extern hx_TEXT hx_list[MAXHXMSGS];

#endif
