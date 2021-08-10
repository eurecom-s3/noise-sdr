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
#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

//Android
#include <ctype.h>
//#include <sys/stat.h>
//#include <cstdlib>
#include "flmsg_util.h"


/*Android Not used here
#include <FL/Fl.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Sys_Menu_Bar.H>
#include <FL/x.H>
#include <FL/Fl_Help_Dialog.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_File_Icon.H>
#include "config.h"
*/
#include "flmsg_config.h"

#include "flmsg.h"
#include "templates.h"
//Android #include "debug.h"
#include "flmsg_util.h"
/*Android
#include "gettext.h"
#include "flmsg_dialog.h"
#include "flinput2.h"
#include "date.h"
#include "calendar.h"
#include "icons.h"
#include "fileselect.h"
*/
#include "wrap.h"
#include "status.h"
//Android #include "parse_xml.h"
#include "arl_msgs.h"

#ifdef WIN32
#  include "flmsgrc.h"
#  include "compat.h"
#  define dirent fl_dirent_no_thanks
#endif

/*Android
#include <FL/filename.H>
#include "dirent-check.h"

#include <FL/x.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Image.H>
*/

//Android
//#include "radiogram.h"

using namespace std;

string base_rg_filename = "";
string def_rg_filename = "";
string def_rg_TemplateName = "";

//Android
string txt_rg_msg;

// Radiogram rgfields
/*Android not used
const char *s_prec[] = {
"ROUTINE", "WELFARE", "PRIORITY", "EMERGENCY",
"TEST ROUTINE", "TEST WELFARE", "TEST PRIORITY", "TEST EMERGENCY"};
const char precitems[] = 
"ROUTINE|WELFARE|PRIORITY|EMERGENCY|\
TEST ROUTINE|TEST WELFARE|TEST PRIORITY|TEST EMERGENCY";

const char *s_hx[] = {"", "HXA", "HXB", "HXC", "HXD", "HXE", "HXF", "HXG"};
const char hxitems[] = " |HXA|HXB|HXC|HXD|HXE|HXF|HXG";
*/
// compatibility fields required to read older data files

string a_rg_nbr		= "<nbr:";		// 0.
string a_rg_prec	= "<prec:";		// 1.
string a_rg_hx		= "<hx:";		// 2.
string a_rg_d1		= "<d1:";		// 3.
string a_rg_t1		= "<t1:";		// 4.
string a_rg_dt2		= "<dt2:";		// 5.
string a_rg_dt3		= "<dt3:";		// 6.
string a_rg_to		= "<to:";		// 7.
string a_rg_phone	= "<tel:";		// 8.
string a_rg_opnote	= "<opn:";		// 9
string a_rg_msg		= "<msg:";		// 10.
string a_rg_sig		= "<sig:";		// 11.
string a_rg_opnote2	= "<op2:";		// 12
string a_rg_check	= "<ck:";		// 13.
string a_rg_station	= "<sta:";		// 14.
string a_rg_place	= "<org:";		// 15
string a_rg_orig	= "<ori:";		// 16
string a_rg_dlvd_to	= "<dlv:";		// 17
string a_rg_sent_to	= "<sto:";		// 18
string a_rg_snt_net	= "<snt:";		// 19
string a_rg_dt4		= "<dt4:";		// 20
string a_rg_rcv_fm	= "<rfm:";		// 21
string a_rg_rcv_net	= "<rnt:";		// 22
string a_rg_dt5		= "<dt5:";		// 23
string a_rg_svc		= "<svc:";		// 24

