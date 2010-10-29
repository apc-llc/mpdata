//
//      $Id: AMRData.h,v 1.6 2009/04/30 20:34:31 clynejp Exp $
//
//***********************************************************************
//                                                                      *
//                      Copyright (C)  2006	                        *
//          University Corporation for Atmospheric Research             *
//                      All Rights Reserved                             *
//                                                                      *
//***********************************************************************
//
//	File:		AMRData.h
//
//	Author:		John Clyne
//			National Center for Atmospheric Research
//			PO 3000, Boulder, Colorado
//
//	Date:		Thu Jan 5 16:57:20 MST 2006
//
//	Description:	
//
//

#ifndef	_AMRData_h_
#define	_AMRData_h_

#include <vector>
#include <vaporinternal/common.h>
#include <vapor/MyBase.h>
#include <vapor/AMRTree.h>


namespace VAPoR {

//
//! \class AMRData
//! \brief This class manages an Adaptive Mesh Refinement grid.
//! \author John Clyne
//! \version $Revision: 1.6 $
//! \date    $Date: 2009/04/30 20:34:31 $
//!
//! This class manages block-structured Adaptive Mesh Refinement 
//! grids.
//! The AMR grid is composed of a Cartesian base grid of cells
//! (blocks) that are recursively refined into octants. The 
//! representation of the tree data structure is managed by
//! a AMRTree object.
//!
//! \note All blocks in the AMR grid, whether leaf or non-leaf nodes, contain
//! data. 
//!
//! This class is derived from the MyBase base
//! class. Hence all of the methods make use of MyBase's
//! error reporting capability - the success of any method
//! (including constructors) can (and should) be tested
//! with the GetErrCode() method. If non-zero, an error
//! message can be retrieved with GetErrMsg().
//! 
//! \sa AMRTree
//

class AMRData : public VetsUtil::MyBase {

public:



 //! Constructor for the AMRData class.
 //!
 //! Construct block structured AMR grid data object. The \param tree 
 //! parameter defines the octree hierarchy for the grid. The AMR grid
 //! defined by this object may be a subdomain of the tree pointed to by
 //! \p tree. The \p min and \p max parameters may be used to restrict
 //! the spatial domain to a subset of base blocks. Similarly, the 
 //! \p reflevel parameter may be used to restrict the refinement level
 //!
 //! \param[in] tree A pointer to an initialized AMRTree 
 //! \param[in] cell_dim A three-element array specifying the topological
 //! dimensions, in voxels, of all blocks in the grid
 //! \param[in] min A three-element array specifying the coordinates, in
 //! base blocks, of the minimum extents of the subgrid.
 //! \param[in] max A three-element array specifying the coordinates, in
 //! base blocks, of the maximum extents of the subgrid.
 //! \param[in] reflevel The maximum refinement level of the grid.
 //! If a negative value is specified, the refinement level of the tree
 //! pointed to by the \p tree parameter is used.
 //! 
 //! \note The AMRTree object is shallow copied - it's contents may not be
 //! changed by application until after this object is destroyed.
 //
 AMRData(
	const AMRTree *tree,
	const size_t cell_dim[3],
	const size_t min[3],
	const size_t max[3],
	int reflevel = -1
 );

 //! Constructor for the AMRData class.
 //!
 //! Construct block structured AMR grid data object. The \param tree 
 //! parameter defines the octree hierarchy for the grid. 
 //! The 
 //! \p reflevel parameter may be used to restrict the refinement level
 //!
 //! \param[in] tree A pointer to an initialized AMRTree 
 //! \param[in] cell_dim A three-element array specifying the topological
 //! dimensions, in voxels, of all blocks in the grid
 //! \param[in] reflevel The maximum refinement level of the grid.
 //! If a negative value is specified, the refinement level of the tree
 //! pointed to by the \p tree parameter is used.
 //! 
 //! \note The AMRTree object is shallow copied - it's contents may not be
 //! changed by application until after this object is destroyed.
 //
 AMRData(
	const AMRTree *tree,
	const size_t cell_dim[3],
	int reflevel = -1
 );

