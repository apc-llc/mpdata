#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <sstream>

#include <vapor/OptionParser.h>
#include <vapor/MetadataVDC.h>
#include <vapor/WaveCodecIO.h>
#include <vapor/VDCFactory.h>
#include <vapor/CFuncs.h>
#ifdef WIN32
#pragma warning(disable : 4996 4800)
#endif
using namespace VetsUtil;
using namespace VAPoR;


struct opt_t {
	OptionParser::Dimension3D_T	dim;
	OptionParser::Boolean_T	help;
	OptionParser::Boolean_T	vdc2;
};

static opt_t opt;

static OptionParser::OptDescRec_T	set_opts[] = {
	{"dimension",1, "512x512x512",	"Data volume dimensions expressed in "
		"grid points (NXxNYxNZ)"},
	{"help",	0,	"",	"Print this message and exit"},
	{
		"vdc2", 0,  "",
		"Generate a VDC Type 2 .vdf file (default is VDC Type 1)"
	},

	{NULL}
};


static OptionParser::Option_T	get_options[] = {
	{"dimension", VetsUtil::CvtToDimension3D, &opt.dim, sizeof(opt.dim)},
	{"help", VetsUtil::CvtToBoolean, &opt.help, sizeof(opt.help)},
	{"vdc2", VetsUtil::CvtToBoolean, &opt.vdc2, sizeof(opt.vdc2)},
	{NULL}
};

static string ProgName;


extern "C" int vdfcreate(int argc, char **argv) {

	OptionParser op;

	MyBase::SetErrMsgFilePtr(stderr);
	//
	// Parse command line arguments
	//
	ProgName = Basename(argv[0]);


	if (op.AppendOptions(set_opts) < 0) {
		exit(1);
	}

	if (op.ParseOptions(&argc, argv, get_options) < 0) {
		exit(1);
	}

	VDCFactory	vdcf(opt.vdc2);
	if (vdcf.Parse(&argc, argv) < 0) {
		exit(1);
	}

	if (opt.help) {
		cerr << "Usage: " << ProgName << " [options] filename" << endl;
		op.PrintOptionHelp(stderr, 80, false);
		vdcf.Usage(stderr);
		exit(0);
	}

	if (argc != 2) {
		cerr << "Usage: " << ProgName << " [options] filename" << endl;
		op.PrintOptionHelp(stderr);
		vdcf.Usage(stderr);
		exit(1);
	}

	MetadataVDC *file;

	size_t dims[] = {opt.dim.nx, opt.dim.ny, opt.dim.nz};
	file = vdcf.New(dims);
	if (! file) exit(1);

	if (file->GetVariableNames().size() == 0) {
		vector <string> vars(1, "var1");
		file->SetVariables3D(vars);
	}



	if (file->Write(argv[1]) < 0) {
		exit(1);
	}
	return(0);
}
