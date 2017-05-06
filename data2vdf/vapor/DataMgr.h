//
//      $Id$
//

#ifndef	_DataMgr_h_
#define	_DataMgr_h_

#include <list>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <vapor/MyBase.h>
#include <vapor/BlkMemMgr.h>
#include <vapor/common.h>
#include <vapor/RegularGrid.h>
#include <vapor/LayeredGrid.h>
#include <vapor/SphericalGrid.h>
#include <vapor/StretchedGrid.h>

namespace VAPoR {
class PipeLine;

//
//! \class DataMgr
//! \brief A cache based data reader
//! \author John Clyne
//! \version $Revision$
//! \date    $Date$

//!
//! The DataMgr class is an abstract class that defines public methods for
//! accessing (reading) 2D and 3D field variables. The class implements a 
//! memory cache to speed data access -- once a variable is read it is
//! stored in cache for subsequent access. The DataMgr class is abstract:
//! it declares a number of protected pure virtual methods that must be
//! implemented by specializations of this class.
//
class VDF_API DataMgr : public VetsUtil::MyBase {

public:

 //! An enum of variable types. Variables defined in a data collection
 //! may be either three-dimensional
 //! (\p VAR3D), or two-dimensional. In the latter case the two-dimesional
 //! data are constrained to lie in the XY, XZ, or YZ coordinate planes
 //! of a 3D volume
 //!
 enum VarType_T {
    VARUNKNOWN = -1,
    VAR3D, VAR2D_XY, VAR2D_XZ, VAR2D_YZ
 };

 //! Constructor for the DataMgr class. 
 //!
 //! \param[in] mem_size Size of memory cache to be created, specified 
 //! in MEGABYTES!!
 //!
 //! \note The success or failure of this constructor can be checked
 //! with the GetErrCode() method.
 //! 
 //! \sa GetErrCode(), 
 //
 DataMgr(
	size_t mem_size
 );

 virtual ~DataMgr(); 

 //! \copydoc _GetDim()
 //
 virtual void   GetDim(size_t dim[3], int reflevel = 0) const {
	return(_GetDim(dim, reflevel));
 }

 //! \copydoc _GetNumTransforms()
 //
 int GetNumTransforms() const { return(_GetNumTransforms()); };

 //! \copydoc GetCRatios()
 //
 virtual vector <size_t> GetCRatios() const { return( _GetCRatios()); }

 //! \copydoc _GetCoordSystemType()
 //
 virtual string GetCoordSystemType() const {
	return(_GetCoordSystemType());
 }

 //! \copydoc _GetGridType()
 //
 virtual string GetGridType() const { return(_GetGridType()); };

 //! \copydoc _GetExtents()
 //
 virtual vector<double> GetExtents(size_t ts = 0);

 //! \copydoc _GetNumTimeSteps()
 //
 virtual long GetNumTimeSteps() const { return(_GetNumTimeSteps()); };


 //! \copydoc _GetMapProjection()
 //
 virtual string GetMapProjection() const {
	return(_GetMapProjection()); 
 }

 virtual vector <string> GetVariableNames() const;

 //! \copydoc _GetVariables3D()
 //
 virtual vector <string> GetVariables3D() const;

 //! \copydoc _GetVariables2DXY()
 //
 virtual vector <string> GetVariables2DXY() const;

 //! \copydoc _GetVariablesXZ()
 //
 virtual vector <string> GetVariables2DXZ() const;

 //! \copydoc _GetVariablesYZ()
 //
 virtual vector <string> GetVariables2DYZ() const;

 //! \copydoc _GetCoordinateVariables()
 //
 virtual vector <string> GetCoordinateVariables() const {
	return(_GetCoordinateVariables());
 };

 //! \copydoc _GetPeriodicBoundary()
 //
 virtual vector<long> GetPeriodicBoundary() const {
	return(_GetPeriodicBoundary());
 };

 //! \copydoc _GetGridPermutation()
 //
 virtual vector<long> GetGridPermutation() const {
	return(_GetGridPermutation());
 };

 //! \copydoc _GetTSUserTime()
 //
 virtual double GetTSUserTime(size_t ts) const {
	return(_GetTSUserTime(ts));
 };

 //! \copydoc _GetTSUserTimeStamp()
 //
 virtual void GetTSUserTimeStamp(size_t ts, string &s) const {
	return(_GetTSUserTimeStamp(ts, s));
 };

