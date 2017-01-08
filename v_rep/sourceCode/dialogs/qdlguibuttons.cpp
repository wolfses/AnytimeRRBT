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
#include "qdlguibuttons.h"
#include "ui_qdlguibuttons.h"
#include "App.h"
#include "Tt.h"
#include "GV.h"
#include "IloIlo.h"
#include "qdlgtextures.h"
#include "qdlgcolor.h"
#include "v_repStrings.h"

CQDlgUiButtons::CQDlgUiButtons(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgUiButtons)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
}

CQDlgUiButtons::~CQDlgUiButtons()
{
    delete ui;
}

void CQDlgUiButtons::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	// We close it by ending the button edit mode:
	App::ct->objCont->endEditMode(true);
}

void CQDlgUiButtons::refresh()
{
	inMainRefreshRoutine=true;

	int uiID=App::ct->buttonBlockContainer->getBlockInEdition();
	CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(uiID);
	CSoftButton* itButton=NULL;
	if (it==NULL)
		return; // shouldn't happen
	int buttonSelectedNb=0;
	int emptyButtonSelectedNb=0;
	if (App::ct->buttonBlockContainer->selectedButtons.size()>0)
	{
		int pos=App::ct->buttonBlockContainer->selectedButtons[App::ct->buttonBlockContainer->selectedButtons.size()-1];
		VPoint size;
		it->getBlockSize(size);
		itButton=it->getButtonAtPos(pos%size.x,pos/size.x);
		for (int i=0;i<int(App::ct->buttonBlockContainer->selectedButtons.size());i++)
		{
			pos=App::ct->buttonBlockContainer->selectedButtons[i];
			CSoftButton* anyButton=it->getButtonAtPos(pos%size.x,pos/size.x);
			if (anyButton!=NULL)
				buttonSelectedNb++;
			else
				emptyButtonSelectedNb++;
		}
	}
	else
		return; // shouldn't happen

	ui->qqInsertButtons->setEnabled(emptyButtonSelectedNb!=0);
	ui->qqInsertMergedButton->setEnabled(emptyButtonSelectedNb>1);
	ui->qqRemoveButtons->setEnabled(buttonSelectedNb!=0);
	bool lastSelIsButton=false;
	bool lastSelStaysDown=false;
	int bt=0;
	if (itButton!=NULL)
	{
		lastSelIsButton=itButton->getButtonType()==sim_buttonproperty_button;
		bt=itButton->getButtonType();
	}
	if (lastSelIsButton)
		lastSelStaysDown=(itButton->getAttributes()&sim_buttonproperty_staydown)!=0;


	ui->qqButtonHandle->setEnabled(itButton!=NULL);

	ui->qqTypeCombo->setEnabled(itButton!=NULL);
	ui->qqEnabled->setEnabled((itButton!=NULL)&&(bt!=sim_buttonproperty_label));
	ui->qqStayDown->setEnabled((itButton!=NULL)&&lastSelIsButton);
	ui->qqRollUp->setEnabled((itButton!=NULL)&&lastSelIsButton&&lastSelStaysDown);
	ui->qqCenteredH->setEnabled((itButton!=NULL)&&(bt!=sim_buttonproperty_slider));
	ui->qqUpDownEvent->setEnabled((itButton!=NULL)&&lastSelIsButton&&(!lastSelStaysDown));
	ui->qqCloseAction->setEnabled((itButton!=NULL)&&lastSelIsButton);
	ui->qqCenteredV->setEnabled((itButton!=NULL)&&(itButton->getButtonType()!=sim_buttonproperty_slider));
	ui->qqBorderless->setEnabled(itButton!=NULL);
	ui->qqIgnoreMouse->setEnabled(itButton!=NULL);
	ui->qqApplyType->setEnabled((itButton!=NULL)&&(buttonSelectedNb>1));

	ui->qqLabelUp->setEnabled((itButton!=NULL)&&(bt!=sim_buttonproperty_slider));
	ui->qqLabelDown->setEnabled((itButton!=NULL)&&lastSelIsButton&&lastSelStaysDown);
	ui->qqApplyLabel->setEnabled((itButton!=NULL)&&(buttonSelectedNb>1));

	ui->qqColorUp->setEnabled(itButton!=NULL);
	ui->qqColorDown->setEnabled((itButton!=NULL)&&lastSelIsButton);
	ui->qqColorLabel->setEnabled((itButton!=NULL)&&(bt!=sim_buttonproperty_slider));
	ui->qqApplyColor->setEnabled((itButton!=NULL)&&(buttonSelectedNb>1));

	ui->qqTransparent->setEnabled(itButton!=NULL);
	ui->qqNoBackground->setEnabled(itButton!=NULL);
	ui->qqSetTexture->setEnabled((itButton!=NULL)&&(bt!=sim_buttonproperty_editbox));
	ui->qqApplyOtherProperties->setEnabled((itButton!=NULL)&&(buttonSelectedNb>1));


	ui->qqTypeCombo->clear();
	ui->qqEnabled->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_enabled)!=0));
	ui->qqStayDown->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_staydown)!=0));
	ui->qqRollUp->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_rollupaction)!=0));
	ui->qqCenteredH->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_horizontallycentered)!=0));
	ui->qqUpDownEvent->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_downupevent)!=0));
	ui->qqCloseAction->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_closeaction)!=0));
	ui->qqCenteredV->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_verticallycentered)!=0));
	ui->qqBorderless->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_borderless)!=0));
	ui->qqIgnoreMouse->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_ignoremouse)!=0));


	ui->qqTransparent->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_transparent)!=0));
	ui->qqNoBackground->setChecked((itButton!=NULL)&&((itButton->getAttributes()&sim_buttonproperty_nobackgroundcolor)!=0));

	if (itButton!=NULL)
	{
		ui->qqButtonHandle->setText(tt::FNb(0,itButton->buttonID,false).c_str());
		ui->qqLabelUp->setText(itButton->label.c_str());
		ui->qqLabelDown->setText(itButton->downLabel.c_str());

		ui->qqTypeCombo->addItem(strTranslate(IDSN_BUTTON),QVariant(sim_buttonproperty_button));
		ui->qqTypeCombo->addItem(strTranslate(IDSN_LABEL),QVariant(sim_buttonproperty_label));
		ui->qqTypeCombo->addItem(strTranslate(IDSN_EDIT_BOX),QVariant(sim_buttonproperty_editbox));
		ui->qqTypeCombo->addItem(strTranslate(IDSN_SLIDER),QVariant(sim_buttonproperty_slider));
		for (int i=0;i<ui->qqTypeCombo->count();i++)
		{
			if (ui->qqTypeCombo->itemData(i).toInt()==(bt&7))
			{
				ui->qqTypeCombo->setCurrentIndex(i);
				break;
			}
		}
	}
	else
	{
		ui->qqButtonHandle->setText("");
		ui->qqLabelUp->setText("");
		ui->qqLabelDown->setText("");
	}

	inMainRefreshRoutine=false;
}

