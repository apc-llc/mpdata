#pragma once

#include "smolar.h"

/**
 * Generate the (u,v) 2d ringed velocity field with specified center,
 * inner and outer radiuses and scaling. Uniform flag indicates if
 * ring in every point must have the same module.
 * @param cx - The ring center point X coordinate
 * @param cy - The ring center point Y coordinate
 * @param nx - The grid X dimension
 * @param ny - The grid Y dimension
 * @param dx - The grid X step
 * @param dy - The grid Y step
 * @param rmin - The ring minimum radius
 * @param rmax - The ring maximum radius
 * @param u - The velocity field X component; will be filled on exit
 * @param v - The velocity field Y component; will be filled on exit
 * @param scaling - The scaling factor
 * @param uniform - The uniform/non-uniform mode flag
 * @return The 0 value on successful run or an error code
 */
int genring2d(int cx, int cy, int nx, int ny, real* dx, real* dy,
	real rmin, real rmax, real* u, real* v, /** OUT **/
	real scaling, int uniform);

