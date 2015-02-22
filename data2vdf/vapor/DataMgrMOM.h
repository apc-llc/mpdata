#include <vector>
#include <string>
#include <vapor/DCReaderMOM.h>
#include <vapor/DataMgr.h>
#include <vapor/common.h>

#ifndef	_DataMgrMOM_h_
#define	_DataMgrMOM_h_

namespace VAPoR {

//
//! \class DataMgrMOM
//! \brief A cache based data reader
//! \author John Clyne
//! \version $Revision$
//! \date    $Date$
//!
//
class VDF_API DataMgrMOM : public DataMgr, DCReaderMOM {

public:

 DataMgrMOM(
	const vector <string> &files,
	size_t mem_size
 );


 virtual ~DataMgrMOM() {  }; 

protected:


 //
 //	Metadata methods
 //

 virtual void   _GetDim(size_t dim[3], int ) const {
	return(DCReaderMOM::GetGridDim(dim));
 };

 virtual void _GetBlockSize(size_t bs[3], int reflevel) const {
	return(DCReaderMOM::GetGridDim(bs));
 }

 virtual int _GetNumTransforms() const {
	return(0);
 };

 virtual string _GetGridType() const { 
	return(DCReaderMOM::GetGridType());
 }

 virtual vector<double> _GetExtents(size_t ) const {
	return(DCReaderMOM::GetExtents());
 };

 virtual vector <double> _GetTSZCoords(size_t ts) const {
	return(DCReaderMOM::GetTSZCoords(ts));
 }


 virtual long _GetNumTimeSteps() const {
	return(DCReaderMOM::GetNumTimeSteps());
 };

 virtual string _GetMapProjection() const {
	return(DCReaderMOM::GetMapProjection());
 };

 virtual vector <string> _GetVariables3D() const {
	return(DCReaderMOM::GetVariables3D());
 };

 virtual vector <string> _GetVariables2DXY() const {
	return(DCReaderMOM::GetVariables2DXY());
 };

 virtual vector <string> _GetVariables2DXZ() const {
	return(DCReaderMOM::GetVariables2DXZ());
 };

 virtual vector <string> _GetVariables2DYZ() const {
	return(DCReaderMOM::GetVariables2DYZ());
 };

 virtual vector<long> _GetPeriodicBoundary() const {
	return(DCReaderMOM::GetPeriodicBoundary());
 };

 virtual vector<long> _GetGridPermutation() const {
	return(DCReaderMOM::GetGridPermutation());
 };

 virtual double _GetTSUserTime(size_t ts) const {
	return(DCReaderMOM::GetTSUserTime(ts));
 };

 virtual void _GetTSUserTimeStamp(size_t ts, string &s) const {
    DCReaderMOM::GetTSUserTimeStamp(ts,s);
 }

 virtual int _VariableExists(
	size_t ts,
	const char *varname,
	int reflevel = 0,
	int lod  = 0
 ) const {
	return (DCReaderMOM::VariableExists(ts,varname));
 };


 virtual int    _OpenVariableRead(
    size_t timestep,
    const char *varname,
    int,
    int
 ) {
	return(DCReaderMOM::OpenVariableRead(timestep, varname));
 };

 virtual const float *_GetDataRange() const {
	return(NULL);	// Not implemented. Let DataMgr figure it out
 }

 virtual void _GetValidRegion(
    size_t min[3], size_t max[3], int 
 ) const {
	size_t dim[3]; DCReaderMOM::GetGridDim(dim);
	min[0] = min[1] = min[2] = 0;
	max[0] = dim[0]-1; max[1] = dim[1]-1; max[2] = dim[2]-1;
 };

 virtual bool _GetMissingValue(string varname, float &value) const {
    return(DCReaderMOM::GetMissingValue(varname, value));
 };


 virtual int    _BlockReadRegion(
    const size_t *, const size_t *, float *region
 ) {
	return(DCReaderMOM::Read(region));
 };

 virtual int    _CloseVariable() {
	return (DCReaderMOM::CloseVariable());
 };

};

};

#endif	//	_DataMgrMOM_h_
