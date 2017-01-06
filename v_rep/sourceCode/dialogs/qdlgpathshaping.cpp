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
#include "qdlgpathshaping.h"
#include "ui_qdlgpathshaping.h"
#include "GV.h"
#include "Tt.h"
#include "qdlgmaterial.h"
#include "App.h"
#include "v_repStrings.h"

bool CQDlgPathShaping::showWindow=false;

CQDlgPathShaping::CQDlgPathShaping(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgPathShaping)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
}

CQDlgPathShaping::~CQDlgPathShaping()
{
    delete ui;
}

void CQDlgPathShaping::cancelEvent()
{ // no cancel event allowed
	showWindow=false;
	CDlgEx::cancelEvent();
	App::ct->objCont->setFullDialogRefreshFlag();
}

void CQDlgPathShaping::refresh()
{
	inMainRefreshRoutine=true;
	bool noEditMode=(App::ct->objCont->getEditModeType()==NO_EDIT_MODE);
	bool noSim=App::ct->simulation->isSimulationStopped();
	int sectionType=0;
	CPath* it=NULL;
	bool en=false;
	if (App::ct->objCont->isLastSelectionAPath()&&noEditMode)
	{
		it=(CPath*)App::ct->objCont->getLastSelection();
		en=it->getShapingEnabled();
		sectionType=it->getShapingType();
	}

	ui->qqEnabled->setEnabled(noSim&&(it!=NULL));

	ui->qqFollowOrientation->setEnabled(noSim&&(it!=NULL)&&en);
	ui->qqConvexHull->setEnabled(noSim&&(it!=NULL)&&en);
	ui->qqAdjustColor->setEnabled(noSim&&(it!=NULL)&&en);
	ui->qqGenerateShape->setEnabled(noSim&&(it!=NULL)&&en);
	ui->qqMaxLength->setEnabled(noSim&&(it!=NULL)&&en);

	ui->qqTypeCombo->setEnabled(noSim&&(it!=NULL)&&en);
	ui->qqCyclic->setEnabled(noSim&&(it!=NULL)&&en&&(sectionType==0));
	ui->qqScalingFactor->setEnabled(noSim&&(it!=NULL)&&en);
	ui->qqCoordinates->setEnabled(noSim&&(it!=NULL)&&en&&(sectionType==0));

	ui->qqEnabled->setChecked((it!=NULL)&&it->getShapingEnabled());
	ui->qqFollowOrientation->setChecked((it!=NULL)&&it->getShapingFollowFullOrientation());
	ui->qqConvexHull->setChecked((it!=NULL)&&it->getShapingThroughConvexHull());

	ui->qqTypeCombo->clear();
	ui->qqCyclic->setChecked((it!=NULL)&&it->getShapingSectionClosed());

	if (it!=NULL)
	{
		ui->qqMaxLength->setText(gv::getSizeStr(false,it->getShapingElementMaxLength(),0).c_str());
		ui->qqScalingFactor->setText(tt::FNb(0,it->getShapingScaling(),2,false).c_str());

		ui->qqTypeCombo->addItem(strTranslate(IDS_SHAPING_CIRCLE),QVariant(1));
		ui->qqTypeCombo->addItem(strTranslate(IDS_SHAPING_SQUARE),QVariant(2));
		ui->qqTypeCombo->addItem(strTranslate(IDS_SHAPING_HORIZONTAL_SEGMENT),QVariant(3));
		ui->qqTypeCombo->addItem(strTranslate(IDS_SHAPING_VERTICAL_SEGMENT),QVariant(4));
		ui->qqTypeCombo->addItem(strTranslate(IDS_SHAPING_USER_DEFINED),QVariant(0));
		for (int i=0;i<ui->qqTypeCombo->count();i++)
		{
			if (ui->qqTypeCombo->itemData(i).toInt()==it->getShapingType())
			{
				ui->qqTypeCombo->setCurrentIndex(i);
				break;
			}
		}
		ui->qqCoordinates->clear();
		ui->qqCoordinates->setPlainText("");

		std::string tmp;
		for (int i=0;i<int(it->shapingCoordinates.size())/2;i++)
		{
			tmp+=gv::getSizeStr(true,it->shapingCoordinates[2*i+0],0)+",";
			if (i<int(it->shapingCoordinates.size())/2-1)
			{
				tmp+=gv::getSizeStr(true,it->shapingCoordinates[2*i+1],0)+",";
				tmp+=char(13);
				tmp+=char(10);
			}
			else
				tmp+=gv::getSizeStr(true,it->shapingCoordinates[2*i+1],0);
		}
		ui->qqCoordinates->setPlainText(tmp.c_str());

	}
	else
	{
		ui->qqMaxLength->setText("");
		ui->qqScalingFactor->setText("");
		ui->qqCoordinates->setPlainText("");
		ui->qqCoordinates->clear();
	}

	inMainRefreshRoutine=false;
}

