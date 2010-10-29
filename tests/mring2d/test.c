// Test base name.
const char* name = "mring2d";

#include "init.h"
#include "smolar.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The max(abs(smolar - msmolar)) difference allowed
// in floating-point tests.
#ifdef INTEGER
#define eps 0 // require exact match
#else
#define eps 1e-06 // floating-point tolerance
#endif

// Set to perform gather on each step and write
// distributed mode result.
//#define DISTOUT

int main(int argc, char* argv[])
{
	char usage[40];
	sprintf(usage, "%s <n> <nt> <sx> <sy> <ss> <mode>", name);

	if (argc != 7)
	{
		printf("%s\n", usage);
		return 0;
	}

	int mode = -1;
	
	if (!strcmp(argv[6], "serial")) 	mode = MSMOLAR_SERIAL;
	if (!strcmp(argv[6], "pthreads")) 	mode = MSMOLAR_PTHREADS;
	if (!strcmp(argv[6], "openmp"))		mode = MSMOLAR_OPENMP;
	
	if (mode == -1)
	{
		printf("%s\n", usage);
		printf("\tmodes supported: serial, pthreads, openmp\n");
		return 0;
	}
	
	int n = atoi(argv[1]), n2 = n * n, n2b = n2 * sizeof(real);
	int nt = atoi(argv[2]), size;
	
	int sx = atoi(argv[3]), sy = atoi(argv[4]), ss = atoi(argv[5]);

	// Create test configuration.
	struct test_config_t* t1 = test_init(n, 3, &size);
	struct test_config_t* t2 = test_clone(size, t1);
	t2->desc = NULL;

	// Create GrADS visualization ctl file.
	test_create_grads_ctl(n, nt, name);
	
	// Create GrADS visualization gs script.
	test_create_grads_gs(n, nt, name);

	// Create GrADS visualization gs script.
	test_create_grads_pl(name);

	char filename[20];
	sprintf(filename, "%s.bin", name);
	FILE* fp = fopen(filename, "wb");

	fwrite(t1->u + n2, 1, n2b, fp);
	fwrite(t1->v + n2, 1, n2b, fp);
	fwrite(t1->c0 + n2, 1, n2b, fp);

	printf("step 0\n");

#define abs(x) (((x) < 0) ? -(x) : (x))
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

#ifdef DISTOUT
	real* c = (real*)malloc(n2b);
#endif

	for (int it = 1; it <= nt; it++)
	{
		// Step serial smolar solver.
		smolar(t1->n, t1->n, t1->m, t1->dx, t1->dy, t1->ds, t1->dt,
			t1->u, t1->u, t1->v, t1->v, t1->w, t1->w,
			&t1->c0, &t1->c1, t1->flow_type, t1->order);

		// Step distributed smolar solver.
		msmolar(&t2->desc,
			t2->n, t2->n, t2->m, t2->dx, t2->dy, t2->ds, t2->dt,
			t2->u, t2->u, t2->v, t2->v, t2->w, t2->w,
			&t2->c0, &t2->c1, t2->flow_type, t2->order,
			sx, sy, ss, mode);

#ifdef DISTOUT
		msmolar_gather(&t2->desc,
			t2->n, t2->n, t2->m, t2->dx, t2->dy, t2->ds, t2->dt,
			t2->u, t2->u, t2->v, t2->v, t2->w, t2->w,
			&t2->c0, &t2->c1, t2->flow_type, t2->order,
			sx, sy, ss, mode);

		real *t1c = t1->c1 + n2, *t2c = t2->c1 + n2;
		real cmin = abs(t1c[0] - t2c[0]), cmax = cmin;
		for (int i = 1; i < n2; i++)
		{
			c[i] = abs(t1c[i] - t2c[i]);
			cmin = min(cmin, c[i]);
			cmax = max(cmax, c[i]);
		}

		fwrite(t2->u + n2, 1, n2b, fp);
		fwrite(t2->v + n2, 1, n2b, fp);
		fwrite(t2->c1 + n2, 1, n2b, fp);
		
		printf("step %d %f %f %f\n", it, cmin, cmax,
			c[n / 2 + t1->xof + n * (n / 2 + t1->yof)]);
#else
		fwrite(t1->u + n2, 1, n2b, fp);
		fwrite(t1->v + n2, 1, n2b, fp);
		fwrite(t1->c1 + n2, 1, n2b, fp);
		
		printf("step %d %f\n", it,
			t1->c1[n / 2 + t1->xof + n * (n / 2 + t1->yof) + n2]);
#endif

		// Swap serial smolar concentrations.
		real* c = t1->c0;
		t1->c0 = t1->c1; t1->c1 = c;

		// Swap distributed smolar concentrations.
		c = t2->c0;
		t2->c0 = t2->c1; t2->c1 = c;
	}

	msmolar_gather(&t2->desc,
		t2->n, t2->n, t2->m, t2->dx, t2->dy, t2->ds, t2->dt,
		t2->u, t2->u, t2->v, t2->v, t2->w, t2->w,
		&t2->c0, &t2->c1, t2->flow_type, t2->order,
		sx, sy, ss, mode);

	real *t1c = t1->c1 + n2, *t2c = t2->c1 + n2;
	for (int i = 0; i < n2; i++)
		assert(abs(t1c[i] - t2c[i]) <= eps);

#ifdef DISTOUT
	free(c);
#endif

	fclose(fp);

	free(t1);
	free(t2);

	printf("Test passed.\n");

	return 0;
}

