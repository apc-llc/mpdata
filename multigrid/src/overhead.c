#include "grid.h"

#include <assert.h>
#include <stdint.h>

#define SET_OVERLAPS(bxv, exv, byv, eyv, bsv, esv) \
{ \
	struct domain_t* domain = domains + iv; \
	domain->grid[igrid].bx = bxv; domain->grid[igrid].ex = exv; \
	domain->grid[igrid].by = byv; domain->grid[igrid].ey = eyv; \
	domain->grid[igrid].bs = bsv; domain->grid[igrid].es = esv; \
}

#define SET_LINKS(bx, ex, by, ey, bs, es) \
{ \
	struct domain_t* domain = domains + iv; \
	struct domain_t** dlinks = (domain->links.sparse); \
	dlinks[PTRN_L] = (struct domain_t*)((1 - bx) * (intptr_t)empty); \
	dlinks[PTRN_R] = (struct domain_t*)((1 - ex) * (intptr_t)empty); \
	dlinks[PTRN_D] = (struct domain_t*)((1 - by) * (intptr_t)empty); \
	dlinks[PTRN_U] = (struct domain_t*)((1 - ey) * (intptr_t)empty); \
	dlinks[PTRN_B] = (struct domain_t*)((1 - bs) * (intptr_t)empty); \
	dlinks[PTRN_F] = (struct domain_t*)((1 - es) * (intptr_t)empty); \
\
	char** clinks = (char**)(domain->links.sparse); \
	clinks[PTRN_L] += (bx * (intptr_t)(domains + iv - 1)); \
	clinks[PTRN_R] += (ex * (intptr_t)(domains + iv + 1)); \
	clinks[PTRN_D] += (by * (intptr_t)(domains + iv - sx)); \
	clinks[PTRN_U] += (ey * (intptr_t)(domains + iv + sx)); \
	clinks[PTRN_B] += (bs * (intptr_t)(domains + iv - sx * sy)); \
	clinks[PTRN_F] += (es * (intptr_t)(domains + iv + sx * sy)); \
\
	int iptrn = 0; \
	domain->links.dense[iptrn] = &(dlinks[PTRN_L]); iptrn += bx; \
	domain->links.dense[iptrn] = &(dlinks[PTRN_R]); iptrn += ex; \
	domain->links.dense[iptrn] = &(dlinks[PTRN_D]); iptrn += by; \
	domain->links.dense[iptrn] = &(dlinks[PTRN_U]); iptrn += by; \
	domain->links.dense[iptrn] = &(dlinks[PTRN_B]); iptrn += bs; \
	domain->links.dense[iptrn] = &(dlinks[PTRN_F]); \
}

#define SET_DOMAIN(bx, ex, by, ey, bs, es) \
	SET_OVERLAPS(bx, ex, by, ey, bs, es); \
	SET_LINKS(bx, ex, by, ey, bs, es);

// Set normilized domains overheads (0 or 1).
void overhead(
	struct domain_t* domains, struct domain_t* empty, int igrid,
	int sx, int sy, int ss, int incx, int incy, int incs,
	int bx, int ex, int by, int ey, int bs, int es)
{
	// For each domain set the local data overhead.
	{
		int iv = 0;
		
		SET_DOMAIN(0, ex, 0, ey, 0, es);
		iv += incx;

		for (int ix = 1; ix < sx - 1; ix++, iv++)
		{
			SET_DOMAIN(bx, ex, 0, ey, 0, es);
		}

		SET_DOMAIN(bx, 0, 0, ey, 0, es);
		iv += incy;

		for (int iy = 1; iy < sy - 1; iy++)
		{
			SET_DOMAIN(0, ex, by, ey, 0, es);
			iv += incx;

			for (int ix = 1; ix < sx - 1; ix++, iv++)
			{
				SET_DOMAIN(bx, ex, by, ey, 0, es);
			}

			SET_DOMAIN(bx, 0, by, ey, 0, es);
			iv++;
		}

		SET_DOMAIN(0, ex, by, 0, 0, es);
		iv += incx;

		for (int ix = 1; ix < sx - 1; ix++, iv++)
		{
			SET_DOMAIN(bx, ex, by, 0, 0, es);
		}

		SET_DOMAIN(bx, 0, by, 0, 0, es);
		iv += incs;

		for (int is = 1; is < ss - 1; is++)
		{
			SET_DOMAIN(0, ex, 0, ey, bs, es);
			iv += incx;

			for (int ix = 1; ix < sx - 1; ix++, iv++)
			{
				SET_DOMAIN(bx, ex, 0, ey, bs, es);
			}

			SET_DOMAIN(bx, 0, 0, ey, bs, es);
			iv += incy;

			for (int iy = 1; iy < sy - 1; iy++)
			{
				SET_DOMAIN(0, ex, by, ey, bs, es);
				iv += incx;

				for (int ix = 1; ix < sx - 1; ix++, iv++)
				{
					SET_DOMAIN(bx, ex, by, ey, bs, es);
				}
				
				SET_DOMAIN(bx, 0, by, ey, bs, es);
				iv++;
			}

			SET_DOMAIN(0, ex, by, 0, bs, es);
			iv += incx;

			for (int ix = 1; ix < sx - 1; ix++, iv++)
			{
				SET_DOMAIN(bx, ex, by, 0, bs, es);
			}

			SET_DOMAIN(bx, 0, by, 0, bs, es);
			iv++;
		}

		SET_DOMAIN(0, ex, 0, ey, bs, 0);
		iv += incx;

		for (int ix = 1; ix < sx - 1; ix++, iv++)
		{
			SET_DOMAIN(bx, ex, 0, ey, bs, 0);
		}

		SET_DOMAIN(bx, 0, 0, ey, bs, 0);
		iv += incy;

		for (int iy = 1; iy < sy - 1; iy++)
		{
			SET_DOMAIN(0, ex, by, ey, bs, 0);
			iv += incx;

			for (int ix = 1; ix < sx - 1; ix++, iv++)
			{
				SET_DOMAIN(bx, ex, by, ey, bs, 0);
			}

			SET_DOMAIN(bx, 0, by, ey, bs, 0);
			iv++;
		}

		SET_DOMAIN(0, ex, by, 0, bs, 0);
		iv += incx;

		for (int ix = 1; ix < sx - 1; ix++, iv++)
		{
			SET_DOMAIN(bx, ex, by, 0, bs, 0);
		}

		SET_DOMAIN(bx, 0, by, 0, bs, 0);
		iv++;

		assert(iv == sx * sy * ss);
	}
}

