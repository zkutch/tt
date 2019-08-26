
#include <sys/ioctl.h>
#include "tt.h"

#define underline "\033[4m"
//  #define reset_underline "\033[0m"
//  #define bold "\e[1m"    
#define bold "\033[1m" // {0x1b, 0x5b, 0x31, 0x6d, 0x0}
//  #define italic "\e[3m"
//  #define strikethrough "\e[9m"
//  #define reset "\e[0m"
#define reset "\033[0m"     //  {0x1b, 0x5b, 0x30, 0x6d, 0x0}


short count_substrings( char * string, char * substring);

void help(void)
{
    struct winsize w;
    unsigned short width = 80;
    
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w))    
        printf("ioctl error: %s. Assumed width %d\n", strerror(errno), width);
    else
       width  = w.ws_col;    
    width  -=20;
    if(width <= 0)
    {
        printf("Terminal width is less then 20. Consider increasing, please.\n");
        _exit(75);
    }

    printf(bold "tt\n" reset);    
    string_2_column(width, "tt - measures the running time of the selected " bold "application" reset ".");
    printf(bold "\nDESCRIPTION\n" reset);    
    string_2_column(width, bold "tt" reset " runs chosen " bold "application" reset " with absolute " underline "path" reset ", serially, " underline "amount" reset " times. For each run, it is possible to save a separate execution time. For the entire series the maximum and minimum time, average time, variance and standard deviation are calculated. Time is measured using 7 standard functions: clock_gettime, time, clock, gettimeofday, timespec_get, times, getrusage. If defined _POSIX_TIMERS, then " bold "tt" reset " for clock_gettime function selects CLOCK with lowest available resolution in first place.");
    
    printf(bold "\nSYNOPSIS\n" reset);
    string_2_column(width, bold "tt " reset "[" bold "-arqgtomspHfM" reset "] [" bold "-S " reset underline "seconds" reset  "] [" bold "-b " reset  underline "path" reset  "] ["  bold "-c " reset  underline "amount" reset "]");
    printf( bold "\ttt " reset "[" bold "-R" reset "] [" bold "-d " reset "] [" bold "-R" reset "] [" bold "-V " reset "] [" bold "-h" reset "]\n");
    
    printf(bold "\nOPTIONS\n" reset);
    string_2_column(width, bold "-a" reset "\tfor second clock_gettime selects CLOCK_PROCESS_CPUTIME_ID.");
    string_2_column(width,bold "\n\t-r" reset "\tincludes function getrusage.\n");
    string_2_column(width,bold "-q" reset "\tquiet - hides standard output for " bold "application" reset ".\n");
    string_2_column(width,bold "-g" reset "\tincludes function gettimeofday.\n");
    string_2_column(width,bold "-t" reset "\tincludes function times.\n");
    string_2_column(width,bold "-o" reset "\tincludes function clock.\n");
    string_2_column(width,bold "-m" reset "\tincludes function time.\n");
    string_2_column(width,bold "-s" reset "\tincludes function timespec_get.\n");
    string_2_column(width,bold "-p" reset "\tforce posix_spawn to run " bold "application" reset ", by default is used fork/exec.\n");
    string_2_column(width,bold "-H" reset "\thide intermediate results for each, " bold "application" reset " run, implies -M.\n");
    string_2_column(width,bold "-f" reset "\tprints to file “tt_results.txt” in current directory.\n");
    string_2_column(width,bold "-M" reset "\tnot keep intermediate results\n");
    string_2_column(width,bold "-S " reset underline "seconds"reset);
    string_2_column(width,"\tsleep " underline "seconds" reset " between each iteration.\n");
    string_2_column(width,bold "-b " reset underline "path"reset);
    string_2_column(width,"\tabsolute " underline "path" reset " for " bold "application.\n" reset);
    string_2_column(width,bold "-c " reset underline "amount"reset);
    string_2_column(width,"\trun " bold "application " reset underline "amount"reset " times.\n");
    string_2_column(width,bold "-R" reset "\tshow resolution for some clocks.\n");
    string_2_column(width,bold "-d" reset "\tshow size limits for some types.\n");
    string_2_column(width,bold "-V" reset "\tVersion.\n");
    string_2_column(width,bold "-h" reset "\thelp.\n");
    
    printf(bold "OTHER\n" reset);
    string_2_column(width, "It is possible to save intermediate results either in the " bold "tt" reset " stack or in the heap. By default "  bold "tt" reset " use stack. For heap, before compiling, it should be configured with option --enable-MEMORY.\n");
    
    printf(bold "\nAUTHOR/REPORTING BUGS\n" reset);
    printf("\tzkutch@yahoo.com\n\n");
    
    printf(bold "COPYRIGHT\n" reset);
    printf("\tMIT License\n\n");
    
    printf(bold "EXAMPLES\n" reset);
    string_2_column(width,bold "tt -b /usr/bin/find -c 30 -q -a -S 1 -m -t -g -r -s -o\n" reset);
    string_2_column(width, "runs " bold "application" reset " find  in  current directory 30 times, hide standard output for it, use second clock_gettime, time, times, gettimeofday, getrusage, timespec_get and clock functions for measuring time. Sleep 1 second between each run.");
    
    printf(bold "\nSEE ALSO\n" reset);
    printf("\ttime(1)\n"); 
    
    printf(bold "\nBUGS\n" reset);
    printf("\tYet none known\n");
                    
    puts("\n");
    

}


