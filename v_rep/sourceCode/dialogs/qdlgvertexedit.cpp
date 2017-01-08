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

#include "qdlgvertexedit.h"
#include "ui_qdlgvertexedit.h"
#include "App.h"
#include "GV.h"
#include "TriangleVertexEdition.h"
#include "Tt.h"
#include "v_repStrings.h"
#include <boost/lexical_cast.hpp>

CQDlgVertexEdit::CQDlgVertexEdit(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgVertexEdit)
{
    ui->setupUi(this);
}

CQDlgVertexEdit::~CQDlgVertexEdit()
{
    delete ui;
}

void CQDlgVertexEdit::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(SHAPE_EDITION_DLG);
}

void CQDlgVertexEdit::refresh()
{
	ui->qqShowHiddenVertices->setChecked(App::ct->objCont->showHiddenVerticeAndEdges);

	std::string tmp=std::string(IDS_TOTAL_VERTICES)+": "+boost::lexical_cast<std::string>(App::ct->objCont->editModeBuffer.size())+"/"+
		boost::lexical_cast<std::string>(App::ct->objCont->_editionVertices.size()/3);
	ui->qqInfo->setText(tmp.c_str());

	ui->qqInsertTriangles->setEnabled(App::ct->objCont->editModeBuffer.size()>2);
	ui->qqInsertFan->setEnabled(App::ct->objCont->editModeBuffer.size()>3);
	ui->qqMakeDummies->setEnabled(App::ct->objCont->editModeBuffer.size()!=0);
}

void CQDlgVertexEdit::on_qqShowHiddenVertices_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->objCont->showHiddenVerticeAndEdges=!App::ct->objCont->showHiddenVerticeAndEdges;
		refresh();
	}
}

void CQDlgVertexEdit::on_qqInsertTriangles_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CTriangleVertexEdition::evaluateMenuSelection(INSERT_TRIANGLES_CMD); // has own lock
		refresh();
	}
}

void CQDlgVertexEdit::on_qqInsertFan_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CTriangleVertexEdition::evaluateMenuSelection(INSERT_TRIANGLE_FAN_CMD); // has own lock
		refresh();
	}
}

void CQDlgVertexEdit::on_qqMakeDummies_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CTriangleVertexEdition::evaluateMenuSelection(MAKE_DUMMIES_WITH_SELECTED_VERTICES_CMD); // has own lock
		refresh();
	}
}

void CQDlgVertexEdit::on_qqClearSelection_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		App::ct->objCont->deselectEditModeBuffer();
		refresh();
	}
}

void CQDlgVertexEdit::on_qqInvertSelection_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		for (int i=0;i<int(App::ct->objCont->_editionVertices.size())/3;i++)
			App::ct->objCont->xorAddItemToEditModeBuffer(i);
		refresh();
	}
}
