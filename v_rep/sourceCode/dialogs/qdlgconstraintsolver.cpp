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
#include "qdlgconstraintsolver.h"
#include "ui_qdlgconstraintsolver.h"
#include "Tt.h"
#include "GV.h"
#include <QShortcut>
#include "editboxdelegate.h"
#include "GeometricConstraintSolverInt.h"
#include "IloIlo.h"
#include "v_repStrings.h"
#include "App.h"

CQDlgConstraintSolver::CQDlgConstraintSolver(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgConstraintSolver)
{
	ui->setupUi(this);
	inSelectionRoutine=false;
	QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	connect(shortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	QShortcut* shortcut2 = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
	connect(shortcut2,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	CEditBoxDelegate* delegate=new CEditBoxDelegate();
	ui->qqObjectList->setItemDelegate(delegate);
}

CQDlgConstraintSolver::~CQDlgConstraintSolver()
{
    delete ui;
}

void CQDlgConstraintSolver::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(CALCULATION_DLG);
}

void CQDlgConstraintSolver::refresh()
{
	bool sel1=(App::ct->objCont->getSelSize()==1);
	bool noSim=App::ct->simulation->isSimulationStopped();
	ui->qqAllEnabled->setChecked(App::ct->mainSettings->gcsCalculationEnabled);
	ui->qqAddNew->setEnabled(sel1&&noSim);

	if (!inSelectionRoutine)
	{
		int selectedObjectID=getSelectedObjectID();
		updateObjectsInList();
		selectObjectInList(selectedObjectID);
	}

	CConstraintSolverObject* it=App::ct->constraintSolver->getObject(getSelectedObjectID());


	ui->qqIterations->setEnabled(it!=NULL);
	ui->qqInterpolation->setEnabled(it!=NULL);
	ui->qqDamping->setEnabled(it!=NULL);
	ui->qqAngularVariation->setEnabled(it!=NULL);
	ui->qqLinearVariation->setEnabled(it!=NULL);
	ui->qqLoopClosurePosTol->setEnabled(it!=NULL);
	ui->qqLoopClosureAngTol->setEnabled(it!=NULL);
	ui->qqExplicitHandling->setEnabled(it!=NULL);
	if (it!=NULL)
	{
		ui->qqIterations->setText(tt::FNb(0,it->getMaximumIterations()).c_str());
		ui->qqInterpolation->setText(tt::FNb(0,it->getInterpolation(),2,false).c_str());
		ui->qqDamping->setText(tt::FNb(0,it->getGeneralDamping(),5,false).c_str());
		ui->qqAngularVariation->setText(gv::getAngleStr(false,it->getMaxAngleVariation(),0).c_str());
		ui->qqLinearVariation->setText(gv::getSizeStr(false,it->getMaxLinearVariation(),0).c_str());
		ui->qqLoopClosurePosTol->setText(gv::getSizeStr(false,it->getLoopClosurePositionTolerance(),0).c_str());
		ui->qqLoopClosureAngTol->setText(gv::getAngleStr(false,it->getLoopClosureOrientationTolerance(),0).c_str());
		ui->qqExplicitHandling->setChecked(it->getExplicitHandling());
	}
	else
	{
		ui->qqIterations->setText("");
		ui->qqInterpolation->setText("");
		ui->qqDamping->setText("");
		ui->qqAngularVariation->setText("");
		ui->qqLinearVariation->setText("");
		ui->qqLoopClosurePosTol->setText("");
		ui->qqLoopClosureAngTol->setText("");
		ui->qqExplicitHandling->setChecked(false);
	}

}

void CQDlgConstraintSolver::updateObjectsInList()
{
	ui->qqObjectList->clear();
	for (int i=0;i<int(App::ct->constraintSolver->allGcsObjects.size());i++)
	{
		CConstraintSolverObject* it=App::ct->constraintSolver->allGcsObjects[i];
		std::string tmp=it->getObjectName();
		int id=it->getObjectID();
		C3DObject* it2=App::ct->objCont->getObject(it->getBase3DObject());
		if (it2!=NULL)
		{
			tmp+=" (base: ";
			tmp+=it2->getName();
			tmp+=")";
		}
		QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
		itm->setData(Qt::UserRole,QVariant(id));
		itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
		ui->qqObjectList->addItem(itm);
	}
}

int CQDlgConstraintSolver::getSelectedObjectID()
{
	QList<QListWidgetItem*> sel=ui->qqObjectList->selectedItems();
	if (sel.size()>0)
		return(sel.at(0)->data(Qt::UserRole).toInt());
	return(-1);
}

void CQDlgConstraintSolver::selectObjectInList(int objectID)
{
	for (int i=0;i<ui->qqObjectList->count();i++)
	{
		QListWidgetItem* it=ui->qqObjectList->item(i);
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

void CQDlgConstraintSolver::on_qqAllEnabled_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->mainSettings->gcsCalculationEnabled=!App::ct->mainSettings->gcsCalculationEnabled;
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgConstraintSolver::on_qqAddNew_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (App::ct->objCont->getSelSize()==1)
		{
			C3DObject* it=App::ct->objCont->getLastSelection();
			if (it!=NULL)
			{
				CGeometricConstraintSolverInt work(it->getID(),-1);
				CConstraintSolverObject* similarObject=App::ct->constraintSolver->getObjectFromMechanismID(work.getMechanismID());
				if (similarObject!=NULL)
				{
					std::string tmp(IDS_GCSOBJECT_ALREADY_THERE_MESSAGE);
					tmp+=" '";
					tmp+=similarObject->getObjectName();
					tmp+="'.";
					App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_CONSTRAINT_SOLVER),tmp,VMessageBox::OKELI);
				}
				else
				{
					CConstraintSolverObject* no=new CConstraintSolverObject();
					no->setBase3DObject(it->getID());
					App::ct->constraintSolver->addObject(no,false);
					App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
					updateObjectsInList();
					selectObjectInList(no->getObjectID());
				}
			}
		}
		refresh();
	}
}

