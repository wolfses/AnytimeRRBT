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
#include "IloIlo.h"
#include "OGL.h"
#include "Tt.h"
#include "App.h"
#include "pluginContainer.h"
#include "VVarious.h"
#include "sensingThreadPool.h"
#include "collDistInterface.h"
#include "dynInterface.h"
#include "pathPlanningInterface.h"

const int DEFAULT_MOUSE_MODE=sim_navigation_camerashift|sim_navigation_clickselection|sim_navigation_ctrlselection|sim_navigation_shiftselection|sim_navigation_camerazoomwheel|sim_navigation_camerarotaterightbutton;

CMainContainer::CMainContainer()
{
	FUNCTION_DEBUG;
	_mouseMode=DEFAULT_MOUSE_MODE;

	copyBuffer=NULL;
	globalTextureCont=NULL;
	modelDirectoryRoot=NULL;
	serialPortContainer=NULL;
//	persistentDataContainer=NULL;
	simulatorMessageQueue=NULL;

	hierarchy=NULL;
	browser=NULL;

	scintillaEditorContainer=NULL;
	scintillaConsoleContainer=NULL;
	commTubeContainer=NULL;
	signalContainer=NULL;
	dynamicsContainer=NULL;
	undoBufferContainer=NULL;
	genericDialogContainer=NULL;
	outsideCommandQueue=NULL;
	buttonBlockContainer=NULL;
	collections=NULL;
	distances=NULL;
	collisions=NULL;
	environment=NULL;
	oglSurface=NULL;
	mainSettings=NULL;
	pathPlanning=NULL;
	motionPlanning=NULL;
	luaScriptContainer=NULL;
	textureCont=NULL;
	dynMaterialCont=NULL;
	confContainer=NULL;
	simulation=NULL;
	customSceneData=NULL;
	cacheData=NULL;
	infoDisplay=NULL;
	constraintSolver=NULL;
	apiErrors=NULL;
	drawingCont=NULL;
	pointCloudCont=NULL;
	ghostObjectCont=NULL;
	bannerCont=NULL;

	globalInstanceCreationOrSwitchingLocked=false;
	currentInstanceIndex=-1;

	ikGroups=NULL;
	objCont=NULL;
}

CMainContainer::~CMainContainer()
{
}


int CMainContainer::getMouseMode()
{
	return(_mouseMode);
}

void CMainContainer::setMouseMode(int mm)
{ // can be called by any thread
	_mouseMode=mm;
	if (objCont!=NULL)
	{
		if ((App::mainWindow!=NULL)&&(oglSurface!=NULL))
		{
			int bla=mm&0x00f;
			if (!oglSurface->isViewSelectionActive())
			{
				if ((bla==sim_navigation_objectshift)||(bla==sim_navigation_objectrotate))
				{
					App::uiThread->openOrBringDlgToFront(TRANSLATION_ROTATION_DLG);
					App::uiThread->activateMainWindow();
				}
				else
					App::uiThread->closeDlg(TRANSLATION_ROTATION_DLG);
			}
			else
			{
				if ((bla==sim_navigation_objectshift)||(bla==sim_navigation_objectrotate))
					_dialogsClosedTemporarily_viewSelector.push_back(TRANSLATION_ROTATION_DLG); // very special case here!!!
			}
		}
		objCont->toolbarRefreshFlag=true;
		objCont->setLightDialogRefreshFlag();
	}
}

void CMainContainer::removeAllObjectsFromAllScenes()
{
	copyBuffer->clearBuffer();
	for (int i=0;i<int(objContList.size());i++)
	{
		makeInstanceCurrentFromIndex(i,true);
		objCont->removeAllObjects();
	}
}

void CMainContainer::setDefaultMouseMode()
{
	setMouseMode(DEFAULT_MOUSE_MODE);
}

void CMainContainer::addMenu(VMenu* menu)
{ 
	for (int i=0;i<int(collisionList.size());i++)
	{
		std::string txt=VVarious::splitPath_fileBase(mainSettingsList[i]->scenePathAndName);
		if (txt=="")
			txt="new scene";
		txt+=tt::decorateString(" (scene ",tt::FNb(i+1),")");

		VMenubar::appendMenuItem(menu,!isInstanceSwitchingLocked(),currentInstanceIndex==i,SWITCH_TO_INSTANCE_INDEX0_CMD+i,txt,true);
	}
}

bool CMainContainer::evaluateMenuSelection(int commandID)
{ // Return value is true if the command belonged to object edition menu and was executed
	if ( (commandID>=SWITCH_TO_INSTANCE_INDEX0_CMD)&&(commandID<=SWITCH_TO_INSTANCE_INDEX39_CMD) )
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("SWITCH_TO_INSTANCE...")
		{
			if (getCurrentInstanceIndex()==commandID-SWITCH_TO_INSTANCE_INDEX0_CMD)
				return(true); // we already have this instance!
			if (!isInstanceSwitchingLocked())
			{
				if (App::mainWindow!=NULL)
					App::mainWindow->openglWidget->setOpenGlDisplayEnabled(true);
				makeInstanceCurrentFromIndex(commandID-SWITCH_TO_INSTANCE_INDEX0_CMD,true);
			}
		}
		return(true);
	}
	return(false);
}

