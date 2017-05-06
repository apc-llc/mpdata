#ifndef	RAMSESDataMg_h
#define RAMSESDataMg_h

#include <list>
#include <string>
#include <vector>

#include <vapor/DataMgr.h>

using namespace std;

namespace VAPoR {

class VDF_API DataMgrRAMSES : public DataMgr {

public: 
  
 DataMgrRAMSES( const std::vector <string> &files, size_t mem_size );
 virtual ~DataMgrRAMSES();

protected:

 virtual int Initialize(std::vector <string> files);

 virtual string _GetGridType() const { 	return("regular"); }

 virtual int _GetNumTransforms() const { return(_numTransforms); };

 virtual long _GetNumTimeSteps() const { return(_userTime.size()); };

 virtual string _GetMapProjection() const {string empty; return (empty); };

 virtual void _GetBlockSize(size_t bs[3], int reflevel) const {
	bs[0] = bs[1] = bs[2] = _bs;
 }

 virtual std::vector <string> _GetVariables3D() const {
	return(_variableNames);
 }

 virtual std::vector <string> _GetVariables2DXY() const {
	std::vector <string> empty; return (empty); 
 }

 virtual std::vector <string> _GetVariables2DXZ() const {
	std::vector <string> empty; return (empty);
 }

 virtual std::vector <string> _GetVariables2DYZ() const {
	std::vector <string> empty; return (empty);
 }

 virtual int _OpenVariableRead(
	size_t timestep, const char *varname, int level, int
 );

 virtual bool _GetMissingValue(string varname, float &value) const {
	value=0.0;
	return(false);
 } 

 virtual int _CloseVariable();

 virtual std::vector<double> _GetExtents(size_t ts) const {
	return(_extents);
 }

 virtual std::vector<long> _GetPeriodicBoundary() const {return(_bool_false);};

 virtual std::vector<long> _GetGridPermutation() const { 
	vector <long> p; p.push_back(0); p.push_back(1); p.push_back(2);
	return(p); 
 }

 virtual const float *_GetDataRange() const {return(NULL);};

 virtual void _GetDim(size_t dim[3], int level) const ;

 virtual void _GetValidRegion( size_t min[3], size_t max[3], int) const; 

 virtual int _BlockReadRegion( const size_t *, const size_t *, float *region);
 
 virtual double _GetTSUserTime(size_t ts) const { 
	if (ts>=_userTime.size()) ts = _userTime.size()-1; 
	return(_userTime[ts]);
 };

 virtual void _GetTSUserTimeStamp(size_t ts, string &s) const;

 virtual int _VariableExists(
	size_t ts, const char *varname, int reflevel = 0, int lod  = 0
 ) const;


private: 
 vector <string> _dataset;
 size_t _base_level;
 int _numTransforms;
 std::vector <string> _variableNames;
 std::vector <double> _userTime;
 double _boxlen;
 size_t _bs;
 std::vector <double> _extents;
 std::vector<long> _bool_false;
 int _open_varid;
 size_t _open_timestep;
 int _open_level;
 string _open_path;

 int _varLookup(string varname);


};

};
#endif
