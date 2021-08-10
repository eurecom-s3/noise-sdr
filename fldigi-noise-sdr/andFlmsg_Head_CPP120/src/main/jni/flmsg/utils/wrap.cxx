// WRAP - wrap.cxx
//
// Author: Dave Freese, W1HKJ
//         Stelios Bounanos, M0GLD
//
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
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

/*Android
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
*/
#include <math.h>
#include <endian.h>

//Android #include "config.h"
#include "flmsg_config.h"

#include "flmsg.h"

//Android #include "templates.h"
#include "flmsg_debug.h"
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
//Android #include "pixmaps.h"

#include "base64.h"
#include "base128.h"
#include "base256.h"
//Android#include "lzma/LzmaLib.h"
#include "LzmaLib.h"
#include "Types.h"
#include "status.h"
#include "flmsg_config.h"

//Android #include "timeops.h"

//Android #include "socket.h"

//Android
#include "AndFlmsg_Flmsg_Interface.h"

#ifdef WIN32
#  include "flmsgrc.h"
#  include "compat.h"
#  define dirent fl_dirent_no_thanks
#endif

//Android
#include "fltk_extracts.h"
/*Android no GUI here
#include <FL/filename.H>
#include "dirent-check.h"

#include <FL/x.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Image.H>

#ifdef WIN32
#  include <winsock2.h>
#else
#  include <arpa/inet.h>
#endif
*/

using namespace std;

//Android not here
//Socket *tcpip = (Socket *)0;
//Address *localaddr = (Address *)0;

const char *wrap_beg = "[WRAP:beg]";
const char *wrap_end = "[WRAP:end]";
const char *wrap_crlf = "[WRAP:crlf]";
const char *wrap_lf = "[WRAP:lf]";
const char *wrap_chksum = "[WRAP:chksum ";

const char *b64_start = "[b64:start]";
const char *b64_end = "[b64:end]";
const char *b128_start = "[b128:start]\n";
const char *b128_end = "\n[b128:end]";
const char *b256_start = "[b256:start]\n";
const char *b256_end = "\n[b256:end]";

const char *wrap_fn = "[WRAP:fn ";

const char *dashes = "\n====================\n";

//const char *binaryfile[] = {
//	".jpg", ".jpeg", ".png", ".gif", ".bmp", ".ico", ".zip", ".gz", ".tgz", ".bz2", 0 };

string test_version = "1.1.23A";
bool old_version = false;

Ccrc16 chksum;
base64 b64(1); // insert lf for ease of viewing
base128 b128;
base256 b256;

string inptext = "";
string wtext = "";
string check = "";
string wrap_outfilename = "";
string wrap_inpfilename = "";
string wrap_inpshortname = "";
string wrap_outshortname = "";
string wrap_foldername = "";

bool isExtension(const char *s1, const char *s2)
{
#ifdef WIN32
	char *sz1 = new char[strlen(s1) + 1];
	char *sz2 = new char[strlen(s2) + 1];
	char *p = 0;
	strcpy(sz1, s1);
	strcpy(sz2, s2);
	for (size_t i = 0; i < strlen(sz1); i++) sz1[i] = toupper(sz1[i]);
	for (size_t i = 0; i < strlen(sz2); i++) sz2[i] = toupper(sz2[i]);
	p = strstr(sz1, sz2);
	delete [] sz1;
	delete [] sz2;
	return (p != 0);
#else
	const char *p = strcasestr(s1, s2);
#endif
	return (p != 0);
}

void base64encode(string &inptext)
{
	string outtext;
	outtext = b64.encode(inptext);
	inptext = b64_start;
	inptext.append(outtext);
	inptext.append(b64_end);
}

void base128encode(string &inptext)
{
	string outtext;
	outtext = b128.encode(inptext);
	inptext = b128_start;
	inptext.append(outtext);
	inptext.append(b128_end);
}

void base256encode(string &inptext)
{
	string outtext = b256.encode(inptext);
	inptext = b256_start;
	inptext.append(outtext);
	inptext.append(b256_end);
}

// escape chars that cause transmission problems
// compabible with flwrap

void dress(string &str)
{
	string s = "";
	if (str.empty()) return;
	size_t p = 0;
	string rpl = "";
	unsigned char ch;
	for (p = 0; p < str.length(); p++) {
		ch = str[p] & 0xFF;
		rpl = ch;
		if (ch == ' ') rpl = "  ";
		else if ( ch == '|')
			rpl = "||";
		else if (ch >= 0x80 && ch < 0xC0) {
			rpl = "|-"; rpl += (ch - 0x60);
		}
		else if (ch >= 0xC0) {
			rpl = "|"; rpl += (ch - 0xA0);
		}
		else if (ch < ' ') {
			rpl = " "; rpl += (ch + '0');
		}
		s.append(rpl);
	}
	str = s;
}

