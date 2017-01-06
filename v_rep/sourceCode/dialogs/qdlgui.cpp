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
#include "qdlgui.h"
#include "ui_qdlgui.h"
#include "App.h"
#include "Tt.h"
#include "GV.h"
#include "IloIlo.h"
#include "editboxdelegate.h"
#include "qdlgtextures.h"
#include "qdlgnewui.h"
#include "qdlguirolledup.h"
#include "v_repStrings.h"
#include <boost/lexical_cast.hpp>


CQDlgUi::CQDlgUi(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgUi)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
	inListSelectionRoutine=false;
	noListSelectionAllowed=false;
	delKeyShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	connect(delKeyShortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	backspaceKeyShortcut = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
	connect(backspaceKeyShortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	CEditBoxDelegate* delegate=new CEditBoxDelegate();
	ui->qqList->setItemDelegate(delegate);
}

CQDlgUi::~CQDlgUi()
{
    delete ui;
}

void CQDlgUi::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	// We close it by ending the button edit mode:
	App::ct->objCont->endEditMode(true);
}

void CQDlgUi::refresh()
{
	inMainRefreshRoutine=true;
	int uiID=App::ct->buttonBlockContainer->getBlockInEdition();
	CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(uiID);

	if (!inListSelectionRoutine)
	{
		updateObjectsInList();
		selectObjectInList(uiID);
	}

	ui->qqCellCountH->setEnabled(it!=NULL);
	ui->qqCellCountV->setEnabled(it!=NULL);
	ui->qqCellSizeH->setEnabled(it!=NULL);
	ui->qqCellSizeV->setEnabled(it!=NULL);
	ui->qqUiPosH->setEnabled(it!=NULL);
	ui->qqUiPosV->setEnabled(it!=NULL);
	ui->qqUiBorderL->setEnabled(it!=NULL);
	ui->qqUiBorderT->setEnabled(it!=NULL);
	ui->qqVisible->setEnabled(it!=NULL);
	ui->qqMoveable->setEnabled(it!=NULL);
	ui->qqVisibleDuringSimulation->setEnabled(it!=NULL);
	ui->qqVisibleWhenObjectSelected->setEnabled(it!=NULL);
	ui->qqFixedWidthFont->setEnabled(it!=NULL);
	ui->qqSelectObject->setEnabled(it!=NULL);
	ui->qqPageCombo->setEnabled(it!=NULL);
	ui->qqAssociatedObject->setEnabled(it!=NULL);
	ui->qqSetTexture->setEnabled(it!=NULL);
	ui->qqAdjustRolledUpSizes->setEnabled(it!=NULL);

	ui->qqUiBorderL->setChecked((it!=NULL)&&(it->getAttributes()&sim_ui_property_relativetoleftborder));
	ui->qqUiBorderT->setChecked((it!=NULL)&&(it->getAttributes()&sim_ui_property_relativetotopborder));
	ui->qqVisible->setChecked((it!=NULL)&&(it->getAttributes()&sim_ui_property_visible));
	ui->qqMoveable->setChecked((it!=NULL)&&(it->getAttributes()&sim_ui_property_moveable));
	ui->qqVisibleDuringSimulation->setChecked((it!=NULL)&&(it->getAttributes()&sim_ui_property_visibleduringsimulationonly));
	ui->qqVisibleWhenObjectSelected->setChecked((it!=NULL)&&(it->getAttributes()&sim_ui_property_visiblewhenobjectselected));
	ui->qqFixedWidthFont->setChecked((it!=NULL)&&(it->getAttributes()&sim_ui_property_fixedwidthfont));
	ui->qqSelectObject->setChecked((it!=NULL)&&(it->getAttributes()&sim_ui_property_selectassociatedobject));

	ui->qqPageCombo->clear();
	ui->qqAssociatedObject->clear();

	if (it!=NULL)
	{
		VPoint blockSize;
		it->getBlockSize(blockSize);
		ui->qqCellCountH->setText(tt::FNb(0,blockSize.x,false).c_str());
		ui->qqCellCountV->setText(tt::FNb(0,blockSize.y,false).c_str());
		VPoint buttonSize;
		it->getButtonSize(buttonSize);
		ui->qqCellSizeH->setText(tt::FNb(0,buttonSize.x,false).c_str());
		ui->qqCellSizeV->setText(tt::FNb(0,buttonSize.y,false).c_str());
		VPoint blockPos;
		it->getBlockPositionRelative(blockPos);
		ui->qqUiPosH->setText(tt::FNb(0,blockPos.x,false).c_str());
		ui->qqUiPosV->setText(tt::FNb(0,blockPos.y,false).c_str());

		ui->qqPageCombo->addItem(strTranslate(IDS_ALL_PAGES),QVariant(-1));
		for (int i=0;i<8;i++)
		{
			std::string tmp(std::string(strTranslate(IDSN_PAGE_X))+" "+boost::lexical_cast<std::string>(i+1));
			ui->qqPageCombo->addItem(tmp.c_str(),QVariant(i));
		}
		for (int i=0;i<ui->qqPageCombo->count();i++)
		{
			if (ui->qqPageCombo->itemData(i).toInt()==it->getViewToAppearOn())
			{
				ui->qqPageCombo->setCurrentIndex(i);
				break;
			}
		}

		std::vector<std::string> names;
		std::vector<int> ids;
		ui->qqAssociatedObject->addItem(strTranslate(IDSN_NONE),QVariant(-1));
		for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
		{
			C3DObject* it2=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
			names.push_back(it2->getName());
			ids.push_back(it2->getID());
		}
		tt::orderStrings(names,ids);
		for (int i=0;i<int(names.size());i++)
			ui->qqAssociatedObject->addItem(names[i].c_str(),QVariant(ids[i]));
		for (int i=0;i<ui->qqAssociatedObject->count();i++)
		{
			if (ui->qqAssociatedObject->itemData(i).toInt()==it->getObjectIDAttachedTo())
			{
				ui->qqAssociatedObject->setCurrentIndex(i);
				break;
			}
		}
	}
	else
	{
		ui->qqCellCountH->setText("");
		ui->qqCellCountV->setText("");
		ui->qqCellSizeH->setText("");
		ui->qqCellSizeV->setText("");
		ui->qqUiPosH->setText("");
		ui->qqUiPosV->setText("");
	}


	inMainRefreshRoutine=false;
}

