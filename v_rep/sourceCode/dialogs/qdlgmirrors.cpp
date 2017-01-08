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
#include "qdlgmirrors.h"
#include "ui_qdlgmirrors.h"
#include "Tt.h"
#include "GV.h"
#include "qdlgmaterial.h"
#include "qdlgcolor.h"
#include "App.h"
#include "v_repStrings.h"

CQDlgMirrors::CQDlgMirrors(QWidget *parent) :
	CDlgEx(parent),
	ui(new Ui::CQDlgMirrors)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
}

CQDlgMirrors::~CQDlgMirrors()
{
    delete ui;
}

void CQDlgMirrors::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgMirrors::refresh()
{
	inMainRefreshRoutine=true;
	CMirror* it=NULL;
	if (App::ct->objCont->isLastSelectionAMirror())
		it=(CMirror*)App::ct->objCont->getLastSelection();
	bool simStopped=App::ct->simulation->isSimulationStopped();
	bool bigSel=(App::ct->objCont->getMirrorNumberInSelection()>1);


	ui->qqDisableAllMirrors->setChecked(App::ct->mainSettings->mirrorsDisabled);
	ui->qqDisableAllClippingPlanes->setChecked(App::ct->mainSettings->clippingPlanesDisabled);


	ui->qqEnabled->setEnabled(it!=NULL);
	ui->qqWidth->setEnabled(it!=NULL);
	ui->qqHeight->setEnabled(it!=NULL);
	ui->qqReflectance->setEnabled((it!=NULL)&&it->getIsMirror());
	ui->qqColor->setEnabled((it!=NULL));

	ui->qqIsMirror->setEnabled((it!=NULL)&&simStopped);
	ui->qqIsClippingPlane->setEnabled((it!=NULL)&&simStopped);

	ui->qqApplyAll->setEnabled(bigSel&&simStopped);

	ui->qqEntityCombo->setEnabled(it!=NULL);
	ui->qqEntityCombo->clear();

	if (it!=NULL)
	{
		ui->qqEnabled->setChecked(it->getActive());

		ui->qqWidth->setText(gv::getSizeStr(false,it->getMirrorWidth(),0).c_str());
		ui->qqHeight->setText(gv::getSizeStr(false,it->getMirrorHeight(),0).c_str());
		ui->qqReflectance->setText(tt::FNb(0,it->getReflectance(),2,false).c_str());
		ui->qqIsMirror->setChecked(it->getIsMirror());
		ui->qqIsClippingPlane->setChecked(!it->getIsMirror());

		if (!it->getIsMirror())
		{

			ui->qqEntityCombo->addItem(strTranslate(IDS_ALL_VISIBLE_OBJECTS_IN_SCENE),QVariant(-1));

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
				ui->qqEntityCombo->addItem(names[i].c_str(),QVariant(ids[i]));

			names.clear();
			ids.clear();

			// Now objects:
			for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
			{
				C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
				std::string name(tt::decorateString("[",strTranslate(IDS_OBJECT),"] "));
				name+=it->getName();
				names.push_back(name);
				ids.push_back(it->getID());
			}
			tt::orderStrings(names,ids);
			for (int i=0;i<int(names.size());i++)
				ui->qqEntityCombo->addItem(names[i].c_str(),QVariant(ids[i]));

			// Select current item:
			for (int i=0;i<ui->qqEntityCombo->count();i++)
			{
				if (ui->qqEntityCombo->itemData(i).toInt()==it->getClippingObjectOrCollection())
				{
					ui->qqEntityCombo->setCurrentIndex(i);
					break;
				}
			}
		}
	}
	else
	{
		ui->qqEnabled->setChecked(false);
		ui->qqIsMirror->setChecked(true);
		ui->qqIsClippingPlane->setChecked(false);
		ui->qqWidth->setText("");
		ui->qqHeight->setText("");
		ui->qqReflectance->setText("");
	}
	inMainRefreshRoutine=false;
}