void CMainContainer::simulationAboutToStart()
{

	if (!CCollDistInterface::initializeFunctionsIfNeeded()) // the functions should already have been initialized earlier!!!
	{
		float cols[6]={0.8f,0.0f,0.0f,1.0f,1.0f,1.0f};
		float cols2[6]={0.5f,0.0f,0.0f,0.8f,0.8f,0.8f};
		CGenericDialog* it=new CGenericDialog("ERROR","The 'MeshCalc' plugin could not be initialized. Collision detection, distance calculation,&&nproximity sensor simulation and cutting simulation will not work.",sim_dlgstyle_ok,false,NULL,cols,cols2);
		it->setPauseActive(true);
		genericDialogContainer->addDialog(it);
	}
	if (!CDynInterface::initializeFunctionsIfNeeded()) // the functions should already have been initialized earlier!!!
	{
		float cols[6]={0.9f,0.1f,0.0f,1.0f,1.0f,1.0f};
		float cols2[6]={0.6f,0.08f,0.0f,0.8f,0.8f,0.8f};
		CGenericDialog* it=new CGenericDialog("ERROR","The 'Dynamics' plugin could not be initialized. Dynamics simulation will not work.",sim_dlgstyle_ok,false,NULL,cols,cols2);
		it->setPauseActive(true);
		genericDialogContainer->addDialog(it);
	}
	if (!CPathPlanningInterface::initializeFunctionsIfNeeded()) // the functions should already have been initialized earlier!!!
	{
		float cols[6]={0.98f,0.18f,0.0f,1.0f,1.0f,1.0f};
		float cols2[6]={0.69f,0.15f,0.0f,0.8f,0.8f,0.8f};
		CGenericDialog* it=new CGenericDialog("ERROR","The 'PathPlanning' plugin could not be initialized. Path planning tasks will not work.",sim_dlgstyle_ok,false,NULL,cols,cols2);
		it->setPauseActive(true);
		genericDialogContainer->addDialog(it);
	}

	drawingCont->simulationAboutToStart();
	pointCloudCont->simulationAboutToStart();
	ghostObjectCont->simulationAboutToStart();
	bannerCont->simulationAboutToStart();
	apiErrors->simulationAboutToStart();
	buttonBlockContainer->simulationAboutToStart();
	constraintSolver->simulationAboutToStart();
	customSceneData->simulationAboutToStart();
	dynamicsContainer->simulationAboutToStart();
	signalContainer->simulationAboutToStart();
	environment->simulationAboutToStart();
	genericDialogContainer->simulationAboutToStart();
	infoDisplay->simulationAboutToStart();
	luaScriptContainer->simulationAboutToStart();
	mainSettings->simulationAboutToStart();
	confContainer->simulationAboutToStart();
	objCont->simulationAboutToStart();
	oglSurface->simulationAboutToStart();
	collisions->simulationAboutToStart();
	distances->simulationAboutToStart();
	collections->simulationAboutToStart();
	ikGroups->simulationAboutToStart();
	pathPlanning->simulationAboutToStart();
	motionPlanning->simulationAboutToStart();
	simulation->simulationAboutToStart();
	textureCont->simulationAboutToStart();
	dynMaterialCont->simulationAboutToStart();
	undoBufferContainer->simulationAboutToStart();
	commTubeContainer->simulationAboutToStart();

	void* retVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_simulationabouttostart,NULL,NULL,NULL);
	delete[] (char*)retVal;

	setModificationFlag(2048); // simulation started

	// reset those:
	CIloIlo::proxSensorClickSelectDown=0;
	CIloIlo::proxSensorClickSelectUp=0;
}

void CMainContainer::simulationEnded()
{
	FUNCTION_DEBUG;
	CSensingThreadPool::killThreads(); // just in case we forgot to turn it off in the main script!

	void* retVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_simulationended,NULL,NULL,NULL);
	delete[] (char*)retVal;

	setModificationFlag(4096); // simulation ended
	drawingCont->simulationEnded();
	pointCloudCont->simulationEnded();
	ghostObjectCont->simulationEnded();
	bannerCont->simulationEnded();
	apiErrors->simulationEnded();
	buttonBlockContainer->simulationEnded();
	constraintSolver->simulationEnded();
	customSceneData->simulationEnded();
	dynamicsContainer->simulationEnded();
	signalContainer->simulationEnded();
	environment->simulationEnded();
	genericDialogContainer->simulationEnded();
	infoDisplay->simulationEnded();
	luaScriptContainer->simulationEnded();
	mainSettings->simulationEnded();
	confContainer->simulationEnded();
	objCont->simulationEnded();
	oglSurface->simulationEnded();
	collisions->simulationEnded();
	distances->simulationEnded();
	collections->simulationEnded();
	ikGroups->simulationEnded();
	pathPlanning->simulationEnded();
	motionPlanning->simulationEnded();
	simulation->simulationEnded();
	textureCont->simulationEnded();
	dynMaterialCont->simulationEnded();
	undoBufferContainer->simulationEnded();
	commTubeContainer->simulationEnded();
	serialPortContainer->simulationEnded();

//	App::uiThread->signalSimEnd();
	App::uiThread->simulationEnded(); // will indirectly invoke the GUI thread, and wait until done!

	// reset those:
	CIloIlo::proxSensorClickSelectDown=0;
	CIloIlo::proxSensorClickSelectUp=0;
}

