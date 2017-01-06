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
#include "qdlgvisionsensors.h"
#include "ui_qdlgvisionsensors.h"
#include "Tt.h"
#include "GV.h"
#include "IloIlo.h"
#include "qdlgmaterial.h"
#include "qdlgimagecolor.h"
#include "v_repStrings.h"
#include "App.h"

bool CQDlgVisionSensors::showFilterWindow=false;

CQDlgVisionSensors::CQDlgVisionSensors(QWidget *parent) :
	CDlgEx(parent),
	ui(new Ui::CQDlgVisionSensors)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
}

CQDlgVisionSensors::~CQDlgVisionSensors()
{
    delete ui;
}

void CQDlgVisionSensors::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgVisionSensors::refresh()
{
	inMainRefreshRoutine=true;
	CVisionSensor* it=NULL;
	if (App::ct->objCont->isLastSelectionAVisionSensor())
		it=(CVisionSensor*)App::ct->objCont->getLastSelection();


	bool isSensor=App::ct->objCont->isLastSelectionAVisionSensor();
	bool manySensors=App::ct->objCont->getVisionSensorNumberInSelection()>1;
	bool noEditMode=App::ct->objCont->getEditModeType()==NO_EDIT_MODE;
	bool noSim=App::ct->simulation->isSimulationStopped();

	ui->qqNearPlane->setEnabled(isSensor&&noEditMode);
	ui->qqFarPlane->setEnabled(isSensor&&noEditMode);

	ui->qqPerspectiveAngleOrOrthographicSize->setEnabled(isSensor&&noEditMode);

	ui->qqResX->setEnabled(isSensor&&noEditMode&&noSim);
	ui->qqResY->setEnabled(isSensor&&noEditMode&&noSim);

	ui->qqSizeX->setEnabled(isSensor&&noEditMode);
	ui->qqSizeY->setEnabled(isSensor&&noEditMode);
	ui->qqSizeZ->setEnabled(isSensor&&noEditMode);

	ui->qqPerspective->setEnabled(isSensor&&noEditMode);
	ui->qqExplicitHandling->setEnabled(isSensor&&noEditMode);
	ui->qqExternalInput->setEnabled(isSensor&&noEditMode);
	ui->qqLocalLights->setEnabled(isSensor&&noEditMode);

	ui->qqShowFog->setEnabled(isSensor&&noEditMode);

	ui->qqShowNotDetecting->setEnabled(isSensor&&noEditMode);
	ui->qqShowDetecting->setEnabled(isSensor&&noEditMode);

	ui->qqIgnoreRGB->setEnabled(isSensor&&noEditMode);
	ui->qqIgnoreDepth->setEnabled(isSensor&&noEditMode);

	ui->qqCasingColorPassive->setEnabled(isSensor&&noEditMode);
	ui->qqCasingColorActive->setEnabled(isSensor&&noEditMode);
	ui->qqVolumeColorPassive->setEnabled(isSensor&&noEditMode);
	ui->qqVolumeColorActive->setEnabled(isSensor&&noEditMode);

	ui->qqEnabled->setChecked(App::ct->mainSettings->visionSensorsEnabled);

	ui->qqApplyMainProperties->setEnabled(isSensor&&manySensors&&noEditMode&&noSim);
	ui->qqApplyColors->setEnabled(isSensor&&manySensors&&noEditMode);

	ui->qqAdjustImageColor->setEnabled(isSensor&&noEditMode);
	ui->qqAdjustFilters->setEnabled(true);
	ui->qqAdjustFilters->setChecked(showFilterWindow);

	ui->qqEntityCombo->setEnabled(isSensor&&noEditMode);
	ui->qqEntityCombo->clear();

	if (isSensor)
	{
		CVisionSensor* s=(CVisionSensor*)App::ct->objCont->getLastSelection();
//		CComposedFilter* filters=s->getComposedFilter();

		ui->qqShowFog->setChecked(s->getShowFogIfAvailable());

		ui->qqNearPlane->setText(gv::getSizeStr(false,s->getNearClippingPlane()).c_str());
		ui->qqFarPlane->setText(gv::getSizeStr(false,s->getFarClippingPlane()).c_str());

		ui->qqPerspective->setChecked(s->getPerspectiveOperation());
		if (s->getPerspectiveOperation())
			ui->qqPerspectiveAngleOrOrthographicSize->setText(gv::getAngleStr(false,s->getViewAngle(),0).c_str());
		else
			ui->qqPerspectiveAngleOrOrthographicSize->setText(gv::getSizeStr(false,s->getOrthoViewSize()).c_str());

		int r[2];
		s->getDesiredResolution(r);
		ui->qqResX->setText(tt::FNb(0,r[0],false).c_str());
		ui->qqResY->setText(tt::FNb(0,r[1],false).c_str());
		C3Vector size(s->getSize());
		ui->qqSizeX->setText(gv::getSizeStr(false,size(0)).c_str());
		ui->qqSizeY->setText(gv::getSizeStr(false,size(1)).c_str());
		ui->qqSizeZ->setText(gv::getSizeStr(false,size(2)).c_str());

		ui->qqExplicitHandling->setChecked(s->getExplicitHandling());
		ui->qqExternalInput->setChecked(s->getUseExternalImage());
		ui->qqLocalLights->setChecked(s->getuseLocalLights());


		ui->qqShowNotDetecting->setChecked(s->getShowVolumeWhenNotDetecting());
		ui->qqShowDetecting->setChecked(s->getShowVolumeWhenDetecting());

		ui->qqIgnoreRGB->setChecked(s->getIgnoreRGBInfo());
		ui->qqIgnoreDepth->setChecked(s->getIgnoreDepthInfo());


		ui->qqEntityCombo->addItem(strTranslate(IDS_ALL_RENDERABLE_OBJECTS_IN_SCENE),QVariant(-1));

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
			ui->qqEntityCombo->addItem(names[i].c_str(),QVariant(ids[i]));

		names.clear();
		ids.clear();

		// Now paths:
		for (int i=0;i<int(App::ct->objCont->pathList.size());i++)
		{
			CPath* it=App::ct->objCont->getPath(App::ct->objCont->pathList[i]);
			std::string name(tt::decorateString("[",strTranslate(IDSN_PATH),"] "));
			name+=it->getName();
			names.push_back(name);
			ids.push_back(it->getID());
		}
		tt::orderStrings(names,ids);
		for (int i=0;i<int(names.size());i++)
			ui->qqEntityCombo->addItem(names[i].c_str(),QVariant(ids[i]));

		names.clear();
		ids.clear();

		// Now graphs:
		for (int i=0;i<int(App::ct->objCont->graphList.size());i++)
		{
			CGraph* it=App::ct->objCont->getGraph(App::ct->objCont->graphList[i]);
			std::string name(tt::decorateString("[",strTranslate(IDSN_GRAPH),"] "));
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
			if (ui->qqEntityCombo->itemData(i).toInt()==it->getDetectableEntityID())
			{
				ui->qqEntityCombo->setCurrentIndex(i);
				break;
			}
		}
	}
	else
	{
		ui->qqNearPlane->setText("");
		ui->qqFarPlane->setText("");

		ui->qqPerspectiveAngleOrOrthographicSize->setText("");

		ui->qqResX->setText("");
		ui->qqResY->setText("");

		ui->qqSizeX->setText("");
		ui->qqSizeY->setText("");
		ui->qqSizeZ->setText("");

		ui->qqPerspective->setChecked(false);
		ui->qqExplicitHandling->setChecked(false);
		ui->qqExternalInput->setChecked(false);
		ui->qqLocalLights->setChecked(false);

		ui->qqShowFog->setChecked(false);

		ui->qqShowNotDetecting->setChecked(false);
		ui->qqShowDetecting->setChecked(false);
		ui->qqIgnoreRGB->setChecked(false);
		ui->qqIgnoreDepth->setChecked(false);
	}


	inMainRefreshRoutine=false;
}

