//************************************************************************
//                                                                       *
//           Copyright (C)  2004                                         *
//     University Corporation for Atmospheric Research                   *
//           All Rights Reserved                                         *
//                                                                       *
//************************************************************************/
//
//  File:        GribParser.h
//
//  Author:      Scott Pearse
//               National Center for Atmospheric Research
//               PO 3000, Boulder, Colorado
//
//  Date:        June 2014
//
//  Description: TBD 
//               
//

#ifndef GRIBPARSER_H
#define GRIBPARSER_H
#include "grib_api.h"
#include <vapor/DCReader.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <vapor/UDUnitsClass.h>
#ifdef _WINDOWS
#include "vapor/udunits2.h"
#else
#include <udunits2.h>
#endif

namespace VAPoR {

class UDUnits;

 struct MessageLocation {
	string fileName;
	int offset;	
 };

 class VDF_API Variable {
    public:
     Variable();
     ~Variable();
	 //! Add a udunits-created double time value to _unitTimes
	 void _AddTime(double t) {_unitTimes.push_back(t);}
	 int  _AddMessage(int msg) {_messages.push_back(msg); return 0;}
	 void _AddLevel(float lvl) {_levels.push_back(lvl);}
	 void _AddIndex(double time, float level, string file, int offset);

	 int GetOffset(double time, float level);
	 string GetFileName(double time, float level); 
	 std::vector<int> GetMessages() const {return _messages;}
	 std::vector<double> GetTimes() const {return _unitTimes;}
	 std::vector<float> GetLevels() const {return _levels;}
	 float GetLevel(int index) const {return _levels[index];}
	 void PrintTimes();
	 void PrintMessages();
	 void PrintIndex(double time, float level);
	 void _SortLevels() {sort(_levels.begin(), _levels.end());}
	 bool _Exists(double time) const;

    private:
	 //		  time             level
	 std::map<double, std::map<float, MessageLocation> > _indices;
     std::vector<int> _messages;
     std::vector<float> _levels;

	 //! list of time indices that a variable exists within
	 std::vector<size_t> _varTimes;
	 //! times stored in udunit2 format
	 std::vector<double> _unitTimes;  
 };

 class VDF_API DCReaderGRIB : public DCReader {
    public:
	 DCReaderGRIB();
	 ~DCReaderGRIB();

	/////
	// DCReader Virtual Functions
     virtual int OpenVariableRead(size_t timestep, string varname, 
                                  int reflevel=0, int lod=0);
	 virtual int CloseVariable() {return 0;}
	 virtual int ReadSlice(float *slice);
	 virtual bool VariableExists(size_t ts, string varname,
                                 int reflevel=0, int lod=0) const;     // not pure
	 virtual void GetLatLonExtents(size_t ts, double lon_exts[2],
                                   double lat_exts[2]) const;
	 virtual std::vector<std::string> GetVariables2DExcluded() const;
 	 virtual std::vector<std::string> GetVariables3DExcluded() const;

     virtual long GetNumTimeSteps() const;                                 // from Metadata.h
     virtual void GetGridDim(size_t dim[3]) const;                         // from Metadata.h
     virtual std::vector<string> GetVariables3D() const;                   // from Metadata.h
     virtual std::vector<string> GetVariables2DXY() const;                 // from Metadata.h
     virtual double GetTSUserTime(size_t ts) const {return _gribTimes[ts];}   // from Metadata.h
	 virtual std::vector<string> GetVariables2DXZ() const {                // from Metadata.h
        std::vector<string> empty; return(empty);};
     virtual std::vector<string> GetVariables2DYZ() const {                // from Metadata.h
        std::vector<string> empty; return(empty);};

     //virtual std::string GetCoordSystemType() const;
     
