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
#include "qdlgdummies.h"
#include "ui_qdlgdummies.h"
#include "Tt.h"
#include "GV.h"
#include "qdlgmaterial.h"
#include "App.h"
#include "v_repStrings.h"

CQDlgDummies::CQDlgDummies(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgDummies)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
}

CQDlgDummies::~CQDlgDummies()
{
    delete ui;
}

void CQDlgDummies::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgDummies::refresh()
{
	inMainRefreshRoutine=true;
//	bool simStopped=App::ct->simulation->isSimulationStopped();
	bool sel=App::ct->objCont->isLastSelectionADummy();
	bool bigSel=(App::ct->objCont->getDummyNumberInSelection()>1);
	CDummy* it=NULL;
	if (sel)
		it=(CDummy*)App::ct->objCont->getLastSelection();

	ui->qqSize->setEnabled(sel);
	ui->qqColor->setEnabled(sel);
	ui->qqApplyMainProperties->setEnabled(bigSel);

	ui->qqLinkedDummyCombo->setEnabled(sel);
	ui->qqLinkedDummyCombo->clear();
	ui->qqLinkTypeCombo->setEnabled(sel&&(it->getLinkedDummyID()!=-1));
	ui->qqLinkTypeCombo->clear();

	ui->qqfollowParentOrientation->setEnabled(sel);
	ui->qqFollow->setEnabled(sel);

	if (sel)
	{
		ui->qqSize->setText(gv::getSizeStr(false,it->getSize()).c_str());

		ui->qqLinkedDummyCombo->addItem(strTranslate(IDSN_NONE),QVariant(-1));
		std::vector<std::string> names;
		std::vector<int> ids;
		for (int i=0;i<int(App::ct->objCont->dummyList.size());i++)
		{
			CDummy* it2=App::ct->objCont->getDummy(App::ct->objCont->dummyList[i]);
			if (it2!=it)
			{
				names.push_back(it2->getName());
				ids.push_back(it2->getID());
			}
		}
		tt::orderStrings(names,ids);
		for (int i=0;i<int(names.size());i++)
			ui->qqLinkedDummyCombo->addItem(names[i].c_str(),QVariant(ids[i]));
		for (int i=0;i<ui->qqLinkedDummyCombo->count();i++)
		{
			if (ui->qqLinkedDummyCombo->itemData(i).toInt()==it->getLinkedDummyID())
			{
				ui->qqLinkedDummyCombo->setCurrentIndex(i);
				break;
			}
		}

		if (it->getLinkedDummyID()!=-1)
		{
			ui->qqLinkTypeCombo->addItem(strTranslate(IDS_DUMMY_LINK_TYPE_IK_TIP_TARGET),QVariant(sim_dummy_linktype_ik_tip_target));
			ui->qqLinkTypeCombo->addItem(strTranslate(IDS_DUMMY_LINK_TYPE_GCS_LOOP_CLOSURE),QVariant(sim_dummy_linktype_gcs_loop_closure));
			ui->qqLinkTypeCombo->addItem(strTranslate(IDS_DUMMY_LINK_TYPE_GCS_TIP),QVariant(sim_dummy_linktype_gcs_tip));
			ui->qqLinkTypeCombo->addItem(strTranslate(IDS_DUMMY_LINK_TYPE_GCS_TARGET),QVariant(sim_dummy_linktype_gcs_target));
			ui->qqLinkTypeCombo->addItem(strTranslate(IDS_DUMMY_LINK_TYPE_DYNAMICS_LOOP_CLOSURE),QVariant(sim_dummy_linktype_dynamics_loop_closure));
			// Here we select the appropriate item:
			for (int i=0;i<ui->qqLinkTypeCombo->count();i++)
			{
				if (ui->qqLinkTypeCombo->itemData(i).toInt()==it->getLinkType())
				{
					ui->qqLinkTypeCombo->setCurrentIndex(i);
					break;
				}
			}
		}

		ui->qqfollowParentOrientation->setChecked(it->getAssignedToParentPathOrientation());
		ui->qqFollow->setChecked(it->getAssignedToParentPath());

		ui->qqFree->setEnabled(it->getAssignedToParentPath());
		ui->qqFixed->setEnabled(it->getAssignedToParentPath());
		ui->qqFree->setChecked(it->getAssignedToParentPath()&&it->getFreeOnPathTrajectory());
		ui->qqFixed->setChecked(it->getAssignedToParentPath()&&(!it->getFreeOnPathTrajectory()));


		ui->qqOffset->setEnabled(!it->getFreeOnPathTrajectory());
		ui->qqIncrement->setEnabled(!it->getFreeOnPathTrajectory());
		if (it->getAssignedToParentPath()&&(!it->getFreeOnPathTrajectory()))
		{
			ui->qqOffset->setText(gv::getSizeStr(true,it->getVirtualDistanceOffsetOnPath()).c_str());
			ui->qqIncrement->setText(gv::getSizeStr(true,it->getVirtualDistanceOffsetOnPath_variationWhenCopy()).c_str());
		}
		else
		{
			ui->qqOffset->setText("");
			ui->qqIncrement->setText("");
		}
	}
	else
	{
		ui->qqSize->setText("");
		ui->qqfollowParentOrientation->setChecked(false);
		ui->qqFollow->setChecked(false);
		ui->qqFree->setEnabled(false);
		ui->qqFree->setChecked(false);
		ui->qqFixed->setEnabled(false);
		ui->qqFixed->setChecked(false);
		ui->qqOffset->setEnabled(false);
		ui->qqOffset->setText("");
		ui->qqIncrement->setEnabled(false);
		ui->qqIncrement->setText("");
	}


	inMainRefreshRoutine=false;
}

