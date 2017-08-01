/*
 * 1597_dispatcher.cpp
 *
 *  Created on: 12.07.2017
 *      Author: eckstein
 */

#include "1597_QueueDispatcher.h"
#include "1597_ipc_listener.h"	//TODO for the emitReceiptMsg(); sollte in andere Datei;
#include "1597_scanToMFile.h"
#include "1597_searcherTask.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <string>

#include <gdal.h>

#include "json.hpp"

// for convenience
using json = nlohmann::json;

using namespace std;

//these are the "file globals" for maintenance of the Task queue
static pthread_mutex_t *taskQueueMutex;
static pthread_cond_t *taskQueueConditional;
static std::queue<tsocketMessage> *taskQueue;


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
		case SCAN:{
			cout
					<< "Dispatcher Thread: dispatching task to scanForLandingPlanes()"
					<< endl;
			cout << currentTask.taskDescription.dump(4) << endl;

			tSearchTaskThreadStarterParam searchTaskThreadStarterPar;
			searchTaskThreadStarterPar.taskDescription =
					currentTask.taskDescription;
			searchTaskThreadStarterPar.commSocket = commSocket;

			pthread_t newScanTask;
			pthread_create(&newScanTask, NULL, searchTaskThreadStarter,
					&searchTaskThreadStarterPar);
			pthread_join(newScanTask, NULL);//we only want to dispatch one task at a time;
			//the tasks itself may spawn more threads if needed;
			break;
		}
		case SAVE_2_M_FILE: {
			cout << "Dispatcher Thread: dispatching task to saveDataToMFile()"
					<< endl;
			cout << currentTask.taskDescription.dump(4) << endl;

			tScanToMFileThreadStarterParam scanToMFileThreadStarterPar;
			scanToMFileThreadStarterPar.taskDescription =
					currentTask.taskDescription;
			scanToMFileThreadStarterPar.commSocket = commSocket;

			pthread_t newScanTask;
			pthread_create(&newScanTask, NULL, scanToMFileThreadStarter,
					&scanToMFileThreadStarterPar);
			pthread_join(newScanTask, NULL);//we only want to dispatch one task at a time;
			//the tasks itself may spawn more threads if needed;
			break;
		}
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

