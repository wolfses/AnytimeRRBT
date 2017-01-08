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
#include "qdlgcollections.h"
#include "ui_qdlgcollections.h"
#include "App.h"
#include "Tt.h"
#include <QShortcut>
#include "v_repStrings.h"

CQDlgCollections::CQDlgCollections(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgCollections)
{
    ui->setupUi(this);

	QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	connect(shortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	QShortcut* shortcut2 = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
	connect(shortcut2,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	initializationEvent();
}

CQDlgCollections::~CQDlgCollections()
{
    delete ui;
}

void CQDlgCollections::initializationEvent()
{
	CDlgEx::initializationEvent();
	baseInclusive=true;
	tipInclusive=true;
	operationType=2;
}

void CQDlgCollections::refresh()
{
	int group=getSelectedGroupID();
	refreshGroupList();
	selectGroup(group);
	refreshSubGroupList();
	refreshButtons();
}

void CQDlgCollections::refreshButtons()
{
	int selGrp=getSelectedGroupID();
	bool noEditMode=(App::ct->objCont->getEditModeType()==NO_EDIT_MODE);

	ui->qqAllObjects->setEnabled(getAllowedOpType(0)==0);
	ui->qqLooseObjects->setEnabled(getAllowedOpType(1)==1);
	ui->qqTrees->setEnabled(getAllowedOpType(2)==2);
	ui->qqChains->setEnabled(getAllowedOpType(3)==3);
	int opType=getAllowedOpType(operationType);
	ui->qqAllObjects->setChecked(opType==0);
	ui->qqLooseObjects->setChecked(opType==1);
	ui->qqTrees->setChecked(opType==2);
	ui->qqChains->setChecked(opType==3);
	ui->qqBaseIncluded->setEnabled(opType==2);
	ui->qqBaseIncluded->setChecked(baseInclusive);
	ui->qqTipIncluded->setEnabled(opType==3);
	ui->qqTipIncluded->setChecked(tipInclusive);

	ui->qqAdd->setEnabled(opType!=-1);
	ui->qqSubtract->setEnabled(opType>0);

	ui->qqVisualizeCollection->setEnabled((selGrp!=-1)&&noEditMode);
}

int CQDlgCollections::getAllowedOpType(int desiredOp)
{
	int selGrp=getSelectedGroupID();
	bool noEditMode=(App::ct->objCont->getEditModeType()==NO_EDIT_MODE);
	if ((selGrp==-1)||(!noEditMode))
		return(-1);
	int selSize=App::ct->objCont->getSelSize();
	bool grSizeZero=true;
	CRegGroup* it=App::ct->collections->getGroup(selGrp);
	if (it!=NULL)
		grSizeZero=(it->groupObjects.size()==0);
	int opType=desiredOp;
	int impossibleTypes=0;

	if (!grSizeZero)
		impossibleTypes|=1;
	if (selSize==0)
		impossibleTypes|=2;
	if (selSize!=1)
	{
		impossibleTypes|=4;
		impossibleTypes|=8;
	}


	if ((opType==0)&&(impossibleTypes&1))
		opType=1;
	if ((opType==1)&&(impossibleTypes&2))
	{
		if (impossibleTypes&1)
			opType=2;
		else
			opType=0;
	}
	if ( ((opType==2)&&(impossibleTypes&4))||((opType==3)&&(impossibleTypes&8)) )
	{
		if (impossibleTypes&1)
		{
			if (impossibleTypes&2)
				return(-1);
			else
				opType=1;
		}
		else
			opType=0;
	}
	return(opType);
}

void CQDlgCollections::refreshSubGroupList()
{
	CRegGroup* it=App::ct->collections->getGroup(getSelectedGroupID());
	ui->qqElementList->clear();
	ui->qqOverride->setEnabled(it!=NULL);
	ui->qqElementList->setEnabled(it!=NULL);
	if (it!=NULL)
	{
		for (int i=0;i<int(it->subGroupList.size());i++)
		{
			CRegGroupEl* it2=it->subGroupList[i];
			if (it2!=NULL)
			{
				std::string signChar="+";
				if (!it2->isAdditive())
					signChar="-";
				std::string objName=" [";
				C3DObject* theObj=App::ct->objCont->getObject(it2->getMainObject());
				if (theObj!=NULL)
				{
					objName=objName.append(theObj->getName());
					objName=objName.append("]");
				}
				else
					objName=" [ - ]";
				std::string tmp;
				if (it2->getGroupType()==GROUP_LOOSE)
					tmp=signChar.append(IDS_LOOSE_OBJECT);
				if (it2->getGroupType()==GROUP_FROM_BASE_INCLUDED)
					tmp=signChar.append(IDS_FROM_OBJECT__INCL___UP);
				if (it2->getGroupType()==GROUP_FROM_BASE_EXCLUDED)
					tmp=signChar.append(IDS_FROM_OBJECT__EXCL___UP);
				if (it2->getGroupType()==GROUP_FROM_TIP_INCLUDED)
					tmp=signChar.append(IDS_FROM_OBJECT__INCL___DOWN);
				if (it2->getGroupType()==GROUP_FROM_TIP_EXCLUDED)
					tmp=signChar.append(IDS_FROM_OBJECT__EXCL___DOWN);
				if (it2->getGroupType()==GROUP_EVERYTHING)
					tmp=signChar.append(IDS_ALL_OBJECTS);
				tmp=tmp.append(objName);
				int id=it2->getSubGroupID();
				QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
				itm->setData(Qt::UserRole,QVariant(id));
				ui->qqElementList->addItem(itm);
			}
		}
		ui->qqOverride->setChecked(it->getOverridesObjectMainProperties());
	}
	else
		ui->qqOverride->setChecked(false);
}

int CQDlgCollections::getSelectedGroupID()
{
	QList<QListWidgetItem*> sel=ui->qqCollectionList->selectedItems();
	if (sel.size()>0)
		return(sel.at(0)->data(Qt::UserRole).toInt());
	return(-1);
}

void CQDlgCollections::selectGroup(int groupID)
{
	for (int i=0;i<ui->qqCollectionList->count();i++)
	{
		QListWidgetItem* it=ui->qqCollectionList->item(i);
		if (it!=NULL)
		{
			if (it->data(Qt::UserRole).toInt()==groupID)
			{
				it->setSelected(true);
				break;
			}
		}
	}
}

void CQDlgCollections::refreshGroupList()
{ // It is not good to clear all, then add everything again, because the selection state gets lost
			//	// 1. Remove all items that are not valid anymore, and update the existing ones (REMOVED)
	ui->qqCollectionList->clear();
	for (int i=0;i<int(App::ct->collections->allGroups.size());i++)
	{
		CRegGroup* it=App::ct->collections->allGroups[i];
		std::string tmp=it->getGroupName();
		int id=it->getGroupID();
		QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
		itm->setData(Qt::UserRole,QVariant(id));
		itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
		ui->qqCollectionList->addItem(itm);
	}
}

void CQDlgCollections::on_qqNewCollection_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CRegGroup* newGroup=new CRegGroup(IDSOGL_COLLECTION);
		App::ct->collections->addGroup(newGroup,false);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refreshGroupList();
		selectGroup(newGroup->getGroupID());
		refreshSubGroupList();
		refreshButtons();
	}
}

