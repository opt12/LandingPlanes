/*
 * 1597_Fakedurchmusterung.h
 *
 *  Created on: 21.07.2017
 *      Author: eckstein
 */

#ifndef FAKEDURCHMUSTERUNG_H_
#define FAKEDURCHMUSTERUNG_H_

#include "1597_Task_Definition.h"
#include "../../GeoTiff_Handler/src/GeoTiffHandler.h"

struct tcreateFakePlaneStarterParam {
	const tileData *actualTile;
	GeoTiffHandler *myGeoTiffHandler;
	const json *taskDescription;
	float actualHeading;
	int commSocket;
	int threadNum;
	eResult taskResult;
        float noDataValue;
       rectSize pixelSize;
};

eResult fakeScan(const tileData *actualTile, GeoTiffHandler *myGeoTiffHandler,
		const json *taskDescription, float actualHeading, int commSocket,float noDataValue, rectSize pixelSize );



#endif /* FAKEDURCHMUSTERUNG_H_ */
