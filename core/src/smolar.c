#include "smolar.h"
#include "smolar_pass.h"

// Copy 3d array boundaries.
void bndcpy(int nx, int ny, int ns, real* to, real* from)
{
	real *fromPtr = from, *toPtr = to;

	for (int j = 0; j < ny; j++)
		for (int i = 0; i < nx; i++)
			*toPtr++ = *fromPtr++;

	for (int k = 1; k < ns - 1; k++)
	{
		for (int i = 0; i < nx; i++)
			*toPtr++ = *fromPtr++;
			
		for (int j = 1; j < ny - 1; j++)
		{
			*toPtr = *fromPtr; toPtr += nx - 1; fromPtr += nx - 1;
			*toPtr = *fromPtr; toPtr++; fromPtr++;
		}

		for (unsigned int i = 0; i < nx; i++)
			*toPtr++ = *fromPtr++;
	}

	for (unsigned int j = 0; j < ny; j++)
		for (unsigned int i = 0; i < nx; i++)
			*toPtr++ = *fromPtr++;
}

int smolar(int nx, int ny, int ns,
	real* dx, real* dy, real* ds, const real dt,
	real* u0, real* u1, real* v0, real* v1, real* w0, real* w1,
	real** c0, real** c1, int flow_type, int order)
{
	struct pvelocity pv;
	
	pvelocity_init(
		&pv, nx, ny, ns, dx, dy, ds, dt,
		u0, u1, v0, v1, w0, w1,	flow_type);
	
	real *cold = *c0, *cnew = *c1, *cswap;

	for (int o = 0; o < order; o++)
	{
		if (o) pvelocity_pass(&pv, cold);

		// Set boundaries in the top level copied from the previous level.
		bndcpy(nx, ny, ns, cnew, cold);

		smolar_pass(&pv, cold, cnew);
		
		cswap = cnew; cnew = cold; cold = cswap;
	}
	
	*c1 = cold; *c0 = cnew;

	pvelocity_done(&pv);
	
	return 0;
}

