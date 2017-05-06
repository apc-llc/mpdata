#ifndef VDFCREATE_H
#define VDFCREATE_H

#include <iostream>
#include <cstdio>
#include <vapor/OptionParser.h>
#include <vapor/MetadataVDC.h>
#include <vapor/DCReader.h>
#include <vapor/VDCFactory.h>


namespace VAPoR {

class VDF_API vdfcreate : public VetsUtil::MyBase {
public:
 vdfcreate();
 ~vdfcreate();
int launchVdfCreate(int argc, char **argv, string NetCDFtype);

private:
 string _progname;

 //
 // command line options passed to launchVdfCreate
 //
 vector <string> _vars;
 VetsUtil::OptionParser::Boolean_T _help;
 VetsUtil::OptionParser::Boolean_T _quiet;
 VetsUtil::OptionParser::Boolean_T _debug;
 int _numTS;

 void Usage(VetsUtil::OptionParser &op, const char * msg);
 void populateVariables(std::vector<std::string> vars, 
	std::vector<std::string> candidate_vars,
	MetadataVDC *file, 
	int (MetadataVDC::*SetVarFunction)(const std::vector<std::string>&)
 );

 void writeToScreen(DCReader *DCdata, MetadataVDC *file);
 MetadataVDC *CreateMetadataVDC(const VDCFactory &vdcf,
				const DCReader *DCdata);


};
};

#endif
