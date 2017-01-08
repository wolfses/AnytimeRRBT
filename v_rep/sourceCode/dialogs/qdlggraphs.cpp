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
#include "qdlggraphs.h"
#include "ui_qdlggraphs.h"
#include "Tt.h"
#include "GV.h"
#include "qdlgmaterial.h"
#include "IloIlo.h"
#include "qdlgdatastreamselection.h"
#include "GraphingRoutines.h"
#include "editboxdelegate.h"
#include "qdlgcolor.h"
#include "qdlg2d3dgraphproperties.h"
#include "v_repStrings.h"
#include "App.h"

CQDlgGraphs::CQDlgGraphs(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgGraphs)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
	inListSelectionRoutine=false;
	noListSelectionAllowed=false;
	delKeyShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	connect(delKeyShortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	backspaceKeyShortcut = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
	connect(backspaceKeyShortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	CEditBoxDelegate* delegate=new CEditBoxDelegate();
	ui->qqRecordingList->setItemDelegate(delegate);
}

CQDlgGraphs::~CQDlgGraphs()
{
    delete ui;
}

void CQDlgGraphs::cancelEvent()
{
	// we override this cancel event. The container window should close, not this one!!
	App::mainWindow->dlgCont->close(OBJECT_DLG);
}

void CQDlgGraphs::refresh()
{
	inMainRefreshRoutine=true;
	bool sel=App::ct->objCont->isLastSelectionAGraph();
	bool simStopped=App::ct->simulation->isSimulationStopped();
	bool noEditMode=App::ct->objCont->getEditModeType()==NO_EDIT_MODE;
	int streamId=-1;
	CGraph* it=NULL;
	CGraphData* graphData=NULL;
	if (sel)
	{
		it=(CGraph*)App::ct->objCont->getLastSelection();
		streamId=getSelectedObjectID();
		graphData=it->getGraphData(streamId);
	}

	ui->qqExplicitHandling->setEnabled(sel&&noEditMode);
	ui->qqObjectSize->setEnabled(sel&&simStopped&&noEditMode);
	ui->qqBufferIsCyclic->setEnabled(sel&&simStopped&&noEditMode);
	ui->qqBufferSize->setEnabled(sel&&simStopped&&noEditMode);
	ui->qqShowXYZPlanes->setEnabled(sel&&noEditMode);
	ui->qqRemoveAllStatics->setEnabled(sel&&noEditMode);

	ui->qqDisplayGrids->setEnabled(sel&&noEditMode);
	ui->qqDisplayGridValues->setEnabled(sel&&noEditMode);
	ui->qqAdjustBackgroundColor->setEnabled(sel&&noEditMode&&simStopped);
	ui->qqAdjustGridColor->setEnabled(sel&&noEditMode&&simStopped);

	ui->qqAddNewDataStream->setEnabled(sel&&noEditMode&&simStopped);

	ui->qqRecordingList->setEnabled(sel&&noEditMode);

	ui->qqTransformationCombo->setEnabled(sel&&noEditMode&&(graphData!=NULL));
	ui->qqTransformationCoeff->setEnabled(sel&&noEditMode&&(graphData!=NULL));
	ui->qqTransformationOffset->setEnabled(sel&&noEditMode&&(graphData!=NULL));
	ui->qqMovingAveragePeriod->setEnabled(sel&&noEditMode&&(graphData!=NULL));

	ui->qqTimeGraphVisible->setEnabled(sel&&noEditMode&&(graphData!=NULL));
	ui->qqShowLabel->setEnabled(sel&&noEditMode&&(graphData!=NULL));
	ui->qqLinkPoints->setEnabled(sel&&noEditMode&&(graphData!=NULL));
	ui->qqAdjustCurveColor->setEnabled(sel&&noEditMode&&(graphData!=NULL));
	ui->qqDuplicateToStatic->setEnabled(sel&&noEditMode&&(graphData!=NULL));

	ui->qqEditXYGraphs->setEnabled(sel&&noEditMode);
	ui->qqEdit3DCurves->setEnabled(sel&&noEditMode);

	ui->qqExplicitHandling->setChecked(sel&&it->getExplicitHandling());
	ui->qqBufferIsCyclic->setChecked(sel&&it->getCyclic());
	ui->qqShowXYZPlanes->setChecked(sel&&it->xYZPlanesDisplay);

	ui->qqDisplayGrids->setChecked(sel&&it->graphGrid);
	ui->qqDisplayGridValues->setChecked(sel&&it->graphValues);

	ui->qqTimeGraphVisible->setChecked(sel&&(graphData!=NULL)&&graphData->getVisible());
	ui->qqShowLabel->setChecked(sel&&(graphData!=NULL)&&graphData->getLabel());
	ui->qqLinkPoints->setChecked(sel&&(graphData!=NULL)&&graphData->getLinkPoints());

	ui->qqTransformationCombo->clear();

	if (!inListSelectionRoutine)
	{
		updateObjectsInList();
		selectObjectInList(streamId);
	}

	if (sel)
	{
		ui->qqObjectSize->setText(gv::getSizeStr(false,it->getSize()).c_str());
		ui->qqBufferSize->setText(tt::FNb(0,it->getBufferSize()).c_str());

		ui->qqTransformationCombo->clear();
		ui->qqTransformationCombo->addItem(strTranslate(IDS_RAW),QVariant(DATA_STREAM_ORIGINAL));
		ui->qqTransformationCombo->addItem(strTranslate(IDS_DERIVATIVE),QVariant(DATA_STREAM_DERIVATIVE));
		ui->qqTransformationCombo->addItem(strTranslate(IDS_INTEGRAL),QVariant(DATA_STREAM_INTEGRAL));
		ui->qqTransformationCombo->addItem(strTranslate(IDS_CUMULATIVE),QVariant(DATA_STREAM_CUMULATIVE));

		if (graphData!=NULL)
		{
			for (int i=0;i<ui->qqTransformationCombo->count();i++)
			{
				if (ui->qqTransformationCombo->itemData(i).toInt()==graphData->getDerivativeIntegralAndCumulative())
				{
					ui->qqTransformationCombo->setCurrentIndex(i);
					break;
				}
			}

			ui->qqTransformationCoeff->setText(tt::FNb(0,graphData->getZoomFactor(),3,true).c_str());
			ui->qqTransformationOffset->setText(tt::FNb(0,graphData->getAddCoeff(),3,true).c_str());
			ui->qqMovingAveragePeriod->setText(tt::FNb(0,graphData->getMovingAverageCount()).c_str());
		}
		else
		{
			ui->qqTransformationCoeff->setText("");
			ui->qqTransformationOffset->setText("");
			ui->qqMovingAveragePeriod->setText("");
		}
	}
	else
	{
		ui->qqObjectSize->setText("");
		ui->qqBufferSize->setText("");

		ui->qqTransformationCoeff->setText("");
		ui->qqTransformationOffset->setText("");
		ui->qqMovingAveragePeriod->setText("");
	}
	inMainRefreshRoutine=false;
}


void CQDlgGraphs::updateObjectsInList()
{
	noListSelectionAllowed=true;
	ui->qqRecordingList->clear();
	noListSelectionAllowed=false;
	if (!App::ct->objCont->isLastSelectionAGraph())
		return;
	CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
	noListSelectionAllowed=true;
	for (int i=0;i<int(it->daten.size());i++)
	{
		int dataType=it->daten[i]->getDataType();
		int dataObjectID=it->daten[i]->getDataObjectID();
		int theID=it->daten[i]->getIdentifier();
		std::string tmp=it->daten[i]->getName();
		tmp=tmp.append(" [");
		std::string tmp2=IDS_ERROR;
		CGraphingRoutines::loopThroughAllAndGetGraphObjectName(dataType,dataObjectID,tmp2);
		tmp=tmp.append(tmp2);
		tmp=tmp.append("]");
		QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
		itm->setData(Qt::UserRole,QVariant(theID));
		itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
		ui->qqRecordingList->addItem(itm);
	}
	noListSelectionAllowed=false;
}

int CQDlgGraphs::getSelectedObjectID()
{
	QList<QListWidgetItem*> sel=ui->qqRecordingList->selectedItems();
	if (sel.size()>0)
		return(sel.at(0)->data(Qt::UserRole).toInt());
	return(-1);
}

void CQDlgGraphs::selectObjectInList(int objectID)
{
	noListSelectionAllowed=true;
	for (int i=0;i<ui->qqRecordingList->count();i++)
	{
		QListWidgetItem* it=ui->qqRecordingList->item(i);
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


void CQDlgGraphs::on_qqAddNewDataStream_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CQDlgDataStreamSelection theDialog(this);
		theDialog.refresh();
		if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
		{
			int currentDataType=theDialog.box1Id;
			int index=theDialog.box2Id;

			int objID;
			CGraphingRoutines::loopThroughAllAndGetObjectsFromGraphCategory(index,currentDataType,objID);
			CGraphData* newGraphDat=new CGraphData(currentDataType,objID,-1);
			CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
			int theID=it->addNewGraphData(newGraphDat);

			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			updateObjectsInList();
			selectObjectInList(theID);
		}
		refresh();
	}
}

void CQDlgGraphs::onDeletePressed()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		if (focusWidget()==ui->qqRecordingList)
		{
			int objID=getSelectedObjectID();
			if (objID!=-1)
			{
				it->removeGraphData(objID);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				refresh();
			}
			return;
		}
	}
}

