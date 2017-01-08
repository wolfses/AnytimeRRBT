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
#include "qdlgselection.h"
#include "ui_qdlgselection.h"
#include "App.h"
#include "v_repStrings.h"
#include <boost/lexical_cast.hpp>

CQDlgSelection::CQDlgSelection(QWidget *parent) :
	  CDlgEx(parent),
	  ui(new Ui::CQDlgSelection)
{
    ui->setupUi(this);
}

CQDlgSelection::~CQDlgSelection()
{
    delete ui;
}

void CQDlgSelection::refresh()
{
//	bool full=(refreshDegree>1);
	std::string tmp;
	tmp=std::string(IDS_SIMPLE_SHAPES)+": "+boost::lexical_cast<std::string>(App::ct->objCont->getSimpleShapeNumberInSelection())+"/"+
		boost::lexical_cast<std::string>(App::ct->objCont->getSimpleShapeNumberInSelection(&App::ct->objCont->objectList));
	ui->simpleShapesButton->setText(tmp.c_str());
	tmp=std::string(IDS_GROUPED_SHAPES)+": "+boost::lexical_cast<std::string>(App::ct->objCont->getGroupingNumberInSelection())+"/"+
		boost::lexical_cast<std::string>(App::ct->objCont->getGroupingNumberInSelection(&App::ct->objCont->objectList));
	ui->groupedShapesButton->setText(tmp.c_str());
	tmp=std::string(IDS_JOINTS)+": "+boost::lexical_cast<std::string>(App::ct->objCont->getJointNumberInSelection())+"/"+
		boost::lexical_cast<std::string>(App::ct->objCont->getJointNumberInSelection(&App::ct->objCont->objectList));
	ui->jointsButton->setText(tmp.c_str());
	tmp=std::string(IDS_CAMERAS)+": "+boost::lexical_cast<std::string>(App::ct->objCont->getCameraNumberInSelection())+"/"+
		boost::lexical_cast<std::string>(App::ct->objCont->getCameraNumberInSelection(&App::ct->objCont->objectList));
	ui->camerasButton->setText(tmp.c_str());
	tmp=std::string(IDS_LIGHTS)+": "+boost::lexical_cast<std::string>(App::ct->objCont->getLightNumberInSelection())+"/"+
		boost::lexical_cast<std::string>(App::ct->objCont->getLightNumberInSelection(&App::ct->objCont->objectList));
	ui->lightsButton->setText(tmp.c_str());
	tmp=std::string(IDS_PROXSENSORS)+": "+boost::lexical_cast<std::string>(App::ct->objCont->getProxSensorNumberInSelection())+"/"+
		boost::lexical_cast<std::string>(App::ct->objCont->getProxSensorNumberInSelection(&App::ct->objCont->objectList));
	ui->proximitySensorsButton->setText(tmp.c_str());
	tmp=std::string(IDS_VISION_SENSORS)+": "+boost::lexical_cast<std::string>(App::ct->objCont->getVisionSensorNumberInSelection())+"/"+
		boost::lexical_cast<std::string>(App::ct->objCont->getVisionSensorNumberInSelection(&App::ct->objCont->objectList));
	ui->renderingSensorsButton->setText(tmp.c_str());
	tmp=std::string(IDS_FORCE_SENSORS)+": "+boost::lexical_cast<std::string>(App::ct->objCont->getForceSensorNumberInSelection())+"/"+
		boost::lexical_cast<std::string>(App::ct->objCont->getForceSensorNumberInSelection(&App::ct->objCont->objectList));
	ui->forceSensorsButton->setText(tmp.c_str());
	tmp=std::string(IDS_DUMMIES)+": "+boost::lexical_cast<std::string>(App::ct->objCont->getDummyNumberInSelection())+"/"+
		boost::lexical_cast<std::string>(App::ct->objCont->getDummyNumberInSelection(&App::ct->objCont->objectList));
	ui->dummiesButton->setText(tmp.c_str());
	tmp=std::string(IDS_GRAPHS)+": "+boost::lexical_cast<std::string>(App::ct->objCont->getGraphNumberInSelection())+"/"+
		boost::lexical_cast<std::string>(App::ct->objCont->getGraphNumberInSelection(&App::ct->objCont->objectList));
	ui->graphsButton->setText(tmp.c_str());
	tmp=std::string(IDS_MILLS)+": "+boost::lexical_cast<std::string>(App::ct->objCont->getMillNumberInSelection())+"/"+
		boost::lexical_cast<std::string>(App::ct->objCont->getMillNumberInSelection(&App::ct->objCont->objectList));
	ui->millsButton->setText(tmp.c_str());
	tmp=std::string(IDS_PATHS)+": "+boost::lexical_cast<std::string>(App::ct->objCont->getPathNumberInSelection())+"/"+
		boost::lexical_cast<std::string>(App::ct->objCont->getPathNumberInSelection(&App::ct->objCont->objectList));
	ui->pathsButton->setText(tmp.c_str());
	tmp=std::string(IDS_MIRRORS)+": "+boost::lexical_cast<std::string>(App::ct->objCont->getMirrorNumberInSelection())+"/"+
		boost::lexical_cast<std::string>(App::ct->objCont->getMirrorNumberInSelection(&App::ct->objCont->objectList));
	ui->mirrorsButton->setText(tmp.c_str());

	tmp.clear();

	bool objEnabled=App::ct->objCont->getEditModeType()==NO_EDIT_MODE;

	ui->clearSelectionButton->setEnabled(objEnabled);
	ui->invertSelectionButton->setEnabled(objEnabled);

	ui->simpleShapesButton->setEnabled(objEnabled);
	ui->jointsButton->setEnabled(objEnabled);
	ui->groupedShapesButton->setEnabled(objEnabled);
	ui->camerasButton->setEnabled(objEnabled);
	ui->lightsButton->setEnabled(objEnabled);
	ui->proximitySensorsButton->setEnabled(objEnabled);
	ui->renderingSensorsButton->setEnabled(objEnabled);
	ui->forceSensorsButton->setEnabled(objEnabled);
	ui->millsButton->setEnabled(objEnabled);
	ui->dummiesButton->setEnabled(objEnabled);
	ui->graphsButton->setEnabled(objEnabled);
	ui->pathsButton->setEnabled(objEnabled);
	ui->mirrorsButton->setEnabled(objEnabled);


	if ((App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
	{
		tmp=std::string(IDS_TOTAL_OBJECTS)+": "+boost::lexical_cast<std::string>(App::ct->objCont->getSelSize())+"/"+
			boost::lexical_cast<std::string>(App::ct->objCont->objectList.size());
	}
	ui->selectionInfo->setText(tmp.c_str());
	refreshDegree=0;
}

void CQDlgSelection::processIt()
{
	for (int i=0;i<int(list.size());i++)
		App::ct->objCont->addObjectToSelection(list[i]);
	refresh();
}

void CQDlgSelection::on_simpleShapesButton_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		list.clear();
		for (int i=0;i<int(App::ct->objCont->shapeList.size());i++)
		{
			CShape* it=App::ct->objCont->getShape(App::ct->objCont->shapeList[i]);
			if (!it->isGrouping())
				list.push_back(it->getID());
		}
		processIt();
	}
}

