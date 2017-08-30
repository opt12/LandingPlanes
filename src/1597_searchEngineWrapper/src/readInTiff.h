/*
 * readInTiff.h
 *
 *  Created on: 21.05.2017
 *      Author: eckstein
 */

#ifndef READINTIFF_H_
#define READINTIFF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <tiff.h>
#include "errorCodes.h"

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
	size_t memsize;
	float* buf;
} tileCharacteristics;

int getImageInformation(tileCharacteristics* info, const char * infilename);
int makeExtractFromTIFFFile(const extractionParameters p,
		tileCharacteristics* tile, const char * infilename);

#ifdef __cplusplus
}
#endif
#endif /* READINTIFF_H_ */
