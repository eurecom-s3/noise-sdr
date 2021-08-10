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

#ifndef FLMSG_H
#define FLMSG_H

#include <string>
/*Android
#include <FL/Fl.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
*/
//Android
#include "arl_msgs.h"
#include "hx_msgs.h"
#include "crc16.h"
//#include "threads.h"

#define DEBUG 0

using namespace std;


extern string unwrapText;
extern string unwrapFilename;

enum MSGTYPE { NONE,
ICS203, ICS205, ICS205A, ICS206, ICS213, ICS214, ICS216, ICS309,
HICS203, HICS206, HICS213, HICS214, IARU,
RADIOGRAM, PLAINTEXT, BLANK, CSV, CUSTOM,
MARSDAILY, MARSINEEI, MARSNET, MARSARMY, MARSNAVY,
REDXSNW, REDX5739, REDX5739A, REDX5739B,
WXHC, SEVEREWX, STORMREP, CAP105, CAP110, TRANSFER, CUSTOM_TRANSFER };

struct FIELD { string f_type; string f_data; void **w; char w_type; };
extern FIELD fields[];
extern FIELD ptfields[];
extern FIELD rgfields[];

extern int selected_form;
extern void checkdirectories(void);
extern int parse_args(int argc, char **argv, int& idx);
extern void showoptions();
//Android added for interface with Java
extern bool wrap_import(string inpbuffer);


/*Android
extern Fl_Double_Window *mainwindow;
extern Fl_Double_Window *optionswindow;
extern Fl_Double_Window *arlwindow;
extern Fl_Double_Window *config_files_window;
extern Fl_Double_Window *hxwindow;
extern Fl_Double_Window *header_window;
extern Fl_Double_Window *time_estimate_window;
extern Fl_Double_Window *socket_window;
*/
extern string flmsgHomeDir;
extern string IcsHomeDir;

extern bool printme;

extern int arl_nbr;

extern string FLMSG_dir;
extern string ARQ_dir;
extern string ARQ_files_dir;
extern string ARQ_recv_dir;
extern string ARQ_send_dir;
extern string WRAP_dir;
extern string WRAP_recv_dir;
extern string WRAP_send_dir;
extern string WRAP_auto_dir;
extern string ICS_dir;
extern string ICS_msg_dir;
extern string ICS_tmp_dir;
extern string CSV_dir;
extern string CUSTOM_dir;
extern string XFR_dir;
extern string FLMSG_temp_dir;

extern string title;
extern string buffer;

extern string evalstr;

extern char *named_file();

extern pthread_t *xmlrpc_thread;
extern pthread_mutex_t mutex_xmlrpc;

//menu callbacks
extern void cb_new();
extern void cb_open();
extern void cb_save();
extern void cb_save_as();
//Android extern void cb_text();
extern string cb_text(string flmsgBuffer);
extern void cb_import();
extern void cb_export();
extern void cb_wrap_import();
extern void cb_wrap_export();
extern void cb_wrap_autosend();
extern void cb_exit();
extern void cb_load_template();
extern void cb_save_template();
extern void cb_save_as_template();
extern void cb_config_files();
extern void cb_config_date_time();
extern void cb_config_personal();
extern void cb_config_radiogram();
extern void cb_config_socket();
extern void showoptions();
extern void show_help();
extern void custom_download();
extern void cb_About();
//Android extern void cb_html();
extern string cb_html(string flmsgBuffer);
extern void cb_html_fcopy();
extern void cb_folders();

extern void open_url(const char* url);

extern void cb_arl();
extern void cb_arl_cancel();
extern void cb_arl_add();

extern void cb_hx();
extern void cb_hx_select();
extern void cb_hx_select_add();
extern void cb_hx_select_cancel();
extern void cb_hx_select_ok();

extern void closeoptions();

extern void remove_spaces_from_filename(string &fname);
extern char *szTime(int typ = 0);
extern char *szDate();
extern char *szAbbrevDate();
extern char *szDateTime();
extern char *szMarsDateTime();
extern char *szCAPDateTime();

extern void set_main_label();
extern void show_filename(string);
extern void clear_fields();
extern void update_fields();

extern void to_html(string &s);
extern void fm_html(string &html);

extern void drop_box_changed();

extern int eval_transfer_size();

// used by all form management

extern string lineout( string &, string & );
extern string binout( string &, bool & );

extern string hdr_from;
extern string hdr_edit;

extern void clear_header();
extern string header(const char *);
extern void read_header(string &str);
//Android To ensure correspondence with the Java declarations
//Android enum hdr_reason {NEW, CHANGED, FROM};
enum hdr_reason {NEW = 1, CHANGED = 2, FROM = 3};
extern void update_header(hdr_reason sender = NEW);

extern string findstr(string &, string &);
extern bool   findbin(string &, string &);
extern void   replacestr(string &form, string &where, string &what);
extern void   replacelf(string &form, int n = 0);
extern void   striplf(string &);

extern void print_and_exit();

// ics
//Android extern void ics_changed(Fl_Widget *, void *);

// ics205
extern string base_205_filename;
extern string def_205_filename;
extern string def_205_TemplateName;
extern bool using_ics205_template;