void strip(string &s)
{
	if (s.empty()) return;
	size_t p = 0;
	unsigned char ch;
	string str = "";
	for (p = 0; p < s.length(); p++) {
		ch = s[p] & 0xFF;
		if (s.find("|+", p) == p) {
			ch = (s[p+2] & 0xFF) - 0x20;
			p += 2;
		}
		if (s.find("|-", p) == p) {
			ch = (s[p+2] & 0xFF) + 0x60;
			p += 2;
		}
		if (s.find("|_", p) == p) {
			ch = (s[p+2] & 0xFF) + 0xA0;
			p += 2;
		}
		if (s.find("||", p) == p) {
			ch = '|';
			p++;
		}
		str += ch;
	}
	s = str;
}

void convert2crlf(string &s)
{
	size_t p = s.find('\n', 0);

	while (p != string::npos) {
		s.replace(p, 1, "\r\n");
		p = s.find('\n', p + 2);
	}
}

bool convert2lf(string &s)
{
	bool converted = false;
	size_t p = s.find("\r\n", 0);

	while (p != string::npos) {
		s.replace(p, 2, "\n");
		p = s.find("\r\n", p + 1);
		converted = true;
	}
	return converted;
}

#define LZMA_STR "\1LZMA"

void compress_maybe(string& input, bool file_transfer)
{

	//Android Added access to progStatus(preferences) in the Java side
	progStatus.use_compression		= getPreferenceB2("USECOMPRESSION", true);
	progStatus.encoder				= getPreferenceI2("COMPRESSIONENCODER", 1);
	progStatus.force_compression	= getPreferenceB2("FORCECOMPRESSION", false);

	if (!progStatus.use_compression && !file_transfer) return;

	// allocate 110% of the original size for the output buffer
	size_t outlen = (size_t)ceil(input.length() * 1.1);
	unsigned char* buf = new unsigned char[outlen];

	size_t plen = LZMA_PROPS_SIZE;
	unsigned char outprops[LZMA_PROPS_SIZE];
	uint32_t origlen = htonl(input.length());

	string bufstr;

	if (progStatus.use_compression || file_transfer) {
// replace input with: LZMA_STR + original size (in network byte order) + props + data
		int r;
		bufstr.assign(LZMA_STR);
		if ((r = LzmaCompress(
					buf, &outlen,
					(const unsigned char*)input.data(), input.length(),
					outprops, &plen, 4, 0, -1, -1, -1, -1, -1) ) == SZ_OK) {
			bufstr.append((const char*)&origlen, sizeof(origlen));
			bufstr.append((const char*)&outprops, sizeof(outprops));
			bufstr.append((const char*)buf, outlen);
			if (file_transfer && input.length() < bufstr.length())
				bufstr.assign(input);
		} else {
//Android temp			//Android Temp LOG_ERROR("Lzma Compress failed: %s", LZMA_ERRORS[r]);
			bufstr.assign(input);
		}
		if (progStatus.encoder == BASE128)
			base128encode(bufstr);
		else if (progStatus.encoder == BASE256)
			base256encode(bufstr);
		else
			base64encode(bufstr);
	}

	delete [] buf;

	if (progStatus.force_compression || file_transfer || bufstr.length() < input.length()) {
		//Android temp LOG_INFO("Input size %d, Compressed size %d",
		//	(int)input.length(), (int)bufstr.length());
		input = bufstr;
	}

	return;
}