void CQDlgSelection::on_groupedShapesButton_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		list.clear();
		for (int i=0;i<int(App::ct->objCont->shapeList.size());i++)
		{
			CShape* it=App::ct->objCont->getShape(App::ct->objCont->shapeList[i]);
			if (it->isGrouping())
				list.push_back(it->getID());
		}
		processIt();
	}
}

void CQDlgSelection::on_dummiesButton_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		list.clear();
		for (int i=0;i<int(App::ct->objCont->dummyList.size());i++)
			list.push_back(App::ct->objCont->dummyList[i]);
		processIt();
	}
}

void CQDlgSelection::on_millsButton_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		list.clear();
		for (int i=0;i<int(App::ct->objCont->millList.size());i++)
			list.push_back(App::ct->objCont->millList[i]);
		processIt();
	}
}

void CQDlgSelection::on_camerasButton_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		list.clear();
		for (int i=0;i<int(App::ct->objCont->cameraList.size());i++)
			list.push_back(App::ct->objCont->cameraList[i]);
		processIt();
	}
}

void CQDlgSelection::on_lightsButton_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		list.clear();
		for (int i=0;i<int(App::ct->objCont->lightList.size());i++)
			list.push_back(App::ct->objCont->lightList[i]);
		processIt();
	}
}

void CQDlgSelection::on_proximitySensorsButton_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		list.clear();
		for (int i=0;i<int(App::ct->objCont->proximitySensorList.size());i++)
			list.push_back(App::ct->objCont->proximitySensorList[i]);
		processIt();
	}
}

void CQDlgSelection::on_renderingSensorsButton_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		list.clear();
		for (int i=0;i<int(App::ct->objCont->visionSensorList.size());i++)
			list.push_back(App::ct->objCont->visionSensorList[i]);
		processIt();
	}
}

void CQDlgSelection::on_forceSensorsButton_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		list.clear();
		for (int i=0;i<int(App::ct->objCont->forceSensorList.size());i++)
			list.push_back(App::ct->objCont->forceSensorList[i]);
		processIt();
	}
}

void CQDlgSelection::on_jointsButton_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		list.clear();
		for (int i=0;i<int(App::ct->objCont->jointList.size());i++)
			list.push_back(App::ct->objCont->jointList[i]);
		processIt();
	}
}

void CQDlgSelection::on_graphsButton_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		list.clear();
		for (int i=0;i<int(App::ct->objCont->graphList.size());i++)
			list.push_back(App::ct->objCont->graphList[i]);
		processIt();
	}
}

void CQDlgSelection::on_pathsButton_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		list.clear();
		for (int i=0;i<int(App::ct->objCont->pathList.size());i++)
			list.push_back(App::ct->objCont->pathList[i]);
		processIt();
	}
}

void CQDlgSelection::on_clearSelectionButton_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->objCont->deselectObjects();
		refresh();
	}
}

void CQDlgSelection::on_invertSelectionButton_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if ((App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
		{
			for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
			{
				C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
				App::ct->objCont->xorAddObjectToSelection(it->getID());
			}
		}
		refresh();
	}
}

void CQDlgSelection::on_mirrorsButton_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		list.clear();
		for (int i=0;i<int(App::ct->objCont->mirrorList.size());i++)
			list.push_back(App::ct->objCont->mirrorList[i]);
		processIt();
	}
}
