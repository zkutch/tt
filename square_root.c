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
