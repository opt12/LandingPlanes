/*
 * GeoTiffHandler.cpp
 *
 *  Created on: 16.06.2017
 *      Author: eckstein
 */

#define DEBUG

#include "GeoTiffHandler.h"
#include "readInTiff.h"
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

std::ostream& operator<<(std::ostream& o, const pixelCoordFloat& pc) {
	return o << "(X: " << pc.x << " | Y: " << pc.y << ")";
}

std::ostream& operator<<(std::ostream& o, const tilingCharacteristics& tc) {
	o << "Requested overall Size: (" << tc.overallXSize << ", "
			<< tc.overallYSize << ") [pixel]\n";
	o << "Will be tiled in (" << tc.tilesInX << ", " << tc.tilesInY
			<< ") [tiles]\n";
	o << "max tile size: (" << tc.maxTileSizeXPix << ", " << tc.maxTileSizeYPix
			<< ") [pixel]\n";
	o << "Overlap is " << tc.overlap << " [meter]; This is " << tc.overlapXPix
			<< " [pixel] in X, and " << tc.overlapYPix << " [pixel] in Y.\n";
	o << "max Memsize = " << tc.maxTileMemsize << " [bytes]\n";
	return o;
}

std::ostream& operator<<(std::ostream& o, const tileData& td) {
	o << "Tile (" << td.xTile << ", " << td.yTile << ") characteristics:\n";
	o << "Tile size: (" << td.width.x << ", " << td.width.y << ") [pixel]\n";
	o << "Offset of top left corner: (" << td.offset.x << ", " << td.offset.y
			<< ") [pixel]\n";
	o << "Memsize = " << td.memsize << " [bytes]\n";
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
	myDatasetInfo.extent.x = poDataset->GetRasterXSize();
	myDatasetInfo.extent.y = poDataset->GetRasterYSize();
	assert(poDataset->GetRasterCount() == 1);//we assume, that there is exactly one Raster Band in the data

	//let's get some internals of the dataset:
	int bGotNodata = FALSE;
	GDALRasterBandH const hBand = poDataset->GetRasterBand(1);
	if (GDALGetRasterDataType(hBand) != GDT_Float32) {
		return IMPROPER_DATA_FORMAT_FOR_ALTITUDE;
	}
	myDatasetInfo.noDataValue = (float) GDALGetRasterNoDataValue(hBand,
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
	myDatasetInfo.pixelSize = {(float)adfGeoTransfPixelToGeo[1], (float) adfGeoTransfPixelToGeo[5]};
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

	filename = string(pszFilename);
	return SUCCESS;
}

resultType GeoTiffHandler::closeGeoTiff() {

	if (curTile.outstandingReferences) {
		//there are still threads holding a handle on the data, so I cannot close
		return ERROR_DATASET_STILL_IN_USE;
	}

	poDataset->Release();
	poDataset = NULL;
	fill(adfGeoTransfPixelToGeo, adfGeoTransfPixelToGeo + 6, 0.0);
	fill(adfInvGeoTransfGeoToPixel, adfInvGeoTransfGeoToPixel + 6, 0.0);
	poPixel2GeoTransform = NULL;
	poGeo2PixelTransform = NULL;
	myDatasetInfo = {};
	myTilingCharatcteristics = {};

	if (curTile.tileBuf != NULL)
		free(curTile.tileBuf);
	curTile = {};

	return SUCCESS;
}

resultType GeoTiffHandler::getDatasetInfo(datasetInfo* info) {
	if (poDataset == NULL) {
		return NO_OPEN_DATASET;
	}

	*info = this->myDatasetInfo;
	return SUCCESS;
}

resultType GeoTiffHandler::getPixelExtent(rectSize* pixelSize) {
	if (poDataset == NULL) {
		return NO_OPEN_DATASET;
	}

	*pixelSize = this->myDatasetInfo.pixelSize;
	return SUCCESS;

}

resultType GeoTiffHandler::getTilingInfo(const geoCoord topLeft,
		const geoCoord bottomRight, const float overlap, const size_t maxSize,
		tilingCharacteristics* tilingResult) {

	//first, check if a new tiling info may be requested, or the old tilibng is till in use
	if (curTile.outstandingReferences) {
		//there are still threads holding a handle on the data, so I cannot close
		return ERROR_DATASET_STILL_IN_USE;
	}

	myTilingCharatcteristics = {};

	if (curTile.tileBuf != NULL)
		free(curTile.tileBuf);
	curTile = {};

	resultType result = SUCCESS;	//we change the return type if needed
	//convert the requested geo coordinates to pixel coordinates
	pixelCoord topLeftPix = geo2Pixel(topLeft), bottomRightPix = geo2Pixel(
			bottomRight);
	if (topLeftPix.x >= bottomRightPix.x
			|| topLeftPix.x >= myDatasetInfo.extent.x
			|| topLeftPix.y >= bottomRightPix.y
			|| topLeftPix.y >= myDatasetInfo.extent.y) {
		result = INVALID_AREA_REQUESTED;
		return result;
	}

	//crop requested area to max covered area of image file
	if (topLeftPix.x < 0) {
		topLeftPix.x = 0;
		result = SUCCESS_NOT_ENTIRELY_COVERED;
	}
	if (topLeftPix.y < 0) {
		topLeftPix.y = 0;
		result = SUCCESS_NOT_ENTIRELY_COVERED;
	}
	if (bottomRightPix.x > myDatasetInfo.extent.x) {
		bottomRightPix.x = myDatasetInfo.extent.x;//this is all done with the top left corner of the pixel
		result = SUCCESS_NOT_ENTIRELY_COVERED;
	}
	if (bottomRightPix.y > myDatasetInfo.extent.y) {
		bottomRightPix.y = myDatasetInfo.extent.y; //this is all done with the top left corner of the pixel
		result = SUCCESS_NOT_ENTIRELY_COVERED;
	}

	const pixelPair areaExtentPix = { bottomRightPix.x - topLeftPix.x,
			bottomRightPix.y - topLeftPix.y };
	myTilingCharatcteristics.overallXSize = areaExtentPix.x;
	myTilingCharatcteristics.overallYSize = areaExtentPix.y;
	myTilingCharatcteristics.topLeftPix = topLeftPix;
	myTilingCharatcteristics.bottomRightPix = bottomRightPix;

	//check the amount of available RAM
	size_t maxUsableRAM = sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGESIZE);

#ifdef DEBUG
	cout << "Currently, there are " << maxUsableRAM
			<< " Bytes of memory available on the heap.\n";
#endif

	//TODO Wie viel Prozent des maximalen Speihcers wollen wir benutzen?
	if (maxSize == MAX_SIZE)
		maxUsableRAM = maxUsableRAM / 2; //we really want to use the max size...
	else
		maxUsableRAM = min(maxUsableRAM / 2, maxSize); //we only want to use half of the maximum Memory
#ifdef DEBUG
	cout << "Will use maximum  " << maxUsableRAM << " Bytes of memory.\n";
#endif

	//we have to check the tiling
	myTilingCharatcteristics.overlapXPix = ceil(
			overlap / fabs(myDatasetInfo.pixelSize.x));
	myTilingCharatcteristics.overlapYPix = ceil(
			overlap / fabs(myDatasetInfo.pixelSize.y));

	myTilingCharatcteristics.maxTileSizeYPix = //determines how big the tile may be
			sqrt(
					(maxUsableRAM / sizeof(float))
							* ((float) areaExtentPix.y / (float) areaExtentPix.x));

	myTilingCharatcteristics.maxTileSizeXPix =
			myTilingCharatcteristics.maxTileSizeYPix
					* ((float) areaExtentPix.x / (float) areaExtentPix.y);

	//limit the maximum size (RAM-wise) to the maximum size requested
	if (areaExtentPix.y <= myTilingCharatcteristics.maxTileSizeYPix) {
		//we need only one tile in y direction
		myTilingCharatcteristics.maxTileSizeYPix = areaExtentPix.y;
		myTilingCharatcteristics.overlapYPix = 0;
	}
	myTilingCharatcteristics.tilesInY = ceil(
			(float) areaExtentPix.y
					/ (myTilingCharatcteristics.maxTileSizeYPix
							- myTilingCharatcteristics.overlapYPix));

	if (areaExtentPix.x <= myTilingCharatcteristics.maxTileSizeXPix) {
		//we need only one tile in x direction
		myTilingCharatcteristics.maxTileSizeXPix = areaExtentPix.x;
		myTilingCharatcteristics.overlapXPix = 0;
	}
	myTilingCharatcteristics.tilesInX = ceil(
			(float) areaExtentPix.x
					/ (myTilingCharatcteristics.maxTileSizeXPix
							- myTilingCharatcteristics.overlapXPix));

	myTilingCharatcteristics.overlap = overlap;	//will be returned in [meter] even if there may be rounding differences

	myTilingCharatcteristics.maxTileMemsize =
			myTilingCharatcteristics.maxTileSizeXPix
					* myTilingCharatcteristics.maxTileSizeYPix * sizeof(float);

	assert(myTilingCharatcteristics.maxTileMemsize <= maxUsableRAM);

	curTile.tileBuf = (float*) malloc(myTilingCharatcteristics.maxTileMemsize);

	//already allocate the maximum RAM needed for the maximum tile size
	if (curTile.tileBuf == NULL) {
		cerr << "Unable to allocate enough memory to prepare extract ("
				<< curTile.tileBuf << " bytes needed).\n";
		return ERROR_MEMORY_ALLOCATION;
	}

	*tilingResult = myTilingCharatcteristics;
	return result;
}

