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
#include "dlgpathvelocityprofile.h"
#include "ui_dlgpathvelocityprofile.h"
#include "GV.h"
#include "Tt.h"
#include "App.h"

CDlgPathVelocityProfile::CDlgPathVelocityProfile(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CDlgPathVelocityProfile)
{
    ui->setupUi(this);
}

CDlgPathVelocityProfile::~CDlgPathVelocityProfile()
{
    delete ui;
}

void CDlgPathVelocityProfile::cancelEvent()
{
//	defaultModalDialogEndRoutine(false);
}

void CDlgPathVelocityProfile::okEvent()
{
//	defaultModalDialogEndRoutine(true);
}

void CDlgPathVelocityProfile::refresh()
{
	if (!App::ct->objCont->isLastSelectionAPath())
		return;
	CPath* it=(CPath*)App::ct->objCont->getLastSelection();
	if (it==NULL)
		return;
	_pathID=it->getID();
	CPathCont* pc=NULL;
	if ((App::ct->objCont->getEditModeType()==PATH_EDIT_MODE)&&(App::ct->objCont->getEditModeObjectID()==it->getID()))
		pc=App::ct->objCont->_editionPath;
	else
		pc=it->pathContainer;
	if (pc==NULL)
		return;
	ui->qqTurningCircleCheck->setChecked(pc->getAttributes()&sim_pathproperty_auto_velocity_profile_translation);
	ui->qqRelativeVelocityCheck->setChecked(pc->getAttributes()&sim_pathproperty_auto_velocity_profile_rotation);
	ui->qqTurningCircleValue->setEnabled(pc->getAttributes()&sim_pathproperty_auto_velocity_profile_translation);
	ui->qqRelativeVelocityValue->setEnabled(pc->getAttributes()&sim_pathproperty_auto_velocity_profile_rotation);

	ui->qqTurningCircleValue->setText(gv::getSizeStr(false,pc->getAvpTurningCircleRadiusForHalfVelocity()*2.0f,0).c_str());
	ui->qqRelativeVelocityValue->setText(tt::FNb(0,pc->getAvpRelativeVelocityAtRotationAxisChange(),2,false).c_str());

	ui->qqEndPointsZero->setChecked(pc->getAttributes()&sim_pathproperty_endpoints_at_zero);

	ui->qqRelativeAcceleration->setText(tt::FNb(0,pc->getAvpRelativeAcceleration(),4,false).c_str());
}

void CDlgPathVelocityProfile::on_qqTurningCircleCheck_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPath* it=App::ct->objCont->getPath(_pathID);
		CPathCont* pc=NULL;
		if (it!=NULL)
		{
			if ((App::ct->objCont->getEditModeType()==PATH_EDIT_MODE)&&(App::ct->objCont->getEditModeObjectID()==it->getID()))
				pc=App::ct->objCont->_editionPath;
			else
				pc=it->pathContainer;
		}
		if (pc!=NULL)
		{
			pc->setAttributes(pc->getAttributes()^sim_pathproperty_auto_velocity_profile_translation);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			refresh();
		}
	}
}

void CDlgPathVelocityProfile::on_qqTurningCircleValue_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPath* it=App::ct->objCont->getPath(_pathID);
		CPathCont* pc=NULL;
		if (it!=NULL)
		{
			if ((App::ct->objCont->getEditModeType()==PATH_EDIT_MODE)&&(App::ct->objCont->getEditModeObjectID()==it->getID()))
				pc=App::ct->objCont->_editionPath;
			else
				pc=it->pathContainer;
		}
		if (pc!=NULL)
		{
			bool ok;
			float newVal=ui->qqTurningCircleValue->text().toFloat(&ok);
			if (ok)
			{
				pc->setAvpTurningCircleRadiusForHalfVelocity(0.5f*newVal*gv::userToMeter);
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			}
			refresh();
		}
	}
}

void CDlgPathVelocityProfile::on_qqRelativeVelocityCheck_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPath* it=App::ct->objCont->getPath(_pathID);
		CPathCont* pc=NULL;
		if (it!=NULL)
		{
			if ((App::ct->objCont->getEditModeType()==PATH_EDIT_MODE)&&(App::ct->objCont->getEditModeObjectID()==it->getID()))
				pc=App::ct->objCont->_editionPath;
			else
				pc=it->pathContainer;
		}
		if (pc!=NULL)
		{
			pc->setAttributes(pc->getAttributes()^sim_pathproperty_auto_velocity_profile_rotation);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			refresh();
		}
	}
}

void CDlgPathVelocityProfile::on_qqRelativeVelocityValue_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPath* it=App::ct->objCont->getPath(_pathID);
		CPathCont* pc=NULL;
		if (it!=NULL)
		{
			if ((App::ct->objCont->getEditModeType()==PATH_EDIT_MODE)&&(App::ct->objCont->getEditModeObjectID()==it->getID()))
				pc=App::ct->objCont->_editionPath;
			else
				pc=it->pathContainer;
		}
		if (pc!=NULL)
		{
			bool ok;
			float newVal=ui->qqRelativeVelocityValue->text().toFloat(&ok);
			if (ok)
			{
				pc->setAvpRelativeVelocityAtRotationAxisChange(newVal);
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			}
			refresh();
		}
	}
}

void CDlgPathVelocityProfile::on_qqEndPointsZero_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPath* it=App::ct->objCont->getPath(_pathID);
		CPathCont* pc=NULL;
		if (it!=NULL)
		{
			if ((App::ct->objCont->getEditModeType()==PATH_EDIT_MODE)&&(App::ct->objCont->getEditModeObjectID()==it->getID()))
				pc=App::ct->objCont->_editionPath;
			else
				pc=it->pathContainer;
		}
		if (pc!=NULL)
		{
			pc->setAttributes(pc->getAttributes()^sim_pathproperty_endpoints_at_zero);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			refresh();
		}
	}
}

void CDlgPathVelocityProfile::on_qqRelativeAcceleration_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPath* it=App::ct->objCont->getPath(_pathID);
		CPathCont* pc=NULL;
		if (it!=NULL)
		{
			if ((App::ct->objCont->getEditModeType()==PATH_EDIT_MODE)&&(App::ct->objCont->getEditModeObjectID()==it->getID()))
				pc=App::ct->objCont->_editionPath;
			else
				pc=it->pathContainer;
		}
		if (pc!=NULL)
		{
			bool ok;
			float newVal=ui->qqRelativeAcceleration->text().toFloat(&ok);
			if (ok)
			{
				pc->setAvpRelativeAcceleration(newVal);
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			}
			refresh();
		}
	}
}

void CDlgPathVelocityProfile::on_qqClose_clicked(QAbstractButton *button)
{
	defaultModalDialogEndRoutine(true);
}
