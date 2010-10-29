#include "grid.h"
#include "overhead.h"

#include <assert.h>
#include <malloc.h>
#include <stdint.h>
#include <string.h>

// Create the decomposition grid.
void setgrid(struct domain_t** pdomains,
	int nx, int ny, int ns,	int sx, int sy, int ss,
	int bx, int ex, int by, int ey, int bs, int es)
{
	int nsubdomains = sx * sy * ss;

	// Allocate space for domains configuration,
	// including nested subdomains and links:
	// parent domain + nsubdomains + PTRN_N nested domains in each subdomain.
	size_t szdomains = sizeof(struct domain_t) *
		((PTRN_N + 1) * nsubdomains + 1);
	*pdomains = (struct domain_t*)malloc(szdomains);
	struct domain_t* domains = *pdomains;

	// XXX: Trap overhead size >= dimension,
	// it is not possible to handle this case correctly.
	assert((sx == 1) || ((sx > 1) && (bx < nx) && (ex < nx)));
	assert((sy == 1) || ((sy > 1) && (by < ny) && (ey < ny)));
	assert((ss == 1) || ((ss > 1) && (bs < ns) && (es < ns)));

	// Zero domains config space.
	memset(domains, 0, szdomains);

	// Adjust grid if it is inappropriate for partitioning.
	if (sx > nx) sx = nx;
	if (sy > ny) sy = ny;
	if (ss > ns) ss = ns;	

	int nv = nx * ny * ns;
	int sv = nsubdomains;

	// Put the parent domain on the last place.
	struct domain_t* parent = domains + sv;

	// Record global grid into parent domain.
	parent->grid[0].nx = nx;
	parent->grid[0].ny = ny;
	parent->grid[0].ns = ns;
	parent->grid[1] = parent->grid[0];
	
	parent->parent = NULL;
	parent->subdomains = domains;

	parent->sx = sx;
	parent->sy = sy;
	parent->ss = ss;
	parent->nsubdomains = nsubdomains;
		
	// Set parent and subdomains for all domains.
	for (int iv = 0; iv < sv; iv++)
	{
		domains[iv].parent = parent;
		domains[iv].subdomains = NULL;
	}

	// Calculate the subdomains dimensions.
	// TODO: probably add 16 or some other "good"
	// divide factor to favor aligned memory?
	int snx = nx / sx, srx = nx % sx;
	int sny = ny / sy, sry = ny % sy;
	int sns = ns / ss, srs = ns % ss;

	// Setup the subdomains dimensions.
	{
		int iv = 0;

		for (int is = 0; is < srs; is++)
		{
			for (int iy = 0; iy < sry; iy++)
			{
				for (int ix = 0; ix < srx; ix++, iv++)
				{
					domains[iv].grid[0].nx = snx + 1;
					domains[iv].grid[0].ny = sny + 1;
					domains[iv].grid[0].ns = sns + 1;
				}
				for (int ix = srx; ix < sx; ix++, iv++)
				{
					domains[iv].grid[0].nx = snx;
					domains[iv].grid[0].ny = sny + 1;
					domains[iv].grid[0].ns = sns + 1;
				}
			}
			for (int iy = sry; iy < sy; iy++)
			{
				for (int ix = 0; ix < srx; ix++, iv++)
				{
					domains[iv].grid[0].nx = snx + 1;
					domains[iv].grid[0].ny = sny;
					domains[iv].grid[0].ns = sns + 1;
				}
				for (int ix = srx; ix < sx; ix++, iv++)
				{
					domains[iv].grid[0].nx = snx;
					domains[iv].grid[0].ny = sny;
					domains[iv].grid[0].ns = sns + 1;
				}
			}
		}
		for (int is = srs; is < ss; is++)
		{
			for (int iy = 0; iy < sry; iy++)
			{
				for (int ix = 0; ix < srx; ix++, iv++)
				{
					domains[iv].grid[0].nx = snx + 1;
					domains[iv].grid[0].ny = sny + 1;
					domains[iv].grid[0].ns = sns;
				}
				for (int ix = srx; ix < sx; ix++, iv++)
				{
					domains[iv].grid[0].nx = snx;
					domains[iv].grid[0].ny = sny + 1;
					domains[iv].grid[0].ns = sns;
				}
			}
			for (int iy = sry; iy < sy; iy++)
			{
				for (int ix = 0; ix < srx; ix++, iv++)
				{
					domains[iv].grid[0].nx = snx + 1;
					domains[iv].grid[0].ny = sny;
					domains[iv].grid[0].ns = sns;
				}
				for (int ix = srx; ix < sx; ix++, iv++)
				{
					domains[iv].grid[0].nx = snx;
					domains[iv].grid[0].ny = sny;
					domains[iv].grid[0].ns = sns;
				}
			}
		}
		
		assert (iv == sv);
	}

	// XXX: Handle a special case:
	// when domains grid dimension = 1
	// we cannot put any overhead, other than zero.
	int incx = 1, incy = 1, incs = 1;
	if (sx == 1) { bx = 0; ex = 0; incx = 0; }
	if (sy == 1) { by = 0; ey = 0; incy = 1 - sx; }
	if (ss == 1) { bs = 0; es = 0; incs = 1 - sx * sy; }

	// Normalized versions of fictive boundaries
	// thincknesses.
	int nbx = bx ? 1 : 0, nby = bx ? 1 : 0, nbs = bs ? 1 : 0;
	int nex = ex ? 1 : 0, ney = ex ? 1 : 0, nes = es ? 1 : 0;

	// Create an fictive empty domain
	// to be referenced in heighbourhood links.
	size_t szempty = sizeof(struct domain_t) * (PTRN_N + 1);
	struct domain_t* empty = (struct domain_t*)malloc(szempty);
	memset(empty, 0, szempty);
	empty->subdomains = (struct domain_t*)(empty + 1);

	// Set domains overheads using fictive empty domain.
	overhead(domains, empty, 0,
		sx, sy, ss, incx, incy, incs,
		bx, ex, by, ey, bs, es);
	
	// Get space for subdomains from the previously
	// allocated memory.
	struct domain_t* subdomains = domains + nsubdomains + 1;
	
	// Set pointers for subdomains.
	for (int iv = 0; iv < sv; iv++)
	{
		struct domain_t* domain = domains + iv;
		domain->subdomains = subdomains + iv * PTRN_N;
	}

	// Create local subgrids for each first level
	// grid domain.	
	for (int iv = 0; iv < sv; iv++)
	{
		struct domain_t* domain = domains + iv;
		
		int nx = domain->grid[0].nx;
		int ny = domain->grid[0].ny;
		int ns = domain->grid[0].ns;
		
		nx += domain->grid[0].bx * bx + domain->grid[0].ex * ex;
		ny += domain->grid[0].by * by + domain->grid[0].ey * ey;
		ns += domain->grid[0].bs * bs + domain->grid[0].es * es;

		// Calculate domain nested grid dimensions.
		domain->sx = domain->grid[0].bx + domain->grid[0].ex;
		domain->sy = domain->grid[0].by + domain->grid[0].ey;
		domain->ss = domain->grid[0].bs + domain->grid[0].es;
		
		// Create subgrid of inner boundary only domains,
		// specialized for inter-domain boundaries exchanging:
		// each domain must have b*/e* fictive border thickness
		// both from the parent edge and from the inner side.
		domain->nsubdomains = 0;

		// Copy the configured grid structure to the
		// subdomain, and set sparse & dense pattern
		// links for it.
		#define LINK(ptrn, mask, offset) \
		{ \
			domain->subdomains[idomain].grid[0] = grids[0]; \
			domain->subdomains[idomain].grid[1] = grids[1]; \
		\
			domain->subdomains[idomain].parent = domain; \
			domain->subdomains[idomain].links.sparse[ptrn] = \
				(struct domain_t*)(mask * (intptr_t)( \
				domain->links.sparse[ptrn]->subdomains + offset)); \
		\
			int* ndense = &(domain->subdomains[idomain].links.ndense); \
			assert(*ndense < PTRN_N); \
			domain->subdomains[idomain].links.dense[*ndense] = \
				(struct domain_t**)(mask * (intptr_t)( \
				&(domain->subdomains[idomain].links.sparse[ptrn]))); \
			*ndense += mask; idomain += mask; \
			domain->nsubdomains += mask; \
		}

		// Set the grid configuration.
		#define GRID(i, \
			_bx, _nx, _ex, _by, _ny, _ey, \
			_bs, _ns, _es, _ix, _iy, _is) \
		{ \
			struct grid_t* grid = grids + i; \
			memset(grid, 0, sizeof(struct grid_t)); \
			grid->bx = _bx; grid->nx = _nx; grid->ex = _ex; \
			grid->by = _by; grid->ny = _ny; grid->ey = _ey; \
			grid->bs = _bs; grid->ns = _ns; grid->es = _es; \
			grid->ix = _ix; grid->iy = _iy; grid->is = _is; \
		}

		// Setup subdomains. Not all subdomains exist
		// for boundary domains, they are simply rewritten,
		// if the corresponding flag is zero.
		{
			int idomain = 0;
			
			// For each subdomain prepare both primary and
			// secondary grids. Secondary grid is adjusted
			// to accept the side boundary in neighbours'
			// edges synchronization process.
			struct grid_t grids[2];

			// Edge (0, Y, Z) subdomain.			
			// Link LEFT edge to the RIGHT edge
			// of the LEFT neighbour.
			GRID(0, 1, bx, 1, 0, ny, 0, 0, ns, 0, bx, 0, 0);
			GRID(1, 0, bx, 2, 0, ny, 0, 0, ns, 0, 0, 0, 0);
			LINK(PTRN_L, domain->grid[0].bx * nbx,
				domain->links.sparse[PTRN_L]->grid[0].bx);

			// Edge (1, Y, Z) subdomain.
			// Link RIGHT edge to the LEFT edge
			// of the RIGHT neighbour.
			GRID(0, 1, ex, 1, 0, ny, 0, 0, ns, 0, nx - 2 * ex, 0, 0);
			GRID(1, 2, ex, 0, 0, ny, 0, 0, ns, 0, nx - ex, 0, 0);
			LINK(PTRN_R, domain->grid[0].ex * nex, 0);

			// Edge (X, 0, Z) subdomain.
			// Link LOWER edge to the UPPER edge
			// of the LOWER neighbour.
			GRID(0, 0, nx, 0, 1, by, 1, 0, ns, 0, 0, by, 0);
			GRID(1, 0, nx, 0, 0, by, 2, 0, ns, 0, 0, 0, 0);
			LINK(PTRN_D, domain->grid[0].by * nby,
				domain->links.sparse[PTRN_D]->grid[0].bx +
				domain->links.sparse[PTRN_D]->grid[0].ex +
				domain->links.sparse[PTRN_D]->grid[0].by);

			// Edge (X, 1, Z) subdomain.
			// Link UPPER edge to the LOWER edge
			// of the UPPER neighbour.
			GRID(0, 0, nx, 0, 1, ey, 1, 0, ns, 0, 0, ny - 2 * ey, 0);
			GRID(1, 0, nx, 0, 2, ey, 0, 0, ns, 0, 0, ny - ey, 0);
			LINK(PTRN_U, domain->grid[0].ey * ney,
				domain->links.sparse[PTRN_U]->grid[0].bx +
				domain->links.sparse[PTRN_U]->grid[0].ex);

			// Edge (X, Y, 0) subdomain.
			// Link BACK edge to the FRONT edge
			// of the BACK neighbour.
			GRID(0, 0, nx, 0, 0, ny, 0, 1, bs, 1, 0, 0, bs);
			GRID(1, 0, nx, 0, 0, ny, 0, 0, bs, 2, 0, 0, 0);
			LINK(PTRN_B, domain->grid[0].bs * nbs,
				domain->links.sparse[PTRN_B]->grid[0].bx +
				domain->links.sparse[PTRN_B]->grid[0].ex +
				domain->links.sparse[PTRN_B]->grid[0].by +
				domain->links.sparse[PTRN_B]->grid[0].ey +
				domain->links.sparse[PTRN_B]->grid[0].bs);

			// Edge (X, Y, 1) subdomain.			
			// Link FRONT edge to the BACK edge
			// of the FRONT neighbour.
			GRID(0, 0, nx, 0, 0, ny, 0, 1, es, 1, 0, 0, ns - 2 * es);
			GRID(1, 0, nx, 0, 0, ny, 0, 2, es, 0, 0, 0, ns - es);
			LINK(PTRN_F, domain->grid[0].es * nes,
				domain->links.sparse[PTRN_F]->grid[0].bx +
				domain->links.sparse[PTRN_F]->grid[0].ex +
				domain->links.sparse[PTRN_F]->grid[0].by +
				domain->links.sparse[PTRN_F]->grid[0].ey);
			
			assert(idomain <= nsubdomains);
		}
	}

	// Set domains overheads without fictive empty domain.
	free(empty);
	overhead(domains, NULL, 0,
		sx, sy, ss, incx, incy, incs,
		bx, ex, by, ey, bs, es);

	// Multiply domains overheads by the actual
	// overheads values.
	for (int iv = 0; iv < sv; iv++)
	{
		struct domain_t* domain = domains + iv;

		domain->grid[0].bx *= bx; domain->grid[0].ex *= ex;
		domain->grid[0].by *= by; domain->grid[0].ey *= ey;
		domain->grid[0].bs *= bs; domain->grid[0].es *= es;

		domain->grid[1] = domain->grid[0];

		int nsubdomains = domain->nsubdomains;
		domain->subdomains = subdomains + iv * PTRN_N;

		for (int i = 0; i < nsubdomains; i++)
		{
			struct domain_t* sub = domain->subdomains + i;
		
			sub->grid[0].bx *= bx; sub->grid[0].ex *= ex;
			sub->grid[0].by *= by; sub->grid[0].ey *= ey;
			sub->grid[0].bs *= bs; sub->grid[0].es *= es;			

			sub->grid[1].bx *= bx; sub->grid[1].ex *= ex;
			sub->grid[1].by *= by; sub->grid[1].ey *= ey;
			sub->grid[1].bs *= bs; sub->grid[1].es *= es;
		}
	}

	// Calculate domains sizes.
	for (int iv = 0; iv < sv + 1; iv++)
	{
		struct domain_t* domain = domains + iv;

		// Caluclate normal and extended domain size
		// for the domain primary grid.
		domain->grid[0].size = 
			domain->grid[0].nx * domain->grid[0].ny * domain->grid[0].ns;
		domain->grid[0].extsize = 
			(domain->grid[0].bx + domain->grid[0].nx + domain->grid[0].ex) *
			(domain->grid[0].by + domain->grid[0].ny + domain->grid[0].ey) *
			(domain->grid[0].bs + domain->grid[0].ns + domain->grid[0].es);

		// Caluclate normal and extended domain size
		// for the domain secondary grid.
		domain->grid[1].size = 
			domain->grid[1].nx * domain->grid[1].ny * domain->grid[1].ns;
		domain->grid[1].extsize = 
			(domain->grid[1].bx + domain->grid[1].nx + domain->grid[1].ex) *
			(domain->grid[1].by + domain->grid[1].ny + domain->grid[1].ey) *
			(domain->grid[1].bs + domain->grid[1].ns + domain->grid[1].es);

		int nsubdomains = domain->nsubdomains;
		for (int i = 0; i < nsubdomains; i++)
		{
			struct domain_t* sub = domain->subdomains + i;

			// Caluclate normal and extended domain size
			// for the subdomain primary grid.		
			sub->grid[0].size =
				sub->grid[0].nx * sub->grid[0].ny * sub->grid[0].ns;
			sub->grid[0].extsize =
				(sub->grid[0].bx + sub->grid[0].nx + sub->grid[0].ex) *
				(sub->grid[0].by + sub->grid[0].ny + sub->grid[0].ey) *
				(sub->grid[0].bs + sub->grid[0].ns + sub->grid[0].es);

			// Caluclate normal and extended domain size
			// for the subdomain secondary grid.		
			sub->grid[1].size =
				sub->grid[1].nx * sub->grid[1].ny * sub->grid[1].ns;
			sub->grid[1].extsize =
				(sub->grid[1].bx + sub->grid[1].nx + sub->grid[1].ex) *
				(sub->grid[1].by + sub->grid[1].ny + sub->grid[1].ey) *
				(sub->grid[1].bs + sub->grid[1].ns + sub->grid[1].es);
		}
	}
	
	// Perform consistency checks.
	int all_null = 1;
	for (int iv = 0; iv < sv; iv++)
	{
		struct domain_t* domain = domains + iv;

		int nsubdomains = domain->nsubdomains;
		for (int i = 0; i < nsubdomains; i++)
		{
			struct domain_t* subdomain1 = domain->subdomains + i;

			// Positive relative poistion indexes.
			assert(subdomain1->grid[0].ix >= 0);
			assert(subdomain1->grid[0].iy >= 0);
			assert(subdomain1->grid[0].is >= 0);

			assert(subdomain1->grid[1].ix >= 0);
			assert(subdomain1->grid[1].iy >= 0);
			assert(subdomain1->grid[1].is >= 0);
		
			// For each domain every subdomain must be linked
			// with domain's neighbour and vise versa.
			struct domain_t* subdomain2 = *(subdomain1->links.dense[0]);
			if (subdomain2)
			{
				assert(subdomain1->grid[0].extsize ==
					subdomain2->grid[0].extsize);
				assert(subdomain1 == *(subdomain2->links.dense[0]));
				all_null = 0;
			}
		}
	}
	assert(!all_null);
};

