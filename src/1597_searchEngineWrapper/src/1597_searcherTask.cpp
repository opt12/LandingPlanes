/*
 * 1597_searcherTask.cpp
 *
 *  Created on: 21.07.2017
 *      Author: eckstein
 */

#include "1597_FakeDurchmusterung.h"

#include "1597_searcherTask.h"

#include "1597_ipc_listener.h"
#include <string>


#include "json.hpp"

// for convenience
using json = nlohmann::json;

#include "GeoTiffHandler.h"
#include "readInTiff.h"
#include "config.h"	//TODO: Das ist unschön, wenn wir nicht auch autoconfig und autoheaders benutzen wollen.

static eResult scanForLandingPlanes(json taskDescription, int commSocket) {
	cout << "SCAN Thread: scanning for Landing Planes" << endl;

	GeoTiffHandler myGeoTiffHandler;

	datasetInfo info;
	string filename = taskDescription["TiffFileName"].get<std::string>();
	if (myGeoTiffHandler.openGeoTiff(filename.c_str()) != SUCCESS)
		return GEO_TIFF_READ_ERROR;

	if (myGeoTiffHandler.getDatasetInfo(&info) != SUCCESS) {
		cout << "SCAN Thread: This should have succeeded, as we tried to open a file"
				<< endl;
		return GEO_TIFF_READ_ERROR;
	}

	cout << "SCAN Thread: Just opened the GeoTIFF file with this info params:" << endl;
	cout << info << endl;

	resultType result;
	geoCoord geoTopLeft = { taskDescription["mapExtent"]["topLeftLat"],
			taskDescription["mapExtent"]["topLeftLng"] };
	geoCoord geoBotRight = { taskDescription["mapExtent"]["botRightLat"],
			taskDescription["mapExtent"]["botRightLng"] };

	float minLength = taskDescription["scanParameters"]["minLength"];
	vector<float> headings = taskDescription["scanHeadings"];


	tilingCharacteristics myTilingChar;
	myGeoTiffHandler.getTilingInfo(geoTopLeft, geoBotRight, minLength,
			MAX_SIZE,
			&myTilingChar);
	cout << "SCAN Thread: The file for " << geoTopLeft << " to " << geoBotRight
			<< " will be tiled like this:\n" << myTilingChar;

	for (int idxX = 0; idxX < myTilingChar.tilesInX; idxX++) {
		for (int idxY = 0; idxY < myTilingChar.tilesInY; idxY++) {
			tileData tile;
			myGeoTiffHandler.getTile(idxX, idxY, &tile);
			cout << "SCAN Thread: just got data for tile ["<<idxX<<"]["<<idxY<<"];"<<endl;
			for (const float actualHeading : headings){
				cout << "SCAN Thread: scanning for heading " << actualHeading << "..." << endl;

				cout << "SCAN Thread: Scan Parameters are: " << taskDescription["scanParameters"].dump(4)<<endl;

				fakeScan(&tile, &myGeoTiffHandler,
						&taskDescription, actualHeading, commSocket,info.noDataValue,info.pixelSize  );
				/********************************************************************
				 * //TODO spawn threads for scanning exactly here!!!
				 ********************************************************************/
				cout << "SCAN Thread: scanning for heading " << actualHeading << " is finshed" << endl;
			}
			myGeoTiffHandler.releaseTile(idxX, idxY);
		}
	}

	myGeoTiffHandler.closeGeoTiff();

	return NORMAL_EXIT;
}

void *searchTaskThreadStarter(void *par) {
	tSearchTaskThreadStarterParam p = *((tSearchTaskThreadStarterParam *) par);

	cout << "SCAN Thread: started" << endl;

	//now start the worker
	p.taskResult = scanForLandingPlanes(p.taskDescription, p.commSocket);
	json j;
	switch (p.taskResult) {
	case SUCCESS:
		cout << "SCAN Thread: scanning finished successfully" << endl;
		j= {
				{"type", "SCAN"},
				{"finished", true},
				{"text", "success"},
				{"parameter", p.taskDescription}
		};
		break;
	case SUCCESS_NOT_ENTIRELY_COVERED:
		cout << "SCAN Thread: scanning finished successfully, but not the whole area was covered entirely" << endl;
		j= {
				{"type", "SCAN"},
				{"finished", true},
				{"text", "area not entirely covered"},
				{"parameter", p.taskDescription}
		};
		break;
	case GEO_TIFF_READ_ERROR:
		cout << "SCAN Thread: GEO_TIFF_READ_ERROR occurred (maybe the altitude is in the wrong format (must be FLOAT32)" << endl;
		j= {
				{"type", "SCAN"},
				{"finished", false},
				{"text", "GEO_TIFF_READ_ERROR occurred"},
				{"parameter", p.taskDescription}
		};
		break;
		default:
			cout << "SCAN Thread: some error occurred" << endl;
			j= {
					{"type", "SCAN"},
					{"finished", false},
					{"text", "some unspecified error occurred"},
					{"parameter", p.taskDescription}
			};
			break;

			break;
	}
	emitReceiptMsg(p.commSocket, "taskReceipt", j);

	p.taskResult = NORMAL_EXIT;

	return NULL;
}


