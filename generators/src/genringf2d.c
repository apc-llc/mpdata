#include "genringf2d.h"
#include "genring2d.h"

#include <math.h>

// Generate the (u,v) 2d faded ringed flow with specified center, inner and outer
// radiuses, scaling and fade out to 0. Uniform flag indicates if flow in every
// point must have the same module.
int genringf2d(int cx, int cy, int nx, int ny, real* dx, real* dy,
	real rmin, real rmax, real* u, real* v, /** OUT **/
	real scaling, int uniform)
{
	genring2d(cx, cy, nx, ny, dx, dy, rmin, rmax, u, v, scaling, uniform);

	if (uniform)
	{
		for (int iy = 0; iy < ny; iy++)
			for (int ix = 0; ix < nx; ix++)
			{
				real coordx = ix - cx;
				real coordy = iy - cy;
				real radius = sqrt(coordx * coordx + coordy * coordy);

				if (radius > rmax)
				{
					u[ix + nx * iy] = -coordy / radius * dx[ix] * exp((rmax - radius) * dx[ix]);
					v[ix + nx * iy] =  coordx / radius * dy[iy] * exp((rmax - radius) * dy[iy]);
				}
			}
	}
	else
	{
		for (int iy = 0; iy < ny; iy++)
			for (int ix = 0; ix < nx; ix++)
			{
				real coordx = ix - cx;
				real coordy = iy - cy;
				real radius = sqrt(coordx * coordx + coordy * coordy);

				if (radius > rmax)
				{
					u[ix + nx * iy] = -coordy / rmin * dx[ix] * exp((rmax - radius) * dx[ix]);
					v[ix + nx * iy] =  coordx / rmin * dy[iy] * exp((rmax - radius) * dy[iy]);
				}
			}
	}

	// Normalization (can unclude directioning).
	if (scaling != 1.0)
	{
		for (int iy = 0; iy < ny; iy++)
			for (int ix = 0; ix < nx; ix++)
			{
				real coordx = ix - cx;
				real coordy = iy - cy;
				real radius = sqrt(coordx * coordx + coordy * coordy);

				if (radius > rmax)
				{
					u[ix + nx * iy] *= scaling;
					v[ix + nx * iy] *= scaling;
				}
			}
	}

	return 0;
}

