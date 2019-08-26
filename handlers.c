#include "tt.h"

void signal_handler(int sig)
{    
    write(STDOUT_FILENO, "\nIn signal_handler", 18);
    if (sig == SIGINT)
        write(STDOUT_FILENO, ": SIGINT\n", 9);    
    longjmp(point, 1);
} 
void sigaction_handler(int sig, siginfo_t *info, void *context)
{
    write(STDOUT_FILENO, "\nIn sigaction_handler", 21);    
    if (sig == SIGSEGV)
        write(STDOUT_FILENO, ": SIGSEGV\n", 10);
    if (sig == SIGBUS)
        write(STDOUT_FILENO, ": SIGBUS\n", 9);
    write(STDOUT_FILENO, "Consider decreasing cycles amount.\n", 35);
    longjmp(point, 1);
}
