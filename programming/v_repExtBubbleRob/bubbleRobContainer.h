// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// -------------------------------------------------------------------
// THIS FILE IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// You are free to use/modify/distribute this file for whatever purpose!
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#pragma once

#include <vector>
#include "bubbleRob.h"

class CBubbleRobContainer
{

public:
	CBubbleRobContainer();
	virtual ~CBubbleRobContainer();


	void removeAll();
	void removeFromID(int theID);
	int insert(CBubbleRob* theBubbleRob);
	CBubbleRob* getFromID(int theID);
	CBubbleRob* getFromAssociatedObject(int theAssociatedObjectID);
	CBubbleRob* getFromIndex(int ind);
	int getCount();

	void actualizeForSceneContent();

	void startOfSimulation();
	void endOfSimulation();
	void handleSimulation();

protected:
	std::vector<CBubbleRob*> _allBubbleRobs; // all CBubbleRobs
};
