/*
 * GeoTiffHandler.h
 *
 *  Created on: 16.06.2017
 *      Author: eckstein
 */

//use -std=c++0x to use C++ 11 for compatibility with extended initializer lists;

#ifndef GEOTIFFHANDLER_H_
#define GEOTIFFHANDLER_H_

#include <stddef.h>
#include <string>

// make sure, includes are looked up in /usr/include/gdal
#include "gdal_priv.h"
#include "cpl_conv.h" // for CPLMalloc()
#include <ogr_spatialref.h>

#include "json.hpp"

// for convenience
using json = nlohmann::json;


using namespace std;

static const size_t MAX_SIZE = 0;

struct pixelPair {
	int x;
	int y;
};

struct pixelCoordFloat {
	double x;
	double y;
};

std::ostream& operator<<(std::ostream& o, const pixelCoordFloat& pc);

struct pixelCoord {
	int x;
	int y;

    //implicit conversion
    operator pixelCoordFloat() const { return {(double) x, (double) y};}
};

std::ostream& operator<<(std::ostream& o, const pixelCoord& pc);

struct rectSize {
	float x;
	float y;
};

struct datasetInfo {
	pixelPair extent = {0,0};	//the extent of the current dataset
	rectSize pixelSize = {0,0}; 	//dimension of a single pixel in [meter]
	float noDataValue = 0;	//marker for "no data" in the dataset;
		//TODO: Is it a good idea to have a comparison like if(pixelVal != noDataValue){...}?
		//Better use if(pixelVal > noDataValue){...}; It looks like the noDataValue is smaller like averything else
};

std::ostream& operator<<(std::ostream& o, const datasetInfo& inf);

struct tilingCharacteristics {
	int overallXSize = 0, overallYSize = 0;	//the extent of the currently requested cutout of the dataset
	pixelCoord topLeftPix = {0,0};	//top left pixel coordinate of the requested area
	pixelCoord bottomRightPix = {0,0};	//bottom right pixel coordinate of the requested area
	int maxTileSizeXPix = 0, maxTileSizeYPix = 0;
	int overlapXPix =0, overlapYPix =0;
	int tilesInX = 0, tilesInY = 0;	//the amount of tiles in X- and Y-direction
	float overlap = 0;	//the overlap of the tiles in [meter]
	size_t maxTileMemsize = 0;	//the maximum amount of bytes a tile needs in memory
};

std::ostream& operator<<(std::ostream& o, const tilingCharacteristics& tc);

struct tileData {
	int xTile, yTile;
	pixelPair offset;
	pixelPair width;
	size_t memsize;
	float* buf;
};

std::ostream& operator<<(std::ostream& o, const tileData& td);


struct geoCoord {
	double latitude;
	double longitude;
};

std::ostream& operator<<(std::ostream& o, const geoCoord& gc);


enum resultType {SUCCESS, ///< success
	SUCCESS_NOT_ENTIRELY_COVERED,	///< tiling info is correctly returned, but not the entire requested area is covered by Dataset
	ERROR, ///< unspecified error, should be avoided
	OBJECT_NOT_MUTABLE, /**< error when the called function tries to
		alter the underlying object when it should be immutable.
		Try to closeGeoTiff() before;*/
	IMPROPER_PROJECTION,
	IMPROPER_AFFINE_TRANSFORM,
	THIS_IS_NOT_EARTH,
	COULD_NOT_CREATE_TRANSFORMATION,
	NO_OPEN_DATASET,
	INVALID_AREA_REQUESTED,
	NO_TILING_INFO_AVAILABLE,
	INVALID_TILE_REQUESTED,
	ERROR_MEMORY_ALLOCATION,
	ERROR_DATASET_STILL_IN_USE,
	IMPROPER_DATA_FORMAT_FOR_ALTITUDE,
};


class GeoTiffHandler {
public:
	/**
	 * standard constructor to create an uninitialized GeoTiffHandler object
	 */
	GeoTiffHandler();

	/**
	 * constructor to create an initialized GeoTiffHandler object from the given filename
	 * @param [in]  infilename the filename of the file to be opened
	 * @return A new object associated with the given file
	 */
	GeoTiffHandler(const char * infilename);

	/**
	 * associate an uninitialized GeoTiffHandler object with the file from the given filename
	 * @param [in]  infilename the filename of the file to be opened
	 * @return information whether the information succeeded
	 */
	resultType openGeoTiff(const char * infilename);

	/**
	 * Releases the file the GeoTiffHandler object is associated with. Shall be called before
	 * "fundamental" changes are performed to the underlying object. Sets the internal state to "mutable" again.
	 * @return information whether the information succeeded
	 */
	resultType closeGeoTiff();

	resultType getDatasetInfo(datasetInfo *info);
	resultType getPixelExtent(rectSize *pixelSize);
	resultType getTilingInfo(const geoCoord topLeft, const geoCoord bottomRight,
			const float overlap, const size_t maxSize, tilingCharacteristics *tilingResult);
	resultType getTile(const int xTile, const int yTile, tileData *tile);
	resultType releaseTile(const int xTile, const int yTile);

	geoCoord pixel2Geo(const pixelCoordFloat source);
	geoCoord pixel2Geo(const int xTile, const int yTile, const pixelCoordFloat source);
	pixelCoord geo2Pixel(const geoCoord source);
//	pixelCoord geo2Pixel(const int xTile, const int yTile, const geoCoord source);

	/**
	 * returns a valid geoJSON Object with the properties set to "properties":{}
	 * This has to be overwritten later on.
	 * see https://gis.stackexchange.com/questions/144084/using-gdal-c-to-calculate-distance-in-meters
	 * see https://geographiclib.sourceforge.io/1.40/C/inverse_8c_source.html
	 */
	json getGeoJsonPolygon(const pixelCoordFloat start, const pixelCoordFloat end, const float width);
	json getGeoJsonPolygon(const pixelCoordFloat start, const float length, const float heading, const float width);
	json getGeoJsonPolygon(const pixelCoordFloat pix0, const pixelCoordFloat pix1, const pixelCoordFloat pix2, const pixelCoordFloat pix3);

	virtual ~GeoTiffHandler();

private:

	inline float DEG_2_RAD(float deg) const { return (deg*M_PI/180.0);}
	inline float RAD_2_DEG(float rad) const { return (rad*180.0/M_PI);}

	struct currentTile {
		bool tileLoaded = false;
		int outstandingReferences = 0;	//this is a reference counter
		int xTile=-1, yTile=-1;
		pixelPair offset={0,0};
		pixelPair width ={0,0};
		size_t memsize=0;
		float* tileBuf = NULL;
	};

	currentTile curTile;

	string filename;

	string sourceSpatialSystemWkt;

	GDALDataset *poDataset = NULL;

	double adfGeoTransfPixelToGeo[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	double adfInvGeoTransfGeoToPixel[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

	OGRCoordinateTransformation *poPixel2GeoTransform = NULL, *poGeo2PixelTransform = NULL;

	datasetInfo myDatasetInfo;
	tilingCharacteristics myTilingCharatcteristics;
};

#endif /* GEOTIFFHANDLER_H_ */