void CQDlgGraphs::on_qqRecordingList_itemSelectionChanged()
{
	if (!noListSelectionAllowed)
	{
		IF_UI_EVENT_CAN_WRITE_DATA
		{
			if (!App::ct->objCont->isLastSelectionAGraph())
				return;
			CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
			int objID=getSelectedObjectID();
			CGraphData* grData=it->getGraphData(objID);
			if (grData!=NULL)
				((CEditBoxDelegate*)ui->qqRecordingList->itemDelegate())->initialText=grData->getName();
			else
				((CEditBoxDelegate*)ui->qqRecordingList->itemDelegate())->initialText="";
			inListSelectionRoutine=true;
			refresh();
			inListSelectionRoutine=false;
		}
	}
}

void CQDlgGraphs::on_qqRecordingList_itemChanged(QListWidgetItem *item)
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		if ( (item!=NULL)&&(it!=NULL) )
		{
			std::string newName(item->text().toStdString());
			CGraphData* grData=it->getGraphData(item->data(Qt::UserRole).toInt());
			if ( (grData!=NULL)&&(newName!="") )
			{
				if (grData->getName()!=newName)
				{
					tt::removeIllegalCharacters(newName,false);
					if (it->getGraphData(newName)==NULL)
					{
						grData->setName(newName);
						App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
					}
				}
			}
			refresh();
		}
	}
}

