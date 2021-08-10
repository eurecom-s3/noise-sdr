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

#include <iostream>
#include <fstream>
#include <string>

//Android #include <FL/Fl_Preferences.H>

#include "status.h"
//Android #include "config.h"
#include "flmsg.h"
//Android #include "flmsg_dialog.h"
#include "wrap.h"

status progStatus = {
	50,				// int mainX;
	50,				// int mainY;

	5,				// int wpl;
	false,			// bool open_on_export;
	0,				// int  UTC
// 0: HHMM,     1: HH:MM
// 2: HHMMZ,    3: HH:MMZ
// 4: HHMM UTC, 5: HH:MM UTC
	0,				// date format 0: YYYY-MM-DD, 1: MM/DD/YY, 2: DD/MM/YY
	"",				// my_call
	"",				// my_tel
	"",				// my_name
	"",				// my_addr
	"",				// my_city
	"",				// my_email
	true,			// bool sernbr_fname;
	true,			// bool call_fname;
	false,			// bool dt_fname;
	true,			// bool rgrnbr_fname;
	true,			// bool arl_desc;
	"1",			// string sernbr;
	"1",			// string rgnbr
	false,			// bool insert_x;
	RADIOGRAM,		// int tab;
	"",
	72,				// charcount
	true,			// autowordwrap
	"127.0.0.1",	// fldigi socket address
	"7322",			// fldigi socket port
	"127.0.0.1",	// fldigi xmlrpc socket address
	"7362",			// fldigi xmlrpc socket port
	false,			// use_compression
	BASE64,			// encoder
	0,				// selected_mode
	0,				// index default_state
	0,				// index default_county
	"",				// swx_default_city;
	"",				// swx_default_location;
	"",				// swx_default_zone;
	"",				// swx_default_profile;
	true,			// change_modem_with_autosend
	false,			// sync_modem_to_fldigi
	false,			// force_compression
	false			// caplocal
};

void status::saveLastState()
{
/*Android Nothing for now

 	Fl_Preferences flmsgpref(FLMSG_dir.c_str(), "w1hkj.com",  PACKAGE_NAME);

	int mX = mainwindow->x();
	int mY = mainwindow->y();
	if (mX >= 0 && mX >= 0) {
		mainX = mX;
		mainY = mY;
	}
	selected_mode = cbo_modes->index();

	flmsgpref.set("version", PACKAGE_VERSION);
	flmsgpref.set("mainx", mX);
	flmsgpref.set("mainy", mY);

	flmsgpref.set("wpl", wpl);
	flmsgpref.set("open_on_export", open_on_export);
	flmsgpref.set("utc", UTC);
	flmsgpref.set("dtformat", dtformat);
	flmsgpref.set("mycall", my_call.c_str());
	flmsgpref.set("mytel", my_tel.c_str());
	flmsgpref.set("myname", my_name.c_str());
	flmsgpref.set("myaddr", my_addr.c_str());
	flmsgpref.set("mycity", my_city.c_str());
	flmsgpref.set("myemail", my_email.c_str());
	flmsgpref.set("sernbr", sernbr.c_str());
	flmsgpref.set("rgnbr", rgnbr.c_str());
	flmsgpref.set("sernbr_fname", sernbr_fname);
	flmsgpref.set("rgnbr_fname", rgnbr_fname);
	flmsgpref.set("arl_desc", arl_desc);
	flmsgpref.set("call_fname", call_fname);
	flmsgpref.set("dt_fname", dt_fname);

	flmsgpref.set("insert_x", insert_x);

	flmsgpref.set("charcount", charcount);
	flmsgpref.set("autowordwrap", autowordwrap);

	tab = selected_form;
	flmsgpref.set("preset_tab", tab);

	flmsgpref.set("mars_roster_file", mars_roster_file.c_str());

	flmsgpref.set("socket_address", socket_addr.c_str());
	flmsgpref.set("socket_port", socket_port.c_str());

	flmsgpref.set("use_compression", use_compression);
	flmsgpref.set("force_compression", force_compression);
	flmsgpref.set("encoder", encoder);
	flmsgpref.set("selected_mode", selected_mode);

	flmsgpref.set("swx_index_default_state", swx_index_default_state);
	flmsgpref.set("swx_index_default_county", swx_index_default_county);
	flmsgpref.set("swx_default_city", swx_default_city.c_str());
	flmsgpref.set("swx_default_location", swx_default_location.c_str());
	flmsgpref.set("swx_default_zone", swx_default_zone.c_str());
	flmsgpref.set("swx_default_profile", swx_default_profile.c_str());

	flmsgpref.set("change_modem_with_autosend", change_modem_with_autosend);
	flmsgpref.set("sync_modem_to_fldigi", sync_modem_to_fldigi);

	flmsgpref.set("caplocal", caplocal);
*/
}

