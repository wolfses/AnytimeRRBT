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
#include "qdlgsettings.h"
#include "ui_qdlgsettings.h"
#include "IloIlo.h"
#include "Tt.h"
#include "GV.h"
#include "v_repStrings.h"
#include "VFileDialog.h"
#include "qdlgopenglsettings.h"
#include "App.h"


CQDlgSettings::CQDlgSettings(QWidget *parent) :
    CDlgEx(parent),
    ui(new Ui::CQDlgSettings)
{
    ui->setupUi(this);
}

CQDlgSettings::~CQDlgSettings()
{
    delete ui;
}

void CQDlgSettings::refresh()
{
#ifndef WIN_VREP
	ui->hideConsole->setVisible(false);
#endif

	ui->userUnitRadians->setChecked(!gv::degrees);
	ui->userUnitDegrees->setChecked(gv::degrees);
	ui->userUnitMeters->setChecked(gv::meters);
	ui->userUnitMillimeters->setChecked(!gv::meters);
	ui->userUnitSeconds->setChecked(gv::seconds);
	ui->userUnitMilliseconds->setChecked(!gv::seconds);

	ui->translationStepSize->clear();
	ui->translationStepSize->addItem(gv::getSizeStr(false,0.001f,0).c_str(),QVariant(1));
	ui->translationStepSize->addItem(gv::getSizeStr(false,0.002f,0).c_str(),QVariant(2));
	ui->translationStepSize->addItem(gv::getSizeStr(false,0.005f,0).c_str(),QVariant(5));
	ui->translationStepSize->addItem(gv::getSizeStr(false,0.01f,0).c_str(),QVariant(10));
	ui->translationStepSize->addItem(gv::getSizeStr(false,0.025f,0).c_str(),QVariant(25));
	ui->translationStepSize->addItem(gv::getSizeStr(false,0.05f,0).c_str(),QVariant(50));
	ui->translationStepSize->addItem(gv::getSizeStr(false,0.1f,0).c_str(),QVariant(100));
	ui->translationStepSize->addItem(gv::getSizeStr(false,0.25f,0).c_str(),QVariant(250));
	ui->translationStepSize->addItem(gv::getSizeStr(false,0.5f,0).c_str(),QVariant(500));
	CIloIlo::selectItemOfCombobox(ui->translationStepSize,int((App::userSettings->getTranslationStepSize()+0.0005f)*1000.0f));

	ui->rotationStepSize->clear();
	ui->rotationStepSize->addItem(gv::getAngleStr(false,1.0f*degToRad,0).c_str(),QVariant(1));
	ui->rotationStepSize->addItem(gv::getAngleStr(false,2.0f*degToRad,0).c_str(),QVariant(2));
	ui->rotationStepSize->addItem(gv::getAngleStr(false,5.0f*degToRad,0).c_str(),QVariant(5));
	ui->rotationStepSize->addItem(gv::getAngleStr(false,10.0f*degToRad,0).c_str(),QVariant(10));
	ui->rotationStepSize->addItem(gv::getAngleStr(false,15.0f*degToRad,0).c_str(),QVariant(15));
	ui->rotationStepSize->addItem(gv::getAngleStr(false,30.0f*degToRad,0).c_str(),QVariant(30));
	ui->rotationStepSize->addItem(gv::getAngleStr(false,45.0f*degToRad,0).c_str(),QVariant(45));
	CIloIlo::selectItemOfCombobox(ui->rotationStepSize,int(App::userSettings->getRotationStepSize()*radToDeg+0.5f));

	ui->removeIdenticalVerticesCheckbox->setChecked(App::userSettings->identicalVerticesCheck);
	if (gv::meters)
		ui->removeIdenticalVerticesTolerance->setText(tt::FNb(0,App::userSettings->identicalVerticesTolerance,5,false).c_str());
	else
		ui->removeIdenticalVerticesTolerance->setText(tt::FNb(0,App::userSettings->identicalVerticesTolerance*gv::meterToUser,2,false).c_str());

	ui->removeIdenticalTriangles->setChecked(App::userSettings->identicalTrianglesCheck);
	ui->ignoreTriangleWinding->setEnabled(App::userSettings->identicalTrianglesCheck);
	ui->ignoreTriangleWinding->setChecked(App::userSettings->identicalTrianglesWindingCheck);

	ui->antialiasing->setChecked(App::userSettings->antiAliasing);
	ui->worldReference->setChecked(App::userSettings->displayWorldReference);
	ui->boundingBoxDisplay->setChecked(App::userSettings->displayBoundingBoxeWhenObjectSelected);
	ui->undoRedo->setChecked(App::userSettings->getUndoRedoEnabled());
	ui->hideConsole->setChecked(!App::userSettings->alwaysShowConsole);
	ui->qqTransparentDlgs->setChecked(App::userSettings->allowTransparentDialogs);
	ui->qqAutoSave->setChecked(App::userSettings->autoSaveDelay!=0);

	ui->qqWorkThreads->setChecked(App::userSettings->workThreadCount!=0);

	ui->qqThreadedRendering->setChecked(App::userSettings->threadedRenderingDuringSimulation==1);
	ui->qqThreadedRendering->setEnabled(App::ct->simulation->isSimulationStopped()&&(App::userSettings->threadedRenderingDuringSimulation>=0));

	ui->qqExternalScriptEditor->setChecked(!App::userSettings->getUseBuiltInScriptEditorToSave());
	ui->qqScriptEditor->setText(App::userSettings->externalScriptEditorToSave.c_str());
	ui->qqSelectScriptEditor->setEnabled(!App::userSettings->getUseBuiltInScriptEditorToSave());

	ui->qqAdjustOpenGl->setEnabled(App::ct->simulation->isSimulationStopped());
}

