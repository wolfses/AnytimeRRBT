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
#include "qdlgfilters.h"
#include "ui_qdlgfilters.h"
#include "App.h"
#include "Tt.h"
#include "GV.h"
#include "IloIlo.h"
#include <QShortcut>
#include "qdlgvisionsensors.h"
#include "pluginContainer.h"
#include "v_repStrings.h"
#include "qdlgsimplefilter_3x3.h"
#include "qdlgsimplefilter_5x5.h"
#include "qdlgsimplefilter_binary.h"
#include "qdlgsimplefilter_blob.h"
#include "qdlgsimplefilter_circularCut.h"
#include "qdlgsimplefilter_colorSegmentation.h"
#include "qdlgsimplefilter_correlation.h"
#include "qdlgsimplefilter_edge.h"
#include "qdlgsimplefilter_intensityScale.h"
#include "qdlgsimplefilter_keepRemoveCols.h"
#include "qdlgsimplefilter_rectangularCut.h"
#include "qdlgsimplefilter_resize.h"
#include "qdlgsimplefilter_rotate.h"
#include "qdlgsimplefilter_scaleCols.h"
#include "qdlgsimplefilter_shift.h"
#include "qdlgsimplefilter_coordExtraction.h"
#include "qdlgsimplefilter_pixelChange.h"
#include "qdlgsimplefilter_uniformImage.h"

CQDlgFilters::CQDlgFilters(QWidget *parent) :
	CDlgEx(parent),
    ui(new Ui::CQDlgFilters)
{
    ui->setupUi(this);
	inMainRefreshRoutine=false;
	currentComboIndex=0;
	QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	connect(shortcut,SIGNAL(activated()), this, SLOT(onDeletePressed()));
	QShortcut* shortcut2 = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
	connect(shortcut2,SIGNAL(activated()), this, SLOT(onDeletePressed()));
}

CQDlgFilters::~CQDlgFilters()
{
    delete ui;
}

void CQDlgFilters::cancelEvent()
{ // no cancel event allowed
	CQDlgVisionSensors::showFilterWindow=false;
	CDlgEx::cancelEvent();
	App::ct->objCont->setFullDialogRefreshFlag();
}

void CQDlgFilters::refresh()
{
	inMainRefreshRoutine=true;

	CVisionSensor* it=NULL;
	if (App::ct->objCont->isLastSelectionAVisionSensor())
		it=(CVisionSensor*)App::ct->objCont->getLastSelection();

	int selectedObjectID=getSelectedObjectID();
	updateObjectsInList();
	selectObjectInList(selectedObjectID);

	bool isSensor=App::ct->objCont->isLastSelectionAVisionSensor();
	bool manySensors=App::ct->objCont->getVisionSensorNumberInSelection()>1;
	bool noEditMode=App::ct->objCont->getEditModeType()==NO_EDIT_MODE;
//	bool noSim=App::ct->simulation->isSimulationStopped();

	ui->qqFilterCombo->setEnabled(isSensor&&noEditMode);
	ui->qqFilterCombo->clear();

	ui->qqAddFilter->setEnabled(isSensor&&noEditMode);

	ui->qqApply->setEnabled(isSensor&&manySensors&&noEditMode);

	if (it!=NULL)
	{
		CComposedFilter* filters=it->getComposedFilter();
		int cnt=0;
		int tpe=CSimpleFilter::getAvailableFilter(cnt++);
		while (tpe!=-1)
		{
			ui->qqFilterCombo->addItem(CSimpleFilter::getSpecificFilterString(tpe).c_str(),QVariant(tpe));
			tpe=CSimpleFilter::getAvailableFilter(cnt++);
		}

		// Select current item:
		if (currentComboIndex>=ui->qqFilterCombo->count())
			currentComboIndex=0;
		ui->qqFilterCombo->setCurrentIndex(currentComboIndex);

		int objectIndexInList=getSelectedObjectID();
		ui->qqUp->setEnabled(isSensor&&noEditMode&&(objectIndexInList>=0));
		ui->qqDown->setEnabled(isSensor&&noEditMode&&(objectIndexInList>=0));

		ui->qqComponentEnabled->setEnabled(isSensor&&noEditMode&&(objectIndexInList>=0));
		if (objectIndexInList>=0)
			ui->qqComponentEnabled->setChecked(filters->getSimpleFilter(objectIndexInList)->getEnabled());
		else
			ui->qqComponentEnabled->setChecked(false);
	}
	else
	{
		ui->qqUp->setEnabled(false);
		ui->qqDown->setEnabled(false);
		ui->qqComponentEnabled->setEnabled(false);
		ui->qqComponentEnabled->setChecked(false);
	}

	inMainRefreshRoutine=false;
}

