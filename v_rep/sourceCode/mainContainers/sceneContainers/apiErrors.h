// This file is part of V-REP, the Virtual Robot Experimentation Platform.
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// V-REP is dual-licensed, under the terms of EITHER (at your option):
//   1. V-REP commercial license (contact us for details)
//   2. GNU GPL (see below)
// 
// GNU GPL license:
// -------------------------------------------------------------------
// V-REP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// V-REP IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with V-REP.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.1.2 on June 16th 2014

#pragma once

#include "MainCont.h"
#include <vector>

#define SIM_ERROR_SIMULATOR_NOT_INITIALIZED 			"Simulator not initialized."
#define SIM_ERRROR_EDIT_MODE_ACTIVE 			"Edit mode is active."
#define SIM_ERROR_CANNOT_SET_GET_PARAM_LAUNCH 			"Cannot set/get parameter: simulator launch state is wrong."
#define SIM_ERROR_CANNOT_SET_GET_PARAM_WINDOW 			"Cannot set/get parameter: simulator window initialization state is wrong."
#define SIM_ERROR_CANNOT_SET_GET_PARAM_SIM 			"Cannot set/get parameter: simulation run state is wrong."
#define SIM_ERROR_OBJECT_INEXISTANT 			"Object does not exist."
#define SIM_ERROR_VISION_SENSOR_INEXISTANT 			"Vision sensor does not exist."
#define SIM_ERROR_PATH_INEXISTANT 			"Path does not exist."
#define SIM_ERROR_ENTITY_INEXISTANT 			"Entity does not exist."
#define SIM_ERROR_COLLECTION_INEXISTANT 			"Collection does not exist."
#define SIM_ERROR_COLLISION_INEXISTANT 			"Collision object does not exist."
#define SIM_ERROR_DISTANCE_INEXISTANT 			"Distance object does not exist."
#define SIM_ERROR_OBJECT_NOT_JOINT	 			"Object is not a joint."
#define SIM_ERROR_OBJECT_NOT_SHAPE 			"Object is not a shape."
#define SIM_ERROR_OBJECT_NOT_PROX_SENSOR 			"Object is not a proximity sensor."
#define SIM_ERROR_OBJECT_NOT_MILL 			"Object is not a mill."
#define SIM_ERROR_OBJECT_NOT_FORCE_SENSOR 			"Object is not a force sensor."
#define SIM_ERROR_OBJECT_NOT_VISION_SENSOR 			"Object is not a vision sensor."
#define SIM_ERROR_OBJECT_NOT_GRAPH 			"Object is not a graph."
#define SIM_ERROR_OBJECT_NOT_PATH 			"Object is not a path."
#define SIM_ERROR_OBJECT_NOT_LIGHT 			"Object is not a light."
#define SIM_ERROR_OBJECT_NOT_DUMMY 			"Object is not a dummy."
#define SIM_ERROR_UI_INEXISTANT 			"UI does not exist."
#define SIM_ERROR_UI_BUTTON_INEXISTANT 			"UI button does not exist."
#define SIM_ERROR_IK_GROUP_INEXISTANT 			"IK group does not exist."
#define SIM_ERROR_IK_ELEMENT_INEXISTANT 			"IK element does not exist."
#define SIM_ERROR_MECHANISM_INEXISTANT 			"Mechanism does not exist."
#define SIM_ERROR_BUFFER_INEXISTANT				"Buffer does not exist."
#define SIM_ERROR_PATH_PLANNING_OBJECT_INEXISTANT 			"Path planning object does not exist."
#define SIM_ERROR_MOTION_PLANNING_OBJECT_INEXISTANT 		"Motion planning object does not exist."
#define SIM_ERROR_SCRIPT_INEXISTANT				"Script does not exist."
#define SIM_ERROR_PAGE_INEXISTANT				"Page does not exist."
#define SIM_ERROR_ILLEGAL_OBJECT_NAME 			"Illegal object name."
#define SIM_ERROR_ILLEGAL_COLLECTION_NAME 			"Illegal collection name."
#define SIM_ERROR_JOINT_SPHERICAL 			"Joint is spherical."
#define SIM_ERROR_JOINT_NOT_SPHERICAL 			"Joint is not spherical."
#define SIM_ERROR_JOINT_NOT_IN_FORCE_TORQUE_MODE 			"Joint is not in force/torque mode."
#define SIM_ERROR_SIMULATION_NOT_STOPPED 			"Simulation is not stopped."
#define SIM_ERROR_SIMULATION_NOT_RUNNING 			"Simulation is not running."
#define SIM_ERROR_SIMULATION_NOT_REAL_TIME			"Simulation is not real-time."
#define SIM_ERROR_FILE_NOT_FOUND 			"File not found."
#define SIM_ERROR_SCENE_COULD_NOT_BE_READ 			"Scene could not be read."
#define SIM_ERROR_MODEL_COULD_NOT_BE_READ 			"Model could not be read."
#define SIM_ERROR_UI_COULD_NOT_BE_READ 			"UI could not be read."
#define SIM_ERROR_WRONG_POS_SIZE_PARAMS 			"Wrong position/size parameters."
#define SIM_ERROR_BUTTON_NOT_SLIDER 			"Button is not a slider."
#define SIM_ERROR_SCENE_LOCKED 			"Scene is locked."
#define SIM_ERROR_SCENE_COULD_NOT_BE_SAVED 			"Scene could not be saved."
#define SIM_ERROR_OBJECT_NOT_MODEL_BASE 			"Object is not tagged as model base."
#define SIM_ERROR_MODEL_COULD_NOT_BE_SAVED 			"Model could not be saved."
#define SIM_ERROR_UI_COULD_NOT_BE_SAVED 			"UI could not be saved."
#define SIM_ERROR_TOO_MANY_TEMP_OBJECTS 			"Too many temporary path search objects."
#define SIM_ERROR_PATH_PLANNING_OBJECT_NOT_CONSISTENT 			"Path planning object is not consistent."
#define SIM_ERROR_PATH_PLANNING_OBJECT_NOT_CONSISTENT_ANYMORE "Path planning object is not consistent anymore."
#define SIM_ERROR_TEMP_PATH_SEARCH_OBJECT_INEXISTANT 			"Temporary path search object does not exist."
#define SIM_ERROR_OBJECT_NOT_TAGGED_FOR_EXPLICIT_HANDLING 			"Object not tagged for explicit handling."
#define SIM_ERROR_SCRIPT_NOT_TAGGED_FOR_EXPLICIT_HANDLING			"Script not tagged for explicit handling."
#define SIM_ERROR_SCRIPT_WAS_DESTROYED 			"Script was destroyed."
#define SIM_ERROR_SCRIPT_NOT_CHILD_SCRIPT 			"Script is not a child script."
#define SIM_ERROR_CUSTOM_LUA_FUNC_COULD_NOT_BE_REGISTERED 			"Custom function could not be registered."
#define SIM_ERROR_CUSTOM_LUA_VAR_COULD_NOT_BE_REGISTERED 			"Custom variable could not be registered."
#define SIM_ERROR_MAIN_WINDOW_NOT_INITIALIZED 			"Main window not initialized."
#define SIM_ERROR_OPERATION_FAILED 			"Operation failed."
#define SIM_ERROR_INVALID_PARAMETER 			"Invalid parameter."
#define SIM_ERROR_INVALID_ARGUMENT 			"Invalid argument."
#define SIM_ERROR_INVALID_ARGUMENTS 			"Invalid arguments."
#define SIM_ERROR_INVALID_FIRST_ARGUMENT		"Invalid first argument."
#define SIM_ERROR_INVALID_DIMENSIONS 			"Invalid dimensions."
#define SIM_ERROR_INVALID_ITEM_HANDLE 			"Invalid item handle."
#define SIM_ERROR_INVALID_HANDLE 			"Invalid handle."
#define SIM_ERROR_INVALID_HANDLES 			"Invalid handles."
#define SIM_ERROR_INVALID_TARGET_HANDLE 			"Invalid target handle."
#define SIM_ERROR_INVALID_DATA_HEADER 			"Invalid data header."
#define SIM_ERROR_INVALID_DATA_NAME 			"Invalid data name."
#define SIM_ERROR_INVALID_DATA 			"Invalid data."
#define SIM_ERROR_INVALID_TYPE 			"Invalid type."
#define SIM_ERROR_INVALID_NUMBERS		"Invalid numbers."
#define SIM_ERROR_INVALID_ANTENNA_HANDLE 			"Invalid antenna handle."
#define SIM_ERROR_INVALID_DATA_STREAM 			"Invalid data stream."
#define SIM_ERROR_INVALID_PORT_HANDLE 			"Invalid port handle."
#define SIM_ERROR_INVALID_FILE_FORMAT 			"Invalid file format."
#define SIM_ERROR_FOUND_INVALID_HANDLES		"Found invalid handles."
#define SIM_ERROR_DATA_STREAM_NOT_USER_DEFINED 			"Data stream is not user-defined."
#define SIM_ERROR_PATH_EMPTY 			"Path is empty."
#define SIM_ERROR_PORT_NOT_OPEN 			"Port is not open."
#define SIM_ERROR_CANNOT_BE_CALLED_FROM_MAIN_THREAD 			"Cannot be called from the main thread."
#define SIM_ERROR_OBJECT_IS_NOT_VIEWABLE 			"Associated object is not viewable."
#define SIM_ERROR_NOT_ENOUGH_SHAPES 			"Not enough shapes specified."
#define SIM_ERROR_CANNOT_DIVIDE_GROUPED_SHAPE "Cannot divide grouped shape."
#define SIM_ERROR_MATERIAL_INEXISTANT 			"Material does not exist."
#define SIM_ERROR_TEXTURE_INEXISTANT 			"Texture does not exist."

