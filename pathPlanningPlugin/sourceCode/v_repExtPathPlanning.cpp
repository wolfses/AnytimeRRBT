// This file is part of the PATH PLANNING PLUGIN for V-REP
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The PATH PLANNING PLUGIN is licensed under the terms of EITHER:
//   1. PATH PLANNING PLUGIN commercial license (contact us for details)
//   2. PATH PLANNING PLUGIN educational license (see below)
// 
// PATH PLANNING PLUGIN educational license:
// -------------------------------------------------------------------
// The PATH PLANNING PLUGIN educational license applies only to EDUCATIONAL
// ENTITIES composed by following people and institutions:
// 
// 1. Hobbyists, students, teachers and professors
// 2. Schools and universities
// 
// EDUCATIONAL ENTITIES do NOT include companies, research institutions,
// non-profit organisations, foundations, etc.
// 
// An EDUCATIONAL ENTITY may use, modify, compile and distribute the
// modified/unmodified PATH PLANNING PLUGIN under following conditions:
//  
// 1. Distribution should be free of charge.
// 2. Distribution should be to EDUCATIONAL ENTITIES only.
// 3. Usage should be non-commercial.
// 4. Altered source versions must be plainly marked as such and distributed
//    along with any compiled code.
// 5. When using the PATH PLANNING PLUGIN in conjunction with V-REP, the "EDU"
//    watermark in the V-REP scene view should not be removed.
// 6. The origin of the PATH PLANNING PLUGIN must not be misrepresented. you must
//    not claim that you wrote the original software.
// 
// THE PATH PLANNING PLUGIN IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR
// IMPLIED WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#include <unistd.h>
#include "v_repExtPathPlanning.h"
#include "v_repLib.h"
#include <iostream>
#include "pathPlanningInterface.h"
#include <cstdio>

#ifdef _WIN32
#include <direct.h>
#endif

#define PATHPLANNING_PLUGIN_VERSION 4 // 4 since release 3.1.2

LIBRARY vrepLib;

VREP_DLLEXPORT unsigned char v_repStart(void* reservedPointer,int reservedInt)
{ // This is called just once, at the start of V-REP.

	// Dynamically load and bind V-REP functions:
	 char curDirAndFile[1024];
 #ifdef _WIN32
	 _getcwd(curDirAndFile, sizeof(curDirAndFile));
 #elif defined (__linux) || defined (__APPLE__)
	 getcwd(curDirAndFile, sizeof(curDirAndFile));
 #endif
	 std::string currentDirAndPath(curDirAndFile);
	 // 2. Append the V-REP library's name:
	 std::string temp(currentDirAndPath);
 #ifdef _WIN32
	 temp+="/v_rep.dll";
 #elif defined (__linux)
	 temp+="/libv_rep.so";
 #elif defined (__APPLE__)
	 temp+="/libv_rep.dylib";
 #endif /* __linux || __APPLE__ */
	 // 3. Load the V-REP library:
	 vrepLib=loadVrepLibrary(temp.c_str());
	 if (vrepLib==NULL)
	 {
		 std::cout << "Error, could not find or correctly load the V-REP library. Cannot start 'PathPlanning' plugin.\n";
		 return(0); // Means error, V-REP will unload this plugin
	 }
	 if (getVrepProcAddresses(vrepLib)==0)
	 {
		 std::cout << "Error, could not find all required functions in the V-REP library. Cannot start 'PathPlanning' plugin.\n";
		 unloadVrepLibrary(vrepLib);
		 return(0); // Means error, V-REP will unload this plugin
	 }
	 // ******************************************

	 // Check the version of V-REP:
	 // ******************************************
	 int vrepVer;
	 simGetIntegerParameter(sim_intparam_program_version,&vrepVer);
	 if (vrepVer<20607) // if V-REP version is smaller than 2.06.07
	 {
		 std::cout << "Sorry, your V-REP copy is somewhat old. Cannot start 'PathPlanning' plugin.\n";
		 unloadVrepLibrary(vrepLib);
		 return(0); // Means error, V-REP will unload this plugin
	 }
	 // ******************************************

	return(PATHPLANNING_PLUGIN_VERSION);	// initialization went fine, we return the version number of this plugin (can be queried with simGetModuleName)
}

VREP_DLLEXPORT void v_repEnd()
{
	unloadVrepLibrary(vrepLib); 
}

VREP_DLLEXPORT void* v_repMessage(int message,int* auxiliaryData,void* customData,int* replyData)
{
	void* retVal=NULL;
	if (message==sim_message_eventcallback_pathplanningplugin)
	{
		if (auxiliaryData[0]==0)
			replyData[0]=PATHPLANNING_PLUGIN_VERSION; // this is the version number of this plugin
		int v=1;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::createNonHolonomicPathPlanningObject);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::createHolonomicPathPlanningObject);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::destroyPathPlanningObject);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::searchPath);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::getPathNodeCount);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::setPartialPath);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::smoothFoundPath);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::getFoundPath);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::getSearchTree);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::releaseBuffer);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::createMpObject);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::destroyMpObject);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::setMpObjectData);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::calculateMpNodesPhase1);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::getMpPhase1NodesRenderData);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::getMpPhase2NodesRenderData);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::getMpSerializationData);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::getMpPhase1NodeCnt);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::getMpRobotConfigFromTipPose);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::mpFindPath);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::mpFindIkPath);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::mpSimplifyPath);
		v++;
		if (auxiliaryData[0]==v)
			return((void*)CPathPlanningInterface::mpGetConfigTransition);
		v++;
	}
	return(retVal);
}
