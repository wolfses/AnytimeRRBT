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
#include "qdlgpathedit.h"
#include "ui_qdlgpathedit.h"
#include "App.h"
#include "GV.h"
#include "Tt.h"
#include "v_repStrings.h"
#include <boost/lexical_cast.hpp>

CQDlgPathEdit::CQDlgPathEdit(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgPathEdit)
{
    ui->setupUi(this);
}

CQDlgPathEdit::~CQDlgPathEdit()
{
    delete ui;
}

void CQDlgPathEdit::cancelEvent()
{
	App::mainWindow->dlgCont->evaluateMenuSelection(TOGGLE_PATH_EDITION_DLG_CMD);
//	defaultModalDialogEndRoutine(false);
}

void CQDlgPathEdit::okEvent()
{
//	defaultModalDialogEndRoutine(true);
}

void CQDlgPathEdit::refresh()
{
	if (App::ct->objCont->getEditModeType()!=PATH_EDIT_MODE)
		return;
	CPathCont* pathCont=App::ct->objCont->_editionPath;
	int selectedPointCount=App::ct->objCont->editModeBuffer.size();

	ui->qqClosed->setChecked(((pathCont->getAttributes()&sim_pathproperty_closed_path)!=0));
	ui->qqFlat->setChecked(((pathCont->getAttributes()&sim_pathproperty_flat_path)!=0));
	ui->qqAutomaticOrientation->setChecked(((pathCont->getAttributes()&sim_pathproperty_automatic_orientation)!=0));
	ui->qqKeepXup->setChecked(((pathCont->getAttributes()&sim_pathproperty_keep_x_up)!=0));

	std::string tmp=std::string(IDS_TOTAL_PATH_POINTS)+": "+boost::lexical_cast<std::string>(App::ct->objCont->editModeBuffer.size())+"/"+
		boost::lexical_cast<std::string>(App::ct->objCont->_editionPath->getSimplePathPointCount());
	ui->qqInfo->setText(tmp.c_str());

	ui->qqFactor1->setEnabled(selectedPointCount!=0);
	ui->qqFactor2->setEnabled(selectedPointCount!=0);
	ui->qqPointCount->setEnabled(selectedPointCount!=0);
	ui->qqRelativeVelocity->setEnabled(selectedPointCount!=0);
	ui->qqVirtualDistance->setEnabled(selectedPointCount!=0);
	ui->qqAuxFlags->setEnabled(selectedPointCount!=0);
	ui->qqAuxChannel1->setEnabled(selectedPointCount!=0);
	ui->qqAuxChannel2->setEnabled(selectedPointCount!=0);
	ui->qqAuxChannel3->setEnabled(selectedPointCount!=0);
	ui->qqAuxChannel4->setEnabled(selectedPointCount!=0);

	ui->qqApply->setEnabled(selectedPointCount>1);

	if (selectedPointCount!=0)
	{
		CSimplePathPoint* it=App::ct->objCont->_editionPath->getSimplePathPoint(App::ct->objCont->editModeBuffer[App::ct->objCont->editModeBuffer.size()-1]);
		float bInt0,bInt1;
		it->getBezierFactors(bInt0,bInt1);
		ui->qqFactor1->setText(tt::FNb(0,bInt0,2,false).c_str());
		ui->qqFactor2->setText(tt::FNb(0,bInt1,2,false).c_str());
		ui->qqPointCount->setText(tt::FNb(it->getBezierPointCount()).c_str());
		ui->qqRelativeVelocity->setText(tt::FNb(0,it->getMaxRelAbsVelocity(),2,false).c_str());
		ui->qqVirtualDistance->setText(tt::FNb(0,it->getOnSpotDistance(),2,false).c_str());
		ui->qqAuxFlags->setText(tt::FNb(it->getAuxFlags()).c_str());
		float auxChannels[4];
		it->getAuxChannels(auxChannels);
		ui->qqAuxChannel1->setText(tt::FNb(0,auxChannels[0],4,false).c_str());
		ui->qqAuxChannel2->setText(tt::FNb(0,auxChannels[1],4,false).c_str());
		ui->qqAuxChannel3->setText(tt::FNb(0,auxChannels[2],4,false).c_str());
		ui->qqAuxChannel4->setText(tt::FNb(0,auxChannels[3],4,false).c_str());
	}
	else
	{
		ui->qqFactor1->setText("");
		ui->qqFactor2->setText("");
		ui->qqPointCount->setText("");
		ui->qqRelativeVelocity->setText("");
		ui->qqVirtualDistance->setText("");
		ui->qqAuxFlags->setText("");
		ui->qqAuxChannel1->setText("");
		ui->qqAuxChannel2->setText("");
		ui->qqAuxChannel3->setText("");
		ui->qqAuxChannel4->setText("");
	}
}