 //! Read in and return a subregion from the dataset.
 //!
 //! GetGrid() will first check to see if the requested region resides
 //! in cache. If so, no reads are performed. If the named variable is not
 //! in cache, GetGrid() will next check to see if the variable can
 //! be calculated by recursively executing PipeLine stages 
 //! (\see NewPipeline()). Finally,
 //! if the varible is not the result of PipeLine execution GetGrid()
 //! will attempt to access the varible through methods implemented by
 //! derived classes of the DataMgr class.
 //!
 //! The \p ts, \p varname, \p lod, and \p level pararmeter tuple identifies 
 //! the time step, variable name, level-of-detail, and refinement level, 
 //! respectively, of the requested volume.
 //! The \p min and \p max vectors identify the minium and
 //! maximum extents, in voxel coordinates, of the subregion of interest. The
 //! minimum valid value of \p min is (0,0,0), the maximum valid value of
 //! \p max is (nx-1,ny-1,nz-1), where nx, ny, and nz are the 
 //! voxel dimensions
 //! of the volume at the resolution indicated by \p level. I.e.
 //! the coordinates are specified relative to the desired volume
 //! resolution. If the requested region is available, GetGrid() returns 
 //! a pointer to a RegularGrid class containg the requested
 //! subregion. It is the callers responsbility to delete the 
 //! returned pointer when it is no longer needed. Subsequent calls 
 //! to GetGrid()
 //! may invalidate the memory space returned by previous calls unless
 //! the \p lock parameter is set, in which case the grid returned by
 //! GetGrid() is locked into memory until freed by a call the
 //! UnlockGrid() method (or the class is destroyed).
 //!
 //! GetGrid() will fail if the requested data are not present. The
 //! VariableExists() method may be used to determine if the data
 //! identified by a (resolution,timestep,variable) tupple are
 //! available on disk.
 //!
 //! \note The \p lock parameter increments a counter associated 
 //! with the requested region of memory. The counter is decremented
 //! when UnlockGrid() is invoked.
 //!
 //! If \p varname is the empty string, a pointer to dataless RegularGrid
 //! is returned. 
 //!
 //! \param[in] ts A valid time step between 0 and GetNumTimesteps()-1
 //! \param[in] varname A valid variable name  or the empty string
 //! \param[in] reflevel Refinement level requested
 //! \param[in] lod Level of detail requested
 //! \param[in] min Minimum region bounds in voxels
 //! \param[in] max Maximum region bounds in voxels
 //! \param[in] lock If true, the memory region will be locked into the 
 //! cache (i.e. valid after subsequent GetRegion() calls).
 //! \retval ptr A pointer to a region containing the desired data, or NULL
 //! if the region can not be extracted.
 //! \sa NewPipeline(), GetErrMsg()
 //
virtual RegularGrid   *GetGrid(
    size_t ts,
    string varname,
    int reflevel,
    int lod,
    const size_t min[3],
    const size_t max[3],
    bool lock = false
 );

 //! Unlock a floating-point region of memory 
 //!
 //! Decrement the lock counter associatd with a 
 //! region of memory, and if zero,
 //! unlock region of memory previously locked GetRegion(). 
 //! When the lock counter reaches zero the region is simply 
 //! marked available for
 //! internal garbage collection during subsequent GetRegion() calls
 //!
 //! \param[in] rg A pointer to a RegularGrid previosly 
 //! returned by GetGrid()
 //! \retval status Returns a non-negative value on success
 //!
 //! \sa GetRegion(), GetRegion()
 //
 void UnlockGrid(const RegularGrid *rg);

 //! \deprecated
 //
 int UnlockRegion(const float *) {return(-1);};

 //! Clear the memory cache
 //!
 //! This method clears the internal memory cache of all entries
 //
 void	Clear();

 //! Return the current data range as a two-element array
 //!
 //! This method returns the minimum and maximum data values
 //! for the indicated time step and variable
 //!
 //! \param[in] ts A valid time step between 0 and GetNumTimesteps()-1
 //! \param[in] varname Name of variable 
 //! \param[in] reflevel Refinement level requested
 //! \param[in] lod Level of detail requested
 //! \param[out] range  A two-element vector containing the current 
 //! minimum and maximum.
 //! \retval status Returns a non-negative value on success 
 //! quantization mapping.
 //!
 //
 int GetDataRange(
	size_t ts, const char *varname, float range[2],
	int reflevel = 0, int lod = 0
 );

