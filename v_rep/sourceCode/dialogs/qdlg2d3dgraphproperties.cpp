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
#include "qdlg2d3dgraphproperties.h"
#include "ui_qdlg2d3dgraphproperties.h"
#include "GV.h"
#include "Tt.h"
#include "GraphingRoutines.h"
#include <QShortcut>
#include "editboxdelegate.h"
#include "qdlgcolor.h"
#include "qdlgmaterial.h"
#include "IloIlo.h"
#include "qdlgaddgraphcurve.h"
#include "v_repStrings.h"
#include "App.h"

CQDlg2D3DGraphProperties::CQDlg2D3DGraphProperties(QWidget *parent) :
	VDialog(parent,QT_MODAL_DLG_STYLE),
    ui(new Ui::CQDlg2D3DGraphProperties)
{
    ui->setupUi(this);
	inListSelectionRoutine=false;
	QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	connect(shortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	QShortcut* shortcut2 = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
	connect(shortcut2,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	CEditBoxDelegate* delegate=new CEditBoxDelegate();
	ui->qqList->setItemDelegate(delegate);
}

CQDlg2D3DGraphProperties::~CQDlg2D3DGraphProperties()
{
    delete ui;
}

void CQDlg2D3DGraphProperties::cancelEvent()
{
//	defaultModalDialogEndRoutine(false);
}

void CQDlg2D3DGraphProperties::okEvent()
{
//	defaultModalDialogEndRoutine(true);
}

void CQDlg2D3DGraphProperties::onDeletePressed()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		if (focusWidget()==ui->qqList)
		{
			int objID=getSelectedObjectID();
			if (objID!=-1)
			{
				if (xyGraph)
					it->remove2DPartners(objID);
				else
					it->remove3DPartners(objID);
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				refresh();
			}
			return;
		}
	}
}

void CQDlg2D3DGraphProperties::refresh()
{
	if (xyGraph)
	{
		setWindowTitle(strTranslate(IDSN_XY_CURVES));
		ui->qqGroupBox->setTitle(strTranslate(IDSN_XY_CURVE_PROPERTIES));
	}
	else
	{
		setWindowTitle(strTranslate(IDSN_3D_CURVES));
		ui->qqGroupBox->setTitle(strTranslate(IDSN_3D_CURVE_PROPERTIES));
	}

	ui->qqWidthLabel->setVisible(!xyGraph);
	ui->qqWidth->setVisible(!xyGraph);
	ui->qqRelativeToGraph->setVisible(!xyGraph);
	ui->qqRelativeToWorld->setVisible(!xyGraph);
	ui->qqAlwaysOnTop->setVisible(!xyGraph);


	CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
	CGraphDataComb* graphData=NULL;
	int graphDataId=getSelectedObjectID();
	if (xyGraph)
		graphData=it->getGraphData2D(graphDataId);
	else
		graphData=it->getGraphData3D(graphDataId);

	ui->qqVisible->setEnabled(graphData!=NULL);
	ui->qqDisplayLabel->setEnabled(graphData!=NULL);
	ui->qqLinkPoints->setEnabled(graphData!=NULL);
	ui->qqAdjustColor->setEnabled(graphData!=NULL);
	ui->qqDuplicate->setEnabled(graphData!=NULL);
	ui->qqWidth->setEnabled((graphData!=NULL)&&(!xyGraph));
	ui->qqRelativeToGraph->setEnabled((graphData!=NULL)&&(!xyGraph));
	ui->qqRelativeToWorld->setEnabled((graphData!=NULL)&&(!xyGraph));
	ui->qqAlwaysOnTop->setVisible((graphData!=NULL)&&(!xyGraph));

	ui->qqVisible->setChecked((graphData!=NULL)&&graphData->getVisible());
	ui->qqDisplayLabel->setChecked((graphData!=NULL)&&graphData->getLabel());
	ui->qqLinkPoints->setChecked((graphData!=NULL)&&graphData->getLinkPoints());
	ui->qqRelativeToGraph->setChecked((graphData!=NULL)&&(!graphData->getCurveRelativeToWorld()));
	ui->qqRelativeToWorld->setChecked((graphData!=NULL)&&(graphData->getCurveRelativeToWorld()));
	ui->qqAlwaysOnTop->setChecked((graphData!=NULL)&&(graphData->getVisibleOnTopOfEverything()));

	if ((graphData!=NULL)&&(!xyGraph))
		ui->qqWidth->setText(tt::FNb(0,int(graphData->get3DCurveWidth()+0.1f),false).c_str());
	else
		ui->qqWidth->setText("");

	if (!inListSelectionRoutine)
	{
		updateObjectsInList();
		selectObjectInList(graphDataId);
	}

}


