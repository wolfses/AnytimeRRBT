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

#include "qdlgedgeedit.h"
#include "ui_qdlgedgeedit.h"
#include "App.h"
#include "GV.h"
#include "Tt.h"
#include "TriangleVertexEdition.h"
#include "v_repStrings.h"
#include <boost/lexical_cast.hpp>

CQDlgEdgeEdit::CQDlgEdgeEdit(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgEdgeEdit)
{
    ui->setupUi(this);
}

CQDlgEdgeEdit::~CQDlgEdgeEdit()
{
    delete ui;
}

void CQDlgEdgeEdit::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(SHAPE_EDITION_DLG);
}

void CQDlgEdgeEdit::refresh()
{
	ui->qqShowHiddenEdges->setChecked(App::ct->objCont->showHiddenVerticeAndEdges);
	ui->qqAutoFollowing->setChecked(App::ct->objCont->automaticallyFollowEdges);
	ui->qqEdgeAngle->setEnabled(App::ct->objCont->automaticallyFollowEdges);
	ui->qqDirectionChangeAngle->setEnabled(App::ct->objCont->automaticallyFollowEdges);
	ui->qqEdgeAngle->setText(gv::getAngleStr(false,App::ct->objCont->edgeMaxAngle,1).c_str());
	ui->qqDirectionChangeAngle->setText(gv::getAngleStr(false,App::ct->objCont->edgeDirectionChangeMaxAngle,1).c_str());

	std::string tmp=std::string(IDS_TOTAL_EDGES)+": "+boost::lexical_cast<std::string>(App::ct->objCont->editModeBuffer.size())+"/"+
		boost::lexical_cast<std::string>(App::ct->objCont->_edgeCont.allEdges.size()/2);
	ui->qqInfo->setText(tmp.c_str());

	ui->qqMakePath->setEnabled(App::ct->objCont->editModeBuffer.size()!=0);
}

void CQDlgEdgeEdit::on_qqShowHiddenEdges_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->objCont->showHiddenVerticeAndEdges=!App::ct->objCont->showHiddenVerticeAndEdges;
		refresh();
	}
}

void CQDlgEdgeEdit::on_qqAutoFollowing_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->objCont->automaticallyFollowEdges=!App::ct->objCont->automaticallyFollowEdges;
		refresh();
	}
}

void CQDlgEdgeEdit::on_qqEdgeAngle_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqEdgeAngle->text().toFloat(&ok);
		if (ok)
		{
			newVal=tt::getLimitedFloat(1.0f*degToRad,180.0f*degToRad,newVal*gv::userToRad);
			App::ct->objCont->edgeMaxAngle=newVal;
		}
		refresh();
	}
}

void CQDlgEdgeEdit::on_qqDirectionChangeAngle_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		bool ok;
		float newVal=ui->qqDirectionChangeAngle->text().toFloat(&ok);
		if (ok)
		{
			newVal=tt::getLimitedFloat(0.0f*degToRad,180.0f*degToRad,newVal*gv::userToRad);
			App::ct->objCont->edgeDirectionChangeMaxAngle=newVal;
		}
		refresh();
	}
}

void CQDlgEdgeEdit::on_qqMakePath_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CTriangleVertexEdition::evaluateMenuSelection(MAKE_PATH_WITH_SELECTED_EDGES_CMD);
		refresh();
	}
}

void CQDlgEdgeEdit::on_qqClearSelection_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::ct->objCont->deselectEditModeBuffer();
		refresh();
	}
}

void CQDlgEdgeEdit::on_qqInvertSelection_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		for (int i=0;i<int(App::ct->objCont->_edgeCont.allEdges.size())/2;i++)
			App::ct->objCont->xorAddItemToEditModeBuffer(i);
		refresh();
	}
}