void decompress_maybe(string& input)
{
// input is LZMA_STR + original size (in network byte order) + props + data
//	if (input.find(LZMA_STR) == string::npos)
//		return;

	int encode = BASE64;
	size_t	p0 = string::npos,
			p1 = string::npos,
			p2 = string::npos,
			p3 = string::npos;
	if ((p0 = p1 = input.find(b64_start)) != string::npos) {
		p1 += strlen(b64_start);
		p2 = input.find(b64_end, p1);
	} else if ((p0 = p1 = input.find(b128_start)) != string::npos) {
		p1 += strlen(b128_start);
		p2 = input.find(b128_end, p1);
		encode = BASE128;
	} else if ((p0 = p1 = input.find(b256_start)) != string::npos) {
		p1 += strlen(b256_start);
		p2 = input.find(b256_end, p1);
		encode = BASE256;
	}

	if (p2 == string::npos) {
		if (encode == BASE256)
			//Android Temp LOG_ERROR("%s", "Base 256 decode failed");
			;
		else if (encode == BASE128)
			//Android Temp LOG_ERROR("%s", "Base 128 decode failed");
			;
		else if (encode == BASE64)
			//Android Temp LOG_ERROR("%s", "Base 64 decode failed");
		return;
	}
	switch (encode) {
		case BASE128 :
			p3 = p2 + strlen(b128_end); break;
		case BASE256 :
			p3 = p2 + strlen(b256_end); break;
		case BASE64 :
		default :
			p3 = p2 + strlen(b64_end);
	}

	string cmpstr = input.substr(p1, p2-p1);

	switch (encode) {
		case BASE128 :
			cmpstr = b128.decode(cmpstr); break;
		case BASE256 :
			cmpstr = b256.decode(cmpstr); break;
		case BASE64 :
		default:
			cmpstr = b64.decode(cmpstr);
	}

	if (cmpstr.find("ERROR") != string::npos) {
		//Android Temp LOG_ERROR("%s", cmpstr.c_str());
		return;
	}

	if (cmpstr.find(LZMA_STR) == string::npos) {
		input.replace(p0, p3 - p0, cmpstr);
		return;
	}

	const char* in = cmpstr.data();
	size_t outlen = ntohl(*reinterpret_cast<const uint32_t*>(in + strlen(LZMA_STR)));
	if (outlen > 1 << 25) {
		//Android Temp LOG_ERROR("%s", "Refusing to decompress data (> 32 MiB)");
		return;
	}
	unsigned char* buf = new unsigned char[outlen];
	unsigned char inprops[LZMA_PROPS_SIZE];
	memcpy(inprops, in + strlen(LZMA_STR) + sizeof(uint32_t), LZMA_PROPS_SIZE);
	size_t inlen = cmpstr.length() - strlen(LZMA_STR) - sizeof(uint32_t) - LZMA_PROPS_SIZE;

	int r;
	if ((r = LzmaUncompress(buf, &outlen, (const unsigned char*)in + cmpstr.length() - inlen, &inlen,
			inprops, LZMA_PROPS_SIZE)) != SZ_OK)
		//Android Temp LOG_ERROR("Lzma Uncompress failed: %s", LZMA_ERRORS[r]);
		;
	else {
		//Android temp LOG_INFO("Decompress: in = %ld, out = %ld", (long int)inlen, (long int)outlen);
		cmpstr.assign((const char*)buf, outlen);
		input.replace(p0, p3 - p0, cmpstr);
	}
	delete [] buf;
}

bool wrapfile(bool with_ext)
{
	bool iscrlf = false;
	if (with_ext)
		wrap_outfilename.append(WRAP_EXT);
	wrap_outshortname = fl_filename_name(wrap_outfilename.c_str());


// checksum & data transfer always based on Unix end-of-line char
	iscrlf = convert2lf(inptext);

	string originalfilename = wrap_fn;
	originalfilename.append(wrap_inpshortname);
	originalfilename += ']';

	inptext.insert(0, originalfilename);

	check = chksum.scrc16(inptext);

	string ostr(wrap_beg);
	ostr.append(iscrlf ? wrap_crlf : wrap_lf);
	ostr.append(inptext).append(wrap_chksum).append(check).append("]");
	ostr.append(wrap_end);

	ofstream wrapstream(wrap_outfilename.c_str(), ios::binary);
	if (wrapstream) {
		//Android temp LOG_INFO("Writing wrapfile: %s", wrap_outfilename.c_str());
		wrapstream << ostr;
		wrapstream.close();
	} else {
		//Android temp LOG_INFO("Cannot write to: %s", wrap_outfilename.c_str());
		errtext = "Cannot open output file";
		return false;
	}

	return true;
}


// all of this crlf checking is necessary because the original file may have had crlf pairs
// or the OS save may have inserted them into the text (MSDOS) har!

