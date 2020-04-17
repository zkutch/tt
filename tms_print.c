/*   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */


#include "tt.h"




void tms_print(struct tms * tmsi)
{
    DUMP(tmsi);
    printf("tms_utime %jd\n", tmsi->tms_utime);
    printf("tms_stime %jd\n", tmsi->tms_stime);
    printf("tms_cutime %jd\n", tmsi->tms_cutime);
    printf("tms_cstime %jd\n", tmsi->tms_cstime);    
}


// http://man7.org/linux/man-pages/man2/times.2.html
// struct tms
// {
//     clock_t tms_utime;  /* user time */
//     clock_t tms_stime;  /* system time */
//     clock_t tms_cutime; /* user time of children */
//     clock_t tms_cstime; /* system time of children */
// };
