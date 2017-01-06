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
#include "qdlgsimulation.h"
#include "ui_qdlgsimulation.h"
#include "App.h"
#include "Tt.h"
#include "GV.h"
#include "v_repStrings.h"

CQDlgSimulation::CQDlgSimulation(QWidget *parent) :
	CDlgEx(parent),
	ui(new Ui::CQDlgSimulation)
{
	ui->setupUi(this);
	inMainRefreshRoutine=false;
}

CQDlgSimulation::~CQDlgSimulation()
{
    delete ui;
}

void CQDlgSimulation::refresh()
{
	inMainRefreshRoutine=true;

	bool custom=(App::ct->simulation->getDefaultSimulationParameterIndex()==5);

	bool noEditMode=(App::ct->objCont->getEditModeType()==NO_EDIT_MODE);

	ui->qqConfigCombo->setEnabled(noEditMode&&App::ct->simulation->isSimulationStopped());
	ui->qqConfigCombo->clear();
	ui->qqConfigCombo->addItem(strTranslate(IDSN_TIME_STEP_CONFIG_200),QVariant(0));
	ui->qqConfigCombo->addItem(strTranslate(IDSN_TIME_STEP_CONFIG_100),QVariant(1));
	ui->qqConfigCombo->addItem(strTranslate(IDSN_TIME_STEP_CONFIG_50),QVariant(2));
	ui->qqConfigCombo->addItem(strTranslate(IDSN_TIME_STEP_CONFIG_25),QVariant(3));
	ui->qqConfigCombo->addItem(strTranslate(IDSN_TIME_STEP_CONFIG_10),QVariant(4));

	float dt=(App::ct->simulation->getSimulationTimeStep_raw(5)*1000.0f);
	std::string txt("dt=");
	txt+=tt::FNb(0,dt,1,false);
	txt+=IDSN_TIME_STEP_CONFIG_CUSTOM;
	ui->qqConfigCombo->addItem(txt.c_str(),QVariant(5));
//	ui->qqConfigCombo->addItem(strTranslate(IDSN_TIME_STEP_CONFIG_CUSTOM),QVariant(5));

	for (int i=0;i<ui->qqConfigCombo->count();i++)
	{
		if (ui->qqConfigCombo->itemData(i).toInt()==App::ct->simulation->getDefaultSimulationParameterIndex())
		{
			ui->qqConfigCombo->setCurrentIndex(i);
			break;
		}
	}

	ui->qqRealTime->setChecked(App::ct->simulation->getRealTimeSimulation());
	ui->qqCatchUp->setChecked(App::ct->simulation->getCatchUpIfLate());
	ui->qqTimeStep->setText(gv::getTimeStr(App::ct->simulation->getSimulationTimeStep_speedModified()).c_str());
	ui->qqTimeStep->setEnabled(custom&&noEditMode&&App::ct->simulation->isSimulationStopped());
	ui->qqFullscreen->setChecked(App::ct->simulation->getFullscreenAtSimulationStart());
	ui->qqFullscreen->setEnabled(noEditMode&&App::ct->simulation->isSimulationStopped());

	ui->qqScriptExecutionPasses->setText(tt::FNb(0,App::ct->simulation->getSimulationPassesPerRendering_speedModified(),false).c_str());
	ui->qqScriptExecutionPasses->setEnabled(custom&&noEditMode&&App::ct->simulation->isSimulationStopped());

	ui->qqRealTime->setEnabled(noEditMode&&App::ct->simulation->isSimulationStopped());
	ui->qqCatchUp->setEnabled(noEditMode&&App::ct->simulation->isSimulationStopped()&&App::ct->simulation->getRealTimeSimulation());
	ui->qqMultiplicationCoefficient->setEnabled(noEditMode&&App::ct->simulation->isSimulationStopped()&&App::ct->simulation->getRealTimeSimulation());
	ui->qqMultiplicationCoefficient->setText(tt::FNb(0,App::ct->simulation->getRealTimeCoefficient_speedModified(),2,false).c_str());

	ui->qqPauseTime->setEnabled(noEditMode&&(!App::ct->simulation->isSimulationRunning())&&App::ct->simulation->getPauseEnabled());
	ui->qqPauseWhenTimeHigher->setEnabled(noEditMode&&(!App::ct->simulation->isSimulationRunning()));
	ui->qqPauseOnScriptError->setEnabled(noEditMode&&(!App::ct->simulation->isSimulationRunning()));

	ui->qqResetScene->setEnabled(noEditMode);
	ui->qqResetScene->setChecked(App::ct->simulation->getResetSceneAtSimulationEnd());

	ui->qqRemoveNewObjects->setEnabled(noEditMode);
	ui->qqRemoveNewObjects->setChecked(App::ct->simulation->getRemoveNewObjectsAtSimulationEnd());


	ui->qqPauseTime->setText(gv::getTimeStr(App::ct->simulation->getPauseTime()).c_str());
	ui->qqPauseWhenTimeHigher->setChecked(App::ct->simulation->getPauseEnabled());
	ui->qqPauseOnScriptError->setChecked(App::ct->simulation->getPauseAtError());

	inMainRefreshRoutine=false;
}

