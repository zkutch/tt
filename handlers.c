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