extern void cb_205_SetDateTime1();
extern void cb_205_SetDateTime2();
extern void cb_205_SetDateTime3();
extern void clear_205fields();
extern void update_205fields();
extern void clear_205_form();
extern string find(string haystack, string needle);
extern void read_205_buffer(string data);
extern void cb_205_new();
extern void cb_205_import();
extern void cb_205_export();
extern void cb_205_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_205_wrap_export();
extern void cb_205_wrap_autosend();
extern void cb_205_load_template();
extern void cb_205_save_template();
extern void cb_205_save_as_template();
extern void cb_205_open();
extern void write_205(string s);
extern bool cb_205_save_as();
extern void cb_205_save();
extern void cb_205_html();
extern void cb_205_msg_type();
extern void cb_205_textout();
extern int  eval_205_fsize();

// ics205a
extern string base_205a_filename;
extern string def_205a_filename;
extern string def_205a_TemplateName;
extern bool using_ics205a_template;

extern void cb_205a_set_date_fm();
extern void cb_205a_set_time_fm();
extern void cb_205a_set_date_to();
extern void cb_205a_set_time_to();
extern void clear_205afields();
extern void update_205afields();
extern void update_205aform();
extern void clear_205a_form();
extern void read_205a_buffer(string data);
extern void cb_205a_new();
extern void cb_205a_import();
extern void cb_205a_export();
extern void cb_205a_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_205a_wrap_export();
extern void cb_205a_wrap_autosend();
extern void cb_205a_load_template();
extern void cb_205a_save_template();
extern void cb_205a_save_as_template();
extern void cb_205a_open();
extern void write_205a(string s);
extern bool cb_205a_save_as();
extern void cb_205a_save();
extern void cb_205a_html();
extern void cb_205a_msg_type();
extern void cb_205a_textout();
extern int  eval_205a_fsize();

// ics203
extern string yes;
extern string no;

extern string buff203;
extern string def_203_filename;
extern string base_203_filename;
extern string def_203_TemplateName;
extern bool  using_203_template;

extern void cb_203_set_date();
extern void cb_203_set_time();
extern void clear_203fields();
extern void update_203fields();
extern void clear_203_form();
extern void read_203_buffer(string data);
extern void cb_203_new();
extern void cb_203_import();
extern void cb_203_export();
extern void cb_203_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_203_wrap_export();
extern void cb_203_wrap_autosend();
extern void cb_203_load_template();
extern void cb_203_save_template();
extern void cb_203_save_as_template();
extern void cb_203_open();
extern void write_203(string s);
extern bool cb_203_save_as();
extern void cb_203_save();
extern void cb_203_html();
extern void cb_203_msg_type();
extern void cb_203_textout();
extern int  eval_203_fsize();

// ics206
extern string yes;
extern string no;

extern string buff206;
extern string def_206_filename;
extern string base_206_filename;
extern string def_206_TemplateName;
extern bool using_ics206_template;

extern void cb_206_setdate();
extern void cb_206_settime();
extern void clear_206fields();
extern void update_206fields();
extern void clear_206_form();
extern void read_206_buffer(string data);
extern void cb_206_new();
extern void cb_206_import();
extern void cb_206_export();
extern void cb_206_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_206_wrap_export();
extern void cb_206_wrap_autosend();
extern void cb_206_load_template();
extern void cb_206_save_template();
extern void cb_206_save_as_template();
extern void cb_206_open();
extern void write_206(string s);
extern bool cb_206_save_as();
extern void cb_206_save();
extern void cb_206_html();
extern void cb_206_msg_type();
extern void cb_206_textout();
extern int  eval_206_fsize();

// ics213
extern bool using_213Template;
extern string base_213_filename;
extern string def_213_filename;
extern string def_213_TemplateName;

extern void clear_213_form();
extern void read_213(string);
extern void write_213(string);
extern void read_213_buffer(string);

extern void cb_213_new();
extern void cb_213_open();
extern void cb_213_save();
extern bool cb_213_save_as();
extern void cb_213_write();
extern void cb_213_html();
extern void cb_213_textout();
extern void cb_213_import();
extern void cb_213_export();
extern void cb_213_wrap_import(string, string);
extern void cb_213_wrap_export();
extern void cb_213_wrap_autosend();
extern void cb_213_load_template();
extern void cb_213_save_template();
extern void cb_213_save_as_template();
extern void cb_SetDate1();
extern void cb_SetDate2();
extern void cb_SetTime1();
extern void cb_SetTime2();
extern int  eval_213_fsize();

// ics214

extern string buff214;
extern string def_214_filename;
extern string base_214_filename;
extern string def_214_TemplateName;
extern bool using_ics214_template;

extern void cb_214_set_date();
extern void cb_214_set_time();
extern void clear_214fields();
extern void update_214fields();
extern void clear_214_form();
extern void read_214_buffer(string data);
extern void cb_214_new();
extern void cb_214_import();
extern void cb_214_export();
extern void cb_214_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_214_wrap_export();
extern void cb_214_wrap_autosend();
extern void cb_214_load_template();
extern void cb_214_save_template();
extern void cb_214_save_as_template();
extern void cb_214_open();
extern void write_214(string s);
extern bool cb_214_save_as();
extern void cb_214_save();
extern void cb_214_html();
extern void cb_214_msg_type();
extern void cb_214_textout();
extern int  eval_214_fsize();

