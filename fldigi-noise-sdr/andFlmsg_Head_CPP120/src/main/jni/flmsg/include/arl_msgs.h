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

#ifndef ARL_MSGS_H
#define ARL_MSGS_H

//Android added maximum number of ARL messages for use in JAVA GUI
#define MAXARLMSGS 100

#include <string.h>

struct ARL_TEXT { const char *sznbr; int nfills; const char *text; };

//Android fixed the size (quick fix for compile issue)
//extern ARL_TEXT arl_list[];
extern ARL_TEXT arl_list[MAXARLMSGS];

std::string expand_arl(std::string &);

#endif
