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
#include "RegDist.h"
#include "3DObject.h"
#include "Global.h"
#include "DistanceRoutine.h"
#include "OGL.h"
#include "Tt.h"
#include "App.h"
#include "IloIlo.h"
#include "GV.h"
#include "v_repStrings.h"
#include "VDateTime.h"
#include "sensingThreadPool.h"
#include "collDistInterface.h"

CRegDist::CRegDist(int obj1ID,int obj2ID,std::string objName,int objID)
{
	object1ID=obj1ID;
	object2ID=obj2ID;
	objectName=objName;
	objectID=objID;
	displaySegment=true;
	explicitHandling=false;
	for (int i=0;i<7;i++)
		distanceResult[i]=0.0f;
	distanceBuffer[0]=-1;
	distanceBuffer[1]=-1;
	distanceBuffer[2]=-1;
	distanceBuffer[3]=-1;
	treshhold=0.5f;
	treshholdActive=false;
	distanceIsValid=false;
	_segmentWidth=2;
	segmentColor.setColorsAllBlack();
	_lifeID=-1;
	_initialValuesInitialized=false;

	_calcTimeInMs=0;
}

void CRegDist::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	_initialValuesInitialized=simulationIsRunning;
	if (simulationIsRunning)
	{
		_initialExplicitHandling=explicitHandling;
	}
}

void CRegDist::simulationAboutToStart()
{
	initializeInitialValues(true);
}
void CRegDist::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
	{
		explicitHandling=_initialExplicitHandling;
	}
	_initialValuesInitialized=false;
}

int CRegDist::getLifeID()
{
	if (_lifeID==-1)
		_lifeID=CLifeControl::getLifeID();
	return(_lifeID);
}

CRegDist::~CRegDist()
{
	CLifeControl::announceDeath(_lifeID);
}

bool CRegDist::isSame(int obj1ID,int obj2ID)
{
	if ( (obj1ID==object1ID)&&(obj2ID==object2ID) )
		return(true);
	if ( (obj2ID==object1ID)&&(obj1ID==object2ID) )
		return(true);
	return(false);
}
int CRegDist::getObjectID()
{
	return(objectID);
}

std::string CRegDist::getObjectName()
{
	return(objectName);
}
std::string CRegDist::getObjectPartnersName()
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
		CDummy* it2=App::ct->objCont->getDummy(object1ID);
		if (it2!=NULL)
		{
			theName+=strTranslate(IDSN_DUMMY);
			theName+=":";
			theName+=it2->getName();
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
		bool done=false;
		CShape* it=App::ct->objCont->getShape(object2ID);
		if (it!=NULL)
		{
			done=true;
			theName+=strTranslate(IDSN_SHAPE);
			theName+=":";
			theName+=it->getName();
		}
		CDummy* it2=App::ct->objCont->getDummy(object2ID);
		if (it2!=NULL)
		{
			done=true;
			theName+=strTranslate(IDSN_DUMMY);
			theName+=":";
			theName+=it2->getName();
		}
		if (!done)
			theName+=strTranslate(IDS_ALL_OTHER_ENTITIES);
	}
	theName=theName.append(")");	
	return(theName);


}
void CRegDist::setObjectName(std::string newName)
{
	objectName=newName;
}
void CRegDist::setObjectID(int newID)
{
	objectID=newID;
}
int CRegDist::getObject1ID()
{
	return(object1ID);
}
int CRegDist::getObject2ID()
{
	return(object2ID);
}
void CRegDist::setTreshhold(float tr)
{
	treshhold=tr;
}
float CRegDist::getTreshhold()
{
	return(treshhold);
}
void CRegDist::setTreshholdActive(bool active)
{
	treshholdActive=active;
}
bool CRegDist::getTreshholdActive()
{
	return(treshholdActive);
}
void CRegDist::clearDistanceResult()
{
	distanceIsValid=false;
	_calcTimeInMs=0;
}

void CRegDist::setDisplaySegment(bool display)
{
	displaySegment=display;
}

void CRegDist::setExplicitHandling(bool explicitHandl)
{
	explicitHandling=explicitHandl;
}

bool CRegDist::getExplicitHandling()
{
	return(explicitHandling);
}

bool CRegDist::getDisplaySegment()
{
	return(displaySegment);
}

bool CRegDist::getDistanceResult(float dist[7])
{ // Return value false means the distance was not measured or above the specified treshhold.
	if (distanceIsValid)
	{
		dist[0]=distanceResult[0];
		dist[1]=distanceResult[1];
		dist[2]=distanceResult[2];
		dist[3]=distanceResult[3];
		dist[4]=distanceResult[4];
		dist[5]=distanceResult[5];
		dist[6]=distanceResult[6];
	}
	return(distanceIsValid);
}

float CRegDist::getCalculationTime()
{
	return(float(_calcTimeInMs)*0.001f);
}

