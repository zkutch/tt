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


struct timeval timeval_diff(struct timeval t1, struct timeval t2)
{
  struct timeval tv;
  if(t2.tv_usec < t1.tv_usec)
    {
      tv.tv_sec = t2.tv_sec - t1.tv_sec -1;
      tv.tv_usec = 1000000 + t2.tv_usec - t1.tv_usec;
    }
  else
    {
      tv.tv_sec = t2.tv_sec - t1.tv_sec;
      tv.tv_usec = t2.tv_usec - t1.tv_usec;
    }
  return tv;
}


// https://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
// 
// struct timeval
// {
//     time_t         tv_sec     // seconds
//     suseconds_t    tv_usec    // microseconds
// }

