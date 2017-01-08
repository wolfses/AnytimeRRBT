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
#include "qdlgdetectionvolume.h"
#include "ui_qdlgdetectionvolume.h"
#include "GV.h"
#include "Tt.h"
#include "App.h"
#include "v_repStrings.h"

bool CQDlgDetectionVolume::showVolumeWindow=false;

CQDlgDetectionVolume::CQDlgDetectionVolume(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgDetectionVolume)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
}

CQDlgDetectionVolume::~CQDlgDetectionVolume()
{
    delete ui;
}

void CQDlgDetectionVolume::cancelEvent()
{ // no cancel event allowed
	showVolumeWindow=false;
	CDlgEx::cancelEvent();
	App::ct->objCont->setFullDialogRefreshFlag();
}

void CQDlgDetectionVolume::refresh()
{
	inMainRefreshRoutine=true;
	bool snr=App::ct->simulation->isSimulationStopped();
	bool prox=App::ct->objCont->isLastSelectionAProxSensor();
	bool mill=App::ct->objCont->isLastSelectionAMill();
	bool ssel=false;
	if (prox)
		ssel=(App::ct->objCont->getProxSensorNumberInSelection()>1);
	if (mill)
		ssel=(App::ct->objCont->getMillNumberInSelection()>1);
	CConvexVolume* cv=NULL;
	CProxSensor* proxIt=NULL;
	CMill* millIt=NULL;
	if (prox)
	{
		proxIt=(CProxSensor*)App::ct->objCont->getLastSelection();
		cv=proxIt->convexVolume;
	}
	if (mill)
	{
		millIt=(CMill*)App::ct->objCont->getLastSelection();
		cv=millIt->convexVolume;
	}

	ui->qqType->clear();
	ui->qqType->setEnabled((prox||mill)&&snr);
	ui->qqApplyAll->setEnabled(ssel&&snr);


	if (prox||mill)
	{
		// The combo first:
		if (prox)
		{
			ui->qqType->addItem(strTranslate(IDSN_RAY),QVariant(0));
			ui->qqType->addItem(strTranslate(IDSN_RANDOMIZED_RAY),QVariant(1));
		}
		ui->qqType->addItem(strTranslate(IDSN_PYRAMID),QVariant(2));
		ui->qqType->addItem(strTranslate(IDSN_CYLINDER),QVariant(3));
		ui->qqType->addItem(strTranslate(IDSN_DISC),QVariant(4));
		ui->qqType->addItem(strTranslate(IDSN_CONE),QVariant(5));
		// Select the current item:
		if (prox)
		{
			if (proxIt->getSensorType()==sim_proximitysensor_ray_subtype)
			{
				if (proxIt->getRandomizedDetection())
					ui->qqType->setCurrentIndex(1);
				else
					ui->qqType->setCurrentIndex(0);
			}
			if (proxIt->getSensorType()==sim_proximitysensor_pyramid_subtype)
				ui->qqType->setCurrentIndex(2);
			if (proxIt->getSensorType()==sim_proximitysensor_cylinder_subtype)
				ui->qqType->setCurrentIndex(3);
			if (proxIt->getSensorType()==sim_proximitysensor_disc_subtype)
				ui->qqType->setCurrentIndex(4);
			if (proxIt->getSensorType()==sim_proximitysensor_cone_subtype)
				ui->qqType->setCurrentIndex(5);
		}
		if (mill)
		{
			if (millIt->getMillType()==sim_mill_pyramid_subtype)
				ui->qqType->setCurrentIndex(0);
			if (millIt->getMillType()==sim_mill_cylinder_subtype)
				ui->qqType->setCurrentIndex(1);
			if (millIt->getMillType()==sim_mill_disc_subtype)
				ui->qqType->setCurrentIndex(2);
			if (millIt->getMillType()==sim_mill_cone_subtype)
				ui->qqType->setCurrentIndex(3);
		}

		if (prox)
		{
			setWindowTitle(strTranslate(IDSN_DETECTION_VOLUME_PROPERTIES));

			ui->qqOffset->setEnabled(true);
			if ( (proxIt->getSensorType()==sim_proximitysensor_ray_subtype)&&proxIt->getRandomizedDetection() )
				ui->qqOffset->setText(gv::getSizeStr(true,cv->getRadius(),0).c_str()); // Special
			else
				ui->qqOffset->setText(gv::getSizeStr(true,cv->getOffset(),0).c_str());
			ui->qqRange->setEnabled(true);
			ui->qqRange->setText(gv::getSizeStr(false,cv->getRange(),0).c_str());
			if (proxIt->getSensorType()==sim_proximitysensor_pyramid_subtype)
			{
				ui->qqSizeX->setEnabled(true);
				ui->qqSizeX->setText(gv::getSizeStr(false,cv->getXSize(),0).c_str());
				ui->qqSizeY->setEnabled(true);
				ui->qqSizeY->setText(gv::getSizeStr(false,cv->getYSize(),0).c_str());
				ui->qqSizeFarX->setEnabled(true);
				ui->qqSizeFarX->setText(gv::getSizeStr(false,cv->getXSizeFar(),0).c_str());
				ui->qqSizeFarY->setEnabled(true);
				ui->qqSizeFarY->setText(gv::getSizeStr(false,cv->getYSizeFar(),0).c_str());

				ui->qqRadius->setEnabled(false);
				ui->qqRadius->setText("");
				ui->qqRadiusFar->setEnabled(false);
				ui->qqRadiusFar->setText("");

				ui->qqAngle->setEnabled(false);
				ui->qqAngle->setText("");

				ui->qqFaceCount->setEnabled(false);
				ui->qqFaceCount->setText("");
				ui->qqFaceCountFar->setEnabled(false);
				ui->qqFaceCountFar->setText("");

				ui->qqSubdivisions->setEnabled(false);
				ui->qqSubdivisions->setText("");
				ui->qqSubdivisionsFar->setEnabled(false);
				ui->qqSubdivisionsFar->setText("");

				ui->qqInsideGap->setEnabled(false);
				ui->qqInsideGap->setText("");
			}
			if (proxIt->getSensorType()==sim_proximitysensor_cylinder_subtype)
			{
				ui->qqRadius->setEnabled(true);
				ui->qqRadius->setText(gv::getSizeStr(false,cv->getRadius(),0).c_str());
				ui->qqRadiusFar->setEnabled(true);
				ui->qqRadiusFar->setText(gv::getSizeStr(false,cv->getRadiusFar(),0).c_str());
				ui->qqFaceCount->setEnabled(true);
				ui->qqFaceCount->setText(tt::FNb(0,cv->getFaceNumber()).c_str());

				ui->qqAngle->setEnabled(false);
				ui->qqAngle->setText("");

				ui->qqSizeX->setEnabled(false);
				ui->qqSizeX->setText("");
				ui->qqSizeY->setEnabled(false);
				ui->qqSizeY->setText("");
				ui->qqSizeFarX->setEnabled(false);
				ui->qqSizeFarX->setText("");
				ui->qqSizeFarY->setEnabled(false);
				ui->qqSizeFarY->setText("");

				ui->qqFaceCountFar->setEnabled(false);
				ui->qqFaceCountFar->setText("");

				ui->qqSubdivisions->setEnabled(false);
				ui->qqSubdivisions->setText("");
				ui->qqSubdivisionsFar->setEnabled(false);
				ui->qqSubdivisionsFar->setText("");

				ui->qqInsideGap->setEnabled(false);
				ui->qqInsideGap->setText("");
			}
			if (proxIt->getSensorType()==sim_proximitysensor_ray_subtype)
			{
				ui->qqAngle->setEnabled(proxIt->getRandomizedDetection());
				if (proxIt->getRandomizedDetection())
					ui->qqAngle->setText(gv::getAngleStr(false,cv->getAngle(),0).c_str());
				else
					ui->qqAngle->setText("");

				ui->qqRadius->setEnabled(false);
				ui->qqRadius->setText("");
				ui->qqRadiusFar->setEnabled(false);
				ui->qqRadiusFar->setText("");

				ui->qqSizeX->setEnabled(false);
				ui->qqSizeX->setText("");
				ui->qqSizeY->setEnabled(false);
				ui->qqSizeY->setText("");
				ui->qqSizeFarX->setEnabled(false);
				ui->qqSizeFarX->setText("");
				ui->qqSizeFarY->setEnabled(false);
				ui->qqSizeFarY->setText("");

				ui->qqFaceCount->setEnabled(false);
				ui->qqFaceCount->setText("");
				ui->qqFaceCountFar->setEnabled(false);
				ui->qqFaceCountFar->setText("");

				ui->qqSubdivisions->setEnabled(false);
				ui->qqSubdivisions->setText("");
				ui->qqSubdivisionsFar->setEnabled(false);
				ui->qqSubdivisionsFar->setText("");

				ui->qqInsideGap->setEnabled(false);
				ui->qqInsideGap->setText("");
			}
			if (proxIt->getSensorType()==sim_proximitysensor_disc_subtype)
			{
				ui->qqSizeY->setEnabled(true);
				ui->qqSizeY->setText(gv::getSizeStr(false,cv->getYSize(),0).c_str());
				ui->qqRadius->setEnabled(true);
				ui->qqRadius->setText(gv::getSizeStr(false,cv->getRadius(),0).c_str());
				ui->qqFaceCount->setEnabled(true);
				ui->qqFaceCount->setText(tt::FNb(0,cv->getFaceNumber()).c_str());
				ui->qqFaceCountFar->setEnabled(true);
				ui->qqFaceCountFar->setText(tt::FNb(0,cv->getFaceNumberFar()).c_str());
				ui->qqAngle->setEnabled(true);
				ui->qqAngle->setText(gv::getAngleStr(false,cv->getAngle(),0).c_str());
				ui->qqInsideGap->setEnabled(true);
				ui->qqInsideGap->setText(tt::FNb(0,cv->getInsideAngleThing(),2,false).c_str());

				ui->qqRadiusFar->setEnabled(false);
				ui->qqRadiusFar->setText("");

				ui->qqSizeX->setEnabled(false);
				ui->qqSizeX->setText("");
				ui->qqSizeFarX->setEnabled(false);
				ui->qqSizeFarX->setText("");
				ui->qqSizeFarY->setEnabled(false);
				ui->qqSizeFarY->setText("");

				ui->qqSubdivisions->setEnabled(false);
				ui->qqSubdivisions->setText("");
				ui->qqSubdivisionsFar->setEnabled(false);
				ui->qqSubdivisionsFar->setText("");
			}
			if (proxIt->getSensorType()==sim_proximitysensor_cone_subtype)
			{
				ui->qqRadius->setEnabled(true);
				ui->qqRadius->setText(gv::getSizeStr(false,cv->getRadius(),0).c_str());
				ui->qqFaceCount->setEnabled(true);
				ui->qqFaceCount->setText(tt::FNb(0,cv->getFaceNumber()).c_str());
				ui->qqAngle->setEnabled(true);
				ui->qqAngle->setText(gv::getAngleStr(false,cv->getAngle(),0).c_str());
				ui->qqInsideGap->setEnabled(true);
				ui->qqInsideGap->setText(tt::FNb(0,cv->getInsideAngleThing(),2,false).c_str());
				ui->qqSubdivisions->setEnabled(true);
				ui->qqSubdivisions->setText(tt::FNb(0,cv->getSubdivisions()).c_str());
				ui->qqSubdivisionsFar->setEnabled(true);
				ui->qqSubdivisionsFar->setText(tt::FNb(0,cv->getSubdivisionsFar()).c_str());

				ui->qqRadiusFar->setEnabled(false);
				ui->qqRadiusFar->setText("");

				ui->qqSizeX->setEnabled(false);
				ui->qqSizeX->setText("");
				ui->qqSizeY->setEnabled(false);
				ui->qqSizeY->setText("");
				ui->qqSizeFarX->setEnabled(false);
				ui->qqSizeFarX->setText("");
				ui->qqSizeFarY->setEnabled(false);
				ui->qqSizeFarY->setText("");

				ui->qqFaceCountFar->setEnabled(false);
				ui->qqFaceCountFar->setText("");
			}
		}

		if (mill)
		{
			setWindowTitle(strTranslate(IDSN_CUTTING_VOLUME_PROPERTIES));
			ui->qqInsideGap->setEnabled(false);
			ui->qqInsideGap->setText("");
			ui->qqSubdivisions->setEnabled(false);
			ui->qqSubdivisions->setText("");

			ui->qqOffset->setEnabled(true);
			ui->qqOffset->setText(gv::getSizeStr(true,cv->getOffset(),0).c_str());
			ui->qqRange->setEnabled(true);
			ui->qqRange->setText(gv::getSizeStr(false,cv->getRange(),0).c_str());
			if (millIt->getMillType()==sim_mill_pyramid_subtype)
			{
				ui->qqSizeX->setEnabled(true);
				ui->qqSizeX->setText(gv::getSizeStr(false,cv->getXSize(),0).c_str());
				ui->qqSizeY->setEnabled(true);
				ui->qqSizeY->setText(gv::getSizeStr(false,cv->getYSize(),0).c_str());
				ui->qqSizeFarX->setEnabled(true);
				ui->qqSizeFarX->setText(gv::getSizeStr(false,cv->getXSizeFar(),0).c_str());
				ui->qqSizeFarY->setEnabled(true);
				ui->qqSizeFarY->setText(gv::getSizeStr(false,cv->getYSizeFar(),0).c_str());

				ui->qqRadius->setEnabled(false);
				ui->qqRadius->setText("");
				ui->qqRadiusFar->setEnabled(false);
				ui->qqRadiusFar->setText("");

				ui->qqAngle->setEnabled(false);
				ui->qqAngle->setText("");

				ui->qqFaceCount->setEnabled(false);
				ui->qqFaceCount->setText("");
				ui->qqFaceCountFar->setEnabled(false);
				ui->qqFaceCountFar->setText("");

				ui->qqSubdivisions->setEnabled(false);
				ui->qqSubdivisions->setText("");
				ui->qqSubdivisionsFar->setEnabled(false);
				ui->qqSubdivisionsFar->setText("");

				ui->qqInsideGap->setEnabled(false);
				ui->qqInsideGap->setText("");
			}
			if (millIt->getMillType()==sim_mill_cylinder_subtype)
			{
				ui->qqRadius->setEnabled(true);
				ui->qqRadius->setText(gv::getSizeStr(false,cv->getRadius(),0).c_str());
				ui->qqRadiusFar->setEnabled(true);
				ui->qqRadiusFar->setText(gv::getSizeStr(false,cv->getRadiusFar(),0).c_str());
				ui->qqFaceCount->setEnabled(true);
				ui->qqFaceCount->setText(tt::FNb(0,cv->getFaceNumber()).c_str());


				ui->qqAngle->setEnabled(false);
				ui->qqAngle->setText("");

				ui->qqSizeX->setEnabled(false);
				ui->qqSizeX->setText("");
				ui->qqSizeY->setEnabled(false);
				ui->qqSizeY->setText("");
				ui->qqSizeFarX->setEnabled(false);
				ui->qqSizeFarX->setText("");
				ui->qqSizeFarY->setEnabled(false);
				ui->qqSizeFarY->setText("");

				ui->qqFaceCountFar->setEnabled(false);
				ui->qqFaceCountFar->setText("");

				ui->qqSubdivisions->setEnabled(false);
				ui->qqSubdivisions->setText("");
				ui->qqSubdivisionsFar->setEnabled(false);
				ui->qqSubdivisionsFar->setText("");

				ui->qqInsideGap->setEnabled(false);
				ui->qqInsideGap->setText("");
			}
			if (millIt->getMillType()==sim_mill_disc_subtype)
			{
				ui->qqSizeY->setEnabled(true);
				ui->qqSizeY->setText(gv::getSizeStr(false,cv->getYSize(),0).c_str());
				ui->qqFaceCountFar->setEnabled(true);
				ui->qqFaceCountFar->setText(tt::FNb(0,cv->getFaceNumberFar()).c_str());
				ui->qqAngle->setEnabled(true);
				ui->qqAngle->setText(gv::getAngleStr(false,cv->getAngle(),0).c_str());

				ui->qqRadius->setEnabled(false);
				ui->qqRadius->setText("");
				ui->qqRadiusFar->setEnabled(false);
				ui->qqRadiusFar->setText("");


				ui->qqSizeX->setEnabled(false);
				ui->qqSizeX->setText("");
				ui->qqSizeFarX->setEnabled(false);
				ui->qqSizeFarX->setText("");
				ui->qqSizeFarY->setEnabled(false);
				ui->qqSizeFarY->setText("");

				ui->qqFaceCount->setEnabled(false);
				ui->qqFaceCount->setText("");

				ui->qqSubdivisions->setEnabled(false);
				ui->qqSubdivisions->setText("");
				ui->qqSubdivisionsFar->setEnabled(false);
				ui->qqSubdivisionsFar->setText("");

				ui->qqInsideGap->setEnabled(false);
				ui->qqInsideGap->setText("");
			}
			if (millIt->getMillType()==sim_mill_cone_subtype)
			{
				ui->qqFaceCount->setEnabled(true);
				ui->qqFaceCount->setText(tt::FNb(0,cv->getFaceNumber()).c_str());
				ui->qqAngle->setEnabled(true);
				ui->qqAngle->setText(gv::getAngleStr(false,cv->getAngle(),0).c_str());
				ui->qqSubdivisionsFar->setEnabled(true);
				ui->qqSubdivisionsFar->setText(tt::FNb(0,cv->getSubdivisionsFar()).c_str());

				ui->qqRadius->setEnabled(false);
				ui->qqRadius->setText("");
				ui->qqRadiusFar->setEnabled(false);
				ui->qqRadiusFar->setText("");


				ui->qqSizeX->setEnabled(false);
				ui->qqSizeX->setText("");
				ui->qqSizeY->setEnabled(false);
				ui->qqSizeY->setText("");
				ui->qqSizeFarX->setEnabled(false);
				ui->qqSizeFarX->setText("");
				ui->qqSizeFarY->setEnabled(false);
				ui->qqSizeFarY->setText("");

				ui->qqFaceCountFar->setEnabled(false);
				ui->qqFaceCountFar->setText("");

				ui->qqSubdivisions->setEnabled(false);
				ui->qqSubdivisions->setText("");

				ui->qqInsideGap->setEnabled(false);
				ui->qqInsideGap->setText("");
			}
		}
	}
	else
	{
		setWindowTitle(strTranslate(IDSN_VOLUME_PROPERTIES));
		ui->qqOffset->setEnabled(false);
		ui->qqOffset->setText("");
		ui->qqRange->setEnabled(false);
		ui->qqRange->setText("");

		ui->qqRadius->setEnabled(false);
		ui->qqRadius->setText("");
		ui->qqRadiusFar->setEnabled(false);
		ui->qqRadiusFar->setText("");

		ui->qqAngle->setEnabled(false);
		ui->qqAngle->setText("");

		ui->qqSizeX->setEnabled(false);
		ui->qqSizeX->setText("");
		ui->qqSizeY->setEnabled(false);
		ui->qqSizeY->setText("");
		ui->qqSizeFarX->setEnabled(false);
		ui->qqSizeFarX->setText("");
		ui->qqSizeFarY->setEnabled(false);
		ui->qqSizeFarY->setText("");

		ui->qqFaceCount->setEnabled(false);
		ui->qqFaceCount->setText("");
		ui->qqFaceCountFar->setEnabled(false);
		ui->qqFaceCountFar->setText("");

		ui->qqSubdivisions->setEnabled(false);
		ui->qqSubdivisions->setText("");
		ui->qqSubdivisionsFar->setEnabled(false);
		ui->qqSubdivisionsFar->setText("");

		ui->qqInsideGap->setEnabled(false);
		ui->qqInsideGap->setText("");
	}

	inMainRefreshRoutine=false;
}

