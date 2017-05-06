#ifndef _LayeredGrid_
#define _LayeredGrid_
#include <vapor/common.h>
#include "RegularGrid.h"

//
//! \class LayeredGrid
//!
//! \brief This class implements a 2D or 3D layered grid: a generalization
//! of a regular grid where the spacing of grid points along a single dimension
//! may vary at each grid point. The spacing along the remaining one (2D case)
//! or two (3D case) dimensions is invariant between grid points. For example,
//! if K is the layered dimension than the z coordinate is given by some
//! function f(i,j,k):
//!
//! z = f(i,j,k)
//!
//! where f() is monotonically increasing (or decreasing) with k.
//! The remaining x and y coordinates are givey by (i*dx, j*dy)
//! for some real dx and dy .
//!
//

namespace VAPoR {
class VDF_API LayeredGrid : public RegularGrid {
public:

 //!
 //! Construct a layered grid sampling a 3D or 2D scalar function
 //!
 //! \param[in] bs A three-element vector specifying the dimensions of
 //! each block storing the sampled scalar function.
 //! \param[in] min A three-element vector specifying the ijk index
 //! of the first point in the grid. The first grid point need not coincide 
 //! with
 //! block boundaries. I.e. the indecies need not be (0,0,0)
 //! \param[in] max A three-element vector specifying the ijk index
 //! of the last point in the grid
 //! \param[in] extents A six-element vector specifying the user coordinates
 //! of the first (first three elements) and last (last three elements) of
 //! the grid points indicated by \p min and \p max, respectively.
 //! The elements of the \p extents parameter corresponding to the
 //! varying dimension are ignored. The varying dimension extents are
 //! provided by the \p coords parameter.
 //! \param[in] periodic A three-element boolean vector indicating
 //! which i,j,k indecies, respectively, are periodic. The varying 
 //! dimension may not be periodic.
 //! \param[in] blks An array of blocks containing the sampled function.
 //! The dimensions of each block
 //! is given by \p bs. The number of blocks is given by the product
 //! of the terms:
 //!
 //! \code (max[i]/bs[i] - min[i]/bs[i] + 1) \endcode
 //!
 //! over i = 0..2.
 //!
 //! \param[in] coords An array of blocks with dimension and number the
 //! same as \p blks specifying the varying dimension grid point coordinates. 
 //! \param[in] varying_dim An enumerant indicating which axis is the
 //! varying dimension: 0 for I, 1 for J, 2 for K
 //!
 LayeredGrid(
	const size_t bs[3],
	const size_t min[3],
	const size_t max[3],
	const double extents[6],
	const bool periodic[3],
	float ** blks,
	float ** coords,
	int varying_dim
);

 //! 
 //! Construct a layered grid sampling a 3D or 2D scalar function
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
 LayeredGrid(
	const size_t bs[3],
	const size_t min[3],
	const size_t max[3],
	const double extents[6],
	const bool periodic[3],
	float ** blks,
	float ** coords,
	int varying_dim,
	float missing_value
 );

 virtual ~LayeredGrid();


 //! \copydoc RegularGrid::GetValue()
 //!
 float GetValue(double x, double y, double z) const;

 //! \copydoc RegularGrid::GetUserExtents()
 //!
 virtual void GetUserExtents(double extents[6]) const {
	for (int i=0; i<6; i++) extents[i] = _extents[i];
 }


 //! \copydoc RegularGrid::GetBoundingBox()
 //!
 virtual void GetBoundingBox(
    const size_t min[3],
    const size_t max[3],
    double extents[6]
 ) const;

 //! \copydoc RegularGrid::GetEnclosingRegion()
 //!
 virtual void    GetEnclosingRegion(
	const double minu[3], const double maxu[3],
	size_t min[3], size_t max[3]
 ) const;


 //! \copydoc RegularGrid::GetUserCoordinates()
 //!
 int GetUserCoordinates(
	size_t i, size_t j, size_t k, 
	double *x, double *y, double *z
 ) const;

 //! \copydoc RegularGrid::GetIJKIndex()
 //!
 void GetIJKIndex(
	double x, double y, double z,
	size_t *i, size_t *j, size_t *k
 ) const;

 //! \copydoc RegularGrid::GetIJKIndexFloor()
 //!
 void GetIJKIndexFloor(
	double x, double y, double z,
	size_t *i, size_t *j, size_t *k
 ) const;

 //! \copydoc RegularGrid::Reshape()
 //!
 int Reshape(
	const size_t min[3],
	const size_t max[3],
	const bool periodic[3]
 );

 //! \copydoc RegularGrid::InsideGrid()
 //!
 bool InsideGrid(double x, double y, double z) const;


 //! Set periodic boundaries
 //!
 //! This method changes the periodicity of boundaries set 
 //! by the class constructor. It overides the base class method
 //! to ensure the varying dimension is not periodic.
 //!
 //! \param[in] periodic A three-element boolean vector indicating
 //! which i,j,k indecies, respectively, are periodic. The varying dimension
 //! may not be periodic.
 //
 virtual void SetPeriodic(const bool periodic[3]);

 //! \copydoc RegularGrid::GetMinCellExtents()
 //!
 virtual void GetMinCellExtents(double *x, double *y, double *z) const;

 //! Return the internal data structure containing a copy of the coordinate
 //! blocks passed in by the constructor
 //!
 float **GetCoordBlks() const { return(_coords); };


private:
 float **_coords;
 int _varying_dim;
 double _extents[6];

 double _GetValueLinear(double x, double y, double z) const;
 void _GetUserExtents(double extents[6]) const;
 void _GetBoundingBox(
    const size_t min[3],
    const size_t max[3],
    double extents[6]
 ) const;
 double _GetVaryingCoord(size_t i, size_t j, size_t k) const;

 //double verticalLinearInterpolation(double x, double y, double z) const;
 //double quadraticInterpolation(double x, double y, double z) const;

 double _interpolateVaryingCoord(
	size_t i0, size_t j0, size_t k0,
	double x, double y, double z 
 ) const;

};
};
#endif
