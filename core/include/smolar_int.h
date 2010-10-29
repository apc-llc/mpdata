#pragma once

#include "smolar.h"

#define ui unsigned int
#define ull unsigned long long

#define abs(a) ((a) > 0 ? (a) : -(a))

// XXX: set to check Courant condition on steps/speeds.
#define CHK_COURANT(val)

#ifdef SINGLE
#define ONE	1.0f
#define HALF	0.5f
#define QUAT	0.25f
#include <float.h>
#define MINVAL	FLT_MIN
#else
#ifdef DOUBLE
#define ONE	1.0
#define HALF	0.5
#define QUAT	0.25
#include <float.h>
#define MINVAL	DBL_MIN
#else
#ifdef INTEGER
#define ONE	4
#define HALF	2
#define QUAT	1
#define MINVAL	1
#endif
#endif
#endif

