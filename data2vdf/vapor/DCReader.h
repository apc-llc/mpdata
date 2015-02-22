//
//      $Id$
//


#ifndef	_DCReader_h_
#define	_DCReader_h_

#include <vector>
#include <algorithm>
#include <vapor/Metadata.h>
#include <vapor/MyBase.h>
#include <vapor/common.h>

namespace VAPoR {

//
//! \class DCReader
//! \brief ???
//!
//! Implementers must derive a concrete class from DCReader to support
//! a particular data collection type
//!
//! \author John Clyne
//! \version $Revision$
//! \date    $Date$
//!
//!
class VDF_API DCReader : public Metadata, public VetsUtil::MyBase {
public:

 virtual ~DCReader() {};

 //! Open the named variable for reading
 //!
 //! This method prepares a data volume (slice), indicated by a
 //! variable name and time step pair, for subsequent read operations by
 //! methods of this class.  The number of the refinement levels
 //! parameter, \p reflevel, indicates the resolution of the volume in
 //! the multiresolution hierarchy. The valid range of values for
 //! \p reflevel is [0..max_refinement], where \p max_refinement is the
 //! maximum refinement level of the data set: Metadata::GetNumTransforms().
 //! A value of zero indicates the
 //! coarsest resolution data, a value of \p max_refinement (or -1) indicates
 //! the
 //! finest resolution data.
 //! The level-of-detail parameter, \p lod, selects
 //! the approximation level. Valid values for \p lod are integers in
 //! the range 0..GetCRatios().size()-1, or the value -1 may be used
 //! to select the best approximation available: GetCRatios().size()-1.
 //!
 //! An error occurs, indicated by a negative return value, if the
 //! volume identified by the {varname, timestep, reflevel, lod} tupple
 //! is not available. Note the availability of a volume can be tested
 //! with the VariableExists() method.
 //!
 //! \param[in] timestep Time step of the variable to read
 //! \param[in] varname Name of the variable to read
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined for the VDC
 //! \param[in] lod Approximation level of the variable. A value of -1
 //! indicates the maximum approximation level defined for the VDC
 //! \retval status Returns a non-negative value on success
 //!
 //! \sa Metadata::GetVariableNames(), Metadata::GetNumTransforms(),
 //! GetNumTimeSteps(), GetCRatios()
 //!
 virtual int OpenVariableRead(
    size_t timestep, string varname, int reflevel=0, int lod=0
 ) = 0;

 //! Close the currently opened variable.
 //!
 //! \sa OpenVariableRead()
 //
 virtual int CloseVariable() = 0;

  //! Read the next volume slice from the currently opened file
 //!
 //! Read in and return a slice (2D array) of
 //! voxels from the currently opened data volume at the current
 //! refinement level.
 //! Subsequent calls will read successive slices
 //! until the entire volume has been read.
 //! It is the caller's responsibility to ensure that the array pointed
 //! to by \p slice contains enough space to accomodate
 //! an NX by NY dimensioned slice, where NX is the dimesion of the
 //! volume along the X axis, specified
 //! in **voxels**, and NY is the Y axis dimension, as returned by
 //! GetDim().
 //!
 //! \note ReadSlice returns 0 if the entire volume has been read.
 //!
 //! \param[out] slice The requested volume slice
 //!
 //! \retval status Returns a non-negative value on success
 //! \sa OpenVariableRead(), Metadata::GetDim()
 //!
 //
 virtual int ReadSlice(float *slice) = 0;

 //! Return true if the named variable is available
 //!
 //! This method returns true if the variable named
 //! by \p varname is available in the data collection
 //! at the time step given by \p ts, and the specified
 //! level-of-detail and resolution.. A variable may be 
 //! contained in a data set, but not exist for all time steps,
 //! levels-of-detail, etc..
 //!
 //! \param[in] timestep Time step of the variable to read
 //! \param[in] varname Name of the variable to read
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined for the VDC
 //! \param[in] lod Approximation level of the variable. A value of -1
 //! indicates the maximum approximation level defined for the VDC
 //! \retval status True if variable is present in the data collection
 //!
 //! \sa GetNumTimeSteps(), GetVariableNames()
 //!
 virtual bool VariableExists(
	size_t ts, string varname,int reflevel=0, int lod=0) const {
	std::vector <string> v = GetVariableNames();
	return((find(v.begin(), v.end(), varname)!=v.end())&&ts<GetNumTimeSteps());
 }

 //! Return the Longitude and Latitude extents of the grid
 //!
 //! This method returns the longitude extents (west-most and east-most)
 //! points, and the latitude extents (south-most and north-most) in degrees.
 //!
 //! If the time step \p ts is invalid, or if for any reason the extents
 //! can't be calculated, both lat and lon extents will be set to zero.
 //! 
 //! \param[in] timestep Time step of the grid
 //! \param[out] lon_exts A two-element array containing the west-most, and
 //! east-most points, in that order. Moreover, lon[0] <= lon[1]
 //! \param[out] lat_exts A two-element array containing the south-most, and
 //! north-most points, in that order. Moreover, lat[0] <= lat[1]
 //!
 virtual void GetLatLonExtents(
    size_t ts, double lon_exts[2], double lat_exts[2]
 ) const = 0;

 virtual std::vector <string> GetVariables2DExcluded() const = 0;
 virtual std::vector <string> GetVariables3DExcluded() const = 0;

};
};

#endif	//	_DCReader_h_
