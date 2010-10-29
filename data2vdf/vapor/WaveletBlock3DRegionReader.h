//
//      $Id: WaveletBlock3DRegionReader.h,v 1.9 2009/03/06 14:24:38 alannorton Exp $
//


#ifndef	_WavletBlock3DRegionReader_h_
#define	_WavletBlock3DRegionReader_h_

#include <vapor/MyBase.h>
#include "WaveletBlock3DIO.h"

namespace VAPoR {

//
//! \class WaveletBlock3DRegionReader
//! \brief A sub-region reader for VDF files
//! \author John Clyne
//! \version $Revision: 1.9 $
//! \date    $Date: 2009/03/06 14:24:38 $
//!
//! This class provides an API for extracting volume sub-regions  
//! from a VDF file
//
class VDF_API	WaveletBlock3DRegionReader : public WaveletBlock3DIO {

public:

 //! Constructor for the WaveletBlock3DRegionReader class. 
 //! \param[in] metadata A pointer to a Metadata structure identifying the
 //! data set upon which all future operations will apply. 
 //! \param[in] nthreads The number of parallel execution threads to
 //! create.
 //! \note The success or failure of this constructor can be checked
 //! with the GetErrCode() method.
 //!
 //! \sa Metadata, GetErrCode()
 //
 WaveletBlock3DRegionReader(
	const Metadata *metadata,
	unsigned int	nthreads = 1
 );

 //! Constructor for the WaveletBlock3DRegionReader class. 
 //! \param[in] metafile Path to a metadata file for which all
 //! future class operations will apply
 //! \param[in] nthreads The number of parallel execution threads to
 //! create.
 //! \note The success or failure of this constructor can be checked
 //! with the GetErrCode() method.
 //!
 //! \sa Metadata, GetErrCode()
 //
 WaveletBlock3DRegionReader(
	const char	*metafile,
	unsigned int	nthreads = 1
 );

 virtual ~WaveletBlock3DRegionReader();


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

 virtual int OpenVariableWrite(
	size_t /*timestep*/,
	const char * /*varname*/,
	int /* reflevel */ = 0
 ) {SetErrMsg("Operation not supported"); return(-1);};


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

#ifdef	DEAD

 // Read a subregion of gamma (detail) coefficients from disk. The 
 // spatial coordinates of the region (min, max) are given 
 // in **block** units, where 'min' specifies the minimum bounds
 // and 'max' specifies the maximum bound.  The coefficients are stored
 // in the three arrays pointed to by xregion, yregion, and zregion. Each
 // much have enough space to accomodate the X, Y, and Z gamma coefficients
 // respectively. The dimenions of the zregion in block units are nbx*nby*nbz,
 // where nbx == max[0]-min[0]+1, and so on. The dimensions of the 
 // yregion and xregion
 // are nbx*nby*(nbz*2), and nbx*(nby*2)*(nbz*2), respectively (there are 
 // twice as many Y coefficients as Z, and four times as many X coefficients). 
 //
 // The coefficients returned are those needed to forward transform 
 // a volume from level 'level-1', to 'level'. Thus, 'level' must be at least
 // one.
 //
 // The coefficients will be unblocked if the 'unblock' flag is set. Otherwise,
 // they are left in their native blocked-storage order
 //
 int	ReadRegionGamma(
	size_t min[3], size_t max[3], float *xregion, 
	float *yregion, float *zregion, int unblock = 1
 );

#endif

private:
 int	_objInitialized;	// has the obj successfully been initialized?

 float	*lambda_blks_c[MAX_LEVELS];
 float	*gamma_blks_c[MAX_LEVELS];

 int	row_inv_xform(
	const float *lambda_row, 
	unsigned int ljx0, unsigned int ljy0, unsigned int ljz0, 
	unsigned int ljnx, unsigned int j, float *region, 
	const size_t min[3], const size_t max[3], unsigned int level, int unblock
	);
 int	my_realloc(); 
 void	my_free(); 

 int	_ReadRegion(
	const size_t min[3], const size_t max[3], 
	float *region, int unblock = 1
 );

 void	_WaveletBlock3DRegionReader();

};

};

#endif	//	_WavletBlock3DRegionReader_h_