// ics216

extern string buff216;
extern string def_216_filename;
extern string base_216_filename;
extern string def_216_TemplateName;
extern bool using_ics216_template;

extern void cb_216_set_date();
extern void cb_216_set_time();
extern void clear_216fields();
extern void update_216fields();
extern void clear_216_form();
extern void read_216_buffer(string data);
extern void cb_216_new();
extern void cb_216_import();
extern void cb_216_export();
extern void cb_216_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_216_wrap_export();
extern void cb_216_wrap_autosend();
extern void cb_216_load_template();
extern void cb_216_save_template();
extern void cb_216_save_as_template();
extern void cb_216_open();
extern void write_216(string s);
extern bool cb_216_save_as();
extern void cb_216_save();
extern void cb_216_html();
extern void cb_216_msg_type();
extern void cb_216_textout();
extern int  eval_216_fsize();

// ics309

extern string buff309;
extern string def_309_filename;
extern string base_309_filename;
extern string def_309_TemplateName;
extern bool using_ics309_template;

extern void cb_309_set_date_fm();
extern void cb_309_set_time_fm();
extern void cb_309_set_date_to();
extern void cb_309_set_time_to();
extern void cb_309_set_date_time();
extern void clear_309fields();
extern void update_309fields();
extern void clear_309_form();
extern void read_309_buffer(string data);
extern void cb_309_new();
extern void cb_309_import();
extern void cb_309_export();
extern void cb_309_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_309_wrap_export();
extern void cb_309_wrap_autosend();
extern void cb_309_load_template();
extern void cb_309_save_template();
extern void cb_309_save_as_template();
extern void cb_309_open();
extern void write_309(string s);
extern bool cb_309_save_as();
extern void cb_309_save();
extern void cb_309_html();
extern void cb_309_msg_type();
extern void cb_309_textout();
extern int  eval_309_fsize();
//Android extern void ics309_csv(Fl_Widget *w, void *d);

// radiogram
extern bool using_rg_template;
extern string base_rg_filename;
extern string def_rg_filename;
extern string def_rg_TemplateName;

extern const char hxitems[];
extern const char precitems[];
extern const char *s_prec[];
extern const char *s_hx[];

extern void cb_rgSetDate1();
extern void cb_rgSetTime1();
extern void cb_rgSetDateTime2();
extern void cb_rgSetDateTime3();
extern void cb_rgSetDateTime4();
extern void cb_rgSetDateTime5();
extern void clear_rgfields();
extern void update_rgfields();
extern void clear_rg_form();
extern void read_rg_buffer(string data);
extern void cb_rg_new();
extern void cb_rg_import();
extern void cb_rg_export();
extern void cb_rg_wrap_import(string, string);
extern void cb_rg_wrap_export();
extern void cb_rg_wrap_autosend();
extern void cb_rg_load_template();
extern void cb_rg_save_template();
extern void cb_rg_save_as_template();
extern void read_rg(string s);
extern void cb_rg_open();
extern void write_rg(string s);
extern bool cb_rg_save_as();
extern void cb_rg_save();
extern void cb_rg_html();
extern void cb_rg_html_fcopy();
extern void cb_rg_rtf();
extern void cb_rg_textout();
extern void set_rg_choices();
//Android
//extern void cb_rg_check();
extern string cb_rg_check(string msgFieldValue, bool stdFormat);
//Android extern void cb_rg_nbr(Fl_Widget *);
//Android extern void cb_rg_filter_input(Fl_Widget *);
extern int  eval_rg_fsize();

extern void read_data_file(string);

// plaintext
extern bool using_pt_template;
extern string base_pt_filename;
extern string def_pt_filename;
extern string def_pt_TemplateName;

extern void cb_set_pt_date();
extern void cb_set_pt_time();
extern void clear_ptfields();
extern void update_ptfields();
extern void clear_pt_form();
extern void read_ptbuffer(string data);
extern void cb_pt_new();
extern void cb_pt_import();
extern void cb_pt_export();
extern void cb_pt_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_pt_wrap_export();
extern void cb_pt_wrap_autosend();
extern void cb_pt_load_template();
extern void cb_pt_save_template();
extern void cb_pt_save_as_template();
extern void read_pt_data_file(string s);
extern void cb_pt_open();
extern void write_pt(string s);
extern bool cb_pt_save_as();
extern void cb_pt_save();
extern void cb_pt_html();
extern void cb_pt_msg_type();
extern void cb_pt_textout();
extern int  eval_pt_fsize();

// blank form
extern bool using_blank_template;
extern string base_blank_filename;
extern string def_blank_filename;
extern string def_blank_TemplateName;

extern string TITLE;
extern string errtext;

