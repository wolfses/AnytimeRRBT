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

#pragma once

#include <vector>
#include "DrawingObject.h"
#include "MainCont.h"
#include "7Vector.h"

class CDrawingContainer : public CMainCont
{
public:
	CDrawingContainer();
	virtual ~CDrawingContainer();

	void newSceneProcedure();
	void simulationAboutToStart();
	void simulationEnded();
	void renderYour3DStuff_nonTransparent(CViewableBase* renderingObject,int displayAttrib);
	void renderYour3DStuff_transparent(CViewableBase* renderingObject,int displayAttrib);
	void renderYour3DStuff_overlay(CViewableBase* renderingObject,int displayAttrib);
	int addObject(CDrawingObject* it);
	CDrawingObject* getObject(int objectID);
	void removeAllObjects(bool onlyThoseCreatedFromScripts);
	void announceObjectWillBeErased(int objID);
	void removeObject(int objectID);
	void adjustForFrameChange(int objectID,const C7Vector& preCorrection);
	void adjustForScaling(int objectID,float xScale,float yScale,float zScale);
	void drawAll(bool overlay,bool transparentObject,int displayAttrib,const C4X4Matrix& cameraCTM);
	void drawObjectsParentedWith(bool overlay,bool transparentObject,int parentObjectID,int displayAttrib,const C4X4Matrix& cameraCTM);

	bool getExportableMeshAtIndex(int parentObjectID,int index,std::vector<float>& vertices,std::vector<int>& indices);
private:
	std::vector<CDrawingObject*> _allObjects;
	VMutex _objectMutex;
};
