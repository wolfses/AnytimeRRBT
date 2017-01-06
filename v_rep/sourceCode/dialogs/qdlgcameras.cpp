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
#include "qdlgcameras.h"
#include "ui_qdlgcameras.h"
#include "Tt.h"
#include "GV.h"
#include "qdlgmaterial.h"
#include "App.h"
#include "v_repStrings.h"

CQDlgCameras::CQDlgCameras(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgCameras)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
}

CQDlgCameras::~CQDlgCameras()
{
    delete ui;
}

void CQDlgCameras::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgCameras::refresh()
{
	inMainRefreshRoutine=true;
	CCamera* it=NULL;
	if (App::ct->objCont->isLastSelectionACamera())
		it=(CCamera*)App::ct->objCont->getLastSelection();

	ui->qqAlongX->setEnabled(it!=NULL);
	ui->qqAlongY->setEnabled(it!=NULL);
	ui->qqAlongZ->setEnabled(it!=NULL);
	ui->qqColorA->setEnabled(it!=NULL);
	ui->qqColorB->setEnabled(it!=NULL);
	ui->qqFarClipping->setEnabled(it!=NULL);
	ui->qqNearClipping->setEnabled(it!=NULL);
	ui->qqFullRotation->setEnabled(it!=NULL);
	ui->qqKeepHeadUp->setEnabled(it!=NULL);
	ui->qqLocalLights->setEnabled(it!=NULL);
	ui->qqAllowPicking->setEnabled(it!=NULL);
	ui->qqManipProxy->setEnabled(it!=NULL);
	ui->qqPerspectiveProjectionAngle->setEnabled(it!=NULL);
	ui->qqOrthographicProjectionSize->setEnabled(it!=NULL);
	ui->qqShowFog->setEnabled(it!=NULL);
	ui->qqSize->setEnabled(it!=NULL);
	ui->qqTilting->setEnabled(it!=NULL);
	ui->qqTrackedCombo->setEnabled(it!=NULL);

	ui->qqTrackedCombo->clear();

	if (it!=NULL)
	{
		ui->qqManipProxy->setChecked(it->getUseParentObjectAsManipulationProxy());
		ui->qqKeepHeadUp->setChecked((it->getCameraManipulationModePermissions()&0x020)!=0);
		ui->qqFullRotation->setChecked((it->getCameraManipulationModePermissions()&0x008)!=0);
		ui->qqTilting->setChecked((it->getCameraManipulationModePermissions()&0x010)!=0);
		ui->qqAlongX->setChecked((it->getCameraManipulationModePermissions()&0x001)!=0);
		ui->qqAlongY->setChecked((it->getCameraManipulationModePermissions()&0x002)!=0);
		ui->qqAlongZ->setChecked((it->getCameraManipulationModePermissions()&0x004)!=0);
		ui->qqPerspectiveProjectionAngle->setText(gv::getAngleStr(false,it->getViewAngle(),0).c_str());
		ui->qqOrthographicProjectionSize->setText(gv::getSizeStr(false,it->getOrthoViewSize(),0).c_str());
		ui->qqSize->setText(gv::getSizeStr(false,it->getCameraSize(),0).c_str());
		ui->qqNearClipping->setText(gv::getSizeStr(false,it->getNearClippingPlane(),0).c_str());
		ui->qqFarClipping->setText(gv::getSizeStr(false,it->getFarClippingPlane(),0).c_str());
		ui->qqShowFog->setChecked(it->getShowFogIfAvailable());
		ui->qqLocalLights->setChecked(it->getuseLocalLights());
		ui->qqAllowPicking->setChecked(it->getAllowPicking());

		ui->qqTrackedCombo->addItem(strTranslate(IDSN_NONE),QVariant(-1));
		std::vector<std::string> names;
		std::vector<int> ids;
		for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
		{
			C3DObject* it2=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
			if (it2!=it)
			{
				names.push_back(it2->getName());
				ids.push_back(it2->getID());
			}
		}
		tt::orderStrings(names,ids);
		for (int i=0;i<int(names.size());i++)
			ui->qqTrackedCombo->addItem(names[i].c_str(),QVariant(ids[i]));
		for (int i=0;i<ui->qqTrackedCombo->count();i++)
		{
			if (ui->qqTrackedCombo->itemData(i).toInt()==it->getTrackedObjectID())
			{
				ui->qqTrackedCombo->setCurrentIndex(i);
				break;
			}
		}
	}
	else
	{
		ui->qqAlongX->setChecked(false);
		ui->qqAlongY->setChecked(false);
		ui->qqAlongZ->setChecked(false);
		ui->qqFullRotation->setChecked(false);
		ui->qqKeepHeadUp->setChecked(false);
		ui->qqLocalLights->setChecked(false);
		ui->qqManipProxy->setChecked(false);
		ui->qqShowFog->setChecked(false);
		ui->qqTilting->setChecked(false);
		ui->qqAllowPicking->setChecked(false);

		ui->qqFarClipping->setText("");
		ui->qqNearClipping->setText("");
		ui->qqPerspectiveProjectionAngle->setText("");
		ui->qqOrthographicProjectionSize->setText("");
		ui->qqSize->setText("");
	}
	inMainRefreshRoutine=false;
}

