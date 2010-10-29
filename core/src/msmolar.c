#include "smolar.h"
#include "msmolar.h"
#include "grid.h"
#include "scatter.h"
#include "gather.h"
#include "subcpy.h"

#include <assert.h>
#include <malloc.h>
#include <stdint.h>
#include <string.h>

int msmolar(void** desc, int nx, int ny, int ns,
	real* dx, real* dy, real* ds, const real dt,
	real* u0, real* u1, real* v0, real* v1, real* w0, real* w1,
	real** c0, real** c1, int flow_type, int order,
	int sx, int sy, int ss, int mode)
{
	// The number of distributed data arrays.
	int narrays = 8;

	size_t szelem = sizeof(real);

	// On first step perform the initial scatter.
	if (!*desc)
	{
		msmolar_scatter(desc,
			nx, ny, ns, dx, dy, ds, dt,
			u0, u1, v0, v1, w0, w1,
			c0, c1, flow_type, order,
			sx, sy, ss, mode);
	}

	struct domain_t* domains = (struct domain_t*)*desc;

	int ndomains = sx * sy * ss;
	
	// Remember the concentration levels disposition
	// in domains before processing, to swap the
	// output pointers accordingly.
	char* pc = domains[0].arrays[0];

	// Compute 3d cross pattern in subdomains' edges.
	for (int iv = 0; iv < ndomains; iv++)
	{
		struct domain_t* domain = domains + iv;
		struct domain_t* subdomains = domain->subdomains;
	
		int nsubdomains = domain->nsubdomains;

		// Scatter domain edges for separate computation.
		scatter(subdomains, 0, domain->arrays, narrays,
			szelem, LAYOUT_MODE_CUSTOM);

		// Process edges subdomains.
		for (int i = 0; i < nsubdomains; i++)
		{
			struct domain_t* sub = subdomains + i;

			// XXX The newer concentration data goes first.
			#define C1 (real**)(&sub->arrays[0])
			#define C0 (real**)(&sub->arrays[1])
			#define U0 (real*)(sub->arrays[2])
			#define U1 (real*)(sub->arrays[3])
			#define V0 (real*)(sub->arrays[4])
			#define V1 (real*)(sub->arrays[5])
			#define W0 (real*)(sub->arrays[6])
			#define W1 (real*)(sub->arrays[7])

			int success = smolar(
				sub->grid[0].bx + sub->grid[0].nx + sub->grid[0].ex,
				sub->grid[0].by + sub->grid[0].ny + sub->grid[0].ey,
				sub->grid[0].bs + sub->grid[0].ns + sub->grid[0].es,
				dx, dy, ds, dt,
				U0, U1, V0, V1, W0, W1, C0, C1, flow_type, order);

			assert(!success);

			#undef U0
			#undef U1
			#undef V0
			#undef V1
			#undef W0
			#undef W1
			#undef C0
			#undef C1
		}
	}

	// Share edges between linked subdomains.
	for (int iv = 0; iv < ndomains; iv++)
	{
		struct domain_t* domain = domains + iv;
		struct domain_t* subdomains = domain->subdomains;

		int nsubdomains = domain->nsubdomains;

		for (int i = 0; i < nsubdomains; i++)
		{
			struct domain_t* src = subdomains + i;
			struct domain_t* dst = *(src->links.dense[0]);

			assert(dst->grid[1].extsize == src->grid[0].extsize);

			size_t dnx = dst->grid[1].nx * szelem;
			size_t dbx = dst->grid[1].bx * szelem;
			size_t dex = dst->grid[1].ex * szelem;
			
			size_t dny = dst->grid[1].ny, dns = dst->grid[1].ns;
			size_t dby = dst->grid[1].by, dbs = dst->grid[1].bs;
			size_t dey = dst->grid[1].ey, des = dst->grid[1].es;

			size_t doffset = dbx + (dbx + dnx + dex) *
				(dby + dbs * (dby + dny + dey));

			size_t snx = src->grid[0].nx * szelem;
			size_t sbx = src->grid[0].bx * szelem;
			size_t sex = src->grid[0].ex * szelem;
			
			size_t sny = src->grid[0].ny, sns = src->grid[0].ns;
			size_t sby = src->grid[0].by, sbs = src->grid[0].bs;
			size_t sey = src->grid[0].ey, ses = src->grid[0].es;

			size_t soffset = sbx + (sbx + snx + sex) *
				(sby + sbs * (sby + sny + sey));

			// XXX Copy newer concentration source that is #0 in arrays
			// table to the #1 - in place of old one.
			subcpy(dnx, dny, dns,
				dst->arrays[1] + doffset,
				dbx + dnx + dex, dby + dny + dey, dbs + dns + des,
				src->arrays[0] + soffset,
				sbx + snx + sex, sby + sny + sey, sbs + sns + ses,
				memcpy);
		}
	}

	// Run smolar for each subdomain.
	for (int iv = 0; iv < ndomains; iv++)
	{
		struct domain_t* domain = domains + iv;
		struct domain_t* subdomains = domain->subdomains;

		// XXX The newer concentration data goes first.
		#define C1 (real**)(&domain->arrays[0])
		#define C0 (real**)(&domain->arrays[1])
		#define U0 (real*)(domain->arrays[2])
		#define U1 (real*)(domain->arrays[3])
		#define V0 (real*)(domain->arrays[4])
		#define V1 (real*)(domain->arrays[5])
		#define W0 (real*)(domain->arrays[6])
		#define W1 (real*)(domain->arrays[7])

		int success = smolar(
			domain->grid[0].bx + domain->grid[0].nx + domain->grid[0].ex,
			domain->grid[0].by + domain->grid[0].ny + domain->grid[0].ey,
			domain->grid[0].bs + domain->grid[0].ns + domain->grid[0].es,
			dx, dy, ds, dt,
			U0, U1, V0, V1, W0, W1, C0, C1, flow_type, order);
		
		assert(!success);

		#undef U0
		#undef U1
		#undef V0
		#undef V1
		#undef W0
		#undef W1
		#undef C0
		#undef C1

		#define C1 (char**)(&domain->arrays[0])	

		// Sync domain result with the separately
		// computed edges, using secondary subdomains grid.
		gather(subdomains, 1, C1, 2,
			szelem, LAYOUT_MODE_CUSTOM);
		
		#undef C1
	}
	
	// Swap concentration levels, if needed.
	if (pc != domains[0].arrays[0])
	{
		real* c = *c0;
		*c0 = *c1; *c1 = c;
	}
}