CPathCont* CQDlgPathEdit::getPathCont()
{
	CPathCont* pathCont=NULL;
	if (App::ct->objCont->getEditModeType()==PATH_EDIT_MODE)
		pathCont=App::ct->objCont->_editionPath;
	else
	{
		CPath* path=App::ct->objCont->getPath(App::ct->objCont->getLastSelectionID());
		if (path!=NULL)
			pathCont=path->pathContainer;
	}
	return(pathCont);
}

void CQDlgPathEdit::on_qqClosed_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		pathCont->setAttributes(pathCont->getAttributes()^sim_pathproperty_closed_path);
		refresh();
	}
}

void CQDlgPathEdit::on_qqFlat_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		pathCont->setAttributes(pathCont->getAttributes()^sim_pathproperty_flat_path);
		refresh();
	}
}

void CQDlgPathEdit::on_qqAutomaticOrientation_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		pathCont->setAttributes(pathCont->getAttributes()^sim_pathproperty_automatic_orientation);
		refresh();
	}
}

void CQDlgPathEdit::on_qqKeepXup_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		pathCont->setAttributes(pathCont->getAttributes()^sim_pathproperty_keep_x_up);
		refresh();
	}
}

void CQDlgPathEdit::on_qqFactor1_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if ((App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)==0)
			return;
		int selectedPointCount=App::ct->objCont->editModeBuffer.size();
		if (selectedPointCount==0)
			return;
		CSimplePathPoint* it=App::ct->objCont->_editionPath->getSimplePathPoint(App::ct->objCont->editModeBuffer[selectedPointCount-1]);
		float bInt0,bInt1;
		it->getBezierFactors(bInt0,bInt1);
		bool ok;
		float newVal=ui->qqFactor1->text().toFloat(&ok);
		if (ok)
			it->setBezierFactors(newVal,bInt1);
		App::ct->objCont->_editionPath->actualizePath();
		refresh();
	}
}

void CQDlgPathEdit::on_qqFactor2_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if ((App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)==0)
			return;
		int selectedPointCount=App::ct->objCont->editModeBuffer.size();
		if (selectedPointCount==0)
			return;
		CSimplePathPoint* it=App::ct->objCont->_editionPath->getSimplePathPoint(App::ct->objCont->editModeBuffer[selectedPointCount-1]);
		float bInt0,bInt1;
		it->getBezierFactors(bInt0,bInt1);
		bool ok;
		float newVal=ui->qqFactor2->text().toFloat(&ok);
		if (ok)
			it->setBezierFactors(bInt0,newVal);
		App::ct->objCont->_editionPath->actualizePath();
		refresh();
	}
}

void CQDlgPathEdit::on_qqPointCount_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if ((App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)==0)
			return;
		int selectedPointCount=App::ct->objCont->editModeBuffer.size();
		if (selectedPointCount==0)
			return;
		CSimplePathPoint* it=App::ct->objCont->_editionPath->getSimplePathPoint(App::ct->objCont->editModeBuffer[selectedPointCount-1]);
		bool ok;
		int newVal=ui->qqPointCount->text().toInt(&ok);
		if (ok)
			it->setBezierPointCount(newVal);
		App::ct->objCont->_editionPath->actualizePath();
		refresh();
	}
}

void CQDlgPathEdit::on_qqRelativeVelocity_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if ((App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)==0)
			return;
		int selectedPointCount=App::ct->objCont->editModeBuffer.size();
		if (selectedPointCount==0)
			return;
		CSimplePathPoint* it=App::ct->objCont->_editionPath->getSimplePathPoint(App::ct->objCont->editModeBuffer[selectedPointCount-1]);
		bool ok;
		float newVal=ui->qqRelativeVelocity->text().toFloat(&ok);
		if (ok)
			it->setMaxRelAbsVelocity(newVal);
		App::ct->objCont->_editionPath->actualizePath();
		refresh();
	}
}

void CQDlgPathEdit::on_qqVirtualDistance_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if ((App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)==0)
			return;
		int selectedPointCount=App::ct->objCont->editModeBuffer.size();
		if (selectedPointCount==0)
			return;
		CSimplePathPoint* it=App::ct->objCont->_editionPath->getSimplePathPoint(App::ct->objCont->editModeBuffer[selectedPointCount-1]);
		bool ok;
		float newVal=ui->qqVirtualDistance->text().toFloat(&ok);
		if (ok)
			it->setOnSpotDistance(newVal);
		App::ct->objCont->_editionPath->actualizePath();
		refresh();
	}
}

