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

#include <stdio.h>

//Android #include "config.h"
#include "ext_string.h"

#if !HAVE_STRCASESTR
#  include <ctype.h>
// from git 1.6.1.2 compat/strcasestr.c

//Android duplication fldigi/flmsg code. Renamed this one.
//static char *strcasestr(const char *haystack, const char *needle)
static char *strcasestr1(const char *haystack, const char *needle)
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

size_t extstring::ufind(std::string needle, size_t pos)
{
	if (pos > length())
		return std::string::npos;
	char *cp = 0;
	char *np = (char *)&needle[0];
	char *hp = (char *)(c_str() + pos);
//Dup declararion (fldigi/flmsg code
	//cp = strcasestr(hp, np);
	cp = strcasestr(hp, np);
//printf("found %s @ %d\n", needle.c_str(), (int)(pos + cp - hp));
	if (cp) return pos + (cp - hp);
	return std::string::npos;
}

void extstring::ureplace(std::string needle)
{
	size_t p;
	size_t len = needle.length();
	p = ufind(needle, 0);
	while (p != std::string::npos) {
		replace(p, len, needle);
		p = ufind(needle, p + len);
	}
}