resultType GeoTiffHandler::getTile(const int xTile, const int yTile,
		tileData* tile) {
	//first check the requested tile data:
	if (xTile < 0 || yTile < 0 || xTile >= myTilingCharatcteristics.tilesInX
			|| yTile >= myTilingCharatcteristics.tilesInY) {
		//the requested tile is invalid
		*tile= {0,0, {0,0}, {0,0}, 0,NULL};
		return INVALID_TILE_REQUESTED;
	}

	tile->xTile = xTile;
	tile->yTile = yTile;

	//now let's calculate the top left pixel of the requested tile:
	tile->offset.x = myTilingCharatcteristics.topLeftPix.x +
	xTile*(myTilingCharatcteristics.maxTileSizeXPix- myTilingCharatcteristics.overlapXPix);
	tile->offset.y = myTilingCharatcteristics.topLeftPix.y +
	yTile*(myTilingCharatcteristics.maxTileSizeYPix- myTilingCharatcteristics.overlapYPix);

	if((tile->offset.x + myTilingCharatcteristics.maxTileSizeXPix) <=
			myTilingCharatcteristics.topLeftPix.x + myTilingCharatcteristics.overallXSize) {
		tile->width.x = (myTilingCharatcteristics.maxTileSizeXPix);
	} else {
		tile->width.x = myTilingCharatcteristics.overallXSize - tile->offset.x;	//the last tile in x direction might be samller
	}

	if((tile->offset.y + myTilingCharatcteristics.maxTileSizeYPix) <=
			myTilingCharatcteristics.topLeftPix.y + myTilingCharatcteristics.overallYSize) {
		tile->width.y = (myTilingCharatcteristics.maxTileSizeYPix);
	} else {
		tile->width.y = myTilingCharatcteristics.overallYSize - tile->offset.y;	//the last tile in x direction might be samller
	}

	tile->memsize = tile->width.x*tile->width.y*sizeof(float);

	//get the real tile data
	//check if the tile data is already loaded into memory
	if(curTile.tileLoaded) {
		if(curTile.xTile == xTile && curTile.yTile == yTile) {
			//the requested tile is already loaded to the buffer at curTile.tileBuf
			tile->buf = curTile.tileBuf;
			curTile.outstandingReferences++;//increment the reference counter
			return SUCCESS;
		} else if(curTile.outstandingReferences) {
			//another tile is requested, but the current tile is still in use
			return INVALID_TILE_REQUESTED;
		}
	}

	/* Either the
	 * - file is not loaded yet, or
	 * - we need a new tile and the outstandingReferences == 0;
	 * So, we have to load the tile from file.
	 */

	extractionParameters extP;
	extP.requestedwidth = tile->width.x;
	extP.requestedlength = tile->width.y;
	extP.requestedxmin = tile->offset.x;
	extP.requestedymin = tile->offset.y;
#ifdef DEBUG
	extP.verbose = true;
#else
	extP.verbose = true;
#endif

	tileCharacteristics tileC;
	tileC.buf = curTile.tileBuf;//this is the preallocated buffer for the data

	getImageInformation(&tileC, filename.c_str());

	//TODO the sanity checks need some sanity check :-)
	//there we go and should have some sanity checks;
//	if((unsigned) tile->width.x != tileC.outwidth || (unsigned) tile->width.y != tileC.outlength){
//		return INVALID_AREA_REQUESTED;
//	}
//	if(tileC.memsize != tile->memsize){
//		//we are in deep shit. something with the memory management went terribly wrong
//		return ERROR_MEMORY_ALLOCATION;
//	}

	makeExtractFromTIFFFile(extP, &tileC, filename.c_str());
	curTile.tileLoaded = true;
	curTile.outstandingReferences++;
	curTile.xTile = xTile;
	curTile.yTile = yTile;
	curTile.width.x = tile->width.x;
	curTile.width.y = tile->width.y;
	curTile.offset.x = tile->offset.x;
	curTile.offset.y = tile->offset.y;
	curTile.memsize = tile->memsize;
	tile->buf = curTile.tileBuf;

	return SUCCESS;
}

