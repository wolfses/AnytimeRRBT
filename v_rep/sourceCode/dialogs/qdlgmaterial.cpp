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
#include "qdlgmaterial.h"
#include "ui_qdlgmaterial.h"
#include "GV.h"
#include "Tt.h"
#include "App.h"
#include "qdlgcolorpulsation.h"

CQDlgMaterial::CQDlgMaterial(QWidget *parent) :
	CDlgEx(parent),
	ui(new Ui::CQDlgMaterial)
{
    ui->setupUi(this);
	ambient=NULL;
	diffuse=NULL;
	specular=NULL;
	emissive=NULL;
	transparencyFactor=NULL;
	shininess=NULL;
	transparency=NULL;
	flashing=NULL;
	simulationTimeFlashing=NULL;
	flashingFrequency=NULL;
	flashingPhase=NULL;
	flashingRatio=NULL;
	color=NULL;
	colorName=NULL;
	_identification[0]=-1;
	_identification[1]=-1;
	_identification[2]=-1;
	_identification[3]=-1;
	isModal=false;
	if (App::mainWindow!=NULL)
		App::mainWindow->dlgCont->close(COLOR_DLG);
}

CQDlgMaterial::~CQDlgMaterial()
{
    delete ui;
	if (App::mainWindow!=NULL)
		App::mainWindow->dlgCont->close(COLOR_DLG);
}

void CQDlgMaterial::refresh()
{
	ui->qqAmbientAdjust->setEnabled(ambient!=NULL);
	ui->qqDiffuseAdjust->setEnabled(diffuse!=NULL);
	ui->qqSpecularAdjust->setEnabled(specular!=NULL);
	ui->qqEmissiveAdjust->setEnabled(emissive!=NULL);
	ui->qqPulsationAdjust->setEnabled(flashing!=NULL);
	ui->qqShininess->setEnabled(shininess!=NULL);

	if (shininess!=NULL)
		ui->qqShininess->setText(tt::FNb(shininess[0]).c_str());
	else
		ui->qqShininess->setText("");
	ui->qqOpacityEnable->setEnabled(transparency!=NULL);
	if (transparency!=NULL)
	{
		ui->qqOpacityEnable->setChecked(transparency[0]);
		ui->qqOpacity->setEnabled(transparency[0]);
		ui->qqOpacity->setText(tt::FNb(0,transparencyFactor[0],2,false).c_str());
	}
	else
	{
		ui->qqOpacityEnable->setChecked(0);
		ui->qqOpacity->setEnabled(false);
		ui->qqOpacity->setText("");
	}

	ui->qqColorName->setEnabled(colorName!=NULL);
	if (colorName!=NULL)
		ui->qqColorName->setText(colorName->c_str());
	else
		ui->qqColorName->setText("");
}

bool CQDlgMaterial::needsDestruction()
{
	if (!isLinkedDataValid())
		return(true);
	return(CDlgEx::needsDestruction());
}

void CQDlgMaterial::display(int identification[4],QWidget* theParentWindow,float ambientP[3],float diffuseP[3],float specularP[3],float emissiveP[3],bool* flashingP,int* shininessP,bool* transparencyP,float* transparencyFactorP,std::string* theColorName)
{	// Any value of ambient, diffuse, etc. can be NULL.
	// If a value is NULL, it can't be adjusted.
	if (App::mainWindow==NULL)
		return;
	App::mainWindow->dlgCont->close(MATERIAL_DLG);
	App::mainWindow->dlgCont->close(COLOR_DLG);
	if (App::mainWindow->dlgCont->openOrBringToFront(MATERIAL_DLG))
	{
		CQDlgMaterial* mat=(CQDlgMaterial*)App::mainWindow->dlgCont->getDialog(MATERIAL_DLG);
		if (mat!=NULL)
			mat->_initialize(identification,ambientP,diffuseP,specularP,emissiveP,flashingP,shininessP,transparencyP,transparencyFactorP,NULL,theColorName);
	}
}

