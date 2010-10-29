#include "gengauss2d.h"

#include <math.h>

// Build 2d Gauss function.
int gengauss2d(int cx, int cy, int nx, int ny,
	real radius, real sigmax, real sigmay, real* gauss, /** OUT **/
	real scaling)
{
	real sigmax2 = sigmax * sigmax;
	real sigmay2 = sigmay * sigmay;

	for (int iy = 0; iy < ny; iy++)
		for (int ix = 0; ix < nx; ix++)
		{
			int dx = ix - cx, dy = iy - cy;
			real dx2 = dx * dx, dy2 = dy * dy;
			
			if (sqrt(dx2 + dy2) <= radius)
				gauss[ix + nx * iy] =
					exp(-0.5 * (dx2 / sigmax2 + dy2 / sigmay2));
		}

	if (scaling != 1.0)
	{
		// Multiply by the scale factor.
		const unsigned int np = nx * ny;
		for (unsigned int ip = 0; ip < np; ip++)
			gauss[ip] *= scaling;
	}
	
	return 0;
}