 //! Return the valid region bounds for the specified region
 //!
 //! This method returns the minimum and maximum valid coordinate
 //! bounds (in voxels) of the subregion indicated by the timestep
 //! \p ts, variable name \p varname, and refinement level \p reflevel
 //! for the indicated time step and variable. Data are guaranteed to
 //! be available for this region.
 //!
 //!
 //! \param[in] ts A valid time step between 0 and GetNumTimesteps()-1
 //! \param[in] varname Name of variable 
 //! \param[in] reflevel Refinement level of the variable
 //! \param[out] min Minimum coordinate bounds (in voxels) of volume
 //! \param[out] max Maximum coordinate bounds (in voxels) of volume
 //! \retval status A non-negative int is returned on success
 //!
 //
 int GetValidRegion(
    size_t ts,
    const char *varname,
    int reflevel,
    size_t min[3],
    size_t max[3]
 );


 //! \copydoc _VariableExists()
 //
 virtual int VariableExists(
	size_t ts,
	const char *varname,
	int reflevel = 0,
	int lod = 0
 ) ;

 bool BestMatch(
    size_t ts, const char *varname, int req_reflevel, int req_lod,
    int &reflevel, int &lod
 );


 //!
 //! Add a pipeline stage to produce derived variables
 //!
 //! Add a new pipline stage for derived variable calculation. If a 
 //! pipeline already exists with the same
 //! name it is replaced. The output variable names are added to
 //! the list of variables available for this data 
 //! set (see GetVariables3D, etc.).
 //!
 //! An error occurs if:
 //!
 //! \li The output variable names match any of the native variable 
 //! names - variable names returned via _GetVariables3D(), etc. 
 //! \li The output variable names match the output variable names
 //! of pipeline stage previously added with NewPipeline()
 //! \li A circular dependency is introduced by adding \p pipeline
 //!
 //! \retval status A negative int is returned on failure.
 //!
 int NewPipeline(PipeLine *pipeline);

 //!
 //! Remove the named pipline if it exists. Otherwise this method is a
 //! no-op
 //!
 //! \param[in] name The name of the pipeline as returned by 
 //! PipeLine::GetName()
 //!
 void RemovePipeline(string name);

 //! Return true if the named variable is the output of a pipeline
 //!
 //! This method returns true if \p varname matches a variable name
 //! in the output list (PipeLine::GetOutputs()) of any pipeline added
 //! with NewPipeline()
 //!
 //! \sa NewPipeline()
 //
 bool IsVariableDerived(string varname) const;

 //! Return true if the named variable is availble from the derived 
 //! classes data access methods. 
 //!
 //! A return value of true does not imply that the variable can
 //! be read (\see VariableExists()), only that it is part of the 
 //! data set known to the derived class
 //!
 //! \sa NewPipeline()
 //
 bool IsVariableNative(string varname) const;

 virtual VarType_T GetVarType(const string &varname) const; 

 bool GetMissingValue(string varname, float &value) const;


	
 //! Purge the cache of a variable
 //!
 //! \param[in] varname is the variable name
 //!
 void PurgeVariable(string varname);

 //! Map floating point coordinates to integer voxel offsets.
 //!
 //! Map floating point coordinates, specified relative to a
 //! user-defined coordinate system, to the closest integer voxel
 //! coordinates for a voxel at a given refinement level.
 //! The integer voxel coordinates, \p vcoord1,
 //! returned are specified relative to the refinement level
 //! indicated by \p reflevel for time step, \p timestep.
 //! The mapping is performed by using linear interpolation
 //! Results are undefined if \p vcoord0 is outside of the volume
 //! boundary.
 //!
 //! If a user coordinate system is not defined for the specified
 //! time step, \p timestep, the global extents for the VDC will
 //! be used.
 //!
 //! \param[in] timestep Time step of the variable  If an invalid
 //! timestep is supplied the global domain extents are used.
 //! \param[in] vcoord0 Coordinate of input point in floating point
 //! coordinates
 //! \param[out] vcoord1 Integer coordinates of closest voxel, at the
 //! indicated refinement level, to the specified point.
 //! integer coordinates
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined for the VDC. In fact,
 //! any invalid value is treated as the maximum refinement level
 //!
 //! \sa GetGridType(), GetExtents()
 //
 virtual void   MapUserToVox(
    size_t timestep,
    const double vcoord0[3], size_t vcoord1[3], int reflevel = 0, int lod = 0
 );

