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
#include "qdlgmills.h"
#include "ui_qdlgmills.h"
#include "Tt.h"
#include "GV.h"
#include "qdlgmaterial.h"
#include "IloIlo.h"
#include "qdlgdetectionvolume.h"
#include "v_repStrings.h"
#include "App.h"

CQDlgMills::CQDlgMills(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgMills)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
}

CQDlgMills::~CQDlgMills()
{
    delete ui;
}

void CQDlgMills::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgMills::refresh()
{
	inMainRefreshRoutine=true;
	CMill* it=NULL;
	if (App::ct->objCont->isLastSelectionAMill())
		it=(CMill*)App::ct->objCont->getLastSelection();


	bool isMill=App::ct->objCont->isLastSelectionAMill();
	bool manyMills=App::ct->objCont->getMillNumberInSelection()>1;
//	bool noEditMode=App::ct->objCont->getEditModeType()==NO_EDIT_MODE;
	bool noSim=App::ct->simulation->isSimulationStopped();

	ui->qqEnableAll->setChecked(App::ct->mainSettings->millsEnabled);

	ui->qqShowBase->setEnabled(isMill);
	ui->qqExplicitHandling->setEnabled(isMill);
	ui->qqBaseSize->setEnabled(isMill);
	ui->qqAdjustVolume->setEnabled(true);
	ui->qqAdjustVolume->setChecked(CQDlgDetectionVolume::showVolumeWindow);


	ui->qqApplyEntityToCut->setEnabled(isMill&&noSim&&manyMills);
	ui->qqEntityToCut->setEnabled(isMill&&noSim);
	ui->qqActiveVolumeColor->setEnabled(isMill);
	ui->qqPassiveVolumeColor->setEnabled(isMill);
	ui->qqApplyColors->setEnabled(isMill&&manyMills);

	ui->qqShowBase->setChecked(isMill&&it->getShowMillBase());
	ui->qqExplicitHandling->setChecked(isMill&&it->getExplicitHandling());
	ui->qqEntityToCut->clear();

	if (it!=NULL)
	{ // Last selection is a mill
		ui->qqBaseSize->setText(gv::getSizeStr(false,it->getSize(),0).c_str());

		ui->qqEntityToCut->addItem(strTranslate(IDSN_ALL_CUTTABLE_OBJECTS_IN_SCENE),QVariant(-1));

		std::vector<std::string> names;
		std::vector<int> ids;

		// Now collections:
		for (int i=0;i<int(App::ct->collections->allGroups.size());i++)
		{
			CRegGroup* it=App::ct->collections->allGroups[i];
			std::string name(tt::decorateString("[",strTranslate(IDSN_COLLECTION),"] "));
			name+=it->getGroupName();
			names.push_back(name);
			ids.push_back(it->getGroupID());
		}
		tt::orderStrings(names,ids);
		for (int i=0;i<int(names.size());i++)
			ui->qqEntityToCut->addItem(names[i].c_str(),QVariant(ids[i]));

		names.clear();
		ids.clear();

		// Now shapes:
		for (int i=0;i<int(App::ct->objCont->shapeList.size());i++)
		{
			CShape* it=App::ct->objCont->getShape(App::ct->objCont->shapeList[i]);
			std::string name(tt::decorateString("[",strTranslate(IDSN_SHAPE),"] "));
			name+=it->getName();
			names.push_back(name);
			ids.push_back(it->getID());
		}
		tt::orderStrings(names,ids);
		for (int i=0;i<int(names.size());i++)
			ui->qqEntityToCut->addItem(names[i].c_str(),QVariant(ids[i]));

		// Select current item:
		for (int i=0;i<ui->qqEntityToCut->count();i++)
		{
			if (ui->qqEntityToCut->itemData(i).toInt()==it->getMillableObject())
			{
				ui->qqEntityToCut->setCurrentIndex(i);
				break;
			}
		}
	}
	else
	{ // last selection is not a mill
		ui->qqBaseSize->setText("");
	}
	inMainRefreshRoutine=false;
}

void CQDlgMills::on_qqEnableAll_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->mainSettings->millsEnabled=!App::ct->mainSettings->millsEnabled;
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgMills::on_qqExplicitHandling_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAMill())
			return;
		CMill* theMill=(CMill*)App::ct->objCont->getLastSelection();
		theMill->setExplicitHandling(!theMill->getExplicitHandling());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgMills::on_qqShowBase_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAMill())
			return;
		CMill* it=(CMill*)App::ct->objCont->getLastSelection();
		it->setShowMillBase(!it->getShowMillBase());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgMills::on_qqBaseSize_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAMill())
			return;
		bool ok;
		float newVal=ui->qqBaseSize->text().toFloat(&ok);
		if (ok)
		{
			CMill* it=(CMill*)App::ct->objCont->getLastSelection();
			it->setSize(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgMills::on_qqAdjustVolume_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CQDlgDetectionVolume::showVolumeWindow=!CQDlgDetectionVolume::showVolumeWindow;
		if (App::mainWindow->dlgCont->isVisible(DETECTION_VOLUME_DLG)!=CQDlgDetectionVolume::showVolumeWindow)
			App::mainWindow->dlgCont->toggle(DETECTION_VOLUME_DLG);
	}
}

void CQDlgMills::on_qqEntityToCut_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAMill())
			return;
		if (!inMainRefreshRoutine)
		{
			int objID=ui->qqEntityToCut->itemData(ui->qqEntityToCut->currentIndex()).toInt();
			CMill* it=(CMill*)App::ct->objCont->getLastSelection();
			it->setMillableObject(objID);
			if (objID!=-1)
			{
				bool displayWarning=false;
				if ((objID<SIM_IDSTART_COLLECTION)&&(objID>=0))
				{
					C3DObject* it2=App::ct->objCont->getObject(objID);
					if (it2!=NULL)
						displayWarning|=((it2->getMainPropertyLocal()&sim_objectspecialproperty_cuttable)==0);
				}
				if (displayWarning)
					App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Scene object"),strTranslate(IDS_OBJECT_NOT_CUTTABLE_WARNING),VMessageBox::OKELI);
			}

			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgMills::on_qqApplyEntityToCut_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAMill())
			return;
		CMill* it=(CMill*)App::ct->objCont->getLastSelection();
		for (int i=0;i<App::ct->objCont->getSelSize();i++)
		{
			CMill* it2=App::ct->objCont->getMill(App::ct->objCont->getSelID(i));
			if (it2!=NULL)
			{
				if (it->getID()!=it2->getID())
					it2->setMillableObject(it->getMillableObject());
			}
		}
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
	}
}

void CQDlgMills::on_qqPassiveVolumeColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAMill())
			return;
		CMill* it=(CMill*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&it->passiveVolumeColor,App::mainWindow,true,true,true,true,false,false,true,false);
	}
}

void CQDlgMills::on_qqActiveVolumeColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAMill())
			return;
		CMill* it=(CMill*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&it->activeVolumeColor,App::mainWindow,true,true,true,true,false,false,true,false);
	}
}

void CQDlgMills::on_qqApplyColors_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAMill())
			return;
		CMill* it=(CMill*)App::ct->objCont->getLastSelection();
		for (int i=0;i<App::ct->objCont->getSelSize();i++)
		{
			CMill* it2=App::ct->objCont->getMill(App::ct->objCont->getSelID(i));
			if (it2!=NULL)
			{
				if (it->getID()!=it2->getID())
				{
					it->passiveVolumeColor.copyYourselfInto(&it2->passiveVolumeColor);
					it->activeVolumeColor.copyYourselfInto(&it2->activeVolumeColor);
				}
			}
		}
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
	}
}
