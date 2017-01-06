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
#include "qdlgpathplanningparams.h"
#include "ui_qdlgpathplanningparams.h"
#include "GV.h"
#include "Tt.h"
#include "App.h"

CQDlgPathPlanningParams::CQDlgPathPlanningParams(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgPathPlanningParams)
{
    ui->setupUi(this);
}

CQDlgPathPlanningParams::~CQDlgPathPlanningParams()
{
    delete ui;
}

void CQDlgPathPlanningParams::cancelEvent()
{
//	defaultModalDialogEndRoutine(false);
}

void CQDlgPathPlanningParams::okEvent()
{
//	defaultModalDialogEndRoutine(true);
}

void CQDlgPathPlanningParams::refresh()
{
	inMainRefreshRoutine=true;
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	int t=task->getPathPlanningType();
	int ht=task->getHolonomicTaskType();
	ui->qqHoloTypeCombo->setEnabled(t==sim_pathplanning_holonomic);
	ui->qqDeltaX->setEnabled(t==sim_pathplanning_holonomic);
	ui->qqDeltaY->setEnabled(t==sim_pathplanning_holonomic);
	ui->qqdeltaZ->setEnabled(t==sim_pathplanning_holonomic);
	ui->qqStepSizeA->setEnabled(t==sim_pathplanning_holonomic);
	ui->qqMinTurningCircle->setEnabled(t==sim_pathplanning_nonholonomic);


	ui->qqMinX->setEnabled((t==sim_pathplanning_nonholonomic)||(ht!=sim_holonomicpathplanning_abg));
	ui->qqMinX_2->setEnabled((t==sim_pathplanning_nonholonomic)||(ht!=sim_holonomicpathplanning_abg));
	ui->qqMinusX->setEnabled((t==sim_pathplanning_holonomic)&&(ht!=sim_holonomicpathplanning_abg));
	ui->qqPlusX->setEnabled((t==sim_pathplanning_holonomic)&&(ht!=sim_holonomicpathplanning_abg));

	ui->qqMinY->setEnabled((t==sim_pathplanning_nonholonomic)||((ht!=sim_holonomicpathplanning_xg)&&(ht!=sim_holonomicpathplanning_xg)&&(ht!=sim_holonomicpathplanning_abg)&&(ht!=sim_holonomicpathplanning_xabg)));
	ui->qqMinY_2->setEnabled((t==sim_pathplanning_nonholonomic)||((ht!=sim_holonomicpathplanning_xg)&&(ht!=sim_holonomicpathplanning_xg)&&(ht!=sim_holonomicpathplanning_abg)&&(ht!=sim_holonomicpathplanning_xabg)));
	ui->qqMinusY->setEnabled((t==sim_pathplanning_holonomic)&&((ht!=sim_holonomicpathplanning_xg)&&(ht!=sim_holonomicpathplanning_xg)&&(ht!=sim_holonomicpathplanning_abg)&&(ht!=sim_holonomicpathplanning_xabg)));
	ui->qqPlusY->setEnabled((t==sim_pathplanning_holonomic)&&((ht!=sim_holonomicpathplanning_xg)&&(ht!=sim_holonomicpathplanning_xg)&&(ht!=sim_holonomicpathplanning_abg)&&(ht!=sim_holonomicpathplanning_xabg)));

	ui->qqMinZ->setEnabled((t==sim_pathplanning_holonomic)&&((ht==sim_holonomicpathplanning_xyz)||(ht==sim_holonomicpathplanning_xyzg)||(ht==sim_holonomicpathplanning_xyzabg)));
	ui->qqMinZ_2->setEnabled((t==sim_pathplanning_holonomic)&&((ht==sim_holonomicpathplanning_xyz)||(ht==sim_holonomicpathplanning_xyzg)||(ht==sim_holonomicpathplanning_xyzabg)));
	ui->qqMinusZ->setEnabled((t==sim_pathplanning_holonomic)&&((ht==sim_holonomicpathplanning_xyz)||(ht==sim_holonomicpathplanning_xyzg)||(ht==sim_holonomicpathplanning_xyzabg)));
	ui->qqPlusZ->setEnabled((t==sim_pathplanning_holonomic)&&((ht==sim_holonomicpathplanning_xyz)||(ht==sim_holonomicpathplanning_xyzg)||(ht==sim_holonomicpathplanning_xyzabg)));

	ui->qqMinD->setEnabled((t==sim_pathplanning_holonomic)&&((ht==sim_holonomicpathplanning_xg)||(ht==sim_holonomicpathplanning_xyg)||(ht==sim_holonomicpathplanning_xyzg)));
	ui->qqMinD_2->setEnabled((t==sim_pathplanning_holonomic)&&((ht==sim_holonomicpathplanning_xg)||(ht==sim_holonomicpathplanning_xyg)||(ht==sim_holonomicpathplanning_xyzg)));
	ui->qqMinusD->setEnabled((t==sim_pathplanning_holonomic)&&((ht==sim_holonomicpathplanning_xg)||(ht==sim_holonomicpathplanning_xyg)||(ht==sim_holonomicpathplanning_xyzg)));
	ui->qqPlusD->setEnabled((t==sim_pathplanning_holonomic)&&((ht==sim_holonomicpathplanning_xg)||(ht==sim_holonomicpathplanning_xyg)||(ht==sim_holonomicpathplanning_xyzg)));

	ui->qqVisualizeArea->setChecked(task->getVisualizeSearchArea());

	ui->qqHoloTypeCombo->clear();

	float mMin[4];
	float mMax[4];
	task->getSearchRange(mMin,mMax);
	int dir[4];
	task->getSearchDirection(dir);

	ui->qqStepSizeL->setText(gv::getSizeStr(false,task->getStepSize(),0).c_str());

	if (t==sim_pathplanning_holonomic)
	{
		const char* texts[9]={strTranslate("X-Y"),strTranslate("X-Delta"),strTranslate("X-Y-Z"),strTranslate("X-Y-Delta"),strTranslate("Alpha-Beta-Gamma"),
						strTranslate("X-Y-Z-Delta"),strTranslate("X-Alpha-Beta-Gamma"),strTranslate("X-Y-Alpha-Beta-Gamma"),
						strTranslate("X-Y-Z-Alpha-Beta-Gamma")};
		const int datas[9]={sim_holonomicpathplanning_xy,sim_holonomicpathplanning_xg,sim_holonomicpathplanning_xyz,sim_holonomicpathplanning_xyg,
						sim_holonomicpathplanning_abg,sim_holonomicpathplanning_xyzg,sim_holonomicpathplanning_xabg,
						sim_holonomicpathplanning_xyabg,sim_holonomicpathplanning_xyzabg};
		for (int i=0;i<9;i++)
			ui->qqHoloTypeCombo->addItem(texts[i],QVariant(datas[i]));

		for (int i=0;i<ui->qqHoloTypeCombo->count();i++)
		{
			if (ui->qqHoloTypeCombo->itemData(i).toInt()==task->getHolonomicTaskType())
			{
				ui->qqHoloTypeCombo->setCurrentIndex(i);
				break;
			}
		}

		C3Vector a(task->getGammaAxis());

		ui->qqDeltaX->setText(tt::FNb(0,a(0),2,true).c_str());
		ui->qqDeltaY->setText(tt::FNb(0,a(1),2,true).c_str());
		ui->qqdeltaZ->setText(tt::FNb(0,a(2),2,true).c_str());
		ui->qqStepSizeA->setText(gv::getAngleStr(false,task->getAngularStepSize(),0).c_str());
		ui->qqMinTurningCircle->setText("");
	}
	else
	{
		ui->qqDeltaX->setText("");
		ui->qqDeltaY->setText("");
		ui->qqdeltaZ->setText("");
		ui->qqStepSizeA->setText("");
		ui->qqMinTurningCircle->setText(gv::getSizeStr(false,task->getMinTurningCircleDiameter(),0).c_str());
	}

	if ((t==sim_pathplanning_nonholonomic)||(ht!=sim_holonomicpathplanning_abg))
	{
		ui->qqMinX->setText(gv::getSizeStr(true,mMin[0],0).c_str());
		ui->qqMinX_2->setText(gv::getSizeStr(true,mMax[0],0).c_str());
		if (t==sim_pathplanning_nonholonomic)
		{
			ui->qqMinusX->setChecked(false);
			ui->qqPlusX->setChecked(false);
		}
		else
		{
			ui->qqMinusX->setChecked(dir[0]<=0);
			ui->qqPlusX->setChecked(dir[0]>=0);
		}
	}
	else
	{
		ui->qqMinX->setText("");
		ui->qqMinX_2->setText("");
		ui->qqMinusX->setChecked(false);
		ui->qqPlusX->setChecked(false);
	}

	if ((t==sim_pathplanning_nonholonomic)||((ht!=sim_holonomicpathplanning_xg)&&(ht!=sim_holonomicpathplanning_xg)&&(ht!=sim_holonomicpathplanning_abg)&&(ht!=sim_holonomicpathplanning_xabg)))
	{
		ui->qqMinY->setText(gv::getSizeStr(true,mMin[1],0).c_str());
		ui->qqMinY_2->setText(gv::getSizeStr(true,mMax[1],0).c_str());
		if (t==sim_pathplanning_nonholonomic)
		{
			ui->qqMinusY->setChecked(false);
			ui->qqPlusY->setChecked(false);
		}
		else
		{
			ui->qqMinusY->setChecked(dir[1]<=0);
			ui->qqPlusY->setChecked(dir[1]>=0);
		}
	}
	else
	{
		ui->qqMinY->setText("");
		ui->qqMinY_2->setText("");
		ui->qqMinusY->setChecked(false);
		ui->qqPlusY->setChecked(false);
	}

	if ((t==sim_pathplanning_holonomic)&&((ht==sim_holonomicpathplanning_xyz)||(ht==sim_holonomicpathplanning_xyzg)||(ht==sim_holonomicpathplanning_xyzabg)))
	{
		ui->qqMinZ->setText(gv::getSizeStr(true,mMin[2],0).c_str());
		ui->qqMinZ_2->setText(gv::getSizeStr(true,mMax[2],0).c_str());
		ui->qqMinusZ->setChecked(dir[2]<=0);
		ui->qqPlusZ->setChecked(dir[2]>=0);
	}
	else
	{
		ui->qqMinZ->setText("");
		ui->qqMinZ_2->setText("");
		ui->qqMinusZ->setChecked(false);
		ui->qqPlusZ->setChecked(false);
	}


	if ((t==sim_pathplanning_holonomic)&&((ht==sim_holonomicpathplanning_xg)||(ht==sim_holonomicpathplanning_xyg)||(ht==sim_holonomicpathplanning_xyzg)))
	{
		ui->qqMinD->setText(gv::getSizeStr(true,mMin[3],0).c_str());
		ui->qqMinD_2->setText(gv::getSizeStr(true,mMax[3],0).c_str());
		ui->qqMinusD->setChecked(dir[3]<=0);
		ui->qqPlusD->setChecked(dir[3]>=0);
	}
	else
	{
		ui->qqMinD->setText("");
		ui->qqMinD_2->setText("");
		ui->qqMinusD->setChecked(false);
		ui->qqPlusD->setChecked(false);
	}

	inMainRefreshRoutine=false;
}