 //! Map integer voxel coordinates to user-defined floating point coords.
 //!
 //! Map the integer coordinates of the specified voxel to floating
 //! point coordinates in a user defined space. The voxel coordinates,
 //! \p vcoord0 are specified relative to the refinement level
 //! indicated by \p reflevel for time step \p timestep.
 //! The mapping is performed by using linear interpolation
 //! The user coordinates are returned in \p vcoord1.
 //! Results are undefined if vcoord is outside of the volume
 //! boundary.
 //!
 //! \param[in] timestep Time step of the variable. If an invalid
 //! timestep is supplied the global domain extents are used.
 //! \param[in] vcoord0 Coordinate of input voxel in integer (voxel)
 //! coordinates
 //! \param[out] vcoord1 Coordinate of transformed voxel in user-defined,
 //! floating point  coordinates
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined. In fact,
 //! any invalid value is treated as the maximum refinement level
 //!
 //! \sa Metatdata::GetGridType(), GetExtents(),
 //! GetTSXCoords()
 //
 virtual void   MapVoxToUser(
    size_t timestep,
    const size_t vcoord0[3], double vcoord1[3], int ref_level = 0, int lod=0
 );

 //!
 //! Get voxel coordinates of grid containing a region
 //!
 //! Calculates the IJK voxel coordinates of the smallest grid
 //! completely containing the region defined by the user 
 //! coordinates \p minu and \p maxu
 //!
 //! \param[in] timestep Time step of the variable  If an invalid
 //! timestep is supplied the global domain extents are used.
 //! \param[in] minu User coordinates of minimum coorner
 //! \param[in] maxu User coordinates of maximum coorner
 //! \param[out] min Integer coordinates of minimum coorner
 //! \param[out] max Integer coordinates of maximum coorner
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined for the VDC. In fact,
 //! any invalid value is treated as the maximum refinement level
 //!
 virtual void    GetEnclosingRegion(
    size_t ts, const double minu[3], const double maxu[3],
    size_t min[3], size_t max[3],
    int reflevel = 0, int lod = 0
 );

 //!
 //! Returns true if the named variable is a coordinate variable
 //!
 //! This method is a convenience function that returns true if  
 //! if the variable named by \p varname is a coordinate variable. A
 //! variable is a coordinate variable if it is returned by 
 //! GetCoordinateVariables();
 //!
 //! \sa GetCoordinateVariables()
 //
 virtual bool IsCoordinateVariable(string varname) const;

 //! Print out contents of data cache
 //!
 //! Debugging method for dumping out the contents of the data cache
 //!
 void PrintCache(std::ostream &o);

 //! Set the default interpolation order used by RegularGrid objects
 //! returned by the DataMgr.
 //!
 //! This method sets the default interpolation order for any returned
 //! RegularGrid class objects by passing \p order to 
 //! RegularGrid::SetInterpolationOrder();
 //!
 //! \param[in] order Interpolation order passed to 
 //! RegularGrid::SetInterpolationOrder()
 //!
 //! \sa GetInterpolationOrder()
 //!
 void SetInterpolationOrder(int order) {_interpolationOrder = order;}

 //! Return the default interpolation order
 //! \sa SetInterpolationOrder()
 //!
 int GetInterpolationOrder() const {return(_interpolationOrder); }

protected:


 const vector<string> emptyVec;

 // The protected methods below are pure virtual and must be implemented by any 
 // child class  of the DataMgr.

 //! Get the dimension of a volume
 //!
 //! Returns the X,Y,Z coordinate dimensions of all data variables
 //! in grid (voxel) coordinates at the resolution
 //! level indicated by \p reflevel. Hence, all variables of a given
 //! type (3D or 2D)
 //! must have the same dimension. If \p reflevel is -1 (or the value
 //! returned by GetNumTransforms()) the native grid resolution is
 //! returned. In fact, any value outside the valid range is treated
 //! as the maximum refinement level
 //!
 //! \param[in] reflevel Refinement level of the variable
 //! \param[out] dim A three element vector (ordered X, Y, Z) containing the
 //! voxel dimensions of the data at the specified resolution.
 //!
 //! \sa GetNumTransforms()
 //
 virtual void   _GetDim(size_t dim[3], int reflevel = 0) const = 0;


 //! Return the internal blocking factor at a given refinement level
 //!
 //! For multi-resolution data this method returns the dimensions
 //! of a data block at refinement level \p reflevel, where reflevel
 //! is in the range 0 to GetNumTransforms(). A value of -1 may be
 //! specified to indicate the maximum refinement level. In fact,
 //! any value outside the valid refinement level range will be treated
 //! as the maximum refinement level.
 //!
 //! \param[in] reflevel Refinement level
 //! \param[bs] dim Transformed dimension.
 //!
 //! \retval bs  A three element vector containing the voxel dimension of
 //! a data block
 //
 virtual void _GetBlockSize(size_t bs[3], int reflevel) const = 0;

