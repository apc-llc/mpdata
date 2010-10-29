#pragma once

#include "smolar.h"

// Perform method initial data scatter.
int msmolar_scatter(void** desc, int nx, int ny, int ns,
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