extern void clear_blankfields();
extern void update_blankfields();
extern void clear_blank_form();
extern void read_blankbuffer(string data);
extern void cb_blank_new();
extern void cb_blank_import();
extern void cb_blank_export();
extern void cb_blank_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_blank_wrap_export();
extern void cb_blank_wrap_autosend();
extern void cb_blank_load_template();
extern void cb_blank_save_template();
extern void cb_blank_save_as_template();
extern void read_blank_data_file(string s);
extern void cb_blank_open();
extern void write_blank(string s);
extern bool cb_blank_save_as();
extern void cb_blank_save();
extern void cb_blank_html();
extern void cb_blank_msg_type();
extern void cb_blank_textout();
extern int  eval_blank_fsize();

// csv form
extern bool using_csv_template;
extern string base_csv_filename;
extern string def_csv_filename;
extern string def_csv_TemplateName;

extern void clear_csvfields();
extern void update_csvfields();
extern void clear_csv_form();
extern void read_csvbuffer(string data);
extern void cb_csv_new();
extern void cb_csv_import();
extern void cb_csv_export();
extern void cb_csv_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_csv_wrap_export();
extern void cb_csv_wrap_autosend();
extern void cb_csv_load_template();
extern void cb_csv_save_template();
extern void cb_csv_save_as_template();
extern void read_csv_data_file(string s);
extern void cb_csv_open();
extern void write_csv(string s);
extern bool cb_csv_save_as();
extern void cb_csv_save();
extern void cb_csv_html();
extern void cb_csv_msg_type();
extern void cb_csv_textout();
extern void cb_csv_export_data(bool);
extern void cb_csv_import_data();
extern void csv_set_fname(const char *);
extern int  eval_csv_fsize();

// custom form

enum HANDLE_TYPE {HANDLE_NONE, HANDLE_EDIT, HANDLE_VIEW, HANDLE_WAITING};

extern int custom_select;
extern int handle_type;

extern int flmsg_webserver_portnbr;
extern char flmsg_webserver_szportnbr[];

extern pthread_t *web_server_thread;
extern pthread_mutex_t mutex_web_server;

struct CUSTOM_PAIRS {
	char *mnu_name;
	char *file_name;
};

extern CUSTOM_PAIRS custom_pairs[];

extern bool using_custom_template;
extern string base_custom_filename;
extern string def_custom_filename;
extern string def_custom_TemplateName;

extern void clear_customfields();
extern void update_customfields();
extern void clear_custom_form();
extern void read_custombuffer(string data);
extern void cb_custom_new();
extern void cb_custom_import();
extern void cb_custom_export();
extern void cb_custom_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_custom_wrap_export();
extern void cb_custom_wrap_autosend();
extern void cb_custom_load_template();
extern void cb_custom_save_template();
extern void cb_custom_save_as_template();
extern void read_custom_data_file(string s);
extern void cb_custom_open();
extern void write_custom(string s);
extern bool cb_custom_save_as();
extern void cb_custom_save();
extern void cb_custom_html();
extern void cb_custom_msg_type();
extern void cb_custom_textout();
extern void cb_custom_export_data(bool);
extern void cb_custom_import_data();
extern void custom_set_fname(const char *);
extern int  eval_custom_fsize();

// mars daily
extern string	def_mars_daily_filename;
extern string	def_mars_daily_TemplateName;
extern string	base_mars_daily_filename;

//Android extern void mars_changed(Fl_Widget *, void *);

extern void clear_mars_dailyfields();
extern void update_mars_dailyfields();
extern void clear_mars_daily_form();
extern void read_mars_daily_buffer(string data);
extern void cb_mars_daily_new();
extern void cb_mars_daily_import();
extern void cb_mars_daily_export();
extern void cb_mars_daily_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_mars_daily_wrap_export();
extern void cb_mars_daily_wrap_autosend();
extern void cb_mars_daily_load_template();
extern void cb_mars_daily_save_template();
extern void cb_mars_daily_save_as_template();
extern void read_mars_daily__data_file(string s);
extern void cb_mars_daily_open();
extern void write_mars_daily(string s);
extern bool cb_mars_daily_save_as();
extern void cb_mars_daily_save();
extern void cb_mars_daily_html();
extern void cb_mars_daily_msg_type();
extern void cb_mars_daily_textout();
extern int  eval_mars_daily_fsize();

// mars ineei
extern string	def_mars_ineei_filename;
extern string	def_mars_ineei_TemplateName;
extern string	base_mars_ineei_filename;

extern void cb_mars_ineei_SetDTG();
extern void clear_mars_ineei_fields();
extern void update_mars_ineei_fields();
extern void update_mars_ineeiform();
extern void clear_mars_ineei_form();
extern void read_mars_ineei_buffer(string data);
extern void cb_mars_ineei_new();
extern void cb_mars_ineei_import();
extern void cb_mars_ineei_export();
extern void cb_mars_ineei_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_mars_ineei_wrap_export();
extern void cb_mars_ineei_wrap_autosend();
extern void cb_mars_ineei_load_template();
extern void cb_mars_ineei_save_template();
extern void cb_mars_ineei_save_as_template();
extern void cb_mars_ineei_open();
extern void write_mars_ineei(string s);
extern bool cb_mars_ineei_save_as();
extern void cb_mars_ineei_save();
extern void cb_mars_ineei_html();
extern void cb_mars_ineei_textout();
extern void cb_mars_ineei_html();
extern void cb_mars_ineei_textout();
extern int  eval_mars_ineei_fsize();

