/*
 * GeoTiffHandler.cpp
 *
 *  Created on: 16.06.2017
 *      Author: eckstein
 */

#define DEBUG

#include "GeoTiffHandler.h"
#include <cassert>
#include <cmath>
#include <iostream>

#include <gdal_priv.h>

//standard output operators for some structs
std::ostream& operator<<(std::ostream& o, const datasetInfo& inf) {
	o << "The currently loaded dataset has the following properties:\n";
	o << "Extent:\t" << inf.extent.x << " * " << inf.extent.y << " [Pixels]\n";
	o << "PixelSize:\t" << inf.pixelSize.x << " * " << inf.pixelSize.y
			<< " [meters]\n";
	o << "NoDataVal:\t" << inf.noDataValue << std::endl;
	return o;
}

std::ostream& operator<<(std::ostream& o, const geoCoord& gc) {
	return o << "(Lat: " << gc.latitude << " | Long: " << gc.longitude << ")";
}

std::ostream& operator<<(std::ostream& o, const pixelCoord& pc) {
	return o << "(X: " << pc.x << " | Y: " << pc.y << ")";
}

std::ostream& operator<<(std::ostream& o, const tilingCharacteristics& tc) {
	o << "Requested overall Size: ("<<tc.overallXSize<<", "<<tc.overallYSize<<") [pixel]\n";
	o << "Will be tiled in (" <<tc.tilesInX <<", "<<tc.tilesInY <<") [tiles]\n";
	o << "Overlap is "<< tc.overlap <<" [meter]\n";
	o << "max Memsize = " << tc.maxTileMemsize <<" [bytes]\n";
	return o;
}


GeoTiffHandler::GeoTiffHandler() {
}

GeoTiffHandler::GeoTiffHandler(const char* pszFilename) {
	GeoTiffHandler();

	switch (openGeoTiff(pszFilename)) {
	case SUCCESS:
		return;
	case IMPROPER_PROJECTION:
		throw "Could not get proper projection Well known Text to establish spatial system out of the dataset";
	case IMPROPER_AFFINE_TRANSFORM:
		throw "Could not get proper affine transformation out of the dataset";
	case THIS_IS_NOT_EARTH:
		throw("Don't know, what  it is, but it's nothing from mother earth");
	case COULD_NOT_CREATE_TRANSFORMATION:
		throw("could not create transformation from Tiff projection to WGS84 geo coordinates or vice versa");
	default:
		throw("WTF...");
	}
}

resultType GeoTiffHandler::openGeoTiff(const char* pszFilename) {
	//the full blown coordinate systems are only used when a new file is read in
	OGRSpatialReference oTiffSRS; //the Spatial Raster coordinate System used in the original TIFF file
	OGRSpatialReference oWGS84SRS;

	poDataset = (GDALDataset *) GDALOpen(pszFilename, GA_ReadOnly);

	assert(poDataset != NULL);	//TODO throw a proper exception

	//get basic dataset information
	myDatatsetInfo.extent.x = poDataset->GetRasterXSize();
	myDatatsetInfo.extent.y = poDataset->GetRasterYSize();
	assert(poDataset->GetRasterCount() == 1);//we assume, that there is exactly one Raster Band in the data

	//let's get some internals of the dataset:
	int bGotNodata = FALSE;
	GDALRasterBandH const hBand = poDataset->GetRasterBand(1);
	if (GDALGetRasterDataType(hBand) != GDT_Float32) {
		throw("Incorrect Data format (expected Float32 for altitude values)");
	}
	myDatatsetInfo.noDataValue = (float) GDALGetRasterNoDataValue(hBand,
			&bGotNodata);//we assume to have only Float32, not doubles in the dataset

	if ((sourceSpatialSystemWkt = poDataset->GetProjectionRef()).empty()) {
		return IMPROPER_PROJECTION;
	}
	//set the source Spatial System to the one read out from the file
	{	//just to limit the lifetime of temporary *cstr
		char *cstr = &sourceSpatialSystemWkt[0u];//OGRErr importFromWkt( char ** ); mutates pointer to the string, not the string itself
		oTiffSRS.importFromWkt(&cstr);
	}

	if (poDataset->GetGeoTransform(adfGeoTransfPixelToGeo) != CE_None) {
		return IMPROPER_AFFINE_TRANSFORM;
	}
	myDatatsetInfo.pixelSize = {(float)adfGeoTransfPixelToGeo[1], (float) adfGeoTransfPixelToGeo[5]};
	//calculate the inverse geotransform from Geo to Pixel
	if (!GDALInvGeoTransform(adfGeoTransfPixelToGeo,
			adfInvGeoTransfGeoToPixel)) {
		return IMPROPER_AFFINE_TRANSFORM;
	}

	OGRErr eErr = OGRERR_NONE;
	// Check that it looks like Earth before trying to reproject to wgs84...
	// Check that the major axis has approximately the correct length like mother earth has
	if (fabs(oTiffSRS.GetSemiMajor(&eErr) - 6378137.0)
			< 10000.0&& eErr == OGRERR_NONE) {
		//set up the target system to be geo coordinates in the WGS84 format which is used bei Google Maps and OSM
		oWGS84SRS.SetWellKnownGeogCS("WGS84");
	} else {
		return THIS_IS_NOT_EARTH;
	}

	//now define the transformation from pixels to geo coordinates and back again
	if (!(poPixel2GeoTransform = OGRCreateCoordinateTransformation(&oTiffSRS,
			&oWGS84SRS))) {
		return COULD_NOT_CREATE_TRANSFORMATION;
	}
	if (!(poGeo2PixelTransform = OGRCreateCoordinateTransformation(&oWGS84SRS,
			&oTiffSRS))) {
		return COULD_NOT_CREATE_TRANSFORMATION;
	}

#ifdef DEBUG
	printf("Driver: %s/%s\n", poDataset->GetDriver()->GetDescription(),
			poDataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME));
	printf("Size is %dx%dx%d\n", poDataset->GetRasterXSize(),
			poDataset->GetRasterYSize(), poDataset->GetRasterCount());
	if (poDataset->GetProjectionRef() != NULL)
		printf("Projection is `%s'\n", poDataset->GetProjectionRef());
	printf("Origin = (%.6f,%.6f)\n", adfGeoTransfPixelToGeo[0],
			adfGeoTransfPixelToGeo[3]);
	printf("Pixel Size = (%.6f,%.6f)\n", adfGeoTransfPixelToGeo[1],
			adfGeoTransfPixelToGeo[5]);
