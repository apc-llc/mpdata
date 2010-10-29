#pragma once

#include "grid.h"

/**
 * Decompose arrays given for the specified global grids
 * into continuous subdomains on subgrids selected according
 * to the mapping parameters.
 */
void scatter(struct domain_t* domains, int igrid,
	char** arrays, int narrays, int szelem,
	int layout);