void CQDlgGraphs::on_qqExplicitHandling_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		it->setExplicitHandling(!it->getExplicitHandling());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgGraphs::on_qqBufferIsCyclic_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		it->setCyclic(!it->getCyclic());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgGraphs::on_qqShowXYZPlanes_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		it->xYZPlanesDisplay=!it->xYZPlanesDisplay;
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgGraphs::on_qqObjectSize_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		bool ok;
		float newVal=ui->qqObjectSize->text().toFloat(&ok);
		if (ok)
			it->setSize(newVal*gv::userToMeter);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgGraphs::on_qqBufferSize_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		bool ok;
		int newVal=ui->qqBufferSize->text().toInt(&ok);
		if (ok)
			it->setBufferSize(newVal);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgGraphs::on_qqRemoveAllStatics_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		it->removeAllStatics();
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgGraphs::on_qqDisplayGrids_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		it->graphGrid=!it->graphGrid;
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgGraphs::on_qqDisplayGridValues_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		it->graphValues=!it->graphValues;
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgGraphs::on_qqAdjustBackgroundColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgColor::display(identification,it->backgroundColor,App::mainWindow);
	}
}

void CQDlgGraphs::on_qqAdjustGridColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		int identification[4]={it->getLifeID(),0,it->getID(),-1};
		CQDlgColor::display(identification,it->textColor,App::mainWindow);
	}
}

