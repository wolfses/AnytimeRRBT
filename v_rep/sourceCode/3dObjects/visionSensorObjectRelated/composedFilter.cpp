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
#include "composedFilter.h"
#include "OGL.h"
#include "visionSensor.h"

CComposedFilter::CComposedFilter()
{
	buffer1=NULL;
	buffer2=NULL;
	workImage=NULL;
}

CComposedFilter::~CComposedFilter()
{
	delete[] buffer1;
	delete[] buffer2;
	delete[] workImage;
	removeAllSimpleFilters();
}

int CComposedFilter::getSimpleFilterCount()
{
	return(_allSimpleFilters.size());
}
void CComposedFilter::insertSimpleFilter(CSimpleFilter* it)
{
	_allSimpleFilters.push_back(it);
}

CSimpleFilter* CComposedFilter::getSimpleFilter(int index)
{
	if ((index<0)||(index>=int(_allSimpleFilters.size())))
		return(NULL);
	return(_allSimpleFilters[index]);
}

CSimpleFilter* CComposedFilter::getSimpleFilterFromUniqueID(int uniqueID)
{
	for (int i=0;i<int(_allSimpleFilters.size());i++)
	{
		if (_allSimpleFilters[i]->getUniqueID()==uniqueID)
			return(_allSimpleFilters[i]);
	}
	return(NULL);
}

void CComposedFilter::removeSimpleFilter(int index)
{
	if ((index<0)||(index>=int(_allSimpleFilters.size())))
		return;
	delete _allSimpleFilters[index];
	_allSimpleFilters.erase(_allSimpleFilters.begin()+index);
}

void CComposedFilter::removeAllSimpleFilters()
{
	for (int i=0;i<int(_allSimpleFilters.size());i++)
		delete _allSimpleFilters[i];
	_allSimpleFilters.clear();
}

bool CComposedFilter::moveSimpleFilter(int index,bool up)
{
	if ((index<0)||(index>=int(_allSimpleFilters.size())))
		return(false);
	if ((index==0)&&up)
		return(false);
	if ((index==int(_allSimpleFilters.size())-1)&&(!up))
		return(false);
	if (up)
	{
		_allSimpleFilters.insert(_allSimpleFilters.begin()+index-1,_allSimpleFilters[index]);
		_allSimpleFilters.erase(_allSimpleFilters.begin()+index+1);
	}
	else
	{
		_allSimpleFilters.insert(_allSimpleFilters.begin()+index+2,_allSimpleFilters[index]);
		_allSimpleFilters.erase(_allSimpleFilters.begin()+index);
	}
	return(true);
}

CComposedFilter* CComposedFilter::copyYourself()
{
	CComposedFilter* newFilter=new CComposedFilter();
	for (int i=0;i<int(_allSimpleFilters.size());i++)
	{
		CSimpleFilter* it=_allSimpleFilters[i]->copyYourself();
		newFilter->_allSimpleFilters.push_back(it);
	}
	return(newFilter);
}

void CComposedFilter::serialize(CSer& ar)
{
	if (ar.isStoring())
	{
		for (int i=0;i<int(_allSimpleFilters.size());i++)
		{
			ar.storeDataName("Sfr");
			ar.setCountingMode();
			_allSimpleFilters[i]->serialize(ar);
			if (ar.setWritingMode())
				_allSimpleFilters[i]->serialize(ar);
		}

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
				if (theName.compare("Sfr")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					CSimpleFilter* it=new CSimpleFilter();
					it->serialize(ar);
					_allSimpleFilters.push_back(it);
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}

void CComposedFilter::initializeInitialValues(bool simulationIsRunning)
{ // following comment was still from the 'simulationAboutToStart' routine. Maybe now we could solve the problem in another way!
	// nothing for now (the buffers cannot be reserved here... if a sensor is copied during the simulation--> problem)
	if (simulationIsRunning)
	{
	}
	else
	{ // remember, simulationEnded is not guaranteed to be run (the object can be copied during a simulation and pasted after it ended!)
		removeBuffers();
	}
}

void CComposedFilter::simulationEnded()
{ // delete the buffers! Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	removeBuffers();
//	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
//	{
//	}
}

void CComposedFilter::removeBuffers()
{
	delete[] buffer1;
	buffer1=NULL;
	delete[] buffer2;
	buffer2=NULL;
	delete[] workImage;
	workImage=NULL;
}

bool CComposedFilter::processAndTrigger(CVisionSensor* sensor,int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,std::vector<std::vector<float> >& returnData)
{
	bool retVal=false;
	if (workImage==NULL)
	{
		int s=sizeX*sizeY*3;
		buffer1=new float[s];
		buffer2=new float[s];
		workImage=new float[s];
		for (int i=0;i<s;i++)
		{
			outputImage[i]=0.0f;
			workImage[i]=0.0f;
			buffer1[i]=0.0f;
			buffer2[i]=0.0f;
		}
	}
	drawingContainer.removeAllObjects();
	for (int i=0;i<int(_allSimpleFilters.size());i++)
	{
		CSimpleFilter* it=_allSimpleFilters[i];
		std::vector<float> retDat;
		bool trigger=it->processAndTrigger(sensor,sizeX,sizeY,inputImage,inputDepth,outputImage,workImage,retDat,buffer1,buffer2,drawingContainer);
		if (retDat.size()!=0)
			returnData.push_back(retDat);
		if (trigger)
			retVal=true;
	}
	return(retVal);
}

void CComposedFilter::displayOverlay(int c0[2],int c1[2])
{
	drawingContainer.render(c0,c1);
}
