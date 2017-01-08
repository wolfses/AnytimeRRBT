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
#include "ptCloud.h"
#include "MainCont.h"
#include "7Vector.h"
#include "VMutex.h"

class CPointCloudContainer : public CMainCont
{
public:
	CPointCloudContainer();
	virtual ~CPointCloudContainer();

	void newSceneProcedure();
	void simulationAboutToStart();
	void simulationEnded();
	void renderYour3DStuff_nonTransparent(CViewableBase* renderingObject,int displayAttrib);
	void renderYour3DStuff_transparent(CViewableBase* renderingObject,int displayAttrib);
	void renderYour3DStuff_overlay(CViewableBase* renderingObject,int displayAttrib);
	int addObject(CPtCloud* it);
	CPtCloud* getObject(int objectID);
	void removeAllObjects(bool onlyNonPersistentOnes);
	void announceObjectWillBeErased(int objID);
	bool removeObject(int objectID);
	void drawAll();//bool overlay,bool transparentObject,int displayAttrib,const C4X4Matrix& cameraCTM);

private:
	std::vector<CPtCloud*> _allObjects;
	VMutex _objectMutex;
};