void CMainContainer::renderYourGeneralObject3DStuff_beforeRegularObjects(CViewableBase* renderingObject,int displayAttrib,int windowSize[2],float verticalViewSizeOrAngle,bool perspective)
{ // Render here things that are not transparent and not overlay
	distances->renderYour3DStuff(renderingObject,displayAttrib);
	drawingCont->renderYour3DStuff_nonTransparent(renderingObject,displayAttrib);
	pointCloudCont->renderYour3DStuff_nonTransparent(renderingObject,displayAttrib);
	ghostObjectCont->renderYour3DStuff_nonTransparent(renderingObject,displayAttrib);
	bannerCont->renderYour3DStuff_nonTransparent(renderingObject,displayAttrib,windowSize,verticalViewSizeOrAngle,perspective);

	dynamicsContainer->renderYour3DStuff(renderingObject,displayAttrib);
	apiErrors->renderYour3DStuff(renderingObject,displayAttrib);
	buttonBlockContainer->renderYour3DStuff(renderingObject,displayAttrib);
	constraintSolver->renderYour3DStuff(renderingObject,displayAttrib);
	customSceneData->renderYour3DStuff(renderingObject,displayAttrib);
	signalContainer->renderYour3DStuff(renderingObject,displayAttrib);
	environment->renderYour3DStuff(renderingObject,displayAttrib);
	genericDialogContainer->renderYour3DStuff(renderingObject,displayAttrib);
	infoDisplay->renderYour3DStuff(renderingObject,displayAttrib);
	luaScriptContainer->renderYour3DStuff(renderingObject,displayAttrib);
	mainSettings->renderYour3DStuff(renderingObject,displayAttrib);
	confContainer->renderYour3DStuff(renderingObject,displayAttrib);
	objCont->renderYour3DStuff(renderingObject,displayAttrib);
	oglSurface->renderYour3DStuff(renderingObject,displayAttrib);
	collections->renderYour3DStuff(renderingObject,displayAttrib);
	ikGroups->renderYour3DStuff(renderingObject,displayAttrib);
	pathPlanning->renderYour3DStuff(renderingObject,displayAttrib);
	motionPlanning->renderYour3DStuff(renderingObject,displayAttrib);
	simulation->renderYour3DStuff(renderingObject,displayAttrib);
	textureCont->renderYour3DStuff(renderingObject,displayAttrib);
	dynMaterialCont->renderYour3DStuff(renderingObject,displayAttrib);
	undoBufferContainer->renderYour3DStuff(renderingObject,displayAttrib);
	commTubeContainer->renderYour3DStuff(renderingObject,displayAttrib);
}

void CMainContainer::renderYourGeneralObject3DStuff_afterRegularObjects(CViewableBase* renderingObject,int displayAttrib,int windowSize[2],float verticalViewSizeOrAngle,bool perspective)
{ // Render here things supposed to be transparent
	drawingCont->renderYour3DStuff_transparent(renderingObject,displayAttrib);
	pointCloudCont->renderYour3DStuff_transparent(renderingObject,displayAttrib);
	ghostObjectCont->renderYour3DStuff_transparent(renderingObject,displayAttrib);
	bannerCont->renderYour3DStuff_transparent(renderingObject,displayAttrib,windowSize,verticalViewSizeOrAngle,perspective);
}

void CMainContainer::renderYourGeneralObject3DStuff_onTopOfRegularObjects(CViewableBase* renderingObject,int displayAttrib,int windowSize[2],float verticalViewSizeOrAngle,bool perspective)
{ // Render here things supposed to be overlay
	drawingCont->renderYour3DStuff_overlay(renderingObject,displayAttrib);
	pointCloudCont->renderYour3DStuff_overlay(renderingObject,displayAttrib);
	ghostObjectCont->renderYour3DStuff_overlay(renderingObject,displayAttrib);
	bannerCont->renderYour3DStuff_overlay(renderingObject,displayAttrib,windowSize,verticalViewSizeOrAngle,perspective);
	collisions->renderYour3DStuff(renderingObject,displayAttrib); 
	dynamicsContainer->renderYour3DStuff_overlay(renderingObject,displayAttrib);
}

void CMainContainer::closeAllScintillaEditorsInAllScenes()
{
	for (int i=0;i<int(scintillaEditorContainerList.size());i++)
		scintillaEditorContainerList[i]->closeAllEditors();
}

void CMainContainer::visibleInstanceAboutToChange_infoForDialogsOnly()
{
	if (scintillaEditorContainer!=NULL)
		scintillaEditorContainer->showOrHideAllEditors(false);
	if ((objCont!=NULL)&&(App::mainWindow!=NULL))
	{
		if (objCont->getEditModeType()==BUTTON_EDIT_MODE)
			App::mainWindow->dlgCont->close(CUSTOM_UI_DLG); // has only an effect with the QT version
		if (objCont->getEditModeType()==PATH_EDIT_MODE)
			App::mainWindow->dlgCont->close(PATH_EDITION_DLG); // has only an effect with the QT version
		if (objCont->getEditModeType()&SHAPE_EDIT_MODE)
			App::mainWindow->dlgCont->close(SHAPE_EDITION_DLG); // has only an effect with the QT version
		if (objCont->getEditModeType()!=NO_EDIT_MODE)
			reopenTemporarilyClosedNonEditModeDialogs();
	}
}

void CMainContainer::visibleInstanceJustChanged_infoForDialogsOnly()
{
	if (scintillaEditorContainer!=NULL)
		scintillaEditorContainer->showOrHideAllEditors(true);
	if ((objCont!=NULL)&&(App::mainWindow!=NULL))
	{
		if (objCont->getEditModeType()==BUTTON_EDIT_MODE)
			App::mainWindow->dlgCont->openOrBringToFront(CUSTOM_UI_DLG); // has only an effect with the QT version
		if (objCont->getEditModeType()==PATH_EDIT_MODE)
			App::mainWindow->dlgCont->openOrBringToFront(PATH_EDITION_DLG); // has only an effect with the QT version
		if (objCont->getEditModeType()&SHAPE_EDIT_MODE)
			App::mainWindow->dlgCont->openOrBringToFront(SHAPE_EDITION_DLG); // has only an effect with the QT version
		if (objCont->getEditModeType()!=NO_EDIT_MODE)
			closeTemporarilyNonEditModeDialogs();
	}
}

bool CMainContainer::closeAllNonToolDialogs()
{ // We close only dialogs of the current instance!
	scintillaEditorContainer->closeAllEditors();
	return(true);
}

void CMainContainer::renderingPassAllNonToolDialogs()
{
	scintillaEditorContainer->updateWindowsExceptContentText();
}

void CMainContainer::setAllHierarchyRefreshViewFlags()
{
	hierarchy->setRefreshViewFlag();
}

