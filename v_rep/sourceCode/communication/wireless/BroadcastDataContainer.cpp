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
#include "BroadcastDataContainer.h"
#include "IloIlo.h"
#include "App.h"
#include "VDateTime.h"

CBroadcastDataContainer::CBroadcastDataContainer()
{
	
}

CBroadcastDataContainer::~CBroadcastDataContainer()
{
	removeAllObjects();
}

void CBroadcastDataContainer::simulationAboutToStart()
{

}

void CBroadcastDataContainer::simulationEnded()
{
	removeAllObjects();
}

void CBroadcastDataContainer::broadcastData(int emitterID,int targetID,int dataHeader,std::string& dataName,float timeOutSimulationTime,float actionRadius,int antennaHandle,float emissionAngle1,float emissionAngle2,const char* data,int dataLength)
{ // Called by the SIM or UI thread
	EASYLOCK(_objectMutex);
	CBroadcastData* it=new CBroadcastData(emitterID,targetID,dataHeader,dataName,timeOutSimulationTime,actionRadius,antennaHandle,emissionAngle1,emissionAngle2,data,dataLength);
	_allObjects.push_back(it);
	if (App::ct->environment->getVisualizeWirelessEmitters()||CIloIlo::wirelessForceShow_emission)
	{
		bool err=false;
		C7Vector antennaConf;
		antennaConf.setIdentity();
		if (antennaHandle!=sim_handle_default)
		{
			C3DObject* it=App::ct->objCont->getObject(antennaHandle);
			if (it!=NULL)
				antennaConf=it->getCumulativeTransformationPart1();
			else
				err=true;
		}
		if (!err)
		{
			CBroadcastDataVisual* itv=new CBroadcastDataVisual(timeOutSimulationTime,actionRadius,antennaConf,emissionAngle1,emissionAngle2);
			_allVisualObjects.push_back(itv);
		}
	}
}

char* CBroadcastDataContainer::receiveData(int receiverID,float simulationTime,int dataHeader,std::string& dataName,int antennaHandle,int& dataLength,int index,int& senderID,int& dataHeaderR,std::string& dataNameR)
{
	EASYLOCK(_objectMutex);
	int originalIndex=index;
	char* retVal=NULL;
	for (int i=0;i<int(_allObjects.size());i++)
	{
		retVal=_allObjects[i]->receiveData(receiverID,simulationTime,dataHeader,dataName,antennaHandle,dataLength,senderID,dataHeaderR,dataNameR,originalIndex==-1);
		if (retVal!=NULL)
		{
			if (originalIndex==-1)
			{
				if (App::ct->environment->getVisualizeWirelessReceivers()||CIloIlo::wirelessForceShow_reception)
				{
					bool err=false;
					C3Vector antennaPos1;
					antennaPos1.clear();
					if (_allObjects[i]->getAntennaHandle()!=sim_handle_default)
					{
						C3DObject* it=App::ct->objCont->getObject(_allObjects[i]->getAntennaHandle());
						if (it!=NULL)
							antennaPos1=it->getCumulativeTransformationPart1().X;
						else
							err=true;
					}
					C3Vector antennaPos2;
					antennaPos2.clear();
					if (antennaHandle!=sim_handle_default)
					{
						C3DObject* it=App::ct->objCont->getObject(antennaHandle);
						if (it!=NULL)
							antennaPos2=it->getCumulativeTransformationPart1().X;
						else
							err=true;
					}
					if (!err)
					{
						CBroadcastDataVisual* itv=new CBroadcastDataVisual(antennaPos1,antennaPos2);
						_allVisualObjects.push_back(itv);
					}
				}
				return(retVal);
			}
			index--;
			if (index==-1)
				return(retVal);
			retVal=NULL;
		}
	}
	return(retVal);
}

void CBroadcastDataContainer::removeAllObjects()
{
	EASYLOCK(_objectMutex);
	for (int i=0;i<int(_allObjects.size());i++)
		delete _allObjects[i];
	_allObjects.clear();
	for (int i=0;i<int(_allVisualObjects.size());i++)
		delete _allVisualObjects[i];
	_allVisualObjects.clear();
}

void CBroadcastDataContainer::removeObject(int index)
{
	EASYLOCK(_objectMutex);
	delete _allObjects[index];
	_allObjects.erase(_allObjects.begin()+index);
}

void CBroadcastDataContainer::removeTimedOutObjects(float simulationTime)
{
	EASYLOCK(_objectMutex);
	for (int i=0;i<int(_allObjects.size());i++)
	{
		if (_allObjects[i]->doesRequireDestruction(simulationTime))
		{
			removeObject(i);
			i--;
		}
	}

	for (int i=0;i<int(_allVisualObjects.size());i++)
	{
		if (_allVisualObjects[i]->doesRequireDestruction(simulationTime))
		{
			delete _allVisualObjects[i];
			_allVisualObjects.erase(_allVisualObjects.begin()+i);
			i--;
		}
	}
}

void CBroadcastDataContainer::visualizeCommunications(int pcTimeInMs)
{ // called by the UI thread
	EASYLOCK(_objectMutex);
	for (int i=0;i<int(_allVisualObjects.size());i++)
		_allVisualObjects[i]->visualize();
	/*
	// First recievers...
	if (App::ct->environment->getVisualizeWirelessReceivers())
	{
		for (int i=0;i<int(_allVisualObjects.size());i++)
		{
			if (_allVisualObjects[i]->isReceiver())
			{
				if (_allVisualObjects[i]->visualize(pcTimeInMs))
				{
					delete _allVisualObjects[i];
					_allVisualObjects.erase(_allVisualObjects.begin()+i);
					i--;
				}
			}
		}
	}
	// ..then emitters
	if (App::ct->environment->getVisualizeWirelessEmitters())
	{
		for (int i=0;i<int(_allVisualObjects.size());i++)
		{
			if (!_allVisualObjects[i]->isReceiver())
			{
				if (_allVisualObjects[i]->visualize(pcTimeInMs))
				{
					delete _allVisualObjects[i];
					_allVisualObjects.erase(_allVisualObjects.begin()+i);
					i--;
				}
			}
		}
	}
	*/
}