CConvexVolume* CQDlgDetectionVolume::getCurrentConvexVolume()
{
	bool prox=App::ct->objCont->isLastSelectionAProxSensor();
	bool mill=App::ct->objCont->isLastSelectionAMill();
	CConvexVolume* cv=NULL;
	CProxSensor* proxIt=NULL;
	CMill* millIt=NULL;
	if (prox)
	{
		proxIt=(CProxSensor*)App::ct->objCont->getLastSelection();
		cv=proxIt->convexVolume;
	}
	if (mill)
	{
		millIt=(CMill*)App::ct->objCont->getLastSelection();
		cv=millIt->convexVolume;
	}
	return(cv);
}

CProxSensor* CQDlgDetectionVolume::getCurrentProxSensor()
{
	if (!App::ct->objCont->isLastSelectionAProxSensor())
		return(NULL);
	return((CProxSensor*)App::ct->objCont->getLastSelection());
}

CMill* CQDlgDetectionVolume::getCurrentMill()
{
	if (!App::ct->objCont->isLastSelectionAMill())
		return(NULL);
	return((CMill*)App::ct->objCont->getLastSelection());
}

void CQDlgDetectionVolume::on_qqType_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!inMainRefreshRoutine)
		{
			CConvexVolume* cv=getCurrentConvexVolume();
			if (cv!=NULL)
			{
				int index=ui->qqType->currentIndex();
				bool prox=App::ct->objCont->isLastSelectionAProxSensor();
				bool mill=App::ct->objCont->isLastSelectionAMill();
				CProxSensor* proxIt=NULL;
				CMill* millIt=NULL;
				if (prox)
				{
					proxIt=(CProxSensor*)App::ct->objCont->getLastSelection();
					if ((index==0)||(index==1))
					{
						proxIt->setSensorType(sim_proximitysensor_ray_subtype);
						proxIt->setRandomizedDetection(index==1);
					}
					if (index==2)
						proxIt->setSensorType(sim_proximitysensor_pyramid_subtype);
					if (index==3)
						proxIt->setSensorType(sim_proximitysensor_cylinder_subtype);
					if (index==4)
						proxIt->setSensorType(sim_proximitysensor_disc_subtype);
					if (index==5)
						proxIt->setSensorType(sim_proximitysensor_cone_subtype);
				}
				if (mill)
				{
					millIt=(CMill*)App::ct->objCont->getLastSelection();
					if (index==0)
						millIt->setMillType(sim_mill_pyramid_subtype);
					if (index==1)
						millIt->setMillType(sim_mill_cylinder_subtype);
					if (index==2)
						millIt->setMillType(sim_mill_disc_subtype);
					if (index==3)
						millIt->setMillType(sim_mill_cone_subtype);
				}
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			}
			refresh();
		}
	}
}

