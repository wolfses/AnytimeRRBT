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

class C3DObject;

class CViewSelector  
{
public:
	CViewSelector();
	virtual ~CViewSelector();

	void newSceneProcedure();
	void setUpDefaultValues();
	void render();
	void setViewSizeAndPosition(int sizeX,int sizeY,int posX,int posY);
	int getObjectIndexInViewSelection(int mousePos[2]);

	bool leftMouseButtonDown(int x,int y,int selectionStatus);
	void leftMouseButtonUp(int x,int y);
	void mouseMove(int x,int y,bool passiveAndFocused);
	int getCursor(int x,int y);
	C3DObject* getViewableObject(int x,int y);
	bool rightMouseButtonDown(int x,int y);
	void rightMouseButtonUp(int x,int y,int absX,int absY,QWidget* mainWindow);
	bool leftMouseButtonDoubleClick(int x,int y,int selectionStatus);
	void setViewSelectionInfo(int objType,int viewInd,int subViewInd);
	void keyPress(int key);
	bool evaluateMenuSelection(int commandID,int subViewIndex);
	int getCaughtElements();
	void clearCaughtElements(int keepMask);

private:
	int viewSize[2];
	int viewPosition[2];
	int mouseDownRelativePosition[2];
	int mouseRelativePosition[2];

	int _caughtElements;

	std::vector<int> viewSelectionBuffer;
	std::vector<int> viewSelectionBufferType;
	int viewSelectionSize[2];
	int tns[2];		// Thumnail size
	int objectType;	// Type of objects to display
	int viewIndex;
	int subViewIndex;
};