resultType GeoTiffHandler::releaseTile(const int xTile, const int yTile) {

	if (curTile.xTile != xTile || curTile.yTile != yTile) {
		//this is the incorrect Tile index
		return INVALID_TILE_REQUESTED;
	}

	curTile.outstandingReferences--;	//decrement the reference counter
	assert(curTile.outstandingReferences >= 0);

	return SUCCESS;
}

pixelCoord GeoTiffHandler::geo2Pixel(const geoCoord geoCoord) {
	double backX = 0.0, backY = 0.0;
	double dfGeoX = geoCoord.longitude, dfGeoY = geoCoord.latitude, dfZ = 0.0;
	//First apply the transformation from geo to planar coordinates
	poGeo2PixelTransform->Transform(1, &dfGeoX, &dfGeoY, &dfZ);
	// then apply the affine Transformation on the projection
	GDALApplyGeoTransform(adfInvGeoTransfGeoToPixel, dfGeoX, dfGeoY, &backX,
			&backY);

	return {(int )(backX), (int )(backY)};//TODO check whether we need to add +0.5 somewhere to get the middle of a pixel
}

geoCoord GeoTiffHandler::pixel2Geo(const pixelCoordFloat pixCoord) {
	double dfGeoX /**< longitude*/, dfGeoY /**< latitude*/, dfZ = 0.0;
	// First apply the affine Transformation on the projection
	GDALApplyGeoTransform(adfGeoTransfPixelToGeo, pixCoord.x, pixCoord.y,
			&dfGeoX, &dfGeoY);//TODO check whether we need to add +0.5 somewhere to get the middle of a pixel
	//then apply the transformation from planar to geo coordinates
	poPixel2GeoTransform->Transform(1, &dfGeoX, &dfGeoY, &dfZ);
	return {dfGeoY, dfGeoX};
}

