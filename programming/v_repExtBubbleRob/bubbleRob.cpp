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
// This class represents a BubbleRob inside this
// plugin. Each BubbleRob model in V-REP
// should have its corresponding CBubbleRob object
// in this plugin. If a BubbleRob model
// in V-REP is removed (i.e. erased), then its
// corresponding CBubbleRob object should also be
// removed. Same when a new BubbleRob model is added
//**************************************************

#include "bubbleRob.h"
#include "access.h"
#include "v_repLib.h"

CBubbleRob::CBubbleRob()
{
	_id=-1;
	_associatedObjectID=-1;
	_associatedObjectUniqueID=-1;
	_initialized=false;
}

CBubbleRob::~CBubbleRob()
{
}

void CBubbleRob::setID(int newID)
{
	_id=newID;
}

int CBubbleRob::getID()
{
	return(_id);
}

void CBubbleRob::setAssociatedObject(int objID,int objUniqueID)
{
	_associatedObjectID=objID;
	_associatedObjectUniqueID=objUniqueID;
}

int CBubbleRob::getAssociatedObject()
{
	return(_associatedObjectID);
}

int CBubbleRob::getAssociatedObjectUniqueId()
{
	return(_associatedObjectUniqueID);
}

void CBubbleRob::synchronizeSceneObject()
{ // We update CBubbleRob's associated scene object custom data:
	putBubbleRobTagToSceneObject(_associatedObjectID,_maxVelocity);
}

void CBubbleRob::synchronize()
{ // We update CBubbleRob's data from its associated scene object custom data:
	// 1. Get all the developer data attached to the associated scene object (this is custom data added by the developer):
	int buffSize=simGetObjectCustomDataLength(_associatedObjectID,DEVELOPER_DATA_HEADER);
	char* datBuff=new char[buffSize];
	simGetObjectCustomData(_associatedObjectID,DEVELOPER_DATA_HEADER,datBuff);
	std::vector<unsigned char> developerCustomData(datBuff,datBuff+buffSize);
	delete[] datBuff;
	// 2. From that retrieved data, try to extract sub-data with the BUBBLEROB_DATA_MAXVELOCITY tag:
	std::vector<unsigned char> bubbleRobVelocityData;
	if (CAccess::extractSerializationData(developerCustomData,BUBBLEROB_DATA_MAXVELOCITY,bubbleRobVelocityData))
	{ // Yes, the tag is there. For now we only have to synchronize _maxVelocity:
		_maxVelocity=CAccess::pop_float(bubbleRobVelocityData);
	}
}

void CBubbleRob::setMaxVelocity(float maxVel)
{
	_maxVelocity=maxVel;
	if (_maxVelocity>2.0f*3.1415f)
		_maxVelocity=2.0f*3.1415f; // limit it to max 80 RPM
	if (_maxVelocity<3.1415f/8.0f)
		_maxVelocity=3.1415f/8.0f; // limit it to min 5 RPM
	synchronizeSceneObject(); // make sure the associated scene object has the same values stored as this
}

float CBubbleRob::getMaxVelocity()
{
	return(_maxVelocity);
}

void CBubbleRob::putBubbleRobTagToSceneObject(int objectHandle,float maxVelocity)
{	// This creates/updates a BUBBLEROB_DATA_MAXVELOCITY tag
	// 1. Get all the developer data attached to the associated scene object (this is custom data added by the developer):
	int buffSize=simGetObjectCustomDataLength(objectHandle,DEVELOPER_DATA_HEADER);
	char* datBuff=new char[buffSize];
	simGetObjectCustomData(objectHandle,DEVELOPER_DATA_HEADER,datBuff);
	std::vector<unsigned char> developerCustomData(datBuff,datBuff+buffSize);
	delete[] datBuff;
	// 2. From that retrieved data, extract sub-data with the BUBBLEROB_DATA_MAXVELOCITY tag, update it, and add it back to the retrieved data:
	std::vector<unsigned char> bubbleRobVelocityData;
	CAccess::extractSerializationData(developerCustomData,BUBBLEROB_DATA_MAXVELOCITY,bubbleRobVelocityData);
	bubbleRobVelocityData.clear(); // we discard the old value (if present)
	CAccess::push_float(bubbleRobVelocityData,maxVelocity); // we replace it with the new value
	CAccess::insertSerializationData(developerCustomData,BUBBLEROB_DATA_MAXVELOCITY,bubbleRobVelocityData);
	// 3. We add/update the scene object with the updated custom data:
	simAddObjectCustomData(objectHandle,DEVELOPER_DATA_HEADER,(simChar*)&developerCustomData[0],int(developerCustomData.size()));
}

void CBubbleRob::startOfSimulation()
{
	_initialize();
}

bool CBubbleRob::endOfSimulation()
{
	_initialized=false;
	return(false); // We don't want this object automatically destroyed at the end of simulation
}

