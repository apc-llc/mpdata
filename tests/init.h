#include "smolar.h"

// Defines test configuration parameters.
struct test_config_t
{
	void* desc;

	int n, m;
	real dt;
	real *u, *v, *w;
	real *c0, *c1;
	real *dx, *dy, *ds;
	int order, flow_type;
	
	int xof, yof;
};

// Initialize a test instance on the given grid.
// Reports the test configuration record size.
struct test_config_t* test_init(int n, int m, int* size /** OUT **/);

// Clone the specified test config.
struct test_config_t* test_clone(int size, struct test_config_t* t);

// Create GrADS visualization ctl file.
void test_create_grads_ctl(int n, int nt, const char* name);

// Create GrADS visualization gs script.
void test_create_grads_gs(int n, int nt, const char* name);

// Create GrADS visualization perl script.
void test_create_grads_pl(const char* name);

// Create a data file in UCAR Vapor format.
void test_create_vapor_vdf(int n, int m, int nt, const char* name);

// Write step variables into the previously created
// UCAR Vapor format data file.
void test_write_vapor_vdf(int n, int m,
	int it, const char* name,
	real* u, real* v, real* w, real* c);