 //! Constructor an AMRData object from a Paramesh (FLASH) AMR data set.
 //!
 //! Construct block structured AMR grid data object using a Paramesh
 //! (FLASH) AMR data set to populate the grid's data elements.
 //! The \p reflevel parameter may be used to restrict 
 //! the refinement level
 //!
 //! \param[in] tree A pointer to an initialized AMRTree 
 //! \param[in] cell_dim A three-element array specifying the topological
 //! dimensions, in voxels, of all blocks in the grid
 //! \param[in] paramesh_gids A paramesh global identifier array
 //! \param[in] paramesh_bboxs An array of Paramesh bounding boxes
 //! \param[in] paramesh_unk A paramesh dependent data array
 //! \param[in] paramesh_total_blocks Total number of nodes (both leaf and
 //! \param[in] reflevel The maximum refinement level of the grid.
 //! If a negative value is specified, the refinement level of the tree
 //! pointed to by the \p tree parameter is used.
 //! 
 //! \note The AMRTree object is shallow copied - it's contents may not be
 //! changed by application until after this object is destroyed.
 //
 AMRData(
	const AMRTree *tree,
	const size_t cell_dim[3],
	const int paramesh_gids[][15],
	const float paramesh_bboxs [][3][2],
	const float paramesh_unk[],
	int total_blocks,
	int reflevel = -1
 );

 virtual ~AMRData();

 //! Change the spatial region of interest
 //!
 //! This method redefines the spatial region of interest (spatial extents
 //! and the refinement level) of the AMR data object. Any field data
 //! contained in the object are lost. 
 //!
 //! \param[in] min A three-element array specifying the coordinates, in
 //! base blocks, of the minimum extents of the subgrid.
 //! \param[in] max A three-element array specifying the coordinates, in
 //! base blocks, of the maximum extents of the subgrid.
 //! \param[in] reflevel The maximum refinement level of the grid.
 //! If a negative value is specified, the refinement level of the tree
 //! pointed to by the \p tree parameter is used.
 //
 int SetRegion(
	const size_t min[3],
	const size_t max[3],
	int reflevel
 ); 

 //! Retrieve the current defined region of interest
 //!
 //! This method returns the parameters defining the AMR data objects
 //! region of interest
 //!
 //! \sa SetRegion()
 //
 void GetRegion(
	const size_t **min,
	const size_t **max,
	int *reflevel
 ) const;

 
 //! Update the data range
 //!
 //! This method updates the data range and should be called
 //! prior to calling GetDataRange() if the field values have 
 //! been changed.
 //!
 //! \sa GetDataRange();
 void Update();

 //! Save the object to a netCDF file
 //!
 //! This method writes the entire AMR data object to the file specified
 //! by the \p path paramter. The refiment levels stored may be limited 
 //! with the \p reflevel parameter.
 //!
 //! \param[in] path Path name of the file to be written
 //! \param[in] reflevel The maximum refinement level to write. If negative,
 //! the maximum refinement level present is written
 //
 int    WriteNCDF(
    const string &path,
	int reflevel = -1
 );

 //! Read an AMR data object from a file
 //!
 //! Read an ARM data object from a file. The subregion of the data read
 //! is determined by the objects currently defined extents and the 
 //! refinement level specified by the \p reflevel parameter
 //!
 //! \param[in] path Path name of the file to be read
 //! \param[in] reflevel The maximum refinement level to write. If negative,
 //! the maximum current refinement level defined by the object is read
 //!
 //! \sa GetRegion(), SetRegion()
 //
 int ReadNCDF(
    const string &path,
	int reflevel = -1
 );


 //! Get a block of data from the AMR grid
 //!
 //! This method returns a pointer to the block of field data associated 
 //! with the cell id indicated by the \p cellid parameter
 //!
 //! \param[in] cellid The cell identifier of the block to be returned.
 //! \retval data A pointer to a block of field values
 //
 float *GetBlock(
	AMRTree::cid_t cellid
 ) const;

