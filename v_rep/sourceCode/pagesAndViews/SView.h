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

#include "vrepPrecompiledHeader.h"
#include <vector>
#include "VMenubar.h"
#include "Ser.h"

class CSView  
{
public:
	CSView(int theLinkedObjectID);
	virtual ~CSView();
	void initializeInitialValues(bool simulationIsRunning);
	bool simulationEnded();
	void setDefaultValues();
	int getLinkedObjectID();
	void setLinkedObjectID(int theNewLinkedObjectID,bool noDefaultValues);
	void setAlternativeViewName(const char* name);
	std::string getAlternativeViewName();
	void setPerspectiveDisplay(bool perspective);
	bool getPerspectiveDisplay();

	void setShowEdges(bool edges);
	bool getShowEdges();
	void setThickEdges(bool thickEdges);
	bool getThickEdges();

	void setRenderingMode(int mode);
	int getRenderingMode();
	void setRemoveFloatingViewAtSimulationEnd(bool remove);
	bool getRemoveFloatingViewAtSimulationEnd();
	void setXYGraphAutoModeDuringSimulation(bool autoMode);
	bool getXYGraphAutoModeDuringSimulation();
	void setTimeGraphXAutoModeDuringSimulation(bool autoMode);
	bool getTimeGraphXAutoModeDuringSimulation();
	void setTimeGraphYAutoModeDuringSimulation(bool autoMode);
	bool getTimeGraphYAutoModeDuringSimulation();
	void setXYGraphIsOneOneProportional(bool autoMode);
	bool getXYGraphIsOneOneProportional();
	void setTimeGraph(bool timeGraph);
	bool getTimeGraph();
	void setCanSwapViewWithMainView(bool canDoIt);
	bool getCanSwapViewWithMainView();

	void setCanBeClosed(bool canDoIt);
	bool getCanBeClosed();
	void setCanBeShifted(bool canDoIt);
	bool getCanBeShifted();
	void setCanBeResized(bool canDoIt);
	bool getCanBeResized();

	void setFitViewToScene(bool doIt);
	bool getFitViewToScene();
	void setFitViewToSelection(bool doIt);
	bool getFitViewToSelection();

	void setGraphPosition(float x,float y);
	void getGraphPosition(float position[2]);
	void setGraphSize(float x,float y);
	void getGraphSize(float size[2]);
	void serialize(CSer& ar);
	bool announceObjectWillBeErased(int objectID);
	void performObjectLoadingMapping(std::vector<int>* map);
	void setViewSizeAndPosition(int sizeX,int sizeY,int posX,int posY);
	void getViewSize(int size[2]);
	void getViewPosition(int pos[2]);
	void getMouseDownRelativePosition(int p[2]);
	void getMouseRelativePosition(int p[2]);
	void getPreviousMouseRelativePosition(int p[2]);
	bool isMouseDown();
	bool didMouseJustGoDown();
	bool wasMouseJustWentDownEventProcessed();
	bool didMouseJustGoUp();
	bool didMouseMoveWhileDown();
	int getSelectionStatus();
	void addMenu(VMenu* menu);
	bool evaluateMenuSelection(int commandID,int subViewIndex);
	
	
	bool leftMouseButtonDown(int x,int y,int selStatus);
	bool getMouseRelPosObjectAndViewSize(int x,int y,int relPos[2],int& objType,int& objID,int vSize[2],bool& viewIsPerspective);
	void leftMouseButtonUp(int x,int y);
	void mouseMove(int x,int y,bool passiveAndFocused);
	bool mouseWheel(int deltaZ,int x,int y);
	bool rightMouseButtonDown(int x,int y,bool subViewIsPassive);
	bool rightMouseButtonUp(int x,int y,int absX,int absY,QWidget* mainWindow,int subViewIndex);
	bool leftMouseButtonDoubleClick(int x,int y,int selStatus);
	void setCenterPosition(float pos[3]);
	void getCenterPosition(float pos[3]);
	void setMousePositionDepth(float depth);
	float getMousePositionDepth();

	void setViewIndex(int ind);
	int getViewIndex();

	void clearMouseJustWentDownAndUpFlag();
	void render(int mainWindowXPos,bool clipWithMainWindowXPos,bool drawText,bool passiveSubView);
	void handleCameraOrGraphMotion();

	int getCaughtElements();
	void clearCaughtElements(int keepMask);

	void setTrackedGraphCurveIndex(int index);
	int getTrackedGraphCurveIndex();

	int getUniqueID();


	CSView* copyYourself(); // special! Only used when copy and pasting viewable objects in the copy buffer that are associated with a floating view!
	// Following two lines are only used when copy and pasting viewable objects are associated with a floating view!
	float _relativeViewPosition[2];
	float _relativeViewPositionOffset;
	float _relativeViewSize[2];


private:

	void cameraAndObjectMotion();
	void graphMotion();
	void _handleClickRayIntersection(int x,int y,bool mouseDown);

	// Variables which need to be serialized:
	int linkedObjectID;
	bool perspectiveDisplay;
	bool _showEdges;
	bool _thickEdges;
	bool _removeFloatingViewAtSimulationEnd;
	int _renderingMode;
	bool _xyGraphInAutoModeDuringSimulation;
	bool _timeGraphXInAutoModeDuringSimulation;
	bool _timeGraphYInAutoModeDuringSimulation;
	bool _xyGraphIsOneOneProportional;
	bool graphIsTimeGraph;
	bool _canSwapViewWithMainView;
	bool _canBeClosed;
	bool _canBeShifted;
	bool _canBeResized;
	float graphPosition[2];
	float graphSize[2];
	bool _fitSceneToView;
	bool _fitSelectionToView;
	
	// Variables which don't need to be serialized:
	int _viewPosition[2];
	int _viewSize[2];
	std::string _alternativeViewName;

	int _caughtElements;

	int mouseDownRelativePosition[2];
	int _rightMouseButtonDownCoord[2];

	int rightMouseDownRelativePosition[2];
	bool dontActivatePopup;
	int mouseRelativePosition[2];
	int mousePreviousRelativePosition[2];
	int selectionStatus;
	bool mouseIsDown;
	bool rightButtonIsCaught;
	bool mouseJustWentDownFlag;
	int mouseDownTimings;
	bool mouseJustWentDownWasProcessed;
	bool mouseJustWentUpFlag;
	bool _mouseMovedWhileDownFlag;

	float centerPosition[3];
	float mousePositionDepth;

	bool subviewIsPassive;
	int _trackedCurveIndex;
	int _uniqueID;

	bool _initialValuesInitialized;
	bool _initialPerspectiveDisplay;
	bool _initialShowEdges;
	bool _initialThickEdges;
	int _initialRenderingMode;
	bool _initialGraphIsTimeGraph;

	int _viewIndex; // set before each rendering
};
