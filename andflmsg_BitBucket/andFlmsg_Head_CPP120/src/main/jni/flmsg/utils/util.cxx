// =====================================================================
//
// util.cxx
//
// Author: Stelios Buonanos, M0GLD, Dave Freese, W1HKJ
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

#include <jni.h>
#include <string.h>
#include <time.h>

//Android #include "config.h"
#include "flmsg_util.h"
#ifdef __MINGW32__
#  include "compat.h"
#endif

//Android
using namespace std;

/* Return the smallest power of 2 not less than n */
uint32_t ceil2(uint32_t n)
{
	--n;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	return n + 1;
}

/* Return the largest power of 2 not greater than n */
uint32_t floor2(uint32_t n)
{
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	return n - (n >> 1);
}

#include <stdlib.h>
unsigned long ver2int(const char* version)
{
	unsigned long v;
	char* p;

	v = (unsigned long)(strtod(version, &p) * 1e7 + 0.5);
	while (*p)
		v += *p++;

	return v;
}

#if !HAVE_STRCASESTR
#  include <ctype.h>
// from git 1.6.1.2 compat/strcasestr.c
char *strcasestr(const char *haystack, const char *needle)
{
	int nlen = strlen(needle);
	int hlen = strlen(haystack) - nlen + 1;
	int i;

	for (i = 0; i < hlen; i++) {
		int j;
		for (j = 0; j < nlen; j++) {
			unsigned char c1 = haystack[i+j];
			unsigned char c2 = needle[j];
			if (toupper(c1) != toupper(c2))
				goto next;
		}
		return (char *) haystack + i;
	next: ;
	}
	return NULL;
}
#endif // !HAVE_STRCASESTR

#if !HAVE_STRLCPY
// from git 1.6.1.2 compat/strcasestr.c
size_t strlcpy(char *dest, const char *src, size_t size)
{
	size_t ret = strlen(src);

	if (size) {
		size_t len = (ret >= size) ? size - 1 : ret;
		memcpy(dest, src, len);
		dest[len] = '\0';
	}
	return ret;
}
#endif // !HAVE_STRLCPY

#ifdef __WIN32__
int set_cloexec(int fd, unsigned char v) { return 0; }
#else
#  include <unistd.h>
#  include <fcntl.h>
int set_cloexec(int fd, unsigned char v)
{
	int f = fcntl(fd, F_GETFD);
	return f == -1 ? f : fcntl(fd, F_SETFD, (v ? f | FD_CLOEXEC : f & ~FD_CLOEXEC));
}
#endif // __WIN32__


int set_nonblock(int fd, unsigned char v)
{
#ifndef __WIN32__
	int f = fcntl(fd, F_GETFL);
	return f == -1 ? f : fcntl(fd, F_SETFL, (v ? f | O_NONBLOCK : f & ~O_NONBLOCK));
#else // __WIN32__
	u_long v_ = (u_long)v;
	errno = 0;
	if (ioctlsocket(fd, FIONBIO, &v_) == SOCKET_ERROR) {
		errno = WSAGetLastError();
		return -1;
	}
	else
		return 0;
#endif // __WIN32__
}

#ifndef __WIN32__
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <netinet/tcp.h>
#endif
int set_nodelay(int fd, unsigned char v)
{
	int val = v;
	return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&val, sizeof(val));
}


#ifdef __WIN32__
#  include <ws2tcpip.h>
#endif

int get_bufsize(int fd, int dir, int* len)
{
	socklen_t optlen = sizeof(*len);
	return getsockopt(fd, SOL_SOCKET, (dir == 0 ? SO_RCVBUF : SO_SNDBUF),
			  (char*)len, &optlen);
}
int set_bufsize(int fd, int dir, int len)
{
	return setsockopt(fd, SOL_SOCKET, (dir == 0 ? SO_RCVBUF : SO_SNDBUF),
			  (const char*)&len, sizeof(len));
}

#ifndef __WIN32__
#include <pthread.h>
#include <signal.h>
#ifndef NSIG
#  define NSIG 64
#endif
static size_t nsig = 0;
static struct sigaction* sigact = 0;
static pthread_mutex_t sigmutex = PTHREAD_MUTEX_INITIALIZER;
#endif

