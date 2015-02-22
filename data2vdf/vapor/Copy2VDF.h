#ifndef COPYTOVDF_H
#define COPYTOVDF_H

#include <iostream>
#include <cstdio>
#include <vapor/OptionParser.h>
#include <vapor/MetadataVDC.h>
#include <vapor/DCReader.h>
#include <vapor/WaveCodecIO.h>
#include <vapor/WaveletBlock3DBufWriter.h>
#include <vapor/CFuncs.h>
 
namespace VAPoR {
             
class VDF_API Copy2VDF : public VetsUtil::MyBase {
public:
 Copy2VDF();
 ~Copy2VDF();

 //! copy data set from source to VDC
 //!
 //! \retval status A status of 0 implies success. A negative status
 //! implies failure, the destination VDC may be corrupt. A status 
 //! greater than 0 implies some variables were not copied
 //!
int launch2vdf(int argc, char **argv, string dataType);

private:
 string _progname;
 DCReader *DCData;
 WaveCodecIO *wcwriter;
 //
 // Missing data masks
 //
 unsigned char *_mvMask3D;
 unsigned char *_mvMask2DXY;
 unsigned char *_mvMask2DXZ;
 unsigned char *_mvMask2DYZ;

 //
 // Command line options passed to launch2vdf
 //
 vector <string> _vars;
 int _numts;
 int _startts;
 int _level;
 int _lod;
 int _nthreads;
 VetsUtil::OptionParser::Boolean_T _help;
 VetsUtil::OptionParser::Boolean_T _quiet;
 VetsUtil::OptionParser::Boolean_T _debug;


 void Usage(VetsUtil::OptionParser &op, const char * msg);
 void GetTimeMap(const VDFIOBase *vdfio,
          	const DCReader *DCData,
          	int startts,
          	int numts,
          	map <size_t, size_t> &timemap);
 void MissingValue(VDFIOBase *vdfio,
	         DCReader *DCData,
        	 size_t vdcTS,
       		 string vdcVar,
         	 string ncdfVar,
            	 VDFIOBase::VarType_T vtype,
         	 int slice,
         	 float *buf);
 int CopyVar(VDFIOBase *vdfio,
         DCReader *DCData,
         int vdcTS,
         int ncdfTS,
         string vdcVar,
         string ncdfVar,
         int level,
         int lod);

public:
 void deleteObjects();
 void GetVariables(const VDFIOBase *vdfio,
              const DCReader *DCData,
              const vector <string> &in_varnames,
              vector <string> &out_varnames);

};

};

#endif
