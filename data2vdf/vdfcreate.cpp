#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <sstream>

#include <vapor/OptionParser.h>
#include <vapor/Metadata.h>
#include <vapor/MetadataSpherical.h>

using namespace VetsUtil;
using namespace VAPoR;

namespace ns_vdfcreate
{

int	cvtToExtents(const char *from, void *to);
int	cvtTo3DBool(const char *from, void *to);
int	cvtToOrder(const char *from, void *to);
int	getUserTimes(const char *path, vector <double> &timevec);

struct opt_t {
	OptionParser::Dimension3D_T	dim;
	OptionParser::Dimension3D_T	bs;
	int	numts;
	int	level;
	int	nfilter;
	int	nlifting;
	char *comment;
	char *coordsystem;
	char *gridtype;
	char *usertimes;
	char *mapprojection;
	float extents[6];
	int order[3];
	int periodic[3];
	vector <string> varnames;
	vector <string> vars3d;
	vector <string> vars2dxy;
	vector <string> vars2dxz;
	vector <string> vars2dyz;
	OptionParser::Boolean_T	mtkcompat;
	OptionParser::Boolean_T	help;
} opt;

OptionParser::OptDescRec_T	set_opts[] = {
	{"dimension",1, "512x512x512",	"Data volume dimensions expressed in "
		"grid points (NXxNYxNZ)"},
	{"numts",	1, 	"1",			"Number of timesteps in the data set"},
	{"bs",		1, 	"32x32x32",		"Internal storage blocking factor "
		"expressed in grid points (NXxNYxNZ)"},
	{"level",	1, 	"0",		"Number of approximation levels in hierarchy. "
		"0 => no approximations, 1 => one approximation, and so on"},
	{"nfilter",	1, 	"1",			"Number of wavelet filter coefficients"},
	{"nlifting",1, 	"1",			"Number of wavelet lifting coefficients"},
	{"comment",	1,	"",				"Top-level comment to be included in VDF"},
	{"gridtype",	1,	"regular",	"Data grid type "
		"(regular|layered|stretched|block_amr)"}, 
	{"usertimes",	1,	"",	"Path to a file containing a whitespace "
		"delineated list of user times. If present, -numts "
		"option is ignored."}, 
	{"mapprojection",	1,	"",	"A whitespace delineated, quoted list "
        "of PROJ key/value pairs of the form '+paramname=paramvalue'. "
		"vdfcreate does not validate the string for correctness."},
	{"coordsystem",	1,	"cartesian","Data coordinate system "
		"(cartesian|spherical)"},
	{"extents",	1,	"0:0:0:0:0:0",	"Colon delimited 6-element vector "
		"specifying domain extents in user coordinates (X0:Y0:Z0:X1:Y1:Z1)"},
	{"order",	1,	"0:1:2",	"Colon delimited 3-element vector specifying "
		"permutation ordering of raw data on disk "},
	{"periodic",	1,	"0:0:0",	"Colon delimited 3-element boolean "
		"(0=>nonperiodic, 1=>periodic) vector specifying periodicity of "
		"X,Y,Z coordinate axes (X:Y:Z)"},
	{"varnames",1,	"",				"Deprecated. Use -vars3d instead"},
	{"vars3d",1,	"var1",			"Colon delimited list of 3D variable "
		"names to be included in the VDF"},
	{"vars2dxy",1,	"",			"Colon delimited list of 2D XY-plane variable "
		"names to be included in the VDF"},
	{"vars2dxz",1,	"",			"Colon delimited list of 3D XZ-plane variable "
		"names to be included in the VDF"},
	{"vars2dyz",1,	"",			"Colon delimited list of 3D YZ-plane variable "
		"names to be included in the VDF"},
	{"mtkcompat",	0,	"",			"Force compatibility with older mtk files"},
	{"help",	0,	"",				"Print this message and exit"},
	{NULL}
};


OptionParser::Option_T	get_options[] = {
	{"dimension", VetsUtil::CvtToDimension3D, &opt.dim, sizeof(opt.dim)},
	{"bs", VetsUtil::CvtToDimension3D, &opt.bs, sizeof(opt.bs)},
	{"numts", VetsUtil::CvtToInt, &opt.numts, sizeof(opt.numts)},
	{"level", VetsUtil::CvtToInt, &opt.level, sizeof(opt.level)},
	{"nfilter", VetsUtil::CvtToInt, &opt.nfilter, sizeof(opt.nfilter)},
	{"nlifting", VetsUtil::CvtToInt, &opt.nlifting, sizeof(opt.nlifting)},
	{"comment", VetsUtil::CvtToString, &opt.comment, sizeof(opt.comment)},
	{"gridtype", VetsUtil::CvtToString, &opt.gridtype, sizeof(opt.gridtype)},
	{"usertimes", VetsUtil::CvtToString, &opt.usertimes, sizeof(opt.usertimes)},
	{"mapprojection", VetsUtil::CvtToString, &opt.mapprojection, sizeof(opt.mapprojection)},
	{"coordsystem", VetsUtil::CvtToString, &opt.coordsystem, sizeof(opt.coordsystem)},
	{"extents", cvtToExtents, &opt.extents, sizeof(opt.extents)},
	{"order", cvtToOrder, &opt.order, sizeof(opt.order)},
	{"periodic", cvtTo3DBool, &opt.periodic, sizeof(opt.periodic)},
	{"varnames", VetsUtil::CvtToStrVec, &opt.varnames, sizeof(opt.varnames)},
	{"vars3d", VetsUtil::CvtToStrVec, &opt.vars3d, sizeof(opt.vars3d)},
	{"vars2dxy", VetsUtil::CvtToStrVec, &opt.vars2dxy, sizeof(opt.vars2dxy)},
	{"vars2dxz", VetsUtil::CvtToStrVec, &opt.vars2dxz, sizeof(opt.vars2dxz)},
	{"vars2dyz", VetsUtil::CvtToStrVec, &opt.vars2dyz, sizeof(opt.vars2dyz)},
	{"mtkcompat", VetsUtil::CvtToBoolean, &opt.mtkcompat, sizeof(opt.mtkcompat)},
	{"help", VetsUtil::CvtToBoolean, &opt.help, sizeof(opt.help)},
	{NULL}
};

extern "C" int vdfcreate(int argc, char **argv) {

	OptionParser op;

	size_t bs[3];
	size_t dim[3];
	string	s;
	Metadata *file;


	if (op.AppendOptions(set_opts) < 0) {
		cerr << argv[0] << " : " << op.GetErrMsg();
		exit(1);
	}

	if (op.ParseOptions(&argc, argv, get_options) < 0) {
		cerr << argv[0] << " : " << OptionParser::GetErrMsg();
		exit(1);
	}

	if (opt.help) {
		cerr << "Usage: " << argv[0] << " [options] filename" << endl;
		op.PrintOptionHelp(stderr);
		exit(0);
	}

	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " [options] filename" << endl;
		op.PrintOptionHelp(stderr);
		exit(1);
	}

