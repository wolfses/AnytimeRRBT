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

#include "pathPlanningInterface.h"
#include "v_repConst.h"
#include <math.h>

#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
#include "pluginContainer.h"
#endif

#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
#include "NonHolonomicPathPlanning.h"
#include "HolonomicPathPlanning.h"
#include "mpObject.h"
#endif

#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
ptr_createNonHolonomicPathPlanningObject CPathPlanningInterface::_createNonHolonomicPathPlanningObject;
ptr_createHolonomicPathPlanningObject CPathPlanningInterface::_createHolonomicPathPlanningObject;
ptr_destroyPathPlanningObject CPathPlanningInterface::_destroyPathPlanningObject;
ptr_searchPath CPathPlanningInterface::_searchPath;
ptr_getPathNodeCount CPathPlanningInterface::_getPathNodeCount;
ptr_setPartialPath CPathPlanningInterface::_setPartialPath;
ptr_smoothFoundPath CPathPlanningInterface::_smoothFoundPath;
ptr_getFoundPath CPathPlanningInterface::_getFoundPath;
ptr_getSearchTree CPathPlanningInterface::_getSearchTree;
ptr_releaseBuffer CPathPlanningInterface::_releaseBuffer;

ptr_createMpObject CPathPlanningInterface::_createMpObject;
ptr_destroyMpObject CPathPlanningInterface::_destroyMpObject;
ptr_setMpObjectData CPathPlanningInterface::_setMpObjectData;
ptr_calculateMpNodesPhase1 CPathPlanningInterface::_calculateMpNodesPhase1;
ptr_getMpPhase1NodesRenderData CPathPlanningInterface::_getMpPhase1NodesRenderData;
ptr_getMpPhase2NodesRenderData CPathPlanningInterface::_getMpPhase2NodesRenderData;
ptr_getMpSerializationData CPathPlanningInterface::_getMpSerializationData;
ptr_getMpPhase1NodeCnt CPathPlanningInterface::_getMpPhase1NodeCnt;
ptr_getMpRobotConfigFromTipPose CPathPlanningInterface::_getMpRobotConfigFromTipPose;
ptr_mpFindPath CPathPlanningInterface::_mpFindPath;
ptr_mpFindIkPath CPathPlanningInterface::_mpFindIkPath;
ptr_mpSimplifyPath CPathPlanningInterface::_mpSimplifyPath;
ptr_mpGetConfigTransition CPathPlanningInterface::_mpGetConfigTransition;




bool CPathPlanningInterface::_pathPlanningInterfaceInitialized=false;
bool CPathPlanningInterface::_pathPlanningSystemPluginsLoadPhaseOver=false;

void CPathPlanningInterface::setSystemPluginsLoadPhaseOver()
{
	_pathPlanningSystemPluginsLoadPhaseOver=true;
}

bool CPathPlanningInterface::initializeFunctionsIfNeeded()
{
	static bool alreadyTried=false;
	if (_pathPlanningInterfaceInitialized)
		return(true);
	if (alreadyTried&&_pathPlanningSystemPluginsLoadPhaseOver)
		return(false);
	alreadyTried=true;
	CPlugin* plugin=CPluginContainer::getPluginFromName("PathPlanning");
	if (plugin==NULL)
		return(false); // plugin not there!
	// Get the version first:
	int auxVals[4]={0,0,0,0};
	int retVals[4];
	plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,retVals);
//	int version=retVals[0];
	// Now get all the function pointers:
	int v=1;
	auxVals[0]=v++;
	_createNonHolonomicPathPlanningObject=(ptr_createNonHolonomicPathPlanningObject)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_createHolonomicPathPlanningObject=(ptr_createHolonomicPathPlanningObject)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_destroyPathPlanningObject=(ptr_destroyPathPlanningObject)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_searchPath=(ptr_searchPath)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getPathNodeCount=(ptr_getPathNodeCount)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_setPartialPath=(ptr_setPartialPath)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_smoothFoundPath=(ptr_smoothFoundPath)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getFoundPath=(ptr_getFoundPath)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getSearchTree=(ptr_getSearchTree)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_releaseBuffer=(ptr_releaseBuffer)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;

	_createMpObject=(ptr_createMpObject)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_destroyMpObject=(ptr_destroyMpObject)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_setMpObjectData=(ptr_setMpObjectData)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_calculateMpNodesPhase1=(ptr_calculateMpNodesPhase1)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getMpPhase1NodesRenderData=(ptr_getMpPhase1NodesRenderData)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getMpPhase2NodesRenderData=(ptr_getMpPhase2NodesRenderData)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getMpSerializationData=(ptr_getMpSerializationData)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getMpPhase1NodeCnt=(ptr_getMpPhase1NodeCnt)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getMpRobotConfigFromTipPose=(ptr_getMpRobotConfigFromTipPose)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_mpFindPath=(ptr_mpFindPath)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_mpFindIkPath=(ptr_mpFindIkPath)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_mpSimplifyPath=(ptr_mpSimplifyPath)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_mpGetConfigTransition=(ptr_mpGetConfigTransition)plugin->sendEventCallbackMessage(sim_message_eventcallback_pathplanningplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_pathPlanningInterfaceInitialized=true;
	return(true);
}

