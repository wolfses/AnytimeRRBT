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
#include "qdlgtextureselection.h"
#include "ui_qdlgtextureselection.h"
#include "App.h"
#include "Tt.h"
#include "v_repStrings.h"
#include <boost/lexical_cast.hpp>

CQDlgTextureSelection::CQDlgTextureSelection(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgTextureSelection)
{
	ui->setupUi(this);
	selectedTextureObject=-1;
	refresh();
}

CQDlgTextureSelection::~CQDlgTextureSelection()
{
    delete ui;
}

void CQDlgTextureSelection::cancelEvent()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgTextureSelection::okEvent()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgTextureSelection::refresh()
{
	ui->qqTextureList->clear();
	int itemCount=0;
	while (App::ct->textureCont->getObjectAtIndex(itemCount)!=NULL)
	{
		CTextureObject* it=App::ct->textureCont->getObjectAtIndex(itemCount);
		std::string txt(it->getObjectName());
		int sx,sy;
		it->getTextureSize(sx,sy);
		txt+=" [";
		txt+=boost::lexical_cast<std::string>(sx)+"x"+boost::lexical_cast<std::string>(sy)+"] ";
		txt+=tt::decorateString(" (",strTranslate(IDSN_STATIC_TEXTURE),")");
		QListWidgetItem* itm=new QListWidgetItem(txt.c_str());
		itm->setData(Qt::UserRole,QVariant(it->getObjectID()));
		itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
		ui->qqTextureList->addItem(itm);
		itemCount++;
	}
	for (int i=0;i<int(App::ct->objCont->visionSensorList.size());i++)
	{
		CVisionSensor* rs=App::ct->objCont->getVisionSensor(App::ct->objCont->visionSensorList[i]);
		std::string txt(rs->getName());
		int s[2];
		rs->getRealResolution(s);
		txt+=" [";
		txt+=boost::lexical_cast<std::string>(s[0])+"x"+boost::lexical_cast<std::string>(s[1])+"] ";
		txt+=tt::decorateString(" (",strTranslate(IDSN_DYNAMIC_TEXTURE),")");
		QListWidgetItem* itm=new QListWidgetItem(txt.c_str());
		itm->setData(Qt::UserRole,QVariant(rs->getID()));
		itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
		ui->qqTextureList->addItem(itm);
		itemCount++;
	}
}

void CQDlgTextureSelection::on_qqOkCancel_accepted()
{
	QList<QListWidgetItem*> sel=ui->qqTextureList->selectedItems();
	if (sel.size()>0)
		selectedTextureObject=sel.at(0)->data(Qt::UserRole).toInt();
	else
		selectedTextureObject=-1;
	defaultModalDialogEndRoutine(true);
}

void CQDlgTextureSelection::on_qqOkCancel_rejected()
{
	selectedTextureObject=-1;
	defaultModalDialogEndRoutine(false);
}
