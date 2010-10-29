#include "pvelocity.h"

int pvelocity_init(struct pvelocity* pv,
	int nx, int ny, int ns,
	real* dx, real* dy, real *ds, real dt,
	real* uflow0, real* uflow1,
	real* vflow0, real* vflow1,
	real* wflow0, real* wflow1,
	int flow_type)
{
	// Initialize pseudo velocity components
	uu_init(&pv->uu, nx, ny, ns, dx, dt, uflow0, uflow1, flow_type);
	vv_init(&pv->vv, nx, ny, ns, dy, dt, vflow0, vflow1, flow_type);
	ww_init(&pv->ww, nx, ny, ns, ds, dt, wflow0, wflow1, flow_type);
	
	// Pin results pointers
	pv->u = pv->uu.u1; pv->v = pv->vv.v1; pv->w = pv->ww.w1;
	
	return 0;
}

int pvelocity_pass(struct pvelocity* pv, real* c)
{
	pv->uu.v0 = pv->vv.v1; pv->uu.w0 = pv->ww.w1; pv->uu.c = c;
	pv->vv.u0 = pv->uu.u1; pv->vv.w0 = pv->ww.w1; pv->vv.c = c;
	pv->ww.u0 = pv->uu.u1; pv->ww.v0 = pv->vv.v1; pv->ww.c = c;
	
	uu_pass(&pv->uu);
	vv_pass(&pv->vv);
	ww_pass(&pv->ww);

	// Pin results pointers
	pv->u = pv->uu.u1; pv->v = pv->vv.v1; pv->w = pv->ww.w1;
	
	return 0;
}

int pvelocity_done(struct pvelocity* pv)
{ 
	uu_done(&pv->uu);
	vv_done(&pv->vv);
	ww_done(&pv->ww);
	
	return 0;
}

