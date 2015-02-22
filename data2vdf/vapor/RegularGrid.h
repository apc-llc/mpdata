#ifndef _RegularGrid_
#define _RegularGrid_

#include <ostream>
#include <vapor/common.h>

//
//! \class RegularGrid
//!
//! \brief This class implements a 2D or 3D regular grid: a tessellation
//! of Euculidean space by rectangles (2D) or parallelpipeds (3D). Each
//! grid point can be addressed by an index(i,j,k), where \a i, \p a
//! and \a k range from 0 to \a dim - 1, where \a dim is the dimension of the
//! \a I, \a J, or \a K axis, respectively. Moreover, each grid point
//! has a coordinate in a user-defined coordinate system given by 
//! (\a i * \a dx, \a j * \a dy, \a k * \a dz) for some real 
//! numbers \a dx, \a dy, and \a dz representing the grid spacing.
//!
//! The regular grid samples a scalar function at each grid point. The
//! scalar function samples are stored as an array decomposed into 
//! equal-sized blocks.
//!
//! Because grid samples are repesented internally as arrays, when accessing
//! multiple grid points better performance is achieved by using
//! unit stride. The \a I axis varies fastest (has unit stride), 
//! followed by \a J, then \a K. Best performance is achieved
//! when using the class iterator: RegularGrid::Iterator.
//!
//! For methods that allow the specification of grid indecies or coordinates
//! as a single parameter tuple (e.g. float coordinate[3]) the first element
//! of the tuple corresponds to the \a I axis, etc.
//

namespace VAPoR {
class VDF_API RegularGrid {
public:

 //!
 //! Construct a regular grid sampling a 3D or 2D scalar function
 //!
 //! The sampled function is represented as a 2D or 3D array, decomposed
 //! into smaller blocks. The dimensions of the array are not
 //! constrained to coincide with block boundaries. 
 //!
 //! If \p blks is NULL a dataless RegularGrid object is returned. 
 //! Data can not be retrieved from a dataless RegularGrid. However,
 //! coordinate access methods may still be invoked.
 //!
 //! \param[in] bs A three-element vector specifying the dimensions of
 //! each block storing the sampled scalar function.
 //! \param[in] min A three-element vector specifying the ijk index
 //! of the first point in the grid. The first grid point need not coincide 
 //! with
 //! block boundaries. I.e. the indecies need not be (0,0,0): the first
 //! grid point is not required to be the first element of the array.
 //! \param[in] max A three-element vector specifying the ijk index
 //! of the last point in the grid
 //! \param[in] extents A six-element vector specifying the user coordinates
 //! of the first (first three elements) and last (last three elements) of
 //! the grid points indicated by \p min and \p max, respectively.
 //! These two points define the extents of the smallest axis-aligned bounding
 //! box that completely encloses the grid.
 //! \param[in] periodic A three-element boolean vector indicating
 //! which i,j,k indecies, respectively, are periodic
 //! \param[in] blks An array of blocks containing the sampled function. 
 //! The dimensions of each block 
 //! is given by \p bs. The number of blocks is given by the product 
 //! of the terms: 
 //!
 //! \code (max[i]/bs[i] - min[i]/bs[i] + 1) \endcode
 //!
 //! over i = 0..2.
 //!
 //! A shallow copy of the blocks is made by the constructor
 //!
 RegularGrid(
	const size_t bs[3],
	const size_t min[3],
	const size_t max[3],
	const double extents[6],
	const bool periodic[3],
	float ** blks
 );

 //! 
 //! Construct a regular grid sampling a 3D or 2D scalar function
 //! that contains missing values.
 //!
 //! This constructor adds a parameter, \p missing_value, that specifies 
 //! the value of missing values in the sampled function. When 
 //! reconstructing the function at arbitrary coordinates special 
 //! consideration is given to grid points with missing values that 
 //! are used in the reconstruction.
 //! 
 //! \sa GetValue()
 //!
 RegularGrid(
	const size_t bs[3],
	const size_t min[3],
	const size_t max[3],
	const double extents[6],
	const bool periodic[3],
	float ** blks,
	float missing_value
 );

 virtual ~RegularGrid();

