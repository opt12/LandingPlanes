/*
 * 1597_QueueDispatcher.h
 *
 *  Created on: 12.07.2017
 *      Author: eckstein
 */

#ifndef SCANTOMFILE_H_
#define SCANTOMFILE_H_

#include <string>
#include <queue>

#include "1597_Task_Definition.h"

struct tScanToMFileThreadStarterParam {
	json taskDescription;
	int commSocket;
	eResult taskResult;
};

void *scanToMFileThreadStarter(void *par);

#endif /* SCANTOMFILE_H_ */