void status::loadLastState()
{
/*Android Nothing for now

	Fl_Preferences flmsgpref(FLMSG_dir.c_str(), "w1hkj.com", PACKAGE_NAME);

	if (flmsgpref.entryExists("version")) {
		int i = 0;
		char *defbuffer;

		flmsgpref.get("mainx", mainX, mainX);
		flmsgpref.get("mainy", mainY, mainY);

		flmsgpref.get("wpl", wpl, wpl);
		if (flmsgpref.get("open_on_export", i, i)) open_on_export = i;

		flmsgpref.get("utc", UTC, UTC);

		flmsgpref.get("dtformat", dtformat, dtformat);

		flmsgpref.get("mycall", defbuffer, "");
		my_call = defbuffer; free(defbuffer);

		flmsgpref.get("mytel", defbuffer, "");
		my_tel = defbuffer; free(defbuffer);

		flmsgpref.get("myname", defbuffer, "");
		my_name = defbuffer; free(defbuffer);

		flmsgpref.get("myaddr", defbuffer, "");
		my_addr = defbuffer; free(defbuffer);

		flmsgpref.get("mycity", defbuffer, "");
		my_city = defbuffer; free(defbuffer);

		flmsgpref.get("myemail", defbuffer, "");
		my_email = defbuffer; free(defbuffer);

		if (flmsgpref.get("sernbr", defbuffer, ""))
			sernbr = defbuffer;
		free(defbuffer);

		if (flmsgpref.get("rgnbr", defbuffer, ""))
			rgnbr = defbuffer;
		free(defbuffer);

		if (flmsgpref.get("sernbr_fname", i, i)) sernbr_fname = i;
		if (flmsgpref.get("arl_desc", i, i)) arl_desc = i;
		if (flmsgpref.get("rgnbr_fname", i, i)) rgnbr_fname = i;
		if (flmsgpref.get("call_fname", i, i)) call_fname = i;
		if (flmsgpref.get("dt_fname", i, i)) dt_fname = i;

		if (flmsgpref.get("insert_x", i, i)) insert_x = i;

		flmsgpref.get("charcount", charcount, charcount);
		if (flmsgpref.get("autowordwrap", i, autowordwrap)) autowordwrap = i;

		flmsgpref.get("preset_tab", tab, tab);

		flmsgpref.get("mars_roster_file", defbuffer, "");
		mars_roster_file = defbuffer; free(defbuffer);

		if (mars_roster_file.empty()) {
			mars_roster_file = ICS_dir;
			mars_roster_file.append("MARS_ROSTER.csv");
		}

		flmsgpref.get("socket_address", defbuffer, socket_addr.c_str());
		socket_addr = defbuffer; free(defbuffer);

		flmsgpref.get("socket_port", defbuffer, socket_port.c_str());
		socket_port = defbuffer; free(defbuffer);

		if (flmsgpref.get("use_compression", i, use_compression)) use_compression = i;
		if (flmsgpref.get("force_compression", i, force_compression)) force_compression = i;

		flmsgpref.get("encoder", encoder, encoder);

		flmsgpref.get("selected_mode", selected_mode, selected_mode);

		flmsgpref.get("swx_index_default_state", swx_index_default_state, swx_index_default_state);
		flmsgpref.get("swx_index_default_county", swx_index_default_county, swx_index_default_county);

		flmsgpref.get("swx_default_city", defbuffer, swx_default_city.c_str());
		swx_default_city = defbuffer; free(defbuffer);

		flmsgpref.get("swx_default_location", defbuffer, swx_default_location.c_str());
		swx_default_location = defbuffer; free(defbuffer);

		flmsgpref.get("swx_default_zone", defbuffer, swx_default_zone.c_str());
		swx_default_zone = defbuffer; free(defbuffer);

		flmsgpref.get("swx_default_profile", defbuffer, swx_default_profile.c_str());
		swx_default_profile = defbuffer; free(defbuffer);

		if (flmsgpref.get("change_modem_with_autosend", i, change_modem_with_autosend))
			change_modem_with_autosend = i;

		if (flmsgpref.get("sync_modem_to_fldigi", i, sync_modem_to_fldigi))
			sync_modem_to_fldigi = i;

		if (flmsgpref.get("caplocal", i, caplocal))
			caplocal = i;
	}
*/
}