void CQDlgVisionSensors::on_qqEnabled_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->mainSettings->visionSensorsEnabled=!App::ct->mainSettings->visionSensorsEnabled;
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgVisionSensors::on_qqExplicitHandling_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
		it->setExplicitHandling(!it->getExplicitHandling());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgVisionSensors::on_qqExternalInput_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
		it->setUseExternalImage(!it->getUseExternalImage());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgVisionSensors::on_qqPerspective_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
		it->setPerspectiveOperation(!it->getPerspectiveOperation());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgVisionSensors::on_qqLocalLights_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
		it->setUseLocalLights(!it->getuseLocalLights());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgVisionSensors::on_qqShowNotDetecting_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
		it->setShowVolumeWhenNotDetecting(!it->getShowVolumeWhenNotDetecting());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgVisionSensors::on_qqShowFog_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
		it->setShowFogIfAvailable(!it->getShowFogIfAvailable());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgVisionSensors::on_qqShowDetecting_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
		it->setShowVolumeWhenDetecting(!it->getShowVolumeWhenDetecting());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgVisionSensors::on_qqNearPlane_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		bool ok;
		float newVal=ui->qqNearPlane->text().toFloat(&ok);
		if (ok)
		{
			CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();

			float ncp=newVal*gv::userToMeter;
			tt::limitValue(0.0001f,it->getFarClippingPlane(),ncp);
			it->setNearClippingPlane(ncp);

			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgVisionSensors::on_qqFarPlane_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		bool ok;
		float newVal=ui->qqFarPlane->text().toFloat(&ok);
		if (ok)
		{
			CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
			float ncp=newVal*gv::userToMeter;
			tt::limitValue(it->getNearClippingPlane(),100000.0f,ncp);
			it->setFarClippingPlane(ncp);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgVisionSensors::on_qqPerspectiveAngleOrOrthographicSize_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		bool ok;
		float newVal=ui->qqPerspectiveAngleOrOrthographicSize->text().toFloat(&ok);
		if (ok)
		{
			CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
			if (it->getPerspectiveOperation())
			{
				float v=newVal*gv::userToRad;
				tt::limitValue(1.0f*degToRad,135.0f*degToRad,v); // with 90 degrees, objects disappear!! Really??? Changed to 135 on 2010/11/12
				it->setViewAngle(v);
			}
			else
			{
				float v=newVal*gv::userToMeter;
				tt::limitValue(0.001f,200000.0f,v);
				it->setOrthoViewSize(v);
			}
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgVisionSensors::on_qqResX_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		bool ok;
		int newVal=ui->qqResX->text().toInt(&ok);
		if (ok)
		{
			CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
			int r[2];
			it->getDesiredResolution(r);
			if (newVal!=r[0])
			{
				r[0]=newVal;
				// Check if the resolution is a power of 2:
				newVal&=(32768-1);
				WORD tmp=32768;
				while (tmp!=1)
				{
					if (newVal&tmp)
						newVal&=tmp;
					tmp/=2;
				}
				if (newVal!=r[0])
					App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Vision sensor"),strTranslate(IDS_VISION_SENSOR_RESOLUTION_NOT_POWER_OF_TWO_WARNING),VMessageBox::OKELI);
				it->setDesiredResolution(r);
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			}
		}
		refresh();
	}
}

void CQDlgVisionSensors::on_qqResY_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		bool ok;
		int newVal=ui->qqResY->text().toInt(&ok);
		if (ok)
		{
			CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
			int r[2];
			it->getDesiredResolution(r);
			if (newVal!=r[1])
			{
				r[1]=newVal;
				// Check if the resolution is a power of 2:
				newVal&=(32768-1);
				WORD tmp=32768;
				while (tmp!=1)
				{
					if (newVal&tmp)
						newVal&=tmp;
					tmp/=2;
				}
				if (newVal!=r[1])
					App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Vision sensor"),strTranslate(IDS_VISION_SENSOR_RESOLUTION_NOT_POWER_OF_TWO_WARNING),VMessageBox::OKELI);
				it->setDesiredResolution(r);
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			}
		}
		refresh();
	}
}

