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

/**
 * Perform a Smolarkiewicz transport equation scheme step.
 * @param nx - The grid X dimension
 * @param ny - The grid Y dimension
 * @param ns - The grid Z dimension
 * @param dx - The X grid step
 * @param dy - The Y grid step
 * @param ds - The Z grid step
 * @param dt - The time step
 * @param u0 - The X component of velocity field on the previous time step
 * @param u1 - The X component of velocity filed on the current time step
 * @param v0 - The Y component of velocity field on the previous time step
 * @param v1 - The Y component of velocity filed on the current time step
 * @param w0 - The Z component of velocity field on the previous time step
 * @param w1 - The Z component of velocity filed on the current time step
 * @param c0 - The pointer to the previous concentration array; on exit will change
 * and contain the previous intermediate concentration distribution
 * @param c1 - The pointer to the new concentration array; on exit will be
 * filled with the actual result
 * @param flow_type - The flow type: divergent or solenoidal
 * @param order - The number of accuracy improvement passes
 * @return The 0 value on successful run or an error code
 */
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

/**
 * Perform a distributed Smolarkiewicz transport equation scheme step
 * (version for clusters, multicore and hybrid systems).
 * @param desc - The distributed scheme descriptor; will be filled after
 * the first call, and must be passed to all further steps.
 * @param nx - The grid X dimension
 * @param ny - The grid Y dimension
 * @param ns - The grid Z dimension
 * @param dx - The X grid step
 * @param dy - The Y grid step
 * @param ds - The Z grid step
 * @param dt - The time step
 * @param u0 - The X component of velocity field on the previous time step
 * @param u1 - The X component of velocity filed on the current time step
 * @param v0 - The Y component of velocity field on the previous time step
 * @param v1 - The Y component of velocity filed on the current time step
 * @param w0 - The Z component of velocity field on the previous time step
 * @param w1 - The Z component of velocity filed on the current time step
 * @param c0 - The pointer to the previous concentration array; on exit will change
 * and contain the previous intermediate concentration distribution
 * @param c1 - The pointer to the new concentration array; on exit will be
 * filled with the actual result
 * @param flow_type - The flow type: divergent or solenoidal
 * @param order - The number of accuracy improvement passes
 * @param sx - The computational nodes grid X dimension
 * @param sy - The computational nodes grid Y dimension
 * @param ss - The computational nodes grid Z dimension
 * @param mode - The distributed computations mode
 * @return The 0 value on successful run or an error code
 */
int msmolar(void** desc, int nx, int ny, int ns,
	real* dx, real* dy, real* ds, const real dt,
	real* u0, real* u1, real* v0, real* v1, real* w0, real* w1,
	real** c0, real** c1, int flow_type, int order,
	int sx, int sy, int ss, int mode);

/**
 * Perform the full data gathering for a distributed
 * Smolarkiewicz transport equation scheme step
 * (version for clusters, multicore and hybrid systems).
 * @param desc - The distributed scheme descriptor; will be filled after
 * the first call, and must be passed to all further steps.
 * @param nx - The grid X dimension
 * @param ny - The grid Y dimension
 * @param ns - The grid Z dimension
 * @param dx - The X grid step
 * @param dy - The Y grid step
 * @param ds - The Z grid step
 * @param dt - The time step
 * @param u0 - The X component of velocity field on the previous time step
 * @param u1 - The X component of velocity filed on the current time step
 * @param v0 - The Y component of velocity field on the previous time step
 * @param v1 - The Y component of velocity filed on the current time step
 * @param w0 - The Z component of velocity field on the previous time step
 * @param w1 - The Z component of velocity filed on the current time step
 * @param c0 - The pointer to the previous concentration array; on exit will change
 * and contain the previous intermediate concentration distribution
 * @param c1 - The pointer to the new concentration array; on exit will be
 * filled with the actual result
 * @param flow_type - The flow type: divergent or solenoidal
 * @param order - The number of accuracy improvement passes
 * @param sx - The computational nodes grid X dimension
 * @param sy - The computational nodes grid Y dimension
 * @param ss - The computational nodes grid Z dimension
 * @param mode - The distributed computations mode
 * @return The 0 value on successful run or an error code
 */
int msmolar_gather(void** desc, int nx, int ny, int ns,
	real* dx, real* dy, real* ds, const real dt,
	real* u0, real* u1, real* v0, real* v1, real* w0, real* w1,
	real** c0, real** c1, int flow_type, int order,
	int sx, int sy, int ss, int mode);