void CQDlgCameras::on_qqPerspectiveProjectionAngle_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqPerspectiveProjectionAngle->text().toFloat(&ok);
		if (ok)
		{
			float v=newVal*gv::userToRad;
			tt::limitValue(1.0f*degToRad,135.0f*degToRad,v); // with 90 degrees, objects disappear!! Really??? Changed to 135 on 2010/11/12
			((CCamera*)App::ct->objCont->getLastSelection())->setViewAngle(v);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgCameras::on_qqOrthographicProjectionSize_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqOrthographicProjectionSize->text().toFloat(&ok);
		if (ok)
		{
			float v=newVal*gv::userToMeter;
			tt::limitValue(0.001f,200000.0f,v);
			((CCamera*)App::ct->objCont->getLastSelection())->setOrthoViewSize(v);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgCameras::on_qqNearClipping_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqNearClipping->text().toFloat(&ok);
		if (ok)
		{
			float ncp=newVal*gv::userToMeter;
			tt::limitValue(0.0001f,((CCamera*)App::ct->objCont->getLastSelection())->getFarClippingPlane(),ncp);
			((CCamera*)App::ct->objCont->getLastSelection())->setNearClippingPlane(ncp);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgCameras::on_qqFarClipping_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqFarClipping->text().toFloat(&ok);
		if (ok)
		{
			float ncp=newVal*gv::userToMeter;
			tt::limitValue(((CCamera*)App::ct->objCont->getLastSelection())->getNearClippingPlane(),100000.0f,ncp);
			((CCamera*)App::ct->objCont->getLastSelection())->setFarClippingPlane(ncp);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgCameras::on_qqTrackedCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!inMainRefreshRoutine)
		{
			if (!App::ct->objCont->isLastSelectionACamera())
				return;
			int objID=ui->qqTrackedCombo->itemData(ui->qqTrackedCombo->currentIndex()).toInt();
			CCamera* it=(CCamera*)App::ct->objCont->getLastSelection();
			it->setTrackedObjectID(objID);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgCameras::on_qqShowFog_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionACamera())
			return;
		CCamera* it=(CCamera*)App::ct->objCont->getLastSelection();
		it->setShowFogIfAvailable(!it->getShowFogIfAvailable());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCameras::on_qqKeepHeadUp_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionACamera())
			return;
		CCamera* it=(CCamera*)App::ct->objCont->getLastSelection();
		it->setCameraManipulationModePermissions(it->getCameraManipulationModePermissions()^0x020);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCameras::on_qqManipProxy_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionACamera())
			return;
		CCamera* it=(CCamera*)App::ct->objCont->getLastSelection();
		it->setUseParentObjectAsManipulationProxy(!it->getUseParentObjectAsManipulationProxy());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCameras::on_qqAlongX_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionACamera())
			return;
		CCamera* it=(CCamera*)App::ct->objCont->getLastSelection();
		it->setCameraManipulationModePermissions(it->getCameraManipulationModePermissions()^0x001);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCameras::on_qqAlongY_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionACamera())
			return;
		CCamera* it=(CCamera*)App::ct->objCont->getLastSelection();
		it->setCameraManipulationModePermissions(it->getCameraManipulationModePermissions()^0x002);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCameras::on_qqAlongZ_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionACamera())
			return;
		CCamera* it=(CCamera*)App::ct->objCont->getLastSelection();
		it->setCameraManipulationModePermissions(it->getCameraManipulationModePermissions()^0x004);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCameras::on_qqFullRotation_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionACamera())
			return;
		CCamera* it=(CCamera*)App::ct->objCont->getLastSelection();
		it->setCameraManipulationModePermissions(it->getCameraManipulationModePermissions()^0x008);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCameras::on_qqTilting_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionACamera())
			return;
		CCamera* it=(CCamera*)App::ct->objCont->getLastSelection();
		it->setCameraManipulationModePermissions(it->getCameraManipulationModePermissions()^0x010);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCameras::on_qqColorA_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionACamera())
			return;
		CCamera* it=(CCamera*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&it->colorPart1,App::mainWindow,true,true,true,true,true,false,true,false);
	}
}

void CQDlgCameras::on_qqColorB_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionACamera())
			return;
		CCamera* it=(CCamera*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&it->colorPart2,App::mainWindow,true,true,true,true,true,false,true,false);
	}
}

void CQDlgCameras::on_qqSize_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqSize->text().toFloat(&ok);
		if (ok)
		{
			((CCamera*)App::ct->objCont->getLastSelection())->setCameraSize(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}


void CQDlgCameras::on_qqLocalLights_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionACamera())
			return;
		CCamera* it=(CCamera*)App::ct->objCont->getLastSelection();
		it->setUseLocalLights(!it->getuseLocalLights());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCameras::on_qqAllowPicking_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionACamera())
			return;
		CCamera* it=(CCamera*)App::ct->objCont->getLastSelection();
		it->setAllowPicking(!it->getAllowPicking());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}
