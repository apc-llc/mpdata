//
//      $Id: LayeredIO.h,v 1.9 2009/03/16 20:56:28 clynejp Exp $
//


#ifndef	_LayeredIO_h_
#define	_LayeredIO_h_

#include "vapor/MyBase.h"
#include "WaveletBlock3DRegionReader.h"

namespace VAPoR {

//
//! \class LayeredIO
//! \brief Performs data IO of Layered data to vdf
//! \author Alan Norton
//! \version $Revision: 1.9 $
//! \date    $Date: 2009/03/16 20:56:28 $
//!
//! This class provides an API for performing low-level IO
//! to/from VDF files.  Data is retrieved in a two-step process.  First,
//! the data is reconstructed from wavelet coefficients to a 3D array, 
//!representing
//! the data values along horizontal layers of the data.  Then the data is
//! interpolated from the layered representation to a uniform gridded representation.
//! The height of the uniform grid can vary dynamically.
//
class VDF_API	LayeredIO : public WaveletBlock3DRegionReader {

public:

 //! data set upon which all future operations will apply. 
 //! \param[in] nthreads The number of parallel execution threads to
 //! create.
 //! \note The success or failure of this constructor can be checked
 //! with the GetErrCode() method.
 //!
 //! \sa Metadata, GetErrCode()
 //
 LayeredIO(
	const Metadata *metadata,
	unsigned int	nthreads = 1
 );

 //! Constructor for the LayeredIO class. 
 //! \param[in] metafile Path to a metadata file for which all
 //! future class operations will apply
 //! \param[in] nthreads The number of parallel execution threads to
 //! create.
 //! \note The success or failure of this constructor can be checked
 //! with the GetErrCode() method.
 //!
 //! \sa Metadata, GetErrCode()
 //
 LayeredIO(
	const char	*metafile,
	unsigned int	nthreads = 1
 );

 virtual ~LayeredIO();


 //! Open the named variable for reading
 //!
 //! This method prepares the multiresolution data volume, indicated by a
 //! variable name and time step pair, for subsequent read operations by
 //! methods of this class.  Furthermore, the number of the refinement level
 //! parameter, \p reflevel indicates the resolution of the volume in
 //! the multiresolution hierarchy. The valid range of values for
 //! \p reflevel is [0..max_refinement], where \p max_refinement is the
 //! maximum finement level of the data set: Metadata::GetNumTransforms() - 1.
 //! volume when the volume was created. A value of zero indicates the
 //! coarsest resolution data, a value of \p max_refinement indicates the
 //! finest resolution data.
 //!
 //! An error occurs, indicated by a negative return value, if the
 //! volume identified by the {varname, timestep, reflevel} tripple
 //! is not present on disk. Note the presence of a volume can be tested
 //! for with the VariableExists() method.
 //! \param[in] timestep Time step of the variable to read
 //! \param[in] varname Name of the variable to read
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined for the VDC
 //! \retval status Returns a non-negative value on success
 //! \sa Metadata::GetVariableNames(), Metadata::GetNumTransforms()
 //!
 virtual int	OpenVariableRead(
	size_t timestep,
	const char *varname,
	int reflevel = 0
 );



 //! Close the data volume opened by the most recent call to 
 //! OpenVariableRead()
 //! \retval status Returns a non-negative value on success
 //! \sa OpenVariableRead()
 //!
 virtual int	CloseVariable();

