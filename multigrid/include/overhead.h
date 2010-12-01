#pragma once

// Set normilized domains overheads (0 or 1).
void overhead(
	struct domain_t* domains, struct domain_t* empty, int igrid,
	int sx, int sy, int ss, int incx, int incy, int incs,
	int bx, int ex, int by, int ey, int bs, int es);

