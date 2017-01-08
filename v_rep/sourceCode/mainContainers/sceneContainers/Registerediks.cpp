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
#include "Registerediks.h"
#include "ikRoutine.h"
#include "App.h"
#include "Tt.h"
#include "v_rep_internal.h"
#include "IloIlo.h"
#include "VDateTime.h"

CRegisterediks::CRegisterediks()
{
}

CRegisterediks::~CRegisterediks()
{
	removeAllIkGroups();
}


void CRegisterediks::simulationAboutToStart()
{
	 for (int i=0;i<int(ikGroups.size());i++)
		ikGroups[i]->simulationAboutToStart();
}

void CRegisterediks::simulationEnded()
{
	 for (int i=0;i<int(ikGroups.size());i++)
		ikGroups[i]->simulationEnded();
}

void CRegisterediks::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

void CRegisterediks::addIkGroup(CikGroup* anIkGroup,bool objectIsACopy)
{
	addIkGroupWithSuffixOffset(anIkGroup,objectIsACopy,1);
}

void CRegisterediks::addIkGroupWithSuffixOffset(CikGroup* anIkGroup,bool objectIsACopy,int suffixOffset)
{ // Be careful! We don't check if the group is valid!!
	// We look for a free id:
	int newID=SIM_IDSTART_IKGROUP;
	while (getIkGroup(newID)!=NULL)
		newID++;
	anIkGroup->setObjectID(newID);
	// We make a unique name:
	std::string theName=anIkGroup->getObjectName();
	if (objectIsACopy)
		theName=tt::generateNewName_dash(theName,suffixOffset);
	else
	{
		while (getIkGroup(theName)!=NULL)
			theName=tt::generateNewName_noDash(theName);
	}
	anIkGroup->setObjectName(theName);
	ikGroups.push_back(anIkGroup);
}

void CRegisterediks::announceGroupWillBeErased(int groupID)
{ // Never called from copy buffer!
	int i=0;
	while (i<int(ikGroups.size()))
	{
		if (ikGroups[i]->announceGroupWillBeErased(groupID,false))
		{ // This case should never happen (at least for now!
			removeIkGroup(ikGroups[i]->getObjectID()); // This will call announceIkGroupWillBeErased!
			i=0; // ordering may have changed!
		}
		else
			i++;
	}
}

void CRegisterediks::getMinAndMaxNameSuffixes(int& minSuffix,int& maxSuffix)
{
	minSuffix=-1;
	maxSuffix=-1;
	for (int i=0;i<int(ikGroups.size());i++)
	{
		int s=tt::getNameSuffixNumber(ikGroups[i]->getObjectName().c_str(),true);
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

bool CRegisterediks::canSuffix1BeSetToSuffix2(int suffix1,int suffix2)
{
	for (int i=0;i<int(ikGroups.size());i++)
	{
		int s1=tt::getNameSuffixNumber(ikGroups[i]->getObjectName().c_str(),true);
		if (s1==suffix1)
		{
			std::string name1(tt::getNameWithoutSuffixNumber(ikGroups[i]->getObjectName().c_str(),true));
			for (int j=0;j<int(ikGroups.size());j++)
			{
				int s2=tt::getNameSuffixNumber(ikGroups[j]->getObjectName().c_str(),true);
				if (s2==suffix2)
				{
					std::string name2(tt::getNameWithoutSuffixNumber(ikGroups[j]->getObjectName().c_str(),true));
					if (name1==name2)
						return(false); // NO! We would have a name clash!
				}
			}
		}
	}
	return(true);
}

void CRegisterediks::setSuffix1ToSuffix2(int suffix1,int suffix2)
{
	for (int i=0;i<int(ikGroups.size());i++)
	{
		int s1=tt::getNameSuffixNumber(ikGroups[i]->getObjectName().c_str(),true);
		if (s1==suffix1)
		{
			std::string name1(tt::getNameWithoutSuffixNumber(ikGroups[i]->getObjectName().c_str(),true));
			ikGroups[i]->setObjectName(tt::generateNewName_dash(name1,suffix2+1));
		}
	}
}


CikGroup* CRegisterediks::getIkGroup(int groupID)
{
	 for (int i=0;i<int(ikGroups.size());i++)
	 {
		if (ikGroups[i]->getObjectID()==groupID)
			return(ikGroups[i]);
	 }
	 return(NULL);
}

CikGroup* CRegisterediks::getIkGroup(std::string groupName)
{
	for (int i=0;i<int(ikGroups.size());i++)
	{
		if (ikGroups[i]->getObjectName().compare(groupName)==0)
			return(ikGroups[i]);
	}
	return(NULL);
}

void CRegisterediks::removeIkGroup(int ikGroupID)
{
	App::ct->objCont->announceIkGroupWillBeErased(ikGroupID);
	for (int i=0;i<int(ikGroups.size());i++)
	{
		if (ikGroups[i]->getObjectID()==ikGroupID)
		{
			delete ikGroups[i];
			ikGroups.erase(ikGroups.begin()+i);
			App::ct->objCont->setFullDialogRefreshFlag();
			return;
		}
	}
}

void CRegisterediks::removeAllIkGroups()
{
	while (ikGroups.size()!=0)
		removeIkGroup(ikGroups[0]->getObjectID());
}

void CRegisterediks::announceObjectWillBeErased(int objID)
{ // Never called from copy buffer!
	int i=0;
	while (i<int(ikGroups.size()))
	{
		if (ikGroups[i]->announceObjectWillBeErased(objID,false))
		{ // This ik group has to be erased:
			removeIkGroup(ikGroups[i]->getObjectID()); // This will call announceIkGroupWillBeErased!
			i=0; // order may have changed!
		}
		else
			i++;
	}
}

void CRegisterediks::resetCalculationResults()
{
	for (int i=0;i<int(ikGroups.size());i++)
		ikGroups[i]->resetCalculationResult();
}

void CRegisterediks::announceIkGroupWillBeErased(int ikGroupID)
{ // Never called from copy buffer!
	int i=0;
	while (i<int(ikGroups.size()))
	{
		if (ikGroups[i]->announceIkGroupWillBeErased(ikGroupID,false))
		{ // This ik group has to be erased (normally never happens)
			removeIkGroup(ikGroups[i]->getObjectID()); // This will call announceIkGroupWillBeErased!
			i=0; // ordering may have changed!
		}
		else
			i++;
	}
}

int CRegisterediks::computeAllIkGroups(bool exceptExplicitHandling)
{
	int performedCount=0;

	if (App::ct->mainSettings->ikCalculationEnabled)
	{
		for (int i=0;i<int(ikGroups.size());i++)
		{
			if ((!exceptExplicitHandling)||(!ikGroups[i]->getExplicitHandling()))
			{
				int stTime=VDateTime::getTimeInMs();
				int res=0;
				res=ikGroups[i]->computeGroupIk(false);
				ikGroups[i]->setCalculationResult(res,VDateTime::getTimeDiffInMs(stTime));
				if (res!=sim_ikresult_not_performed)
					performedCount++;
			}
		}
	}
	return(performedCount);
}



