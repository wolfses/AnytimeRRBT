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
#include "qdlgpathplanning.h"
#include "ui_qdlgpathplanning.h"
#include "Tt.h"
#include "GV.h"
#include "App.h"
#include "editboxdelegate.h"
#include "qdlgpathplanningaddnew.h"
#include "qdlgpathplanningparams.h"
#include "v_repStrings.h"

CQDlgPathPlanning::CQDlgPathPlanning(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgPathPlanning)
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

CQDlgPathPlanning::~CQDlgPathPlanning()
{
    delete ui;
}

void CQDlgPathPlanning::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(CALCULATION_DLG);
}

void CQDlgPathPlanning::refresh()
{
	inMainRefreshRoutine=true;
	bool simStopped=App::ct->simulation->isSimulationStopped();
	bool noEditMode=(App::ct->objCont->getEditModeType()==NO_EDIT_MODE);
	int taskID=getSelectedObjectID();
	CPathPlanningTask* task=App::ct->pathPlanning->getObject(taskID);

	if (!inListSelectionRoutine)
	{
		updateObjectsInList();
		selectObjectInList(taskID);
	}

	ui->qqAddNew->setEnabled(simStopped&&noEditMode);

	ui->qqGoalCombo->setEnabled(simStopped&&noEditMode&&(task!=NULL));
	ui->qqPathCombo->setEnabled(simStopped&&noEditMode&&(task!=NULL));
	ui->qqCheckTypeCombo->setEnabled(simStopped&&noEditMode&&(task!=NULL));
	ui->qqRobotCombo->setEnabled(simStopped&&noEditMode&&(task!=NULL));
	ui->qqObstacleCombo->setEnabled(simStopped&&noEditMode&&(task!=NULL));
	ui->qqMinDist->setEnabled(simStopped&&noEditMode&&(task!=NULL)&&(!task->getCollisionDetection()));
	ui->qqMaxDist->setEnabled(simStopped&&noEditMode&&(task!=NULL)&&(!task->getCollisionDetection())&&task->getObstacleMaxDistanceEnabled());
	ui->qqAdjustSearchParameters->setEnabled(simStopped&&noEditMode&&(task!=NULL));
	ui->qqUsePartialPath->setEnabled(simStopped&&noEditMode&&(task!=NULL));
	ui->qqShowNodes->setEnabled(simStopped&&noEditMode&&(task!=NULL));
	ui->qqMaxCalcTime->setEnabled(simStopped&&noEditMode&&(task!=NULL));
	ui->qqPostProcessingPasses->setEnabled(simStopped&&noEditMode&&(task!=NULL));

	ui->qqComputePath->setEnabled(simStopped&&noEditMode&&(task!=NULL)&&task->isTaskValid());


	ui->qqGoalCombo->clear();
	ui->qqPathCombo->clear();
	ui->qqCheckTypeCombo->clear();
	ui->qqRobotCombo->clear();
	ui->qqObstacleCombo->clear();
	ui->qqUsePartialPath->setChecked((task!=NULL)&&task->getPartialPathIsOk());
	ui->qqShowNodes->setChecked((task!=NULL)&&task->getShowSearchTrees());

	if (task!=NULL)
	{
		if (task->getCollisionDetection())
		{
			ui->qqMinDist->setText("");
			ui->qqMaxDist->setText("");
		}
		else
		{
			ui->qqMinDist->setText(gv::getSizeStr(false,task->getObstacleClearance(),0).c_str());
			if (task->getObstacleMaxDistanceEnabled())
				ui->qqMaxDist->setText(gv::getSizeStr(false,task->getObstacleMaxDistance(),0).c_str());
			else
				ui->qqMaxDist->setText("");
		}
		ui->qqMaxCalcTime->setText(gv::getTimeStr(task->getMaxTime()).c_str());
		ui->qqPostProcessingPasses->setText(tt::FNb(task->getPostProcessingPassCount()).c_str());

		ui->qqCheckTypeCombo->addItem(strTranslate(IDSN_CHECK_ROBOT_OBSTACLE_COLLISION),QVariant(0));
		ui->qqCheckTypeCombo->addItem(strTranslate(IDSN_CHECK_ROBOT_OBSTACLE_MINIMUM_DISTANCE),QVariant(1));
		ui->qqCheckTypeCombo->addItem(strTranslate(IDSN_CHECK_ROBOT_OBSTACLE_MINIMUM_AND_MAXIMUM_DISTANCE),QVariant(2));
		if (task->getCollisionDetection())
			ui->qqCheckTypeCombo->setCurrentIndex(0);
		else
		{
			if (task->getObstacleMaxDistanceEnabled())
				ui->qqCheckTypeCombo->setCurrentIndex(2);
			else
				ui->qqCheckTypeCombo->setCurrentIndex(1);
		}

		std::vector<std::string> names;
		std::vector<int> ids;

		// Goal dummy
		names.clear();
		ids.clear();
		ui->qqGoalCombo->addItem(strTranslate(IDSN_NONE),QVariant(-1));
		for (int i=0;i<int(App::ct->objCont->dummyList.size());i++)
		{
			CDummy* it2=App::ct->objCont->getDummy(App::ct->objCont->dummyList[i]);
			if (it2->getID()!=task->getStartDummyID())
			{
				names.push_back(it2->getName());
				ids.push_back(it2->getID());
			}
		}
		tt::orderStrings(names,ids);
		for (int i=0;i<int(names.size());i++)
			ui->qqGoalCombo->addItem(names[i].c_str(),QVariant(ids[i]));
		for (int i=0;i<ui->qqGoalCombo->count();i++)
		{
			if (ui->qqGoalCombo->itemData(i).toInt()==task->getGoalDummyID())
			{
				ui->qqGoalCombo->setCurrentIndex(i);
				break;
			}
		}

		// Path object
		names.clear();
		ids.clear();
		ui->qqPathCombo->addItem(strTranslate(IDSN_NONE),QVariant(-1));
		for (int i=0;i<int(App::ct->objCont->pathList.size());i++)
		{
			CPath* it2=App::ct->objCont->getPath(App::ct->objCont->pathList[i]);
			names.push_back(it2->getName());
			ids.push_back(it2->getID());
		}
		tt::orderStrings(names,ids);
		for (int i=0;i<int(names.size());i++)
			ui->qqPathCombo->addItem(names[i].c_str(),QVariant(ids[i]));
		for (int i=0;i<ui->qqPathCombo->count();i++)
		{
			if (ui->qqPathCombo->itemData(i).toInt()==task->getPathID())
			{
				ui->qqPathCombo->setCurrentIndex(i);
				break;
			}
		}

		// robot:
		fillComboWithCollisionOrDistanceEntities(ui->qqRobotCombo,task->getCollisionDetection(),false,true,-1);
		if (!selectItemInCombo(ui->qqRobotCombo,task->getRobotEntityID()))
		{ // should not happen
			task->setRobotEntityID(-1); // none
			ui->qqRobotCombo->setCurrentIndex(0);
		}

		// obstacles:
		fillComboWithCollisionOrDistanceEntities(ui->qqObstacleCombo,task->getCollisionDetection(),true,false,task->getRobotEntityID());
		if (!selectItemInCombo(ui->qqObstacleCombo,task->getObstacleEntityID()))
		{ // should not happen
			task->setObstacleEntityID(-1); // all
			ui->qqObstacleCombo->setCurrentIndex(0);
		}

	}
	else
	{
		ui->qqMinDist->setText("");
		ui->qqMaxDist->setText("");
		ui->qqMaxCalcTime->setText("");
		ui->qqPostProcessingPasses->setText("");
	}


	inMainRefreshRoutine=false;
}

