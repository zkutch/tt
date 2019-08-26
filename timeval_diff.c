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

