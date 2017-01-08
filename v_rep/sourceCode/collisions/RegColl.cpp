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
#include "RegColl.h"
#include "3DObject.h"
#include "Global.h"
#include "CollisionRoutine.h"
#include "OGL.h"
#include "Tt.h"
#include "App.h"
#include "sensingThreadPool.h"
#include "IloIlo.h"
#include "v_repStrings.h"
#include "VDateTime.h"
#include "collDistInterface.h"

CRegColl::CRegColl(int obj1ID,int obj2ID,std::string objName,int objID)
{ // obj2ID can be -1, in which case obj1ID will be checked against all other collidable objects in the scene
	object1ID=obj1ID;
	object2ID=obj2ID;
	objectName=objName;
	objectID=objID;
	collisionResult=false;
	_collisionResultValid=false;
	_countourWidth=1;
	contourColor.setColorsAllBlack();
	contourColor.setColor(1.0f,1.0f,1.0f,EMISSION_MODE);
	_lifeID=-1;
	_initialValuesInitialized=false;

	_calcTimeInMs=0;
	collisionBuffer[0]=-1;
	collisionBuffer[1]=-1;
	collisionBuffer[2]=-1;
	collisionBuffer[3]=-1;

	colliderChangesColor=true;
	collideeChangesColor=false;
	detectAllCollisions=false;
	explicitHandling=false;
}

void CRegColl::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	_initialValuesInitialized=simulationIsRunning;
	if (simulationIsRunning)
	{
		_initialExplicitHandling=explicitHandling;
	}
}

void CRegColl::simulationAboutToStart()
{
	initializeInitialValues(true);
}
void CRegColl::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
	{
		explicitHandling=_initialExplicitHandling;
	}
	_initialValuesInitialized=false;
}

int CRegColl::getLifeID()
{
	if (_lifeID==-1)
		_lifeID=CLifeControl::getLifeID();
	return(_lifeID);
}

CRegColl::~CRegColl()
{
	CLifeControl::announceDeath(_lifeID);
}

bool CRegColl::isSame(int obj1ID,int obj2ID)
{
	if ( (obj1ID==object1ID)&&(obj2ID==object2ID) )
		return(true);
	if ( (obj2ID==object1ID)&&(obj1ID==object2ID) )
		return(true);
	return(false);
}

int CRegColl::getObjectID()
{
	return(objectID);
}

std::string CRegColl::getObjectName()
{
	return(objectName);
}
std::string CRegColl::getObjectPartnersName()
{
	std::string theName=getObjectName();
	theName=theName.append(" (");
	if (object1ID<SIM_IDSTART_COLLECTION)
	{
		CShape* it=App::ct->objCont->getShape(object1ID);
		if (it!=NULL)
		{
			theName+=strTranslate(IDSN_SHAPE);
			theName+=":";
			theName+=it->getName();
		}
	}
	else
	{
		CRegGroup* it=App::ct->collections->getGroup(object1ID);
		if (it!=NULL)
		{
			theName+=strTranslate(IDSN_COLLECTION);
			theName+=":";
			theName+=it->getGroupName();
		}
	}
	theName=theName.append(" - ");
	if (object2ID>=SIM_IDSTART_COLLECTION)
	{
		CRegGroup* it=App::ct->collections->getGroup(object2ID);
		if (it!=NULL)
		{
			theName+=strTranslate(IDSN_COLLECTION);
			theName+=":";
			theName+=it->getGroupName();
		}
	}
	else
	{
		CShape* it=App::ct->objCont->getShape(object2ID);
		if (it!=NULL)
		{
			theName+=strTranslate(IDSN_SHAPE);
			theName+=":";
			theName+=it->getName();
		}
		else
			theName+=strTranslate(IDS_ALL_OTHER_ENTITIES);
	}
	theName=theName.append(")");	
	return(theName);
}