float CRegDist::handleDistance(bool handledByWorkThread)
{	
	distanceIsValid=false;
	_calcTimeInMs=0;
	if (!App::ct->mainSettings->distanceCalculationEnabled)
		return(-1.0f);
	CSensingThreadPool::setProtectedAccess(true);
	if (!CCollDistInterface::initializeFunctionsIfNeeded()) // function is not reentrant!
	{
		CSensingThreadPool::setProtectedAccess(false);
		return(-1.0f);
	}
	CSensingThreadPool::setProtectedAccess(false);
	int stTime=VDateTime::getTimeInMs();
	_distance=SIM_MAX_FLOAT;
	if (treshholdActive)
		_distance=treshhold;
	distanceIsValid=CDistanceRoutine::getDistanceBetweenEntitiesIfSmaller(object1ID,object2ID,_distance,distanceResult,distanceBuffer,false,false,false,handledByWorkThread);

//	int tt=int(App::ct->simulation->getSimulationTime()*1000.0f);
//	static int preT=tt;
//	printf("Time diff: %i\n",tt-preT);
//	preT=tt;

	if (handledByWorkThread)
		_calcTimeInMs=0;
	else
		_calcTimeInMs=VDateTime::getTimeDiffInMs(stTime);
	return(readDistance());
}

float CRegDist::readDistance()
{
	if (distanceIsValid)
		return(_distance);
	return(-1.0f);
}

bool CRegDist::announceObjectWillBeErased(int objID,bool copyBuffer)
{ // Return value true means that this distance object should be destroyed
	if (object1ID==objID)
		return(true);
	if (object2ID==objID)
		return(true);
	return(false);
}

bool CRegDist::announceGroupWillBeErased(int groupID,bool copyBuffer)
{ // Return value true means that this distance object should be destroyed
	if (object1ID==groupID)
		return(true);
	if (object2ID==groupID)
		return(true);
	return(false);
}

void CRegDist::performObjectLoadingMapping(std::vector<int>* map)
{
	if (object1ID<SIM_IDSTART_COLLECTION)
		object1ID=App::ct->objCont->getLoadingMapping(map,object1ID);
	if ( (object2ID<SIM_IDSTART_COLLECTION)&&(object2ID!=-1) )
		object2ID=App::ct->objCont->getLoadingMapping(map,object2ID);
}
void CRegDist::performGroupLoadingMapping(std::vector<int>* map)
{
	if (object1ID>=SIM_IDSTART_COLLECTION)
		object1ID=App::ct->objCont->getLoadingMapping(map,object1ID);
	if (object2ID>=SIM_IDSTART_COLLECTION)
		object2ID=App::ct->objCont->getLoadingMapping(map,object2ID);
}

CRegDist* CRegDist::copyYourself()
{
	CRegDist* newDistObj=new CRegDist(object1ID,object2ID,objectName,objectID);
	newDistObj->treshhold=treshhold;
	newDistObj->treshholdActive=treshholdActive;
	newDistObj->displaySegment=displaySegment;
	newDistObj->explicitHandling=explicitHandling;

	newDistObj->_segmentWidth=_segmentWidth;
	segmentColor.copyYourselfInto(&newDistObj->segmentColor);

	newDistObj->_initialValuesInitialized=_initialValuesInitialized;
	newDistObj->_initialExplicitHandling=_initialExplicitHandling;

	return(newDistObj);
}

void CRegDist::displayDistanceSegment()
{
	if ( (!distanceIsValid)||(!displaySegment) )
		return;
	segmentColor.makeCurrentColor();
	glLineWidth(float(_segmentWidth));
	glBegin(GL_LINES);
	glVertex3fv(distanceResult+0);
	glVertex3fv(distanceResult+3);
	glEnd();
	glLineWidth(1.0f);
	std::string txt("d=");
	txt+=tt::FNb(0,distanceResult[6]*gv::meterToUser,6,false)+" ";
	txt+=gv::getSizeUnitStr();
	ogl::drawBitmapTextIntoScene((distanceResult[0]+distanceResult[3])/2.0f,(distanceResult[1]+distanceResult[4])/2.0f,(distanceResult[2]+distanceResult[5])/2.0f,txt.c_str());			
}

void CRegDist::setSegmentWidth(int w)
{
	w=tt::getLimitedInt(1,4,w);
	_segmentWidth=w;
}

int CRegDist::getSegmentWidth()
{
	return(_segmentWidth);
}


void CRegDist::serialize(CSer& ar)
{
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Inx");
		ar << object1ID << object2ID << objectID;
		ar.flush();

		ar.storeDataName("Trh");
		ar << treshhold;
		ar.flush();

		ar.storeDataName("Swt");
		ar << _segmentWidth;
		ar.flush();

		ar.storeDataName("Col");
		ar.setCountingMode();
		segmentColor.serialize(ar);
		if (ar.setWritingMode())
			segmentColor.serialize(ar);

		ar.storeDataName("Var");
		BYTE nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,treshholdActive);
		SIM_SET_CLEAR_BIT(nothing,1,!displaySegment);
		SIM_SET_CLEAR_BIT(nothing,2,explicitHandling);
		ar << nothing;
		ar.flush();

		ar.storeDataName("Nme");
		ar << objectName;
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
				if (theName.compare("Inx")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> object1ID >> object2ID >> objectID;
				}
				if (theName.compare("Trh")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> treshhold;
				}
				if (theName.compare("Swt")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _segmentWidth;
				}
				if (theName.compare("Col")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					segmentColor.serialize(ar);
				}
				if (theName.compare("Var")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					treshholdActive=SIM_IS_BIT_SET(nothing,0);
					displaySegment=!SIM_IS_BIT_SET(nothing,1);
					explicitHandling=SIM_IS_BIT_SET(nothing,2);
				}
				if (theName.compare("Nme")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> objectName;
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}
