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
#include "qdlgfog.h"
#include "ui_qdlgfog.h"
#include "GV.h"
#include "Tt.h"
#include "qdlgcolor.h"
#include "App.h"

CQDlgFog::CQDlgFog(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgFog)
{
    ui->setupUi(this);
}

CQDlgFog::~CQDlgFog()
{
    delete ui;
}

void CQDlgFog::refresh()
{
	ui->qqFogEnabled->setChecked(App::ct->environment->fogEnabled);

	ui->qqLinear->setEnabled(App::ct->environment->fogEnabled);
	ui->qqExp->setEnabled(App::ct->environment->fogEnabled);
	ui->qqExp2->setEnabled(App::ct->environment->fogEnabled);
	ui->qqStartDistance->setEnabled(App::ct->environment->fogEnabled&&(App::ct->environment->fogType==0));
	ui->qqEndDistance->setEnabled(App::ct->environment->fogEnabled&&(App::ct->environment->fogType==0));
	ui->qqDensity->setEnabled(App::ct->environment->fogEnabled&&(App::ct->environment->fogType!=0));
	ui->qqAdjustColor->setEnabled(App::ct->environment->fogEnabled);

	ui->qqLinear->setChecked(App::ct->environment->fogType==0);
	ui->qqExp->setChecked(App::ct->environment->fogType==1);
	ui->qqExp2->setChecked(App::ct->environment->fogType==2);
	if (App::ct->environment->fogType!=0)
	{
		ui->qqStartDistance->setText("");
		ui->qqEndDistance->setText("");
		ui->qqDensity->setText(tt::FNb(0,App::ct->environment->fogDensity,3,false).c_str());
	}
	else
	{
		ui->qqStartDistance->setText(gv::getSizeStr(false,App::ct->environment->fogStart,-1).c_str());
		ui->qqEndDistance->setText(gv::getSizeStr(false,App::ct->environment->fogEnd,-1).c_str());
		ui->qqDensity->setText("");
	}
}

void CQDlgFog::on_qqFogEnabled_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->environment->fogEnabled=!App::ct->environment->fogEnabled;
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgFog::on_qqLinear_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->environment->fogType=0;
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgFog::on_qqExp_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->environment->fogType=1;
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgFog::on_qqExp2_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->environment->fogType=2;
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgFog::on_qqStartDistance_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqStartDistance->text().toFloat(&ok);
		if (ok)
		{
			newVal*=gv::userToMeter;
			tt::limitValue(0.0f,1000.0f,newVal); // "inverted" is allowed in opengl!
			App::ct->environment->fogStart=newVal;
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgFog::on_qqEndDistance_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqEndDistance->text().toFloat(&ok);
		if (ok)
		{
			newVal*=gv::userToMeter;
			tt::limitValue(0.01f,1000.0f,newVal); // "inverted" is allowed in opengl!
			App::ct->environment->fogEnd=newVal;
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgFog::on_qqDensity_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqDensity->text().toFloat(&ok);
		if (ok)
		{
			tt::limitValue(0.0f,1000.0f,newVal);
			App::ct->environment->fogDensity=newVal;
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgFog::on_qqAdjustColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		int identification[4]={App::ct->environment->getLifeID(),-1,-1,-1};
		CQDlgColor::display(identification,App::ct->environment->fogBackgroundColor,App::mainWindow);
	}
}
