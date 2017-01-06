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
#include "ToolDlgWrapper.h"
#include "Global.h"
#include "App.h"
#include "qdlgsettings.h"
#include "qdlglayers.h"
#include "qdlgavirecorder.h"
#include "qdlgselection.h"
#include "qdlgsimulation.h"
#include "qdlgcommonproperties.h"
#include "qdlgenvironment.h"
#include "qdlgcolor.h"
#include "qdlgmaterial.h"
#include "qdlgfog.h"
#include "qdlgcollections.h"
#include "qdlgcollisions.h"
#include "qdlgdistances.h"
#include "qdlgscripts.h"
#include "qdlgdynamics.h"
#include "qdlgcameras.h"
#include "qdlglights.h"
#include "qdlgdummies.h"
#include "qdlgforcesensors.h"
#include "qdlgconstraintsolver.h"
#include "qdlgvisionsensors.h"
#include "qdlgfilters.h"
#include "qdlgshapes.h"
#include "qdlgtextures.h"
#include "qdlggeometry.h"
#include "qdlgshapedyn.h"
#include "qdlgproximitysensors.h"
#include "qdlgdetectionvolume.h"
#include "qdlgmills.h"
#include "qdlgobjectdialogcontainer.h"
#include "qdlgcalcdialogcontainer.h"
#include "qdlgshapeeditioncontainer.h"
#include "qdlgjoints.h"
#include "qdlgjointdyn.h"
#include "qdlggraphs.h"
#include "qdlgik.h"
#include "qdlguidialogcontainer.h"
#include "qdlgpathshaping.h"
#include "qdlgpathedit.h"
#include "qdlgtranslationrotationcontainer.h"

CToolDlgWrapper::CToolDlgWrapper(int dlgType,int resID)
{	// The dialog is not created when calling the constructor! 
	// It is created/destroyed in setVisible
	dialogType=dlgType;
	resourceID=resID;
	toolDialog=NULL;
	position[0]=42000; // indicates: uninitialized
	position[1]=42000;
}

CToolDlgWrapper::~CToolDlgWrapper()
{
	if (toolDialog!=NULL)
	{
		delete toolDialog;
	}
}

void CToolDlgWrapper::setRefreshDegree(int d)
{
	if (toolDialog!=NULL)
		toolDialog->setRefreshDegree(d);
}
void CToolDlgWrapper::refresh()
{
	if (toolDialog!=NULL)
		toolDialog->refresh();
}

void CToolDlgWrapper::showDialogButDontOpenIt()
{
	if (toolDialog!=NULL)
	{
		toolDialog->showDialog(true);
#ifdef LIN_VREP
		toolDialog->bringDialogToTop(); // otherwise the dialog opens behind the last dialog on Linux
#endif
	}
}

void CToolDlgWrapper::hideDialogButDontCloseIt()
{
	if (toolDialog!=NULL)
		toolDialog->showDialog(false);
}

