#pragma once

#define LAYOUT_MODE_AUTO   0
#define LAYOUT_MODE_CUSTOM 1

#define PTRN_C 0
#define PTRN_L 1
#define PTRN_R 2
#define PTRN_D 3
#define PTRN_U 4
#define PTRN_B 5
#define PTRN_F 6
#define PTRN_N 7

#include <stddef.h> // size_t

typedef void* (*memcpy_func_t)(void* dst, const void* src, size_t size);

struct domain_t;

struct pattern_t
{
	struct domain_t* sparse[PTRN_N];
	struct domain_t** dense[PTRN_N];
	
	// The number of non-null elements in pattern.
	int ndense;
};

struct grid_t
{
	int nx, ny, ns;
	int bx, by, bs;
	int ex, ey, es;

	int size, extsize;

	// The position of domain relative to its parent.
	int ix, iy, is;
};

// Define a set of a domain-specific parameters.
struct domain_t
{
	// Primary and secondary domain grids.
	struct grid_t grid[2];

	char** arrays;
	int narrays, szelem;

	// A pointer to access the parent domain
	// configuration. Top level domains should have
	// NULL parent.
	struct domain_t* parent;

	// A pointer to access the nested domains -
	// the domains of inner grid decomposition. Domains
	// without nested subdomains should have NULL subdomains.
	struct domain_t* subdomains;
	int nsubdomains;

	// Nested domains subgrid.
	int sx, sy, ss;

#ifdef HAVE_COACCEL

	// The COACCEL compatible device mode
	// for I/O pipeline between memory levels
	// (should be defined in coaccel_devmodes.h).
	int devmode;

#endif

	memcpy_func_t load, save;

	// Indirectly linked domains array.
	struct pattern_t links;
};

// Configure decomposition grid.
void setgrid(struct domain_t** domains,
	int nx, int ny, int ns,	int sx, int sy, int ss,
	int bx, int ex, int by, int ey, int bs, int es);

