// Test base name.
const char* name = "ring3d";

#include "init.h"
#include "smolar.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		char usage[20];
		sprintf(usage, "%s <n> <nt>", name);
		printf("%s\n", usage);
		return 0;
	}
	
	int n = atoi(argv[1]), n2 = n * n, n2b = n2 * sizeof(real);
	int nt = atoi(argv[2]), size;

	// Create test configuration.
	struct test_config_t* t = test_init(n, n, &size);

	// Create a data file in UCAR Vapor format.
	test_create_vapor_vdf(n, n, nt + 1, name);

	// Write step variables into the previously created
	// UCAR Vapor format data file.
	test_write_vapor_vdf(n, n, 0, name, t->u, t->v, t->w, t->c0);

	printf("step 0\n");

	for (int it = 1; it <= nt; it++)
	{
		smolar(t->n, t->n, t->m, t->dx, t->dy, t->ds, t->dt,
			t->u, t->u, t->v, t->v, t->w, t->w,
			&t->c0, &t->c1, t->flow_type, t->order);

		test_write_vapor_vdf(n, n, it, name, t->u, t->v, t->w, t->c1);
		
		printf("step %d\n", it);

		real* c = t->c0;
		t->c0 = t->c1; t->c1 = c;
	}

	free(t);

	return 0;
}

