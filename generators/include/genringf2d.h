#pragma once

#include "smolar.h"

// Generate the (u,v) 2d faded ringed flow with specified center, inner and outer
// radiuses, scaling and fade out to 0. Uniform flag indicates if flow in every
// point must have the same module.
int genringf2d(int cx, int cy, int nx, int ny, real* dx, real* dy,
	real rmin, real rmax, real* u, real* v, /** OUT **/
	real scaling, int uniform);

