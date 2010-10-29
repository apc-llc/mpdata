//
//      $Id: WaveletBlockIOBase.h,v 1.1 2008/06/17 14:42:56 clynejp Exp $
//


#ifndef	_WavletBlockIOBase_h_
#define	_WavletBlockIOBase_h_

#include <cstdio>
#include <netcdf.h>
#include <vapor/MyBase.h>
#include <vapor/WaveletBlock3D.h>
#include <vapor/VDFIOBase.h>

namespace VAPoR {


//
//! \class WaveletBlockIOBase
//! \brief Performs data IO to VDF files.
//! \author John Clyne
//! \version $Revision: 1.1 $
//! \date    $Date: 2008/06/17 14:42:56 $
//!
//! This class provides an API for performing low-level IO 
//! to/from VDF files
//
class VDF_API	WaveletBlockIOBase : public VAPoR::VDFIOBase {

public:

 //! Constructor for the WaveletBlockIOBase class.
 //! \param[in] metadata Pointer to a metadata class object for which all
 //! future class operations will apply
 //! \param[in] nthreads Number of execution threads that may be used by
 //! the class for parallel execution.
 //! \note The success or failure of this constructor can be checked
 //! with the GetErrCode() method.
 //!
 //! \sa Metadata, WaveletBlock3DRegionReader, GetErrCode(),
 //
 WaveletBlockIOBase(
	const Metadata *metadata,
	unsigned int	nthreads = 1
 );

 //! Constructor for the WaveletBlockIOBase class.
 //! \param[in] metafile Path to a metadata file for which all
 //! future class operations will apply
 //! \param[in] nthreads Number of execution threads that may be used by
 //! the class for parallel execution.
 //! \note The success or failure of this constructor can be checked
 //! with the GetErrCode() method.
 //!
 //! \sa Metadata, WaveletBlock3DRegionReader, GetErrCode(),
 //
 WaveletBlockIOBase(
	const char *metafile,
	unsigned int	nthreads = 1
 );

 virtual ~WaveletBlockIOBase();

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
 virtual int    VariableExists(
	size_t ts,
	const char *varname,
	int reflevel = 0
 ) const ;


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
 );

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

 //! Close the currently opened variable.
 //!
 //! \sa OpenVariableWrite(), OpenVariableRead()
 //
 virtual int	CloseVariable();

 //! Return the minimum data values for each block in the volume
 //!
 //! This method returns an a pointer to an internal array containing
 //! the minimum data value for each block at the specified refinement
 //! level. The serial array is dimensioned nbx by nby by nbz, where
 //! nbx, nby, and nbz are the dimensions of the volume in blocks
 //! at the requested refinement level.
 //! \param[in] reflevel Refinement level requested. The coarsest
 //! refinement level is 0 (zero). A value of -1 indicates the finest
 //! refinement level contained in the VDC.
 //! \param[out] mins The address of a pointer to float to which will
 //! be assigned the address of the internal min data array
 //!
 //! \nb The values returned are undefined if the variable is either
 //! not open for reading, closed after writing
 //!
 int	GetBlockMins(const float **mins, int reflevel);

 //! Return the maximum data values for each block in the volume
 //!
 int	GetBlockMaxs(const float **maxs, int reflevel);

 //! Return the transform timer
 //!
 //! This method returns the accumulated clock time, in seconds,
 //! spent peforming wavelet transforms. There is presently no
 //! way to reset the counter (without creating a new class object)
 //!
 double	GetXFormTimer() const { return(_xform_timer); };


protected:
 static const int MAX_LEVELS = 16;	// Max # of forward transforms permitted


 int	_reflevel;	// refinement level of currently opened file.
						
 double	_xform_timer;	// records transform time by derived classes

 size_t _timeStep;		// Currently opened timestep
 string _varName;		// Currently opened variable

 float	*_mins[MAX_LEVELS];	// min value contained in a block
 float	*_maxs[MAX_LEVELS];	// max value contained in a block

 virtual int	ncDefineDimsVars(
	int j,
	const string &path,
	const int bs_dim_ids[3], 
	const int dim_ids[3]

 ) = 0;

 virtual int	ncVerifyDimsVars(
	int j,
	const string &path
 ) = 0;

 // This method moves the file pointer associated with the currently
 // open variable to the disk block indicated by 'offset' and
 // 'reflevel', where 'offset' indicates the desired position, in 
 // blocks, and 'reflevel' indicates the refinement level
 // If 'reflevel' is zero, for example, 
 // the file pointer is moved 'offset' blocks past the beginning
 // of the coefficients associated with the coarsest data resolution.
 //
 int	seekBlocks(unsigned int offset, int reflevel = 0);