void CQDlgUi::updateObjectsInList()
{
	noListSelectionAllowed=true;
	ui->qqList->clear();

	for (int i=0;i<int(App::ct->buttonBlockContainer->allBlocks.size());i++)
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->allBlocks[i];
		if ( (itBlock->getAttributes()&sim_ui_property_systemblock)==0 )
		{
			std::string txt=itBlock->getBlockName();
			int objID=itBlock->getBlockID();
			QListWidgetItem* itm=new QListWidgetItem(txt.c_str());
			itm->setData(Qt::UserRole,QVariant(objID));
			itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
			ui->qqList->addItem(itm);
		}
	}
	noListSelectionAllowed=false;
}

void CQDlgUi::selectObjectInList(int objectID)
{
	noListSelectionAllowed=true;
	for (int i=0;i<ui->qqList->count();i++)
	{
		QListWidgetItem* it=ui->qqList->item(i);
		if (it!=NULL)
		{
			if (it->data(Qt::UserRole).toInt()==objectID)
			{
				it->setSelected(true);
				break;
			}
		}
	}
	noListSelectionAllowed=false;
}


void CQDlgUi::onDeletePressed()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (focusWidget()==ui->qqList)
		{
			int objID=App::ct->buttonBlockContainer->getBlockInEdition();
			if (objID!=-1)
			{
				App::ct->buttonBlockContainer->removeBlockFromID(objID);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				refresh();
			}
			return;
		}
	}
}

void CQDlgUi::on_qqAddNew_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		App::ct->buttonBlockContainer->deselectButtons();
		CQDlgNewUi theDialog(this);
		theDialog.refresh();
		if (theDialog.makeDialogModal()==VDIALOG_MODAL_RETURN_CANCEL)
			return;
		int s[2]={theDialog.xSize,theDialog.ySize};
		int cs[2]={14,14};
		int hs[20];
		int h=simCreateUI_internal("UI",theDialog.menuBarAttributes,s,cs,hs);
		App::ct->buttonBlockContainer->setBlockInEdition(h);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUi::on_qqList_itemSelectionChanged()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!noListSelectionAllowed)
		{
			QList<QListWidgetItem*> sel=ui->qqList->selectedItems();
			int objID=-1;
			if (sel.size()>0)
				objID=sel.at(0)->data(Qt::UserRole).toInt();
			CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(objID);
			if (it!=NULL)
				((CEditBoxDelegate*)ui->qqList->itemDelegate())->initialText=it->getBlockName();
			else
				((CEditBoxDelegate*)ui->qqList->itemDelegate())->initialText="";
			App::ct->buttonBlockContainer->setBlockInEdition(objID);
			App::ct->objCont->setFullDialogRefreshFlag();
		}
	}
}