/*Android
FIELD argfields[] = {
{ a_rg_nbr,		"", (void **)&txt_rg_nbr,	't' },	// 0
{ a_rg_prec,	"0", (void **)&sel_rg_prec,	's' },	// 1
{ a_rg_hx,		"", (void **)&txt_rg_hx,		't' },	// 2
{ a_rg_d1,		"", (void **)&txt_rg_d1,		't' },	// 3
{ a_rg_t1,		"", (void **)&txt_rg_t1,		't' },	// 4
{ a_rg_dt2,		"", (void **)&txt_rg_dt2,	't' },	// 5
{ a_rg_dt3,		"", (void **)&txt_rg_dt3,	't' },	// 6
{ a_rg_to,		"", (void **)&txt_rg_to,		't' },	// 7
{ a_rg_phone,	"", (void **)&txt_rg_phone,	't' },	// 8
{ a_rg_opnote,	"", (void **)&txt_rg_opnote,	't' },	// 9
{ a_rg_msg,		"", (void **)&txt_rg_msg,	'e' },	// 10
{ a_rg_sig,		"", (void **)&txt_rg_sig,	't' },	// 11
{ a_rg_opnote2,	"", (void **)&txt_rg_opnote2,'t' },	// 12
{ a_rg_check,	"", (void **)&txt_rg_check,	't' },	// 13
{ a_rg_station,	"", (void **)&txt_rg_station,'t' },	// 14
{ a_rg_place,	"", (void **)&txt_rg_place,	't' },	// 15
{ a_rg_orig,	"", (void **)&txt_rg_orig,	't' },	// 16
{ a_rg_dlvd_to,	"", (void **)&txt_rg_dlvd_to,'t' },	// 17
{ a_rg_sent_to,	"", (void **)&txt_rg_sent_to,'t' },	// 18
{ a_rg_snt_net,	"", (void **)&txt_rg_snt_net,'t' },	// 19
{ a_rg_dt4,		"", (void **)&txt_rg_dt4,	't' },	// 20
{ a_rg_rcv_fm,	"", (void **)&txt_rg_rcv_fm,	't' },	// 21
{ a_rg_rcv_net,	"", (void **)&txt_rg_rcv_net,'t' },	// 22
{ a_rg_dt5,		"", (void **)&txt_rg_dt5,	't' },	// 23
{ a_rg_svc,		"", (void **)&btn_rg_svc,	'b' }	// 24
};

// new tag strings

string _rg_nbr		= ":nbr:";		// 0.
string _rg_prec		= ":prec:";		// 1.
string _rg_hx		= ":hx:";		// 2.
string _rg_d1		= ":d1:";		// 3.
string _rg_t1		= ":t1:";		// 4.
string _rg_dt2		= ":dt2:";		// 5.
string _rg_dt3		= ":dt3:";		// 6.
string _rg_to		= ":to:";		// 7.
string _rg_phone	= ":tel:";		// 8.
string _rg_opnote	= ":opn:";		// 9
string _rg_msg		= ":msg:";		// 10.
string _rg_sig		= ":sig:";		// 11.
string _rg_opnote2	= ":op2:";		// 12
string _rg_check	= ":ck:";		// 13.
string _rg_station	= ":sta:";		// 14.
string _rg_place	= ":org:";		// 15
string _rg_orig		= ":ori:";		// 16
string _rg_dlvd_to	= ":dlv:";		// 17
string _rg_sent_to	= ":sto:";		// 18
string _rg_snt_net	= ":snt:";		// 19
string _rg_dt4		= ":dt4:";		// 20
string _rg_rcv_fm	= ":rfm:";		// 21
string _rg_rcv_net	= ":rnt:";		// 22
string _rg_dt5		= ":dt5:";		// 23
string _rg_svc		= ":svc:";		// 24
string _rg_standard	= ":std:";		// 25

FIELD rgfields[] = {
{ _rg_nbr,		"", (void **)&txt_rg_nbr,		't' },	// 0
{ _rg_prec,		"0", (void **)&sel_rg_prec,		's' },	// 1
{ _rg_hx,		"", (void **)&txt_rg_hx,		't' },	// 2
{ _rg_d1,		"", (void **)&txt_rg_d1,		't' },	// 3
{ _rg_t1,		"", (void **)&txt_rg_t1,		't' },	// 4
{ _rg_dt2,		"", (void **)&txt_rg_dt2,		't' },	// 5
{ _rg_dt3,		"", (void **)&txt_rg_dt3,		't' },	// 6
{ _rg_to,		"", (void **)&txt_rg_to,		't' },	// 7
{ _rg_phone,	"", (void **)&txt_rg_phone,		't' },	// 8
{ _rg_opnote,	"", (void **)&txt_rg_opnote,	't' },	// 9
{ _rg_msg,		"", (void **)&txt_rg_msg,		'e' },	// 10
{ _rg_sig,		"", (void **)&txt_rg_sig,		't' },	// 11
{ _rg_opnote2,	"", (void **)&txt_rg_opnote2,	't' },	// 12
{ _rg_check,	"", (void **)&txt_rg_check,		't' },	// 13
{ _rg_station,	"", (void **)&txt_rg_station,	't' },	// 14
{ _rg_place,	"", (void **)&txt_rg_place,		't' },	// 15
{ _rg_orig,		"", (void **)&txt_rg_orig,		't' },	// 16
{ _rg_dlvd_to,	"", (void **)&txt_rg_dlvd_to,	't' },	// 17
{ _rg_sent_to,	"", (void **)&txt_rg_sent_to,	't' },	// 18
{ _rg_snt_net,	"", (void **)&txt_rg_snt_net,	't' },	// 19
{ _rg_dt4,		"", (void **)&txt_rg_dt4,		't' },	// 20
{ _rg_rcv_fm,	"", (void **)&txt_rg_rcv_fm,	't' },	// 21
{ _rg_rcv_net,	"", (void **)&txt_rg_rcv_net,	't' },	// 22
{ _rg_dt5,		"", (void **)&txt_rg_dt5,		't' },	// 23
{ _rg_svc,		"F", (void **)&btn_rg_svc,		'b' },	// 24
{ _rg_standard,	"T", (void **)&btn_rg_standard,	'B' }	// 25
};

bool using_rg_template = false;

int num_rgfields = sizeof(rgfields) / sizeof(FIELD);

int fld_nbr(string &fld)
{
	for (int i = 0; i < num_rgfields; i++)
		if (fld == rgfields[i].f_type)
			return i;
	printf("err %s\n", fld.c_str());
	exit(1);
}

//======================================================================

void cb_rgSetDate1()
{
	txt_rg_d1->value(szAbbrevDate());
}

void cb_rgSetTime1()
{
	txt_rg_t1->value(szTime(progStatus.UTC <= 1 ? 0 : 2));
}

void cb_rgSetDateTime2()
{
	txt_rg_dt2->value(szDateTime());
}

void cb_rgSetDateTime3()
{
	txt_rg_dt3->value(szDateTime());
}

void cb_rgSetDateTime4()
{
	txt_rg_dt4->value(szDateTime());
}

void cb_rgSetDateTime5()
{
	txt_rg_dt5->value(szDateTime());
}

void cb_rg_nbr(Fl_Widget *wdg)
{
	Fl_Input2 *inp = (Fl_Input2 *)wdg;
	string s = inp->value();
	for (size_t n = 0; n < s.length(); n++)
		if (!isdigit(s[n])) s.erase(n,1);
	strip_leading_zeros(s);
	inp->value(s.c_str());
}

static char valid_input[] = "0123456789/ ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";

void cb_rg_filter_input(Fl_Widget *wdg)
{
	Fl_Input2 *inp = (Fl_Input2 *)wdg;
	int p = inp->position();
	string s = inp->value();
	ucase(s);
	for (size_t n = 0; n < s.length(); n++)
		if (strchr(valid_input, s[n]) == NULL)
			s.erase(n,1);
	inp->value(s.c_str());
	inp->position(p);
}

void clear_rgfields()
{
	for (int i = 0; i < num_rgfields; i++) {
		switch (rgfields[i].w_type) {
			case 's': 
				rgfields[i].f_data = "0";
				break;
			case 'b':
				rgfields[i].f_data = "F";
				break;
			case 'B':
				rgfields[i].f_data = "T";
				break;
			default:
				rgfields[i].f_data.clear();
		}
	}
}

string numeric(int n)
{
	static char snum[10];
	snprintf(snum, sizeof(snum), "%d", n);
	return snum;
}

void set_rg_choices() {
	sel_rg_prec->clear();
	sel_rg_prec->add(precitems);
	sel_rg_prec->index(0);
}

bool check_rgfields()
{
	string temp;
	for (int i = 0; i < num_rgfields; i++) {
		if (rgfields[i].w == NULL) return false;
		if (rgfields[i].w_type == 'd') {
			if (rgfields[i].f_data != ((Fl_DateInput *)(*rgfields[i].w))->value())
				return true;
		} else if (rgfields[i].w_type == 't') {
			if (rgfields[i].f_data != ((Fl_Input2 *)(*rgfields[i].w))->value())
				return true;
		} else if (rgfields[i].w_type == 's') {
			int choice = ((Fl_ListBox *)(*rgfields[i].w))->index();
			if (rgfields[i].f_data != numeric(choice))
				return true;
		} else if (rgfields[i].w_type == 'e') {
			if (rgfields[i].f_data != ((FTextEdit *)(*rgfields[i].w))->buffer()->text())
				return true;
		} else if (rgfields[i].w_type == 'b') {
			temp = ((Fl_Button *)(*rgfields[i].w))->value() ? "T" : "F";
			if (rgfields[i].f_data != temp)
				return true;
		} else if (rgfields[i].w_type == 'B'){
			temp = ((Fl_Button *)(*rgfields[i].w))->value() ? "T" : "F";
			if (rgfields[i].f_data != temp)
				return true;
		}
	}
	return false;
}

void update_rgfields()
{
	for (int i = 0; i < num_rgfields; i++) {
		if (rgfields[i].w_type == 'd')
			rgfields[i].f_data = ((Fl_DateInput *)(*rgfields[i].w))->value();
		else if (rgfields[i].w_type == 't')
			rgfields[i].f_data = ((Fl_Input2 *)(*rgfields[i].w))->value();
		else if (rgfields[i].w_type == 's') {
			int choice = ((Fl_ListBox *)(*rgfields[i].w))->index();
			if (choice >= 0) 
				rgfields[i].f_data = numeric(choice);
		} else if (rgfields[i].w_type == 'e')
			rgfields[i].f_data = ((FTextEdit *)(*rgfields[i].w))->buffer()->text();
		else if (rgfields[i].w_type == 'b')
			rgfields[i].f_data = ((Fl_Button *)(*rgfields[i].w))->value() ? "T" : "F";
		else if (rgfields[i].w_type == 'B')
			rgfields[i].f_data = ((Fl_Button *)(*rgfields[i].w))->value() ? "T" : "F";
	}
}

void clear_rg_form()
{
	clear_rgfields();

	if (progStatus.rgnbr_fname)
		txt_rg_nbr->value(progStatus.rgnbr.c_str());
	else
		txt_rg_nbr->value("");

	for (int i = 1; i < num_rgfields; i++)
		if (rgfields[i].w_type == 'd')
			((Fl_DateInput *)(*rgfields[i].w))->value("");
		else if (rgfields[i].w_type == 't')
			((Fl_Input2 *)(*rgfields[i].w))->value("");
		else if (rgfields[i].w_type == 's')
			((Fl_ListBox *)(*rgfields[i].w))->index(0);
		else if (rgfields[i].w_type == 'e')
			((FTextEdit *)(*rgfields[i].w))->clear();
		else if (rgfields[i].w_type == 'b')
			((Fl_Button *)(*rgfields[i].w))->value(0);
		else if (rgfields[i].w_type == 'B')
			((Fl_Button *)(*rgfields[i].w))->value(1);
	update_rgfields();
}

void update_rg_form()
{
	for (int i = 0; i < num_rgfields; i++) {
		if (rgfields[i].w_type == 'd')
			((Fl_DateInput *)(*rgfields[i].w))->value(rgfields[i].f_data.c_str());
		else if (rgfields[i].w_type == 't')
			((Fl_Input2 *)(*rgfields[i].w))->value(rgfields[i].f_data.c_str());
		else if (rgfields[i].w_type == 's')
			((Fl_ListBox *)(*rgfields[i].w))->index(atoi(rgfields[i].f_data.c_str()));
		else if (rgfields[i].w_type == 'e') {
			((FTextEdit *)(*rgfields[i].w))->clear();
			((FTextEdit *)(*rgfields[i].w))->add(rgfields[i].f_data.c_str());
		} else if (rgfields[i].w_type == 'b')
			((Fl_Button *)(*rgfields[i].w))->value(rgfields[i].f_data == "T" ? 1 : 0);
		else if (rgfields[i].w_type == 'B')
			((Fl_Button *)(*rgfields[i].w))->value(rgfields[i].f_data == "T" ? 1 : 0);
	}
}

void make_rg_buffer(bool compress = false)
{
	string mbuff;
	mbuff.clear();
	for (int i = 0; i < num_rgfields; i++)
		mbuff.append( lineout( rgfields[i].f_type, rgfields[i].f_data ) );
	if (compress) compress_maybe(mbuff);
	buffer.append(mbuff);
}

void read_rg_buffer(string data)
{
	bool data_ok = false;
	clear_fields();
	read_header(data);

	string temp;
	for (int i = 0; i < num_rgfields; i++) {
		temp = findstr(data, rgfields[i].f_type);
		if (!temp.empty()) rgfields[i].f_data = temp;
		if (!rgfields[i].f_data.empty()) data_ok = true;
	}
	if (!data_ok)
		for (int i = 0; i < num_rgfields; i++)
			rgfields[i].f_data = findstr(data, argfields[i].f_type);

	update_rg_form();
}

void cb_rg_new()
{
	if (check_rgfields()) {
		if (fl_choice2("Form modified, save?", "No", "Yes", NULL) == 1) {
			update_header(CHANGED);
			cb_rg_save();
		}
	}
	clear_rg_form();
	clear_header();
	def_rg_filename = ICS_msg_dir;
	def_rg_filename.append("new"RGFILE_EXT);
	using_rg_template = false;
	show_filename(def_rg_filename);
}

void cb_rg_import()
{
	string def_rg_filename = ICS_dir;
	def_rg_filename.append("DEFAULT.XML");
	const char *p = FSEL::select(
		"Open Qforms xml file",
		"Qforms xml\t*.{xml,XML}",
		def_rg_filename.c_str());
	if (p){
		clear_rg_form();
		qform_rg_import(p);
		using_rg_template = false;
	}
}

void cb_rg_export()
{
	string exp_rgFileName = ICS_dir;
	exp_rgFileName.append(base_rg_filename);
	exp_rgFileName.append(".XML");
	const char *p = FSEL::saveas(
			"Open Qforms xml file",
			"Qforms xml\t*.{xml,XML}",
			exp_rgFileName.c_str());
	if (p) {
		const char *pext = fl_filename_ext(p);
		exp_rgFileName = p;
		if (strlen(pext) == 0) exp_rgFileName.append(".XML");
		qform_rg_export(exp_rgFileName);
	}
}

void cb_rg_wrap_import(string wrapfilename, string inpbuffer)
{
	clear_rg_form();
	read_rg_buffer(inpbuffer);
	def_rg_filename = ICS_msg_dir;
	def_rg_filename.append(wrapfilename);
	show_filename(def_rg_filename);
	using_rg_template = false;
}

int eval_rg_fsize()
{
	Ccrc16 chksum;
	evalstr.assign("[WRAP:beg][WRAP:lf][WRAP:fn ");
	evalstr.append(base_rg_filename).append("]");
	update_rgfields();
	update_header(FROM);
	evalstr.append(header("<radiogram>"));
	buffer.clear();
	make_rg_buffer(true);
	if (buffer.empty()) return 0;
	evalstr.append( buffer );
	evalstr.append("[WRAP:chksum ").append(chksum.scrc16(evalstr)).append("][WRAP:end]");
	return evalstr.length();
}

void cb_rg_wrap_export()
{
	if (btn_rg_check->labelcolor() == FL_RED)
		cb_rg_check();

	if (check_rgfields()) {
		if (fl_choice2("Form modified, save?", "No", "Yes", NULL) == 0)
			return;
		update_header(CHANGED);
	}
	update_rgfields();

	if (base_rg_filename == "new"RGFILE_EXT || base_rg_filename == "default"RGFILE_EXT)
		if (!cb_rg_save_as()) return;

	string wrapfilename = WRAP_send_dir;
	wrapfilename.append(base_rg_filename);
	wrapfilename.append(WRAP_EXT);
	const char *p = FSEL::saveas(
			"Save as wrapped radiogram file",
			"Wrap file\t*.{wrap,WRAP}",
			wrapfilename.c_str());
	if (p) {
		if (btn_rg_check->labelcolor() == FL_RED)
			cb_rg_check();
		string pext = fl_filename_ext(p);
		wrapfilename = p;

		update_header(FROM);
		buffer.assign(header("<radiogram>"));
		make_rg_buffer(true);
		export_wrapfile(base_rg_filename, wrapfilename, buffer, pext != WRAP_EXT);

		buffer.assign(header("<radiogram>"));
		make_rg_buffer(false);
		write_rg(def_rg_filename);
	}
}

void cb_rg_wrap_autosend()
{
	if (btn_rg_check->labelcolor() == FL_RED)
		cb_rg_check();

	if (check_rgfields()) {
		if (fl_choice2("Form modified, save?", "No", "Yes", NULL) == 0)
			return;
		update_header(CHANGED);
	}
	update_rgfields();

	if (base_rg_filename == "new"RGFILE_EXT || base_rg_filename == "default"RGFILE_EXT)
		if (!cb_rg_save_as()) return;

	update_header(FROM);
	buffer.assign(header("<radiogram>"));
	make_rg_buffer(true);
	xfr_via_socket(base_rg_filename, buffer);

	buffer.assign(header("<radiogram>"));
	make_rg_buffer(false);
	write_rg(def_rg_filename);
}

void cb_rg_load_template()
{
	string def_rg_filename = def_rg_TemplateName;
	const char *p = FSEL::select(
			"Open template file",
			"Template file\t*"RGTEMP_EXT,
			def_rg_filename.c_str());
	if (p) {
		clear_rg_form();
		read_data_file(p);
		def_rg_TemplateName = p;
		show_filename(def_rg_TemplateName);
		using_rg_template = true;
	}
}

void cb_rg_save_template()
{
	if (!using_rg_template) {
		cb_rg_save_as_template();
		return;
	}
	string def_rg_filename = def_rg_TemplateName;
	const char *p = FSEL::saveas(
			"Save template file",
			"Template file\t*"RGTEMP_EXT,
			def_rg_filename.c_str());
	if (p) {
		update_header(CHANGED);
		update_rgfields();
		buffer.assign(header("<radiogram>"));
		make_rg_buffer();
		write_rg(p);
	}
}

void cb_rg_save_as_template()
{
	string def_rg_filename = def_rg_TemplateName;
	const char *p = FSEL::saveas(
			"Save as template file",
			"Template file\t*"RGTEMP_EXT,
			def_rg_filename.c_str());
	if (p) {
		const char *pext = fl_filename_ext(p);
		def_rg_TemplateName = p;
		if (strlen(pext) == 0) def_rg_TemplateName.append(RGTEMP_EXT);
		remove_spaces_from_filename(def_rg_TemplateName);

		clear_header();
		update_header(CHANGED);
		buffer.assign(header("<radiogram>"));
		make_rg_buffer();
		write_rg(def_rg_TemplateName);

		show_filename(def_rg_TemplateName);
		using_rg_template = true;
	}
}

void cb_rg_open()
{
	const char *p = FSEL::select(_("Open data file"), "radiogram\t*"RGFILE_EXT,
					def_rg_filename.c_str());
	if (!p) return;
	if (strlen(p) == 0) return;
	clear_rg_form();
	read_data_file(p);
	using_rg_template = false;
	def_rg_filename = p;
	show_filename(def_rg_filename);
}

void write_rg(string s)
{
	FILE *rgfile = fopen(s.c_str(), "w");
	if (!rgfile) return;

	fwrite(buffer.c_str(), buffer.length(), 1, rgfile);
	fclose(rgfile);
}

bool cb_rg_save_as()
{
	const char *p;
	string newfilename;
	string name = named_file();

	if (!name.empty()) {
		name.append(RGFILE_EXT);
		newfilename = ICS_msg_dir;
		newfilename.append(name);
	} else
		newfilename = def_rg_filename;
	p = FSEL::saveas(_("Save data file"), "radiogram\t*"RGFILE_EXT,
						newfilename.c_str());

	if (!p) return false;
	if (strlen(p) == 0) return false;

	if (progStatus.rgnbr_fname) {
		int n = atoi(progStatus.rgnbr.c_str());
		n++;
		char szn[10];
		snprintf(szn, sizeof(szn), "%d", n);
		progStatus.rgnbr = szn;
		txt_rgnbr->value(szn);
		txt_rgnbr->redraw();
	} else if (progStatus.sernbr_fname) {
		int n = atoi(progStatus.sernbr.c_str());
		n++;
		char szn[10];
		snprintf(szn, sizeof(szn), "%d", n);
		progStatus.sernbr = szn;
		txt_sernbr->value(szn);
		txt_sernbr->redraw();
	}
	const char *pext = fl_filename_ext(p);
	def_rg_filename = p;
	if (strlen(pext) == 0) def_rg_filename.append(RGFILE_EXT);

	remove_spaces_from_filename(def_rg_filename);

	update_header(NEW);
	update_rgfields();
	buffer.assign(header("<radiogram>"));
	make_rg_buffer();
	write_rg(def_rg_filename);

	using_rg_template = false;
	show_filename(def_rg_filename);
	return true;
}

void cb_rg_save()
{
	if (base_rg_filename == "new"RGFILE_EXT || 
		base_rg_filename == "default"RGFILE_EXT ||
		using_rg_template == true) {
		cb_rg_save_as();
		return;
	}

	if (check_rgfields()) update_header(CHANGED);
	update_rgfields();
	buffer.assign(header("<radiogram>"));
	make_rg_buffer();
	write_rg(def_rg_filename);
	using_rg_template = false;
}

//End Android section
*/

