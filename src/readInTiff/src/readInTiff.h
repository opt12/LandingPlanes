/*
 * readInTiff.h
 *
 *  Created on: 21.05.2017
 *      Author: eckstein
 */

#ifndef READINTIFF_H_
#define READINTIFF_H_

#include <tiff.h>

typedef struct {
	uint32 requestedxmin;
	uint32 requestedymin;
	uint32 requestedwidth;
	uint32 requestedlength;
	int verbose;
} extractionParameters;

typedef struct {
	uint16 spp;
	uint16 bitspersample;
	uint32 outwidth;
	uint32 outlength;
	unsigned int memsize;
	float* buf;
} tileCharacteristics;

#define EXIT_SYNTAX_ERROR        -1
#define EXIT_IO_ERROR            -2
#define EXIT_UNHANDLED_FILE_TYPE -3
#define EXIT_INSUFFICIENT_MEMORY -4
#define EXIT_UNABLE_TO_ACHIEVE_TILE_DIMENSIONS -5
#define EXIT_GEOMETRY_ERROR	-6

int getImageInformation(tileCharacteristics* info, const char * infilename);
int makeExtractFromTIFFFile(const extractionParameters p, tileCharacteristics* tile, const char * infilename);


#endif /* READINTIFF_H_ */
