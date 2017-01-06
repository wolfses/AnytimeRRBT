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
#include "TextureContainer.h"
#include "Tt.h"
#include "IloIlo.h"
#include "App.h"

CTextureContainer::CTextureContainer()
{
	newSceneProcedure();
}

CTextureContainer::~CTextureContainer()
{
	removeAllObjects();
}

void CTextureContainer::newSceneProcedure()
{
}

void CTextureContainer::simulationAboutToStart()
{

}

void CTextureContainer::simulationEnded()
{

}

void CTextureContainer::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

CTextureObject* CTextureContainer::getObject(int objectID)
{
	for (int i=0;i<int(_allTextureObjects.size());i++)
	{
		if (_allTextureObjects[i]->getObjectID()==objectID)
			return(_allTextureObjects[i]);
	}
	return(NULL);
}

CTextureObject* CTextureContainer::getObject(const char* objectName)
{
	for (int i=0;i<int(_allTextureObjects.size());i++)
	{
		if (_allTextureObjects[i]->getObjectName()==objectName)
			return(_allTextureObjects[i]);
	}
	return(NULL);
}

CTextureObject* CTextureContainer::getObjectAtIndex(int index)
{
	if ( (index<0)||(index>=int(_allTextureObjects.size())) )
		return(NULL);
	return(_allTextureObjects[index]);
}

void CTextureContainer::getMinAndMaxNameSuffixes(int& minSuffix,int& maxSuffix)
{
	minSuffix=-1;
	maxSuffix=-1;
	for (int i=0;i<int(_allTextureObjects.size());i++)
	{
		int s=tt::getNameSuffixNumber(_allTextureObjects[i]->getObjectName().c_str(),true);
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

bool CTextureContainer::canSuffix1BeSetToSuffix2(int suffix1,int suffix2)
{
	for (int i=0;i<int(_allTextureObjects.size());i++)
	{
		int s1=tt::getNameSuffixNumber(_allTextureObjects[i]->getObjectName().c_str(),true);
		if (s1==suffix1)
		{
			std::string name1(tt::getNameWithoutSuffixNumber(_allTextureObjects[i]->getObjectName().c_str(),true));
			for (int j=0;j<int(_allTextureObjects.size());j++)
			{
				int s2=tt::getNameSuffixNumber(_allTextureObjects[j]->getObjectName().c_str(),true);
				if (s2==suffix2)
				{
					std::string name2(tt::getNameWithoutSuffixNumber(_allTextureObjects[j]->getObjectName().c_str(),true));
					if (name1==name2)
						return(false); // NO! We would have a name clash!
				}
			}
		}
	}
	return(true);
}

void CTextureContainer::setSuffix1ToSuffix2(int suffix1,int suffix2)
{
	for (int i=0;i<int(_allTextureObjects.size());i++)
	{
		int s1=tt::getNameSuffixNumber(_allTextureObjects[i]->getObjectName().c_str(),true);
		if (s1==suffix1)
		{
			std::string name1(tt::getNameWithoutSuffixNumber(_allTextureObjects[i]->getObjectName().c_str(),true));
			_allTextureObjects[i]->setObjectName(tt::generateNewName_dash(name1,suffix2+1).c_str());
		}
	}
}

int CTextureContainer::addObject(CTextureObject* anObject,bool objectIsACopy)
{ // If object already exists (well, similar object), it is destroyed in here!
	return(addObjectWithSuffixOffset(anObject,objectIsACopy,1));
}

int CTextureContainer::addObjectWithSuffixOffset(CTextureObject* anObject,bool objectIsACopy,int suffixOffset)
{ // If object already exists (well, similar object), it is destroyed in here!
	CTextureObject* theOldData=_getEquivalentTextureObject(anObject);
	if (theOldData!=NULL)
	{ // we already have a similar object!!
		// We transfer the dependencies (since 10/2/2012 (was forgotten before)):
		anObject->transferDependenciesToThere(theOldData);

		delete anObject;
		return(theOldData->getObjectID());
 	}

	int newID=SIM_IDSTART_TEXTURE;
	while (getObject(newID)!=NULL)
		newID++;
	anObject->setObjectID(newID);
	std::string newName(anObject->getObjectName());
	while (getObject(newName.c_str())!=NULL)
	{
		// TEXTURE OBJECTS SHOULDn'T HAVE A DASHED NAME!!
		newName=tt::generateNewName_noDash(newName);
	}
	anObject->setObjectName(newName.c_str());
	_allTextureObjects.push_back(anObject);
	return(newID);
}

CTextureObject* CTextureContainer::_getEquivalentTextureObject(CTextureObject *theData)
{
	for (int i=0;i<int(_allTextureObjects.size());i++)
	{
		if (_allTextureObjects[i]->isSame(theData))
			return(_allTextureObjects[i]);
	}
	return(NULL);
}

void CTextureContainer::removeObject(int objectID)
{
	for (int i=0;i<int(_allTextureObjects.size());i++)
	{
		if (_allTextureObjects[i]->getObjectID()==objectID)
		{
			delete _allTextureObjects[i];
			_allTextureObjects.erase(_allTextureObjects.begin()+i);
			App::ct->objCont->setFullDialogRefreshFlag();
			return;
		}
	}
}

void CTextureContainer::clearAllDependencies()
{
	for (int i=0;i<int(_allTextureObjects.size());i++)
		_allTextureObjects[i]->clearAllDependencies();
}

void CTextureContainer::updateAllDependencies()
{ // should not be called from "ct::objCont->addObjectsToSceneAndPerformMapping" routine!!
	clearAllDependencies();
	App::ct->buttonBlockContainer->setTextureDependencies();
	for (int i=0;i<int(App::ct->objCont->shapeList.size());i++)
		App::ct->objCont->getShape(App::ct->objCont->shapeList[i])->geomData->setTextureDependencies(App::ct->objCont->shapeList[i]);
}

void CTextureContainer::announceGeneralObjectWillBeErased(int generalObjectID,int subID)
{
	int i=0;
	while (i<int(_allTextureObjects.size()))
	{
		if (_allTextureObjects[i]->announceGeneralObjectWillBeErased(generalObjectID,subID))
		{
			removeObject(_allTextureObjects[i]->getObjectID());
			i=0; // ordering may have changed!
		}
		else
			i++;
	}
}

int CTextureContainer::getSameObjectID(CTextureObject* anObject)
{
	for (int i=0;i<int(_allTextureObjects.size());i++)
	{
		if (_allTextureObjects[i]->isSame(anObject))
			return(_allTextureObjects[i]->getObjectID());
	}	
	return(-1);
}

void CTextureContainer::removeAllObjects()
{
	for (int i=0;i<int(_allTextureObjects.size());i++)
		delete _allTextureObjects[i];
	_allTextureObjects.clear();
}

void CTextureContainer::storeTextureObject(CSer& ar,CTextureObject* it)
{
	ar.storeDataName(SER_TEXTURE);
	ar.setCountingMode();
	it->serialize(ar);
	if (ar.setWritingMode())
		it->serialize(ar);
}

CTextureObject* CTextureContainer::loadTextureObject(CSer& ar,std::string theName,bool &noHit)
{
	int byteNumber;
	if (theName.compare(SER_TEXTURE)==0)
	{
		noHit=false;
		ar >> byteNumber; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
		CTextureObject* myNewObject=new CTextureObject();
		myNewObject->serialize(ar);
		return(myNewObject);
	}
	return(NULL);
}
