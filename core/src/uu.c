#include "uu.h"
#include "patterns.h"
#include "smolar_int.h"

#include <malloc.h>

#define V_NX (nx-2)
#define W_NP np_2
#define C_NX nx
#define C_NP np

#define reduced_stroke_V (HALF * (_C(V) + _U(V)))
#define stroke_V (QUAT * (_R(V) + _RU(V) + _U(V) + _C(V)))

#define reduced_stroke_W (HALF * (_C(W) + _F(W)))
#define stroke_W (QUAT * (_R(W) + _RF(W) + _F(W) + _C(W)))

#define pattern(DIV, V, W) \
\
	(-DIV + (abs(_C(U0)) * (ONE - abs(_C(U0)))) * \
\
	(_R(C) - _C(C)) / (_R(C) + _C(C) + MINVAL) - \
\
	HALF * (_C(U0) * (V * \
\
	(_RU(C) + _U(C) - _D(C) - _RD(C)) / \
	(_RU(C) + _U(C) + _D(C) + _RD(C) + MINVAL) + \
\
	(W * \
\
	(_RF(C) + _F(C) - _B(C) - _RB(C)) / \
	(_RF(C) + _F(C) + _B(C) + _RB(C) + MINVAL)))))

int uu_pass(struct UU* desc)
{
	// Grid
	int nx = desc->nx, ny = desc->ny, ns = desc->ns;
	int np = desc->np, np_2 = (nx - 2) * (ny - 2);
	int nx2 = 2 * nx;

	// Swap old and new velocity, assuming old or initial
	// destribution resides u1.
	real *uswap = desc->u0;
	desc->u0 = desc->u1; desc->u1 = uswap;

	// Pseudo velocity components.
	real *c = desc->c;
	real *u0 = desc->u0, *u1 = desc->u1;
	real *v0 = desc->v0;
	real *w0 = desc->w0;
	
#define C	(&c	[k * np + j * nx + i])
#define U0 	(&u0	[(k - 1) * (nx - 1) * (ny - 2) + (j - 1) * (nx - 1) + i])
#define U1	(&u1	[(k - 1) * (nx - 1) * (ny - 2) + (j - 1) * (nx - 1) + i])

	for (int k = 1; k < (ns-1); k++)
	{
		for (int j = 1; j < (ny-1); j++)
		{
			int i = 0;
			
#define V	(&v0	[(k - 1) * (nx - 2) * (ny - 1) + (j - 1) * (nx - 2) + i])
#define W	(&w0	[(k - 1) * (nx - 2) * (ny - 2) + (j - 1) * (nx - 2) + i])
			
			// Reduced pattern for strokes in left boundary grid points.
			{
				// The 18-point pattern.
				_C(U1) = pattern(0, reduced_stroke_V, reduced_stroke_W);
				CHK_COURANT(_C(U1));
			}

#undef V
#undef W
			
#define V	(&v0	[(k - 1) * (nx - 2) * (ny - 1) + (j - 1) * (nx - 2) + i - 1])
#define W	(&w0	[(k - 1) * (nx - 2) * (ny - 2) + (j - 1) * (nx - 2) + i - 1])

			for (i = 1; i < (nx-2); i++)
			{
				// The additional divergent term.
				real div =
					QUAT * _C(U0) * (_R(U0) - _L(U0) +		
					_R(V) - _RU(V) - _U(V) + _C(V) +
					_R(W) - _RF(W) - _F(W) + _C(W));

				// The 18-point pattern.
				_C(U1) = pattern(div, stroke_V, stroke_W);
				CHK_COURANT(_C(U1));
			}
			

			// Reduced pattern for strokes in right boundary grid points.
			{
				// The 18-point pattern.
				_C(U1) = pattern(0, reduced_stroke_V, reduced_stroke_W);
				CHK_COURANT(_C(U1));
			}
		}
	}
	
	return 0;
}

int uu_init(
	struct UU* desc, int nx, int ny, int ns, real *dx, real dt,
	real* u0, real* u1, int flow_type)
{
	desc->nx = nx; desc->ny = ny;
	desc->ns = ns; desc->np = nx * ny;
	desc->dx = dx; desc->dt = dt;
	desc->flow_type = flow_type;

	int nx2 = 2 * nx, np = desc->np;	
	int size = (nx - 1) * (ny - 2) * (ns - 2) * sizeof(real);
	desc->u0 = (real*)malloc(size); desc->u1 = (real*)malloc(size);

	real *u = desc->u1;
	
#undef U0
#undef U1

#define U 	(&u	[(k - 1) * (nx - 1) * (ny - 2) + (j - 1) * (nx - 1) + i])
#define U0 	(&u0	[k * np + j * nx + i])
#define U1 	(&u1	[k * np + j * nx + i])
	
	for (int k = 1; k < ns-1; k++)
	{
		for (int j = 1; j < ny-1; j++)
		{
			for (int i = 0; i < nx-1; i++)
			{
				real qdtdx = QUAT * dt / dx[i];

				_C(U) = qdtdx * (_C(U0) + _R(U0) + _C(U1) + _R(U1));
				CHK_COURANT(_C(U));
			}
		}
	}
	
	return 0;
}

int uu_done(struct UU* desc)
{
	free((void*)desc->u0); free((void*)desc->u1);
	
	return 0;
}