CSoftButton* CQDlgUiButtons::getLastSelectedButton()
{
	CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
	if (itBlock==NULL)
		return(NULL);
	if (App::ct->buttonBlockContainer->selectedButtons.size()==0)
		return(NULL);
	int butt=App::ct->buttonBlockContainer->selectedButtons[App::ct->buttonBlockContainer->selectedButtons.size()-1];
	VPoint size;
	itBlock->getBlockSize(size);
	CSoftButton* itButton=itBlock->getButtonAtPos(butt%size.x,butt/size.x);
	if (itButton==NULL)
		return(NULL);
	return(itButton);
}

void CQDlgUiButtons::on_qqInsertButtons_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		if (App::ct->buttonBlockContainer->selectedButtons.size()<1)
			return;
		VPoint size;
		itBlock->getBlockSize(size);

		// We first check how many buttons we have to insert:
		int btnCnt=0;
		for (int i=0;i<int(App::ct->buttonBlockContainer->selectedButtons.size());i++)
		{
			int val=App::ct->buttonBlockContainer->selectedButtons[i];
			CSoftButton* itButton=itBlock->getButtonAtPos(val%size.x,val/size.x);
			if (itButton==NULL)
				btnCnt++;
		}
		// if we insert just one button, its ID will be the smallest available ID, otherwise
		// the buttons IDs will start from biggestButtonID+1:
		int freeID=itBlock->getFreeButtonHandle();
		if (btnCnt>1)
		{ // We find the biggest button handle and increment it by one:
			freeID=-1;
			for (int i=0;i<int(itBlock->allButtons.size());i++)
			{
				if (itBlock->allButtons[i]->buttonID>freeID)
					freeID=itBlock->allButtons[i]->buttonID;
			}
			freeID++;
		}
		for (int i=0;i<int(App::ct->buttonBlockContainer->selectedButtons.size());i++)
		{
			int val=App::ct->buttonBlockContainer->selectedButtons[i];
			CSoftButton* itButton=itBlock->getButtonAtPos(val%size.x,val/size.x);
			if (itButton==NULL)
			{
				CSoftButton* newButton=new CSoftButton("",val%size.x,val/size.x,1,1);
				newButton->buttonID=freeID;
				freeID++;
				if (!itBlock->insertButtonWithoutChecking(newButton))
					delete newButton;
				App::ct->undoBufferContainer->announceChangeStart(); // **************** UNDO THINGY ****************
			}
		}
		App::ct->undoBufferContainer->announceChangeEnd(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUiButtons::on_qqInsertMergedButton_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		if (App::ct->buttonBlockContainer->selectedButtons.size()<2)
			return;
		// We first determin the selection square:
		VPoint size;
		itBlock->getBlockSize(size);
		int smallestCo=600000000;
		int biggestCo=-1;
		for (int i=0;i<int(App::ct->buttonBlockContainer->selectedButtons.size());i++)
		{
			int butt=App::ct->buttonBlockContainer->selectedButtons[i];
			if (butt<smallestCo)
				smallestCo=butt;
			if (butt>biggestCo)
				biggestCo=butt;
		}
		VPoint smallest(smallestCo%size.x,smallestCo/size.x);
		VPoint biggest(biggestCo%size.x,biggestCo/size.x);
		// Now we check if all selected buttons are inside the selection square:
		for (int i=0;i<int(App::ct->buttonBlockContainer->selectedButtons.size());i++)
		{
			int butt=App::ct->buttonBlockContainer->selectedButtons[i];
			int x=butt%size.x;
			int y=butt/size.x;
			if (x<smallest.x)
				return;
			if (x>biggest.x)
				return;
			if (y<smallest.y)
				return;
			if (y>biggest.y)
				return;
		}
		// Now we check that the number of selected buttons is equal to the number of cells
		// inside the selection square:
		VPoint cellSize(biggest.x-smallest.x+1,biggest.y-smallest.y+1);
		if (cellSize.x*cellSize.y!=int(App::ct->buttonBlockContainer->selectedButtons.size()))
			return;
		// Last verification: we check that all selected cells are free:
		for (int i=0;i<int(App::ct->buttonBlockContainer->selectedButtons.size());i++)
		{
			int butt=App::ct->buttonBlockContainer->selectedButtons[i];
			if (itBlock->getButtonAtPos(butt%size.x,butt/size.x)!=NULL)
				return;
		}
		// Now we can create the merged button:
		CSoftButton* itButton=new CSoftButton("",smallest.x,smallest.y,cellSize.x,cellSize.y);
		if (itBlock->insertButton(itButton))
		{
			App::ct->buttonBlockContainer->deselectButtons();
			App::ct->buttonBlockContainer->addToSelection(smallest.x+smallest.y*size.x);
		}
		else
		{
			App::ct->buttonBlockContainer->deselectButtons();
			delete itButton;
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUiButtons::on_qqRemoveButtons_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		if (App::ct->buttonBlockContainer->selectedButtons.size()<1)
			return;
		VPoint size;
		itBlock->getBlockSize(size);
		for (int i=0;i<int(App::ct->buttonBlockContainer->selectedButtons.size());i++)
		{
			int val=App::ct->buttonBlockContainer->selectedButtons[i];
			itBlock->removeButtonFromPos(val%size.x,val/size.x,false);
			App::ct->undoBufferContainer->announceChangeStart(); // **************** UNDO THINGY ****************
		}
		itBlock->recomputePositionFastIndex();
		App::ct->undoBufferContainer->announceChangeEnd(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUiButtons::on_qqButtonHandle_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		bool ok;
		int newVal=ui->qqButtonHandle->text().toInt(&ok);
		if (ok)
		{
			if ((newVal>=0)&&(!itBlock->doesButtonIDExist(newVal)))
			{
				itButton->buttonID=newVal;
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			}
		}
		refresh();
	}
}

void CQDlgUiButtons::on_qqTypeCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
		{
			CSoftButton* itButton=getLastSelectedButton();
			if (itButton==NULL)
				return;
			itButton->setAttributes((itButton->getAttributes()&0xfff8)|(ui->qqTypeCombo->itemData(ui->qqTypeCombo->currentIndex()).toInt()));
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgUiButtons::on_qqEnabled_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		itButton->setAttributes(itButton->getAttributes()^sim_buttonproperty_enabled);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUiButtons::on_qqStayDown_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		itButton->setAttributes(itButton->getAttributes()^sim_buttonproperty_staydown);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUiButtons::on_qqRollUp_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		itButton->setAttributes(itButton->getAttributes()^sim_buttonproperty_rollupaction);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUiButtons::on_qqCenteredH_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		itButton->setAttributes(itButton->getAttributes()^sim_buttonproperty_horizontallycentered);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUiButtons::on_qqUpDownEvent_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		itButton->setAttributes(itButton->getAttributes()^sim_buttonproperty_downupevent);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUiButtons::on_qqCloseAction_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		itButton->setAttributes(itButton->getAttributes()^sim_buttonproperty_closeaction);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUiButtons::on_qqCenteredV_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		itButton->setAttributes(itButton->getAttributes()^sim_buttonproperty_verticallycentered);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUiButtons::on_qqBorderless_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		itButton->setAttributes(itButton->getAttributes()^sim_buttonproperty_borderless);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUiButtons::on_qqIgnoreMouse_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		itButton->setAttributes(itButton->getAttributes()^sim_buttonproperty_ignoremouse);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUiButtons::on_qqApplyType_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		if (App::ct->buttonBlockContainer->selectedButtons.size()<2)
			return;
		VPoint size;
		itBlock->getBlockSize(size);
		int butt=App::ct->buttonBlockContainer->selectedButtons[App::ct->buttonBlockContainer->selectedButtons.size()-1];
		CSoftButton* itButton=itBlock->getButtonAtPos(butt%size.x,butt/size.x);
		if (itButton!=NULL)
		{
			for (int i=0;i<int(App::ct->buttonBlockContainer->selectedButtons.size())-1;i++)
			{
				int butt2=App::ct->buttonBlockContainer->selectedButtons[i];
				CSoftButton* itButton2=itBlock->getButtonAtPos(butt2%size.x,butt2/size.x);
				if (itButton2!=NULL)
				{
					int atrToCopy=0x0007|sim_buttonproperty_staydown|sim_buttonproperty_enabled|
						sim_buttonproperty_horizontallycentered|sim_buttonproperty_ignoremouse|
						sim_buttonproperty_borderless|sim_buttonproperty_verticallycentered|sim_buttonproperty_downupevent;
					int oldAtr=itButton2->getAttributes()|atrToCopy;
					oldAtr-=atrToCopy;
					oldAtr|=itButton->getAttributes()&atrToCopy;
					itButton2->setAttributes(oldAtr);
					App::ct->undoBufferContainer->announceChangeStart(); // **************** UNDO THINGY ****************
				}
			}
			App::ct->undoBufferContainer->announceChangeEnd(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgUiButtons::on_qqLabelUp_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		itButton->label=ui->qqLabelUp->text().toStdString();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUiButtons::on_qqLabelDown_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		itButton->downLabel=ui->qqLabelDown->text().toStdString();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUiButtons::on_qqApplyLabel_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		if (App::ct->buttonBlockContainer->selectedButtons.size()<2)
			return;
		int butt=App::ct->buttonBlockContainer->selectedButtons[App::ct->buttonBlockContainer->selectedButtons.size()-1];
		VPoint size;
		itBlock->getBlockSize(size);
		CSoftButton* itButton=itBlock->getButtonAtPos(butt%size.x,butt/size.x);
		if (itButton==NULL)
			return;
		for (int i=0;i<int(App::ct->buttonBlockContainer->selectedButtons.size())-1;i++)
		{
			butt=App::ct->buttonBlockContainer->selectedButtons[i];
			CSoftButton* it2Button=itBlock->getButtonAtPos(butt%size.x,butt/size.x);
			if (it2Button!=NULL)
			{
				for (int j=0;j<3;j++)
				{
					it2Button->label=itButton->label;
					it2Button->downLabel=itButton->downLabel;
				}
				App::ct->undoBufferContainer->announceChangeStart(); // **************** UNDO THINGY ****************
			}
		}
		App::ct->undoBufferContainer->announceChangeEnd(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUiButtons::on_qqColorUp_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		int identification[4]={itButton->getLifeID(),-1,-1,-1};
		CQDlgColor::displayModal(identification,itButton->backgroundColor,this);
	}
}

void CQDlgUiButtons::on_qqColorDown_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		int identification[4]={itButton->getLifeID(),-1,-1,-1};
		CQDlgColor::displayModal(identification,itButton->downBackgroundColor,this);
	}
}

void CQDlgUiButtons::on_qqColorLabel_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		int identification[4]={itButton->getLifeID(),-1,-1,-1};
		CQDlgColor::displayModal(identification,itButton->textColor,this);
	}
}