 //! Set or Get the data value at the indicated grid point
 //!
 //! This method provides read or write access to the scalar data value
 //! defined at the grid point indicated by index(i,j,k). The range
 //! of valid indecies is between zero and \a dim - 1, where \a dim
 //! is the dimesion of the grid returned by GetDimensions()
 //!
 //! If any of the indecies \p i, \p j, or \p k are outside of the 
 //! valid range the results are undefined
 //!
 //! \param[in] i index of grid point along fastest varying dimension
 //! \param[in] j index of grid point along second fastest varying dimension
 //! \param[in] k index of grid point along third fastest varying dimension
 //!
 float &AccessIJK(size_t i, size_t j, size_t k) const;

 //! Get the reconstructed value of the sampled scalar function
 //!
 //! This method reconstructs the scalar field at an arbitrary point
 //! in space. If the point's coordinates are outside of the grid's 
 //! coordinate extents as returned by GetUserExtents(), and the grid
 //! is not periodic along the out-of-bounds axis, the value
 //! returned will be the \a missing_value. 
 //!
 //! If the value of any of the grid point samples used in the reconstruction
 //! is the \a missing_value then the result returned is the \a missing_value.
 //!
 //! The reconstruction method used is determined by interpolation 
 //! order returned by GetInterpolationOrder()
 //!
 //! \param[in] x coordinate along fastest varying dimension
 //! \param[in] y coordinate along second fastest varying dimension
 //! \param[in] z coordinate along third fastest varying dimension
 //!
 //! \sa GetInterpolationOrder(), HasPeriodic(), GetMissingValue()
 //! \sa GetUserExtents()
 //!
 virtual float GetValue(double x, double y, double z) const;

 //! Return the extents of the user coordinate system
 //!
 //! This method returns min and max extents of the user coordinate 
 //! system defined on the grid. The minimum extent is the coordinate
 //! of the grid point with index(0,0,0). The maximum extent
 //! is the coordinate of the grid point with index(nx-1, ny-1, nz-1), 
 //! where \a nx, \a ny, and \a nz are the I, J, K dimensions,
 //! respectively.
 //!
 //! \param[out] extents A six-element array, the first three values will
 //! contain the minimum coordinate, and the last three values the 
 //! maximum coordinate
 //!
 //! \sa GetDimensions(), RegularGrid()
 //!
 virtual void GetUserExtents(double extents[6]) const;

 //! Return the extents of the axis-aligned bounding box enclosign a region
 //!
 //! This method returns min and max extents, in user coordinates,
 //! of the smallest axis-aligned box enclosing the region defined
 //! by the corner grid points, \p min and \p max.
 //!
 //! \note The results are undefined if any coordinate of \p min is 
 //! greater than the coresponding coordinate of \p max.
 //!
 //! \param[out] extents A six-element array, the first three values will
 //! contain the minimum coordinate, and the last three values the 
 //! maximum coordinate
 //!
 //! \sa GetDimensions(), RegularGrid()
 //!
 virtual void GetBoundingBox(
	const size_t min[3], const size_t max[3], double extents[6]
 ) const;

 //!
 //! Get voxel coordinates of grid containing a region
 //!
 //! Calculates the starting and ending IJK voxel coordinates of the 
 //! smallest grid
 //! completely containing the rectangular region defined by the user
 //! coordinates \p minu and \p maxu
 //! If rectangluar region defined by \p minu and \p maxu can 
 //! not be contained the
 //! minimum and maximum IJK coordinates are returned in 
 //! \p min and \p max, respectively
 //!
 //! \param[in] minu User coordinates of minimum coorner
 //! \param[in] maxu User coordinates of maximum coorner
 //! \param[out] min Integer coordinates of minimum coorner
 //! \param[out] max Integer coordinates of maximum coorner
 //!
 virtual void    GetEnclosingRegion(
	const double minu[3], const double maxu[3], size_t min[3], size_t max[3]
 ) const;

 //! Return the min and max valid IJK index
 //!
 //! This method returns the minimum and maximum valid IJK index
 //!
 //! \deprecated  This method is deprecated. Use GetDimensions()
 //!
 //! \param[out] min[3] Minimum valid IJK index
 //! \param[out] max[3] Maximum valid IJK index
 //
 virtual void GetIJKMinMax(size_t min[3], size_t max[3]) const {
	min[0] = min[1] = min[2] = 0;
	max[0] = _max[0]-_min[0]; max[1] = _max[1]-_min[1]; max[2] = _max[2]-_min[2];
 }

