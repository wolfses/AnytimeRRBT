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

#include "Ser.h"
#include "dummyClasses.h"
#include "3Vector.h"

class CPathPlanningTask  
{
public:
	CPathPlanningTask();
	CPathPlanningTask(int theID,int thePathPlanningType);
	virtual ~CPathPlanningTask();

	void initializeInitialValues(bool simulationIsRunning);
	void simulationAboutToStart();
	void simulationEnded();

	CPathPlanningTask* copyYourself();
	void setDefaultValues();
	void serialize(CSer& ar);
	void performObjectLoadingMapping(std::vector<int>* map);
	void performGroupLoadingMapping(std::vector<int>* map);
	bool announceObjectWillBeErased(int objID,bool copyBuffer);
	bool announceGroupWillBeErased(int groupID,bool copyBuffer);
	int getObjectID();
	void setObjectID(int newID);
	std::string getObjectName();
	void setObjectName(std::string newName);
	bool isTaskValid();
	bool performSearch(bool showProgressDlg,float maxTime);
	bool initiateSteppedSearch(bool showProgressDlg,float maxTime,float subDt);
	int performSteppedSearch();

	void setShowSearchTrees(bool s);
	bool getShowSearchTrees();
	void getAndDisconnectSearchTrees(int& tree1Handle,int& tree2Handle);
	void connectExternalSearchTrees(int tree1Handle,int tree2Handle);

	void setGoalDummyID(int theID);
	void setPathID(int theID);
	void setHolonomicTaskType(int type);
	void setStepSize(float size);
	void setAngularStepSize(float step);
	void setSearchRange(float searchMin[4],float searchR[4]);
	void setMaxTime(float mTime);
	void setSearchDirection(int dir[4]);
	void setCollisionDetection(bool c);
	void setObstacleClearance(float c);
	void setMinTurningCircleDiameter(float d);

	int getGoalDummyID();
	int getPathID();
	void setRobotEntityID(int theID);
	int getRobotEntityID();
	void setObstacleEntityID(int theID);
	int getObstacleEntityID();

	void setObstacleMaxDistance(float c);
	float getObstacleMaxDistance();
	void setObstacleMaxDistanceEnabled(bool e);
	bool getObstacleMaxDistanceEnabled();


	int getHolonomicTaskType();
	float getStepSize();
	float getAngularStepSize();
	void getSearchRange(float searchMin[4],float searchR[4]);
	float getMaxTime();
	void getSearchDirection(int dir[4]);
	bool getCollisionDetection();
	float getObstacleClearance();
	int getPathPlanningType();
	int getStartDummyID();
	float getMinTurningCircleDiameter();

	void setPostProcessingPassCount(int p);
	int getPostProcessingPassCount();
	void setPartialPathIsOk(bool ok);
	bool getPartialPathIsOk();
	void setGammaAxis(const C3Vector& axis);
	C3Vector getGammaAxis();

	void setVisualizeSearchArea(bool v);
	bool getVisualizeSearchArea();
	void renderYour3DStuff();

	void setOriginalTask(CPathPlanningTask* originalTask);
	CPathPlanningTask* getOriginalTask();

protected:
	CPathPlanningTask* _originalTask_useWhenCopied;
	int _steppedSearchTemp_maxTimeInMs;
	int _steppedSearchTemp_initTimeInMs;
	int _steppedSearchTemp_maxSubTimeInMs;
	bool _steppedSearchTemp_showProgressDlg;
	CDummyPathPlanning* _steppedSearchTemp_theTask;
	int _steppedSearchTemp_foundPathStatus; // 0=not yet, 1=partial, 2=full
	int _steppedSearchTemp_currentSmoothingPass;

	bool _partialPathIsOk;
	bool _showSearchTrees;
	int _searchTree1Handle;
	int _searchTree2Handle;
	int _postProcessingPassCount;
	int pathPlanningType;
	int _startDummyID;
	int _goalDummyID;
	int _robotEntityID;
	int _obstacleEntityID;
	int _pathID;
	int holonomicTaskType;
	float stepSize;
	float angularStepSize;
	float searchMinValue[4];
	float searchRange[4];
	int searchDirection[4];
	C3Vector _gammaAxis;

	float maximumTime;
	int _objectID;
	bool collisionDetection;
	bool _visualizeSearchArea;
	float obstacleClearance;
	float _obstacleMaxDistance;
	bool _obstacleMaxDistanceEnabled;
	float minTurningCircleDiameter;
	std::string objectName;
};