void CQDlgFilters::onDeletePressed()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (focusWidget()==ui->qqFilterList)
		{
			int objID=getSelectedObjectID();
			if (objID!=-1)
			{
				CVisionSensor* it=App::ct->objCont->getVisionSensor(App::ct->objCont->getLastSelectionID());
				if (it!=NULL)
				{
					CComposedFilter* filters=it->getComposedFilter();
					filters->removeSimpleFilter(objID);
					App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				}
				refresh();
			}
			return;
		}
	}
}

void CQDlgFilters::on_qqFilterCombo_currentIndexChanged(int index)
{
	if (!inMainRefreshRoutine)
		currentComboIndex=index;
}

void CQDlgFilters::updateObjectsInList()
{
	ui->qqFilterList->clear();

	if (!App::ct->objCont->isLastSelectionAVisionSensor())
		return;
	CVisionSensor* it=(CVisionSensor*)App::ct->objCont->getLastSelection();
	CComposedFilter* filters=it->getComposedFilter();

	for (int i=0;i<int(filters->getSimpleFilterCount());i++)
	{
		CSimpleFilter* sf=filters->getSimpleFilter(i);
		std::string tmp(sf->getFilterString().c_str());
		if (!sf->getEnabled())
		{
			tmp+=" ";
			tmp+=std::string(IDS_FILTER_DISABLED);
		}

		if (sf->canFilterBeEdited())
		{
			tmp+=" ";
			tmp+=tt::decorateString("(",strTranslate(IDSN_DOUBLE_CLICK_TO_EDIT),")");
		}

		QListWidgetItem* itm=new QListWidgetItem(tmp.c_str());
		itm->setData(Qt::UserRole,QVariant(i));
		itm->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
		ui->qqFilterList->addItem(itm);
	}
}

int CQDlgFilters::getSelectedObjectID()
{
	QList<QListWidgetItem*> sel=ui->qqFilterList->selectedItems();
	if (sel.size()>0)
		return(sel.at(0)->data(Qt::UserRole).toInt());
	return(-1);
}

void CQDlgFilters::selectObjectInList(int objectID)
{
	for (int i=0;i<ui->qqFilterList->count();i++)
	{
		QListWidgetItem* it=ui->qqFilterList->item(i);
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

void CQDlgFilters::on_qqFilterList_itemDoubleClicked(QListWidgetItem *item)
{
	IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
	{
		if (item!=NULL)
			_editFilter(item->data(Qt::UserRole).toInt());
	}
}

bool CQDlgFilters::_editFilter(int index)
{
	bool retVal=false;
	CVisionSensor* it=App::ct->objCont->getVisionSensor(App::ct->objCont->getLastSelectionID());
	if (it!=NULL)
	{
		CComposedFilter* filters=it->getComposedFilter();
		CSimpleFilter* filter=filters->getSimpleFilter(index);
		if ((filter!=NULL)&&(filter->canFilterBeEdited()))
		{
			int ft=filter->getFilterType();

			if (ft==simple_filter_rotate)
			{
				CQDlgSimpleFilter_rotate theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft==simple_filter_uniformimage)
			{
				CQDlgSimpleFilter_uniformImage theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft==simple_filter_3x3filter)
			{
				CQDlgSimpleFilter_3x3 theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft==simple_filter_5x5filter)
			{
				CQDlgSimpleFilter_5x5 theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft==simple_filter_keeporremovecolors)
			{
				CQDlgSimpleFilter_keepRemoveCols theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft==simple_filter_scaleandoffsetcolors)
			{
				CQDlgSimpleFilter_scaleCols theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft==simple_filter_correlationwithbuffer1)
			{
				CQDlgSimpleFilter_correlation theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft==simple_filter_shift)
			{
				CQDlgSimpleFilter_shift theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft==simple_filter_imagetocoord)
			{
				CQDlgSimpleFilter_coordExtraction theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft==simple_filter_pixelchange)
			{
				CQDlgSimpleFilter_pixelChange theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft==simple_filter_circularcut)
			{
				CQDlgSimpleFilter_circularCut theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft==simple_filter_colorsegmentation)
			{
				CQDlgSimpleFilter_colorSegmentation theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft==simple_filter_rectangularcut)
			{
				CQDlgSimpleFilter_rectangularCut theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft==simple_filter_intensityscale)
			{
				CQDlgSimpleFilter_intensityScale theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft==simple_filter_binary)
			{
				CQDlgSimpleFilter_binary theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft==simple_filter_edge)
			{
				CQDlgSimpleFilter_edge theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft==simple_filter_blobextraction)
			{
				CQDlgSimpleFilter_blob theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft==simple_filter_resize)
			{
				CQDlgSimpleFilter_resize theDialog(this);
				theDialog.initializeDialogValues(filter);
				if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
				{
					theDialog.retrieveDialogValues(filter);
					retVal=true;
				}
			}
			if (ft>=simple_filter_customized)//&&(CIloIlo::eventCallback!=NULL))
			{
				std::vector<BYTE> params;
				filter->getCustomFilterParameters(params);
				int header,id;
				filter->getCustomFilterInfo(header,id);
				int auxData[4]={header,id,params.size(),-1};
				int retVals[4]={-1,-1,-1,-1};
				void* callbackReturnVal=NULL;
				if (params.size()==0)
					callbackReturnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_imagefilter_adjustparams,auxData,NULL,retVals);
				else
					callbackReturnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_imagefilter_adjustparams,auxData,&params[0],retVals);
				std::vector<BYTE> paramsNew;
				if ( (retVals[0]>0)&&(callbackReturnVal!=NULL) )
				{
					for (int i=0;i<retVals[0];i++)
						paramsNew.push_back(((BYTE*)callbackReturnVal)[i]);
				}
				if (params.size()==paramsNew.size())
				{
					for (int i=0;i<int(params.size());i++)
					{
						if (params[i]!=paramsNew[i])
							retVal=true; // there was a change
					}
				}
				else
					retVal=true; // there was a change
				filter->setCustomFilterParameters(paramsNew);
				delete[] ((char*)callbackReturnVal);
			}
		}
	}
	if (retVal)
		App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
	return(retVal);
}

