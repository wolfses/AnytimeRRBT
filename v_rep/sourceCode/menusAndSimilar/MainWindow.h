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

// This file requires serious refactoring!!

#pragma once

#include <QMainWindow>
#include <QToolBar>
#include <QComboBox>
#include <QToolButton>
#include <QSignalMapper>
#include "qvrep_openglwidget.h"
#include <QSplitter>
#include <QPlainTextEdit>
#include "Global.h"
#include "DlgCont.h"
#include "CustomMenuBarItemContainer.h"
#include "SimRecorder.h"
#include "VPoint.h"

enum {FOCUS_ON_PAGE=0,FOCUS_ON_HIERARCHY,FOCUS_ON_SOFT_DIALOG,FOCUS_ON_SCENE_SELECTION_WINDOW,FOCUS_ON_VIEW_SELECTION_WINDOW,FOCUS_ON_PAGE_SELECTION_WINDOW,FOCUS_ON_UNKNOWN_OBJECT,FOCUS_ON_BROWSER};

class CMainWindow : public QMainWindow
{
    Q_OBJECT
public:
	CMainWindow();
	virtual ~CMainWindow();
	void refreshDimensions();
	void setWindowDimensions(int x,int y,bool clientSurface,bool maximized);
	void createHierarchyWindow();

	bool event(QEvent* event);
	void dragEnterEvent(QDragEnterEvent* dEvent);
	void dropEvent(QDropEvent* dEvent);
    QMenu* createPopupMenu() { return NULL; } // required to avoid having a popup when over the toolbar (and other situations). 4/12/2011

//------------------------
	void uiThread_renderScene(bool bufferMainDisplayStateVariables);
	void simThread_prepareToRenderScene();
	void refreshDialogs_uiThread();
	int renderOpenGlContent_callFromRenderingThreadOnly();
//------------------------

	bool isFullScreen();
	void setFullScreen(bool f);
	void createDefaultMenuBar(bool reserved);
	void removeDefaultMenuBar();
	void setInitialDimensions(bool maximized);

	void createDefaultToolBars();
	int getMouseButtonState();

	void hideWindow();
	void showWindow();
	void hideWindowAndAllDialogs();
	void showWindowAndAllDialogs();

	void setHasStereo(bool s);
	bool getHasStereo();
	void setStereoDistance(float d);
	float getStereoDistance();
	void setLeftEye(bool l);
	bool getLeftEye();

	static bool ctrlKeyDown;
	static bool shiftKeyDown;
	static bool upKeyDown;
	static bool downKeyDown;
	static bool leftKeyDown;
	static bool rightKeyDown;
	static bool leftMouseDown;
	static bool middleMouseDown;
	static bool rightMouseDown;

	void handleCommonKeyPressEvents(int key);

	CDlgCont* dlgCont;
	CCustomMenuBarItemContainer* customMenuBarItemContainer;
	CSimRecorder* simulationRecorder;

	bool _fullscreen;
	bool _hasStereo;
	float _stereoDistance;
	bool _leftEye;

	int _savedMouseMode;

	void onLeftMouseButtonDownTT(int xPos,int yPos);
	void onRightMouseButtonDownTT(int xPos,int yPos);
	void onLeftMouseButtonDoubleClickTT(int xPos,int yPos);
	void onLeftMouseButtonUpTT(int xPos,int yPos);
	void onRightMouseButtonUpTT(int xPos,int yPos);
	void onMouseMoveTT(int xPos,int yPos);
	void actualizeButtonState();
	void getOglMousePos(int pos[2]);

	void _dropFilesIntoScene(const std::vector<std::string>& tttFiles,const std::vector<std::string>& ttmFiles,const std::vector<std::string>& ttbFiles);

	void recomputeMousePos(int xPos,int yPos);
	void recomputeClientSizeAndPos();

	VPoint mouseRenderingPos;	// Mouse position in the view-rendering zone
	VPoint mousePos;			// Mouse position in the window
	VPoint clientArea;			// Rendering window size
	VPoint clientPos;			// Rendering window position

