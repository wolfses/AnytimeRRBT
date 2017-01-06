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
#include "qdlgsimplefilter_correlation.h"
#include "ui_qdlgsimplefilter_correlation.h"
#include "GV.h"
#include "Tt.h"

CQDlgSimpleFilter_correlation::CQDlgSimpleFilter_correlation(QWidget *parent) :
	CQDlgSimpleFilter(parent),
	ui(new Ui::CQDlgSimpleFilter_correlation)
{
    ui->setupUi(this);
}

CQDlgSimpleFilter_correlation::~CQDlgSimpleFilter_correlation()
{
    delete ui;
}

void CQDlgSimpleFilter_correlation::refresh()
{
	ui->qqGreyScale->setChecked((_byteParameters[0]&16)!=0);
	ui->qqVisualizeCenter->setChecked((_byteParameters[0]&8)!=0);
	ui->qqTemplateX->setText(tt::FNb(0,_floatParameters[0],3,false).c_str());
	ui->qqTemplateY->setText(tt::FNb(0,_floatParameters[1],3,false).c_str());
	ui->qqFrameX->setText(tt::FNb(0,_floatParameters[2],3,false).c_str());
	ui->qqFrameY->setText(tt::FNb(0,_floatParameters[3],3,false).c_str());
}

void CQDlgSimpleFilter_correlation::on_qqOkCancel_accepted()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgSimpleFilter_correlation::on_qqOkCancel_rejected()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgSimpleFilter_correlation::on_qqGreyScale_clicked()
{
	_byteParameters[0]^=16;
	refresh();
}

void CQDlgSimpleFilter_correlation::on_qqVisualizeCenter_clicked()
{
	_byteParameters[0]^=8;
	refresh();
}

void CQDlgSimpleFilter_correlation::on_qqTemplateX_editingFinished()
{
	bool ok;
	float newVal=ui->qqTemplateX->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0f,1.0f,newVal);
		_floatParameters[0]=newVal;
		tt::limitValue(newVal,1.0f,_floatParameters[2]); // make sure the frame is not smaller!
	}
	refresh();
}

void CQDlgSimpleFilter_correlation::on_qqTemplateY_editingFinished()
{
	bool ok;
	float newVal=ui->qqTemplateY->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0f,1.0f,newVal);
		_floatParameters[1]=newVal;
		tt::limitValue(newVal,1.0f,_floatParameters[3]); // make sure the frame is not smaller!
	}
	refresh();
}

void CQDlgSimpleFilter_correlation::on_qqFrameX_editingFinished()
{
	bool ok;
	float newVal=ui->qqFrameX->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0f,1.0f,newVal);
		_floatParameters[2]=newVal;
		tt::limitValue(0.0f,newVal,_floatParameters[0]); // make sure the template is not bigger!
	}
	refresh();
}

void CQDlgSimpleFilter_correlation::on_qqFrameY_editingFinished()
{
	bool ok;
	float newVal=ui->qqFrameY->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0f,1.0f,newVal);
		_floatParameters[3]=newVal;
		tt::limitValue(0.0f,newVal,_floatParameters[1]); // make sure the template is not bigger!
	}
	refresh();
}