#endif
	return SUCCESS;
}

resultType GeoTiffHandler::closeGeoTiff() {
	poDataset->Release();
	poDataset = NULL;
	fill(adfGeoTransfPixelToGeo, adfGeoTransfPixelToGeo + 6, 0.0);
	fill(adfInvGeoTransfGeoToPixel, adfInvGeoTransfGeoToPixel + 6, 0.0);
	poPixel2GeoTransform = NULL;
	poGeo2PixelTransform = NULL;
	myDatatsetInfo = {};
	myTilingCharatcteristics = {};

	return SUCCESS;
}

resultType GeoTiffHandler::getDatasetInfo(datasetInfo* info) {
	if (poDataset == NULL) {
		return NO_OPEN_DATASET;
	}

	*info = this->myDatatsetInfo;
	return SUCCESS;
}

resultType GeoTiffHandler::getPixelExtent(rectSize* pixelSize) {
	if (poDataset == NULL) {
		return NO_OPEN_DATASET;
	}

	*pixelSize = this->myDatatsetInfo.pixelSize;
	return SUCCESS;

}

resultType GeoTiffHandler::getTilingInfo(const geoCoord topLeft,
		const geoCoord bottomRight, const float overlap, const size_t maxSize,
		tilingCharacteristics* tilingResult) {
	//convert the requested geo coordinates to pixel coordinates
	const pixelCoord topLeftPix = geo2Pixel(topLeft), bottomRightPix =
			geo2Pixel(bottomRight);
	if (topLeftPix.x >= bottomRightPix.x || topLeftPix.y >= bottomRightPix.y) {
		return INVALID_AREA_REQUESTED;
	}
	//TODO Überprüfe ob das angefragte Gebiet überhaupt im Datensatz liegt
	//Dabei unbedingt auch beachten, dass das 4-Eck in alle Richtungen schief sein kann
	//Dann Fehler zurück, oder einfach auf maximale Abdeckung reduzieren

	const rectSize areaExtentPix = { bottomRightPix.x - topLeftPix.x,
			bottomRightPix.y - topLeftPix.y};
	myTilingCharatcteristics.overallXSize = areaExtentPix.x;
	myTilingCharatcteristics.overallYSize = areaExtentPix.y;

	if (maxSize == MAX_SIZE) {//no size limit is requested, so no tiling is needed
		myTilingCharatcteristics.tilesInX = myTilingCharatcteristics.tilesInY =
				1;
		myTilingCharatcteristics.overlap = 0;
		myTilingCharatcteristics.maxTileMemsize = areaExtentPix.x
				* areaExtentPix.y * sizeof(float);
		*tilingResult = myTilingCharatcteristics;
		return SUCCESS;
	}

	//we have to check the tiling
	const int overlapXPix = ceil(overlap / fabs(myDatatsetInfo.pixelSize.x));
	const int overlapYPix = ceil(overlap / fabs(myDatatsetInfo.pixelSize.y));
	const float r = sqrt((maxSize/sizeof(float)) / (fabs(myDatatsetInfo.pixelSize.x) * fabs(myDatatsetInfo.pixelSize.y)));//determines how big the tile may be
	//TODO Das hier ist noch völlig falsch
	const int maxTileSizeXPix = r * fabs(myDatatsetInfo.pixelSize.y);//rounded down to the nearest pixel value
	const int maxTileSizeYPix = r * fabs(myDatatsetInfo.pixelSize.x);//aspect ratio of the pixel sizes is maintained
	myTilingCharatcteristics.tilesInX = ceil(
			areaExtentPix.x / (maxTileSizeXPix - overlapXPix));
	myTilingCharatcteristics.tilesInY = ceil(
			areaExtentPix.y / (maxTileSizeYPix - overlapYPix));
	myTilingCharatcteristics.overlap = overlap;	//will be returned in [meter] even if there may be rounding differences

	pixelPair maxTileSize;
	getTileSizePix(0, 0, &maxTileSize);
	myTilingCharatcteristics.maxTileMemsize =
			maxTileSize.x*maxTileSize.y*sizeof(float);

	*tilingResult = myTilingCharatcteristics;
	return SUCCESS;
}

