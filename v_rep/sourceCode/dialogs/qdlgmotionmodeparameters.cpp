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
#include "qdlgmotionmodeparameters.h"
#include "ui_qdlgmotionmodeparameters.h"
#include "GV.h"
#include "Tt.h"
#include "App.h"

CQDlgMotionModeParameters::CQDlgMotionModeParameters(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgMotionModeParameters)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
}

CQDlgMotionModeParameters::~CQDlgMotionModeParameters()
{
    delete ui;
}

void CQDlgMotionModeParameters::cancelEvent()
{ // no cancel event allowed
	//defaultModalDialogEndRoutine(false);
}

void CQDlgMotionModeParameters::okEvent()
{ // no ok event allowed
	//defaultModalDialogEndRoutine(false);
}

void CQDlgMotionModeParameters::refresh()
{
	inMainRefreshRoutine=true;
	bool sel=App::ct->objCont->isLastSelectionAJoint();
	bool motionPartActive=false;
	CJoint* it=NULL;
	if (sel)
	{
		it=(CJoint*)App::ct->objCont->getLastSelection();
		motionPartActive=it->getJointMode()==sim_jointmode_motion;
	}
	ui->qqExplicitHandling->setEnabled(motionPartActive);
	ui->qqAcceleration->setEnabled(motionPartActive&&(!it->getUnlimitedAcceleration()));
	ui->qqTargetVelocity->setEnabled(motionPartActive);
	ui->qqInvert->setEnabled(motionPartActive);
	ui->qqInfiniteAcceleration->setEnabled(motionPartActive);

	ui->qqExplicitHandling->setChecked(motionPartActive&&it->getExplicitHandling());
	ui->qqInvert->setChecked(motionPartActive&&it->getInvertTargetVelocityAtLimits());
	ui->qqInfiniteAcceleration->setChecked(motionPartActive&&it->getUnlimitedAcceleration());
	if (sel)
	{
		if (it->getJointType()==sim_joint_prismatic_subtype)
		{
			ui->qqAcceleration->setText(gv::getLinearAccelStr(false,it->getMaxAcceleration()).c_str());
			ui->qqTargetVelocity->setText(gv::getLinearVelocityStr(true,it->getTargetVelocity()).c_str());
		}
		else
		{
			ui->qqAcceleration->setText(gv::getAngularAccelStr(false,it->getMaxAcceleration()).c_str());
			ui->qqTargetVelocity->setText(gv::getAngularVelocityStr(true,it->getTargetVelocity()).c_str());
		}
	}

	inMainRefreshRoutine=false;
}

void CQDlgMotionModeParameters::on_qqExplicitHandling_clicked()
{
	if (!App::ct->objCont->isLastSelectionAJoint())
		return;
	C3DObject* it=App::ct->objCont->getLastSelection();
	((CJoint*)it)->setExplicitHandling(!((CJoint*)it)->getExplicitHandling());
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();
}

void CQDlgMotionModeParameters::on_qqInfiniteAcceleration_clicked()
{
	if (!App::ct->objCont->isLastSelectionAJoint())
		return;
	C3DObject* it=App::ct->objCont->getLastSelection();
	((CJoint*)it)->setUnlimitedAcceleration(!((CJoint*)it)->getUnlimitedAcceleration());
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();
}

void CQDlgMotionModeParameters::on_qqInvert_clicked()
{
	if (!App::ct->objCont->isLastSelectionAJoint())
		return;
	C3DObject* it=App::ct->objCont->getLastSelection();
	((CJoint*)it)->setInvertTargetVelocityAtLimits(!((CJoint*)it)->getInvertTargetVelocityAtLimits());
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();
}

void CQDlgMotionModeParameters::on_qqTargetVelocity_editingFinished()
{
	if (!App::ct->objCont->isLastSelectionAJoint())
		return;
	CJoint* it=(CJoint*)App::ct->objCont->getLastSelection();
	bool ok;
	float newVal=ui->qqTargetVelocity->text().toFloat(&ok);
	if (ok)
	{
		if (it->getJointType()==sim_joint_prismatic_subtype)
			it->setTargetVelocity(newVal*gv::userToLinearVel);
		else
			it->setTargetVelocity(newVal*gv::userToAngularVel);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgMotionModeParameters::on_qqAcceleration_editingFinished()
{
	if (!App::ct->objCont->isLastSelectionAJoint())
		return;
	CJoint* it=(CJoint*)App::ct->objCont->getLastSelection();
	bool ok;
	float newVal=ui->qqAcceleration->text().toFloat(&ok);
	if (ok)
	{
		if (it->getJointType()==sim_joint_prismatic_subtype)
			it->setMaxAcceleration(newVal*gv::userToLinearAccel);
		else
			it->setMaxAcceleration(newVal*gv::userToAngularAccel);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgMotionModeParameters::on_qqClose_clicked(QAbstractButton *button)
{
	defaultModalDialogEndRoutine(true);
}
