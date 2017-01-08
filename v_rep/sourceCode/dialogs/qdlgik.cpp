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
#include "qdlgik.h"
#include "ui_qdlgik.h"
#include "Tt.h"
#include "GV.h"
#include "App.h"
#include "editboxdelegate.h"
#include "qdlgikelements.h"
#include "qdlgikconditional.h"
#include "qdlgikavoidance.h"
#include "v_repStrings.h"

CQDlgIk::CQDlgIk(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgIk)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
	inListSelectionRoutine=false;
	noListSelectionAllowed=false;
	delKeyShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	connect(delKeyShortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	backspaceKeyShortcut = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
	connect(backspaceKeyShortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	CEditBoxDelegate* delegate=new CEditBoxDelegate();
	ui->qqList->setItemDelegate(delegate);
}

CQDlgIk::~CQDlgIk()
{
    delete ui;
}

void CQDlgIk::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(CALCULATION_DLG);
}

void CQDlgIk::refresh()
{
	inMainRefreshRoutine=true;
	int groupID=getSelectedObjectID();
	CikGroup* it=App::ct->ikGroups->getIkGroup(groupID);

	if (!inListSelectionRoutine)
	{
		updateObjectsInList();
		selectObjectInList(groupID);
	}

	ui->qqUp->setEnabled(it!=NULL);
	ui->qqDown->setEnabled(it!=NULL);
	ui->qqExplicitHandling->setEnabled(it!=NULL);
	ui->qqGroupIsActive->setEnabled(it!=NULL);
	ui->qqRedundant->setEnabled(it!=NULL);
	ui->qqIgnoreMaxStepSizes->setEnabled(it!=NULL);
	ui->qqCalcMethodCombo->setEnabled(it!=NULL);
	ui->qqDamping->setEnabled((it!=NULL)&&(it->getCalculationMethod()==sim_ik_damped_least_squares_method));
	ui->qqMaxIterations->setEnabled(it!=NULL);
	ui->qqWeightJointLimits->setEnabled(it!=NULL);
	ui->qqWeightObstacleAvoidance->setEnabled(it!=NULL);
	ui->qqThresholdLinear->setEnabled(it!=NULL);
	ui->qqThresholdAngular->setEnabled(it!=NULL);
	ui->qqEditConditional->setEnabled(it!=NULL);
	ui->qqEditObstacleAvoidance->setEnabled(it!=NULL);
	ui->qqEditIkElements->setEnabled(it!=NULL);

	ui->qqIkEnabled->setChecked(App::ct->mainSettings->ikCalculationEnabled);

	ui->qqExplicitHandling->setChecked((it!=NULL)&&it->getExplicitHandling());
	ui->qqGroupIsActive->setChecked((it!=NULL)&&it->getActive());
	ui->qqRedundant->setChecked((it!=NULL)&&it->getCorrectJointLimits());
	ui->qqIgnoreMaxStepSizes->setChecked((it!=NULL)&&it->getIgnoreMaxStepSizes());

	ui->qqCalcMethodCombo->clear();

	if (it!=NULL)
	{
		ui->qqCalcMethodCombo->addItem(strTranslate(IDS_PSEUDO_INVERSE),QVariant(sim_ik_pseudo_inverse_method));
		ui->qqCalcMethodCombo->addItem(strTranslate(IDS_DLS),QVariant(sim_ik_damped_least_squares_method));
		for (int i=0;i<ui->qqCalcMethodCombo->count();i++)
		{
			if (ui->qqCalcMethodCombo->itemData(i).toInt()==it->getCalculationMethod())
			{
				ui->qqCalcMethodCombo->setCurrentIndex(i);
				break;
			}
		}

		ui->qqDamping->setText(tt::FNb(0,it->getDlsFactor(),4,false).c_str());
		ui->qqMaxIterations->setText(tt::FNb(0,it->getMaxIterations(),false).c_str());
		ui->qqWeightJointLimits->setText(tt::FNb(0,it->getJointLimitWeight(),3,false).c_str());
		ui->qqWeightObstacleAvoidance->setText(tt::FNb(0,it->getAvoidanceWeight(),3,false).c_str());
		ui->qqThresholdLinear->setText(gv::getSizeStr(false,it->getJointTreshholdLinear(),0).c_str());
		ui->qqThresholdAngular->setText(gv::getAngleStr(false,it->getJointTreshholdAngular(),0).c_str());
	}
	else
	{
		ui->qqDamping->setText("");
		ui->qqMaxIterations->setText("");
		ui->qqWeightJointLimits->setText("");
		ui->qqWeightObstacleAvoidance->setText("");
		ui->qqThresholdLinear->setText("");
		ui->qqThresholdAngular->setText("");
	}

	inMainRefreshRoutine=false;
}

