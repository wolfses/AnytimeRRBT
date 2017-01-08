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

//**************************************************
// This class represents a container for CMtbRobot
// objects.
// This container should make sure that if a MTB
// model in a V-REP scene is added/removed, that
// its corresponding CMtbRobot object is also
// added/removed.
//**************************************************

#include "mtbRobotContainer.h"
#include "Access.h"
#include "v_repLib.h"
#include "mtbGlobal.h"

CMtbRobotContainer::CMtbRobotContainer()
{
}

CMtbRobotContainer::~CMtbRobotContainer()
{
	removeAll();	
}

void CMtbRobotContainer::removeAll()
{
	for (int i=0;i<getCount();i++)
		delete _allMtbRobots[i];
	_allMtbRobots.clear();
}

void CMtbRobotContainer::removeFromID(int theID)
{
	for (int i=0;i<getCount();i++)
	{
		if (_allMtbRobots[i]->getID()==theID)
		{
			delete _allMtbRobots[i];
			_allMtbRobots.erase(_allMtbRobots.begin()+i);
			break;
		}
	}
}

int CMtbRobotContainer::insert(CMtbRobot* theMtbRobot)
{
	int newID=0;
	while (getFromID(newID)!=NULL)
		newID++;
	_allMtbRobots.push_back(theMtbRobot);
	theMtbRobot->setID(newID);
	return(newID);
}

CMtbRobot* CMtbRobotContainer::getFromID(int theID)
{
	for (int i=0;i<getCount();i++)
	{
		if (_allMtbRobots[i]->getID()==theID)
			return(_allMtbRobots[i]);
	}
	return(NULL);
}

CMtbRobot* CMtbRobotContainer::getFromIndex(int ind)
{
	if ( (ind<0)||(ind>=getCount()) )
		return(NULL);
	return(_allMtbRobots[ind]);
}

CMtbRobot* CMtbRobotContainer::getFromAssociatedObject(int theAssociatedObjectID)
{
	for (int i=0;i<getCount();i++)
	{
		if (_allMtbRobots[i]->getAssociatedObject()==theAssociatedObjectID)
			return(_allMtbRobots[i]);
	}
	return(NULL);
}


int CMtbRobotContainer::getCount()
{
	return(int(_allMtbRobots.size()));
}

void CMtbRobotContainer::startOfSimulation()
{ // Relay the message to all CMtbRobot objects:
	for (int i=0;i<getCount();i++)
		_allMtbRobots[i]->startOfSimulation();
}

void CMtbRobotContainer::endOfSimulation()
{ // Relay the message to all CMtbRobot objects:
	for (int i=0;i<getCount();i++)
		_allMtbRobots[i]->endOfSimulation();
}

void CMtbRobotContainer::actualizeForSceneContent()
{	// This is called everytime the scene content in V-REP might have changed (e.g. object added/removed/etc.)
	// We need to make sure that every MTB model in V-REP's scene has exactly one CMtbRobot in this container.
	// We now synchronize this container with current V-REP's scene content:

	// 1. We first check which CMtbRobot is not valid anymore, and remove it
	int i=0;
	while (i<int(_allMtbRobots.size()))
	{
		CMtbRobot* mtbRobot=_allMtbRobots[i];
		int associatedObject=mtbRobot->getAssociatedObject(); // this is the scene object handle
		int uniqueID;
		if (simGetObjectUniqueIdentifier(associatedObject,&uniqueID)==-1)
		{ // the scene object doesn't exist anymore!
			removeFromID(mtbRobot->getID()); // we remove its associated CMtbRobot object
			i=-1; // ordering in _allMtbRobots might have changed, we start from the beginning
		}
		else
		{ // An object with that handle exists, is its unique ID same as getAssociatedObjectUniqueId()? (a scene object handle can be re-used!)
			if (uniqueID!=mtbRobot->getAssociatedObjectUniqueId())
			{ // the object doesn't exist anymore! (the object was destroyed and then another object was assigned the same handle (e.g. new scene was loaded))
				removeFromID(mtbRobot->getID());
				i=-1; // ordering in _allMtbRobots might have changed, we start from the beginning
			}
			else
			{	// the object still exists. Does it still have its MTB_BASE tag?
				// a. Get all the developer data attached to this object (this is custom data added by the developer):
				int buffSize=simGetObjectCustomDataLength(associatedObject,DEVELOPER_DATA_HEADER);
				char* datBuff=new char[buffSize];
				simGetObjectCustomData(associatedObject,DEVELOPER_DATA_HEADER,datBuff);
				std::vector<unsigned char> developerCustomData(datBuff,datBuff+buffSize);
				delete[] datBuff;
				// b. From that retrieved data, try to extract sub-data with the MTB_BASE tag:
				std::vector<unsigned char> mtbData;
				if (!CAccess::extractSerializationData(developerCustomData,MTB_BASE,mtbData))
				{ // No, there is no data with the MTB_BASE tag present! We remove this object:
					removeFromID(mtbRobot->getID());
					i=-1; // ordering in _allMtbRobots might have changed, we start from the beginning
				}
			}
		}
		i++;
	}

	// 2. Now all CMtbRobot objects in this container are up-to-date. We now go through all objects in the scene
	// that have the MTB_BASE tag, and check if they have a related CMtbRobot object in this container.
	// If not, we have to add a related CMtbRobot object:

	int objHandle=0;
	int ind=0;
	while (objHandle!=-1)
	{
		objHandle=simGetObjects(ind++,sim_handle_all);
		if (objHandle!=-1)
		{ // Scene object present at index ind
			// Is that scene object already associated with a CMtbRobot object?
			if (getFromAssociatedObject(objHandle)==NULL)
			{ // No, not yet
				// Does the object have a MTB_BASE tag?
				// a. Get all the developer data attached to this object (this is custom data added by the developer):
				int buffSize=simGetObjectCustomDataLength(objHandle,DEVELOPER_DATA_HEADER);
				char* datBuff=new char[buffSize];
				simGetObjectCustomData(objHandle,DEVELOPER_DATA_HEADER,datBuff);
				std::vector<unsigned char> developerCustomData(datBuff,datBuff+buffSize);
				delete[] datBuff;
				// b. From that retrieved data, try to extract sub-data with the MTB_BASE tag:
				std::vector<unsigned char> mtbData;
				if (CAccess::extractSerializationData(developerCustomData,MTB_BASE,mtbData))
				{ // Yes, the tag is there. We have to add a new CMtbRobot object associated with this scene object:
					CMtbRobot* mtbRobot=new CMtbRobot();
					insert(mtbRobot);
					int uniqueID;
					simGetObjectUniqueIdentifier(objHandle,&uniqueID);
					mtbRobot->setAssociatedObject(objHandle,uniqueID);
					mtbRobot->readCCustomDataFromSceneObject(); // Make sure the CMtbRobot object has the same values
				}
			}
		}
	}		
}
