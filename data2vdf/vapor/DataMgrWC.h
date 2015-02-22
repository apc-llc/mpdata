//
//      $Id$
//

#ifndef	_DataMgrWC_h_
#define	_DataMgrWC_h_


#include <vector>
#include <string>
#include <vapor/DataMgr.h>
#include <vapor/WaveCodecIO.h>
#include <vapor/common.h>

namespace VAPoR {

//
//! \class DataMgrWC
//! \brief A cache based data reader
//! \author John Clyne
//! \version $Revision$
//! \date    $Date$
//!
//! This class provides a wrapper to the WaveCodecIO()
//! classes that includes a memory cache. Data regions read from disk through 
//! this
//! interface are stored in a cache in main memory, where they may be
//! be available for future access without reading from disk.
//
class VDF_API DataMgrWC : public DataMgr, public WaveCodecIO {

public:

 DataMgrWC(
	const string &metafile,
	size_t mem_size
 );

 DataMgrWC(
	const MetadataVDC &metadata,
	size_t mem_size
 );


 virtual ~DataMgrWC() {}; 

protected:


 //
 //	Metadata methods
 //

 virtual void   _GetDim(size_t dim[3], int reflevel) const {
    return(WaveCodecIO::GetDim(dim, reflevel));
 };

 virtual void _GetBlockSize(size_t bs[3], int reflevel) const {
	WaveCodecIO::GetBlockSize(bs, reflevel);
 }

 virtual int _GetNumTransforms() const {
	return(WaveCodecIO::GetNumTransforms());
 };

 virtual vector <size_t> _GetCRatios() const {
	return(WaveCodecIO::GetCRatios());
 };

 virtual string _GetCoordSystemType() const {
	return(WaveCodecIO::GetCoordSystemType());
 };

 virtual string _GetGridType() const {
	return(WaveCodecIO::GetGridType());
 };

 virtual vector<double> _GetExtents(size_t ts) const {
	return(WaveCodecIO::GetExtents(ts));
 };

 virtual vector <double> _GetTSXCoords(size_t ts) const {
	return(WaveCodecIO::GetTSXCoords(ts));
 }

 virtual vector <double> _GetTSYCoords(size_t ts) const {
	return(WaveCodecIO::GetTSYCoords(ts));
 }
    
 virtual vector <double> _GetTSZCoords(size_t ts) const {
	return(WaveCodecIO::GetTSZCoords(ts));
 }

 virtual long _GetNumTimeSteps() const {
	return(WaveCodecIO::GetNumTimeSteps());
 };

 virtual string _GetMapProjection() const {
	return(WaveCodecIO::GetMapProjection());
 };

 virtual vector <string> _GetVariables3D() const {
	return(WaveCodecIO::GetVariables3D());
 };

 virtual vector <string> _GetVariables2DXY() const {
	return(WaveCodecIO::GetVariables2DXY());
 };

 virtual vector <string> _GetVariables2DXZ() const {
	return(WaveCodecIO::GetVariables2DXZ());
 };

 virtual vector <string> _GetVariables2DYZ() const {
	return(WaveCodecIO::GetVariables2DYZ());
 };

 virtual vector <string> _GetCoordinateVariables() const {
	return(WaveCodecIO::GetCoordinateVariables());
 };

 virtual vector<long> _GetPeriodicBoundary() const {
	return(WaveCodecIO::GetPeriodicBoundary());
 };

 virtual vector<long> _GetGridPermutation() const {
	return(WaveCodecIO::GetGridPermutation());
 };

 virtual double _GetTSUserTime(size_t ts) const {
	return(WaveCodecIO::GetTSUserTime(ts));
 };

 virtual void _GetTSUserTimeStamp(size_t ts, string &s) const {
	WaveCodecIO::GetTSUserTimeStamp(ts,s);
 };


 virtual int _VariableExists(
	size_t ts,
	const char *varname,
	int reflevel = 0,
	int lod = 0
 ) const {
	return (WaveCodecIO::VariableExists(ts,varname,reflevel, lod));
 };


 virtual int	_OpenVariableRead(
	size_t timestep,
	const char *varname,
	int reflevel = 0,
	int lod = 0
 ) {
	_ts = timestep; _varname = varname;
	return(WaveCodecIO::OpenVariableRead(timestep, varname, reflevel, lod)); 
 };

 virtual void _GetValidRegion(
    size_t min[3], size_t max[3], int reflevel
 ) const {
 	return(WaveCodecIO::GetValidRegion( min, max, reflevel));
 };

 virtual const float *_GetDataRange() const {
	return(WaveCodecIO::GetDataRange());
 }

 virtual bool _GetMissingValue(string varname, float &value) const;


 virtual int    _BlockReadRegion(
    const size_t bmin[3], const size_t bmax[3],
    float *region
 )  {
 	return(WaveCodecIO::BlockReadRegion(bmin, bmax, region, false));
 }; 

 virtual int	_CloseVariable() {
	 return (WaveCodecIO::CloseVariable());
 };


 size_t _ts;
 string _varname;
};

};

#endif	//	_DataMgrWC_h_
