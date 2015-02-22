#include <vector>
#include <string>
#include <vapor/DCReaderROMS.h>
#include <vapor/DataMgr.h>
#include <vapor/common.h>

#ifndef	_DataMgrROMS_h_
#define	_DataMgrROMS_h_

namespace VAPoR {

//
//! \class DataMgrROMS
//! \brief A cache based data reader
//! \author John Clyne
//! \version $Revision$
//! \date    $Date$
//!
//
class VDF_API DataMgrROMS : public DataMgr, DCReaderROMS {

public:

 DataMgrROMS(
	const vector <string> &files,
	size_t mem_size
 );


 virtual ~DataMgrROMS() {  }; 

protected:


 //
 //	Metadata methods
 //

 virtual void   _GetDim(size_t dim[3], int ) const {
	return(DCReaderROMS::GetGridDim(dim));
 };

 virtual void _GetBlockSize(size_t bs[3], int reflevel) const {
	return(DCReaderROMS::GetGridDim(bs));
 }

 virtual int _GetNumTransforms() const {
	return(0);
 };

 virtual string _GetGridType() const { 
	return(DCReaderROMS::GetGridType());
 }

 virtual vector<double> _GetExtents(size_t ) const {
	return(DCReaderROMS::GetExtents());
 };

 virtual long _GetNumTimeSteps() const {
	return(DCReaderROMS::GetNumTimeSteps());
 };

 virtual string _GetMapProjection() const {
	return(DCReaderROMS::GetMapProjection());
 };

 virtual vector <string> _GetVariables3D() const {
	return(DCReaderROMS::GetVariables3D());
 };

 virtual vector <string> _GetVariables2DXY() const {
	return(DCReaderROMS::GetVariables2DXY());
 };

 virtual vector <string> _GetVariables2DXZ() const {
	return(DCReaderROMS::GetVariables2DXZ());
 };

 virtual vector <string> _GetVariables2DYZ() const {
	return(DCReaderROMS::GetVariables2DYZ());
 };

 virtual vector<long> _GetPeriodicBoundary() const {
	return(DCReaderROMS::GetPeriodicBoundary());
 };

 virtual vector<long> _GetGridPermutation() const {
	return(DCReaderROMS::GetGridPermutation());
 };

 virtual double _GetTSUserTime(size_t ts) const {
	return(DCReaderROMS::GetTSUserTime(ts));
 };

 virtual void _GetTSUserTimeStamp(size_t ts, string &s) const {
    DCReaderROMS::GetTSUserTimeStamp(ts,s);
 }

 virtual int _VariableExists(
	size_t ts,
	const char *varname,
	int reflevel = 0,
	int lod  = 0
 ) const {
	return (DCReaderROMS::VariableExists(ts,varname));
 };


 virtual int    _OpenVariableRead(
    size_t timestep,
    const char *varname,
    int,
    int
 ) {
	return(DCReaderROMS::OpenVariableRead(timestep, varname));
 };

 virtual const float *_GetDataRange() const {
	return(NULL);	// Not implemented. Let DataMgr figure it out
 }

 virtual void _GetValidRegion(
    size_t min[3], size_t max[3], int 
 ) const {
	size_t dim[3]; DCReaderROMS::GetGridDim(dim);
	min[0] = min[1] = min[2] = 0;
	max[0] = dim[0]-1; max[1] = dim[1]-1; max[2] = dim[2]-1;
 };

 virtual bool _GetMissingValue(string varname, float &value) const {
    return(DCReaderROMS::GetMissingValue(varname, value));
 };


 virtual int    _BlockReadRegion(
    const size_t *, const size_t *, float *region
 ) {
	return(DCReaderROMS::Read(region));
 };

 virtual int    _CloseVariable() {
	return (DCReaderROMS::CloseVariable());
 };

};

};

#endif	//	_DataMgrROMS_h_
