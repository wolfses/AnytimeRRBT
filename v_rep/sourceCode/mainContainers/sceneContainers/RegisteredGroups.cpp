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
#include "RegisteredGroups.h"
#include "Tt.h"
#include "OGL.h"
#include "App.h"

CRegisteredGroups::CRegisteredGroups()
{
}

CRegisteredGroups::~CRegisteredGroups()
{
	unmarkAll();
	removeAllGroups();
}

void CRegisteredGroups::simulationAboutToStart()
{
	for (int i=0;i<int(allGroups.size());i++)
		allGroups[i]->simulationAboutToStart();
}

void CRegisteredGroups::simulationEnded()
{
	for (int i=0;i<int(allGroups.size());i++)
		allGroups[i]->simulationEnded();
}

void CRegisteredGroups::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

void CRegisteredGroups::newScene()
{
	unmarkAll();
	removeAllGroups();
}

void CRegisteredGroups::announceObjectWillBeErased(int objID)
{ // Never called from copy buffer!
	int i=0;
	while (i<int(allGroups.size()))
	{
		// position could be changed after a call to announceObject...
		if (allGroups[i]->announceObjectWillBeErased(objID,false))
		{
			removeGroup(allGroups[i]->getGroupID()); // This will call announceGroupWillBeErased!!
			i=0; // We never know. The ordering may have changed!
		}
		else
			i++;
	}
}

void CRegisteredGroups::actualizeAllGroups()
{
	int i=0;
	while (i<int(allGroups.size()))
	{
		if (!allGroups[i]->actualizeGroup())
		{
			removeGroup(allGroups[i]->getGroupID());
			i=0; // We start at 0 again. The ordering may have changed in removeGroup
		}
		else
			i++;
	}
}

void CRegisteredGroups::unmarkAll()
{	// Will deselect all groups. Use only from CObjCont
	for (int i=0;i<int(allGroups.size());i++)
		allGroups[i]->unmarkAll();
	selectedGroups.clear();
}

void CRegisteredGroups::mark(int objID)
{	// Will mark all objects objID in groups. If a group is completely selected,
	// it will be inserted in selectedGroups. Use only from CObjCont
	for (int i=0;i<int(allGroups.size());i++)
	{
		if (!allGroups[i]->areAllMarked())
		{
			allGroups[i]->mark(objID);
			if (allGroups[i]->areAllMarked())
			{	
				// We first remove the group from selection (just in case)
				for (int j=0;j<int(selectedGroups.size());j++)
				{
					if (selectedGroups[j]==allGroups[i]->getGroupID())
					{
						selectedGroups.erase(selectedGroups.begin()+j);
						break;
					}
				}
				// We add the group to the selection:
				selectedGroups.push_back(allGroups[i]->getGroupID());
			}
		}
	}
}

bool CRegisteredGroups::getShapesAndVolumesFromGroup(int groupID,std::vector<C3DObject*>* objInGroup,int propMask,bool pathPlanningRoutineCalling)
{ // If propMask==-1, then object main properties are not checked an all objects are taken!
	CRegGroup* theGroup=getGroup(groupID);
	if (theGroup==NULL)
		return(false);
	objInGroup->reserve(theGroup->groupObjects.size());
	objInGroup->clear();
	bool overridePropertyFlag=theGroup->getOverridesObjectMainProperties();
	for (int i=0;i<int(theGroup->groupObjects.size());i++)
	{
		C3DObject* anObject=App::ct->objCont->getObject(theGroup->groupObjects[i]);
		if (anObject!=NULL)
		{
			if ( (anObject->getMainProperty()&propMask)||(propMask==-1)||overridePropertyFlag )
			{
				if ( ((!pathPlanningRoutineCalling)||((anObject->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)==0))||overridePropertyFlag ) // condition added on 2010/08/25
				{
					if (anObject->getObjectType()==sim_object_shape_type)
						objInGroup->push_back(anObject);
					if (anObject->getObjectType()==sim_object_volume_type)
						objInGroup->push_back(anObject);
				}
			}
		}
	}
	return(true);
}

