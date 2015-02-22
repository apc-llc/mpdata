#include <vector>
#include <string>
#include <vapor/DCReaderWRF.h>
#include <vapor/DataMgr.h>
#include <vapor/common.h>

#ifndef	_DataMgrWRF
#define	_DataMgrWRF

namespace VAPoR {

//
//! \class DataMgrWRF
//! \brief A cache based data reader
//! \author John Clyne
//! \version $Revision$
//! \date    $Date$
//!
//
class VDF_API DataMgrWRF : public DataMgr, DCReaderWRF {

public:

 DataMgrWRF(
	const vector <string> &files,
	size_t mem_size
 );


 virtual ~DataMgrWRF() {  }; 

protected:


 //
 //	Metadata methods
 //

 virtual void   _GetDim(size_t dim[3], int ) const {
	return(DCReaderWRF::GetGridDim(dim));
 };

 virtual void _GetBlockSize(size_t bs[3], int reflevel) const {
	return(DCReaderWRF::GetGridDim(bs));
 }

 virtual int _GetNumTransforms() const {
	return(0);
 };

 virtual string _GetGridType() const { 
	return(DCReaderWRF::GetGridType());
 }

 virtual vector<double> _GetExtents(size_t ts) const {
	return(DCReaderWRF::GetExtents(ts));
 };

 virtual long _GetNumTimeSteps() const {
	return(DCReaderWRF::GetNumTimeSteps());
 };

 virtual string _GetMapProjection() const {
	return(DCReaderWRF::GetMapProjection());
 };

 virtual vector <string> _GetVariables3D() const {
	return(DCReaderWRF::GetVariables3D());
 };

 virtual vector <string> _GetVariables2DXY() const {
	return(DCReaderWRF::GetVariables2DXY());
 };

 virtual vector <string> _GetVariables2DXZ() const {
	return(DCReaderWRF::GetVariables2DXZ());
 };

 virtual vector <string> _GetVariables2DYZ() const {
	return(DCReaderWRF::GetVariables2DYZ());
 };

 virtual vector<long> _GetPeriodicBoundary() const {
	return(DCReaderWRF::GetPeriodicBoundary());
 };

 virtual vector<long> _GetGridPermutation() const {
	return(DCReaderWRF::GetGridPermutation());
 };

 virtual double _GetTSUserTime(size_t ts) const {
	return(DCReaderWRF::GetTSUserTime(ts));
 };

 virtual void _GetTSUserTimeStamp(size_t ts, string &s) const {
    DCReaderWRF::GetTSUserTimeStamp(ts,s);
 }

 virtual int _VariableExists(
	size_t ts,
	const char *varname,
	int reflevel = 0,
	int lod  = 0
 ) const {
	return (DCReaderWRF::VariableExists(ts,varname));
 };


 virtual int    _OpenVariableRead(
    size_t timestep,
    const char *varname,
    int,
    int
 ) {
	return(DCReaderWRF::OpenVariableRead(timestep, varname));
 };

 virtual const float *_GetDataRange() const {
	return(NULL);	// Not implemented. Let DataMgr figure it out
 }

 virtual void _GetValidRegion(
    size_t min[3], size_t max[3], int 
 ) const {
	size_t dim[3]; DCReaderWRF::GetGridDim(dim);
	min[0] = min[1] = min[2] = 0;
	max[0] = dim[0]-1; max[1] = dim[1]-1; max[2] = dim[2]-1;
 };

 virtual bool _GetMissingValue(string varname, float &value) const {
    return(DCReaderWRF::GetMissingValue(varname, value));
 };


 virtual int    _BlockReadRegion(
    const size_t *, const size_t *, float *region
 ) {
	return(DCReaderWRF::Read(region));
 };

 virtual int    _CloseVariable() {
	return (DCReaderWRF::CloseVariable());
 };

};

};

#endif	//	_DataMgrWRF_h_