#define SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS "The function requires more arguments."
#define SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG "One of the function's argument type is not correct."
#define SIM_ERROR_ONE_TABLE_SIZE_IS_WRONG "One of the function's table size is not correct."
#define SIM_ERROR_ONE_STRING_SIZE_IS_WRONG "One of the function's string size is not correct."
#define SIM_ERROR_NOT_MAIN_NOR_CHILD_SCRIPT "Script is not a main script nor a child script."
#define SIM_ERROR_NON_EXPLICIT_CHILD_SCRIPT_EXECUTIONS_WERE_DELEGATED "Non-explicit child script executions were delegated."
#define SIM_ERROR_THREAD_LAUNCHING_SCRIPTS_WILL_NOT_BE_CALLED_FROM_SENSING_SECTION "Scripts launching a thread will not be called when in a sensing section."
#define SIM_ERROR_INVALID_CHILD_SCRIPT_HANDLE_OR_CHILD_SCRIPT_NOT_IN_CURRENT_HIERARCHY "Invalid child script handle, or child script not in current hierarchy tree."
#define SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_MAIN_SCRIPT "Can only be called from the main script."
#define SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_MAIN_SCRIPT_OR_CHILD_SCRIPT "Can only be called from the main script or a child script."
#define SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD "Can only be called from a thread."
#define SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_THE_MAIN_THREAD "Can only be called from the main thread."
#define SIM_ERROR_CANNOT_BE_CALLED_IF_DIDN_T_LAUNCH_THREAD "Cannot be called from a script that didn't launch a thread."
#define SIM_ERROR_ARGUMENT_VALID_ONLY_WITH_CHILD_SCRIPTS "Argument only valid with child scripts."
#define SIM_ERROR_SCRIPT_MUST_RUN_IN_THREAD_FOR_MODAL_OPERATION "Script must run in a thread for modal operation."
#define SIM_ERROR_CANNOT_USE_THAT_STYLE_IN_MODAL_OPERATION "Can't use that dialog style in modal operation."
#define SIM_ERROR_TABLE_CONTAINS_INVALID_TYPES "Table contains invalid types."
#define SIM_ERROR_FUNCTION_DEPRECATED_AND_HAS_NO_EFFECT "Function is deprecated and has no effect."
#define SIM_ERROR_OBJECT_OR_TARGET_OBJECT_DOES_NOT_EXIST "Object or target object does not exist."
#define SIM_ERROR_OBJECT_IS_SAME_AS_TARGET_OBJECT "Object is same as target object."
#define SIM_ERROR_TARGET_OBJECT_IS_NOT_A_PATH "Target object is not a path."
#define SIM_ERROR_BLOCKING_OPERATION_ONLY_FROM_THREAD "Blocking operation only available when called from a thread."
#define SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_READ "Could not lock resources for data read operation."
#define SIM_ERROR_COULD_NOT_LOCK_RESOURCES_FOR_WRITE "Could not lock resources for data write operation."