void CQDlgCollections::on_qqOverride_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		int grpID=getSelectedGroupID();
		if (grpID!=-1)
		{
			CRegGroup* theGroup=App::ct->collections->getGroup(grpID);
			if (theGroup!=NULL)
			{
				theGroup->setOverridesObjectMainProperties(!theGroup->getOverridesObjectMainProperties());
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			}
		}
	}
}

void CQDlgCollections::on_qqCollectionList_itemSelectionChanged()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		refreshSubGroupList();
		refreshButtons();
	}
}

void CQDlgCollections::onDeletePressed()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (focusWidget()==ui->qqCollectionList)
		{
			int grpID=getSelectedGroupID();
			if (grpID!=-1)
			{
				App::ct->collections->removeGroup(grpID);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				refreshGroupList();
				refreshSubGroupList();
				refreshButtons();
			}
			return;
		}
		if (focusWidget()==ui->qqElementList)
		{
			int grpID=getSelectedGroupID();
			if (grpID!=-1)
			{
				CRegGroup* theGroup=App::ct->collections->getGroup(grpID);
				if (theGroup!=NULL)
				{
					QList<QListWidgetItem*> sel=ui->qqElementList->selectedItems();
					for (int i=0;i<sel.size();i++)
					{
						QListWidgetItem* it=sel.at(i);
						theGroup->removeSubGroup(it->data(Qt::UserRole).toInt());
					}
					refreshSubGroupList();
					if (theGroup->subGroupList.size()==0)
					{ // The group is empty and we have to remove it
						App::ct->collections->removeGroup(theGroup->getGroupID());
						refreshGroupList();
						refreshSubGroupList();
						refreshButtons();
					}
					App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				}
			}
			return;
		}
	}
}