 //! Read in and return a subregion from the currently opened multiresolution
 //! data volume.  
 //! The \p min and \p max vectors identify the minium and
 //! maximum extents, in voxel coordinates, of the subregion of interest. The
 //! minimum valid value of 'min' is (0,0,0), the maximum valid value of
 //! \p max is (nx-1,ny-1,nz-1), where nx, ny, and nz are the voxel dimensions
 //! of the volume at the resolution indicated by \p num_xforms. I.e. 
 //! the coordinates are specified relative to the desired volume 
 //! resolution. The volume
 //! returned is stored in the memory region pointed to by \p region. It 
 //! is the caller's responsbility to ensure adequate space is available.
 //!
 //! ReadRegion will fail if the requested data are not present. The
 //! VariableExists() method may be used to determine if the data
 //! identified by a (resolution,timestep,variable) tupple are 
 //! available on disk.
 //! \param[in] min Minimum region extents in voxel coordinates
 //! \param[in] max Maximum region extents in voxel coordinates
 //! \param[out] region The requested volume subregion
 //! \retval status Returns a non-negative value on success
 //! \sa OpenVariableRead(), Metadata::GetDimension()
 //
 virtual int	ReadRegion(
	const size_t min[3], const size_t max[3], 
	float *region
 );


 //! Read in and return a subregion from the currently opened multiresolution
 //! data volume.  
 //!
 //! This method is identical to the ReadRegion() method with the exception
 //! that the region boundaries are defined in block, not voxel, coordinates.
 //! Secondly, unless the 'unblock' parameter  is set, the internal
 //! blocking of the data will be preserved. 
 //!
 //! BlockReadRegion will fail if the requested data are not present. The
 //! VariableExists() method may be used to determine if the data
 //! identified by a (resolution,timestep,variable) tupple are 
 //! available on disk.
 //! \param[in] bmin Minimum region extents in block coordinates
 //! \param[in] bmax Maximum region extents in block coordinates
 //! \param[out] region The requested volume subregion
 //! \param[in] unblock If true, unblock the data before copying to \p region
 //! \retval status Returns a non-negative value on success
 //! \sa OpenVariableRead(), Metadata::GetBlockSize(), MapVoxToBlk()
 //
 virtual int	BlockReadRegion(
	const size_t bmin[3], const size_t bmax[3], 
	float *region, int unblock = 1
 );

 //! Establish the constant data values that will be returned when a volume
 //! lies outside (below) the valid volume for which data values are specified.
 //! This is needed when using layered data.
 //! This method modifies all the below values
 //! associated with specified vector of variable names
 //! \p varNames, to the corresponding values specified by
 //! \p values.
 //! If a specified variable name is not in the metadata,
 //! that name will be ignored.
 //! The vectors of low values values must
 //! be the same length as the vector of variable names.
 //! Any pre-existing low values are removed.
 //!
 //! If constant data value is not set for a variable, the extrapolation
 //! method that will be used is replication of the boundary value.
 //!
 //! \param[in] varNames A vector of variable names (strings)
 //! \param[in] values A vector of low values for associated variables (floats)
 //!
 //
 void SetLowVals(
     const vector<string>& varNames,
     const vector<float>& values
 );

 //! Establish the constant data values that will be returned when a volume
 //! lies outside (above) the valid volume for which data values are specified.
 //! This is needed when using layered data.
 //! This method modifies all the above values
 //! associated with specified vector of variable names
 //! \p varNames, to the corresponding values specified by
 //! \p values.
 //! If a specified variable name is not in the metadata,
 //! that name will be ignored.
 //! The vectors of high values values must
 //! be the same length as the vector of variable names.
 //! Any pre-existing high values are removed.
 //!
 //! If constant data value is not set for a variable, the extrapolation
 //! method that will be used is replication of the boundary value.
 //!
 //! \param[in] varNames A vector of variable names (strings)
 //! \param[in] values A vector of high values for associated variables (floats)
 //!
 //
 void SetHighVals(
     const vector<string>& varNames,
     const vector<float>& values
 );
 //! Method to retrieve current low value for all variables
 //!
 //! \param[out] varNames list of variables with low values
 //! \param[out] values list of low values 
 //!
 void GetLowVals(vector<string>&varNames, vector<float>&vals);

 //! Method to retrieve current high value for all variables
 //!
 //! \param[out] varNames list of variables with low values
 //! \param[out] values list of low values 
 //!
 void GetHighVals(vector<string>&varNames, vector<float>&vals);

