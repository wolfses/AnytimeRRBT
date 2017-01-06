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

// This file requires some serious refactoring!

#pragma once

#include <vector>
#include "SView.h"
#include "vrepPrecompiledHeader.h"
#include "Ser.h"

class CSPage  
{
public:
	CSPage(int thePageType);
	virtual ~CSPage();
	void initializeInitialValues(bool simulationIsRunning,int initializeOnlyForThisNewObject);
	void simulationEnded();
	void serialize(CSer& ar);
	bool setPageType(int newType);
	void announceObjectWillBeErased(int objectID);
	void performObjectLoadingMapping(std::vector<int>* map);
	void render();
	void clearLastMouseDownViewIndex();
	int getLastMouseDownViewIndex();
	void getPageSizeAndPosition(int& sizeX,int& sizeY,int& posX,int& posY);
	void setPageSizeAndPosition(int sizeX,int sizeY,int posX,int posY);
	void drawSeparationLines();
	CSView* getView(int viewIndex);
	bool leftMouseButtonDown(int x,int y,int selectionStatus);
	bool getMouseRelPosObjectAndViewSize(int x,int y,int relPos[2],int& objType,int& objID,int vSize[2],bool& viewIsPerspective);
	void leftMouseButtonUp(int x,int y);
	int getCursor(int x,int y);
	void mouseMove(int x,int y,bool passiveAndFocused);
	void mouseWheel(int deltaZ,int x,int y);
	bool rightMouseButtonDown(int x,int y);
	bool rightMouseButtonUp(int x,int y,int absX,int absY,QWidget* mainWindow);
	bool leftMouseButtonDoubleClick(int x,int y,int selectionStatus);
	void getViewSizeAndPosition(int sViewSize[2],int sViewPos[2],int subViewIndex);
	void getFloatingViewRelativeSizeAndPosition(float sViewSize[2],float sViewPos[2],int subViewIndex);
	void setViewSizesAndPositions();
	void getViewRelativeMousePosition(int mouseX,int mouseY,int& relMouseX,int& relMouseY,int index);
	int getViewIndexOfMousePosition(int mouseX,int mouseY);
	bool doubleClickActionForView(int viewIndex);
	void clearAllMouseJustWentDownAndUpFlags();
	int getMousePosRelativeToFloatingViewBorders(int mouseX,int mouseY,int index);
	bool viewIsPassive(int viewIndex);
	int getRegularViewCount();
	int getViewCount();
	void addFloatingView();
	void addFloatingView(CSView* theFloatingView,float relSize[2],float relPos[2]);
	void swapViews(int index1,int index2,bool alsoSizeAndPosInfo);
	int bringViewToFrontIfPossible(int index);
	void getBorderCorrectedFloatingViewPosition(int posX,int posY,int sizeX,int sizeY,int& newPosX,int& newPosY);
	int getCaughtElements();
	void clearCaughtElements(int keepMask);
	bool removeFloatingView(int viewIndex);
	int getViewIndexFromViewUniqueID(int uniqueID);


private:
	// Variables which need to be serialized:
	int _pageType;
	int _lastMouseDownViewIndex;
	std::vector<CSView*> _allViews;
	// Positions and sizes are relative now (2009/05/22)
	std::vector<float> _allViewAuxSizesAndPos;

	std::vector<float> _initialAuxViewSizesAndPos;
	std::vector<int> _initialAuxViewUniqueIDs;
	bool _initialValuesInitialized;

	// Variables which don't need to be serialized:
	int _pagePosition[2];
	int _pageSize[2];
	int _caughtElements;
	int mouseRelativePosition[2];
	int previousMouseRelativePosition[2];

	int auxViewResizingAction;
	int viewIndexOfResizingAction;
	// Positions and sizes are relative now (2009/05/22)
	float floatingViewAddOffset;
};
