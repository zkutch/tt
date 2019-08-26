//  https://www.johndcook.com/blog/standard_deviation/
// Knuth TAOCP vol 2, 3rd edition, page 232


#include "tt.h"

void moving_average_variance(size_t * i, void * x, void * A, void * V, char type)
{
    double OldA ;
    float olda;
    
    switch(type)
    {
         case 'f':   // float
            if(*i)
            {                    
                olda = *(float*)A;
                *(float*)A +=  (*(float*)x - *(float*)A)/(*i+1);        
                *(float*)V += (*(float*)x - olda)*(*(float*)x - *(float*)A);
            }
            else
            {                             
                *(float*)A = *(float*)x;
                *(float*)V = 0;
            }
            break;
        case 'd':   // double
            if(*i)
            {
                OldA = *(double*)A;
                *(double*)A +=  (*(double*)x - *(double*)A)/(*i+1);        
                *(double*)V += (*(double*)x - OldA)*(*(double*)x - *(double*)A);
            }
            else
            {        
                *(double*)A = *(double*)x;
                *(double*)V = 0;
            }
            break;  
        default:
            printf("moving_average_variance: illegal option %c\n", type);                                                           
            break;
    }
        
    
}