void CQDlgMirrors::on_qqEnabled_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CMirror* it=(CMirror*)App::ct->objCont->getLastSelection();
		it->setActive(!it->getActive());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgMirrors::on_qqWidth_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAMirror())
			return;
		bool ok;
		float newVal=ui->qqWidth->text().toFloat(&ok);
		if (ok)
		{
			((CMirror*)App::ct->objCont->getLastSelection())->setMirrorWidth(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgMirrors::on_qqHeight_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAMirror())
			return;
		bool ok;
		float newVal=ui->qqHeight->text().toFloat(&ok);
		if (ok)
		{
			((CMirror*)App::ct->objCont->getLastSelection())->setMirrorHeight(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgMirrors::on_qqColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAMirror())
			return;
		CMirror* it=(CMirror*)App::ct->objCont->getLastSelection();
		if (it->getIsMirror())
		{
			int identification[4]={it->getLifeID(),0,it->getID(),-1};
			CQDlgColor::display(identification,it->mirrorColor,App::mainWindow);
		}
		else
		{
			int identification[4]={it->getLifeID(),0,it->getID(),-1};
			CQDlgMaterial::display(identification,&it->clipPlaneColor,App::mainWindow,true,true,true,true,true,true,true,false);
		}
	}
}


void CQDlgMirrors::on_qqReflectance_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAMirror())
			return;
		bool ok;
		float newVal=ui->qqReflectance->text().toFloat(&ok);
		if (ok)
		{
			((CMirror*)App::ct->objCont->getLastSelection())->setReflectance(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgMirrors::on_qqIsMirror_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAMirror())
			return;
		CMirror* it=(CMirror*)App::ct->objCont->getLastSelection();
		it->setIsMirror(true);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgMirrors::on_qqIsClippingPlane_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAMirror())
			return;
		CMirror* it=(CMirror*)App::ct->objCont->getLastSelection();
		it->setIsMirror(false);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgMirrors::on_qqApplyAll_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
	}
	if (!App::ct->objCont->isLastSelectionAMirror())
		return;
	if (App::ct->objCont->getMirrorNumberInSelection()<2)
		return;
	CMirror* last=(CMirror*)App::ct->objCont->getLastSelection();
	for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
	{
		CMirror* it=App::ct->objCont->getMirror(App::ct->objCont->getSelID(i));
		if (it!=NULL)
		{
			it->setIsMirror(last->getIsMirror());
			it->setActive(last->getActive());
			it->setMirrorWidth(last->getMirrorWidth());
			it->setMirrorHeight(last->getMirrorHeight());
			it->setReflectance(last->getReflectance());
			it->mirrorColor[0]=last->mirrorColor[0];
			it->mirrorColor[1]=last->mirrorColor[1];
			it->mirrorColor[2]=last->mirrorColor[2];
			last->clipPlaneColor.copyYourselfInto(&it->clipPlaneColor);
			it->setClippingObjectOrCollection(last->getClippingObjectOrCollection());
			App::ct->undoBufferContainer->announceChangeStart(); // **************** UNDO THINGY ****************
		}
	}
	App::ct->undoBufferContainer->announceChangeEnd(); // **************** UNDO THINGY ****************
	refresh();
}

void CQDlgMirrors::on_qqEntityCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
		{
			if (!App::ct->objCont->isLastSelectionAMirror())
				return;
			int objID=ui->qqEntityCombo->itemData(ui->qqEntityCombo->currentIndex()).toInt();
			CMirror* it=(CMirror*)App::ct->objCont->getLastSelection();
			it->setClippingObjectOrCollection(objID);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgMirrors::on_qqDisableAllMirrors_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->mainSettings->mirrorsDisabled=!App::ct->mainSettings->mirrorsDisabled;
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgMirrors::on_qqDisableAllClippingPlanes_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->mainSettings->clippingPlanesDisabled=!App::ct->mainSettings->clippingPlanesDisabled;
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}
