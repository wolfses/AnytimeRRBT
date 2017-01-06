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
#include "qdlgdynamics.h"
#include "ui_qdlgdynamics.h"
#include "Tt.h"
#include "App.h"
#include "GV.h"
#include "propBrowser_engineProp_general.h"
#include "v_repStrings.h"

CQDlgDynamics::CQDlgDynamics(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgDynamics)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
}

CQDlgDynamics::~CQDlgDynamics()
{
    delete ui;
}

void CQDlgDynamics::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(CALCULATION_DLG);
}

void CQDlgDynamics::refresh()
{
	inMainRefreshRoutine=true;

	bool en=App::ct->dynamicsContainer->getDynamicsEnabled();
	bool stopped=App::ct->simulation->isSimulationStopped();
	ui->qqEnabled->setChecked(en);
	ui->qqEngineCombo->clear();

	ui->qqEngineCombo->setEnabled(stopped&&en);
	ui->qqContactPoints->setEnabled(en);
	ui->qqAdjustEngine->setEnabled(stopped&&en);
	ui->qqGravityX->setEnabled(en);
	ui->qqGravityY->setEnabled(en);
	ui->qqGravityZ->setEnabled(en);


	ui->qqEngineCombo->addItem(strTranslate(IDS_BULLET),QVariant(sim_physics_bullet));
	ui->qqEngineCombo->addItem(strTranslate(IDS_ODE),QVariant(sim_physics_ode));
	ui->qqEngineCombo->addItem(strTranslate(IDS_VORTEX),QVariant(sim_physics_vortex));
	if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_bullet)
		ui->qqEngineCombo->setCurrentIndex(0);
	if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_ode)
		ui->qqEngineCombo->setCurrentIndex(1);
	if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_vortex)
		ui->qqEngineCombo->setCurrentIndex(2);

	ui->qqContactPoints->setChecked(App::ct->dynamicsContainer->getDisplayContactPoints());

	C3Vector accel(App::ct->dynamicsContainer->getGravity());
	ui->qqGravityX->setText(gv::getLinearAccelStr(true,accel(0)).c_str());
	ui->qqGravityY->setText(gv::getLinearAccelStr(true,accel(1)).c_str());
	ui->qqGravityZ->setText(gv::getLinearAccelStr(true,accel(2)).c_str());

	inMainRefreshRoutine=false;
}

void CQDlgDynamics::on_qqEnabled_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		App::ct->dynamicsContainer->setDynamicsEnabled(!App::ct->dynamicsContainer->getDynamicsEnabled());
		App::ct->dynamicsContainer->removeWorld();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgDynamics::on_qqEngineCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
		{
			int engine=ui->qqEngineCombo->itemData(ui->qqEngineCombo->currentIndex()).toInt();
			App::ct->dynamicsContainer->setDynamicEngineType(engine);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgDynamics::on_qqContactPoints_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->dynamicsContainer->setDisplayContactPoints(!App::ct->dynamicsContainer->getDisplayContactPoints());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgDynamics::on_qqAdjustEngine_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CPropBrowserEngineGeneral dlg(App::mainWindow);
		dlg.setModal(true);
		dlg.exec();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgDynamics::on_qqGravityX_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqGravityX->text().toFloat(&ok);
		if (ok)
		{
			C3Vector vect=App::ct->dynamicsContainer->getGravity();
			vect(0)=gv::userToMeter*newVal/(gv::userToSeconds*gv::userToSeconds);
			App::ct->dynamicsContainer->setGravity(vect);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgDynamics::on_qqGravityY_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqGravityY->text().toFloat(&ok);
		if (ok)
		{
			C3Vector vect=App::ct->dynamicsContainer->getGravity();
			vect(1)=gv::userToMeter*newVal/(gv::userToSeconds*gv::userToSeconds);
			App::ct->dynamicsContainer->setGravity(vect);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgDynamics::on_qqGravityZ_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqGravityZ->text().toFloat(&ok);
		if (ok)
		{
			C3Vector vect=App::ct->dynamicsContainer->getGravity();
			vect(2)=gv::userToMeter*newVal/(gv::userToSeconds*gv::userToSeconds);
			App::ct->dynamicsContainer->setGravity(vect);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}
