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
#include "qdlgmodelproperties.h"
#include "ui_qdlgmodelproperties.h"
#include "v_rep_internal.h"
#include "qdlgmodelthumbnailvisu.h"
#include "qdlgmodelthumbnail.h"
#include "App.h"
#include "Tt.h"

CQDlgModelProperties::CQDlgModelProperties(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgModelProperties)
{
    ui->setupUi(this);
}

CQDlgModelProperties::~CQDlgModelProperties()
{
    delete ui;
}

void CQDlgModelProperties::cancelEvent()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgModelProperties::okEvent()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgModelProperties::refresh()
{
	int ovProp=modelBaseObject->getMainPropertyOverride();
	ui->qqNotVisible->setChecked((ovProp&sim_modelproperty_not_visible)!=0);
	ui->qqNotCollidable->setChecked((ovProp&sim_modelproperty_not_collidable)!=0);
	ui->qqNotMeasurable->setChecked((ovProp&sim_modelproperty_not_measurable)!=0);
	ui->qqNotRenderable->setChecked((ovProp&sim_modelproperty_not_renderable)!=0);
	ui->qqNotCuttable->setChecked((ovProp&sim_modelproperty_not_cuttable)!=0);
	ui->qqNotDetectable->setChecked((ovProp&sim_modelproperty_not_detectable)!=0);
	ui->qqStatic->setChecked((ovProp&sim_modelproperty_not_dynamic)!=0);
	ui->qqNotRespondable->setEnabled((ovProp&sim_modelproperty_not_dynamic)!=0);
	ui->qqNotRespondable->setChecked((ovProp&sim_modelproperty_not_respondable)!=0);

	ui->qqAcknowledgments->setPlainText(modelBaseObject->getModelAcknowledgement().c_str());
}

void CQDlgModelProperties::on_qqSelectThumbnail_clicked()
{
	bool keepCurrentThumbnail=false;
	int modelBase=modelBaseObject->getID();
	while (true)
	{
		if (App::ct->environment->modelThumbnail_notSerializedHere.hasImage())
		{ // we already have a thumbnail!
			CQDlgModelThumbnailVisu dlg(this);
			dlg.applyThumbnail(&App::ct->environment->modelThumbnail_notSerializedHere);
			keepCurrentThumbnail=(dlg.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL);
		}
		if (!keepCurrentThumbnail)
		{
			CQDlgModelThumbnail dlg(this);
			dlg.modelBaseDummyID=modelBase;
			dlg.initialize();
			dlg.actualizeBitmap();
			if (dlg.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
			{
				App::ct->environment->modelThumbnail_notSerializedHere.copyFrom(&dlg.thumbnail);
				if (!dlg.thumbnailIsFromFile)
					break;
			}
			else
			{
				break;
			}
		}
		else
			break;
	}
	// Undo point is set when this modal dialog ends!
	refresh();
}

void CQDlgModelProperties::on_qqNotVisible_clicked()
{
	modelBaseObject->setMainPropertyOverride(modelBaseObject->getMainPropertyOverride()^sim_modelproperty_not_visible);
	// Undo point is set when this modal dialog ends!
	refresh();
}

void CQDlgModelProperties::on_qqNotCollidable_clicked()
{
	modelBaseObject->setMainPropertyOverride(modelBaseObject->getMainPropertyOverride()^sim_modelproperty_not_collidable);
	// Undo point is set when this modal dialog ends!
	refresh();
}

void CQDlgModelProperties::on_qqNotMeasurable_clicked()
{
	modelBaseObject->setMainPropertyOverride(modelBaseObject->getMainPropertyOverride()^sim_modelproperty_not_measurable);
	// Undo point is set when this modal dialog ends!
	refresh();
}

void CQDlgModelProperties::on_qqNotRenderable_clicked()
{
	modelBaseObject->setMainPropertyOverride(modelBaseObject->getMainPropertyOverride()^sim_modelproperty_not_renderable);
	// Undo point is set when this modal dialog ends!
	refresh();
}

void CQDlgModelProperties::on_qqNotCuttable_clicked()
{
	modelBaseObject->setMainPropertyOverride(modelBaseObject->getMainPropertyOverride()^sim_modelproperty_not_cuttable);
	// Undo point is set when this modal dialog ends!
	refresh();
}

void CQDlgModelProperties::on_qqNotDetectable_clicked()
{
	modelBaseObject->setMainPropertyOverride(modelBaseObject->getMainPropertyOverride()^sim_modelproperty_not_detectable);
	// Undo point is set when this modal dialog ends!
	refresh();
}

void CQDlgModelProperties::on_qqStatic_clicked()
{
	int ovr=modelBaseObject->getMainPropertyOverride();
	if (ovr&sim_modelproperty_not_dynamic)
	{ // We wanna switch to dynamic again. We have to disable the "not respondable"
		ovr=(ovr|sim_modelproperty_not_respondable)-sim_modelproperty_not_respondable;
	}
	modelBaseObject->setMainPropertyOverride(ovr^sim_modelproperty_not_dynamic);
	// Undo point is set when this modal dialog ends!
	refresh();
}

void CQDlgModelProperties::on_qqNotRespondable_clicked()
{
	int ovr=modelBaseObject->getMainPropertyOverride();
	if ((ovr&sim_modelproperty_not_dynamic)==0)
		return;
	modelBaseObject->setMainPropertyOverride(ovr^sim_modelproperty_not_respondable);
	// Undo point is set when this modal dialog ends!
	refresh();
}

void CQDlgModelProperties::on_qqOkCancel_accepted()
{
	acknowledgment=ui->qqAcknowledgments->toPlainText().toStdString();
	tt::removeSpacesAndEmptyLinesAtBeginningAndEnd(acknowledgment);
	defaultModalDialogEndRoutine(true);
}

void CQDlgModelProperties::on_qqOkCancel_rejected()
{
	defaultModalDialogEndRoutine(false);
}