void CQDlgPathPlanningParams::on_qqClose_clicked(QAbstractButton *button)
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgPathPlanningParams::on_qqHoloTypeCombo_currentIndexChanged(int index)
{
	if (!inMainRefreshRoutine)
	{
		CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
		task->setHolonomicTaskType(ui->qqHoloTypeCombo->itemData(ui->qqHoloTypeCombo->currentIndex()).toInt());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgPathPlanningParams::on_qqDeltaX_editingFinished()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	bool ok;
	float newVal=ui->qqDeltaX->text().toFloat(&ok);
	if (ok)
	{
		C3Vector a(task->getGammaAxis());
		a(0)=newVal;
		task->setGammaAxis(a);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgPathPlanningParams::on_qqDeltaY_editingFinished()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	bool ok;
	float newVal=ui->qqDeltaY->text().toFloat(&ok);
	if (ok)
	{
		C3Vector a(task->getGammaAxis());
		a(1)=newVal;
		task->setGammaAxis(a);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgPathPlanningParams::on_qqdeltaZ_editingFinished()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	bool ok;
	float newVal=ui->qqdeltaZ->text().toFloat(&ok);
	if (ok)
	{
		C3Vector a(task->getGammaAxis());
		a(2)=newVal;
		task->setGammaAxis(a);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgPathPlanningParams::on_qqStepSizeL_editingFinished()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	bool ok;
	float newVal=ui->qqStepSizeL->text().toFloat(&ok);
	if (ok)
	{
		task->setStepSize(newVal*gv::userToMeter);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgPathPlanningParams::on_qqStepSizeA_editingFinished()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	bool ok;
	float newVal=ui->qqStepSizeA->text().toFloat(&ok);
	if (ok)
	{
		task->setAngularStepSize(newVal*gv::userToRad);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgPathPlanningParams::on_qqMinTurningCircle_editingFinished()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	bool ok;
	float newVal=ui->qqMinTurningCircle->text().toFloat(&ok);
	if (ok)
	{
		task->setMinTurningCircleDiameter(newVal*gv::userToMeter);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgPathPlanningParams::on_qqVisualizeArea_clicked()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	task->setVisualizeSearchArea(!task->getVisualizeSearchArea());
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();
}

void CQDlgPathPlanningParams::on_qqMinX_editingFinished()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	bool ok;
	float newVal=ui->qqMinX->text().toFloat(&ok);
	if (ok)
	{
		float mMin[4];
		float mMax[4];
		task->getSearchRange(mMin,mMax);
		mMin[0]=newVal*gv::userToMeter;
		task->setSearchRange(mMin,mMax);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgPathPlanningParams::on_qqMinY_editingFinished()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	bool ok;
	float newVal=ui->qqMinY->text().toFloat(&ok);
	if (ok)
	{
		float mMin[4];
		float mMax[4];
		task->getSearchRange(mMin,mMax);
		mMin[1]=newVal*gv::userToMeter;
		task->setSearchRange(mMin,mMax);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgPathPlanningParams::on_qqMinZ_editingFinished()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	bool ok;
	float newVal=ui->qqMinZ->text().toFloat(&ok);
	if (ok)
	{
		float mMin[4];
		float mMax[4];
		task->getSearchRange(mMin,mMax);
		mMin[2]=newVal*gv::userToMeter;
		task->setSearchRange(mMin,mMax);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgPathPlanningParams::on_qqMinD_editingFinished()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	bool ok;
	float newVal=ui->qqMinD->text().toFloat(&ok);
	if (ok)
	{
		float mMin[4];
		float mMax[4];
		task->getSearchRange(mMin,mMax);
		mMin[3]=newVal*gv::userToRad;
		task->setSearchRange(mMin,mMax);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgPathPlanningParams::on_qqMinX_2_editingFinished()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	bool ok;
	float newVal=ui->qqMinX_2->text().toFloat(&ok);
	if (ok)
	{
		float mMin[4];
		float mMax[4];
		task->getSearchRange(mMin,mMax);
		mMax[0]=newVal*gv::userToMeter;
		task->setSearchRange(mMin,mMax);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgPathPlanningParams::on_qqMinY_2_editingFinished()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	bool ok;
	float newVal=ui->qqMinY_2->text().toFloat(&ok);
	if (ok)
	{
		float mMin[4];
		float mMax[4];
		task->getSearchRange(mMin,mMax);
		mMax[1]=newVal*gv::userToMeter;
		task->setSearchRange(mMin,mMax);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgPathPlanningParams::on_qqMinZ_2_editingFinished()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	bool ok;
	float newVal=ui->qqMinZ_2->text().toFloat(&ok);
	if (ok)
	{
		float mMin[4];
		float mMax[4];
		task->getSearchRange(mMin,mMax);
		mMax[2]=newVal*gv::userToMeter;
		task->setSearchRange(mMin,mMax);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgPathPlanningParams::on_qqMinD_2_editingFinished()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	bool ok;
	float newVal=ui->qqMinD_2->text().toFloat(&ok);
	if (ok)
	{
		float mMin[4];
		float mMax[4];
		task->getSearchRange(mMin,mMax);
		mMax[3]=newVal*gv::userToRad;
		task->setSearchRange(mMin,mMax);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgPathPlanningParams::on_qqMinusX_clicked()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	int dir[4];
	task->getSearchDirection(dir);
	if (dir[0]==0)
		dir[0]=1;
	else
	{
		if (dir[0]>0)
			dir[0]=0;
		else
			dir[0]=1;
	}
	task->setSearchDirection(dir);
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();
}

void CQDlgPathPlanningParams::on_qqPlusX_clicked()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	int dir[4];
	task->getSearchDirection(dir);
	if (dir[0]==0)
		dir[0]=-1;
	else
	{
		if (dir[0]<0)
			dir[0]=0;
		else
			dir[0]=-1;
	}
	task->setSearchDirection(dir);
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();
}

void CQDlgPathPlanningParams::on_qqMinusY_clicked()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	int dir[4];
	task->getSearchDirection(dir);
	if (dir[1]==0)
		dir[1]=1;
	else
	{
		if (dir[1]>0)
			dir[1]=0;
		else
			dir[1]=1;
	}
	task->setSearchDirection(dir);
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();
}

void CQDlgPathPlanningParams::on_qqPlusY_clicked()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	int dir[4];
	task->getSearchDirection(dir);
	if (dir[1]==0)
		dir[1]=-1;
	else
	{
		if (dir[1]<0)
			dir[1]=0;
		else
			dir[1]=-1;
	}
	task->setSearchDirection(dir);
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();
}

void CQDlgPathPlanningParams::on_qqMinusZ_clicked()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	int dir[4];
	task->getSearchDirection(dir);
	if (dir[2]==0)
		dir[2]=1;
	else
	{
		if (dir[2]>0)
			dir[2]=0;
		else
			dir[2]=1;
	}
	task->setSearchDirection(dir);
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();
}

void CQDlgPathPlanningParams::on_qqPlusZ_clicked()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	int dir[4];
	task->getSearchDirection(dir);
	if (dir[2]==0)
		dir[2]=-1;
	else
	{
		if (dir[2]<0)
			dir[2]=0;
		else
			dir[2]=-1;
	}
	task->setSearchDirection(dir);
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();
}

void CQDlgPathPlanningParams::on_qqMinusD_clicked()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	int dir[4];
	task->getSearchDirection(dir);
	if (dir[3]==0)
		dir[3]=1;
	else
	{
		if (dir[3]>0)
			dir[3]=0;
		else
			dir[3]=1;
	}
	task->setSearchDirection(dir);
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();
}

void CQDlgPathPlanningParams::on_qqPlusD_clicked()
{
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);
	int dir[4];
	task->getSearchDirection(dir);
	if (dir[3]==0)
		dir[3]=-1;
	else
	{
		if (dir[3]<0)
			dir[3]=0;
		else
			dir[3]=-1;
	}
	task->setSearchDirection(dir);
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();
}