 //! Return number of transformations in hierarchy
 //!
 //! For multi-resolution data this method returns the number of
 //! coarsened approximations present. If no approximations
 //! are available - if only the native data are present - the return
 //! value is 0.
 //!
 //! \retval n  The number of coarsened data approximations available
 //
 virtual int _GetNumTransforms() const = 0;

 //! Return the compression ratios available.
 //!
 //! For data sets offering level-of-detail, the method returns a
 //! vector of integers, each specifying an available compression factor.
 //! For example, a factor of 10 indicates a compression ratio of 10:1.
 //! The vector returned is sorted from highest compression ratio
 //! to lowest. I.e. the most compressed data maps to index 0 in
 //! the returned vector.
 //!
 //! \retval cr A vector of one or more compression factors
 //
 virtual vector <size_t> _GetCRatios() const {
	vector <size_t> cr; cr.push_back(1); return(cr);
 }

 //! Return the coordinate system type. One of \b cartesian or \b spherical
 //! \retval type
 //!
 //
 virtual string _GetCoordSystemType() const { return("cartesian"); };

 //! Return the grid type. One of \b regular, \b stretched, \b block_amr,
 //! or \b spherical
 //!
 //! \retval type
 //!
 //
 virtual string _GetGridType() const { return("regular"); };

 //! Return the domain extents specified in user coordinates
 //!
 //! Return the domain extents specified in user coordinates
 //! for the indicated time step. Variables in the data have
 //! spatial positions defined in a user coordinate system.
 //! These positions may vary with time. This method returns
 //! min and max bounds, in user coordinates, of all variables
 //! at a given time step.
 //!
 //! \param[in] ts A valid data set time step in the range from zero to
 //! GetNumTimeSteps() - 1. If \p ts is out of range, GetExtents()
 //! will return a reasonable default value.
 //!
 //! \retval extents A six-element array containing the min and max
 //! bounds of the data domain in user-defined coordinates. The first
 //! three elements specify the minimum X, Y, and Z bounds, respectively,
 //! the second three elements specify the maximum bounds.
 //!
 //
 virtual vector<double> _GetExtents(size_t ts = 0) const = 0;

 //! Return the X dimension coordinate array, if it exists
 //!
 //! For stretched grids, _GetGridType() == "stretched", this method
 //! returns the X component of the grid user coordinates. This method
 //! is only called for stretched grids. 
 //!
 //! \retval value An array of monotonically changing values specifying
 //! the X dimension user coordinates, in a user-defined coordinate 
 //! system, of each
 //! YZ sample plane. An empty vector is returned if the coordinate
 //! dimension array is not defined for the specified time step.
 //!
 //! \sa _GetGridType(), GetTSXCoords()
 //!
 //
 virtual vector <double> _GetTSXCoords(size_t ts) const {
	vector <double> empty; return(empty);
 }

 //! Return the Y dimension coordinate array, if it exists
 //!
 virtual vector <double> _GetTSYCoords(size_t ts) const {
	vector <double> empty; return(empty);
 }

 //! Return the Z dimension coordinate array, if it exists
 //!
 virtual vector <double> _GetTSZCoords(size_t ts) const {
	vector <double> empty; return(empty);
 }

 //! Return the number of time steps in the data collection
 //!
 //! \retval value The number of time steps
 //!
 //
 virtual long _GetNumTimeSteps() const = 0;


 //! Return the Proj4 map projection string.
 //!
 //! \retval value An empty string if a Proj4 map projection is
 //! not available, otherwise a properly formatted Proj4 projection
 //! string is returned.
 //!
 //
 virtual string _GetMapProjection() const {string empty; return (empty); };

 //!
 //! \retval value is a space-separated list of 3D variable names.
 //! An emptry string is returned if no variables of this type are present
 //!
 //
 virtual vector <string> _GetVariables3D() const = 0;

 //! Return the names of the 2D, XY variables in the collection
 //!
 //! \retval value is a space-separated list of 2D XY variable names
 //! An emptry string is returned if no variables of this type are present
 //!
 //
 virtual vector <string> _GetVariables2DXY() const = 0;

 //! Return the names of the 2D, XZ variables in the collection
 //!
 //! \retval value is a space-separated list of 2D ZY variable names
 //! An emptry string is returned if no variables of this type are present
 //!
 //
 virtual vector <string> _GetVariables2DXZ() const = 0;

 //! Return the names of the 2D, YZ variables in the collection
 //!
 //! \retval value is a space-separated list of 2D YZ variable names
 //! An emptry string is returned if no variables of this type are present
 //!
 //
 virtual vector <string> _GetVariables2DYZ() const = 0;

