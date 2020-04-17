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


//  https://en.wikipedia.org/wiki/Halley%27s_method
//  http://www.netlib.org/fdlibm/
#include <stdio.h>

double square_root(double var, float e)
{
    double initial = 1;
    double result = 0;
    float diff=1.0;
    while (diff > e)
    {
        //         result = (initial*initial*initial + 3*var*result)/(3*result*result + var);
//         result = 0.5*(result + initial/result);
//         initial = result;
//         i++;
        result=(initial+var/initial)/2;
        diff=result-initial;
        if (diff<0)
            diff=-diff;
        initial=result;
    }
    return result;
    
}