 //! Return the origin of the grid in global IJK coordinates
 //!
 //! This method returns the value of the \p min parameter passed
 //! to the constructor
 //!
 //! \param[out] min[3] Minimum IJK coordinates in global grid coordinates
 //
 virtual void GetIJKOrigin(size_t min[3]) const {
	min[0] = _minabs[0]; min[1] = _minabs[1]; min[2] = _minabs[2];
 }

 //! Return the ijk dimensions of grid
 //!
 //! Returns the number of grid points defined along each axis of the grid
 //! 
 //! \param[out] dims A three element array containing the grid dimensions
 //!
 void GetDimensions(size_t dims[3]) const;

 //! Return the value of the \a missing_value parameter
 //!
 //! The missing value is a special value intended to indicate that the 
 //! value of the sampled or reconstructed function is unknown at a
 //! particular point.
 //!
 float GetMissingValue() const {return (_missingValue); };

 //! Set the missing value indicator
 //!
 //! This method sets the value of the missing value indicator. The
 //! method does not the value of any grid point locations. 
 //!
 //! \sa HasMissingData(), GetMissingValue()
 //!
 void SetMissingValue(float missing_value) {
	_missingValue = missing_value; _hasMissing = true;
 };

 //! Return missing data flag
 //!
 //! This method returns true iff the class instance was created with 
 //! the constructor specifying \p missing_value parameter or if 
 //! the SetMissingValue() method has been called. This does not
 //! imply that grid points exist with missing data, only that the 
 //! class was constructed with the missing data version of the constructor.
 //
 bool HasMissingData() const { return (_hasMissing); };

 //! Return the interpolation order to be used during function reconstruction
 //!
 //! This method returns the order of the interpolation method that will
 //! be used when reconstructing the sampled scalar function
 //!
 //! \sa SetInterpolationOrder()
 //!
 virtual int GetInterpolationOrder() const {return _interpolationOrder;};

 //! Set the interpolation order to be used during function reconstruction
 //!
 //! This method sets the order of the interpolation method that will
 //! be used when reconstructing the sampled scalar function. Valid values
 //! of \p order are 0 and 1, corresponding to nearest-neighbor and linear
 //! interpolation, respectively. If \p order is invalid it will be silently
 //! set to 1. The default interpolation order is 1 
 //!
 //! \param[in] order interpolation order
 //! \sa GetInterpolationOrder()
 //!
 virtual void SetInterpolationOrder(int order);

 //! Return the user coordinates of a grid point
 //!
 //! This method returns the user coordinates of the grid point
 //! specified by index(i,j,k)
 //!
 //! \param[in] i index of grid point along fastest varying dimension
 //! \param[in] j index of grid point along second fastest varying dimension
 //! \param[in] k index of grid point along third fastest varying dimension
 //! \param[out] x coordinate of grid point along fastest varying dimension
 //! \param[out] y coordinate of grid point along second fastest 
 //! varying dimension
 //! \param[out] z coordinate of grid point along third fastest 
 //! varying dimension
 //!
 //! \retval status A negative int is returned if index(i,j,k) is out
 //! out of bounds
 //!
 virtual int GetUserCoordinates(
	size_t i, size_t j, size_t k, 
	double *x, double *y, double *z
 ) const;

 //! Return the closest grid point to the specified user coordinates
 //!
 //! This method returns the ijk index of the grid point closest to
 //! the specified user coordinates based on Euclidean distance. If any 
 //! of the input coordinates correspond to periodic dimensions the 
 //! the coordinate(s) are first re-mapped to lie inside the grid
 //! extents as returned by GetUserExtents()
 //!
 //! \param[in] x coordinate along fastest varying dimension
 //! \param[in] y coordinate along second fastest varying dimension
 //! \param[in] z coordinate along third fastest varying dimension
 //! \param[out] i index of grid point along fastest varying dimension
 //! \param[out] j index of grid point along second fastest varying dimension
 //! \param[out] k index of grid point along third fastest varying dimension
 //!
 virtual void GetIJKIndex(
	double x, double y, double z,
	size_t *i, size_t *j, size_t *k
 ) const;

