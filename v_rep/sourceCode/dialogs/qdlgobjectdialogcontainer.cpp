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
#include "qdlgobjectdialogcontainer.h"
#include "ui_qdlgobjectdialogcontainer.h"
#include "App.h"
#include "v_repStrings.h"
#include "qdlgmirrors.h"
#include "qdlgcameras.h"
#include "qdlglights.h"
#include "qdlgdummies.h"
#include "qdlgforcesensors.h"
#include "qdlgvisionsensors.h"
#include "qdlgshapes.h"
#include "qdlgproximitysensors.h"
#include "qdlgmills.h"
#include "qdlgjoints.h"
#include "qdlggraphs.h"
#include "qdlgdetectionvolume.h"
#include "qdlgshapedyn.h"
#include "qdlgpaths.h"
#include "qdlgpathshaping.h"

#include "qdlgcommonproperties.h"

#define TOP_BORDER_WIDTH 30

CQDlgObjectDialogContainer::CQDlgObjectDialogContainer(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgObjectDialogContainer)
{
    ui->setupUi(this);
	pageDlgs[0]=new CQDlgDummies();
	originalHeights[0]=pageDlgs[0]->size().height();
	ui->qqObjectProp->setText(strTranslate(IDSN_DUMMY));
	objTypeDlg=sim_object_dummy_type;

	pageDlgs[1]=new CQDlgCommonProperties();
	originalHeights[1]=pageDlgs[1]->size().height();

	currentPage=0;
	desiredPage=0;
	bl=new QVBoxLayout();
	bl->setContentsMargins(0,TOP_BORDER_WIDTH,0,0);
	setLayout(bl);
	bl->addWidget(pageDlgs[0]);
	bl->addWidget(pageDlgs[1]);
	pageDlgs[1]->setVisible(false);

	QSize s(pageDlgs[currentPage]->size());
	s.setHeight(originalHeights[currentPage]+TOP_BORDER_WIDTH);
	setFixedSize(s);
/*
#ifndef WIN_VREP
	// Since Qt5, we have problems on Linux (resizing-->dlg shifts in position) and Mac (resising-->ugly glitch)
	// In that case we keep constant-size dialogs.
	if (QT_VERSION>=0x050000)
	{
		s.setHeight(531+TOP_BORDER_WIDTH);
		setFixedSize(s);
	}
#endif
*/
}

CQDlgObjectDialogContainer::~CQDlgObjectDialogContainer()
{
	if (App::mainWindow->dlgCont->isVisible(JOINT_DYN_DLG))
		App::mainWindow->dlgCont->toggle(JOINT_DYN_DLG);
	if (App::mainWindow->dlgCont->isVisible(VISION_SENSOR_FILTER_DLG))
		App::mainWindow->dlgCont->toggle(VISION_SENSOR_FILTER_DLG);
	if (App::mainWindow->dlgCont->isVisible(DETECTION_VOLUME_DLG))
		App::mainWindow->dlgCont->toggle(DETECTION_VOLUME_DLG);
	if (App::mainWindow->dlgCont->isVisible(SHAPE_DYN_DLG))
		App::mainWindow->dlgCont->toggle(SHAPE_DYN_DLG);
	if (App::mainWindow->dlgCont->isVisible(PATH_SHAPING_DLG))
		App::mainWindow->dlgCont->toggle(PATH_SHAPING_DLG);
	delete ui;
}

