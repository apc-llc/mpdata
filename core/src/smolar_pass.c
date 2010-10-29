#include "smolar_pass.h"
#include "patterns.h"
#include "smolar_int.h"

int smolar_pass(struct pvelocity* pv, real* c0, real* c1)
{
	int nx = pv->uu.nx, ny = pv->uu.ny;
	int ns = pv->uu.ns, np = pv->uu.np;

	int np_2 = (nx - 2) * (ny - 2);
	int nx_1_ny_2 = (nx - 1) * (ny - 2);
	int nx_2_ny_1 = (nx - 2) * (ny - 1);

	// Create the pseudo velocity arrays sliders.
	real *u = pv->u, *v = pv->v, *w = pv->w;

#define C0_NX nx
#define C0_NP np
#define V_NX (nx-2)
#define W_NP np_2

#define C0	(&c0	[k * np + j * nx + i])
#define C1	(&c1	[k * np + j * nx + i])

#define U	(&u	[(k - 1) * nx_1_ny_2 + (j - 1) * (nx - 1) + i])
#define V	(&v	[(k - 1) * nx_2_ny_1 + j * (nx - 2) + i - 1])
#define W	(&w	[k * np_2 + (j - 1) * (nx - 2) + i - 1])

	// Evaluate the next approximation.
	for (int k = 1; k < (ns-1); k++)
	{
		for (int j = 1; j < (ny-1); j++)
		{
			for (int i = 1; i < (nx-1); i++)
			{
				real abs_CU = abs(_C(U)), abs_LU = abs(_L(U));
				real abs_CV = abs(_C(V)), abs_DV = abs(_D(V));
				real abs_CW = abs(_C(W)), abs_BW = abs(_B(W));

#define F(L, R, V, AV) (L * (V + AV) + R * (V - AV))
			
				_C(C1) = _C(C0) - HALF * (
					(
						F(_C(C0), _R(C0), _C(U), abs_CU) -
						F(_L(C0), _C(C0), _L(U), abs_LU)
					) +
					(
						F(_C(C0), _U(C0), _C(V), abs_CV) -
						F(_D(C0), _C(C0), _D(V), abs_DV)
					) +
					(
						F(_C(C0), _F(C0), _C(W), abs_CW) -
						F(_B(C0), _C(C0), _B(W), abs_BW)
					));
			}
		}

	}
	
	return 0;
}