void CQDlg2D3DGraphProperties::updateObjectsInList()
{
	ui->qqList->clear();
	if (!App::ct->objCont->isLastSelectionAGraph())
		return;
	CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
	if (xyGraph)
	{
		for (int i=0;i<int(it->twoDPartners.size());i++)
		{
			CGraphDataComb* aa=it->twoDPartners[i];
			std::string tmp(aa->getName());
			tmp+=" [";
			if (aa->data[0]==-1)
				tmp+="0.0 - ";
			else
				tmp+=it->getGraphData(aa->data[0])->getName()+" - ";
			if (aa->data[1]==-1)
				tmp+="0.0]";
			else
				tmp+=it->getGraphData(aa->data[1])->getName()+"]";

			QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
			itm->setData(Qt::UserRole,QVariant(aa->getIdentifier()));
			itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
			ui->qqList->addItem(itm);
		}
	}
	else
	{
		for (int i=0;i<int(it->threeDPartners.size());i++)
		{
			CGraphDataComb* aa=it->threeDPartners[i];

			std::string tmp(aa->getName());
			tmp+=" [";
			if (aa->data[0]==-1)
				tmp+="0.0 - ";
			else
				tmp+=it->getGraphData(aa->data[0])->getName()+" - ";
			if (aa->data[1]==-1)
				tmp+="0.0 - ";
			else
				tmp+=it->getGraphData(aa->data[1])->getName()+" - ";
			if (aa->data[2]==-1)
				tmp+="0.0]";
			else
				tmp+=it->getGraphData(aa->data[2])->getName()+"]";

			QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
			itm->setData(Qt::UserRole,QVariant(aa->getIdentifier()));
			itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
			ui->qqList->addItem(itm);
		}
	}
}

int CQDlg2D3DGraphProperties::getSelectedObjectID()
{
	QList<QListWidgetItem*> sel=ui->qqList->selectedItems();
	if (sel.size()>0)
		return(sel.at(0)->data(Qt::UserRole).toInt());
	return(-1);
}

void CQDlg2D3DGraphProperties::selectObjectInList(int objectID)
{
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
}

void CQDlg2D3DGraphProperties::on_qqAddNewCurve_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CQDlgAddGraphCurve theDialog(this);
		theDialog.xyGraph=xyGraph;
		theDialog.refresh();
		if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
		{
			updateObjectsInList();
			selectObjectInList(theDialog.newID);
		}
	}
}

void CQDlg2D3DGraphProperties::on_qqList_itemSelectionChanged()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		int objID=getSelectedObjectID();
		CGraphDataComb* grData;
		if (xyGraph)
			grData=it->getGraphData2D(objID);
		else
			grData=it->getGraphData3D(objID);
		if (grData!=NULL)
			((CEditBoxDelegate*)ui->qqList->itemDelegate())->initialText=grData->getName();
		else
			((CEditBoxDelegate*)ui->qqList->itemDelegate())->initialText="";
		inListSelectionRoutine=true;
		refresh();
		inListSelectionRoutine=false;
	}
}

void CQDlg2D3DGraphProperties::on_qqList_itemChanged(QListWidgetItem *item)
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		if ( (item!=NULL)&&(it!=NULL) )
		{
			std::string newName(item->text().toStdString());
			CGraphDataComb* grData;
			if (xyGraph)
				grData=it->getGraphData2D(item->data(Qt::UserRole).toInt());
			else
				grData=it->getGraphData3D(item->data(Qt::UserRole).toInt());
			if ( (grData!=NULL)&&(newName!="") )
			{
				if (grData->getName()!=newName)
				{
					tt::removeIllegalCharacters(newName,false);
					if (xyGraph)
					{
						if (it->getGraphData2D(newName)==NULL)
						{
							grData->setName(newName);
							App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
						}
					}
					else
					{
						if (it->getGraphData3D(newName)==NULL)
						{
							grData->setName(newName);
							App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
						}
					}
				}
			}
			refresh();
		}
	}
}