// mars net
extern string	def_mars_net_filename;
extern string	def_mars_net_TemplateName;
extern string	base_mars_net_filename;

extern void cb_mars_net_SetDTG();
extern void cb_mars_net_SetDTGSTART();
extern void cb_mars_net_SetDTGEND();
extern void clear_mars_net_fields();
extern void update_mars_net_fields();
extern void update_mars_netform();
extern void clear_mars_net_form();
extern void read_mars_net_buffer(string data);
extern void cb_mars_net_new();
extern void cb_mars_net_import();
extern void cb_mars_net_export();
extern void cb_mars_net_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_mars_net_wrap_export();
extern void cb_mars_net_wrap_autosend();
extern void cb_mars_net_load_template();
extern void cb_mars_net_save_template();
extern void cb_mars_net_save_as_template();
extern void cb_mars_net_open();
extern void write_mars_net(string s);
extern bool cb_mars_net_save_as();
extern void cb_mars_net_save();
extern void cb_mars_net_html();
extern void cb_mars_net_textout();
extern int  eval_mars_net_fsize();

// mars army
extern bool using_mars_army_template;
extern string base_mars_army_filename;
extern string def_mars_army_filename;
extern string def_mars_army_TemplateName;

extern void clear_mars_armyfields();
extern void update_mars_armyfields();
extern void update_mars_armyform();
extern void clear_mars_army_form();
extern void read_mars_army_buffer(string data);
extern void cb_mars_army_new();
extern void cb_mars_army_import();
extern void cb_mars_army_export();
extern void cb_mars_army_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_mars_army_wrap_export();
extern void cb_mars_army_wrap_autosend();
extern void cb_mars_army_load_template();
extern void cb_mars_army_save_template();
extern void cb_mars_army_save_as_template();
extern void cb_mars_army_open();
extern void write_mars_army(string s);
extern bool cb_mars_army_save_as();
extern void cb_mars_army_save();
extern void cb_mars_army_html();
extern void cb_mars_army_msg_type();
extern void cb_mars_army_textout();
extern int  eval_mars_army_fsize();

// mars navy
extern bool using_mars_navy_template;
extern string base_mars_navy_filename;
extern string def_mars_navy_filename;
extern string def_mars_navy_TemplateName;

extern void clear_mars_navyfields();
extern void update_mars_navyfields();
extern void update_mars_navyform();
extern void clear_mars_navy_form();
extern void read_mars_navy_buffer(string data);
extern void cb_mars_navy_new();
extern void cb_mars_navy_import();
extern void cb_mars_navy_export();
extern void cb_mars_navy_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_mars_navy_wrap_export();
extern void cb_mars_navy_wrap_autosend();
extern void cb_mars_navy_load_template();
extern void cb_mars_navy_save_template();
extern void cb_mars_navy_save_as_template();
extern void cb_mars_navy_open();
extern void write_mars_navy(string s);
extern bool cb_mars_navy_save_as();
extern void cb_mars_navy_save();
extern void cb_mars_navy_html();
extern void cb_mars_navy_msg_type();
extern void cb_mars_navy_textout();
extern int  eval_mars_navy_fsize();

// hics203
extern string hics_buff203;
extern string def_hics203_filename;
extern string base_hics203_filename;
extern string def_hics203_TemplateName;
extern bool   using_hics203_template;
//Android extern void   hics_changed(Fl_Widget *, void *);

extern void cb_hics203_set_date();
extern void cb_hics203_set_time();
extern void clear_hics203fields();
extern void update_hics203fields();
extern void clear_hics203_form();
extern void read_hics203_buffer(string data);
extern void cb_hics203_new();
extern void cb_hics203_import();
extern void cb_hics203_export();
extern void cb_hics203_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_hics203_wrap_export();
extern void cb_hics203_wrap_autosend();
extern void cb_hics203_load_template();
extern void cb_hics203_save_template();
extern void cb_hics203_save_as_template();
extern void cb_hics203_open();
extern void write_hics203(string s);
extern bool cb_hics203_save_as();
extern void cb_hics203_save();
extern void cb_hics203_html();
extern void cb_hics203_msg_type();
extern void cb_hics203_textout();
extern int  eval_hics203_fsize();

// ics_h206
extern string h206_buff;
extern string h206_def_filename;
extern string h206_base_filename;
extern string h206_def_template_name;
extern bool   h206_using_template;

extern void h206_cb_setdate();
extern void h206_cb_settime();
extern void h206_clear_fields();
extern void h206_update_fields();
extern void h206_clear__form();
extern void h206_read_buffer(string data);
extern void h206_cb_new();
extern void h206_cb_import();
extern void h206_cb_export();
extern void h206_cb_wrap_import(string wrapfilename, string inpbuffer);
extern void h206_cb_wrap_export();
extern void h206_cb_wrap_autosend();
extern void h206_cb_load_template();
extern void h206_cb_save_template();
extern void h206_cb_save_as_template();
extern void h206_cb_open();
extern void h206_write(string s);
extern bool h206_cb_save_as();
extern void h206_cb_save();
extern void h206_cb_html();
extern void h206_cb_textout();
extern int  eval_h206_fsize();

// ics_h213

