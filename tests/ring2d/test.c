// Test base name.
const char* name = "ring2d";

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
	struct test_config_t* t = test_init(n, 3, &size);

	// Create GrADS visualization ctl file.
	test_create_grads_ctl(n, nt, name);
	
	// Create GrADS visualization gs script.
	test_create_grads_gs(n, nt, name);

	// Create GrADS visualization gs script.
	test_create_grads_pl(name);

	char filename[20];
	sprintf(filename, "%s.bin", name);
	FILE* fp = fopen(filename, "wb");

	fwrite(t->u + n2, 1, n2b, fp);
	fwrite(t->v + n2, 1, n2b, fp);
	fwrite(t->c0 + n2, 1, n2b, fp);

	printf("step 0\n");

	for (int it = 1; it <= nt; it++)
	{
		smolar(t->n, t->n, t->m, t->dx, t->dy, t->ds, t->dt,
			t->u, t->u, t->v, t->v, t->w, t->w,
			&t->c0, &t->c1, t->flow_type, t->order);

		fwrite(t->u + n2, 1, n2b, fp);
		fwrite(t->v + n2, 1, n2b, fp);
		fwrite(t->c1 + n2, 1, n2b, fp);
		
		printf("step %d %f\n", it,
			t->c1[n / 2 + t->xof + n * (n / 2 + t->yof) + n2]);

		real* c = t->c0;
		t->c0 = t->c1; t->c1 = c;
	}

	fclose(fp);

	free(t);

	return 0;
}

