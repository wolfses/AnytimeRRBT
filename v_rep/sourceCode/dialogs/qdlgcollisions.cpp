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
#include "qdlgcollisions.h"
#include "ui_qdlgcollisions.h"
#include "Tt.h"
#include <QShortcut>
#include "qdlgentityvsentityselection.h"
#include "editboxdelegate.h"
#include "qdlgmaterial.h"
#include "App.h"

CQDlgCollisions::CQDlgCollisions(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgCollisions)
{
    ui->setupUi(this);
	inSelectionRoutine=false;
	QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	connect(shortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	QShortcut* shortcut2 = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
	connect(shortcut2,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	CEditBoxDelegate* delegate=new CEditBoxDelegate();
	ui->qqCollisionList->setItemDelegate(delegate);
}

CQDlgCollisions::~CQDlgCollisions()
{
    delete ui;
}

void CQDlgCollisions::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(CALCULATION_DLG);
}

void CQDlgCollisions::refresh()
{
	ui->qqEnableAll->setChecked(App::ct->mainSettings->collisionDetectionEnabled);

	if (!inSelectionRoutine)
	{
		int selectedObjectID=getSelectedObjectID();
		updateObjectsInList();
		selectObjectInList(selectedObjectID);
	}

	CRegColl* it=App::ct->collisions->getObject(getSelectedObjectID());
	ui->qqComputeContour->setEnabled(it!=NULL);

	ui->qqColliderColorChanges->setEnabled(it!=NULL);
	ui->qqCollideeColorChanges->setEnabled(it!=NULL);
	ui->qqExplicitHandling->setEnabled(it!=NULL);
	ui->qqContourWidth->setEnabled((it!=NULL)&&it->getExhaustiveDetection());
	ui->qqAdjustContourColor->setEnabled((it!=NULL)&&it->getExhaustiveDetection());
	if (it!=NULL)
	{
		ui->qqComputeContour->setChecked(it->getExhaustiveDetection());
		ui->qqColliderColorChanges->setChecked(it->getColliderChangesColor());
		ui->qqCollideeColorChanges->setChecked(it->getCollideeChangesColor());
		ui->qqExplicitHandling->setChecked(it->getExplicitHandling());
		ui->qqContourWidth->setText(tt::FNb(it->getContourWidth()).c_str());
	}
	else
	{
		ui->qqComputeContour->setChecked(false);
		ui->qqColliderColorChanges->setChecked(false);
		ui->qqCollideeColorChanges->setChecked(false);
		ui->qqExplicitHandling->setChecked(false);
		ui->qqContourWidth->setText("");
	}
}

void CQDlgCollisions::updateObjectsInList()
{
	ui->qqCollisionList->clear();
	for (int i=0;i<int(App::ct->collisions->collisionObjects.size());i++)
	{
		CRegColl* it=App::ct->collisions->collisionObjects[i];
		std::string tmp=it->getObjectPartnersName();
		int id=it->getObjectID();
		QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
		itm->setData(Qt::UserRole,QVariant(id));
		itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
		ui->qqCollisionList->addItem(itm);
	}
}

void CQDlgCollisions::on_qqAddNewObject_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CQDlgEntityVsEntitySelection theDialog(this);
		theDialog.initialize(0);
		if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
		{
			int newObjID=App::ct->collisions->addNewObject(theDialog.entity1,theDialog.entity2,"");
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			updateObjectsInList();
			selectObjectInList(newObjID);
		}
		refresh();
	}
}

int CQDlgCollisions::getSelectedObjectID()
{
	QList<QListWidgetItem*> sel=ui->qqCollisionList->selectedItems();
	if (sel.size()>0)
		return(sel.at(0)->data(Qt::UserRole).toInt());
	return(-1);
}

void CQDlgCollisions::selectObjectInList(int objectID)
{
	for (int i=0;i<ui->qqCollisionList->count();i++)
	{
		QListWidgetItem* it=ui->qqCollisionList->item(i);
		if (it!=NULL)
		{
			if (it->data(Qt::UserRole).toInt()==objectID)
			{
				it->setSelected(true);
				break;
			}
		}
	}
}


void CQDlgCollisions::on_qqCollisionList_itemSelectionChanged()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		int objID=getSelectedObjectID();
		CRegColl* coll=App::ct->collisions->getObject(objID);
		if (coll!=NULL)
			((CEditBoxDelegate*)ui->qqCollisionList->itemDelegate())->initialText=coll->getObjectName();
		else
			((CEditBoxDelegate*)ui->qqCollisionList->itemDelegate())->initialText="";
		inSelectionRoutine=true;
		refresh();
		inSelectionRoutine=false;
	}
}

void CQDlgCollisions::on_qqCollisionList_itemChanged(QListWidgetItem *item)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (item!=NULL)
		{
			std::string newName(item->text().toStdString());
			CRegColl* it=App::ct->collisions->getObject(item->data(Qt::UserRole).toInt());
			if ( (it!=NULL)&&(newName!="") )
			{
				if (it->getObjectName()!=newName)
				{
					tt::removeIllegalCharacters(newName,true);
					if (App::ct->collisions->getObject(newName)==NULL)
					{
						it->setObjectName(newName);
						App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
					}
				}
			}
			refresh();
		}
	}
}

void CQDlgCollisions::onDeletePressed()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (focusWidget()==ui->qqCollisionList)
		{
			int objID=getSelectedObjectID();
			if (objID!=-1)
			{
				App::ct->collisions->removeObject(objID);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				refresh();
			}
			return;
		}
	}
}

void CQDlgCollisions::on_qqEnableAll_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->mainSettings->collisionDetectionEnabled=!App::ct->mainSettings->collisionDetectionEnabled;
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCollisions::on_qqCollisionColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		int identification[4]={App::ct->mainSettings->getLifeID(),-1,-1,-1};
		CQDlgMaterial::display(identification,&App::ct->mainSettings->collisionColor,App::mainWindow,true,true,true,true,true,true,true,false);
	}
}

void CQDlgCollisions::on_qqExplicitHandling_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CRegColl* it=App::ct->collisions->getObject(getSelectedObjectID());
		if (it!=NULL)
		{
			it->setExplicitHandling(!it->getExplicitHandling());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgCollisions::on_qqColliderColorChanges_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CRegColl* it=App::ct->collisions->getObject(getSelectedObjectID());
		if (it!=NULL)
		{
			it->setColliderChangesColor(!it->getColliderChangesColor());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgCollisions::on_qqComputeContour_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CRegColl* it=App::ct->collisions->getObject(getSelectedObjectID());
		if (it!=NULL)
		{
			it->setExhaustiveDetection(!it->getExhaustiveDetection());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgCollisions::on_qqCollideeColorChanges_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CRegColl* it=App::ct->collisions->getObject(getSelectedObjectID());
		if (it!=NULL)
		{
			it->setCollideeChangesColor(!it->getCollideeChangesColor());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgCollisions::on_qqAdjustContourColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CRegColl* it=App::ct->collisions->getObject(getSelectedObjectID());
		if (it!=NULL)
		{
			int identification[4]={it->getLifeID(),-1,-1,-1};
			CQDlgMaterial::display(identification,&it->contourColor,App::mainWindow,true,true,true,true,true,true,true,false);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
	}
}

void CQDlgCollisions::on_qqContourWidth_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CRegColl* it=App::ct->collisions->getObject(getSelectedObjectID());
		if (it!=NULL)
		{
			bool ok;
			int newVal=ui->qqContourWidth->text().toInt(&ok);
			if (ok)
			{
				it->setContourWidth(newVal);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			}
		}
		refresh();
	}
}
