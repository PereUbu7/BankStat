#include "utils.h"

rgb getRandomRgbColor(double saturation, double value)
{
    static int firstCall = TRUE;
    static double hue = 0.0;
    hsv randomColor;

    if(firstCall)
    {
        hue = randomValue(0.0, 359.9);
        firstCall = FALSE;
    }
    else
    {
        hue += 37;
    }

    /* Make sure hue is 0 <= hue <= 360 */
    while(hue >= 360)
        hue -= 360;
    
    randomColor.h = hue;
    randomColor.s = saturation;
    randomColor.v = value;
    
    return hsv2rgb(randomColor);
}

double randomValue(double low, double high)
{
    time_t t;
    double randV;
    static int firstInvokation = TRUE;
    int quotient;
    double modulo;

    if(firstInvokation)
    {
        srand((unsigned)time(&t));
        firstInvokation = FALSE;
    }

    randV = (double)rand();

    quotient = (int)(randV / (high - low));
    modulo = randV - quotient * (high - low);
    

    return modulo + low;
}

hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) 
    { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } 
    else 
    {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = -9999;
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) 
    {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) 
    {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}