 //! Return the names of the coordinate variables.
 //!
 //! This method returns a three-element vector naming the
 //! X, Y, and Z coordinate variables, respectively. The special
 //! name "NONE" indicates that a coordinate variable name does not exist
 //! for a particular dimension.
 //!
 //! \note The existence of a coordinate variable name does not imply
 //! the existence of the coordinate variable itself.
 //!
 //! \retval vector is three-element vector of coordinate variable names.
 //!
 //
 virtual vector <string> _GetCoordinateVariables() const { 
	vector <string> v;
	v.push_back("NONE"); v.push_back("NONE"); v.push_back("ELEVATION");
	return(v);
 }


 //! Return a three-element boolean array indicating if the X,Y,Z
 //! axes have periodic boundaries, respectively.
 //!
 //! \retval boolean-vector
 //!
 //
 virtual vector<long> _GetPeriodicBoundary() const = 0;


 //! Return a three-element integer array indicating the coordinate
 //! ordering permutation.
 //!
 //! \retval integer-vector
 //!
 virtual vector<long> _GetGridPermutation() const {
	vector <long> v; v.push_back(0); v.push_back(1); v.push_back(2); return(v);
 };

 //! Return the time for a time step
 //!
 //! This method returns the time, in user-defined coordinates,
 //! associated with the time step, \p ts. Variables such as
 //! velocity field components that are expressed in distance per
 //! units of time are expected to use the same time coordinates
 //! as the values returned by this mehtod.
 //!
 //! \param[in] ts A valid data set time step in the range from zero to
 //! GetNumTimeSteps() - 1.
 //!
 //! \retval value The user time at time step \p ts. If \p ts is outside
 //! the valid range zero is returned.
 //!
 //
 virtual double _GetTSUserTime(size_t ts) const = 0;


 //! Return the time for a time step
 //!
 //! This method returns the user time,
 //! associated with the time step, \p ts, as a formatted string.
 //! The returned time stamp is intended to be used for annotation
 //! purposes
 //!
 //! \param[in] ts A valid data set time step in the range from zero to
 //! GetNumTimeSteps() - 1.
 //! \param[out] s A formated time string. If \p ts is outside
 //! the valid range zero the empty string is returned.
 //!
 //
 virtual void _GetTSUserTimeStamp(size_t ts, string &s) const = 0;


 //! Returns true if indicated data volume is available
 //!
 //! Returns true if the variable identified by the timestep, variable
 //! name, refinement level, and level-of-detail is present in 
 //! the data set. Returns 0 if
 //! the variable is not present.
 //! \param[in] ts A valid time step between 0 and GetNumTimesteps()-1
 //! \param[in] varname A valid variable name
 //! \param[in] reflevel Refinement level requested. The coarsest 
 //! refinement level is 0 (zero). A value of -1 indicates the finest
 //! refinement level contained in the VDC.
 //! \param[in] lod Compression level of detail requested. The coarsest 
 //! approximation level is 0 (zero). A value of -1 indicates the finest
 //! refinement level contained in the VDC.
 //
 virtual int _VariableExists(
	size_t ts,
	const char *varname,
	int reflevel = 0,
	int lod = 0
 ) const = 0;


 //! Open the named variable for reading
 //!
 //! This method prepares the multi-resolution, multi-lod data volume, 
 //! indicated by a
 //! variable name and time step pair, for subsequent read operations by
 //! methods of this class.  Furthermore, the number of the refinement level
 //! parameter, \p reflevel indicates the resolution of the volume in
 //! the multiresolution hierarchy, and the \p lod parameter indicates
 //! the level of detail. 
 //!
 //! The valid range of values for
 //! \p reflevel is [0..max_refinement], where \p max_refinement is the
 //! maximum finement level of the data set: GetNumTransforms().
 //! A value of zero indicates the
 //! coarsest resolution data, a value of \p max_refinement indicates the
 //! finest resolution data.
 //!
 //! The valid range of values for
 //! \p lod is [0..max_lod], where \p max_lod is the
 //! maximum lod of the data set: GetCRatios().size() - 1.
 //! A value of zero indicates the
 //! highest compression ratio, a value of \p max_lod indicates the
 //! lowest compression ratio.
 //!
 //! An error occurs, indicated by a negative return value, if the
 //! volume identified by the {varname, timestep, reflevel, lod} tupple
 //! is not present on disk. Note the presence of a volume can be tested
 //! for with the VariableExists() method.
 //! \param[in] timestep Time step of the variable to read
 //! \param[in] varname Name of the variable to read
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined for the VDC
 //! \param[in] lod Level of detail requested. A value of -1
 //! indicates the lowest compression level available for the VDC
 //!
 //! \sa GetVariables3D(), GetVariables2DXY(), GetNumTransforms()
 //!
 virtual int	_OpenVariableRead(
	size_t timestep,
	const char *varname,
	int reflevel = 0,
	int lod = 0
 ) = 0;