const char *punctuation[] = {
		". ", " X ",
		",", " COMMA ",
		"?", " QUERY ",
		"\\", " BACKSLASH ",
		"://", " COLON SLASH SLASH ",
		"~", " TILDE ",
		"_", " UNDERSCORE ",
		"@", " AT ",
		"#", " POUNDSIGN ",
		"\"", " QUOTE ",
		"\'", "",
		0, 0 };



//Android void cb_rg_check()
string cb_rg_check(string msgFieldValue, bool stdFormat)
{
	//Android	string temp = txt_rg_msg->buffer()->text();
	string temp = msgFieldValue;
	/*Android not used
	if (temp.empty()) {
		txt_rg_check->value("");
		btn_rg_check->labelcolor(FL_BLACK);
		btn_rg_check->redraw_label();
		return;
	}
	 */
	size_t pos = string::npos;

	//Android	if (btn_rg_standard->value()) {
	if (stdFormat) {
		// convert to uppercase
		for (size_t n = 0; n < temp.length(); n++)
			temp[n] = toupper(temp[n]);

		strip_lfs(temp);
		// remove trailing period
		if (temp[temp.length()-1] == '.') temp.erase(temp.length()-1,1);
		// convert punctuation
		for (int n = 0; punctuation[n]; n += 2)
			while ((pos = temp.find(punctuation[n])) != string::npos)
				temp.replace(pos, strlen(punctuation[n]), punctuation[n+1]);
		//convert embedded periods
		while ((pos = temp.find(".")) != string::npos)
			if (isdigit(temp[pos-1]) || isdigit(temp[pos+1]))
				temp[pos] = 'R';
			else
				temp.replace(pos, 1, " DOT ");
	}

	// remove any user inserted end-of-lines
	while ((pos = temp.find('\n')) != string::npos) temp[pos] = ' ';

	// only single spaces no trailing spaces, no leading spaces
	while ((pos = temp.find("  ")) != string::npos) temp.erase(pos,1);
	while (temp[temp.length() -1] == ' ') temp.erase(temp.length()-1, 1);
	if (temp[0] == ' ') temp.erase(0,1);

	// count number of words in textdef_rg_filename
	int numwords = 1;
	if (temp.length()) {
		pos = 0;
		while ((pos = temp.find(" ", pos + 1)) != string::npos) numwords++;
	}

	// no more than specified # words to a line
	if (numwords > progStatus.wpl) {
		int wc = numwords;
		size_t pos = 0;
		while (wc > progStatus.wpl) {
			for (int i = 0; i < progStatus.wpl; i++) pos = temp.find(' ', pos + 1);
			temp[pos] = '\n';
			wc -= progStatus.wpl;
		}
	}
	// insert trailing end-of-line
	temp += '\n';

	// return converted text to editor
	//Android	txt_rg_msg->clear();
	//Android	txt_rg_msg->addstr(temp.c_str());
	txt_rg_msg = temp;

	char snum[10];
	snprintf(snum, sizeof(snum), "%s%d", 
			temp.find("ARL") != string::npos ? "ARL " : "",
					numwords);
	/*Android
	txt_rg_check->value(snum);
	update_rgfields();
	btn_rg_check->labelcolor(FL_BLACK);
	btn_rg_check->redraw_label();
	 */
	string temp2;
	temp2.assign(snum);
	//Android
	return temp2;
}

