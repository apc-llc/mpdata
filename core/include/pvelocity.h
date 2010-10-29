#pragma once

#include "uu.h"
#include "vv.h"
#include "ww.h"

struct pvelocity
{
	// Pseudo velocity components
	real *u, *v, *w;
	
	// Pseudo velocity internal data structures
	struct UU uu;
	struct VV vv;
	struct WW ww;
};

int pvelocity_init(struct pvelocity* pv,
	int nx, int ny, int ns,
	real* dx, real* dy, real *ds, real dt,
	real* uflow0, real* uflow1,
	real* vflow0, real* vflow1,
	real* wflow0, real* wflow1,
	int flow_type);

int pvelocity_pass(struct pvelocity* pv, real* c);

int pvelocity_done(struct pvelocity* pv);

