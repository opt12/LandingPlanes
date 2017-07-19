/*
 * 1597_Task_Definition.h
 *
 *  Created on: 12.07.2017
 *      Author: eckstein
 */

#ifndef TASK_DEFINITION_H_
#define TASK_DEFINITION_H_

#include <string>

#include "json.hpp"
// for convenience
using json = nlohmann::json;

using namespace std;


enum eTaskType {
	MESSAGE, TASK, SCAN, SAVE_2_M_FILE, PRINT_FILE_INFO, QUIT, INVALID,
};

inline eTaskType convertTaskType(string type){
	if(type == "SCAN")	return SCAN;
	if(type == "SAVE_2_M_FILE") return SAVE_2_M_FILE;
	if(type == "QUIT") return QUIT;

	return INVALID;
}

struct tScanTask {
	//TODO: define task structure
};

struct tSave_2_M_FileTask {
	//TODO: define task structure
};

//union tTaskDescription {
//	tScanTask scanTask;
//	tSave_2_M_FileTask save_2_M_FileTask;
//};

struct tTask {
	eTaskType taskType;
	json taskDescription;
};

struct tsocketMessage{
	int connectedSocket;
	json msg;
};

enum eResult {
	NORMAL_EXIT, READ_ERROR, SOCKET_ERROR, BIND_ERROR, LISTEN_ERROR, PATHNAME_ERROR,
	GEO_TIFF_READ_ERROR,
};

#endif /* TASK_DEFINITION_H_ */
