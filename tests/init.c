#include "init.h"
#include "gengauss2d.h"
#include "gengauss3d.h"
#include "genringf2d.h"

#include <malloc.h>
#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h> // mkdir

// Initialize a test instance on the given grid.
// Reports the test configuration record size.
struct test_config_t* test_init(int n, int m, int* size /** OUT **/)
{
	int nb = n * sizeof(real), n2 = n * n, n2b = n2 * sizeof(real);

	// The wind flow circle inner and outer radiuses.
	real rmin = 1, rmax = n / 2.5f;

	// The time step.
	real dt = 0.0125;

	// Gauss source initial distribution parameters.
	int xof = n / 4, yof = 0;
	real sigma = 6.0f, radius = n / 8;

	// MPDATA scheme parameters.
	int order = 3, flow_type = 1;

	// Setting up the data storage.
	*size = sizeof(struct test_config_t) + n2b * m * 5 + 2 * nb + m * sizeof(real);
	struct test_config_t* t = (struct test_config_t*)malloc(*size);
	memset(t, 0, *size);
	real* mptr = (real*)(t + 1); 
	
	t->n = n; t->m = m; t->dt = dt;
	t->order = order; t->flow_type = flow_type;
	
	// Wind flow components (w is zero).
	t->u = mptr; mptr += n2 * m;
	t->v = mptr; mptr += n2 * m;
	t->w = mptr; mptr += n2 * m;

	// Grid steps.
	t->dx = mptr; mptr += n;
	t->dy = mptr; mptr += n;
	t->ds = mptr; mptr += m;

	for (int i = 0; i < m; i++)
	{
		t->ds[i] = 1;
	}
	
	// Set uniform grid.
	for (int i = 0; i < n; i++)
	{
		t->dx[i] = t->dy[i] = 1.0;
	}
	
	// If third dimension is meaningful, assume the
	// 3d flow is tested, and constant vertical speed.
	if (m > 3)
	{
		for (int i = 0; i < n2 * m; i++)
			t->w[i] = 1.0;
	}

	t->xof = xof; t->yof = yof;	

	// Populate wind flow components with faded ringed flow.
	genringf2d(n / 2, n / 2, n, n, t->dx, t->dy,
		rmin, rmax, t->u, t->v, 1.0, 0);
	
	// Clone ring flow on all vertical levels.	
	for (int i = 1; i < m; i++)
	{
		memcpy(t->u + i * n2, t->u, n2b);
		memcpy(t->v + i * n2, t->v, n2b);
	}

	// Traced concentrations.
	t->c0 = mptr; mptr += n2 * m;
	t->c1 = mptr;

	// Set Gauss source initial distribution,
	// taking in account 2d/3d mode.
	if (m > 3)
	{
		gengauss3d(n / 2 + xof, n / 2 + yof, radius, n, n, m,
			radius, sigma, sigma, sigma, t->c0, 1.0);
	}
	else
	{
		gengauss2d(n / 2 + xof, n / 2 + yof, n, n,
			radius, sigma, sigma, t->c0, 1.0);
		for (int i = 1; i < m; i++)
			memcpy(t->c0 + i * n2, t->c0, n2b);
	}
		
	return t;
}

// Clone the specified test config.
struct test_config_t* test_clone(int size, struct test_config_t* t)
{
	struct test_config_t* r = (struct test_config_t*)malloc(size);

	memcpy(r, t, size);

	r->n = t->n; r->m = t->m;
	r->dt = t->dt;
	r->u = t->u; r->v = t->v; r->w = t->w;
	r->c0 = t->c0; r->c1 = t->c1;
	r->dx = t->dx; r->dy = t->dy; r->ds = t->ds;
	r->order = t->order; r->flow_type = t->flow_type;
	r->xof = t->xof; r->yof = t->yof;
	
	return r;
}

// Create GrADS visualization ctl file.
void test_create_grads_ctl(int n, int nt, const char* name)
{
	char filename[20];
	sprintf(filename, "%s.ctl", name);
	FILE* fp = fopen(filename, "w");

	fprintf(fp, "DSET ^%s.bin\n", name);
	fprintf(fp, "XDEF %d LINEAR 0 1\n", n);
	fprintf(fp, "YDEF %d LINEAR 0 1\n", n);
	fprintf(fp, "ZDEF 1 LINEAR 0 1\n");
	
	fprintf(fp, "EDEF 3 NAMES u v c\n");
	fprintf(fp, "TDEF 4001 LINEAR 18:00Z04jul2000 1hr\n");
	fprintf(fp, "UNDEF 1E+300\n");
	
	fprintf(fp, "VARS 3\n");
	fprintf(fp, "u 1 -1 uwind\n");
	fprintf(fp, "v 1 -1 vwind\n");
	fprintf(fp, "c 1 -1 source\n");
	fprintf(fp, "ENDVARS\n");
	
	fclose(fp);
}

