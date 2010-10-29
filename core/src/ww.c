#include "ww.h"
#include "patterns.h"
#include "smolar_int.h"

#include <malloc.h>

#define U_NP nx_1_ny_2
#define V_NX (nx-2)
#define V_NP nx_2_ny_1
#define W0_NP np_2
#define C_NX nx
#define C_NP np

#define reduced_stroke_U (HALF * (_C(U) + _R(U)))
#define stroke_U (QUAT * (_R(U) + _RF(U) + _F(U) + _C(U)))

#define reduced_stroke_V (HALF * (_C(V) + _U(V)))
#define stroke_V (QUAT * (_F(V) + _UF(V) + _U(V) + _C(V)))

#define pattern(DIV, U, V) \
\
	(-DIV + (abs(_C(W0)) * (1 - abs(_C(W0)))) * \
\
	(_F(C) - _C(C)) / (_F(C) + _C(C) + MINVAL) - \
\
	HALF * (_C(W0) * (U * \
\
	(_RF(C) + _R(C) - _L(C) - _LF(C)) / \
	(_RF(C) + _R(C) + _L(C) + _LF(C) + MINVAL) + \
\
	(V * \
\
	(_UF(C) + _U(C) - _D(C) - _DF(C)) / \
	(_UF(C) + _U(C) + _D(C) + _DF(C) + MINVAL)))))

int ww_pass(struct WW* desc)
{
	// Grid
	int nx = desc->nx, ny = desc->ny, ns = desc->ns;
	int np = desc->np, np_2 = (nx - 2) * (ny - 2);
	int nx2 = 2 * nx;
	int nx_1_ny_2 = (nx - 1) * (ny - 2), nx_2_ny_1 = (nx - 2) * (ny - 1);

	// Swap old and new velocity, assuming old or initial
	// destribution resides u1.
	real *wswap = desc->w0;
	desc->w0 = desc->w1; desc->w1 = wswap;

	// Pseudo velocity components.
	real *c = desc->c;
	real *u0 = desc->u0;
	real *v0 = desc->v0;
	real *w0 = desc->w0, *w1 = desc->w1;

#define C	(&c	[k * np + j * nx + i])
#define W0 	(&w0	[k * (nx - 2) * (ny - 2) + (j - 1) * (nx - 2) + i - 1])
#define W1	(&w1	[k * (nx - 2) * (ny - 2) + (j - 1) * (nx - 2) + i - 1])

#define U	(&u0	[k * (nx - 1) * (ny - 2) + (j - 1) * (nx - 1) + i - 1])
#define V	(&v0	[k * (nx - 2) * (ny - 1) + (j - 1) * (nx - 2) + i - 1])

	int k = 0;

	// Reduced pattern for strokes in back boundary grid points.
	for (int j = 1; j < (ny-1); j++)
	{
		for (int i = 1; i < (nx-1); i++)
		{
			// The 18-point pattern.
			_C(W1) = pattern(0, reduced_stroke_U, reduced_stroke_V);
			CHK_COURANT(_C(W1));
		}
	}

#undef U
#undef V

#define U	(&u0	[(k - 1) * (nx - 1) * (ny - 2) + (j - 1) * (nx - 1) + i - 1])
#define V	(&v0	[(k - 1) * (nx - 2) * (ny - 1) + (j - 1) * (nx - 2) + i - 1])

	for (k = 1; k < (ns-2); k++)
	{
		for (int j = 1; j < (ny-1); j++)
		{
			for (int i = 1; i < (nx-1); i++)
			{
				// The additional divergent term.
				real div =

					QUAT * _C(W0) * (_F(W0) - _B(W0) +
					_F(U) - _RF(U) - _R(U) + _C(U) +
					_F(V) - _UF(V) - _U(V) + _C(V));

				// The 18-point pattern.
				_C(W1) = pattern(div, stroke_U, stroke_V);
				CHK_COURANT(_C(W1));
			}
		}
	}

	// Reduced pattern for strokes in front boundary grid points.
	for (int j = 1; j < (ny-1); j++)
	{
		for (int i = 1; i < (nx-1); i++)
		{
			// The 18-point pattern.
			_C(W1) = pattern(0, reduced_stroke_U, reduced_stroke_V);
			CHK_COURANT(_C(W1));
		}
	}
	
	return 0;
}

int ww_init(
	struct WW* desc, int nx, int ny, int ns, real *ds, real dt,
	real* w0, real* w1, int flow_type)
{
	desc->nx = nx; desc->ny = ny;
	desc->ns = ns; desc->np = nx * ny;
	desc->ds = ds; desc->dt = dt;
	desc->flow_type = flow_type;

	int nx2 = 2 * nx, np = desc->np;	
	int size = (nx - 2) * (ny - 2) * (ns - 1) * sizeof(real);
	desc->w0 = (real*)malloc(size); desc->w1 = (real*)malloc(size);

	real *w = desc->w1;

#undef W0_NP

#define W_NP np
#define W0_NP np
#define W1_NP np

#undef W0
#undef W1

#define W 	(&w	[k * (nx - 2) * (ny - 2) + (j - 1) * (nx - 2) + i - 1])
#define W0 	(&w0	[k * np + j * nx + i])
#define W1 	(&w1	[k * np + j * nx + i])

	for (int k = 0; k < (ns-1); k++)
	{
		real qdtds = QUAT * dt / ds[k];

		for (int j = 1; j < (ny-1); j++)
		{
			for (int i = 1; i < (nx-1); i++)
			{
				_C(W) = qdtds * (_C(W0) + _F(W0) + _C(W1) + _F(W1));
				CHK_COURANT(_C(W));
			}
		}
	}
	
	return 0;
}

int ww_done(struct WW* desc)
{
	free((void*)desc->w0); free((void*)desc->w1);
	
	return 0;
}

