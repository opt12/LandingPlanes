/*
 * 1597_ipc_listener.cpp
 *
 *  Created on: 12.07.2017
 *      Author: eckstein
 */

#include "1597_ipc_listener.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <string>

#include "json.hpp"

// for convenience
using json = nlohmann::json;

using namespace std;

// the default socket for communicating via ipc
static string socket_path = "/tmp/1597-ipc";

//these are the "file globals" for maintenance of the Task queue
static pthread_mutex_t *taskQueueMutex;
static pthread_cond_t *taskQueueConditional;
static std::queue<tsocketMessage> *taskQueue;

static eResult IpcListener(string alternatePath) {

	struct sockaddr_un addr;
	char buf[1024];

	json receivedJson;

	int socketDescriptor, connectedSocket, readCount;

	if ((socketDescriptor = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		return SOCKET_ERROR;
	}

	if (alternatePath.length() > 0)
		socket_path = (alternatePath);
	if (socket_path.length() > 91) {
		perror(
				"pathname too long for some implementations (limit is 92 including \\0 at the end");
		return PATHNAME_ERROR;
	}
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, socket_path.c_str(), socket_path.length());
	unlink(socket_path.c_str());//this is adapted from an example form https://stackoverflow.com/a/39848936

	if (bind(socketDescriptor, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
		perror("bind error");
		return BIND_ERROR;
	}

	cout << "Listener Thread: bound ipc to \"" << addr.sun_path << endl;

	if (listen(socketDescriptor, 5) == -1) {
		perror("listen error");
		return LISTEN_ERROR;
	}

	cout << "IPC Listener: Waiting for data: " << endl;

	while (1) {
		if ((connectedSocket = accept(socketDescriptor, NULL, NULL)) == -1) {
			perror("accept error");
			continue;
		}

		while ((readCount = read(connectedSocket, buf, sizeof(buf))) > 0) {
			string receivedMsg = string(buf, readCount - 1);
			//obviously, node adds the \0 at the end of the string which is then transferred via IPC
			cout << "read " << readCount << " bytes: " << receivedMsg << endl;

			receivedJson = json::parse(receivedMsg);
			cout << receivedJson.dump(4) << endl;

			// ensure we have exclusive access to whathever comprises the condition
			pthread_mutex_lock(taskQueueMutex);
			taskQueue->push({connectedSocket, receivedJson});
			// Wakeup at least one of the threads that are waiting on the condition (if any)
			pthread_cond_signal(taskQueueConditional);
			// allow others to proceed
			pthread_mutex_unlock(taskQueueMutex);
		}

		if (readCount == -1) {
			perror("read");
			return READ_ERROR;
		} else if (readCount == 0) {
			printf("EOF\n");
			close(connectedSocket);
			return NORMAL_EXIT;	//TODO we don't want to exit in real life
		}
	}

	return NORMAL_EXIT;
}

void *IPCListenerThreadStarter(void *par) {
	tIpcListenerThreadParam p = *((tIpcListenerThreadParam *) par);

	//stow away the maintenance data to avoid passing them around over and over
	taskQueueMutex = p.taskQueueMutex;
	taskQueueConditional = p.taskQueueConditional;
	taskQueue = p.taskQueue;

	cout << "IPC Listener: Thread started" << endl;

	//now start the worker
	p.taskResult = IpcListener(p.alternatePath);

	cout << "IPC Listener: I'm out" << endl;

	p.taskResult = NORMAL_EXIT;

	return NULL;
}


/**************************************/
/*
 * Helper for 2 way socket communication
 */

void emitReceiptMsg(int connectedSock, string msgType, json msgJSON) {
	json j = { { "type", msgType }, { "data", msgJSON } };
	//XXX The node.js module needs a "\f" at the end of the serialized JSON-Message :-)
	string msgString = j.dump() + "\f";
	unsigned int msgSize = msgString.length();
	for (unsigned int bytesTransferred = 0; bytesTransferred < msgSize;) {
		bytesTransferred += write(connectedSock, msgString.c_str(), msgSize);
	}
}
