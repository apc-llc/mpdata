#pragma once

#include "smolar.h"

struct VV
{
	// Grid
	int nx, ny, ns, np;
	real *dy, dt;

	// Pseudo velocity components.
	real *c, *u0, *v0, *v1, *w0;
	
	// Flow type.
	int flow_type;
};

int vv_init(
	struct VV* desc, int nx, int ny, int ns,
	real *dy, real dt,
	real* v0, real* v1, int flow_type);

int vv_pass(struct VV* desc);

int vv_done(struct VV* desc);

