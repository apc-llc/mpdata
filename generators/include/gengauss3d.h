#pragma once

#include "smolar.h"

// Build 3d Gauss function.
int gengauss3d(int cx, int cy, int cs, int nx, int ny, int ns,
	real radius, real sigmax, real sigmay, real sigmas, real* gauss, /** OUT **/
	real scaling);

