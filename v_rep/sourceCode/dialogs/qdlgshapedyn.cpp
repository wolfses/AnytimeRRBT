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
#include "qdlgshapedyn.h"
#include "ui_qdlgshapedyn.h"
#include "Tt.h"
#include "GV.h"
#include "IloIlo.h"
#include "propBrowser_engineProp_material.h"
#include "v_repStrings.h"
#include "App.h"
#include "qdlginertiatensor.h"

bool CQDlgShapeDyn::showDynamicWindow=false;
bool CQDlgShapeDyn::inertiaFrameRelativeToParent=true;

CQDlgShapeDyn::CQDlgShapeDyn(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgShapeDyn)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
}

CQDlgShapeDyn::~CQDlgShapeDyn()
{
    delete ui;
}

void CQDlgShapeDyn::cancelEvent()
{ // no cancel event allowed
	showDynamicWindow=false;
	CDlgEx::cancelEvent();
	App::ct->objCont->setFullDialogRefreshFlag();
}

void CQDlgShapeDyn::refresh()
{
	inMainRefreshRoutine=true;
	bool sel=App::ct->objCont->isLastSelectionAShape();
	int sc=App::ct->objCont->getShapeNumberInSelection();
	bool snr=App::ct->simulation->isSimulationStopped();
	bool notHeightfield=true;
	CShape* it=NULL;
	CGeomProxy* geom=NULL;
	if (sel)
	{
		it=App::ct->objCont->getShape(App::ct->objCont->getLastSelectionID());
		geom=it->geomData;
		notHeightfield=(it->geomData->geomInfo->getPurePrimitiveType()!=sim_pure_primitive_heightfield);
	}

	ui->qqStatic->setEnabled(sel&&snr&&notHeightfield);
	ui->qqAutomaticToNonStatic->setEnabled(sel&&snr&&notHeightfield&&it->getShapeIsDynamicallyStatic());
	ui->qqRespondable->setEnabled(sel&&snr);
	ui->qqParentFollows->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr&&(it->getParent()!=NULL));
	ui->qqApplyAll->setEnabled((sc>1)&&sel&&snr);

	ui->qqSleepModeStart->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);
	ui->qqMass->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);

	ui->qqMassD2->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);
	ui->qqMassT2->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);
	ui->qqID2->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);
	ui->qqIT2->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);
	ui->qqTensorMatrix->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);

	ui->qqSleepModeStart->setChecked(sel&&it->getStartInDynamicSleeping());
	ui->qqParentFollows->setChecked(sel&&it->getParentFollowsDynamic());

	ui->a_1->setEnabled(sel&&snr&&it->getRespondable());
	ui->a_2->setEnabled(sel&&snr&&it->getRespondable());
	ui->a_3->setEnabled(sel&&snr&&it->getRespondable());
	ui->a_4->setEnabled(sel&&snr&&it->getRespondable());
	ui->a_5->setEnabled(sel&&snr&&it->getRespondable());
	ui->a_6->setEnabled(sel&&snr&&it->getRespondable());
	ui->a_7->setEnabled(sel&&snr&&it->getRespondable());
	ui->a_8->setEnabled(sel&&snr&&it->getRespondable());
	ui->a_9->setEnabled(sel&&snr&&it->getRespondable());
	ui->a_10->setEnabled(sel&&snr&&it->getRespondable());
	ui->a_11->setEnabled(sel&&snr&&it->getRespondable());
	ui->a_12->setEnabled(sel&&snr&&it->getRespondable());
	ui->a_13->setEnabled(sel&&snr&&it->getRespondable());
	ui->a_14->setEnabled(sel&&snr&&it->getRespondable());
	ui->a_15->setEnabled(sel&&snr&&it->getRespondable());
	ui->a_16->setEnabled(sel&&snr&&it->getRespondable());

	ui->a_1->setChecked(sel&&(it->getDynamicCollisionMask()&0x0001));
	ui->a_2->setChecked(sel&&(it->getDynamicCollisionMask()&0x0002));
	ui->a_3->setChecked(sel&&(it->getDynamicCollisionMask()&0x0004));
	ui->a_4->setChecked(sel&&(it->getDynamicCollisionMask()&0x0008));
	ui->a_5->setChecked(sel&&(it->getDynamicCollisionMask()&0x0010));
	ui->a_6->setChecked(sel&&(it->getDynamicCollisionMask()&0x0020));
	ui->a_7->setChecked(sel&&(it->getDynamicCollisionMask()&0x0040));
	ui->a_8->setChecked(sel&&(it->getDynamicCollisionMask()&0x0080));
	ui->a_9->setChecked(sel&&(it->getDynamicCollisionMask()&0x0100));
	ui->a_10->setChecked(sel&&(it->getDynamicCollisionMask()&0x0200));
	ui->a_11->setChecked(sel&&(it->getDynamicCollisionMask()&0x0400));
	ui->a_12->setChecked(sel&&(it->getDynamicCollisionMask()&0x0800));
	ui->a_13->setChecked(sel&&(it->getDynamicCollisionMask()&0x1000));
	ui->a_14->setChecked(sel&&(it->getDynamicCollisionMask()&0x2000));
	ui->a_15->setChecked(sel&&(it->getDynamicCollisionMask()&0x4000));
	ui->a_16->setChecked(sel&&(it->getDynamicCollisionMask()&0x8000));

	ui->qqMaterial->setEnabled(sel&&snr&&it->getRespondable());
	ui->qqMaterial->clear();
	for (int i=0;i<int(App::ct->dynMaterialCont->_allDynMaterialObjects.size());i++)
	{
		CDynMaterialObject* mat=App::ct->dynMaterialCont->_allDynMaterialObjects[i];
		ui->qqMaterial->addItem(mat->getObjectName().c_str(),QVariant(mat->getObjectID()));
	}
	int desiredMatID=App::ct->dynMaterialCont->getDefaultObject()->getObjectID();
	if (sel)
		desiredMatID=it->geomData->geomInfo->getDynMaterialObject()->getObjectID();
	for (int i=0;i<ui->qqMaterial->count();i++)
	{
		if (ui->qqMaterial->itemData(i).toInt()==desiredMatID)
		{
			ui->qqMaterial->setCurrentIndex(i);
			break;
		}
	}
