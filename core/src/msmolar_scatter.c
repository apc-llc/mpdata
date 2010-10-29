#include "msmolar.h"
#include "scatter.h"

#include <malloc.h>
#include <string.h>

// Perform method initial data scatter.
int msmolar_scatter(void** desc, int nx, int ny, int ns,
	real* dx, real* dy, real* ds, const real dt,
	real* u0, real* u1, real* v0, real* v1, real* w0, real* w1,
	real** c0, real** c1, int flow_type, int order,
	int sx, int sy, int ss, int mode)
{
	// The fictive boundaries thicknesses,
	// specific for the numerical scheme pattern.
	int bx = 1, by = 1, bs = 1;
	int ex = 1, ey = 1, es = 1;

	// The number of distributed data arrays.
	int narrays = 8;

	size_t szelem = sizeof(real);

	int ndomains = sx * sy * ss + 1;

	// The data arrays table.
	// XXX The order of arrays supplied to multigrid
	// is important: the newer concentration data goes first.
	// Domains and subdomains also inherit this arrays order.
	char* arrays[] =
	{
		(char*)*c1, (char*)*c0, (char*)u0, (char*)u1,
		(char*)v0, (char*)v1, (char*)w0, (char*)w1
	};

	// Setup the grid and subdomains dimensions.
	struct domain_t* domains;
	setgrid(&domains,
		nx, ny, ns, sx, sy, ss,
		bx, ex, by, ey, bs, es);
	*desc = (void*)domains;

	// Allocate space for each domain data.
	ndomains--;
	for (int iv = 0; iv < ndomains; iv++)
	{
		struct domain_t* domain = domains + iv;
		struct domain_t* subdomains = domain->subdomains;

		int nsubdomains = domain->nsubdomains;

		// Count and allocate the required memory for each domain.
		size_t szarrays = narrays * (sizeof(char*) +
			domain->grid[0].extsize * szelem);
		for (int i = 0; i < nsubdomains; i++)
			szarrays += narrays * (sizeof(char*) +
				subdomains[i].grid[0].extsize * szelem);
		domain->arrays = (char**)malloc(szarrays);
	
		char* pdarrays = (char*)(domain->arrays + narrays);
		for (int iarray = 0; iarray < narrays; iarray++)
		{
			domain->arrays[iarray] = pdarrays;
			pdarrays += domain->grid[0].extsize * szelem;
		}
		for (int i = 0; i < nsubdomains; i++)
		{
			struct domain_t* subdomain = subdomains + i;
	
			subdomain->arrays = (char**)pdarrays;
			pdarrays = (char*)(subdomain->arrays + narrays);
			for (int iarray = 0; iarray < narrays; iarray++)
			{
				subdomain->arrays[iarray] = pdarrays;
				pdarrays += subdomain->grid[0].extsize * szelem;
			}

			// Set subdomain data copying callbacks.
			subdomain->load = &memcpy;
			subdomain->save = &memcpy;
		}
	
		// Set domain data copying callbacks.
		domain->load = &memcpy;
		domain->save = &memcpy;
	}

	// Scatter arrays data to the subdomains.
	scatter(domains, 0, arrays, narrays,
		szelem, LAYOUT_MODE_AUTO);
}