	int focusObject;
	int _currentCursor;
	int _mouseButtonsState; // 1=left, 2=wheel activity, 4=right, 8=middle wheel down
	int _mouseWheelEventTime;
	int _oglMousePos[2];
	int _renderingTimeInMs;
	float _fps;

	QToolBar* _toolbar1;
	QToolBar* _toolbar2;

	QAction* _toolbarActionCameraShift;
	QAction* _toolbarActionCameraRotate;
	QAction* _toolbarActionCameraZoom;
	QAction* _toolbarActionCameraAngle;
	QAction* _toolbarActionCameraSizeToScreen;
	QAction* _toolbarActionCameraFly;
	QAction* _toolbarActionObjectShift;
	QAction* _toolbarActionObjectRotate;
	QAction* _toolbarActionClickSelection;
//	QAction* _toolbarActionPathPointCreation;
	QAction* _toolbarActionAssemble;
	QAction* _toolbarActionTransferDna;
	QAction* _toolbarActionUndo;
	QAction* _toolbarActionRedo;
	QAction* _toolbarActionDynamicContentVisualization;
	QComboBox* _engineSelectCombo;
	QComboBox* _enginePrecisionCombo;
	QComboBox* _timeStepConfigCombo;
	QAction* _toolbarActionStart;
	QAction* _toolbarActionPause;
	QAction* _toolbarActionStop;
	QAction* _toolbarActionRealTime;
	QAction* _toolbarActionReduceSpeed;
	QAction* _toolbarActionIncreaseSpeed;
	QAction* _toolbarActionThreadedRendering;

	QAction* _toolbarActionPageSelector;
	QAction* _toolbarActionSceneSelector;
	QAction* _toolbarActionSimulationSettings;
	QAction* _toolbarActionObjectProperties;
	QAction* _toolbarActionCalculationModules;
	QAction* _toolbarActionCollections;
	QAction* _toolbarActionScripts;
	QAction* _toolbarActionShapeEdition;
	QAction* _toolbarAction2dElements;
	QAction* _toolbarActionPathEdition;
	QAction* _toolbarActionSelection;
	QAction* _toolbarActionModelBrowser;
	QAction* _toolbarActionSceneHierarchy;
	QAction* _toolbarActionLayers;
	QAction* _toolbarActionAviRecorder;
	QAction* _toolbarActionUserSettings;

	VMenubar* _menubar;
	VMENU_HANDLE fileSystemMenu;
	VMENU_HANDLE editSystemMenu;
	VMENU_HANDLE addSystemMenu;
	VMENU_HANDLE simulationSystemMenu;
	VMENU_HANDLE windowSystemMenu;
	VMENU_HANDLE addOnSystemMenu;
	VMENU_HANDLE helpSystemMenu;
	VMENU_HANDLE instancesSystemMenu;
	VMENU_HANDLE secretSystemMenu;

	std::string secretText;

public:
	COpenglWidget* openglWidget;
	QSignalMapper* signalMapper;
	QSignalMapper* popupSignalMapper;
	QSplitter* splitter;
	QPlainTextEdit* statusBar;

public slots:
	void _engineSelectedViaToolbar(int index);
	void _enginePrecisionViaToolbar(int index);
	void _timeStepConfigViaToolbar(int index);

	void _vrepPopupMessageHandler(int id);
	void _vrepMessageHandler(int id);

	void _aboutToShowFileSystemMenu();
	void _aboutToShowEditSystemMenu();
	void _aboutToShowAddSystemMenu();
	void _aboutToShowSimulationSystemMenu();
	void _aboutToShowWindowSystemMenu();
	void _aboutToShowAddOnSystemMenu();
	void _aboutToShowHelpSystemMenu();
	void _aboutToShowInstancesSystemMenu();
	void _aboutToShowCustomMenu();

	void splitterMoved(int pos,int index);
};
