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
#include "qdlgikconditional.h"
#include "ui_qdlgikconditional.h"
#include "App.h"
#include "Tt.h"
#include "v_repStrings.h"

CQDlgIkConditional::CQDlgIkConditional(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgIkConditional)
{
    ui->setupUi(this);
	firstHere=true;
	inMainRefreshRoutine=false;
}

CQDlgIkConditional::~CQDlgIkConditional()
{
    delete ui;
}

void CQDlgIkConditional::cancelEvent()
{
//	defaultModalDialogEndRoutine(false);
}

void CQDlgIkConditional::okEvent()
{
//	defaultModalDialogEndRoutine(true);
}

void CQDlgIkConditional::refresh()
{
	inMainRefreshRoutine=true;
	std::vector<std::string> names;
	std::vector<int> ids;
	if (firstHere)
	{
		firstHere=false;

		ui->qqIkGroupCombo->clear();
		ui->qqIkGroupCombo->addItem(strTranslate(IDSN_PERFORM_ALWAYS),QVariant(-1));
		for (int i=0;i<int(App::ct->ikGroups->ikGroups.size());i++)
		{
			CikGroup* it=App::ct->ikGroups->ikGroups[i];
			if (it->getObjectID()==ikGroup->getObjectID())
				break; // If no ik group comes before, we can't do anything!
			names.push_back(it->getObjectName());
			ids.push_back(it->getObjectID());
		}
		tt::orderStrings(names,ids);
		for (int i=0;i<int(names.size());i++)
			ui->qqIkGroupCombo->addItem(names[i].c_str(),QVariant(ids[i]));
	}

	for (int i=0;i<ui->qqIkGroupCombo->count();i++)
	{
		if (ui->qqIkGroupCombo->itemData(i).toInt()==ikGroup->getDoOnFailOrSuccessOf())
		{
			ui->qqIkGroupCombo->setCurrentIndex(i);
			break;
		}
	}


	ui->qqIkResultCombo->clear();
	int sel=ui->qqIkGroupCombo->itemData(ui->qqIkGroupCombo->currentIndex()).toInt();
	ui->qqIkResultCombo->setEnabled(sel!=-1);

	ui->qqFailedAngular->setChecked(ikGroup->getRestoreIfOrientationNotReached());
	ui->qqFailedLinear->setChecked(ikGroup->getRestoreIfPositionNotReached());

	if (ikGroup->getDoOnFailOrSuccessOf()!=-1)
	{
		ui->qqIkResultCombo->addItem(strTranslate(IDSN_WAS_PERFORMED_AND_FAILED),QVariant(0));
		ui->qqIkResultCombo->addItem(strTranslate(IDSN_WAS_PERFORMED_AND_SUCCEEDED),QVariant(1));
		ui->qqIkResultCombo->addItem(strTranslate(IDSN_WAS_NOT_PERFORMED),QVariant(2));

		if (!ikGroup->getDoOnPerformed())
			ui->qqIkResultCombo->setCurrentIndex(2);
		else
		{
			if (ikGroup->getDoOnFail())
				ui->qqIkResultCombo->setCurrentIndex(0);
			else
				ui->qqIkResultCombo->setCurrentIndex(1);
		}
	}

	inMainRefreshRoutine=false;
}

void CQDlgIkConditional::on_qqClose_clicked(QAbstractButton *button)
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgIkConditional::on_qqIkGroupCombo_currentIndexChanged(int index)
{
	if (!inMainRefreshRoutine)
	{
		ikGroup->setDoOnFailOrSuccessOf(ui->qqIkGroupCombo->itemData(ui->qqIkGroupCombo->currentIndex()).toInt());
		refresh();
	}
}

void CQDlgIkConditional::on_qqIkResultCombo_currentIndexChanged(int index)
{
	if (!inMainRefreshRoutine)
	{
		int v=ui->qqIkResultCombo->itemData(ui->qqIkResultCombo->currentIndex()).toInt();
		ikGroup->setDoOnPerformed(v<2);
		ikGroup->setDoOnFail(v==0);
		refresh();
	}
}

void CQDlgIkConditional::on_qqFailedLinear_clicked()
{
	ikGroup->setRestoreIfPositionNotReached(!ikGroup->getRestoreIfPositionNotReached());
	refresh();
}

void CQDlgIkConditional::on_qqFailedAngular_clicked()
{
	ikGroup->setRestoreIfOrientationNotReached(!ikGroup->getRestoreIfOrientationNotReached());
	refresh();
}