void CQDlgMaterial::displayModal(int identification[4],QWidget* theParentWindow,float ambientP[3],float diffuseP[3],float specularP[3],float emissiveP[3],bool* flashingP,int* shininessP,bool* transparencyP,float* transparencyFactorP,std::string* theColorName)
{	// Any value of ambient, diffuse, etc. can be NULL.
	// If a value is NULL, it can't be adjusted.
	if (App::mainWindow==NULL)
		return;
	App::mainWindow->dlgCont->close(MATERIAL_DLG);
	App::mainWindow->dlgCont->close(COLOR_DLG);

	CQDlgMaterial it(theParentWindow);
	it._initialize(identification,ambientP,diffuseP,specularP,emissiveP,flashingP,shininessP,transparencyP,transparencyFactorP,NULL,theColorName);
	it.isModal=true;
	it.makeDialogModal();
}

void CQDlgMaterial::_initialize(int identification[4],float ambientP[3],float diffuseP[3],float specularP[3],float emissiveP[3],bool* flashingP,int* shininessP,bool* transparencyP,float* transparencyFactorP,bool* flashSimTimeP,std::string* theColorName)
{
	_identification[0]=identification[0];
	_identification[1]=identification[1];
	_identification[2]=identification[2];
	_identification[3]=identification[3];
	ambient=ambientP;
	diffuse=diffuseP;
	specular=specularP;
	emissive=emissiveP;
	flashing=NULL;
	simulationTimeFlashing=flashSimTimeP;
	flashingFrequency=NULL;
	flashingPhase=NULL;
	flashingRatio=NULL;
	shininess=shininessP;
	colorName=theColorName;
	if ((transparencyP==NULL)||(transparencyFactorP==NULL))
	{
		transparency=NULL;
		transparencyFactor=NULL;
	}
	else
	{
		transparency=transparencyP;
		transparencyFactor=transparencyFactorP;
	}
	refresh();
}

void CQDlgMaterial::_initialize(int identification[4],CVisualParam* it,bool amb,bool diff,bool spec,
					bool emiss,bool shin,bool transp,bool allowFlashing,bool colorNameEnabled)
{ // All boolean value are true by default! If a value is true, it can be adjusted
	_identification[0]=identification[0];
	_identification[1]=identification[1];
	_identification[2]=identification[2];
	_identification[3]=identification[3];
	ambient=it->colors+0;
	diffuse=it->colors+3;
	specular=it->colors+6;
	emissive=it->colors+9;
	if (allowFlashing)
	{
		flashing=&it->flash;
		simulationTimeFlashing=&it->useSimulationTime;
		flashingFrequency=&it->flashFrequency;
		flashingPhase=&it->flashPhase;
		flashingRatio=&it->flashRatio;
	}
	else
	{
		flashing=NULL;
		simulationTimeFlashing=NULL;
		flashingFrequency=NULL;
		flashingPhase=NULL;
		flashingRatio=NULL;
	}
	shininess=&it->shininess;
	transparency=&it->translucid;
	transparencyFactor=&it->transparencyFactor;
	colorName=&it->colorName;
	if (!amb)
		ambient=NULL;
	if (!diff)
		diffuse=NULL;
	if (!spec)
		specular=NULL;
	if (!emiss)
		emissive=NULL;
	if (!shin)
		shininess=NULL;
	if (!transp)
	{
		transparency=NULL;
		transparencyFactor=NULL;
	}
	if (!colorNameEnabled)
		colorName=NULL;
	refresh();
}

bool CQDlgMaterial::isLinkedDataValid()
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

void CQDlgMaterial::display(int identification[4],CVisualParam* it,QWidget* theParentWindow,bool amb,bool diff,bool spec,
					bool emiss,bool shin,bool transp,bool flashing,bool colorNameEnabled)
{ // All boolean value are true by default! If a value is true, it can be adjusted
	if (App::mainWindow==NULL)
		return;
	App::mainWindow->dlgCont->close(MATERIAL_DLG);
	App::mainWindow->dlgCont->close(COLOR_DLG);
	if (App::mainWindow->dlgCont->openOrBringToFront(MATERIAL_DLG))
	{
		CQDlgMaterial* mat=(CQDlgMaterial*)App::mainWindow->dlgCont->getDialog(MATERIAL_DLG);
		if (mat!=NULL)
			mat->_initialize(identification,it,amb,diff,spec,emiss,shin,transp,flashing,colorNameEnabled);
	}
}

