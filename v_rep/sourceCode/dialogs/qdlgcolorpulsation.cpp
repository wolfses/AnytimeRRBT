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
#include "qdlgcolorpulsation.h"
#include "ui_qdlgcolorpulsation.h"
#include "GV.h"
#include "Tt.h"
#include "App.h"

CQDlgColorPulsation::CQDlgColorPulsation(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgColorPulsation)
{
    ui->setupUi(this);
}

CQDlgColorPulsation::~CQDlgColorPulsation()
{
    delete ui;
}

void CQDlgColorPulsation::cancelEvent()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgColorPulsation::okEvent()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgColorPulsation::refresh()
{
	ui->qqEnablePulsation->setChecked(pulsationEnabled);
	ui->qqRealTime->setEnabled(pulsationEnabled);
	ui->qqSimulationTime->setEnabled(pulsationEnabled);
	ui->qqFrequency->setEnabled(pulsationEnabled);
	ui->qqPhase->setEnabled(pulsationEnabled);
	ui->qqRatio->setEnabled(pulsationEnabled);

	ui->qqRealTime->setChecked(pulsationRealTime);
	ui->qqSimulationTime->setChecked(!pulsationRealTime);
	ui->qqFrequency->setText(gv::getFrequencyStr(pulsationFrequency).c_str());
	ui->qqPhase->setText(tt::FNb(0,pulsationPhase,2,false).c_str());
	ui->qqRatio->setText(tt::FNb(0,pulsationRatio,2,false).c_str());
}

void CQDlgColorPulsation::on_qqEnablePulsation_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		pulsationEnabled=!pulsationEnabled;
		refresh();
	}
}

void CQDlgColorPulsation::on_qqRealTime_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		pulsationRealTime=true;
		refresh();
	}
}

void CQDlgColorPulsation::on_qqSimulationTime_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		pulsationRealTime=false;
		refresh();
	}
}

void CQDlgColorPulsation::on_qqFrequency_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqFrequency->text().toFloat(&ok);
		if (ok)
		{
			pulsationFrequency=newVal/gv::userToSeconds;
			pulsationFrequency=tt::getLimitedFloat(0.001f,1000.0f,pulsationFrequency);
		}
		refresh();
	}
}

void CQDlgColorPulsation::on_qqPhase_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqPhase->text().toFloat(&ok);
		if (ok)
		{
			pulsationPhase=newVal;
			pulsationPhase=tt::getLimitedFloat(0.0f,0.999f,pulsationPhase);
		}
		refresh();
	}
}

void CQDlgColorPulsation::on_qqRatio_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqRatio->text().toFloat(&ok);
		if (ok)
		{
			pulsationRatio=newVal;
			pulsationRatio=tt::getLimitedFloat(0.01f,1.0f,pulsationRatio);
		}
		refresh();
	}
}

void CQDlgColorPulsation::on_qqOkCancel_accepted()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		defaultModalDialogEndRoutine(true);
	}
}

void CQDlgColorPulsation::on_qqOkCancel_rejected()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		defaultModalDialogEndRoutine(false);
	}
}
