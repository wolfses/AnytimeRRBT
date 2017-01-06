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
#include "qdlgsimplefilter_uniformImage.h"
#include "ui_qdlgsimplefilter_uniformImage.h"
#include "GV.h"
#include "Tt.h"

CQDlgSimpleFilter_uniformImage::CQDlgSimpleFilter_uniformImage(QWidget *parent) :
	CQDlgSimpleFilter(parent),
	ui(new Ui::CQDlgSimpleFilter_uniformImage)
{
    ui->setupUi(this);
}

CQDlgSimpleFilter_uniformImage::~CQDlgSimpleFilter_uniformImage()
{
    delete ui;
}

void CQDlgSimpleFilter_uniformImage::refresh()
{
	ui->qqRed->setText(tt::FNb(0,_floatParameters[0],3,false).c_str());
	ui->qqGreen->setText(tt::FNb(0,_floatParameters[1],3,false).c_str());
	ui->qqBlue->setText(tt::FNb(0,_floatParameters[2],3,false).c_str());
}

void CQDlgSimpleFilter_uniformImage::on_qqOkCancel_accepted()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgSimpleFilter_uniformImage::on_qqOkCancel_rejected()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgSimpleFilter_uniformImage::on_qqRed_editingFinished()
{
	bool ok;
	float newVal=ui->qqRed->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0f,1.0f,newVal);
		_floatParameters[0]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_uniformImage::on_qqGreen_editingFinished()
{
	bool ok;
	float newVal=ui->qqGreen->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0f,1.0f,newVal);
		_floatParameters[1]=newVal;
	}
	refresh();
}

void CQDlgSimpleFilter_uniformImage::on_qqBlue_editingFinished()
{
	bool ok;
	float newVal=ui->qqBlue->text().toFloat(&ok);
	if (ok)
	{
		tt::limitValue(0.0f,1.0f,newVal);
		_floatParameters[2]=newVal;
	}
	refresh();
}
