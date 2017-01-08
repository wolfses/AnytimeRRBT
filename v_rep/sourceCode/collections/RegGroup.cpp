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
#include "RegGroup.h"
#include "3DObject.h"
#include "Tt.h"
#include "OGL.h"
#include "App.h"
#include "IloIlo.h"

CRegGroup::CRegGroup(std::string grName)
{
	groupName=grName;
	subGroupList.reserve(4);
	subGroupList.clear();
	markedObjects.reserve(4);
	markedObjects.clear();
	groupObjects.reserve(4);
	groupObjects.clear();
	_overridesObjectMainProperties=false;
	groupID=SIM_IDSTART_COLLECTION;
}

CRegGroup::~CRegGroup()
{
	for (int i=0;i<int(subGroupList.size());i++)
		delete subGroupList[i];
	subGroupList.clear();
}

void CRegGroup::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	//_initialValuesInitialized=simulationIsRunning;
	//if (simulationIsRunning)
	//{
	//}
}

void CRegGroup::simulationAboutToStart()
{
	initializeInitialValues(true);
}
void CRegGroup::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	//if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
	//{
	//}
	//_initialValuesInitialized=false;
}

bool CRegGroup::getOverridesObjectMainProperties()
{
	return(_overridesObjectMainProperties);
}

void CRegGroup::setOverridesObjectMainProperties(bool o)
{
	_overridesObjectMainProperties=o;
}


bool CRegGroup::isObjectInGroup(int objID)
{
	for (int i=0;i<int(groupObjects.size());i++)
	{
		if (groupObjects[i]==objID)
			return(true);
	}
	return(false);
}

int CRegGroup::getGroupObjectCount()
{
	return(int(groupObjects.size()));
}

CRegGroupEl* CRegGroup::getSubGroup(int subGroupID)
{
	for (int i=0;i<int(subGroupList.size());i++)
	{
		if (subGroupList[i]->getSubGroupID()==subGroupID)
			return(subGroupList[i]);
	}
	return(NULL);
}

bool CRegGroup::addSubGroup(CRegGroupEl* subGr)
{ // If return value is false, this subGroup is already present and you have to destroy subGr
	bool full=(subGroupList.size()!=0);
	// We set a new ID:
	int i=0;
	while (getSubGroup(i)!=NULL)
		i++;
	subGr->setSubGroupID(i);
	subGroupList.push_back(subGr);
	actualizeGroup(full);
	App::ct->collections->repairMarking();
	App::ct->objCont->setFullDialogRefreshFlag();
	return(true);
}

bool CRegGroup::actualizeGroup(bool full)
{	// return value false means that this group is empty and should be removed. Full is true by default!
	App::ct->objCont->setLightDialogRefreshFlag();
	// First we erase all sub-groups which are not active anymore:
	int i=0;
	// First we remove all sub groups which are not valid anymore:
	while (i<int(subGroupList.size()))
	{
		C3DObject* it=App::ct->objCont->getObject(subGroupList[i]->getMainObject());
		if (it==NULL)
		{
			if (subGroupList[i]->getGroupType()!=GROUP_EVERYTHING)
			{
				delete subGroupList[i];
				subGroupList.erase(subGroupList.begin()+i);
			}
			else
				i++;
		}
		else
			i++;
	}
	// Now we have to take care of the GROUP_EVERYTHING type:
	if (full)
	{
		bool removeAll=true;
		for (int i=0;i<int(subGroupList.size());i++)
		{
			if (subGroupList[i]->getGroupType()!=GROUP_EVERYTHING)
				removeAll=false;
		}
		if (removeAll)
		{
			for (int i=0;i<int(subGroupList.size());i++)
				delete subGroupList[i];
			subGroupList.clear();
		}
	}
	// Is this group still valid?
	if (subGroupList.size()==0)
		return(false);

	// Now we set-up the object list:
	groupObjects.clear();
	for (int i=0;i<int(subGroupList.size());i++)
		subGroupList[i]->addOrRemoveYourObjects(&groupObjects);

	markedObjects.clear();
	return(true);
}

void CRegGroup::mark(int objID)
{
	bool found=false;
	for (int i=0;i<int(groupObjects.size());i++)
	{
		if (groupObjects[i]==objID)
		{
			found=true;
			break;
		}
	}
	if (!found)
		return;
	for (int i=0;i<int(markedObjects.size());i++)
	{
		if (markedObjects[i]==objID)
			return;
	}
	markedObjects.push_back(objID);
}

void CRegGroup::unmark(int objID)
{
	for (int i=0;i<int(markedObjects.size());i++)
	{
		if (markedObjects[i]==objID)
		{
			markedObjects.erase(markedObjects.begin()+i);
			break;
		}
	}
}