void CQDlgMaterial::colorAdjust(float* col)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (col!=NULL)
		{
			if (!isModal)
			{
				if (isLinkedDataValid())
				{
					if (App::mainWindow->dlgCont->openOrBringToFront(COLOR_DLG))
					{
						CQDlgColor* it=(CQDlgColor*)App::mainWindow->dlgCont->getDialog(COLOR_DLG);
						if (it!=NULL)
							it->initialize(col,_identification);
					}
				}
			}
			else
			{
				CQDlgColor it(this);
				it.initialize(col,_identification);
				it.makeDialogModal();
			}
		}
	}
}

void CQDlgMaterial::cancelEvent()
{ // We just hide the dialog and destroy it at next rendering pass
	ambient=NULL;
	diffuse=NULL;
	specular=NULL;
	emissive=NULL;
	flashing=NULL;
	transparencyFactor=NULL;
	shininess=NULL;
	transparency=NULL;
	if (((QDialog*)this)->isModal()) // this condition and next line on 31/3/2013: on Linux the dlg couldn't be closed!
		defaultModalDialogEndRoutine(false);
	else
		CDlgEx::cancelEvent();
}

bool CQDlgMaterial::doesInstanceSwitchRequireDestruction()
{
	return(true);
}

void CQDlgMaterial::on_qqAmbientAdjust_clicked()
{
	colorAdjust(ambient);
}

void CQDlgMaterial::on_qqDiffuseAdjust_clicked()
{
	colorAdjust(diffuse);
}

void CQDlgMaterial::on_qqSpecularAdjust_clicked()
{
	colorAdjust(specular);
}

void CQDlgMaterial::on_qqEmissiveAdjust_clicked()
{
	colorAdjust(emissive);
}

void CQDlgMaterial::on_qqPulsationAdjust_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		if (flashing!=NULL)
		{
			CQDlgColorPulsation theDialog(this);
			theDialog.pulsationEnabled=flashing[0];
			theDialog.pulsationRealTime=!simulationTimeFlashing[0];
			theDialog.pulsationFrequency=flashingFrequency[0];
			theDialog.pulsationPhase=flashingPhase[0];
			theDialog.pulsationRatio=flashingRatio[0];
			theDialog.refresh();
			if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
			{
				flashing[0]=theDialog.pulsationEnabled;
				simulationTimeFlashing[0]=!theDialog.pulsationRealTime;
				flashingFrequency[0]=theDialog.pulsationFrequency;
				flashingPhase[0]=theDialog.pulsationPhase;
				flashingRatio[0]=theDialog.pulsationRatio;
			}
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgMaterial::on_qqShininess_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!isLinkedDataValid())
			return;
		if (shininess==NULL)
			return;
		bool ok;
		float newVal=ui->qqShininess->text().toFloat(&ok);
		if (ok)
		{
			shininess[0]=tt::getLimitedInt(0,128,newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgMaterial::on_qqOpacityEnable_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!isLinkedDataValid())
			return;
		if (transparency==NULL)
			return;
		transparency[0]=!transparency[0];
		if (_identification[1]==0)
		{ // we have a 3D object here
			CShape* it=App::ct->objCont->getShape(_identification[2]);
			if (it!=NULL)
			{ // we have a shape here
				// Now let's check if the outside and inside faces are transparent:
				it->actualizeContainsTransparentComponent();
			}
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgMaterial::on_qqOpacity_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!isLinkedDataValid())
			return;
		if (transparencyFactor==NULL)
			return;
		bool ok;
		float newVal=ui->qqOpacity->text().toFloat(&ok);
		if (ok)
		{
			transparencyFactor[0]=tt::getLimitedFloat(0.0f,1.0f,newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgMaterial::on_qqColorName_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!isLinkedDataValid())
			return;
		if (colorName==NULL)
			return;
		colorName[0]=ui->qqColorName->text().toStdString();
		tt::removeIllegalCharacters(colorName[0],false);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}
