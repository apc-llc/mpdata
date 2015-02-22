//
//      $Id$
//


#ifndef	_VDFIOBase_h_
#define	_VDFIOBase_h_

#include <cstdio>
#include <vapor/MyBase.h>
#include <vapor/MetadataVDC.h>

namespace VAPoR {


//
//! \class VDFIOBase
//! \brief Abstract base class for performing data IO to a VDC
//! \author John Clyne
//! \version $Revision$
//! \date    $Date$
//!
//! This class provides an API for performing low-level IO 
//! to/from a Vapor Data Collection (VDC)
//
class VDF_API	VDFIOBase : public MetadataVDC {

public:

 //! Constructor for the VDFIOBase class.
 //! \param[in] metadata Pointer to a metadata class object for which all
 //! future class operations will apply
 //! \note The success or failure of this constructor can be checked
 //! with the GetErrCode() method.
 //!
 //! \sa Metadata, GetErrCode(),
 //
 VDFIOBase(
	const MetadataVDC &metadata
 );

 //! Constructor for the VDFIOBase class.
 //! \param[in] metafile Path to a metadata file for which all
 //! future class operations will apply
 //! \note The success or failure of this constructor can be checked
 //! with the GetErrCode() method.
 //!
 //! \sa Metadata, GetErrCode(),
 //
 VDFIOBase(
	const string &metafile
 );

 virtual ~VDFIOBase();

 //! Return the read timer
 //!
 //! This method returns the accumulated clock time, in seconds, 
 //! spent reading data from files. 
 //!
 double	GetReadTimer() const { return(_read_timer_acc); };

 //! Return the seek timer
 //!
 //! This method returns the accumulated clock time, in seconds, 
 //! spent performing file seeks (in general this is zero)
 //!
 double	GetSeekTimer() const { return(_seek_timer_acc); };
 void SeekTimerReset() {_seek_timer_acc = 0;};
 void SeekTimerStart() {_seek_timer = GetTime();};
 void SeekTimerStop() {_seek_timer_acc += (GetTime() - _seek_timer);};

 //! Return the write timer
 //!
 //! This method returns the accumulated clock time, in seconds, 
 //! spent writing data to files. 
 //!
 double	GetWriteTimer() const { return(_write_timer_acc); };

 //! Return the transform timer
 //!
 //! This method returns the accumulated clock time, in seconds, 
 //! spent transforming data. 
 //!
 double	GetXFormTimer() const { return(_xform_timer_acc); };

 double GetTime() const;

 virtual int OpenVariableRead(
    size_t timestep, const char *varname, int reflevel=0, int lod=0
 ) {_varname = varname; return(0); };

 virtual int BlockReadRegion(
    const size_t bmin[3], const size_t bmax[3], float *region, bool unblock=true
 ) = 0;

 virtual int ReadRegion(
    const size_t min[3], const size_t max[3], float *region
 ) = 0;

 virtual int ReadRegion(float *region) = 0;

 virtual int ReadSlice(float *slice) = 0;

 virtual int OpenVariableWrite(
    size_t timestep, const char *varname, int reflevel=0, int lod=0
 ) {_varname = varname; return(0); };

 virtual int BlockWriteRegion(
    const float *region, const size_t bmin[3], const size_t bmax[3], 
	bool block=true
 ) = 0;

 virtual int WriteRegion(
    const float *region, const size_t min[3], const size_t max[3]
 ) = 0;

 virtual int WriteRegion(const float *region) = 0;

 virtual int WriteSlice(const float *slice) = 0;

 virtual int CloseVariable() {_varname.clear(); return(0);};

 virtual const float *GetDataRange() const = 0;

protected:

 //
 // A Bit Mask class for supporting missing data
 //
 class BitMask {
 public:
	//
	// nbits is the number of elements to be stored in the mask
	//
	BitMask();
	BitMask(size_t nbits);
	BitMask(const BitMask &bm);	// copy constructor
	~BitMask();

	//
	// Resize the bit mask to accomodate 'nbit' items. All previous data
	// is lost
	//
	void resize(size_t nbits);

	//
	// returns the size, in bytes, of internal storage needed to support
	// a bit map of 'nbits' elements
	//
	static size_t getSize(size_t nbits) {return ((nbits+7) >> 3);}