void CQDlgCollections::on_qqVisualizeCollection_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		int grpID=getSelectedGroupID();
		CRegGroup* coll=App::ct->collections->getGroup(grpID);
		if (coll->groupObjects.size()!=0)
		{
			App::ct->objCont->deselectObjects();
			for (int i=0;i<int(coll->groupObjects.size());i++)
				App::ct->objCont->addObjectToSelection(coll->groupObjects[i]);
		}
	}
}

void CQDlgCollections::on_qqCollectionList_itemChanged(QListWidgetItem *item)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (item!=NULL)
		{
			std::string newName(item->text().toStdString());
			CRegGroup* it=App::ct->collections->getGroup(item->data(Qt::UserRole).toInt());
			if ( (it!=NULL)&&(newName!="") )
			{
				if (it->getGroupName()!=newName)
				{
					tt::removeIllegalCharacters(newName,true);
					if (App::ct->collections->getGroup(newName)==NULL)
					{
						it->setGroupName(newName);
						App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
					}
				}
			}
			refresh();
		}
	}
}

void CQDlgCollections::on_qqAllObjects_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		operationType=0;
		refresh();
	}
}

void CQDlgCollections::on_qqLooseObjects_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		operationType=1;
		refresh();
	}
}

void CQDlgCollections::on_qqTrees_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		operationType=2;
		refresh();
	}
}

void CQDlgCollections::on_qqBaseIncluded_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		baseInclusive=!baseInclusive;
		refresh();
	}
}

void CQDlgCollections::on_qqChains_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		operationType=3;
		refresh();
	}
}

void CQDlgCollections::on_qqTipIncluded_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		tipInclusive=!tipInclusive;
		refresh();
	}
}

void CQDlgCollections::on_qqAdd_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		int opType=getAllowedOpType(operationType);
		if (opType!=-1)
			doTheOperation(opType,true);
	}
}

void CQDlgCollections::on_qqSubtract_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		int opType=getAllowedOpType(operationType);
		if (opType>0)
			doTheOperation(opType,false);
	}
}

void CQDlgCollections::doTheOperation(int opType,bool additive)
{
	if (getSelectedGroupID()!=-1)
	{ // Selection  not empty
		int currentlySelGroup=getSelectedGroupID();
		if (currentlySelGroup!=-1)
		{ // Only one item is selected
			CRegGroup* it=App::ct->collections->getGroup(currentlySelGroup);
			if (it!=NULL)
			{ // Just in case
				if (opType==0)
				{
					if (it->groupObjects.size()==0)
					{ // "Everything"-tag can only be added to an empty collection (first position)
						CRegGroupEl* grEl=new CRegGroupEl(-1,GROUP_EVERYTHING,true);
						if (!it->addSubGroup(grEl))
							delete grEl; // Element already present!
						else
							App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
					}
				}
				if (opType==1)
				{
					for (int i=0;i<App::ct->objCont->getSelSize();i++)
					{
						CRegGroupEl* grEl=new CRegGroupEl(App::ct->objCont->getSelID(i),GROUP_LOOSE,additive);
						if (!it->addSubGroup(grEl))
							delete grEl; // Element already present!
						else
							App::ct->undoBufferContainer->announceChangeStart(); // **************** UNDO THINGY ****************
					}
				}
				if (opType==2)
				{
					C3DObject* lastSel=App::ct->objCont->getLastSelection();
					if (lastSel==NULL)
						return;
					int grpType=GROUP_FROM_BASE_INCLUDED;
					if (!baseInclusive)
						grpType=GROUP_FROM_BASE_EXCLUDED;
					CRegGroupEl* grEl=new CRegGroupEl(lastSel->getID(),grpType,additive);
					if (!it->addSubGroup(grEl))
						delete grEl; // Element already present!
					else
						App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				}
				if (opType==3)
				{
					C3DObject* lastSel=App::ct->objCont->getLastSelection();
					if (lastSel==NULL)
						return;
					int grpType=GROUP_FROM_TIP_INCLUDED;
					if (!tipInclusive)
						grpType=GROUP_FROM_TIP_EXCLUDED;
					CRegGroupEl* grEl=new CRegGroupEl(lastSel->getID(),grpType,additive);
					if (!it->addSubGroup(grEl))
						delete grEl; // Element already present!
					else
						App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				}
			}
			refreshSubGroupList();
			refreshButtons();
		}
	}

}