 //! Set height of interpolate grid
 //!
 //! This method establishes the height (Z dimension) of the 
 //! interpolation grid.  The grid height is specified for the finest
 //! resolution grid in the multiresolution grid hierarchy.
 //!
 //! \param[in] height Z dimension of interpolation grid
 //
 int SetGridHeight(size_t height);

 //! Get height of interpolate grid
 //!
 //! This method returns the currently set grid height.
 //! interpolation grid.  The grid height is specified for the finest
 //! resolution grid in the multiresolution grid hierarchy.
 //!
 //! \retval height A size_t value specifying Z dimension of interpolation grid
 //!
 size_t GetGridHeight() const {return _gridHeight;} 

 //! Enable or disable interpolation
 //!
 //! When enabled, data read with this class are interpolated to a
 //! uniform grid whose height is determined by SetGridHeight(). Furthermore,
 //! all coordinate parameters for this class are specified/returned in the
 //! coordinate system of the uniform intepolation grid. When disabled,
 //! no intepolation is performed and the Z grid dimension specified by
 //! SetGridHeight() is ignored. By default interpolation is enabled.
 // 
 void SetInterpolateOnOff(bool on);

 //! Get the dimension of a volume
 //!
 //! Get the resulting dimension of the volume
 //! after undergoing a specified number of forward transforms.
 //! If the number of transforms is zero, and the
 //! grid type is not layered, the
 //! value returned is the native volume dimension as specified in the
 //! Metadata structure used to construct the class.
 //! With layered data, the third coordinate depends
 //! on the user-specified interpolation.
 //! \param[in] reflevel Refinement level of the variable
 //! \param[out] dim Transformed dimension.
 //!
 //! \sa Metadata::GetDimension(), LayeredIO::SetInterpolateOnOff()
 //
 virtual void   GetDim(size_t dim[3], int reflevel = 0) const;

 //! Get dimension of a volume in blocks
 //!
 //! Performs same operation as GetDim() except returns
 //! dimensions in block coordinates instead of voxels.
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined for the VDC
 //! \param[out] bdim Transformed dimension in blocks.
 //!
 //! \sa Metadata::GetDimension(), LayeredIO::SetInterpolateOnOff()
 //
 virtual void   GetDimBlk(size_t bdim[3], int reflevel = 0) const;

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
 //! indicates the maximum refinment level defined for the VDC
 //!
 //! \sa LayeredIO::SetInterpolateOnOff()
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
 //! indicates the maximum refinment level defined for the VDC 
 //! \retval boolean True if region is valid
 //!
 //! \sa LayeredIO::SetInterpolateOnOff()
 //
 virtual int IsValidRegionBlk(
    const size_t min[3], const size_t max[3], int reflevel = 0
 ) const;

 //! Return the valid bounds of the currently opened region
 //! 
 //! The VDC permits the storage of volume subregions. This method may
 //! be used to query the valid domain of the currently opened volume. Results
 //! are returned in voxel coordinates, relative to the refinement level
 //! indicated by \p reflevel.
 //! 
 //! When layered data is used, valid subregions can be restricted
 //! horizontally, but must include the full vertical extent of
 //! the data.  In that case the result of GetValidRegion depends
 //! on the region height in voxels, and can vary from region to region.
 //!
 //! \param[out] min A pointer to the minimum bounds of the subvolume
 //! \param[out] max A pointer to the maximum bounds of the subvolume
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined for the VDC
 //! \retval status Returns a negative value if the volume is not opened
 //! for reading or writing.
 //! 
 //! \sa OpenVariableWrite(), OpenVariableRead(), 
 //! LayeredIO::SetInterpolateOnOff()
 //
 virtual void GetValidRegion(
    size_t min[3], size_t max[3], int reflevel
 ) const;
 


