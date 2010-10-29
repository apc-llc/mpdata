#include "subcpy.h"

#include <assert.h>

// Copy 3d source subdomain to destanation domain.
void subcpy(int nx, int ny, int ns,
	char* dst, int dnx, int dny, int dns,
	char* src, int snx, int sny, int sns,
	memcpy_func_t memcpy_func)
{
	assert(dst); assert(src);

	int dnp = dnx * dny, snp = snx * sny;

	for (int is = 0; is < ns; is++)
		for (int iy = 0; iy < ny; iy++)
			memcpy_func(dst + is * dnp + iy * dnx,
				src + is * snp + iy * snx, nx);
}

