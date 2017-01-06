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

#include "vrepPrecompiledHeader.h"
#include "v_rep_internal.h"
#include "RegisteredDistances.h"
#include "Tt.h"
#include "IloIlo.h"
#include "OGL.h"
#include "DistanceRoutine.h"
#include "App.h"
#include "GV.h"
#include "sensingThreadPool.h"

CRegisteredDistances::CRegisteredDistances()
{
	setUpDefaultValues();
}

CRegisteredDistances::~CRegisteredDistances()
{
	removeAllDistanceObjects();
}

void CRegisteredDistances::simulationAboutToStart()
{
	for (int i=0;i<int(distanceObjects.size());i++)
		distanceObjects[i]->simulationAboutToStart();
}

void CRegisteredDistances::simulationEnded()
{
	for (int i=0;i<int(distanceObjects.size());i++)
		distanceObjects[i]->simulationEnded();
}

void CRegisteredDistances::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{
	if (displayAttrib&sim_displayattribute_renderpass)
		displayDistanceSegments();
}

void CRegisteredDistances::getMinAndMaxNameSuffixes(int& minSuffix,int& maxSuffix)
{
	minSuffix=-1;
	maxSuffix=-1;
	for (int i=0;i<int(distanceObjects.size());i++)
	{
		int s=tt::getNameSuffixNumber(distanceObjects[i]->getObjectName().c_str(),true);
		if (i==0)
		{
			minSuffix=s;
			maxSuffix=s;
		}
		else
		{
			if (s<minSuffix)
				minSuffix=s;
			if (s>maxSuffix)
				maxSuffix=s;
		}
	}
}

bool CRegisteredDistances::canSuffix1BeSetToSuffix2(int suffix1,int suffix2)
{
	for (int i=0;i<int(distanceObjects.size());i++)
	{
		int s1=tt::getNameSuffixNumber(distanceObjects[i]->getObjectName().c_str(),true);
		if (s1==suffix1)
		{
			std::string name1(tt::getNameWithoutSuffixNumber(distanceObjects[i]->getObjectName().c_str(),true));
			for (int j=0;j<int(distanceObjects.size());j++)
			{
				int s2=tt::getNameSuffixNumber(distanceObjects[j]->getObjectName().c_str(),true);
				if (s2==suffix2)
				{
					std::string name2(tt::getNameWithoutSuffixNumber(distanceObjects[j]->getObjectName().c_str(),true));
					if (name1==name2)
						return(false); // NO! We would have a name clash!
				}
			}
		}
	}
	return(true);
}

void CRegisteredDistances::setSuffix1ToSuffix2(int suffix1,int suffix2)
{
	for (int i=0;i<int(distanceObjects.size());i++)
	{
		int s1=tt::getNameSuffixNumber(distanceObjects[i]->getObjectName().c_str(),true);
		if (s1==suffix1)
		{
			std::string name1(tt::getNameWithoutSuffixNumber(distanceObjects[i]->getObjectName().c_str(),true));
			distanceObjects[i]->setObjectName(tt::generateNewName_dash(name1,suffix2+1));
		}
	}
}

void CRegisteredDistances::addObject(CRegDist* newDistObj,bool objectIsACopy)
{
	addObjectWithSuffixOffset(newDistObj,objectIsACopy,1);
}

void CRegisteredDistances::addObjectWithSuffixOffset(CRegDist* newDistObj,bool objectIsACopy,int suffixOffset)
{	// Here we don't check whether such an object already exists or is valid.
	// This routine is mainly used for loading and copying operations
	// We check if that name already exists:
	std::string oName=newDistObj->getObjectName();
	if (oName.length()==0)
		oName="Distance";
	if (objectIsACopy)
		oName=tt::generateNewName_dash(oName,suffixOffset);
	else
	{
		while (getObject(oName)!=NULL)
			oName=tt::generateNewName_noDash(oName);
	}
	newDistObj->setObjectName(oName);
	// We find a free ID:
	int id=SIM_IDSTART_DISTANCE;
	while (getObject(id)!=NULL)
		id++;
	newDistObj->setObjectID(id);
	distanceObjects.push_back(newDistObj);
	App::ct->objCont->setFullDialogRefreshFlag();
}

int CRegisteredDistances::addNewObject(int obj1ID,int obj2ID,
										std::string objName)
{	// We check if the objects are valid:
	if (obj1ID<SIM_IDSTART_COLLECTION)
	{
		if (App::ct->objCont->getObject(obj1ID)==NULL)
			return(-1);
	}
	else
	{
		if (App::ct->collections->getGroup(obj1ID)==NULL)
			return(-1);
	}
	if (obj2ID>=SIM_IDSTART_COLLECTION)
	{
		if (App::ct->collections->getGroup(obj2ID)==NULL)
			return(-1);
	}
	else
	{
		if ( (App::ct->objCont->getObject(obj2ID)==NULL)&&(obj2ID!=-1) )
			return(-1);
	}
	// We check if we try to measure an object against itself (forbidden, except for collections):
	if ( (obj1ID<SIM_IDSTART_COLLECTION)&&
		(obj2ID<SIM_IDSTART_COLLECTION) )
	{
		if (obj1ID==obj2ID)
			return(-1);
	}

	// We check if such an object already exists:
	for (int i=0;i<int(distanceObjects.size());i++)
	{
		if (distanceObjects[i]->isSame(obj1ID,obj2ID))
			return(-1);
	}

	CRegDist* newDistObject=new CRegDist(obj1ID,obj2ID,objName,SIM_IDSTART_DISTANCE);
	addObject(newDistObject,false);

	return(newDistObject->getObjectID());
}

