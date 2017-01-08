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
#include "qdlgscripts.h"
#include "ui_qdlgscripts.h"
#include "Tt.h"
#include <QShortcut>
#include "qdlginsertscript.h"
#include "IloIlo.h"
#include "App.h"
#include "v_repStrings.h"
#include "jointObject.h"
#include "ScintillaModalDlg.h"


CQDlgScripts::CQDlgScripts(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgScripts)
{
    ui->setupUi(this);
	inSelectionRoutine=false;
	rebuildingRoutine=false;
	inMainRefreshRoutine=false;
	QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	connect(shortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	QShortcut* shortcut2 = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
	connect(shortcut2,SIGNAL(activated()), this, SLOT(onDeletePressed()));
}

CQDlgScripts::~CQDlgScripts()
{
    delete ui;
}

void CQDlgScripts::refresh()
{
	inMainRefreshRoutine=true;

	int selectedObjectID=getSelectedObjectID();
	updateObjectsInList();
	selectObjectInList(selectedObjectID);

	ui->qqAddNewScript->setEnabled(App::ct->simulation->isSimulationStopped());
	ui->qqScriptList->setEnabled(App::ct->simulation->isSimulationStopped());

	ui->qqExecutionOrder->clear();
	ui->qqAssociatedObjectCombo->clear();

	CLuaScriptObject* theScript=App::ct->luaScriptContainer->getScriptFromID(getSelectedObjectID());
	ui->qqExecutionOrder->setEnabled((theScript!=NULL)&&(theScript->getScriptType()==sim_scripttype_childscript)&&App::ct->simulation->isSimulationStopped());
	ui->qqAssociatedObjectCombo->setEnabled((theScript!=NULL)&&(theScript->getScriptType()==sim_scripttype_childscript)&&App::ct->simulation->isSimulationStopped());
	ui->qqExplicitHandling->setEnabled((theScript!=NULL)&&App::ct->simulation->isSimulationStopped()&&(theScript->getScriptType()==sim_scripttype_childscript)&&(!theScript->getExecuteInSensingPhase()));
	ui->qqThreaded->setEnabled((theScript!=NULL)&&App::ct->simulation->isSimulationStopped()&&(theScript->getScriptType()==sim_scripttype_childscript)&&(!theScript->getExecuteInSensingPhase()));
	ui->qqExecuteOnce->setEnabled((theScript!=NULL)&&App::ct->simulation->isSimulationStopped()&&(theScript->getScriptType()==sim_scripttype_childscript));
	ui->qqExecuteInSensingPhase->setEnabled((theScript!=NULL)&&App::ct->simulation->isSimulationStopped()&&(theScript->getScriptType()==sim_scripttype_childscript)&&(!theScript->getExplicitHandling())&&(!theScript->getThreadedExecution()));

	if (theScript!=NULL)
	{
		if (theScript->getScriptType()==sim_scripttype_childscript)
		{
			ui->qqExecutionOrder->addItem(strTranslate(IDSN_FIRST),QVariant(SCRIPT_EXECUTION_ORDER_FIRST));
			ui->qqExecutionOrder->addItem(strTranslate(IDSN_NORMAL),QVariant(SCRIPT_EXECUTION_ORDER_NORMAL));
			ui->qqExecutionOrder->addItem(strTranslate(IDSN_LAST),QVariant(SCRIPT_EXECUTION_ORDER_LAST));
			ui->qqExecutionOrder->setCurrentIndex(theScript->getExecutionOrder());

			ui->qqAssociatedObjectCombo->addItem(strTranslate(IDSN_NONE),QVariant(-1));
			std::vector<std::string> names;
			std::vector<int> ids;
			for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
			{
				C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
				names.push_back(it->getName());
				ids.push_back(it->getID());
			}
			tt::orderStrings(names,ids);
			for (int i=0;i<int(names.size());i++)
				ui->qqAssociatedObjectCombo->addItem(names[i].c_str(),QVariant(ids[i]));
			for (int i=0;i<ui->qqAssociatedObjectCombo->count();i++)
			{
				if (ui->qqAssociatedObjectCombo->itemData(i).toInt()==theScript->getObjectIDThatScriptIsAttachedTo())
				{
					ui->qqAssociatedObjectCombo->setCurrentIndex(i);
					break;
				}
			}
		}

		ui->qqExplicitHandling->setChecked(theScript->getExplicitHandling());
		ui->qqThreaded->setChecked(theScript->getThreadedExecution());
		ui->qqExecuteOnce->setChecked(theScript->getExecuteJustOnce());
		ui->qqExecuteInSensingPhase->setChecked(theScript->getExecuteInSensingPhase());
	}
	else
	{
		ui->qqExplicitHandling->setChecked(false);
		ui->qqThreaded->setChecked(false);
		ui->qqExecuteOnce->setChecked(false);
		ui->qqExecuteInSensingPhase->setChecked(false);
	}
	inMainRefreshRoutine=false;
}

void CQDlgScripts::updateObjectsInList()
{
	rebuildingRoutine=true;
	ui->qqScriptList->clear();

	// The contact callback script:
	{
	CLuaScriptObject* it=App::ct->luaScriptContainer->getCustomContactHandlingScript_callback();
	if (it!=NULL)
	{
		std::string tmp=it->getDescriptiveName();
		int id=it->getScriptID();
		QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
		itm->setData(Qt::UserRole,QVariant(id));
		itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
		itm->setForeground(QColor(128,128,255));
		ui->qqScriptList->addItem(itm);
	}
	}

	// The add-on scripts:
	for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->allScripts[i];
		int t=it->getScriptType();
		if (t==sim_scripttype_addonscript)
		{
			std::string tmp=it->getDescriptiveName();
			int id=it->getScriptID();
			QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
			itm->setData(Qt::UserRole,QVariant(id));
			itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			itm->setForeground(QColor(64,200,64));
			ui->qqScriptList->addItem(itm);
		}
	}

	// The main script:
	{
	CLuaScriptObject* it=App::ct->luaScriptContainer->getMainScript();
	if (it!=NULL)
	{
		std::string tmp=it->getDescriptiveName();
		int id=it->getScriptID();
		QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
		itm->setData(Qt::UserRole,QVariant(id));
		itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
		itm->setForeground(QColor(255,128,128));
		ui->qqScriptList->addItem(itm);
	}
	}

	// The child scripts:
	for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->allScripts[i];
		int t=it->getScriptType();
		if (t==sim_scripttype_childscript)
		{
			std::string tmp=it->getDescriptiveName();
			int id=it->getScriptID();
			QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
			itm->setData(Qt::UserRole,QVariant(id));
			itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			itm->setForeground(QColor(128,128,128));
			ui->qqScriptList->addItem(itm);
		}
	}

	// The joint custom ctrl callback scripts:
	for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->allScripts[i];
		int t=it->getScriptType();
		if (t==sim_scripttype_jointctrlcallback)
		{
			std::string tmp=it->getDescriptiveName();
			int id=it->getScriptID();
			QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
			itm->setData(Qt::UserRole,QVariant(id));
			itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			itm->setForeground(QColor(225,128,225));
			ui->qqScriptList->addItem(itm);
		}
	}



	rebuildingRoutine=false;
}