geoCoord GeoTiffHandler::pixel2Geo(const int xTile, const int yTile,
		const pixelCoordFloat pixCoord) {
	//first check the requested tile data:
	if (xTile < 0 || yTile < 0 || xTile >= myTilingCharatcteristics.tilesInX
			|| yTile >= myTilingCharatcteristics.tilesInY) {
		//the requested tile is invalid
		return {999,999};	//TODO Das ist ein echt blöder Fehlerwert
	}

	pixelPair offset;

	//now let's calculate the top left pixel of the requested tile:
	offset.x = myTilingCharatcteristics.topLeftPix.x
			+ xTile
					* (myTilingCharatcteristics.maxTileSizeXPix
							- myTilingCharatcteristics.overlapXPix);
	offset.y = myTilingCharatcteristics.topLeftPix.y
			+ yTile
					* (myTilingCharatcteristics.maxTileSizeYPix
							- myTilingCharatcteristics.overlapYPix);

	return pixel2Geo( { offset.x + pixCoord.x, offset.y + pixCoord.y });

}

pixelCoord GeoTiffHandler::geo2Pixel(const int xTile, const int yTile,
		const geoCoord source) {
}

json GeoTiffHandler::getGeoJsonPolygon(const pixelCoordFloat pix0,
		const pixelCoordFloat pix1, const pixelCoordFloat pix2,
		const pixelCoordFloat pix3) {

	const geoCoord geo0 = pixel2Geo(pix0);
	const geoCoord geo1 = pixel2Geo(pix1);
	const geoCoord geo2 = pixel2Geo(pix2);
	const geoCoord geo3 = pixel2Geo(pix3);

	return {
		{	"type", "Feature"},
		{	"geometry", {
				{	"type", "Polygon"},
				{	"coordinates", { {
							{	geo0.longitude, geo0.latitude},
							{	geo1.longitude, geo1.latitude},
							{	geo2.longitude, geo2.latitude},
							{	geo3.longitude, geo3.latitude},
							{	geo0.longitude, geo0.latitude}
						}}
				}
			}},
		{	"properties", json::object()}
	};
}