int CMainContainer::getInstanceIndexOfAnEditMode()
{
	for (int i=0;i<getInstanceNumber();i++)
	{
		if (objContList[i]->getEditModeType()!=NO_EDIT_MODE)
			return(i);
	}
	return(-1);
}

int CMainContainer::getInstanceIndexOfASceneNotYetSaved()
{
	for (int i=0;i<getInstanceNumber();i++)
	{
		if (undoBufferContainerList[i]->isSceneSaveMaybeNeededFlagSet())
			return(i);
	}
	return(-1);
}

void CMainContainer::setModificationFlag(int bitMask)
{
	_modificationFlags|=bitMask;
}

int CMainContainer::getModificationFlags(bool clearTheFlagsAfter)
{
	if (objCont->getEditModeType()!=NO_EDIT_MODE)
		_modificationFlags|=128;
	std::vector<int> currentUniqueIdsOfSel;
	for (int i=0;i<int(objCont->getSelSize());i++)
	{
		C3DObject* it=objCont->getObject(objCont->getSelID(i));
		if (it!=NULL)
			currentUniqueIdsOfSel.push_back(it->getUniqueID());
	}
	if (currentUniqueIdsOfSel.size()==_uniqueIdsOfSelectionSinceLastTimeGetAndClearModificationFlagsWasCalled.size())
	{
		for (int i=0;i<int(currentUniqueIdsOfSel.size());i++)
		{
			if (currentUniqueIdsOfSel[i]!=_uniqueIdsOfSelectionSinceLastTimeGetAndClearModificationFlagsWasCalled[i])
			{
				_modificationFlags|=512; // selection state changed
				break;
			}
		}
	}
	else
		_modificationFlags|=512; // selection state changed

	int retVal=_modificationFlags;
	if (clearTheFlagsAfter)
		_modificationFlags=0;
	return(retVal);
}

void CMainContainer::closeDialogTemporarilyIfOpened(int dlgID,std::vector<int>& vect)
{
	if (App::mainWindow->dlgCont->isVisible(dlgID))
	{
		App::mainWindow->dlgCont->close(dlgID);
		vect.push_back(dlgID);
	}
}

void CMainContainer::closeTemporarilyDialogsForPageSelector()
{
	closeDialogTemporarilyIfOpened(SETTINGS_DLG,_dialogsClosedTemporarily_pageSelector);
	closeDialogTemporarilyIfOpened(SELECTION_DLG,_dialogsClosedTemporarily_pageSelector);
	closeDialogTemporarilyIfOpened(SIMULATION_DLG,_dialogsClosedTemporarily_pageSelector);
	closeDialogTemporarilyIfOpened(ENVIRONMENT_DLG,_dialogsClosedTemporarily_pageSelector);
	closeDialogTemporarilyIfOpened(COLLECTION_DLG,_dialogsClosedTemporarily_pageSelector);
	closeDialogTemporarilyIfOpened(LUA_SCRIPT_DLG,_dialogsClosedTemporarily_pageSelector);
	closeDialogTemporarilyIfOpened(OBJECT_DLG,_dialogsClosedTemporarily_pageSelector);
	closeDialogTemporarilyIfOpened(CALCULATION_DLG,_dialogsClosedTemporarily_pageSelector);

	closeDialogTemporarilyIfOpened(TRANSLATION_ROTATION_DLG,_dialogsClosedTemporarily_pageSelector);

	App::mainWindow->dlgCont->close(COLOR_DLG);
	App::mainWindow->dlgCont->close(MATERIAL_DLG);
	App::mainWindow->dlgCont->close(FOG_DLG);
	App::mainWindow->dlgCont->close(TEXTURE_DLG);
}

void CMainContainer::reopenTemporarilyClosedDialogsForPageSelector()
{
	for (int i=0;i<int(_dialogsClosedTemporarily_pageSelector.size());i++)
		App::mainWindow->dlgCont->openOrBringToFront(_dialogsClosedTemporarily_pageSelector[i]);
	_dialogsClosedTemporarily_pageSelector.clear();
}


void CMainContainer::closeTemporarilyDialogsForViewSelector()
{
	closeDialogTemporarilyIfOpened(SETTINGS_DLG,_dialogsClosedTemporarily_viewSelector);
	closeDialogTemporarilyIfOpened(SELECTION_DLG,_dialogsClosedTemporarily_viewSelector);
	closeDialogTemporarilyIfOpened(SIMULATION_DLG,_dialogsClosedTemporarily_viewSelector);
	closeDialogTemporarilyIfOpened(ENVIRONMENT_DLG,_dialogsClosedTemporarily_viewSelector);
	closeDialogTemporarilyIfOpened(COLLECTION_DLG,_dialogsClosedTemporarily_viewSelector);
	closeDialogTemporarilyIfOpened(LUA_SCRIPT_DLG,_dialogsClosedTemporarily_viewSelector);
	closeDialogTemporarilyIfOpened(OBJECT_DLG,_dialogsClosedTemporarily_viewSelector);
	closeDialogTemporarilyIfOpened(CALCULATION_DLG,_dialogsClosedTemporarily_viewSelector);

	closeDialogTemporarilyIfOpened(TRANSLATION_ROTATION_DLG,_dialogsClosedTemporarily_viewSelector);

	App::mainWindow->dlgCont->close(COLOR_DLG);
	App::mainWindow->dlgCont->close(MATERIAL_DLG);
	App::mainWindow->dlgCont->close(FOG_DLG);
	App::mainWindow->dlgCont->close(TEXTURE_DLG);
}

void CMainContainer::reopenTemporarilyClosedDialogsForViewSelector()
{
	for (int i=0;i<int(_dialogsClosedTemporarily_viewSelector.size());i++)
		App::mainWindow->dlgCont->openOrBringToFront(_dialogsClosedTemporarily_viewSelector[i]);
	_dialogsClosedTemporarily_viewSelector.clear();
}


