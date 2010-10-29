#pragma once

#include "smolar.h"

struct WW
{
	// Grid
	int nx, ny, ns, np;
	real *ds, dt;

	// Pseudo velocity components.
	real *c, *u0, *v0, *w0, *w1;
	
	// Flow type.
	int flow_type;
};

int ww_init(
	struct WW* desc, int nx, int ny, int ns,
	real* ds, real dt,
	real* w0, real* w1, int flow_type);

int ww_pass(struct WW* desc);

int ww_done(struct WW* desc);

