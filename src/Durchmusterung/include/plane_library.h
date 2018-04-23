/*! \file plane_library.h */

/*! \brief central entry point for plane search 
 *
 *
 *  This function is the entry point to the search procedure and must be used in combination with the search library.
 */
int search_for_planes(
		const tileData* actualTile /** [in] pointer to tile object including linear array of geo values  */,
		GeoTiffHandler* myGeoTiffHandler /** [in] pointer to GeoTiffHandler for performing GeoTiff translations  */,
		float heading /** [in] angle for plane orientation */,
		float minLength /** [in] minimum length of planes */,
		float width /** [in] desired width or plane */,
		int commSocket /** [out] socket for MongoDB  */,
		const json* taskDescription /** [in] task description json object for identification purpose in MongoDB */,
		float noDataValue /** [in] definition of the not defined value  */,
		rectSize pixelSize /** [in] scaling information on pixel densitiy  */,
		double short_range_slope /** [in] max allowed slope between neighbour tiles */,
		double long_range_slope /** [in] max allowed slope between starting and endpoint of plane */,
		double orthogonal_slope /** [in] maximum allowed slope in orthogonal direction of neighboured tiles */,
		int numThreads /** [in] number of threads that should be used */,
		double max_diff_neighbours = 0.5 /** [in] max allowed elevation diff of neighboured tiles in [m] */,
		double slope_range_distance = 10 /** distance of elevation points to be treated for short range slope in [m] */
		);

