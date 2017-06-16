/*
 * gdalWrapper.c
 *
 *  Created on: 12.06.2017
 *      Author: eckstein
 */

#include <cpl_port.h>

#include <gdal.h>
#include <gdal_utils.h>
#include <cpl_conv.h> /* for CPLMalloc() */

#include <ogr_srs_api.h>

#include "gdalWrapper.h"

/**
 * This helper function shall convert from given x and y coordinates of the
 * currently loaded GeoTIFF to latitude and longitude coordinates in the EPSG:3857 coordinate system
 * used by OpenStreetMap and Google Maps.
 */
int latLongFromPixel(int x, int y, double *latitude, double *longitude,
		GDALDatasetH hDataset) {

	//look here to understand (partly) what's going on :-)
	// http://www.gdal.org/osr_tutorial.html

	double adfGeoTransfPixelToGeo[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	double adfInvGeoTransfGeoToPixel[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	double dfGeoX = 0.0;
	double dfGeoY = 0.0;

	const bool bJson = true; //originally in gdalinfo_lib.cpp: const bool bJson = psOptions->eFormat == GDALINFO_FORMAT_JSON;

	/* -------------------------------------------------------------------- */
	/*      Setup projected to lat/long transform if appropriate.           */
	/* -------------------------------------------------------------------- */
	const char *pszProjection = NULL;
	if (GDALGetGeoTransform(hDataset, adfGeoTransfPixelToGeo) == CE_None) {
		pszProjection = GDALGetProjectionRef(hDataset);

		printf("hDataset has Projection set to:\n%s\n", pszProjection);

		//calculate the inverse geotransform from Geo to Pixel
		GDALInvGeoTransform(adfGeoTransfPixelToGeo, adfInvGeoTransfGeoToPixel);
	}

	OGRCoordinateTransformationH hTransPixelLatLong = NULL,
			hTransLatLongPixel = NULL;

	bool bTransformToWGS84 = false;

	if (pszProjection != NULL && strlen(pszProjection) > 0) {
		//There is a projection given in the dataset

		OGRSpatialReferenceH hLatLong = NULL;

		OGRSpatialReferenceH hProj = OSRNewSpatialReference(pszProjection);
		if (hProj != NULL) {
			OGRErr eErr = OGRERR_NONE;
			// Check that it looks like Earth before trying to reproject to wgs84...
			// Check that the major axis has approximately the correct length like mother earth has
			if (bJson
					&& fabs(OSRGetSemiMajor(hProj, &eErr) - 6378137.0)
							< 10000.0&&
							eErr == OGRERR_NONE) {
				bTransformToWGS84 = true;
				hLatLong = OSRNewSpatialReference( NULL);
				OSRSetWellKnownGeogCS(hLatLong, "WGS84");
			} else {
				hLatLong = OSRCloneGeogCS(hProj);
			}
		}

		if (hLatLong != NULL) {
			CPLPushErrorHandler(CPLQuietErrorHandler);
			hTransPixelLatLong = OCTNewCoordinateTransformation(hProj,
					hLatLong); //define transformation from Pixel to Latitude and Longitude
			hTransLatLongPixel = OCTNewCoordinateTransformation(hLatLong,
					hProj);	//define transformation from Pixel to Latitude and Longitude
			CPLPopErrorHandler();

			OSRDestroySpatialReference(hLatLong);
			// the coordinate system is not needed anymore after the transformation is defined
		}

		if (hProj != NULL)
			OSRDestroySpatialReference(hProj);
		// the coordinate system is not needed anymore after the transformation is defined
	}

	if (GDALGetGeoTransform(hDataset, adfGeoTransfPixelToGeo) == CE_None) {
		// there is another affine transformation on the input dataset to be considered
		GDALApplyGeoTransform(adfGeoTransfPixelToGeo, x, y, &dfGeoX, &dfGeoY);
	}

	double dfZ = 0.0;
	if (hTransPixelLatLong != NULL
			&& OCTTransform(hTransPixelLatLong, 1, &dfGeoX, &dfGeoY, &dfZ)) {
		printf("converted to (lat/long)= ( %lf %lf ) \n", dfGeoY, dfGeoX);
	}
	if (hTransLatLongPixel != NULL
			&& OCTTransform(hTransLatLongPixel, 1, &dfGeoX, &dfGeoY, &dfZ)) {
		double backX = 0.0, backY = 0.0;
		GDALApplyGeoTransform(adfInvGeoTransfGeoToPixel, dfGeoX, dfGeoY, &backX,
				&backY);
		printf("converted to (X/Y)= ( %lf %lf ) \n", backX, backY);
	}
}

int printGeoDataInfo(const char * infilename) {
	//most of the code is shamelessly copied from http://www.gdal.org/gdal_tutorial.html

	GDALDatasetH hDataset;
	GDALAllRegister();
	hDataset = GDALOpen(infilename, GA_ReadOnly);
	if (hDataset == NULL) {
		return EXIT_IO_ERROR;
	}

	//Ok, now we successfully opened the file and have a handle in hDataset
	//let's get some information on the dataset

	GDALDriverH hDriver;
	double adfGeoTransform[6];//see http://lists.osgeo.org/pipermail/gdal-dev/2011-July/029449.html
	hDriver = GDALGetDatasetDriver(hDataset);
	printf("Driver: %s/%s\n", GDALGetDriverShortName(hDriver),
			GDALGetDriverLongName(hDriver));
	printf("Size is %dx%dx%d\n", GDALGetRasterXSize(hDataset),
			GDALGetRasterYSize(hDataset), GDALGetRasterCount(hDataset));
	if (GDALGetProjectionRef(hDataset) != NULL)
		printf("Projection is `%s'\n", GDALGetProjectionRef(hDataset));
	if (GDALGetGeoTransform(hDataset, adfGeoTransform) == CE_None) {
		printf("Origin = (%.6f,%.6f)\n", adfGeoTransform[0],
				adfGeoTransform[3]);
		printf("Pixel Size = (%.6f,%.6f)\n", adfGeoTransform[1],
				adfGeoTransform[5]);
	}

	//Now let's fetch the raster information band
	GDALRasterBandH hBand;
	int nBlockXSize, nBlockYSize;
	int bGotMin, bGotMax;
	double adfMinMax[2];
	hBand = GDALGetRasterBand(hDataset, 1);
	GDALGetBlockSize(hBand, &nBlockXSize, &nBlockYSize);
	printf("Block=%dx%d Type=%s, ColorInterp=%s\n", nBlockXSize, nBlockYSize,
			GDALGetDataTypeName(GDALGetRasterDataType(hBand)),
			GDALGetColorInterpretationName(
					GDALGetRasterColorInterpretation(hBand)));
	adfMinMax[0] = GDALGetRasterMinimum(hBand, &bGotMin);
	adfMinMax[1] = GDALGetRasterMaximum(hBand, &bGotMax);
	if (!(bGotMin && bGotMax))
		GDALComputeRasterMinMax(hBand, TRUE, adfMinMax);
	printf("Min=%.3fd, Max=%.3f\n", adfMinMax[0], adfMinMax[1]);
	if (GDALGetOverviewCount(hBand) > 0)
		printf("Band has %d overviews.\n", GDALGetOverviewCount(hBand));
	if (GDALGetRasterColorTable(hBand) != NULL)
		printf("Band has a color table with %d entries.\n",
				GDALGetColorEntryCount(GDALGetRasterColorTable(hBand)));

	float Xpixel = GDALGetRasterXSize(hDataset) - 0.5;//-0.5 to go into the middle of the pixel
	float Yline = GDALGetRasterYSize(hDataset) - 0.5;

	double Xgeo = adfGeoTransform[0] + Xpixel * adfGeoTransform[1]
			+ Yline * adfGeoTransform[2];
	double Ygeo = adfGeoTransform[3] + Xpixel * adfGeoTransform[4]
			+ Yline * adfGeoTransform[5];

	printf("lower right corner is (%12.3f, %12.3f) in northing and easting\n",
			Xgeo, Ygeo);

	GDALApplyGeoTransform(adfGeoTransform, Xpixel, Yline, &Xgeo, &Ygeo);

	printf("lower right corner is (%12.3f, %12.3f) in northing and easting\n",
			Xgeo, Ygeo);

	latLongFromPixel(Xpixel + 0.5, Yline + .5, NULL, NULL, hDataset);

	latLongFromPixel(5000, 5000, NULL, NULL, hDataset);

//    GDALInfoOptions *options = GDALInfoOptionsNew(NULL, NULL);
//    printf("GDALInfo:\n%s\n",GDALInfo(hDataset, options));
//    GDALInfoOptionsFree(options);

	GDALClose(hDataset);

	return EXIT_SUCCESS;
}