void save_signals(void)
{
#ifndef __WIN32__
	pthread_mutex_lock(&sigmutex);
	if (!sigact)
		sigact = new struct sigaction[NSIG];
	for (nsig = 1; nsig <= NSIG; nsig++)
		if (sigaction(nsig, NULL, &sigact[nsig-1]) == -1)
			break;
	pthread_mutex_unlock(&sigmutex);
#endif
}

void restore_signals(void)
{
#ifndef __WIN32__
	pthread_mutex_lock(&sigmutex);
	for (size_t i = 1; i <= nsig; i++)
		sigaction(i, &sigact[i-1], NULL);
	delete [] sigact;
	sigact = 0;
	nsig = 0;
	pthread_mutex_unlock(&sigmutex);
#endif
}


uint32_t simple_hash_data(const unsigned char* buf, size_t len, uint32_t code)
{
	for (size_t i = 0; i < len; i++)
		code = ((code << 4) | (code >> (32 - 4))) ^ (uint32_t)buf[i];

	return code;
}
uint32_t simple_hash_str(const unsigned char* str, uint32_t code)
{
	while (*str)
		code = ((code << 4) | (code >> (32 - 4))) ^ (uint32_t)*str++;
	return code;
}

#include <vector>
#include <climits>

static const char hexsym[] = "0123456789ABCDEF";
static std::vector<char>* hexbuf;
const char* str2hex(const unsigned char* str, size_t len)
{
//Android	if (unlikely(len == 0))
	if (len == 0)
		return "";
	if (!hexbuf) {
		hexbuf = new std::vector<char>;
		hexbuf->reserve(192);
	}
	if (hexbuf->size() < len * 3)
		hexbuf->resize(len * 3);
 
	char* p = &(*hexbuf)[0];
	size_t i;
	for (i = 0; i < len; i++) {
		*p++ = hexsym[str[i] >> 4];
		*p++ = hexsym[str[i] & 0xF];
		*p++ = ' ';
	}
	*(p - 1) = '\0';
 
	return &(*hexbuf)[0];
}

const char* str2hex(const char* str, size_t len)
{
	return str2hex((const unsigned char*)str, len ? len : strlen(str));
}
 
static std::vector<char>* binbuf;
const char* uint2bin(unsigned u, size_t len)
{
	if (len == 0)
		len = sizeof(u) * CHAR_BIT;
 
	if (!binbuf) {
		binbuf = new std::vector<char>;
		binbuf->reserve(sizeof(u) * CHAR_BIT);
	}
	if (binbuf->size() < len + 1)
		binbuf->resize(len + 1);

	for (size_t i = 0; i < len; i++) {
		(*binbuf)[len - i - 1] = '0' + (u & 1);
		u >>= 1;
	}
	(*binbuf)[len] = '\0';

	return &(*binbuf)[0];
}

void MilliSleep(long msecs)
{
#ifndef __WIN32__
	struct timespec tv;
	tv.tv_sec = msecs / 1000;
	tv.tv_nsec = (msecs - tv.tv_sec * 1000) * 1000000L;
	nanosleep(&tv, NULL);
#else
	Sleep(msecs);
#endif

}

//======================================================================

void ucase(std::string &s)
{
	for (size_t n = 0; n < s.length(); n++) s[n] = toupper(s[n]);
}

void strip_spaces(std::string &s)
{
	while (!s.empty() && s[0] == ' ') s.erase(0,1);
	while (!s.empty() && s[s.length() - 1] == ' ') s.erase(s.length() - 1, 1);
}

void strip_leading_zeros(std::string &s)
{
	while(!s.empty() && s[0] == '0') s.erase(0,1);
}

void strip_lfs(std::string &s)
{
	while (!s.empty() && s[0] == '\n') s.erase(0,1);
	while (!s.empty() && s[s.length() - 1] == '\n') s.erase(s.length() - 1, 1);
}

std::string wordwrap(std::string &s, int cnt)
{
	static std::string nustr;
	nustr.clear();
	int cntr = 1;
	char c;
	for (size_t n = 0; n < s.length(); n++) {
		c = s[n];
		if (c == '\n') {nustr += c; cntr = 1; }
		else if (c == ' ' && cntr >= cnt) {
			nustr += '\n';
			cntr = 1;
		}
		else { nustr += c; cntr++; }
	}
	return nustr;
}
