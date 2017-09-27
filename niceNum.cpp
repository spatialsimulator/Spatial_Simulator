/*
 * Nice Numbers for Graph Labels
 * by Paul Heckbert
 * from "Graphics Gems", Academic Press, 1990
 */

/*
 * label.c: demonstrate nice graph labeling
 *
 * Paul Heckbert    2 Dec 88
 */
#include "spatialsim/niceNum.h"

using namespace std;

/*
 * loose_label: demonstrate loose labeling of data range from min_value to max_value.
 * (tight method is similar)
 * Following values will be assigned with this function.
 *   double d : tick mark spacing
 *   double graphmax : graph range max
 */
void loose_label(double min_value, double max_value, double& d, double& graphmax) {
  double range;

  /* we expect min_value != max_value */
  range = nicenum(max_value - min_value, 0);
  d = nicenum(range/(NTICK-1), 1);
  //double graphmin = floor(min_value/d)*d;
  graphmax = ceil(max_value/d)*d;
}

/*
 * nicenum: find a "nice" number approximately equal to x.
 * Round the number if round=1, take ceiling if round=0
 */
double nicenum(double x, int round) {
  int expv;                /* exponent of x */
  double f;                /* fractional part of x */
  double nf;                /* nice, rounded fraction */

  expv = floor(log10(x));
  f = x/pow(10., expv);        /* between 1 and 10 */
  if (round)
    if (f<1.5) nf = 1.;
    else if (f<3.) nf = 2.;
    else if (f<7.) nf = 5.;
    else nf = 10.;
  else
    if (f<=1.) nf = 1.;
    else if (f<=2.) nf = 2.;
    else if (f<=5.) nf = 5.;
    else nf = 10.;
  return nf*pow(10., expv);
}
