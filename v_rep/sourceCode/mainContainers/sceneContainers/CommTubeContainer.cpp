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
#include "CommTubeContainer.h"
#include "v_rep_internal.h"

int CCommTubeContainer::_nextPartnerID=0;

CCommTubeContainer::CCommTubeContainer()
{

}

CCommTubeContainer::~CCommTubeContainer()
{
	removeAllTubes();
}

void CCommTubeContainer::removeAllTubes()
{
	for (int i=0;i<int(_allTubes.size());i++)
		delete _allTubes[i];
}


void CCommTubeContainer::newSceneProcedure()
{ // don't do anything here! (plugin or add-on might be using that functionality too) 
}

void CCommTubeContainer::simulationAboutToStart()
{

}

void CCommTubeContainer::simulationEnded()
{ // Remove handles that were created from a script:
	for (int i=0;i<int(_allTubes.size());i++)
	{
		if (_allTubes[i]->simulationEnded())
		{
			delete _allTubes[i];
			_allTubes.erase(_allTubes.begin()+i);
			i--; // We need to reprocess this position
		}
	}
}

void CCommTubeContainer::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

int CCommTubeContainer::openTube(int header,const std::string& identifier,bool killAtSimulationEnd,int readBufferSize)
{ // return value is the tube handle for this partner
	// 1. Check if a related tube exists:
	int tubeIndex=-1;
	for (int i=0;i<int(_allTubes.size());i++)
	{
		if (_allTubes[i]->isSameHeaderAndIdentifier(header,identifier))
		{
			tubeIndex=i;
			break;
		}
	}
	if (tubeIndex!=-1)
	{ // Yes, such a tube already exists
		// 2. Is it already connected?
		if (_allTubes[tubeIndex]->isConnected())
			return(-1); // yes, indicate an error
		else
		{ // No, connect it:
			int retVal;
			retVal=2*_nextPartnerID+0;
			_nextPartnerID++;
			_allTubes[tubeIndex]->connectPartner(retVal,killAtSimulationEnd,readBufferSize);
			return(retVal);
		}
	}
	else
	{ // No, we have to create such a tube
		int retVal;
		retVal=2*_nextPartnerID+0;
		_nextPartnerID++;
		CCommTube* it=new CCommTube(header,identifier,retVal,killAtSimulationEnd,readBufferSize);
		_allTubes.push_back(it);
		return(retVal);
	}
}

bool CCommTubeContainer::closeTube(int tubeHandle)
{ // returns true if tube could be closed
	int index=_getTubeIndexForHandle(tubeHandle);
	if (index!=-1)
	{
		if (_allTubes[index]->disconnectPartner(tubeHandle))
		{
			delete _allTubes[index];
			_allTubes.erase(_allTubes.begin()+index);
		}
		return(true);
	}
	return(false);
}

int CCommTubeContainer::_getTubeIndexForHandle(int tubeHandle)
{
	for (int i=0;i<int(_allTubes.size());i++)
	{
		if (_allTubes[i]->isPartnerThere(tubeHandle))
			return(i);
	}
	return(-1);
}

bool CCommTubeContainer::writeToTube_copyBuffer(int tubeHandle,const char* data,int dataLength)
{
	int index=_getTubeIndexForHandle(tubeHandle);
	if ( (index==-1)||(dataLength==0) )
		return(false);
	if (!_allTubes[index]->isConnected()) // Added on 2011/01/06 (writing to a non-connected tube will otherwise result in memory leak)
		return(false);
	char* data2=new char[dataLength];
	for (int i=0;i<dataLength;i++)
		data2[i]=data[i];
	bool retVal=_allTubes[index]->writeData(tubeHandle,data2,dataLength);
	if (!retVal) // Following two lines added on 22/11/2011 because of some memory leak
		delete[] data2;
	return(retVal);
}

char* CCommTubeContainer::readFromTube_bufferNotCopied(int tubeHandle,int& dataLength)
{
	int index=_getTubeIndexForHandle(tubeHandle);
	if (index==-1)
		return(NULL);
	char* retVal=_allTubes[index]->readData(tubeHandle,dataLength);
	return(retVal);
}

int CCommTubeContainer::getTubeStatus(int tubeHandle,int& readBufferFill,int& writeBufferFill)
{
	int index=_getTubeIndexForHandle(tubeHandle);
	if (index==-1)
		return(-1); // Tube inexistant
	return(_allTubes[index]->getTubeStatus(tubeHandle,readBufferFill,writeBufferFill));
}
