/*
 * 1597_FakeDurchmusterung.cpp
 *
 *  Created on: 21.07.2017
 *      Author: eckstein
 */

#include "1597_FakeDurchmusterung.h"
#include "geojson_utils.h"
#include "1597_searcherTask.h"

#include "1597_ipc_listener.h"
#include <string>

#include "json.hpp"

// for convenience
using json = nlohmann::json;

#include "../../GeoTiff_Handler/src/GeoTiffHandler.h"

static json createFakeLandingPlane(const tileData *actualTile,
		GeoTiffHandler *myGeoTiffHandler, float heading, float minLength, float width) {
	const int startX = actualTile->offset.x+ (rand() % static_cast<int>(actualTile->width.x + 1));
	const int startY = actualTile->offset.y+ (rand() % static_cast<int>(actualTile->width.y + 1));
	const float MIN_RAND = minLength, MAX_RAND = 2*minLength;
	const float range = MAX_RAND - MIN_RAND;
	const float length = range * ((((float) rand()) / (float) RAND_MAX)) + MIN_RAND ;

//	const float length= minLength;	//XXX TODO just for debug

	json j =myGeoTiffHandler->getGeoJsonPolygon({(float) startX, (float) startY},length, heading, width);
	j["properties"]["actualLength"] = length;
	//TODO und hier müssen dann auch die aktuellen Parameter für "actualVariance" und "actualRise" eingetragen werden

	return j;
}

eResult fakeScan(const tileData *actualTile, GeoTiffHandler *myGeoTiffHandler,
		const json *taskDescription, float actualHeading, int commSocket) {

	/*TODO: Es ist sehr unschön, dass ich den GeoTiffHandler myGeoTiffHandler mit übergeben muss
	 * nur um dort die Funktionen
	 * 	geoCoord pixel2Geo(const pixelCoordFloat source);
	 * 	json getGeoJsonPolygon(const pixelCoordFloat start, const pixelCoordFloat end, const float width);
	 *  json getGeoJsonPolygon(const pixelCoordFloat start, const float length, const float heading, const float width);
	 *  json getGeoJsonPolygon(const pixelCoordFloat pix0, const pixelCoordFloat pix1, const pixelCoordFloat pix2, const pixelCoordFloat pix3);
	 * aufzurufen. Dazu sollte es ausreichen, dass ich die tileData actualTile bekomme.
	 *
	 * Evtl. muss ich da noch eine eigene Klasse draus bauen.
	 */

	//Das hier ist eine funktion, die 4 Threads aufspannt.
	//Jeder der threads gibt eine zufällige Ladebahn im Scan-Bereich zurück und meldet sie an
	//den Unix Socket
	json fakePlane = createFakeLandingPlane(actualTile, myGeoTiffHandler,
			actualHeading, (*taskDescription)["scanParameters"]["minLength"],
			(*taskDescription)["scanParameters"]["minWidth"]);

	float lengthFromJson = fakePlane["properties"]["actualLength"];	//TODO das müsste schöner gehen: Erst auspacken, dann ersetzten, dann wieder reinstecken ist blöd
	//TODO und hier müssen dann auch die aktuellen Parameter für "actualVariance" und "actualRise" eingetragen werden
	fakePlane["properties"] = (*taskDescription)["scanParameters"];
	fakePlane["properties"]["actualLength"] = lengthFromJson;
	fakePlane["properties"]["actualRise"] = 666.666;
	fakePlane["properties"]["actualVariance"] = 666.666;
	fakePlane["properties"]["actualHeading"] = actualHeading;

	cout << "SCANNING: created fake plane: " << endl;
	cout << fakePlane.dump(4)<< endl;



	emitReceiptMsg(commSocket, "landingPlane", fakePlane);


	return NORMAL_EXIT;
}