//	ui->qqDuplicateMaterial->setEnabled(sel&&snr&&it->getRespondable());
	ui->qqAdjustEngineProperties->setEnabled(sel&&snr&&it->getRespondable());//&&it->geomData->geomInfo->getDynMaterialObject()->getEditableTag());


	ui->qqIX->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);
	ui->qqIY->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);
	ui->qqIZ->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);

	ui->qqPX->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);
	ui->qqPY->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);
	ui->qqPZ->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);
	ui->qqAlpha->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);
	ui->qqBeta->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);
	ui->qqGamma->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);

	ui->qqRespondable->setChecked(sel&&it->getRespondable());
	ui->qqStatic->setChecked(sel&&it->getShapeIsDynamicallyStatic());
	ui->qqAutomaticToNonStatic->setChecked(sel&&it->getSetAutomaticallyToNonStaticIfGetsParent());

	ui->qqCoordWorld->setChecked(!inertiaFrameRelativeToParent);
	ui->qqCoordParent->setChecked(inertiaFrameRelativeToParent);

	ui->qqCoordWorld->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);
	ui->qqCoordParent->setEnabled(sel&&(!it->getShapeIsDynamicallyStatic())&&snr);

	if (sel)
	{
		ui->qqMass->setText(gv::getMassStr(geom->geomInfo->getMass()).c_str());

		C3Vector v(it->geomData->geomInfo->getPrincipalMomentsOfInertia());
		ui->qqIX->setText(gv::getInertiaOverMassStr(v(0)).c_str());
		ui->qqIY->setText(gv::getInertiaOverMassStr(v(1)).c_str());
		ui->qqIZ->setText(gv::getInertiaOverMassStr(v(2)).c_str());

		C4X4Matrix m(it->geomData->geomInfo->getLocalInertiaFrame().getMatrix());

		if (!inertiaFrameRelativeToParent)
			m=it->getCumulativeTransformation().getMatrix()*m;

		C3Vector e(m.M.getEulerAngles());
		ui->qqPX->setText(gv::getSizeStr(true,m.X(0)).c_str());
		ui->qqPY->setText(gv::getSizeStr(true,m.X(1)).c_str());
		ui->qqPZ->setText(gv::getSizeStr(true,m.X(2)).c_str());
		ui->qqAlpha->setText(gv::getAngleStr(true,e(0)).c_str());
		ui->qqBeta->setText(gv::getAngleStr(true,e(1)).c_str());
		ui->qqGamma->setText(gv::getAngleStr(true,e(2)).c_str());
	}
	else
	{
		ui->qqMass->setText("");

		ui->qqIX->setText("");
		ui->qqIY->setText("");
		ui->qqIZ->setText("");

		ui->qqPX->setText("");
		ui->qqPY->setText("");
		ui->qqPZ->setText("");

		ui->qqAlpha->setText("");
		ui->qqBeta->setText("");
		ui->qqGamma->setText("");
	}
	inMainRefreshRoutine=false;
}

