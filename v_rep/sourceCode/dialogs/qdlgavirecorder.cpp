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
#include "qdlgavirecorder.h"
#include "ui_qdlgavirecorder.h"
#include "IloIlo.h"
#include "Tt.h"
#include "VFileDialog.h"
#include "v_repStrings.h"
#include "VVarious.h"
#include "App.h"

CQDlgAviRecorder::CQDlgAviRecorder(QWidget *parent) :
	  CDlgEx(parent),
	  ui(new Ui::CQDlgAviRecorder)
{
    ui->setupUi(this);
	inRefreshRoutine=false;
}

CQDlgAviRecorder::~CQDlgAviRecorder()
{
    delete ui;
}

void CQDlgAviRecorder::refresh()
{
	inRefreshRoutine=true;
	bool manualStarted=(App::mainWindow->simulationRecorder->getManualStart()&&App::mainWindow->simulationRecorder->getIsRecording());
	ui->recordNow->setEnabled(((!App::mainWindow->simulationRecorder->getIsRecording())&&(!App::mainWindow->simulationRecorder->getRecorderEnabled()))||manualStarted);
	if (!manualStarted)
		ui->recordNow->setText(IDS_RECORD_NOW);
	else
		ui->recordNow->setText(IDS_STOP_RECORDING);

	ui->launchAtSimulationStart->setEnabled(App::ct->simulation->isSimulationStopped()&&(!App::mainWindow->simulationRecorder->getIsRecording()));
	ui->launchAtSimulationStart->setChecked(App::mainWindow->simulationRecorder->getRecorderEnabled());

	ui->showCursor->setEnabled(!App::mainWindow->simulationRecorder->getIsRecording());
	ui->showCursor->setChecked(App::mainWindow->simulationRecorder->getShowCursor());

	ui->showButtons->setEnabled((!App::mainWindow->simulationRecorder->getIsRecording())&&App::mainWindow->simulationRecorder->getShowCursor());
	ui->showButtons->setChecked(App::mainWindow->simulationRecorder->getShowButtonStates());

	ui->selectLocation->setEnabled(!App::mainWindow->simulationRecorder->getIsRecording());
	ui->displayedFramesVsRecordedFrame->setEnabled((!App::mainWindow->simulationRecorder->getIsRecording()));
	ui->displayedFramesVsRecordedFrame->setText(tt::FNb(0,App::mainWindow->simulationRecorder->getRecordEveryXRenderedFrame(),false).c_str());

	ui->autoFrameRate->setEnabled(!App::mainWindow->simulationRecorder->getIsRecording());
	ui->autoFrameRate->setChecked(App::mainWindow->simulationRecorder->getAutoFrameRate());
	ui->frameRate->setEnabled((!App::mainWindow->simulationRecorder->getIsRecording())&&(!App::mainWindow->simulationRecorder->getAutoFrameRate()));
	ui->frameRate->setText(tt::FNb(0,App::mainWindow->simulationRecorder->getFrameRate(),false).c_str());

	ui->hideInfos->setEnabled(!App::mainWindow->simulationRecorder->getIsRecording());
	ui->hideInfos->setChecked(App::mainWindow->simulationRecorder->getHideInfoTextAndStatusBar());

	ui->fileLocation->setText(App::mainWindow->simulationRecorder->getFilenameAndPath().c_str());

	ui->qqOutputTypeCombo->clear();
	ui->qqOutputTypeCombo->setEnabled(!App::mainWindow->simulationRecorder->getIsRecording());
	int cnt=0;
	std::string txt(App::mainWindow->simulationRecorder->getEncoderString(cnt));

	while (txt.length()!=0)
	{
		ui->qqOutputTypeCombo->addItem(txt.c_str(),QVariant(cnt));
		cnt++;
		txt=App::mainWindow->simulationRecorder->getEncoderString(cnt);
	}
	ui->qqOutputTypeCombo->setCurrentIndex(App::mainWindow->simulationRecorder->getEncoderIndex());

	int resX,resY;
	App::mainWindow->simulationRecorder->getRecordingSize(resX,resY);
	txt=tt::FNb(0,resX,false);
	txt+="x";
	txt+=tt::FNb(0,resY,false);
	ui->qqResolution->setText(txt.c_str());

	inRefreshRoutine=false;
}

