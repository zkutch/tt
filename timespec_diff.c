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

struct timespec timespec_diff(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if (end.tv_nsec < start.tv_nsec) 
	  {
	    temp.tv_sec = end.tv_sec-start.tv_sec-1;
	    temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	  } 
	else 
	  {
	    temp.tv_sec = end.tv_sec-start.tv_sec;
	    temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	  }
	return temp;
}

// https://linux.die.net/man/3/clock_gettime
// struct timespec 
// {
//     time_t   tv_sec;        /* seconds */
//     long     tv_nsec;       /* nanoseconds */
// };