void CQDlgGraphs::on_qqTransformationCombo_currentIndexChanged(int index)
{
	if (!inMainRefreshRoutine)
	{
		IF_UI_EVENT_CAN_WRITE_DATA
		{
			if (!App::ct->objCont->isLastSelectionAGraph())
				return;
			CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
			CGraphData* grData=it->getGraphData(getSelectedObjectID());
			if (grData!=NULL)
			{
				int objID=ui->qqTransformationCombo->itemData(ui->qqTransformationCombo->currentIndex()).toInt();
				grData->setDerivativeIntegralAndCumulative(objID);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			}
			refresh();
		}
	}
}

void CQDlgGraphs::on_qqTransformationCoeff_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		CGraphData* grData=it->getGraphData(getSelectedObjectID());
		if (grData==NULL)
			return;
		bool ok;
		float newVal=ui->qqTransformationCoeff->text().toFloat(&ok);
		if (ok)
			grData->setZoomFactor(newVal);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgGraphs::on_qqTransformationOffset_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		CGraphData* grData=it->getGraphData(getSelectedObjectID());
		if (grData==NULL)
			return;
		bool ok;
		float newVal=ui->qqTransformationOffset->text().toFloat(&ok);
		if (ok)
			grData->setAddCoeff(newVal);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgGraphs::on_qqMovingAveragePeriod_editingFinished()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		CGraphData* grData=it->getGraphData(getSelectedObjectID());
		if (grData==NULL)
			return;
		bool ok;
		int newVal=ui->qqMovingAveragePeriod->text().toInt(&ok);
		if (ok)
			grData->setMovingAverageCount(newVal);
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgGraphs::on_qqTimeGraphVisible_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		CGraphData* grData=it->getGraphData(getSelectedObjectID());
		if (grData==NULL)
			return;
		grData->setVisible(!grData->getVisible());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgGraphs::on_qqShowLabel_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		CGraphData* grData=it->getGraphData(getSelectedObjectID());
		if (grData==NULL)
			return;
		grData->setLabel(!grData->getLabel());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgGraphs::on_qqLinkPoints_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		CGraphData* grData=it->getGraphData(getSelectedObjectID());
		if (grData==NULL)
			return;
		grData->setLinkPoints(!grData->getLinkPoints());
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlgGraphs::on_qqAdjustCurveColor_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		CGraphData* grData=it->getGraphData(getSelectedObjectID());
		if (grData==NULL)
			return;
		int identification[4]={grData->getLifeID(),0,it->getID(),-1};
		CQDlgColor::display(identification,grData->ambientColor,App::mainWindow);
	}
}

void CQDlgGraphs::on_qqDuplicateToStatic_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		CGraphData* grData=it->getGraphData(getSelectedObjectID());
		if (grData==NULL)
			return;
		if (grData->getDataLength()!=0)
		{
			it->makeCurveStatic(getSelectedObjectID(),0);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			App::uiThread->messageBox_information(this,strTranslate(IDSN_GRAPH_CURVE),strTranslate(IDSN_CURVE_WAS_DUPLICATED_TO_STATIC),VMessageBox::OKELI);
			refresh();
		}
	}
}

void CQDlgGraphs::on_qqEditXYGraphs_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CQDlg2D3DGraphProperties theDialog(this);
		theDialog.xyGraph=true;
		theDialog.refresh();
		delKeyShortcut->setEnabled(false);
		backspaceKeyShortcut->setEnabled(false);
		theDialog.makeDialogModal();
		backspaceKeyShortcut->setEnabled(true);
		delKeyShortcut->setEnabled(true);
	}
}

void CQDlgGraphs::on_qqEdit3DCurves_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		if (!App::ct->objCont->isLastSelectionAGraph())
			return;
		CQDlg2D3DGraphProperties theDialog(this);
		theDialog.xyGraph=false;
		theDialog.refresh();
		delKeyShortcut->setEnabled(false);
		backspaceKeyShortcut->setEnabled(false);
		theDialog.makeDialogModal();
		backspaceKeyShortcut->setEnabled(true);
		delKeyShortcut->setEnabled(true);
	}
}