void CMainContainer::closeTemporarilyDialogsForSceneSelector()
{
	closeDialogTemporarilyIfOpened(SETTINGS_DLG,_dialogsClosedTemporarily_sceneSelector);
	closeDialogTemporarilyIfOpened(SELECTION_DLG,_dialogsClosedTemporarily_sceneSelector);
	closeDialogTemporarilyIfOpened(SIMULATION_DLG,_dialogsClosedTemporarily_sceneSelector);
	closeDialogTemporarilyIfOpened(ENVIRONMENT_DLG,_dialogsClosedTemporarily_sceneSelector);
	closeDialogTemporarilyIfOpened(COLLECTION_DLG,_dialogsClosedTemporarily_sceneSelector);
	closeDialogTemporarilyIfOpened(LUA_SCRIPT_DLG,_dialogsClosedTemporarily_sceneSelector);
	closeDialogTemporarilyIfOpened(OBJECT_DLG,_dialogsClosedTemporarily_sceneSelector);
	closeDialogTemporarilyIfOpened(CALCULATION_DLG,_dialogsClosedTemporarily_sceneSelector);

	closeDialogTemporarilyIfOpened(TRANSLATION_ROTATION_DLG,_dialogsClosedTemporarily_sceneSelector);

	App::mainWindow->dlgCont->close(COLOR_DLG);
	App::mainWindow->dlgCont->close(MATERIAL_DLG);
	App::mainWindow->dlgCont->close(FOG_DLG);
	App::mainWindow->dlgCont->close(TEXTURE_DLG);
}

void CMainContainer::reopenTemporarilyClosedDialogsForSceneSelector()
{
	for (int i=0;i<int(_dialogsClosedTemporarily_sceneSelector.size());i++)
		App::mainWindow->dlgCont->openOrBringToFront(_dialogsClosedTemporarily_sceneSelector[i]);
	_dialogsClosedTemporarily_sceneSelector.clear();
}

void CMainContainer::reopenTemporarilyClosedNonEditModeDialogs()
{
	for (int i=0;i<int(_dialogsClosedTemporarily_editModes.size());i++)
		App::mainWindow->dlgCont->openOrBringToFront(_dialogsClosedTemporarily_editModes[i]);
	_dialogsClosedTemporarily_editModes.clear();
}
void CMainContainer::closeTemporarilyNonEditModeDialogs()
{
	closeDialogTemporarilyIfOpened(SETTINGS_DLG,_dialogsClosedTemporarily_editModes);
	closeDialogTemporarilyIfOpened(SELECTION_DLG,_dialogsClosedTemporarily_editModes);
	closeDialogTemporarilyIfOpened(SIMULATION_DLG,_dialogsClosedTemporarily_editModes);
	closeDialogTemporarilyIfOpened(ENVIRONMENT_DLG,_dialogsClosedTemporarily_editModes);
	closeDialogTemporarilyIfOpened(COLLECTION_DLG,_dialogsClosedTemporarily_editModes);
	closeDialogTemporarilyIfOpened(LUA_SCRIPT_DLG,_dialogsClosedTemporarily_editModes);
	closeDialogTemporarilyIfOpened(OBJECT_DLG,_dialogsClosedTemporarily_editModes);
	closeDialogTemporarilyIfOpened(CALCULATION_DLG,_dialogsClosedTemporarily_editModes);

	App::mainWindow->dlgCont->close(COLOR_DLG);
	App::mainWindow->dlgCont->close(MATERIAL_DLG);
	App::mainWindow->dlgCont->close(FOG_DLG);
	App::mainWindow->dlgCont->close(TEXTURE_DLG);
}



