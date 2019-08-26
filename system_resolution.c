// https://linux.die.net/man/2/clock_gettime
// https://linux.die.net/man/3/clock_gettime


#include "tt.h"

clockid_t clocks[] = {CLOCK_MONOTONIC, CLOCK_REALTIME, CLOCK_REALTIME_COARSE, CLOCK_MONOTONIC_COARSE, CLOCK_MONOTONIC_RAW, CLOCK_BOOTTIME, CLOCK_PROCESS_CPUTIME_ID, CLOCK_THREAD_CPUTIME_ID,};
 const char * clocks_names[] = {"CLOCK_MONOTONIC", "CLOCK_REALTIME", "CLOCK_REALTIME_COARSE", "CLOCK_MONOTONIC_COARSE", "CLOCK_MONOTONIC_RAW", "CLOCK_BOOTTIME", "CLOCK_PROCESS_CPUTIME_ID", "CLOCK_THREAD_CPUTIME_ID", };

void  system_resolution(void)
{
    // checking stack limit
    #include <sys/resource.h>
    struct rlimit rl;
    if(getrlimit(RLIMIT_STACK, &rl) == -1)
    {
        printf("Problems with getrlimit. Error: %s\n", strerror(errno));
        _Exit(56);
    }
    
    printf("Stack limit current %lu and max %lu\n", rl.rlim_cur, rl.rlim_max );// 8388608 = 2**23 - 1 = 8mib; 18446744073709551615 = 2**64 - 1
    if(getrlimit(RLIMIT_RSS, &rl) == -1)
    {
        printf("Problems with getrlimit. Error: %s\n", strerror(errno));
        _Exit(57);
    }
    printf("Memory limit current %lu and max %lu\n", rl.rlim_cur, rl.rlim_max );
    if(getrlimit(RLIMIT_MEMLOCK, &rl) == -1)
    {
        printf("Problems with getrlimit. Error: %s\n", strerror(errno));
        _Exit(58);
    }
    printf("Memory lock current %lu and max %lu\n", rl.rlim_cur, rl.rlim_max );// 67108864 = 2**26 - 1;
    //  https://linux.die.net/man/7/feature_test_macros
    #ifdef _POSIX_C_SOURCE
    printf("_POSIX_C_SOURCE defined: %ldL\n", (long) _POSIX_C_SOURCE);
    #endif
    #ifdef _GNU_SOURCE
    printf("_GNU_SOURCE defined\n");
    #endif
    int tick_per_second, i;
    if((tick_per_second = sysconf(_SC_CLK_TCK)) == -1)      
        printf("Detect sysconf error: %s", strerror(errno));
    else      
        printf("Found ticks per second is %i on given comp.\n", tick_per_second);
    #if (defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0)
    printf("_POSIX_TIMERS defined: %ld\n", (long) _POSIX_TIMERS);
    struct timespec resolution;
    for (i = 0; i<=7; i++) 
    {      	      
        if (clock_getres (clocks[i], &resolution) == -1)
            printf("Detect clock_getres for %s error: %s", clocks_names[i], strerror(errno));
        else
            printf("For %s resolution is sec=%ld nsec=%ld\n", clocks_names[i], resolution.tv_sec, resolution.tv_nsec);
    }	 
    #else
    printf("_POSIX_TIMERS not defined.\n");
    #endif
    _Exit(0);
    
    
}
