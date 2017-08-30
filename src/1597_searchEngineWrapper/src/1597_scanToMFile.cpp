/*
 * 1597_scanToMFile.cpp
 *
 *  Created on: 19.07.2017
 *      Author: eckstein
 */

#include "1597_scanToMFile.h"
#include "1597_ipc_listener.h"
#include <string>


#include "json.hpp"

// for convenience
using json = nlohmann::json;

#include "../../GeoTiff_Handler/src/GeoTiffHandler.h"
#include "../../GeoTiff_Handler/src/readInTiff.h"
#include "../../GeoTiff_Handler/src/config.h"	//TODO: Das ist unschön, wenn wir nicht auch autoconfig und autoheaders benutzen wollen.

int printMatrixToFile(const char* filename, const char* matrixName,
		float NaNValue, const int tileSizeX, const int tileSizeY,
		float *matrix) {
	FILE* outputFile1 = fopen(filename, "w");
	if (!outputFile1) {
		fprintf(stderr, "Could not open file %s for writing\n", filename);
		return 1;
	}

	fprintf(outputFile1, "# Created by testReadInTiff.c\n"
			"# name: %s\n"
			"# type: matrix\n"
			"# rows: %d\n"
			"# columns: %d\n", matrixName, tileSizeY, tileSizeX);

	for (int y = 0; y < tileSizeY; y++) {
		for (int x = 0; x < tileSizeX; x++) {
			const int lin = y * tileSizeX + x;
			fprintf(outputFile1, "%f ",
					matrix[lin] == NaNValue ?
							std::numeric_limits<double>::quiet_NaN() :
							matrix[lin]);
		}
		fprintf(outputFile1, "\n");
	}
	fprintf(outputFile1, "\n\n");

	fclose(outputFile1);
	return EXIT_SUCCESS;
}

static eResult saveDataToMFile(json taskDescription) {
	cout << "converting data to *.m file" << endl;

	GeoTiffHandler myGeoTiffHandler;

	datasetInfo info;
	string filename = taskDescription["TiffFileName"].get<std::string>();
	if (myGeoTiffHandler.openGeoTiff(filename.c_str()) != SUCCESS)
		return GEO_TIFF_READ_ERROR;

	if (myGeoTiffHandler.getDatasetInfo(&info) != SUCCESS) {
		cout << "This should have succeeded, as we tried to open a file"
				<< endl;
		return GEO_TIFF_READ_ERROR;
	}

	cout << info;

	resultType result;
	cout << taskDescription["mapExtent"] << endl;
	geoCoord geoTopLeft = { taskDescription["mapExtent"]["topLeftLat"],
			taskDescription["mapExtent"]["topLeftLng"] };
	geoCoord geoBotRight = { taskDescription["mapExtent"]["botRightLat"],
			taskDescription["mapExtent"]["botRightLng"] };

	tilingCharacteristics myTilingChar;
	myGeoTiffHandler.getTilingInfo(geoTopLeft, geoBotRight, 2000, MAX_SIZE,
			&myTilingChar);
	cout << "The file for " << geoTopLeft << " to " << geoBotRight
			<< " will be tiled like this:\n" << myTilingChar;

	for (int idxX = 0; idxX < myTilingChar.tilesInX; idxX++) {
		for (int idxY = 0; idxY < myTilingChar.tilesInY; idxY++) {
			tileData tile;
			myGeoTiffHandler.getTile(idxX, idxY, &tile);
			cout << tile;
			string mObjName= taskDescription["mFile"]["mFileObjName"].get<std::string>();
			string fileNameOut =
					taskDescription["mFile"]["mFilePath"].get<std::string>()+
							mObjName+
					taskDescription["mFile"]["mFileExtension"].get<std::string>();
			printMatrixToFile(fileNameOut.c_str(), mObjName.c_str(),
					info.noDataValue, tile.width.x, tile.width.y, tile.buf);
			myGeoTiffHandler.releaseTile(idxX, idxY);
		}
	}

	myGeoTiffHandler.closeGeoTiff();

	return NORMAL_EXIT;
}


void *scanToMFileThreadStarter(void *par) {
	tScanToMFileThreadStarterParam p = *((tScanToMFileThreadStarterParam *) par);

	cout << "Scan2M Thread: started" << endl;
	//now start the worker
	p.taskResult = saveDataToMFile(p.taskDescription);

	json j;
	switch (p.taskResult) {
	case SUCCESS:
		cout << "Scan2M Thread: scanning finished successfully" << endl;
		j= {
				{"type", "SAVE_2_M_FILE"},
				{"finished", true},
				{"text", "success"},
				{"parameter", p.taskDescription}
		};
		break;
	case SUCCESS_NOT_ENTIRELY_COVERED:
		cout << "Scan2M Thread: scanning finished successfully, but not the whole area was covered entirely" << endl;
		j= {
				{"type", "SAVE_2_M_FILE"},
				{"finished", true},
				{"text", "area not entirely covered"},
				{"parameter", p.taskDescription}
		};
		break;
	case GEO_TIFF_READ_ERROR:
		cout << "Scan2M Thread: GEO_TIFF_READ_ERROR occurred (maybe the altitude is in the wrong format (must be FLOAT32)" << endl;
		j= {
				{"type", "SAVE_2_M_FILE"},
				{"finished", false},
				{"text", "GEO_TIFF_READ_ERROR occurred"},
				{"parameter", p.taskDescription}
		};
		break;
		default:
			cout << "Scan2M Thread: some error occurred" << endl;
			j= {
					{"type", "SAVE_2_M_FILE"},
					{"finished", false},
					{"text", "some unspecified error occurred"},
					{"parameter", p.taskDescription}
			};
			break;

			break;
	}

	emitReceiptMsg(p.commSocket, "taskReceipt", j);

	extern string getGeoJsonString();

	cout << "Test-GeoJSON as String: "<<endl  << getGeoJsonString()<< endl;

	p.taskResult = NORMAL_EXIT;

	return NULL;
}



