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

#if 0

int genring2d(int cx, int cy, int nx, int ny, real* dx, real* dy,
	real rmin, real rmax, real* u, real* v, /** OUT **/
	real scaling, int uniform)
{
	real rmin2 = rmin * rmin;
	real rmax2 = rmax * rmax;

	if (uniform)
	{
		// Left X side.
		real dxsum = 0;
		for (int ix = cx; (ix >= 0) && (dxsum <= rmax); ix--)
		{
			dxsum += dx[ix];		
			real dxsum2 = dxsum * dxsum;

			// Upper Y side.
			real dysum = 0;
			real rymin = 0;
			if (dxsum < rmin) rymin = sqrt(rmin2 - dxsum2);
			real rymax = sqrt(rmax2 - dxsum2);
			for (int iy = cy; (iy >= 0) && (dysum <= rymax); iy--)
			{
				dysum += dy[iy];

				if (dysum <= rymin) continue;

				real dysum2 = dysum * dysum;
				real radius = sqrt(dxsum2 + dysum2);
			
				u[ix + iy * nx] =  dysum / radius * scaling;
				v[ix + iy * nx] = -dxsum / radius * scaling;
			}

			// Lower Y side.
			dysum = 0;
			for (int iy = cy; (iy <= ny) && (dysum <= rymax); iy++)
			{
				dysum += dy[iy];

				if (dysum <= rymin) continue;

				real dysum2 = dysum * dysum;
				real radius = sqrt(dxsum2 + dysum2);
			
				u[ix + iy * nx] = -dysum / radius * scaling;
				v[ix + iy * nx] = -dxsum / radius * scaling;
			}
		}

		// Right X side.
		dxsum = 0;
		for (int ix = cx; (ix >= 0) && (dxsum <= rmax); ix++)
		{
			dxsum += dx[ix];
			real dxsum2 = dxsum * dxsum;

			// Upper Y side.
			real dysum = 0;
			real rymin = 0;
			if (dxsum < rmin) rymin = sqrt(rmin2 - dxsum2);
			real rymax = sqrt(rmax2 - dxsum2);
			for (int iy = cy - 1; (iy >= 0) && (dysum <= rymax); iy--)
			{
				real dysum2 = dysum * dysum;
				dysum += dy[iy];

				if (dysum <= rymin) continue;

				real radius = sqrt(dxsum2 + dysum2);
			
				u[ix + iy * nx] = dysum / radius * scaling;
				v[ix + iy * nx] = dxsum / radius * scaling;
			}

			// Lower Y side.
			dysum = 0;
			for (int iy = cy + 1; (iy <= ny) && (dysum <= rymax); iy++)
			{
				real dysum2 = dysum * dysum;
				dysum += dy[iy];

				if (dysum <= rymin) continue;

				real radius = sqrt(dxsum2 + dysum2);
						
				u[ix + iy * nx] = -dysum / radius * scaling;
				v[ix + iy * nx] =  dxsum / radius * scaling;
			}
		}
	}
	else
	{
		// Left X side.
		real dxsum = 0;
		for (int ix = cx; (ix >= 0) && (dxsum <= rmax); ix--)
		{
			dxsum += dx[ix];		
			real dxsum2 = dxsum * dxsum;

			// Upper Y side.
			real dysum = 0;
			real rymin = 0;
			if (dxsum < rmin) rymin = sqrt(rmin2 - dxsum2);
			real rymax = sqrt(rmax2 - dxsum2);
			for (int iy = cy; (iy >= 0) && (dysum <= rymax); iy--)
			{
				dysum += dy[iy];

				if (dysum <= rymin) continue;

				real dysum2 = dysum * dysum;
				real radius = sqrt(dxsum2 + dysum2);
			
				u[ix + iy * nx] =  dysum / rmin * scaling;
				v[ix + iy * nx] = -dxsum / rmin * scaling;
			}

			// Lower Y side.
			dysum = 0;
			for (int iy = cy; (iy <= ny) && (dysum <= rymax); iy++)
			{
				dysum += dy[iy];

				if (dysum <= rymin) continue;

				real dysum2 = dysum * dysum;
				real radius = sqrt(dxsum2 + dysum2);
			
				u[ix + iy * nx] = -dysum / rmin * scaling;
				v[ix + iy * nx] = -dxsum / rmin * scaling;
			}
		}

		// Right X side.
		dxsum = 0;
		for (int ix = cx; (ix >= 0) && (dxsum <= rmax); ix++)
		{
			dxsum += dx[ix];
			real dxsum2 = dxsum * dxsum;

			// Upper Y side.
			real dysum = 0;
			real rymin = 0;
			if (dxsum < rmin) rymin = sqrt(rmin2 - dxsum2);
			real rymax = sqrt(rmax2 - dxsum2);
			for (int iy = cy; (iy >= 0) && (dysum <= rymax); iy--)
			{
				real dysum2 = dysum * dysum;
				dysum += dy[iy];

				if (dysum <= rymin) continue;

				real radius = sqrt(dxsum2 + dysum2);
			
				u[ix + iy * nx] = dysum / rmin * scaling;
				v[ix + iy * nx] = dxsum / rmin * scaling;
			}

			// Lower Y side.
			dysum = 0;
			for (int iy = cy; (iy <= ny) && (dysum <= rymax); iy++)
			{
				real dysum2 = dysum * dysum;
				dysum += dy[iy];

				if (dysum <= rymin) continue;

				real radius = sqrt(dxsum2 + dysum2);
						
				u[ix + iy * nx] = -dysum / rmin * scaling;
				v[ix + iy * nx] =  dxsum / rmin * scaling;
			}
		}
	}
}

#endif

