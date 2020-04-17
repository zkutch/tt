/* Calculate moving_average_variance for different types   
   Copyright (C) 2019, 2020 Zurab Z.Kutchava

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

/* Written by Zurab Z.Kutchava.  */

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
