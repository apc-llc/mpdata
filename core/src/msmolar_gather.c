#include "msmolar.h"
#include "gather.h"

// Perform the full data gathering.
int msmolar_gather(void** desc, int nx, int ny, int ns,
	real* dx, real* dy, real* ds, const real dt,
	real* u0, real* u1, real* v0, real* v1, real* w0, real* w1,
	real** c0, real** c1, int flow_type, int order,
	int sx, int sy, int ss, int mode)
{
	struct domain_t* domains = (struct domain_t*)*desc;

	// The number of distributed data arrays.
	int narrays = 8;

	// The data arrays table.
	char* arrays[] =
	{
		(char*)*c1, (char*)*c0, (char*)u0, (char*)u1,
		(char*)v0, (char*)v1, (char*)w0, (char*)w1
	};

	size_t szelem = sizeof(real);

	// Gather subdomains data to the second array.
	gather(domains, 0, (char**)arrays, 2,
		szelem, LAYOUT_MODE_AUTO);
}