 //! Return the valid bounds of the currently opened region
 //!
 //! The data model permits the storage of volume subregions. This method may
 //! be used to query the valid domain of the currently opened volume. Results
 //! are returned in voxel coordinates, relative to the refinement level
 //! indicated by \p reflevel.
 //!
 //!
 //! \param[out] min A pointer to the minimum bounds of the subvolume
 //! \param[out] max A pointer to the maximum bounds of the subvolume
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined for the VDC
 //!
 //! \retval status Returns a negative value if the volume is not opened
 //! for reading.
 //!
 //! \sa _OpenVariableRead()
 //
 virtual void _GetValidRegion(
    size_t min[3], size_t max[3], int reflevel
 ) const = 0;

 //! Return the data range for the currently open variable
 //!
 //! The method returns the minimum and maximum data values, respectively, 
 //! for the variable currently opened. If the variable is not opened,
 //! or if it is opened for writing, the results are undefined.
 //!
 //! \return range A pointer to a two-element array containing the
 //! Min and Max data values, respectively. If the derived class' 
 //! implementation of this method returns NULL, the DataMgr class
 //! will compute the min and max itself.
 //!
 virtual const float *_GetDataRange() const { return(NULL);};

 //! Return the value of the missing data value
 //!
 //! This method returns the value of the missing data value for 
 //! the currently opened variable. If no missing data are present
 //! the method returns false
 //!
 //! \param[out] value The missing data value. Undefined if no missing
 //! value is present.
 //!
 //! \retval returns true if missing data are present
 //
 virtual bool _GetMissingValue(string varname, float &value) const {
	return(false); 
 };


 //! Read in and return a subregion from the currently opened multiresolution
 //! data volume.
 //!
 //! The dimensions of the region are provided in block coordinates. However,
 //! the returned region is not blocked. 
 //! 
 //!
 //! \param[in] bmin Minimum region extents in block coordinates
 //! \param[in] bmax Maximum region extents in block coordinates
 //! \param[out] region The requested volume subregion
 //!
 //! \retval status Returns a non-negative value on success
 //! \sa _OpenVariableRead(), _GetBlockSize()
 //
 virtual int    _BlockReadRegion(
    const size_t bmin[3], const size_t bmax[3],
    float *region
 ) = 0;

 //! Close the currently opened variable.
 //!
 //! \sa _OpenVariableRead()
 //
 virtual int	_CloseVariable() = 0;

private:
 int _interpolationOrder;

 size_t _mem_size;

 typedef struct {
	size_t ts;
	string varname;
	int reflevel;
	int lod;
	size_t min[3];
	size_t max[3];
	int lock_counter;
	float *blks;
 } region_t;

 // a list of all allocated regions
 list <region_t> _regionsList;

 BlkMemMgr	*_blk_mem_mgr;

 vector <PipeLine *> _PipeLines;


 //
 // Cache for various metadata attributes 
 //
 class VarInfoCache  {
 public:

	//
	// min and max variable value range
	//
	bool GetRange(
		size_t ts, string varname, int ref, int lod, float range[2]
	) const;
	void SetRange(
		size_t ts, string varname, int ref, int lod, const float range[2]
	);
	void PurgeRange(
		size_t ts, string varname, int ref, int lod
	);

	//
	// min and max region extents
	//
	bool GetRegion(
		size_t ts, string varname, int ref, size_t min[3], size_t max[3]
	) const;
	void SetRegion(
		size_t ts, string varname, int ref, const size_t min[3], 
		const size_t max[3]
	);
	void PurgeRegion(
		size_t ts, string varname, int ref
	);

	//
	// does the variable exist?
	//
	bool GetExist(
		size_t ts, string varname, int ref, int lod, bool &exist
	) const;
	void SetExist(size_t ts, string varname, int ref, int lod, bool exist);
	void PurgeExist(size_t ts, string varname, int ref, int lod);

	void PurgeVariable(string varname);
	void Clear() {_cache.clear(); }



 private:

