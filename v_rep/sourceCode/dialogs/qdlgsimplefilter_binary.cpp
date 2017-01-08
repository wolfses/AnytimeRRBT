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
#include "qdlgsimplefilter_binary.h"
#include "ui_qdlgsimplefilter_binary.h"
#include "GV.h"
#include "Tt.h"
#include "Global.h"

CQDlgSimpleFilter_binary::CQDlgSimpleFilter_binary(QWidget *parent) :
	CQDlgSimpleFilter(parent),
	ui(new Ui::CQDlgSimpleFilter_binary)
{
    ui->setupUi(this);
}

CQDlgSimpleFilter_binary::~CQDlgSimpleFilter_binary()
{
    delete ui;
}

void CQDlgSimpleFilter_binary::refresh()
{
	ui->qqTriggerEnabled->setChecked((_byteParameters[0]&4)!=0);
	ui->qqVisualize->setChecked((_byteParameters[0]&8)!=0);
	ui->qqThreshold->setText(tt::FNb(0,_floatParameters[0],3,false).c_str());
	ui->qqProportion->setText(tt::FNb(0,_floatParameters[4],3,false).c_str());
	ui->qqProportionTol->setText(tt::FNb(0,_floatParameters[5],3,false).c_str());
	ui->qqPosX->setText(tt::FNb(0,_floatParameters[6],3,false).c_str());
	ui->qqPosXTol->setText(tt::FNb(0,_floatParameters[7],3,false).c_str());
	ui->qqPosY->setText(tt::FNb(0,_floatParameters[8],3,false).c_str());
	ui->qqPosYTol->setText(tt::FNb(0,_floatParameters[9],3,false).c_str());
	ui->qqAngle->setText(gv::getAngleStr(true,_floatParameters[10],-1).c_str());
	ui->qqAngleTol->setText(gv::getAngleStr(false,_floatParameters[11],-1).c_str());
	ui->qqRoundness->setText(tt::FNb(0,_floatParameters[12],3,false).c_str());
}

void CQDlgSimpleFilter_binary::on_qqOkCancel_accepted()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgSimpleFilter_binary::on_qqOkCancel_rejected()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgSimpleFilter_binary::on_qqThreshold_editingFinished()
{
	bool ok;
	float newVal=ui->qqThreshold->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0f,1.0f,newVal);
		_floatParameters[0]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_binary::on_qqTriggerEnabled_clicked()
{
	_byteParameters[0]^=4;
	refresh();
}

void CQDlgSimpleFilter_binary::on_qqVisualize_clicked()
{
	_byteParameters[0]^=8;
	refresh();
}

void CQDlgSimpleFilter_binary::on_qqProportion_editingFinished()
{
	bool ok;
	float newVal=ui->qqProportion->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0f,1.0f,newVal);
		_floatParameters[4]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_binary::on_qqProportionTol_editingFinished()
{
	bool ok;
	float newVal=ui->qqProportionTol->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0f,1.0f,newVal);
		_floatParameters[5]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_binary::on_qqPosX_editingFinished()
{
	bool ok;
	float newVal=ui->qqPosX->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0f,1.0f,newVal);
		_floatParameters[6]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_binary::on_qqPosXTol_editingFinished()
{
	bool ok;
	float newVal=ui->qqPosXTol->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0f,1.0f,newVal);
		_floatParameters[7]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_binary::on_qqPosY_editingFinished()
{
	bool ok;
	float newVal=ui->qqPosY->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0f,1.0f,newVal);
		_floatParameters[8]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_binary::on_qqPosYTol_editingFinished()
{
	bool ok;
	float newVal=ui->qqPosYTol->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0f,1.0f,newVal);
		_floatParameters[9]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_binary::on_qqAngle_editingFinished()
{
	bool ok;
	float newVal=ui->qqAngle->text().toFloat(&ok);
	if (ok)
	{
		newVal*=gv::userToRad;
		tt::limitValue(-piValD2,piValD2,newVal);
		_floatParameters[10]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_binary::on_qqAngleTol_editingFinished()
{
	bool ok;
	float newVal=ui->qqAngleTol->text().toFloat(&ok);
	if (ok)
	{
		newVal*=gv::userToRad;
		tt::limitValue(0.0f,piValue*0.501f,newVal);
		_floatParameters[11]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_binary::on_qqRoundness_editingFinished()
{
	bool ok;
	float newVal=ui->qqRoundness->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0f,1.0f,newVal);
		_floatParameters[12]=newVal;
	}
	refresh();
}
