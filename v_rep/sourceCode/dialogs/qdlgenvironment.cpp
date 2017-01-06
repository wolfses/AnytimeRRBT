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
#include "qdlgenvironment.h"
#include "ui_qdlgenvironment.h"
#include "GV.h"
#include "IloIlo.h"
#include "qdlgcolor.h"
#include "qdlgmaterial.h"
#include "Tt.h"
#include "App.h"
#include "v_repStrings.h"
#include "ScintillaModalDlg.h"

CQDlgEnvironment::CQDlgEnvironment(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgEnvironment)
{
    ui->setupUi(this);
}

CQDlgEnvironment::~CQDlgEnvironment()
{
    delete ui;
}

void CQDlgEnvironment::refresh()
{
	bool simStopped=App::ct->simulation->isSimulationStopped();
	ui->qqShowWirelessEmissions->setChecked(App::ct->environment->getVisualizeWirelessEmitters());
	ui->qqShowWirelessReceptions->setChecked(App::ct->environment->getVisualizeWirelessReceivers());
	ui->qqWirelessEmissionColor->setEnabled(App::ct->environment->getVisualizeWirelessEmitters());
	ui->qqWirelessReceptionColor->setEnabled(App::ct->environment->getVisualizeWirelessReceivers());

	ui->qqMaxTriangleSize->setText(gv::getSizeStr(false,App::ct->environment->getCalculationMaxTriangleSize(),0).c_str());
	ui->qqMinRelTriangleSize->setText(tt::FNb(0,App::ct->environment->getCalculationMinRelTriangleSize(),3,false).c_str());
	ui->qqSaveCalcStruct->setChecked(App::ct->environment->getSaveExistingCalculationStructures());
	ui->qqShapeTexturesDisabled->setChecked(!App::ct->environment->getShapeTexturesEnabled());
	ui->qqUserInterfaceTexturesDisabled->setChecked(!App::ct->environment->get2DElementTexturesEnabled());

	ui->qqNextSaveIsDefinitive->setEnabled(!App::ct->environment->getSceneLocked());
	ui->qqNextSaveIsDefinitive->setChecked(App::ct->environment->getRequestFinalSave());

	ui->qqCustomContactHandling->setEnabled(simStopped);
	ui->qqCustomContactHandling->setChecked(App::ct->environment->getEnableCustomContactHandlingViaScript());
	ui->qqEditCustomContact->setEnabled(simStopped&&App::ct->environment->getEnableCustomContactHandlingViaScript());

	ui->qqCleanUpHashNames->setEnabled(simStopped);


	ui->qqAcknowledgments->setPlainText(App::ct->environment->getAcknowledgement().c_str());
}

void CQDlgEnvironment::on_qqBackgroundColorUp_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		int identification[4]={App::ct->environment->getLifeID(),-1,-1,-1};
		CQDlgColor::display(identification,App::ct->environment->backGroundColor,App::mainWindow);
	}
}

void CQDlgEnvironment::on_qqBackgroundColorDown_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		int identification[4]={App::ct->environment->getLifeID(),-1,-1,-1};
		CQDlgColor::display(identification,App::ct->environment->backGroundColorDown,App::mainWindow);
	}
}

void CQDlgEnvironment::on_qqAmbientLightColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		int identification[4]={App::ct->environment->getLifeID(),-1,-1,-1};
		CQDlgColor::display(identification,App::ct->environment->ambientLightColor,App::mainWindow);
	}
}

void CQDlgEnvironment::on_qqFogAdjust_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::mainWindow->dlgCont->evaluateMenuSelection(OPEN_FOG_DLG_CMD);
	}
}

void CQDlgEnvironment::on_qqShowWirelessEmissions_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->environment->setVisualizeWirelessEmitters(!App::ct->environment->getVisualizeWirelessEmitters());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgEnvironment::on_qqShowWirelessReceptions_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->environment->setVisualizeWirelessReceivers(!App::ct->environment->getVisualizeWirelessReceivers());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgEnvironment::on_qqWirelessEmissionColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		int identification[4]={App::ct->environment->getLifeID(),-1,-1,-1};
		CQDlgMaterial::display(identification,&App::ct->environment->wirelessEmissionVolumeColor,App::mainWindow,true,true,true,true,true,true,true,false);
	}
}

void CQDlgEnvironment::on_qqWirelessReceptionColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		int identification[4]={App::ct->environment->getLifeID(),-1,-1,-1};
		CQDlgMaterial::display(identification,&App::ct->environment->wirelessTransmissionColor,App::mainWindow,true,true,true,true,true,true,true,false);
	}
}

void CQDlgEnvironment::on_qqMaxTriangleSize_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqMaxTriangleSize->text().toFloat(&ok);
		if (ok)
		{
			newVal*=gv::userToMeter;
			App::ct->environment->setCalculationMaxTriangleSize(newVal);
			for (int i=0;i<int(App::ct->objCont->shapeList.size());i++)
			{
				CShape* sh=App::ct->objCont->getShape(App::ct->objCont->shapeList[i]);
				sh->removeCollisionInformation();
			}
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgEnvironment::on_qqSaveCalcStruct_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		App::ct->environment->setSaveExistingCalculationStructures(!App::ct->environment->getSaveExistingCalculationStructures());
		if (App::ct->environment->getSaveExistingCalculationStructures())
			App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_CALCULATION_STRUCTURE),strTranslate(IDS_SAVING_CALCULATION_STRUCTURE),VMessageBox::OKELI);
		refresh();
	}
}