void CToolDlgWrapper::visibleInstanceAboutToSwitch()
{
	if (toolDialog!=NULL)
	{
		if (toolDialog->doesInstanceSwitchRequireDestruction())
			setVisible(false,NULL);
	}
}
void CToolDlgWrapper::setVisible(bool visible,QWidget* parentWindow)
{
	if ( (toolDialog!=NULL)&&(!visible) )
	{
		getPosition(NULL); // To save the current position
		delete toolDialog;
		toolDialog=NULL;
	}
	if (visible)
	{
		if (toolDialog!=NULL)
			toolDialog->bringDialogToTop();
		else
		{
			if (dialogType==SETTINGS_DLG)
				toolDialog=new CQDlgSettings(App::mainWindow);
			if (dialogType==LAYERS_DLG)
				toolDialog=new CQDlgLayers(App::mainWindow);
			if (dialogType==AVI_RECORDER_DLG)
				toolDialog=new CQDlgAviRecorder(App::mainWindow);
			if (dialogType==SELECTION_DLG)
				toolDialog=new CQDlgSelection(App::mainWindow);
			if (dialogType==SIMULATION_DLG)
				toolDialog=new CQDlgSimulation(App::mainWindow);
			if (dialogType==ENVIRONMENT_DLG)
				toolDialog=new CQDlgEnvironment(App::mainWindow);
			if (dialogType==COLOR_DLG)
				toolDialog=new CQDlgColor(App::mainWindow);
			if (dialogType==MATERIAL_DLG)
				toolDialog=new CQDlgMaterial(App::mainWindow);
			if (dialogType==FOG_DLG)
				toolDialog=new CQDlgFog(App::mainWindow);
			if (dialogType==COLLECTION_DLG)
				toolDialog=new CQDlgCollections(App::mainWindow);
			if (dialogType==LUA_SCRIPT_DLG)
				toolDialog=new CQDlgScripts(App::mainWindow);
			if (dialogType==VISION_SENSOR_FILTER_DLG)
				toolDialog=new CQDlgFilters(App::mainWindow);
			if (dialogType==TEXTURE_DLG)
				toolDialog=new CQDlgTextures(App::mainWindow);
			if (dialogType==GEOMETRY_DLG)
				toolDialog=new CQDlgGeometry(App::mainWindow);
			if (dialogType==SHAPE_DYN_DLG)
				toolDialog=new CQDlgShapeDyn(App::mainWindow);
			if (dialogType==DETECTION_VOLUME_DLG)
				toolDialog=new CQDlgDetectionVolume(App::mainWindow);
			if (dialogType==SHAPE_EDITION_DLG)
				toolDialog=new CQDlgShapeEditionContainer(App::mainWindow);
			if (dialogType==OBJECT_DLG)
				toolDialog=new CQDlgObjectDialogContainer(App::mainWindow);
			if (dialogType==CALCULATION_DLG)
				toolDialog=new CQDlgCalcDialogContainer(App::mainWindow);
			if (dialogType==JOINT_DYN_DLG)
				toolDialog=new CQDlgJointDyn(App::mainWindow);
			if (dialogType==CUSTOM_UI_DLG)
				toolDialog=new CQDlgUiDialogContainer(App::mainWindow);
			if (dialogType==TRANSLATION_ROTATION_DLG)
				toolDialog=new CQDlgTranslationRotationContainer(App::mainWindow);
			if (dialogType==PATH_SHAPING_DLG)
				toolDialog=new CQDlgPathShaping(App::mainWindow);
			if (dialogType==PATH_EDITION_DLG)
				toolDialog=new CQDlgPathEdit(App::mainWindow);

			if (toolDialog!=NULL)
			{
				if (position[0]==42000)
				{ // first time we open that dialog
#ifdef WIN_VREP
					position[0]=App::mainWindow->geometry().width()+App::mainWindow->geometry().x()-toolDialog->geometry().width()-10; // -10 because when in full-screen, the main window doesn't have a frame and things are a little bit different!!
					position[1]=App::mainWindow->geometry().y()+100;
#endif
#ifdef MAC_VREP
					position[0]=400; // doesn't work like under windows somehow...
					position[1]=200;
#endif
#ifdef LIN_VREP
					position[0]=App::mainWindow->geometry().width()+App::mainWindow->geometry().x()-toolDialog->geometry().width()-10; // -10 because when in full-screen, the main window doesn't have a frame and things are a little bit different!!
					position[1]=App::mainWindow->geometry().y()+100;
#endif
				}
				setPosition(position[0],position[1]);
				toolDialog->setRefreshDegree(4);
				toolDialog->refresh();
				toolDialog->showDialog(true);
#ifdef LIN_VREP
				toolDialog->bringDialogToTop(); // otherwise the dialog opens behind the last dialog on Linux
#endif
			}
		}
	}
}

void CToolDlgWrapper::destroyIfDestructionNeeded()
{
	if (toolDialog!=NULL)
	{
		if (toolDialog->needsDestruction())
		{
			getPosition(NULL); // To save the current position
			delete toolDialog;
			toolDialog=NULL;
		}
	}
}

bool CToolDlgWrapper::getVisible()
{
	return(toolDialog!=NULL);
}
void CToolDlgWrapper::getPosition(int pos[2])
{
	if (toolDialog!=NULL)
		toolDialog->getDialogPositionAndSize(position,NULL);
	if (pos!=NULL)
	{
		pos[0]=position[0];
		pos[1]=position[1];
	}
}

void CToolDlgWrapper::setPosition(int x,int y)
{
	position[0]=x;
	position[1]=y;
	if (toolDialog!=NULL)
		toolDialog->setDialogPositionAndSize(position,NULL);
}

int CToolDlgWrapper::getDialogType()
{
	return(dialogType);
}

CDlgEx* CToolDlgWrapper::getDialog()
{
	return(toolDialog);
}