void CQDlgUi::on_qqList_itemChanged(QListWidgetItem *item)
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (item!=NULL)
		{
			std::string newName(item->text().toStdString());
			CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(item->data(Qt::UserRole).toInt());
			if ( (it!=NULL)&&(newName!="") )
			{
				if (it->getBlockName()!=newName)
				{
					tt::removeIllegalCharacters(newName,true);
					if (App::ct->buttonBlockContainer->getBlockWithName(newName)==NULL)
					{
						it->setBlockName(newName);
						App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
					}
				}
			}
			App::ct->objCont->setFullDialogRefreshFlag();
		}
	}
}

void CQDlgUi::on_qqCellCountH_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		bool ok;
		int newVal=ui->qqCellCountH->text().toInt(&ok);
		if (ok)
		{
			VPoint size;
			itBlock->getBlockSize(size);
			size.x=newVal;
			itBlock->setBlockSize(size);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgUi::on_qqCellCountV_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		bool ok;
		int newVal=ui->qqCellCountV->text().toInt(&ok);
		if (ok)
		{
			VPoint size;
			itBlock->getBlockSize(size);
			size.y=newVal;
			itBlock->setBlockSize(size);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgUi::on_qqCellSizeH_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		bool ok;
		int newVal=ui->qqCellSizeH->text().toInt(&ok);
		if (ok)
		{
			VPoint size;
			itBlock->getButtonSize(size);
			size.x=newVal;
			itBlock->setButtonSize(size);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgUi::on_qqCellSizeV_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		bool ok;
		int newVal=ui->qqCellSizeV->text().toInt(&ok);
		if (ok)
		{
			VPoint size;
			itBlock->getButtonSize(size);
			size.y=newVal;
			itBlock->setButtonSize(size);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgUi::on_qqUiPosH_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		bool ok;
		int newVal=ui->qqUiPosH->text().toInt(&ok);
		if (ok)
		{
			tt::limitValue(-10000,10000,newVal);
			VPoint blockPos;
			itBlock->getDesiredBlockPosition(blockPos);
			itBlock->setDesiredBlockPosition(newVal,blockPos.y);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgUi::on_qqUiPosV_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		bool ok;
		int newVal=ui->qqUiPosV->text().toInt(&ok);
		if (ok)
		{
			tt::limitValue(-10000,10000,newVal);
			VPoint blockPos;
			itBlock->getDesiredBlockPosition(blockPos);
			itBlock->setDesiredBlockPosition(blockPos.x,newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgUi::on_qqUiBorderL_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		itBlock->setAttributes(itBlock->getAttributes()^sim_ui_property_relativetoleftborder);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUi::on_qqUiBorderT_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		itBlock->setAttributes(itBlock->getAttributes()^sim_ui_property_relativetotopborder);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUi::on_qqVisible_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		itBlock->setAttributes(itBlock->getAttributes()^sim_ui_property_visible);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUi::on_qqMoveable_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		itBlock->setAttributes(itBlock->getAttributes()^sim_ui_property_moveable);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUi::on_qqVisibleDuringSimulation_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		itBlock->setAttributes(itBlock->getAttributes()^sim_ui_property_visibleduringsimulationonly);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUi::on_qqFixedWidthFont_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		itBlock->setAttributes(itBlock->getAttributes()^sim_ui_property_fixedwidthfont);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUi::on_qqVisibleWhenObjectSelected_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		itBlock->setAttributes(itBlock->getAttributes()^sim_ui_property_visiblewhenobjectselected);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUi::on_qqSelectObject_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		itBlock->setAttributes(itBlock->getAttributes()^sim_ui_property_selectassociatedobject);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUi::on_qqPageCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
		{
			CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
			if (itBlock==NULL)
				return;
			itBlock->setViewToAppearOn(ui->qqPageCombo->itemData(ui->qqPageCombo->currentIndex()).toInt());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgUi::on_qqAssociatedObject_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
		{
			CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
			if (itBlock==NULL)
				return;
			itBlock->setObjectIDAttachedTo(ui->qqAssociatedObject->itemData(ui->qqAssociatedObject->currentIndex()).toInt());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgUi::on_qqSetTexture_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		int identification[4]={itBlock->getLifeID(),1,itBlock->getBlockID(),-1};
		delKeyShortcut->setEnabled(false);
		backspaceKeyShortcut->setEnabled(false);
		CQDlgTextures::displayModal(identification,this);
		backspaceKeyShortcut->setEnabled(true);
		delKeyShortcut->setEnabled(true);
	}
}

void CQDlgUi::on_qqAdjustRolledUpSizes_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		CQDlgUiRolledUp theDialog(this);
		theDialog.itBlock=itBlock;
		theDialog.refresh();
		delKeyShortcut->setEnabled(false);
		backspaceKeyShortcut->setEnabled(false);
		theDialog.makeDialogModal();
		backspaceKeyShortcut->setEnabled(true);
		delKeyShortcut->setEnabled(true);
	}
}
