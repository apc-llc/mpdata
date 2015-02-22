//
//      $Id$
//


#ifndef	_Metadata_h_
#define	_Metadata_h_

#include <vector>
#include "vapor/common.h"

#ifdef WIN32
#pragma warning(disable : 4251)
#endif

namespace VAPoR {

//
//! \class Metadata
//! \brief An abstract class for managing metadata for a collection 
//! of gridded data. The data collection may support two forms 
//! of data reduction: multi-resolution (a hierarchy of grids, each dimension
//! a factor of two coarser than the preceeding), and level-of-detail (a
//! sequence of one more compressions). 
//!
//! Implementers must derive a concrete class from Metadata to support
//! a particular data collection type
//!
//! \author John Clyne
//! \version $Revision$
//! \date    $Date$
//!
//!
class VDF_API Metadata {
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

 Metadata() {_deprecated_get_dim = false;}
 virtual ~Metadata() {};

 //! Get the native dimension of a volume
 //!
 //! Returns the X,Y,Z coordinate dimensions of all data variables
 //! in grid (voxel) coordinates full resolution.
 //!
 //! \param[in] reflevel Refinement level of the variable
 //! \param[out] dim A three element vector (ordered X, Y, Z) containing the 
 //! voxel dimensions of the data at full resolution.
 //!
 //
 virtual void   GetGridDim(size_t dim[3]) const = 0;

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
 virtual void GetBlockSize(size_t bs[3], int reflevel) const = 0;


 //! Return number of transformations in hierarchy 
 //!
 //! For multi-resolution data this method returns the number of
 //! coarsened approximations present. If no approximations 
 //! are available - if only the native data are present - the return
 //! value is 0.
 //!
 //! \retval n  The number of coarsened data approximations available
 //
 virtual int GetNumTransforms() const {return(0); };

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
 virtual std::vector <size_t> GetCRatios() const {
	std::vector <size_t> cr; cr.push_back(1); return(cr);
 }

 //! Return the coordinate system type. One of \b cartesian or \b spherical 
 //! \retval type 
 //! 
 //
 virtual std::string GetCoordSystemType() const { return("cartesian"); };

 //! Return the grid type. One of \b regular, \b stretched, \b block_amr,
 //! or \b spherical
 //!
 //! \retval type 
 //! 
 //
 virtual std::string GetGridType() const { return("regular"); };

 //! Return the X dimension coordinate array, if it exists
 //!
 //! \retval value An array of monotonically changing values specifying
 //! the X coordinates, in a user-defined coordinate system, of each
 //! YZ sample plane. An empty vector is returned if the coordinate
 //! dimension array is not defined for the specified time step.
 //! \sa GetGridType() 
 //!
 //
 virtual std::vector<double> GetTSXCoords(size_t ts) const {
	std::vector <double> empty; return(empty);
 };
 virtual std::vector<double> GetTSYCoords(size_t ts) const {
	std::vector <double> empty; return(empty);
 };
 virtual std::vector<double> GetTSZCoords(size_t ts) const {
	std::vector <double> empty; return(empty);
 };



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
 virtual std::vector<double> GetExtents(size_t ts = 0) const = 0;


 //! Return the number of time steps in the data collection
 //!
 //! \retval value The number of time steps 
 //!
 //
 virtual long GetNumTimeSteps() const = 0;


 //! Return the names of the variables in the collection 
 //!
 //! This method returns a vector of all variables of all types
 //! in the data collection
 //!
 //! \retval value is a space-separated list of variable names
 //!
 //
 virtual std::vector <std::string> GetVariableNames() const;

 //! Return the Proj4 map projection string.
 //!
 //! \retval value An empty string if a Proj4 map projection is
 //! not available, otherwise a properly formatted Proj4 projection
 //! string is returned.
 //!
 //
 virtual std::string GetMapProjection() const {std::string empty; return (empty); };

 //! Return the names of the 3D variables in the collection 
 //!
 //! \retval value is a vector of 3D variable names.
 //! An emptry string is returned if no variables of this type are present
 //!
 //
 virtual std::vector <std::string> GetVariables3D() const = 0;

 //! Return the names of the 2D, XY variables in the collection 
 //!
 //! \retval value is a vector of 2D XY variable names
 //! An emptry string is returned if no variables of this type are present
 //!
 //
 virtual std::vector <std::string> GetVariables2DXY() const = 0;

 //! Return the names of the 2D, XZ variables in the collection 
 //!
 //! \retval value is a vectort of 2D ZY variable names
 //! An emptry string is returned if no variables of this type are present
 //!
 //
 virtual std::vector <std::string> GetVariables2DXZ() const = 0;

