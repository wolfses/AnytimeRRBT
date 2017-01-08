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
#include "qdlgdatastreamselection.h"
#include "ui_qdlgdatastreamselection.h"
#include "GV.h"
#include "Tt.h"
#include "Global.h"
#include "GraphingRoutines.h"
#include <QPushButton>
#include "v_repStrings.h"
#include "App.h"

CQDlgDataStreamSelection::CQDlgDataStreamSelection(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgDataStreamSelection)
{
    ui->setupUi(this);
	inRefreshRoutine=false;
	first=true;
	box1Id=-1;
	box2Id=-1;
}

CQDlgDataStreamSelection::~CQDlgDataStreamSelection()
{
    delete ui;
}

void CQDlgDataStreamSelection::cancelEvent()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgDataStreamSelection::okEvent()
{ // we want the user to click "ok"
//	defaultModalDialogEndRoutine(true);
}

void CQDlgDataStreamSelection::refresh()
{
	inRefreshRoutine=true;

	if (first)
	{
		first=false;
		ui->qqTypeCombo->clear();

		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_ALL_X_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_ALL_Y_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_ALL_Z_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_ALL_X_REL);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_ALL_Y_REL);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_ALL_Z_REL);

		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_ALL_VEL_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_ALL_XVEL_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_ALL_YVEL_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_ALL_ZVEL_ABS);


		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_ALL_ALPHA_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_ALL_BETA_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_ALL_GAMMA_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_ALL_ALPHA_REL);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_ALL_BETA_REL);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_ALL_GAMMA_REL);

		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_ALL_ANGULAR_VEL_ABS);

		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_PROXSENSOR_X_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_PROXSENSOR_Y_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_PROXSENSOR_Z_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_PROXSENSOR_X_REL);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_PROXSENSOR_Y_REL);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_PROXSENSOR_Z_REL);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_PROXSENSOR_X_REL_TO_SENSOR);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_PROXSENSOR_Y_REL_TO_SENSOR);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_PROXSENSOR_Z_REL_TO_SENSOR);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_PROXSENSOR_DIST);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_PROXSENSOR_AZIMUTH);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_PROXSENSOR_ELEVATION);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_PROXSENSOR_DETECTION_STATE);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_PROXSENSOR_CALCULATION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_NOOBJECT_PROXSENSOR_OVERALL_CALCULATION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_VISIONSENSOR_MIN_RED);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_VISIONSENSOR_MIN_GREEN);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_VISIONSENSOR_MIN_BLUE);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_VISIONSENSOR_MIN_INTENSITY);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_VISIONSENSOR_MIN_DEPTH);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_VISIONSENSOR_MAX_RED);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_VISIONSENSOR_MAX_GREEN);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_VISIONSENSOR_MAX_BLUE);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_VISIONSENSOR_MAX_INTENSITY);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_VISIONSENSOR_MAX_DEPTH);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_VISIONSENSOR_AVG_RED);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_VISIONSENSOR_AVG_GREEN);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_VISIONSENSOR_AVG_BLUE);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_VISIONSENSOR_AVG_INTENSITY);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_VISIONSENSOR_AVG_DEPTH);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_VISIONSENSOR_DETECTION_STATE);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_VISIONSENSOR_CALCULATION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_NOOBJECT_VISIONSENSOR_OVERALL_CALCULATION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_FORCESENSOR_FORCE_LENGTH);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_FORCESENSOR_FORCE_X);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_FORCESENSOR_FORCE_Y);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_FORCESENSOR_FORCE_Z);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_FORCESENSOR_TORQUE_LENGTH);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_FORCESENSOR_TORQUE_X);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_FORCESENSOR_TORQUE_Y);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_FORCESENSOR_TORQUE_Z);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_FORCESENSOR_POSITION_ERROR);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_FORCESENSOR_ORIENTATION_ERROR);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_FORCESENSOR_BROKEN);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_MILL_CUT_SURFACE);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_MILL_CUT_VOLUME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_MILL_CALCULATION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_NOOBJECT_MILL_OVERALL_CUT_SURFACE);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_NOOBJECT_MILL_OVERALL_CUT_VOLUME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_NOOBJECT_MILL_OVERALL_CALCULATION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_JOINT_POSITION);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_JOINT_VELOCITY);


		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_JOINT_DYN_POSITION_ERROR);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_JOINT_DYN_ORIENTATION_ERROR);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_JOINT_DYN_FORCE_OR_TORQUE);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_PATH_POSITION);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_PATH_POSITION_LINEAR_COMPONENT);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_3DOBJECT_PATH_POSITION_ANGULAR_COMPONENT);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_COLLISION_BOOLEAN);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_NOOBJECT_COLLISION_QUANTITY);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_COLLISION_CALCULATION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_NOOBJECT_COLLISION_OVERALL_CALCULATION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_DISTANCE_DIST);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_DISTANCE_SEG1_X_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_DISTANCE_SEG1_Y_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_DISTANCE_SEG1_Z_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_DISTANCE_SEG2_X_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_DISTANCE_SEG2_Y_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_DISTANCE_SEG2_Z_ABS);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_DISTANCE_SEG1_X_REL);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_DISTANCE_SEG1_Y_REL);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_DISTANCE_SEG1_Z_REL);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_DISTANCE_SEG2_X_REL);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_DISTANCE_SEG2_Y_REL);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_DISTANCE_SEG2_Z_REL);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_DISTANCE_CALCULATION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_NOOBJECT_DISTANCE_OVERALL_CALCULATION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_IK_CALCULATION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_NOOBJECT_IK_OVERALL_CALCULATION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_GCS_CALCULATION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_NOOBJECT_GCS_OVERALL_CALCULATION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_NOOBJECT_DYNAMICS_OVERALL_CALCULATION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_NOOBJECT_WORK_THREADS_EXECUTION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_NOOBJECT_RENDERING_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_NOOBJECT_MAINSCRIPT_EXECUTION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_NOOBJECT_SIMULATIONPASS_EXECUTION_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_NOOBJECT_TIME);
		addElementToCombobox(ui->qqTypeCombo,GRAPH_NOOBJECT_USER_DEFINED);
	}

	ui->qqItemCombo->clear();
	ui->qqItemCombo->addItem(strTranslate(IDSN_NONE),QVariant(-1));

	int w=ui->qqTypeCombo->itemData(ui->qqTypeCombo->currentIndex()).toInt();
	int counter=0;
	while(true)
	{
		int objID;
		if (CGraphingRoutines::loopThroughAllAndGetObjectsFromGraphCategory(counter,w,objID))
		{
			std::string tmp=IDS_ERROR;
			CGraphingRoutines::loopThroughAllAndGetGraphObjectName(w,objID,tmp);
			ui->qqItemCombo->addItem(tmp.c_str(),QVariant(counter));
			counter++;
		}
		else
			break;
	}

	ui->qqOkCancel->button(QDialogButtonBox::Ok)->setEnabled(false);

	inRefreshRoutine=false;
}

void CQDlgDataStreamSelection::addElementToCombobox(QComboBox* box,int dataType)
{
	std::string tmp;
	CGraphingRoutines::loopThroughAllAndGetDataName(dataType,tmp);
	box->addItem(tmp.c_str(),QVariant(dataType));
}

void CQDlgDataStreamSelection::on_qqTypeCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inRefreshRoutine)
		{
			refresh();
		}
	}
}

void CQDlgDataStreamSelection::on_qqItemCombo_currentIndexChanged(int index)
{
	if (!inRefreshRoutine)
	{
		ui->qqOkCancel->button(QDialogButtonBox::Ok)->setEnabled(ui->qqItemCombo->itemData(ui->qqItemCombo->currentIndex()).toInt()!=-1);
	}
}

void CQDlgDataStreamSelection::on_qqOkCancel_accepted()
{
	box1Id=ui->qqTypeCombo->itemData(ui->qqTypeCombo->currentIndex()).toInt();
	box2Id=ui->qqItemCombo->itemData(ui->qqItemCombo->currentIndex()).toInt();
	defaultModalDialogEndRoutine(true);
}

void CQDlgDataStreamSelection::on_qqOkCancel_rejected()
{
	defaultModalDialogEndRoutine(false);
}
