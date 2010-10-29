#include "gather.h"
#include "subcpy.h"

#include <assert.h>

void gather(struct domain_t* domains, int igrid,
	char** arrays, int narrays, int szelem,
	int layout)
{
	struct domain_t* parent = domains->parent;

	int nx = parent->grid[igrid].nx;
	int ny = parent->grid[igrid].ny;
	int ns = parent->grid[igrid].ns;

	int bx = parent->grid[igrid].bx;
	int by = parent->grid[igrid].by;
	int bs = parent->grid[igrid].bs;

	int ex = parent->grid[igrid].ex;
	int ey = parent->grid[igrid].ey;
	int es = parent->grid[igrid].es;

	int sx = parent->sx, sy = parent->sy, ss = parent->ss;
	
	// Region dimensions in bytes.
	int bnx = nx * szelem, bny = ny, bns = ns;
	int bbx = bx * szelem, bby = by, bbs = bs;
	int bex = ex * szelem, bey = ey, bes = es;

	// With automatic layout data offsets are generated,
	// presuming subdomains should go one after another,
	// without spacings.
	if (layout == LAYOUT_MODE_AUTO)
	{
		// Set data for inner (non-boundary) subdomains.
		int sv = sx * sy * ss, iv = 0;
		for (int is = 0, offset = 0, offmuls = 0; is < ss; is++)
		{
			for (int iy = 0, offmuly = 0; iy < sy; iy++)
			{
				for (int ix = 0; ix < sx; ix++, iv++)
				{
					struct domain_t* domain = domains + iv;
				
					offmuly = domain->grid[igrid].ny - 1;
					offmuls = domain->grid[igrid].ns - 1;

					int snx = domain->grid[igrid].nx;
					int sny = domain->grid[igrid].ny;
					int sns = domain->grid[igrid].ns;
				
					int sbx = domain->grid[igrid].bx;
					int sex = domain->grid[igrid].ex;

					int sby = domain->grid[igrid].by;
					int sey = domain->grid[igrid].ey;

					int sbs = domain->grid[igrid].bs;
					int ses = domain->grid[igrid].es;
				
					int bsnx = szelem * (sbx + snx + sex);
					int bsny = (sby + sny + sey);
					int bsns = (sbs + sns + ses);

					for (int i = 0; i < narrays; i++)
						subcpy(szelem * snx, sny, sns,
							arrays[i] + offset,
							bbx + bnx + bex, bby + bny + bey,
							bbs + bns + bes,
							domain->arrays[i] + szelem *
							(sbx + sby * (sbx + snx + sex) +
							       sbs * (sbx + snx + sex) *
								     (sby + sny + sey)),
							bsnx, bsny, bsns, domain->save);
				
					offset += domain->grid[igrid].nx * szelem;
				}
			
				offset += nx * offmuly * szelem;
			}

			offset += nx * ny * offmuls * szelem;
		}

		assert(iv == sv);
		return;
	}

	// With custom layout offsets are generated
	// from the predefined (ix, iy, is) positions
	// given for each subdomain.
	if (layout == LAYOUT_MODE_CUSTOM)
	{
		int nsubdomains = parent->nsubdomains;

		// Set data for inner (non-boundary) subdomains.
		for (int iv = 0; iv < nsubdomains; iv++)
		{
			struct domain_t* domain = domains + iv;

			int snx = domain->grid[igrid].nx;
			int sny = domain->grid[igrid].ny;
			int sns = domain->grid[igrid].ns;
		
			int sbx = domain->grid[igrid].bx;
			int sex = domain->grid[igrid].ex;

			int sby = domain->grid[igrid].by;
			int sey = domain->grid[igrid].ey;

			int sbs = domain->grid[igrid].bs;
			int ses = domain->grid[igrid].es;
		
			int bsnx = szelem * (sbx + snx + sex);
			int bsny = (sby + sny + sey);
			int bsns = (sbs + sns + ses);
			
			int ix = domain->grid[igrid].ix;
			int iy = domain->grid[igrid].iy;
			int is = domain->grid[igrid].is;

			int offset = szelem * (ix +
				iy * (bx + nx + ex) +
				is * (bx + nx + ex) * (by + ny + ey));

			for (int i = 0; i < narrays; i++)
				subcpy(szelem * snx, sny, sns,
					arrays[i] + offset,
					bbx + bnx + bex, bby + bny + bey, bbs + bns + bes,
					domain->arrays[i] + szelem *
					(sbx + sby * (sbx + snx + sex) +
					       sbs * (sbx + snx + sex) *
						     (sby + sny + sey)),
					bsnx, bsny, bsns, domain->save);
		}
		return;
	}
}

