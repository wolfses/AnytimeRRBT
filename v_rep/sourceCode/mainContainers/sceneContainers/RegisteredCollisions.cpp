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
#include "RegisteredCollisions.h"
#include "Tt.h"
#include "IloIlo.h"
#include "OGL.h"
#include "CollisionRoutine.h"
#include "App.h"
#include "sensingThreadPool.h"

CRegisteredCollisions::CRegisteredCollisions()
{
	setUpDefaultValues();
}

CRegisteredCollisions::~CRegisteredCollisions()
{
	removeAllCollisionObjects();
}

void CRegisteredCollisions::simulationAboutToStart()
{
	for (int i=0;i<int(collisionObjects.size());i++)
		collisionObjects[i]->simulationAboutToStart();
}

void CRegisteredCollisions::simulationEnded()
{
	for (int i=0;i<int(collisionObjects.size());i++)
		collisionObjects[i]->simulationEnded();
}

void CRegisteredCollisions::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{
	if (displayAttrib&sim_displayattribute_renderpass)
		displayCollisionContours();
}

void CRegisteredCollisions::getMinAndMaxNameSuffixes(int& minSuffix,int& maxSuffix)
{
	minSuffix=-1;
	maxSuffix=-1;
	for (int i=0;i<int(collisionObjects.size());i++)
	{
		int s=tt::getNameSuffixNumber(collisionObjects[i]->getObjectName().c_str(),true);
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

bool CRegisteredCollisions::canSuffix1BeSetToSuffix2(int suffix1,int suffix2)
{
	for (int i=0;i<int(collisionObjects.size());i++)
	{
		int s1=tt::getNameSuffixNumber(collisionObjects[i]->getObjectName().c_str(),true);
		if (s1==suffix1)
		{
			std::string name1(tt::getNameWithoutSuffixNumber(collisionObjects[i]->getObjectName().c_str(),true));
			for (int j=0;j<int(collisionObjects.size());j++)
			{
				int s2=tt::getNameSuffixNumber(collisionObjects[j]->getObjectName().c_str(),true);
				if (s2==suffix2)
				{
					std::string name2(tt::getNameWithoutSuffixNumber(collisionObjects[j]->getObjectName().c_str(),true));
					if (name1==name2)
						return(false); // NO! We would have a name clash!
				}
			}
		}
	}
	return(true);
}

void CRegisteredCollisions::setSuffix1ToSuffix2(int suffix1,int suffix2)
{
	for (int i=0;i<int(collisionObjects.size());i++)
	{
		int s1=tt::getNameSuffixNumber(collisionObjects[i]->getObjectName().c_str(),true);
		if (s1==suffix1)
		{
			std::string name1(tt::getNameWithoutSuffixNumber(collisionObjects[i]->getObjectName().c_str(),true));
			collisionObjects[i]->setObjectName(tt::generateNewName_dash(name1,suffix2+1));
		}
	}
}

void CRegisteredCollisions::addObject(CRegColl* newCollObj,bool objectIsACopy)
{
	addObjectWithSuffixOffset(newCollObj,objectIsACopy,1);
}

void CRegisteredCollisions::addObjectWithSuffixOffset(CRegColl* newCollObj,bool objectIsACopy,int suffixOffset)
{	// Here we don't check whether such an object already exists or is valid.
	// This routine is mainly used for loading and copying operations
	// We check if that name already exists:
	std::string oName=newCollObj->getObjectName();
	if (oName.length()==0)
		oName="Collision";
	if (objectIsACopy)
		oName=tt::generateNewName_dash(oName,suffixOffset);
	else
	{
		while (getObject(oName)!=NULL)
			oName=tt::generateNewName_noDash(oName);
	}
	newCollObj->setObjectName(oName);
	// We find a free ID:
	int id=SIM_IDSTART_COLLISION;
	while (getObject(id)!=NULL)
		id++;
	newCollObj->setObjectID(id);
	collisionObjects.push_back(newCollObj);
	App::ct->objCont->setFullDialogRefreshFlag();
}

int CRegisteredCollisions::addNewObject(int obj1ID,int obj2ID,std::string objName)
{
	// We check if the objects are valid:
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
	// We check if we try to check an object against itself (forbidden, except for collections):
	if ( (obj1ID<SIM_IDSTART_COLLECTION)&&(obj2ID<SIM_IDSTART_COLLECTION) )
	{
		if (obj1ID==obj2ID)
			return(-1);
	}
	// We check if such an object already exists:
	for (int i=0;i<int(collisionObjects.size());i++)
	{
		if (collisionObjects[i]->isSame(obj1ID,obj2ID))
			return(-1);
	}
	// We create and insert the object
	CRegColl* newCollObject=new CRegColl(obj1ID,obj2ID,objName,SIM_IDSTART_COLLISION);
	addObject(newCollObject,false);
	return(newCollObject->getObjectID());
}
bool CRegisteredCollisions::removeObject(int objID)
{
	App::ct->objCont->announceCollisionWillBeErased(objID);
	for (int i=0;i<int(collisionObjects.size());i++)
	{
		if (collisionObjects[i]->getObjectID()==objID)
		{
			delete collisionObjects[i];
			collisionObjects.erase(collisionObjects.begin()+i);
			App::ct->objCont->setFullDialogRefreshFlag();
			return(true);	// We could remove the object
		}
	}
	return(false);	// The object don't exist
}
CRegColl* CRegisteredCollisions::getObject(int objID)
{
	for (int i=0;i<int(collisionObjects.size());i++)
	{
		if (collisionObjects[i]->getObjectID()==objID)
			return(collisionObjects[i]);
	}
	return(NULL);
}
CRegColl* CRegisteredCollisions::getObject(std::string objName)
{
	for (int i=0;i<int(collisionObjects.size());i++)
	{
		if (collisionObjects[i]->getObjectName().compare(objName)==0)
			return(collisionObjects[i]);
	}
	return(NULL);
}
void CRegisteredCollisions::removeAllCollisionObjects()
{
	while (collisionObjects.size()!=0)
		removeObject(collisionObjects[0]->getObjectID());
}

void CRegisteredCollisions::announceGroupWillBeErased(int groupID)
{ // Never called from the copy buffer!
	int i=0;
	while (i<int(collisionObjects.size()))
	{
		if (collisionObjects[i]->announceGroupWillBeErased(groupID,false))
		{ // We have to remove this collision object
			removeObject(collisionObjects[i]->getObjectID()); // This will call announceCollisionWillBeErased!
			i=0; // Ordering may have changed
		}
		else
			i++;
	}
}


void CRegisteredCollisions::announceObjectWillBeErased(int objID)
{ // Never called from copy buffer!
	int i=0;
	while (i<int(collisionObjects.size()))
	{
		if (collisionObjects[i]->announceObjectWillBeErased(objID,false))
		{ // We have to remove this collision object
			removeObject(collisionObjects[i]->getObjectID()); // This will call announceCollisionWillBeErased
			i=0; // Ordering may have changed
		}
		else
			i++;
	}
}

void CRegisteredCollisions::setUpDefaultValues()
{
	removeAllCollisionObjects();
}

bool CRegisteredCollisions::setObjectName(int objID,std::string newName)
{
	CRegColl* it=getObject(newName);
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

int CRegisteredCollisions::getCollisionColor(int entityID)
{
	int retVal=0;
	for (int i=0;i<int(collisionObjects.size());i++)
		retVal|=collisionObjects[i]->getCollisionColor(entityID);
	return(retVal);
}

void CRegisteredCollisions::resetAllCollisions(bool exceptExplicitHandling)
{
	for (int i=0;i<int(collisionObjects.size());i++)
	{
		if ( (!collisionObjects[i]->getExplicitHandling())||(!exceptExplicitHandling) )
			collisionObjects[i]->clearCollisionResult();
	}
}

int CRegisteredCollisions::handleAllCollisions(bool exceptExplicitHandling)
{
	if (CSensingThreadPool::getThreadCount()==0)
	{ // sequential handling
		int retVal=0;
		for (int i=0;i<int(collisionObjects.size());i++)
		{
			if ( (!collisionObjects[i]->getExplicitHandling())||(!exceptExplicitHandling) )
			{
				if (collisionObjects[i]->handleCollision(false))
					retVal++;
			}
		}
		return(retVal);
	}
	else
	{ // Concurrent handling!
		for (int i=0;i<int(collisionObjects.size());i++)
		{
			SParallelWorkDat e;
			e.object=collisionObjects[i];
			e.objectType=0; // collision object
			e.exceptExplicit=exceptExplicitHandling;
			e.handlingStarted=false;
			e.freeSlot=false;
			CSensingThreadPool::addTask(e);
		}
		return(0);
	}
}

void CRegisteredCollisions::displayCollisionContours()
{
	for (int i=0;i<int(collisionObjects.size());i++)
		collisionObjects[i]->displayCollisionContour();
}
