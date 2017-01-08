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
#include "RegGroupEl.h"
#include "Global.h"
#include "3DObject.h"
#include "OGL.h"
#include "Tt.h"
#include "App.h"
#include "IloIlo.h"

CRegGroupEl::CRegGroupEl(int objID,int grType,bool add)
{
	objectID=objID;
	groupType=grType;
	additive=add;
	subGroupID=0;
}

CRegGroupEl::~CRegGroupEl()
{
}

void CRegGroupEl::addOrRemoveAnObject(std::vector<int>* theObjects,int objectID)
{
	if (additive)
	{
		for (int i=0;i<int(theObjects->size());i++)
		{
			if (theObjects->at(i)==objectID)
				return;
		}
		theObjects->push_back(objectID);
	}
	else
	{
		for (int i=0;i<int(theObjects->size());i++)
		{
			if (theObjects->at(i)==objectID)
			{
				theObjects->erase(theObjects->begin()+i);
				return;
			}
		}
	}
}

void CRegGroupEl::addOrRemoveYourObjects(std::vector<int>* theObjects)
{
	C3DObject* it=App::ct->objCont->getObject(objectID);
	if (it==NULL)
	{
		for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
			addOrRemoveAnObject(theObjects,App::ct->objCont->objectList[i]);
	}
	if (groupType==GROUP_LOOSE)
		addOrRemoveAnObject(theObjects,it->getID());
	if ( (groupType==GROUP_FROM_BASE_INCLUDED)||(groupType==GROUP_FROM_BASE_EXCLUDED) )
	{
		std::vector<C3DObject*> allObj;
		allObj.reserve(8);
		allObj.clear();
		it->getAllObjectsRecursive(&allObj,groupType==GROUP_FROM_BASE_INCLUDED);
		for (int i=0;i<int(allObj.size());i++)
			addOrRemoveAnObject(theObjects,allObj[i]->getID());
	}
	if ( (groupType==GROUP_FROM_TIP_INCLUDED)||(groupType==GROUP_FROM_TIP_EXCLUDED) )
	{
		if (groupType==GROUP_FROM_TIP_INCLUDED)
			addOrRemoveAnObject(theObjects,it->getID());
		C3DObject* itIt=it->getParent();
		while (itIt!=NULL)
		{
			addOrRemoveAnObject(theObjects,itIt->getID());
			itIt=itIt->getParent();
		}
	}
}

bool CRegGroupEl::isAdditive()
{
	return(additive);
}

int CRegGroupEl::getMainObject()
{
	return(objectID);
}

int CRegGroupEl::getGroupType()
{
	return(groupType);
}

bool CRegGroupEl::isSame(CRegGroupEl* it)
{
	if (it->getMainObject()!=objectID)
		return(false);
	if (it->getGroupType()!=groupType)
		return(false);
	return(it->isAdditive()==additive);
}

int CRegGroupEl::getSubGroupID()
{
	return(subGroupID);
}

void CRegGroupEl::setSubGroupID(int newID)
{
	subGroupID=newID;
}

void CRegGroupEl::performObjectLoadingMapping(std::vector<int>* map)
{
	objectID=App::ct->objCont->getLoadingMapping(map,objectID);
}

CRegGroupEl* CRegGroupEl::copyYourself()
{
	CRegGroupEl* newGroupEl=new CRegGroupEl(objectID,groupType,additive);
	newGroupEl->setSubGroupID(subGroupID);
	return(newGroupEl);
}

void CRegGroupEl::serialize(CSer& ar)
{
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Obj");
		ar << objectID << subGroupID;
		ar.flush();

		ar.storeDataName("Gty");
		ar << (BYTE)groupType;
		ar.flush();
		
		ar.storeDataName("Var");
		BYTE dummy=0;
		dummy=dummy+1*additive;
		ar << dummy;
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
				if (theName.compare("Obj")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> objectID >> subGroupID;
				}
				if (theName.compare("Gty")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
					groupType=(int)dummy;
				}
				if (theName.compare("Var")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
					additive=(dummy&1)==1;
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}
