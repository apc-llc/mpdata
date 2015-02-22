//
//      $Id$
//


#ifndef	_DCReaderWRF_h_
#define	_DCReaderWRF_h_

#include <vector>
#include <cassert>
#include <vapor/DCReader.h>
#include <vapor/NetCDFCollection.h>
#include <vapor/common.h>
#include <vapor/Proj4API.h>

#ifdef WIN32
#pragma warning(disable : 4251)
#endif

namespace VAPoR {

//
//! \class DCReaderWRF
//! \brief ???
//!
//! \author John Clyne
//! \version $Revision$
//! \date    $Date$
//!
//!
class VDF_API DCReaderWRF : public DCReader {

public:

 DCReaderWRF(const std::vector <string> &files);

 virtual ~DCReaderWRF();

 virtual void   GetGridDim(size_t dim[3]) const {
	for (int i=0; i<3; i++) dim[i] = _dims[i];
 }

 void   GetBlockSize(size_t bs[3], int) const {
	DCReaderWRF::GetGridDim(bs);
 }

 virtual string GetGridType() const {
	return("layered"); 
 }

 virtual std::vector <double> GetExtents(size_t ts = 0) const;

 long GetNumTimeSteps() const {
	return(_ncdfc->GetNumTimeSteps());
 }

 virtual string GetMapProjection() const {
	return(_projString);
 };

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
	v.push_back("NONE"); v.push_back("NONE"); v.push_back("ELEVATION");
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

 bool GetMissingValue(string varname, float &value) const {
	value = 0.0;
	return(false);
 };

 void GetTSUserTimeStamp(size_t ts, string &s) const;
 
 virtual bool IsCoordinateVariable(string varname) const;


 virtual int OpenVariableRead(
    size_t timestep, string varname, int reflevel=0, int lod=0
 );

 virtual int CloseVariable();

 virtual int ReadSlice(float *slice);

 virtual int Read(float *data);

 virtual bool VariableExists(size_t ts, string varname, int i0=0, int i1=0) const {
    return(_ncdfc->VariableExists(ts, varname));
 }


 void EnableLegacyTimeConversion() {
	_timeBias = 978307200.0;  // Make consistent with legacy code
 }

 virtual void GetLatLonExtents(
    size_t ts, double lon_exts[2], double lat_exts[2]
 ) const {
	double dummy[4];
	(void) _GetLatLonExtentsCorners(
		_ncdfc, ts, lon_exts, lat_exts, dummy, dummy
	);
 }


private:

 class DerivedVarElevation : public NetCDFCollection::DerivedVar {
 public:
  DerivedVarElevation(
    NetCDFCollection *ncdfcf, float grav
  );
  virtual ~DerivedVarElevation();

  virtual int Open(size_t ts);
  virtual int ReadSlice(float *slice, int );
  virtual int Read(float *buf, int );
  virtual int SeekSlice(int offset, int whence, int );
  virtual int Close(int fd);
  virtual bool TimeVarying() const {return(true); };
  virtual std::vector <size_t>  GetSpatialDims() const { return(_dims); }
  virtual std::vector <string>  GetSpatialDimNames() const { return(_dimnames); }
  virtual size_t  GetTimeDim() const { return(_num_ts); }
  virtual string  GetTimeDimName() const { return("Time"); };
  virtual bool GetMissingValue(double &mv) const {mv=0.0; return(false); }

 private:
  std::vector <size_t> _dims;
  std::vector <string> _dimnames;
  float _grav;
  string _PHvar;
  string _PHBvar;
  float *_PH;
  float *_PHB;
  int _PHfd;
  int _PHBfd;
  size_t _num_ts;
  bool _is_open;
  bool _ok;
 };


 std::vector <size_t> _dims;
 std::vector <string> _vars3d;
 std::vector <string> _vars2dXY;
 std::vector <string> _vars3dExcluded;
 std::vector <string> _vars2dExcluded;
 std::vector <string> _timeStamps;
 std::vector <double> _times;
 NetCDFCollection *_ncdfc;
 float *_sliceBuffer;	// buffer for reading data
 int _ovr_fd;
 string _projString;
 Proj4API _proj4API;
 int _mapProj;
 float _dx;
 float _dy;
 float _cen_lat;
 float _cen_lon;
 float _pole_lat;
 float _pole_lon;
 float _grav;
 float _days_per_year;
 float _radius;	// planet radius for PlanetWRF
 float _p2si;	// multiplier for time conversion for PlanetWRF
 DerivedVarElevation *_elev;
 double _timeBias;

 
 std::vector <size_t> _GetSpatialDims(
    NetCDFCollection *ncdfc, string varname
 ) const;
 std::vector <string> _GetSpatialDimNames(
    NetCDFCollection *ncdfc, string varname
 ) const;


 int _InitAtts(NetCDFCollection *ncdfc);
 int _InitProjection(NetCDFCollection *ncdfc, float radius);
 int _InitDimensions(NetCDFCollection *ncdfc);
 int _InitVerticalCoordinates(NetCDFCollection *ncdfc);
 int _InitTime(NetCDFCollection *ncdfc);
 int _InitVars(NetCDFCollection *ncdfc);

 int _GetVerticalExtents(
	NetCDFCollection *ncdfc, size_t ts, double height[2]
 ) const; 

 int _GetLatLonExtentsCorners(
    NetCDFCollection *ncdfc,
    size_t ts, double lon_exts[2], double lat_exts[2],
    double lon_corners[4], double lat_corners[4]
 ) const;

};

};

#endif	//	_DCReaderWRF_h_