 //! Return the names of the 2D, YZ variables in the collection 
 //!
 //! \retval value is a vector of 2D YZ variable names
 //! An emptry string is returned if no variables of this type are present
 //!
 //
 virtual std::vector <std::string> GetVariables2DYZ() const = 0;

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
 virtual std::vector <std::string> GetCoordinateVariables() const {;
	std::vector <std::string> v;
	v.push_back("NONE"); v.push_back("NONE"); v.push_back("ELEVATION");
	return(v);
 }


 //! Return a three-element boolean array indicating if the X,Y,Z
 //! axes have periodic boundaries, respectively.
 //!
 //! \retval boolean-vector  
 //!
 //
 virtual std::vector<long> GetPeriodicBoundary() const = 0;

 //! Return a three-element integer array indicating the coordinate
 //! ordering permutation.
 //!
 //! \retval integer-vector  
 //!
 virtual std::vector<long> GetGridPermutation() const = 0;

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
 virtual double GetTSUserTime(size_t ts) const = 0;


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
 virtual void GetTSUserTimeStamp(size_t ts, std::string &s) const = 0;


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
 virtual void   GetDim(size_t dim[3], int reflevel = 0) const;

 //! Get dimension of a volume in blocks
 //!
 //! Performs same operation as GetDim() except returns
 //! dimensions in block coordinates instead of voxels.
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined. In fact, any value 
 //! outside the valid range is treated as the maximum refinement level
 //! \param[out] bdim Transformed dimension in blocks.
 //!
 //! \sa Metadata::GetNumTransforms()
 //
 virtual void   GetDimBlk(size_t bdim[3], int reflevel = 0) const; 

 //! Return the value of the missing data value
 //! 
 //! This method returns the value of the missing data value 
 //! the specified variable. If no missing data are present
 //! the method returns false
 //!
 //! \param[in] varname A 3D or 2D variable name
 //! \param[out] value The missing data value. Undefined if no missing
 //! value is present.
 //! 
 //! \retval returns true if missing data are present
 //
 virtual bool GetMissingValue(std::string varname, float &value) const {
	return(false); 
 };



 //! Map integer voxel coordinates into integer block coordinates. 
 //! 
 //! Compute the integer coordinate of the block containing
 //! a specified voxel. 
 //! \param[in] vcoord Coordinate of input voxel in integer (voxel)
 //! coordinates
 //! \param[out] bcoord Coordinate of block in integer coordinates containing
 //! the voxel. 
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined.
 //!
 virtual void	MapVoxToBlk(const size_t vcoord[3], size_t bcoord[3], int reflevel = -1) const;

 virtual void   MapVoxToUser(
	size_t timestep,
	const size_t vcoord0[3], double vcoord1[3], int ref_level = 0
 ) const;

 void MapUserToVox(
	size_t timestep, const double vcoord0[3], size_t vcoord1[3],
	int reflevel
 ) const;


 //! Return the variable type for the indicated variable
 //!
 //! This method returns the variable type for the variable 
 //! named by \p varname.
 //!
 //! \param[in] varname A 3D or 2D variable name
 //! \retval type The variable type. The constant VAR
 //
 virtual VarType_T GetVarType(const std::string &varname) const; 

 //! Return true if indicated region coordinates are valid
 //!
 //! Returns true if the region defined by \p reflevel,
 //! \p min, \p max is valid. I.e. returns true if the indicated
 //! volume subregion is contained within the volume. Coordinates are
 //! specified relative to the refinement level.
 //! 
 //! \param[in] min Minimum region extents in voxel coordinates
 //! \param[in] max Maximum region extents in voxel coordinates
 //! \retval boolean True if region is valid
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined. In fact,
 //! any invalid value is treated as the maximum refinement level. 
 //
 virtual int IsValidRegion(
	const size_t min[3], const size_t max[3], int reflevel = 0
 ) const;

 //! Return true if indicated region coordinates are valid
 //!
 //! Returns true if the region defined by \p reflevel, and the block
 //! coordinates
 //! \p min, \p max are valid. I.e. returns true if the indicated
 //! volume subregion is contained within the volume.
 //! Coordinates are
 //! specified relative to the refinement level.
 //!
 //! \param[in] min Minimum region extents in block coordinates
 //! \param[in] max Maximum region extents in block coordinates
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined. In fact,
 //! any invalid value is treated as the maximum refinement level. 
 //! \retval boolean True if region is valid
 //
 virtual int IsValidRegionBlk(
	const size_t min[3], const size_t max[3], int reflevel = 0
 ) const;

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
 virtual bool IsCoordinateVariable(std::string varname) const;


protected:
 bool _deprecated_get_dim;
};
};

#endif	//	_Metadata_h_