bool CRegisteredGroups::getShapesAndDummiesFromGroup(int groupID,std::vector<C3DObject*>* objInGroup,int propMask,bool pathPlanningRoutineCalling)
{ // If propMask==-1, then object main properties are not checked an all objects are taken!
	CRegGroup* theGroup=getGroup(groupID);
	if (theGroup==NULL)
		return(false);
	objInGroup->reserve(theGroup->groupObjects.size());
	objInGroup->clear();
	bool overridePropertyFlag=theGroup->getOverridesObjectMainProperties();
	for (int i=0;i<int(theGroup->groupObjects.size());i++)
	{
		C3DObject* anObject=App::ct->objCont->getObject(theGroup->groupObjects[i]);
		if (anObject!=NULL)
		{
			if ( (anObject->getMainProperty()&propMask)||(propMask==-1)||overridePropertyFlag )
			{
				if ( ((!pathPlanningRoutineCalling)||((anObject->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)==0))||overridePropertyFlag ) // condition added on 2010/08/25
				{
					if (anObject->getObjectType()==sim_object_shape_type)
						objInGroup->push_back(anObject);
					if (anObject->getObjectType()==sim_object_dummy_type)
						objInGroup->push_back(anObject);
				}
			}
		}
	}
	return(true);
}

bool CRegisteredGroups::getShapesFromGroup(int groupID,std::vector<C3DObject*>* objInGroup,int propMask,bool pathPlanningRoutineCalling)
{ // If propMask==-1, then object main properties are not checked an all objects are taken!
	CRegGroup* theGroup=getGroup(groupID);
	if (theGroup==NULL)
		return(false);
	objInGroup->reserve(theGroup->groupObjects.size());
	objInGroup->clear();
	bool overridePropertyFlags=theGroup->getOverridesObjectMainProperties();
	for (int i=0;i<int(theGroup->groupObjects.size());i++)
	{
		C3DObject* anObject=App::ct->objCont->getObject(theGroup->groupObjects[i]);
		if (anObject!=NULL)
		{
			if (anObject->getObjectType()==sim_object_shape_type)
			{
				if ( (anObject->getMainProperty()&propMask)||(propMask==-1)||overridePropertyFlags )
				{
					if ( ((!pathPlanningRoutineCalling)||((anObject->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)==0))||overridePropertyFlags ) // condition added on 2010/08/25
						objInGroup->push_back(anObject);
				}
			}
		}
	}
	return(true);
}



void CRegisteredGroups::unmark(int objID)
{	// Will unmark all objects objID in groups. If a group is deselected,
	// it will be removed from selectedGroups. Use only from CObjCont
	for (int i=0;i<int(allGroups.size());i++)
	{
		allGroups[i]->unmark(objID);
		if (!allGroups[i]->areAllMarked())
		{
			if (isGroupInSelection(allGroups[i]->getGroupID()))
			{
				for (int j=0;j<int(selectedGroups.size());j++)
				{
					if (selectedGroups[j]==allGroups[i]->getGroupID())
					{
						selectedGroups.erase(selectedGroups.begin()+j);
						break;
					}
				}
			}
		}
	}
}

void CRegisteredGroups::repairMarking()
{
	unmarkAll();
	if (App::ct->objCont==NULL)
		return;
	for (int i=0;i<App::ct->objCont->getSelSize();i++)
		mark(App::ct->objCont->getSelID(i));
}

void CRegisteredGroups::removeAllGroups()
{
	deselectAllGroups();
	while (allGroups.size()!=0)
		removeGroup(allGroups[0]->getGroupID());
	if (App::ct->objCont!=NULL)
		App::ct->objCont->setFullDialogRefreshFlag();
	repairMarking();
}

void CRegisteredGroups::setUpDefaultValues()
{
	removeAllGroups();
}

CRegGroup* CRegisteredGroups::getGroup(int groupID)
{
	for (int i=0;i<int(allGroups.size());i++)
	{
		if (allGroups[i]->getGroupID()==groupID)
			return(allGroups[i]);
	}
	return(NULL);
}

CRegGroup* CRegisteredGroups::getGroup(std::string groupName)
{
	for (int i=0;i<int(allGroups.size());i++)
	{
		if (allGroups[i]->getGroupName().compare(groupName)==0)
			return(allGroups[i]);
	}
	return(NULL);
}

bool CRegisteredGroups::removeGroup(int groupID)
{	// Return value true: the group was removed
	removeGroupFromSelection(groupID);
	App::ct->objCont->announceGroupWillBeErased(groupID);
	for (int i=0;i<int(allGroups.size());i++)
	{
		if (allGroups[i]->getGroupID()==groupID)
		{
			delete allGroups[i];
			allGroups.erase(allGroups.begin()+i);
			App::ct->objCont->setFullDialogRefreshFlag();
			repairMarking();
			return(false);
		}
	}
	return(false);
}

