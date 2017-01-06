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
#include "qdlgimagecolor.h"
#include "ui_qdlgimagecolor.h"
#include "Tt.h"
#include "App.h"

CQDlgImageColor::CQDlgImageColor(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgImageColor)
{
    ui->setupUi(this);
}

CQDlgImageColor::~CQDlgImageColor()
{
    delete ui;
}

void CQDlgImageColor::cancelEvent()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgImageColor::okEvent()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgImageColor::refresh()
{
	ui->qqFromFogColor->setChecked(sameAsFog);
	ui->qqRed->setEnabled(!sameAsFog);
	ui->qqGreen->setEnabled(!sameAsFog);
	ui->qqBlue->setEnabled(!sameAsFog);
	ui->qqRed->setText(tt::FNb(0,red,2,false).c_str());
	ui->qqGreen->setText(tt::FNb(0,green,2,false).c_str());
	ui->qqBlue->setText(tt::FNb(0,blue,2,false).c_str());
}

void CQDlgImageColor::on_qqFromFogColor_clicked()
{
	sameAsFog=!sameAsFog;
	refresh();
}

void CQDlgImageColor::on_qqRed_editingFinished()
{
	bool ok;
	float newVal=ui->qqRed->text().toFloat(&ok);
	if (ok)
		red=tt::getLimitedFloat(0.0f,1.0f,newVal);
	refresh();
}

void CQDlgImageColor::on_qqGreen_editingFinished()
{
	bool ok;
	float newVal=ui->qqGreen->text().toFloat(&ok);
	if (ok)
		green=tt::getLimitedFloat(0.0f,1.0f,newVal);
	refresh();
}

void CQDlgImageColor::on_qqBlue_editingFinished()
{
	bool ok;
	float newVal=ui->qqBlue->text().toFloat(&ok);
	if (ok)
		blue=tt::getLimitedFloat(0.0f,1.0f,newVal);
	refresh();
}

void CQDlgImageColor::on_qqOkCancel_accepted()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgImageColor::on_qqOkCancel_rejected()
{
	defaultModalDialogEndRoutine(false);
}
