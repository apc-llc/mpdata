//
//      $Id$
//

#ifndef	_DataMgrWB_h_
#define	_DataMgrWB_h_


#include <vector>
#include <string>
#include <vapor/DataMgr.h>
#include <vapor/WaveletBlock3DRegionReader.h>
#include <vapor/common.h>

namespace VAPoR {

//
//! \class DataMgrWB
//! \brief A cache based data reader
//! \author John Clyne
//! \version $Revision$
//! \date    $Date$
//!
//! This class provides a wrapper to the WaveletBlock3DRegionReader()
//! and WaveletBlock2DRegionReader()
//! classes that includes a memory cache. Data regions read from disk through 
//! this
//! interface are stored in a cache in main memory, where they may be
//! be available for future access without reading from disk.
//
class VDF_API DataMgrWB : public DataMgr, public WaveletBlock3DRegionReader {

public:

 DataMgrWB(
	const string &metafile,
	size_t mem_size
 );

 DataMgrWB(
	const MetadataVDC &metadata,
	size_t mem_size
 );


 virtual ~DataMgrWB() {}; 

protected:


 //
 //	Metadata methods
 //
 virtual void   _GetDim(size_t dim[3], int reflevel) const {
	return(WaveletBlock3DRegionReader::GetDim(dim, reflevel));
 };

 virtual void _GetBlockSize(size_t bs[3], int reflevel) const {
	WaveletBlock3DRegionReader::GetBlockSize(bs, reflevel);
 }

 virtual int _GetNumTransforms() const {
	return(WaveletBlock3DRegionReader::GetNumTransforms());
 };

 virtual string _GetCoordSystemType() const {
	return(WaveletBlock3DRegionReader::GetCoordSystemType());
 };

 virtual string _GetGridType() const {
	return(WaveletBlock3DRegionReader::GetGridType());
 };
	
 virtual vector<double> _GetExtents(size_t ts) const {
	return(WaveletBlock3DRegionReader::GetExtents(ts));
 };

 virtual vector <double> _GetTSXCoords(size_t ts) const {
	return(WaveletBlock3DRegionReader::GetTSXCoords(ts));
 }

 virtual vector <double> _GetTSYCoords(size_t ts) const {
	return(WaveletBlock3DRegionReader::GetTSYCoords(ts));
 }
   
 virtual vector <double> _GetTSZCoords(size_t ts) const {
	return(WaveletBlock3DRegionReader::GetTSZCoords(ts));
 }

 virtual long _GetNumTimeSteps() const {
	return(WaveletBlock3DRegionReader::GetNumTimeSteps());
 };

 virtual string _GetMapProjection() const {
	return(WaveletBlock3DRegionReader::GetMapProjection());
 };

 virtual vector <string> _GetVariables3D() const {
	return(WaveletBlock3DRegionReader::GetVariables3D());
 };

 virtual vector <string> _GetVariables2DXY() const {
	return(WaveletBlock3DRegionReader::GetVariables2DXY());
 };
 virtual vector <string> _GetVariables2DXZ() const {
	return(WaveletBlock3DRegionReader::GetVariables2DXZ());
 };
 virtual vector <string> _GetVariables2DYZ() const {
	return(WaveletBlock3DRegionReader::GetVariables2DYZ());
 };

 virtual vector <string> _GetCoordinateVariables() const {
	return(WaveletBlock3DRegionReader::GetCoordinateVariables());
 };

 virtual vector<long> _GetPeriodicBoundary() const {
	return(WaveletBlock3DRegionReader::GetPeriodicBoundary());
 };

 virtual vector<long> _GetGridPermutation() const {
	return(WaveletBlock3DRegionReader::GetGridPermutation());
 };

 virtual double _GetTSUserTime(size_t ts) const {
	return(WaveletBlock3DRegionReader::GetTSUserTime(ts));
 };

 virtual void _GetTSUserTimeStamp(size_t ts, string &s) const {
	WaveletBlock3DRegionReader::GetTSUserTimeStamp(ts,s);
 };

 virtual int _VariableExists(
	size_t ts,
	const char *varname,
	int reflevel = 0,
	int lod = 0
 ) const {
	return (WaveletBlock3DRegionReader::VariableExists(ts,varname,reflevel));
 };


 virtual int	_OpenVariableRead(
	size_t timestep,
	const char *varname,
	int reflevel = 0,
	int lod = 0
 ) {
	return(WaveletBlock3DRegionReader::OpenVariableRead(
		timestep, varname, reflevel)
	); 
 };

 virtual void _GetValidRegion(
    size_t min[3], size_t max[3], int reflevel
 ) const {
 	return(WaveletBlock3DRegionReader::GetValidRegion(
		min, max, reflevel)
	);
 };

 virtual const float *_GetDataRange() const {
	return(WaveletBlock3DRegionReader::GetDataRange());
 }


 virtual int    _BlockReadRegion(
    const size_t bmin[3], const size_t bmax[3],
    float *region
 )  {
 	return(WaveletBlock3DRegionReader::BlockReadRegion(
		bmin, bmax, region, false)
	);
 }; 

 virtual int	_CloseVariable() {
	 return (WaveletBlock3DRegionReader::CloseVariable());
 };

};

};

#endif	//	_DataMgrWB_h_