	bs[0] = opt.bs.nx;
	bs[1] = opt.bs.ny;
	bs[2] = opt.bs.nz;
	dim[0] = opt.dim.nx;
	dim[1] = opt.dim.ny;
	dim[2] = opt.dim.nz;

	s.assign(opt.coordsystem);
	if (s.compare("spherical") == 0) {
		size_t perm[] = {opt.order[0], opt.order[1], opt.order[2]};

		file = new MetadataSpherical(
			dim,opt.level,bs,perm, opt.nfilter,opt.nlifting
		);
	}
	else {
		if (opt.mtkcompat) {
			file = new Metadata(
				dim,opt.level,bs,opt.nfilter,opt.nlifting, 0, 0
			);
		}
		else {
			file = new Metadata(
				dim,opt.level,bs,opt.nfilter,opt.nlifting
			);
		}
	}

	if (Metadata::GetErrCode()) {
		cerr << Metadata::GetErrMsg() << endl;
		exit(1);
	}

	if (strlen(opt.usertimes)) {
		vector <double> usertimes;
		if (getUserTimes(opt.usertimes, usertimes)<0) {
			cerr << MyBase::GetErrMsg() << endl;
			exit(1);
		}
		if (file->SetNumTimeSteps(usertimes.size()) < 0) {
			cerr << Metadata::GetErrMsg() << endl;
			exit(1);
		}
		for (size_t t=0; t<usertimes.size(); t++) {
			vector <double> vec(1,usertimes[t]);
			if (file->SetTSUserTime(t, vec) < 0) {
				cerr << Metadata::GetErrMsg() << endl;
				exit(1);
			}
		}
	}
	else {
		if (file->SetNumTimeSteps(opt.numts) < 0) {
			cerr << Metadata::GetErrMsg() << endl;
			exit(1);
		}
	}

	if (strlen(opt.mapprojection)) {
		if (file->SetMapProjection(opt.mapprojection) < 0) {
			cerr << Metadata::GetErrMsg() << endl;
			exit(1);
		}
	}

	s.assign(opt.comment);
	if (file->SetComment(s) < 0) {
		cerr << Metadata::GetErrMsg() << endl;
		exit(1);
	}

	s.assign(opt.gridtype);
	if (file->SetGridType(s) < 0) {
		cerr << Metadata::GetErrMsg() << endl;
		exit(1);
	}

#ifdef	DEAD
	s.assign(opt.coordsystem);
	if (file->SetCoordSystemType(s) < 0) {
		cerr << Metadata::GetErrMsg() << endl;
		exit(1);
	}
#endif

