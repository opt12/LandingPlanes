/*
 * 1597_ipc_listener.h
 *
 *  Created on: 12.07.2017
 *      Author: eckstein
 */

#ifndef IPC_LISTENER_H_
#define IPC_LISTENER_H_

#include <string>
#include <queue>

#include "1597_Task_Definition.h"


struct tIpcListenerThreadParam {
	pthread_mutex_t *taskQueueMutex;
	pthread_cond_t *taskQueueConditional;
	std::string alternatePath="";
	std::queue<json> *taskQueue;
	int socketDescriptor;	//TODO: wir wollen den Socket eine ebene weiter oben öffnen, damit wir ihn durchreichen können und in beide richtungen einen Kanal haben
	eResult taskResult;
};


void *IPCListenerThreadStarter(void *par);

#endif /* IPC_LISTENER_H_ */
