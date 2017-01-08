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

#include "RegisteredGroups.h"
#include "RegisteredDistances.h"
#include "RegisteredCollisions.h"
#include "RegisteredPathPlanningTasks.h"
#include "RegisteredMotionPlanningTasks.h"
#include "CopyBuffer.h"
#include "ScintillaEditorContainer.h"
#include "Environment.h"
#include "OglSurface.h"
#include "MainSettings.h"
#include "MemorizedConfContainer.h"
#include "Simulation.h"
#include "LifeControl.h"
#include "ButtonBlockContainer.h"
#include "OutsideCommandQueue.h"
#include "LuaScriptContainer.h"
#include "CustomData.h"
#include "CacheCont.h"
#include "InfoDisplay.h"
#include "ConstraintSolverContainer.h"
#include "apiErrors.h"
#include "GenericDialogContainer.h"
#include "TextureContainer.h"
#include "DynMaterialContainer.h"
#include "GlobalTextureContainer.h"
#include "DrawingContainer.h"
#include "pointCloudContainer.h"
#include "GhostObjectContainer.h"
#include "BannerContainer.h"
#include "undoBufferCont.h"
#include "DynamicsContainer.h"
#include "SignalContainer.h"
#include "DirBrowserElement.h"
#include "SerialPortContainer.h"
#include "CommTubeContainer.h"
#include "ScintillaConsoleContainer.h"
//#include "persistentDataContainer.h"
#include "simulatorMessageQueue.h"
#include "Hierarchy.h"
#include "Browser.h"
#include "Registerediks.h"
#include "ObjCont.h"

class CMainContainer
{
public:
	CMainContainer();
	virtual ~CMainContainer();

	void initialize(bool enableMultipleInst);
	void deinitialize();

	void addMenu(VMenu* menu);
	bool evaluateMenuSelection(int commandID);
	void removeAllObjectsFromAllScenes();

	void visibleInstanceAboutToChange_infoForDialogsOnly();
	void visibleInstanceJustChanged_infoForDialogsOnly();
	bool closeAllNonToolDialogs();
	void renderingPassAllNonToolDialogs();
	void setAllHierarchyRefreshViewFlags();

	int getInstanceIndexOfAnEditMode();
	int getInstanceIndexOfASceneNotYetSaved();
	int getMouseMode();
	void setMouseMode(int mm);
	void setDefaultMouseMode();

	void simulationAboutToStart();
	void simulationEnded();
	void renderYourGeneralObject3DStuff_beforeRegularObjects(CViewableBase* renderingObject,int displayAttrib,int windowSize[2],float verticalViewSizeOrAngle,bool perspective);
	void renderYourGeneralObject3DStuff_afterRegularObjects(CViewableBase* renderingObject,int displayAttrib,int windowSize[2],float verticalViewSizeOrAngle,bool perspective);
	void renderYourGeneralObject3DStuff_onTopOfRegularObjects(CViewableBase* renderingObject,int displayAttrib,int windowSize[2],float verticalViewSizeOrAngle,bool perspective);

	void setModificationFlag(int bitMask);
	int getModificationFlags(bool clearTheFlagsAfter);

	void closeDialogTemporarilyIfOpened(int dlgID,std::vector<int>& vect);

	void closeTemporarilyNonEditModeDialogs();
	void reopenTemporarilyClosedNonEditModeDialogs();
	void closeTemporarilyDialogsForViewSelector();
	void reopenTemporarilyClosedDialogsForViewSelector();
	void closeTemporarilyDialogsForPageSelector();
	void reopenTemporarilyClosedDialogsForPageSelector();
	void closeTemporarilyDialogsForSceneSelector();
	void reopenTemporarilyClosedDialogsForSceneSelector();

	void closeAllScintillaEditorsInAllScenes();

private:

	std::vector<int> _dialogsClosedTemporarily_editModes;
	std::vector<int> _dialogsClosedTemporarily_viewSelector;
	std::vector<int> _dialogsClosedTemporarily_pageSelector;
	std::vector<int> _dialogsClosedTemporarily_sceneSelector;


	std::vector<int> _uniqueIdsOfSelectionSinceLastTimeGetAndClearModificationFlagsWasCalled;
	int _modificationFlags;
	// +bit 0: object(s) erased
	// +bit 1: object(s) created
	// +bit 2: model loaded
	// +bit 3: scene loaded
	// +bit 4: undo called
	// +bit 5: redo called
	// +bit 6: instance switched
	// +bit 7: edit mode active			--> is set when getAndClearModificationFlags is called
	// +bit 8: object(s) scaled
	// +bit 9: selection state changed	--> is set when getAndClearModificationFlags is called
	// +bit 10: key pressed
	// +bit 11: simulation started
	// +bit 12: simulation ended