void CQDlgShapeDyn::on_qqStatic_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setShapeIsDynamicallyStatic(!it->getShapeIsDynamicallyStatic());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_qqSleepModeStart_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setStartInDynamicSleeping(!it->getStartInDynamicSleeping());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_qqParentFollows_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		C3DObject* parent=it->getParent();
		if (parent!=NULL)
		{
			it->setParentFollowsDynamic(!it->getParentFollowsDynamic());
			if (it->getParentFollowsDynamic())
			{ // Now make sure there are no other children that have the same setting:
				for (int i=0;i<int(parent->childList.size());i++)
				{
					C3DObject* child=parent->childList[i];
					if ((child!=it)&&(child->getObjectType()==sim_object_shape_type))
						((CShape*)child)->setParentFollowsDynamic(false);
				}
			}
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgShapeDyn::on_qqAdjustEngineProperties_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CPropBrowserEngineMaterial dlg(App::mainWindow);
		dlg.setModal(true);
		dlg.exec();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_qqRespondable_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setRespondable(!it->getRespondable());
		if (it->getRespondable()&&(!it->geomData->geomInfo->isPure()))
		{
			if (it->geomData->geomInfo->isConvex())
				App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Shape"),strTranslate(IDS_MAKING_NON_PURE_CONVEX_SHAPE_RESPONDABLE_WARNING),VMessageBox::OKELI);
			else
				App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Shape"),strTranslate(IDS_MAKING_NON_PURE_CONCAVE_SHAPE_RESPONDABLE_WARNING),VMessageBox::OKELI);
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_a_1_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setDynamicCollisionMask(it->getDynamicCollisionMask()^0x0001);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_a_2_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setDynamicCollisionMask(it->getDynamicCollisionMask()^0x0002);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_a_3_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setDynamicCollisionMask(it->getDynamicCollisionMask()^0x0004);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_a_4_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setDynamicCollisionMask(it->getDynamicCollisionMask()^0x0008);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_a_5_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setDynamicCollisionMask(it->getDynamicCollisionMask()^0x0010);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_a_6_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setDynamicCollisionMask(it->getDynamicCollisionMask()^0x0020);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_a_7_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setDynamicCollisionMask(it->getDynamicCollisionMask()^0x0040);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_a_8_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setDynamicCollisionMask(it->getDynamicCollisionMask()^0x0080);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_a_9_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setDynamicCollisionMask(it->getDynamicCollisionMask()^0x0100);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_a_10_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setDynamicCollisionMask(it->getDynamicCollisionMask()^0x0200);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_a_11_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setDynamicCollisionMask(it->getDynamicCollisionMask()^0x0400);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_a_12_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setDynamicCollisionMask(it->getDynamicCollisionMask()^0x0800);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_a_13_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setDynamicCollisionMask(it->getDynamicCollisionMask()^0x1000);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_a_14_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setDynamicCollisionMask(it->getDynamicCollisionMask()^0x2000);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_a_15_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setDynamicCollisionMask(it->getDynamicCollisionMask()^0x4000);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_a_16_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setDynamicCollisionMask(it->getDynamicCollisionMask()^0x8000);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_qqMass_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* shape=(CShape*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqMass->text().toFloat(&ok);
		if (ok)
		{
			shape->geomData->geomInfo->setMass(newVal*gv::userToKilograms);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgShapeDyn::on_qqMassT2_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		std::vector<CGeomWrap*> allComponents;
		for (int i=0;i<App::ct->objCont->getSelSize();i++)
		{
			CShape* it=App::ct->objCont->getShape(App::ct->objCont->getSelID(i));
			if ( (it!=NULL)&&(!it->getShapeIsDynamicallyStatic()) )
			{
				CGeomWrap* sc=it->geomData->geomInfo;
				for (int j=0;j<int(allComponents.size());j++)
				{
					if (allComponents[j]==sc)
					{
						sc=NULL;
						break;
					}
				}
				if (sc!=NULL)
					allComponents.push_back(sc);
			}
		}
		for (int i=0;i<int(allComponents.size());i++)
			allComponents[i]->setMass(allComponents[i]->getMass()*2.0f);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_qqMassD2_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		std::vector<CGeomWrap*> allComponents;
		for (int i=0;i<App::ct->objCont->getSelSize();i++)
		{
			CShape* it=App::ct->objCont->getShape(App::ct->objCont->getSelID(i));
			if ( (it!=NULL)&&(!it->getShapeIsDynamicallyStatic()) )
			{
				CGeomWrap* sc=it->geomData->geomInfo;
				for (int j=0;j<int(allComponents.size());j++)
				{
					if (allComponents[j]==sc)
					{
						sc=NULL;
						break;
					}
				}
				if (sc!=NULL)
					allComponents.push_back(sc);
			}
		}
		for (int i=0;i<int(allComponents.size());i++)
			allComponents[i]->setMass(allComponents[i]->getMass()/2.0f);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_qqIX_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* shape=(CShape*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqIX->text().toFloat(&ok);
		if (ok)
		{
			C3Vector v(shape->geomData->geomInfo->getPrincipalMomentsOfInertia());
			v(0)=newVal*gv::userToMeter*gv::userToMeter;
			shape->geomData->geomInfo->setPrincipalMomentsOfInertia(v);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgShapeDyn::on_qqIY_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* shape=(CShape*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqIY->text().toFloat(&ok);
		if (ok)
		{
			C3Vector v(shape->geomData->geomInfo->getPrincipalMomentsOfInertia());
			v(1)=newVal*gv::userToMeter*gv::userToMeter;
			shape->geomData->geomInfo->setPrincipalMomentsOfInertia(v);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgShapeDyn::on_qqIZ_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* shape=(CShape*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqIZ->text().toFloat(&ok);
		if (ok)
		{
			C3Vector v(shape->geomData->geomInfo->getPrincipalMomentsOfInertia());
			v(2)=newVal*gv::userToMeter*gv::userToMeter;
			shape->geomData->geomInfo->setPrincipalMomentsOfInertia(v);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}


void CQDlgShapeDyn::on_qqIT2_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		std::vector<CGeomWrap*> allComponents;
		for (int i=0;i<App::ct->objCont->getSelSize();i++)
		{
			CShape* it=App::ct->objCont->getShape(App::ct->objCont->getSelID(i));
			if ( (it!=NULL)&&(!it->getShapeIsDynamicallyStatic()) )
			{
				CGeomWrap* sc=it->geomData->geomInfo;
				for (int j=0;j<int(allComponents.size());j++)
				{
					if (allComponents[j]==sc)
					{
						sc=NULL;
						break;
					}
				}
				if (sc!=NULL)
					allComponents.push_back(sc);
			}
		}
		for (int i=0;i<int(allComponents.size());i++)
		{
			C3Vector v(allComponents[i]->getPrincipalMomentsOfInertia());
			v*=2.0f;
			allComponents[i]->setPrincipalMomentsOfInertia(v);
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_qqID2_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		std::vector<CGeomWrap*> allComponents;
		for (int i=0;i<App::ct->objCont->getSelSize();i++)
		{
			CShape* it=App::ct->objCont->getShape(App::ct->objCont->getSelID(i));
			if ( (it!=NULL)&&(!it->getShapeIsDynamicallyStatic()) )
			{
				CGeomWrap* sc=it->geomData->geomInfo;
				for (int j=0;j<int(allComponents.size());j++)
				{
					if (allComponents[j]==sc)
					{
						sc=NULL;
						break;
					}
				}
				if (sc!=NULL)
					allComponents.push_back(sc);
			}
		}
		for (int i=0;i<int(allComponents.size());i++)
		{
			C3Vector v(allComponents[i]->getPrincipalMomentsOfInertia());
			v/=2.0f;
			allComponents[i]->setPrincipalMomentsOfInertia(v);
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_qqPX_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* shape=(CShape*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqPX->text().toFloat(&ok);
		if (ok)
		{
			if (inertiaFrameRelativeToParent)
			{
				C7Vector tr(shape->geomData->geomInfo->getLocalInertiaFrame());
				tr.X(0)=newVal*gv::userToMeter;
				shape->geomData->geomInfo->setLocalInertiaFrame(tr);
			}
			else
			{
				C7Vector tr(shape->getCumulativeTransformation());
				C7Vector trf(shape->geomData->geomInfo->getLocalInertiaFrame());
				C7Vector tra(tr*trf);
				tra.X(0)=newVal*gv::userToMeter;
				trf=tr.getInverse()*tra;
				shape->geomData->geomInfo->setLocalInertiaFrame(trf);
			}
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgShapeDyn::on_qqPY_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* shape=(CShape*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqPY->text().toFloat(&ok);
		if (ok)
		{
			if (inertiaFrameRelativeToParent)
			{
				C7Vector tr(shape->geomData->geomInfo->getLocalInertiaFrame());
				tr.X(1)=newVal*gv::userToMeter;
				shape->geomData->geomInfo->setLocalInertiaFrame(tr);
			}
			else
			{
				C7Vector tr(shape->getCumulativeTransformation());
				C7Vector trf(shape->geomData->geomInfo->getLocalInertiaFrame());
				C7Vector tra(tr*trf);
				tra.X(1)=newVal*gv::userToMeter;
				trf=tr.getInverse()*tra;
				shape->geomData->geomInfo->setLocalInertiaFrame(trf);
			}
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgShapeDyn::on_qqPZ_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* shape=(CShape*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqPZ->text().toFloat(&ok);
		if (ok)
		{
			if (inertiaFrameRelativeToParent)
			{
				C7Vector tr(shape->geomData->geomInfo->getLocalInertiaFrame());
				tr.X(2)=newVal*gv::userToMeter;
				shape->geomData->geomInfo->setLocalInertiaFrame(tr);
			}
			else
			{
				C7Vector tr(shape->getCumulativeTransformation());
				C7Vector trf(shape->geomData->geomInfo->getLocalInertiaFrame());
				C7Vector tra(tr*trf);
				tra.X(2)=newVal*gv::userToMeter;
				trf=tr.getInverse()*tra;
				shape->geomData->geomInfo->setLocalInertiaFrame(trf);
			}
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgShapeDyn::on_qqAlpha_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* shape=(CShape*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqAlpha->text().toFloat(&ok);
		if (ok)
		{
			if (inertiaFrameRelativeToParent)
			{
				C4X4Matrix m(shape->geomData->geomInfo->getLocalInertiaFrame().getMatrix());
				C3Vector e(m.M.getEulerAngles());
				e(0)=newVal*gv::userToRad;
				m.M.setEulerAngles(e);
				shape->geomData->geomInfo->setLocalInertiaFrame(m.getTransformation());
			}
			else
			{
				C4X4Matrix tr(shape->getCumulativeTransformation().getMatrix());
				C4X4Matrix trf(shape->geomData->geomInfo->getLocalInertiaFrame().getMatrix());
				C4X4Matrix tra(tr*trf);
				C3Vector e(tra.M.getEulerAngles());
				e(0)=newVal*gv::userToRad;
				tra.M.setEulerAngles(e);
				trf=tr.getInverse()*tra;
				shape->geomData->geomInfo->setLocalInertiaFrame(trf.getTransformation());
			}
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgShapeDyn::on_qqBeta_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* shape=(CShape*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqBeta->text().toFloat(&ok);
		if (ok)
		{
			if (inertiaFrameRelativeToParent)
			{
				C4X4Matrix m(shape->geomData->geomInfo->getLocalInertiaFrame().getMatrix());
				C3Vector e(m.M.getEulerAngles());
				e(1)=newVal*gv::userToRad;
				m.M.setEulerAngles(e);
				shape->geomData->geomInfo->setLocalInertiaFrame(m.getTransformation());
			}
			else
			{
				C4X4Matrix tr(shape->getCumulativeTransformation().getMatrix());
				C4X4Matrix trf(shape->geomData->geomInfo->getLocalInertiaFrame().getMatrix());
				C4X4Matrix tra(tr*trf);
				C3Vector e(tra.M.getEulerAngles());
				e(1)=newVal*gv::userToRad;
				tra.M.setEulerAngles(e);
				trf=tr.getInverse()*tra;
				shape->geomData->geomInfo->setLocalInertiaFrame(trf.getTransformation());
			}
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgShapeDyn::on_qqGamma_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* shape=(CShape*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqGamma->text().toFloat(&ok);
		if (ok)
		{
			if (inertiaFrameRelativeToParent)
			{
				C4X4Matrix m(shape->geomData->geomInfo->getLocalInertiaFrame().getMatrix());
				C3Vector e(m.M.getEulerAngles());
				e(2)=newVal*gv::userToRad;
				m.M.setEulerAngles(e);
				shape->geomData->geomInfo->setLocalInertiaFrame(m.getTransformation());
			}
			else
			{
				C4X4Matrix tr(shape->getCumulativeTransformation().getMatrix());
				C4X4Matrix trf(shape->geomData->geomInfo->getLocalInertiaFrame().getMatrix());
				C4X4Matrix tra(tr*trf);
				C3Vector e(tra.M.getEulerAngles());
				e(2)=newVal*gv::userToRad;
				tra.M.setEulerAngles(e);
				trf=tr.getInverse()*tra;
				shape->geomData->geomInfo->setLocalInertiaFrame(trf.getTransformation());
			}
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgShapeDyn::on_qqApplyAll_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		if (App::ct->objCont->getShapeNumberInSelection()<2)
			return;
		CShape* last=(CShape*)App::ct->objCont->getLastSelection();
		C7Vector trLast(last->getCumulativeTransformation());
		C7Vector trfLast(last->geomData->geomInfo->getLocalInertiaFrame());
		bool lastIsHeightfield=(last->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_heightfield);
		for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
		{
			CShape* it=App::ct->objCont->getShape(App::ct->objCont->getSelID(i));
			if (it!=NULL)
			{
				bool itIsHeightfield=(it->geomData->geomInfo->getPurePrimitiveType()==sim_pure_primitive_heightfield);
				if ( (it!=NULL) && ((!itIsHeightfield)||lastIsHeightfield) ) // second part added on 15/8/2013: heightfields are ALWAYS static, no matter what
				{
					it->setShapeIsDynamicallyStatic(last->getShapeIsDynamicallyStatic());
					it->setRespondable(last->getRespondable());
					it->setStartInDynamicSleeping(last->getStartInDynamicSleeping());
					it->geomData->geomInfo->setMass(last->geomData->geomInfo->getMass());
					it->geomData->geomInfo->setPrincipalMomentsOfInertia(last->geomData->geomInfo->getPrincipalMomentsOfInertia());

					if (inertiaFrameRelativeToParent)
						it->geomData->geomInfo->setLocalInertiaFrame(last->geomData->geomInfo->getLocalInertiaFrame());
					else
					{ // since 18/4/2013
						C7Vector trIt(it->getCumulativeTransformation());
						C7Vector trfIt(trIt.getInverse()*trLast*trfLast);
						it->geomData->geomInfo->setLocalInertiaFrame(trfIt);
					}

					it->geomData->geomInfo->setDynMaterialId(last->geomData->geomInfo->getDynMaterialId());
					//last->geomData->geomInfo->copyEnginePropertiesTo(it->geomData->geomInfo);

					it->setDynamicCollisionMask(last->getDynamicCollisionMask());
					App::ct->undoBufferContainer->announceChangeStart(); // **************** UNDO THINGY ****************
				}
			}
		}
		App::ct->undoBufferContainer->announceChangeEnd(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgShapeDyn::on_qqTensorMatrix_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		if (App::ct->objCont->getSelSize()==0)
			return;
		CShape* shape=(CShape*)App::ct->objCont->getLastSelection();
		CQDlgInertiaTensor theDialog(this);

		C7Vector tr(shape->geomData->geomInfo->getLocalInertiaFrame());
		tr.X.clear(); // we just wanna reorient the inertia matrix, no shifting!
		C3X3Matrix m(CGeomWrap::getNewTensor(shape->geomData->geomInfo->getPrincipalMomentsOfInertia(),tr));

		m.copyToInterface(theDialog.tensor);
		C3Vector com(shape->geomData->geomInfo->getLocalInertiaFrame().X);
		com.copyTo(theDialog.com);
		theDialog.relativeToShapeFrame=true;
		theDialog.shapeHandle=shape->getID();
		theDialog.refresh();
		if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
		{
			m.copyFromInterface(theDialog.tensor);
			C4Vector rot;
			C3Vector pmoment;
			CGeomWrap::findPrincipalMomentOfInertia(m,rot,pmoment);
			shape->geomData->geomInfo->setPrincipalMomentsOfInertia(pmoment);
			if (theDialog.relativeToShapeFrame)
				shape->geomData->geomInfo->setLocalInertiaFrame(C7Vector(rot,C3Vector(theDialog.com)));
			else
				shape->geomData->geomInfo->setLocalInertiaFrame(shape->getCumulativeTransformation().getInverse()*C7Vector(rot,C3Vector(theDialog.com)));
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}


void CQDlgShapeDyn::on_qqCoordWorld_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		inertiaFrameRelativeToParent=false;
		App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_INERTIA_FRAME),strTranslate(IDSN_INERTIA_FRAME_RELATIVE_TO_WORLD),VMessageBox::OKELI);
		refresh();
	}
}

void CQDlgShapeDyn::on_qqCoordParent_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		inertiaFrameRelativeToParent=true;
		refresh();
	}
}

void CQDlgShapeDyn::on_qqMaterial_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* shape=(CShape*)App::ct->objCont->getLastSelection();
		if (!inMainRefreshRoutine)
		{
			int materialID=ui->qqMaterial->itemData(ui->qqMaterial->currentIndex()).toInt();
			shape->geomData->geomInfo->setDynMaterialId(materialID);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			refresh();
		}
	}
}

void CQDlgShapeDyn::on_qqAutomaticToNonStatic_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAShape())
			return;
		CShape* it=(CShape*)App::ct->objCont->getLastSelection();
		it->setSetAutomaticallyToNonStaticIfGetsParent(!it->getSetAutomaticallyToNonStaticIfGetsParent());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}