void CQDlgVisionSensors::on_qqSizeX_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		bool ok;
		float newVal=ui->qqSizeX->text().toFloat(&ok);
		if (ok)
		{
			CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
			C3Vector s(it->getSize());
			s(0)=newVal*gv::userToMeter;
			it->setSize(s);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgVisionSensors::on_qqSizeY_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		bool ok;
		float newVal=ui->qqSizeY->text().toFloat(&ok);
		if (ok)
		{
			CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
			C3Vector s(it->getSize());
			s(1)=newVal*gv::userToMeter;
			it->setSize(s);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgVisionSensors::on_qqSizeZ_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		bool ok;
		float newVal=ui->qqSizeZ->text().toFloat(&ok);
		if (ok)
		{
			CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
			C3Vector s(it->getSize());
			s(2)=newVal*gv::userToMeter;
			it->setSize(s);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgVisionSensors::on_qqAdjustImageColor_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
		CQDlgImageColor theDialog(this);
		float col[3];
		it->getDefaultBufferValues(col);
		theDialog.sameAsFog=it->getUseEnvironmentBackgroundColor();
		theDialog.red=col[0];
		theDialog.green=col[1];
		theDialog.blue=col[2];
		theDialog.refresh();
		if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
		{
			col[0]=theDialog.red;
			col[1]=theDialog.green;
			col[2]=theDialog.blue;
			it->setDefaultBufferValues(col);
			it->setUseEnvironmentBackgroundColor(theDialog.sameAsFog);
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgVisionSensors::on_qqApplyMainProperties_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		CVisionSensor* last=(CVisionSensor*)App::ct->objCont->getLastSelection();
		for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
		{
			CVisionSensor* it=App::ct->objCont->getVisionSensor(App::ct->objCont->getSelID(i));
			int r[2];
			last->getDesiredResolution(r);
			float b[3];
			last->getDefaultBufferValues(b);
			if (it!=NULL)
			{
				it->setExplicitHandling(last->getExplicitHandling());
				it->setUseExternalImage(last->getUseExternalImage());
				it->setPerspectiveOperation(last->getPerspectiveOperation());
				it->setShowFogIfAvailable(last->getShowFogIfAvailable());
				it->setShowVolumeWhenNotDetecting(last->getShowVolumeWhenNotDetecting());
				it->setShowVolumeWhenDetecting(last->getShowVolumeWhenDetecting());
				it->setIgnoreRGBInfo(last->getIgnoreRGBInfo());
				it->setIgnoreDepthInfo(last->getIgnoreDepthInfo());
				it->setNearClippingPlane(last->getNearClippingPlane());
				it->setFarClippingPlane(last->getFarClippingPlane());
				it->setViewAngle(last->getViewAngle());
				it->setOrthoViewSize(last->getOrthoViewSize());
				it->setSize(last->getSize());
				it->setDesiredResolution(r);
				it->setUseEnvironmentBackgroundColor(last->getUseEnvironmentBackgroundColor());
				it->setDefaultBufferValues(b);
				App::ct->undoBufferContainer->announceChangeStart(); // ************************** UNDO thingy **************************
			}
		}
		App::ct->undoBufferContainer->announceChangeEnd(); // ************************** UNDO thingy **************************
	}
}

void CQDlgVisionSensors::on_qqAdjustFilters_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		showFilterWindow=!showFilterWindow;
		if (App::mainWindow->dlgCont->isVisible(VISION_SENSOR_FILTER_DLG)!=showFilterWindow)
			App::mainWindow->dlgCont->toggle(VISION_SENSOR_FILTER_DLG);
	}
}

