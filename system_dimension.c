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

void  system_dimension(void)
{  
    printf("PATH_MAX = %d\n", PATH_MAX);
    printf("The sizeof of clock_t = %lu\n", sizeof(clock_t)); 
    printf("The sizeof of int = %lu\n", sizeof(int));
    printf("The sizeof of double = %lu\n", sizeof(double));
    printf("The sizeof of __time_t = %lu\n", sizeof(__time_t));
    printf("CLOCKS_PER_SEC %lu\n", CLOCKS_PER_SEC);
    printf("clock ticks per second _SC_CLK_TCK = %ld\n", sysconf (_SC_CLK_TCK)); 
    printf("The sizeof of char = %lu\n", sizeof(char));
    printf("The sizeof of unsigned char = %lu\n", sizeof(unsigned char));
    printf("The sizeof of boolean = %lu\n", sizeof(_Bool));
    printf("The sizeof of unsigned short = %lu\n", sizeof(unsigned short));
    printf("The sizeof of unsigned long = %lu\n", sizeof(unsigned long));
    printf("The sizeof of unsigned long long = %lu\n", sizeof(unsigned long long));
    unsigned int ul = -1;
    printf("The maximum value of unsigned int = %u\n", ul);  //2**32-1 = 4294967295  
    unsigned long ull =-1;
    printf("The maximum value of unsigned long = %lu\n", ull); // 2**64-1 = 18446744073709551615

    unsigned long long mmu = ~0;
    unsigned int mu = ~0;
    if (mmu == ull)
    {
        printf("Representation of unsigned long is %lu together with -1\n", ull);
    }
    else
    {
        printf("Representation of unsigned long is not %lu together with -1, but is %llu\n", ull, mmu);
    }
    if (mu == ul)
    {
        printf("Representation of unsigned int is %u together with -1\n", ul);
    }
    else
    {
        printf("Representation of unsigned int is not %u together with -1, but is %u\n", ul, mu);
    }
}
