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
#include "DrawingContainer.h"
#include "ViewableBase.h"
#include "OGL.h"
#include "easyLock.h"

CDrawingContainer::CDrawingContainer()
{
}

CDrawingContainer::~CDrawingContainer()
{
	removeAllObjects(false);
}

void CDrawingContainer::simulationAboutToStart()
{

}

void CDrawingContainer::simulationEnded()
{
	removeAllObjects(true);
//	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
//	{
//	}
}

void CDrawingContainer::renderYour3DStuff_nonTransparent(CViewableBase* renderingObject,int displayAttrib)
{
	drawAll(false,false,displayAttrib,renderingObject->getCumulativeTransformation().getMatrix());
}

void CDrawingContainer::renderYour3DStuff_transparent(CViewableBase* renderingObject,int displayAttrib)
{
	drawAll(false,true,displayAttrib,renderingObject->getCumulativeTransformation().getMatrix());
}

void CDrawingContainer::renderYour3DStuff_overlay(CViewableBase* renderingObject,int displayAttrib)
{
	drawAll(true,true,displayAttrib,renderingObject->getCumulativeTransformation().getMatrix());
}


void CDrawingContainer::newSceneProcedure()
{ // don't do anything here! (plugin or add-on might be using that functionality too) 
}

CDrawingObject* CDrawingContainer::getObject(int objectID)
{
	for (int i=0;i<int(_allObjects.size());i++)
	{
		if (_allObjects[i]->getObjectID()==objectID)
			return(_allObjects[i]);
	}
	return(NULL);
}


int CDrawingContainer::addObject(CDrawingObject* it)
{
	EASYLOCK(_objectMutex);
	int newID=0;
	newID++;
	while (getObject(newID)!=NULL)
		newID++;
	it->setObjectID(newID);
	_allObjects.push_back(it);
	return(newID);
}

void CDrawingContainer::removeAllObjects(bool onlyThoseCreatedFromScripts)
{
	EASYLOCK(_objectMutex);
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

void CDrawingContainer::removeObject(int objectID)
{
	EASYLOCK(_objectMutex);
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

void CDrawingContainer::drawAll(bool overlay,bool transparentObject,int displayAttrib,const C4X4Matrix& cameraCTM)
{
	EASYLOCK(_objectMutex);
	for (int i=0;i<int(_allObjects.size());i++)
		_allObjects[i]->draw(overlay,transparentObject,displayAttrib,cameraCTM);
}

void CDrawingContainer::drawObjectsParentedWith(bool overlay,bool transparentObject,int parentObjectID,int displayAttrib,const C4X4Matrix& cameraCTM)
{
	EASYLOCK(_objectMutex);
	for (int i=0;i<int(_allObjects.size());i++)
	{
		if ( (_allObjects[i]->getSceneObjectID()==parentObjectID)&&((_allObjects[i]->getObjectType()&sim_drawing_painttag)!=0) )
			_allObjects[i]->draw(overlay,transparentObject,displayAttrib,cameraCTM);
	}
}

bool CDrawingContainer::getExportableMeshAtIndex(int parentObjectID,int index,std::vector<float>& vertices,std::vector<int>& indices)
{
	vertices.clear();
	indices.clear();
	int cnt=0;
	for (int i=0;i<int(_allObjects.size());i++)
	{
		if ((_allObjects[i]->getSceneObjectID()==parentObjectID)&&_allObjects[i]->canMeshBeExported())
		{
			cnt++;
			if (cnt==index+1)
			{
				_allObjects[i]->getExportableMesh(vertices,indices);
				return(true);
			}
		}
	}
	return(false);
}

void CDrawingContainer::adjustForFrameChange(int objectID,const C7Vector& preCorrection)
{
	for (int i=0;i<int(_allObjects.size());i++)
	{
		if (_allObjects[i]->getSceneObjectID()==objectID)
			_allObjects[i]->adjustForFrameChange(preCorrection);
	}
}

void CDrawingContainer::adjustForScaling(int objectID,float xScale,float yScale,float zScale)
{
	for (int i=0;i<int(_allObjects.size());i++)
	{
		if (_allObjects[i]->getSceneObjectID()==objectID)
			_allObjects[i]->adjustForScaling(xScale,yScale,zScale);
	}
}


void CDrawingContainer::announceObjectWillBeErased(int objID)
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