/*Android not used
void cb_rg_html()
{
	string rgname;
	string html_text;
	size_t nbr;
	rgname = ICS_dir;
	rgname.append("radiogram.html");

	update_rgfields();
	cb_rg_check();
	string form = rg_html_template;

	for (int i = 0; i < num_rgfields; i++) {
		if (rgfields[i].f_type == _rg_prec) {
			sscanf(rgfields[i].f_data.c_str(), "%d", &nbr);
			if (nbr >= 0 && nbr < (sizeof(s_prec) / sizeof(*s_prec)))
				html_text = s_prec[nbr];
			else
				html_text = s_prec[0];
			replacestr( form, rgfields[i].f_type, html_text );
		} else if (rgfields[i].w_type == 'b') {
			replacestr( form, rgfields[i].f_type, rgfields[i].f_data == "T" ? yes : no);
		} else
			replacestr( form, rgfields[i].f_type, rgfields[i].f_data );
	}

	string rxstr = "";
	rxstr.append(progStatus.my_call).append(" ").append(progStatus.my_tel);
	rxstr.append("\n").append(progStatus.my_name);
	rxstr.append("\n").append(progStatus.my_addr);
	rxstr.append("\n").append(progStatus.my_city);
	html_text = ":rx:";
	replacestr( form, html_text, rxstr);

	html_text = ":exp:";
	string arlmsgs = "";
	if (progStatus.arl_desc)
		arlmsgs = expand_arl(rgfields[10].f_data);
	replacestr( form, html_text, arlmsgs);

	FILE *rgfile = fopen(rgname.c_str(), "w");
	fprintf(rgfile,"%s", form.c_str());
	fclose(rgfile);

	open_url(rgname.c_str());
}

void cb_rg_html_fcopy()
{
	string rgname;
	string MSG = "";
	string html_text;
	size_t nbr;
	rgname = ICS_dir;
	rgname.append("rg_file_copy.html");

	update_rgfields();
	cb_rg_check();
	string form = rg_html_fcopy_template;

	for (int i = 0; i < num_rgfields; i++) {
		if (rgfields[i].f_type == _rg_prec) {
			sscanf(rgfields[i].f_data.c_str(), "%d", &nbr);
			if (nbr >= 0 && nbr < (sizeof(s_prec) / sizeof(*s_prec)))
				html_text = s_prec[nbr];
			else
				html_text = s_prec[0];
			replacestr( form, rgfields[i].f_type, html_text);
		} else if (rgfields[i].w_type == 'b') {
			replacestr( form, rgfields[i].f_type, rgfields[i].f_data == "T" ? yes : no);
		} else
			replacestr( form, rgfields[i].f_type, rgfields[i].f_data );
	}

	string rxstr = "";
	rxstr.append(progStatus.my_call).append(" ").append(progStatus.my_tel);
	rxstr.append("\n").append(progStatus.my_name);
	rxstr.append("\n").append(progStatus.my_addr);
	rxstr.append("\n").append(progStatus.my_city);
	html_text = ":rx:";
	replacestr( form, html_text, rxstr);

	FILE *rgfile = fopen(rgname.c_str(), "w");
	fprintf(rgfile,"%s", form.c_str());
	fclose(rgfile);

	open_url(rgname.c_str());
}

void cb_rg_textout()
{
	string rgname;
	string lines;
	string str;
	size_t nbr = 0;
	rgname = ICS_dir;
	rgname.append("radiogram.txt");

	update_rgfields();
	cb_rg_check();

	string form = rg_txt_template;

	for (int i = 0; i < num_rgfields; i++) {
		str.clear();
		if (rgfields[i].f_type == _rg_prec) {
			sscanf(rgfields[i].f_data.c_str(), "%d", &nbr);
			if (nbr < 0) nbr = 0;
			if (nbr >= sizeof(s_prec)/sizeof(*s_prec)) nbr = 0;
			str = s_prec[nbr];
			if (str.find("TEST") != string::npos) {				// test message
				if (str.find("EMERGENCY") == string::npos)
					str = str.substr(0, 6);
			} else {
				if (str.find("EMERGENCY") == string::npos)
					str = str[0];
			}
			replacestr( form, rgfields[i].f_type, str);
		} else if (rgfields[i].w_type == 'e' || rgfields[i].w_type == 't') {
			if (rgfields[i].f_type == _rg_opnote || rgfields[i].f_type == _rg_opnote2) {
			    if (!rgfields[i].f_data.empty())
					str.append(" OPNOTE ").append(rgfields[i].f_data);
			} else if (rgfields[i].f_type == _rg_hx && !rgfields[i].f_data.empty()) {
				str = " ";
				str.append(rgfields[i].f_data);
			} else {
				str = rgfields[i].f_data;
				strip_lfs(str);
			}
			replacestr( form, rgfields[i].f_type, str );
		} else
			replacestr( form, rgfields[i].f_type, rgfields[i].f_data );
	}

	FILE *rgfile = fopen(rgname.c_str(), "w");
	fprintf(rgfile,"%s", form.c_str());
	fclose(rgfile);
	open_url(rgname.c_str());
}
*/

