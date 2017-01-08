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
#include "DlgCont.h"
#include "Global.h"
#include "OGL.h"
#include "OglSurface.h"
#include "IloIlo.h"
#include "App.h"
#include "auxLibVideo.h"
#include "v_repStrings.h"
#include "qdlgobjectdialogcontainer.h"

CDlgCont::CDlgCont(QWidget* pWindow)
{
	MUST_BE_UI_THREAD;
	_destroyingContainerNow=false;
	initialize(pWindow);
}

CDlgCont::~CDlgCont()
{
	MUST_BE_UI_THREAD;
	killAllDialogs();
}

void CDlgCont::initialize(QWidget* pWindow)
{
	MUST_BE_UI_THREAD;
	if (dialogs.size()!=0)
		return; // Already initialized!
	parentWindow=pWindow;

	dialogs.push_back(new CToolDlgWrapper(DETECTION_VOLUME_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(VISION_SENSOR_FILTER_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(SHAPE_DYN_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(SHAPE_EDITION_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(OBJECT_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(CALCULATION_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(JOINT_DYN_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(CUSTOM_UI_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(TRANSLATION_ROTATION_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(PATH_EDITION_DLG,0));

	dialogs.push_back(new CToolDlgWrapper(GENERAL_PROPERTIES_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(TRANSFORMATIONS_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(SHAPE_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(JOINT_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(CAMERA_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(LIGHT_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(MIRROR_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(PROXIMITY_SENSOR_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(VISION_SENSOR_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(MILL_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(FORCE_SENSOR_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(GRAPH_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(SETTINGS_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(SELECTION_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(COLLECTION_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(ENVIRONMENT_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(COLLISION_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(DISTANCE_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(IK_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(BUTTON_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(LUA_SCRIPT_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(DUMMY_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(LAYERS_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(DYNAMICS_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(PATH_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(PATH_PLANNING_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(CONSTRAINT_SOLVER_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(SIMULATION_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(FOG_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(MATERIAL_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(COLOR_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(TEXTURE_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(GEOMETRY_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(PATH_SHAPING_DLG,0));
	dialogs.push_back(new CToolDlgWrapper(AVI_RECORDER_DLG,0));
}

void CDlgCont::setRefreshDegree(int d)
{
	MUST_BE_UI_THREAD;
	if (!_destroyingContainerNow)
	{
		for (int i=0;i<int(dialogs.size());i++)
			dialogs[i]->setRefreshDegree(d);
	}
}
void CDlgCont::refresh()
{
	MUST_BE_UI_THREAD;
	if (!_destroyingContainerNow)
	{
		for (int i=0;i<int(dialogs.size());i++)
			dialogs[i]->refresh();
	}
}
void CDlgCont::destroyWhatNeedsDestruction()
{
	MUST_BE_UI_THREAD;
	if (!_destroyingContainerNow)
	{
		for (int i=0;i<int(dialogs.size());i++)
			dialogs[i]->destroyIfDestructionNeeded();
	}
}

void CDlgCont::visibleInstanceAboutToSwitch()
{
	MUST_BE_UI_THREAD;
	if (!_destroyingContainerNow)
	{
		for (int i=0;i<int(dialogs.size());i++)
			dialogs[i]->visibleInstanceAboutToSwitch();
	}
}

void CDlgCont::showDialogsButDontOpenThem()
{
	MUST_BE_UI_THREAD;
	if (!_destroyingContainerNow)
	{
		for (int i=0;i<int(dialogs.size());i++)
			dialogs[i]->showDialogButDontOpenIt();
	}
}

void CDlgCont::hideDialogsButDontCloseThem()
{
	MUST_BE_UI_THREAD;
	if (!_destroyingContainerNow)
	{
		for (int i=0;i<int(dialogs.size());i++)
			dialogs[i]->hideDialogButDontCloseIt();
	}
}

bool CDlgCont::openOrBringToFront(int dlgID)
{
	MUST_BE_UI_THREAD;
	if (!_destroyingContainerNow)
	{
		if (dlgID==HIERARCHY_DLG)
			App::ct->oglSurface->setHierarchyEnabled(true);
		else if (dlgID==BROWSER_DLG)
			App::ct->oglSurface->setBrowserEnabled(true);
		else
		{
		// Here we check if we can open the dialog: (check also "toggle" function!)

			if ((App::operationalUIParts&sim_gui_dialogs)==0)
			{
				return(false);
			}
			CToolDlgWrapper* it=_getDialogWrapper(dlgID);
			if (it!=NULL)
				it->setVisible(true,parentWindow);
		}
		App::ct->objCont->toolbarRefreshFlag=true;
		return(true);
	}
	return(false);
}

void CDlgCont::close(int dlgID)
{
	MUST_BE_UI_THREAD;
	if (!_destroyingContainerNow)
	{
		if (dlgID==HIERARCHY_DLG)
			App::ct->oglSurface->setHierarchyEnabled(false);
		else if (dlgID==BROWSER_DLG)
			App::ct->oglSurface->setBrowserEnabled(false);
		else
		{
			CToolDlgWrapper* it=_getDialogWrapper(dlgID);
			if (it!=NULL)
				it->setVisible(false,parentWindow);
		}
		App::ct->objCont->toolbarRefreshFlag=true;
	}
}

bool CDlgCont::toggle(int dlgID)
{
	MUST_BE_UI_THREAD;
	if (!_destroyingContainerNow)
	{
		if (dlgID==HIERARCHY_DLG)
			App::ct->oglSurface->setHierarchyEnabled(!App::ct->oglSurface->isHierarchyEnabled());
		else if (dlgID==BROWSER_DLG)
			App::ct->oglSurface->setBrowserEnabled(!App::ct->oglSurface->isBrowserEnabled());
		else
		{
		// Here we check if we can open the dialog: (check also "openOrBringToFront" function!)
			if ((App::operationalUIParts&sim_gui_dialogs)==0)
			{
				return(false);
			}

			CToolDlgWrapper* it=_getDialogWrapper(dlgID);
			if (it!=NULL)
				it->setVisible(!it->getVisible(),parentWindow);
		}
		App::ct->objCont->toolbarRefreshFlag=true;
		return(true);
	}
	return(false);
}

CToolDlgWrapper* CDlgCont::_getDialogWrapper(int dlgID)
{
	if (!_destroyingContainerNow)
	{
		for (int i=0;i<int(dialogs.size());i++)
		{
			if (dialogs[i]->getDialogType()==dlgID)
				return(dialogs[i]);
		}
	}
	return(NULL);
}

VDialog* CDlgCont::getDialog(int dlgID)
{
	MUST_BE_UI_THREAD;
	if (!_destroyingContainerNow)
	{
		for (int i=0;i<int(dialogs.size());i++)
		{
			if (dialogs[i]->getDialogType()==dlgID)
				return(dialogs[i]->getDialog());
		}
	}
	return(NULL);
}

bool CDlgCont::isVisible(int dlgID)
{
	MUST_BE_UI_THREAD;
	if (!_destroyingContainerNow)
	{
		if (dlgID==HIERARCHY_DLG)
			return(App::ct->oglSurface->isHierarchyEnabled());
		else if (dlgID==BROWSER_DLG)
			return(App::ct->oglSurface->isBrowserEnabled());
		else
		{
			CToolDlgWrapper* it=_getDialogWrapper(dlgID);
			if (it!=NULL)
				return(it->getVisible());
		}
	}
	return(false);
}

void CDlgCont::killAllDialogs()
{
	MUST_BE_UI_THREAD;
	_destroyingContainerNow=true;
	for (int i=0;i<int(dialogs.size());i++)
		delete dialogs[i];
	dialogs.clear();
}

void CDlgCont::getWindowPos(int dlgID,int pos[2],bool& visible)
{
	MUST_BE_UI_THREAD;
	if (!_destroyingContainerNow)
	{
		if (dlgID==HIERARCHY_DLG)
		{
			pos[0]=App::ct->oglSurface->getHierarchyWidth();
			pos[1]=0;
			visible=App::ct->oglSurface->isHierarchyEnabled();
		}
		else if (dlgID==BROWSER_DLG)
		{
			pos[0]=App::ct->oglSurface->getBrowserWidth();
			pos[1]=0;
			visible=App::ct->oglSurface->isBrowserEnabled();
		}
		else
		{
			CToolDlgWrapper* it=_getDialogWrapper(dlgID);
			if (it!=NULL)
			{
				it->getPosition(pos);
				visible=it->getVisible();
			}
		}
	}
}

void CDlgCont::setWindowPos(int dlgID,int pos[2],bool visible)
{
	MUST_BE_UI_THREAD;
	if (!_destroyingContainerNow)
	{
		if (dlgID==HIERARCHY_DLG)
		{
			App::ct->oglSurface->setHierarchyWidth(pos[0]);
			App::ct->oglSurface->setHierarchyEnabled(visible);
		}
		else if (dlgID==BROWSER_DLG)
		{
			App::ct->oglSurface->setBrowserWidth(pos[0]);
			App::ct->oglSurface->setBrowserEnabled(visible);
		}
		else
		{
			CToolDlgWrapper* it=_getDialogWrapper(dlgID);
			if (it!=NULL)
			{
				it->setPosition(pos[0],pos[1]);
				it->setVisible(visible,parentWindow);
			}
		}
	}
}

void CDlgCont::addMenu(VMenu* menu)
{ 
	bool noShapePathEditModeNoSelector=true;
	if ((App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)||(App::ct->objCont->getEditModeType()==PATH_EDIT_MODE))
		noShapePathEditModeNoSelector=false;
	if (App::ct->oglSurface->isSceneSelectionActive()||App::ct->oglSurface->isPageSelectionActive())
		noShapePathEditModeNoSelector=false;

	VMenubar::appendMenuItem(menu,noShapePathEditModeNoSelector,isVisible(OBJECT_DLG),TOGGLE_OBJECT_DLG_CMD,IDSN_OBJECT_PROPERTIES_MENU_ITEM,true);
	VMenubar::appendMenuItem(menu,noShapePathEditModeNoSelector,isVisible(CALCULATION_DLG),TOGGLE_CALCULATION_DLG_CMD,IDSN_CALCULATION_MODULE_PROPERTIES_MENU_ITEM,true);

	VMenubar::appendMenuSeparator(menu);
	VMenubar::appendMenuItem(menu,noShapePathEditModeNoSelector,isVisible(LUA_SCRIPT_DLG),TOGGLE_LUA_SCRIPT_DLG_CMD,IDSN_SCRIPTS,true);

	VMenubar::appendMenuItem(menu,(App::ct->simulation->isSimulationStopped()&&App::ct->objCont->isLastSelectionAShape()&&(App::ct->objCont->getEditModeType()==NO_EDIT_MODE))||(App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE),App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE,TOGGLE_SHAPE_EDITION_DLG_CMD,IDSN_SHAPE_EDITION_MENU_ITEM,true);
	VMenubar::appendMenuItem(menu,(App::ct->simulation->isSimulationStopped()&&App::ct->objCont->isLastSelectionAPath()&&(App::ct->objCont->getEditModeType()==NO_EDIT_MODE))||(App::ct->objCont->getEditModeType()==PATH_EDIT_MODE),App::ct->objCont->getEditModeType()==PATH_EDIT_MODE,TOGGLE_PATH_EDITION_DLG_CMD,IDSN_PATH_EDITION_MENU_ITEM,true);
	VMenubar::appendMenuItem(menu,App::ct->simulation->isSimulationStopped()&&((App::ct->objCont->getEditModeType()==NO_EDIT_MODE)||(App::ct->objCont->getEditModeType()==BUTTON_EDIT_MODE)),App::ct->objCont->getEditModeType()==BUTTON_EDIT_MODE,TOGGLE_CUSTOM_UI_DLG_CMD,IDS_2D_ELEMENTS,true);

	VMenubar::appendMenuItem(menu,noShapePathEditModeNoSelector,isVisible(COLLECTION_DLG),TOGGLE_COLLECTION_DLG_CMD,IDSN_COLLECTIONS,true);
	VMenubar::appendMenuItem(menu,noShapePathEditModeNoSelector,isVisible(SELECTION_DLG),TOGGLE_SELECTION_DLG_CMD,IDSN_SELECTION,true);
	VMenubar::appendMenuItem(menu,noShapePathEditModeNoSelector,isVisible(ENVIRONMENT_DLG),TOGGLE_ENVIRONMENT_DLG_CMD,IDSN_ENVIRONMENT,true);
	VMenubar::appendMenuItem(menu,noShapePathEditModeNoSelector,App::ct->oglSurface->isBrowserEnabled(),TOGGLE_BROWSER_DLG_CMD,IDSN_MODEL_BROWSER,true);
	VMenubar::appendMenuItem(menu,true,App::ct->oglSurface->isHierarchyEnabled(),TOGGLE_HIERARCHY_DLG_CMD,IDSN_SCENE_HIERARCHY,true);
	VMenubar::appendMenuItem(menu,true,isVisible(LAYERS_DLG),TOGGLE_LAYERS_DLG_CMD,IDS_LAYERS,true);
	VMenubar::appendMenuItem(menu,CAuxLibVideo::video_recorderGetEncoderString!=NULL,isVisible(AVI_RECORDER_DLG),TOGGLE_AVI_RECORDER_DLG_CMD,IDSN_AVI_RECORDER,true);
	VMenubar::appendMenuItem(menu,noShapePathEditModeNoSelector,isVisible(SETTINGS_DLG),TOGGLE_SETTINGS_DLG_CMD,IDSN_USER_SETTINGS,true);


}

bool CDlgCont::evaluateMenuSelection(int commandID)
{ // Return value is true if the command belonged to dlg menu and was executed
	if (commandID==OPEN_GRAPH_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_GRAPH_DLG_CMD")
		{
			openOrBringToFront(GRAPH_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_CAMERA_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_CAMERA_DLG_CMD")
		{
			openOrBringToFront(CAMERA_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_LIGHT_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_LIGHT_DLG_CMD")
		{
			openOrBringToFront(LIGHT_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_MIRROR_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_MIRROR_DLG_CMD")
		{
			openOrBringToFront(MIRROR_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_SHAPE_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_SHAPE_DLG_CMD")
		{
			openOrBringToFront(SHAPE_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_PROXIMITY_SENSOR_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_PROXIMITY_SENSOR_DLG_CMD")
		{
			openOrBringToFront(PROXIMITY_SENSOR_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_VISION_SENSOR_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_VISION_SENSOR_DLG_CMD")
		{
			openOrBringToFront(VISION_SENSOR_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_MILL_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_MILL_DLG_CMD")
		{
			openOrBringToFront(MILL_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_OBJECT_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_OBJECT_DLG_CMD")
		{
			openOrBringToFront(OBJECT_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_SHAPE_EDITION_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_SHAPE_EDITION_DLG_CMD")
		{
			openOrBringToFront(SHAPE_EDITION_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_OBJECT_DLG_OBJECT_SPECIFIC_PART_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_OBJECT_DLG_OBJECT_SPECIFIC_PART_CMD")
		{
			openOrBringToFront(OBJECT_DLG);
			VDialog* dlg=getDialog(OBJECT_DLG);
			if (dlg!=NULL)
			{
				((CQDlgObjectDialogContainer*)dlg)->desiredPage=0;
				((CQDlgObjectDialogContainer*)dlg)->refresh();
			}
		}
		return(true);
	}
	if (commandID==OPEN_FORCE_SENSOR_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_FORCE_SENSOR_DLG_CMD")
		{
			openOrBringToFront(FORCE_SENSOR_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_DUMMY_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_DUMMY_DLG_CMD")
		{
			openOrBringToFront(DUMMY_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_JOINT_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_JOINT_DLG_CMD")
		{
			openOrBringToFront(JOINT_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_PATH_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_PATH_DLG_CMD")
		{
			openOrBringToFront(PATH_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_BUTTON_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_BUTTON_DLG_CMD")
		{
			openOrBringToFront(BUTTON_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_TRANSFORMATIONS_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_TRANSFORMATIONS_DLG_CMD")
		{
			openOrBringToFront(TRANSFORMATIONS_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_ENVIRONMENT_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_ENVIRONMENT_DLG_CMD")
		{
			openOrBringToFront(ENVIRONMENT_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_FOG_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_FOG_DLG_CMD")
		{
			openOrBringToFront(FOG_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_GENERAL_PROPERTIES_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_GENERAL_PROPERTIES_DLG_CMD")
		{
			toggle(GENERAL_PROPERTIES_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_GRAPH_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_GRAPH_DLG_CMD")
		{
			toggle(GRAPH_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_TRANSFORMATIONS_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_TRANSFORMATIONS_DLG_CMD")
		{
			toggle(TRANSFORMATIONS_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_COLLECTION_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_COLLECTION_DLG_CMD")
		{
			toggle(COLLECTION_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_CAMERA_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_CAMERA_DLG_CMD")
		{
			toggle(CAMERA_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_LIGHT_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_LIGHT_DLG_CMD")
		{
			toggle(LIGHT_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_MIRROR_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_MIRROR_DLG_CMD")
		{
			toggle(MIRROR_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_COLLISION_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_COLLISION_DLG_CMD")
		{
			toggle(COLLISION_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_SELECTION_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_SELECTION_DLG_CMD")
		{
			toggle(SELECTION_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_SHAPE_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_SHAPE_DLG_CMD")
		{
			toggle(SHAPE_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_PROXIMITY_SENSOR_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_PROXIMITY_SENSOR_DLG_CMD")
		{
			toggle(PROXIMITY_SENSOR_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_VISION_SENSOR_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_VISION_SENSOR_DLG_CMD")
		{
			toggle(VISION_SENSOR_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_MILL_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_MILL_DLG_CMD")
		{
			toggle(MILL_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_FORCE_SENSOR_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_FORCE_SENSOR_DLG_CMD")
		{
			toggle(FORCE_SENSOR_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_LUA_SCRIPT_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_LUA_SCRIPT_DLG_CMD")
		{
			toggle(LUA_SCRIPT_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_SETTINGS_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_SETTINGS_DLG_CMD")
		{
			toggle(SETTINGS_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_DUMMY_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_DUMMY_DLG_CMD")
		{
			toggle(DUMMY_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_DISTANCE_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_DISTANCE_DLG_CMD")
		{
			toggle(DISTANCE_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_JOINT_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_JOINT_DLG_CMD")
		{
			toggle(JOINT_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_ENVIRONMENT_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_ENVIRONMENT_DLG_CMD")
		{
			toggle(ENVIRONMENT_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_IK_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_IK_DLG_CMD")
		{
			toggle(IK_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_DYNAMICS_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_DYNAMICS_DLG_CMD")
		{
			toggle(DYNAMICS_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_CONSTRAINT_SOLVER_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_CONSTRAINT_SOLVER_DLG_CMD")
		{
			toggle(CONSTRAINT_SOLVER_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_SIMULATION_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_SIMULATION_DLG_CMD")
		{
			toggle(SIMULATION_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_AVI_RECORDER_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_AVI_RECORDER_DLG_CMD")
		{
			toggle(AVI_RECORDER_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_BUTTON_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_BUTTON_DLG_CMD")
		{
			toggle(BUTTON_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_CUSTOM_UI_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_CUSTOM_UI_DLG_CMD")
		{
			if ( (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped() )
			{
				App::uiThread->addStatusLine(IDSNS_INITIALIZING_USER_INTERFACE_EDIT_MODE);
				App::ct->objCont->enterEditMode(-1,BUTTON_EDIT_MODE);
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
			else
			{
				if (App::ct->objCont->getEditModeType()==BUTTON_EDIT_MODE)
				{
					App::uiThread->addStatusLine(IDSNS_ENDING_EDIT_MODE_AND_APPLYING_CHANGES);
					App::ct->objCont->endEditMode(false);
					App::uiThread->addStatusLine(IDSNS_DONE);
				}
			}
		}
		return(true);
	}
	if (commandID==TOGGLE_SHAPE_EDITION_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_SHAPE_EDITION_DLG_CMD")
		{
			if ( (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped() )
			{
				CShape* it=NULL;
				if (App::ct->objCont->getSelSize()>=1)
					it=App::ct->objCont->getShape(App::ct->objCont->getSelID(App::ct->objCont->getSelSize()-1));
				if (it!=NULL)
				{
					if (it->isGrouping())
						App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_SHAPE_EDIT_MODE),strTranslate(IDS_SHAPE_IS_NOT_SIMPLE_FOR_EDITION_INFO_MESSAGE),VMessageBox::OKELI);
					else
					{
						bool goOn=true;
						if (goOn&&(it->geomData->geomInfo->isPure()))
							goOn=(VMessageBox::REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_SHAPE_EDIT_MODE),strTranslate(IDS_SHAPE_IS_PURE_PRIMITIVE_INFO_MESSAGE),VMessageBox::YES_NO));
						if (goOn)
						{
							App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************

							App::uiThread->addStatusLine(IDSNS_STARTING_TRIANGLE_EDIT_MODE);
							App::ct->objCont->enterEditMode(it->getID(),TRIANGLE_EDIT_MODE);
							App::uiThread->addStatusLine(IDSNS_DONE);

							CSPage* thePage=App::ct->oglSurface->pageCont->getPage(App::ct->oglSurface->pageCont->getActivePageIndex());
							if (thePage!=NULL)
							{
								CSView* theView=thePage->getView(0);
								if (theView!=NULL)
								{
									CCamera* theCamera=App::ct->objCont->getCamera(theView->getLinkedObjectID());
									if (theCamera!=NULL)
									{
										int viewSize[2];
										theView->getViewSize(viewSize);
										theCamera->frameSceneOrSelectedObjects(float(viewSize[0])/float(viewSize[1]),theView->getPerspectiveDisplay(),NULL,true,true,0.7f);
										theCamera->setFogTimer(4.0f);
									}
								}
							}
						}
					}
				}
			}
			else
			{
				if (App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)
				{
					WORD res=App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_SHAPE_EDIT_MODE),strTranslate(IDSN_DO_YOU_WANT_TO_APPLY_THE_CHANGES),VMessageBox::YES_NO_CANCEL);
					if (res!=VMessageBox::REPLY_CANCEL)
					{
						if (res==VMessageBox::REPLY_YES)
							App::uiThread->addStatusLine(IDSNS_ENDING_EDIT_MODE_AND_APPLYING_CHANGES);
						else
							App::uiThread->addStatusLine(IDSNS_ENDING_EDIT_MODE_AND_CANCELLING_CHANGES);
						App::ct->objCont->endEditMode(res==VMessageBox::REPLY_NO);
						if (res==VMessageBox::REPLY_YES)
							App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
						App::uiThread->addStatusLine(IDSNS_DONE);
					}
				}
			}
		}
		return(true);
	}
	if (commandID==TOGGLE_PATH_EDITION_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_PATH_EDITION_DLG_CMD")
		{
			if ( (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped() )
			{
				CPath* it=NULL;
				if (App::ct->objCont->getSelSize()>=1)
					it=App::ct->objCont->getPath(App::ct->objCont->getSelID(App::ct->objCont->getSelSize()-1));
				if (it!=NULL)
				{
					App::uiThread->addStatusLine(IDSNS_STARTING_PATH_EDIT_MODE);

					App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************

					App::ct->objCont->enterEditMode(it->getID(),PATH_EDIT_MODE);
					App::uiThread->addStatusLine(IDSNS_DONE);

					CSPage* thePage=App::ct->oglSurface->pageCont->getPage(App::ct->oglSurface->pageCont->getActivePageIndex());
					if (thePage!=NULL)
					{
						CSView* theView=thePage->getView(0);
						if (theView!=NULL)
						{
							CCamera* theCamera=App::ct->objCont->getCamera(theView->getLinkedObjectID());
							if (theCamera!=NULL)
							{
								int viewSize[2];
								theView->getViewSize(viewSize);
								theCamera->frameSceneOrSelectedObjects(float(viewSize[0])/float(viewSize[1]),theView->getPerspectiveDisplay(),NULL,true,true,0.7f);
								theCamera->setFogTimer(4.0f);
							}
						}
					}
				}
			}
			else
			{
				if (App::ct->objCont->getEditModeType()==PATH_EDIT_MODE)
				{
					WORD res=App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_PATH_EDIT_MODE),strTranslate(IDSN_DO_YOU_WANT_TO_APPLY_THE_CHANGES),VMessageBox::YES_NO_CANCEL);
					if (res!=VMessageBox::REPLY_CANCEL)
					{
						if (res==VMessageBox::REPLY_YES)
							App::uiThread->addStatusLine(IDSNS_ENDING_EDIT_MODE_AND_APPLYING_CHANGES);
						else
							App::uiThread->addStatusLine(IDSNS_ENDING_EDIT_MODE_AND_CANCELLING_CHANGES);
						App::ct->objCont->endEditMode(res==VMessageBox::REPLY_NO);
						if (res==VMessageBox::REPLY_YES)
							App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
						App::uiThread->addStatusLine(IDSNS_DONE);
					}
				}
			}
		}
		return(true);
	}

	if (commandID==TOGGLE_HIERARCHY_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_HIERARCHY_DLG_CMD")
		{
			toggle(HIERARCHY_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_BROWSER_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_BROWSER_DLG_CMD")
		{
			toggle(BROWSER_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_LAYERS_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_LAYERS_DLG_CMD")
		{
			toggle(LAYERS_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_DYNAMICS_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_DYNAMICS_DLG_CMD")
		{
			toggle(DYNAMICS_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_PATH_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_PATH_DLG_CMD")
		{
			toggle(PATH_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_PATH_PLANNING_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_PATH_PLANNING_DLG_CMD")
		{
			toggle(PATH_PLANNING_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_OBJECT_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_OBJECT_DLG_CMD")
		{
			toggle(OBJECT_DLG);
		}
		return(true);
	}
	if (commandID==TOGGLE_CALCULATION_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_CALCULATION_DLG_CMD")
		{
			toggle(CALCULATION_DLG);
		}
		return(true);
	}
	if (commandID==OPEN_CALCULATION_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("OPEN_CALCULATION_DLG_CMD")
		{
			openOrBringToFront(CALCULATION_DLG);
		}
		return(true);
	}
	return(false);
}