extern string h213_buffer;
extern string h213_def_filename;
extern string h213_base_filename;
extern string h213_def_template_name;
extern bool   h213_using_template;

extern void h213_cb_set_date();
extern void h213_cb_set_time0();
extern void h213_cb_set_time1();
extern void h213_cb_set_time2();
extern void h213_clear_fields();
extern void h213_update_fields();
extern void h213_clear_form();
extern void h213_update_form();
extern void h213_read_buffer(string data);
extern void h213_cb_new();
extern void h213_cb_import();
extern void h213_cb_export();
extern void h213_cb_wrap_import(string wrapfilename, string inpbuffer);
extern void h213_cb_wrap_export();
extern void h213_cb_wrap_autosend();
extern void h213_cb_load_template();
extern void h213_cb_save_template();
extern void h213_cb_save_as_template();
extern void h213_cb_open();
extern void h213_write(string s);
extern bool h213_cb_save_as();
extern void h213_cb_save();
extern void h213_cb_html();
extern void h213_cb_textout();
extern int  eval_h213_fsize();

// hics 214 variables and functions

extern string hics214_incident;
extern string hics214_date;
extern string hics214_time;
extern string hics214_op_period;
extern string hics214_sec_brch;
extern string hics214_position;
extern string hics214_activity_time[30];
extern string hics214_activity_event[30];
extern string hics214_prepared_by;
extern string hics214_facility;

extern string hics214_buff;
extern string hics214_def_filename;
extern string hics214_base_filename;
extern string hics214_template_name;
extern bool   hics214_using_template;

extern void hics214_cb_set_date();
extern void hics214_cb_set_time();
extern void hics214_clear_fields();
extern void hics214_update_fields();
extern void hics214_update_form();
extern void hics214_clear_form();
extern void hics214_read_buffer(string data);
extern void hics214_cb_new();
extern void hics214_cb_import();
extern void hics214_cb_export();
extern void hics214_cb_wrap_import(string wrapfilename, string inpbuffer);
extern void hics214_cb_wrap_export();
extern void hics214_cb_wrap_autosend();
extern void hics214_cb_load_template();
extern void hics214_cb_save_template();
extern void hics214_cb_save_as_template();
extern void hics214_cb_open();
extern void hics214_write(string s);
extern bool hics214_cb_save_as();
extern void hics214_cb_save();
extern void hics214_cb_html();
extern void hics214_cb_textout();
extern int  eval_hics214_fsize();

// IARU form

extern bool iaru_using_template;
extern int iaru_num_fields;

extern void iaru_cb_set_d1();
extern void iaru_cb_set_t1();
extern void iaru_cb_set_d2();
extern void iaru_cb_set_t2();
extern void iaru_cb_set_d3();
extern void iaru_cb_set_t3();
//Android extern void iaru_cb_nbr(Fl_Widget *wdg);
//Android extern void iaru_cb_filter_input(Fl_Widget *wdg);
extern void iaru_clear_fields();
extern void iaru_set_choices();
extern void iaru_update_fields();
extern void iaru_clear_form();
extern void iaru_update_form();
extern void iaru_read_buffer(string data);
extern void iaru_cb_new();
extern void iaru_cb_import();
extern void iaru_cb_export();
extern void iaru_cb_wrap_import(string wrapfilename, string inpbuffer);
extern void iaru_cb_wrap_export();
extern void iaru_cb_wrap_autosend();
extern void iaru_cb_load_template();
extern void iaru_cb_save_template();
extern void iaru_cb_save_as_template();
extern void iaru_cb_open();
extern void iaru_write(string s);
extern bool iaru_cb_save_as();
extern void iaru_cb_save();
extern void iaru_cb_check();
extern void iaru_cb_html();
extern void iaru_cb_textout();
extern int  eval_iaru_fsize();

extern string iaru_base_filename;
extern string iaru_def_filename;
extern string iaru_def_template_name;

// Red Cross

//Android extern void redx_changed(Fl_Widget *, void *);

// Safety & Welfare

extern bool using_redx_snw_template;
extern string base_redx_snw_filename;
extern string def_redx_snw_filename;
extern string def_redx_snw_TemplateName;

extern void clear_redx_snwfields();
extern void update_redx_snwfields();
extern void update_redx_snwform();
extern void clear_redx_snw_form();
extern void read_redx_snw_buffer(string data);
extern void cb_redx_snw_new();
extern void cb_redx_snw_import();
extern void cb_redx_snw_export();
extern void cb_redx_snw_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_redx_snw_wrap_export();
extern void cb_redx_snw_wrap_autosend();
extern void cb_redx_snw_load_template();
extern void cb_redx_snw_save_template();
extern void cb_redx_snw_save_as_template();
extern void cb_redx_snw_open();
extern void write_redx_snw(string s);
extern bool cb_redx_snw_save_as();
extern void cb_redx_snw_save();
extern void cb_redx_snw_html();
extern void cb_snw_msg_type();
extern void cb_redx_snw_textout();
extern int  eval_redx_snw_fsize();

// Form 5739

extern string base_redx_5739_filename;
extern string def_redx_5739_filename;
extern string def_redx_5739_TemplateName;