CPath* CQDlgPathShaping::getPath()
{
	if (!App::ct->objCont->isLastSelectionAPath())
		return(NULL);
	CPath* it=(CPath*)App::ct->objCont->getLastSelection();
	return(it);
}

void CQDlgPathShaping::on_qqEnabled_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPath* it=getPath();
		if (it!=NULL)
		{
			it->setShapingEnabled(!it->getShapingEnabled());
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgPathShaping::on_qqFollowOrientation_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPath* it=getPath();
		if (it!=NULL)
		{
			it->setShapingFollowFullOrientation(!it->getShapingFollowFullOrientation());
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgPathShaping::on_qqConvexHull_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPath* it=getPath();
		if (it!=NULL)
		{
			it->setShapingThroughConvexHull(!it->getShapingThroughConvexHull());
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgPathShaping::on_qqAdjustColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPath* it=getPath();
		if (it!=NULL)
		{
			int identification[4]={it->getLifeID(),0,it->getID(),-1};
			CQDlgMaterial::display(identification,&it->shapingColor,App::mainWindow,true,true,true,true,true,false,true,false);
		}
	}
}

void CQDlgPathShaping::on_qqGenerateShape_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CPath* it=getPath();
		if (it!=NULL)
		{
			CGeomProxy* geom;
			CShape* shape;
			if (it->getShape(&geom,&shape))
			{
				shape->geomData=geom;
				App::ct->objCont->addObjectToScene(shape,false);
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			}
		}
	}
}

void CQDlgPathShaping::on_qqMaxLength_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPath* it=getPath();
		if (it!=NULL)
		{
			bool ok;
			float newVal=ui->qqMaxLength->text().toFloat(&ok);
			if (ok)
			{
				it->setShapingElementMaxLength(newVal*gv::userToMeter);
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			}
		}
		refresh();
	}
}

void CQDlgPathShaping::on_qqTypeCombo_currentIndexChanged(int index)
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPath* it=getPath();
		if (it==NULL)
			return;
		if (!inMainRefreshRoutine)
		{
			it->setShapingType(ui->qqTypeCombo->itemData(ui->qqTypeCombo->currentIndex()).toInt());
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			refresh();
		}
	}
}

void CQDlgPathShaping::on_qqCyclic_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPath* it=getPath();
		if (it!=NULL)
		{
			it->setShapingSectionClosed(!it->getShapingSectionClosed());
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		refresh();
	}
}

void CQDlgPathShaping::on_qqScalingFactor_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPath* it=getPath();
		if (it!=NULL)
		{
			bool ok;
			float newVal=ui->qqScalingFactor->text().toFloat(&ok);
			if (ok)
			{
				tt::limitValue(0.01f,100.0f,newVal);
				it->setShapingScaling(newVal);
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			}
		}
		refresh();
	}
}

void CQDlgPathShaping::on_qqCoordinates_textChanged()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CPath* it=getPath();
		if ( (it!=NULL)&&(!inMainRefreshRoutine) )
		{
			it->shapingCoordinates.clear();
			std::string str=ui->qqCoordinates->toPlainText().toStdString();
			float tmp;
			while (_extractOneFloat(str,tmp))
				it->shapingCoordinates.push_back(tmp);
			if (it->shapingCoordinates.size()&1)
				it->shapingCoordinates.push_back(0.0f); // no odd number!
			it->setShapingElementMaxLength(it->getShapingElementMaxLength()); // To trigger an actualization!
			App::ct->undoBufferContainer->announceChangeGradual(); // **************** UNDO THINGY ****************
			// No refresh here!! (otherwise we can't edit the item properly)
		}
	//	refresh();
	}
}

bool CQDlgPathShaping::_extractOneFloat(std::string& txt,float& val)
{
	if (txt=="")
		return(false);
	std::string nb;
	bool brokenOut=false;
	for (int i=0;i<int(txt.length());i++)
	{
		if (txt[i]!=',')
		{
			if ((txt[i]!=10)&&(txt[i]!=13))
				nb+=txt[i];
		}
		else
		{
			txt.erase(txt.begin(),txt.begin()+i+1);
			brokenOut=true;
			break;
		}
	}
	if (!brokenOut)
		txt="";
	if (!tt::getValidFloat(nb,val))
		val=0.0f;
	return(true);
}