#endif // COMPILING_EXTERNAL_PATHPLANNING_DLL


CDummyNonHolonomicPathPlanning* CPathPlanningInterface::createNonHolonomicPathPlanningObject(int theStartDummyID,int theGoalDummyID,
							int theRobotCollectionID,int theObstacleCollectionID,float theAngularCoeff,
							float theSteeringAngleCoeff,float theMaxSteeringAngleVariation,float theMaxSteeringAngle,
							float theStepSize,const float theSearchMinVal[2],const float theSearchRange[2],
							const int theDirectionConstraints[2],const float clearanceAndMaxDistance[2])
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		//printf("%s - %s : %d = new CNonHolonomicPathPlanning\n",__FILE__,__func__,__LINE__);
		CNonHolonomicPathPlanning* p=new CNonHolonomicPathPlanning(theStartDummyID,theGoalDummyID,theRobotCollectionID,
				theObstacleCollectionID,-1,theAngularCoeff,theSteeringAngleCoeff,theMaxSteeringAngleVariation,theMaxSteeringAngle,theStepSize,
				theSearchMinVal,theSearchRange,theDirectionConstraints,clearanceAndMaxDistance);
		return((CDummyNonHolonomicPathPlanning*)p);
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return((CDummyNonHolonomicPathPlanning*)_createNonHolonomicPathPlanningObject(theStartDummyID,theGoalDummyID,theRobotCollectionID,
			theObstacleCollectionID,theAngularCoeff,theSteeringAngleCoeff,theMaxSteeringAngleVariation,theMaxSteeringAngle,theStepSize,
			theSearchMinVal,theSearchRange,theDirectionConstraints,clearanceAndMaxDistance));
#endif
}

CDummyHolonomicPathPlanning* CPathPlanningInterface::createHolonomicPathPlanningObject(int theStartDummyID,int theGoalDummyID,
							int theRobotCollectionID,int theObstacleCollectionID,int thePlanningType,
							float theAngularCoeff,float theStepSize,const float theSearchMinVal[4],const float theSearchRange[4],
							const int theDirectionConstraints[4],const float clearanceAndMaxDistance[2],const float gammaAxis[3])
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		CHolonomicPathPlanning* p=new CHolonomicPathPlanning(theStartDummyID,theGoalDummyID,theRobotCollectionID,
				theObstacleCollectionID,-1,thePlanningType,theAngularCoeff,theStepSize,
				theSearchMinVal,theSearchRange,theDirectionConstraints,clearanceAndMaxDistance,C3Vector(gammaAxis));
		return((CDummyHolonomicPathPlanning*)p);
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return((CDummyHolonomicPathPlanning*)_createHolonomicPathPlanningObject(theStartDummyID,theGoalDummyID,theRobotCollectionID,
			theObstacleCollectionID,thePlanningType,theAngularCoeff,theStepSize,
			theSearchMinVal,theSearchRange,theDirectionConstraints,clearanceAndMaxDistance,gammaAxis));
#endif
}


void CPathPlanningInterface::destroyPathPlanningObject(CDummyPathPlanning* obj)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		delete (CPathPlanning*)obj;
		return;
	}
#else
		return;
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	_destroyPathPlanningObject(obj);
#endif
}

int CPathPlanningInterface::searchPath(CDummyPathPlanning* obj,int maxTimeInMsPerPass)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		return(((CPathPlanning*)obj)->searchPath(maxTimeInMsPerPass));
	}
#else
		return(1);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return(_searchPath(obj,maxTimeInMsPerPass));
#endif
}

int CPathPlanningInterface::getPathNodeCount(CDummyPathPlanning* obj,char fromStart)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		if (((CPathPlanning*)obj)->isHolonomic)
		{
			if (fromStart!=0)
				return(int(((CHolonomicPathPlanning*)obj)->fromStart.size()));
			return(int(((CHolonomicPathPlanning*)obj)->fromGoal.size()));
		}
		else
		{
			if (fromStart!=0)
				return(int(((CNonHolonomicPathPlanning*)obj)->tree.size()));
			return(int(((CNonHolonomicPathPlanning*)obj)->tree.size()));
		}
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return(_getPathNodeCount(obj,fromStart));
#endif
}

