#pragma once

#include "grid.h"

/**
 * Compose continuous arrays given on distributed subdomains
 * into global grid space using mapping parameters specified
 * by decomposition descriptor.
 */
void gather(struct domain_t* domains, int igrid,
	char** arrays, int narrays, int szelem,
	int layout);