void CQDlgSimulation::on_qqTimeStep_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (App::ct->simulation->isSimulationStopped())
		{
			bool ok;
			float newVal=ui->qqTimeStep->text().toFloat(&ok);
			if (ok)
			{
				App::ct->simulation->setSimulationTimeStep_raw(newVal*gv::userToSeconds);
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			}
		}
		refresh();
	}
}

void CQDlgSimulation::on_qqScriptExecutionPasses_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		int newVal=ui->qqScriptExecutionPasses->text().toInt(&ok);
		if (ok)
		{
			App::ct->simulation->setSimulationPassesPerRendering_raw(newVal);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgSimulation::on_qqRealTime_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (App::ct->simulation->isSimulationStopped())
		{
			App::ct->simulation->setRealTimeSimulation(!App::ct->simulation->getRealTimeSimulation());
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgSimulation::on_qqMultiplicationCoefficient_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (App::ct->simulation->isSimulationStopped())
		{
			bool ok;
			float newVal=ui->qqMultiplicationCoefficient->text().toFloat(&ok);
			if (ok)
			{
				App::ct->simulation->setRealTimeCoefficient_raw(newVal);
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			}
		}
		refresh();
	}
}

void CQDlgSimulation::on_qqCatchUp_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (App::ct->simulation->isSimulationStopped())
		{
			App::ct->simulation->setCatchUpIfLate(!App::ct->simulation->getCatchUpIfLate());
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgSimulation::on_qqPauseWhenTimeHigher_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->simulation->enablePause(!App::ct->simulation->getPauseEnabled());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgSimulation::on_qqPauseTime_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqPauseTime->text().toFloat(&ok);
		if (ok)
		{
			App::ct->simulation->setPauseTime(newVal);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgSimulation::on_qqPauseOnScriptError_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->simulation->setPauseAtError(!App::ct->simulation->getPauseAtError());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgSimulation::on_qqResetScene_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->simulation->setResetSceneAtSimulationEnd(!App::ct->simulation->getResetSceneAtSimulationEnd());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgSimulation::on_qqRemoveNewObjects_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->simulation->setRemoveNewObjectsAtSimulationEnd(!App::ct->simulation->getRemoveNewObjectsAtSimulationEnd());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgSimulation::on_qqConfigCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
		{
			int config=ui->qqConfigCombo->itemData(ui->qqConfigCombo->currentIndex()).toInt();
			App::ct->simulation->setDefaultSimulationParameterIndex(config);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			refresh();
		}
	}
}

void CQDlgSimulation::on_qqFullscreen_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->simulation->setFullscreenAtSimulationStart(!App::ct->simulation->getFullscreenAtSimulationStart());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}
