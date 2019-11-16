#ifndef UTILS
#define UTILS

#include <stdlib.h>
#include <time.h>
#include <math.h>

#define FALSE 0
#define TRUE !FALSE

typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

double randomValue(double low, double high);
rgb getRandomRgbColor(double saturation, double value);
static hsv rgb2hsv(rgb in);
static rgb hsv2rgb(hsv in);

#endif