 //! Return the corner grid point of the cell containing the 
 //! specified user coordinates
 //!
 //! This method returns the smallest ijk index of the grid point of
 //! associated with the cell containing 
 //! the specified user coordinates. If any 
 //! of the input coordinates correspond to periodic dimensions the 
 //! the coordinate(s) are first re-mapped to lie inside the grid
 //! extents as returned by GetUserExtents()
 //!
 //! If the specified coordinates lie outside of the grid (are not 
 //! contained by any cell) the lowest valued ijk index of the grid points
 //! defining the boundary cell closest
 //! to the point are returned. 
 //!
 //! \param[in] x coordinate along fastest varying dimension
 //! \param[in] y coordinate along second fastest varying dimension
 //! \param[in] z coordinate along third fastest varying dimension
 //! \param[out] i index of grid point along fastest varying dimension
 //! \param[out] j index of grid point along second fastest varying dimension
 //! \param[out] k index of grid point along third fastest varying dimension
 //!
 virtual void GetIJKIndexFloor(
	double x, double y, double z,
	size_t *i, size_t *j, size_t *k
 ) const;

 //! Return the min and max data value
 //!
 //! This method returns the values of grid points with min and max values,
 //! respectively.
 //!
 //! \param[out] range[2] A two-element array containing the mininum and
 //! maximum values, in that order
 //!
 virtual void GetRange(float range[2]) const;

 //! Change the voxel exents specified by the constructor
 //!
 //! This method permits the grid to be reshaped under the constraints
 //! that: 1) the number of blocks does not change, and 2) the grid
 //! can only get smaller, not larger. I.e. \p min can only increase 
 //! and \p max can only decrease.
 //!
 //! \param[in] min A three-element vector specifying the ijk index
 //! of the first point in the grid. The first grid point need not coincide 
 //! with
 //! block boundaries. I.e. the indecies need not be (0,0,0): the first
 //! grid point is not required to be the first element of the array
 //! \param[in] max A three-element vector specifying the ijk index
 //! of the last point in the grid.
 //! \param[in] periodic A three-element boolean vector indicating
 //! which i,j,k indecies, respectively, are periodic
 //!
 virtual int Reshape(
	const size_t min[3],
	const size_t max[3],
	const bool periodic[3]
 );


 //! Return true if the specified point lies inside the grid
 //!
 //! This method can be used to determine if a point expressed in
 //! user coordinates reside inside or outside the grid
 //!
 //! \param[in] x coordinate along fastest varying dimension
 //! \param[in] y coordinate along second fastest varying dimension
 //! \param[in] z coordinate along third fastest varying dimension
 //!
 //! \retval bool True if point is inside the grid
 //!
 virtual bool InsideGrid(double x, double y, double z) const;


 //! Check for periodic boundaries
 //!
 //! This method returns a boolean for each dimension indicating 
 //! whether the data along that dimension has periodic boundaries.
 //!
 //! \param[out] idim periodicity of fastest varying dimension
 //! \param[out] jdim periodicity of second fastest varying dimension
 //! \param[out] kdim periodicity of third fastest varying dimension
 //
 virtual void HasPeriodic(bool *idim, bool *jdim, bool *kdim) const {
	*idim = _periodic[0]; *jdim = _periodic[1]; *kdim = _periodic[2];
 }

 //! Set periodic boundaries
 //!
 //! This method changes the periodicity of boundaries set 
 //! by the class constructor
 //!
 //! \param[in] periodic A three-element boolean vector indicating
 //! which i,j,k indecies, respectively, are periodic
 //
 virtual void SetPeriodic(const bool periodic[3]) {
	_periodic[0] = periodic[0];
	_periodic[1] = periodic[1];
	_periodic[2] = periodic[2];
 }

 void GetBlockSize(size_t bs[3]) const {
	bs[0] = _bs[0]; bs[1] = _bs[1]; bs[2] = _bs[2];
 }

