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
#include "qdlguirolledup.h"
#include "ui_qdlguirolledup.h"
#include "Tt.h"
#include "App.h"

CQDlgUiRolledUp::CQDlgUiRolledUp(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgUiRolledUp)
{
    ui->setupUi(this);
}

CQDlgUiRolledUp::~CQDlgUiRolledUp()
{
    delete ui;
}

void CQDlgUiRolledUp::cancelEvent()
{
//	defaultModalDialogEndRoutine(false);
}

void CQDlgUiRolledUp::okEvent()
{
//	defaultModalDialogEndRoutine(true);
}

void CQDlgUiRolledUp::refresh()
{
	VPoint s;
	itBlock->getRollupMin(s);
	ui->qqFirstH->setText(tt::FNb(0,s.x+1,false).c_str());
	ui->qqFirstV->setText(tt::FNb(0,s.y+1,false).c_str());
	itBlock->getRollupMax(s);
	ui->qqLastH->setText(tt::FNb(0,s.x+1,false).c_str());
	ui->qqLastV->setText(tt::FNb(0,s.y+1,false).c_str());
}

void CQDlgUiRolledUp::on_qqFirstH_editingFinished()
{
	bool ok;
	int newVal=ui->qqFirstH->text().toInt(&ok);
	if (ok)
	{
		VPoint size;
		itBlock->getRollupMin(size);
		size.x=newVal-1;
		itBlock->setRollupMin(size);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgUiRolledUp::on_qqLastH_editingFinished()
{
	bool ok;
	int newVal=ui->qqLastH->text().toInt(&ok);
	if (ok)
	{
		VPoint size;
		itBlock->getRollupMax(size);
		size.x=newVal-1;
		itBlock->setRollupMax(size);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgUiRolledUp::on_qqFirstV_editingFinished()
{
	bool ok;
	int newVal=ui->qqFirstV->text().toInt(&ok);
	if (ok)
	{
		VPoint size;
		itBlock->getRollupMin(size);
		size.y=newVal-1;
		itBlock->setRollupMin(size);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgUiRolledUp::on_qqLastV_editingFinished()
{
	bool ok;
	int newVal=ui->qqLastV->text().toInt(&ok);
	if (ok)
	{
		VPoint size;
		itBlock->getRollupMax(size);
		size.y=newVal-1;
		itBlock->setRollupMax(size);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgUiRolledUp::on_qqClose_clicked(QAbstractButton *button)
{
	defaultModalDialogEndRoutine(true);
}
