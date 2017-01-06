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
#include "qdlgassembly.h"
#include "ui_qdlgassembly.h"
#include "v_rep_internal.h"
#include "Tt.h"
#include "IloIlo.h"
#include "App.h"
#include "v_repStrings.h"

CQDlgAssembly::CQDlgAssembly(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
	ui(new Ui::CQDlgAssembly)
{
	obj=NULL;
    ui->setupUi(this);
}

CQDlgAssembly::~CQDlgAssembly()
{
    delete ui;
}

void CQDlgAssembly::cancelEvent()
{
//	defaultModalDialogEndRoutine(false);
}

void CQDlgAssembly::okEvent()
{
//	defaultModalDialogEndRoutine(true);
}

void CQDlgAssembly::refresh()
{
	ui->qqCanBeChild->setChecked(obj->getAssemblyRole(true));
	ui->qqCanBeParent->setChecked(obj->getAssemblyRole(false));
	ui->qqSetLocalMatrix->setEnabled(obj->getAssemblyRole(true));
	ui->qqChildMatchValue->setText(tt::FNb(0,obj->getAssemblyMatchValue(true),false).c_str());
	ui->qqParentMatchValue->setText(tt::FNb(0,obj->getAssemblyMatchValue(false),false).c_str());
}

void CQDlgAssembly::on_qqCanBeChild_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		obj->setAssemblyRole(true,!obj->getAssemblyRole(true));
		refresh();
	}
}

void CQDlgAssembly::on_qqChildMatchValue_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		int newVal=ui->qqChildMatchValue->text().toInt(&ok);
		if (ok)
		{
			newVal=tt::getLimitedInt(0,999999,newVal);
			obj->setAssemblyMatchValue(true,newVal);
		}
		refresh();
	}
}

void CQDlgAssembly::on_qqCanBeParent_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		obj->setAssemblyRole(false,!obj->getAssemblyRole(false));
		refresh();
	}
}

void CQDlgAssembly::on_qqParentMatchValue_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		int newVal=ui->qqParentMatchValue->text().toInt(&ok);
		if (ok)
		{
			newVal=tt::getLimitedInt(0,999999,newVal);
			obj->setAssemblyMatchValue(false,newVal);
		}
		refresh();
	}
}

void CQDlgAssembly::on_qqSetLocalMatrix_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		WORD res=App::uiThread->messageBox_question(App::mainWindow,strTranslate(IDSN_ASSEMBLING_METHOD),strTranslate(IDSN_ASSEMBLING_METHOD_QUESTION),VMessageBox::YES_NO);
		if (res==VMessageBox::REPLY_YES)
		{
			obj->setAssemblingLocalTransformation(obj->getLocalTransformationPart1());
		}
	}
}

void CQDlgAssembly::on_qqClose_clicked(QAbstractButton *button)
{
	defaultModalDialogEndRoutine(true);
}
