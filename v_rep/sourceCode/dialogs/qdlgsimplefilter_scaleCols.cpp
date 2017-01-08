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
#include "qdlgsimplefilter_scaleCols.h"
#include "ui_qdlgsimplefilter_scaleCols.h"
#include "GV.h"
#include "Tt.h"

CQDlgSimpleFilter_scaleCols::CQDlgSimpleFilter_scaleCols(QWidget *parent) :
	CQDlgSimpleFilter(parent),
	ui(new Ui::CQDlgSimpleFilter_scaleCols)
{
    ui->setupUi(this);
}

CQDlgSimpleFilter_scaleCols::~CQDlgSimpleFilter_scaleCols()
{
    delete ui;
}

void CQDlgSimpleFilter_scaleCols::refresh()
{
	ui->qqRgb->setChecked((_byteParameters[0]&2)!=0);
	ui->qqHsl->setChecked((_byteParameters[0]&2)==0);
	ui->qqRed1->setText(tt::FNb(0,_floatParameters[0],3,true).c_str());
	ui->qqGreen1->setText(tt::FNb(0,_floatParameters[1],3,true).c_str());
	ui->qqBlue1->setText(tt::FNb(0,_floatParameters[2],3,true).c_str());
	ui->qqRed2->setText(tt::FNb(0,_floatParameters[3],3,true).c_str());
	ui->qqGreen2->setText(tt::FNb(0,_floatParameters[4],3,true).c_str());
	ui->qqBlue2->setText(tt::FNb(0,_floatParameters[5],3,true).c_str());
	ui->qqRed3->setText(tt::FNb(0,_floatParameters[6],3,true).c_str());
	ui->qqGreen3->setText(tt::FNb(0,_floatParameters[7],3,true).c_str());
	ui->qqBlue3->setText(tt::FNb(0,_floatParameters[8],3,true).c_str());
}

void CQDlgSimpleFilter_scaleCols::on_qqOkCancel_accepted()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgSimpleFilter_scaleCols::on_qqOkCancel_rejected()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgSimpleFilter_scaleCols::on_qqRgb_clicked()
{
	_byteParameters[0]|=2;
	refresh();
}

void CQDlgSimpleFilter_scaleCols::on_qqHsl_clicked()
{
	_byteParameters[0]&=255-2;
	refresh();
}

void CQDlgSimpleFilter_scaleCols::on_qqRed1_editingFinished()
{
	bool ok;
	float newVal=ui->qqRed1->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-1.0f,1.0f,newVal);
		_floatParameters[0]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_scaleCols::on_qqGreen1_editingFinished()
{
	bool ok;
	float newVal=ui->qqGreen1->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-1.0f,1.0f,newVal);
		_floatParameters[1]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_scaleCols::on_qqBlue1_editingFinished()
{
	bool ok;
	float newVal=ui->qqBlue1->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-1.0f,1.0f,newVal);
		_floatParameters[2]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_scaleCols::on_qqRed2_editingFinished()
{
	bool ok;
	float newVal=ui->qqRed2->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[3]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_scaleCols::on_qqGreen2_editingFinished()
{
	bool ok;
	float newVal=ui->qqGreen2->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[4]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_scaleCols::on_qqBlue2_editingFinished()
{
	bool ok;
	float newVal=ui->qqBlue2->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-100.0f,100.0f,newVal);
		_floatParameters[5]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_scaleCols::on_qqRed3_editingFinished()
{
	bool ok;
	float newVal=ui->qqRed3->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-1.0f,1.0f,newVal);
		_floatParameters[6]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_scaleCols::on_qqGreen3_editingFinished()
{
	bool ok;
	float newVal=ui->qqGreen3->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-1.0f,1.0f,newVal);
		_floatParameters[7]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_scaleCols::on_qqBlue3_editingFinished()
{
	bool ok;
	float newVal=ui->qqBlue3->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(-1.0f,1.0f,newVal);
		_floatParameters[8]=newVal;
	}
	refresh();
}
