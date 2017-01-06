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
#include "MemorizedConfContainer.h"
#include "App.h"
#include "Tt.h"

CMemorizedConfContainer::CMemorizedConfContainer()
{
	removeMemorized();	
}

CMemorizedConfContainer::~CMemorizedConfContainer()
{
	removeMemorized();	
}

void CMemorizedConfContainer::simulationAboutToStart()
{

}

void CMemorizedConfContainer::simulationEnded()
{

}

void CMemorizedConfContainer::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

void CMemorizedConfContainer::memorize()
{
	removeMemorized();
	for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
		if (it!=NULL)
			configurations.push_back(new CMemorizedConf(it));
	}
}

void CMemorizedConfContainer::restoreMemorized()
{
	if (isBufferEmpty()) // this also destroys unexisting configurations
		return;
	for (int i=0;i<int(configurations.size());i++)
	{
		CMemorizedConf* aConf=configurations[i];
		aConf->restore();
	}
}

bool CMemorizedConfContainer::isBufferEmpty()
{ // this also destroys all objects that don't exist anymore:
	for (int i=0;i<int(configurations.size());i++)
	{
		CMemorizedConf* aConf=configurations[i];
		if (!aConf->doesStillExist())
		{
			delete aConf;
			configurations.erase(configurations.begin()+i);
			i--; // one element was removed, we have to adjust the loop!
		}
	}
	return(configurations.size()==0);
}

void CMemorizedConfContainer::removeMemorized()
{
	for (int i=0;i<int(configurations.size());i++)
		delete configurations[i];
	configurations.clear();
}

char* CMemorizedConfContainer::getObjectConfiguration(int objID)
{
	std::vector<char> data;
	C3DObject* it=App::ct->objCont->getObject(objID);
	if (it==NULL)
		return(NULL);
	CMemorizedConf temp(it);
	temp.serializeToMemory(data);
	char* retBuffer=new char[data.size()+4];
	((int*)retBuffer)[0]=int(data.size());
	for (int i=0;i<int(data.size());i++)
		retBuffer[4+i]=data[i];
	return(retBuffer);
}

void CMemorizedConfContainer::setObjectConfiguration(const char* data)
{
	if (data==NULL)
		return;
	std::vector<char> arr;
	int l=((int*)data)[0];
	for (int i=0;i<l;i++)
		arr.push_back(data[i]);
	CMemorizedConf temp;
	temp.serializeFromMemory(arr);
	temp.restore();
}

char* CMemorizedConfContainer::getConfigurationTree(int objID)
{
	std::vector<char> data;
	C3DObject* it=App::ct->objCont->getObject(objID);
	std::vector<C3DObject*> sel;
	if (it==NULL)
	{ // We memorize everything:
		for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
			sel.push_back(App::ct->objCont->getObject(App::ct->objCont->objectList[i]));
	}
	else
	{ // We memorize just the object and all its children:
		it->getAllObjectsRecursive(&sel,true,true);
	}
	for (int i=0;i<int(sel.size());i++)
	{
		CMemorizedConf temp(sel[i]);
		temp.serializeToMemory(data);
	}
	char* retBuffer=new char[data.size()+4];
	((int*)retBuffer)[0]=int(data.size());
	for (int i=0;i<int(data.size());i++)
		retBuffer[4+i]=data[i];
	return(retBuffer);
}

void CMemorizedConfContainer::setConfigurationTree(const char* data)
{
	if (data==NULL)
		return;
	int l=((int*)data)[0];
	std::vector<char> arr;
	for (int i=0;i<l;i++)
		arr.push_back(data[i+4]);
	std::vector<CMemorizedConf*> allConfs;
	std::vector<int> parentCount;
	std::vector<int> index;
	while (arr.size()!=0)
	{
		CMemorizedConf* temp=new CMemorizedConf();
		temp->serializeFromMemory(arr);
		parentCount.push_back(temp->getParentCount());
		index.push_back(index.size());
		allConfs.push_back(temp);
	}
	tt::orderAscending(parentCount,index);
	for (int i=0;i<int(index.size());i++)
	{
		allConfs[index[i]]->restore();
		delete allConfs[index[i]];
	}
}