void CBubbleRob::handleSimulation()
{ // called when the main script calls: simHandleModule
	_initialize();	
	if (_movementDuration>0.0f)
	{ // movement mode
		if (simReadProximitySensor(_handleOfProximitySensor,NULL,NULL,NULL)>0)
			_backMovementDuration=3.0f; // we detected an obstacle, we move backward for 3 seconds
		if (_backMovementDuration>0.0f)
		{ // We move backward
			simSetJointTargetVelocity(_handleOfLeftJoint,-_maxVelocity*_backRelativeVelocities[0]);
			simSetJointTargetVelocity(_handleOfRightJoint,-_maxVelocity*_backRelativeVelocities[1]);
			_backMovementDuration-=simGetSimulationTimeStep();
		}
		else
		{ // We move forward
			simSetJointTargetVelocity(_handleOfLeftJoint,_maxVelocity);
			simSetJointTargetVelocity(_handleOfRightJoint,_maxVelocity);
		}
		_movementDuration-=simGetSimulationTimeStep();
		if ((_movementDuration<=0.0f)&&(_setToZeroAtMovementEnd!=NULL))
		{
			_setToZeroAtMovementEnd[0]=0;	// This will unblock the thread that called simExtBubbleMoveAndAvoid(x,y,false)
											// Above memory location can only be manipulated if we are sure the simulation is still running!!
			_setToZeroAtMovementEnd=NULL;
		}
	}
	else
	{ // stopped mode
		simSetJointTargetVelocity(_handleOfLeftJoint,0.0f);
		simSetJointTargetVelocity(_handleOfRightJoint,0.0f);
	}
}

void CBubbleRob::moveAndAvoid(float duration,float backRelativeVelocities[2],char* setToZeroAtMovementEnd)
{ // This starts a movement sequence "MoveAndAvoid". For the actual movement, see the 'handleSimulation' routine
	_initialize();
	if (duration>0.0f)
		_movementDuration=duration;
	_backRelativeVelocities[0]=backRelativeVelocities[0];
	_backRelativeVelocities[1]=backRelativeVelocities[1];
	if (setToZeroAtMovementEnd[0]!=0)
		_setToZeroAtMovementEnd=setToZeroAtMovementEnd; // If we called the function in a blocking fashion (simExtBubbleMoveAndAvoid(x,y,false))
}

void CBubbleRob::stop()
{ // This stops a "MoveAndAvoid" movement sequence
	_initialize();
	_movementDuration=0.0f;
	_backMovementDuration=0.0f;
}

void CBubbleRob::_initialize()
{
	if (_initialized)
		return;
	// We need to find the handle of BubbleRob's left/right motors, and proximity sensor. We know that
	// the left motor has a BUBBLEROB_DATA_LEFTMOTOR-tag, the right motor has a BUBBLEROB_DATA_RIGHTMOTOR-tag,
	// the proximity sensor has a BUBBLEROB_DATA_SENSOR-tag. Since there might be several BubbleRobs in the
	// scene, we should only explore this BubbleRob's tree hierarchy to find those tags:
	_handleOfLeftJoint=-1;
	_handleOfRightJoint=-1;
	_handleOfProximitySensor=-1;
	std::vector<int> toExplore;
	toExplore.push_back(_associatedObjectID); // We start exploration with the base of the BubbleRob-tree
	while (toExplore.size()!=0)
	{
		int objHandle=toExplore[toExplore.size()-1];
		toExplore.pop_back();
		// 1. Add this object's children to the list to explore:
		int index=0;
		int childHandle=simGetObjectChild(objHandle,index++);
		while (childHandle!=-1)
		{
			toExplore.push_back(childHandle);
			childHandle=simGetObjectChild(objHandle,index++);
		}
		// 2. Now check if this object has one of the tags we are looking for:
		// a. Get all the developer data attached to this scene object (this is custom data added by the developer):
		int buffSize=simGetObjectCustomDataLength(objHandle,DEVELOPER_DATA_HEADER);
		if (buffSize!=0)
		{ // Yes there is some custom data written by us (the developer with the DEVELOPER_DATA_HEADER header)
			char* datBuff=new char[buffSize];
			simGetObjectCustomData(objHandle,DEVELOPER_DATA_HEADER,datBuff);
			std::vector<unsigned char> developerCustomData(datBuff,datBuff+buffSize);
			delete[] datBuff;
			// b. From that retrieved data, try to extract sub-data with the searched tags:
			std::vector<unsigned char> bubbleRobTagData;
			if (CAccess::extractSerializationData(developerCustomData,BUBBLEROB_DATA_LEFTMOTOR,bubbleRobTagData))
				_handleOfLeftJoint=objHandle; // We found the BUBBLEROB_DATA_LEFTMOTOR tag. This is the left motor!
			if (CAccess::extractSerializationData(developerCustomData,BUBBLEROB_DATA_RIGHTMOTOR,bubbleRobTagData))
				_handleOfRightJoint=objHandle; // We found the BUBBLEROB_DATA_RIGHTMOTOR tag. This is the right motor!
			if (CAccess::extractSerializationData(developerCustomData,BUBBLEROB_DATA_SENSOR,bubbleRobTagData))
				_handleOfProximitySensor=objHandle; // We found the BUBBLEROB_DATA_SENSOR tag. This is the proximity sensor!
		}
	}
	_movementDuration=0.0f;		// Start in a stopped state
	_backMovementDuration=0.0f; // Forward
	_backRelativeVelocities[0]=0.5f;  // default value
	_backRelativeVelocities[1]=0.25f; // default value
	_setToZeroAtMovementEnd=NULL;
	_initialized=true;
}
