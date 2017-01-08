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
#include "qdlgmotionplanning.h"
#include "ui_qdlgmotionplanning.h"
#include "Tt.h"
#include "GV.h"
#include "IloIlo.h"
#include "editboxdelegate.h"
#include "v_repStrings.h"
#include "App.h"
#include <boost/lexical_cast.hpp>
#include "qdlgmotionplanningjoints.h"
#include "qdlgworkspacemetric.h"


CQDlgMotionPlanning::CQDlgMotionPlanning(QWidget *parent) :
	CDlgEx(parent),
	ui(new Ui::CQDlgMotionPlanning)
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

CQDlgMotionPlanning::~CQDlgMotionPlanning()
{
    delete ui;
}

void CQDlgMotionPlanning::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(CALCULATION_DLG);
}

void CQDlgMotionPlanning::refresh()
{
	inMainRefreshRoutine=true;
	bool simStopped=App::ct->simulation->isSimulationStopped();
	bool noEditMode=(App::ct->objCont->getEditModeType()==NO_EDIT_MODE);
	int taskID=getSelectedObjectID();
	CMotionPlanningTask* task=App::ct->motionPlanning->getObject(taskID);

	if (!inListSelectionRoutine)
	{
		updateObjectsInList();
		selectObjectInList(taskID);
	}

	ui->qqAddNew->setEnabled(simStopped&&noEditMode);

	ui->qqIkGroupCombo->setEnabled(simStopped&&noEditMode&&(task!=NULL));
	ui->qqEntity1Combo->setEnabled(simStopped&&noEditMode&&(task!=NULL));
	ui->qqEntity2Combo->setEnabled(simStopped&&noEditMode&&(task!=NULL)&&(task->getRobotSelfCollEntity1()!=-1));
	ui->qqSelfColl->setEnabled(simStopped&&noEditMode&&(task!=NULL)&&(task->getRobotSelfCollEntity1()!=-1));
	ui->qqSelfDist->setEnabled(simStopped&&noEditMode&&(task!=NULL)&&(task->getRobotSelfCollEntity1()!=-1));
	ui->qqSelfDistDist->setEnabled(simStopped&&noEditMode&&(task!=NULL)&&(task->getSelfCollDistanceThreshold()!=0.0f)&&(task->getRobotSelfCollEntity1()!=-1));
	ui->qqRobotCombo->setEnabled(simStopped&&noEditMode&&(task!=NULL));
	ui->qqObstacleCombo->setEnabled(simStopped&&noEditMode&&(task!=NULL)&&(task->getRobotEntity()!=-1));
	ui->qqColl->setEnabled(simStopped&&noEditMode&&(task!=NULL)&&(task->getRobotEntity()!=-1));
	ui->qqDist->setEnabled(simStopped&&noEditMode&&(task!=NULL)&&(task->getRobotEntity()!=-1));
	ui->qqDistDist->setEnabled(simStopped&&noEditMode&&(task!=NULL)&&(task->getDistanceThreshold()!=0.0f)&&(task->getRobotEntity()!=-1));

	ui->qqIkGroupCombo->clear();
	ui->qqEntity1Combo->clear();
	ui->qqEntity2Combo->clear();
	ui->qqRobotCombo->clear();
	ui->qqObstacleCombo->clear();

	ui->qqEditJointProp->setEnabled(simStopped&&noEditMode&&(task!=NULL));
	ui->qqEditWorkspaceMetric->setEnabled(simStopped&&noEditMode&&(task!=NULL));
	ui->qqComputeNodes->setEnabled(simStopped&&noEditMode&&(task!=NULL));
	ui->showPhase1Nodes->setEnabled(noEditMode&&(task!=NULL));

	if (task!=NULL)
	{

		// IK group:
		fillComboWithIkGroups(ui->qqIkGroupCombo);
		if (!selectItemInCombo(ui->qqIkGroupCombo,task->getIkGroup()))
		{ // should not happen
			task->setIkGroup(-1); // none
			ui->qqIkGroupCombo->setCurrentIndex(0);
		}

		if (task->getIkGroup()!=-1)
		{
			std::string warningText;

			CikGroup* ikGroup=App::ct->ikGroups->getIkGroup(task->getIkGroup());
			if (ikGroup->ikElements.size()!=1)
			{
				if (warningText.length()==0)
					warningText="Warning: IK group should contain exactly 1 IK element";
			}

			C3DObject* obj=task->getBaseObject();
			if (obj==NULL)
			{
				ui->qqBase->setText("world");
				if (warningText.length()==0)
					warningText="Warning: base should not be 'world'";
			}
			else
				ui->qqBase->setText(obj->getName().c_str());

			obj=task->getTipObject();
			if (obj==NULL)
			{
				ui->qqTip->setText("-");
				if (warningText.length()==0)
					warningText="Warning: tip is not defined";
			}
			else
				ui->qqTip->setText(obj->getName().c_str());

			obj=task->getTargetObject();
			if (obj==NULL)
			{
				ui->qqTarget->setText("-");
				if (warningText.length()==0)
					warningText="Warning: target is not defined";
			}
			else
				ui->qqTarget->setText(obj->getName().c_str());

			if (warningText.length()==0)
			{
				if (ikGroup->ikElements.size()==1)
				{
					if (!ikGroup->ikElements[0]->getActive())
					{
						if (warningText.length()==0)
							warningText="Warning: IK element is not active";
					}
					if (ikGroup->getCalculationMethod()!=sim_ik_pseudo_inverse_method)
					{
						if (warningText.length()==0)
							warningText="Warning: IK resolution method is not the pseudo inverse";
					}
					if (!ikGroup->getExplicitHandling())
					{
						if (warningText.length()==0)
							warningText="Warning: IK group should be flagged as 'explicit handling'";
					}
				}
			}

			ui->qqWarning->setText(warningText.c_str());
		}
		else
		{
			ui->qqBase->setText("-");
			ui->qqTip->setText("-");
			ui->qqTarget->setText("-");
			ui->qqWarning->setText("Warning: no IK group selected");
		}

		// self-collision entity1:
		fillComboWithCollisionOrDistanceEntities(ui->qqEntity1Combo,task->getSelfCollDistanceThreshold()==0.0f,false,true,-1);
		if (!selectItemInCombo(ui->qqEntity1Combo,task->getRobotSelfCollEntity1()))
		{ // should not happen
			task->setRobotSelfCollEntity1(-1); // none
			ui->qqEntity1Combo->setCurrentIndex(0);
		}

		// self-collentity2:
		fillComboWithCollisionOrDistanceEntities(ui->qqEntity2Combo,task->getSelfCollDistanceThreshold()==0.0f,true,false,task->getRobotSelfCollEntity1());
		if (!selectItemInCombo(ui->qqEntity2Combo,task->getRobotSelfCollEntity2()))
		{ // should not happen
			task->setRobotSelfCollEntity2(-1); // all
			ui->qqEntity2Combo->setCurrentIndex(0);
		}

		ui->qqSelfColl->setChecked(task->getSelfCollDistanceThreshold()==0.0f);
		ui->qqSelfDist->setChecked(task->getSelfCollDistanceThreshold()!=0.0f);

		if (task->getSelfCollDistanceThreshold()!=0.0f)
			ui->qqSelfDistDist->setText(gv::getSizeStr(false,task->getSelfCollDistanceThreshold(),-1).c_str());
		else
			ui->qqSelfDistDist->setText("");


		// robot:
		fillComboWithCollisionOrDistanceEntities(ui->qqRobotCombo,task->getDistanceThreshold()==0.0f,false,true,-1);
		if (!selectItemInCombo(ui->qqRobotCombo,task->getRobotEntity()))
		{ // should not happen
			task->setRobotEntity(-1); // none
			ui->qqRobotCombo->setCurrentIndex(0);
		}

		// obstacles:
		fillComboWithCollisionOrDistanceEntities(ui->qqObstacleCombo,task->getDistanceThreshold()==0.0f,true,false,task->getRobotEntity());
		if (!selectItemInCombo(ui->qqObstacleCombo,task->getObstacleEntity()))
		{ // should not happen
			task->setObstacleEntity(-1); // all
			ui->qqObstacleCombo->setCurrentIndex(0);
		}

		ui->qqColl->setChecked(task->getDistanceThreshold()==0.0f);
		ui->qqDist->setChecked(task->getDistanceThreshold()!=0.0f);

		if (task->getDistanceThreshold()!=0.0f)
			ui->qqDistDist->setText(gv::getSizeStr(false,task->getDistanceThreshold(),-1).c_str());
		else
			ui->qqDistDist->setText("");

		ui->showPhase1Nodes->setChecked(task->getPhase1NodeVisualization());
	}
	else
	{
		ui->qqBase->setText("-");
		ui->qqTip->setText("-");
		ui->qqTarget->setText("-");
		ui->qqWarning->setText("");
		ui->qqSelfColl->setChecked(false);
		ui->qqSelfDist->setChecked(false);
		ui->qqSelfDistDist->setText("");
		ui->qqColl->setChecked(false);
		ui->qqDist->setChecked(false);
		ui->qqDistDist->setText("");
		ui->showPhase1Nodes->setChecked(false);
	}

	inMainRefreshRoutine=false;
}

