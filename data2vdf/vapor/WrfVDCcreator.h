#ifndef WRF2VDF_H
#define WRF2VDF_H

#include <iostream>
#include <cstdio>
#include <vapor/OptionParser.h>
#include <vapor/MetadataVDC.h>
#include <vapor/DCReaderWRF.h>
#include <vapor/WaveCodecIO.h>
#include <vapor/WaveletBlock3DBufWriter.h>
#include <vapor/CFuncs.h>
 
namespace VAPoR {
             
class VDF_API Wrf2vdf : public VetsUtil::MyBase {
public:
 Wrf2vdf();
 ~Wrf2vdf();

 //! copy data set from source to VDC
 //!
 //! \retval status A status of 0 implies success. A negative status
 //! implies failure, the destination VDC may be corrupt. A status 
 //! greater than 0 implies some variables were not copied
 //!
 int launchWrf2Vdf(int argc, char **argv);

private:
 string _progname;
 DCReaderWRF *wrfData;
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
          	const DCReaderWRF *wrfData,
          	int startts,
          	int numts,
          	map <size_t, size_t> &timemap);
 int CopyVar(VDFIOBase *vdfio,
         DCReaderWRF *wrfData,
         int vdcTS,
         int ncdfTS,
         string vdcVar,
         string ncdfVar,
         int level,
         int lod);

public:
 void deleteObjects();
 void GetVariables(const VDFIOBase *vdfio,
              const DCReaderWRF *wrfData,
              const vector <string> &in_varnames,
              vector <string> &out_varnames);

};

};

#endif