char CPathPlanningInterface::setPartialPath(CDummyPathPlanning* obj)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		return(((CPathPlanning*)obj)->setPartialPath());
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return(_setPartialPath(obj));
#endif
}

int CPathPlanningInterface::smoothFoundPath(CDummyPathPlanning* obj,int steps,int maxTimePerPass)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		return(((CPathPlanning*)obj)->smoothFoundPath(steps,maxTimePerPass));
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return(_smoothFoundPath(obj,steps,maxTimePerPass));
#endif
}

float* CPathPlanningInterface::getFoundPath(CDummyPathPlanning* obj,int* nodeCount)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		float* retVal=NULL;
		std::vector<float> data;
		((CPathPlanning*)obj)->getPathData(data);
		nodeCount[0]=data.size()/7;
		if (nodeCount[0]!=0)
		{
			retVal=new float[nodeCount[0]*7];
			for (int i=0;i<nodeCount[0]*7;i++)
				retVal[i]=data[i];
		}
		return(retVal);
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return(_getFoundPath(obj,nodeCount));
#endif
}

float* CPathPlanningInterface::getSearchTree(CDummyPathPlanning* obj,int* segmentCount,char fromStart)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		float* retVal=NULL;
		std::vector<float> data;
		((CPathPlanning*)obj)->getSearchTreeData(data,fromStart!=0);
		segmentCount[0]=data.size()/6;
		if (segmentCount[0]!=0)
		{
			retVal=new float[segmentCount[0]*6];
			for (int i=0;i<segmentCount[0]*6;i++)
				retVal[i]=data[i];
		}
		return(retVal);
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return(_getSearchTree(obj,segmentCount,fromStart));
#endif
}

void CPathPlanningInterface::releaseBuffer(void* buff)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		delete[] ((char*)buff);
		return;
	}
#else
		return;
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	_releaseBuffer(buff);
#endif
}


CDummyMpObject* CPathPlanningInterface::createMpObject()
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		return((CDummyMpObject*)new CmpObject());
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return(_createMpObject());
#endif
}


void CPathPlanningInterface::destroyMpObject(CDummyMpObject* mpObject)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		delete (CmpObject*)mpObject;
		return;
	}
#else
		return;
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	_destroyMpObject(mpObject);
#endif
}

void CPathPlanningInterface::setMpObjectData(CDummyMpObject* mpObject,int jointCnt,const int* jointHandles,const int* jointStepCount,int ikGroupId,int baseFrameId,int tipFrameId,int robotSelfCollEntity1,int robotSelfCollEntity2,int robotEntity,int obstacleEntity,float distanceThreshold,const float* tipMetricAndIkElConstraints,const float* robotMetric)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		((CmpObject*)mpObject)->setData(jointCnt,jointHandles,jointStepCount,ikGroupId,baseFrameId,tipFrameId,robotSelfCollEntity1,robotSelfCollEntity2,robotEntity,obstacleEntity,distanceThreshold,tipMetricAndIkElConstraints,robotMetric);
		return;
	}
#else
		return;
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	_setMpObjectData(mpObject,jointCnt,jointHandles,jointStepCount,ikGroupId,baseFrameId,tipFrameId,robotSelfCollEntity1,robotSelfCollEntity2,robotEntity,obstacleEntity,distanceThreshold,tipMetricAndIkElConstraints,robotMetric);
#endif
}

char CPathPlanningInterface::calculateMpNodesPhase1(CDummyMpObject* mpObject,const char* serializationData,int serializationDataSize)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		return(((CmpObject*)mpObject)->calculateNodes(serializationData,serializationDataSize));
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return(_calculateMpNodesPhase1(mpObject,serializationData,serializationDataSize));
#endif
}


int CPathPlanningInterface::getMpPhase1NodesRenderData(CDummyMpObject* mpObject,int index,float** pos)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		return(((CmpObject*)mpObject)->getPhase1NodesRenderData(index,pos));
	}
#else
		return(-1);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return(_getMpPhase1NodesRenderData(mpObject,index,pos));
#endif
}

int CPathPlanningInterface::getMpPhase2NodesRenderData(CDummyMpObject* mpObject,unsigned char what,int index,float** pos1,float** pos2)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		return(((CmpObject*)mpObject)->getPhase2NodesRenderData(what,index,pos1,pos2));
	}
#else
		return(-1);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return(_getMpPhase2NodesRenderData(mpObject,what,index,pos1,pos2));
#endif
}

char* CPathPlanningInterface::getMpSerializationData(CDummyMpObject* mpObject,int* cnt)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		return(((CmpObject*)mpObject)->getSerializationData(cnt));
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return(_getMpSerializationData(mpObject,cnt));
#endif
}

