// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// -------------------------------------------------------------------
// THIS FILE IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// You are free to use/modify/distribute this file for whatever purpose!
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#include "mtbrobotdialog.h"
#include "ui_mtbrobotdialog.h"
#include "v_repLib.h"
#include "Access.h"

CMtbRobotDialog::CMtbRobotDialog(QWidget *parent) :
	QDialog(parent,Qt::Tool),
    ui(new Ui::CMtbRobotDialog)
{
    ui->setupUi(this);
	inRefreshRoutine=false;
	_simulationStopped=true;
	_lastSelectedObject=-1;
}

CMtbRobotDialog::~CMtbRobotDialog()
{
    delete ui;
}

void CMtbRobotDialog::refresh()
{
	inRefreshRoutine=true;
	// Get handle of last selected object:
	int lastSel=_lastSelectedObject;

	updateObjectsInList();

	// Now initialized other dialog elements:
	CMtbRobot* taggedObj=NULL;
	ui->qqRobotProgram->setPlainText("");

	ui->qqAutomatic->setChecked(false);
	if (lastSel!=-1)
	{
		taggedObj=CAccess::mtbRobotContainer->getFromAssociatedObject(lastSel);
		if (taggedObj==NULL)
			selectObjectInList(-1);
		else
		{ // The last selected object is the base object of a MTB model!
			selectObjectInList(taggedObj->getID());
			std::string prg(taggedObj->getProgram());
			ui->qqRobotProgram->setPlainText(prg.c_str());
			ui->qqAutomatic->setChecked((taggedObj->getOptions()&1)!=0);
		}
	}
	else
		selectObjectInList(-1);
	ui->qqRobotProgram->setEnabled((taggedObj!=NULL)&&_simulationStopped);
	ui->qqAutomatic->setEnabled((taggedObj!=NULL)&&_simulationStopped);
	inRefreshRoutine=false;
}

void CMtbRobotDialog::makeVisible(bool visible)
{
	setVisible(visible);

	SSimulationThreadCommand cmd;
	cmd.cmdId=MAKE_VISIBLE_CMD;
	cmd.boolParams.push_back(visible);
	addCommand(cmd);
}

bool CMtbRobotDialog::getVisible()
{
	return(isVisible());
}

void CMtbRobotDialog::selectObjectInList(int objectHandle)
{ // select an item in the list control:
	for (int i=0;i<ui->qqRobotList->count();i++)
	{
		QListWidgetItem* it=ui->qqRobotList->item(i);
		if (it!=NULL)
		{
			if (it->data(Qt::UserRole).toInt()==objectHandle)
			{
				it->setSelected(true);
				break;
			}
		}
	}
}

int CMtbRobotDialog::getSelectedObjectInList()
{ // get the item selected in the list control:
	QList<QListWidgetItem*> sel=ui->qqRobotList->selectedItems();
	if (sel.size()>0)
	{
		int data=sel.at(0)->data(Qt::UserRole).toInt();
		CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromID(data);
		if (mtbRobot!=NULL)
			return(mtbRobot->getID());
	}
	return(-1);
}

void CMtbRobotDialog::updateObjectsInList()
{
	ui->qqRobotList->clear();
	for (int i=0;i<CAccess::mtbRobotContainer->getCount();i++)
	{
		CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromIndex(i);
		int sceneObjectHandle=mtbRobot->getAssociatedObject();
		std::string txt;
		char* name=NULL;
		if (sceneObjectHandle!=-1)
			name=simGetObjectName(sceneObjectHandle);
		if (name==NULL)
			txt="Error";
		else
		{
			txt=name;
			simReleaseBuffer(name);
		}
		QListWidgetItem* itm=new QListWidgetItem(txt.c_str());
		itm->setData(Qt::UserRole,QVariant(mtbRobot->getID()));
		itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
		ui->qqRobotList->addItem(itm);
	}
}

void CMtbRobotDialog::on_qqAutomatic_clicked()
{
	SSimulationThreadCommand cmd;
	cmd.cmdId=TOGGLE_AUTOMATIC_CMD;
	addCommand(cmd);
}

void CMtbRobotDialog::on_qqRobotList_itemSelectionChanged()
{
	if (!inRefreshRoutine)
	{
		int objectHandle=getSelectedObjectInList();
		SSimulationThreadCommand cmd;
		cmd.cmdId=SELECT_OBJECT_CMD;
		cmd.intParams.push_back(-1); // none
		CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromID(objectHandle);
		if (mtbRobot!=NULL)
		{
			int sceneObjHandle=mtbRobot->getAssociatedObject();
			if (sceneObjHandle!=-1)
				cmd.intParams[0]=sceneObjHandle;
		}
		addCommand(cmd);
	}
}

void CMtbRobotDialog::reject()
{
	makeVisible(false);
	done(0);
}


void CMtbRobotDialog::on_qqRobotProgram_textChanged()
{
	SSimulationThreadCommand cmd;
	cmd.cmdId=UPDATE_PRG_TEXT_CMD;
	cmd.intParams.push_back(getSelectedObjectInList());
	cmd.stringParams.push_back(ui->qqRobotProgram->toPlainText().toStdString());
	addCommand(cmd);
}

void CMtbRobotDialog::setState(bool stopped,int lastSelection)
{
	_simulationStopped=stopped;
	_lastSelectedObject=lastSelection;
}

void CMtbRobotDialog::addCommand(SSimulationThreadCommand cmd)
{
	_simulThreadCommands.push_back(cmd);
}

void CMtbRobotDialog::handleCommands()
{ // Called from the main SIM thread
	for (int i=0;i<int(_simulThreadCommands.size());i++)
	{
		SSimulationThreadCommand cmd=_simulThreadCommands[i];
		if (cmd.cmdId==MAKE_VISIBLE_CMD)
			simSetModuleMenuItemState(dialogMenuItemHandle,(cmd.boolParams[0]?3:1),"MTB Robot Properties");
		if (cmd.cmdId==TOGGLE_AUTOMATIC_CMD)
		{
			int objectHandle=getSelectedObjectInList();
			if (objectHandle!=-1)
			{
				CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromID(objectHandle);
				if (mtbRobot!=NULL)
				{
					mtbRobot->setOptions(mtbRobot->getOptions()^1);
					simAnnounceSceneContentChange(); // so that we have an undo point here
				}
			}
			simRefreshDialogs(2);
		}
		if (cmd.cmdId==SELECT_OBJECT_CMD)
		{
			simRemoveObjectFromSelection(sim_handle_all,-1);
			if (cmd.intParams[0]!=-1)
				simAddObjectToSelection(sim_handle_single,cmd.intParams[0]);
		}
		if (cmd.cmdId==UPDATE_PRG_TEXT_CMD)
		{
			int objectHandle=cmd.intParams[0];
			if (objectHandle!=-1)
			{
				CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromID(objectHandle);
				if (mtbRobot!=NULL)
					mtbRobot->setProgram(cmd.stringParams[0]); // Update the program
			}
		}
	}
	_simulThreadCommands.clear();
}

