//
//      $Id: VDFIOBase.h,v 1.8 2009/03/16 20:56:28 clynejp Exp $
//


#ifndef	_VDFIOBase_h_
#define	_VDFIOBase_h_

#include <cstdio>
#include <vapor/MyBase.h>
#include <vapor/Metadata.h>

namespace VAPoR {


#if	defined(IRIX) || defined(LINUX)
#define	WAVELET_BLOCK_TIMER
#endif

#ifdef	WAVELET_BLOCK_TIMER
#include <time.h>
#include <assert.h>


double  inline gettime() {
	struct timespec ts;
	double  t;

	ts.tv_sec = ts.tv_nsec = 0;

	clock_gettime(CLOCK_REALTIME, &ts);


	t = (double) ts.tv_sec + (double) ts.tv_nsec*1.0e-9;

	return(t);
}

#define	TIMER_START(T0)		double (T0) = VAPoR::gettime();
#define	TIMER_STOP(T0, T1)	(T1) += VAPoR::gettime() - (T0);

#else

#define	TIMER_START(T0)
#define	TIMER_STOP(T0, T1)

#endif


//
//! \class VDFIOBase
//! \brief Abstract base class for performing data IO to a VDC
//! \author John Clyne
//! \version $Revision: 1.8 $
//! \date    $Date: 2009/03/16 20:56:28 $
//!
//! This class provides an API for performing low-level IO 
//! to/from a Vapor Data Collection (VDC)
//
class VDF_API	VDFIOBase : public VetsUtil::MyBase {

public:

 enum VarType_T {
	VARUNKNOWN = -1,
	VAR3D, VAR2D_XY, VAR2D_XZ, VAR2D_YZ
 };

 //! Constructor for the VDFIOBase class.
 //! \param[in] metadata Pointer to a metadata class object for which all
 //! future class operations will apply
 //! \param[in] nthreads Number of execution threads that may be used by
 //! the class for parallel execution.
 //! \note The success or failure of this constructor can be checked
 //! with the GetErrCode() method.
 //!
 //! \sa Metadata, GetErrCode(),
 //
 VDFIOBase(
	const Metadata *metadata,
	unsigned int	nthreads = 1
 );

 //! Constructor for the VDFIOBase class.
 //! \param[in] metafile Path to a metadata file for which all
 //! future class operations will apply
 //! \param[in] nthreads Number of execution threads that may be used by
 //! the class for parallel execution.
 //! \note The success or failure of this constructor can be checked
 //! with the GetErrCode() method.
 //!
 //! \sa Metadata, GetErrCode(),
 //
 VDFIOBase(
	const char *metafile,
	unsigned int	nthreads = 1
 );

 virtual ~VDFIOBase();

 //! Returns true if indicated data volume exists on disk
 //!
 //! Returns true if the variable identified by the timestep, variable
 //! name, and refinement level is present on disk. Returns 0 if
 //! the variable is not present.
 //! \param[in] ts A valid time step from the Metadata object used
 //! to initialize the class
 //! \param[in] varname A valid variable name
 //! \param[in] reflevel Refinement level requested. The coarsest 
 //! refinement level is 0 (zero). A value of -1 indicates the finest
 //! refinement level contained in the VDC.
 //
 virtual int VariableExists(
	size_t ts,
	const char *varname,
	int reflevel = 0
 ) const = 0;

 //! Open the named variable for writing
 //!
 //! Prepare a vapor data file for the creation of a multiresolution
 //! data volume via subsequent write operations by
 //! other methods of this classes derived from this class.
 //! The data volume is identified by the specfied time step and
 //! variable name. The number of forward transforms applied to
 //! the volume is determined by the Metadata object used to
 //! initialize the class. The number of refinement levels actually 
 //! saved to the data collection are determined by \p reflevels. If
 //! \p reflevels is zero, the default, only the coarsest approximation is
 //! saved. If \p reflevels is one, all the coarsest and first refinement 
 //! level is saved, and so on. A value of -1 indicates the maximum
 //! refinment level permitted by the VDF
 //!
 //! \param[in] timestep Time step of the variable to read
 //! \param[in] varname Name of the variable to read
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level.
 //! \retval status Returns a non-negative value on success
 //! \sa Metadata::GetVariableNames(), Metadata::GetNumTransforms()
 //!
 virtual int	OpenVariableWrite(
	size_t timestep,
	const char *varname,
	int reflevel = -1
 ) = 0;

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
 //! \sa Metadata::GetVariableNames(), Metadata::GetNumTransforms()
 //!
 virtual int	OpenVariableRead(
	size_t timestep,
	const char *varname,
	int reflevel = 0
 ) = 0;

 //! Close the currently opened variable.
 //!
 //! \sa OpenVariableWrite(), OpenVariableRead()
 //
 virtual int	CloseVariable() = 0;

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
 //! \sa Metadata::GetDimension()
 //
 virtual void	GetDim(size_t dim[3], int reflevel = 0) const;

 //! Get dimension of a volume in blocks
 //!
 //! Performs same operation as GetDim() except returns
 //! dimensions in block coordinates instead of voxels.
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined for the VDC
 //! \param[out] bdim Transformed dimension in blocks.
 //!
 //! \sa Metadata::GetDimension()
 //
 virtual void	GetDimBlk(size_t bdim[3], int reflevel = 0) const;

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
 //! \sa OpenVariableWrite(), OpenVariableRead()
 //
 virtual void GetValidRegion(
    size_t min[3], size_t max[3], int reflevel
 ) const;