int CPathPlanningInterface::getMpPhase1NodeCnt(CDummyMpObject* mpObject,char collidingNodesOnly)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		return(((CmpObject*)mpObject)->getPhase1NodeCnt(collidingNodesOnly));
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return(_getMpPhase1NodeCnt(mpObject,collidingNodesOnly));
#endif
}


int CPathPlanningInterface::getMpRobotConfigFromTipPose(CDummyMpObject* mpObject,const float* tipPos,const float* tipQuat,int options,float* robotJoints,const float* constraints,const float* configConstraints,int trialCount,float tipCloseDistance,const float* referenceConfigs,int configCount,const int* jointBehaviour,int correctionPasses,int maxTimeInMs)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		return(((CmpObject*)mpObject)->getRobotConfigFromTipPose(tipPos,tipQuat,options,robotJoints,constraints,configConstraints,trialCount,tipCloseDistance,referenceConfigs,configCount,jointBehaviour,correctionPasses,maxTimeInMs));
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return(_getMpRobotConfigFromTipPose(mpObject,tipPos,tipQuat,options,robotJoints,constraints,configConstraints,trialCount,tipCloseDistance,referenceConfigs,configCount,jointBehaviour,correctionPasses,maxTimeInMs));
#endif
}

float* CPathPlanningInterface::mpFindPath(CDummyMpObject* mpObject,const float* startConfig,const float* goalConfig,int options,float stepSize,int* outputConfigsCnt,int maxTimeInMs,const int* auxIntParams,const float* auxFloatParams)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		return(((CmpObject*)mpObject)->findPath(startConfig,goalConfig,options,stepSize,outputConfigsCnt,maxTimeInMs,auxIntParams,auxFloatParams));
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return(_mpFindPath(mpObject,startConfig,goalConfig,options,stepSize,outputConfigsCnt,maxTimeInMs,auxIntParams,auxFloatParams));
#endif
}


float* CPathPlanningInterface::mpFindIkPath(CDummyMpObject* mpObject,const float* startConfig,const float* goalPos,const float* goalQuat,int options,float stepSize,int* outputConfigsCnt,const int* auxIntParams,const float* auxFloatParams)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		return(((CmpObject*)mpObject)->findIkPath(startConfig,goalPos,goalQuat,options,stepSize,outputConfigsCnt,auxIntParams,auxFloatParams));
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return(_mpFindIkPath(mpObject,startConfig,goalPos,goalQuat,options,stepSize,outputConfigsCnt,auxIntParams,auxFloatParams));
#endif
}

float* CPathPlanningInterface::mpSimplifyPath(CDummyMpObject* mpObject,const float* pathBuffer,int configCnt,int options,float stepSize,int increment,int* outputConfigsCnt,int maxTimeInMs,const int* auxIntParams,const float* auxFloatParams)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		return(((CmpObject*)mpObject)->simplifyPath(pathBuffer,configCnt,options,stepSize,increment,outputConfigsCnt,maxTimeInMs,auxIntParams,auxFloatParams));
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return(_mpSimplifyPath(mpObject,pathBuffer,configCnt,options,stepSize,increment,outputConfigsCnt,maxTimeInMs,auxIntParams,auxFloatParams));
#endif
}

float* CPathPlanningInterface::mpGetConfigTransition(CDummyMpObject* mpObject,const float* startConfig,const float* goalConfig,int options,const int* select,float calcStepSize,float maxOutStepSize,int wayPointCnt,const float* wayPoints,int* outputConfigsCnt,const int* auxIntParams,const float* auxFloatParams)
{
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	if (!initializeFunctionsIfNeeded())
#endif
#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL
	{ // we use the internal functions
		return(((CmpObject*)mpObject)->getConfigTransition(startConfig,goalConfig,options,select,calcStepSize,maxOutStepSize,wayPointCnt,wayPoints,outputConfigsCnt,auxIntParams,auxFloatParams));
	}
#else
		return(0);
#endif
#ifndef COMPILING_EXTERNAL_PATHPLANNING_DLL
	// we call the plugin function:
	return(_mpGetConfigTransition(mpObject,startConfig,goalConfig,options,select,calcStepSize,maxOutStepSize,wayPointCnt,wayPoints,outputConfigsCnt,auxIntParams,auxFloatParams));
#endif
}


#ifdef COMPILING_EXTERNAL_PATHPLANNING_DLL

float CPathPlanningInterface::getNormalizedAngle(float angle)
{ // Returns an angle between -PI and +PI
	angle=fmod(angle,6.28318531f);
	if (angle<-3.14159266f)
		angle+=6.28318531f;
	else if (angle>+3.14159266f)
		angle-=6.28318531f;
	return(angle);
}
#endif






