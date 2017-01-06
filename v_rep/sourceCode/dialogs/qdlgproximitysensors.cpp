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
#include "qdlgproximitysensors.h"
#include "ui_qdlgproximitysensors.h"
#include "Tt.h"
#include "GV.h"
#include "qdlgmaterial.h"
#include "IloIlo.h"
#include "qdlgproxsensdetectionparam.h"
#include "qdlgdetectionvolume.h"
#include "v_repStrings.h"
#include "App.h"

CQDlgProximitySensors::CQDlgProximitySensors(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgProximitySensors)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
}

CQDlgProximitySensors::~CQDlgProximitySensors()
{
    delete ui;
}

void CQDlgProximitySensors::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgProximitySensors::refresh()
{
	inMainRefreshRoutine=true;
	CProxSensor* it=NULL;
	if (App::ct->objCont->isLastSelectionAProxSensor())
		it=(CProxSensor*)App::ct->objCont->getLastSelection();


	bool isSensor=App::ct->objCont->isLastSelectionAProxSensor();
	bool manySensors=App::ct->objCont->getProxSensorNumberInSelection()>1;
//	bool noEditMode=App::ct->objCont->getEditModeType()==NO_EDIT_MODE;
	bool noSim=App::ct->simulation->isSimulationStopped();

	ui->qqEnableAll->setChecked(App::ct->mainSettings->proximitySensorsEnabled);

	ui->qqExplicitHandling->setEnabled(isSensor);
	ui->qqShowDetecting->setEnabled(isSensor);
	ui->qqShowNotDetecting->setEnabled(isSensor);
	ui->qqPointSize->setEnabled(isSensor);
	ui->qqSensorTypeCombo->setEnabled(isSensor);
	ui->qqSensorTypeCombo->clear();
	ui->qqApplyMain->setEnabled(isSensor&&manySensors);

	ui->qqAdjustVolume->setEnabled(true);
	ui->qqAdjustVolume->setChecked(CQDlgDetectionVolume::showVolumeWindow);

	ui->qqEntityToDetect->setEnabled(isSensor&&noSim);
	ui->qqEntityToDetect->clear();
	ui->qqApplyEntityToDetect->setEnabled(isSensor&&noSim&&manySensors);

	ui->qqAdjustDetectionParams->setEnabled(isSensor&&noSim);
	ui->qqApplyDetectionParams->setEnabled(isSensor&&noSim&&manySensors);

	ui->qqPassiveVolumeColor->setEnabled(isSensor);
	ui->qqActiveVolumeColor->setEnabled(isSensor);
	ui->qqMinDistColor->setEnabled(isSensor);
	ui->qqRayColor->setEnabled(isSensor);
	ui->qqApplyColors->setEnabled(isSensor&&manySensors);

	ui->qqShowDetecting->setChecked(isSensor&&it->getShowVolumeWhenDetecting());
	ui->qqShowNotDetecting->setChecked(isSensor&&it->getShowVolumeWhenNotDetecting());
	ui->qqExplicitHandling->setChecked(isSensor&&it->getExplicitHandling());

	if (isSensor)
	{
		ui->qqPointSize->setText(gv::getSizeStr(false,it->getSize(),0).c_str());

		ui->qqSensorTypeCombo->addItem(strTranslate(IDS_DETECTABLE_ULTRASONIC),QVariant(sim_objectspecialproperty_detectable_ultrasonic));
		ui->qqSensorTypeCombo->addItem(strTranslate(IDS_DETECTABLE_INFRARED),QVariant(sim_objectspecialproperty_detectable_infrared));
		ui->qqSensorTypeCombo->addItem(strTranslate(IDS_DETECTABLE_LASER),QVariant(sim_objectspecialproperty_detectable_laser));
		ui->qqSensorTypeCombo->addItem(strTranslate(IDS_DETECTABLE_INDUCTIVE),QVariant(sim_objectspecialproperty_detectable_inductive));
		ui->qqSensorTypeCombo->addItem(strTranslate(IDS_DETECTABLE_CAPACITIVE),QVariant(sim_objectspecialproperty_detectable_capacitive));
		for (int i=0;i<ui->qqSensorTypeCombo->count();i++)
		{
			if (ui->qqSensorTypeCombo->itemData(i).toInt()==it->getSensableType())
			{
				ui->qqSensorTypeCombo->setCurrentIndex(i);
				break;
			}
		}


		ui->qqEntityToDetect->addItem(strTranslate(IDSN_ALL_DETECTABLE_OBJECTS_IN_SCENE),QVariant(-1));
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
			ui->qqEntityToDetect->addItem(names[i].c_str(),QVariant(ids[i]));

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
			ui->qqEntityToDetect->addItem(names[i].c_str(),QVariant(ids[i]));

		names.clear();
		ids.clear();

		// Now dummies:
		for (int i=0;i<int(App::ct->objCont->dummyList.size());i++)
		{
			CDummy* it=App::ct->objCont->getDummy(App::ct->objCont->dummyList[i]);
			std::string name(tt::decorateString("[",strTranslate(IDSN_DUMMY),"] "));
			name+=it->getName();
			names.push_back(name);
			ids.push_back(it->getID());
		}
		tt::orderStrings(names,ids);
		for (int i=0;i<int(names.size());i++)
			ui->qqEntityToDetect->addItem(names[i].c_str(),QVariant(ids[i]));

		// Select current item:
		for (int i=0;i<ui->qqEntityToDetect->count();i++)
		{
			if (ui->qqEntityToDetect->itemData(i).toInt()==it->getSensableObject())
			{
				ui->qqEntityToDetect->setCurrentIndex(i);
				break;
			}
		}


	}
	else
	{
		ui->qqPointSize->setText("");
	}

	inMainRefreshRoutine=false;
}