void CQDlgMotionPlanning::fillComboWithCollisionOrDistanceEntities(QComboBox* combo,bool collisionEntities,bool insertAllAtBeginning,bool insertNoneAtBeginning,int idToExclude)
{
	combo->clear();
	if (insertAllAtBeginning)
	{
		if (collisionEntities)
			combo->addItem(strTranslate(IDS_ALL_OTHER_COLLIDABLE_OBJECTS_IN_SCENE),QVariant(-1));
		else
			combo->addItem(strTranslate(IDS_ALL_OTHER_MEASURABLE_OBJECTS_IN_SCENE),QVariant(-1));
	}
	if (insertNoneAtBeginning)
		combo->addItem(strTranslate(IDSN_NONE),QVariant(-1));

	std::vector<std::string> names;
	std::vector<int> ids;

	// Now collections:
	for (int i=0;i<int(App::ct->collections->allGroups.size());i++)
	{
		CRegGroup* it=App::ct->collections->allGroups[i];
		std::string name(tt::decorateString("[",strTranslate(IDSN_COLLECTION),"] "));
		name+=it->getGroupName();
		names.push_back(name);
		ids.push_back(it->getGroupID());
	}
	tt::orderStrings(names,ids);
	for (int i=0;i<int(names.size());i++)
		combo->addItem(names[i].c_str(),QVariant(ids[i]));

	names.clear();
	ids.clear();

	// Now shapes:
	for (int i=0;i<int(App::ct->objCont->shapeList.size());i++)
	{
		CShape* it=App::ct->objCont->getShape(App::ct->objCont->shapeList[i]);
		if (it->getID()!=idToExclude)
		{
			std::string name(tt::decorateString("[",strTranslate(IDSN_SHAPE),"] "));
			name+=it->getName();
			names.push_back(name);
			ids.push_back(it->getID());
		}
	}
	tt::orderStrings(names,ids);
	for (int i=0;i<int(names.size());i++)
		combo->addItem(names[i].c_str(),QVariant(ids[i]));

	if (!collisionEntities)
	{
		names.clear();
		ids.clear();

		// Now dummies:
		for (int i=0;i<int(App::ct->objCont->dummyList.size());i++)
		{
			CDummy* it=App::ct->objCont->getDummy(App::ct->objCont->dummyList[i]);
			if (it->getID()!=idToExclude)
			{
				std::string name(tt::decorateString("[",strTranslate(IDSN_DUMMY),"] "));
				name+=it->getName();
				names.push_back(name);
				ids.push_back(it->getID());
			}
		}
		tt::orderStrings(names,ids);
		for (int i=0;i<int(names.size());i++)
			combo->addItem(names[i].c_str(),QVariant(ids[i]));
	}
}

