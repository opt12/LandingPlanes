/*
 * testReadInTiff.c
 *
 *  Created on: 21.05.2017
 *      Author: eckstein
 */

#include <stdio.h>
#include <stdlib.h>
#include <tiff.h>
#include <tiffio.h>


#include "readInTiff.h"
#include "gdalWrapper.h"

#include "config.h"	//TODO: Das ist unschön, wenn wir nicht auch autoconfig und autoheaders benutzen wollen.


#define PACKAGE_VERSION "0.1"

static void usage()
{
	fprintf(stderr, "testReadInTiff v" PACKAGE_VERSION " license GNU GPL v3 (c) 2013-2016 Christophe Deroulers\n\n");
	fprintf(stderr, "Quote \"Deroulers et al., Diagnostic Pathology 2013, 8:92\" in your production\n       http://doi.org/10.1186/1746-1596-8-92\n\n");
	fprintf(stderr, "Usage: tifffastcrop [options] input.tif\n\n");
	fprintf(stderr, " Extracts (crops), without loading the full image input.tif into memory, a\nrectangular region from it, and saves it. Output file name is output_name if\ngiven, otherwise a name derived from input.tif. Output file format is guessed\nfrom output_name's extension if possible. Options:\n");
	fprintf(stderr, " -v                verbose monitoring\n");
	fprintf(stderr, " -T                report TIFF errors/warnings on stderr (no dialog boxes)\n");
	fprintf(stderr, " -E x,y,w,l        region to extract/crop (x,y: coordinates of top left corner,\n");
	fprintf(stderr, "   w,l: width and length in pixels)\n");
}

static void stderrErrorHandler(const char* module, const char* fmt, va_list ap)
{
	if (module != NULL)
		fprintf(stderr, "%s: ", module);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, ".\n");
}


static void stderrWarningHandler(const char* module, const char* fmt, va_list ap)
{	//we assume verbose == true holds for the purpose of testing
	if (module != NULL)
		fprintf(stderr, "%s: ", module);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, ".\n");
}

static int processExtractGeometryOptions(char* cp, extractionParameters* extP)
{
	while (*cp == ' ')
		cp++;
	if (sscanf(cp, UINT32_FORMAT "," UINT32_FORMAT ","
	    UINT32_FORMAT "," UINT32_FORMAT,
	    &(extP->requestedxmin), &(extP->requestedymin), &(extP->requestedwidth),
	    &(extP->requestedlength)) != 4)
		return 0;
	return 1;
}

/**
 * Prints out a matrix into a file. The format used is compatible with GNU Octave/Matlab, so that the
 * file can be easily read into these systems for verification.
 * The tiling of the matrix is considered and the original element ordering is restored while
 * writing the file.
 *
 * The file can be read into GNU Octave/MATLAB using the load(filename) command.
 */

int printMatrixToFile(const char* filename, const char* matrixName, const int tileSizeX, const int tileSizeY, const int tilesX, const int tilesY, float *matrix){
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
			matrixName, tileSizeY*tilesY, tileSizeX*tilesX);

	for(int y=0; y < tilesY*tileSizeY; y++){
		for(int x= 0; x< tilesX*tileSizeX; x++){
    		const int lin = y*tilesX*tileSizeX+x;
			fprintf(outputFile1, "%f ", matrix[lin]);
		}
		fprintf(outputFile1,"\n");
	}
	fprintf(outputFile1,"\n\n");

    fclose(outputFile1);
    return EXIT_SUCCESS;
}

int main(int argc, char * argv[])
{
	int arg = 1;
	int seen_extract_geometry_on_the_command_line = 0;

	extractionParameters extP;

	while (arg < argc && argv[arg][0] == '-') {	//so lange noch Argumente da sind, die mit '-' beginnen

		if (argv[arg][1] == 'v')
			extP.verbose = 1;
		else if (argv[arg][1] == 'T') {
			TIFFSetErrorHandler(stderrErrorHandler);
			TIFFSetWarningHandler(stderrWarningHandler);
			}
		else if (argv[arg][1] == 'E') {
			if (arg+1 >= argc ||
			    !processExtractGeometryOptions(argv[arg+1], &extP)) {
				fprintf(stderr, "Syntax error in the "
					"specification of region to "
					"extract (option -E).\n");
				usage();
				return EXIT_SYNTAX_ERROR;
			}
			seen_extract_geometry_on_the_command_line = 1;
			arg++;
		} else {
			fprintf(stderr, "Unknown option \"%s\"\n", argv[arg]);
			usage();
			return EXIT_SYNTAX_ERROR;
		}
	arg++;
	}

	if (argc > 1 && !seen_extract_geometry_on_the_command_line) {
		fprintf(stderr, "The extract's position and size must be specified on the command line as argument to the '-E' option. Aborting.\n");
		return EXIT_GEOMETRY_ERROR;
	}

	if (argc >= arg+1) {
		tileCharacteristics info, tile;
		getImageInformation(&info,  argv[arg]);

		printf("file %s\n has these characteristics:\n"
				"Width: %d; Height: %d;\n"
				"Samples per Pixel: %d; Bits per Sample: %d;\n"
				,argv[arg], info.outwidth, info.outlength, info.spp, info.bitspersample);


		makeExtractFromTIFFFile(extP, &tile, argv[arg]);

		printf("extracted tile form file %s\n"
				"Width: %d; Height: %d;\n"
				"Samples per Pixel: %d; Bits per Sample: %d;\n"
				"Buffer with %ld Bytes allocated in Memory\n"
				,argv[arg], tile.outwidth, tile.outlength, tile.spp, tile.bitspersample, tile.memsize);

		printf("\n******************************************\n");
		printf("Geo-Information obtained using the GDAL helper functions;\n");
		printGeoDataInfo(argv[arg]);


	if (argc >= arg+2) {
		printMatrixToFile(argv[arg+1], "Tile", tile.outwidth, tile.outlength, 1, 1, tile.buf);
	}

		free(tile.buf);
	} else {
		usage();
		return EXIT_SYNTAX_ERROR;
	}

}