bool CRegisteredDistances::removeObject(int objID)
{
	App::ct->objCont->announceDistanceWillBeErased(objID);
	for (int i=0;i<int(distanceObjects.size());i++)
	{
		if (distanceObjects[i]->getObjectID()==objID)
		{
			delete distanceObjects[i];
			distanceObjects.erase(distanceObjects.begin()+i);
			App::ct->objCont->setFullDialogRefreshFlag();
			return(true);	// We could remove the object
		}
	}
	return(false);	// The object doesn't exist
}

CRegDist* CRegisteredDistances::getObject(int objID)
{
	for (int i=0;i<int(distanceObjects.size());i++)
	{
		if (distanceObjects[i]->getObjectID()==objID)
			return(distanceObjects[i]);
	}
	return(NULL);
}

CRegDist* CRegisteredDistances::getObject(std::string objName)
{
	for (int i=0;i<int(distanceObjects.size());i++)
	{
		if (distanceObjects[i]->getObjectName().compare(objName)==0)
			return(distanceObjects[i]);
	}
	return(NULL);
}

void CRegisteredDistances::removeAllDistanceObjects()
{
	while (distanceObjects.size()!=0)
		removeObject(distanceObjects[0]->getObjectID());
}

void CRegisteredDistances::resetAllDistances(bool exceptExplicitHandling)
{
	for (int i=0;i<int(distanceObjects.size());i++)
	{
		if ( (!distanceObjects[i]->getExplicitHandling())||(!exceptExplicitHandling) )
			distanceObjects[i]->clearDistanceResult();
	}
}

void CRegisteredDistances::announceGroupWillBeErased(int groupID)
{ // never called from the copy buffer
	int i=0;
	while (i<int(distanceObjects.size()))
	{
		if (distanceObjects[i]->announceGroupWillBeErased(groupID,false))
		{ // We have to remove this distance object
			removeObject(distanceObjects[i]->getObjectID()); // This will call announceDistanceWillBeErased!
			i=0; // Ordering may have changed
		}
		else
			i++;
	}
}

void CRegisteredDistances::announceObjectWillBeErased(int objID)
{ // Never called from copy buffer!
	int i=0;
	while (i<int(distanceObjects.size()))
	{
		if (distanceObjects[i]->announceObjectWillBeErased(objID,false))
		{ // We have to remove this collision object
			removeObject(distanceObjects[i]->getObjectID()); // This will call announceDistanceWillBeErased!
			i=0; // Ordering may have changed
		}
		else
			i++;
	}
}

void CRegisteredDistances::setUpDefaultValues()
{
	removeAllDistanceObjects();
}

bool CRegisteredDistances::setObjectName(int objID,std::string newName)
{
	CRegDist* it=getObject(newName);
	if (it==NULL)
	{
		it=getObject(objID);
		if (it==NULL)
			return(false); // Failure
		it->setObjectName(newName);
		return(true); // Success
	}
	if (it->getObjectID()==objID)
		return(true); // Success
	return(false); // Failure
}

float CRegisteredDistances::handleAllDistances(bool exceptExplicitHandling)
{ // Return value is the smallest distance measured or negative if nothing was measured
	resetAllDistances(exceptExplicitHandling);
	if (!App::ct->mainSettings->distanceCalculationEnabled)
		return(-1.0f);

	if (CSensingThreadPool::getThreadCount()==0)
	{ // sequential handling
		float retVal=SIM_MAX_FLOAT;
		bool retPos=false;
		for (int i=0;i<int(distanceObjects.size());i++)
		{
			if ( (!distanceObjects[i]->getExplicitHandling())||(!exceptExplicitHandling) )
			{
				float d=distanceObjects[i]->handleDistance(false);
				if (d>=0.0f)
				{
					retPos=true;
					if (d<retVal)
						retVal=d;
				}
			}
		}
		if (!retPos)
			return(-1.0f);
		return(retVal);
	}
	else
	{ // Concurrent handling!
		for (int i=0;i<int(distanceObjects.size());i++)
		{
			SParallelWorkDat e;
			e.object=distanceObjects[i];
			e.objectType=1; // distance object
			e.exceptExplicit=exceptExplicitHandling;
			e.handlingStarted=false;
			e.freeSlot=false;
			CSensingThreadPool::addTask(e);
		}
		return(-1);
	}
}

void CRegisteredDistances::displayDistanceSegments()
{
	for (int i=0;i<int(distanceObjects.size());i++)
		distanceObjects[i]->displayDistanceSegment();
}