void CQDlgConstraintSolver::on_qqObjectList_itemSelectionChanged()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		int objID=getSelectedObjectID();
		CConstraintSolverObject* it=App::ct->constraintSolver->getObject(objID);
		if (it!=NULL)
			((CEditBoxDelegate*)ui->qqObjectList->itemDelegate())->initialText=it->getObjectName();
		else
			((CEditBoxDelegate*)ui->qqObjectList->itemDelegate())->initialText="";
		inSelectionRoutine=true;
		refresh();
		inSelectionRoutine=false;
	}
}

void CQDlgConstraintSolver::on_qqObjectList_itemChanged(QListWidgetItem *item)
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (item!=NULL)
		{
			std::string newName(item->text().toStdString());
			CConstraintSolverObject* it=App::ct->constraintSolver->getObject(item->data(Qt::UserRole).toInt());
			if ( (it!=NULL)&&(newName!="") )
			{
				if (it->getObjectName()!=newName)
				{
					tt::removeIllegalCharacters(newName,true);
					if (App::ct->constraintSolver->getObject(newName.c_str())==NULL)
					{
						it->setObjectName(newName.c_str());
						App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
					}
				}
			}
			refresh();
		}
	}
}

void CQDlgConstraintSolver::onDeletePressed()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (focusWidget()==ui->qqObjectList)
		{
			int objID=getSelectedObjectID();
			if (objID!=-1)
			{
				App::ct->constraintSolver->removeObject(objID);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				refresh();
			}
			return;
		}
	}
}

void CQDlgConstraintSolver::on_qqExplicitHandling_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CConstraintSolverObject* it=App::ct->constraintSolver->getObject(getSelectedObjectID());
		if (it==NULL)
			return;
		it->setExplicitHandling(!it->getExplicitHandling());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgConstraintSolver::on_qqIterations_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CConstraintSolverObject* it=App::ct->constraintSolver->getObject(getSelectedObjectID());
		if (it==NULL)
			return;
		bool ok;
		int newVal=ui->qqIterations->text().toInt(&ok);
		if (ok)
		{
			it->setMaximumIterations(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgConstraintSolver::on_qqInterpolation_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CConstraintSolverObject* it=App::ct->constraintSolver->getObject(getSelectedObjectID());
		if (it==NULL)
			return;
		bool ok;
		float newVal=ui->qqInterpolation->text().toFloat(&ok);
		if (ok)
		{
			it->setInterpolation(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgConstraintSolver::on_qqDamping_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CConstraintSolverObject* it=App::ct->constraintSolver->getObject(getSelectedObjectID());
		if (it==NULL)
			return;
		bool ok;
		float newVal=ui->qqDamping->text().toFloat(&ok);
		if (ok)
		{
			it->setGeneralDamping(newVal);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgConstraintSolver::on_qqLinearVariation_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CConstraintSolverObject* it=App::ct->constraintSolver->getObject(getSelectedObjectID());
		if (it==NULL)
			return;
		bool ok;
		float newVal=ui->qqLinearVariation->text().toFloat(&ok);
		if (ok)
		{
			it->setMaxLinearVariation(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgConstraintSolver::on_qqAngularVariation_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CConstraintSolverObject* it=App::ct->constraintSolver->getObject(getSelectedObjectID());
		if (it==NULL)
			return;
		bool ok;
		float newVal=ui->qqAngularVariation->text().toFloat(&ok);
		if (ok)
		{
			it->setMaxAngleVariation(newVal*gv::userToRad);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgConstraintSolver::on_qqLoopClosurePosTol_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CConstraintSolverObject* it=App::ct->constraintSolver->getObject(getSelectedObjectID());
		if (it==NULL)
			return;
		bool ok;
		float newVal=ui->qqLoopClosurePosTol->text().toFloat(&ok);
		if (ok)
		{
			it->setLoopClosurePositionTolerance(newVal*gv::userToMeter);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}

void CQDlgConstraintSolver::on_qqLoopClosureAngTol_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CConstraintSolverObject* it=App::ct->constraintSolver->getObject(getSelectedObjectID());
		if (it==NULL)
			return;
		bool ok;
		float newVal=ui->qqLoopClosureAngTol->text().toFloat(&ok);
		if (ok)
		{
			it->setLoopClosureOrientationTolerance(newVal*gv::userToRad);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		}
		refresh();
	}
}