int CRegColl::getCollisionColor(int entityID)
{
	if (!collisionResult)
		return(0);
	int retVal=0;
	if ( (entityID==object1ID)&&colliderChangesColor)
		retVal|=1;
	if ( (entityID==object2ID)&&collideeChangesColor)
		retVal|=2;
	// Here we need to check for the special case where object2ID==-1 (which means all other objects)
	if ((object2ID==-1)&&collideeChangesColor&&(entityID<SIM_IDSTART_COLLECTION))
	{
		C3DObject* it=App::ct->objCont->getObject(entityID);
		if ( (it!=NULL)&&(it->getMainProperty()&sim_objectspecialproperty_collidable) )
		{
			if (object1ID<SIM_IDSTART_COLLECTION)
			{
				if (entityID!=object1ID)
					retVal|=2;
			}
			else
			{
				std::vector<C3DObject*> group1;
				App::ct->collections->getShapesFromGroup(object1ID,&group1,-1,false);
				bool isContained=false;
				for (int i=0;i<int(group1.size());i++)
				{
					if (group1[i]==it)
					{
						isContained=true;
						break;
					}
				}
				if (!isContained)
					retVal|=2;
			}
		}
	}
	return(retVal);
}

bool CRegColl::announceObjectWillBeErased(int objID,bool copyBuffer)
{ // Return value true means that this collision object should be destroyed
	if (object1ID==objID)
		return(true);
	if (object2ID==objID)
		return(true);
	return(false);
}

bool CRegColl::announceGroupWillBeErased(int groupID,bool copyBuffer)
{ // Return value true means that this collision object should be destroyed
	if (object1ID==groupID)
		return(true);
	if (object2ID==groupID)
		return(true);
	return(false);
}

void CRegColl::setObjectName(std::string newName)
{
	objectName=newName;
}
void CRegColl::setObjectID(int newID)
{
	objectID=newID;
}
int CRegColl::getObject1ID()
{
	return(object1ID);
}
int CRegColl::getObject2ID()
{
	return(object2ID);
}

void CRegColl::setColliderChangesColor(bool changes)
{
	colliderChangesColor=changes;
}

bool CRegColl::getColliderChangesColor()
{
	return(colliderChangesColor);
}

void CRegColl::setCollideeChangesColor(bool changes)
{
	collideeChangesColor=changes;
}

bool CRegColl::getCollideeChangesColor()
{
	return(collideeChangesColor);
}

void CRegColl::setExhaustiveDetection(bool exhaustive)
{
	detectAllCollisions=exhaustive;
}

bool CRegColl::getExhaustiveDetection()
{
	return(detectAllCollisions);
}

void CRegColl::setExplicitHandling(bool explicitHandl)
{
	explicitHandling=explicitHandl;
}

bool CRegColl::getExplicitHandling()
{
	return(explicitHandling);
}

void CRegColl::clearCollisionResult()
{
	collisionResult=false;
	_collisionResultValid=false;
	_calcTimeInMs=0;
	intersections.clear();
}

bool CRegColl::isCollisionResultValid()
{
	return(_collisionResultValid);
}

bool CRegColl::getCollisionResult()
{
	return(collisionResult);
}

float CRegColl::getCalculationTime()
{
	return(float(_calcTimeInMs)*0.001f);
}

void CRegColl::setContourWidth(int w)
{
	w=tt::getLimitedInt(1,4,w);
	_countourWidth=w;
}

int CRegColl::getContourWidth()
{
	return(_countourWidth);
}

bool CRegColl::handleCollision(bool handledByWorkThread)
{	// Return value true means there was a collision
	clearCollisionResult();
	if (!App::ct->mainSettings->collisionDetectionEnabled)
		return(false);
	CSensingThreadPool::setProtectedAccess(true);
	if (!CCollDistInterface::initializeFunctionsIfNeeded()) // function is not reeintrant!
	{
		CSensingThreadPool::setProtectedAccess(false);
		return(false);
	}
	CSensingThreadPool::setProtectedAccess(false);
	int stT=VDateTime::getTimeInMs();
	if (detectAllCollisions)
	{
		std::vector<float> collCont;
		collisionResult=CCollisionRoutine::doEntitiesCollide(object1ID,object2ID,&collCont,collisionBuffer,false,false,false,handledByWorkThread);
		for (int i=0;i<int(collCont.size());i++)
			intersections.push_back(collCont[i]);
	}
	else
		collisionResult=CCollisionRoutine::doEntitiesCollide(object1ID,object2ID,NULL,collisionBuffer,false,false,false,handledByWorkThread);
	if (handledByWorkThread)
		_calcTimeInMs=0;
	else
		_calcTimeInMs=VDateTime::getTimeDiffInMs(stT);
	_collisionResultValid=true;
	return(collisionResult);
}

int CRegColl::readCollision()
{
	if (_collisionResultValid)
	{
		if (collisionResult)
			return(1);
		return(0);
	}
	return(-1);
}

