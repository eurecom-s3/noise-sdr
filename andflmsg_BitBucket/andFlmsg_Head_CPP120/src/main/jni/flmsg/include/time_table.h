// =====================================================================
//
// 	time_table.h
//
//  Author(s):
//    Robert Stiles, KK5VD, Copyright (C) 2013
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  It is
// copyright under the GNU General Public License.
//
// You should have received a copy of the GNU General Public License
// along with the program; if not, write to the Free Software
// Foundation, Inc.
// 59 Temple Place, Suite 330
// Boston, MA  02111-1307 USA
//
// =====================================================================

#ifndef __TIME_TABLE_H
#define __TIME_TABLE_H

//Android #include <config.h>
#include <string>
#include <cstring>

extern float seconds_from_c_string(const char *mode, const char *string, int length, float *overhead);
extern float minutes_from_c_string(const char *mode, const char *string, int length, float *overhead);

extern float seconds_from_string(std::string mode, std::string& str, float *overhead);
extern float minutes_from_string(std::string mode, std::string& str, float *overhead);

#endif // __TIME_TABLE_H
