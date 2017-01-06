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

#include "vrepPrecompiledHeader.h"
#include "SPage.h"
#include "Global.h"
#include "ButtonBlock.h"

class CPageContainer  
{
public:
	CPageContainer();
	virtual ~CPageContainer();
	void initializeInitialValues(bool simulationIsRunning,int initializeOnlyForThisNewObject);
	void simulationEnded();
	void newSceneProcedure();
	void removeAllPages();
	void setUpDefaultPages(bool createASingleView=false);
	void setActivePage(int pageIndex);
	int getActivePageIndex();
	void announceObjectWillBeErased(int objectID);
	void serialize(CSer& ar);
	void performObjectLoadingMapping(std::vector<int>* map);
	CSPage* getPage(int pageIndex);
	void renderCurrentPage();
	void setPageSizeAndPosition(int sizeX,int sizeY,int posX,int posY);
	void removePage(int pageIndex);
	int getPageCount();

	void clearAllLastMouseDownViewIndex();

	bool leftMouseButtonDown(int x,int y,int selectionStatus);
	bool getMouseRelPosObjectAndViewSize(int x,int y,int relPos[2],int& objType,int& objID,int vSize[2],bool& viewIsPerspective);
	void leftMouseButtonUp(int x,int y);
	void mouseMove(int x,int y,bool passiveAndFocused);
	int getCursor(int x,int y);
	void mouseWheel(int deltaZ,int x,int y);
	bool rightMouseButtonDown(int x,int y);
	void rightMouseButtonUp(int x,int y,int absX,int absY,QWidget* mainWindow);
	bool leftMouseButtonDoubleClick(int x,int y,int selectionStatus);
	void addEmptyPageMenu(VMenu* menu);
	bool evaluateEmptyPageMenuSelection(int commandID,int viewIndex);
	void looseFocus();
	int getFocusObject();
	void setFocusObject(int obj);
	void keyPress(int key,QWidget* mainWindow);
	int getCaughtElements();
	void clearCaughtElements(int keepMask);

private:
	CSPage* _allPages[MAX_NUMBER_OF_PAGES];
	int _activePageIndex;

	int _initialValuesInitialized;
	int _initialActivePageIndex;
	int _numberOfPages;
	int _pageSize[2];
	int _pagePosition[2];

	int mouseRelativePosition[2];

	int _caughtElements;


	int rightMouseCaughtBy;
	int leftMouseCaughtBy;
	bool rightMouseCaughtSoftDialog;
	bool leftMouseCaughtSoftDialog;

	int prepareForPopupMenu;
	int focusObject;
};