void CQDlgFilters::on_qqAddFilter_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		CVisionSensor* it=App::ct->objCont->getVisionSensor(App::ct->objCont->getLastSelectionID());
		if (it!=NULL)
		{
			CComposedFilter* filters=it->getComposedFilter();
			CSimpleFilter* sf=new CSimpleFilter();
			int objID=ui->qqFilterCombo->itemData(ui->qqFilterCombo->currentIndex()).toInt();
			sf->setFilterType(objID);
			filters->insertSimpleFilter(sf);
			App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			updateObjectsInList();
			selectObjectInList(filters->getSimpleFilterCount()-1);
			refresh();
		}
	}
}

void CQDlgFilters::on_qqUp_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		int data=getSelectedObjectID();
		CVisionSensor* it=App::ct->objCont->getVisionSensor(App::ct->objCont->getLastSelectionID());
		if ( (it!=NULL)&&(data>=0) )
		{
			CComposedFilter* filters=it->getComposedFilter();
			if (data<filters->getSimpleFilterCount())
			{
				if (filters->moveSimpleFilter(data,true))
					data--;
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				selectObjectInList(data);
				refresh();
			}
		}
	}
}

void CQDlgFilters::on_qqDown_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		int data=getSelectedObjectID();
		CVisionSensor* it=App::ct->objCont->getVisionSensor(App::ct->objCont->getLastSelectionID());
		if ( (it!=NULL)&&(data>=0) )
		{
			CComposedFilter* filters=it->getComposedFilter();
			if (data<filters->getSimpleFilterCount())
			{
				if (filters->moveSimpleFilter(data,false))
					data++;
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				selectObjectInList(data);
				refresh();
			}
		}
	}
}

void CQDlgFilters::on_qqComponentEnabled_clicked()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		int data=getSelectedObjectID();
		CVisionSensor* it=App::ct->objCont->getVisionSensor(App::ct->objCont->getLastSelectionID());
		if ( (it!=NULL)&&(data>=0) )
		{
			CComposedFilter* filters=it->getComposedFilter();
			if (data<filters->getSimpleFilterCount())
			{
				filters->getSimpleFilter(data)->setEnabled(!filters->getSimpleFilter(data)->getEnabled());
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
				refresh();
			}
		}
	}
}

void CQDlgFilters::on_qqFilterList_itemSelectionChanged()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		if (!inMainRefreshRoutine)
			refresh();
	}
}

void CQDlgFilters::on_qqApply_clicked()
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (!App::ct->objCont->isLastSelectionAVisionSensor())
			return;
		CVisionSensor* last=(CVisionSensor*)App::ct->objCont->getLastSelection();
		for (int i=0;i<App::ct->objCont->getSelSize()-1;i++)
		{
			CVisionSensor* it=App::ct->objCont->getVisionSensor(App::ct->objCont->getSelID(i));
			if (it!=NULL)
			{
				it->setComposedFilter(last->getComposedFilter()->copyYourself());
				App::ct->undoBufferContainer->announceChangeStart(); // ************************** UNDO thingy **************************
			}
		}
		App::ct->undoBufferContainer->announceChangeEnd(); // ************************** UNDO thingy **************************
	}
}