void CQDlg2D3DGraphProperties::on_qqVisible_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		if (xyGraph)
		{
			CGraphDataComb* grDataComb=it->getGraphData2D(getSelectedObjectID());
			if (grDataComb==NULL)
				return;
			grDataComb->setVisible(!grDataComb->getVisible());
		}
		else
		{
			CGraphDataComb* grDataComb=it->getGraphData3D(getSelectedObjectID());
			if (grDataComb==NULL)
				return;
			grDataComb->setVisible(!grDataComb->getVisible());
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlg2D3DGraphProperties::on_qqDisplayLabel_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		if (xyGraph)
		{
			CGraphDataComb* grDataComb=it->getGraphData2D(getSelectedObjectID());
			if (grDataComb==NULL)
				return;
			grDataComb->setLabel(!grDataComb->getLabel());
		}
		else
		{
			CGraphDataComb* grDataComb=it->getGraphData3D(getSelectedObjectID());
			if (grDataComb==NULL)
				return;
			grDataComb->setLabel(!grDataComb->getLabel());
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlg2D3DGraphProperties::on_qqLinkPoints_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		if (xyGraph)
		{
			CGraphDataComb* grDataComb=it->getGraphData2D(getSelectedObjectID());
			if (grDataComb==NULL)
				return;
			grDataComb->setLinkPoints(!grDataComb->getLinkPoints());
		}
		else
		{
			CGraphDataComb* grDataComb=it->getGraphData3D(getSelectedObjectID());
			if (grDataComb==NULL)
				return;
			grDataComb->setLinkPoints(!grDataComb->getLinkPoints());
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlg2D3DGraphProperties::on_qqAdjustColor_clicked()
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		if (xyGraph)
		{
			CGraphDataComb* grDataComb=it->getGraphData2D(getSelectedObjectID());
			if (grDataComb==NULL)
				return;
			int identification[4]={grDataComb->getLifeID(),0,it->getID(),-1};
			CQDlgColor::displayModal(identification,grDataComb->ambientColor,this);
		}
		else
		{
			CGraphDataComb* grDataComb=it->getGraphData3D(getSelectedObjectID());
			if (grDataComb==NULL)
				return;
			int identification[4]={grDataComb->getLifeID(),0,it->getID(),-1};
			CQDlgMaterial::displayModal(identification,this,grDataComb->ambientColor,NULL,NULL,grDataComb->emissiveColor,NULL,NULL,NULL,NULL,NULL);
		}
	}
}

void CQDlg2D3DGraphProperties::on_qqDuplicate_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		if (xyGraph)
		{
			CGraphDataComb* grDataComb=it->getGraphData2D(getSelectedObjectID());
			if (grDataComb==NULL)
				return;
			it->makeCurveStatic(getSelectedObjectID(),1);
		}
		else
		{
			CGraphDataComb* grDataComb=it->getGraphData3D(getSelectedObjectID());
			if (grDataComb==NULL)
				return;
			it->makeCurveStatic(getSelectedObjectID(),2);
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		App::uiThread->messageBox_information(this,strTranslate(IDSN_GRAPH_CURVE),strTranslate(IDSN_CURVE_WAS_DUPLICATED_TO_STATIC),VMessageBox::OKELI);
		refresh();
	}
}

void CQDlg2D3DGraphProperties::on_qqWidth_editingFinished()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		if (!xyGraph)
		{
			CGraphDataComb* grDataComb=it->getGraphData3D(getSelectedObjectID());
			if (grDataComb==NULL)
				return;
			bool ok;
			int newVal=ui->qqWidth->text().toInt(&ok);
			if (ok)
				grDataComb->set3DCurveWidth(float(newVal));
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlg2D3DGraphProperties::on_qqRelativeToGraph_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		if (!xyGraph)
		{
			CGraphDataComb* grDataComb=it->getGraphData3D(getSelectedObjectID());
			if (grDataComb==NULL)
				return;
			grDataComb->setCurveRelativeToWorld(false);
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlg2D3DGraphProperties::on_qqRelativeToWorld_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		if (!xyGraph)
		{
			CGraphDataComb* grDataComb=it->getGraphData3D(getSelectedObjectID());
			if (grDataComb==NULL)
				return;
			grDataComb->setCurveRelativeToWorld(true);
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}

void CQDlg2D3DGraphProperties::on_qqClose_clicked(QAbstractButton *button)
{
	defaultModalDialogEndRoutine(true);
}

void CQDlg2D3DGraphProperties::on_qqAlwaysOnTop_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		CGraph* it=(CGraph*)App::ct->objCont->getLastSelection();
		if (!xyGraph)
		{
			CGraphDataComb* grDataComb=it->getGraphData3D(getSelectedObjectID());
			if (grDataComb==NULL)
				return;
			grDataComb->setVisibleOnTopOfEverything(!grDataComb->getVisibleOnTopOfEverything());
		}
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
		refresh();
	}
}
