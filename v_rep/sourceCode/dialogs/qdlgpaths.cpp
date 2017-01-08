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
#include "qdlgpaths.h"
#include "ui_qdlgpaths.h"
#include "Tt.h"
#include "GV.h"
#include "qdlgmaterial.h"
#include "App.h"
#include "dlgpathvelocityprofile.h"
#include "qdlgpathshaping.h"
#include "v_repStrings.h"

CQDlgPaths::CQDlgPaths(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgPaths)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
}

CQDlgPaths::~CQDlgPaths()
{
    delete ui;
}

void CQDlgPaths::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgPaths::refresh()
{
	inMainRefreshRoutine=true;
	bool sel=App::ct->objCont->isLastSelectionAPath();
	bool ssel=sel&&(App::ct->objCont->getPathNumberInSelection()>1);
	bool noSim=App::ct->simulation->isSimulationStopped();

	if (App::ct->objCont->getEditModeType()!=NO_EDIT_MODE)
	{
		sel=false;
		ssel=false;
	}
	CPath* path=NULL;
	CPathCont* pathCont=NULL;
	if (sel)
	{
		path=(CPath*)App::ct->objCont->getLastSelection();
		pathCont=path->pathContainer;
	}

	ui->qqEnableAll->setEnabled(App::ct->objCont->getEditModeType()==NO_EDIT_MODE);

	ui->qqShowOrientation->setEnabled(sel);
	ui->qqShowPathLine->setEnabled(sel);
	ui->qqShowPosition->setEnabled(sel);
	ui->qqAdjustColor->setEnabled(sel);
	ui->qqLineSize->setEnabled(sel);
	ui->qqControlPointSize->setEnabled(sel);
	ui->qqDistanceCombo->setEnabled(sel&&noSim);

	ui->qqExplicitHandling->setEnabled(sel);
	ui->qqInvertVelocity->setEnabled(sel&&noSim);
	ui->qqInfiniteAcceleration->setEnabled(sel&&noSim);
	ui->qqMaxAcceleration->setEnabled(sel&&noSim);
	ui->qqPosition->setEnabled(sel);
	ui->qqTargetVelocity->setEnabled(sel);
	ui->qqAdjustVelocityProfile->setEnabled(sel);
	ui->qqConversionCoeff->setEnabled(sel);
	ui->qqVirtualDistanceScaling->setEnabled(sel);
	ui->qqApplyMotionProperties->setEnabled(ssel&&noSim);

	ui->qqShowShapingDialog->setEnabled(sel);
	ui->qqShowShapingDialog->setChecked(CQDlgPathShaping::showWindow);

	ui->qqEnableAll->setChecked(App::ct->mainSettings->pathMotionHandlingEnabled);
	ui->qqShowOrientation->setChecked((pathCont!=NULL)&&((pathCont->getAttributes()&sim_pathproperty_show_orientation)!=0));
	ui->qqShowPathLine->setChecked((pathCont!=NULL)&&((pathCont->getAttributes()&sim_pathproperty_show_line)!=0));
	ui->qqShowPosition->setChecked((pathCont!=NULL)&&((pathCont->getAttributes()&sim_pathproperty_show_position)!=0));
	ui->qqDistanceCombo->clear();

	ui->qqExplicitHandling->setChecked(sel&&path->getExplicitHandling());
	ui->qqInvertVelocity->setChecked((pathCont!=NULL)&&((pathCont->getAttributes()&sim_pathproperty_invert_velocity)!=0));
	ui->qqInfiniteAcceleration->setChecked((pathCont!=NULL)&&((pathCont->getAttributes()&sim_pathproperty_infinite_acceleration)!=0));

	if (pathCont!=NULL)
	{
		ui->qqLineSize->setText(tt::FNb(0,pathCont->getLineSize(),false).c_str());
		ui->qqControlPointSize->setText(gv::getSizeStr(false,pathCont->getSquareSize(),0).c_str());

		ui->qqDistanceCombo->addItem(strTranslate(IDS_PATH_LENGTH_CALC_DL),QVariant(sim_distcalcmethod_dl));
		ui->qqDistanceCombo->addItem(strTranslate(IDS_PATH_LENGTH_CALC_DAC),QVariant(sim_distcalcmethod_dac));
		ui->qqDistanceCombo->addItem(strTranslate(IDS_PATH_LENGTH_CALC_MAX_DL_DAC),QVariant(sim_distcalcmethod_max_dl_dac));
		ui->qqDistanceCombo->addItem(strTranslate(IDS_PATH_LENGTH_CALC_DL_AND_DAC),QVariant(sim_distcalcmethod_dl_and_dac));
		ui->qqDistanceCombo->addItem(strTranslate(IDS_PATH_LENGTH_CALC_SQRT_DL2_AND_DAC2),QVariant(sim_distcalcmethod_sqrt_dl2_and_dac2));
		ui->qqDistanceCombo->addItem(strTranslate(IDS_PATH_LENGTH_CALC_DL_IF_NONZERO),QVariant(sim_distcalcmethod_dl_if_nonzero));
		ui->qqDistanceCombo->addItem(strTranslate(IDS_PATH_LENGTH_CALC_DAC_IF_NONZERO),QVariant(sim_distcalcmethod_dac_if_nonzero));
		for (int i=0;i<ui->qqDistanceCombo->count();i++)
		{
			if (ui->qqDistanceCombo->itemData(i).toInt()==pathCont->getPathLengthCalculationMethod())
			{
				ui->qqDistanceCombo->setCurrentIndex(i);
				break;
			}
		}

		ui->qqMaxAcceleration->setText(gv::getLinearAccelStr(false,pathCont->getMaxAcceleration()).c_str());
		ui->qqPosition->setText(gv::getSizeStr(true,float(pathCont->getPosition())).c_str());
		ui->qqTargetVelocity->setText(gv::getLinearVelocityStr(true,pathCont->getTargetNominalVelocity()).c_str());
		ui->qqConversionCoeff->setText(gv::getSizeOverAngleStr(false,pathCont->getAngleVarToDistanceCoeff()).c_str());
		ui->qqVirtualDistanceScaling->setText(gv::getSizeStr(false,pathCont->getOnSpotDistanceToDistanceCoeff()).c_str());
	}
	else
	{
		ui->qqLineSize->setText("");
		ui->qqControlPointSize->setText("");

		ui->qqMaxAcceleration->setText("");
		ui->qqPosition->setText("");
		ui->qqTargetVelocity->setText("");
		ui->qqConversionCoeff->setText("");
		ui->qqVirtualDistanceScaling->setText("");
	}

	inMainRefreshRoutine=false;
}

