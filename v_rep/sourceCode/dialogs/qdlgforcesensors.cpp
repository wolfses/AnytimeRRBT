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
#include "qdlgforcesensors.h"
#include "ui_qdlgforcesensors.h"
#include "Tt.h"
#include "GV.h"
#include "qdlgmaterial.h"
#include "App.h"

CQDlgForceSensors::CQDlgForceSensors(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgForceSensors)
{
    ui->setupUi(this);
}

CQDlgForceSensors::~CQDlgForceSensors()
{
    delete ui;
}

void CQDlgForceSensors::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgForceSensors::refresh()
{
	bool sel=App::ct->objCont->isLastSelectionAForceSensor();
	bool bigSel=(App::ct->objCont->getForceSensorNumberInSelection()>1);
	bool simNotRunning=App::ct->simulation->isSimulationStopped();
	CForceSensor* it=NULL;
	if (sel)
		it=(CForceSensor*)App::ct->objCont->getLastSelection();
	bool breakingEnabled=false;
	if ( (it!=NULL)&&(it->getEnableForceThreshold()||it->getEnableTorqueThreshold()) )
		breakingEnabled=true;

	ui->qqSize->setEnabled(sel);
	ui->qqColorA->setEnabled(sel);
	ui->qqColorB->setEnabled(sel);
	ui->qqApplyMain->setEnabled(bigSel);

	ui->qqSampleSize->setEnabled(sel&&simNotRunning);
	ui->qqAverage->setEnabled(sel&&simNotRunning);
	ui->qqMedian->setEnabled(sel&&simNotRunning);
	ui->qqApplyFilter->setEnabled(bigSel&&simNotRunning);

	ui->qqForceThresholdEnable->setEnabled(sel&&simNotRunning);
	ui->qqTorqueThresholdEnable->setEnabled(sel&&simNotRunning);
	ui->qqForceThreshold->setEnabled(sel&&it->getEnableForceThreshold()&&simNotRunning);
	ui->qqTorqueThreshold->setEnabled(sel&&it->getEnableTorqueThreshold()&&simNotRunning);
	ui->qqViolationCount->setEnabled(breakingEnabled&&simNotRunning);
	ui->qqApplyBreaking->setEnabled(bigSel&&simNotRunning);

	if (sel)
	{
		ui->qqSize->setText(gv::getSizeStr(false,it->getSize(),0).c_str());

		ui->qqSampleSize->setText(tt::FNb(it->getValueCountForFilter()).c_str());
		ui->qqAverage->setChecked(it->getFilterType()==0);
		ui->qqMedian->setChecked(it->getFilterType()==1);

		ui->qqForceThresholdEnable->setChecked(it->getEnableForceThreshold());
		ui->qqForceThreshold->setText(tt::FNb(1,it->getForceThreshold()*gv::kilogramsToUser*gv::meterToUser/(gv::secondsToUser*gv::secondsToUser),3,false).c_str());
		ui->qqTorqueThresholdEnable->setChecked(it->getEnableTorqueThreshold());
		ui->qqTorqueThreshold->setText(tt::FNb(1,it->getTorqueThreshold()*gv::kilogramsToUser*gv::meterToUser*gv::meterToUser/(gv::secondsToUser*gv::secondsToUser),3,false).c_str());
		ui->qqViolationCount->setText(tt::FNb(it->getConsecutiveThresholdViolationsForBreaking()).c_str());
	}
	else
	{
		ui->qqSize->setText("");

		ui->qqSampleSize->setText("");
		ui->qqAverage->setChecked(false);
		ui->qqMedian->setChecked(false);

		ui->qqForceThresholdEnable->setChecked(false);
		ui->qqForceThreshold->setText("");
		ui->qqTorqueThresholdEnable->setChecked(false);
		ui->qqTorqueThreshold->setText("");
		ui->qqViolationCount->setText("");
	}
}

void CQDlgForceSensors::on_qqSize_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAForceSensor())
			return;
		CForceSensor* it=(CForceSensor*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqSize->text().toFloat(&ok);
		if (ok)
		{
			it->setSize(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgForceSensors::on_qqColorA_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAForceSensor())
			return;
		CForceSensor* it=(CForceSensor*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&it->colorPart1,App::mainWindow,true,true,true,true,true,false,true,false);
	}
}

void CQDlgForceSensors::on_qqColorB_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAForceSensor())
			return;
		CForceSensor* it=(CForceSensor*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&it->colorPart2,App::mainWindow,true,true,true,true,true,false,true,false);
	}
}

