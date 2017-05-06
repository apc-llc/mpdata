#include <vector>
#include <string>
#include <vapor/DCReaderGRIB.h>
#include <vapor/DataMgr.h>
#include <vapor/common.h>

#ifndef	_DataMgrGRIB_h_
#define	_DataMgrGRIB_h_

namespace VAPoR {

//
//! \class DataMgrGRIB
//! \brief A cache based data reader
//! \author John Clyne
//! \version $Revision$
//! \date    $Date$
//!
//
class VDF_API DataMgrGRIB : public DataMgr, DCReaderGRIB {

public:

 DataMgrGRIB(
	const vector <string> &files,
	size_t mem_size
 );


 virtual ~DataMgrGRIB() {  }; 

protected:


 //
 //	Metadata methods
 //

 virtual void   _GetDim(size_t dim[3], int ) const {
	return(DCReaderGRIB::GetGridDim(dim));
 };

 virtual void _GetBlockSize(size_t bs[3], int reflevel) const {
	return(DCReaderGRIB::GetGridDim(bs));
 }

 virtual int _GetNumTransforms() const {
	return(0);
 };

 virtual string _GetGridType() const { 
	return(DCReaderGRIB::GetGridType());
 }

 virtual vector<double> _GetExtents(size_t ) const {
	return(DCReaderGRIB::GetExtents());
 };

 virtual long _GetNumTimeSteps() const {
	return(DCReaderGRIB::GetNumTimeSteps());
 };

 virtual string _GetMapProjection() const {
	return(DCReaderGRIB::GetMapProjection());
 };

 virtual vector <string> _GetVariables3D() const {
	return(DCReaderGRIB::GetVariables3D());
 };

 virtual vector <string> _GetVariables2DXY() const {
	return(DCReaderGRIB::GetVariables2DXY());
 };

 virtual vector <string> _GetVariables2DXZ() const {
	return(DCReaderGRIB::GetVariables2DXZ());
 };

 virtual vector <string> _GetVariables2DYZ() const {
	return(DCReaderGRIB::GetVariables2DYZ());
 };

 virtual vector<long> _GetPeriodicBoundary() const {
	return(DCReaderGRIB::GetPeriodicBoundary());
 };

 virtual vector<long> _GetGridPermutation() const {
	return(DCReaderGRIB::GetGridPermutation());
 };

 virtual double _GetTSUserTime(size_t ts) const {
	return(DCReaderGRIB::GetTSUserTime(ts));
 };

 virtual void _GetTSUserTimeStamp(size_t ts, string &s) const {
    DCReaderGRIB::GetTSUserTimeStamp(ts,s);
 }

 virtual int _VariableExists(
	size_t ts,
	const char *varname,
	int reflevel = 0,
	int lod  = 0
 ) const {
	return (DCReaderGRIB::VariableExists(ts,varname));
 };


 virtual int    _OpenVariableRead(
    size_t timestep,
    const char *varname,
    int,
    int
 ) {
	return(DCReaderGRIB::OpenVariableRead(timestep, varname));
 };

 virtual const float *_GetDataRange() const {
	return(NULL);	// Not implemented. Let DataMgr figure it out
 }

 virtual void _GetValidRegion(
    size_t min[3], size_t max[3], int 
 ) const {
	size_t dim[3]; DCReaderGRIB::GetGridDim(dim);
	min[0] = min[1] = min[2] = 0;
	max[0] = dim[0]-1; max[1] = dim[1]-1; max[2] = dim[2]-1;
 };

 virtual bool _GetMissingValue(string varname, float &value) const {
    return(DCReaderGRIB::GetMissingValue(varname, value));
 };


 virtual int    _BlockReadRegion(
    const size_t *, const size_t *, float *region
 ) {
	return(DCReaderGRIB::Read(region));
 };

 virtual int    _CloseVariable() {
	return (DCReaderGRIB::CloseVariable());
 };

};

};

#endif	//	_DataMgrGRIB_h_
