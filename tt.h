#include <time.h>	// tm, time_t, clock_gettime, clock_t
#include <errno.h>	// errno
#include <stdio.h>  // printf, size_t
#include <string.h>   // strcpy, strcat, strlen, strerror
#include <sys/time.h>	// tv, tz
#include <unistd.h> //  STDOUT_FILENO, dup2, sysconf (_SC_CLK_TCK), sleep
#include <sys/times.h>	// tms, times
#include <sys/resource.h> // getrusage
#include <limits.h>
#include <stdlib.h> // exit
#include <signal.h>
#include <setjmp.h>

#define DUMP(varname) printf("%s = %x\n", #varname, varname)

void system_resolution(void);
void system_dimension(void);
void tms_print(struct tms * tmsi);
void rusage_print(struct rusage * rusi);
double square_root(double var, float iterations);
void moving_average_variance(size_t * i, void *  x, void * A, void * V, char type);

extern clockid_t clocks[];
extern const char *clocks_names[] ;
void signal_handler(int sig);
void sigaction_handler(int sig, siginfo_t *info, void *context);
jmp_buf point;
void string_2_column(unsigned short width, char * middle);
void help(void);
