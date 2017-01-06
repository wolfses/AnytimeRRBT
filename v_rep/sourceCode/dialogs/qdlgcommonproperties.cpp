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
#include "qdlgcommonproperties.h"
#include "ui_qdlgcommonproperties.h"
#include "Tt.h"
#include "GV.h"
#include "ObjectEdition.h"
#include "IloIlo.h"
#include "qdlgdetectableproperties.h"
#include "qdlgmodelproperties.h"
#include "qdlgassembly.h"
#include "qdlgscaling.h"
#include "qdlgcustomdata.h"
#include "v_repStrings.h"
#include "App.h"

CQDlgCommonProperties::CQDlgCommonProperties(QWidget *parent) :
	  CDlgEx(parent),
	  ui(new Ui::CQDlgCommonProperties)
{
    ui->setupUi(this);
	inPlaceState=false;
	scalingState=0.5f;
}

CQDlgCommonProperties::~CQDlgCommonProperties()
{
    delete ui;
}

void CQDlgCommonProperties::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgCommonProperties::refresh()
{
	bool noSim=App::ct->simulation->isSimulationStopped();
	C3DObject* ls=App::ct->objCont->getLastSelection();
	bool objIsSelected=(ls!=NULL);
	bool isDummy=false;
	bool isShape=false;
	bool isMirror=false;
	bool isSimpleShape=false;
	bool isPurePrimitive=false;
	bool isPath=false;
	bool isGraph=false;
	bool isVolume=false;
	bool bigSel=App::ct->objCont->getSelSize()>=2;
	if (objIsSelected)
	{
		isMirror=(ls->getObjectType()==sim_object_mirror_type);
		isVolume=(ls->getObjectType()==sim_object_volume_type);
		isDummy=(ls->getObjectType()==sim_object_dummy_type);
		isShape=(ls->getObjectType()==sim_object_shape_type);
		isPath=(ls->getObjectType()==sim_object_path_type);
		isGraph=(ls->getObjectType()==sim_object_graph_type);
		if (isShape)
		{
			isSimpleShape=!((CShape*)ls)->isGrouping();
			isPurePrimitive=((CShape*)ls)->geomData->geomInfo->isPure();
		}
	}

	ui->qqSelectable->setEnabled(noSim&&objIsSelected);
	ui->qqSelectInvisible->setEnabled(noSim&&objIsSelected);
	ui->qqDepthInvisible->setEnabled(noSim&&objIsSelected);
	ui->qqIgnoreForViewFitting->setEnabled(noSim&&objIsSelected);
	ui->qqSelectBaseInstead->setEnabled(noSim&&objIsSelected);
	ui->qqDontShowInModelSelection->setEnabled(noSim&&objIsSelected);
	ui->qqApplyGeneralProperties->setEnabled(noSim&&bigSel);
	ui->qqAssembling->setEnabled(noSim&&objIsSelected);

	if (ls!=NULL)
	{
		ui->qqSelectable->setChecked((ls->getObjectProperty()&sim_objectproperty_selectable)!=0);
		ui->qqSelectInvisible->setChecked((ls->getObjectProperty()&sim_objectproperty_selectinvisible)!=0);
		ui->qqDepthInvisible->setChecked((ls->getObjectProperty()&sim_objectproperty_depthinvisible)!=0);
		ui->qqIgnoreForViewFitting->setChecked(ls->getIgnoredByViewFitting());
		ui->qqSelectBaseInstead->setChecked((ls->getObjectProperty()&sim_objectproperty_selectmodelbaseinstead)!=0);
		ui->qqDontShowInModelSelection->setChecked((ls->getObjectProperty()&sim_objectproperty_dontshowasinsidemodel)!=0);
	}
	else
	{
		ui->qqSelectable->setChecked(false);
		ui->qqSelectInvisible->setChecked(false);
		ui->qqDepthInvisible->setChecked(false);
		ui->qqIgnoreForViewFitting->setChecked(false);
		ui->qqSelectBaseInstead->setChecked(false);
		ui->qqDontShowInModelSelection->setChecked(false);
	}

	ui->a_1->setEnabled(noSim&&objIsSelected);
	ui->a_2->setEnabled(noSim&&objIsSelected);
	ui->a_3->setEnabled(noSim&&objIsSelected);
	ui->a_4->setEnabled(noSim&&objIsSelected);
	ui->a_5->setEnabled(noSim&&objIsSelected);
	ui->a_6->setEnabled(noSim&&objIsSelected);
	ui->a_7->setEnabled(noSim&&objIsSelected);
	ui->a_8->setEnabled(noSim&&objIsSelected);
	ui->a_9->setEnabled(noSim&&objIsSelected);
	ui->a_10->setEnabled(noSim&&objIsSelected);
	ui->a_11->setEnabled(noSim&&objIsSelected);
	ui->a_12->setEnabled(noSim&&objIsSelected);
	ui->a_13->setEnabled(noSim&&objIsSelected);
	ui->a_14->setEnabled(noSim&&objIsSelected);
	ui->a_15->setEnabled(noSim&&objIsSelected);
	ui->a_16->setEnabled(noSim&&objIsSelected);
	ui->qqApplyLayers->setEnabled(noSim&&bigSel);
	if (ls!=NULL)
	{
		ui->a_1->setChecked(ls->layer&1);
		ui->a_2->setChecked(ls->layer&2);
		ui->a_3->setChecked(ls->layer&4);
		ui->a_4->setChecked(ls->layer&8);
		ui->a_5->setChecked(ls->layer&16);
		ui->a_6->setChecked(ls->layer&32);
		ui->a_7->setChecked(ls->layer&64);
		ui->a_8->setChecked(ls->layer&128);
		ui->a_9->setChecked(ls->layer&256);
		ui->a_10->setChecked(ls->layer&512);
		ui->a_11->setChecked(ls->layer&1024);
		ui->a_12->setChecked(ls->layer&2048);
		ui->a_13->setChecked(ls->layer&4096);
		ui->a_14->setChecked(ls->layer&8192);
		ui->a_15->setChecked(ls->layer&16384);
		ui->a_16->setChecked(ls->layer&32768);
	}
	else
	{
		ui->a_1->setChecked(false);
		ui->a_2->setChecked(false);
		ui->a_3->setChecked(false);
		ui->a_4->setChecked(false);
		ui->a_5->setChecked(false);
		ui->a_6->setChecked(false);
		ui->a_7->setChecked(false);
		ui->a_8->setChecked(false);
		ui->a_9->setChecked(false);
		ui->a_10->setChecked(false);
		ui->a_11->setChecked(false);
		ui->a_12->setChecked(false);
		ui->a_13->setChecked(false);
		ui->a_14->setChecked(false);
		ui->a_15->setChecked(false);
		ui->a_16->setChecked(false);
	}

	ui->qqModelBase->setEnabled(noSim&&objIsSelected);
	if (ls!=NULL)
	{
		ui->qqModelBase->setChecked(ls->getModelBase());
		ui->qqEditModelProperties->setEnabled(ls->getModelBase());
	}
	else
	{
		ui->qqModelBase->setChecked(false);
		ui->qqEditModelProperties->setEnabled(false);
	}

	ui->qqApplySpecialProperties->setEnabled((bigSel&&(isShape||isPath||isDummy||isVolume||isGraph||isMirror))&&noSim);
	if (ls!=NULL)
	{
		//** Collidable
		ui->qqCollidable->setEnabled(isShape&&noSim);
		if (isShape)
			ui->qqCollidable->setChecked((ls->getMainPropertyLocal()&sim_objectspecialproperty_collidable)!=0);
		else
			ui->qqCollidable->setChecked(false);
		//**

		//** Measurable
		ui->qqMeasurable->setEnabled((isShape||isDummy||isVolume)&&noSim);
		if (isShape||isDummy||isVolume)
		{
			if (!isVolume)
				ui->qqMeasurable->setChecked((ls->getMainPropertyLocal()&sim_objectspecialproperty_measurable)!=0);
			else
				ui->qqMeasurable->setChecked(false);
		}
		else
			ui->qqMeasurable->setChecked(false);
		//**

		//** Ignored during path planning
		ui->qqPathPlanningIgnored->setEnabled((isShape||isDummy)&&noSim&&((ls->getMainPropertyLocal()&(sim_objectspecialproperty_collidable|sim_objectspecialproperty_measurable))!=0));
		if (isShape||isDummy)
			ui->qqPathPlanningIgnored->setChecked((ls->getMainPropertyLocal()&sim_objectspecialproperty_pathplanning_ignored)!=0);
		else
			ui->qqPathPlanningIgnored->setChecked(false);
		//**

		//** Renderable
		ui->qqRenderable->setEnabled((isShape||isPath||isGraph||isVolume||isMirror)&&noSim);
		if (isShape||isPath||isGraph||isVolume||isMirror)
			ui->qqRenderable->setChecked((ls->getMainPropertyLocal()&sim_objectspecialproperty_renderable)!=0);
		else
			ui->qqRenderable->setChecked(false);
		//**

		//** Cuttable
		ui->qqCuttable->setEnabled(((isSimpleShape&&(!isPurePrimitive))||isVolume)&&noSim);
		if (isSimpleShape||isVolume)
			ui->qqCuttable->setChecked((ls->getMainPropertyLocal()&sim_objectspecialproperty_cuttable)!=0);
		else
			ui->qqCuttable->setChecked(false);
		if (isShape)
		{
			if (isSimpleShape&&(!isPurePrimitive))
				ui->qqCuttable->setText(tr(IDS_CUTTABLE_PROP));
			else
				ui->qqCuttable->setText(tr(IDS_CUTTABLE_BUT_NOT_SIMPLE_NON_PURE_SHAPE));
		}
		else
			ui->qqCuttable->setText(tr(IDS_CUTTABLE_PROP));
		//**

		//** Detectable
		ui->qqDetectable->setEnabled((isShape||isDummy||isVolume)&&noSim);
		ui->qqEditDetectableDetails->setEnabled((isShape||isDummy||isVolume)&&noSim);
		if (isShape||isDummy||isVolume)
		{
			if ((ls->getMainPropertyLocal()&sim_objectspecialproperty_detectable_all)==0)
				ui->qqDetectable->setCheckState(Qt::Unchecked);
			else
			{
				if ((ls->getMainPropertyLocal()&sim_objectspecialproperty_detectable_all)==sim_objectspecialproperty_detectable_all)
					ui->qqDetectable->setCheckState(Qt::Checked);
				else
					ui->qqDetectable->setCheckState(Qt::PartiallyChecked);
			}
		}
		else
		{
			ui->qqDetectable->setCheckState(Qt::Unchecked);
		}
		//**
	}
	else
	{
		ui->qqCollidable->setEnabled(false);
		ui->qqMeasurable->setEnabled(false);
		ui->qqPathPlanningIgnored->setEnabled(false);
		ui->qqRenderable->setEnabled(false);
		ui->qqCuttable->setEnabled(false);
		ui->qqDetectable->setEnabled(false);
		ui->qqEditDetectableDetails->setEnabled(false);
		ui->qqCollidable->setChecked(false);
		ui->qqMeasurable->setChecked(false);
		ui->qqPathPlanningIgnored->setChecked(false);
		ui->qqRenderable->setChecked(false);
		ui->qqCuttable->setChecked(false);
		ui->qqCuttable->setText(tr(IDS_CUTTABLE_PROP));
		ui->qqDetectable->setChecked(false);
	}

	ui->qqUpdatable->setEnabled(objIsSelected&&noSim);
	ui->qqSizeFactor->setEnabled(objIsSelected&&noSim);
	ui->qqOpenScalingDialog->setEnabled(objIsSelected&&noSim);
	ui->qqOpenCustomDataDialog->setEnabled(objIsSelected&&noSim);

	ui->qqSelfCollisionIndicator->setEnabled(objIsSelected&&noSim&&(isDummy||isShape));

	if (ls!=NULL)
	{
		if (isDummy||isShape)
			ui->qqSelfCollisionIndicator->setText(tt::FNb(0,ls->getCollectionSelfCollisionIndicator(),false).c_str());
		else
			ui->qqSelfCollisionIndicator->setText("");
		ui->qqSizeFactor->setText(tt::FNb(0,ls->getSizeFactor(),5,false).c_str());
		ui->qqUpdatable->setChecked((ls->getObjectProperty()&sim_objectproperty_canupdatedna)!=0);
	}
	else
	{
		ui->qqSelfCollisionIndicator->setText("");
		ui->qqSizeFactor->setText("");
		ui->qqUpdatable->setChecked(false);
	}
}

