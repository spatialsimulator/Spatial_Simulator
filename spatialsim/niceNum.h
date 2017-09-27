#ifndef NICENUM_H_
#define NICENUM_H_

#include <cfenv>
#include <cmath>
#include <iostream>

#define NTICK 5            /* desired number of tick marks */

void loose_label(double min_value, double max_value, double& d, double& graphmax);

double nicenum(double x, int round);

#endif