 //! Resample an AMR grid to a Cartesian grid
 //!
 //! This method permits the resampling of the AMR grid represented by this 
 //! object to a regular, Cartesian grid. The \p refinemenlevel parameter
 //! indicates the refinement level of the AMR grid to use in the 
 //! resampling. AMR blocks, intersecting the domain of interest that are
 //! coarser than the indicated refinement level, are interpolated to match
 //! the desired refinement level. 
 //! The default interpolation method is
 //! linear.
 //!
 //! \param[in] min A three-element array specifying, in voxels, the minimum 
 //! extents of the region of interest. The coordinate system employed is
 //! relative to the refinement level. I.e. with each successive refinement
 //! level the coordinate of a voxel is given by xyz = xyz' * 2, where xyz'
 //! is the coordinate a the preceeding level.
 //! \param[in] max A three-element array specifying, in voxels, the maximum 
 //! extents of the region of interest. 
 //! \param[in] reflevel The maximum refinement level of the grid.
 //! If a negative value is specified, the refinement level of the tree
 //! pointed to by the \p tree parameter is used.
 //! \param[out] grid A pointer to floating point array large enough to 
 //! to contain the resampled data (max[i] - min[i] + 1);
 //!
 //! \retval status A non-negative value is returned upon success
 int ReGrid(
	const size_t min[3],
	const size_t max[3],
	int reflevel,
	float *grid
 ) const;

 //! Return the data range for the grid
 //!
 //! The method returns the minimum and maximum data values, respectively,
 //! for the current AMR grid. 
 //!
 //! \param[out] range Min and Max data values, respectively
 //! \retval status A negative integer is returned on failure, otherwise
 //! the method has succeeded.
 const float *GetDataRange() const {return (_dataRange);};



 const AMRTree	*GetTree() const {return(_tree);};


private:

 static const string _numNodeToken;
 static const string _blockSizeXToken;
 static const string _blockSizeYToken;
 static const string _blockSizeZToken;
 static const string _varToken;
 static const string _blockMinToken;
 static const string _blockMaxToken;
 static const string _cellDimToken;
 static const string _refinementLevelToken;
 static const string _scalarRangeToken;
			
 const AMRTree	*_tree;
 size_t _cellDim[3];    // Dimensions, in # samples, of each cell
 float  **_treeData;    // data for each tree branch
 int *_treeDataMemSize;	// mem size allocated to treeData
 size_t _bmin[3];	// bounds of tree subregion (in base blocks)
 size_t _bmax[3];
 int _maxRefinementLevel;	// Maximum refinement level present
 float _dataRange[2];       // min and max range of data;

 void	_AMRData(
	const AMRTree *tree,
	const size_t cell_dim[3],
	const size_t min[3],
	const size_t max[3],
	int reflevel = -1
 );

 int paramesh_copy_data(
	int index,
	int pid,
	const int paramesh_gids[][15],
	const float paramesh_unk[]
 );

 // Resample a tree branch (base block and children) to a regular 
 // Cartesian grid. x,y,z are the topological coordinates of the tree
 // branch, specified in base blocks (blocks at refinement level 0).
 // min and max specify the extents 
 // of the area to be resampled
 // in block coordinates **relative to the indicated refinement level**. 
 // The coordinate system for min and max is global, i.e. the origin is 
 // the corner of the first base block. 
 //
 void regrid_branch(
	unsigned int x,	
	unsigned int y,
	unsigned int z,
	const size_t min[3],
	const size_t max[3],
	int reflevel,
	float *grid
 ) const;

 // Resample a single cell to a regular grid. This is a help method called
 // by resample_branch. tbranch is the tree branch containing the cell to
 // be processed. 
 //
 void regrid_cell(
	const AMRTreeBranch *tbranch,
	const float *branch_data,		// data associated with tbranch
	AMRTreeBranch::cid_t cellid,	// id of cell to be resampled.
	const size_t min[3],		// same is in regrid_branch
	const size_t max[3],
	int reflevel,
	float *grid
 ) const;

};

};

#endif	//	_AMRData_h_