	class var_info {
	public:
		map <size_t, bool> exist;
		map <size_t, vector <size_t> > region;
		map <size_t, vector <float> > range;
	};

	var_info *get_var_info(size_t ts, string varname) const;

	map <size_t, map <string, var_info> > _cache;
 };

 VarInfoCache _VarInfoCache;
 std::map <size_t, vector <double> > _extentsCache;

 float	*get_region_from_cache(
	size_t ts,
	string varname,
	int reflevel,
	int lod,
	const size_t min[3],
	const size_t max[3],
	bool lock
 );

 float	*get_region_from_fs(
	size_t ts,
	string varname,
	int reflevel,
	int lod,
	const size_t min[3],
	const size_t max[3],
	bool lock
 );

 float *get_region(
	size_t ts, string varname, int reflevel, int lod, 
	const size_t min[3], const size_t max[3], bool lock, bool *ondisk
 );

 void unlock_blocks(const float *blks);

 float    *alloc_region(
	size_t ts,
	const char *varname,
	VarType_T vtype,
	int reflevel,
	int lod,
	const size_t min[3],
	const size_t max[3],
	bool lock,
	bool fill
 ); 

 void	free_region(
	size_t ts,
	string varname,
	int reflevel,
	int lod,
	const size_t min[3],
	const size_t max[3]
 );

 void	free_var(const string &, int do_native);

 int	free_lru();

 int	_DataMgr(size_t mem_size);

 RegularGrid *execute_pipeline(
	size_t ts, string varname, int reflevel, int lod,
	const size_t min[3], const size_t max[3], bool lock,
	float *xcblks, float *ycblks, float *zcblks
 ); 

 // Check for circular dependencies in a pipeline 
 //
 bool cycle_check(
	const map <string, vector <string> > &graph,
	const string &node,
	const vector <string> &depends
 ) const;

 // Return true if the inputs of a require the outputs of b
 //
 bool depends_on(
	const PipeLine *a, const PipeLine *b
 ) const;

 vector <string> get_native_variables() const;
 vector <string> get_derived_variables() const;

 PipeLine *get_pipeline_for_var(string varname) const;

 void coord_array(
	const vector <double> &xin, vector <double> &xout, size_t ldelta
 ) const;

 void map_vox_to_user_regular(
	size_t timestep, const size_t vcoord0[3], double vcoord1[3], int reflevel
 ) const;

 void map_vox_to_blk(
	const size_t vcoord[3], size_t bcoord[3], int reflevel = -1
 ) const;

 void get_dim_blk( size_t bdim[3], int reflevel) const;

 RegularGrid *make_grid(
	size_t ts, string varname, int reflevel, int lod,
	const size_t bmin[3], const size_t bmax[3],
	float *blocks, float *xcblocks, float *ycblocks, float *zcblocks
 );





};

//! \class PipeLine
//!
//! The PipeLine abstract class declares pure virtual methods
//! that may be defined to allow the construction of a data 
//! transformation pipeline. 
//!
class VDF_API PipeLine {
    public:

	//! PipeLine stage constructor
	//!
	//! \param[in] name an identifier
	//! \param[in] inputs A list of variable names required as inputs
	//! \param[out] outputs A list of variable names and variable
	//! type pairs that will be output by the Calculate() method.
	//!
	PipeLine(
		string name,
		vector <string> inputs, 
		vector <pair <string, DataMgr::VarType_T> > outputs
	) {
		_name = name;
		_inputs = inputs;
		_outputs = outputs;
	}
	virtual ~PipeLine(){
	}

	//! Execute the pipeline stage
	//!
	//! This pure virtual method is called from the DataMgr whenever 
	//! a variable 
	//! is requested whose name matches one of the output variable
	//! names. All output variables computed - including the requested
	//! one - will be stored in the cache for subsequent retrieval
	//
	virtual int Calculate (
		vector <const RegularGrid *> input_grids,
		vector <RegularGrid *> output_grids,	// space for the output variables
		size_t ts, // current time step
		int reflevel, // refinement level
		int lod //
	) = 0;

	//! Returns the PipeLine stages name
	//
	const string &GetName() const {return (_name); };

	//! Returns the PipeLine inputs
	//
	const vector <string> &GetInputs() const {return (_inputs); };

	//! Returns the PipeLine outputs
	//
	const vector <pair <string, DataMgr::VarType_T> > &GetOutputs() const { 
		return (_outputs); 
	};


private:
	string _name;
	vector <string> _inputs;
	vector<pair<string, DataMgr::VarType_T> > _outputs;
};


};

#endif	//	_DataMgr_h_