 //! Return the minimum grid spacing between all grid points
 //!
 //! This method returns the minimum distance, in user coordinates,
 //! between adjacent grid points  for all cells in the grid. 
 //!
 //! \param[out] x Minimum distance between grid points along X axis
 //! \param[out] y Minimum distance between grid points along Y axis
 //! \param[out] z Minimum distance between grid points along Z axis
 //!
 //! \note For a regular grid all cells have the same dimensions
 //!
 virtual void GetMinCellExtents(double *x, double *y, double *z) const {
	*x = _delta[0]; *y = _delta[1]; *z = _delta[2];
 };

 //! Return the internal data structure containing a copy of the blocks
 //! passed in by the constructor
 //!
 float **GetBlks() const { return(_blks); };

 //! Return the number of blocks that were passed in to the constructor
 //!
 size_t GetNumBlks() const { return(_nblocks); };

 //! \deprecated  This method is deprecated
 float Next();

 //! \deprecated  This method is deprecated
 void ResetItr();

 
 class VDF_API Iterator {
 public:
	Iterator (RegularGrid *rg);
	Iterator ();
	~Iterator () {}

	inline float &operator*() {return (*_itr);}

	Iterator &operator++();	// ++prefix 
	Iterator operator++(int);	// postfix++

	bool operator==(const Iterator &other);
	bool operator!=(const Iterator &other);

    int GetUserCoordinates(
	double *x, double *y, double *z
    ) const { return(_rg->GetUserCoordinates(
		_x - _rg->_min[0], _y - _rg->_min[1], _z - _rg->_min[2], x,y,z));
	};

 private:
	RegularGrid *_rg;
	size_t _x, _y, _z;	// current index into _rg->_min[3]
	size_t _xb;	// x index within a block
	float *_itr;
	bool _end;


 };

 Iterator begin() { return( Iterator(this)); }
 Iterator end() { return(Iterator()); }


 class VDF_API ConstIterator {
 public:
	ConstIterator (const RegularGrid *rg);
	ConstIterator ();
	~ConstIterator () {}

	inline float &operator*() {return (*_itr);}

	ConstIterator &operator++();	// ++prefix 
	ConstIterator operator++(int);	// postfix++

	bool operator==(const ConstIterator &other);
	bool operator!=(const ConstIterator &other);

    int GetUserCoordinates(
	double *x, double *y, double *z
    ) const { return(_rg->GetUserCoordinates(
		_x - _rg->_min[0], _y - _rg->_min[1], _z - _rg->_min[2], x,y,z));
	};

 private:
	const RegularGrid *_rg;
	size_t _x, _y, _z;	// current index into _rg->_min[3]
	size_t _xb;	// x index within a block
	float *_itr;
	bool _end;


 };

 ConstIterator begin() const { return( ConstIterator(this)); }
 ConstIterator end() const { return(ConstIterator()); }




 
 VDF_API friend std::ostream &operator<<(std::ostream &o, const RegularGrid &rg);



protected: 

 float &_AccessIJK(float **blks, size_t i, size_t j, size_t k) const;
 void _ClampCoord(double &x, double &y, double &z) const;
 void _SetExtents(const double extents[6]);

private:
	size_t _bs[3];	// dimensions of each block
	size_t _bdims[3];	// dimensions (specified in blocks) of ROI 
	size_t _min[3];	// ijk offset of first voxel in ROI (relative to ROI)
	size_t _max[3];	// ijk offset of last voxel in ROI (relative to ROI)
	size_t _minabs[3];	// ijk offset of first voxel in ROI (absolute coords)
	size_t _maxabs[3];	// ijk offset of last voxel in ROI (absolute coords)
	double _minu[3];	
	double _maxu[3];	// User coords of first and last voxel
	double _delta[3];	// increment between grid points in user coords
	bool _periodic[3];	// periodicity of boundaries
	float _missingValue;
	bool _hasMissing;
	int _interpolationOrder;	// Order of interpolation 
	size_t _x, _y, _z, _xb;
	bool _end;
	size_t _nblocks; // num blocks allocated to _blks
	float *_itr;
	float **_blks;

	int _RegularGrid(
		const size_t bs[3], const size_t min[3], const size_t max[3],
		const double extents[6], const bool periodic[3], float ** blks
	);
	float _GetValueNearestNeighbor(double x, double y, double z) const;
	float _GetValueLinear(double x, double y, double z) const;

};
};
#endif
