// Copyright (C) 2016-2020 zkutch@yahoo.com 
 

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef MEMORY   // arrays stack or heap allocation
#define MEMORY 0
#endif

#ifndef OPTS_DELIMITER   // delimiter for executable application options
#define OPTS_DELIMITER '@'
#endif

#ifndef EXECUTABLE_OPT_MAX_SIZE
#define EXECUTABLE_OPT_MAX_SIZE 1000
#endif

#ifndef MAX_OPTS
#define MAX_OPTS 100
#endif

#include <stdlib.h>	// atoi, system(), malloc
#include <spawn.h>	// spawn
#include <sys/wait.h>	// waitpid
#include <sys/stat.h> 	// stat
#include </usr/include/stdint.h>
#include "tt.h"
#include <fcntl.h>  //  O_RDONLY, O_WRONLY, O_CLOEXEC
#include "translation.h"
#define RED  "\x1B[31m"
#define RESET "\x1B[0m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define BLU   "\x1B[34m"

#ifndef Version
#define Version 2.1
#endif

struct timespec timespec_diff(struct timespec start, struct timespec end);
struct timeval timeval_diff(struct timeval t1, struct timeval t2);
int check_if_number(char * number);

extern char **environ;


int main(int argc, char **argv) 
{
    // catch Ctr-C
    if(signal(SIGINT, signal_handler) == SIG_ERR)
    {
        printf("\ncan't catch SIGINT\n");
        return 69;
    }
    // prepare additional stack for case overflow
#define SEGV_STACK_SIZE 4096
    stack_t segv_stack;
    if((segv_stack.ss_sp = aligned_alloc(SEGV_STACK_SIZE, SEGV_STACK_SIZE)) == NULL)
    {
        printf("aligned_alloc error.\n");
        return 63;
    }
    segv_stack.ss_flags = 0;
    segv_stack.ss_size = SEGV_STACK_SIZE;
    if(sigaltstack(&segv_stack, NULL))
    {
        printf("Detect sigaltstack error: %s\n", strerror(errno));
        return 64;
    }
    
    // catch SIGSEGV, SIGBUS in additional stack
    struct sigaction sigact_sigsegv;
    if(memset(&sigact_sigsegv, '\0', sizeof(sigaction)) == NULL)
    {
        printf("sigsegv memset error.\n");
        return 66;
    }
    sigact_sigsegv.sa_sigaction = sigaction_handler;
    if(sigfillset(&sigact_sigsegv.sa_mask))
    {
        printf("sigsegv sigfillset error.\n");
        return 65;
    }
    sigact_sigsegv.sa_flags = SA_ONSTACK; 
    if(sigaction(SIGSEGV, &sigact_sigsegv, NULL))
    {
        printf("\ncan't catch SIGSEGV\n");    
        return 70;
    }
        
    struct sigaction sigact_sigbus;
    if(memset(&sigact_sigbus, '\0', sizeof(sigaction)) == NULL)
    {
        printf("sigbus memset error.\n");
        return 67;
    }
    sigact_sigbus.sa_sigaction = sigaction_handler;
    if(sigfillset(&sigact_sigbus.sa_mask))
    {
        printf("sigbus sigfillset error.\n");
        return 68;
    }
    sigact_sigbus.sa_flags = SA_ONSTACK;   
    if(sigaction(SIGBUS, &sigact_sigbus, NULL))
    {
        printf("\ncan't catch SIGBUS\n");
        return 71;
    }

    
    if(setjmp(point))
    {        
        write(STDOUT_FILENO, "Stopped after signal handled.\n\n", 31);
        if(segv_stack.ss_sp != NULL)
            free(segv_stack.ss_sp);
        _exit(62);
    }     

    i18n(); // internationalization 
     
// for measuring whole time          
    struct timeval t01, t02;
    struct tm * tm1, *tm2;    
	
    if (gettimeofday( &t01, NULL ) == -1)
    {
        printf(_("Detect gettimeofday error: %s"), strerror(errno));
        return 17;
    }
    if(!(tm1 = localtime ( &t01.tv_sec )))
    {
        printf(_("Detect localtime error: %s"), strerror(errno));
        return 18;
    }
    puts("");
    printf(_("Start working"));
    printf(" %d:%02d:%02d:%ld.", tm1->tm_hour, tm1->tm_min, tm1->tm_sec, t01.tv_usec);
    puts("\n");
    size_t i =0;
    
    char *argv1[MAX_OPTS]; 
    char * executable;
    
    _Bool w = 1; // key for lunch work works only '-R' or '-d' options
// start command line arguments  
    size_t circles = 0;
    size_t sleep_sec = 0;
    size_t c;
    _Bool d = 0; // dimensions
    _Bool R = 0; // resolution
    _Bool q = 0; // quiet
    _Bool r = 0; // getrusage
    _Bool g = 0; // gettimeofday
    _Bool t = 0; // times
    _Bool o = 0; // clock
    _Bool m = 0; // time Epoch(jan 1 1970)
    _Bool s = 0; //  timespec_get  ISO 2011 391pg:409epg
    _Bool a = 0; // CLOCK_PROCESS_CPUTIME_ID https://linux.die.net/man/3/clock_gettime   
    _Bool p = 0; // force posix_spawn
    _Bool H = 0; // hide arrays in printing
    _Bool f = 0; // write arrays to file
    _Bool S = 0; // sleep between iterations
    _Bool M = 0; // not keep intermediate results
    _Bool h = 0; // help
    _Bool V = 0; // Version
    struct stat sb;
    
    char * circles_char;
    char * sleep_char;
    while (--argc > 0 )
    {
        if((*++argv)[0] == '-')
        {
            c = *++argv[0];               
            switch (c) 
            {
                case 'c':   // number of cycles
                    if(*++argv == 0x0)
                    {
                        printf(_("After option \"-c\" should be integer.\n\n"));
                        return 23;
                    }
                    circles_char = malloc( strlen((char *)*argv)  * sizeof(char)+1 );
                    if(circles_char == NULL)
                    {
                        puts("Cannot allocate memory for cycles after option \"-c\".\n");
                        return 21;
                    } 
                    strcpy(circles_char, *argv);                    
                    if(check_if_number(circles_char))
                    {
                        printf(_("After option \"-c\" should be integer, number of cycles.\n\n"));
                        free(circles_char);
                        return 11;
                    }
                    circles = atoi(circles_char);
                    free(circles_char);
                    // TODO check if circles is not overflow
                    printf(_("Given %lu amount of cycles.\n"), circles);
                    --argc;
                    break;
                case 'b':   // path to executable
                    if(*++argv == 0x0)
                    {
                        printf(_("After option \"-b\" should be executable.\n\n"));
                        return 24;
                    }
                    executable = malloc(strlen((char *)*argv)  * sizeof(char) + 1);
                    if(executable == NULL)
                    {
                        puts("Cannot allocate memory for path after option \"-b\".\n");
                        return 13;
                    }
                    strcpy(executable, *argv);
                    // TODO check if executable is not too long                    
                    if(!stat(executable, &sb))
                    {
                        if(S_ISREG(sb.st_mode) && sb.st_mode & 0111)
                            printf(_("Given %s is executable.\n"), executable);
                        else
                        {
                            printf(_("Given %s is not executable.\n"), executable);
                            return 16;
                        }
                    }
                    else
                    {
                        printf(_("Problems with stat %s. Error: %s\n"), executable, strerror(errno));
                        return 17;
                    }
                    argv1[0] = executable;
                    short ind1=1;
                    short found_delimiter = 0;
                    char ** quota = argv;
                    if(*++quota != 0x0)
                    {
                    if(**(quota) == OPTS_DELIMITER)
                    {
                        short maxlen = 0;
                        found_delimiter = 1;                        
                        quota = ++argv;
                        --argc;
                        if(strlen(*argv) == 1)  // for case when after OPTS_DELIMITER is space(s)
                        {
                            ind1 = 0;
                        }
                        else
                        {
                            argv1[ind1] = *quota;
                            
                            short jj=0;
                            for(jj=0; jj<strlen(*quota)-1; jj++)       //  eliminate starting OPTS_DELIMITER in first option                 
                                argv1[ind1][jj] = argv1[ind1][jj+1];                        
                            argv1[ind1][jj] = '\0';                           
                            
                            maxlen = strlen(*quota);
                            
                            if(quota[0][strlen(*quota)-1] == OPTS_DELIMITER)
                            {                                
                                found_delimiter = 2;
                                quota[0][strlen(*quota)-1] = '\0'; // eliminate finishing OPTS_DELIMITER in last option
                                argv1[ind1] = *quota;
                                ++ind1;
                                goto EXE;
                            } 
                        }
                        
                        while(ind1 < MAX_OPTS && maxlen < EXECUTABLE_OPT_MAX_SIZE && argc > 0)
                        {
                            ++quota;
                            ind1++;
                            --argc;
                            ++argv;
                            if(*argv == 0x0)
                                break;
                            if(quota[0][strlen(*quota)-1] == OPTS_DELIMITER)
                            {                                
                                quota[0][strlen(*quota)-1] = '\0'; // eliminate finishing OPTS_DELIMITER in last option
                                if(strlen(*quota) > 1)
                                {
                                    argv1[ind1] = *quota;
                                    ind1++;
                                }
                                found_delimiter = 2;
                                break;
                            } 
                            maxlen += strlen(*quota);
                            argv1[ind1] = *quota;
                            
                        }
                        if(maxlen >= EXECUTABLE_OPT_MAX_SIZE)
                        {
                            printf(_("Error in EXECUTABLE_OPT_MAX_SIZE.\n"));
                            return 76;
                        }
                        else if(ind1 >=  MAX_OPTS)
                        {
                            printf(_("Error in MAX_OPTS.\n"));
                            return 77;
                        }
                                            
                    }
                    }
EXE:                if(found_delimiter == 1)
                    {
                        printf(_("Not found closing executable options delimiter.\n"));
                        return 78;                        
                    }
                    argv1[ind1] = NULL;
                    
                    if(ind1>1)
                    {
                        printf(_("Executable line: "));
                        for(short int jj = 0; jj < ind1; jj++)
                        printf(_("%s "), argv1[jj]);
                        puts("");
                    }  
                    --argc;
                    break;
                case 'd':
                    if(strlen(*argv) > 1)
                    {
                        printf(_("Option \"-d\" should consist from one letter.\n"));
                        return 25;
                    }
                    d = 1;
                    break;
                case 'R':   // resolution
                    if(strlen(*argv) > 1)
                    {
                        printf(_("Option \"-R\" should consist from one letter.\n"));
                        return 26;
                    }
                    R = 1;
                    break;
                case 'r':   // getrusage
                    if(strlen(*argv) > 1)
                    {
                        printf(_("Option \"-r\" should consist from one letter.\n"));
                        return 27;
                    }
                    r = 1;
                    break;
                case 'q':   // quiet
                    if(strlen(*argv) > 1)
                    {
                        printf(_("Option \"-q\" should consist from one letter.\n"));
                        return 28;
                    }
                    q = 1;
                    break;
                case 'g':   // gettimeofday
                    if(strlen(*argv) > 1)
                    {
                        printf(_("Option \"-g\" should consist from one letter.\n"));
                        return 29;
                    }
                    g = 1;
                    break;
                case 't':   // times
                    if(strlen(*argv) > 1)
                    {
                        printf(_("Option \"-t\" should consist from one letter.\n"));
                        return 33;
                    }
                    t = 1;
                    break;
                case 'o':   // clock
                    if(strlen(*argv) > 1)
                    {
                        printf(_("Option \"-o\" should consist from one letter.\n"));
                        return 30;
                    }
                    o = 1;
                    break;
                case 'm':   // time
                    if(strlen(*argv) > 1)
                    {
                        printf(_("Option \"-m\" should consist from one letter.\n"));
                        return 36;
                    }
                    m = 1;
                    break;
                case 's':   // timespec_get
                    if(strlen(*argv) > 1)
                    {
                        printf(_("Option \"-s\" should consist from one letter.\n"));
                        return 41;
                    }
                    s = 1;
                    break;
                case 'a':   // timespec_get
                    if(strlen(*argv) > 1)
                    {
                        printf(_("Option \"-a\" should consist from one letter.\n"));
                        return 42;
                    }
                    a = 1;
                    break;
                case 'p':   // posix_spawn
                    if(strlen(*argv) > 1)
                    {
                        printf(_("Option \"-p\" should consist from one letter.\n"));
                        return 45;
                    }
                    p = 1;
                    break;
                case 'H':   // hide arrays
                    if(strlen(*argv) > 1)
                    {
                        printf(_("Option \"-H\" should consist from one letter.\n"));
                        return 46;
                    }
                    H = 1;
                    break;
                case 'f':   // hide arrays
                    if(strlen(*argv) > 1)
                    {
                        printf(_("Option \"-f\" should consist from one letter.\n"));
                        return 47;
                    }
                    f = 1;
                    break;
                case 'S':   // sleep between iterations: TODO random sleep
                    if(*++argv == 0x0)
                    {
                        printf(_("After option \"-S\" should be integer.\n\n"));
                        return 23;
                    }
                    sleep_char = malloc( strlen((char *)*argv)  * sizeof(char)+1 );
                    if(sleep_char == NULL)
                    {
                        puts("Cannot allocate memory for sleep after option \"-S\".\n");
                        return 21;
                    } 
                    strcpy(sleep_char, *argv);                    
                    if(check_if_number(sleep_char))
                    {
                        printf(_("After option \"-S\" should be integer, number of sleep seconds.\n\n"));
                        free(sleep_char);
                        return 11;
                    }
                    sleep_sec = atoi(sleep_char);
                    free(sleep_char);
                    S = 1;
                    // TODO check if sleep_sec is not overflow
                    printf(_("Sleep time %lu sec.\n"), sleep_sec);
                    --argc;
                    break;
                    case 'M':   // hide arrays
                    if(strlen(*argv) > 1)
                    {
                        printf(_("Option \"-M\" should consist from one letter.\n"));
                        return 51;
                    }
                    M = 1;
                    break;
                    case 'h':   // help
                    if(strlen(*argv) > 1)
                    {
                        printf(_("Option \"-h\" should consist from one letter.\n"));
                        return 72;
                    }
                    h = 1;
                    break;
                    case 'V':   // Version
                    if(strlen(*argv) > 1)
                    {
                        printf(_("Option \"-V\" should consist from one letter.\n"));
                        return 72;
                    }
                    V = 1;
                    break;
                    case '-':   //long options
                        if(strcmp(*argv, "-help") == 0)       // help                 
                            h = 1;                        
                        else if(strcmp(*argv, "-version") == 0)     // Version
                            V = 1;                        
                        else
                        {
                            printf(_("Found: unknown option %s\n"), *argv);
                            return 79;
                        }
                        break;
                default:
                    printf(_("Found: illegal option %lu\n"), c);
                    return 12;                                       
                    break;
            }
        }   // finish if((*++argv)[0] == '-')
    }   // finish while (--argc > 0 )
    
    
    if(h == 1)
    {        
        help();
        if(segv_stack.ss_sp != NULL)
            free(segv_stack.ss_sp);
        return 73;
    }
    else if(V == 1)
    {        
        printf(_("Version: %.2f\n"), Version);      
        if(segv_stack.ss_sp != NULL)
            free(segv_stack.ss_sp);
        return 74;
    } 
    else
    {
        if(circles == 0 || executable == 0)
        {
            if(d == 0 && R == 0)
            {          
                puts(_("Nothing to do.\n"));
                return 0;             
            }
            else
            {
                r=g=t=o=m=s=a=p=H=f=S=0;    // works only '-R' or '-d' options
                w = 0;
            }        
        }
    }
    
// finish command line options 
 
 struct timeval tv1, tv2, tv3, tv4, tv5; 
 clock_t tim1,tim2;
 struct timespec tp1, tp2, tp3, tp4, tp5, tp6;
 struct timespec ts1, ts2, ts3;
 struct rusage rus1, rus3;  // rus2,
 
 struct tms tms1, tms2;
 clock_t tim3, tim4;
 
 time_t start, end;
 
 pid_t pid;
     
if(p)
    printf(_("Forced posix_spawn.\n"));

// start getting system dimensions   
if(d)
{
    system_dimension();
    printf("\n");
}
// finish getting system dimensions 
// start getting system resolution
if(R)
{
    system_resolution();
    printf("\n");
}
// finish getting system resolution 

#if MEMORY != 0     // arrays stack or heap allocation
double * clock_timesp;
struct timeval * gettimeofday_timesp;
struct timespec * clock_gettime_timesp;
struct timespec * clock_gettime_times1p;
struct timespec * timespec_get_timesp;
struct rusage * getrusage_timesp;
struct tms * times_timesp;
clock_t * times_real_timesp;
time_t * time_timesp;
#endif

if(w)
{    
    
// start time structures decalration
    
    
    long tick_per_second = sysconf(_SC_CLK_TCK);
    if (tick_per_second == -1)
    {
            printf(_("Detect sysconf error: %s\n"), strerror(errno));
            return 40;
    }
    int status;    
     
    #if MEMORY == 0 // arrays stack allocation
    double clock_times[circles];
    struct timeval gettimeofday_times[circles];
    struct timespec clock_gettime_times[circles];
    struct timespec clock_gettime_times1[circles];
    struct timespec timespec_get_times[circles];
    struct rusage getrusage_times[circles];
    struct tms times_times[circles];
    clock_t times_real_times[circles];
    time_t time_times[circles];
    write(STDOUT_FILENO, "Working in stack.\n\n", 19);
    #else   // arrays heap allocation
    clock_timesp = malloc(circles * sizeof(double));
    if(clock_timesp == NULL)
    {
        printf(_("clock_timesp malloc error: %s\n"), strerror(errno));
        return 55;
    }
    gettimeofday_timesp = malloc(circles * sizeof(struct timeval));
    if(gettimeofday_timesp == NULL)
    {
        printf(_("gettimeofday_timesp malloc error: %s\n"), strerror(errno));
        return 52;
    }
    clock_gettime_timesp = malloc(circles * sizeof(struct timespec));
    if(clock_gettime_timesp == NULL)
    {
        printf(_("clock_gettime_timesp malloc error: %s\n"), strerror(errno));
        return 53;    
    }
    clock_gettime_times1p = malloc(circles * sizeof(struct timespec));
    if(clock_gettime_times1p == NULL)
    {
        printf(_("clock_gettime_times1p malloc error: %s\n"), strerror(errno));
        return 54;    
    }
    timespec_get_timesp = malloc(circles * sizeof(struct timespec));
    if(timespec_get_timesp == NULL)
    {
        printf(_("timespec_get_timesp malloc error: %s\n"), strerror(errno));
        return 59;    
    }
    getrusage_timesp = malloc(circles * sizeof(struct rusage));
    if(getrusage_timesp == NULL)
    {
        printf(_("getrusage_timesp malloc error: %s\n"), strerror(errno));
        return 75;    
    }
    times_timesp = malloc(circles * sizeof(struct tms));
    if(times_timesp == NULL)
    {
        printf(_("times_timesp malloc error: %s\n"), strerror(errno));
        return 60;    
    }
    times_real_timesp = malloc(circles * sizeof(clock_t));
    if(times_real_timesp == NULL)
    {
        printf(_("times_real_timesp malloc error: %s\n"), strerror(errno));
        return 61;    
    }
    time_timesp = malloc(circles * sizeof(time_t));
    if(time_timesp == NULL)
    {
        printf(_("time_timesp malloc error: %s\n"), strerror(errno));
        return 62;    
    }
    printf(_("Working in heap.\n\n"));    
    #endif
    
    
    double Aclock_times = 0;
    double Vclock_times = 0;
    double Mclock_times;
    double mclock_times;
    
    double Agettimeofday_times = 0;
    double Vgettimeofday_times = 0;
    double Mgettimeofday_times = 0;
    double mgettimeofday_times = 0;
    
    double Aclock_gettime_times = 0;
    double Vclock_gettime_times = 0;
    double Mclock_gettime_times;
    double mclock_gettime_times;
    
    double Aclock_gettime_times1 = 0;
    double Vclock_gettime_times1 = 0;
    double Mclock_gettime_times1 = 0;
    double mclock_gettime_times1 = 0;
       
    double Atimespec_get_times = 0;
    double Vtimespec_get_times = 0;
    double Mtimespec_get_times = 0;
    double mtimespec_get_times = 0;
    
    double Asgetrusage_times = 0;
    double Vsgetrusage_times = 0;
    double Msgetrusage_times = 0;
    double msgetrusage_times = 0;
    double Augetrusage_times = 0;
    double Vugetrusage_times = 0;
    double Mugetrusage_times = 0;
    double mugetrusage_times = 0;
    
    float Autimes_times = 0;
    float Vutimes_times = 0;
    float Mutimes_times = 0;
    float mutimes_times = 0;
    float Astimes_times = 0;
    float Vstimes_times = 0;
    float Mstimes_times = 0;
    float mstimes_times = 0;
    float Acutimes_times = 0;
    float Vcutimes_times = 0;
    float Mcutimes_times = 0;
    float mcutimes_times = 0;
    float Acstimes_times = 0;
    float Vcstimes_times = 0;
    float Mcstimes_times = 0;
    float mcstimes_times = 0;
    
    float Atimes_real_times = 0;    
    float Vtimes_real_times = 0;
    float Mtimes_real_times = 0;    
    float mtimes_real_times = 0;
    
    float Atime_times = 0;
    float Vtime_times = 0;
    clock_t Mtime_times = 0;
    clock_t mtime_times = 0;
// finish time structures decalration           

    if(M && !H)   // not keep intermediate results  implies hiding arrays           
        H = 1;
    
// start choose posix clock
    _Bool P = 0; // for posix timers
    short posix_best = -1;
    #if (defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0)
    P = 1;
    struct timespec resolution;
    struct timespec resolution1; 
    printf(_("Detect posix timers.\n"));
    for(short k = 0; k < 6; k++)     // 6 upper limit from array clocks[]
    {        
        if(clock_getres(clocks[k], &resolution) != -1)
        {
            posix_best = k;
            if(k != 5)
            {
                for(short kk = k+1; kk < 6; kk++)
                {
                    if(clock_getres(clocks[kk], &resolution1) != -1)
                    {
                        if(resolution1.tv_sec < resolution.tv_sec)
                            posix_best = kk;
                        else if(resolution1.tv_sec == resolution.tv_sec)
                        {
                            if(resolution1.tv_nsec < resolution.tv_nsec)
                                posix_best = kk;
                        }
                    }                    
                }
                
            }
            break;
        } 
    }
    if(a)
    {
        printf(_("Choose posix 1st clock %s\n"), clocks_names[posix_best]);
        printf(_("Choose posix 2nd clock CLOCK_PROCESS_CPUTIME_ID\n"));
    }
    else
        printf(_("Choose posix clock %s\n"), clocks_names[posix_best]);
    #else
    printf(_("Posix timers not detected.\n"));
    #endif    
// finish choose posix clock
    int devNull = 0;   
    double yd;  // for double moving_average_variance
    float yf;  // for float moving_average_variance
    unsigned int slp;   // for sleep output
    i = 0;
    while(i<circles)
    {
        //	start first check time
        if(r)
            if(getrusage(RUSAGE_CHILDREN, &rus1) == -1)
            {
                printf(_("Detect getrusage error: %s\n"), strerror(errno));
                return 8;
            } 

        if(P)
        {
            if(clock_gettime(clocks[posix_best], &tp1) == -1)  
            {
                printf(_("Detect clock_gettime error: %s\n"), strerror(errno));
                return 3;
            }
            if(a)
            {
                if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp4) == -1) 
                {
                    printf(_("Detect clock_gettime error: %s\n"), strerror(errno));
                    return 43;
                }
            }
        }
        if(m)
            if(time(&start) == ((time_t) -1))
            {
                printf(_("Detect time() error\n"));
                return 38;                
            }            
        if(o)
        {
            if((tim1 = clock()) == -1)
            {
                printf(_("Detect clock error: %s\n"), strerror(errno));
                return 9;
            }  
//             printf(_("tim1 %ld\n", (clock_t)tim1));
        }
        if(g)
            if (gettimeofday(&tv1, NULL) == -1)
            {
                printf(_("Detect gettimeofday error: %s\n"), strerror(errno));
                return 4;
            }
        if(s)
        {
            if(!timespec_get(&ts1, TIME_UTC))
            {
                printf(_("Detect timespec_get error: %s\n"), strerror(errno));
                return 41;    
            }                
        }
        if(t)
        {
            if ((tim3 = times(&tms1)) == -1)
            {
                printf(_("Detect times error: %s\n"), strerror(errno));
                return 5;
            }
        }
        
        //	finish first check time 
        
        //	start process
        if(p)   // force posix_spawn
        {            
            // start redirect STDERR_FILENO to /dev/null        
            if(q)
            {            
                devNull = open("/dev/null", O_WRONLY);
                if (devNull == -1)
                {
                    printf(_("Detect /dev/null open error: %s\n"), strerror(errno));
                    return 33;
                }                
                if (dup3(devNull, STDERR_FILENO, O_CLOEXEC) == -1)                
                {
                    printf(_("Detect dup error: %s\n"), strerror(errno));
                    return 34;
                }            
            }
            // finish redirect STDERR_FILENO to /dev/null
            // start attributes and file actions for posix_spawn
            int status1;
            posix_spawnattr_t attr;
            posix_spawnattr_t * attrp = NULL;
            
            status1 = posix_spawnattr_init(&attr);
            if (status1 != 0)
            {          
                printf(_("Detect posix_spawnattr_init error: %s with status %d\n"), strerror(errno), status1);
                return 27; 
            }
            status1 = posix_spawnattr_setflags(&attr, POSIX_SPAWN_USEVFORK);
            if (status1 != 0) 
            { 
                printf(_("Detect posix_spawnattr_setflags error: %s with status %d\n"), strerror(errno), status1);
                return 28;            
            }
            attrp = &attr;
            
            posix_spawn_file_actions_t file_actions;
            posix_spawn_file_actions_t * file_actionsp = NULL; 
            if(q)
            {
                status1 = posix_spawn_file_actions_init(&file_actions);
                if (status1 != 0) 
                { 
                    printf(_("Detect posix_spawn_file_actions_init error: %s with status %d\n"), strerror(errno), status1);
                    return 29;         
                }                                                                                                                                                                                                                                                                      
                status1 = posix_spawn_file_actions_addclose(&file_actions, STDOUT_FILENO);
                if (status1 != 0) 
                { 
                    printf(_("Detect posix_spawn_file_actions_addclose error: %s with status %d\n"), strerror(errno), status1);
                    return 30;         
                }
                file_actionsp = &file_actions;
            }
            // finish attributes and file actions for posix_spawn
            //	start posix_spawn process	
            status = posix_spawn(&pid, argv1[0], file_actionsp, attrp, argv1, environ);
            if (status == 0) 
            { 	      
                if (waitpid(pid, &status, 0) != -1)
                    //             if(wait4(pid, &status, 0, &rus2) != -1)
                {
                    if(!q)
                        printf(_("Child exited with status %i on step %ld.\n"), status, i);
//                     statuses[i] = status;                    
                } 
                else 
                {
                    printf(_("waitpid error: %s on step %ld.\n"), strerror(errno), i);
                    return 14;
                }
            } 
            else 
            {
                printf(_("posix_spawn error: %s on step %ld.\n"), strerror(status), i);
                return 15;
            }                    
            //	finish posix_spawn process  
            // close /dev/null descriptor  
            if(q)
            {                              
                if (close(devNull) == -1)
                {
                    printf(_("Detect close error: %s\n"), strerror(errno));
                    return 35;
                }               
            }            
            // start destroy spawn attr and file_actions
            if (attrp != NULL) 
            {     
                status1 = posix_spawnattr_destroy(attrp);
                if (status1 != 0) 
                { 
                    printf(_("Detect posix_spawnattr_destroy error: %s with status %d\n"), strerror(errno), status1);
                    return 31;         
                }     
            }
            
            if (file_actionsp != NULL) 
            {
                status1 = posix_spawn_file_actions_destroy(file_actionsp);
                if (status1 != 0) 
                { 
                    printf(_("Detect posix_spawn_file_actions_destroy error: %s with status %d"), strerror(errno), status1);
                    return 32;          
                } 
                
            }
            // finish destroy spawn attr and file_actions
            
        }   // finish force posix_spawn
        else    // use fork execve
        {
            if((pid = fork()) < 0)
            {
                printf(_("fork error: on step %lu.\n"),  i);
                return 15;
            }
            else if(pid == 0) // son
            {
                // start redirect STDERR_FILENO, STDOUT_FILENO to /dev/null            
                if(q)
                {            
                    devNull = open("/dev/null", O_WRONLY);
                    if(devNull == -1)
                    {
                        printf(_("Detect /dev/null open error: %s\n"), strerror(errno));
                        return 35;
                    }                
                    
                    if(dup3(devNull, STDERR_FILENO, O_CLOEXEC) == -1)                    
                    {
                        printf(_("Detect dup3 error for STDERR_FILENO: %s\n"), strerror(errno));
                        return 34;
                    }
                    if(dup3(devNull, STDOUT_FILENO, O_CLOEXEC) == -1)                    
                    {
                        printf(_("Detect dup3 error for STDOUT_FILENO: %s\n"), strerror(errno));
                        return 31;
                    }                
                }
                // finish redirect STDERR_FILENO, STDOUT_FILENO to /dev/null
                if(execve(argv1[0], argv1, environ) == -1)
                {
                    printf(_("execve error: on step %lu.\n"), i);
                    return 14;
                }              
            }
            //if(pid == wait(&status))
            //         if(wait4(pid, &status, 0, &rus2) == -1)
            if(waitpid(pid, &status, 0) == -1)
            {
                printf(_("wait error: on step %lu.\n"), i);
                return 32;            
            }
            else if(!WIFEXITED(status))
            {
                printf(_("Child did not exit successfully on step %lu\n"), i);
                return 39;
            }
            else
            {            
                if(!q)
                    printf(_("Child exited with status %i on step %lu.\n"), status, i);
            }
        }   // finish use fork execve
        
        //	finish process   
        
        //	starting second check time	
        if(r)
            if(getrusage(RUSAGE_CHILDREN, &rus3) == -1)
            {
                printf(_("Detect getrusage error: %s\n"), strerror(errno));
                return 8;
            } 

        if(g)
            if (gettimeofday( &tv2, NULL ) == -1)
            {
                printf(_("Detect gettimeofday error: %s\n"), strerror(errno));
                return 6;
            }
        if(P)
        {
            if(clock_gettime(clocks[posix_best], &tp2) == -1) 
            {
                printf(_("Detect clock_gettime error: %s\n"), strerror(errno));
                return 7;
            }
            if(a)
            {
                if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp5) == -1) 
                {
                    printf(_("Detect clock_gettime error: %s\n"), strerror(errno));
                    return 44;
                }
            }
        }
        if(m)
            if(time(&end) == ((time_t) -1))
            {
                printf(_("Detect time() error\n"));
                return 37;                
            }
        if(o)
        {
            if((tim2 = clock()) == -1)
            {
                printf(_("Detect clock error: %s\n"), strerror(errno));
                return 9;
            } 
        }
        if(s)
        {
            if( !timespec_get(&ts2, TIME_UTC) )
            {
                printf(_("Detect timespec_get error: %s\n"), strerror(errno));
                return 42;   
            }
                
        }
        if(t)
        {
            if ((tim4 = times(&tms2)) == -1)
            {
                printf(_("Detect times error: %s\n"), strerror(errno));
                return 10;
            }
        }
        //	finish second check time
        
        //	starting calculate/save diffs
        if(o)
        {                        
            yd = ((clock_t)(tim2 - tim1));            
            if(!M)
            #if MEMORY == 0
                clock_times[i] = yd;
            #else
                *(clock_timesp + i) = yd;            
            #endif
            
            //   catch Max and Min
            if(i)
            {
                if(Mclock_times < yd)
                    Mclock_times = yd;
                if(mclock_times > yd)
                    mclock_times = yd;
            }
            else
            {
                Mclock_times = yd;
                mclock_times = yd;
            }
            // calculate moving average and Welford variance            
            moving_average_variance(&i, &yd, &Aclock_times, &Vclock_times, 'd');
        }
        
        if(g)
        {
            tv3 = timeval_diff(tv1, tv2);
            yd = (double)(tv3.tv_sec + (double)tv3.tv_usec/1000000);
            if(!M)
            {
                #if MEMORY == 0                
                gettimeofday_times[i].tv_sec = (double)tv3.tv_sec;
                gettimeofday_times[i].tv_usec = tv3.tv_usec;                
                #else            
                (gettimeofday_timesp + i)->tv_sec = (double)tv3.tv_sec; 
                (gettimeofday_timesp + i)->tv_usec = tv3.tv_usec;
                #endif
            }

            //   catch Max and Min
            if(i)
            {
                if(Mgettimeofday_times < yd)
                    Mgettimeofday_times = yd;
                if(mgettimeofday_times > yd)
                    mgettimeofday_times = yd;
            }
            else
            {
                Mgettimeofday_times = yd;
                mgettimeofday_times = yd;
            }
            // calculate moving average and Welford variance            
            moving_average_variance(&i, &yd, &Agettimeofday_times, &Vgettimeofday_times, 'd');
        }
        if(P)
        {
            tp3 = timespec_diff(tp1,tp2);
            yd = ((double)tp3.tv_sec + (double)tp3.tv_nsec/1000000000);
            if(!M)
            {
                #if MEMORY == 0
                clock_gettime_times[i].tv_sec = (double)tp3.tv_sec;
                clock_gettime_times[i].tv_nsec = tp3.tv_nsec;            
                #else            
                (clock_gettime_timesp + i)->tv_sec = (double)tp3.tv_sec;
                (clock_gettime_timesp + i)->tv_nsec = tp3.tv_nsec;
                #endif
            }

            //   catch Max and Min
            if(i)
            {
                if(Mclock_gettime_times < yd)
                    Mclock_gettime_times = yd;
                if(mclock_gettime_times > yd)
                    mclock_gettime_times = yd;
            }
            else
            {
                Mclock_gettime_times = yd;
                mclock_gettime_times = yd;
            }        
            // calculate moving average and Welford variance         
            moving_average_variance(&i, &yd, &Aclock_gettime_times, &Vclock_gettime_times, 'd');
            
            
            if(a)
            {
                tp6 = timespec_diff(tp4,tp5);
                yd = (double)(tp6.tv_sec + (double)tp6.tv_nsec/1000000000);
                if(!M)
                {
                    #if MEMORY == 0                     
                    clock_gettime_times1[i].tv_sec = (double)tp6.tv_sec;
                    clock_gettime_times1[i].tv_nsec = tp6.tv_nsec;
                    #else                
                    (clock_gettime_times1p + i)->tv_sec = (double)tp6.tv_sec;
                    (clock_gettime_times1p + i)->tv_nsec = tp6.tv_nsec;                
                    #endif
                }
                //   catch Max and Min
                if(i)
                {
                    if(Mclock_gettime_times1 < yd)
                        Mclock_gettime_times1 = yd;
                    if(mclock_gettime_times1 > yd)
                        mclock_gettime_times1 = yd;
                }
                else
                {
                    Mclock_gettime_times1 = yd;
                    mclock_gettime_times1 = yd;
                }  
                // calculate moving average and Welford variance            
                moving_average_variance(&i, &yd, &Aclock_gettime_times1, &Vclock_gettime_times1, 'd');
            }
        }
        if(s)
        {            
            ts3 = timespec_diff(ts1,ts2);
            yd = (double)(ts3.tv_sec + (double)ts3.tv_nsec/1000000000);
            if(!M)
            {
                #if MEMORY == 0
                timespec_get_times[i].tv_sec = (double)ts3.tv_sec;
                timespec_get_times[i].tv_nsec = ts3.tv_nsec;             
                #else
                (timespec_get_timesp + i)->tv_sec = (double)ts3.tv_sec;
                (timespec_get_timesp + i)->tv_nsec = ts3.tv_nsec;
                #endif
            }
            //   catch Max and Min
            if(i)
            {
                if(Mtimespec_get_times < yd)
                    Mtimespec_get_times = yd;
                if(mtimespec_get_times > yd)
                    mtimespec_get_times = yd;
            }
            else
            {
                Mtimespec_get_times = yd;
                mtimespec_get_times = yd;
            }  
            // calculate moving average and Welford variance            
            moving_average_variance(&i, &yd, &Atimespec_get_times, &Vtimespec_get_times, 'd');
        }
        
        if(r)
        {  
            tv4 = timeval_diff(rus1.ru_utime, rus3.ru_utime);	
            tv5 = timeval_diff(rus1.ru_stime, rus3.ru_stime);
            yd = (double)(tv5.tv_sec + (double)tv5.tv_usec/1000000);
            if(!M)
            {
                #if MEMORY == 0                
                getrusage_times[i].ru_utime.tv_sec = (double)tv4.tv_sec;
                getrusage_times[i].ru_utime.tv_usec = tv4.tv_usec;                
                getrusage_times[i].ru_stime.tv_sec = (double)tv5.tv_sec;
                getrusage_times[i].ru_stime.tv_usec = tv5.tv_usec;                
                #else
                (getrusage_timesp + i)->ru_utime.tv_sec = (double)tv4.tv_sec;
                (getrusage_timesp + i)->ru_utime.tv_usec = tv4.tv_usec;                
                (getrusage_timesp + i)->ru_stime.tv_sec = (double)tv5.tv_sec;
                (getrusage_timesp + i)->ru_stime.tv_usec = tv5.tv_usec;
                #endif
            }
            //   catch Max and Min
            if(i)
            {
                if(Msgetrusage_times < yd)
                    Msgetrusage_times = yd;
                if(msgetrusage_times > yd)
                    msgetrusage_times = yd;
            }
            else
            {
                Msgetrusage_times = yd;
                msgetrusage_times = yd;
            }  
            // calculate moving average and Welford variance
            moving_average_variance(&i, &yd, &Asgetrusage_times, &Vsgetrusage_times, 'd');
            
            yd = (double)(tv4.tv_sec + (double)tv4.tv_usec/1000000);
            //   catch Max and Min
            if(i)
            {
                if(Mugetrusage_times < yd)
                    Mugetrusage_times = yd;
                if(mugetrusage_times > yd)
                    mugetrusage_times = yd;
            }
            else
            {
                Mugetrusage_times = yd;
                mugetrusage_times = yd;
            } 
            // calculate moving average and Welford variance
            moving_average_variance(&i, &yd, &Augetrusage_times, &Vugetrusage_times, 'd'); 
        }
        if(m)
        {
            yf = (end - start);
            if(!M)
            #if MEMORY == 0
                time_times[i] = (end - start);
            #else
                *(time_timesp + i) = (end - start);
            #endif

            //   catch Max and Min
            if(i)
            {
                if(Mtime_times < yf)
                    Mtime_times = yf;
                if(mtime_times > yf)
                    mtime_times = yf;
            }
            else
            {
                Mtime_times = yf;
                mtime_times = yf;
            } 
            // calculate moving average  and Welford variance 

            moving_average_variance(&i, &yf, &Atime_times, &Vtime_times, 'f');
        }
            
        if(t)
        {            
            yf = (float)(tim4 - tim3);
            if(!M)
            #if MEMORY == 0
                times_real_times[i] =  (tim4 - tim3);
            #else
                *(times_real_timesp + i) = (tim4 - tim3);
            #endif
            //   catch Max and Min
            if(i)
            {
                if(Mtimes_real_times < yf)
                    Mtimes_real_times = yf;
                if(mtimes_real_times > yf)
                    mtimes_real_times = yf;
            }
            else
            {
                Mtimes_real_times = yf;
                mtimes_real_times = yf;
            } 
            // calculate moving average  and Welford variance

            moving_average_variance(&i, &yf, &Atimes_real_times, &Vtimes_real_times, 'f'); 
            
            yf =  (tms2.tms_utime - tms1.tms_utime);
            if(!M)
            #if MEMORY == 0
                times_times[i].tms_utime =  yf;
            #else
                (times_timesp + i)->tms_utime =  yf;
            #endif
            //   catch Max and Min
            
            if(i)
            {
                if(Mutimes_times < yf)
                    Mutimes_times = yf;
                if(mutimes_times > yf)
                    mutimes_times = yf;
            }
            else
            {
                Mutimes_times = yf;
                mutimes_times = yf;
            } 
            // calculate moving average  and Welford variance

            moving_average_variance(&i, &yf, &Autimes_times, &Vutimes_times, 'f');
            
            yf =  (tms2.tms_stime - tms1.tms_stime);
            if(!M)
            #if MEMORY == 0
                times_times[i].tms_stime =  yf;
            #else
                (times_timesp + i)->tms_stime =  yf;
            #endif   
            //   catch Max and Min
            if(i)
            {
                if(Mstimes_times < yf)
                    Mstimes_times = yf;
                if(mstimes_times > yf)
                    mstimes_times = yf;
            }
            else
            {
                Mstimes_times = yf;
                mstimes_times = yf;
            } 
            // calculate moving average  and Welford variance

            moving_average_variance(&i, &yf, &Astimes_times, &Vstimes_times, 'f');
            
            yf =  (tms2.tms_cstime - tms1.tms_cstime);
            if(!M)
            #if MEMORY == 0    
                times_times[i].tms_cstime =  yf;
            #else
                (times_timesp + i)->tms_cstime =  yf;
            #endif 
            //   catch Max and Min
            if(i)
            {
                if(Mcstimes_times < yf)
                    Mcstimes_times = yf;
                if(mcstimes_times > yf)
                    mcstimes_times = yf;
            }
            else
            {
                Mcstimes_times = yf;
                mcstimes_times = yf;
            } 
            // calculate moving average  and Welford variance

            moving_average_variance(&i, &yf, &Acstimes_times, &Vcstimes_times, 'f');
            
            yf =  (tms2.tms_cutime - tms1.tms_cutime);
            if(!M)
            #if MEMORY == 0
            times_times[i].tms_cutime =  yf;
            #else
                (times_timesp + i)->tms_cutime =  yf;
            #endif 
            //   catch Max and Min
            if(i)
            {
                if(Mcutimes_times < yf)
                    Mcutimes_times = yf;
                if(mcutimes_times > yf)
                    mcutimes_times = yf;
            }
            else
            {
                Mcutimes_times = yf;
                mcutimes_times = yf;
            } 
            // calculate moving average  and Welford variance

            moving_average_variance(&i, &yf, &Acutimes_times, &Vcutimes_times, 'f');         
        }
        //	finish calculate/save diffs        
        printf(MAG "\t\r%ld" RESET, i);
        if(fflush(stdout))
            printf(_("Detect fflush error %s for step %lu\n"), strerror(errno), i);                        
        if(S)
            if((slp = sleep(sleep_sec)) != 0)
                printf(_("Not sleep %usec on step %lu"), slp, i);
        i++;           
    } // end while(i<circles) 


