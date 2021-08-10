// ----------------------------------------------------------------------------
// configuration.cxx
//
// Copyright (C) 2006-2010
//		Dave Freese, W1HKJ
// Copyright (C) 2007-2008
//		Leigh L. Klotz, Jr., WA5ZNU
// Copyright (C) 2007-2010
//		Stelios Bounanos, M0GLD
// Copyright (C) 2013
//		Remi Chateauneu, F4ECW
//
// This file is part of fldigi.
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

//Android#include <config.h>

#include "configuration.h"
//#include "confdialog.h"
//#include "xmlreader.h"
//#include "soundconf.h"
//#include "fl_digi.h"
//#include "main.h"
//#include "gettext.h"
//#include "nls.h"
//#include "icons.h"

#if USE_HAMLIB
	#include "hamlib.h"
	#include "rigclass.h"
#endif

//#include "rigio.h"
//#include "rigxml.h"
//#include "debug.h"

//#include <FL/Fl_Tooltip.H>

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>

#ifdef __linux__
#  include <dirent.h>
#  include <limits.h>
#  include <errno.h>
//#  include <glob.h>
#endif
#ifdef __APPLE__
#  include <glob.h>
#endif
#ifndef __CYGWIN__
#  include <sys/stat.h>
#else
#  include <fcntl.h>
#endif

// this tests depends on a modified FL/filename.H in the Fltk-1.3.0
// change
//#  if defined(WIN32) && !defined(__CYGWIN__) && !defined(__WATCOMC__)
// to
//#  if defined(WIN32) && !defined(__CYGWIN__) && !defined(__WATCOMC__) && !defined(__WOE32__)

#include <dirent.h>

using namespace std;


// By redefining the ELEM_ macro, we can control what the CONFIG_LIST macro
// will expand to, and accomplish several things:
// 1) Declare "struct configuration". See ELEM_DECLARE_CONFIGURATION
//    in configuration.h.
// 2) Define progdefaults, the configuration struct that is initialised with
//    fldigi's default options
#define ELEM_PROGDEFAULTS(type_, var_, tag_, doc_, ...) __VA_ARGS__,
// 3) Define an array of tag element pointers
//Android #define ELEM_TAG_ARRAY(type_, var_, tag_, doc_, ...)                                             \
//        (*tag_ ? new tag_elem<type_>(tag_, "type: " #type_ "; default: " #__VA_ARGS__ "\n" doc_, \
//                                     progdefaults.var_) : 0),

// First define the default config
#undef ELEM_
#define ELEM_ ELEM_PROGDEFAULTS
configuration progdefaults = { CONFIG_LIST };