void CQDlgSettings::on_userUnitDegrees_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		gv::setToDegrees();
		App::ct->objCont->setLightDialogRefreshFlag();
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_userUnitRadians_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		gv::setToRadians();
		App::ct->objCont->setLightDialogRefreshFlag();
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_userUnitMillimeters_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		gv::setToMilimeters();
		App::ct->objCont->setLightDialogRefreshFlag();
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_userUnitMeters_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		gv::setToMeters();
		App::ct->objCont->setLightDialogRefreshFlag();
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_userUnitMilliseconds_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		gv::setToMiliseconds();
		App::ct->objCont->setLightDialogRefreshFlag();
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_userUnitSeconds_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		gv::setToSeconds();
		App::ct->objCont->setLightDialogRefreshFlag();
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_translationStepSize_activated(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		int data=ui->translationStepSize->itemData(index).toInt();
		App::userSettings->setTranslationStepSize(float(data)/1000.0f);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_rotationStepSize_activated(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		int data=ui->translationStepSize->itemData(index).toInt();
		App::userSettings->setRotationStepSize(float(data)*degToRad);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_removeIdenticalVerticesCheckbox_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::userSettings->identicalVerticesCheck=!App::userSettings->identicalVerticesCheck;
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_removeIdenticalVerticesTolerance_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		float newVal;
		bool ok;
		newVal=ui->removeIdenticalVerticesTolerance->text().toFloat(&ok);
		if (ok)
		{
			if (newVal>=0.0f)
			{
				if (newVal>0.5f)
					newVal=0.5f;
				App::userSettings->identicalVerticesTolerance=newVal;
			}
		}
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_removeIdenticalTriangles_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::userSettings->identicalTrianglesCheck=!App::userSettings->identicalTrianglesCheck;
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_ignoreTriangleWinding_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::userSettings->identicalTrianglesWindingCheck=!App::userSettings->identicalTrianglesWindingCheck;
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_antialiasing_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::userSettings->antiAliasing=!App::userSettings->antiAliasing;
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_worldReference_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::userSettings->displayWorldReference=!App::userSettings->displayWorldReference;
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_boundingBoxDisplay_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::userSettings->displayBoundingBoxeWhenObjectSelected=!App::userSettings->displayBoundingBoxeWhenObjectSelected;
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_undoRedo_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::userSettings->setUndoRedoEnabled(!App::userSettings->getUndoRedoEnabled());
		App::ct->objCont->toolbarRefreshFlag=true;
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_hideConsole_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::userSettings->alwaysShowConsole=!App::userSettings->alwaysShowConsole;
		App::setShowConsole(App::userSettings->alwaysShowConsole);
		App::ct->objCont->toolbarRefreshFlag=true;
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_qqTransparentDlgs_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::userSettings->allowTransparentDialogs=!App::userSettings->allowTransparentDialogs;
		App::ct->objCont->setLightDialogRefreshFlag();
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_qqAutoSave_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (App::userSettings->autoSaveDelay!=0)
			App::userSettings->autoSaveDelay=0;
		else
			App::userSettings->autoSaveDelay=2; // 2 minutes
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_qqExternalScriptEditor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::userSettings->setUseBuiltInScriptEditorToSave(!App::userSettings->getUseBuiltInScriptEditorToSave());
		App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_EXTERNAL_SCRIPT_EDITOR),strTranslate(IDSN_EXTERNAL_EDITOR_INFO),VMessageBox::OKELI);
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_qqSelectScriptEditor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		std::string initPath=App::directories->getPathFromFull(App::userSettings->externalScriptEditorToSave);
		std::string filenameAndPath=App::uiThread->getOpenFileName(App::mainWindow,0,strTranslate(IDSN_EXTERNAL_SCRIPT_EDITOR),initPath,App::userSettings->externalScriptEditorToSave,false,"all","*");
		if (filenameAndPath.length()!=0)
			App::userSettings->externalScriptEditorToSave=filenameAndPath.c_str();
		App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_EXTERNAL_SCRIPT_EDITOR),strTranslate(IDSN_EXTERNAL_EDITOR_INFO),VMessageBox::OKELI);
		refresh();
		App::userSettings->saveUserSettings();
	}
}


