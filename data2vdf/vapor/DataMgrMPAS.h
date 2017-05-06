#ifndef	MPASDataMg_h
#define MPASDataMg_h

#include <list>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cfloat>
#include <algorithm>
#include <cstdio>
#include <limits>

#include <vapor/MyBase.h>
#include <vapor/DataMgr.h>
#include <vapor/common.h>
#include <vapor/NetCDFCollection.h>

using namespace std;

struct kdtree;

namespace VAPoR {

class VDF_API DataMgrMPAS : public DataMgr {

  
public: 
  
 DataMgrMPAS( const vector <string> &files, size_t mem_size );
 virtual ~DataMgrMPAS();

protected:

 virtual int Initialize(vector <string> files);
 virtual string _GetGridType() const { 	return("regular"); }
 virtual int _GetNumTransforms() const { return(0); };
 virtual long _GetNumTimeSteps() const { return(_numberOfTimeSteps); };
 virtual string _GetMapProjection() const {return (_proj4String); };
 virtual void _GetBlockSize(size_t bs[3], int reflevel) const;
 virtual vector <string> _GetVariables3D() const;
 virtual vector <string> _GetVariables2DXY() const;
 virtual vector <string> _GetVariables2DXZ() const {vector <string> empty; return (empty); };
 virtual vector <string> _GetVariables2DYZ() const {vector <string> empty; return (empty); };
 virtual int _OpenVariableRead( size_t timestep, const char *varname, int, int);
 virtual bool _GetMissingValue(string varname, float &value) const {
	value=_missing_value; return(_has_missing);
 }; 
 virtual int _CloseVariable() {return (0);};
 virtual vector<double> _GetExtents(size_t ts) const;
 virtual vector<long> _GetPeriodicBoundary() const { return(_bool_false); };
 virtual vector<long> _GetGridPermutation() const { return(_bool_false); };
 virtual const float *_GetDataRange() const {return(_range);};
 virtual void _GetDim(size_t dim[3], int ) const;  
 virtual void _GetValidRegion( size_t min[3], size_t max[3], int) const; 
 virtual int _BlockReadRegion( const size_t *, const size_t *, float *region);
 
 virtual double _GetTSUserTime(size_t ts) const { return(0); };
 virtual void _GetTSUserTimeStamp(size_t ts, string &s) const {  }
 virtual int _VariableExists( size_t ts, const char *varname, int reflevel = 0,	int lod  = 0 ) const { return (1); };

private: 

  enum var_type_t {CELL, VERT, EDGE};

  bool ValidFile();
  bool GetDims();  
  bool GetVars(string cellDimName, string pointDimName, string edgeDimName);
  void ResampleDataCell(const int width, const int height, float *data);
  void _resampleDataCell(const int width, const int height, float *data);
  void _resampleDataPoint(
    const int width, const int height, 
    const float *resampleWeights, const int *resampleIds, float *data
  );
  int ConstructGridGeometry();
  void _constructCellResampleWeights(
    const int *cellsOnVertex, const int *vertsOnCell,
    const int *nVertsOnCell,
    float *cellResampleWeights, int *cellResampleIds
  );
  void _constructPointResampleWeights(
	const int *vertsOnCell, const int *nVertsOnCell,
	const double *pointLons, const double *pointLats, int nLatLons,
	float *pointResampleWeights, int *pointResampleIds
  );

  void _constructEdgeResampleWeights(
	const int *edgesOnCell, const int *nEdgesOnCell, const int *verticesOnEdge,
	const double *vertLons, const double *vertLats, int nLatLons,
	float *edgeResampleWeights, int *edgeResampleIds
  );

  int _constructResampleWeights();
  void CreateKdTree();
  void XYSize();

  bool _barycentricCoordsForCell(
	const vector <int> &vertList, int cellid, const double pt[2],
	const double *pointLons, const double *pointLats, int nLatLons,
	float *lambda, int *vertIds
  ) const;

  bool _barycentricCoordsForTriangle(
	const int *cellsOnVertex, int pointid, int copid, const double pt[2],
	float *lambda, int cellIds[3], int &n
  ) const;
  
  NetCDFCollection _ncCollection;
  float* _varBuf1; 
  float* _varBuf2;	// space to buffer variables on native grid 
  float *_cellResampleWeights;
  int *_cellResampleIds;
  float *_pointResampleWeights;
  int *_pointResampleIds;
  float *_edgeResampleWeights;
  int *_edgeResampleIds;
  int _reg_res_x, _reg_res_y;
  double _oversampling;
  double _missing_value;
  bool _has_missing;
  bool _interpolation; 
  bool _activeVar3D; 
  var_type_t _var_type;
  std::vector<long> _bool_false;
  string _cells_name;
  string _vertices_name;
  string _edges_name;
  string _vertexdegree_name;
  string _maxedges_name;
  string _time_name;
  string _vertlevels_name;
  string _vertlevelsP1_name;
  double _clon_range[2], _clat_range[2], _vlon_range[2], _vlat_range[2];
  double _elon_range[2], _elat_range[2];
  double _lon_range[2], _lat_range[2];
  double _lon_rangePCS[2], _lat_rangePCS[2];
  int _numberOfTimeSteps;      
  
  int _numberOfVertLevels;
  int _numberOfVertLevelsP1;
  int _numberOfCells;
  int _numberOfPoints;
  int _numberOfEdges;

  int _cellDegreeMax;
  int _vertexDegree;
  
  double* _cellLon;      		
  double* _cellLat;      		
  double* _pointLon;      		
  double* _pointLat;      		
  double* _edgeLon;      		
  double* _edgeLat;      		
  int *_cellIndex;
  kdtree *_cellTree;
  string _proj4String;

  bool _swapZ;

  std::vector <string> _cellVars;
  std::vector <string> _pointVars;
  std::vector <string> _edgeVars;
  
  float _range[2];
  
};

};
#endif
