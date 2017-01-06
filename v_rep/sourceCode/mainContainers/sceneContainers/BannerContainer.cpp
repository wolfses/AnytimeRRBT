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
#include "BannerContainer.h"
#include "ViewableBase.h"
#include "OGL.h"

CBannerContainer::CBannerContainer()
{
}

CBannerContainer::~CBannerContainer()
{
	removeAllObjects(false);
}

void CBannerContainer::simulationAboutToStart()
{
}

void CBannerContainer::simulationEnded()
{
	removeAllObjects(true);
//	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
//	{
//	}
}

void CBannerContainer::renderYour3DStuff_nonTransparent(CViewableBase* renderingObject,int displayAttrib,int windowSize[2],float verticalViewSizeOrAngle,bool perspective)
{
	drawAll3DStuff(false,false,displayAttrib,renderingObject->getCumulativeTransformation().getMatrix(),windowSize,verticalViewSizeOrAngle,perspective);
}

void CBannerContainer::renderYour3DStuff_transparent(CViewableBase* renderingObject,int displayAttrib,int windowSize[2],float verticalViewSizeOrAngle,bool perspective)
{
	drawAll3DStuff(false,true,displayAttrib,renderingObject->getCumulativeTransformation().getMatrix(),windowSize,verticalViewSizeOrAngle,perspective);
}

void CBannerContainer::renderYour3DStuff_overlay(CViewableBase* renderingObject,int displayAttrib,int windowSize[2],float verticalViewSizeOrAngle,bool perspective)
{
	drawAll3DStuff(true,true,displayAttrib,renderingObject->getCumulativeTransformation().getMatrix(),windowSize,verticalViewSizeOrAngle,perspective);
}


void CBannerContainer::newSceneProcedure()
{ // don't do anything here! (plugin or add-on might be using that functionality too) 
}

CBannerObject* CBannerContainer::getObject(int objectID)
{
	for (int i=0;i<int(_allObjects.size());i++)
	{
		if (_allObjects[i]->getObjectID()==objectID)
			return(_allObjects[i]);
	}
	return(NULL);
}


int CBannerContainer::addObject(CBannerObject* it)
{
	int newID=0;
	newID++;
	while (getObject(newID)!=NULL)
		newID++;
	it->setObjectID(newID);
	_allObjects.push_back(it);
	return(newID);
}

void CBannerContainer::removeAllObjects(bool onlyThoseCreatedFromScripts)
{
	for (int i=0;i<int(_allObjects.size());i++)
	{
		if ( (!onlyThoseCreatedFromScripts)||_allObjects[i]->getCreatedFromScript() )
		{
			delete _allObjects[i];
			_allObjects.erase(_allObjects.begin()+i);
			i--; // reprocess this position
		}
	}
}

void CBannerContainer::removeObject(int objectID)
{
	for (int i=0;i<int(_allObjects.size());i++)
	{
		if (_allObjects[i]->getObjectID()==objectID)
		{
			delete _allObjects[i];
			_allObjects.erase(_allObjects.begin()+i);
			break;
		}
	}
}

void CBannerContainer::drawAll3DStuff(bool overlay,bool transparentObject,int displayAttrib,const C4X4Matrix& cameraCTM,int windowSize[2],float verticalViewSizeOrAngle,bool perspective)
{
	for (int i=0;i<int(_allObjects.size());i++)
		_allObjects[i]->draw3DStuff(overlay,transparentObject,displayAttrib,cameraCTM,windowSize,verticalViewSizeOrAngle,perspective);
}

void CBannerContainer::renderAll2DBanners()
{
	for (int i=0;i<int(_allObjects.size());i++)
		_allObjects[i]->draw2DStuff();
}

bool CBannerContainer::mouseDownOn2DWindow(int mouseX,int mouseY)
{
	for (int i=int(_allObjects.size())-1;i>=0;i--)
	{ // We have to go from last to first since it is rendered from first to last!!
		if (_allObjects[i]->mouseDownOn2DWindow(mouseX,mouseY))
			return(true);
	}
	return(false);
}

void CBannerContainer::adjustForFrameChange(int objectID,const C7Vector& preCorrection)
{
	for (int i=0;i<int(_allObjects.size());i++)
	{
		if (_allObjects[i]->getSceneObjectID()==objectID)
			_allObjects[i]->adjustForFrameChange(preCorrection);
	}
}

void CBannerContainer::adjustForScaling(int objectID,float xScale,float yScale,float zScale)
{
	for (int i=0;i<int(_allObjects.size());i++)
	{
		if (_allObjects[i]->getSceneObjectID()==objectID)
			_allObjects[i]->adjustForScaling(xScale,yScale,zScale);
	}
}


void CBannerContainer::announceObjectWillBeErased(int objID)
{ // Never called from copy buffer!
	int i=0;
	while (i<int(_allObjects.size()))
	{
		if (_allObjects[i]->announceObjectWillBeErased(objID))
		{
			delete _allObjects[i];
			_allObjects.erase(_allObjects.begin()+i);
		}
		else
			i++;
	}
}
