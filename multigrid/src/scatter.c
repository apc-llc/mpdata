#include "scatter.h"
#include "subcpy.h"

#include <assert.h>

void scatter(struct domain_t* domains, int igrid,
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
	
	int nsubdomains = parent->nsubdomains;
	
	// Set arrays number and size of arrays elements
	// equal for all domains.
	for (int i = 0; i < nsubdomains; i++)
	{
		domains[i].narrays = narrays;
		domains[i].szelem = szelem;
	}
	
	// Region dimensions in bytes.
	int bnx = nx * szelem, bny = ny, bns = ns;
	int bbx = bx * szelem, bby = by, bbs = bs;
	int bex = ex * szelem, bey = ey, bes = es;

	// With automatic layout data offsets are generated,
	// presuming subdomains should go one after another,
	// without spacings.
	if (layout == LAYOUT_MODE_AUTO)
	{
		int sv = sx * sy * ss;
	
		// Set data for inner (non-boundary) subdomains.
		int iv = 0;
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

					int loffset = offset - szelem *
						(sbx + sby * nx + sbs * nx * ny);
					
					assert(loffset >= 0);

					for (int i = 0; i < narrays; i++)
						subcpy(bsnx, bsny, bsns,
							domain->arrays[i], bsnx, bsny, bsns,
							arrays[i] + loffset,
							bbx + bnx + bex, bby + bny + bey,
							bbs + bns + bes, domain->load);
				
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

			int loffset = szelem * (ix +
				iy * (bx + nx + ex) +
				is * (bx + nx + ex) * (by + ny + ey));
			
			loffset -= szelem * (sbx + (bx + nx + ex) *
				(sby + sbs * (by + ny + ey)));
			
			assert(loffset >= 0);

			for (int i = 0; i < narrays; i++)
				subcpy(bsnx, bsny, bsns,
					domain->arrays[i], bsnx, bsny, bsns,
					arrays[i] + loffset,
					bbx + bnx + bex, bby + bny + bey, bbs + bns + bes,
					domain->load);
		}

		return;
	}	
}

