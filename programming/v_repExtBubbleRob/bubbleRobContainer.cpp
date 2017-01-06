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
// This class represents a container for CBubbleRob
// objects.  
// This container should make sure that if a BubbleRob
// model in a V-REP instance is added/removed, that
// its corresponding CBubbleRob object is also
// added/removed.
//**************************************************

#include "bubbleRobContainer.h"
#include "access.h"
#include "v_repLib.h"

CBubbleRobContainer::CBubbleRobContainer()
{
}

CBubbleRobContainer::~CBubbleRobContainer()
{
	removeAll();	
}

void CBubbleRobContainer::removeAll()
{
	for (int i=0;i<getCount();i++)
		delete _allBubbleRobs[i];
	_allBubbleRobs.clear();
}

void CBubbleRobContainer::removeFromID(int theID)
{
	for (int i=0;i<getCount();i++)
	{
		if (_allBubbleRobs[i]->getID()==theID)
		{
			delete _allBubbleRobs[i];
			_allBubbleRobs.erase(_allBubbleRobs.begin()+i);
			break;
		}
	}
}

int CBubbleRobContainer::insert(CBubbleRob* theBubbleRob)
{
	int newID=0;
	while (getFromID(newID)!=NULL)
		newID++;
	_allBubbleRobs.push_back(theBubbleRob);
	theBubbleRob->setID(newID);
	return(newID);
}

CBubbleRob* CBubbleRobContainer::getFromID(int theID)
{
	for (int i=0;i<getCount();i++)
	{
		if (_allBubbleRobs[i]->getID()==theID)
			return(_allBubbleRobs[i]);
	}
	return(NULL);
}

CBubbleRob* CBubbleRobContainer::getFromIndex(int ind)
{
	if ( (ind<0)||(ind>=getCount()) )
		return(NULL);
	return(_allBubbleRobs[ind]);
}

CBubbleRob* CBubbleRobContainer::getFromAssociatedObject(int theAssociatedObjectID)
{
	for (int i=0;i<getCount();i++)
	{
		if (_allBubbleRobs[i]->getAssociatedObject()==theAssociatedObjectID)
			return(_allBubbleRobs[i]);
	}
	return(NULL);
}


int CBubbleRobContainer::getCount()
{
	return(int(_allBubbleRobs.size()));
}

void CBubbleRobContainer::startOfSimulation()
{ // Relay the message to all CBubbleRob objects:
	for (int i=0;i<getCount();i++)
		_allBubbleRobs[i]->startOfSimulation();
}

void CBubbleRobContainer::endOfSimulation()
{ // Relay the message to all CBubbleRob objects:
	for (int i=0;i<getCount();i++)
	{
		if (_allBubbleRobs[i]->endOfSimulation())
		{ // Means that this object wants to be destroyed
			delete _allBubbleRobs[i];
			_allBubbleRobs.erase(_allBubbleRobs.begin()+i);
			i--;
		}
	}
}

void CBubbleRobContainer::handleSimulation()
{ // Relay the message to all CBubbleRob objects:
	for (int i=0;i<getCount();i++)
		_allBubbleRobs[i]->handleSimulation();
}

void CBubbleRobContainer::actualizeForSceneContent()
{	// This is called everytime the scene content in V-REP might have changed (e.g. object added/removed/etc.)
	// We need to make sure every BubbleRob in V-REP's scene has exactly one CBubbleRob in this container.
	// We now synchronize this container with current V-REP's scene content:

	// 1. We first check which CBubbleRob is not valid anymore, and remove it
	int i=0;
	while (i<int(_allBubbleRobs.size()))
	{
		CBubbleRob* bubbleRob=_allBubbleRobs[i];
		int associatedObject=bubbleRob->getAssociatedObject();
		int uniqueID;
		if (simGetObjectUniqueIdentifier(associatedObject,&uniqueID)==-1)
		{ // the object doesn't exist anymore!
			removeFromID(bubbleRob->getID());
			i=-1; // ordering in _allBubbleRobs might have changed, we start from the beginning
		}
		else
		{ // An object with that handle exists, is its unique ID same as getAssociatedObjectUniqueId()?
			if (uniqueID!=bubbleRob->getAssociatedObjectUniqueId())
			{ // the object doesn't exist anymore! (the object was destroyed and then another object was assigned the same handle (e.g. new scene was loaded))
				removeFromID(bubbleRob->getID());
				i=-1; // ordering in _allBubbleRobs might have changed, we start from the beginning
			}
			else
			{	// the object still exists. Does it still have its BUBBLEROB_DATA_MAXVELOCITY tag?
				// a. Get all the developer data attached to this object (this is custom data added by the developer):
				int buffSize=simGetObjectCustomDataLength(associatedObject,DEVELOPER_DATA_HEADER);
				char* datBuff=new char[buffSize];
				simGetObjectCustomData(associatedObject,DEVELOPER_DATA_HEADER,datBuff);
				std::vector<unsigned char> developerCustomData(datBuff,datBuff+buffSize);
				delete[] datBuff;
				// b. From that retrieved data, try to extract sub-data with the BUBBLEROB_DATA_MAXVELOCITY tag:
				std::vector<unsigned char> bubbleRobVelocityData;
				if (!CAccess::extractSerializationData(developerCustomData,BUBBLEROB_DATA_MAXVELOCITY,bubbleRobVelocityData)) // the tag is composed by a string and an integer
				{ // No, there is no data with the BUBBLEROB_DATA_MAXVELOCITY tag present! We remove this object:
					removeFromID(bubbleRob->getID());
					i=-1; // ordering in _allBubbleRobs might have changed, we start from the beginning
				}
			}
		}
		i++;
	}

	// 2. Now all CBubbleRob objects in this container are up-to-date. We now go through all objects in the scene
	// that have the BUBBLEROB_DATA_MAXVELOCITY tag, and check if they have a related CBubbleRob object in this container.
	// If not, we have to add a related CBubbleRob object:

	int objHandle=0;
	int ind=0;
	while (objHandle!=-1)
	{
		objHandle=simGetObjects(ind++,sim_handle_all);
		if (objHandle!=-1)
		{ // Scene object present at index ind
			// Is that scene object already associated with a CBubbleRob object?
			if (getFromAssociatedObject(objHandle)==NULL)
			{ // No, not yet
				// Does the object have a BUBBLEROB_DATA_MAXVELOCITY tag?
				// a. Get all the developer data attached to this object (this is custom data added by the developer):
				int buffSize=simGetObjectCustomDataLength(objHandle,DEVELOPER_DATA_HEADER);
				char* datBuff=new char[buffSize];
				simGetObjectCustomData(objHandle,DEVELOPER_DATA_HEADER,datBuff);
				std::vector<unsigned char> developerCustomData(datBuff,datBuff+buffSize);
				delete[] datBuff;
				// b. From that retrieved data, try to extract sub-data with the BUBBLEROB_DATA_MAXVELOCITY tag:
				std::vector<unsigned char> bubbleRobVelocityData;
				if (CAccess::extractSerializationData(developerCustomData,BUBBLEROB_DATA_MAXVELOCITY,bubbleRobVelocityData))
				{ // Yes, the tag is there. We have to add a new CBubbleRob object associated with this scene object:
					CBubbleRob* bubbleRob=new CBubbleRob();
					insert(bubbleRob);
					int uniqueID;
					simGetObjectUniqueIdentifier(objHandle,&uniqueID);
					bubbleRob->setAssociatedObject(objHandle,uniqueID);
					bubbleRob->synchronize(); // Make sure the CBubbleRob object has the same values
				}
			}
		}
	}		
}
