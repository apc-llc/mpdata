//
//      $Id$
//


#ifndef	_DCReaderMOM_h_
#define	_DCReaderMOM_h_

#include <vector>
#include <cassert>
#include <vapor/NetCDFCFCollection.h>
#include <vapor/DCReaderNCDF.h>
#include <vapor/WeightTable.h>
#ifdef WIN32
#pragma warning(disable : 4251)
#endif

namespace VAPoR {

//
//! \class DCReaderMOM
//! \brief ???
//!
//! \author John Clyne
//! \version $Revision$
//! \date    $Date$
//!
//!
class VDF_API DCReaderMOM : public DCReader {
public:

 DCReaderMOM(const std::vector <string> &files);

 virtual ~DCReaderMOM();

 virtual void   GetGridDim(size_t dim[3]) const {
	for (int i=0; i<3; i++) dim[i] = _dims[i];
 }

 void   GetBlockSize(size_t bs[3], int) const {
	DCReaderMOM::GetGridDim(bs);
 }

 virtual string GetGridType() const {return("stretched"); };

 virtual std::vector<double> GetTSZCoords(size_t ) const {
    return(_vertCoordinates);
 };

 virtual std::vector <double> GetExtents(size_t ts = 0) const {
	return(_cartesianExtents);
 }

 long GetNumTimeSteps() const {
	return(_ncdfc->GetNumTimeSteps());
 }

 virtual string GetMapProjection() const;

 virtual std::vector <string> GetVariables3D() const {
    return(_vars3d);
 }; 

 virtual std::vector <string> GetVariables2DXY() const {
    return(_vars2dXY);
 }; 


 virtual std::vector <string> GetVariables2DXZ() const {
	std::vector <string> empty; return(empty);
 };

 virtual std::vector <string> GetVariables2DYZ() const {
	std::vector <string> empty; return(empty);
 };

 virtual std::vector <string> GetVariables3DExcluded() const {
    return(_vars3dExcluded);
 };

 virtual std::vector <string> GetVariables2DExcluded() const {
    return(_vars2dExcluded);
 };


 //
 // There a no spatial coordinate variables because we project
 // the data to a horizontally uniformly sampled Cartesian
 // grid.
 //
 virtual std::vector <string> GetCoordinateVariables() const {
	vector <string> v;
	v.push_back("NONE"); v.push_back("NONE"); v.push_back("NONE");
	return(v);
}

 //
 // Boundary may be periodic in X (longitude), but we have no way
 // of knowing this by examining the netCDF files
 //
 virtual std::vector<long> GetPeriodicBoundary() const {
    vector <long> p;
    p.push_back(0); p.push_back(0); p.push_back(0);
    return(p);
 }

 virtual std::vector<long> GetGridPermutation() const {
    vector <long> p;
    p.push_back(0); p.push_back(1); p.push_back(2);
    return(p);
}

 double GetTSUserTime(size_t ts) const ;

 bool GetMissingValue(string varname, float &value) const;

 void GetTSUserTimeStamp(size_t ts, string &s) const;
 
 virtual bool IsCoordinateVariable(string varname) const;


 virtual int OpenVariableRead(
    size_t timestep, string varname, int reflevel=0, int lod=0
 );

 virtual int CloseVariable();

 virtual int ReadSlice(float *slice);

 virtual int Read(float *data);

 virtual bool VariableExists(size_t ts, string varname, int i0=0, int i1=0) const {
	if (IsVariableDerived(varname)) return (true);
    return(_ncdfc->VariableExists(ts, varname));
 }

 virtual bool IsVariableDerived(string varname) const {
	return(find(_varsDerived.begin(), _varsDerived.end(), varname) != _varsDerived.end());
 }

 virtual void GetLatLonExtents(
    size_t ts, double lon_exts[2], double lat_exts[2]
 ) const {
	lon_exts[0] = _lonExts[0]; lon_exts[1] = _lonExts[1];
	lat_exts[0] = _latExts[0]; lat_exts[1] = _latExts[1];
 }

private:
 std::vector <size_t> _dims;
 double _latExts[2];
 double _lonExts[2];
 std::vector <double> _vertCoordinates;
 std::vector <double> _cartesianExtents;
 std::vector <string> _vars3d;
 std::vector <string> _vars2dXY;
 std::vector <string> _vars3dExcluded;
 std::vector <string> _vars2dExcluded;
 std::vector <string> _varsDerived;
 std::map <string, WeightTable *> _weightTableMap;
 std::map <string, string> _varsLatLonMap;
 NetCDFCFCollection *_ncdfc;
 float *_sliceBuffer;	// buffer for reading data
 float *_angleRADBuf;	// buffer for derived "angleRAD" variable
 float *_latDEGBuf;	// buffer for derived "latDEG" variable
 string _vertCV;	// vertical coordinate variable name
 string _timeCV;	// time coordinate variable name
 std::vector <string> _latCVs;	// all valid latitude coordinate variables
 std::vector <string> _lonCVs;	// all valid longitude coordinate variables
 WeightTable *_ovr_weight_tbl;
 string _ovr_varname;
 size_t _ovr_slice;
 size_t _ovr_nz;
 int _ovr_fd;
 float _defaultMV;
 bool _reverseRead;

 class latLonBuf {
 public:
  size_t _nx;
  size_t _ny;
  float *_latbuf;
  float *_lonbuf;
  float _latexts[2];
  float _lonexts[2];
 };


 void _encodeLatLon(string latcv, string loncv, string &key) const;
 void _decodeLatLon(string key, string &latcv, string &loncv) const;

 int _initLatLonBuf(
	NetCDFCFCollection *ncdfc, string latvar, string lonvar,
	DCReaderMOM::latLonBuf &llb
 ) const;

 void _getRotationVariables(
	const std::map <string, WeightTable *> _weightTableMap,
	float *_angleRADBuf, float *_latDEGBuf
 ) const;

 std::vector <size_t> _GetSpatialDims(
    NetCDFCFCollection *ncdfc, string varname
 ) const;

 int _InitCoordVars(NetCDFCFCollection *ncdfc);

 void _ParseCoordVarNames(
    NetCDFCFCollection *ncdfc, const vector <string> &cvars,
    string &timecv, string &vertcv, string &latcv, string &loncv
 ) const;


 int _InitVerticalCoordinates(
    NetCDFCFCollection *ncdfc,
    string cvar,
    vector <double> &vertCoords
 );

 void _InitDimensions(
    NetCDFCFCollection *ncdfc,
    vector <size_t> &dims,
    vector <string> &vars3d,
    vector <string> &vars2dxy
 );

 //
 // Convert horizontal extents expressed in lat-lon to Cartesian
 // coordinates in whatever units the vertical coordinate is
 // expressed in. 
 //
 int _InitCartographicExtents(
	string mapProj, 
	const double lonExts[2],
	const double latExts[2],
	const std::vector <double> vertCoordinates,
	std::vector <double> &extents
 ) const;

 float *_get_2d_var(NetCDFCFCollection *ncdfc, size_t ts, string name) const;
 float *_get_1d_var(NetCDFCFCollection *ncdfc, size_t ts, string name) const;

 

};
};

#endif	//	_DCReaderMOM_h_