 //! Map integer voxel coordinates into integer block coordinates. 
 //! 
 //! Compute the integer coordinate of the block containing
 //! a specified voxel. 
 //! \param[in] vcoord Coordinate of input voxel in integer (voxel)
 //! coordinates
 //! \param[out] bcoord Coordinate of block in integer coordinates containing
 //! the voxel. 
 //!
 virtual void	MapVoxToBlk(const size_t vcoord[3], size_t bcoord[3]) const;
		

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
 //! \param[in] timestep Time step of the variable 
 //! \param[in] vcoord0 Coordinate of input voxel in integer (voxel)
 //! coordinates
 //! \param[out] vcoord1 Coordinate of transformed voxel in user-defined,
 //! floating point  coordinates
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined for the VDC
 //!
 //! \sa Metatdata::GetGridType(), Metadata::GetExtents(), 
 //! GetTSXCoords()
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
 //! GetTSXCoords()
 //
 virtual void	MapUserToVox(
	size_t timestep,
	const double vcoord0[3], size_t vcoord1[3], int reflevel = 0
 ) const;

 //! Map floating point coordinates to integer block offsets.
 //!
 //! Map floating point coordinates, specified relative to a 
 //! user-defined coordinate system, to integer coordinates of the block
 //! containing the point at a given refinement level. 
 //! The integer voxel coordinates, \p vcoord1
 //! are specified relative to the refinement level
 //! indicated by \p reflevel for time step, \p timestep.
 //! The mapping is performed by using linear interpolation 
 //! The user defined coordinate system is obtained
 //! from the Metadata structure passed to the class constructor.
 //! The user coordinates are returned in \p vcoord1.
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
 //! \param[out] vcoord1 Integer coordinates of block containing the point
 //! \param[in] reflevel Refinement level of the variable. A value of -1
 //! indicates the maximum refinment level defined for the VDC
 //!
 //! \sa Metatdata::GetGridType(), Metadata::GetExtents(), 
 //! GetTSXCoords()
 //
 virtual void	MapUserToBlk(
	size_t timestep,
	const double vcoord0[3], size_t bcoord0[3], int reflevel = 0
 ) const {
	size_t v[3];
	MapUserToVox(timestep, vcoord0, v, reflevel);
	VDFIOBase::MapVoxToBlk(v, bcoord0);
 }
	


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
 //
 virtual int IsValidRegionBlk(
	const size_t min[3], const size_t max[3], int reflevel = 0
 ) const;

 //! Return the metadata class object associated with this class
 //!
 const	Metadata *GetMetadata() const { return (_metadata); };

 //! Return the read timer
 //!
 //! This method returns the accumulated clock time, in seconds, 
 //! spent reading data from files. There is presently no
 //! way to reset the counter (without creating a new class object)
 //!
 double	GetReadTimer() const { return(_read_timer); };

 //! Return the seek timer
 //!
 //! This method returns the accumulated clock time, in seconds, 
 //! spent performing file seeks (in general this is zero) There
 //! is presently no
 //! way to reset the counter (without creating a new class object)
 //!
 double	GetSeekTimer() const { return(_seek_timer); };

 //! Return the write timer
 //!
 //! This method returns the accumulated clock time, in seconds, 
 //! spent writing data to files. There is presently no
 //! way to reset the counter (without creating a new class object)
 //!
 double	GetWriteTimer() const { return(_write_timer); };

 //! Return the data range for the currently open variable
 //!
 //! The method returns the minimum and maximum data values, respectively, 
 //! for the variable currently opened. If the variable is not opened,
 //! or if it is opened for writing, the results are undefined.
 //!
 //! \param[out] range Min and Max data values, respectively
 //! \retval status A negative integer is returned on failure, otherwise
 //! the method has succeeded.
 const float *GetDataRange() const {return (_dataRange);};

 void SetDataRange(const float* rng){_dataRange[0] = rng[0]; _dataRange[1] = rng[1];}

 static VarType_T	GetVarType(const Metadata *metadata, const string &varname);

protected:
 const VAPoR::Metadata *_metadata;
 int	_nthreads;	// # execution threads

 size_t	_dim[3];		// volume dimensions in voxels (finest resolution)
 size_t	_bdim[3];		// volume dimensions in blocks (finest resolution)

 size_t	_bs[3];		// block dimensions in voxels

 int	_block_size;	// block size in voxels
 int	_num_reflevels;	// number of refinement levels present. The 
						// minimum value this parameter is 1, indicating
						// only refinement level 0 is present.
 int	_version;	// VDF file version number

 float _dataRange[2];	// min and max range of data;
 size_t _validRegMin[3];
 size_t _validRegMax[3];	// Bounds (in voxels) of valid region relative
							// to volume at finest level.  With layered IO,
							// these are the bounds of the layered data.

 double	_read_timer;
 double	_write_timer;
 double	_seek_timer;

private:
 int	_objInitialized;	// has the obj successfully been initialized?
 int	_doFreeMeta;	// does the class own the metadata object?

 static const int VERSION  = 1;


 int	_VDFIOBase(
	unsigned int	nthreads
 );

};


 int	MkDirHier(const string &dir);
 void	DirName(const string &path, string &dir);

}

#endif	//	
