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
#include "qdlgproxsensdetectionparam.h"
#include "ui_qdlgproxsensdetectionparam.h"
#include "GV.h"
#include "Tt.h"
#include "mathDefines.h"
#include "App.h"

CQDlgProxSensDetectionParam::CQDlgProxSensDetectionParam(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgProxSensDetectionParam)
{
    ui->setupUi(this);
}

CQDlgProxSensDetectionParam::~CQDlgProxSensDetectionParam()
{
    delete ui;
}

void CQDlgProxSensDetectionParam::cancelEvent()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgProxSensDetectionParam::okEvent()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgProxSensDetectionParam::refresh()
{
	ui->qqFrontFace->setChecked(frontFace);
	ui->qqBackFace->setChecked(backFace);
	ui->qqFast->setChecked(fast);
	ui->qqLimitedAngle->setChecked(limitedAngle);
	ui->qqAngle->setEnabled(limitedAngle);
	ui->qqAngle->setText(gv::getAngleStr(false,angle,0).c_str());
	ui->qqOcclusionCheck->setEnabled(limitedAngle);
	ui->qqOcclusionCheck->setChecked(limitedAngle&&occlusionCheck);
	ui->qqMinDistEnabled->setChecked(distanceContraint);
	ui->qqMinDist->setEnabled(distanceContraint);
	ui->qqMinDist->setText(gv::getSizeStr(false,minimumDistance,0).c_str());
	ui->qqRayCount->setEnabled(randomizedDetection);
	ui->qqRandomizedDetectionCount->setEnabled(randomizedDetection);
	if (randomizedDetection)
	{
		ui->qqRayCount->setText(tt::FNb(rayCount).c_str());
		ui->qqRandomizedDetectionCount->setText(tt::FNb(rayDetectionCount).c_str());
	}
	else
	{
		ui->qqRayCount->setText("");
		ui->qqRandomizedDetectionCount->setText("");
	}
}

void CQDlgProxSensDetectionParam::on_qqFrontFace_clicked()
{
	frontFace=!frontFace;
	if (!frontFace)
		backFace=true;
	refresh();
}

void CQDlgProxSensDetectionParam::on_qqBackFace_clicked()
{
	backFace=!backFace;
	if (!backFace)
		frontFace=true;
	refresh();
}

void CQDlgProxSensDetectionParam::on_qqFast_clicked()
{
	fast=!fast;
	refresh();
}

void CQDlgProxSensDetectionParam::on_qqLimitedAngle_clicked()
{
	limitedAngle=!limitedAngle;
	refresh();
}

void CQDlgProxSensDetectionParam::on_qqAngle_editingFinished()
{
	bool ok;
	float newVal=ui->qqAngle->text().toFloat(&ok);
	if (ok)
	{
		angle=(newVal*gv::userToRad);
		tt::limitValue(0.1f*degToRad,90.0*degToRad,angle);
	}
	refresh();
}

void CQDlgProxSensDetectionParam::on_qqOcclusionCheck_clicked()
{
	occlusionCheck=!occlusionCheck;
	refresh();
}

void CQDlgProxSensDetectionParam::on_qqMinDistEnabled_clicked()
{
	distanceContraint=!distanceContraint;
	refresh();
}

void CQDlgProxSensDetectionParam::on_qqMinDist_editingFinished()
{
	bool ok;
	float newVal=ui->qqMinDist->text().toFloat(&ok);
	if (ok)
		minimumDistance=(newVal*gv::userToMeter);
	refresh();
}

void CQDlgProxSensDetectionParam::on_qqRayCount_editingFinished()
{
	bool ok;
	int newVal=ui->qqRayCount->text().toInt(&ok);
	if (ok)
	{
		rayCount=tt::getLimitedInt(1,5000,newVal);
		rayDetectionCount=tt::getLimitedInt(1,rayCount,rayDetectionCount);
	}
	refresh();
}

void CQDlgProxSensDetectionParam::on_qqRandomizedDetectionCount_editingFinished()
{
	bool ok;
	int newVal=ui->qqRandomizedDetectionCount->text().toInt(&ok);
	if (ok)
		rayDetectionCount=tt::getLimitedInt(1,rayCount,newVal);
	refresh();
}

void CQDlgProxSensDetectionParam::on_qqOkCancel_accepted()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgProxSensDetectionParam::on_qqOkCancel_rejected()
{
	defaultModalDialogEndRoutine(false);
}