extern void clear_redx_5739fields();
extern void update_redx_5739fields();
extern void update_redx_5739form();
extern void clear_redx_5739_form();
extern void read_redx_5739_buffer(string data);
extern void cb_redx_5739_new();
extern void cb_redx_5739_import();
extern void cb_redx_5739_export();
extern void cb_redx_5739_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_redx_5739_wrap_export();
extern void cb_redx_5739_wrap_autosend();
extern void cb_redx_5739_load_template();
extern void cb_redx_5739_save_template();
extern void cb_redx_5739_save_as_template();
extern void cb_redx_5739_open();
extern void write_redx_5739(string s);
extern bool cb_redx_5739_save_as();
extern void cb_redx_5739_save();
extern void cb_redx_5739_html();
extern void cb_5739_msg_type();
extern void cb_redx_5739_textout();
extern int  eval_redx_5739_fsize();

// Form 5739A

extern string def_redx_5739A_filename;
extern string base_redx_5739A_filename;
extern string def_redx_5739A_TemplateName;

extern void clear_redx_5739Afields();
extern void update_redx_5739Afields();
extern void update_redx_5739Aform();
extern void clear_redx_5739A_form();
extern void read_redx_5739A_buffer(string data);
extern void cb_redx_5739A_new();
extern void cb_redx_5739A_import();
extern void cb_redx_5739A_export();
extern void cb_redx_5739A_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_redx_5739A_wrap_export();
extern void cb_redx_5739A_wrap_autosend();
extern void cb_redx_5739A_load_template();
extern void cb_redx_5739A_save_template();
extern void cb_redx_5739A_save_as_template();
extern void cb_redx_5739A_open();
extern void write_redx_5739A(string s);
extern bool cb_redx_5739A_save_as();
extern void cb_redx_5739A_save();
extern void cb_redx_5739A_html();
extern void cb_5739A_msg_type();
extern void cb_redx_5739A_textout();
extern int  eval_redx_5739A_fsize();

// Form 5739B

extern string def_redx_5739B_filename;
extern string base_redx_5739B_filename;
extern string def_redx_5739B_TemplateName;

extern void clear_redx_5739Bfields();
extern void update_redx_5739Bfields();
extern void update_redx_5739Bform();
extern void clear_redx_5739B_form();
extern void read_redx_5739B_buffer(string data);
extern void cb_redx_5739B_new();
extern void cb_redx_5739B_import();
extern void cb_redx_5739B_export();
extern void cb_redx_5739B_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_redx_5739B_wrap_export();
extern void cb_redx_5739B_wrap_autosend();
extern void cb_redx_5739B_load_template();
extern void cb_redx_5739B_save_template();
extern void cb_redx_5739B_save_as_template();
extern void cb_redx_5739B_open();
extern void write_redx_5739B(string s);
extern bool cb_redx_5739B_save_as();
extern void cb_redx_5739B_save();
extern void cb_redx_5739B_html();
extern void cb_5739B_msg_type();
extern void cb_redx_5739B_textout();
extern int  eval_redx_5739B_fsize();

//======================================================================
// National Hurricane Center, Hurricane Weather Report Form
//======================================================================

extern string buffwxhc;
extern string def_wxhc_filename;
extern string base_wxhc_filename;
extern string def_wxhc_TemplateName;

extern void clear_wxhcfields();
extern bool check_wxhcfields();
extern void update_wxhcfields();
extern void set_nhc_wx_combos();
extern void update_wxhcform();
extern void clear_wxhc_form();
extern void read_wxhc_buffer(string data);
extern void cb_wxhc_new();
extern void cb_wxhc_import();
extern void cb_wxhc_export();
extern void cb_wxhc_wrap_import(string wrapfilename, string inpbuffer);
extern void cb_wxhc_wrap_export();
extern void cb_wxhc_wrap_autosend();
extern void cb_wxhc_load_template();
extern void cb_wxhc_save_template();
extern void cb_wxhc_save_as_template();
extern void cb_wxhc_open();
extern void write_wxhc(string s);
extern bool cb_wxhc_save_as();
extern void cb_wxhc_save();
extern void cb_wxhc_html();
extern void cb_wxhc_textout();
extern int  eval_wxhc_fsize();

//======================================================================
// Severe Weather Report Form
//======================================================================

extern string buffsevere_wx;
extern string def_severe_wx_filename;
extern string base_severe_wx_filename;
extern string def_severe_wx_TemplateName;

extern void clear_severe_wxfields();
extern bool check_severe_wxfields();
extern void update_severe_wxfields();
extern void update_severe_wxform();
extern void clear_severe_wx_form();
extern void read_severe_wx_buffer(string data);
extern void cb_severe_wx_new();
extern void cb_severe_wx_import();
extern void cb_severe_wx_export();
extern void cb_severe_wx_wrap_import(string wrapfilename, string inpbuffer);
extern int  eval_severe_wx_fsize();
extern void cb_severe_wx_wrap_export();
extern void cb_severe_wx_wrap_autosend();
extern void cb_severe_wx_load_template();
extern void cb_severe_wx_save_template();
extern void cb_severe_wx_save_as_template();
extern void cb_severe_wx_open();
extern void write_severe_wx(string s);
extern bool cb_severe_wx_save_as();
extern void cb_severe_wx_save();
extern void cb_severe_wx_html();
extern void cb_severe_wx_textout();
extern void set_severe_wx_combos();
extern void set_severe_wx_counties(int);

