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

#include "visionTransf.h"
#include <vector>

class CVisionTransfCont  
{
public:
	CVisionTransfCont();
	virtual ~CVisionTransfCont();

	int addObject(CVisionTransf* obj);
	bool removeObject(int passiveVisionSensorHandle);
	void removeInvalidObjects();
	void removeAll();
	CVisionTransf* getVisionTransfFromPassiveVisionSensor(int passiveVisionSensorHandle);

private:
	std::vector<CVisionTransf*> _allObjects;
};