bool unwrapfile(string inputBuffer)
{
	size_t p, p1, p2, p3, p4;
	string testsum;
	string inpsum;
	bool iscrlf = false;

	p1 = inptext.find(wrap_beg);
	if (p1 == string::npos) {
		errtext = "Not a wrap file";
		//Android temp LOG_INFO("%s", errtext.c_str());
		return false;
	}

	p1 = inptext.find(wrap_crlf);
	if (p1 != string::npos) { // original text had crlf pairs
		iscrlf = true;
		p1 += strlen(wrap_crlf);
	} else {
		p1 = inptext.find(wrap_lf);
		if (p1 == string::npos) {
			errtext = "Invalid file";
			//Android temp LOG_INFO("%s", errtext.c_str());
			return false;
		}
		p1 += strlen(wrap_lf);
	}

	p2 = inptext.find(wrap_chksum, p1);
	if (p2 == string::npos) return false;
	wtext = inptext.substr(p1, p2-p1); // this is the original document

	p3 = p2 + strlen(wrap_chksum);
	p4 = inptext.find(']', p3);
	if (p4 == string::npos) {
		errtext = "Cannot find checksum in file";
		//Android temp LOG_INFO("%s", errtext.c_str());
		return false;
	}
	inpsum = inptext.substr(p3, p4-p3);
	p4 = inptext.find(wrap_end, p4);
	if (p4 == string::npos) {
		errtext = "No end tag in file";
		//Android temp LOG_INFO("%s", errtext.c_str());
		return false;
	}

	testsum = chksum.scrc16(wtext);

// if failure see if there are CRLF's in the file
	if (testsum != inpsum) {
		//Android Temp LOG_ERROR("%s", "Test for CRLF corrupted file!\n");

		p = inptext.find("\r\n");
		while (p != string::npos) {
			inptext.erase(p, 1);
			p = inptext.find("\r\n");
		}

		p1 = inptext.find(wrap_beg);
		if (p1 == string::npos) {
			errtext = "Not a wrap file";
			//Android temp LOG_INFO("%s", errtext.c_str());
			return false;
		}
		p1 = inptext.find(wrap_crlf);
		if (p1 != string::npos) { // original text had crlf pairs
			iscrlf = true;
			p1 += strlen(wrap_crlf);
		} else {
			p1 = inptext.find(wrap_lf);
			if (p1 == string::npos) {
				errtext = "Invalid file";
				//Android temp LOG_INFO("%s", errtext.c_str());
				return false;
			}
			p1 += strlen(wrap_lf);
		}

		p2 = inptext.find(wrap_chksum, p1);
		if (p2 == string::npos) return false;
		wtext = inptext.substr(p1, p2-p1); // this is the original document

		p3 = p2 + strlen(wrap_chksum);
		p4 = inptext.find(']', p3);
		if (p4 == string::npos) {
			errtext = "Cannot find checksum in file";
			//Android temp LOG_INFO("%s", errtext.c_str());
			return false;
		}
		inpsum = inptext.substr(p3, p4-p3);
		p4 = inptext.find(wrap_end, p4);
		if (p4 == string::npos) {
			errtext = "No end tag in file";
			//Android temp LOG_INFO("%s", errtext.c_str());
			return false;
		}

		testsum = chksum.scrc16(wtext);
	}

	if (inpsum != testsum) {
	    //For international version, remove explicit warning here, add in Java code
		//errtext = "Checksum failed\n";
		errtext = "";
		//errtext.append(inpsum);
		//errtext.append(" in file\n");
		//errtext.append(testsum);
		//errtext.append(" computed\nFile content:\n");
		errtext.append(wtext);
		//Android temp LOG_INFO("%s", errtext.c_str());
		return false;
	}

	if (wtext.find(wrap_fn) == 0) {
		size_t p = wtext.find(']');
		wrap_outshortname = wtext.substr(0, p);
		wrap_outshortname.erase(0, strlen(wrap_fn));
		wtext.erase(0,p+1);
// check for protocol abuse
		bool t1 = (wrap_outshortname.find('/') != string::npos);
		bool t2 = (wrap_outshortname.find('\\') != string::npos);
		bool t3 = (wrap_outshortname == ".");
		bool t4 = (wrap_outshortname == "..");
		bool t5 = (wrap_outshortname.find(":") != string::npos);
		bool t6 = wrap_outshortname.empty();
		if (t1 || t2 || t3 || t4 || t5 || t6) {
			errtext = "Filename corrupt, possible protocol abuse\n";
			if (t1) errtext.append("Filename contains '/' character\n");
			if (t2) errtext.append("Filename contains '\\' character\n");
			if (t3) errtext.append("Filename contains leading '.' character\n");
			if (t4) errtext.append("Filename contains leading '..' string\n");
			if (t5) errtext.append("Filename contains MS directory specifier\n");
			if (t6) errtext.append("Filename is empty\n");
			//Android temp LOG_INFO("%s", errtext.c_str());
			return false;
		}
		wrap_outfilename = wrap_foldername;
		wrap_outfilename.append(wrap_outshortname);
	} else {
		p1 = wrap_outfilename.find(WRAP_EXT);
		if (p1 != string::npos)
			wrap_outfilename.erase(p1);
	}

	if (iscrlf)
		convert2crlf(wtext);
	decompress_maybe(wtext);

	if (old_version) strip(wtext);

	return true;
}


