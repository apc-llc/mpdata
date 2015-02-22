#ifndef weightTable_h
#define weightTable_h
namespace VAPoR {
// The WeightTable class calculates the weight tables that are used to lookup the interpolation weight that determines the corner x,y indices in user space and
// alpha and beta coefficients that are used to interpolate the function value associated with a latitude and longitude grid coordinate.
// The table is calculated using an iteration provided by Phil Jones of LANL.  It provides alpha, beta, and corner userlon/userlat indices associated with 
// input lon/lat indices.  The input lon/lat indices are the lon/lat grid coordinates for which an interpolated value of a variable is desired.
// The resulting ulon/ulat indices correspond to user grid coordinates of the lower left corner of a rectangle whose 4 vertices are used for interpolation.
// A weight table must be calculated for all 4 grids (combinations of staggered and unstaggered ulon/ulat grids).
// There are a couple of important boundary conditions:
//	With regional models, the left and bottom edges of the ulon/ulat grid may be 1/2 cell outside the lon/lat grid.  In this case the coefficients are used to
//	extrapolate from the nearest lon/lat cell.
//	With global models, the left edge of the ulon/ulat grid may wrap around to the right edge.  The corresponding ulon/ulat cell will span the left and right 
//	edge of the grid.
// The longitude and latitude associated with a ulon/ulat cell must always be valid grid corner indices, even though the floating point lon/lat may be outside the
//	[0,360] and [-90,90] intervals
// No special provision is made for points near the north pole.  In that case, the latitude is always less than 90, so a valid rectangle will contain the point in 
//	user ulon/ulat coordinates.
// 
class VDFIOBase;
class VDF_API WeightTable {
public:
	//Construct a weight table (initially empty) for a given geo_lat and geo_lon variable.
 WeightTable(
    const float *geo_lat, const float *geo_lon,
    int ny, int nx,
    const float latexts[2], const float lonexts[2]
 );
 virtual ~WeightTable();
	
	// Interpolate a 2D or 3D variable to the lon/lat grid, using this weights table
	// Sourcedata is where the variable data has already been loaded.
	// Values in the data that are equal to missingValue are mapped to missMap
	void interp2D(const float* sourceData, float* resultData, float srcMV, float dstMV, const size_t* dims);
	
	//Calculate the weights 
	int calcWeights();

	//Calculate angle that grid makes with latitude line at a particular vertex.
	float getAngle(int ilon, int ilat) const;

	const float* getGeoLats() const { return _geo_lat;}

private:
	//Following 2 methods are for debugging:
	float bestLatLonPolar(int ulon, int ulat);
	float bestLatLon(int ulon, int ulat);
	
	bool MOMBased;
	
	//Calculate the weights alpha, beta associated with a point P=(plon,plat), based on rectangle cornered at 
	//user grid vertex nlon, nlat
	//The point P should be known to be inside the lonlat rectangle associated with the user grid cell cornered at (nlon,nlat)
	//
	void findWeight(float plon, float plat, int nlon, int nlat, float* alpha, float *beta);
	//Alternate version, based on mapping lon,lat to polar coordinates in northern hemisphere
	void findWeight2(float x, float y, int nlon, int nlat, float* alpha, float *beta);
	//Check whether a point P=(plon,plat) lies in the quadrilateral defined (in ccw order) by the 4 vertices
	//ll cornered at user grid vertex (nlon,nlat).  Result is negative if point is inside. 
	float testInQuad(float plon, float plat, int nlon, int nlat);
	//Alternate version, test is based on polar coordinates on the upper hemisphere.
	float testInQuad2(float x, float y, int nlon, int nlat);
	
	float finterp(float A,float B,float th[4]) {return (1.-A)*(1.-B)*th[0]+A*(1.-B)*th[1]+A*B*th[2]+(1.-A)*B*th[3];}
	
	float COMDIF2(float th[4],float C){return th[1]-th[0]+C*(th[0]-th[3]+th[2]-th[1]);}
	
	float COMDIF4(float th[4],float C) {return th[3]-th[0]+C*(th[0]-th[3]+th[2]-th[1]);}
	
	float DET(float A,float B,float th[4],float ph[4]){ return COMDIF2(th,B)*COMDIF4(ph,A) - COMDIF4(th,A)*COMDIF2(ph,B);}
	
	float DELT(float theta, float A,float B,float th[4]){
		return (theta-finterp(A,B,th));
	}
	float DELA(float delth,float delph,float th[4],float ph[4],float A,float B){
		float num = delth*COMDIF4(ph,A) - delph*COMDIF4(th,A);
		float det = DET(A,B,th,ph);
		if (det == 0.f) return 1.e30f; //Flag degenerate rectangles
		return num/det;
	}
	float DELB(float delth,float delph,float th[4],float ph[4],float A,float B){
		float num = delph*COMDIF2(th,B) - delth*COMDIF2(ph,B);
		float det = DET(A,B,th,ph);
		if (det == 0.f) return 1.e30f; //Flag degenerate rectangles
		return num/det;
	}
	
	float errP(float theta,float phi,float A,float B,float th[4],float ph[4]){
		return abs(theta-finterp(A,B,th)+abs(phi-finterp(A,B,ph)));
	}
	//Calculate successive approximations of A and B (i.e. alpha and beta) to be used as weights, in the bilinear approximation
	// of a point (theta,phi)
	// where:
	// theta and phi are the coordinates of the point to be approximated,
	// th[4] and ph[4] are the x and y corners of the quad containing (theta,phi) 
	//Bilinear approximation is based on work of Phil Jones, LANL
	float NEWA(float A,float B,float theta,float phi,float th[4],float ph[4]){
		float delth = DELT(theta,A,B,th);
		float delph = DELT(phi,A,B,ph);
		float dela = DELA(delth,delph,th,ph,A,B);
		if (dela == 1.e30f) return 1.e30f;
		return (A+dela);
	}
	float NEWB(float A,float B,float theta,float phi,float th[4],float ph[4]){
		float delth = DELT(theta,A,B,th);
		float delph = DELT(phi,A,B,ph);
		float delb = DELB(delth,delph,th,ph,A,B);
		if (delb == 1.e30f) return 1.e30f;
		return (B+delb);
	}
	
	
	//Storage for weights:
	float* _alphas;
	float* _betas;
	//cornerLats[j] and cornerLons[j] indicate indices of lower-left corner of user coordinate rectangle that contains
	// the lon/lat point indexed by "j".
	int* _cornerLons;
	int* _cornerLats;
	//As the table is constructed, keep track of how good is the test.  Points near boundary get replaced if a better fit is found.
	float* _testValues;
	//Arrays that hold the geo_lat and geo_lon values in the user topo data.  To determine the longitude and latitude of
	//the user grid cell vertex [ilon,ilat], you need to evaluate geo_lon[ilon+ilat*
	float* _geo_lon;
	float* _geo_lat;
	int _nx, _ny;  //Grid dimensions
	float _lonLatExtents[4];
	float _epsilon, _epsRect;
	float _deltaLat, _deltaLon;
	bool _wrapLon, _wrapLat;
		
}; //End WeightTable class
	
}; //End namespace VAPoR
#endif