void CQDlgProximitySensors::on_qqEnableAll_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->mainSettings->proximitySensorsEnabled=!App::ct->mainSettings->proximitySensorsEnabled;
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgProximitySensors::on_qqExplicitHandling_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAProxSensor())
			return;
		CProxSensor* theSensor=(CProxSensor*)App::ct->objCont->getLastSelection();
		theSensor->setExplicitHandling(!theSensor->getExplicitHandling());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgProximitySensors::on_qqSensorTypeCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAProxSensor())
			return;
		if (!inMainRefreshRoutine)
		{
			int objID=ui->qqSensorTypeCombo->itemData(ui->qqSensorTypeCombo->currentIndex()).toInt();
			CProxSensor* it=(CProxSensor*)App::ct->objCont->getLastSelection();
			it->setSensableType(objID);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgProximitySensors::on_qqShowDetecting_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAProxSensor())
			return;
		CProxSensor* it=(CProxSensor*)App::ct->objCont->getLastSelection();
		it->setShowVolumeWhenDetecting(!it->getShowVolumeWhenDetecting());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgProximitySensors::on_qqPointSize_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAProxSensor())
			return;
		bool ok;
		float newVal=ui->qqPointSize->text().toFloat(&ok);
		if (ok)
		{
			CProxSensor* it=(CProxSensor*)App::ct->objCont->getLastSelection();
			it->setSize(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgProximitySensors::on_qqShowNotDetecting_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAProxSensor())
			return;
		CProxSensor* it=(CProxSensor*)App::ct->objCont->getLastSelection();
		it->setShowVolumeWhenNotDetecting(!it->getShowVolumeWhenNotDetecting());
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgProximitySensors::on_qqApplyMain_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAProxSensor())
			return;
		if (App::ct->objCont->getProxSensorNumberInSelection()<2)
			return;
		CProxSensor* last=(CProxSensor*)App::ct->objCont->getLastSelection();
		for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
		{
			CProxSensor* it=App::ct->objCont->getProximitySensor(App::ct->objCont->getSelID(i));
			if (it!=NULL)
			{
				it->setSensableType(last->getSensableType());
				it->setExplicitHandling(last->getExplicitHandling());
				it->setSize(last->getSize());
				it->setShowVolumeWhenNotDetecting(last->getShowVolumeWhenNotDetecting());
				it->setShowVolumeWhenDetecting(last->getShowVolumeWhenDetecting());
			}
		}
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		refresh();
	}
}

void CQDlgProximitySensors::on_qqAdjustVolume_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CQDlgDetectionVolume::showVolumeWindow=!CQDlgDetectionVolume::showVolumeWindow;
		if (App::mainWindow->dlgCont->isVisible(DETECTION_VOLUME_DLG)!=CQDlgDetectionVolume::showVolumeWindow)
			App::mainWindow->dlgCont->toggle(DETECTION_VOLUME_DLG);
	}
}

void CQDlgProximitySensors::on_qqEntityToDetect_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAProxSensor())
			return;
		if (!inMainRefreshRoutine)
		{
			int objID=ui->qqEntityToDetect->itemData(ui->qqEntityToDetect->currentIndex()).toInt();
			CProxSensor* it=(CProxSensor*)App::ct->objCont->getLastSelection();
			it->setSensableObject(objID);
			if (objID!=-1)
			{
				bool displayWarning=false;
				if ((objID<SIM_IDSTART_COLLECTION)&&(objID>=0))
				{
					C3DObject* it2=App::ct->objCont->getObject(objID);
					if (it2!=NULL)
						displayWarning|=((it2->getMainPropertyLocal()&sim_objectspecialproperty_detectable_all)==0);
				}
				if (displayWarning)
					App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Scene object"),strTranslate(IDS_OBJECT_NOT_DETECTABLE_WARNING),VMessageBox::OKELI);
			}
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgProximitySensors::on_qqApplyEntityToDetect_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAProxSensor())
			return;
		CProxSensor* it=(CProxSensor*)App::ct->objCont->getLastSelection();
		for (int i=0;i<App::ct->objCont->getSelSize();i++)
		{
			CProxSensor* it2=App::ct->objCont->getProximitySensor(App::ct->objCont->getSelID(i));
			if (it2!=NULL)
			{
				if (it->getID()!=it2->getID())
					it2->setSensableObject(it->getSensableObject());
			}
		}
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
	}
}