 // This method moves the file pointer associated with the currently
 // open variable to the disk block containing lambda coefficients
 // indicated by the block coordinates 'bcoord'.
 //
 // A non-negative return value indicates success
 //
 virtual int	seekLambdaBlocks(const size_t bcoord[3]) = 0;

 // This method moves the file pointer associated with the currently
 // open variable to the disk block containing gamma coefficients
 // indicated by the block coordinates 'bcoord', and the refinement level
 // 'reflevel'. The parameter 'reflevel'
 // must be in the range [1.._max_reflevel]. Note, if max_xforms_c is 
 // zero, an error is generated as there are no gamma coefficients.
 //
 // A non-negative return value indicates success
 //
 virtual int	seekGammaBlocks(const size_t bcoord[3], int reflevel) = 0;

 // Read 'n' contiguous coefficient blocks, associated with the refinement
 // level, 'reflevel', from the currently open variable
 // file. The 'reflevel' parameter must be in the range [0.._max_reflevel],
 // where a value of zero indicates the coefficients for the
 // finest resolution. The results are stored in 'blks', which must 
 // point to an area of adequately sized memory.
 //
 virtual int	readBlocks(size_t n, float *blks, int reflevel) = 0;

 //
 // Read 'n' contiguous lambda coefficient blocks
 // from the currently open variable file. 
 // The results are stored in 'blks', which must 
 // point to an area of adequately sized memory.
 //
 virtual int	readLambdaBlocks(size_t n, float *blks) = 0;

 //
 // Read 'n' contiguous gamma coefficient blocks, associated with
 // the indicated refinement level, 'ref_level',
 // from the currently open variable file. 
 // The results are stored in 'blks', which must 
 // point to an area of adequately sized memory.
 // An error is generated if 'reflevel' is less than one or
 // 'reflevel' is greater than _max_reflevel.
 //
 virtual int	readGammaBlocks(size_t n, float *blks, int reflevel) = 0;

 // Write 'n' contiguous coefficient blocks, associated with the indicated
 // number of transforms, 'num_xforms', from the currently open variable
 // file. The 'num_xforms' parameter must be in the range [0.._max_reflevel],
 // where a value of zero indicates the coefficients for the
 // finest resolution. The coefficients are copied from the memory area
 // pointed to by 'blks'
 //
 virtual int	writeBlocks(const float *blks, size_t n, int reflevel) = 0;

 // Write 'n' contiguous lambda coefficient blocks
 // from the currently open variable file. 
 // The blocks are copied to disk from the memory area pointed to 
 // by  'blks'.
 //
 virtual int	writeLambdaBlocks(const float *blks, size_t n) = 0;

 // Write 'n' contiguous gamma coefficient blocks, associated with
 // the indicated refinement level, 'ref_level',
 // from the currently open variable file. 
 // The data are copied from the area of memory pointed to by 'blks'.
 // An error is generated if ref_level is less than one or
 // 'ref_level' is greater than _max_reflevel.
 //
 virtual int	writeGammaBlocks(const float *blks, size_t n, int reflevel) = 0;


protected:
 string _ncpaths[MAX_LEVELS];
 int _ncids[MAX_LEVELS];
 int _ncvars[MAX_LEVELS];
 int _ncminvars[MAX_LEVELS];
 int _ncmaxvars[MAX_LEVELS];
 int _ncoffsets[MAX_LEVELS];	// file ptr offset for netcdf

 int	n_c;		// # filter coefficients
 int	ntilde_c;	// # lifting coefficients
 int	is_open_c;	// true if a file is open
 int	write_mode_c;	// true if file opened for writing

 static const string _blockSizeXName;
 static const string _blockSizeYName;
 static const string _blockSizeZName;
 static const string _nBlocksDimName;
 static const string _blockDimXName;
 static const string _blockDimYName;
 static const string _blockDimZName;
 static const string _fileVersionName;
 static const string _refLevelName;
 static const string _nativeMinValidRegionName;
 static const string _nativeMaxValidRegionName;
 static const string _refLevMinValidRegionName;
 static const string _refLevMaxValidRegionName;
 static const string _nativeResName;
 static const string _refLevelResName;
 static const string _filterCoeffName;
 static const string _liftingCoeffName;
 static const string _scalarRangeName;
 static const string _minsName;
 static const string _maxsName;
 static const string _lambdaName;
 static const string _gammaName;

private:
 int	_objInitialized;	// has the obj successfully been initialized?

 int	_WaveletBlockIOBase();

 int open_var_write(const string &basename);
 int open_var_read(size_t ts, const char *varname, const string &basename);


};

}

#endif	//	_WavletBlock3d_h_