void CQDlgMotionPlanning::fillComboWithIkGroups(QComboBox* combo)
{
	combo->clear();
	combo->addItem(strTranslate(IDSN_NONE),QVariant(-1));

	std::vector<std::string> names;
	std::vector<int> ids;

	// Now collections:
	for (int i=0;i<int(App::ct->ikGroups->ikGroups.size());i++)
	{
		CikGroup* it=App::ct->ikGroups->ikGroups[i];
		std::string name=it->getObjectName();
		names.push_back(name);
		ids.push_back(it->getObjectID());
	}
	tt::orderStrings(names,ids);
	for (int i=0;i<int(names.size());i++)
		combo->addItem(names[i].c_str(),QVariant(ids[i]));
}

bool CQDlgMotionPlanning::selectItemInCombo(QComboBox* combo,int itemID)
{
	for (int i=0;i<combo->count();i++)
	{
		if (combo->itemData(i).toInt()==itemID)
		{
			combo->setCurrentIndex(i);
			return(true);
		}
	}
	return(false);
}

void CQDlgMotionPlanning::updateObjectsInList()
{
	noListSelectionAllowed=true;
	ui->qqList->clear();

	for (int i=0;i<int(App::ct->motionPlanning->allObjects.size());i++)
	{
		CMotionPlanningTask* it=App::ct->motionPlanning->allObjects[i];
		std::string tmp=it->getObjectName();
		tmp+=" (";
		tmp+=boost::lexical_cast<std::string>(it->getJointCount());
		tmp+=" joints)";
		QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
		itm->setData(Qt::UserRole,QVariant(it->getObjectID()));
		itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
		ui->qqList->addItem(itm);
	}

	noListSelectionAllowed=false;
}