GeoTiffHandler::~GeoTiffHandler() {
	//we need to release the memory, we allocated to our curTile.tileBuf
	//however, be careful to destroy an object that has still outstandingReferences

	if (curTile.tileBuf)
		free(curTile.tileBuf);
	//TODO: müssen wir noch eine Abfrage auf outstandingReferences einbauen und im Zweifel eine Exception werfen?
	//In dem Fall zeigen ja noch Alias-Referenzen in den jetzt freigegebenen Speicher.
}

json GeoTiffHandler::getGeoJsonPolygon(const pixelCoordFloat start,
		const pixelCoordFloat end, const float width) {
	//TODO This implementation currently does not care about earth being a sphere
	//I just calculate the vertices of the polygon as if earth was flat
	//if this is not sufficient, I'd have to solve the geodesic problem like shown here
	//	 * see https://gis.stackexchange.com/questions/144084/using-gdal-c-to-calculate-distance-in-meters
	//   * see https://geographiclib.sourceforge.io/1.40/C/inverse_8c_source.html
	const double EPSILON = 0.01;

	double deltaX = end.x - start.x;
	double deltaY = end.y - start.y;
	double heading;
	if (abs(deltaY) <= EPSILON) {
		heading = deltaX >= 0 ? (90.0 / 180.0) * M_PI : (270.0 / 180.0) * M_PI;
	} else {
		heading =
				deltaX >= 0 ?
						atan2(deltaX, -deltaY) : atan2(deltaX, deltaY) + M_PI;	//y goes from top to down
	}

	pixelCoordFloat pix0 = { start.x
			+ width * sin(heading - M_PI / 2.0) / myDatasetInfo.pixelSize.x,
			start.y
					+ width * cos(heading - M_PI / 2.0)
							/ myDatasetInfo.pixelSize.y };
	pixelCoordFloat pix1 = { start.x
			+ width * sin(heading + M_PI / 2.0) / myDatasetInfo.pixelSize.x,
			start.y
					+ width * cos(heading + M_PI / 2.0)
							/ myDatasetInfo.pixelSize.y };
	pixelCoordFloat pix2 = { end.x
			+ width * sin(heading + M_PI / 2.0) / myDatasetInfo.pixelSize.x,
			end.y
					+ width * cos(heading + M_PI / 2.0)
							/ myDatasetInfo.pixelSize.y };
	pixelCoordFloat pix3 = { end.x
			+ width * sin(heading - M_PI / 2.0) / myDatasetInfo.pixelSize.x,
			end.y
					+ width * cos(heading - M_PI / 2.0)
							/ myDatasetInfo.pixelSize.y };

	return GeoTiffHandler::getGeoJsonPolygon(pix0, pix1, pix2, pix3);
}

json GeoTiffHandler::getGeoJsonPolygon(const pixelCoordFloat start,
		const float length, const float heading, const float width) {
	//TODO This implementation currently does not care about earth being a sphere
	//I just calculate the vertices of the polygon as if earth was flat
	//if this is not sufficient, I'd have to solve the geodesic problem like shown here
	//	 * see https://gis.stackexchange.com/questions/144084/using-gdal-c-to-calculate-distance-in-meters
	//   * see https://geographiclib.sourceforge.io/1.40/C/inverse_8c_source.html

	const pixelCoordFloat end = {start.x
			+ length * sin(heading) / myDatasetInfo.pixelSize.x,
			start.y
					+ length * cos(heading)
							/ myDatasetInfo.pixelSize.y };

	return GeoTiffHandler::getGeoJsonPolygon(start, end, width);
}
