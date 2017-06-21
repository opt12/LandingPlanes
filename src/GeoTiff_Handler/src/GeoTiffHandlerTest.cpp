//============================================================================
// Name        : GeoTiff_Handler.cpp
// Author      : Felix Eckstein; Matr-# 8161569
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include <cassert>

#include "GeoTiffHandler.h"
#include "readInTiff.h"
#include "config.h"	//TODO: Das ist unschön, wenn wir nicht auch autoconfig und autoheaders benutzen wollen.

using namespace std;

static void usage() {
	fprintf(stderr,
			"testReadInTiff v" PACKAGE_VERSION " license GNU GPL v3 (c) 2013-2016 Christophe Deroulers\n\n");
	fprintf(stderr,
			"Quote \"Deroulers et al., Diagnostic Pathology 2013, 8:92\" in your production\n       http://doi.org/10.1186/1746-1596-8-92\n\n");
	fprintf(stderr, "Usage: tifffastcrop [options] input.tif\n\n");
	fprintf(stderr,
			" Extracts (crops), without loading the full image input.tif into memory, a\nrectangular region from it, and saves it. Output file name is output_name if\ngiven, otherwise a name derived from input.tif. Output file format is guessed\nfrom output_name's extension if possible. Options:\n");
	fprintf(stderr, " -v                verbose monitoring\n");
	fprintf(stderr,
			" -T                report TIFF errors/warnings on stderr (no dialog boxes)\n");
	fprintf(stderr,
			" -E x,y,w,l        region to extract/crop (x,y: coordinates of top left corner,\n");
	fprintf(stderr, "   w,l: width and length in pixels)\n");
}

static void stderrErrorHandler(const char* module, const char* fmt,
		va_list ap) {
	if (module != NULL)
		fprintf(stderr, "%s: ", module);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, ".\n");
}

static void stderrWarningHandler(const char* module, const char* fmt,
		va_list ap) {//we assume verbose == true holds for the purpose of testing
	if (module != NULL)
		fprintf(stderr, "%s: ", module);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, ".\n");
}

int printMatrixToFile(const char* filename, const char* matrixName,
		const int tileSizeX, const int tileSizeY, float *matrix) {
    FILE* outputFile1 = fopen(filename,"w");
    if(!outputFile1)
    {
            fprintf(stderr,"Could not open file %s for writing\n", filename);
            return 1;
    }

    fprintf(outputFile1, "# Created by testReadInTiff.c\n"
    		"# name: %s\n"
    		"# type: matrix\n"
    		"# rows: %d\n"
    		"# columns: %d\n",
			matrixName, tileSizeY, tileSizeX);

	for(int y=0; y < tileSizeY; y++){
		for(int x= 0; x< tileSizeX; x++){
    		const int lin = y*tileSizeX+x;
			fprintf(outputFile1, "%f ", matrix[lin]);
		}
		fprintf(outputFile1,"\n");
	}
	fprintf(outputFile1,"\n\n");

    fclose(outputFile1);
    return EXIT_SUCCESS;
}


static int processExtractPixelCoordOptions(char* cp,
		extractionParameters* extP) {
	while (*cp == ' ')
		cp++;
	if (sscanf(cp, UINT32_FORMAT "," UINT32_FORMAT ","
			UINT32_FORMAT "," UINT32_FORMAT,
			&(extP->requestedxmin), &(extP->requestedymin), &(extP->requestedwidth),
			&(extP->requestedlength)) != 4)
		return 0;
	return 1;
}

