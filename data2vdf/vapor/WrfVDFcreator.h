#ifndef WRFVDFCREATE_H
#define WRFVDFCREATE_H

#include <iostream>
#include <cstdio>
#include <vapor/OptionParser.h>
#include <vapor/MetadataVDC.h>
#include <vapor/DCReaderWRF.h>
#include <vapor/VDCFactory.h>


namespace VAPoR {

class VDF_API wrfvdfcreate : public VetsUtil::MyBase {
public:
 wrfvdfcreate();
 ~wrfvdfcreate();
int launchVdfCreate(int argc, char **argv);

private:
 string _progname;

 //
 // command line options passed to launchVdfCreate
 //
 vector <string> _vars;
 vector <string> _dervars;
 VetsUtil::OptionParser::Boolean_T _vdc2;
 VetsUtil::OptionParser::Boolean_T _append;
 VetsUtil::OptionParser::Boolean_T _help;
 VetsUtil::OptionParser::Boolean_T _quiet;
 VetsUtil::OptionParser::Boolean_T _debug;


 void Usage(VetsUtil::OptionParser &op, const char * msg);
 void populateVariables(std::vector<std::string> &vars, 
    std::vector<std::string> candidate_vars,
    MetadataVDC *file, 
    int (MetadataVDC::*SetVarFunction)(const std::vector<std::string>&)
 );
 char ** argv_merge(
    int argc1, char **argv1, int argc2, char **argv2, 
    int &newargc
 );

 void writeToScreen(DCReaderWRF *DCdata, MetadataVDC *file);
 MetadataVDC *CreateMetadataVDC(const VDCFactory &vdcf,
                const DCReaderWRF *DCdata);


};
};

#endif