resultType GeoTiffHandler::getTileSizePix(int tileIdxX, int tileIdxY,
		pixelPair *tileSize) {
	if (myTilingCharatcteristics.overallXSize == 0 &&
			myTilingCharatcteristics.overallYSize == 0) {
		return NO_TILING_INFO_AVAILABLE;
	}

	if (tileIdxX < myTilingCharatcteristics.tilesInX - 1)
		tileSize->x = ceil(
				(float)myTilingCharatcteristics.overallXSize
						/ myTilingCharatcteristics.tilesInX);
	else if (tileIdxX == myTilingCharatcteristics.tilesInX - 1)
		tileSize->x = myTilingCharatcteristics.overallXSize
				- (myTilingCharatcteristics.tilesInX - 1)
						* ceil(
								(float)myTilingCharatcteristics.overallXSize
										/ myTilingCharatcteristics.tilesInX);
	else
		return INVALID_TILE_REQUESTED;

	if (tileIdxY < myTilingCharatcteristics.tilesInY - 1)
		tileSize->y = ceil(
				(float)myTilingCharatcteristics.overallYSize
						/ myTilingCharatcteristics.tilesInY);
	else if (tileIdxY == myTilingCharatcteristics.tilesInY - 1)
		tileSize->y = myTilingCharatcteristics.overallYSize
				- (myTilingCharatcteristics.tilesInY - 1)
						* ceil(
								(float)myTilingCharatcteristics.overallYSize
										/ myTilingCharatcteristics.tilesInY);
	else
		return INVALID_TILE_REQUESTED;

	return SUCCESS;
}

resultType GeoTiffHandler::getTile(const int xTile, const int yTile,
		tileData* tile) {
}

pixelCoord GeoTiffHandler::geo2Pixel(const geoCoord geoCoord) {
	double backX = 0.0, backY = 0.0;
	double dfGeoX = geoCoord.latitude, dfGeoY = geoCoord.longitude, dfZ = 0.0;
	//First apply the transformation from geo to planar coordinates
	poGeo2PixelTransform->Transform(1, &dfGeoX, &dfGeoY, &dfZ);
	// then apply the affine Transformation on the projection
	GDALApplyGeoTransform(adfInvGeoTransfGeoToPixel, dfGeoX, dfGeoY, &backX,
			&backY);

	return {(int )(backX), (int )(backY)};//TODO check whether we need to add +0.5 somewhere to get the middle of a pixel
}

geoCoord GeoTiffHandler::pixel2Geo(const pixelCoord pixCoord) {
	double dfGeoX /**< latitude*/, dfGeoY /**< longitude*/, dfZ = 0.0;
	// First apply the affine Transformation on the projection
	GDALApplyGeoTransform(adfGeoTransfPixelToGeo, pixCoord.x, pixCoord.y,
			&dfGeoX, &dfGeoY);	//TODO check whether we need to add +0.5 somewhere to get the middle of a pixel
	//then apply the transformation from planar to geo coordinates
	poPixel2GeoTransform->Transform(1, &dfGeoX, &dfGeoY, &dfZ);
	return {dfGeoX, dfGeoY};
}

geoCoord GeoTiffHandler::pixel2Geo(const int xTile, const int yTile,
		const pixelCoord pixCoord) {
}

pixelCoord GeoTiffHandler::geo2Pixel(const int xTile, const int yTile,
		const geoCoord source) {
}

GeoTiffHandler::~GeoTiffHandler() {
	// TODO Auto-generated destructor stub
}