void CRegisteredGroups::getMinAndMaxNameSuffixes(int& minSuffix,int& maxSuffix)
{
	minSuffix=-1;
	maxSuffix=-1;
	for (int i=0;i<int(allGroups.size());i++)
	{
		int s=tt::getNameSuffixNumber(allGroups[i]->getGroupName().c_str(),true);
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

bool CRegisteredGroups::canSuffix1BeSetToSuffix2(int suffix1,int suffix2)
{
	for (int i=0;i<int(allGroups.size());i++)
	{
		int s1=tt::getNameSuffixNumber(allGroups[i]->getGroupName().c_str(),true);
		if (s1==suffix1)
		{
			std::string name1(tt::getNameWithoutSuffixNumber(allGroups[i]->getGroupName().c_str(),true));
			for (int j=0;j<int(allGroups.size());j++)
			{
				int s2=tt::getNameSuffixNumber(allGroups[j]->getGroupName().c_str(),true);
				if (s2==suffix2)
				{
					std::string name2(tt::getNameWithoutSuffixNumber(allGroups[j]->getGroupName().c_str(),true));
					if (name1==name2)
						return(false); // NO! We would have a name clash!
				}
			}
		}
	}
	return(true);
}

void CRegisteredGroups::setSuffix1ToSuffix2(int suffix1,int suffix2)
{
	for (int i=0;i<int(allGroups.size());i++)
	{
		int s1=tt::getNameSuffixNumber(allGroups[i]->getGroupName().c_str(),true);
		if (s1==suffix1)
		{
			std::string name1(tt::getNameWithoutSuffixNumber(allGroups[i]->getGroupName().c_str(),true));
			allGroups[i]->setGroupName(tt::generateNewName_dash(name1,suffix2+1));
		}
	}
}

void CRegisteredGroups::addGroup(CRegGroup* theGroup,bool objectIsACopy)
{
	addGroupWithSuffixOffset(theGroup,objectIsACopy,1);
}

void CRegisteredGroups::addGroupWithSuffixOffset(CRegGroup* theGroup,bool objectIsACopy,int suffixOffset)
{
	// Does that name already exist?
	std::string theName=theGroup->getGroupName();
	if (objectIsACopy)
		theName=tt::generateNewName_dash(theName,suffixOffset);
	else
	{
		while (getGroup(theName)!=NULL)
			theName=tt::generateNewName_noDash(theName);
	}
	theGroup->setGroupName(theName);
	// Find a free ID:
	int id=SIM_IDSTART_COLLECTION;
	while (getGroup(id)!=NULL)
		id++;
	theGroup->setGroupID(id);
	// Add the group:
	allGroups.push_back(theGroup);
	App::ct->objCont->setFullDialogRefreshFlag();
	repairMarking();
}

bool CRegisteredGroups::isObjectInMarkedGroup(int objID)
{
	for (int i=0;i<int(allGroups.size());i++)
	{
		if (allGroups[i]->areAllMarked())
		{
			if (allGroups[i]->isObjectInGroup(objID))
				return(true);
		}
	}
	return(false);
}

void CRegisteredGroups::addGroupToSelection(int groupID)
{
	// Don't call from App::ct->objCont->addObject(), App::ct->objCont->... !!
	CRegGroup* it=getGroup(groupID);
	if (it==NULL)
		return;
	for (int i=0;i<int(selectedGroups.size());i++)
	{
		if (selectedGroups[i]==groupID)
			return; // already selected
	}
	for (int i=0;i<int(it->groupObjects.size());i++)
		App::ct->objCont->addObjectToSelection(it->groupObjects[i]);
}
bool CRegisteredGroups::isGroupInSelection(int groupID)
{
	for (int i=0;i<int(selectedGroups.size());i++)
	{
		if (selectedGroups[i]==groupID)
			return(true);
	}
	return(false);
}
void CRegisteredGroups::removeGroupFromSelection(int groupID)
{
	// Don't call from App::ct->objCont->addObject(), App::ct->objCont->... !!
	CRegGroup* it=getGroup(groupID);
	if (it==NULL)
		return;
	for (int i=0;i<int(it->groupObjects.size());i++)
		App::ct->objCont->removeObjectFromSelection(it->groupObjects[i]);
}
void CRegisteredGroups::deselectAllGroups()
{
	// Don't call from App::ct->objCont->addObject(), App::ct->objCont->... !!
	for (int i=0;i<int(allGroups.size());i++)
		removeGroupFromSelection(allGroups[i]->getGroupID());
}

void CRegisteredGroups::performObjectLoadingMapping(std::vector<int>* map)
{
	for (int i=0;i<int(allGroups.size());i++)
		allGroups[i]->performObjectLoadingMapping(map);
}

void CRegisteredGroups::performObjectLoadingMappingSingleGroup(std::vector<int>* map,int groupID)
{
	CRegGroup* it=getGroup(groupID);
	if (it!=NULL)
		it->performObjectLoadingMapping(map);
}