void CQDlgUiButtons::on_qqApplyColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		if (App::ct->buttonBlockContainer->selectedButtons.size()<2)
			return;
		int butt=App::ct->buttonBlockContainer->selectedButtons[App::ct->buttonBlockContainer->selectedButtons.size()-1];
		VPoint size;
		itBlock->getBlockSize(size);
		CSoftButton* itButton=itBlock->getButtonAtPos(butt%size.x,butt/size.x);
		if (itButton==NULL)
			return;
		for (int i=0;i<int(App::ct->buttonBlockContainer->selectedButtons.size())-1;i++)
		{
			butt=App::ct->buttonBlockContainer->selectedButtons[i];
			CSoftButton* it2Button=itBlock->getButtonAtPos(butt%size.x,butt/size.x);
			if (it2Button!=NULL)
			{
				for (int j=0;j<3;j++)
				{
					it2Button->backgroundColor[j]=itButton->backgroundColor[j];
					it2Button->downBackgroundColor[j]=itButton->downBackgroundColor[j];
					it2Button->textColor[j]=itButton->textColor[j];
				}
				App::ct->undoBufferContainer->announceChangeStart(); // **************** UNDO THINGY ****************
			}
		}
		App::ct->undoBufferContainer->announceChangeEnd(); // **************** UNDO THINGY ****************
	}
}

