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

#include "qdlgopenglsettings.h"
#include "ui_qdlgopenglsettings.h"
#include "App.h"
#include "Tt.h"

CQDlgOpenGlSettings::CQDlgOpenGlSettings(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
	ui(new Ui::CQDlgOpenGlSettings)
{
    ui->setupUi(this);
}

CQDlgOpenGlSettings::~CQDlgOpenGlSettings()
{
    delete ui;
}

void CQDlgOpenGlSettings::cancelEvent()
{
//	defaultModalDialogEndRoutine(false);
}

void CQDlgOpenGlSettings::okEvent()
{
//	defaultModalDialogEndRoutine(true);
}

void CQDlgOpenGlSettings::refresh()
{
	ui->qqContextDefault->setChecked(offscreenContextType==-1);
	ui->qqContextOffscreen->setChecked(offscreenContextType==0);
	ui->qqContextVisibleWindow->setChecked(offscreenContextType==1);
	ui->qqContextInvisibleWindow->setChecked(offscreenContextType==2);

	ui->qqFboDefault->setChecked(fboType==-1);
	ui->qqFboNonQt->setChecked(fboType==0);
	ui->qqFboQt->setChecked(fboType==1);

	ui->qqGuiRenderingOnly->setChecked((exclusiveGuiRendering==-1)||(exclusiveGuiRendering==1));
	ui->qqCompatibilityTweak1->setChecked(compatibilityTweak1);
	ui->qqUsingGlFinish->setChecked(glFinish_normal);
	ui->qqUsingGlFinishVisionSensors->setChecked(glFinish_visionSensors);
	ui->qqIdleFps->setText(tt::FNb(0,idleFps).c_str());
	ui->qqForceExt->setChecked(forceExt);

	ui->qqMajorOpenGlVersion->setText(tt::FNb(0,glVersionMajor).c_str());
	ui->qqMinorOpenGlVersion->setText(tt::FNb(0,glVersionMinor).c_str());
}

void CQDlgOpenGlSettings::on_qqOk_accepted()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgOpenGlSettings::on_qqContextDefault_clicked()
{
	offscreenContextType=-1;
	refresh();
}

void CQDlgOpenGlSettings::on_qqContextOffscreen_clicked()
{
	offscreenContextType=0;
	refresh();
}

void CQDlgOpenGlSettings::on_qqContextVisibleWindow_clicked()
{
	offscreenContextType=1;
	refresh();
}

void CQDlgOpenGlSettings::on_qqContextInvisibleWindow_clicked()
{
	offscreenContextType=2;
	refresh();
}

void CQDlgOpenGlSettings::on_qqFboDefault_clicked()
{
	fboType=-1;
	refresh();
}

void CQDlgOpenGlSettings::on_qqFboNonQt_clicked()
{
	fboType=0;
	refresh();
}

void CQDlgOpenGlSettings::on_qqFboQt_clicked()
{
	fboType=1;
	refresh();
}

void CQDlgOpenGlSettings::on_qqGuiRenderingOnly_clicked()
{
	if ((exclusiveGuiRendering==-1)||(exclusiveGuiRendering==1))
		exclusiveGuiRendering=0;
	else
		exclusiveGuiRendering=1;
	refresh();
}

void CQDlgOpenGlSettings::on_qqCompatibilityTweak1_clicked()
{
	compatibilityTweak1=!compatibilityTweak1;
	refresh();
}

void CQDlgOpenGlSettings::on_qqUsingGlFinish_clicked()
{
	glFinish_normal=!glFinish_normal;
	refresh();
}

void CQDlgOpenGlSettings::on_qqUsingGlFinishVisionSensors_clicked()
{
	glFinish_visionSensors=!glFinish_visionSensors;
	refresh();
}

void CQDlgOpenGlSettings::on_qqMajorOpenGlVersion_editingFinished()
{
	int newVal;
	bool ok;
	newVal=ui->qqMajorOpenGlVersion->text().toInt(&ok);
	if (ok)
	{
		if (newVal!=-1)
			tt::limitValue(1,4,newVal);
		glVersionMajor=newVal;
	}
	refresh();
}

void CQDlgOpenGlSettings::on_qqMinorOpenGlVersion_editingFinished()
{
	int newVal;
	bool ok;
	newVal=ui->qqMinorOpenGlVersion->text().toInt(&ok);
	if (ok)
	{
		if (newVal!=-1)
			tt::limitValue(0,5,newVal);
		glVersionMinor=newVal;
	}
	refresh();
}

void CQDlgOpenGlSettings::on_qqIdleFps_editingFinished()
{
	int newVal;
	bool ok;
	newVal=ui->qqIdleFps->text().toInt(&ok);
	if (ok)
	{
		tt::limitValue(2,25,newVal);
		idleFps=newVal;
	}
	refresh();
}

void CQDlgOpenGlSettings::on_qqForceExt_clicked()
{
	forceExt=!forceExt;
	refresh();
}