int CMainContainer::createNewInstance(bool triggerCallbackAndHandleDialogs)
{
	FUNCTION_DEBUG;
	if (globalInstanceCreationOrSwitchingLocked)
		return(-1);

	if (triggerCallbackAndHandleDialogs)
		visibleInstanceAboutToChange_infoForDialogsOnly();

	genericDialogContainerList.push_back(NULL);
	undoBufferContainerList.push_back(NULL);
	outsideCommandQueueList.push_back(NULL);
	simulationList.push_back(NULL);
	confContainerList.push_back(NULL);
	textureContList.push_back(NULL);
	dynMaterialContList.push_back(NULL);
	buttonBlockContainerList.push_back(NULL);
	collectionList.push_back(NULL);
	luaScriptContainerList.push_back(NULL);
	distanceList.push_back(NULL);
	collisionList.push_back(NULL);
	environmentList.push_back(NULL);
	oglSurfaceList.push_back(NULL);
	mainSettingsList.push_back(NULL);
	pathPlanningList.push_back(NULL);
	motionPlanningList.push_back(NULL);
	customSceneDataList.push_back(NULL);
	cacheDataList.push_back(NULL);
	infoDisplayList.push_back(NULL);
	constraintSolverList.push_back(NULL);
	apiErrorsList.push_back(NULL);
	drawingContainerList.push_back(NULL);
	pointCloudContainerList.push_back(NULL);
	ghostObjectContainerList.push_back(NULL);
	bannerContainerList.push_back(NULL);
	dynamicsContainerList.push_back(NULL);
	signalContainerList.push_back(NULL);
	commTubeContainerList.push_back(NULL);
	scintillaEditorContainerList.push_back(NULL);

	ikGroupList.push_back(NULL);
	objContList.push_back(NULL);

	currentInstanceIndex=int(objContList.size())-1;

	undoBufferContainer=NULL;
	genericDialogContainer=NULL;
	outsideCommandQueue=NULL;
	buttonBlockContainer=NULL;
	simulation=NULL;
	confContainer=NULL;
	textureCont=NULL;
	dynMaterialCont=NULL;
	luaScriptContainer=NULL;
	collections=NULL;
	distances=NULL;
	collisions=NULL;
	pathPlanning=NULL;
	motionPlanning=NULL;
	environment=NULL;
	oglSurface=NULL;
	mainSettings=NULL;
	customSceneData=NULL;
	cacheData=NULL;
	infoDisplay=NULL;
	constraintSolver=NULL;
	apiErrors=NULL;
	drawingCont=NULL;
	pointCloudCont=NULL;
	ghostObjectCont=NULL;
	bannerCont=NULL;
	dynamicsContainer=NULL;
	signalContainer=NULL;
	commTubeContainer=NULL;
	scintillaEditorContainer=NULL;

	ikGroups=NULL;
	objCont=NULL;
	undoBufferContainer=new CUndoBufferCont();
	genericDialogContainer=new CGenericDialogContainer();
	outsideCommandQueue=new COutsideCommandQueue();
	buttonBlockContainer=new CButtonBlockContainer(true);
	simulation=new CSimulation();
	confContainer=new CMemorizedConfContainer();
	textureCont=new CTextureContainer();
	dynMaterialCont=new CDynMaterialContainer();
	luaScriptContainer=new CLuaScriptContainer();
	collections=new CRegisteredGroups();
	ikGroups=new CRegisterediks();
	distances=new CRegisteredDistances();
	collisions=new CRegisteredCollisions();
	pathPlanning=new CRegisteredPathPlanningTasks();
	motionPlanning=new CRegisteredMotionPlanningTasks();
	environment=new CEnvironment();
	oglSurface=new COglSurface();
	objCont=new CObjCont();
	mainSettings=new CMainSettings();
	customSceneData=new CCustomData();
	cacheData=new CCacheCont();
	infoDisplay=new CInfoDisplay();
	constraintSolver=new CConstraintSolverContainer();
	apiErrors=new CApiErrors();
	drawingCont=new CDrawingContainer();
	pointCloudCont=new CPointCloudContainer();
	ghostObjectCont=new CGhostObjectContainer();
	bannerCont=new CBannerContainer();
	dynamicsContainer=new CDynamicsContainer();
	signalContainer=new CSignalContainer();
	commTubeContainer=new CCommTubeContainer();
	scintillaEditorContainer=new CScintillaEditorContainer();

	undoBufferContainerList[currentInstanceIndex]=undoBufferContainer;
	genericDialogContainerList[currentInstanceIndex]=genericDialogContainer;
	outsideCommandQueueList[currentInstanceIndex]=outsideCommandQueue;
	buttonBlockContainerList[currentInstanceIndex]=buttonBlockContainer;
	simulationList[currentInstanceIndex]=simulation;
	confContainerList[currentInstanceIndex]=confContainer;
	textureContList[currentInstanceIndex]=textureCont;
	dynMaterialContList[currentInstanceIndex]=dynMaterialCont;
	collectionList[currentInstanceIndex]=collections;
	luaScriptContainerList[currentInstanceIndex]=luaScriptContainer;
	distanceList[currentInstanceIndex]=distances;
	collisionList[currentInstanceIndex]=collisions;
	environmentList[currentInstanceIndex]=environment;
	oglSurfaceList[currentInstanceIndex]=oglSurface;
	mainSettingsList[currentInstanceIndex]=mainSettings;
	pathPlanningList[currentInstanceIndex]=pathPlanning;
	motionPlanningList[currentInstanceIndex]=motionPlanning;
	customSceneDataList[currentInstanceIndex]=customSceneData;
	cacheDataList[currentInstanceIndex]=cacheData;
	infoDisplayList[currentInstanceIndex]=infoDisplay;
	constraintSolverList[currentInstanceIndex]=constraintSolver;
	apiErrorsList[currentInstanceIndex]=apiErrors;
	drawingContainerList[currentInstanceIndex]=drawingCont;
	pointCloudContainerList[currentInstanceIndex]=pointCloudCont;
	ghostObjectContainerList[currentInstanceIndex]=ghostObjectCont;
	bannerContainerList[currentInstanceIndex]=bannerCont;
	dynamicsContainerList[currentInstanceIndex]=dynamicsContainer;
	signalContainerList[currentInstanceIndex]=signalContainer;
	commTubeContainerList[currentInstanceIndex]=commTubeContainer;
	scintillaEditorContainerList[currentInstanceIndex]=scintillaEditorContainer;

	ikGroupList[currentInstanceIndex]=ikGroups;
	objContList[currentInstanceIndex]=objCont;

	if (triggerCallbackAndHandleDialogs)
	{
		visibleInstanceJustChanged_infoForDialogsOnly();
		int data[4]={getCurrentInstanceIndex(),0,0,0};
		void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_instanceswitch,data,NULL,NULL);
		delete[] (char*)returnVal;
		setModificationFlag(64); // instance switched
	}

	return(currentInstanceIndex);
}

int CMainContainer::getCurrentInstanceIndex()
{
	return (currentInstanceIndex);
}

