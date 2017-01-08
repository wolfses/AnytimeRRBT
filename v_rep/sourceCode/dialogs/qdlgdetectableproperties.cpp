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
#include "qdlgdetectableproperties.h"
#include "ui_qdlgdetectableproperties.h"
#include "v_rep_internal.h"
#include "App.h"

CQDlgDetectableProperties::CQDlgDetectableProperties(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgDetectableProperties)
{
    ui->setupUi(this);
	initializationEvent();
}

CQDlgDetectableProperties::~CQDlgDetectableProperties()
{
    delete ui;
}

void CQDlgDetectableProperties::cancelEvent()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgDetectableProperties::okEvent()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgDetectableProperties::initializationEvent()
{
	defaultDialogInitializationRoutine();
}

void CQDlgDetectableProperties::refresh()
{
	ui->qqCapacitive->setChecked((objectProperties&sim_objectspecialproperty_detectable_capacitive)!=0);
	ui->qqInductive->setChecked((objectProperties&sim_objectspecialproperty_detectable_inductive)!=0);
	ui->qqInfrared->setChecked((objectProperties&sim_objectspecialproperty_detectable_infrared)!=0);
	ui->qqLaser->setChecked((objectProperties&sim_objectspecialproperty_detectable_laser)!=0);
	ui->qqUltrasonic->setChecked((objectProperties&sim_objectspecialproperty_detectable_ultrasonic)!=0);
}

void CQDlgDetectableProperties::on_qqUltrasonic_clicked()
{
	objectProperties^=sim_objectspecialproperty_detectable_ultrasonic;
	refresh();
}

void CQDlgDetectableProperties::on_qqInductive_clicked()
{
	objectProperties^=sim_objectspecialproperty_detectable_inductive;
	refresh();
}

void CQDlgDetectableProperties::on_qqInfrared_clicked()
{
	objectProperties^=sim_objectspecialproperty_detectable_infrared;
	refresh();
}

void CQDlgDetectableProperties::on_qqCapacitive_clicked()
{
	objectProperties^=sim_objectspecialproperty_detectable_capacitive;
	refresh();
}

void CQDlgDetectableProperties::on_qqLaser_clicked()
{
	objectProperties^=sim_objectspecialproperty_detectable_laser;
	refresh();
}

void CQDlgDetectableProperties::on_qqOkCancel_accepted()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgDetectableProperties::on_qqOkCancel_rejected()
{
	defaultModalDialogEndRoutine(false);
}
