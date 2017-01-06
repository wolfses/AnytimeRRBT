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

#include "vrepPrecompiledHeader.h"
#include "VisualParam.h"
#include <vector>

class CRegDist  
{
public:
	CRegDist(int obj1ID,int obj2ID,std::string objName,int objID);
	virtual ~CRegDist();

	void initializeInitialValues(bool simulationIsRunning);
	void simulationAboutToStart();
	void simulationEnded();

	bool isSame(int obj1ID,int obj2ID);
	int getObjectID();
	std::string getObjectName();
	std::string getObjectPartnersName();
	void setObjectName(std::string newName);
	void setObjectID(int newID);
	int getObject1ID();
	int getObject2ID();
	void clearDistanceResult();
	bool getDistanceResult(float dist[7]);
	float getCalculationTime();
	float handleDistance(bool handledByWorkThread);
	float readDistance();

	void serialize(CSer& ar);
	void performObjectLoadingMapping(std::vector<int>* map);
	void performGroupLoadingMapping(std::vector<int>* map);
	bool announceObjectWillBeErased(int objID,bool copyBuffer);
	bool announceGroupWillBeErased(int groupID,bool copyBuffer);
	void setTreshhold(float tr);
	float getTreshhold();
	void setTreshholdActive(bool active);
	bool getTreshholdActive();
	CRegDist* copyYourself();
	void setDisplaySegment(bool display);
	bool getDisplaySegment();
	void setExplicitHandling(bool explicitHandl);
	bool getExplicitHandling();
	void displayDistanceSegment();

	void setSegmentWidth(int w);
	int getSegmentWidth();
	int getLifeID();

	CVisualParam segmentColor;


protected:	

	// Variables which need to be serialized and copied:
	int object1ID;
	int object2ID;
	std::string objectName;
	int objectID;
	int _segmentWidth;
	float treshhold;
	bool treshholdActive;

	// Various:
	float distanceResult[7];
	float _distance;
	int distanceBuffer[4];
	bool distanceIsValid;
	int _calcTimeInMs;
	bool displaySegment;
	bool explicitHandling;
	int _lifeID;

	bool _initialValuesInitialized;
	bool _initialExplicitHandling;
};