int CMainContainer::destroyCurrentInstance()
{
	if (currentInstanceIndex==-1)
		return(-1);

	visibleInstanceAboutToChange_infoForDialogsOnly();
	closeAllNonToolDialogs();

	// Important to empty objects first (since objCont->announce....willBeErase
	// might be called for already destroyed objects!)
	confContainer->removeMemorized();
	collections->removeAllGroups();
	luaScriptContainer->removeAllScripts(false);

	ikGroups->removeAllIkGroups();

	distances->removeAllDistanceObjects();
	collisions->removeAllCollisionObjects();
	pathPlanning->removeAllTasks();
	motionPlanning->removeAllTasks();

	objCont->removeAllObjects();

	simulation->newSceneProcedure();
	if (buttonBlockContainer!=NULL)
		buttonBlockContainer->removeAllBlocks(true);


	delete scintillaEditorContainer;
	scintillaEditorContainer=NULL;
	delete undoBufferContainer;
	undoBufferContainer=NULL;
	delete genericDialogContainer;
	genericDialogContainer=NULL;
	delete confContainer;
	confContainer=NULL;
	delete luaScriptContainer;
	luaScriptContainer=NULL;
	delete dynamicsContainer;
	dynamicsContainer=NULL;
	delete mainSettings;
	mainSettings=NULL;

	delete objCont;
	objCont=NULL;

	delete oglSurface;
	oglSurface=NULL;
	delete environment;
	environment=NULL;
	delete pathPlanning;
	pathPlanning=NULL;
	delete motionPlanning;
	motionPlanning=NULL;
	delete collisions;
	collisions=NULL;
	delete distances;
	distances=NULL;

	delete ikGroups;
	ikGroups=NULL;

	delete collections;
	collections=NULL;
	delete textureCont;
	textureCont=NULL;
	delete dynMaterialCont;
	dynMaterialCont=NULL;
	delete simulation;
	simulation=NULL;
	delete buttonBlockContainer;
	buttonBlockContainer=NULL;
	delete outsideCommandQueue;
	outsideCommandQueue=NULL;
	delete customSceneData;
	customSceneData=NULL;
	delete cacheData;
	cacheData=NULL;
	delete infoDisplay;
	infoDisplay=NULL;
	delete constraintSolver;
	constraintSolver=NULL;
	delete apiErrors;
	apiErrors=NULL;
	delete drawingCont;
	drawingCont=NULL;
	delete pointCloudCont;
	pointCloudCont=NULL;
	delete ghostObjectCont;
	ghostObjectCont=NULL;
	delete bannerCont;
	bannerCont=NULL;
	delete signalContainer;
	signalContainer=NULL;
	delete commTubeContainer;
	commTubeContainer=NULL;

	genericDialogContainerList.erase(genericDialogContainerList.begin()+currentInstanceIndex);
	undoBufferContainerList.erase(undoBufferContainerList.begin()+currentInstanceIndex);
	confContainerList.erase(confContainerList.begin()+currentInstanceIndex);
	luaScriptContainerList.erase(luaScriptContainerList.begin()+currentInstanceIndex);
	mainSettingsList.erase(mainSettingsList.begin()+currentInstanceIndex);
	oglSurfaceList.erase(oglSurfaceList.begin()+currentInstanceIndex);
	environmentList.erase(environmentList.begin()+currentInstanceIndex);
	pathPlanningList.erase(pathPlanningList.begin()+currentInstanceIndex);
	motionPlanningList.erase(motionPlanningList.begin()+currentInstanceIndex);
	collisionList.erase(collisionList.begin()+currentInstanceIndex);
	distanceList.erase(distanceList.begin()+currentInstanceIndex);
	collectionList.erase(collectionList.begin()+currentInstanceIndex);
	textureContList.erase(textureContList.begin()+currentInstanceIndex);
	dynMaterialContList.erase(dynMaterialContList.begin()+currentInstanceIndex);
	simulationList.erase(simulationList.begin()+currentInstanceIndex);
	buttonBlockContainerList.erase(buttonBlockContainerList.begin()+currentInstanceIndex);
	outsideCommandQueueList.erase(outsideCommandQueueList.begin()+currentInstanceIndex);
	customSceneDataList.erase(customSceneDataList.begin()+currentInstanceIndex);
	cacheDataList.erase(cacheDataList.begin()+currentInstanceIndex);
	infoDisplayList.erase(infoDisplayList.begin()+currentInstanceIndex);
	constraintSolverList.erase(constraintSolverList.begin()+currentInstanceIndex);
	apiErrorsList.erase(apiErrorsList.begin()+currentInstanceIndex);
	drawingContainerList.erase(drawingContainerList.begin()+currentInstanceIndex);
	pointCloudContainerList.erase(pointCloudContainerList.begin()+currentInstanceIndex);
	ghostObjectContainerList.erase(ghostObjectContainerList.begin()+currentInstanceIndex);
	bannerContainerList.erase(bannerContainerList.begin()+currentInstanceIndex);
	dynamicsContainerList.erase(dynamicsContainerList.begin()+currentInstanceIndex);
	signalContainerList.erase(signalContainerList.begin()+currentInstanceIndex);
	commTubeContainerList.erase(commTubeContainerList.begin()+currentInstanceIndex);
	scintillaEditorContainerList.erase(scintillaEditorContainerList.begin()+currentInstanceIndex);

	objContList.erase(objContList.begin()+currentInstanceIndex);
	ikGroupList.erase(ikGroupList.begin()+currentInstanceIndex);

	if (objContList.size()!=0)
	{
		currentInstanceIndex=-1; // We set this so that next instruction succeeds (otherwise we might already be in instance 0 and the function will not process). Added on 5/3/2012
		makeInstanceCurrentFromIndex(int(objContList.size())-1,true);
	}
	else
		currentInstanceIndex=-1;
	return(currentInstanceIndex);	
}

int CMainContainer::getInstanceNumber()
{
	return(int(objContList.size()));
}

void CMainContainer::initialize(bool enableMultipleInst)
{
	FUNCTION_DEBUG;
	hierarchy=new CHierarchy();
	browser=new CBrowser();

	simulatorMessageQueue=new CSimulatorMessageQueue();
	copyBuffer=new CCopyBuffer();
	globalTextureCont=new CGlobalTextureContainer();
	scintillaConsoleContainer=new CScintillaConsoleContainer();
	serialPortContainer=new CSerialPortContainer();
//	persistentDataContainer=new CPersistentDataContainer();

	std::string models(App::directories->modelDirectory);
	modelDirectoryRoot=new CDirBrowserElement("Models",models);
	modelDirectoryRoot->setRootElement(modelDirectoryRoot);

	createNewInstance(true);

	// Following 3 have to be done AFTER objCont is created:
	modelDirectoryRoot->setSelectedElement(modelDirectoryRoot->getElementUniqueID());
	int uniqueElement=modelDirectoryRoot->openFoldersLeadingToFolderName("non-mobile");
	modelDirectoryRoot->setSelectedElement(uniqueElement);

	globalInstanceCreationOrSwitchingLocked=!enableMultipleInst;
}