int main(int argc, char * argv[]) {

	GDALAllRegister();

	GeoTiffHandler myGeoTiffHandler;

	int arg = 1;
	int seen_extract_geometry_on_the_command_line = 0;

	extractionParameters extP;

	while (arg < argc && argv[arg][0] == '-') {	//so lange noch Argumente da sind, die mit '-' beginnen

		if (argv[arg][1] == 'v')
			extP.verbose = 1;
//		else if (argv[arg][1] == 'T') {
//			TIFFSetErrorHandler(stderrErrorHandler);
//			TIFFSetWarningHandler(stderrWarningHandler);
//			}
		else if (argv[arg][1] == 'E') {
			if (arg + 1 >= argc
					|| !processExtractPixelCoordOptions(argv[arg + 1], &extP)) {
				fprintf(stderr, "Syntax error in the "
						"specification of region to "
						"extract (option -E).\n");
				usage();
				return -1;
			}
			seen_extract_geometry_on_the_command_line = 1;
			arg++;
		} else if (argv[arg][1] == 'G') {
//			if (arg+1 >= argc ||
//			    !processExtractGeoCoordOptions(argv[arg+1], &extP)) {
//				fprintf(stderr, "Syntax error in the "
//					"specification of region to "
//					"extract (option -E).\n");
//				usage();
//				return -1;
//			}
//			seen_extract_geometry_on_the_command_line = 1;
//			arg++;
		} else {
			fprintf(stderr, "Unknown option \"%s\"\n", argv[arg]);
			usage();
			return -1;
		}
		arg++;
	}

//	if (argc > 1 && !seen_extract_geometry_on_the_command_line) {
//		fprintf(stderr, "The extract's position and size must be specified on the command line as argument to the '-E'  or the '-G' option. Aborting.\n");
//		return -1;
//	}

	datasetInfo info;

	if (myGeoTiffHandler.getDatasetInfo(&info) == SUCCESS) {
		cout << "This should have failed, as there is no file open yet"
				<< endl;
		return -1;
	}

	if (argc >= arg + 1) {

		if (myGeoTiffHandler.openGeoTiff(argv[arg]) != SUCCESS)
			return -1;

		if(myGeoTiffHandler.getDatasetInfo(&info) != SUCCESS) {
			cout << "This should have succeeded, as we tried to open a file" << endl;
			return -1;
		}

		cout<< info;

		resultType result;

		pixelCoord pix = {0,0};
		geoCoord geoTopLeft = myGeoTiffHandler.pixel2Geo(pix);
		cout << "Pixel "<<pix <<" in GeoCoordinates: " << geoTopLeft << endl;
		pix = myGeoTiffHandler.geo2Pixel(geoTopLeft);
		cout << "Geo Coordinates "<< geoTopLeft << "corresponds to Pixel "<< pix << endl;

		pix =  { 1000, 1000 };
		geoCoord geo = myGeoTiffHandler.pixel2Geo(pix);
		cout << "Pixel "<<pix <<" in GeoCoordinates: " << geo << endl;
		pix = myGeoTiffHandler.geo2Pixel(geo);
		cout << "Geo Coordinates " << geo << "corresponds to Pixel " << pix
				<< endl;

		pix = { info.extent.x, info.extent.y};
		geoCoord geoBottomRight = myGeoTiffHandler.pixel2Geo( pix);
		cout << "Pixel "<<pix <<" in GeoCoordinates: " << geoBottomRight << endl;
		pix = myGeoTiffHandler.geo2Pixel(geoBottomRight);
		cout << "Geo Coordinates " << geoBottomRight << "corresponds to Pixel " << pix
				<< endl;

		tilingCharacteristics myTilingChar;
		myGeoTiffHandler.getTilingInfo(geoTopLeft, geoBottomRight, 2000, MAX_SIZE, &myTilingChar);
		cout << "The file for " << geoTopLeft << " to " << geoBottomRight
				<< " will be tiled like this:\n" << myTilingChar;

		myGeoTiffHandler.getTilingInfo(geoTopLeft, geoBottomRight, 2000, 10e6, &myTilingChar);
		cout << "The file for " << geoTopLeft << " to " << geoBottomRight
				<< " will be tiled like this:\n" << myTilingChar;

		geoCoord geoTL={geoTopLeft.latitude+1, geoTopLeft.longitude-1}, geoBR = {geoBottomRight.latitude-1, geoBottomRight.longitude-1};

		result = myGeoTiffHandler.getTilingInfo(geoTL, geoBottomRight, 2000, 10e6, &myTilingChar);
		if(result == SUCCESS_NOT_ENTIRELY_COVERED) {cout << "Tiling data available, but requested area not entirely covered\n";}
		cout << "The file for " << geoTL << " to " << geoBottomRight
				<< " will be tiled like this:\n" << myTilingChar;

		result = myGeoTiffHandler.getTilingInfo(geoTopLeft,
				geoBR, 2000, 10e6, &myTilingChar);
		if(result == SUCCESS_NOT_ENTIRELY_COVERED) {cout << "Tiling data available, but requested area not entirely covered\n";}
		cout << "The file for " << geoTopLeft << " to " << geoBR
				<< " will be tiled like this:\n" << myTilingChar;


		geoBR = {geoBottomRight.latitude-1, geoBottomRight.longitude+1};
		result = myGeoTiffHandler.getTilingInfo(geoTopLeft,
				geoBR, 2000, 100e6, &myTilingChar);
		if(result == SUCCESS_NOT_ENTIRELY_COVERED) {cout << "Tiling data available, but requested area not entirely covered\n";}
		cout << "The file for " << geoTopLeft << " to " << geoBR
				<< " will be tiled like this:\n" << myTilingChar;

		for(int idxX=0; idxX< myTilingChar.tilesInX; idxX++){
			for(int idxY=0; idxY< myTilingChar.tilesInY; idxY++){
				tileData tile;
				myGeoTiffHandler.getTile(idxX, idxY, &tile);
				cout << tile;
				string matrixName= "tile_"+to_string(idxX)+"_"+to_string(idxY);
				string fileNameOut = matrixName+string(".m");
				printMatrixToFile(fileNameOut.c_str(), matrixName.c_str(),
						tile.width.x, tile.width.y, tile.buf);
				myGeoTiffHandler.releaseTile(idxX, idxY);
			}
		}

		//Flughafen Düsseldorf
		geoTL={51.3088, 6.7035}; geoBR = {51.2540, 6.8491};
		result = myGeoTiffHandler.getTilingInfo(geoTL,
				geoBR, 2000, 500e6, &myTilingChar);
		if(result == SUCCESS_NOT_ENTIRELY_COVERED) {cout << "Tiling data available, but requested area not entirely covered\n";}
		cout << "The file for " << geoTL << " to " << geoBR
				<< " will be tiled like this:\n" << myTilingChar;

		for(int idxX=0; idxX< myTilingChar.tilesInX; idxX++){
			for(int idxY=0; idxY< myTilingChar.tilesInY; idxY++){
				tileData tile;
				myGeoTiffHandler.getTile(idxX, idxY, &tile);
				cout << tile;
				string matrixName= "Duesseldorf_tile_"+to_string(idxX)+"_"+to_string(idxY);
				string fileNameOut = matrixName+string(".m");
				printMatrixToFile(fileNameOut.c_str(), matrixName.c_str(),
						tile.width.x, tile.width.y, tile.buf);
				myGeoTiffHandler.releaseTile(idxX, idxY);
			}
		}
		//Tagebau Hambach und Inden
		geoTL={50.9537, 6.3116}; geoBR = {50.8085, 6.6962};
		result = myGeoTiffHandler.getTilingInfo(geoTL,
				geoBR, 2000, 500e6, &myTilingChar);
		if(result == SUCCESS_NOT_ENTIRELY_COVERED) {cout << "Tiling data available, but requested area not entirely covered\n";}
		cout << "The file for " << geoTL << " to " << geoBR
				<< " will be tiled like this:\n" << myTilingChar;

		for(int idxX=0; idxX< myTilingChar.tilesInX; idxX++){
			for(int idxY=0; idxY< myTilingChar.tilesInY; idxY++){
				tileData tile;
				myGeoTiffHandler.getTile(idxX, idxY, &tile);
				cout << tile;
				string matrixName= "Hambach_Inden_tile_"+to_string(idxX)+"_"+to_string(idxY);
				string fileNameOut = matrixName+string(".m");
				printMatrixToFile(fileNameOut.c_str(), matrixName.c_str(),
						tile.width.x, tile.width.y, tile.buf);
				myGeoTiffHandler.releaseTile(idxX, idxY);
			}
		}

		//Flughafen Köln Bonn
		geoTL={50.8894, 7.1027}; geoBR = {50.8478, 7.1799};
		result = myGeoTiffHandler.getTilingInfo(geoTL,
				geoBR, 2000, 500e6, &myTilingChar);
		if(result == SUCCESS_NOT_ENTIRELY_COVERED) {cout << "Tiling data available, but requested area not entirely covered\n";}
		cout << "The file for " << geoTL << " to " << geoBR
				<< " will be tiled like this:\n" << myTilingChar;

		for(int idxX=0; idxX< myTilingChar.tilesInX; idxX++){
			for(int idxY=0; idxY< myTilingChar.tilesInY; idxY++){
				tileData tile;
				myGeoTiffHandler.getTile(idxX, idxY, &tile);
				cout << tile;
				string matrixName= "Koeln_Bonn_tile_"+to_string(idxX)+"_"+to_string(idxY);
				string fileNameOut = matrixName+string(".m");
				printMatrixToFile(fileNameOut.c_str(), matrixName.c_str(),
						tile.width.x, tile.width.y, tile.buf);
				myGeoTiffHandler.releaseTile(idxX, idxY);
			}
		}

		myGeoTiffHandler.closeGeoTiff();
	}
}