void CQDlgVisionSensors::on_qqCasingColorPassive_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&it->bodyColor,App::mainWindow,true,true,true,true,false,false,true,false);
	}
}

void CQDlgVisionSensors::on_qqCasingColorActive_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&it->bodyActiveColor,App::mainWindow,true,true,true,true,false,false,true,false);
	}
}

void CQDlgVisionSensors::on_qqVolumeColorPassive_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&it->detectionVolumeColor,App::mainWindow,true,true,true,true,false,false,true,false);
	}
}

void CQDlgVisionSensors::on_qqVolumeColorActive_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&it->detectionVolumeActiveColor,App::mainWindow,true,true,true,true,false,false,true,false);
	}
}

void CQDlgVisionSensors::on_qqApplyColors_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		CVisionSensor* last=(CVisionSensor*)App::ct->objCont->getLastSelection();
		for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
		{
			CVisionSensor* it=App::ct->objCont->getVisionSensor(App::ct->objCont->getSelID(i));
			if (it!=NULL)
			{
				last->bodyColor.copyYourselfInto(&it->bodyColor);
				last->bodyActiveColor.copyYourselfInto(&it->bodyActiveColor);
				last->detectionVolumeColor.copyYourselfInto(&it->detectionVolumeColor);
				last->detectionVolumeActiveColor.copyYourselfInto(&it->detectionVolumeActiveColor);
				App::ct->undoBufferContainer->announceChangeStart(); // ************************** UNDO thingy **************************
			}
		}
		App::ct->undoBufferContainer->announceChangeEnd(); // ************************** UNDO thingy **************************
	}
}

void CQDlgVisionSensors::on_qqEntityCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!inMainRefreshRoutine)
		{
			if (!App::ct->objCont->isLastSelectionAVisionSensor())
				return;
			int objID=ui->qqEntityCombo->itemData(ui->qqEntityCombo->currentIndex()).toInt();
			CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
			it->setDetectableEntityID(objID);
			if (objID!=-1)
			{
				bool displayWarning=false;
				if ((objID<SIM_IDSTART_COLLECTION)&&(objID>=0))
				{
					C3DObject* it2=App::ct->objCont->getObject(objID);
					if (it2!=NULL)
						displayWarning|=((it2->getMainPropertyLocal()&sim_objectspecialproperty_renderable)==0);
				}
				if (displayWarning)
					App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Scene object"),strTranslate(IDS_OBJECT_NOT_RENDERABLE_WARNING),VMessageBox::OKELI);
			}

			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgVisionSensors::on_qqIgnoreRGB_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
		it->setIgnoreRGBInfo(!it->getIgnoreRGBInfo());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgVisionSensors::on_qqIgnoreDepth_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
		it->setIgnoreDepthInfo(!it->getIgnoreDepthInfo());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}