void CQDlgProximitySensors::on_qqAdjustDetectionParams_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		if (!App::ct->objCont->isLastSelectionAProxSensor())
			return;
		CQDlgProxSensDetectionParam theDialog(this);
		CProxSensor* it=(CProxSensor*)App::ct->objCont->getLastSelection();

		theDialog.frontFace=it->getFrontFaceDetection();
		theDialog.backFace=it->getBackFaceDetection();
		theDialog.fast=!it->getClosestObjectMode();
		theDialog.limitedAngle=it->getNormalCheck();
		theDialog.angle=it->getAllowedNormal();
		theDialog.occlusionCheck=it->getCheckOcclusions();
		theDialog.distanceContraint=it->convexVolume->getSmallestDistanceEnabled();
		theDialog.minimumDistance=it->convexVolume->getSmallestDistanceAllowed();
		theDialog.randomizedDetection=it->getRandomizedDetection();
		theDialog.rayCount=it->getRandomizedDetectionSampleCount();
		theDialog.rayDetectionCount=it->getRandomizedDetectionCountForDetection();
		theDialog.refresh();

		if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
		{
			it->setFrontFaceDetection(theDialog.frontFace);
			it->setBackFaceDetection(theDialog.backFace);
			it->setClosestObjectMode(!theDialog.fast);
			it->setNormalCheck(theDialog.limitedAngle);
			it->setAllowedNormal(theDialog.angle);
			it->setCheckOcclusions(theDialog.occlusionCheck);
			it->convexVolume->setSmallestDistanceEnabled(theDialog.distanceContraint);
			it->convexVolume->setSmallestDistanceAllowed(theDialog.minimumDistance);
			if (it->getRandomizedDetection())
			{
				it->setRandomizedDetectionSampleCount(theDialog.rayCount);
				it->setRandomizedDetectionCountForDetection(theDialog.rayDetectionCount);
			}
		}
		refresh();
	}
}

void CQDlgProximitySensors::on_qqApplyDetectionParams_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAProxSensor())
			return;
		CProxSensor* it=(CProxSensor*)App::ct->objCont->getLastSelection();
		for (int i=0;i<App::ct->objCont->getSelSize();i++)
		{
			CProxSensor* it2=App::ct->objCont->getProximitySensor(App::ct->objCont->getSelID(i));
			if (it2!=NULL)
			{
				if (it->getID()!=it2->getID())
				{
					if (it->getRandomizedDetection())
					{
						it2->setRandomizedDetectionSampleCount(it->getRandomizedDetectionSampleCount());
						it2->setRandomizedDetectionCountForDetection(it->getRandomizedDetectionCountForDetection());
					}
					it2->setFrontFaceDetection(it->getFrontFaceDetection());
					it2->setBackFaceDetection(it->getBackFaceDetection());
					it2->setClosestObjectMode(it->getClosestObjectMode());
					it2->setNormalCheck(it->getNormalCheck());
					it2->setAllowedNormal(it->getAllowedNormal());
					it2->setCheckOcclusions(it->getCheckOcclusions());
					it2->convexVolume->setSmallestDistanceEnabled(it->convexVolume->getSmallestDistanceEnabled());
					it2->convexVolume->setSmallestDistanceAllowed(it->convexVolume->getSmallestDistanceAllowed());
				}
			}
		}
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
	}
}

void CQDlgProximitySensors::on_qqPassiveVolumeColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAProxSensor())
			return;
		CProxSensor* it=(CProxSensor*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&it->passiveVolumeColor,App::mainWindow,true,true,true,true,false,false,true,false);
	}
}

void CQDlgProximitySensors::on_qqActiveVolumeColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAProxSensor())
			return;
		CProxSensor* it=(CProxSensor*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&it->activeVolumeColor,App::mainWindow,true,true,true,true,false,false,true,false);
	}
}

void CQDlgProximitySensors::on_qqRayColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAProxSensor())
			return;
		CProxSensor* it=(CProxSensor*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&it->detectionRayColor,App::mainWindow,true,true,true,true,false,false,true,false);
	}
}

void CQDlgProximitySensors::on_qqMinDistColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAProxSensor())
			return;
		CProxSensor* it=(CProxSensor*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgMaterial::display(identification,&it->closestDistanceVolumeColor,App::mainWindow,true,true,true,true,false,false,true,false);
	}
}

void CQDlgProximitySensors::on_qqApplyColors_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAProxSensor())
			return;
		CProxSensor* it=(CProxSensor*)App::ct->objCont->getLastSelection();
		for (int i=0;i<App::ct->objCont->getSelSize();i++)
		{
			CProxSensor* it2=App::ct->objCont->getProximitySensor(App::ct->objCont->getSelID(i));
			if (it2!=NULL)
			{
				if (it->getID()!=it2->getID())
				{
					it->passiveVolumeColor.copyYourselfInto(&it2->passiveVolumeColor);
					it->activeVolumeColor.copyYourselfInto(&it2->activeVolumeColor);
					it->detectionRayColor.copyYourselfInto(&it2->detectionRayColor);
					it->closestDistanceVolumeColor.copyYourselfInto(&it2->closestDistanceVolumeColor);
				}
			}
		}
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
	}
}
