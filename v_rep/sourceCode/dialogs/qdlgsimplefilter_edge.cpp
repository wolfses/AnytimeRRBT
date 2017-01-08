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
#include "qdlgsimplefilter_edge.h"
#include "ui_qdlgsimplefilter_edge.h"
#include "GV.h"
#include "Tt.h"

CQDlgSimpleFilter_edge::CQDlgSimpleFilter_edge(QWidget *parent) :
	CQDlgSimpleFilter(parent),
	ui(new Ui::CQDlgSimpleFilter_edge)
{
    ui->setupUi(this);
}

CQDlgSimpleFilter_edge::~CQDlgSimpleFilter_edge()
{
    delete ui;
}

void CQDlgSimpleFilter_edge::refresh()
{
	ui->qqThreshold->setText(tt::FNb(0,_floatParameters[0],3,false).c_str());
}

void CQDlgSimpleFilter_edge::on_qqOkCancel_accepted()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgSimpleFilter_edge::on_qqOkCancel_rejected()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgSimpleFilter_edge::on_qqThreshold_editingFinished()
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
