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
#include "qdlgnewui.h"
#include "ui_qdlgnewui.h"
#include "v_rep_internal.h"
#include "Tt.h"
#include "App.h"

CQDlgNewUi::CQDlgNewUi(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgNewUi)
{
    ui->setupUi(this);
	xSize=10;
	ySize=8;
	menuBarAttributes=sim_ui_menu_title|sim_ui_menu_minimize|sim_ui_menu_close;
}

CQDlgNewUi::~CQDlgNewUi()
{
    delete ui;
}

void CQDlgNewUi::cancelEvent()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgNewUi::okEvent()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgNewUi::refresh()
{
	ui->qqSizeX->setText(tt::FNb(0,xSize,false).c_str());
	ui->qqSizeY->setText(tt::FNb(0,ySize,false).c_str());
	ui->qqMinimizeMaximize->setEnabled(menuBarAttributes&sim_ui_menu_title);
	ui->qqCloseButton->setEnabled(menuBarAttributes&sim_ui_menu_title);
	ui->qqTitlebar->setChecked(menuBarAttributes&sim_ui_menu_title);
	ui->qqMinimizeMaximize->setChecked(menuBarAttributes&sim_ui_menu_minimize);
	ui->qqCloseButton->setChecked(menuBarAttributes&sim_ui_menu_close);
}

void CQDlgNewUi::on_qqTitlebar_clicked()
{
	menuBarAttributes^=sim_ui_menu_title;
	if ((menuBarAttributes&sim_ui_menu_title)==0)
		menuBarAttributes=0;
	refresh();
}

void CQDlgNewUi::on_qqMinimizeMaximize_clicked()
{
	menuBarAttributes^=sim_ui_menu_minimize;
	refresh();
}

void CQDlgNewUi::on_qqCloseButton_clicked()
{
	menuBarAttributes^=sim_ui_menu_close;
	refresh();
}

void CQDlgNewUi::on_qqSizeX_editingFinished()
{
	bool ok;
	int newVal=ui->qqSizeX->text().toInt(&ok);
	if (ok)
	{
		tt::limitValue(3,256,newVal);
		xSize=newVal;
	}
	refresh();
}

void CQDlgNewUi::on_qqSizeY_editingFinished()
{
	bool ok;
	int newVal=ui->qqSizeY->text().toInt(&ok);
	if (ok)
	{
		tt::limitValue(2,256,newVal);
		ySize=newVal;
	}
	refresh();
}

void CQDlgNewUi::on_qqOkCancel_accepted()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgNewUi::on_qqOkCancel_rejected()
{
	defaultModalDialogEndRoutine(false);
}