int CQDlgMotionPlanning::getSelectedObjectID()
{
	QList<QListWidgetItem*> sel=ui->qqList->selectedItems();
	if (sel.size()>0)
		return(sel.at(0)->data(Qt::UserRole).toInt());
	return(-1);
}

void CQDlgMotionPlanning::selectObjectInList(int objectID)
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

void CQDlgMotionPlanning::onDeletePressed()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (focusWidget()==ui->qqList)
		{
			int objID=getSelectedObjectID();
			if (objID!=-1)
			{
				App::ct->motionPlanning->removeObject(objID);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				refresh();
			}
			return;
		}
	}
}


void CQDlgMotionPlanning::on_qqList_itemChanged(QListWidgetItem *item)
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (item!=NULL)
		{
			std::string newName(item->text().toStdString());
			CMotionPlanningTask* it=App::ct->motionPlanning->getObject(item->data(Qt::UserRole).toInt());
			if ( (it!=NULL)&&(newName!="") )
			{
				if (it->getObjectName()!=newName)
				{
					tt::removeIllegalCharacters(newName,true);
					if (App::ct->motionPlanning->getObject(newName)==NULL)
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

void CQDlgMotionPlanning::on_qqList_itemSelectionChanged()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!noListSelectionAllowed)
		{
			int objID=getSelectedObjectID();
			CMotionPlanningTask* it=App::ct->motionPlanning->getObject(objID);
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

void CQDlgMotionPlanning::on_qqAddNew_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		std::vector<int> joints;
		App::ct->objCont->getSelectedObjects(joints);
		bool ok=(joints.size()>0);
		for (int i=0;i<int(joints.size());i++)
		{
			CJoint* joint=App::ct->objCont->getJoint(joints[i]);
			if (joint!=NULL)
			{
				if (joint->getJointType()==sim_joint_spherical_subtype)
					ok=false;
			}
			else
				ok=false;
		}
		if (ok)
		{
			App::ct->objCont->deselectObjects();
			CMotionPlanningTask* task=new CMotionPlanningTask(joints);
			App::ct->motionPlanning->addObject(task,false);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		else
			App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_MOTION_PLANNING),strTranslate(IDSN_MOTION_PLANNING_SELECTION_INVALID),VMessageBox::OKELI);
	}
}