void CQDlgPathPlanning::fillComboWithCollisionOrDistanceEntities(QComboBox* combo,bool collisionEntities,bool insertAllAtBeginning,bool insertNoneAtBeginning,int idToExclude)
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

bool CQDlgPathPlanning::selectItemInCombo(QComboBox* combo,int itemID)
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

void CQDlgPathPlanning::updateObjectsInList()
{
	noListSelectionAllowed=true;
	ui->qqList->clear();

	for (int i=0;i<int(App::ct->pathPlanning->allObjects.size());i++)
	{
		CPathPlanningTask* it=App::ct->pathPlanning->allObjects[i];
		std::string tmp=it->getObjectName();
		if (it->getPathPlanningType()==sim_pathplanning_holonomic)
		{ // We have a holonomic path planning here:
			CDummy* startDummy=App::ct->objCont->getDummy(it->getStartDummyID());
			tmp=tmp.append(" [");
			tmp=tmp.append(startDummy->getName().c_str());
			tmp=tmp.append("] (");
			tmp=tmp.append(IDSN_HOLONOMIC);
			tmp=tmp.append(")");
		}
		if (it->getPathPlanningType()==sim_pathplanning_nonholonomic)
		{ // We have a holonomic path planning here:
			CDummy* startDummy=App::ct->objCont->getDummy(it->getStartDummyID());
			tmp=tmp.append(" [");
			tmp=tmp.append(startDummy->getName().c_str());
			tmp=tmp.append("] (");
			tmp=tmp.append(IDSN_NON_HOLONOMIC);
			tmp=tmp.append(")");
		}
		QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
		itm->setData(Qt::UserRole,QVariant(it->getObjectID()));
		itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
		ui->qqList->addItem(itm);
	}

	noListSelectionAllowed=false;
}

