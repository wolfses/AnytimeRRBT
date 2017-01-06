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
#include "qdlgcalcdialogcontainer.h"
#include "ui_qdlgcalcdialogcontainer.h"
#include "App.h"
#include "IloIlo.h"
#include "qdlgcollisions.h"
#include "qdlgdistances.h"
#include "qdlgconstraintsolver.h"
#include "qdlgdynamics.h"
#include "qdlgik.h"
#include "qdlgpathplanning.h"
#include "qdlgmotionplanning.h"

#define TOP_BORDER_WIDTH 60

CQDlgCalcDialogContainer::CQDlgCalcDialogContainer(QWidget *parent) :
	CDlgEx(parent),
	ui(new Ui::CQDlgCalcDialogContainer)
{
    ui->setupUi(this);
	pageDlgs[0]=new CQDlgCollisions();
	originalHeights[0]=pageDlgs[0]->size().height();

	pageDlgs[1]=new CQDlgDistances();
	originalHeights[1]=pageDlgs[1]->size().height();

	pageDlgs[2]=new CQDlgMotionPlanning();
	originalHeights[2]=pageDlgs[2]->size().height();

	pageDlgs[3]=new CQDlgPathPlanning();
	originalHeights[3]=pageDlgs[3]->size().height();

	pageDlgs[4]=new CQDlgIk();
	originalHeights[4]=pageDlgs[4]->size().height();

	pageDlgs[5]=new CQDlgConstraintSolver();
	originalHeights[5]=pageDlgs[5]->size().height();

	pageDlgs[6]=new CQDlgDynamics();
	originalHeights[6]=pageDlgs[6]->size().height();

	currentPage=0;
	desiredPage=0;
	bl=new QVBoxLayout();
	bl->setContentsMargins(0,TOP_BORDER_WIDTH,0,0);
	setLayout(bl);
	bl->addWidget(pageDlgs[0]);
	bl->addWidget(pageDlgs[1]);
	pageDlgs[1]->setVisible(false);
	bl->addWidget(pageDlgs[2]);
	pageDlgs[2]->setVisible(false);
	bl->addWidget(pageDlgs[3]);
	pageDlgs[3]->setVisible(false);
	bl->addWidget(pageDlgs[4]);
	pageDlgs[4]->setVisible(false);
	bl->addWidget(pageDlgs[5]);
	pageDlgs[5]->setVisible(false);
	bl->addWidget(pageDlgs[6]);
	pageDlgs[6]->setVisible(false);

	QSize s(pageDlgs[currentPage]->size());
	s.setHeight(originalHeights[currentPage]+TOP_BORDER_WIDTH);
	setFixedSize(s);

/*
#ifndef WIN_VREP
	// Since Qt5, we have problems on Linux (resizing-->dlg shifts in position) and Mac (resising-->ugly glitch)
	// In that case we keep constant-size dialogs.
	if (QT_VERSION>=0x050000)
	{
		int dlgMaxHeight=0;
		for (int i=0;i<7;i++)
		{
			int si=pageDlgs[i]->size().height();
			if (si>dlgMaxHeight)
				dlgMaxHeight=si;
		}
		s.setHeight(dlgMaxHeight+TOP_BORDER_WIDTH);
		setFixedSize(s);
	}
#endif
*/
}

CQDlgCalcDialogContainer::~CQDlgCalcDialogContainer()
{
    delete ui;
}

void CQDlgCalcDialogContainer::refresh()
{
	ui->qqCollision->setChecked(desiredPage==0);
	ui->qqDistance->setChecked(desiredPage==1);
	ui->qqMotionPlanning->setChecked(desiredPage==2);
	ui->qqPathPlanning->setChecked(desiredPage==3);
	ui->qqIk->setChecked(desiredPage==4);
	ui->qqGcs->setChecked(desiredPage==5);
	ui->qqDynamics->setChecked(desiredPage==6);

	if (desiredPage!=currentPage)
	{
		pageDlgs[currentPage]->setVisible(false);
		currentPage=desiredPage;
		pageDlgs[currentPage]->setVisible(true);

		QSize s(pageDlgs[currentPage]->size());
		s.setHeight(originalHeights[currentPage]+TOP_BORDER_WIDTH);

#ifdef MAC_VREP
		if (QT_VERSION>=0x050000)
		{ // Since Qt5, we have problems on Mac (resising-->ugly glitch)
			setVisible(false);
			setFixedSize(s);
			setVisible(true);
		}
		else
			setFixedSize(s);
#else
		setFixedSize(s);
#endif

/*
#ifdef WIN_VREP
		// Since Qt5, we have problems on Linux (resizing-->dlg shifts in position) and Mac (resising-->ugly glitch)
		// In that case we keep constant-size dialogs.
		QSize s(pageDlgs[currentPage]->size());
		s.setHeight(originalHeights[currentPage]+TOP_BORDER_WIDTH);
		setFixedSize(s);
#else
		if (QT_VERSION<0x050000)
		{
			QSize s(pageDlgs[currentPage]->size());
			s.setHeight(originalHeights[currentPage]+TOP_BORDER_WIDTH);
			setFixedSize(s);
		}
#endif
*/
	}
	pageDlgs[currentPage]->refresh();
}

void CQDlgCalcDialogContainer::on_qqCollision_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		desiredPage=0;
		refresh();
	}
}

void CQDlgCalcDialogContainer::on_qqDistance_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		desiredPage=1;
		refresh();
	}
}

void CQDlgCalcDialogContainer::on_qqMotionPlanning_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		desiredPage=2;
		refresh();
	}
}

void CQDlgCalcDialogContainer::on_qqPathPlanning_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		desiredPage=3;
		refresh();
	}
}

void CQDlgCalcDialogContainer::on_qqIk_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		desiredPage=4;
		refresh();
	}
}

void CQDlgCalcDialogContainer::on_qqGcs_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		desiredPage=5;
		refresh();
	}
}

void CQDlgCalcDialogContainer::on_qqDynamics_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		desiredPage=6;
		refresh();
	}
}