void CQDlgMotionPlanning::on_qqEditJointProp_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CMotionPlanningTask* task=App::ct->motionPlanning->getObject(getSelectedObjectID());
		if (task==NULL)
			return;
		CQDlgMotionPlanningJoints theDialog(this);
		delKeyShortcut->setEnabled(false);
		backspaceKeyShortcut->setEnabled(false);
		theDialog.taskID=getSelectedObjectID();
		theDialog.refresh();
		theDialog.makeDialogModal();
		backspaceKeyShortcut->setEnabled(true);
		delKeyShortcut->setEnabled(true);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgMotionPlanning::on_qqIkGroupCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!inMainRefreshRoutine)
		{
			CMotionPlanningTask* task=App::ct->motionPlanning->getObject(getSelectedObjectID());
			if (task==NULL)
				return;
			task->setIkGroup(ui->qqIkGroupCombo->itemData(ui->qqIkGroupCombo->currentIndex()).toInt());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgMotionPlanning::on_qqEntity1Combo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
		{
			CMotionPlanningTask* task=App::ct->motionPlanning->getObject(getSelectedObjectID());
			if (task==NULL)
				return;
			task->setRobotSelfCollEntity1(ui->qqEntity1Combo->itemData(ui->qqEntity1Combo->currentIndex()).toInt());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgMotionPlanning::on_qqEntity2Combo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
		{
			CMotionPlanningTask* task=App::ct->motionPlanning->getObject(getSelectedObjectID());
			if (task==NULL)
				return;
			task->setRobotSelfCollEntity2(ui->qqEntity2Combo->itemData(ui->qqEntity2Combo->currentIndex()).toInt());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgMotionPlanning::on_qqSelfColl_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CMotionPlanningTask* task=App::ct->motionPlanning->getObject(getSelectedObjectID());
		if (task==NULL)
			return;
		task->setSelfCollDistanceThreshold(0.0f);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgMotionPlanning::on_qqSelfDist_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CMotionPlanningTask* task=App::ct->motionPlanning->getObject(getSelectedObjectID());
		if (task==NULL)
			return;
		task->setSelfCollDistanceThreshold(0.02f);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgMotionPlanning::on_qqSelfDistDist_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CMotionPlanningTask* task=App::ct->motionPlanning->getObject(getSelectedObjectID());
		if (task==NULL)
			return;
		bool ok;
		float newVal=ui->qqSelfDistDist->text().toFloat(&ok);
		if (ok)
		{
			task->setSelfCollDistanceThreshold(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgMotionPlanning::on_qqRobotCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
		{
			CMotionPlanningTask* task=App::ct->motionPlanning->getObject(getSelectedObjectID());
			if (task==NULL)
				return;
			task->setRobotEntity(ui->qqRobotCombo->itemData(ui->qqRobotCombo->currentIndex()).toInt());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgMotionPlanning::on_qqObstacleCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
		{
			CMotionPlanningTask* task=App::ct->motionPlanning->getObject(getSelectedObjectID());
			if (task==NULL)
				return;
			task->setObstacleEntity(ui->qqObstacleCombo->itemData(ui->qqObstacleCombo->currentIndex()).toInt());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgMotionPlanning::on_qqColl_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CMotionPlanningTask* task=App::ct->motionPlanning->getObject(getSelectedObjectID());
		if (task==NULL)
			return;
		task->setDistanceThreshold(0.0f);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgMotionPlanning::on_qqDist_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CMotionPlanningTask* task=App::ct->motionPlanning->getObject(getSelectedObjectID());
		if (task==NULL)
			return;
		task->setDistanceThreshold(0.02f);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgMotionPlanning::on_qqDistDist_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CMotionPlanningTask* task=App::ct->motionPlanning->getObject(getSelectedObjectID());
		if (task==NULL)
			return;
		bool ok;
		float newVal=ui->qqDistDist->text().toFloat(&ok);
		if (ok)
		{
			task->setDistanceThreshold(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgMotionPlanning::on_qqEditWorkspaceMetric_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CQDlgWorkSpaceMetric theDialog(this);
		delKeyShortcut->setEnabled(false);
		backspaceKeyShortcut->setEnabled(false);
		theDialog.taskID=getSelectedObjectID();
		theDialog.refresh();
		theDialog.makeDialogModal();
		backspaceKeyShortcut->setEnabled(true);
		delKeyShortcut->setEnabled(true);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgMotionPlanning::on_qqComputeNodes_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CMotionPlanningTask* task=App::ct->motionPlanning->getObject(getSelectedObjectID());
		if (task==NULL)
			return;
		task->clearDataStructure();
		int collidingNodes,totalNodes;
		if (task->calculateDataStructureIfNeeded(&collidingNodes,&totalNodes,false))
		{
			std::string tmp(IDSN_DONE_TOTAL_NODES);
			tmp+=tt::FNb(0,totalNodes);
			App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_MOTION_PLANNING),tmp.c_str(),VMessageBox::OKELI);
		}
		else
			App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_MOTION_PLANNING),strTranslate(IDSN_MOTION_PLANNING_PARAMETERS_ILL_DEFINED),VMessageBox::OKELI);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgMotionPlanning::on_showPhase1Nodes_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CMotionPlanningTask* task=App::ct->motionPlanning->getObject(getSelectedObjectID());
		if (task==NULL)
			return;
		task->setPhase1NodeVisualization(!task->getPhase1NodeVisualization());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}