void CQDlgPathEdit::on_qqApply_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if ((App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)==0)
			return;
		int selectedPointCount=App::ct->objCont->editModeBuffer.size();
		if (selectedPointCount<2)
			return;
		CSimplePathPoint* it=App::ct->objCont->_editionPath->getSimplePathPoint(App::ct->objCont->editModeBuffer[selectedPointCount-1]);
		float bInt0,bInt1;
		it->getBezierFactors(bInt0,bInt1);
		for (int i=0;i<selectedPointCount-1;i++)
		{
			CSimplePathPoint* it2=App::ct->objCont->_editionPath->getSimplePathPoint(App::ct->objCont->editModeBuffer[i]);
			it2->setBezierFactors(bInt0,bInt1);
			it2->setMaxRelAbsVelocity(it->getMaxRelAbsVelocity());
			it2->setBezierPointCount(it->getBezierPointCount());
			it2->setOnSpotDistance(it->getOnSpotDistance());
			it2->setAuxFlags(it->getAuxFlags());
			float auxChannels[4];
			it->getAuxChannels(auxChannels);
			it2->setAuxChannels(auxChannels);
		}
		App::ct->objCont->_editionPath->actualizePath();
		refresh();
	}
}

void CQDlgPathEdit::on_qqClearSelection_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->objCont->deselectEditModeBuffer();
		refresh();
	}
}

void CQDlgPathEdit::on_qqInvertSelection_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		for (int i=0;i<int(App::ct->objCont->_editionPath->getSimplePathPointCount());i++)
			App::ct->objCont->xorAddItemToEditModeBuffer(i);
		refresh();
	}
}

void CQDlgPathEdit::on_qqAuxFlags_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if ((App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)==0)
			return;
		int selectedPointCount=App::ct->objCont->editModeBuffer.size();
		if (selectedPointCount==0)
			return;
		CSimplePathPoint* it=App::ct->objCont->_editionPath->getSimplePathPoint(App::ct->objCont->editModeBuffer[selectedPointCount-1]);
		bool ok;
		int newVal=ui->qqAuxFlags->text().toInt(&ok);
		if (ok)
		{
			tt::limitValue(0,65535,newVal);
			it->setAuxFlags(newVal);
		}
		App::ct->objCont->_editionPath->actualizePath();
		refresh();
	}
}

void CQDlgPathEdit::on_qqAuxChannel1_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if ((App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)==0)
			return;
		int selectedPointCount=App::ct->objCont->editModeBuffer.size();
		if (selectedPointCount==0)
			return;
		CSimplePathPoint* it=App::ct->objCont->_editionPath->getSimplePathPoint(App::ct->objCont->editModeBuffer[selectedPointCount-1]);
		bool ok;
		float newVal=ui->qqAuxChannel1->text().toFloat(&ok);
		if (ok)
		{
			float auxChannels[4];
			it->getAuxChannels(auxChannels);
			auxChannels[0]=newVal;
			it->setAuxChannels(auxChannels);
		}
		App::ct->objCont->_editionPath->actualizePath();
		refresh();
	}
}

void CQDlgPathEdit::on_qqAuxChannel2_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if ((App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)==0)
			return;
		int selectedPointCount=App::ct->objCont->editModeBuffer.size();
		if (selectedPointCount==0)
			return;
		CSimplePathPoint* it=App::ct->objCont->_editionPath->getSimplePathPoint(App::ct->objCont->editModeBuffer[selectedPointCount-1]);
		bool ok;
		float newVal=ui->qqAuxChannel2->text().toFloat(&ok);
		if (ok)
		{
			float auxChannels[4];
			it->getAuxChannels(auxChannels);
			auxChannels[1]=newVal;
			it->setAuxChannels(auxChannels);
		}
		App::ct->objCont->_editionPath->actualizePath();
		refresh();
	}
}

void CQDlgPathEdit::on_qqAuxChannel3_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if ((App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)==0)
			return;
		int selectedPointCount=App::ct->objCont->editModeBuffer.size();
		if (selectedPointCount==0)
			return;
		CSimplePathPoint* it=App::ct->objCont->_editionPath->getSimplePathPoint(App::ct->objCont->editModeBuffer[selectedPointCount-1]);
		bool ok;
		float newVal=ui->qqAuxChannel3->text().toFloat(&ok);
		if (ok)
		{
			float auxChannels[4];
			it->getAuxChannels(auxChannels);
			auxChannels[2]=newVal;
			it->setAuxChannels(auxChannels);
		}
		App::ct->objCont->_editionPath->actualizePath();
		refresh();
	}
}

void CQDlgPathEdit::on_qqAuxChannel4_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if ((App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)==0)
			return;
		int selectedPointCount=App::ct->objCont->editModeBuffer.size();
		if (selectedPointCount==0)
			return;
		CSimplePathPoint* it=App::ct->objCont->_editionPath->getSimplePathPoint(App::ct->objCont->editModeBuffer[selectedPointCount-1]);
		bool ok;
		float newVal=ui->qqAuxChannel4->text().toFloat(&ok);
		if (ok)
		{
			float auxChannels[4];
			it->getAuxChannels(auxChannels);
			auxChannels[3]=newVal;
			it->setAuxChannels(auxChannels);
		}
		App::ct->objCont->_editionPath->actualizePath();
		refresh();
	}

}