void CQDlgDummies::on_qqSize_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionADummy())
			return;
		CDummy* it=(CDummy*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqSize->text().toFloat(&ok);
		if (ok)
		{
			it->setSize(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgDummies::on_qqColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionADummy())
			return;
		CDummy* it=(CDummy*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&it->color,App::mainWindow,true,true,true,true,true,false,true,false);
	}
}

void CQDlgDummies::on_qqApplyMainProperties_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionADummy())
			return;
		CDummy* it=(CDummy*)App::ct->objCont->getLastSelection();
		for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
		{
			CDummy* it2=App::ct->objCont->getDummy(App::ct->objCont->getSelID(i));
			if (it2!=NULL)
			{
				it->color.copyYourselfInto(&it2->color);
				it2->setSize(it->getSize());
				App::ct->undoBufferContainer->announceChangeStart(); // **************** UNDO THINGY ****************
			}
		}
		App::ct->undoBufferContainer->announceChangeEnd(); // **************** UNDO THINGY ****************
	}
}

void CQDlgDummies::on_qqLinkedDummyCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!inMainRefreshRoutine)
		{
			if (!App::ct->objCont->isLastSelectionADummy())
				return;
			CDummy* it=(CDummy*)App::ct->objCont->getLastSelection();
			int objID=ui->qqLinkedDummyCombo->itemData(ui->qqLinkedDummyCombo->currentIndex()).toInt();
			it->setLinkedDummyID(objID,false);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgDummies::on_qqLinkTypeCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!inMainRefreshRoutine)
		{
			if (!App::ct->objCont->isLastSelectionADummy())
				return;
			CDummy* it=(CDummy*)App::ct->objCont->getLastSelection();
			int dataID=ui->qqLinkTypeCombo->itemData(ui->qqLinkTypeCombo->currentIndex()).toInt();
			it->setLinkType(dataID,false);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgDummies::on_qqfollowParentOrientation_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionADummy())
			return;
		CDummy* it=(CDummy*)App::ct->objCont->getLastSelection();
		it->setAssignedToParentPathOrientation(!it->getAssignedToParentPathOrientation());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgDummies::on_qqFollow_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionADummy())
			return;
		CDummy* it=(CDummy*)App::ct->objCont->getLastSelection();
		it->setAssignedToParentPath(!it->getAssignedToParentPath());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgDummies::on_qqFree_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionADummy())
			return;
		CDummy* it=(CDummy*)App::ct->objCont->getLastSelection();
		it->setFreeOnPathTrajectory(true);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgDummies::on_qqFixed_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionADummy())
			return;
		CDummy* it=(CDummy*)App::ct->objCont->getLastSelection();
		it->setFreeOnPathTrajectory(false);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgDummies::on_qqOffset_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionADummy())
			return;
		CDummy* it=(CDummy*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqOffset->text().toFloat(&ok);
		if (ok)
		{
			it->setVirtualDistanceOffsetOnPath(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgDummies::on_qqIncrement_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionADummy())
			return;
		CDummy* it=(CDummy*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqIncrement->text().toFloat(&ok);
		if (ok)
		{
			it->setVirtualDistanceOffsetOnPath_variationWhenCopy(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}