//======================================================================
// Storm Report Form
//======================================================================

extern string buffstorm;
extern string def_storm_filename;
extern string base_storm_filename;
extern string def_storm_TemplateName;

extern void clear_stormfields();
extern bool check_stormfields();
extern void update_stormfields();
extern void update_stormform();
extern void clear_storm_form();
extern void read_storm_buffer(string data);
extern void cb_storm_new();
extern void cb_storm_import();
extern void cb_storm_export();
extern void cb_storm_wrap_import(string wrapfilename, string inpbuffer);
extern int  eval_storm_fsize();
extern void cb_storm_wrap_export();
extern void cb_storm_wrap_autosend();
extern void cb_storm_load_template();
extern void cb_storm_save_template();
extern void cb_storm_save_as_template();
extern void cb_storm_open();
extern void write_storm(string s);
extern bool cb_storm_save_as();
extern void cb_storm_save();
extern void cb_storm_html();
extern void cb_storm_textout();

//======================================================================
// generic file transfer
//======================================================================

extern string transfer_buffer;
extern string def_transfer_filename;
extern string base_transfer_filename;

extern void clear_transfer_form();
extern void read_transfer_buffer(string data);
extern void cb_transfer_new();
extern void cb_transfer_import();
extern void cb_transfer_export();
extern void cb_transfer_wrap_import(string wrapfilename, string inpbuffer);
extern int eval_transfer_fsize();
extern void cb_transfer_wrap_export();
extern void cb_transfer_wrap_autosend();
extern void cb_transfer_load_template();
extern void cb_transfer_save_template();
extern void cb_transfer_save_as_template();
extern void cb_transfer_open();
extern void write_transfer(string s);
extern bool cb_transfer_save_as();
extern void cb_transfer_save();
extern void cb_transfer_msg_type();
extern void cb_transfer_html();
extern void cb_transfer_textout();
extern void cb_transfer_import_data();
extern void cb_transfer_export_data();

extern string def_custom_transfer_filename;
extern void load_custom_html_file();
extern void load_custom_transfer();
extern void update_custom_transfer();

extern void cb_custom_form_wrap_import(string s1, string s2);
extern int  eval_transfer_custom_form_fsize();
extern void cb_transfer_custom_html();
//Android extern void cb_save_custom_html(Fl_Widget *w, void *d);
//Android extern void cb_btn_select_custom_html(Fl_Widget *w, void *d);

//=====================================================================
// CAP 105
//=====================================================================

extern string cap105_base_filename;
extern string cap105_def_filename;
extern string cap105_def_template_name;

extern void cap105_cb_set_dtm();
extern void cap105_cb_set_rcvd_dtm();
extern void cap105_cb_set_sent_dtm();
//Android extern void cap105_cb_nbr(Fl_Widget *wdg);
//Android extern void cap105_cb_filter_input(Fl_Widget *wdg);
extern void cap105_clear_fields();
extern void cap105_set_choices();
extern bool cap105_check_fields();
extern void cap105_update_fields();
extern void cap105_clear_form();
extern void cap105_update_form();
extern void cap105_make_buffer(bool compress = false);
extern void cap105_read_buffer(string data);
extern void cap105_cb_new();
extern void cap105_cb_import();
extern void cap105_cb_export();
extern void cap105_cb_wrap_import(string wrapfilename, string inpbuffer);
extern int  eval_cap105_fsize();
extern void cap105_cb_wrap_export();
extern void cap105_cb_wrap_autosend();
extern void cap105_cb_load_template();
extern void cap105_cb_save_template();
extern void cap105_cb_save_as_template();
extern void cap105_cb_open();
extern void cap105_write(string s);
extern bool cap105_cb_save_as();
extern void cap105_cb_save();
extern void cap105_cb_check();
extern void cap105_cb_html();
extern void cap105_cb_textout();

//=====================================================================
// CAP 105
//=====================================================================

extern string	cap110_def_filename;
extern string	cap110_base_filename;
extern string	cap110_def_template_name;

extern void cap110_set_date();
extern void clear_110fields();
extern bool check_110fields();
extern void update_110fields();
extern void update_110form();
extern void clear_110_form();
extern string &cap110_nn(string & subst, int n);
extern void read_c110_buffer(string data);
extern void cap110_cb_load_template(string data);
extern void cap110_cb_new();
extern void cap110_cb_import();
extern void cap110_cb_export();
extern void cap110_cb_wrap_import(string wrapfilename, string inpbuffer);
extern int eval_cap110_fsize();
extern void cap110_cb_wrap_export();
extern void cap110_cb_wrap_autosend();
extern void cap110_cb_load_template();
extern void cap110_cb_save_template();
extern void cap110_cb_save_as_template();
extern void cap110_cb_open();
extern void write_110(string s);
extern bool cap110_cb_save_as();
extern void cap110_cb_save();
extern void cap110_cb_html();
extern void cap110_cb_msg_type();
extern void cap110_cb_textout();

#endif