void CRegColl::displayCollisionContour()
{
	glDisable(GL_DEPTH_TEST);
	contourColor.makeCurrentColor();
	for (int i=0;i<int(intersections.size())/6;i++)
	{
		if ( (intersections[6*i+0]==intersections[6*i+3])&&
			(intersections[6*i+1]==intersections[6*i+4])&&
			(intersections[6*i+2]==intersections[6*i+5]) )
		{
			glPointSize(3.0f);
			glBegin(GL_POINTS);
			glVertex3f(intersections[6*i+0],
				intersections[6*i+1],
				intersections[6*i+2]);
			glEnd();
			glPointSize(1.0f);
		}
		else
		{
			glLineWidth(float(_countourWidth));
			glBegin(GL_LINES);
			glVertex3f(intersections[6*i+0],
				intersections[6*i+1],
				intersections[6*i+2]);
			glVertex3f(intersections[6*i+3],
				intersections[6*i+4],
				intersections[6*i+5]);
			glEnd();
			glLineWidth(1.0f);
		}
	}
	glEnable(GL_DEPTH_TEST);
}

void CRegColl::performObjectLoadingMapping(std::vector<int>* map)
{
	if (object1ID<SIM_IDSTART_COLLECTION)
		object1ID=App::ct->objCont->getLoadingMapping(map,object1ID);
	if ( (object2ID<SIM_IDSTART_COLLECTION)&&(object2ID!=-1) )
		object2ID=App::ct->objCont->getLoadingMapping(map,object2ID);
}

void CRegColl::performGroupLoadingMapping(std::vector<int>* map)
{
	if (object1ID>=SIM_IDSTART_COLLECTION)
		object1ID=App::ct->objCont->getLoadingMapping(map,object1ID);
	if (object2ID>=SIM_IDSTART_COLLECTION)
		object2ID=App::ct->objCont->getLoadingMapping(map,object2ID);
}

CRegColl* CRegColl::copyYourself()
{
	CRegColl* newCollObj=new CRegColl(object1ID,object2ID,objectName,objectID);
	newCollObj->colliderChangesColor=colliderChangesColor;
	newCollObj->collideeChangesColor=collideeChangesColor;
	newCollObj->detectAllCollisions=detectAllCollisions;
	newCollObj->explicitHandling=explicitHandling;

	newCollObj->_countourWidth=_countourWidth;
	contourColor.copyYourselfInto(&newCollObj->contourColor);

	newCollObj->_initialValuesInitialized=_initialValuesInitialized;
	newCollObj->_initialExplicitHandling=_initialExplicitHandling;

	return(newCollObj);
}

void CRegColl::serialize(CSer& ar)
{
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Inx");
		ar << object1ID << object2ID << objectID;
		ar.flush();

		ar.storeDataName("Nme");
		ar << objectName;
		ar.flush();

		ar.storeDataName("Cwt");
		ar << _countourWidth;
		ar.flush();

		ar.storeDataName("Col");
		ar.setCountingMode();
		contourColor.serialize(ar);
		if (ar.setWritingMode())
			contourColor.serialize(ar);

		ar.storeDataName("Par");
		BYTE nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,colliderChangesColor);
		SIM_SET_CLEAR_BIT(nothing,1,collideeChangesColor);
		SIM_SET_CLEAR_BIT(nothing,2,detectAllCollisions);
		SIM_SET_CLEAR_BIT(nothing,3,explicitHandling);
		ar << nothing;
		ar.flush();

		ar.storeDataName(SER_END_OF_OBJECT);
	}
	else
	{		// Loading
		int byteQuantity;
		std::string theName="";
		while (theName.compare(SER_END_OF_OBJECT)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_END_OF_OBJECT)!=0)
			{
				bool noHit=true;
				if (theName.compare("Cwt")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _countourWidth;
				}
				if (theName.compare("Col")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					contourColor.serialize(ar);
				}
				if (theName.compare("Inx")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> object1ID >> object2ID >> objectID;
				}
				if (theName.compare("Nme")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> objectName;
				}
				if (theName=="Par")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					colliderChangesColor=SIM_IS_BIT_SET(nothing,0);
					collideeChangesColor=SIM_IS_BIT_SET(nothing,1);
					detectAllCollisions=SIM_IS_BIT_SET(nothing,2);
					explicitHandling=SIM_IS_BIT_SET(nothing,3);
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}
