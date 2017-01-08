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
#include "qdlglights.h"
#include "ui_qdlglights.h"
#include "Tt.h"
#include "GV.h"
#include "qdlgmaterial.h"
#include "App.h"

CQDlgLights::CQDlgLights(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgLights)
{
    ui->setupUi(this);
}

CQDlgLights::~CQDlgLights()
{
    delete ui;
}

void CQDlgLights::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgLights::refresh()
{
	CLight* it=NULL;
	if (App::ct->objCont->isLastSelectionALight())
		it=(CLight*)App::ct->objCont->getLastSelection();

	ui->qqEnabled->setEnabled(it!=NULL);
	ui->qqLocal->setEnabled(it!=NULL);
	ui->qqSize->setEnabled(it!=NULL);
	ui->qqLightColor->setEnabled(it!=NULL);

	if (it!=NULL)
	{
		int lt=it->getLightType();
		ui->qqEnabled->setChecked(it->getLightActive());
		ui->qqLocal->setChecked(it->getLightIsLocal());
		ui->qqCasingColor->setEnabled(lt!=sim_light_omnidirectional_subtype);

		ui->qqSize->setText(gv::getSizeStr(false,it->getLightSize(),0).c_str());

		ui->qqConstantFactor->setEnabled(lt!=sim_light_directional_subtype);
		ui->qqLinearFactor->setEnabled(lt!=sim_light_directional_subtype);
		ui->qqQuadraticFactor->setEnabled(lt!=sim_light_directional_subtype);
		ui->qqSpotExponent->setEnabled(lt==sim_light_spot_subtype);
		ui->qqSpotCutoff->setEnabled(lt==sim_light_spot_subtype);

		if (lt==sim_light_spot_subtype)
		{
			ui->qqSpotExponent->setText(tt::FNb(it->getSpotExponent()).c_str());
			ui->qqSpotCutoff->setText(gv::getAngleStr(false,it->getSpotCutoffAngle()).c_str());
		}
		else
		{
			ui->qqSpotExponent->setText("");
			ui->qqSpotCutoff->setText("");
		}
		if (lt!=sim_light_directional_subtype)
		{
			ui->qqConstantFactor->setText(tt::FNb(0,it->getAttenuationFactor(CONSTANT_ATTENUATION),3,false).c_str());
			ui->qqLinearFactor->setText(tt::FNb(0,it->getAttenuationFactor(LINEAR_ATTENUATION),3,false).c_str());
			ui->qqQuadraticFactor->setText(tt::FNb(0,it->getAttenuationFactor(QUADRATIC_ATTENUATION),3,false).c_str());
		}
		else
		{
			ui->qqConstantFactor->setText("");
			ui->qqLinearFactor->setText("");
			ui->qqQuadraticFactor->setText("");
		}
	}
	else
	{
		ui->qqConstantFactor->setEnabled(false);
		ui->qqLinearFactor->setEnabled(false);
		ui->qqQuadraticFactor->setEnabled(false);
		ui->qqSpotCutoff->setEnabled(false);
		ui->qqSpotExponent->setEnabled(false);
		ui->qqCasingColor->setEnabled(false);

		ui->qqEnabled->setChecked(false);
		ui->qqLocal->setChecked(false);
		ui->qqSize->setText("");
		ui->qqConstantFactor->setText("");
		ui->qqLinearFactor->setText("");
		ui->qqQuadraticFactor->setText("");
		ui->qqSpotCutoff->setText("");
		ui->qqSpotExponent->setText("");
	}
}

void CQDlgLights::on_qqEnabled_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CLight* it=(CLight*)App::ct->objCont->getLastSelection();
		it->setLightActive(!it->getLightActive());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgLights::on_qqLocal_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionALight())
			return;
		CLight* it=(CLight*)App::ct->objCont->getLastSelection();
		it->setLightIsLocal(!it->getLightIsLocal());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgLights::on_qqSize_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionALight())
			return;
		bool ok;
		float newVal=ui->qqSize->text().toFloat(&ok);
		if (ok)
		{
			((CLight*)App::ct->objCont->getLastSelection())->setLightSize(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgLights::on_qqSpotCutoff_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionALight())
			return;
		bool ok;
		float newVal=ui->qqSpotCutoff->text().toFloat(&ok);
		if (ok)
		{
			((CLight*)App::ct->objCont->getLastSelection())->setSpotCutoffAngle(newVal*gv::userToRad);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgLights::on_qqSpotExponent_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionALight())
			return;
		bool ok;
		int newVal=ui->qqSpotExponent->text().toInt(&ok);
		if (ok)
		{
			((CLight*)App::ct->objCont->getLastSelection())->setSpotExponent(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgLights::on_qqConstantFactor_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionALight())
			return;
		bool ok;
		float newVal=ui->qqConstantFactor->text().toFloat(&ok);
		if (ok)
		{
			((CLight*)App::ct->objCont->getLastSelection())->setAttenuationFactor(newVal,CONSTANT_ATTENUATION);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgLights::on_qqLinearFactor_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionALight())
			return;
		bool ok;
		float newVal=ui->qqLinearFactor->text().toFloat(&ok);
		if (ok)
		{
			((CLight*)App::ct->objCont->getLastSelection())->setAttenuationFactor(newVal,LINEAR_ATTENUATION);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgLights::on_qqQuadraticFactor_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionALight())
			return;
		bool ok;
		float newVal=ui->qqQuadraticFactor->text().toFloat(&ok);
		if (ok)
		{
			((CLight*)App::ct->objCont->getLastSelection())->setAttenuationFactor(newVal,QUADRATIC_ATTENUATION);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgLights::on_qqLightColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionALight())
			return;
		CLight* it=(CLight*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&it->lightColor,App::mainWindow,true,true,true,false,false,false,false,false);
	}
}

void CQDlgLights::on_qqCasingColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionALight())
			return;
		CLight* it=(CLight*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&it->objectColor,App::mainWindow,true,true,true,true,true,false,false,false);
	}
}

