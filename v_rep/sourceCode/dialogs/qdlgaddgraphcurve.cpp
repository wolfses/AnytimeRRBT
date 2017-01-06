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
#include "qdlgaddgraphcurve.h"
#include "ui_qdlgaddgraphcurve.h"
#include "GV.h"
#include "Tt.h"
#include "GraphingRoutines.h"
#include "App.h"
#include "IloIlo.h"

CQDlgAddGraphCurve::CQDlgAddGraphCurve(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlgAddGraphCurve)
{
    ui->setupUi(this);
}

CQDlgAddGraphCurve::~CQDlgAddGraphCurve()
{
    delete ui;
}

void CQDlgAddGraphCurve::cancelEvent()
{
	defaultModalDialogEndRoutine(false);
}

void CQDlgAddGraphCurve::okEvent()
{
//	defaultModalDialogEndRoutine(true);
}

void CQDlgAddGraphCurve::refresh()
{
	CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
	ui->qqZValue->setVisible(!xyGraph);
	ui->qqComboZ->setVisible(!xyGraph);

	ui->qqComboX->clear();
	ui->qqComboY->clear();
	ui->qqComboZ->clear();
	ui->qqComboX->addItem("0.0",QVariant(-1));
	ui->qqComboY->addItem("0.0",QVariant(-1));
	ui->qqComboZ->addItem("0.0",QVariant(-1));
	for (int i=0;i<int(it->daten.size());i++)
	{
		int theID=it->daten[i]->getIdentifier();
		ui->qqComboX->addItem(it->daten[i]->getName().c_str(),QVariant(theID));
		ui->qqComboY->addItem(it->daten[i]->getName().c_str(),QVariant(theID));
		if (!xyGraph)
			ui->qqComboZ->addItem(it->daten[i]->getName().c_str(),QVariant(theID));
	}
}

void CQDlgAddGraphCurve::on_qqOkCancel_accepted()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		CGraphDataComb* theNew=new CGraphDataComb();

		theNew->data[0]=ui->qqComboX->itemData(ui->qqComboX->currentIndex()).toInt();
		theNew->data[1]=ui->qqComboY->itemData(ui->qqComboY->currentIndex()).toInt();
		if (!xyGraph)
		{
			theNew->data[2]=ui->qqComboZ->itemData(ui->qqComboZ->currentIndex()).toInt();
			it->add3DPartners(theNew);
		}
		else
			it->add2DPartners(theNew);
		newID=theNew->getIdentifier();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		defaultModalDialogEndRoutine(true);
	}
}

void CQDlgAddGraphCurve::on_qqOkCancel_rejected()
{
	defaultModalDialogEndRoutine(false);
}
