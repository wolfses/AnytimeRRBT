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

#include "visionVelodyneCont.h"
#include "v_repLib.h"

CVisionVelodyneCont::CVisionVelodyneCont()
{
}

CVisionVelodyneCont::~CVisionVelodyneCont()
{
	removeAll();
}

int CVisionVelodyneCont::addObject(CVisionVelodyne* obj)
{
	_allObjects.push_back(obj);
	return(obj->getVelodyneHandle());
}

bool CVisionVelodyneCont::removeObject(int velodyneHandle)
{
	for (int i=0;i<int(_allObjects.size());i++)
	{
		if (_allObjects[i]->getVelodyneHandle()==velodyneHandle)
		{
			delete _allObjects[i];
			_allObjects.erase(_allObjects.begin()+i);
			return(true);
		}
	}
	return(false);
}

void CVisionVelodyneCont::removeAll()
{
	for (int i=0;i<int(_allObjects.size());i++)
		delete _allObjects[i];
	_allObjects.clear();
}

CVisionVelodyne* CVisionVelodyneCont::getObject(int velodyneHandle)
{
	for (int i=0;i<int(_allObjects.size());i++)
	{
		if (_allObjects[i]->getVelodyneHandle()==velodyneHandle)
			return(_allObjects[i]);
	}
	return(0);
}
