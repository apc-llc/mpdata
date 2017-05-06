#ifndef	ICONDataMg_h
#define ICONDataMg_h

#include <cstdlib>
#include <cstdio>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <iostream>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <limits>

#include <vapor/MyBase.h>
#include <vapor/DataMgr.h>
#include <vapor/common.h>

using namespace std;

struct kdtree;

namespace VAPoR {


class VDF_API DataMgrICON : public DataMgr {
  
public: 
  
 DataMgrICON( const vector <string> &files, size_t mem_size );
 virtual ~DataMgrICON();

protected:

 virtual int Initialize(const char *files);
 virtual string _GetGridType() const { 	return("regular"); }
 virtual int _GetNumTransforms() const { return(0); };
 virtual long _GetNumTimeSteps() const { return(this->NumberOfTimeSteps); };
 virtual string _GetMapProjection() const {string empty; return (empty); };
 virtual void _GetBlockSize(size_t bs[3], int reflevel) const;
 virtual vector <string> _GetVariables3D() const;
 virtual vector <string> _GetVariables2DXY() const;
 virtual vector <string> _GetVariables2DXZ() const {vector <string> empty; return (empty); };
 virtual vector <string> _GetVariables2DYZ() const {vector <string> empty; return (empty); };
 virtual int _OpenVariableRead( size_t timestep, const char *varname, int, int);
 virtual bool _GetMissingValue(string varname, float &value) const { value=this->missing_value; return(true);}; 
 virtual int _CloseVariable() {return (1);};
 virtual vector<double> _GetExtents(size_t ts) const;
 virtual vector<long> _GetPeriodicBoundary() const { return(bool_false); };
 virtual vector<long> _GetGridPermutation() const { return(bool_false); };
 virtual const float *_GetDataRange() const;
 virtual void _GetDim(size_t dim[3], int ) const;  
 virtual void _GetValidRegion( size_t min[3], size_t max[3], int) const; 
 virtual int _BlockReadRegion( const size_t *, const size_t *, float *region);
 
 virtual double _GetTSUserTime(size_t ts) const { return(0); };
 virtual void _GetTSUserTimeStamp(size_t ts, string &s) const {  }
 virtual int _VariableExists( size_t ts, const char *varname, int reflevel = 0,	int lod  = 0 ) const { return (1); };

private: 

  int GetDims();  
  int GetVars();
  void Remove_Duplicates(double * PointLon, double * PointLat, int temp_nbr_vertices, int * vertexID, int * nbr_cells); 
  void ResampleDataCell(const int width, const int height, float *data);
  void ResampleDataPoint(const int width, const int height, float *data);
  void findBoundingBox(double *bb_lon, double *bb_lat, double points[6]);
  int CreateLandSeaMask();
  int ConstructGridGeometry();
  int CreateKdTree();
  int XYSize();
  
  double* CellVar; 
  int *LandSeaMaskCell, *LandSeaMaskPoint;
  double diff_cv;
  int reg_res_x, reg_res_y;
  double oversampling;
  double missing_value;
  bool activeVar3D; 
  bool point_var;
  double clon_range[2], clat_range[2], vlon_range[2], vlat_range[2];
  int NumberOfTimeSteps;     
  
  int NumberOfCellVars;
  int NumberOfPointVars;
  int NumberOfVertLevels;
  int NumberOfCells;
  int NumberOfPoints;
  int PointsPerCell;
  bool interpolation; 
  std::vector<long> bool_false;

  int* vertexID;
  double* PointX;      		
  double* PointY;      	
  double* CellX;      		
  double* CellY;      	
  int *CellIndex, *PointIndex;      
  kdtree *CellTree, *PointTree;
  
  int streamID;
  int vlistID;
  int gridID;
  int zaxisID;
  int surfID;
  
  class Internal;
  Internal *Internals;
  
  float range[2];
};

};
#endif
