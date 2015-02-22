//
//      $Id$
//


#ifndef	_DCReaderNCDF_h_
#define	_DCReaderNCDF_h_

#include <vector>
#include <vapor/NetCDFCollection.h>
#include <vapor/DCReader.h>
#ifdef WIN32
#pragma warning(disable : 4251)
#endif

namespace VAPoR {

//
//! \class DCReaderNCDF
//! \brief ???
//!
//! \author John Clyne
//! \version $Revision$
//! \date    $Date$
//!
//!
class VDF_API DCReaderNCDF : public DCReader {
public:

 DCReaderNCDF(
	const std::vector <string> &files, 
	const std::vector <string> &time_dimnames,
	const std::vector <string> &time_coordvars,
	const std::vector <string> &staggerd_dims,
	string missing_attr, size_t dims[3]
);

 virtual ~DCReaderNCDF();

 virtual void   GetGridDim(size_t dim[3]) const;

 virtual void GetBlockSize(size_t bs[3], int reflevel) const;

 virtual string GetCoordSystemType() const;

 virtual string GetGridType() const;

 virtual std::vector <double> GetExtents(size_t ts = 0) const;

 long GetNumTimeSteps() const {
    return (_ncdfC->GetNumTimeSteps());
 }

 virtual std::vector <string> GetVariables3D() const {
    return(_vars3d);
 };

 virtual std::vector <string> GetVariables2DXY() const {
    return(_vars2dXY);
 };

 virtual std::vector <string> GetVariables2DXZ() const {
    return(_vars2dXZ);
 };

 virtual std::vector <string> GetVariables2DYZ() const {
    return(_vars2dYZ);
 };

 virtual std::vector <string> GetVariables3DExcluded() const {
    return(_vars3dExcluded);
 };

 virtual std::vector <string> GetVariables2DExcluded() const {
    return(_vars2dExcluded);
 };


 virtual std::vector <string> GetCoordinateVariables() const;

 virtual std::vector<long> GetPeriodicBoundary() const;

 virtual std::vector<long> GetGridPermutation() const;

 virtual double GetTSUserTime(size_t ts) const;

 virtual void GetTSUserTimeStamp(size_t ts, string &s) const;

 virtual bool GetMissingValue(string varname, float &value) const;

 virtual bool IsCoordinateVariable(string varname) const;

 virtual int OpenVariableRead(
    size_t timestep, string varname, int reflevel=0, int lod=0
 );

 virtual int CloseVariable();

 virtual int ReadSlice(float *slice);

 NetCDFCollection *GetNetCDFCollection() const {return (_ncdfC); };

 virtual bool VariableExists(size_t ts, string varname, int i0=0, int i1=0) const {
	return(_ncdfC->VariableExists(ts, varname));
 }

  virtual void GetLatLonExtents(
    size_t ts, double lon_exts[2], double lat_exts[2]
 ) const {
	lon_exts[0] = lon_exts[1] = lat_exts[0] = lat_exts[1] = 0.0;
 }


private:
 std::vector <size_t> _dims;
 std::vector <string> _vars3d;
 std::vector <string> _vars3dExcluded;
 std::vector <string> _vars2dXY;
 std::vector <string> _vars2dXZ;
 std::vector <string> _vars2dYZ;
 std::vector <string> _vars2dExcluded;
 int _ovr_fd;

 NetCDFCollection *_ncdfC;

 //
 // Get dimensions for variable, varname, flipping the order to be consistent
 // with the VDC library
 //
 vector <size_t> _GetSpatialDims(string varname) const;

  


};
};

#endif	//	_DCReaderNCDF_h_