class CApiErrors : public CMainCont 
{
public:
	CApiErrors();
	virtual ~CApiErrors();
	void simulationAboutToStart();
	void simulationEnded();
	void renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib);

	static std::string getLastErrorMessage();
	std::string _getLastErrorMessage(VTHREAD_ID_TYPE processID);

	static void setLastErrorMessage(const char* functionName,const char* errMsg);
	void _setLastErrorMessage(VTHREAD_ID_TYPE processID,const char* functionName,const char* errMsg);

	static void setErrorReportingMode(int mode);
	static int getErrorReportingMode();
	void _setErrorReportingMode(VTHREAD_ID_TYPE processID,int mode);
	int _getErrorReportingMode(VTHREAD_ID_TYPE processID);

private:

	static VTHREAD_ID_TYPE _getProcessID();
	int _getIndexFromProcessID(VTHREAD_ID_TYPE processID);


	// 1. Main thread (i.e. non-gui, non-auxiliary thread), C API:
	static int c_api_mainThread_errorReportingMode;
	static std::string last_c_api_mainThread_errorMessage;

	// 2. GUI thread, C API:
	static int c_api_guiThread_errorReportingMode;
	static std::string last_c_api_guiThread_errorMessage;

	// 3. Lua API: (could also be the main thread!)
	std::vector<VTHREAD_ID_TYPE> lastLuaAPICallErrorMessage_processIDs;
	std::vector<std::string> lastLuaAPICallErrorMessage_messages;
	std::vector<int> lastLuaAPICallErrorMessage_reportingModes;

};