free(executable);

// calculate averages and ..


printf("\r           \n");  // erase 10 digits of cycles
FILE *fptr = stdout;
if(f)
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) 
    {   
         printf(_("Detect getcwd error: %s\n"), strerror(errno));
        return 48;
    }   
    if(strlen(cwd) + strlen("/tt_results.txt") + 1 >  PATH_MAX)
    {
        printf(_("strlen(cwd) is %lu. Exit.\n"), strlen(cwd));
        return 49;
    }
    strcat(cwd, "/tt_results.txt");
    fptr = fopen(cwd, "a");
    if(fptr == NULL)
    {
        printf(_("Detect open file error: %s for %s\n"), strerror(errno), cwd);
        return 50;
    }
    printf(_("Write to file %s.\n"), cwd);
}
if(!H)  //hide arrays
{
    fprintf(fptr, "\n");
    if(P)
    {
    fprintf(fptr, _("\tclock_gettime"));
    if(a)
        fprintf(fptr, _("\tclock_gettime"));
    }
    if(s)
        fprintf(fptr, _("\ttimespec_get"));
    if(r)
        fprintf(fptr, _("\trusage_utime\trusage_stime"));
    if(g)
        fprintf(fptr, _("\tgettimeofday"));
    if(o)
        fprintf(fptr, _("\tCPU_Time(clock)"));    //https://www.gnu.org/software/libc/manual/html_node/CPU-Time.html#CPU-Time
    if(m)
        fprintf(fptr, _("\tTime"));
    if(t)
        fprintf(fptr, _("\tProcessor_Time(times)"));
        
    
    fprintf(fptr, _("\n#N"));
    if(P)
    {
        fprintf(fptr, _("\tsec.nsec"));
        if(a)
            fprintf(fptr, _("\tsec.nsec"));
    }
    if(s)
        fprintf(fptr, _("\tsec.nsec"));
    if(r)
        fprintf(fptr, _("\tsec.usec\tsec.usec")); 
    if(g)
        fprintf(fptr, _("\tsec.usec"));
    if(o)
        fprintf(fptr, _("\tsec"));
    if(m)
    {
        if(o)
            fprintf(fptr, _("\t\tsec"));
        else
            fprintf(fptr, _("\tsec"));
    }
    if(t)
        fprintf(fptr, _("\treal\tuser\tsystem\tcuser\tcsystem")); 
    fprintf(fptr,"\n\n");
    
    for(int j = 0; j<circles; j++)
    {
        #if MEMORY == 0
        if(P)
        {
            fprintf(fptr, "%i\t%lld.%09ld",j, (long long)clock_gettime_times[j].tv_sec, clock_gettime_times[j].tv_nsec);
            if(a)
                fprintf(fptr, "%i\t%lld.%09ld",j, (long long)clock_gettime_times1[j].tv_sec, clock_gettime_times1[j].tv_nsec);
        }
        if(s)
            fprintf(fptr, "%i\t%lld.%09ld",j, (long long)timespec_get_times[j].tv_sec, timespec_get_times[j].tv_nsec);
        if(r)
        {
            fprintf(fptr, "\t%ld.%06ld", (long int)getrusage_times[j].ru_utime.tv_sec, (long int)getrusage_times[j].ru_utime.tv_usec );   
            fprintf(fptr, "\t%ld.%06ld", (long int)getrusage_times[j].ru_stime.tv_sec, (long int)getrusage_times[j].ru_stime.tv_usec ); 
        }
        if(g)   
            fprintf(fptr, "\t%ld.%06ld", (long int)gettimeofday_times[j].tv_sec, (long int)gettimeofday_times[j].tv_usec);   
        if(o)
            fprintf(fptr, "\t%-9f", clock_times[j]/CLOCKS_PER_SEC);
        if(m)
            fprintf(fptr, "\t%ld", time_times[j]);
        if(t)
        {
            fprintf(fptr, "\t%g", tick_per_second==-1? (double)times_real_times[j]: (double)times_real_times[j]/tick_per_second);
            fprintf(fptr, "\t%g", tick_per_second==-1? (double)times_times[j].tms_utime : (double)times_times[j].tms_utime/tick_per_second ); //(intmax_t) if it is possible
            fprintf(fptr, "\t%g", tick_per_second==-1?(double)times_times[j].tms_stime : (double)times_times[j].tms_stime/tick_per_second );
            fprintf(fptr, "\t%g", tick_per_second==-1?(double)times_times[j].tms_cutime : (double)times_times[j].tms_cutime/tick_per_second);
            fprintf(fptr, "\t%g", tick_per_second==-1?(double)times_times[j].tms_cstime : (double)times_times[j].tms_cstime/tick_per_second); 
        }
        #else
        if(P)
        {
            fprintf(fptr, "%i\t%lld.%09ld",j, (long long)(clock_gettime_timesp + j)->tv_sec, (clock_gettime_timesp + j)->tv_nsec);
            if(a)
                fprintf(fptr, "%i\t%lld.%09ld",j, (long long)(clock_gettime_times1p + j)->tv_sec, (clock_gettime_times1p + j)->tv_nsec);
        }
        if(s)
            fprintf(fptr, "%i\t%lld.%09ld",j, (long long)(timespec_get_timesp + j)->tv_sec, (timespec_get_timesp + j)->tv_nsec);
        if(r)
        {
            fprintf(fptr, "\t%ld.%06ld", (long int)(getrusage_timesp + j)->ru_utime.tv_sec, (long int)(getrusage_timesp + j)->ru_utime.tv_usec );   
            fprintf(fptr, "\t%ld.%06ld", (long int)(getrusage_timesp + j)->ru_stime.tv_sec, (long int)(getrusage_timesp + j)->ru_stime.tv_usec ); 
        }
        if(g)   
            fprintf(fptr, "\t%ld.%06ld", (long int)(gettimeofday_timesp + j)->tv_sec, (long int)(gettimeofday_timesp + j)->tv_usec);   
        if(o)
            fprintf(fptr, "\t%-9f", *(clock_timesp + j)/CLOCKS_PER_SEC);
        if(m)
            fprintf(fptr, "\t%ld", *(time_timesp + j));
        if(t)
        {
            fprintf(fptr, "\t%g", tick_per_second==-1? (double)*(times_real_timesp + j): (double)*(times_real_timesp + j)/tick_per_second);
            fprintf(fptr, "\t%g", tick_per_second==-1? (double)(times_timesp + j)->tms_utime : (double)(times_timesp + j)->tms_utime/tick_per_second ); //(intmax_t) if it is possible
            fprintf(fptr, "\t%g", tick_per_second==-1?(double)(times_timesp + j)->tms_stime : (double)(times_timesp + j)->tms_stime/tick_per_second );
            fprintf(fptr, "\t%g", tick_per_second==-1?(double)(times_timesp + j)->tms_cutime : (double)(times_timesp + j)->tms_cutime/tick_per_second);
            fprintf(fptr, "\t%g", tick_per_second==-1?(double)(times_timesp + j)->tms_cstime : (double)(times_timesp + j)->tms_cstime/tick_per_second);           
        }
        #endif
        fprintf(fptr,"\n");   
    }  // finish for print array results

    if(P)
    {
        fprintf(fptr, "--------------------"); 
        if(a) 
            fprintf(fptr, "--------------------"); 
    }
    if(s) 
        fprintf(fptr, "--------------------"); 
    if(r)
        fprintf(fptr, "--------------------");
    if(g)
        fprintf(fptr, "--------------------");
    if(o)
        fprintf(fptr, "--------------------");
    if(m) 
        fprintf(fptr, "--------------------"); 
    if(t) 
        fprintf(fptr, "--------------------");
    
} //    finish H=hide arrays option
 
 fprintf(fptr, "\n");
 if(P)
 {
     fprintf(fptr, _("\n\tclock_gettime"));
     if(a)
         fprintf(fptr, _("\tclock_gettime"));
 }
 if(s)
     fprintf(fptr, _("\ttimespec_get"));
 if(r)
     fprintf(fptr, _("\trusage_utime\trusage_stime"));
 if(g)
     fprintf(fptr, _("\tgettimeofday"));
 if(o)
     fprintf(fptr, _("\tCPU_Time(clock)"));    //https://www.gnu.org/software/libc/manual/html_node/CPU-Time.html#CPU-Time
 if(m)
     fprintf(fptr, _("\tTime"));
 if(t)
     fprintf(fptr, _("\tProcessor_Time(times)"));
 