void string_2_column(unsigned short width, char * middle)
{    
    char style[] = "\033["; // {0x1b, 0x5b, 0x0}
    int style_len = strlen(style);
    short jj;
    unsigned short middle_len = strlen(middle);
    jj= count_substrings(middle, style);
   
    if(jj == 0) // keep mostly for history, as jj>0 works always
    {        
        unsigned short rows = middle_len/width;
        unsigned short rest = middle_len%width;
        char * working_string = malloc((width + 2 + 1)*sizeof(char));
        
        if(working_string == NULL)
            printf("Printing malloc error.\n");
        else
        {
            int i = 0;            
            for(i=0; i< rows; i++)
            {               
                strncpy(working_string, "\t\0", 2);
                strncat(working_string, (char*)(middle + i*width), width);
                strncat(working_string, "\n", 1);
                printf("%s", working_string);
            }                   
            if(rest > 0)
            {        
                strncpy(working_string, "\t\0", 2);
                strncat(working_string, (char*)(middle + i*width), rest);
                printf("%s", working_string);
            }
            
            if(working_string != NULL)
                free(working_string); 
        }
    }
    // find amount of "\033[" in chosen segment of middle and calculate correction
    else if(jj > 0)     // TODO hyphenation
    {
        unsigned short i = 1;
        unsigned short j = 0;        
        while((char)*(middle + i) != '\0' && i <= middle_len )
        {
            
            j = 0;
            unsigned short m = width;
            
            printf("\t");
            while((char)*(middle + j) != '\0' && j <= middle_len && j < m)
            {
               if((middle + j)[0] != style[0])
               {
                   putchar((char)*(middle + j));
                   j++;
                   i++;
               }
               else
               {
                  if(strncmp(style, middle + j, style_len) == 0)
                  {
                      for(int k = 0; k <= style_len + 1; k++)
                          putchar((char)*(middle + j + k));
                      j += style_len + 2;
                      i += style_len + 2;
                      m += (style_len + 2);
                  }
                  else
                  {
                      putchar((char)*(middle + j));
                      j++;
                      i++;
                  }
               }         
            }
            middle += (j);
            puts("");
        }
    }
    else
        printf("count_substrings error.\n");
    
}

 short count_substrings( char * string, char * substring)
{
    if(strlen(substring) > strlen(string))
        return -1;
    unsigned short count = 0;
    char * big = string;
    char * little = substring;
    while(big != '\0')
    {
        if((big = strstr(big, little)))
        {
            count++;
            big+=strlen(little);
        }
    }    
    return count;
}
