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
#include "qdlgdependencyequation.h"
#include "ui_qdlgdependencyequation.h"
#include "GV.h"
#include "Tt.h"
#include "App.h"
#include "v_repStrings.h"

CQDlgDependencyEquation::CQDlgDependencyEquation(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgDependencyEquation)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
}

CQDlgDependencyEquation::~CQDlgDependencyEquation()
{
    delete ui;
}

void CQDlgDependencyEquation::cancelEvent()
{ // no cancel event allowed
	//defaultModalDialogEndRoutine(false);
}

void CQDlgDependencyEquation::okEvent()
{ // no ok event allowed
	//defaultModalDialogEndRoutine(false);
}

void CQDlgDependencyEquation::refresh()
{
	inMainRefreshRoutine=true;
	bool sel=App::ct->objCont->isLastSelectionAJoint();
	bool dependencyPartActive=false;
	CJoint* it=NULL;
	if (sel)
	{
		it=(CJoint*)App::ct->objCont->getLastSelection();
		dependencyPartActive=((it->getJointMode()==sim_jointmode_dependent)||(it->getJointMode()==sim_jointmode_ikdependent));
	}

	ui->qqOffset->setEnabled(dependencyPartActive&&(it->getDependencyJointID()!=-1));
	ui->qqCoeff->setEnabled(dependencyPartActive&&(it->getDependencyJointID()!=-1));
	ui->qqCombo->setEnabled(dependencyPartActive);
	ui->qqCombo->clear();

	if (sel)
	{
		if (it->getJointType()==sim_joint_revolute_subtype)
		{
			ui->qqOffset->setText(tt::FNb(3,it->getDependencyJointFact(),4,true).c_str());
			ui->qqCoeff->setText(tt::FNb(0,it->getDependencyJointCoeff(),4).c_str());
		}
		else
		{
			ui->qqOffset->setText(tt::FNb(3,it->getDependencyJointFact(),4,true).c_str());
			ui->qqCoeff->setText(tt::FNb(0,it->getDependencyJointCoeff(),4).c_str());
		}

		ui->qqCombo->addItem(strTranslate(IDSN_NONE),QVariant(-1));

		std::vector<std::string> names;
		std::vector<int> ids;

		// Joints:
		for (int i=0;i<int(App::ct->objCont->jointList.size());i++)
		{
			CJoint* it2=App::ct->objCont->getJoint(App::ct->objCont->jointList[i]);
			if ((it2!=it)&&(it2->getJointType()!=sim_joint_spherical_subtype))
			{
				std::string name(tt::decorateString("[",strTranslate(IDSN_JOINT),"] "));
				name+=it2->getName();
				names.push_back(name);
				ids.push_back(it2->getID());
			}
		}
		tt::orderStrings(names,ids);
		for (int i=0;i<int(names.size());i++)
			ui->qqCombo->addItem(names[i].c_str(),QVariant(ids[i]));

		// Select current item:
		for (int i=0;i<ui->qqCombo->count();i++)
		{
			if (ui->qqCombo->itemData(i).toInt()==it->getDependencyJointID())
			{
				ui->qqCombo->setCurrentIndex(i);
				break;
			}
		}
	}
	else
	{
		ui->qqOffset->setText("");
		ui->qqCoeff->setText("");
	}

	inMainRefreshRoutine=false;
}

void CQDlgDependencyEquation::on_qqClose_clicked(QAbstractButton *button)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		defaultModalDialogEndRoutine(true);
	}
}

void CQDlgDependencyEquation::on_qqOffset_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAJoint())
			return;
		bool ok;
		float newVal=ui->qqOffset->text().toFloat(&ok);
		if (ok)
		{
			CJoint* it=(CJoint*)App::ct->objCont->getLastSelection();
			it->setDependencyJointFact(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgDependencyEquation::on_qqCoeff_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAJoint())
			return;
		bool ok;
		float newVal=ui->qqCoeff->text().toFloat(&ok);
		if (ok)
		{
			CJoint* it=(CJoint*)App::ct->objCont->getLastSelection();
			it->setDependencyJointCoeff(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgDependencyEquation::on_qqCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAJoint())
			return;
		if (!inMainRefreshRoutine)
		{
			int objID=ui->qqCombo->itemData(ui->qqCombo->currentIndex()).toInt();
			CJoint* it=(CJoint*)App::ct->objCont->getLastSelection();
			it->setDependencyJointID(objID);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}
