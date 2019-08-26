


int check_if_number(char * number)
{  
  char * x = number;  
  while ( *x != '\0')
    {
        if (*x<'0' || *x>'9')  
            return 1;        
        x++;
    }    
    return 0;
}
