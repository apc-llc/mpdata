#include "vv.h"
#include "patterns.h"
#include "smolar_int.h"

#include <malloc.h>

#define U_NX (nx-1)
#define V0_NX (nx-2)
#define V1_NX (nx-2)
#define W_NX (nx-2)
#define W_NP np_2
#define C_NX nx
#define C_NP np

#define reduced_stroke_U (HALF * (_C(U) + _R(U)))
#define stroke_U (QUAT * (_U(U) + _RU(U) + _R(U) + _C(U)))

#define reduced_stroke_W (HALF * (_C(W) + _F(W)))
#define stroke_W (QUAT * (_U(W) + _UF(W) + _F(W) + _C(W)))

#define pattern(DIV, U, W) \
\
	(-DIV + (abs(_C(V0)) * (ONE - abs(_C(V0)))) * \
\
	(_U(C) - _C(C)) / (_U(C) + _C(C) + MINVAL) - \
\
	HALF * (_C(V0) * (U * \
\
	(_RU(C) + _R(C) - _L(C) - _LU(C)) / \
	(_RU(C) + _R(C) + _L(C) + _LU(C) + MINVAL) + \
\
	(W * \
\
	(_UF(C) + _F(C) - _B(C) - _UB(C)) / \
	(_UF(C) + _F(C) + _B(C) + _UB(C) + MINVAL)))))

int vv_pass(struct VV* desc)
{
	// Grid
	int nx = desc->nx, ny = desc->ny, ns = desc->ns;
	int np = desc->np, np_2 = (nx - 2) * (ny - 2);

	// Swap old and new velocity, assuming old or initial
	// destribution resides u1.
	real *vswap = desc->v0;
	desc->v0 = desc->v1; desc->v1 = vswap;

	// Pseudo velocity components.
	real *c = desc->c;
	real *u0 = desc->u0;
	real *v0 = desc->v0, *v1 = desc->v1;
	real *w0 = desc->w0;
	
#define C	(&c	[k * np + j * nx + i])
#define V0 	(&v0	[(k - 1) * (nx - 2) * (ny - 1) + j * (nx - 2) + i - 1])
#define V1	(&v1	[(k - 1) * (nx - 2) * (ny - 1) + j * (nx - 2) + i - 1])

	for (int k = 1; k < (ns-1); k++)
	{
		int j = 0;

#define U	(&u0	[(k - 1) * (nx - 1) * (ny - 2) + j * (nx - 1) + i - 1])
#define W	(&w0	[(k - 1) * (nx - 2) * (ny - 2) + j * (nx - 2) + i - 1])
		
		// Reduced pattern for strokes in upper boundary grid points.
		for (int i = 1; i < (nx-1); i++)
		{
			// The 18-point pattern.
			_C(V1) = pattern(0, reduced_stroke_U, reduced_stroke_W);
			CHK_COURANT(_C(V1));
		}

#undef U
#undef W

#define U	(&u0	[(k - 1) * (nx - 1) * (ny - 2) + (j - 1) * (nx - 1) + i - 1])
#define W	(&w0	[(k - 1) * (nx - 2) * (ny - 2) + (j - 1) * (nx - 2) + i - 1])

		for (j = 1; j < ny-2; j++)
		{
			// FIXME: i = 1 changed to 2 due to out of range index
			for (int i = 2; i < (nx-1); i++)
			{
				// The additional divergent term.
				real div = 

					QUAT * _C(V0) * (_U(V0) - _D(V0) +
					_U(W) - _UF(W) - _F(W) + _C(W) +
					_U(U) - _LU(U) - _L(U) + _C(U));

				// The 18-point pattern.
				_C(V1) = pattern(div, stroke_U, stroke_W);
				CHK_COURANT(_C(V1));				
			}
		}

		// Reduced pattern for strokes in lower boundary grid points.
		for (int i = 1; i < (nx-1); i++)
		{
			// The 18-point pattern.
			_C(V1) = pattern(0, reduced_stroke_U, reduced_stroke_W);
			CHK_COURANT(_C(V1));
		}
	}
	
	return 0;
}

int vv_init(
	struct VV* desc, int nx, int ny, int ns, real* dy, real dt,
	real* v0, real* v1, int flow_type)
{
	desc->nx = nx; desc->ny = ny;
	desc->ns = ns; desc->np = nx * ny;
	desc->dy = dy; desc->dt = dt;
	desc->flow_type = flow_type;

	int np = desc->np;	
	int size = (nx - 2) * (ny - 1) * (ns - 2) * sizeof(real);
	desc->v0 = (real*)malloc(size); desc->v1 = (real*)malloc(size);

	real *v = desc->v1;

#undef V0_NX
#undef V1_NX

#define V0_NX nx
#define V1_NX nx

#undef V0
#undef V1

#define V 	(&v	[(k - 1) * (nx - 2) * (ny - 1) + j * (nx - 2) + i - 1])
#define V0 	(&v0	[k * np + j * nx + i])
#define V1 	(&v1	[k * np + j * nx + i])

	for (int k = 1; k < (ns-1); k++)
	{
		for (int j = 0; j < (ny-1); j++)
		{
			real qdtdy = QUAT * dt / dy[j];

			for (int i = 1; i < (nx-1); i++)
			{
				_C(V) = qdtdy * (_C(V0) + _U(V0) + _C(V1) + _U(V1));
				CHK_COURANT(_C(V));
			}
		}
	}
	
	return 0;
}

int vv_done(struct VV* desc)
{
	free((void*)desc->v0); free((void*)desc->v1);
	
	return 0;
}

