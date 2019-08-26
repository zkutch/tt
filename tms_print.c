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
