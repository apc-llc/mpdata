#pragma once

#include "smolar.h"

struct UU
{
	// Grid
	int nx, ny, ns, np;
	real *dx, dt;

	// Pseudo velocity components.
	real *c, *u0, *u1, *v0, *w0;
	
	// Flow type.
	int flow_type;
};

int uu_init(
	struct UU* desc, int nx, int ny, int ns,
	real* dx, real dt,
	real* u0, real* u1, int flow_type);

int uu_pass(struct UU* desc);

int uu_done(struct UU* desc);

