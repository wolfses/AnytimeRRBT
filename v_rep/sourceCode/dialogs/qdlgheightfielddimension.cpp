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
#include "qdlgheightfielddimension.h"
#include "ui_qdlgheightfielddimension.h"
#include "GV.h"
#include "Tt.h"
#include "App.h"

CQDlgHeightfieldDimension::CQDlgHeightfieldDimension(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgHeightfieldDimension)
{
    ui->setupUi(this);
	xSize=1.0f;
	ySize=1.0f;
	zScaling=1.0f;
	xSizeTimesThisGivesYSize=1.0f;
}

CQDlgHeightfieldDimension::~CQDlgHeightfieldDimension()
{
    delete ui;
}

void CQDlgHeightfieldDimension::cancelEvent()
{
//	defaultModalDialogEndRoutine(false);
}

void CQDlgHeightfieldDimension::okEvent()
{
//	defaultModalDialogEndRoutine(true);
}

void CQDlgHeightfieldDimension::refresh()
{
	ui->qqSizeX->setText(gv::getSizeStr(false,xSize).c_str());
	ui->qqSizeY->setText(gv::getSizeStr(false,ySize).c_str());
	ui->qqScalingZ->setText(tt::FNb(0,zScaling,3,false).c_str());
}

void CQDlgHeightfieldDimension::on_qqSizeX_editingFinished()
{
	bool ok;
	float newVal=ui->qqSizeX->text().toFloat(&ok);
	if (ok)
	{
		newVal*=gv::userToMeter;
		tt::limitValue(0.01f,10000.0f,newVal);
		xSize=newVal;
		ySize=newVal*xSizeTimesThisGivesYSize;
	}
	refresh();
}

void CQDlgHeightfieldDimension::on_qqSizeY_editingFinished()
{
	bool ok;
	float newVal=ui->qqSizeY->text().toFloat(&ok);
	if (ok)
	{
		newVal*=gv::userToMeter;
		tt::limitValue(0.01f,10000.0f,newVal);
		ySize=newVal;
		xSize=newVal/xSizeTimesThisGivesYSize;
	}
	refresh();
}

void CQDlgHeightfieldDimension::on_qqScalingZ_editingFinished()
{
	bool ok;
	float newVal=ui->qqScalingZ->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.001f,1000.0f,newVal);
		zScaling=newVal;
	}
	refresh();
}

void CQDlgHeightfieldDimension::on_qqOk_accepted()
{
	defaultModalDialogEndRoutine(true);
}