void check_version()
{
	size_t p = inptext.find("<flmsg>");
	if (p == string::npos) return;
	p += 7;
	size_t p1 = inptext.find("\n", p);
	if (p1 == string::npos) return;
	if (inptext.substr(p, p1 - p) < test_version) old_version = true;
}

bool readfile(string inputBuffer)
{
	old_version = false;
//Android
//	char cin;
//	ifstream textfile;
//	textfile.open(wrap_inpfilename.c_str(), ios::binary);
//	if (textfile) {
		inptext.erase();
//		while (textfile.get(cin))
//			inptext += cin;
//		textfile.close();
		inptext = inputBuffer;
		check_version();
		return true;
//	}
//	return false;
}


bool import_wrapfile (	string inputBuffer,
						string &extracted_fname,
						string &extracted_text)
{
//Android	wrap_inpfilename = src_fname;
	if (readfile(inputBuffer))
		if (unwrapfile(inputBuffer)) {
			extracted_fname = wrap_outshortname;
			extracted_text = wtext;
			return true;
		}
	extracted_fname = "badfile";
	extracted_text = inptext;
	return false;
}

void connect_to_fldigi()
{
/*Android Not here

	try {
		if (!localaddr)
			localaddr = new Address(progStatus.socket_addr.c_str(), progStatus.socket_port.c_str());
		if (!tcpip) {
			tcpip = new Socket(*localaddr);
			tcpip->set_timeout(0.01);
			tcpip->connect();
			//Android temp LOG_INFO("Connected to %d", tcpip->fd());
		}
	}
	catch (const SocketException& e) {
		//Android Temp LOG_ERROR("Socket error: %d, %s", e.error(), e.what());
		delete localaddr;
		delete tcpip;
		localaddr = 0;
		tcpip = 0;
		throw;
	}
	return;
 */
}

void disconnect_from_fldigi()
{
/*Android not needed here
	if (!tcpip) return;
	tcpip->close();
	delete tcpip;
	delete localaddr;
	tcpip = 0;
	localaddr = 0;
*/
}

void xfr_via_socket(string basename, string inptext)
{
	bool iscrlf = false;

// checksum & data transfer always based on Unix end-of-line char
	iscrlf = convert2lf(inptext);

	string payload = wrap_fn;
	payload.assign(wrap_fn).append(basename).append("]");
	payload.append(inptext);

	check = chksum.scrc16(payload);

// socket interface
	string autosend;
	autosend.assign("\n\n\n... start\n").append(wrap_beg);
	autosend.append(iscrlf ? wrap_crlf : wrap_lf);
	autosend.append(payload);
	autosend.append(wrap_chksum).append(check).append("]");
	autosend.append(wrap_end).append("\n... end\n\n\n");

/*Android FIX for TX (other method to send)
	try {
		if (!tcpip) connect_to_fldigi();
		tcpip->send(autosend.c_str());
	}
	catch (const SocketException& e) {
		//Android Temp LOG_ERROR("Socket error: %d, %s", e.error(), e.what());
		if (tcpip) disconnect_from_fldigi();
		return;
	}
*/
	//Android temp LOG_INFO("Sent %s", basename.c_str());
	static char szDt[200];
	time_t tmptr;
	tm sTime;
	time (&tmptr);
	gmtime_r (&tmptr, &sTime);
	if (strftime(szDt, sizeof(szDt), "%Y-%m-%d,%H%M", &sTime) == 0) {
		//Android Temp LOG_ERROR("%s", "strftime conversion error");
		return;
	}
	string xfrstr = basename;
	xfrstr.append(",").append(szDt);
	//Android temp LOG_INFO("transfered %s", xfrstr.c_str());

	string xfrs = ICS_dir;
	xfrs.append("auto_sent.csv");
	ofstream xfr_rec_file(xfrs.c_str(), ios::app);
	if (xfr_rec_file.fail()) {
		//Android Temp LOG_ERROR("Could not open %s", xfrs.c_str());
		return;
	}
	long fsize = xfr_rec_file.tellp();
	if (fsize == 0)
		xfr_rec_file << "FILE_NAME,DATE,GMT" << "\n";
	xfr_rec_file << xfrstr << "\n";
	xfr_rec_file.close();

	return;
}

void export_wrapfile(string basename, string fname, string data_text, bool with_ext)
{
	wrap_inpshortname = basename;
	wrap_outfilename = fname;
	inptext = data_text;
	wrapfile(with_ext);
}

