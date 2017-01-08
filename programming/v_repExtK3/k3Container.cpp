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
// This class represents a container for CK3
// objects. 
// This container should make sure that if a K3
// model in a V-REP scene is added/removed, that
// its corresponding CK3 object is also
// added/removed.
//**************************************************

#include "stdafx.h"
#include "k3Container.h"
#include "access.h"
#include "v_repLib.h"

CK3Container::CK3Container()
{
}

CK3Container::~CK3Container()
{
	removeAll();	
}

void CK3Container::removeAll()
{
	for (int i=0;i<getCount();i++)
		delete _allK3s[i];
	_allK3s.clear();
}

void CK3Container::removeFromID(int theID)
{
	for (int i=0;i<getCount();i++)
	{
		if (_allK3s[i]->getID()==theID)
		{
			delete _allK3s[i];
			_allK3s.erase(_allK3s.begin()+i);
			break;
		}
	}
}

int CK3Container::insert(CK3* theK3)
{
	int newID=0;
	while (getFromID(newID)!=NULL)
		newID++;
	_allK3s.push_back(theK3);
	theK3->setID(newID);
	return(newID);
}

CK3* CK3Container::getFromID(int theID)
{
	for (int i=0;i<getCount();i++)
	{
		if (_allK3s[i]->getID()==theID)
			return(_allK3s[i]);
	}
	return(NULL);
}

CK3* CK3Container::getFromIndex(int ind)
{
	if ( (ind<0)||(ind>=getCount()) )
		return(NULL);
	return(_allK3s[ind]);
}

CK3* CK3Container::getFromAssociatedObject(int theAssociatedObjectID)
{
	for (int i=0;i<getCount();i++)
	{
		if (_allK3s[i]->getAssociatedObject()==theAssociatedObjectID)
			return(_allK3s[i]);
	}
	return(NULL);
}


int CK3Container::getCount()
{
	return(int(_allK3s.size()));
}

void CK3Container::startOfSimulation()
{ // Relay the message to all CK3 objects:
	for (int i=0;i<getCount();i++)
		_allK3s[i]->startOfSimulation();
}

void CK3Container::endOfSimulation()
{ // Relay the message to all CK3 objects:
	for (int i=0;i<getCount();i++)
	{
		if (_allK3s[i]->endOfSimulation())
		{ // Means that this object wants to be destroyed
			delete _allK3s[i];
			_allK3s.erase(_allK3s.begin()+i);
			i--;
		}
	}
}

void CK3Container::handleSimulation()
{ // Relay the message to all CK3 objects:
	for (int i=0;i<getCount();i++)
		_allK3s[i]->handleSimulation();
}

void CK3Container::scalingEvent()
{ // Relay the message to all CK3 objects:
	for (int i=0;i<getCount();i++)
		_allK3s[i]->checkScale();
}

void CK3Container::actualizeForSceneContent()
{	// This is called everytime the scene content in V-REP might have changed (e.g. object added/removed/etc.)
	// We need to make sure every K3 in V-REP's scene has exactly one CK3 in this container.
	// We now synchronize this container with current V-REP's scene content:

	// 1. We first check which CK3 is not valid anymore, and remove it
	int i=0;
	while (i<int(_allK3s.size()))
	{
		CK3* k3=_allK3s[i];
		int associatedObject=k3->getAssociatedObject();
		int uniqueID;
		if (simGetObjectUniqueIdentifier(associatedObject,&uniqueID)==-1)
		{ // the object doesn't exist anymore!
			removeFromID(k3->getID());
			i=-1; // ordering in _allK3s might have changed, we start from the beginning
		}
		else
		{ // An object with that handle exists, is its unique ID same as getAssociatedObjectUniqueId()?
			if (uniqueID!=k3->getAssociatedObjectUniqueId())
			{ // the object doesn't exist anymore! (the object was destroyed and then another object was assigned the same handle (e.g. new scene was loaded))
				removeFromID(k3->getID());
				i=-1; // ordering in _allK3s might have changed, we start from the beginning
			}
			else
			{	// the object still exists. Does it still have its K3_BASE tag?
				// a. Get all the developer data attached to this object (this is custom data added by the developer):
				int buffSize=simGetObjectCustomDataLength(associatedObject,DEVELOPER_DATA_HEADER);
				char* datBuff=new char[buffSize];
				simGetObjectCustomData(associatedObject,DEVELOPER_DATA_HEADER,datBuff);
				std::vector<unsigned char> developerCustomData(datBuff,datBuff+buffSize);
				delete[] datBuff;
				// b. From that retrieved data, try to extract sub-data with the K3_BASE tag:
				std::vector<unsigned char> k3Data;
				if (!CAccess::extractSerializationData(developerCustomData,K3_BASE,k3Data))
				{ // No, there is no data with the K3_BASE tag present! We remove this object:
					removeFromID(k3->getID());
					i=-1; // ordering in _allK3s might have changed, we start from the beginning
				}
			}
		}
		i++;
	}

	// 2. Now all CK3 objects in this container are up-to-date. We now go through all objects in the scene
	// that have the K3_BASE tag, and check if they have a related CK3 object in this container.
	// If not, we have to add a related CK3 object:

	int objHandle=0;
	int ind=0;
	while (objHandle!=-1)
	{
		objHandle=simGetObjects(ind++,sim_handle_all);
		if (objHandle!=-1)
		{ // Scene object present at index ind
			// Is that scene object already associated with a CK3 object?
			if (getFromAssociatedObject(objHandle)==NULL)
			{ // No, not yet
				// Does the object have a K3_BASE tag?
				// a. Get all the developer data attached to this object (this is custom data added by the developer):
				int buffSize=simGetObjectCustomDataLength(objHandle,DEVELOPER_DATA_HEADER);
				char* datBuff=new char[buffSize];
				simGetObjectCustomData(objHandle,DEVELOPER_DATA_HEADER,datBuff);
				std::vector<unsigned char> developerCustomData(datBuff,datBuff+buffSize);
				delete[] datBuff;
				// b. From that retrieved data, try to extract sub-data with the K3_BASE tag:
				std::vector<unsigned char> k3Data;
				if (CAccess::extractSerializationData(developerCustomData,K3_BASE,k3Data))
				{ // Yes, the tag is there. We have to add a new CK3 object associated with this scene object:
					CK3* k3=new CK3();
					insert(k3);
					int uniqueID;
					simGetObjectUniqueIdentifier(objHandle,&uniqueID);
					k3->setAssociatedObject(objHandle,uniqueID);
					k3->synchronize(); // Make sure the CK3 object has the same values
				}
			}
		}
	}		
}