void CQDlgEnvironment::on_qqShapeTexturesDisabled_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->environment->setShapeTexturesEnabled(!App::ct->environment->getShapeTexturesEnabled());
		refresh();
	}
}

void CQDlgEnvironment::on_qqUserInterfaceTexturesDisabled_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->environment->set2DElementTexturesEnabled(!App::ct->environment->get2DElementTexturesEnabled());
		refresh();
	}
}

void CQDlgEnvironment::on_qqNextSaveIsDefinitive_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->environment->setRequestFinalSave(!App::ct->environment->getRequestFinalSave());
		if (App::ct->environment->getRequestFinalSave())
			App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_SCENE_LOCKING),strTranslate(IDS_SCENE_LOCKING_INFO),VMessageBox::OKELI);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgEnvironment::on_qqAcknowledgments_textChanged()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		std::string txt=ui->qqAcknowledgments->toPlainText().toStdString();
		tt::removeSpacesAndEmptyLinesAtBeginningAndEnd(txt);
		App::ct->environment->setAcknowledgement(txt);
		App::ct->undoBufferContainer->announceChangeGradual(); // **************** UNDO THINGY ****************
		// No refresh here!! (otherwise we can't edit the item properly)
	}
}

void CQDlgEnvironment::on_qqCustomContactHandling_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (App::ct->environment->getEnableCustomContactHandlingViaScript())
		{ // disable it
			if (VMessageBox::REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Custom collision/contact response"),strTranslate(IDSN_SURE_TO_REMOVE_CUSTOM_CONTACT_WARNING),VMessageBox::YES_NO))
			{
				App::ct->environment->setEnableCustomContactHandlingViaScript(false,NULL);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			}
		}
		else
		{ // enable it
			App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Custom collision/contact response"),strTranslate(IDSN_CUSTOM_CONTACT_IS_SLOW_AND_NOT_RECOMENDED_WARNING),VMessageBox::OKELI);
			std::string filenameAndPath(App::directories->systemDirectory+VREP_SLASH);
			filenameAndPath+=DEFAULT_CONTACTCALLBACKSCRIPT_NAME;
			if (VFile::doesFileExist(filenameAndPath))
			{
				try
				{
					VFile file(filenameAndPath,VFile::READ|VFile::SHARE_DENY_NONE);
					VArchive archive(&file,VArchive::LOAD);
					DWORD archiveLength=DWORD(file.getLength());
					char* defaultScript=new char[archiveLength+1];
					for (int i=0;i<int(archiveLength);i++)
						archive >> defaultScript[i];
					defaultScript[archiveLength]=0;
					App::ct->environment->setEnableCustomContactHandlingViaScript(true,defaultScript);
					delete[] defaultScript;
					archive.close();
					file.close();
				}
				catch(VFILE_EXCEPTION_TYPE e)
				{
					VFile::reportAndHandleFileExceptionError(e);
					char defaultMessage[]="Default script file could not be found!"; // do not use comments ("--"), we want to cause an execution error!
					App::ct->environment->setEnableCustomContactHandlingViaScript(true,defaultMessage);
				}
			}
			else
			{
				char defaultMessage[]="Default collision/contact response script file could not be found!"; // do not use comments ("--"), we want to cause an execution error!
				App::ct->environment->setEnableCustomContactHandlingViaScript(true,defaultMessage);
			}

			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
		App::ct->objCont->setFullDialogRefreshFlag(); // to reflect changes in the script dlg too!
	}
}

void CQDlgEnvironment::on_qqEditCustomContact_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		if (App::ct->environment->getEnableCustomContactHandlingViaScript())
		{
			CLuaScriptObject* script=App::ct->luaScriptContainer->getCustomContactHandlingScript_callback();
			if (script)
			{
				CScintillaModalDlg dlg(App::mainWindow);
				dlg.initialize(script->getScriptID(),"Custom collision/contact response",false);
				dlg.makeDialogModal();
			}
		}
	}
}

void CQDlgEnvironment::on_qqCleanUpHashNames_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (App::ct->simulation->isSimulationStopped())
		{
			App::ct->objCont->cleanupDashNames(-1);
			refresh();
			App::ct->objCont->setFullDialogRefreshFlag(); // to reflect changes everywhere!
		}
	}
}

void CQDlgEnvironment::on_qqCleanUpGhosts_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		App::ct->ghostObjectCont->removeGhost(-1,-1);
	}
}

void CQDlgEnvironment::on_qqMinRelTriangleSize_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		bool ok;
		float newVal=ui->qqMinRelTriangleSize->text().toFloat(&ok);
		if (ok)
		{
			App::ct->environment->setCalculationMinRelTriangleSize(newVal);
			for (int i=0;i<int(App::ct->objCont->shapeList.size());i++)
			{
				CShape* sh=App::ct->objCont->getShape(App::ct->objCont->shapeList[i]);
				sh->removeCollisionInformation();
			}
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}