CPathCont* CQDlgPaths::getPathCont()
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

CPath* CQDlgPaths::getPath()
{
	CPath* path=App::ct->objCont->getPath(App::ct->objCont->getLastSelectionID());
	return(path);
}


void CQDlgPaths::on_qqEnableAll_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->mainSettings->pathMotionHandlingEnabled=!App::ct->mainSettings->pathMotionHandlingEnabled;
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgPaths::on_qqShowOrientation_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		pathCont->setAttributes(pathCont->getAttributes()^sim_pathproperty_show_orientation);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgPaths::on_qqShowPathLine_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		pathCont->setAttributes(pathCont->getAttributes()^sim_pathproperty_show_line);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgPaths::on_qqShowPosition_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		pathCont->setAttributes(pathCont->getAttributes()^sim_pathproperty_show_position);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgPaths::on_qqAdjustColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		int identification[4]={pathCont->getLifeID(),-1,-1,-1};
		CQDlgMaterial::display(identification,&pathCont->_lineColor,App::mainWindow,true,true,true,true,true,false,true,false);
	}
}

void CQDlgPaths::on_qqLineSize_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		bool ok;
		int newVal=ui->qqLineSize->text().toInt(&ok);
		if (ok)
		{
			pathCont->setLineSize(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgPaths::on_qqControlPointSize_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		bool ok;
		float newVal=ui->qqControlPointSize->text().toFloat(&ok);
		if (ok)
		{
			pathCont->setSquareSize(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgPaths::on_qqDistanceCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		if (!inMainRefreshRoutine)
		{
			pathCont->setPathLengthCalculationMethod(ui->qqDistanceCombo->itemData(ui->qqDistanceCombo->currentIndex()).toInt());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgPaths::on_qqExplicitHandling_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPath* path=getPath();
		if (path==NULL)
			return;
		path->setExplicitHandling(!path->getExplicitHandling());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgPaths::on_qqInvertVelocity_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		pathCont->setAttributes(pathCont->getAttributes()^sim_pathproperty_invert_velocity);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgPaths::on_qqInfiniteAcceleration_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		pathCont->setAttributes(pathCont->getAttributes()^sim_pathproperty_infinite_acceleration);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgPaths::on_qqMaxAcceleration_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		bool ok;
		float newVal=ui->qqMaxAcceleration->text().toFloat(&ok);
		if (ok)
		{
			pathCont->setMaxAcceleration(newVal*gv::userToLinearAccel);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgPaths::on_qqPosition_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		bool ok;
		float newVal=ui->qqPosition->text().toFloat(&ok);
		if (ok)
		{
			pathCont->setPosition(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgPaths::on_qqTargetVelocity_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		bool ok;
		float newVal=ui->qqTargetVelocity->text().toFloat(&ok);
		if (ok)
		{
			pathCont->setTargetNominalVelocity(newVal*gv::userToLinearVel);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgPaths::on_qqAdjustVelocityProfile_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		CDlgPathVelocityProfile theDialog(this);
		theDialog.refresh();
		theDialog.makeDialogModal(); // modifications made directly!
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgPaths::on_qqConversionCoeff_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		bool ok;
		float newVal=ui->qqConversionCoeff->text().toFloat(&ok);
		if (ok)
		{
			pathCont->setAngleVarToDistanceCoeff(newVal*gv::userToMeterOverRad);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgPaths::on_qqVirtualDistanceScaling_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CPathCont* pathCont=getPathCont();
		if (pathCont==NULL)
			return;
		bool ok;
		float newVal=ui->qqVirtualDistanceScaling->text().toFloat(&ok);
		if (ok)
		{
			pathCont->setOnSpotDistanceToDistanceCoeff(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgPaths::on_qqApplyMotionProperties_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CPathCont* pathCont=NULL;
		if (App::ct->objCont->getEditModeType()!=NO_EDIT_MODE)
			return;
		CPath* path=App::ct->objCont->getPath(App::ct->objCont->getLastSelectionID());
		if (path!=NULL)
			pathCont=path->pathContainer;
		if (pathCont==NULL)
			return;
		for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
		{
			CPath* it=App::ct->objCont->getPath(App::ct->objCont->getSelID(i));
			if (it!=NULL)
			{
				int atrToCopy=sim_pathproperty_invert_velocity|sim_pathproperty_infinite_acceleration|
								sim_pathproperty_auto_velocity_profile_translation|
								sim_pathproperty_auto_velocity_profile_rotation|sim_pathproperty_endpoints_at_zero;
				int oldAtr=it->pathContainer->getAttributes()|atrToCopy;
				oldAtr-=atrToCopy;
				oldAtr|=pathCont->getAttributes()&atrToCopy;
				it->pathContainer->setAttributes(oldAtr);
				it->setExplicitHandling(path->getExplicitHandling());
				it->pathContainer->setMaxAcceleration(pathCont->getMaxAcceleration());
				it->pathContainer->setPosition(pathCont->getPosition());
				it->pathContainer->setNominalVelocity(pathCont->getNominalVelocity());
				it->pathContainer->setTargetNominalVelocity(pathCont->getTargetNominalVelocity());
				it->pathContainer->setAngleVarToDistanceCoeff(pathCont->getAngleVarToDistanceCoeff());
				it->pathContainer->setOnSpotDistanceToDistanceCoeff(pathCont->getOnSpotDistanceToDistanceCoeff());
				it->pathContainer->setAvpRelativeVelocityAtRotationAxisChange(pathCont->getAvpRelativeVelocityAtRotationAxisChange());
				it->pathContainer->setAvpTurningCircleRadiusForHalfVelocity(pathCont->getAvpTurningCircleRadiusForHalfVelocity());
				it->pathContainer->setAvpRelativeAcceleration(pathCont->getAvpRelativeAcceleration());
			}
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgPaths::on_qqShowShapingDialog_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CQDlgPathShaping::showWindow=!CQDlgPathShaping::showWindow;
		if (App::mainWindow->dlgCont->isVisible(PATH_SHAPING_DLG)!=CQDlgPathShaping::showWindow)
			App::mainWindow->dlgCont->toggle(PATH_SHAPING_DLG);
	}
}