void CQDlgUiButtons::on_qqTransparent_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		if ((itButton->getAttributes()&sim_buttonproperty_transparent)==0)
			itButton->setAttributes(itButton->getAttributes()&(0xffff-sim_buttonproperty_nobackgroundcolor));
		itButton->setAttributes(itButton->getAttributes()^sim_buttonproperty_transparent);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUiButtons::on_qqNoBackground_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		if ((itButton->getAttributes()&sim_buttonproperty_nobackgroundcolor)==0)
			itButton->setAttributes(itButton->getAttributes()&(0xffff-sim_buttonproperty_transparent));
		itButton->setAttributes(itButton->getAttributes()^sim_buttonproperty_nobackgroundcolor);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgUiButtons::on_qqSetTexture_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		CSoftButton* itButton=getLastSelectedButton();
		if (itButton==NULL)
			return;
		int identification[4]={itButton->getLifeID(),2,itBlock->getBlockID(),itButton->getUniqueID()};
		CQDlgTextures::displayModal(identification,this);
	}
}

void CQDlgUiButtons::on_qqApplyOtherProperties_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CButtonBlock* itBlock=App::ct->buttonBlockContainer->getBlockWithID(App::ct->buttonBlockContainer->getBlockInEdition());
		if (itBlock==NULL)
			return;
		if (App::ct->buttonBlockContainer->selectedButtons.size()<2)
			return;
		VPoint size;
		itBlock->getBlockSize(size);
		int butt=App::ct->buttonBlockContainer->selectedButtons[App::ct->buttonBlockContainer->selectedButtons.size()-1];
		CSoftButton* itButton=itBlock->getButtonAtPos(butt%size.x,butt/size.x);
		if (itButton!=NULL)
		{
			for (int i=0;i<int(App::ct->buttonBlockContainer->selectedButtons.size())-1;i++)
			{
				int butt2=App::ct->buttonBlockContainer->selectedButtons[i];
				CSoftButton* itButton2=itBlock->getButtonAtPos(butt2%size.x,butt2/size.x);
				if (itButton2!=NULL)
				{
					int atrToCopy=sim_buttonproperty_transparent|sim_buttonproperty_nobackgroundcolor;
					int oldAtr=itButton2->getAttributes()|atrToCopy;
					oldAtr-=atrToCopy;
					oldAtr|=itButton->getAttributes()&atrToCopy;
					itButton2->setAttributes(oldAtr);

					if (itButton2->getTextureProperty()!=NULL)
					{ // We anyway remove the texture first
						App::ct->textureCont->announceGeneralObjectWillBeErased(itBlock->getBlockID(),itButton2->getUniqueID());
						delete itButton2->getTextureProperty();
						itButton2->setTextureProperty(NULL);
					}
					if (itButton->getTextureProperty()!=NULL)
					{
						int textObjID=itButton->getTextureProperty()->getTextureObjectID();
						if ((textObjID>=SIM_IDSTART_TEXTURE)&&(textObjID<=SIM_IDEND_TEXTURE))
						{
							CTextureObject* to=App::ct->textureCont->getObject(textObjID);
							to->addDependentObject(itBlock->getBlockID(),itButton2->getUniqueID());
						}
						itButton2->setTextureProperty(itButton->getTextureProperty()->copyYourself());
					}
					App::ct->undoBufferContainer->announceChangeStart(); // **************** UNDO THINGY ****************
				}
			}
		}
		App::ct->undoBufferContainer->announceChangeEnd(); // **************** UNDO THINGY ****************
		refresh();
	}
}