int CQDlgScripts::getSelectedObjectID()
{
	QList<QListWidgetItem*> sel=ui->qqScriptList->selectedItems();
	if (sel.size()>0)
		return(sel.at(0)->data(Qt::UserRole).toInt());
	return(-1);
}

void CQDlgScripts::selectObjectInList(int objectID)
{
	for (int i=0;i<ui->qqScriptList->count();i++)
	{
		QListWidgetItem* it=ui->qqScriptList->item(i);
		if (it!=NULL)
		{
			if (it->data(Qt::UserRole).toInt()==objectID)
			{
				inSelectionRoutine=true;
				it->setSelected(true);
				inSelectionRoutine=false;
				break;
			}
		}
	}
}

void CQDlgScripts::onDeletePressed()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (focusWidget()==ui->qqScriptList)
		{
			int scriptID=getSelectedObjectID();
			if (scriptID!=-1)
			{
				CLuaScriptObject* script=App::ct->luaScriptContainer->getScriptFromID(scriptID);
				if (script!=NULL)
				{
					if ((script->getScriptType()==sim_scripttype_mainscript)||(script->getScriptType()==sim_scripttype_childscript))
						App::ct->luaScriptContainer->removeScript(scriptID);
					else if (script->getScriptType()==sim_scripttype_jointctrlcallback)
					{ // a bit more complicated here!
						int objID=script->getObjectIDThatScriptIsAttachedTo_callback();
						C3DObject* it=App::ct->objCont->getObject(objID);
						if (it!=NULL)
						{
							if (it->getObjectType()==sim_object_joint_type)
							{
								CJoint* joint=(CJoint*)it;
								joint->setEnableDynamicMotorCustomControl(false,NULL);
							}
						}
					}
					else if (script->getScriptType()==sim_scripttype_contactcallback)
						App::ct->environment->setEnableCustomContactHandlingViaScript(false,NULL);
					App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
					App::ct->objCont->setLightDialogRefreshFlag();
				}
				refresh();
			}
			return;
		}
	}
}