	int doExtents = 0;
	for(int i=0; i<5; i++) {
		if (opt.extents[i] != opt.extents[i+1]) doExtents = 1;
	}

	// let Metadata class calculate extents automatically if not 
	// supplied by user explicitly.
	//
	if (doExtents) {
		vector <double> extents;
		for(int i=0; i<6; i++) {
			extents.push_back(opt.extents[i]);
		}
		if (file->SetExtents(extents) < 0) {
			cerr << Metadata::GetErrMsg() << endl;
			exit(1);
		}
	}

	{
		vector <long> periodic_vec;

		for (int i=0; i<3; i++) periodic_vec.push_back(opt.periodic[i]);

		if (file->SetPeriodicBoundary(periodic_vec) < 0) {
			cerr << Metadata::GetErrMsg() << endl;
			exit(1);
		}
	}

	// Deal with deprecated option
	if (opt.varnames.size()) opt.vars3d = opt.varnames;

	if (file->GetGridType().compare("layered") == 0){
		//Make sure there's an ELEVATION variable in the vdf
		bool hasElevation = false;
		for (int i = 0; i<opt.vars3d.size(); i++){
			if (opt.vars3d[i].compare("ELEVATION") == 0){
				hasElevation = true;
				break;
			}
		}
		if (!hasElevation){
			opt.vars3d.push_back("ELEVATION");
		}
	}

	vector <string> allvars;
	for (int i=0;i<opt.vars3d.size();i++) allvars.push_back(opt.vars3d[i]);
	for (int i=0;i<opt.vars2dxy.size();i++) allvars.push_back(opt.vars2dxy[i]);
	for (int i=0;i<opt.vars2dxz.size();i++) allvars.push_back(opt.vars2dxz[i]);
	for (int i=0;i<opt.vars2dyz.size();i++) allvars.push_back(opt.vars2dyz[i]);

	if (file->SetVariableNames(allvars) < 0) {
		cerr << Metadata::GetErrMsg() << endl;
		exit(1);
	}

	if (file->SetVariables2DXY(opt.vars2dxy) < 0) {
		cerr << Metadata::GetErrMsg() << endl;
		exit(1);
	}
	if (file->SetVariables2DXZ(opt.vars2dxz) < 0) {
		cerr << Metadata::GetErrMsg() << endl;
		exit(1);
	}
	if (file->SetVariables2DYZ(opt.vars2dyz) < 0) {
		cerr << Metadata::GetErrMsg() << endl;
		exit(1);
	}


	if (file->Write(argv[1]) < 0) {
		cerr << Metadata::GetErrMsg() << endl;
		exit(1);
	}

	
}

int	cvtToOrder(
	const char *from, void *to
) {
	int   *iptr   = (int *) to;

	if (! from) {
		iptr[0] = iptr[1] = iptr[2];
	}
	else if (!  (sscanf(from,"%d:%d:%d", &iptr[0],&iptr[1],&iptr[2]) == 3)) { 

		return(-1);
	}
	return(1);
}

int	cvtToExtents(
	const char *from, void *to
) {
	float   *fptr   = (float *) to;

	if (! from) {
		fptr[0] = fptr[1] = fptr[2] = fptr[3] = fptr[4] = fptr[5] = 0.0;
	}
	else if (! 
		(sscanf(from,"%f:%f:%f:%f:%f:%f",
		&fptr[0],&fptr[1],&fptr[2],&fptr[3],&fptr[4],&fptr[5]) == 6)) { 

		return(-1);
	}
	return(1);
}

int	cvtTo3DBool(
	const char *from, void *to
) {
	int   *iptr   = (int *) to;

	if (! from) {
		iptr[0] = iptr[1] = iptr[2] = 0;
	}
	else if (! (sscanf(from,"%d:%d:%d", &iptr[0],&iptr[1],&iptr[2]) == 3)) { 
		return(-1);
	}
	return(1);
}


int	getUserTimes(const char *path, vector <double> &timevec) {

	ifstream fin(path);
	if (! fin) { 
		MyBase::SetErrMsg("Error opening file %s", path);
		return(-1);
	}

	timevec.clear();

	double d;
	while (fin >> d) {
		timevec.push_back(d);
	}
	fin.close();

	// Make sure times are monotonic.
	//
	int mono = 1;
	if (timevec.size() > 1) {
		if (timevec[0]>=timevec[timevec.size()-1]) {
			for(int i=0; i<timevec.size()-1; i++) {
				if (timevec[i]<timevec[i+1]) mono = 0;
			}
		} else {
			for(int i=0; i<timevec.size()-1; i++) {
				if (timevec[i]>timevec[i+1]) mono = 0;
			}
		}
	}
	if (! mono) {
		MyBase::SetErrMsg("User times sequence must be monotonic");
		return(-1);
	}

	return(0);
}

}; // namespace ns_vdfcreate

