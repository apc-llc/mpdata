#include "gengauss2d.h"

#include <math.h>

// Build 2d Gauss function.
int gengauss3d(int cx, int cy, int cs, int nx, int ny, int ns,
	real radius, real sigmax, real sigmay, real sigmas, real* gauss, /** OUT **/
	real scaling)
{
	real sigmax2 = sigmax * sigmax;
	real sigmay2 = sigmay * sigmay;
	real sigmas2 = sigmas * sigmas;
	
	int np = nx * ny;

	for (int is = 0; is < ns; is++)	
		for (int iy = 0; iy < ny; iy++)
			for (int ix = 0; ix < nx; ix++)
			{
				int dx = ix - cx, dy = iy - cy, ds = is - cs;
				real dx2 = dx * dx, dy2 = dy * dy, ds2 = ds * ds;
			
				if (sqrt(dx2 + dy2 + ds2) <= radius)
					gauss[ix + nx * iy + np * is] = exp(-0.5 * (
						dx2 / sigmax2 +
						dy2 / sigmay2 +
						ds2 / sigmas2));
			}

	if (scaling != 1.0)
	{
		// Multiply by the scale factor.
		const unsigned int nv = np * ns;
		for (unsigned int iv = 0; iv < nv; iv++)
			gauss[iv] *= scaling;
	}
	
	return 0;
}