void CQDlgAviRecorder::on_launchAtSimulationStart_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if ((!App::mainWindow->simulationRecorder->getIsRecording())&&App::ct->simulation->isSimulationStopped() )
			App::mainWindow->simulationRecorder->setRecorderEnabled(!App::mainWindow->simulationRecorder->getRecorderEnabled());
		refresh();
	}
}

void CQDlgAviRecorder::on_recordNow_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::mainWindow->simulationRecorder->getIsRecording())
		{
			App::mainWindow->simulationRecorder->setRecorderEnabled(true);
			App::mainWindow->simulationRecorder->startRecording(true);
		}
		else
		{
			if (App::mainWindow->simulationRecorder->getManualStart())
				App::mainWindow->simulationRecorder->stopRecording(true);
		}
		refresh();
	}
}

void CQDlgAviRecorder::on_showCursor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::mainWindow->simulationRecorder->getIsRecording())
			App::mainWindow->simulationRecorder->setShowCursor(!App::mainWindow->simulationRecorder->getShowCursor());
		refresh();
	}
}

void CQDlgAviRecorder::on_showButtons_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::mainWindow->simulationRecorder->getIsRecording())
			App::mainWindow->simulationRecorder->setShowButtonStates(!App::mainWindow->simulationRecorder->getShowButtonStates());
		refresh();
	}
}

void CQDlgAviRecorder::on_hideInfos_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::mainWindow->simulationRecorder->getIsRecording())
			App::mainWindow->simulationRecorder->setHideInfoTextAndStatusBar(!App::mainWindow->simulationRecorder->getHideInfoTextAndStatusBar());
		refresh();
	}
}

void CQDlgAviRecorder::on_selectLocation_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::mainWindow->simulationRecorder->getIsRecording())
		{
			std::string initPath=App::directories->getPathFromFull(App::mainWindow->simulationRecorder->getFilenameAndPath());
			std::string filenameAndPath=VFileDialog::getSaveFileName(App::mainWindow,0,strTranslate(IDSN_AVI_FILE_LOCATION),initPath,App::mainWindow->simulationRecorder->getFilenameAndPath(),false,"Various","*");
			if (filenameAndPath.length()!=0)
			{
				// Make sure we don't include the extension
				std::string _path=VVarious::splitPath_path(filenameAndPath);
				std::string _name=VVarious::splitPath_fileBase(filenameAndPath);
				filenameAndPath=_path+"/"+_name;
				App::mainWindow->simulationRecorder->setFilenameAndPath(filenameAndPath.c_str());
			}
		}
		refresh();
	}
}

void CQDlgAviRecorder::on_displayedFramesVsRecordedFrame_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::mainWindow->simulationRecorder->getIsRecording())
		{
			int newVal;
			bool ok;
			newVal=ui->displayedFramesVsRecordedFrame->text().toInt(&ok);
			if (ok)
				App::mainWindow->simulationRecorder->setRecordEveryXRenderedFrame(newVal);
		}
		refresh();
	}
}

void CQDlgAviRecorder::on_frameRate_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::mainWindow->simulationRecorder->getIsRecording())
		{
			int newVal;
			bool ok;
			newVal=ui->frameRate->text().toInt(&ok);
			if (ok)
				App::mainWindow->simulationRecorder->setFrameRate(newVal);
		}
		refresh();
	}
}

void CQDlgAviRecorder::on_qqOutputTypeCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inRefreshRoutine)
		{
			App::mainWindow->simulationRecorder->setEncoderIndex(index);
			refresh();
		}
	}
}

void CQDlgAviRecorder::on_autoFrameRate_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::mainWindow->simulationRecorder->getIsRecording())
			App::mainWindow->simulationRecorder->setAutoFrameRate(!App::mainWindow->simulationRecorder->getAutoFrameRate());
		refresh();
	}
}