	int _mouseMode;

public:
	CCopyBuffer* copyBuffer; // We have only one copy buffer!!
	CGlobalTextureContainer* globalTextureCont; // We have only one such object!!
	CScintillaConsoleContainer* scintillaConsoleContainer; // We have only one such object!!
	CSerialPortContainer* serialPortContainer; // We have only one such object!!
//	CPersistentDataContainer* persistentDataContainer; // We have only one such object!!
	CSimulatorMessageQueue* simulatorMessageQueue; // We have only one such object!!

	CHierarchy* hierarchy; // We have only one such object!!
	CBrowser* browser; // We have only one such object!!
	CDirBrowserElement* modelDirectoryRoot; // The root only here


	CScintillaEditorContainer* scintillaEditorContainer;
	CCommTubeContainer* commTubeContainer;
	CSignalContainer* signalContainer;
	CDynamicsContainer* dynamicsContainer;
	CUndoBufferCont* undoBufferContainer;
	CGenericDialogContainer* genericDialogContainer;
	COutsideCommandQueue* outsideCommandQueue;
	CButtonBlockContainer* buttonBlockContainer;
	CRegisteredGroups* collections;
	CRegisteredDistances* distances;
	CRegisteredCollisions* collisions;
	CEnvironment* environment;
	COglSurface* oglSurface;
	CMainSettings* mainSettings;
	CRegisteredPathPlanningTasks* pathPlanning;
	CRegisteredMotionPlanningTasks* motionPlanning;
	CLuaScriptContainer* luaScriptContainer;
	CTextureContainer* textureCont;
	CDynMaterialContainer* dynMaterialCont;
	CMemorizedConfContainer* confContainer;
	CSimulation* simulation;
	CCustomData* customSceneData;
	CCacheCont* cacheData;
	CInfoDisplay* infoDisplay;
	CConstraintSolverContainer* constraintSolver;
	CApiErrors* apiErrors;
	CDrawingContainer* drawingCont;
	CPointCloudContainer* pointCloudCont;
	CGhostObjectContainer* ghostObjectCont;
	CBannerContainer* bannerCont;

	std::vector<CScintillaEditorContainer*> scintillaEditorContainerList;
	std::vector<CCommTubeContainer*> commTubeContainerList;
	std::vector<CSignalContainer*> signalContainerList;
	std::vector<CDynamicsContainer*> dynamicsContainerList;
	std::vector<CUndoBufferCont*> undoBufferContainerList;
	std::vector<CGenericDialogContainer*> genericDialogContainerList;
	std::vector<COutsideCommandQueue*> outsideCommandQueueList;
	std::vector<CButtonBlockContainer*> buttonBlockContainerList;
	std::vector<CRegisteredGroups*> collectionList;
	std::vector<CRegisteredDistances*> distanceList;
	std::vector<CRegisteredCollisions*> collisionList;
	std::vector<CEnvironment*> environmentList;
	std::vector<COglSurface*> oglSurfaceList;
	std::vector<CMainSettings*> mainSettingsList;
	std::vector<CRegisteredPathPlanningTasks*> pathPlanningList;
	std::vector<CRegisteredMotionPlanningTasks*> motionPlanningList;
	std::vector<CLuaScriptContainer*> luaScriptContainerList;
	std::vector<CTextureContainer*> textureContList;
	std::vector<CDynMaterialContainer*> dynMaterialContList;
	std::vector<CMemorizedConfContainer*> confContainerList;
	std::vector<CSimulation*> simulationList;
	std::vector<CCustomData*> customSceneDataList;
	std::vector<CCacheCont*> cacheDataList;
	std::vector<CInfoDisplay*> infoDisplayList;
	std::vector<CConstraintSolverContainer*> constraintSolverList;
	std::vector<CApiErrors*> apiErrorsList;
	std::vector<CDrawingContainer*> drawingContainerList;
	std::vector<CPointCloudContainer*> pointCloudContainerList;
	std::vector<CGhostObjectContainer*> ghostObjectContainerList;
	std::vector<CBannerContainer*> bannerContainerList;

public:
	void emptyScene(bool notCalledFromUndoFunction);
	int getInstanceNumber();

	int createNewInstance(bool triggerCallbackAndHandleDialogs);
	bool makeInstanceCurrentFromIndex(int instanceIndex,bool triggerCallbackAndHandleDialogs);
	int destroyCurrentInstance();
	int getCurrentInstanceIndex();

	bool isInstanceSwitchingLocked();

private: 	
	int currentInstanceIndex;
	bool globalInstanceCreationOrSwitchingLocked;

public:
	CRegisterediks* ikGroups;
	CObjCont* objCont;

	std::vector<CRegisterediks*> ikGroupList;
	std::vector<CObjCont*> objContList;

};