void CQDlgCommonProperties::on_qqSelectable_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (App::ct->objCont->getSelSize()==0)
			return;
		C3DObject* it=App::ct->objCont->getLastSelection();
		it->setObjectProperty(it->getObjectProperty()^sim_objectproperty_selectable);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_qqSelectBaseInstead_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (App::ct->objCont->getSelSize()==0)
			return;
		C3DObject* it=App::ct->objCont->getLastSelection();
		it->setObjectProperty(it->getObjectProperty()^sim_objectproperty_selectmodelbaseinstead);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_qqDontShowInModelSelection_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (App::ct->objCont->getSelSize()==0)
			return;
		C3DObject* it=App::ct->objCont->getLastSelection();
		it->setObjectProperty(it->getObjectProperty()^sim_objectproperty_dontshowasinsidemodel);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_qqIgnoreForViewFitting_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (App::ct->objCont->getSelSize()==0)
			return;
		C3DObject* it=App::ct->objCont->getLastSelection();
		it->setIgnoredByViewFitting(!it->getIgnoredByViewFitting());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_qqSizeFactor_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		bool ok;
		float newVal=ui->qqSizeFactor->text().toFloat(&ok);
		if (ok)
		{
			newVal=tt::getLimitedFloat(0.000001f,1000000,newVal);
			it->setSizeFactor(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgCommonProperties::on_qqApplyGeneralProperties_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (App::ct->objCont->getSelSize()<2)
			return;
		C3DObject* last=App::ct->objCont->getLastSelection();
		for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
		{
			C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->getSelID(i));
			int objPropToCopy=sim_objectproperty_selectable|sim_objectproperty_selectmodelbaseinstead|sim_objectproperty_dontshowasinsidemodel;
			it->setObjectProperty(((it->getObjectProperty()|objPropToCopy)-objPropToCopy)|(last->getObjectProperty()&objPropToCopy));
			it->setIgnoredByViewFitting(last->getIgnoredByViewFitting());
			it->setSizeFactor(last->getSizeFactor());
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
}

void CQDlgCommonProperties::on_a_1_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		it->layer=it->layer^1;
		App::ct->hierarchy->setRefreshViewFlag();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_a_2_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		it->layer=it->layer^2;
		App::ct->hierarchy->setRefreshViewFlag();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_a_3_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		it->layer=it->layer^4;
		App::ct->hierarchy->setRefreshViewFlag();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_a_4_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		it->layer=it->layer^8;
		App::ct->hierarchy->setRefreshViewFlag();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_a_5_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		it->layer=it->layer^16;
		App::ct->hierarchy->setRefreshViewFlag();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_a_6_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		it->layer=it->layer^32;
		App::ct->hierarchy->setRefreshViewFlag();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_a_7_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		it->layer=it->layer^64;
		App::ct->hierarchy->setRefreshViewFlag();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_a_8_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		it->layer=it->layer^128;
		App::ct->hierarchy->setRefreshViewFlag();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_a_9_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		it->layer=it->layer^256;
		App::ct->hierarchy->setRefreshViewFlag();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_a_10_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		it->layer=it->layer^512;
		App::ct->hierarchy->setRefreshViewFlag();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_a_11_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		it->layer=it->layer^1024;
		App::ct->hierarchy->setRefreshViewFlag();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_a_12_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		it->layer=it->layer^2048;
		App::ct->hierarchy->setRefreshViewFlag();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_a_13_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		it->layer=it->layer^4096;
		App::ct->hierarchy->setRefreshViewFlag();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_a_14_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		it->layer=it->layer^8192;
		App::ct->hierarchy->setRefreshViewFlag();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_a_15_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		it->layer=it->layer^16384;
		App::ct->hierarchy->setRefreshViewFlag();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_a_16_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		it->layer=it->layer^32768;
		App::ct->hierarchy->setRefreshViewFlag();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}