int CQDlgPathPlanning::getSelectedObjectID()
{
	QList<QListWidgetItem*> sel=ui->qqList->selectedItems();
	if (sel.size()>0)
		return(sel.at(0)->data(Qt::UserRole).toInt());
	return(-1);
}

void CQDlgPathPlanning::selectObjectInList(int objectID)
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

void CQDlgPathPlanning::onDeletePressed()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (focusWidget()==ui->qqList)
		{
			int objID=getSelectedObjectID();
			if (objID!=-1)
			{
				App::ct->pathPlanning->removeObject(objID);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				refresh();
			}
			return;
		}
	}
}


void CQDlgPathPlanning::on_qqList_itemChanged(QListWidgetItem *item)
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (item!=NULL)
		{
			std::string newName(item->text().toStdString());
			CPathPlanningTask* it=App::ct->pathPlanning->getObject(item->data(Qt::UserRole).toInt());
			if ( (it!=NULL)&&(newName!="") )
			{
				if (it->getObjectName()!=newName)
				{
					tt::removeIllegalCharacters(newName,true);
					if (App::ct->pathPlanning->getObject(newName)==NULL)
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

void CQDlgPathPlanning::on_qqList_itemSelectionChanged()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!noListSelectionAllowed)
		{
			int objID=getSelectedObjectID();
			CPathPlanningTask* it=App::ct->pathPlanning->getObject(objID);
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

void CQDlgPathPlanning::on_qqAddNew_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CQDlgPathPlanningAddNew theDialog(this);
		delKeyShortcut->setEnabled(false);
		backspaceKeyShortcut->setEnabled(false);
		theDialog.refresh();
		if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
		{
			CDummy* it=App::ct->objCont->getDummy(theDialog.selectedDummy);
			if (it!=NULL)
			{
				CPathPlanningTask* task=new CPathPlanningTask(it->getID(),theDialog.selectedTaskType);
				App::ct->pathPlanning->addObject(task,false);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				updateObjectsInList();
				selectObjectInList(task->getObjectID());
				refresh();
			}
		}
		backspaceKeyShortcut->setEnabled(true);
		delKeyShortcut->setEnabled(true);
	}
}

void CQDlgPathPlanning::on_qqGoalCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
		{
			CPathPlanningTask* task=App::ct->pathPlanning->getObject(getSelectedObjectID());
			if (task==NULL)
				return;
			task->setGoalDummyID(ui->qqGoalCombo->itemData(ui->qqGoalCombo->currentIndex()).toInt());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgPathPlanning::on_qqPathCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
		{
			CPathPlanningTask* task=App::ct->pathPlanning->getObject(getSelectedObjectID());
			if (task==NULL)
				return;
			task->setPathID(ui->qqPathCombo->itemData(ui->qqPathCombo->currentIndex()).toInt());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgPathPlanning::on_qqCheckTypeCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
		{
			CPathPlanningTask* task=App::ct->pathPlanning->getObject(getSelectedObjectID());
			if (task==NULL)
				return;
			int dat=ui->qqCheckTypeCombo->itemData(ui->qqCheckTypeCombo->currentIndex()).toInt();
			task->setCollisionDetection(dat==0);
			task->setObstacleMaxDistanceEnabled(dat==2);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgPathPlanning::on_qqRobotCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
		{
			CPathPlanningTask* task=App::ct->pathPlanning->getObject(getSelectedObjectID());
			if (task==NULL)
				return;
			task->setRobotEntityID(ui->qqRobotCombo->itemData(ui->qqRobotCombo->currentIndex()).toInt());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgPathPlanning::on_qqObstacleCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
		{
			CPathPlanningTask* task=App::ct->pathPlanning->getObject(getSelectedObjectID());
			if (task==NULL)
				return;
			task->setObstacleEntityID(ui->qqObstacleCombo->itemData(ui->qqObstacleCombo->currentIndex()).toInt());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgPathPlanning::on_qqMinDist_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathPlanningTask* task=App::ct->pathPlanning->getObject(getSelectedObjectID());
		if (task==NULL)
			return;
		bool ok;
		float newVal=ui->qqMinDist->text().toFloat(&ok);
		if (ok)
		{
			task->setObstacleClearance(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgPathPlanning::on_qqMaxDist_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathPlanningTask* task=App::ct->pathPlanning->getObject(getSelectedObjectID());
		if (task==NULL)
			return;
		bool ok;
		float newVal=ui->qqMaxDist->text().toFloat(&ok);
		if (ok)
		{
			task->setObstacleMaxDistance(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgPathPlanning::on_qqAdjustSearchParameters_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CQDlgPathPlanningParams theDialog(this);
		delKeyShortcut->setEnabled(false);
		backspaceKeyShortcut->setEnabled(false);
		theDialog.taskID=getSelectedObjectID();
		theDialog.refresh();
		theDialog.makeDialogModal();
		backspaceKeyShortcut->setEnabled(true);
		delKeyShortcut->setEnabled(true);
		refresh();
	}
}

void CQDlgPathPlanning::on_qqUsePartialPath_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathPlanningTask* task=App::ct->pathPlanning->getObject(getSelectedObjectID());
		if (task==NULL)
			return;
		task->setPartialPathIsOk(!task->getPartialPathIsOk());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgPathPlanning::on_qqShowNodes_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathPlanningTask* task=App::ct->pathPlanning->getObject(getSelectedObjectID());
		if (task==NULL)
			return;
		task->setShowSearchTrees(!task->getShowSearchTrees());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgPathPlanning::on_qqMaxCalcTime_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathPlanningTask* task=App::ct->pathPlanning->getObject(getSelectedObjectID());
		if (task==NULL)
			return;
		bool ok;
		float newVal=ui->qqMaxCalcTime->text().toFloat(&ok);
		if (ok)
		{
			task->setMaxTime(newVal*gv::userToSeconds);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgPathPlanning::on_qqPostProcessingPasses_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPathPlanningTask* task=App::ct->pathPlanning->getObject(getSelectedObjectID());
		if (task==NULL)
			return;
		bool ok;
		int newVal=ui->qqPostProcessingPasses->text().toInt(&ok);
		if (ok)
		{
			task->setPostProcessingPassCount(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgPathPlanning::on_qqComputePath_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CPathPlanningTask* task=App::ct->pathPlanning->getObject(getSelectedObjectID());
		if (task==NULL)
			return;
		static bool inside=false;
		if (inside)
			return;
		inside=true;
		task->performSearch(true,0.0f);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		inside=false;
	}
}
