#pragma once

#include "pvelocity.h"

/*
 * Calculate the concentration distribution on the next time step.
 */
int smolar_pass(struct pvelocity* pv, real* c0, real* c1);

