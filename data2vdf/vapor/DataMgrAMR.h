//
//      $Id$
//

#ifndef	_DataMgrAMR_h_
#define	_DataMgrAMR_h_


#include <vector>
#include <string>
#include <vapor/DataMgr.h>
#include <vapor/AMRIO.h>
#include <vapor/common.h>

namespace VAPoR {

//
//! \class DataMgrAMR
//! \brief A cache based data reader
//! \author John Clyne
//! \version $Revision$
//! \date    $Date$
//!
//
class VDF_API DataMgrAMR : public DataMgr, public AMRIO {

public:

 DataMgrAMR(
	const string &metafile,
	size_t mem_size
 );

 DataMgrAMR(
	const MetadataVDC &metadata,
	size_t mem_size
 );


 virtual ~DataMgrAMR() {}; 

protected:


 //
 //	Metadata methods
 //
 virtual void   _GetDim(size_t dim[3], int reflevel) const {
    return(AMRIO::GetDim(dim, reflevel));
 };

 virtual void _GetBlockSize(size_t bs[3], int /*reflevel*/) const {
	for (int i=0; i<3; i++) bs[i] = _bs[i];
 }

 virtual int _GetNumTransforms() const {
	return(AMRIO::GetNumTransforms());
 };

 virtual vector<double> _GetExtents(size_t ts = 0) const {
	return(AMRIO::GetExtents(ts));
 };

 virtual long _GetNumTimeSteps() const {
	return(AMRIO::GetNumTimeSteps());
 };

 virtual vector <string> _GetVariables3D() const {
	return(AMRIO::GetVariables3D());
 };

 virtual vector <string> _GetVariables2DXY() const {
	return(AMRIO::GetVariables2DXY());
 };
 virtual vector <string> _GetVariables2DXZ() const {
	return(AMRIO::GetVariables2DXZ());
 };
 virtual vector <string> _GetVariables2DYZ() const {
	return(AMRIO::GetVariables2DYZ());
 };

 virtual vector<long> _GetPeriodicBoundary() const {
	return(AMRIO::GetPeriodicBoundary());
 };

 virtual vector<long> _GetGridPermutation() const {
	return(AMRIO::GetGridPermutation());
 };

 virtual double _GetTSUserTime(size_t ts) const {
	return(AMRIO::GetTSUserTime(ts));
 };

 virtual void _GetTSUserTimeStamp(size_t ts, string &s) const {
	AMRIO::GetTSUserTimeStamp(ts,s);
 };

	
 virtual int _VariableExists(
	size_t ts,
	const char *varname,
	int reflevel = 0,
	int lod = 0
 ) const {
	return (AMRIO::VariableExists(ts,varname,reflevel));
 };

 virtual int	_OpenVariableRead(
	size_t timestep,
	const char *varname,
	int reflevel = 0,
	int lod = 0
 );

 virtual void _GetValidRegion(
    size_t min[3], size_t max[3], int reflevel
 ) const {
 	return(AMRIO::GetValidRegion(
		min, max, reflevel)
	);
 };

 virtual const float *_GetDataRange() const {
	return(AMRIO::GetDataRange());
 }

 virtual int   _BlockReadRegion(
    const size_t bmin[3], const size_t bmax[3],
    float *region
 ); 

 virtual int	_CloseVariable() {
	 return (AMRIO::CloseVariable());
 };


private:
	AMRTree _amrtree;
	size_t _ts;
	int _reflevel;
	size_t _bs[3];
	size_t _bsshift[3];

	int _DataMgrAMR();
	int _ReadBlocks(
		const AMRTree *amrtree, int reflevel,
		const size_t bmin[3], const size_t bmax[3],
		float *blocks
	);

};

};

#endif	//	_DataMgrAMR_h_