	 virtual void GetCoordinates(vector <double> &coords) {};
     Variable* Get3dVariable(string varname) {return _vars3d[varname];}
     Variable* Get2dVariable(string varname) {return _vars2d[varname];}
	// END DCReader Functions
	/////

	
	/////
	// Metadata Pure Virtual Functions
	// virtual void   GetGridDim(size_t dim[3]) const;
 	virtual void GetBlockSize(size_t bs[3], int reflevel) const { GetGridDim(bs); }
	virtual std::vector<double> GetExtents(size_t ts = 0) const {return _cartesianExtents;}
     virtual std::vector<long> GetPeriodicBoundary() const;				// Needs implementation!
     virtual std::vector<long> GetGridPermutation() const;				// Needs implementation!
     virtual void GetTSUserTimeStamp(size_t ts, std::string &s) const;


	
	// Metadata Virtual Functions
	 //virtual int GetNumTransforms() const {return(0); };
	 //virtual std::vector <size_t> GetCRatios() const {
     //				  std::vector <size_t> cr; cr.push_back(1); return(cr);}
	 //virtual std::string GetCoordSystemType() const { return("cartesian"); };
	 //virtual std::string GetGridType() const { return("regular"); };
	 //virtual std::vector<double> GetTSXCoords(size_t ts) const {
     //							std::vector <double> empty; return(empty);};
	 //virtual std::vector<double> GetTSYCoords(size_t ts) const {
     //							std::vector <double> empty; return(empty);};
	 //virtual std::vector<double> GetTSZCoords(size_t ts) const {
	 //				    	std::vector <double> empty; return(empty);};
	 //virtual std::vector <std::string> GetVariableNames() const;
	 virtual std::string GetMapProjection() const;
	 //virtual std::vector <std::string> GetCoordinateVariables() const {;
     //					std::vector <std::string> v; v.push_back("NONE");
	 //					v.push_back("NONE"); v.push_back("ELEVATION"); return(v);}
	 //virtual void GetDim(size_t dim[3], int reflevel = 0) const;
 	 //virtual void GetDimBlk(size_t bdim[3], int reflevel = 0) const;
 	 //virtual bool GetMissingValue(std::string varname, float &value) const
     //							  {return(false);};
	 //virtual void MapVoxToBlk(const size_t vcoord[3], size_t bcoord[3], 
	 //						int reflevel = -1) const;
	 //virtual void MapVoxToUser(size_t timestep, const size_t vcoord0[3], 
	 //						 double vcoord1[3], int ref_level = 0) const;
	 //void MapUserToVox(size_t timestep, const double vcoord0[3], 
	 //				   size_t vcoord1[3], int reflevel) const;
	 //virtual VarType_T GetVarType(const std::string &varname) const;
	 //virtual int IsValidRegion(const size_t min[3], const size_t max[3], 
	 //							int reflevel = 0) const;
	 //virtual int IsValidRegionBlk(const size_t min[3], const size_t max[3],
	 //							  int reflevel = 0) const;
	 //virtual bool IsCoordinateVariable(std::string varname) const;
	// END Metadata Virtual Functions
	/////
	


	/////
	// Convenience functions
	 int _Initialize(std::vector<std::map<std::string, std::string> > records);
	 int PrintVar(string var);
     float GetLevel(int index) {return _levels[index];}
     void PrintLevels();
	 void Print3dVars();
	 void Print2dVars();
	 void Print1dVars();
	 double BarometricFormula(const double pressure) const;
	 int _InitCartographicExtents(string mapProj,
                                  const std::vector <double> vertCoordinates,
                                  std::vector <double> &extents) const;
	/////

    private:
	 static int _sliceNum;
     int _Ni; 
     int _Nj;
	 double _minLat;
	 double _minLon;
	 double _maxLat;
	 double _maxLon;
	 string _openVar;
	 int _openTS; 	
 
     std::vector<double> _levels;
	 std::vector<double> _cartesianExtents;
     std::vector<double> _gribTimes;
	 std::map<std::string, Variable*> _vars1d;
     std::map<std::string, Variable*> _vars2d;
     std::map<std::string, Variable*> _vars3d;
	 UDUnits *_udunit;
 };

 class VDF_API GribParser {
	public:
	 GribParser();
	 ~GribParser();
	 int _LoadRecord(string file, size_t offset);
	 int _LoadAllRecordKeys(string file);// loads all keys
	 int _LoadRecordKeys(string file);	// loads only the keys that we need for vdc creation
	 int _VerifyKeys();					// verifies that key/values conform to our reqs
	 int _DataDump();					// dumps all data
     int _InitializeDCReaderGRIB();
	 DCReaderGRIB* GetDCReaderGRIB() {return _metadata;}

	private:
	 int _err;
	 std::vector<std::map<std::string, std::string> > _recordKeys;
	 std::vector<std::string> _consistentKeys;
	 std::vector<std::string> _varyingKeys;
	 bool _recordKeysVerified;
	 DCReaderGRIB *_metadata;

	 // vars for key iteration 
	 unsigned long _key_iterator_filter_flags;
	 char* _name_space;
	 int _grib_count;
     char* _value;
     size_t _vlen;

	 // vars for data dump
	 double *_values;
	 double _min,_max,_average;
	 size_t _values_len;
	 FILE* _in;
	 string _filename;
	 grib_handle *_h;
 };


 class Record {
	public:
	 Record();
	 Record(string file, int recordNum);
	 ~Record();
	 void setTime(string input) {time = input;}
	 void setUnits(string input) {units = input;}
	 void setNi(string input) {Ni = input;}
	 void setNj(string input) {Nj = input;}
	 void setGridType(string input) {gridType = input;}
	 void setVarName(string input) {varName = input;}
	 string getTime() {return time;}
	 string getUnits() {return units;}
	 string getNi() {return Ni;}
	 string getNj() {return Nj;}
	 string getGridType() {return gridType;}
	 string getVarName() {return varName;}

	private:
	 string time;
	 string units;
	 string Ni;
	 string Nj;
	 string gridType;
	 string varName;
 };
};
#endif // GRIBPARSER_H
