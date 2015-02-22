//
//      $Id$
//


#ifndef	_MetadataSpherical_h_
#define	_MetadataSpherical_h_

#include <vapor/MetadataVDC.h>
#ifdef WIN32
#pragma warning(disable : 4251)
#endif

namespace VAPoR {


//
//! \class MetadataSpherical
//! \brief A class for managing data set Spherical metadata
//! \author John Clyne
//! \version $Revision$
//! \date    $Date$
//!
class VDF_API MetadataSpherical : public MetadataVDC {
public:

 MetadataSpherical(
	const size_t dim[3], size_t numTransforms, size_t bs[3],
	size_t permutation[3],
	int nFilterCoef = 1, int nLiftingCoef = 1, int msbFirst =  1,
	int vdfVersion = VDF_VERSION
	);

 MetadataSpherical(const string &path);

 virtual ~MetadataSpherical();

protected: 
	int SetDefaults();
private:

};


};

#endif	//	_MetadataSpherical_h_
