#include "genring2d.h"

#include <math.h>

// Generate the (u,v) 2d ringed flow with specified center, inner and outer radiuses
// and scaling. Uniform flag indicates if flow in every point must have the same
// module.
int genring2d(int cx, int cy, int nx, int ny, real* dx, real* dy,
	real rmin, real rmax, real* u, real* v, /** OUT **/
	real scaling, int uniform)
{
	if (uniform)
	{
		for (int iy = 0; iy < ny; iy++)
			for (int ix = 0; ix < nx; ix++)
			{
				real coordx = (int)ix - (int)cx;  
				real coordy = (int)iy - (int)cy;
				real radius = sqrt(coordx * coordx + coordy * coordy);
				
				if ((radius >= rmin) && (radius <= rmax))
				{
					u[ix + iy * nx] = -coordy / radius * dx[ix]; 
					v[ix + iy * nx] =  coordx / radius * dy[iy]; 
				}
			}
	}
	else
	{
		for (int iy = 0; iy < ny; iy++)
			for (int ix = 0; ix < nx; ix++)
			{
				real coordx = (int)ix - (int)cx;
				real coordy = (int)iy - (int)cy;
				real radius = sqrt(coordx * coordx + coordy * coordy);

				if ((radius >= rmin) && (radius <= rmax))
				{
					u[ix + iy * nx] = -coordy / rmin * dx[ix];
					v[ix + iy * nx] =  coordx / rmin * dy[iy];
				}
			}
	}

	// Normalization (can unclude directioning).
	if (scaling != 1.0)
	{
		for (int iy = 0; iy < ny; iy++)
			for (int ix = 0; ix < nx; ix++)
			{
				real coordx = (int)ix - (int)cx;
				real coordy = (int)iy - (int)cy;
				real radius = sqrt(coordx * coordx + coordy * coordy);

				if ((radius >= rmin) && (radius <= rmax))
				{			
					u[ix + iy * nx] *= scaling;
					v[ix + iy * nx] *= scaling;
				}
			}
	}

	return 0;
}

