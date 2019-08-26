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
