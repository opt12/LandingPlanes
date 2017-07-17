//============================================================================
// Name        : 1597_searchEngineWrapper.cpp
// Author      : Felix Eckstein; Matr-# 8161569
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <queue>

#include "1597_ipc_listener.h"
#include "1597_QueueDispatcher.h"

using namespace std;

int main(int argc, char *argv[]) {
	//TODO check for alternative socket path in argv

	static pthread_mutex_t taskQueueMutex;
	static pthread_cond_t taskQueueConditional;
	static queue<json> taskQueue;

	pthread_mutex_init(&taskQueueMutex, NULL);
	pthread_cond_init(&taskQueueConditional, NULL);

	pthread_t ipcListener, queueDispatcher;

	tIpcListenerThreadParam IpcListenerThreadPar;
	IpcListenerThreadPar.taskQueueMutex = &taskQueueMutex;
	IpcListenerThreadPar.taskQueueConditional = &taskQueueConditional;
	IpcListenerThreadPar.taskQueue = &taskQueue;
	IpcListenerThreadPar.alternatePath = "";

	pthread_create(&ipcListener, NULL, IPCListenerThreadStarter,
			&IpcListenerThreadPar);

	tQueueDispatcherThreadParam QueueDispatcherThreadPar;
	QueueDispatcherThreadPar.taskQueueMutex = &taskQueueMutex;
	QueueDispatcherThreadPar.taskQueueConditional = &taskQueueConditional;
	QueueDispatcherThreadPar.taskQueue = &taskQueue;

	pthread_create(&queueDispatcher, NULL, QueueDispatcherThreadstarter, &QueueDispatcherThreadPar);

	while (pthread_tryjoin_np(ipcListener, NULL)) {
		sleep(5);
		cout << "Main Thread: going to sleep again" << endl;
	}

	cout << "Main thread: IPC Listener finished with result code "
			<< IpcListenerThreadPar.taskResult << "." << endl;

	return 0;
}