void CMainContainer::deinitialize()
{
	copyBuffer->clearBuffer();
	globalInstanceCreationOrSwitchingLocked=false;

	while (objContList.size()!=0)
	{
		makeInstanceCurrentFromIndex(int(objContList.size())-1,true);
		destroyCurrentInstance();
	}
	delete scintillaConsoleContainer;
	scintillaConsoleContainer=NULL;
//	delete persistentDataContainer;
	delete modelDirectoryRoot;
	delete globalTextureCont;
	delete copyBuffer;
	delete serialPortContainer;
	delete simulatorMessageQueue;

	delete hierarchy;
	delete browser;


}

void CMainContainer::emptyScene(bool notCalledFromUndoFunction)
{
	if (notCalledFromUndoFunction)
		undoBufferContainer->newSceneProcedure();

	// Following is saved:
	simulation->newSceneProcedure();
	if (buttonBlockContainer!=NULL)
		buttonBlockContainer->newSceneProcedure(true);
	collections->setUpDefaultValues();
	luaScriptContainer->removeAllScripts(true);

	ikGroups->removeAllIkGroups();

	distances->setUpDefaultValues();
	collisions->setUpDefaultValues();
	environment->newSceneProcedure();
	oglSurface->newSceneProcedure();
	pathPlanning->removeAllTasks();
	motionPlanning->removeAllTasks();

	objCont->newSceneProcedure();

	customSceneData->newSceneProcedure();
	constraintSolver->newSceneProcedure();

	confContainer->removeMemorized();
	
	if (notCalledFromUndoFunction)
		mainSettings->newSceneProcedure();
	genericDialogContainer->newSceneProcedure();
	cacheData->newSceneProcedure();
	drawingCont->newSceneProcedure();
	pointCloudCont->newSceneProcedure();
	ghostObjectCont->newSceneProcedure();
	bannerCont->newSceneProcedure();
	signalContainer->newSceneProcedure();
	commTubeContainer->newSceneProcedure();
}

bool CMainContainer::makeInstanceCurrentFromIndex(int instanceIndex,bool triggerCallbackAndHandleDialogs)
{ 
	if ( (instanceIndex<0)||(instanceIndex>=int(objContList.size())))
		return(false);

	if (currentInstanceIndex==instanceIndex)
		return(true);
	if (isInstanceSwitchingLocked())
		return(false);

	if (oglSurface!=NULL)
		oglSurface->pageCont->clearAllLastMouseDownViewIndex();

	if (triggerCallbackAndHandleDialogs)
		visibleInstanceAboutToChange_infoForDialogsOnly();

	currentInstanceIndex=instanceIndex;

	undoBufferContainer=undoBufferContainerList[currentInstanceIndex];
	genericDialogContainer=genericDialogContainerList[currentInstanceIndex];
	outsideCommandQueue=outsideCommandQueueList[currentInstanceIndex];
	simulation=simulationList[currentInstanceIndex];
	confContainer=confContainerList[currentInstanceIndex];
	textureCont=textureContList[currentInstanceIndex];
	dynMaterialCont=dynMaterialContList[currentInstanceIndex];
	buttonBlockContainer=buttonBlockContainerList[currentInstanceIndex];
	collections=collectionList[currentInstanceIndex];
	luaScriptContainer=luaScriptContainerList[currentInstanceIndex];
	distances=distanceList[currentInstanceIndex];
	collisions=collisionList[currentInstanceIndex];
	environment=environmentList[currentInstanceIndex];
	oglSurface=oglSurfaceList[currentInstanceIndex];
	mainSettings=mainSettingsList[currentInstanceIndex];
	pathPlanning=pathPlanningList[currentInstanceIndex];
	motionPlanning=motionPlanningList[currentInstanceIndex];
	customSceneData=customSceneDataList[currentInstanceIndex];
	cacheData=cacheDataList[currentInstanceIndex];
	infoDisplay=infoDisplayList[currentInstanceIndex];
	constraintSolver=constraintSolverList[currentInstanceIndex];
	apiErrors=apiErrorsList[currentInstanceIndex];
	drawingCont=drawingContainerList[currentInstanceIndex];
	pointCloudCont=pointCloudContainerList[currentInstanceIndex];
	ghostObjectCont=ghostObjectContainerList[currentInstanceIndex];
	bannerCont=bannerContainerList[currentInstanceIndex];
	dynamicsContainer=dynamicsContainerList[currentInstanceIndex];
	signalContainer=signalContainerList[currentInstanceIndex];
	commTubeContainer=commTubeContainerList[currentInstanceIndex];
	scintillaEditorContainer=scintillaEditorContainerList[currentInstanceIndex];

	ikGroups=ikGroupList[currentInstanceIndex];
	objCont=objContList[currentInstanceIndex];

	if (triggerCallbackAndHandleDialogs)
	{
		visibleInstanceJustChanged_infoForDialogsOnly();
		int data[4]={getCurrentInstanceIndex(),0,0,0};
		void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_instanceswitch,data,NULL,NULL);
		delete[] (char*)returnVal;
		setModificationFlag(64); // instance switched
	}

	return(true);
}

bool CMainContainer::isInstanceSwitchingLocked()
{
	if (simulation==NULL)
		return(false);
	if (!simulation->isSimulationStopped())
		return(true);
	if (oglSurface->isViewSelectionActive()||oglSurface->isPageSelectionActive()) 
		return(true);
	return(globalInstanceCreationOrSwitchingLocked);
}