void CQDlgIk::updateObjectsInList()
{
	noListSelectionAllowed=true;
	ui->qqList->clear();

	for (int i=0;i<int(App::ct->ikGroups->ikGroups.size());i++)
	{
		std::string txt=App::ct->ikGroups->ikGroups[i]->getObjectName();
		txt+=strTranslate(" [containing ");
		txt+=tt::FNb(0,App::ct->ikGroups->ikGroups[i]->ikElements.size()).c_str();
		txt+=strTranslate(" ik element(s)]");
		int objID=App::ct->ikGroups->ikGroups[i]->getObjectID();
		QListWidgetItem* itm=new QListWidgetItem(txt.c_str());
		itm->setData(Qt::UserRole,QVariant(objID));
		itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
		ui->qqList->addItem(itm);
	}

	noListSelectionAllowed=false;
}

int CQDlgIk::getSelectedObjectID()
{
	QList<QListWidgetItem*> sel=ui->qqList->selectedItems();
	if (sel.size()>0)
		return(sel.at(0)->data(Qt::UserRole).toInt());
	return(-1);
}

void CQDlgIk::selectObjectInList(int objectID)
{
	noListSelectionAllowed=true;
	for (int i=0;i<ui->qqList->count();i++)
	{
		QListWidgetItem* it=ui->qqList->item(i);
		if (it!=NULL)
		{
			if (it->data(Qt::UserRole).toInt()==objectID)
			{
				it->setSelected(true);
				break;
			}
		}
	}
	noListSelectionAllowed=false;
}

void CQDlgIk::onDeletePressed()
{
	if (focusWidget()==ui->qqList)
	{
		IF_UI_EVENT_CAN_WRITE_DATA
		{
			int objID=getSelectedObjectID();
			if (objID!=-1)
			{
				App::ct->ikGroups->removeIkGroup(objID);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				refresh();
			}
		}
	}
}

void CQDlgIk::on_qqIkEnabled_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->mainSettings->ikCalculationEnabled=!App::ct->mainSettings->ikCalculationEnabled;
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgIk::on_qqAddNewGroup_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CikGroup* newGroup=new CikGroup();
		newGroup->setObjectName("IK_Group");
		App::ct->ikGroups->addIkGroup(newGroup,false);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		int groupID=newGroup->getObjectID();
		updateObjectsInList();
		selectObjectInList(groupID);
		refresh();
	}
}

void CQDlgIk::on_qqList_itemChanged(QListWidgetItem *item)
{
	if (item!=NULL)
	{
		IF_UI_EVENT_CAN_WRITE_DATA
		{
			std::string newName(item->text().toStdString());
			CikGroup* it=App::ct->ikGroups->getIkGroup(item->data(Qt::UserRole).toInt());
			if ( (it!=NULL)&&(newName!="") )
			{
				if (it->getObjectName()!=newName)
				{
					tt::removeIllegalCharacters(newName,true);
					if (App::ct->ikGroups->getIkGroup(newName)==NULL)
					{
						it->setObjectName(newName);
						App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
					}
				}
			}
			refresh();
		}
	}
}