void CQDlgForceSensors::on_qqApplyMain_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAForceSensor())
			return;
		CForceSensor* it=(CForceSensor*)App::ct->objCont->getLastSelection();
		for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
		{
			CForceSensor* it2=App::ct->objCont->getForceSensor(App::ct->objCont->getSelID(i));
			if (it2!=NULL)
			{
				it->colorPart1.copyYourselfInto(&it2->colorPart1);
				it->colorPart2.copyYourselfInto(&it2->colorPart2);
				it2->setSize(it->getSize());
				App::ct->undoBufferContainer->announceChangeStart(); // **************** UNDO THINGY ****************
			}
		}
		App::ct->undoBufferContainer->announceChangeEnd(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgForceSensors::on_qqSampleSize_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAForceSensor())
			return;
		CForceSensor* it=(CForceSensor*)App::ct->objCont->getLastSelection();
		bool ok;
		int newVal=ui->qqSampleSize->text().toInt(&ok);
		if (ok)
		{
			it->setValueCountForFilter(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgForceSensors::on_qqAverage_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAForceSensor())
			return;
		CForceSensor* it=(CForceSensor*)App::ct->objCont->getLastSelection();
		it->setFilterType(0);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgForceSensors::on_qqMedian_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAForceSensor())
			return;
		CForceSensor* it=(CForceSensor*)App::ct->objCont->getLastSelection();
		it->setFilterType(1);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgForceSensors::on_qqApplyFilter_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAForceSensor())
			return;
		CForceSensor* it=(CForceSensor*)App::ct->objCont->getLastSelection();
		for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
		{
			CForceSensor* it2=App::ct->objCont->getForceSensor(App::ct->objCont->getSelID(i));
			if (it2!=NULL)
			{
				it2->setValueCountForFilter(it->getValueCountForFilter());
				it2->setFilterType(it->getFilterType());
				App::ct->undoBufferContainer->announceChangeStart(); // **************** UNDO THINGY ****************
			}
		}
		App::ct->undoBufferContainer->announceChangeEnd(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgForceSensors::on_qqForceThresholdEnable_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAForceSensor())
			return;
		CForceSensor* it=(CForceSensor*)App::ct->objCont->getLastSelection();
		it->setEnableForceThreshold(!it->getEnableForceThreshold());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgForceSensors::on_qqForceThreshold_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAForceSensor())
			return;
		CForceSensor* it=(CForceSensor*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqForceThreshold->text().toFloat(&ok);
		if (ok)
		{
			it->setForceThreshold(newVal*gv::userToKilograms*gv::userToMeter/(gv::userToSeconds*gv::userToSeconds));
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgForceSensors::on_qqTorqueThresholdEnable_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAForceSensor())
			return;
		CForceSensor* it=(CForceSensor*)App::ct->objCont->getLastSelection();
		it->setEnableTorqueThreshold(!it->getEnableTorqueThreshold());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgForceSensors::on_qqTorqueThreshold_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAForceSensor())
			return;
		CForceSensor* it=(CForceSensor*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqTorqueThreshold->text().toFloat(&ok);
		if (ok)
		{
			it->setTorqueThreshold(newVal*gv::userToKilograms*gv::userToMeter*gv::userToMeter/(gv::userToSeconds*gv::userToSeconds));
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgForceSensors::on_qqViolationCount_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAForceSensor())
			return;
		CForceSensor* it=(CForceSensor*)App::ct->objCont->getLastSelection();
		bool ok;
		int newVal=ui->qqViolationCount->text().toInt(&ok);
		if (ok)
		{
			it->setConsecutiveThresholdViolationsForBreaking(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgForceSensors::on_qqApplyBreaking_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAForceSensor())
			return;
		CForceSensor* it=(CForceSensor*)App::ct->objCont->getLastSelection();
		for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
		{
			CForceSensor* it2=App::ct->objCont->getForceSensor(App::ct->objCont->getSelID(i));
			if (it2!=NULL)
			{
				it2->setEnableForceThreshold(it->getEnableForceThreshold());
				it2->setEnableTorqueThreshold(it->getEnableTorqueThreshold());
				it2->setForceThreshold(it->getForceThreshold());
				it2->setTorqueThreshold(it->getTorqueThreshold());
				it2->setConsecutiveThresholdViolationsForBreaking(it->getConsecutiveThresholdViolationsForBreaking());
				App::ct->undoBufferContainer->announceChangeStart(); // **************** UNDO THINGY ****************
			}
		}
		App::ct->undoBufferContainer->announceChangeEnd(); // **************** UNDO THINGY ****************
		refresh();
	}
}
