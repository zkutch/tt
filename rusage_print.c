#include "tt.h"

void rusage_print(struct rusage * rusi)
{
    DUMP(rusi);
    printf("rusage ru_utime_tv_sec %jd\n", rusi->ru_utime.tv_sec);
    printf("rusage ru_utime_tv_usec %jd\n", rusi->ru_utime.tv_usec);
    printf("rusage ru_stime_tv_sec %jd\n", rusi->ru_stime.tv_sec);
    printf("rusage ru_stime_tv_usec %jd\n", rusi->ru_stime.tv_usec);   
}

// http://man7.org/linux/man-pages/man2/getrusage.2.html
// struct rusage {
//     struct timeval ru_utime; /* user CPU time used */
//     struct timeval ru_stime; /* system CPU time used */
//     long   ru_maxrss;        /* maximum resident set size */
//     long   ru_ixrss;         /* integral shared memory size */
//     long   ru_idrss;         /* integral unshared data size */
//     long   ru_isrss;         /* integral unshared stack size */
//     long   ru_minflt;        /* page reclaims (soft page faults) */
//     long   ru_majflt;        /* page faults (hard page faults) */
//     long   ru_nswap;         /* swaps */
//     long   ru_inblock;       /* block input operations */
//     long   ru_oublock;       /* block output operations */
//     long   ru_msgsnd;        /* IPC messages sent */
//     long   ru_msgrcv;        /* IPC messages received */
//     long   ru_nsignals;      /* signals received */
//     long   ru_nvcsw;         /* voluntary context switches */
//     long   ru_nivcsw;        /* involuntary context switches */
// };


// https://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
// 
// struct timeval
// {
//     time_t         tv_sec     // seconds
//     suseconds_t    tv_usec    // microseconds
// }