void CQDlgSettings::on_qqWorkThreads_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (App::userSettings->workThreadCount==0)
			App::userSettings->workThreadCount=-1; // enable with automatic settings
		else
			App::userSettings->workThreadCount=0; // disable
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_qqThreadedRendering_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (App::userSettings->threadedRenderingDuringSimulation==0)
			App::userSettings->threadedRenderingDuringSimulation=1;
		else if (App::userSettings->threadedRenderingDuringSimulation==1)
			App::userSettings->threadedRenderingDuringSimulation=0;
		if (App::userSettings->threadedRenderingDuringSimulation==1)
			App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_THREADED_RENDERING),strTranslate(IDSN_THREADED_RENDERING_WARNING),VMessageBox::OKELI);
		App::ct->objCont->setLightDialogRefreshFlag();
		refresh();
		App::userSettings->saveUserSettings();
	}
}

void CQDlgSettings::on_qqAdjustOpenGl_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG // required with modal dialogs
	{
		CQDlgOpenGlSettings theDialog(this);

		theDialog.offscreenContextType=App::userSettings->offscreenContextType;
		theDialog.fboType=App::userSettings->fboType;
		theDialog.exclusiveGuiRendering=(App::userSettings->visionSensorsUseGuiThread_windowed!=0);
		theDialog.compatibilityTweak1=App::userSettings->oglCompatibilityTweak1;
		theDialog.glFinish_normal=App::userSettings->useGlFinish;
		theDialog.glFinish_visionSensors=App::userSettings->useGlFinish_visionSensors;
		theDialog.idleFps=App::userSettings->getIdleFps();
		theDialog.forceExt=App::userSettings->forceFboViaExt;
		theDialog.glVersionMajor=App::userSettings->desiredOpenGlMajor;
		theDialog.glVersionMinor=App::userSettings->desiredOpenGlMinor;

		theDialog.refresh();
		if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
		{
			App::userSettings->offscreenContextType=theDialog.offscreenContextType;
			App::userSettings->fboType=theDialog.fboType;
			App::userSettings->visionSensorsUseGuiThread_windowed=-1;
			if (!theDialog.exclusiveGuiRendering)
				App::userSettings->visionSensorsUseGuiThread_windowed=0;
			App::userSettings->oglCompatibilityTweak1=theDialog.compatibilityTweak1;
			App::userSettings->useGlFinish=theDialog.glFinish_normal;
			App::userSettings->useGlFinish_visionSensors=theDialog.glFinish_visionSensors;
			App::userSettings->setIdleFps(theDialog.idleFps);
			App::userSettings->forceFboViaExt=theDialog.forceExt;
			App::userSettings->desiredOpenGlMajor=theDialog.glVersionMajor;
			App::userSettings->desiredOpenGlMinor=theDialog.glVersionMinor;
			App::userSettings->saveUserSettings();
		}
	}
}
