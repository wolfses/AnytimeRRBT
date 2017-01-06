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

// Not very elegant. Needs refactoring!!

#include "vrepPrecompiledHeader.h"
#include "qdlgcolor.h"
#include "ui_qdlgcolor.h"
#include "GV.h"
#include "Tt.h"
#include "App.h"

CQDlgColor::CQDlgColor(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgColor)
{
    ui->setupUi(this);
	inRefreshPart=false;
	colors=NULL;
	_identification[0]=-1;
	_identification[1]=-1;
	_identification[2]=-1;
	_identification[3]=-1;
}

CQDlgColor::~CQDlgColor()
{
    delete ui;
}

void CQDlgColor::refresh()
{
	inRefreshPart=true;
	if (colors!=NULL)
	{
		redState=colors[0];
		greenState=colors[1];
		blueState=colors[2];
		computeHSLValues();
	}

	QImage img(70,240,QImage::Format_RGB32);
	QRgb value=qRgb(int(redState*255.5f),int(greenState*255.5f),int(blueState*255.5f));
	for (int i=0;i<240;i++)
	{
		for (int j=0;j<70;j++)
			img.setPixel(j,i,value);
	}
	QPixmap pimg;
	pimg.convertFromImage(img);
	ui->qqImage->setPixmap(pimg);

	ui->qqRedValue->setText(tt::FNb(0,redState,2,false).c_str());
	ui->qqGreenValue->setText(tt::FNb(0,greenState,2,false).c_str());
	ui->qqBlueValue->setText(tt::FNb(0,blueState,2,false).c_str());
	ui->qqHueValue->setText(tt::FNb(0,hueState,2,false).c_str());
	ui->qqSaturationValue->setText(tt::FNb(0,saturationState,2,false).c_str());
	ui->qqLuminosityValue->setText(tt::FNb(0,luminosityState,2,false).c_str());

	ui->qqRedSlider->setSliderPosition(int(redState*100.5f));
	ui->qqGreenSlider->setSliderPosition(int(greenState*100.5f));
	ui->qqBlueSlider->setSliderPosition(int(blueState*100.5f));
	ui->qqHueSlider->setSliderPosition(int(hueState*100.5f));
	ui->qqSaturationSlider->setSliderPosition(int(saturationState*100.5f));
	ui->qqLuminositySlider->setSliderPosition(int(luminosityState*100.5f));

	inRefreshPart=false;
}

bool CQDlgColor::needsDestruction()
{
	if (!isLinkedDataValid())
		return(true);
	return(CDlgEx::needsDestruction());
}

void CQDlgColor::initializationEvent()
{
	CDlgEx::initializationEvent();
	computeHSLValues();
	refresh();
}

void CQDlgColor::display(int identification[4],float* theColors,QWidget* theParentWindow)
{
	if (App::mainWindow==NULL)
		return;
	App::mainWindow->dlgCont->close(MATERIAL_DLG);
	App::mainWindow->dlgCont->close(COLOR_DLG);
	if (App::mainWindow->dlgCont->openOrBringToFront(COLOR_DLG))
	{
		CQDlgColor* it=(CQDlgColor*)App::mainWindow->dlgCont->getDialog(COLOR_DLG);
		if (it!=NULL)
			it->initialize(theColors,identification);
	}
}

void CQDlgColor::displayModal(int identification[4],float* theColors,QWidget* theParentWindow)
{
	if (App::mainWindow==NULL)
		return;
	App::mainWindow->dlgCont->close(MATERIAL_DLG);
	App::mainWindow->dlgCont->close(COLOR_DLG);

	CQDlgColor it(theParentWindow);
	it.initialize(theColors,identification);
	it.makeDialogModal();
}

void CQDlgColor::initialize(float* theColors,int identification[4])
{
	redState=theColors[0];
	greenState=theColors[1];
	blueState=theColors[2];
	computeHSLValues();
	colors=theColors;
	_identification[0]=identification[0];
	_identification[1]=identification[1];
	_identification[2]=identification[2];
	_identification[3]=identification[3];
	refresh();
}

bool CQDlgColor::isLinkedDataValid()
{
	if (!CLifeControl::isAlive(_identification[0]))
		return(false);
	if (_identification[1]==0)
	{ // we have a 3DObject here
		if (App::ct->objCont!=NULL)
		{
			if (App::ct->objCont->getLastSelectionID()!=_identification[2])
				return(false);
		}
	}
	return(true);
}

void CQDlgColor::computeRGBValues()
{
	float hsl[3]={hueState,saturationState,luminosityState};
	float rgb[3];
	tt::hslToRgb(hsl,rgb);
	redState=rgb[0];
	greenState=rgb[1];
	blueState=rgb[2];
}

void CQDlgColor::computeHSLValues()
{
	float rgb[3]={redState,greenState,blueState};
	float hsl[3];
	tt::rgbToHsl(rgb,hsl);
	hueState=hsl[0];
	saturationState=hsl[1];
	luminosityState=hsl[2];
}

bool CQDlgColor::doesInstanceSwitchRequireDestruction()
{
	return(true);
}

