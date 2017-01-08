// This file is part of the EXTERNAL IK for V-REP
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The EXTERNAL IK is licensed under the terms of EITHER:
//   1. EXTERNAL IK commercial license (contact us for details)
//   2. EXTERNAL IK educational license (see below)
// 
// EXTERNAL IK educational license:
// -------------------------------------------------------------------
// The EXTERNAL IK educational license applies only to EDUCATIONAL
// ENTITIES composed by following people and institutions:
// 
// 1. Hobbyists, students, teachers and professors
// 2. Schools and universities
// 
// EDUCATIONAL ENTITIES do NOT include companies, research institutions,
// non-profit organisations, foundations, etc.
// 
// An EDUCATIONAL ENTITY may use, modify, compile and distribute the
// modified/unmodified EXTERNAL IK under following conditions:
//  
// 1. Distribution should be free of charge.
// 2. Distribution should be to EDUCATIONAL ENTITIES only.
// 3. Usage should be non-commercial.
// 4. Altered source versions must be plainly marked as such and distributed
//    along with any compiled code.
// 5. The origin of the EXTERNAL IK must not be misrepresented. you must
//    not claim that you wrote the original software.
// 
// THE EXTERNAL IK IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#include "Registerediks.h"
#include "ikRoutine.h"
#include "Ct.h"
#include "Tt.h"
#include "v_repConst.h"


CRegisterediks::CRegisterediks()
{
}

CRegisterediks::~CRegisterediks()
{
	removeAllIkGroups();
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
	ct::objCont->announceIkGroupWillBeErased(ikGroupID);
	for (int i=0;i<int(ikGroups.size());i++)
	{
		if (ikGroups[i]->getObjectID()==ikGroupID)
		{
			delete ikGroups[i];
			ikGroups.erase(ikGroups.begin()+i);
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

	{
		for (int i=0;i<int(ikGroups.size());i++)
		{
			if ((!exceptExplicitHandling)||(!ikGroups[i]->getExplicitHandling()))
			{
				int res=0;
				res=ikGroups[i]->computeGroupIk(false);
				ikGroups[i]->setCalculationResult(res,0);
				if (res!=sim_ikresult_not_performed)
					performedCount++;
			}
		}
	}
	return(performedCount);
}



void CRegisterediks::addIkGroupExtIk(CikGroup* anIkGroup)
{ // Be careful! We don't check if the group is valid!!
	ikGroups.push_back(anIkGroup);
}
