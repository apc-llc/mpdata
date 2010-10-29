#pragma once

#include "smolar.h"

// Build 2d Gauss function.
int gengauss2d(int cx, int cy, int nx, int ny,
	real radius, real sigmax, real sigmay, real* gauss, /** OUT **/
	real scaling);