	void clear();

	//
	// returns the current size of the bit mask
	//
	size_t size() const {return (_nbits); };

	//
	// Set or unset the value of the bit mask at location 'idx', where
	// 0 < idx < nbits-1
	//
	void setBit(size_t idx, bool value);

	//
	// Get the value of the bit mask at location 'idx', where
	// 0 < idx < nbits-1
	//
	bool getBit(size_t idx) const;

	//
	// Return pointer to the internal storage for a bit mask. The length
	// of the array returned is getSize(nbits) bytes
	//
	unsigned char *getStorage() const;
	BitMask &operator=(const BitMask &bm);
	
 private:
	unsigned char *_bitmask;	// storage for bitmask
	size_t _bitmask_sz;		// size of _bitmask buffer in bytes
	size_t _nbits;

	void _BitMask(size_t nbits);
 };

 void _ReadTimerReset() {_read_timer_acc = 0;};
 void _ReadTimerStart() {_read_timer = GetTime();};
 void _ReadTimerStop() {_read_timer_acc += (GetTime() - _read_timer);};

 void _WriteTimerReset() {_write_timer_acc = 0;};
 void _WriteTimerStart() {_write_timer = GetTime();};
 void _WriteTimerStop() {_write_timer_acc += (GetTime() - _write_timer);};

 void _XFormTimerReset() {_xform_timer_acc = 0;};
 void _XFormTimerStart() {_xform_timer = GetTime();};
 void _XFormTimerStop() {_xform_timer_acc += (GetTime() - _xform_timer);};

 //
 // The Mask* methods are used to support missing data. If no missing 
 // data are present the methods below are no-ops. The missing value
 // is determined by MetadataVDC::GetMissingValue()
 //

 // Open a mask file in the VDC for writing. 
 //
 int _MaskOpenWrite(size_t timestep, string varname, int reflevel);
 int _MaskOpenRead(size_t timestep, string varname, int reflevel);
 int _MaskClose();
 int _MaskWrite(
	const float *srcblk, const size_t bmin_p[3], const size_t bmax_p[3], 
	bool block
 );
 int _MaskRead(const size_t bmin_p[3], const size_t bmax_p[3]); 

 //
 // Removes missing values from a data block and replaces them with the 
 // block's average value. If all of elements of 'blk' are missing values,
 // 'valid_data' will be set to false, otherwise it will be true
 //
 void _MaskRemove(float *blk, bool &valid_data) const;

 //
 // Using a BitMask read by _MaskRead(), restore the missing values
 // previously removed with _MaskRemove()
 //
 void _MaskReplace(size_t bx, size_t by, size_t bz, float *blk) const;
	

 static void _UnpackCoord(
    VarType_T vtype, const size_t src[3], size_t dst[3], size_t fill
 );

 static void _PackCoord(
	VarType_T vtype, const size_t src[3], size_t dst[3], size_t fill
 );

 static void _FillPackedCoord(
	VarType_T vtype, const size_t src[3], size_t dst[3], size_t fill
 );


private:

 double	_read_timer_acc;
 double	_write_timer_acc;
 double	_seek_timer_acc;
 double	_xform_timer_acc;

 double	_read_timer;
 double	_write_timer;
 double	_seek_timer;
 double	_xform_timer;

 //
 // state info to handle data sets with missing data values
 //
 int _ncid_mask;
 int _varid_mask;
 VarType_T _vtype_mask;
 float _mv_mask;
 size_t _bs_p_mask[3];
 size_t _bdim_p_mask[3];
 int _reflevel_mask;
 int _reflevel_file_mask;
 string _ncpath_mask;
 string _ncpath_mask_tmp;
 bool _open_write_mask;
 vector <BitMask> _bitmasks;
 string _varname; // currently opened variable;

 int _VDFIOBase();

 int _mask_open(
	size_t timestep, string varname, int reflevel, size_t &bitmasksz
 );
 void _downsample(const BitMask &bm0, BitMask &bm1, int l0, int l1) const;

};


 int	MkDirHier(const string &dir);
 void	DirName(const string &path, string &dir);

}

#endif	//	
