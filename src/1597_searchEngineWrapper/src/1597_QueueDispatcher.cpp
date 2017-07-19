/*
 * 1597_dispatcher.cpp
 *
 *  Created on: 12.07.2017
 *      Author: eckstein
 */

#include "1597_QueueDispatcher.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <string>

//TODO: Das muss in einen eigenen thread und in eine eigene Datei ausgelagert werden
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

#include "json.hpp"

// for convenience
using json = nlohmann::json;

using namespace std;

//these are the "file globals" for maintenance of the Task queue
static pthread_mutex_t *taskQueueMutex;
static pthread_cond_t *taskQueueConditional;
static std::queue<tsocketMessage> *taskQueue;

static eResult scanForLandingPlanes(json taskDescription) {
	cout << "scanning for Landing Planes now" << endl;
	return NORMAL_EXIT;
}

static eResult saveDataToMFile(json taskDescription) {
	//TODO: Das muss in einen eigenen thread und in eine eigene Datei ausgelagert werden
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

void emitReceiptMsg(int connectedSock, string msgType, json msgJSON) {
	json j = { { "type", msgType }, { "data", msgJSON } };
	//XXX The node.js module needs a "\f" at the end of the serialized JSON-Message :-)
	string msgString = j.dump() + "\f";
	unsigned int msgSize = msgString.length();
	for (unsigned int bytesTransferred = 0; bytesTransferred < msgSize;) {
		bytesTransferred += write(connectedSock, msgString.c_str(), msgSize);
	}
}

eResult Dispatcher(void) {
	tTask currentTask;

	do {
		// safely examine the condition, prevent other threads from
		// altering it
		pthread_mutex_lock(taskQueueMutex);
		while (taskQueue->empty()) {
			cout
					<< "Dispatcher Thread: Empty Task queue; Going to have a little nap..."
					<< endl;
			pthread_cond_wait(taskQueueConditional, taskQueueMutex);
		}

		// Do whatever you need to do when condition becomes true
		tsocketMessage queueHead = taskQueue->front();
		taskQueue->pop();

		const int commSocket = queueHead.connectedSocket;
		currentTask= {convertTaskType(queueHead.msg["type"].get<std::string>()), queueHead.msg["data"]};

		pthread_mutex_unlock(taskQueueMutex);
		cout << "Dispatcher Thread: popped task:" << endl;
		cout << "taskType: " << currentTask.taskType << endl;
		cout << "taskJSON: " << currentTask.taskDescription.dump(4) << endl;

		switch (currentTask.taskType) {
		case SCAN:
			cout
					<< "Dispatcher Thread: dispatching task to scanForLandingPlanes()"
					<< endl;
			cout << currentTask.taskDescription.dump(4) << endl;
			scanForLandingPlanes(currentTask.taskDescription);
			//TODO change message type
			emitReceiptMsg(commSocket, "message", currentTask.taskDescription);
			break;
		case SAVE_2_M_FILE:
			cout << "Dispatcher Thread: dispatching task to saveDataToMFile()"
					<< endl;
			cout << currentTask.taskDescription.dump(4) << endl;
			saveDataToMFile(currentTask.taskDescription);
			//TODO change message type
			emitReceiptMsg(commSocket, "message", currentTask.taskDescription);
			break;
		default:
			cout << R"(Dispatcher Thread: ignoring unknown taskType: )"
					<< queueHead.msg["type"] << endl;
			cout << currentTask.taskDescription.dump(4) << endl;
			break;
		}

	} while (currentTask.taskType != QUIT);

	return NORMAL_EXIT;
}

void *QueueDispatcherThreadstarter(void *par) {
	tQueueDispatcherThreadParam p = *((tQueueDispatcherThreadParam *) par);

	//stow away the maintenance data to avoid passing them around over and over
	taskQueueMutex = p.taskQueueMutex;
	taskQueueConditional = p.taskQueueConditional;
	taskQueue = p.taskQueue;

	cout << "Dispatcher Thread: started" << endl;

	//register the GDAL parsers for files...
	GDALAllRegister();

	//now start the worker
	p.taskResult = Dispatcher();

	cout << "IPC Listener: I'm out" << endl;

	p.taskResult = NORMAL_EXIT;

	return NULL;
}