// Create GrADS visualization gs script.
void test_create_grads_gs(int n, int nt, const char* name)
{
	char filename[20];
	sprintf(filename, "%s.gs", name);
	FILE* fp = fopen(filename, "w");

	fprintf(fp, "'open '\"%s.ctl\"\n", name);
	for (int it = 0; it < nt; it++)
	{
		fprintf(fp, "'set parea '0.5' '10.5' '1.5' '7.5\n");
		fprintf(fp, "'set xaxis '0' '%d' '16\n", n);
		fprintf(fp, "'set yaxis '0' '%d' '16\n", n);
		
		fprintf(fp, "'set mproj 'off\n");
		fprintf(fp, "'set mpdraw 'off\n");
		fprintf(fp, "'set gxout 'shaded\n");
		
		fprintf(fp, "'set clevs '%f' '%f' '%f' '%f' '%f'",
			0.0, 0.1, 0.2, 0.3, 0.4);
		fprintf(fp, "'%f' '%f' '%f' '%f' '%f' '%f\n",
			0.5, 0.6, 0.7, 0.8, 0.9, 1.0);

		fprintf(fp, "'set t '%d\n", it + 1);
		fprintf(fp, "'display 'c\n");
		fprintf(fp, "'draw title '\"Source in %s flow - step %04d\"\n",
			name, it);
		fprintf(fp, "'run 'cbarm.gs\n");
		fprintf(fp, "'set gxout 'vector\n");

		fprintf(fp, "'display '\"skip(u,4); skip(v,4); mag(u,v)\"\n");
		fprintf(fp, "'printim '\"%s_frames/step%04d.png\"' 'png' 'x800' 'y1024\n",
			name, it);
		fprintf(fp, "'clear 'graphics\n");
	}

	fclose(fp);
}

// Create GrADS visualization perl script.
void test_create_grads_pl(const char* name)
{
	// Create directory to store traces.
	char path[20];
	sprintf(path, "%s_frames", name);
	mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);

	char filename[20];
	sprintf(filename, "%s.pl", name);
	FILE* fp = fopen(filename, "w");

	fprintf(fp, "#!/usr/bin/perl -w\n");
	fprintf(fp, "system(\"grads -lbxc \\\"run %s.gs\\\"\");\n", name);

	fclose(fp);
}

int vdfcreate(int argc, char* argv[]);

// Create a data file in UCAR Vapor format.
void test_create_vapor_vdf(int n, int m, int nt, const char* name)
{
	// Prepare arguments for vdfcreate routine.
	char* argv[] =
	{
		"vdfcreate",
		"-dimension", "%dx%dx%d",
		"-numts", "%d",
		"-vars3d", "u:v:w:c",
		"%s.vdf"
	};
	int argc = sizeof(argv) / sizeof(char*);
	
	char dims[20];
	sprintf(dims, argv[2], n, n, m);
	argv[2] = dims;
	
	char numts[10];
	sprintf(numts, argv[4], nt);
	argv[4] = numts;
	
	char filename[20];
	sprintf(filename, argv[7], name);
	argv[7] = filename;
	
	// Create vdf file with UCAR Vapor vdfcreate.
	vdfcreate(argc, argv);
}

int raw2vdf(int argc, char* argv[]);

// Write step variables into the previously created
// UCAR Vapor format data file.
void test_write_vapor_vdf(int n, int m,
	int it, const char* name,
	real* u, real* v, real* w, real* c)
{
	char* margv[] =
	{
		"raw2vdf",
		"-quiet",
		"-nthreads", "%d",
		"-ts", "%d",
		"-varname", "",
		"%s.vdf", "data.bin"
	};
	int argc = sizeof(margv) / sizeof(char*);
	char* argv[argc];

	char nthreads[10];
	#pragma omp parallel
	{
		#pragma omp master
		{
			sprintf(nthreads, margv[3], omp_get_num_threads());
		}
	}
	margv[3] = nthreads;

	char ts[10];
	sprintf(ts, margv[5], it);
	margv[5] = ts;
	
	char filename[20];
	sprintf(filename, margv[8], name);
	margv[8] = filename;
	
	int n2mb = sizeof(real) * n * n * m;

	FILE* fp;

	// Write u variable.
	fp = fopen("data.bin", "wb");
	fwrite(u, 1, n2mb, fp);
	fclose(fp);
	margv[7] = "u";

	// Write to vdf file with UCAR Vapor raw2vdf.
	memcpy(argv, margv, argc * sizeof(char*));
	raw2vdf(argc, argv);

	// Write u variable.
	fp = fopen("data.bin", "wb");
	fwrite(v, 1, n2mb, fp);
	fclose(fp);
	margv[7] = "v";

	// Write to vdf file with UCAR Vapor raw2vdf.
	memcpy(argv, margv, argc * sizeof(char*));
	raw2vdf(argc, argv);

	// Write u variable.
	fp = fopen("data.bin", "wb");
	fwrite(w, 1, n2mb, fp);
	fclose(fp);
	margv[7] = "w";

	// Write to vdf file with UCAR Vapor raw2vdf.
	memcpy(argv, margv, argc * sizeof(char*));
	raw2vdf(argc, argv);

	// Write c variable.
	fp = fopen("data.bin", "wb");
	fwrite(c, 1, n2mb, fp);
	fclose(fp);
	margv[7] = "c";

	// Write to vdf file with UCAR Vapor raw2vdf.
	memcpy(argv, margv, argc * sizeof(char*));
	raw2vdf(argc, argv);
}

