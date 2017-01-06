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
#include "qdlgtextureloadoptions.h"
#include "ui_qdlgtextureloadoptions.h"
#include "GV.h"
#include "Tt.h"
#include "Global.h"
#include "App.h"

int CQDlgTextureLoadOptions::scaleTo=256;
bool CQDlgTextureLoadOptions::scale=true;

CQDlgTextureLoadOptions::CQDlgTextureLoadOptions(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
	ui(new Ui::CQDlgTextureLoadOptions)
{
    ui->setupUi(this);
}

CQDlgTextureLoadOptions::~CQDlgTextureLoadOptions()
{
    delete ui;
}

void CQDlgTextureLoadOptions::cancelEvent()
{
//	defaultModalDialogEndRoutine(false);
}

void CQDlgTextureLoadOptions::okEvent()
{
//	defaultModalDialogEndRoutine(true);
}

void CQDlgTextureLoadOptions::refresh()
{
	ui->qqScale->setChecked(scale);
	ui->qq128->setChecked(scaleTo==128);
	ui->qq256->setChecked(scaleTo==256);
	ui->qq512->setChecked(scaleTo==512);
	ui->qq1024->setChecked(scaleTo==1024);
	ui->qq2048->setChecked(scaleTo==2048);
}

void CQDlgTextureLoadOptions::on_qqOk_accepted()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgTextureLoadOptions::on_qqScale_clicked()
{
	scale=!scale;
	refresh();
}

void CQDlgTextureLoadOptions::on_qq256_clicked()
{
	scale=true;
	scaleTo=256;
	refresh();
}

void CQDlgTextureLoadOptions::on_qq512_clicked()
{
	scale=true;
	scaleTo=512;
	refresh();
}

void CQDlgTextureLoadOptions::on_qq1024_clicked()
{
	scale=true;
	scaleTo=1024;
	refresh();
}

void CQDlgTextureLoadOptions::on_qq2048_clicked()
{
	scale=true;
	scaleTo=2048;
	refresh();
}

void CQDlgTextureLoadOptions::on_qq128_clicked()
{
	scale=true;
	scaleTo=128;
	refresh();
}