void CQDlgColor::cancelEvent() // this was empty before VDialog wrap thing
{ // We just hide the dialog and destroy it at next rendering pass
	_identification[0]=-1;
	if (isModal()) // this condition and next line on 25/1/2013: on Linux the dlg couldn't be closed!
		defaultModalDialogEndRoutine(false);
	else
		CDlgEx::cancelEvent();
}

void CQDlgColor::on_qqRedValue_editingFinished()
{
	if (inRefreshPart)
		return;
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			bool ok;
			float newVal=ui->qqRedValue->text().toFloat(&ok);
			if (ok)
			{
				tt::limitValue(0.0f,1.0f,newVal);
				redState=newVal;
				if (colors!=NULL)
					colors[0]=newVal;
			}
			computeHSLValues();
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgColor::on_qqGreenValue_editingFinished()
{
	if (inRefreshPart)
		return;
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			bool ok;
			float newVal=ui->qqGreenValue->text().toFloat(&ok);
			if (ok)
			{
				tt::limitValue(0.0f,1.0f,newVal);
				greenState=newVal;
				if (colors!=NULL)
					colors[1]=newVal;
			}
			computeHSLValues();
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgColor::on_qqBlueValue_editingFinished()
{
	if (inRefreshPart)
		return;
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			bool ok;
			float newVal=ui->qqBlueValue->text().toFloat(&ok);
			if (ok)
			{
				tt::limitValue(0.0f,1.0f,newVal);
				blueState=newVal;
				if (colors!=NULL)
					colors[2]=newVal;
			}
			computeHSLValues();
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgColor::on_qqHueValue_editingFinished()
{
	if (inRefreshPart)
		return;
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			bool ok;
			float newVal=ui->qqHueValue->text().toFloat(&ok);
			if (ok)
			{
				tt::limitValue(0.0f,1.0f,newVal);
				hueState=newVal;
			}
			computeRGBValues();
			if (colors!=NULL)
			{
				colors[0]=redState;
				colors[1]=greenState;
				colors[2]=blueState;
			}
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgColor::on_qqSaturationValue_editingFinished()
{
	if (inRefreshPart)
		return;
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			bool ok;
			float newVal=ui->qqSaturationValue->text().toFloat(&ok);
			if (ok)
			{
				tt::limitValue(0.0f,1.0f,newVal);
				saturationState=newVal;
			}
			computeRGBValues();
			if (colors!=NULL)
			{
				colors[0]=redState;
				colors[1]=greenState;
				colors[2]=blueState;
			}
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgColor::on_qqLuminosityValue_editingFinished()
{
	if (inRefreshPart)
		return;
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			bool ok;
			float newVal=ui->qqLuminosityValue->text().toFloat(&ok);
			if (ok)
			{
				tt::limitValue(0.0f,1.0f,newVal);
				luminosityState=newVal;
			}
			computeRGBValues();
			if (colors!=NULL)
			{
				colors[0]=redState;
				colors[1]=greenState;
				colors[2]=blueState;
			}
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgColor::on_qqRedSlider_sliderMoved(int position)
{
	if (inRefreshPart)
		return;
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			redState=float(position)/100.0f;
			if (colors!=NULL)
				colors[0]=redState;
			computeHSLValues();
			App::ct->undoBufferContainer->announceChangeGradual(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgColor::on_qqGreenSlider_sliderMoved(int position)
{
	if (inRefreshPart)
		return;
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			greenState=float(position)/100.0f;
			if (colors!=NULL)
				colors[1]=greenState;
			computeHSLValues();
			App::ct->undoBufferContainer->announceChangeGradual(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgColor::on_qqBlueSlider_sliderMoved(int position)
{
	if (inRefreshPart)
		return;
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			blueState=float(position)/100.0f;
			if (colors!=NULL)
				colors[2]=blueState;
			computeHSLValues();
			App::ct->undoBufferContainer->announceChangeGradual(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgColor::on_qqHueSlider_sliderMoved(int position)
{
	if (inRefreshPart)
		return;
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			hueState=float(position)/100.0f;
			computeRGBValues();
			if (colors!=NULL)
			{
				colors[0]=redState;
				colors[1]=greenState;
				colors[2]=blueState;
			}
			App::ct->undoBufferContainer->announceChangeGradual(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgColor::on_qqSaturationSlider_sliderMoved(int position)
{
	if (inRefreshPart)
		return;
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			saturationState=float(position)/100.0f;
			computeRGBValues();
			if (colors!=NULL)
			{
				colors[0]=redState;
				colors[1]=greenState;
				colors[2]=blueState;
			}
			App::ct->undoBufferContainer->announceChangeGradual(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgColor::on_qqLuminositySlider_sliderMoved(int position)
{
	if (inRefreshPart)
		return;
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (isLinkedDataValid())
		{
			luminosityState=float(position)/100.0f;
			computeRGBValues();
			if (colors!=NULL)
			{
				colors[0]=redState;
				colors[1]=greenState;
				colors[2]=blueState;
			}
			App::ct->undoBufferContainer->announceChangeGradual(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}