void CQDlgScripts::on_qqAddNewScript_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		int newScriptID=-1;
		CQDlgInsertScript theDialog(this);
		theDialog.initialize();
		if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
		{
			if (theDialog.scriptType==sim_scripttype_mainscript)
			{
				CLuaScriptObject* it=App::ct->luaScriptContainer->getMainScript();
				if (it!=NULL)
				{
					if (VMessageBox::REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDS_MAIN_SCRIPT),strTranslate(IDS_INFO_NO_MORE_THAN_ONE_MAIN_SCRIPT),VMessageBox::YES_NO))
					{
						App::ct->luaScriptContainer->removeScript(it->getScriptID());
						newScriptID=App::ct->luaScriptContainer->insertDefaultScript_mainAndChildScriptsOnly(sim_scripttype_mainscript,false);
					}
				}
				else
					newScriptID=App::ct->luaScriptContainer->insertDefaultScript_mainAndChildScriptsOnly(sim_scripttype_mainscript,false);
			}
			if (theDialog.scriptType==sim_scripttype_childscript)
				newScriptID=App::ct->luaScriptContainer->insertDefaultScript_mainAndChildScriptsOnly(sim_scripttype_childscript,false);
			if (theDialog.scriptType==(sim_scripttype_childscript|sim_scripttype_threaded))
				newScriptID=App::ct->luaScriptContainer->insertDefaultScript_mainAndChildScriptsOnly(sim_scripttype_childscript,true);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		App::ct->objCont->setLightDialogRefreshFlag();
		refresh();
		selectObjectInList(newScriptID);
	}
}

void CQDlgScripts::on_qqScriptList_itemSelectionChanged()
{ // react only if the user clicked there!
	IF_UI_EVENT_CAN_READ_DATA
	{
		if ((!inSelectionRoutine)&&(!rebuildingRoutine))
			refresh();
	}
}

void CQDlgScripts::on_qqScriptList_itemDoubleClicked(QListWidgetItem *item)
{
	IF_UI_EVENT_CAN_WRITE_DATA // IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		if (item!=NULL)
		{
			CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(item->data(Qt::UserRole).toInt());
			if (it!=NULL)
			{
				if ((it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript))
					App::ct->scintillaEditorContainer->openEditorForScript(it->getScriptID());
				if (it->getScriptType()==sim_scripttype_jointctrlcallback)
				{
					CScintillaModalDlg dlg(App::mainWindow);
					std::string dlgTitle("Callback script associated with '");
					C3DObject* obj=App::ct->objCont->getObject(it->getObjectIDThatScriptIsAttachedTo_callback());
					if (obj!=NULL)
						dlgTitle+=obj->getName();
					dlgTitle+="'";
					dlg.initialize(it->getScriptID(),dlgTitle.c_str(),false);
					dlg.makeDialogModal();
				}
				if (it->getScriptType()==sim_scripttype_contactcallback)
				{
					CScintillaModalDlg dlg(App::mainWindow);
					dlg.initialize(it->getScriptID(),"Contact callback script",false);
					dlg.makeDialogModal();
				}
				if (it->getScriptType()==sim_scripttype_addonscript)
				{
					CScintillaModalDlg dlg(App::mainWindow);
					std::string dlgTitle("Add-on script '");
					dlgTitle+=it->getAddOnName();
					dlgTitle+="' (READ ONLY)";
					dlg.initialize(it->getScriptID(),dlgTitle.c_str(),true);
					dlg.makeDialogModal();
				}
			}
		}
	}
}

void CQDlgScripts::on_qqExplicitHandling_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getSelectedObjectID());
		if (it!=NULL)
		{
			it->setExplicitHandling(!it->getExplicitHandling());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgScripts::on_qqThreaded_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getSelectedObjectID());
		if (it!=NULL)
		{
			it->setThreadedExecution(!it->getThreadedExecution());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		App::ct->objCont->setLightDialogRefreshFlag();
		refresh();
	}
}

void CQDlgScripts::on_qqExecuteOnce_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getSelectedObjectID());
		if (it!=NULL)
		{
			it->setExecuteJustOnce(!it->getExecuteJustOnce());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		App::ct->objCont->setLightDialogRefreshFlag();
		refresh();
	}
}

void CQDlgScripts::on_qqExecuteInSensingPhase_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getSelectedObjectID());
		if (it!=NULL)
		{
			it->setExecuteInSensingPhase(!it->getExecuteInSensingPhase());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		App::ct->objCont->setLightDialogRefreshFlag();
		refresh();
	}
}

void CQDlgScripts::on_qqAssociatedObjectCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!inMainRefreshRoutine)
		{
			CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getSelectedObjectID());
			if (it!=NULL)
			{
				int objID=ui->qqAssociatedObjectCombo->itemData(ui->qqAssociatedObjectCombo->currentIndex()).toInt();
				C3DObject* it2=App::ct->objCont->getObject(objID);
				if (it2!=NULL)
					it->setObjectIDThatScriptIsAttachedTo(objID);
				else
					it->setObjectIDThatScriptIsAttachedTo(-1);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			}
			App::ct->objCont->setLightDialogRefreshFlag(); // will trigger a refresh
		}
	}
}

void CQDlgScripts::on_qqExecutionOrder_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
		{
			CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getSelectedObjectID());
			if (it!=NULL)
			{
				int executionOrder=ui->qqExecutionOrder->itemData(ui->qqExecutionOrder->currentIndex()).toInt();
				it->setExecutionOrder(executionOrder);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			}
		}
	}
}