void CQDlgObjectDialogContainer::refresh()
{
	ui->qqObjectProp->setChecked(desiredPage==0);
	ui->qqCommonProp->setChecked(desiredPage==1);

	if (desiredPage!=currentPage)
	{
		pageDlgs[currentPage]->setVisible(false);
		currentPage=desiredPage;
		pageDlgs[currentPage]->setVisible(true);

		QSize s(pageDlgs[currentPage]->size());
		s.setHeight(originalHeights[currentPage]+TOP_BORDER_WIDTH);

#ifdef MAC_VREP
		if (QT_VERSION>=0x050000)
		{ // Since Qt5, we have problems on Mac (resising-->ugly glitch)
			setVisible(false);
			setFixedSize(s);
			setVisible(true);
		}
		else
			setFixedSize(s);
#else
		setFixedSize(s);
#endif

/*
#ifdef WIN_VREP
		// Since Qt5, we have problems on Linux (resizing-->dlg shifts in position) and Mac (resising-->ugly glitch)
		// In that case we keep constant-size dialogs.
		QSize s(pageDlgs[currentPage]->size());
		s.setHeight(originalHeights[currentPage]+TOP_BORDER_WIDTH);
		setFixedSize(s);
#else
		if (QT_VERSION<0x050000)
		{
			QSize s(pageDlgs[currentPage]->size());
			s.setHeight(originalHeights[currentPage]+TOP_BORDER_WIDTH);
			setFixedSize(s);
		}
#endif
*/
	}

	C3DObject* sel=App::ct->objCont->getLastSelection();
	int editMode=App::ct->objCont->getEditModeType();
	if (sel!=NULL)
	{
		int t=sel->getObjectType();
		std::string title;
		if (t==sim_object_mirror_type)
			title=strTranslate(IDSN_MIRROR);
		if (t==sim_object_shape_type)
			title=strTranslate(IDSN_SHAPE);
		if (t==sim_object_joint_type)
			title=strTranslate(IDSN_JOINT);
		if (t==sim_object_camera_type)
			title=strTranslate(IDSN_CAMERA);
		if (t==sim_object_dummy_type)
			title=strTranslate(IDSN_DUMMY);
		if (t==sim_object_proximitysensor_type)
			title=strTranslate(IDSN_PROXIMITY_SENSOR);
		if (t==sim_object_path_type)
			title=strTranslate(IDSN_PATH);
		if (t==sim_object_visionsensor_type)
			title=strTranslate(IDSN_VISION_SENSOR);
		if (t==sim_object_volume_type)
			title=strTranslate(IDSN_VOLUME);
		if (t==sim_object_mill_type)
			title=strTranslate(IDSN_MILL);
		if (t==sim_object_forcesensor_type)
			title=strTranslate(IDSN_FORCE_SENSOR);
		if (t==sim_object_light_type)
			title=strTranslate(IDSN_LIGHT);
		if (t==sim_object_graph_type)
			title=strTranslate(IDSN_GRAPH);
		ui->qqObjectProp->setText(title.c_str());
	}
	else
	{
		if (editMode==PATH_EDIT_MODE)
			ui->qqObjectProp->setText(strTranslate(IDSN_GRAPH));
		if (editMode&SHAPE_EDIT_MODE)
			ui->qqObjectProp->setText(strTranslate(IDSN_SHAPE));
	}

	if ((currentPage==0)&&((sel!=NULL)||(editMode==PATH_EDIT_MODE)||(editMode&SHAPE_EDIT_MODE)))
	{ // object properties
		int t=-1;
		if (sel!=NULL)
			t=sel->getObjectType();
		else
		{
			if (editMode==PATH_EDIT_MODE)
				t=sim_object_path_type;
			if (editMode&SHAPE_EDIT_MODE)
				t=sim_object_shape_type;
		}
		if ((t!=objTypeDlg)&&(t!=-1))
		{
			objTypeDlg=t;
			bl->removeWidget(pageDlgs[currentPage]);
			delete pageDlgs[currentPage];
			if (objTypeDlg==sim_object_mirror_type)
				pageDlgs[currentPage]=new CQDlgMirrors();
			if (objTypeDlg==sim_object_shape_type)
				pageDlgs[currentPage]=new CQDlgShapes();
			if (objTypeDlg==sim_object_joint_type)
				pageDlgs[currentPage]=new CQDlgJoints();
			if (objTypeDlg==sim_object_camera_type)
				pageDlgs[currentPage]=new CQDlgCameras();
			if (objTypeDlg==sim_object_dummy_type)
				pageDlgs[currentPage]=new CQDlgDummies();
			if (objTypeDlg==sim_object_proximitysensor_type)
				pageDlgs[currentPage]=new CQDlgProximitySensors();
			if (objTypeDlg==sim_object_graph_type)
				pageDlgs[currentPage]=new CQDlgGraphs();
			if (objTypeDlg==sim_object_visionsensor_type)
				pageDlgs[currentPage]=new CQDlgVisionSensors();
			if (objTypeDlg==sim_object_path_type)
				pageDlgs[currentPage]=new CQDlgPaths();
			if (objTypeDlg==sim_object_mill_type)
				pageDlgs[currentPage]=new CQDlgMills();
			if (objTypeDlg==sim_object_forcesensor_type)
				pageDlgs[currentPage]=new CQDlgForceSensors();
			if (objTypeDlg==sim_object_light_type)
				pageDlgs[currentPage]=new CQDlgLights();

			originalHeights[currentPage]=pageDlgs[currentPage]->size().height();

			bl->addWidget(pageDlgs[currentPage]);

			QSize s(pageDlgs[currentPage]->size());
			s.setHeight(originalHeights[currentPage]+TOP_BORDER_WIDTH);

#ifdef MAC_VREP
			if (QT_VERSION>=0x050000)
			{ // Since Qt5, we have problems on Mac (resising-->ugly glitch)
				setVisible(false);
				setFixedSize(s);
				setVisible(true);
			}
			else
				setFixedSize(s);
#else
			setFixedSize(s);
#endif

/*
#ifdef WIN_VREP
		// Since Qt5, we have problems on Linux (resizing-->dlg shifts in position) and Mac (resising-->ugly glitch)
		// In that case we keep constant-size dialogs.
		QSize s(pageDlgs[currentPage]->size());
		s.setHeight(originalHeights[currentPage]+TOP_BORDER_WIDTH);
		setFixedSize(s);
#else
		if (QT_VERSION<0x050000)
		{
			QSize s(pageDlgs[currentPage]->size());
			s.setHeight(originalHeights[currentPage]+TOP_BORDER_WIDTH);
			setFixedSize(s);
		}
#endif
*/
		}
	}

	if (currentPage==0)
	{
		if (objTypeDlg==sim_object_path_type)
		{
			if (CQDlgPathShaping::showWindow!=App::mainWindow->dlgCont->isVisible(PATH_SHAPING_DLG))
				App::mainWindow->dlgCont->toggle(PATH_SHAPING_DLG);
		}
		else
		{
			if (App::mainWindow->dlgCont->isVisible(PATH_SHAPING_DLG))
				App::mainWindow->dlgCont->toggle(PATH_SHAPING_DLG);
		}
		if (objTypeDlg==sim_object_shape_type)
		{
			if (CQDlgShapeDyn::showDynamicWindow!=App::mainWindow->dlgCont->isVisible(SHAPE_DYN_DLG))
				App::mainWindow->dlgCont->toggle(SHAPE_DYN_DLG);
		}
		else
		{
			if (App::mainWindow->dlgCont->isVisible(SHAPE_DYN_DLG))
				App::mainWindow->dlgCont->toggle(SHAPE_DYN_DLG);
		}
		if (objTypeDlg==sim_object_joint_type)
		{
			if (CQDlgJoints::showDynamicWindow!=App::mainWindow->dlgCont->isVisible(JOINT_DYN_DLG))
				App::mainWindow->dlgCont->toggle(JOINT_DYN_DLG);
		}
		else
		{
			if (App::mainWindow->dlgCont->isVisible(JOINT_DYN_DLG))
				App::mainWindow->dlgCont->toggle(JOINT_DYN_DLG);
		}
		if (objTypeDlg==sim_object_visionsensor_type)
		{
			if (CQDlgVisionSensors::showFilterWindow!=App::mainWindow->dlgCont->isVisible(VISION_SENSOR_FILTER_DLG))
				App::mainWindow->dlgCont->toggle(VISION_SENSOR_FILTER_DLG);
		}
		else
		{
			if (App::mainWindow->dlgCont->isVisible(VISION_SENSOR_FILTER_DLG))
				App::mainWindow->dlgCont->toggle(VISION_SENSOR_FILTER_DLG);
		}
		if ((objTypeDlg==sim_object_proximitysensor_type)||(objTypeDlg==sim_object_mill_type))
		{
			if (CQDlgDetectionVolume::showVolumeWindow!=App::mainWindow->dlgCont->isVisible(DETECTION_VOLUME_DLG))
				App::mainWindow->dlgCont->toggle(DETECTION_VOLUME_DLG);
		}
		else
		{
			if (App::mainWindow->dlgCont->isVisible(DETECTION_VOLUME_DLG))
				App::mainWindow->dlgCont->toggle(DETECTION_VOLUME_DLG);
		}
	}
	else
	{
		if (App::mainWindow->dlgCont->isVisible(JOINT_DYN_DLG))
			App::mainWindow->dlgCont->toggle(JOINT_DYN_DLG);
		if (App::mainWindow->dlgCont->isVisible(VISION_SENSOR_FILTER_DLG))
			App::mainWindow->dlgCont->toggle(VISION_SENSOR_FILTER_DLG);
		if (App::mainWindow->dlgCont->isVisible(DETECTION_VOLUME_DLG))
			App::mainWindow->dlgCont->toggle(DETECTION_VOLUME_DLG);
		if (App::mainWindow->dlgCont->isVisible(SHAPE_DYN_DLG))
			App::mainWindow->dlgCont->toggle(SHAPE_DYN_DLG);
		if (App::mainWindow->dlgCont->isVisible(PATH_SHAPING_DLG))
			App::mainWindow->dlgCont->toggle(PATH_SHAPING_DLG);
	}


	pageDlgs[currentPage]->refresh();
}

void CQDlgObjectDialogContainer::on_qqObjectProp_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		desiredPage=0;
		refresh();
	}
}

void CQDlgObjectDialogContainer::on_qqCommonProp_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		desiredPage=1;
		refresh();
	}
}