void CQDlgIk::on_qqList_itemSelectionChanged()
{
	if (!noListSelectionAllowed)
	{
		IF_UI_EVENT_CAN_WRITE_DATA
		{
			int objID=getSelectedObjectID();
			CikGroup* it=App::ct->ikGroups->getIkGroup(objID);
			if (it!=NULL)
				((CEditBoxDelegate*)ui->qqList->itemDelegate())->initialText=it->getObjectName();
			else
				((CEditBoxDelegate*)ui->qqList->itemDelegate())->initialText="";
			inListSelectionRoutine=true;
			refresh();
			inListSelectionRoutine=false;
		}
	}
}

void CQDlgIk::on_qqUp_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(getSelectedObjectID());
		if (it==NULL)
			return;

		for (int i=0;i<int(App::ct->ikGroups->ikGroups.size());i++)
		{
			if ( (App::ct->ikGroups->ikGroups[i]->getObjectID()==it->getObjectID())&&
				(i>0) )
			{
				App::ct->ikGroups->ikGroups.erase(App::ct->ikGroups->ikGroups.begin()+i);
				App::ct->ikGroups->ikGroups.insert(App::ct->ikGroups->ikGroups.begin()+i-1,it);
				break;
			}
		}

		// Now we have to check if the getDoOnFailOrSuccessOf is still consistent for all IKGroups:
		std::vector<int> seenIDs;
		for (int i=0;i<int(App::ct->ikGroups->ikGroups.size());i++)
		{
			CikGroup* theGroup=App::ct->ikGroups->ikGroups[i];
			int dep=theGroup->getDoOnFailOrSuccessOf();
			if (dep!=-1)
			{
				bool valid=false;
				for (int j=0;j<int(seenIDs.size());j++)
				{
					if (seenIDs[j]==dep)
					{
						valid=true;
						break;
					}
				}
				if (!valid)
					theGroup->setDoOnFailOrSuccessOf(-1);
			}
			seenIDs.push_back(theGroup->getObjectID());
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgIk::on_qqDown_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(getSelectedObjectID());
		if (it==NULL)
			return;
		for (int i=0;i<int(App::ct->ikGroups->ikGroups.size());i++)
		{
			if ( (App::ct->ikGroups->ikGroups[i]->getObjectID()==it->getObjectID())&&
				(i<int(App::ct->ikGroups->ikGroups.size())-1) )
			{
				App::ct->ikGroups->ikGroups.erase(App::ct->ikGroups->ikGroups.begin()+i);
				App::ct->ikGroups->ikGroups.insert(App::ct->ikGroups->ikGroups.begin()+i+1,it);
				break;
			}
		}
		// Now we have to check if the getDoOnFailOrSuccessOf is still consistent for all IKGroups:
		std::vector<int> seenIDs;
		for (int i=0;i<int(App::ct->ikGroups->ikGroups.size());i++)
		{
			CikGroup* theGroup=App::ct->ikGroups->ikGroups[i];
			int dep=theGroup->getDoOnFailOrSuccessOf();
			if (dep!=-1)
			{
				bool valid=false;
				for (int j=0;j<int(seenIDs.size());j++)
				{
					if (seenIDs[j]==dep)
					{
						valid=true;
						break;
					}
				}
				if (!valid)
					theGroup->setDoOnFailOrSuccessOf(-1);
			}
			seenIDs.push_back(theGroup->getObjectID());
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgIk::on_qqExplicitHandling_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(getSelectedObjectID());
		if (it==NULL)
			return;
		it->setExplicitHandling(!it->getExplicitHandling());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgIk::on_qqGroupIsActive_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(getSelectedObjectID());
		if (it==NULL)
			return;
		it->setActive(!it->getActive());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgIk::on_qqRedundant_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(getSelectedObjectID());
		if (it==NULL)
			return;
		it->setCorrectJointLimits(!it->getCorrectJointLimits());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgIk::on_qqIgnoreMaxStepSizes_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(getSelectedObjectID());
		if (it==NULL)
			return;
		it->setIgnoreMaxStepSizes(!it->getIgnoreMaxStepSizes());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgIk::on_qqCalcMethodCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
		{
			CikGroup* it=App::ct->ikGroups->getIkGroup(getSelectedObjectID());
			if (it==NULL)
				return;
			it->setCalculationMethod(ui->qqCalcMethodCombo->itemData(ui->qqCalcMethodCombo->currentIndex()).toInt());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgIk::on_qqDamping_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(getSelectedObjectID());
		if (it==NULL)
			return;
		bool ok;
		float newVal=ui->qqDamping->text().toFloat(&ok);
		if (ok)
		{
			it->setDlsFactor(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgIk::on_qqMaxIterations_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(getSelectedObjectID());
		if (it==NULL)
			return;
		bool ok;
		int newVal=ui->qqMaxIterations->text().toInt(&ok);
		if (ok)
		{
			it->setMaxIterations(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgIk::on_qqWeightJointLimits_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(getSelectedObjectID());
		if (it==NULL)
			return;
		bool ok;
		float newVal=ui->qqWeightJointLimits->text().toFloat(&ok);
		if (ok)
		{
			it->setJointLimitWeight(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgIk::on_qqWeightObstacleAvoidance_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(getSelectedObjectID());
		if (it==NULL)
			return;
		bool ok;
		float newVal=ui->qqWeightObstacleAvoidance->text().toFloat(&ok);
		if (ok)
		{
			it->setAvoidanceWeight(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgIk::on_qqThresholdLinear_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(getSelectedObjectID());
		if (it==NULL)
			return;
		bool ok;
		float newVal=ui->qqThresholdLinear->text().toFloat(&ok);
		if (ok)
		{
			float v=newVal*gv::userToMeter;
			tt::limitValue(0.0001f,1.0f,v);
			it->setJointTreshholdLinear(v);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgIk::on_qqThresholdAngular_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(getSelectedObjectID());
		if (it==NULL)
			return;
		bool ok;
		float newVal=ui->qqThresholdAngular->text().toFloat(&ok);
		if (ok)
		{
			float v=newVal*gv::userToRad;
			tt::limitValue(0.1f*degToRad,10.0f*degToRad,v);
			it->setJointTreshholdAngular(v);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgIk::on_qqEditConditional_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(getSelectedObjectID());
		if (it==NULL)
			return;
		CQDlgIkConditional theDialog(this);
		theDialog.ikGroup=it;
		theDialog.refresh();
		delKeyShortcut->setEnabled(false);
		backspaceKeyShortcut->setEnabled(false);
		theDialog.makeDialogModal();
		backspaceKeyShortcut->setEnabled(true);
		delKeyShortcut->setEnabled(true);
	}
}

void CQDlgIk::on_qqEditObstacleAvoidance_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(getSelectedObjectID());
		if (it==NULL)
			return;
		CQDlgIkAvoidance theDialog(this);
		theDialog.ikGroup=it;
		theDialog.refresh();
		delKeyShortcut->setEnabled(false);
		backspaceKeyShortcut->setEnabled(false);
		theDialog.makeDialogModal();
		backspaceKeyShortcut->setEnabled(true);
		delKeyShortcut->setEnabled(true);
	}
}

void CQDlgIk::on_qqEditIkElements_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CikGroup* it=App::ct->ikGroups->getIkGroup(getSelectedObjectID());
		if (it==NULL)
			return;
		CQDlgIkElements theDialog(this);
		theDialog.ikGroup=it;
		theDialog.refresh();
		delKeyShortcut->setEnabled(false);
		backspaceKeyShortcut->setEnabled(false);
		theDialog.makeDialogModal();
		backspaceKeyShortcut->setEnabled(true);
		delKeyShortcut->setEnabled(true);
		refresh();
	}
}
