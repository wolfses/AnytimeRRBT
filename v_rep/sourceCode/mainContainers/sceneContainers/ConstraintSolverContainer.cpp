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
#include "ConstraintSolverContainer.h"
#include "Tt.h"
#include "IloIlo.h"
#include "App.h"
#include "GeometricConstraintSolverInt.h"

CConstraintSolverContainer::CConstraintSolverContainer()
{
	newSceneProcedure();
}

CConstraintSolverContainer::~CConstraintSolverContainer()
{
	removeAllObjects();
}

void CConstraintSolverContainer::newSceneProcedure()
{
	removeAllObjects();
}


void CConstraintSolverContainer::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

void CConstraintSolverContainer::simulationAboutToStart()
{
	for (int i=0;i<int(allGcsObjects.size());i++)
		allGcsObjects[i]->simulationAboutToStart();
}

void CConstraintSolverContainer::simulationEnded()
{
//	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
//	{
//	}
	for (int i=0;i<int(allGcsObjects.size());i++)
		allGcsObjects[i]->simulationEnded();
}

int CConstraintSolverContainer::computeAllMechanisms(bool exceptExplicitHandling)
{
	int performedCount=0;
	removeMultipleDefinedObjects();
	if (App::ct->mainSettings->gcsCalculationEnabled)
	{
		for (int i=0;i<int(allGcsObjects.size());i++)
		{
			if ((!exceptExplicitHandling)||(!allGcsObjects[i]->getExplicitHandling()))
			{
				if (allGcsObjects[i]->computeGcs())
					performedCount++;
			}
		}
	}
	return(performedCount);
}

void CConstraintSolverContainer::removeMultipleDefinedObjects()
{
	App::ct->objCont->actualizeMechanismIDs();
	std::vector<int> markedMechanismIDs;
	for (int i=0;i<int(allGcsObjects.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(allGcsObjects[i]->getBase3DObject());
		if (it==NULL)
		{
			delete allGcsObjects[i];
			allGcsObjects.erase(allGcsObjects.begin()+i);
			i--;
		}
		else
		{
			int mechanismID=it->getMechanismID();
			bool alreadyPresent=false;
			for (int j=0;j<int(markedMechanismIDs.size());j++)
			{
				if (markedMechanismIDs[j]==mechanismID)
				{
					alreadyPresent=true;
					break;
				}
			}
			if (alreadyPresent)
			{
				delete allGcsObjects[i];
				allGcsObjects.erase(allGcsObjects.begin()+i);
				i--;
			}
			else
				markedMechanismIDs.push_back(mechanismID);
		}
	}
}

CConstraintSolverObject* CConstraintSolverContainer::getObject(int objectID)
{
	for (int i=0;i<int(allGcsObjects.size());i++)
	{
		if (allGcsObjects[i]->getObjectID()==objectID)
			return(allGcsObjects[i]);
	}
	return(NULL);
}

CConstraintSolverObject* CConstraintSolverContainer::getObject(const char* objectName)
{
	for (int i=0;i<int(allGcsObjects.size());i++)
	{
		if (allGcsObjects[i]->getObjectName()==objectName)
			return(allGcsObjects[i]);
	}
	return(NULL);
}

CConstraintSolverObject* CConstraintSolverContainer::getObjectFromMechanismID(int mechanismID)
{
	for (int i=0;i<int(allGcsObjects.size());i++)
	{
		int objID=allGcsObjects[i]->getBase3DObject();
		CGeometricConstraintSolverInt work(objID,-1);
		if (work.getMechanismID()==mechanismID)
			return(allGcsObjects[i]);
	}
	return(NULL);
}

void CConstraintSolverContainer::getMinAndMaxNameSuffixes(int& minSuffix,int& maxSuffix)
{
	minSuffix=-1;
	maxSuffix=-1;
	for (int i=0;i<int(allGcsObjects.size());i++)
	{
		int s=tt::getNameSuffixNumber(allGcsObjects[i]->getObjectName().c_str(),true);
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

bool CConstraintSolverContainer::canSuffix1BeSetToSuffix2(int suffix1,int suffix2)
{
	for (int i=0;i<int(allGcsObjects.size());i++)
	{
		int s1=tt::getNameSuffixNumber(allGcsObjects[i]->getObjectName().c_str(),true);
		if (s1==suffix1)
		{
			std::string name1(tt::getNameWithoutSuffixNumber(allGcsObjects[i]->getObjectName().c_str(),true));
			for (int j=0;j<int(allGcsObjects.size());j++)
			{
				int s2=tt::getNameSuffixNumber(allGcsObjects[j]->getObjectName().c_str(),true);
				if (s2==suffix2)
				{
					std::string name2(tt::getNameWithoutSuffixNumber(allGcsObjects[j]->getObjectName().c_str(),true));
					if (name1==name2)
						return(false); // NO! We would have a name clash!
				}
			}
		}
	}
	return(true);
}

void CConstraintSolverContainer::setSuffix1ToSuffix2(int suffix1,int suffix2)
{
	for (int i=0;i<int(allGcsObjects.size());i++)
	{
		int s1=tt::getNameSuffixNumber(allGcsObjects[i]->getObjectName().c_str(),true);
		if (s1==suffix1)
		{
			std::string name1(tt::getNameWithoutSuffixNumber(allGcsObjects[i]->getObjectName().c_str(),true));
			allGcsObjects[i]->setObjectName(tt::generateNewName_dash(name1,suffix2+1).c_str());
		}
	}
}

void CConstraintSolverContainer::addObject(CConstraintSolverObject* anObject,bool objectIsACopy)
{
	addObjectWithSuffixOffset(anObject,objectIsACopy,1);
}

void CConstraintSolverContainer::addObjectWithSuffixOffset(CConstraintSolverObject* anObject,bool objectIsACopy,int suffixOffset)
{
	int newID=SIM_IDSTART_GCSOBJECT;
	while (getObject(newID)!=NULL)
		newID++;
	anObject->setObjectID(newID);
	std::string newName(anObject->getObjectName());
	if (objectIsACopy)
		newName=tt::generateNewName_dash(newName,suffixOffset);
	else
	{
		while (getObject(newName.c_str())!=NULL)
			newName=tt::generateNewName_noDash(newName);
	}
	anObject->setObjectName(newName.c_str());
	allGcsObjects.push_back(anObject);
}

void CConstraintSolverContainer::removeObject(int objectID)
{
	App::ct->objCont->announceGcsObjectWillBeErased(objectID);
	for (int i=0;i<int(allGcsObjects.size());i++)
	{
		if (allGcsObjects[i]->getObjectID()==objectID)
		{
			delete allGcsObjects[i];
			allGcsObjects.erase(allGcsObjects.begin()+i);
			App::ct->objCont->setFullDialogRefreshFlag();
			return;
		}
	}
}

void CConstraintSolverContainer::removeAllObjects()
{
	for (int i=0;i<int(allGcsObjects.size());i++)
		delete allGcsObjects[i];
	allGcsObjects.clear();
}

void CConstraintSolverContainer::announceObjectWillBeErased(int objID)
{ // Never called from copy buffer!
	int i=0;
	while (i<int(allGcsObjects.size()))
	{
		if (allGcsObjects[i]->announceObjectWillBeErased(objID,false))
		{ // We have to remove this object
			removeObject(allGcsObjects[i]->getObjectID()); // This will call announceGcsObjectWillBeErased!
			i=0; // Ordering may have changed
		}
		else
			i++;
	}

}
