/*
 * 1597_QueueDispatcher.h
 *
 *  Created on: 12.07.2017
 *      Author: eckstein
 */

#ifndef QUEUEDISPATCHER_H_
#define QUEUEDISPATCHER_H_

#include <string>
#include <queue>

#include "1597_Task_Definition.h"

struct tQueueDispatcherThreadParam{
	pthread_mutex_t *taskQueueMutex;
	pthread_cond_t *taskQueueConditional;
	std::queue<json> *taskQueue;
	int socketDescriptor;	//TODO: wir wollen den Socket eine ebene weiter oben öffnen, damit wir ihn durchreichen können und in beide richtungen einen Kanal haben
	eResult taskResult;
};

void *QueueDispatcherThreadstarter(void *par);

#endif /* QUEUEDISPATCHER_H_ */
