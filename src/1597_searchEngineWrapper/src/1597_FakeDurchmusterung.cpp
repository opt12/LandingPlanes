/*
 * 1597_FakeDurchmusterung.cpp
 *
 *  Created on: 21.07.2017
 *      Author: eckstein
 */

#include <unistd.h>	//to get the number of CPUs
#include <vector>

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
		GeoTiffHandler *myGeoTiffHandler, float heading, float minLength,
		float width) {
	const int startX = actualTile->offset.x
			+ (rand() % static_cast<int>(actualTile->width.x + 1));
	const int startY = actualTile->offset.y
			+ (rand() % static_cast<int>(actualTile->width.y + 1));
	const float MIN_RAND = minLength, MAX_RAND = 2 * minLength;
	const float range = MAX_RAND - MIN_RAND;
	const float length = range * ((((float) rand()) / (float) RAND_MAX))
			+ MIN_RAND;

//	const float length= minLength;	//XXX TODO just for debug

	json j = myGeoTiffHandler->getGeoJsonPolygon( { (float) startX,
			(float) startY }, length, heading, width);
	j["properties"]["actualLength"] = length;
	//TODO und hier müssen dann auch die aktuellen Parameter für "actualVariance" und "actualRise" eingetragen werden

	return j;
}

void *createFakePlaneThreadStarter(void *par) {
	tcreateFakePlaneStarterParam p = *((tcreateFakePlaneStarterParam *) par);

	cout << "Fake Plane Thread [" << p.threadNum << "] started." << endl;

	//now start the worker
	json fakePlane = createFakeLandingPlane(p.actualTile, p.myGeoTiffHandler,
			p.actualHeading,
			(*p.taskDescription)["scanParameters"]["minLength"],
			(*p.taskDescription)["scanParameters"]["minWidth"]);

	float lengthFromJson = fakePlane["properties"]["actualLength"];	//TODO das müsste schöner gehen: Erst auspacken, dann ersetzten, dann wieder reinstecken ist blöd
	//TODO und hier müssen dann auch die aktuellen Parameter für "actualVariance" und "actualRise" eingetragen werden
	fakePlane["properties"] = (*p.taskDescription)["scanParameters"];
	fakePlane["properties"]["actualLength"] = lengthFromJson;
	fakePlane["properties"]["actualRise"] = 666.666;
	fakePlane["properties"]["actualVariance"] = 666.666;
	fakePlane["properties"]["actualHeading"] = p.actualHeading;

	cout << "SCANNING: created fake plane: " << endl;
	cout << fakePlane.dump(4) << endl;

	emitReceiptMsg(p.commSocket, "landingPlane", fakePlane);

	p.taskResult = NORMAL_EXIT;

	return NULL;
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

	//Das hier ist eine Funktion, die _SC_NPROCESSORS_ONLN Threads aufspannt.
	//Jeder der Threads gibt eine zufällige Ladebahn im Scan-Bereich zurück und meldet sie an
	//den Unix Socket
	const int NUM_THREADS = sysconf(_SC_NPROCESSORS_ONLN);

	std::vector<pthread_t> scanThreads(NUM_THREADS);

	std::vector<tcreateFakePlaneStarterParam> createFakePlaneStarterParams(
			NUM_THREADS);

	for (int t = 0; t < NUM_THREADS; t++) {
		createFakePlaneStarterParams[t].actualTile = actualTile;
		createFakePlaneStarterParams[t].actualHeading = actualHeading;
		createFakePlaneStarterParams[t].commSocket = commSocket;
		createFakePlaneStarterParams[t].myGeoTiffHandler = myGeoTiffHandler;
		createFakePlaneStarterParams[t].taskDescription = taskDescription;
		createFakePlaneStarterParams[t].threadNum = t;

		pthread_create(&scanThreads[t], NULL, createFakePlaneThreadStarter,
				&createFakePlaneStarterParams[t]);
	}

	cout << "Fake Scan: generated " << NUM_THREADS
			<< " threads to do a Fake scan" << endl;

	for (int t = 0; t < NUM_THREADS; t++) {
		pthread_join(scanThreads[t], NULL);
		cout << "Fake Scan thread "<< t <<" just returned"<< endl;
	}


	return NORMAL_EXIT;
}

