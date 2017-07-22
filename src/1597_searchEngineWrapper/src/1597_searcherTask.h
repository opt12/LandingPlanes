/*
 * 1597_searcherTask.h
 *
 *  Created on: 21.07.2017
 *      Author: eckstein
 */

#ifndef SEARCHERTASK_H_
#define SEARCHERTASK_H_


#include "1597_Task_Definition.h"

struct tSearchTaskThreadStarterParam {
	json taskDescription;
	int commSocket;
	eResult taskResult;
};

void *searchTaskThreadStarter(void *par);




#endif /* SEARCHERTASK_H_ */
