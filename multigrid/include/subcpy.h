#include "grid.h"

// Copy 3d source subdomain to destanation domain.
void subcpy(int nx, int ny, int ns,
	char* dst, int dnx, int dny, int dns,
	char* src, int snx, int sny, int sns,
	memcpy_func_t memcpy_func);

