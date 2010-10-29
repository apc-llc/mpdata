#pragma once

#define SOLENOIDAL	0
#define DIVERGENT	1

#ifdef SINGLE
#define real float
#endif

#ifdef DOUBLE
#define real double
#endif

#ifdef INTEGER
#define real long
#endif

#if !defined(SINGLE) && !defined(DOUBLE) && !defined(INTEGER)
#error "The single or double floating-precision or integer mode must be set"
#endif

int smolar(int nx, int ny, int ns,
	real* dx, real* dy, real* ds, const real dt,
	real* u0, real* u1, real* v0, real* v1, real* w0, real* w1,
	real** c0, real** c1, int flow_type, int order);

/**
 * The execution mode value for serial emulation.
 */
#define MSMOLAR_SERIAL		0

/**
 * The execution mode value for PTHREADS acceleration.
 */
#define MSMOLAR_PTHREADS	1

/**
 * The execution mode value for OpenMP acceleration.
 */
#define MSMOLAR_OPENMP		2

int msmolar(void** desc, int nx, int ny, int ns,
	real* dx, real* dy, real* ds, const real dt,
	real* u0, real* u1, real* v0, real* v1, real* w0, real* w1,
	real** c0, real** c1, int flow_type, int order,
	int sx, int sy, int ss, int mode);

// Perform the full data gathering.
int msmolar_gather(void** desc, int nx, int ny, int ns,
	real* dx, real* dy, real* ds, const real dt,
	real* u0, real* u1, real* v0, real* v1, real* w0, real* w1,
	real** c0, real** c1, int flow_type, int order,
	int sx, int sy, int ss, int mode);

