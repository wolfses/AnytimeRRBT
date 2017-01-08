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
#include "qdlgscriptparameters.h"
#include "ui_qdlgscriptparameters.h"
#include <QShortcut>
#include "editboxdelegate.h"
#include "App.h"
#include "Tt.h"

CQDlgScriptParameters::CQDlgScriptParameters(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgScriptParameters)
{
    ui->setupUi(this);
	inSelectionRoutine=false;
	QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	connect(shortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	QShortcut* shortcut2 = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
	connect(shortcut2,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	CEditBoxDelegate* delegate=new CEditBoxDelegate();
	ui->qqParameterList->setItemDelegate(delegate);
}

CQDlgScriptParameters::~CQDlgScriptParameters()
{
    delete ui;
}

void CQDlgScriptParameters::cancelEvent()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgScriptParameters::okEvent()
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgScriptParameters::refresh()
{
	ui->qqAddNew->setEnabled(App::ct->simulation->isSimulationStopped());
//	CLuaScriptParameters* it=script->getScriptParametersObject();
	int selectedObjectID=getSelectedObjectID();

	if (!inSelectionRoutine)
	{
		updateObjectsInList();
		selectObjectInList(selectedObjectID);
	}

	refreshPart2();
}

void CQDlgScriptParameters::refreshPart2()
{
	ui->qqAddNew->setEnabled(App::ct->simulation->isSimulationStopped());
	CLuaScriptParameters* it=script->getScriptParametersObject();
	int selectedObjectID=getSelectedObjectID();

	bool sel=(selectedObjectID>=0)&&(selectedObjectID<int(it->parameterNames.size()));

	ui->qqValue->setEnabled(sel&&((it->parameterProperties[selectedObjectID]&4)==0));
	ui->qqUnit->setEnabled(sel&&App::ct->simulation->isSimulationStopped());
	ui->qqPrivate->setEnabled(sel&&App::ct->simulation->isSimulationStopped());
	ui->qqPersistent->setEnabled(sel&&App::ct->simulation->isSimulationStopped());

	if (sel)
	{
		if (it->parameterProperties[selectedObjectID]&4)
			ui->qqValue->setText("Contains embedded zeros");
		else
			ui->qqValue->setText(it->parameterValues[selectedObjectID].c_str());
		ui->qqUnit->setText(it->parameterUnits[selectedObjectID].c_str());
		ui->qqPrivate->setChecked(it->parameterProperties[selectedObjectID]&1);
		ui->qqPersistent->setChecked((it->parameterProperties[selectedObjectID]&2)!=0);
	}
	else
	{
		ui->qqValue->setText("");
		ui->qqUnit->setText("");
		ui->qqPrivate->setChecked(false);
		ui->qqPersistent->setChecked(false);
	}
}

void CQDlgScriptParameters::on_qqAddNew_clicked()
{
	CLuaScriptParameters* p=script->getScriptParametersObject();
	std::string name("defaultVariableName");
	std::string dummy;
	while (p->getParameterValue(name.c_str(),dummy))
		name=tt::generateNewName_noDash(name);
	script->getScriptParametersObject()->addParameterValue(name.c_str(),"defaultUnit","defaultValue",12);
	App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
	updateObjectsInList();
	selectObjectInList(p->parameterNames.size()-1);
	refresh();
}

void CQDlgScriptParameters::on_qqParameterList_itemChanged(QListWidgetItem *item)
{
	if (item!=NULL)
	{
		std::string newName(item->text().toStdString());
		CLuaScriptParameters* it=script->getScriptParametersObject();
		int ind=item->data(Qt::UserRole).toInt();
		if ( (ind>=0)&&(ind<int(it->parameterNames.size())) )
		{
			std::string dummy;
			if (!it->getParameterValue(newName.c_str(),dummy))
			{
				if (newName.length()!=0)
				{
					it->parameterNames[ind]=newName;
					App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
				}
			}
		}
		refresh();
	}
}

void CQDlgScriptParameters::on_qqValue_editingFinished()
{
	CLuaScriptParameters* it=script->getScriptParametersObject();
	int _itemIndex=getSelectedObjectID();
	if ( (_itemIndex>=0)&&(_itemIndex<int(it->parameterNames.size())) )
	{
		it->parameterValues[_itemIndex]=ui->qqValue->text().toStdString();
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
	}
	refreshPart2();
}

void CQDlgScriptParameters::on_qqUnit_editingFinished()
{
	CLuaScriptParameters* it=script->getScriptParametersObject();
	int _itemIndex=getSelectedObjectID();
	if ( (_itemIndex>=0)&&(_itemIndex<int(it->parameterNames.size())) )
	{
		it->parameterUnits[_itemIndex]=ui->qqUnit->text().toStdString();
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
	}
	refreshPart2();
}

void CQDlgScriptParameters::on_qqPrivate_clicked()
{
	CLuaScriptParameters* it=script->getScriptParametersObject();
	int _itemIndex=getSelectedObjectID();
	if ( (_itemIndex>=0)&&(_itemIndex<int(it->parameterNames.size())) )
	{
		it->parameterProperties[_itemIndex]^=1;
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
	}
	refreshPart2();
}

void CQDlgScriptParameters::on_qqPersistent_clicked()
{
	CLuaScriptParameters* it=script->getScriptParametersObject();
	int _itemIndex=getSelectedObjectID();
	if ( (_itemIndex>=0)&&(_itemIndex<int(it->parameterNames.size())) )
	{
		it->parameterProperties[_itemIndex]^=2;
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
	}
	refreshPart2();
}

void CQDlgScriptParameters::on_qqClose_clicked(QAbstractButton *button)
{
	defaultModalDialogEndRoutine(true);
}

void CQDlgScriptParameters::onDeletePressed()
{
	if (focusWidget()==ui->qqParameterList)
	{
		int dataID=getSelectedObjectID();
		if (dataID!=-1)
		{
			CLuaScriptParameters* p=script->getScriptParametersObject();
			p->removeParameterValue(dataID);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
		return;
	}
}

void CQDlgScriptParameters::updateObjectsInList()
{
	ui->qqParameterList->clear();
	CLuaScriptParameters* it=script->getScriptParametersObject();
	for (int i=0;i<int(it->parameterNames.size());i++)
	{
		if ( ((it->parameterProperties[i]&1)==0)||App::ct->simulation->isSimulationStopped() )
		{
			QListWidgetItem* itm=new QListWidgetItem(it->parameterNames[i].c_str());
			itm->setData(Qt::UserRole,QVariant(i));
			itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
			ui->qqParameterList->addItem(itm);
		}
	}
}

int CQDlgScriptParameters::getSelectedObjectID()
{
	QList<QListWidgetItem*> sel=ui->qqParameterList->selectedItems();
	if (sel.size()>0)
		return(sel.at(0)->data(Qt::UserRole).toInt());
	return(-1);
}

void CQDlgScriptParameters::selectObjectInList(int objectID)
{
	for (int i=0;i<ui->qqParameterList->count();i++)
	{
		QListWidgetItem* it=ui->qqParameterList->item(i);
		if (it!=NULL)
		{
			if (it->data(Qt::UserRole).toInt()==objectID)
			{
				it->setSelected(true);
				break;
			}
		}
	}
}

void CQDlgScriptParameters::on_qqParameterList_itemSelectionChanged()
{
	int objID=getSelectedObjectID();
	if ( (objID>=0)&&(objID<int(script->getScriptParametersObject()->parameterNames.size())) )
		((CEditBoxDelegate*)ui->qqParameterList->itemDelegate())->initialText=script->getScriptParametersObject()->parameterNames[objID].c_str();
	else
		((CEditBoxDelegate*)ui->qqParameterList->itemDelegate())->initialText="";
	inSelectionRoutine=true;
	refresh();
	inSelectionRoutine=false;
}