void CQDlgCommonProperties::on_qqApplyLayers_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (App::ct->objCont->getSelSize()<2)
			return;
		C3DObject* last=App::ct->objCont->getLastSelection();
		for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
		{
			C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->getSelID(i));
			it->layer=last->layer;
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
}

void CQDlgCommonProperties::on_qqModelBase_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (App::ct->objCont->getSelSize()==0)
			return;
		C3DObject* it=App::ct->objCont->getLastSelection();
		it->setModelBase(!it->getModelBase());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		App::ct->objCont->setFullDialogRefreshFlag();
		refresh();
	}
}

void CQDlgCommonProperties::on_qqEditModelProperties_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG // required with modal dialogs
	{
		if (App::ct->objCont->getSelSize()==0)
			return;
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (!it->getModelBase())
			return;
		CQDlgModelProperties theDialog(this);
		theDialog.modelBaseObject=it;
		theDialog.acknowledgment=it->getModelAcknowledgement();
		int originalMainPropertyOverride=it->getMainPropertyOverride();
		CThumbnail* originalThmb=App::ct->environment->modelThumbnail_notSerializedHere.copyYourself();
		theDialog.refresh();
		if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
		{
			it->setModelAcknowledgement(theDialog.acknowledgment);
			// The rest was directly modified!
		}
		else
		{ // we have to restore a few values:
			it->setMainPropertyOverride(originalMainPropertyOverride);
			App::ct->environment->modelThumbnail_notSerializedHere.copyFrom(originalThmb);
		}
		delete originalThmb;
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_qqCollidable_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* ls=App::ct->objCont->getLastSelection();
		if (ls!=NULL)
		{
			ls->setMainPropertyLocal(ls->getMainPropertyLocal()^sim_objectspecialproperty_collidable);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgCommonProperties::on_qqMeasurable_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* ls=App::ct->objCont->getLastSelection();
		if (ls!=NULL)
		{
			ls->setMainPropertyLocal(ls->getMainPropertyLocal()^sim_objectspecialproperty_measurable);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgCommonProperties::on_qqPathPlanningIgnored_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* ls=App::ct->objCont->getLastSelection();
		if (ls!=NULL)
		{
			ls->setMainPropertyLocal(ls->getMainPropertyLocal()^sim_objectspecialproperty_pathplanning_ignored);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgCommonProperties::on_qqRenderable_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* ls=App::ct->objCont->getLastSelection();
		if (ls!=NULL)
		{
			ls->setMainPropertyLocal(ls->getMainPropertyLocal()^sim_objectspecialproperty_renderable);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgCommonProperties::on_qqCuttable_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* ls=App::ct->objCont->getLastSelection();
		if (ls!=NULL)
		{
			ls->setMainPropertyLocal(ls->getMainPropertyLocal()^sim_objectspecialproperty_cuttable);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgCommonProperties::on_qqDetectable_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* ls=App::ct->objCont->getLastSelection();
		if (ls!=NULL)
		{
			int t=ls->getMainPropertyLocal()&sim_objectspecialproperty_detectable_all;
			if (t==sim_objectspecialproperty_detectable_all)
			{
				t=ls->getMainPropertyLocal()|sim_objectspecialproperty_detectable_all;
				t-=sim_objectspecialproperty_detectable_all;
			}
			else
				t=ls->getMainPropertyLocal()|sim_objectspecialproperty_detectable_all;
			ls->setMainPropertyLocal(t);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgCommonProperties::on_qqEditDetectableDetails_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG // required with modal dialogs
	{
		C3DObject* last=App::ct->objCont->getLastSelection();
		if (last!=NULL)
		{
			CQDlgDetectableProperties dlg(this);
			dlg.objectProperties=last->getMainPropertyLocal();
			dlg.refresh();
			if (dlg.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
			{
				last->setMainPropertyLocal(dlg.objectProperties);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			}
			refresh();
		}
	}
}

void CQDlgCommonProperties::on_qqApplySpecialProperties_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (App::ct->objCont->getSelSize()<2)
			return;
		C3DObject* last=App::ct->objCont->getLastSelection();
		int settableBits=0;
		if (last->getObjectType()==sim_object_shape_type)
		{
			settableBits=sim_objectspecialproperty_collidable|sim_objectspecialproperty_measurable|sim_objectspecialproperty_detectable_all|sim_objectspecialproperty_renderable|sim_objectspecialproperty_pathplanning_ignored;
			bool isSimpleShape=!((CShape*)last)->isGrouping();
			bool isPurePrimitive=((CShape*)last)->geomData->geomInfo->isPure();
			if (isSimpleShape&&(!isPurePrimitive))
				settableBits|=sim_objectspecialproperty_cuttable;
		}
		if (last->getObjectType()==sim_object_volume_type)
		{
			settableBits=sim_objectspecialproperty_collidable|sim_objectspecialproperty_measurable|sim_objectspecialproperty_detectable_all|sim_objectspecialproperty_renderable|sim_objectspecialproperty_pathplanning_ignored|sim_objectspecialproperty_cuttable;
		}
		if (last->getObjectType()==sim_object_dummy_type)
		{
			settableBits=sim_objectspecialproperty_measurable|sim_objectspecialproperty_detectable_all|sim_objectspecialproperty_pathplanning_ignored;
		}
		if (last->getObjectType()==sim_object_mirror_type)
		{
			settableBits=sim_objectspecialproperty_renderable;
		}
		if (last->getObjectType()==sim_object_path_type)
		{
			settableBits=sim_objectspecialproperty_renderable;
		}
		if (last->getObjectType()==sim_object_graph_type)
		{
			settableBits=sim_objectspecialproperty_renderable;
		}
		int stateOfSettableBits=last->getMainPropertyLocal()&settableBits;
		for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
		{
			C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->getSelID(i));
			int objProp=((it->getMainPropertyLocal()|settableBits)-settableBits)|stateOfSettableBits;
			it->setMainPropertyLocal(objProp);
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
}


void CQDlgCommonProperties::on_qqOpenScalingDialog_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG // required with modal dialogs
	{
		if (App::ct->objCont->getSelSize()==0)
			return;
		CQDlgScaling theDialog(this);
		theDialog.refresh();
		theDialog.makeDialogModal();// undo points and scaling handled in the dialog routines
	}
}

void CQDlgCommonProperties::on_qqOpenCustomDataDialog_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG // required with modal dialogs
	{
		if (App::ct->objCont->getSelSize()==0)
			return;
		CQDlgCustomData theDialog(this);
		theDialog.refresh();
		theDialog.makeDialogModal();// undo points and such handled in the dialog routines
	}
}

void CQDlgCommonProperties::on_qqAssembling_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG // required with modal dialogs
	{
		if (App::ct->objCont->getSelSize()==0)
			return;
		CQDlgAssembly theDialog(this);
		theDialog.obj=App::ct->objCont->getLastSelection();
		theDialog.refresh();
		theDialog.makeDialogModal();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
}

void CQDlgCommonProperties::on_qqUpdatable_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* ls=App::ct->objCont->getLastSelection();
		if (ls!=NULL)
		{
			int p=ls->getObjectProperty();
			if (p&sim_objectproperty_canupdatedna)
			{
				if (VMessageBox::REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Updatable property"),strTranslate(IDSN_SURE_TO_DISABLE_UPDATABLE_WARNING),VMessageBox::YES_NO))
					ls->setObjectProperty(p-sim_objectproperty_canupdatedna);
			}
			else
			{
				ls->setObjectProperty(p|sim_objectproperty_canupdatedna);
				ls->generateUniqueUpdatableString();
			}
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgCommonProperties::on_qqSelectInvisible_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (App::ct->objCont->getSelSize()==0)
			return;
		C3DObject* it=App::ct->objCont->getLastSelection();
		it->setObjectProperty(it->getObjectProperty()^sim_objectproperty_selectinvisible);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_qqDepthInvisible_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (App::ct->objCont->getSelSize()==0)
			return;
		C3DObject* it=App::ct->objCont->getLastSelection();
		it->setObjectProperty(it->getObjectProperty()^sim_objectproperty_depthinvisible);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgCommonProperties::on_qqSelfCollisionIndicator_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it==NULL)
			return;
		bool ok;
		int newVal=ui->qqSelfCollisionIndicator->text().toInt(&ok);
		if (ok)
		{
			newVal=tt::getLimitedInt(0,99999999,newVal);
			it->setCollectionSelfCollisionIndicator(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}