 //! Map integer voxel coordinates to user-defined floating point coords.
 //!
 //! Map the integer coordinates of the specified voxel to floating
 //! point coordinates in a user defined space. The voxel coordinates,
 //! \p vcoord0 are specified relative to the refinement level
 //! indicated by \p reflevel for time step \p timestep.  
 //! The mapping is performed by using linear interpolation 
 //! The user-defined coordinate system is obtained
 //! from the Metadata structure passed to the class constructor.
 //! The user coordinates are returned in \p vcoord1.
 //! Results are undefined if vcoord is outside of the volume 
 //! boundary.
 //!
 //! If a user coordinate system is not defined for the specified
 //! time step, \p timestep, the global extents for the VDC will
 //! be used.
 //!
 //! \param[in] timestep Time step of the variable 
 //! \param[in] vcoord0 Coordinate of input voxel in integer (voxel)
 //! coordinates
 //! \param[out] vcoord1 Coordinate of transformed voxel in user-defined,
 //! floating point  coordinates
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined for the VDC
 //!
 //! \sa Metatdata::GetGridType(), Metadata::GetExtents(), 
 //! GetTSXCoords(), LayeredIO::SetInterpolateOnOff()  
 //
 virtual void	MapVoxToUser(
	size_t timestep,
	const size_t vcoord0[3], double vcoord1[3], int ref_level = 0
 ) const;

 //! Map floating point coordinates to integer voxel offsets.
 //!
 //! Map floating point coordinates, specified relative to a 
 //! user-defined coordinate system, to the closest integer voxel 
 //! coordinates for a voxel at a given refinement level. 
 //! The integer voxel coordinates, \p vcoord1, 
 //! returned are specified relative to the refinement level
 //! indicated by \p reflevel for time step, \p timestep.
 //! The mapping is performed by using linear interpolation 
 //! The user defined coordinate system is obtained
 //! from the Metadata structure passed to the class constructor.
 //! Results are undefined if \p vcoord0 is outside of the volume 
 //! boundary.
 //!
 //! If a user coordinate system is not defined for the specified
 //! time step, \p timestep, the global extents for the VDC will
 //! be used.
 //!
 //! \param[in] timestep Time step of the variable 
 //! \param[in] vcoord0 Coordinate of input point in floating point
 //! coordinates
 //! \param[out] vcoord1 Integer coordinates of closest voxel, at the 
 //! indicated refinement level, to the specified point.
 //! integer coordinates
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined for the VDC
 //!
 //! \sa Metatdata::GetGridType(), Metadata::GetExtents(), 
 //! GetTSXCoords(), LayeredIO::SetInterpolateOnOff()  
 //
 virtual void	MapUserToVox(
	size_t timestep,
	const double vcoord0[3], size_t vcoord1[3], int reflevel = 0
 ) const;



private:
 int	_objInitialized;	// has the obj successfully been initialized?
 bool _interpolateOn;	// Is interpolation on?
 size_t _gridHeight;	// Interpolation grid height
 WaveletBlock3DRegionReader *_varReader;
 WaveletBlock3DRegionReader *_elevReader;

 float *_elevBlkBuf;	// buffer for layered elevation data
 float *_varBlkBuf;	// buffer for layered variable data
 size_t _blkBufSize;	// Size of space allocated to daa buffers


 map <string, float> _lowValMap;
 map <string, float> _highValMap;


 size_t _cacheTimeStep;	// Cached elevation data 
 size_t _cacheBMin[3];
 size_t _cacheBMax[3];
 bool _cacheEmpty;
 bool cache_check(
	size_t timestep,
	const size_t bmin[3],
	const size_t bmax[3]
 );

 void cache_set(
	size_t timestep,
	const size_t bmin[3],
	const size_t bmax[3]
 );

 void cache_clear();

 void _setDefaultHighLowVals();


 int	_LayeredIO();

void _interpolateRegion(
    float *region,			// Destination interpolated ROI
	const float *elevBlks,	// Source elevation ROI
	const float *varBlks,	// Source variable ROI
	const size_t blkMin[3],
	const size_t blkMax[3],	// coords of native (layered grid)
							// ROI specified in blocks 
    size_t zmini,
	size_t zmaxi,			// Z coords extents of interpolated ROI
	float *lowVal,
	float *highVal
) const;


};

};

#endif	//	_LayeredIO_h_
