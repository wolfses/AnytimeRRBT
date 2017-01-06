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
#include "qdlgpathplanningaddnew.h"
#include "ui_qdlgpathplanningaddnew.h"
#include "GV.h"
#include "Tt.h"
#include "App.h"
#include "v_repStrings.h"

CQDlgPathPlanningAddNew::CQDlgPathPlanningAddNew(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgPathPlanningAddNew)
{
    ui->setupUi(this);
}

CQDlgPathPlanningAddNew::~CQDlgPathPlanningAddNew()
{
    delete ui;
}

void CQDlgPathPlanningAddNew::cancelEvent()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgPathPlanningAddNew::okEvent()
{
	_getValues();
	defaultModalDialogEndRoutine(true);
}

void CQDlgPathPlanningAddNew::refresh()
{
	ui->qqTaskType->clear();
	ui->qqTaskType->addItem(strTranslate(IDSN_HOLONOMIC),QVariant(sim_pathplanning_holonomic));
	ui->qqTaskType->addItem(strTranslate(IDSN_NON_HOLONOMIC),QVariant(sim_pathplanning_nonholonomic));
	ui->qqTaskType->setCurrentIndex(0);

	std::vector<std::string> names;
	std::vector<int> ids;

	ui->qqDummy->clear();
	names.clear();
	ids.clear();
	for (int i=0;i<int(App::ct->objCont->dummyList.size());i++)
	{
		CDummy* it2=App::ct->objCont->getDummy(App::ct->objCont->dummyList[i]);
		names.push_back(it2->getName());
		ids.push_back(it2->getID());
	}
	tt::orderStrings(names,ids);
	for (int i=0;i<int(names.size());i++)
		ui->qqDummy->addItem(names[i].c_str(),QVariant(ids[i]));
	ui->qqDummy->setCurrentIndex(0);

}

void CQDlgPathPlanningAddNew::_getValues()
{
	selectedTaskType=ui->qqTaskType->itemData(ui->qqTaskType->currentIndex()).toInt();
	selectedDummy=ui->qqDummy->itemData(ui->qqDummy->currentIndex()).toInt();
}

void CQDlgPathPlanningAddNew::on_qqOkCancel_accepted()
{
	_getValues();
	defaultModalDialogEndRoutine(true);
}

void CQDlgPathPlanningAddNew::on_qqOkCancel_rejected()
{
	defaultModalDialogEndRoutine(false);
}
