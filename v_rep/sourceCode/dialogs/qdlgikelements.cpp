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
#include "qdlgikelements.h"
#include "ui_qdlgikelements.h"
#include "GV.h"
#include "Tt.h"
#include <QShortcut>
#include "App.h"
#include "IloIlo.h"
#include "v_repStrings.h"

CQDlgIkElements::CQDlgIkElements(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgIkElements)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
	inListSelectionRoutine=false;
	noListSelectionAllowed=false;
	firstHere=true;
	QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	connect(shortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	QShortcut* shortcut2 = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
	connect(shortcut2,SIGNAL(activated()), this, SLOT(onDeletePressed()));
}

CQDlgIkElements::~CQDlgIkElements()
{
    delete ui;
}

void CQDlgIkElements::cancelEvent()
{
//	defaultModalDialogEndRoutine(false);
}

void CQDlgIkElements::okEvent()
{
//	defaultModalDialogEndRoutine(true);
}

void CQDlgIkElements::onDeletePressed()
{
	if (focusWidget()==ui->qqList)
	{
		int objID=getSelectedObjectID();
		if (objID!=-1)
		{
			ikGroup->removeIkElement(objID);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
		return;
	}
}

void CQDlgIkElements::updateObjectsInList()
{
	noListSelectionAllowed=true;
	ui->qqList->clear();

	for (int i=0;i<int(ikGroup->ikElements.size());i++)
	{
		int tooltipID=ikGroup->ikElements[i]->getTooltip();
		int elementID=ikGroup->ikElements[i]->getObjectID();
		CDummy* theTooltip=App::ct->objCont->getDummy(tooltipID);
		QListWidgetItem* itm=new QListWidgetItem(theTooltip->getName().c_str());
		itm->setData(Qt::UserRole,QVariant(elementID));
		itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
		ui->qqList->addItem(itm);
	}

	noListSelectionAllowed=false;
}

int CQDlgIkElements::getSelectedObjectID()
{
	QList<QListWidgetItem*> sel=ui->qqList->selectedItems();
	if (sel.size()>0)
		return(sel.at(0)->data(Qt::UserRole).toInt());
	return(-1);
}

void CQDlgIkElements::selectObjectInList(int objectID)
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


void CQDlgIkElements::refresh()
{
	inMainRefreshRoutine=true;
	int elementID=getSelectedObjectID();
	CikEl* it=ikGroup->getIkElement(elementID);

	if (!inListSelectionRoutine)
	{
		updateObjectsInList();
		selectObjectInList(elementID);
	}

	ui->qqActive->setEnabled(it!=NULL);
	ui->qqBaseCombo->setEnabled(it!=NULL);
	ui->qqX->setEnabled(it!=NULL);
	ui->qqY->setEnabled(it!=NULL);
	ui->qqZ->setEnabled(it!=NULL);
	ui->qqAlphaBeta->setEnabled(it!=NULL);
	ui->qqGamma->setEnabled(it!=NULL);
	ui->qqRelativeCombo->setEnabled(it!=NULL);
	ui->qqPrecisionLinear->setEnabled(it!=NULL);
	ui->qqPrecisionAngular->setEnabled(it!=NULL);
	ui->qqWeightLinear->setEnabled(it!=NULL);
	ui->qqWeightAngular->setEnabled(it!=NULL);

	ui->qqActive->setChecked((it!=NULL)&&it->getActive());
	ui->qqBaseCombo->clear();
	ui->qqX->setChecked((it!=NULL)&&(it->getConstraints()&sim_ik_x_constraint));
	ui->qqY->setChecked((it!=NULL)&&(it->getConstraints()&sim_ik_y_constraint));
	ui->qqZ->setChecked((it!=NULL)&&(it->getConstraints()&sim_ik_z_constraint));
	ui->qqAlphaBeta->setChecked((it!=NULL)&&(it->getConstraints()&sim_ik_alpha_beta_constraint));
	ui->qqGamma->setChecked((it!=NULL)&&(it->getConstraints()&sim_ik_gamma_constraint));
	ui->qqRelativeCombo->clear();

	std::vector<std::string> names;
	std::vector<int> ids;

	if (firstHere)
	{
		firstHere=false;
		ui->qqTipCombo->clear();
		for (int i=0;i<int(App::ct->objCont->dummyList.size());i++)
		{
			CDummy* it2=App::ct->objCont->getDummy(App::ct->objCont->dummyList[i]);
			names.push_back(it2->getName());
			ids.push_back(it2->getID());
		}
		tt::orderStrings(names,ids);
		for (int i=0;i<int(names.size());i++)
			ui->qqTipCombo->addItem(names[i].c_str(),QVariant(ids[i]));
	}

	if (it!=NULL)
	{
		CDummy* tip=App::ct->objCont->getDummy(it->getTooltip());
		if (tip==NULL)
			ui->qqTargetString->setText("");
		else
		{
			CDummy* target=App::ct->objCont->getDummy(tip->getLinkedDummyID());
			if (target==NULL)
				ui->qqTargetString->setText(strTranslate(IDS_WARNING_TIP_DUMMY_NOT_LINKED));
			else
			{
				if (tip->getLinkType()!=sim_dummy_linktype_ik_tip_target)
					ui->qqTargetString->setText(strTranslate(IDS_WARNING_WRONG_DUMMY_LINK_TYPE));
				else
					ui->qqTargetString->setText(target->getName().c_str());
			}
		}

		CDummy* tooltip=App::ct->objCont->getDummy(it->getTooltip());
		ui->qqBaseCombo->addItem(strTranslate(IDSN_WORLD),QVariant(-1));
		names.clear();
		ids.clear();
		for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
		{
			C3DObject* it2=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
			if (tooltip->isObjectParentedWith(it2))
			{
				names.push_back(it2->getName());
				ids.push_back(it2->getID());
			}
		}
		tt::orderStrings(names,ids);
		for (int i=0;i<int(names.size());i++)
			ui->qqBaseCombo->addItem(names[i].c_str(),QVariant(ids[i]));
		for (int i=0;i<ui->qqBaseCombo->count();i++)
		{
			if (ui->qqBaseCombo->itemData(i).toInt()==it->getBase())
			{
				ui->qqBaseCombo->setCurrentIndex(i);
				break;
			}
		}

		names.clear();
		ids.clear();
		ui->qqRelativeCombo->addItem(strTranslate(IDSN_SAME_AS_BASE),QVariant(-1));
		for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
		{
			C3DObject* it2=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
			if ( (it2->getObjectType()==sim_object_dummy_type)&&(it2!=tooltip) )
			{
				names.push_back(it2->getName());
				ids.push_back(it2->getID());
			}
		}
		tt::orderStrings(names,ids);
		for (int i=0;i<int(names.size());i++)
			ui->qqRelativeCombo->addItem(names[i].c_str(),QVariant(ids[i]));
		for (int i=0;i<ui->qqRelativeCombo->count();i++)
		{
			if (ui->qqRelativeCombo->itemData(i).toInt()==it->getAlternativeBaseForConstraints())
			{
				ui->qqRelativeCombo->setCurrentIndex(i);
				break;
			}
		}

		ui->qqPrecisionLinear->setText(gv::getSizeStr(false,it->getMinLinearPrecision(),0).c_str());
		ui->qqPrecisionAngular->setText(gv::getAngleStr(false,it->getMinAngularPrecision(),0).c_str());
		ui->qqWeightLinear->setText(tt::FNb(0,it->getPositionWeight(),3,false).c_str());
		ui->qqWeightAngular->setText(tt::FNb(0,it->getOrientationWeight(),3,false).c_str());
	}
	else
	{
		ui->qqTargetString->setText("");
		ui->qqPrecisionLinear->setText("");
		ui->qqPrecisionAngular->setText("");
		ui->qqWeightLinear->setText("");
		ui->qqWeightAngular->setText("");
	}


	inMainRefreshRoutine=false;
}

void CQDlgIkElements::on_qqAddNewElement_clicked()
{
	CDummy* it=App::ct->objCont->getDummy(ui->qqTipCombo->itemData(ui->qqTipCombo->currentIndex()).toInt());
	if (it!=NULL)
	{
		CikEl* newIkEl=new CikEl(it->getID());
		if (!ikGroup->addIkElement(newIkEl))
			delete newIkEl;
		else
		{
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			updateObjectsInList();
			selectObjectInList(newIkEl->getObjectID());
			refresh();
		}
	}
}

void CQDlgIkElements::on_qqList_itemSelectionChanged()
{
	if (!noListSelectionAllowed)
	{
		inListSelectionRoutine=true;
		refresh();
		inListSelectionRoutine=false;
	}
}

void CQDlgIkElements::on_qqActive_clicked()
{
	CikEl* it=ikGroup->getIkElement(getSelectedObjectID());
	if (it==NULL)
		return;
	it->setActive(!it->getActive());
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();
}

void CQDlgIkElements::on_qqBaseCombo_currentIndexChanged(int index)
{
	if (!inMainRefreshRoutine)
	{
		CikEl* it=ikGroup->getIkElement(getSelectedObjectID());
		if (it==NULL)
			return;
		it->setBase(ui->qqBaseCombo->itemData(ui->qqBaseCombo->currentIndex()).toInt());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgIkElements::on_qqX_clicked()
{
	CikEl* it=ikGroup->getIkElement(getSelectedObjectID());
	if (it==NULL)
		return;
	it->setConstraints(it->getConstraints()^sim_ik_x_constraint);
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();
}

void CQDlgIkElements::on_qqY_clicked()
{
	CikEl* it=ikGroup->getIkElement(getSelectedObjectID());
	if (it==NULL)
		return;
	it->setConstraints(it->getConstraints()^sim_ik_y_constraint);
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();
}

void CQDlgIkElements::on_qqZ_clicked()
{
	CikEl* it=ikGroup->getIkElement(getSelectedObjectID());
	if (it==NULL)
		return;
	it->setConstraints(it->getConstraints()^sim_ik_z_constraint);
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();
}

void CQDlgIkElements::on_qqAlphaBeta_clicked()
{
	CikEl* it=ikGroup->getIkElement(getSelectedObjectID());
	if (it==NULL)
		return;
	it->setConstraints(it->getConstraints()^sim_ik_alpha_beta_constraint);
	if ((it->getConstraints()&(sim_ik_alpha_beta_constraint|sim_ik_gamma_constraint))==sim_ik_gamma_constraint)
		it->setConstraints(it->getConstraints()^sim_ik_gamma_constraint); // gamma constraint cannot be selected if alpha-beta constraint is not selected!
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();

}

void CQDlgIkElements::on_qqGamma_clicked()
{
	CikEl* it=ikGroup->getIkElement(getSelectedObjectID());
	if (it==NULL)
		return;
	it->setConstraints(it->getConstraints()^sim_ik_gamma_constraint);
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	refresh();
}

void CQDlgIkElements::on_qqRelativeCombo_currentIndexChanged(int index)
{
	if (!inMainRefreshRoutine)
	{
		CikEl* it=ikGroup->getIkElement(getSelectedObjectID());
		if (it==NULL)
			return;
		it->setAlternativeBaseForConstraints(ui->qqRelativeCombo->itemData(ui->qqRelativeCombo->currentIndex()).toInt());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgIkElements::on_qqPrecisionLinear_editingFinished()
{
	CikEl* it=ikGroup->getIkElement(getSelectedObjectID());
	if (it==NULL)
		return;
	bool ok;
	float newVal=ui->qqPrecisionLinear->text().toFloat(&ok);
	if (ok)
	{
		float v=newVal*gv::userToMeter;
		tt::limitValue(0.00001f,1.0f,v);
		it->setMinLinearPrecision(v);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgIkElements::on_qqPrecisionAngular_editingFinished()
{
	CikEl* it=ikGroup->getIkElement(getSelectedObjectID());
	if (it==NULL)
		return;
	bool ok;
	float newVal=ui->qqPrecisionAngular->text().toFloat(&ok);
	if (ok)
	{
		float v=newVal*gv::userToRad;
		tt::limitValue(0.001f*degToRad,180.0f*degToRad,v);
		it->setMinAngularPrecision(v);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgIkElements::on_qqWeightLinear_editingFinished()
{
	CikEl* it=ikGroup->getIkElement(getSelectedObjectID());
	if (it==NULL)
		return;
	bool ok;
	float newVal=ui->qqWeightLinear->text().toFloat(&ok);
	if (ok)
	{
		it->setPositionWeight(newVal);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgIkElements::on_qqWeightAngular_editingFinished()
{
	CikEl* it=ikGroup->getIkElement(getSelectedObjectID());
	if (it==NULL)
		return;
	bool ok;
	float newVal=ui->qqWeightAngular->text().toFloat(&ok);
	if (ok)
	{
		it->setOrientationWeight(newVal);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	}
	refresh();
}

void CQDlgIkElements::on_buttonBox_clicked(QAbstractButton *button)
{
	defaultModalDialogEndRoutine(true);
}
