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

#include "PageContainer.h"
#include "ViewSelector.h"
#include "PageSelector.h"
#include "SceneSelector.h"
#include "ButtonBlock.h"
#include "MainCont.h"

class COglSurface : public CMainCont 
{
public:
	COglSurface();
	virtual ~COglSurface();
	void initializeInitialValues(bool simulationIsRunning,int initializeOnlyForThisNewObject);
	void simulationAboutToStart();
	void simulationEnded();
	void renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib);
	void newSceneProcedure();
	void setSurfaceSizeAndPosition(int sizeX,int sizeY,int posX,int posY);
	bool leftMouseButtonDown(int x,int y,int selectionStatus);
	bool getMouseRelPosObjectAndViewSize(int x,int y,int relPos[2],int& objType,int& objID,int vSize[2],bool& viewIsPerspective);
	void leftMouseButtonUp(int x,int y);
	void mouseMove(int x,int y,bool passiveAndFocused);
	int getCursor(int x,int y);
	bool _browserResizingMousePosition(int x,int y);
	bool _hierarchyResizingMousePosition(int x,int y);
	void mouseWheel(int deltaZ,int x,int y);
	bool rightMouseButtonDown(int x,int y);
	void rightMouseButtonUp(int x,int y,int absX,int absY,QWidget* mainWindow);
	bool leftMouseButtonDoubleClick(int x,int y,int selectionStatus);
	void render(int currentCursor,int mouseButtonState,int mousePos[2]);
	void setViewSelectionActive(bool isActive);
	void setPageSelectionActive(bool isActive);
	void setSceneSelectionActive(bool isActive);
	void setHierarchySurfaceSizeAndPosition();
	void setBrowserSurfaceSizeAndPosition();
	void setViewSurfaceSizeAndPosition();
	void actualizeAllSurfacesSizeAndPosition();
	void setUpDefaultValues();
	void startViewSelection(int objectType,int subViewIndex);
	void keyPress(int key,QWidget* mainWindow);
	void setHierarchyEnabled(bool isEnabled);
	bool isHierarchyEnabled();
	int getHierarchyWidth();
	void setHierarchyWidth(int w);
	void setBrowserEnabled(bool isEnabled);
	bool isBrowserEnabled();
	bool isBrowserEffectivelyVisible();
	int getBrowserWidth();
	void setBrowserWidth(int w);
	void setFocusObject(int obj);
	int getFocusObject();
	int getCaughtElements();
	void clearCaughtElements(int keepMask);
	bool isViewSelectionActive();
	bool isPageSelectionActive();
	bool isSceneSelectionActive();
	void setModelThumbnailDraggingInfo(int thumbnailIndex,int offsetX,int offsetY);
	bool getModelThumbnailDraggingInfo(int& thumbnailIndex,C3Vector* desiredModelPosition);

	CPageContainer* pageCont;
	CViewSelector* viewSelector;
	CPageSelector* pageSelector;
	CSceneSelector* sceneSelector;

	static bool _hierarchyEnabled;
	static bool _browserEnabled;

private:

	bool sceneSelectionActive;
	bool viewSelectionActive;
	bool pageSelectionActive;
	bool _hierarchyClosingButtonDown;
	bool _hierarchyResizingAction;
	bool _browserClosingButtonDown;
	bool _browserResizingAction;

	int _caughtElements; // This refers to the hierarchy/browser closing button and the hierarchy/browser resizing bar only!

	static int _hierarchyWidth;
	static int _browserWidth;

	int surfaceSize[2];
	int surfacePosition[2];

	int mouseRelativePosition[2];
	int mousePreviousRelativePosition[2];

	int focusObject;

	int _modelThumbnailIndexForDragging;
	int _modelThumbnailDraggingOffset[2];
	int _modelThumbnailDraggingLastMousePos[2];
};