void CQDlgDetectionVolume::on_qqOffset_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CProxSensor* proxSensor=getCurrentProxSensor();
		CMill* mill=getCurrentMill();
		CConvexVolume* cv=getCurrentConvexVolume();
		bool ok;
		float newVal=ui->qqOffset->text().toFloat(&ok);
		if (ok&&cv&&(proxSensor!=NULL))
		{
			if ( (proxSensor->getSensorType()==sim_proximitysensor_ray_subtype)&&proxSensor->getRandomizedDetection() )
				cv->setRadius(newVal*gv::userToMeter); // Special case!
			else
				cv->setOffset(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		if (ok&&cv&&(mill!=NULL))
		{
			cv->setOffset(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgDetectionVolume::on_qqRadius_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CConvexVolume* cv=getCurrentConvexVolume();
		bool ok;
		float newVal=ui->qqRadius->text().toFloat(&ok);
		if (ok&&cv)
		{
			cv->setRadius(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgDetectionVolume::on_qqRange_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CConvexVolume* cv=getCurrentConvexVolume();
		bool ok;
		float newVal=ui->qqRange->text().toFloat(&ok);
		if (ok&&cv)
		{
			cv->setRange(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgDetectionVolume::on_qqRadiusFar_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CConvexVolume* cv=getCurrentConvexVolume();
		bool ok;
		float newVal=ui->qqRadiusFar->text().toFloat(&ok);
		if (ok&&cv)
		{
			cv->setRadiusFar(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgDetectionVolume::on_qqSizeX_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CConvexVolume* cv=getCurrentConvexVolume();
		bool ok;
		float newVal=ui->qqSizeX->text().toFloat(&ok);
		if (ok&&cv)
		{
			cv->setXSize(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgDetectionVolume::on_qqAngle_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CConvexVolume* cv=getCurrentConvexVolume();
		bool ok;
		float newVal=ui->qqAngle->text().toFloat(&ok);
		if (ok&&cv)
		{
			cv->setAngle(newVal*gv::userToRad);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgDetectionVolume::on_qqSizeY_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CConvexVolume* cv=getCurrentConvexVolume();
		bool ok;
		float newVal=ui->qqSizeY->text().toFloat(&ok);
		if (ok&&cv)
		{
			cv->setYSize(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgDetectionVolume::on_qqFaceCount_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CConvexVolume* cv=getCurrentConvexVolume();
		bool ok;
		int newVal=ui->qqFaceCount->text().toInt(&ok);
		if (ok&&cv)
		{
			cv->setFaceNumber(newVal);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgDetectionVolume::on_qqSizeFarX_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CConvexVolume* cv=getCurrentConvexVolume();
		bool ok;
		float newVal=ui->qqSizeFarX->text().toFloat(&ok);
		if (ok&&cv)
		{
			cv->setXSizeFar(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgDetectionVolume::on_qqFaceCountFar_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CConvexVolume* cv=getCurrentConvexVolume();
		bool ok;
		int newVal=ui->qqFaceCountFar->text().toInt(&ok);
		if (ok&&cv)
		{
			cv->setFaceNumberFar(newVal);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgDetectionVolume::on_qqSizeFarY_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CConvexVolume* cv=getCurrentConvexVolume();
		bool ok;
		float newVal=ui->qqSizeFarY->text().toFloat(&ok);
		if (ok&&cv)
		{
			cv->setYSizeFar(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgDetectionVolume::on_qqSubdivisions_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CProxSensor* proxSensor=getCurrentProxSensor();
		CConvexVolume* cv=getCurrentConvexVolume();
		bool ok;
		int newVal=ui->qqSubdivisions->text().toInt(&ok);
		if (ok&&cv&&(proxSensor!=NULL))
		{
			cv->setSubdivisions(newVal);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgDetectionVolume::on_qqInsideGap_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CProxSensor* proxSensor=getCurrentProxSensor();
		CConvexVolume* cv=getCurrentConvexVolume();
		bool ok;
		float newVal=ui->qqInsideGap->text().toFloat(&ok);
		if (ok&&cv&&(proxSensor!=NULL))
		{
			cv->setInsideAngleThing(newVal);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgDetectionVolume::on_qqSubdivisionsFar_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CConvexVolume* cv=getCurrentConvexVolume();
		bool ok;
		int newVal=ui->qqSubdivisionsFar->text().toInt(&ok);
		if (ok&&cv)
		{
			cv->setSubdivisionsFar(newVal);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgDetectionVolume::on_qqApplyAll_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CProxSensor* proxSensor=getCurrentProxSensor();
		CMill* mill=getCurrentMill();
	//	CConvexVolume* cv=getCurrentConvexVolume();
		if (proxSensor!=NULL)
		{
			CProxSensor* last=(CProxSensor*)App::ct->objCont->getLastSelection();
			for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
			{
				CProxSensor* it=App::ct->objCont->getProximitySensor(App::ct->objCont->getSelID(i));
				if (it!=NULL)
				{
					float w=it->convexVolume->getSmallestDistanceAllowed();
					bool ww=it->convexVolume->getSmallestDistanceEnabled();
					it->convexVolume->disableVolumeComputation(true);
					// Volume parameters:
					it->setSensorType(last->getSensorType());
					it->setRandomizedDetection(false); // somehow needed for next to work always...??
					it->setRandomizedDetection(last->getRandomizedDetection());
					it->convexVolume->setOffset(last->convexVolume->getOffset());
					it->convexVolume->setRange(last->convexVolume->getRange());
					it->convexVolume->setXSize(last->convexVolume->getXSize());
					it->convexVolume->setYSize(last->convexVolume->getYSize());
					it->convexVolume->setXSizeFar(last->convexVolume->getXSizeFar());
					it->convexVolume->setYSizeFar(last->convexVolume->getYSizeFar());
					it->convexVolume->setRadius(last->convexVolume->getRadius());
					it->convexVolume->setRadiusFar(last->convexVolume->getRadiusFar());
					it->convexVolume->setAngle(last->convexVolume->getAngle());
					it->convexVolume->setFaceNumber(last->convexVolume->getFaceNumber());
					it->convexVolume->setFaceNumberFar(last->convexVolume->getFaceNumberFar());
					it->convexVolume->setSubdivisions(last->convexVolume->getSubdivisions());
					it->convexVolume->setSubdivisionsFar(last->convexVolume->getSubdivisionsFar());
					it->convexVolume->setInsideAngleThing(last->convexVolume->getInsideAngleThing());
					// We have to recompute the planes:
					it->convexVolume->disableVolumeComputation(false);
					it->convexVolume->computeVolumes();
					it->convexVolume->setSmallestDistanceAllowed(w);
					it->convexVolume->setSmallestDistanceEnabled(ww);
					App::ct->undoBufferContainer->announceChangeStart(); // ************************** UNDO thingy **************************
				}
			}
			App::ct->undoBufferContainer->announceChangeEnd(); // ************************** UNDO thingy **************************
		}
		if (mill!=NULL)
		{
			CMill* last=(CMill*)App::ct->objCont->getLastSelection();
			for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
			{
				CMill* it=App::ct->objCont->getMill(App::ct->objCont->getSelID(i));
				if (it!=NULL)
				{
					it->convexVolume->disableVolumeComputation(true);
					// Volume parameters:
					it->setMillType(last->getMillType());
					it->convexVolume->setOffset(last->convexVolume->getOffset());
					it->convexVolume->setRange(last->convexVolume->getRange());
					it->convexVolume->setXSize(last->convexVolume->getXSize());
					it->convexVolume->setYSize(last->convexVolume->getYSize());
					it->convexVolume->setXSizeFar(last->convexVolume->getXSizeFar());
					it->convexVolume->setYSizeFar(last->convexVolume->getYSizeFar());
					it->convexVolume->setRadius(last->convexVolume->getRadius());
					it->convexVolume->setRadiusFar(last->convexVolume->getRadiusFar());
					it->convexVolume->setAngle(last->convexVolume->getAngle());
					it->convexVolume->setFaceNumber(last->convexVolume->getFaceNumber());
					it->convexVolume->setFaceNumberFar(last->convexVolume->getFaceNumberFar());
					it->convexVolume->setSubdivisionsFar(last->convexVolume->getSubdivisionsFar());
					// We have to recompute the planes:
					it->convexVolume->disableVolumeComputation(false);
					it->convexVolume->computeVolumes();
					App::ct->undoBufferContainer->announceChangeStart(); // ************************** UNDO thingy **************************
				}
			}
			App::ct->undoBufferContainer->announceChangeEnd(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}