void CRegGroup::unmarkAll()
{
	markedObjects.clear();
}

bool CRegGroup::areAllMarked()
{
	if (groupObjects.size()!=0)
		return(groupObjects.size()==markedObjects.size());
	return(false);
}

bool CRegGroup::removeSubGroup(int subGroupID)
{
	for (int i=0;i<int(subGroupList.size());i++)
	{
		if (subGroupList[i]->getSubGroupID()==subGroupID)
		{
			subGroupList.erase(subGroupList.begin()+i);
			actualizeGroup();
			App::ct->collections->repairMarking();
			return(true);
		}
	}
	return(false);
}

bool CRegGroup::announceObjectWillBeErased(int objID,bool copyBuffer)
{ // Return value true means that this group is empty and should be removed
	int i=0;
	int initialSubGroupListSize=int(subGroupList.size());
	while (i<int(subGroupList.size()))
	{
		if ( (subGroupList[i]->getMainObject()==objID) ) //  GROUP_EVERYTHING is handled a little bit further down
		{
			delete subGroupList[i];
			subGroupList.erase(subGroupList.begin()+i);
		}
		else
			i++;
	}

	// Now we have to take care of the GROUP_EVERYTHING type:
	bool removeAll=true;
	for (int i=0;i<int(subGroupList.size());i++)
	{
		if (subGroupList[i]->getGroupType()!=GROUP_EVERYTHING)
			removeAll=false;
	}
	if (removeAll)
	{
		for (int i=0;i<int(subGroupList.size());i++)
			delete subGroupList[i];
		subGroupList.clear();
	}

	//We also remove that object from the object list and marked list:
	for (int i=0;i<int(groupObjects.size());i++)
	{
		if (groupObjects[i]==objID)
		{
			groupObjects.erase(groupObjects.begin()+i);
			break;
		}
	}
	for (int i=0;i<int(markedObjects.size());i++)
	{
		if (markedObjects[i]==objID)
		{
			markedObjects.erase(markedObjects.begin()+i);
			break;
		}
	}
	if ( (int(subGroupList.size())!=initialSubGroupListSize)&&copyBuffer )
		return(true); // (during copy/paste, only intact collections should be copied!)
	if (subGroupList.size()!=0)
		return(false);
	return(true);
}

int CRegGroup::getGroupID()
{
	return(groupID);
}

void CRegGroup::setGroupID(int newID)
{
	groupID=newID;
}

std::string CRegGroup::getGroupName()
{
	return(groupName);
}

void CRegGroup::setGroupName(std::string newName)
{
	groupName=newName;
	App::ct->objCont->setFullDialogRefreshFlag();
}

void CRegGroup::performObjectLoadingMapping(std::vector<int>* map)
{
	for (int i=0;i<int(subGroupList.size());i++)
		subGroupList[i]->performObjectLoadingMapping(map);
	actualizeGroup();
}

CRegGroup* CRegGroup::copyYourself()
{
	CRegGroup* newGroup=new CRegGroup(groupName);
	newGroup->setGroupID(groupID);
	newGroup->subGroupList.reserve(subGroupList.size());
	newGroup->subGroupList.clear();
	for (int i=0;i<int(subGroupList.size());i++)
		newGroup->subGroupList.push_back(subGroupList[i]->copyYourself());
	newGroup->_overridesObjectMainProperties=_overridesObjectMainProperties;
	return(newGroup);
}

void CRegGroup::serialize(CSer& ar)
{
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Grn");
		ar << groupName;
		ar.flush();

		ar.storeDataName("Gix");
		ar << groupID;
		ar.flush();

		ar.storeDataName("Par");
		BYTE nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,_overridesObjectMainProperties);
		ar << nothing;
		ar.flush();

		for (int i=0;i<int(subGroupList.size());i++)
		{
			ar.storeDataName("Asg");
			ar.setCountingMode();
			subGroupList[i]->serialize(ar);
			if (ar.setWritingMode())
				subGroupList[i]->serialize(ar);
		}

		ar.storeDataName(SER_END_OF_OBJECT);
	}
	else
	{	// Loading
		subGroupList.clear();
		int byteQuantity;
		std::string theName="";
		while (theName.compare(SER_END_OF_OBJECT)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_END_OF_OBJECT)!=0)
			{
				bool noHit=true;
				if (theName.compare("Grn")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> groupName;
				}
				if (theName.compare("Gix")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> groupID;
				}
				if (theName.compare("Asg")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					CRegGroupEl* it=new CRegGroupEl(0,0,false);
					it->serialize(ar);
					subGroupList.push_back(it);
				}
				if (theName=="Par")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_overridesObjectMainProperties=SIM_IS_BIT_SET(nothing,0);
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}