//  fprintf(fptr, _("\n\tC_MONOTONIC"));
//  if(a)
//      fprintf(fptr, _("\tC_PR_CPUTIME_ID"));
 
 fprintf(fptr, "\n");
 if(P)
 {
     fprintf(fptr, _("\tsec.nsec")); 
     if(a)
         fprintf(fptr, _("\tsec.nsec"));
 }
 if(s)
     fprintf(fptr, _("\tsec.nsec"));
 if(r)
     fprintf(fptr, _("\tsec.usec\tsec.usec")); 
 if(g)
     fprintf(fptr, _("\tsec.usec"));
 if(o)
     fprintf(fptr, _("\tsec"));
 if(m)
 {
     if(o)
         fprintf(fptr, _("\t\tsec"));
     else
         fprintf(fptr, _("\tsec"));
 }
 if(t)
     fprintf(fptr, _("\treal\tuser\tsystem\tcuser\tcsystem"));  
 fprintf(fptr,"\n\n");
 
     // start Max-Min
 fprintf(fptr,_("Max."));
 if(P)
 {
     fprintf(fptr, "\t%f", Mclock_gettime_times);
     if(a)
         fprintf(fptr, "\t%f", Mclock_gettime_times1);
 }
  if(s)
      fprintf(fptr, "\t%f", Mtimespec_get_times);
  if(r)
  {
      fprintf(fptr, "\t%f", Mugetrusage_times);   
      fprintf(fptr, "\t%f", Msgetrusage_times);
  }
  if(g)
      fprintf(fptr, "\t%f", Mgettimeofday_times);
  if(o)     
      fprintf(fptr, "\t%f", Mclock_times/CLOCKS_PER_SEC);
  if(m)
      fprintf(fptr, "\t%ld",  Mtime_times);
  if(t)
  {
      fprintf(fptr, "\t%g", tick_per_second==-1? (double)Mtimes_real_times: (double)Mtimes_real_times/tick_per_second);
      fprintf(fptr, "\t%g", tick_per_second==-1? (double)Mutimes_times : (double)Mutimes_times/tick_per_second ); //(intmax_t) if it is possible
      fprintf(fptr, "\t%g", tick_per_second==-1?(double)Mstimes_times : (double)Mstimes_times/tick_per_second );
      fprintf(fptr, "\t%g", tick_per_second==-1?(double)Mcutimes_times : (double)Mcutimes_times/tick_per_second);
      fprintf(fptr, "\t%g", tick_per_second==-1?(double)Mcstimes_times : (double)Mcstimes_times/tick_per_second);      
  }     
  
  fprintf(fptr,"\n");
  fprintf(fptr, _("Min."));
  if(P)
  {
      fprintf(fptr, "\t%f", mclock_gettime_times);
      if(a)
          fprintf(fptr, "\t%f", mclock_gettime_times1);
  }
  if(s)
      fprintf(fptr, "\t%f", mtimespec_get_times);
  if(r)
  {
      fprintf(fptr, "\t%f", mugetrusage_times);   
      fprintf(fptr, "\t%f", msgetrusage_times);
  }
  if(g)
      fprintf(fptr, "\t%f", mgettimeofday_times);
  if(o)     
      fprintf(fptr, "\t%f", mclock_times/CLOCKS_PER_SEC);
  if(m)
      fprintf(fptr, "\t%ld",  mtime_times);
  if(t)
  {
      fprintf(fptr, "\t%g", tick_per_second==-1? (double)mtimes_real_times: (double)mtimes_real_times/tick_per_second);
      fprintf(fptr, "\t%g", tick_per_second==-1? (double)mutimes_times : (double)mutimes_times/tick_per_second ); //(intmax_t) if it is possible
      fprintf(fptr, "\t%g", tick_per_second==-1?(double)mstimes_times : (double)mstimes_times/tick_per_second );
      fprintf(fptr, "\t%g", tick_per_second==-1?(double)mcutimes_times : (double)mcutimes_times/tick_per_second);
      fprintf(fptr, "\t%g", tick_per_second==-1?(double)mcstimes_times : (double)mcstimes_times/tick_per_second);      
  }    
  // finish Max-Min
  fprintf(fptr,"\n");
  // start averages
  fprintf(fptr, _("Exp."));
  if(P)
  {
      fprintf(fptr, "\t%f", Aclock_gettime_times);
      if(a)
          fprintf(fptr, "\t%f", Aclock_gettime_times1);
  }
  if(s)
      fprintf(fptr, "\t%f", Atimespec_get_times);
  if(r)
  {
      fprintf(fptr, "\t%f", Augetrusage_times);   
      fprintf(fptr, "\t%f", Asgetrusage_times);
  }
  if(g)
      fprintf(fptr, "\t%f", Agettimeofday_times);
 if(o)     
     fprintf(fptr, "\t%f", Aclock_times/CLOCKS_PER_SEC);
 if(m)
     fprintf(fptr, "\t%.2f",  ((int)(100 * Atime_times)) / 100.0);
 if(t) 
 {
     fprintf(fptr, "\t%.4f", tick_per_second==-1 ? ((int)(10000 * Atimes_real_times)) / 10000.0  : (((int)(10000 * (Atimes_real_times/tick_per_second))) / 10000.0));
     fprintf(fptr, "\t%.4f", tick_per_second==-1 ? ((int)(10000 * Autimes_times)) / 10000.0 : (((int)(10000 * (Autimes_times/tick_per_second))) / 10000.0));
     fprintf(fptr, "\t%.4f", tick_per_second==-1 ? ((int)(10000 * Astimes_times)) / 10000.0 : (((int)(10000 * (Astimes_times/tick_per_second))) / 10000.0));
     fprintf(fptr, "\t%.4f", tick_per_second==-1 ? ((int)(10000 * Acutimes_times)) / 10000.0 : (((int)(10000 * (Acutimes_times/tick_per_second))) / 10000.0));
     fprintf(fptr, "\t%.4f", tick_per_second==-1 ? ((int)(10000 * Acstimes_times)) / 10000.0 : (((int)(10000 * (Acstimes_times/tick_per_second))) / 10000.0));
 }
 // finish averages
 // start variance
 fprintf(fptr,"\n");
 fprintf(fptr, _("Var."));
 if(P)
 {
     fprintf(fptr, "\t%f", circles ==1 ? Vclock_gettime_times : Vclock_gettime_times/(circles - 1));
     if(a)
         fprintf(fptr, "\t%f", circles ==1 ? Vclock_gettime_times1 : Vclock_gettime_times1/(circles - 1));
 }
 if(s)     
     fprintf(fptr, "\t%f", circles ==1 ? Vtimespec_get_times : Vtimespec_get_times/(circles - 1));
 if(r)
 {
     fprintf(fptr, "\t%f", circles ==1 ? Vugetrusage_times : Vugetrusage_times/(circles - 1));
     fprintf(fptr, "\t%f", circles ==1 ? Vsgetrusage_times : Vsgetrusage_times/(circles - 1));
 }
 if(g)
     fprintf(fptr, "\t%f", circles ==1 ? Vgettimeofday_times : Vgettimeofday_times/(circles-1));
 if(o)
     fprintf(fptr, "\t%f", circles ==1 ? Vclock_times/CLOCKS_PER_SEC : Vclock_times/(CLOCKS_PER_SEC*(circles-1)));
 if(m)
     fprintf(fptr, "\t%.2f", circles ==1 ? ((int)(100 * Vtime_times)) / 100.0 : (((int)(100 * (Vtime_times/(circles-1)))) / 100.0));
 if(t)
 {
     if(tick_per_second==-1)
     {
         if(circles == 1)
         {
             fprintf(fptr, "\t%.4f", ((((int)(10000 * Vtimes_real_times)) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * Vutimes_times)) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * Vstimes_times)) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * Vcutimes_times)) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * Vcstimes_times)) / 10000.0)));
         }
         else
         {
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (Vtimes_real_times/(circles-1)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (Vutimes_times/(circles-1)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (Vstimes_times/(circles-1)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (Vcutimes_times/(circles-1)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (Vcstimes_times/(circles-1)))) / 10000.0)));
         }
     }
     else
     {
         if(circles == 1)
         {
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (Vtimes_real_times/(tick_per_second*tick_per_second)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (Vutimes_times/(tick_per_second*tick_per_second)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (Vstimes_times/(tick_per_second*tick_per_second)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (Vcutimes_times/(tick_per_second*tick_per_second)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (Vcstimes_times/(tick_per_second*tick_per_second)))) / 10000.0)));
         }
         else
         {
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (Vtimes_real_times/(tick_per_second*tick_per_second*(circles-1))))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (Vutimes_times/(tick_per_second*tick_per_second*(circles-1))))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (Vstimes_times/(tick_per_second*tick_per_second*(circles-1))))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (Vcutimes_times/(tick_per_second*tick_per_second*(circles-1))))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (Vcstimes_times/(tick_per_second*tick_per_second*(circles-1))))) / 10000.0)));
         }
     }
     
 }
 // finish variance
 
 // start std.dev
 fprintf(fptr,"\n");
 fprintf(fptr, _("Std.D"));
 if(P)
 {
     fprintf(fptr, "\t%f", circles ==1 ? square_root(Vclock_gettime_times, 0.000000000001) : square_root(Vclock_gettime_times/(circles - 1), 0.000000000001));
     if(a)
         fprintf(fptr, "\t%f", circles ==1 ? square_root(Vclock_gettime_times1, 0.000000000001) : square_root(Vclock_gettime_times1/(circles - 1), 0.000000000001));
 }
 if(s)
     fprintf(fptr, "\t%f", circles ==1 ? square_root(Vtimespec_get_times, 0.000000000001) : square_root(Vtimespec_get_times/(circles - 1), 0.000000000001));
 if(r)
 {
     fprintf(fptr, "\t%f", circles ==1 ? square_root(Vugetrusage_times, 0.000000000001) : square_root(Vugetrusage_times/(circles - 1), 0.000000000001));
     fprintf(fptr, "\t%f", circles ==1 ? square_root(Vsgetrusage_times, 0.000000000001) : square_root(Vsgetrusage_times/(circles - 1), 0.000000000001));
 }
 if(g)
     fprintf(fptr, "\t%f", circles ==1 ? square_root(Vgettimeofday_times, 0.000000000001) : square_root(Vgettimeofday_times/(circles-1), 0.000000000001));
 if(o)
     fprintf(fptr, "\t%f", circles ==1 ? square_root(Vclock_times/CLOCKS_PER_SEC, 0.000000000001) : square_root(Vclock_times/(CLOCKS_PER_SEC*(circles-1)), 0.000000000001));
 if(m)
     fprintf(fptr, "\t%.2f",  circles ==1 ? ((int)(100 * square_root(Vtime_times, 0.000000000001))) / 100.0 : ((int)(100 * square_root(Vtime_times/(circles-1), 0.000000000001))) / 100.0 );
  if(t)
 {
     if(tick_per_second==-1)
     {
         if(circles == 1)
         {
             fprintf(fptr, "\t%.4f", ((((int)(10000 * square_root(Vtimes_real_times, 0.000000000001))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * square_root(Vutimes_times, 0.000000000001))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * square_root(Vstimes_times, 0.000000000001))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * square_root(Vcutimes_times, 0.000000000001))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * square_root(Vcstimes_times, 0.000000000001))) / 10000.0)));
         }
         else
         {
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (square_root(Vtimes_real_times/(circles-1), 0.000000000001)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (square_root(Vutimes_times/(circles-1), 0.000000000001)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (square_root(Vstimes_times/(circles-1), 0.000000000001)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (square_root(Vcutimes_times/(circles-1), 0.000000000001)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (square_root(Vcstimes_times/(circles-1), 0.000000000001)))) / 10000.0)));
         }
     }
     else
     {
         if(circles == 1)
         {
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (square_root(Vtimes_real_times/(tick_per_second*tick_per_second), 0.000000000001)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (square_root(Vutimes_times/(tick_per_second*tick_per_second), 0.000000000001)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (square_root(Vstimes_times/(tick_per_second*tick_per_second), 0.000000000001)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (square_root(Vcutimes_times/(tick_per_second*tick_per_second), 0.000000000001)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (square_root(Vcstimes_times/(tick_per_second*tick_per_second), 0.000000000001)))) / 10000.0)));
         }
         else
         {
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (square_root(Vtimes_real_times/(tick_per_second*tick_per_second*(circles-1)), 0.000000000001)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (square_root(Vutimes_times/(tick_per_second*tick_per_second*(circles-1)), 0.000000000001)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (square_root(Vstimes_times/(tick_per_second*tick_per_second*(circles-1)), 0.000000000001)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (square_root(Vcutimes_times/(tick_per_second*tick_per_second*(circles-1)), 0.000000000001)))) / 10000.0)));
             fprintf(fptr, "\t%.4f", ((((int)(10000 * (square_root(Vcstimes_times/(tick_per_second*tick_per_second*(circles-1)), 0.000000000001)))) / 10000.0)));
         }
     }
     
 }
 fprintf(fptr, "\n");
 if(f && (fptr != NULL))     
     fclose(fptr);
 // finish std.dev  
 printf("\n");

}   // finish w	
#if MEMORY == 1
free(clock_timesp);
free(gettimeofday_timesp);
free(clock_gettime_timesp);
free(clock_gettime_times1p);
free(timespec_get_timesp);
free(getrusage_timesp);
free(times_timesp);
free(times_real_timesp);
free(time_timesp);
#endif
if(segv_stack.ss_sp != NULL)
    free(segv_stack.ss_sp);
if (gettimeofday( &t02, NULL ) == -1)
{
    printf(_("Detect gettimeofday error: %s"), strerror(errno));
    return 19;
}
if(!(tm2 = localtime ( &t02.tv_sec )))
{
    printf(_("Detect localtime error: %s"), strerror(errno));
    return 20;
}
tv3 = timeval_diff(t01, t02);
printf(_("\nFinish working %d:%02d:%02d:%ld.\nElapsed %.0f:%06li seconds and %lu cycles"), tm2->tm_hour, tm2->tm_min, tm2->tm_sec, t02.tv_usec, (double)tv3.tv_sec, tv3.tv_usec, circles);
if(S)
    printf(_(", Sleeping time %lu sec.\n\n"), sleep_sec*circles);
else
    printf(".\n\n");


return 0;
}

