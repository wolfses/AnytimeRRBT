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
#include "v_rep_internal.h"
#include "PathEdition.h"

#include "OGL.h"
#include "IloIlo.h"
#include "App.h"
#include "v_repStrings.h"

void CPathEdition::_copySelection(std::vector<int>* selection)
{
	if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
	{ // We copy the path points in the order they are in the path
		for (int i=0;i<int(App::ct->objCont->editBufferPathPointsCopy.size());i++)
			delete App::ct->objCont->editBufferPathPointsCopy[i];
		App::ct->objCont->editBufferPathPointsCopy.clear();
		int maxIndex=-1;
		for (int i=0;i<int(selection->size());i++)
		{
			if (selection->at(i)>maxIndex)
				maxIndex=selection->at(i);
		}
		std::vector<bool> toCopy(maxIndex+1,false);
		for (int i=0;i<int(selection->size());i++)
			toCopy[selection->at(i)]=true;
		for (int i=0;i<int(toCopy.size());i++)
		{
			if (toCopy[i])
			{
				CSimplePathPoint* it=App::ct->objCont->_editionPath->getSimplePathPoint(i);
				if (it!=NULL)
					App::ct->objCont->editBufferPathPointsCopy.push_back(it->copyYourself());
			}
		}
		App::ct->objCont->deselectEditModeBuffer();
		App::ct->objCont->setLightDialogRefreshFlag();
	}
}

void CPathEdition::_paste(int insertPosition)
{
	if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
	{
		App::ct->objCont->deselectEditModeBuffer();
		App::ct->objCont->_editionPath->enableActualization(false);
		insertPosition++;
		for (int i=0;i<int(App::ct->objCont->editBufferPathPointsCopy.size());i++)
		{
			App::ct->objCont->_editionPath->insertSimplePathPoint(App::ct->objCont->editBufferPathPointsCopy[i]->copyYourself(),insertPosition+i);
			App::ct->objCont->addItemToEditModeBuffer(insertPosition+i);
		}
		App::ct->objCont->_editionPath->enableActualization(true);
		App::ct->objCont->_editionPath->actualizePath();
		App::ct->objCont->setFullDialogRefreshFlag();
	}
}
void CPathEdition::_insertNewPoint(int insertPosition)
{
	if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
	{
		App::ct->objCont->deselectEditModeBuffer();
		if (insertPosition==-1)
			insertPosition=0;
		CSimplePathPoint* it=App::ct->objCont->_editionPath->getSimplePathPoint(insertPosition);
		if (it==NULL)
		{
			it=new CSimplePathPoint();
			C7Vector tr(it->getTransformation());
			tr.X(2)=0.1f;
			it->setTransformation(tr,App::ct->objCont->_editionPath->getAttributes());
		}
		else
		{
			it=it->copyYourself();
			insertPosition++;
		}
		App::ct->objCont->_editionPath->insertSimplePathPoint(it,insertPosition);
		App::ct->objCont->addItemToEditModeBuffer(insertPosition);
		App::ct->objCont->_editionPath->actualizePath();
		App::ct->objCont->setFullDialogRefreshFlag();
	}
}
void CPathEdition::_cutSelection(std::vector<int>* selection)
{
	_copySelection(selection);
	_deleteSelection(selection);
}
void CPathEdition::_deleteSelection(std::vector<int>* selection)
{
	if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
	{ // We delete the path points in the opposite order they are in the path
		int maxIndex=-1;
		for (int i=0;i<int(selection->size());i++)
		{
			if (selection->at(i)>maxIndex)
				maxIndex=selection->at(i);
		}
		std::vector<bool> toDelete(maxIndex+1,false);
		for (int i=0;i<int(selection->size());i++)
			toDelete[selection->at(i)]=true;
		App::ct->objCont->_editionPath->enableActualization(false);
		for (int i=int(toDelete.size())-1;i>=0;i--)
		{
			if (toDelete[i])
				App::ct->objCont->_editionPath->removeSimplePathPoint(i);
		}
		App::ct->objCont->_editionPath->enableActualization(true);
		App::ct->objCont->_editionPath->actualizePath();
		App::ct->objCont->deselectEditModeBuffer();
		App::ct->objCont->setFullDialogRefreshFlag();
	}
}

void CPathEdition::_keepXAxisAndAlignZAxis(std::vector<int>* selection)
{
	if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
	{
		CPath* path=App::ct->objCont->getPath(App::ct->objCont->getEditModeObjectID());
		if ((path!=NULL)&&(selection->size()>1) )
		{
			C7Vector ctm(path->getCumulativeTransformationPart1());
			CSimplePathPoint* last=App::ct->objCont->_editionPath->getSimplePathPoint((*selection)[selection->size()-1]);
			C3Vector zVect(last->getTransformation().getMatrix().M.axis[2]);
			C3X3Matrix posRot,negRot;
			posRot.buildXRotation(piValue/1800.0f);
			negRot.buildXRotation(-piValue/1800.0f);
			for (int i=0;i<int(selection->size()-1);i++)
			{
				CSimplePathPoint* it=App::ct->objCont->_editionPath->getSimplePathPoint((*selection)[i]);
				C4X4Matrix m=it->getTransformation().getMatrix();
				float d=(m.M.axis[2]-zVect).getLength();
				bool positiveRot=true;
				for (int j=0;j<4000;j++)
				{
					if (positiveRot)
						m.M=m.M*posRot;
					else
						m.M=m.M*negRot;
					float dp=(m.M.axis[2]-zVect).getLength();			
					if (dp>d)
						positiveRot=!positiveRot;
					d=dp;
				}
				it->setTransformation(m.getTransformation(),App::ct->objCont->_editionPath->getAttributes());
			}
			App::ct->objCont->_editionPath->actualizePath();
			App::ct->objCont->setLightDialogRefreshFlag();
		}
	}
}

void CPathEdition::_generatePath()
{
	if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
	{
		CPath* path=App::ct->objCont->getPath(App::ct->objCont->getEditModeObjectID());
		if (path!=NULL)
		{
			CPath* newPath=(CPath*)path->copyYourself();
			newPath->pathContainer->enableActualization(false);
			newPath->pathContainer->removeAllSimplePathPoints();
			int i=0;
			CBezierPathPoint* it=App::ct->objCont->_editionPath->getBezierPathPoint(i++);
			while (it!=NULL)
			{
				CSimplePathPoint* bp=new CSimplePathPoint();
				bp->setTransformation(it->getTransformation(),App::ct->objCont->_editionPath->getAttributes());
				newPath->pathContainer->addSimplePathPoint(bp);
				it=App::ct->objCont->_editionPath->getBezierPathPoint(i++);
			}
			newPath->pathContainer->enableActualization(true);
			newPath->pathContainer->actualizePath();
			App::ct->objCont->addObjectToScene(newPath,false);
			App::ct->objCont->objectsAddedDuringEditMode.push_back(newPath->getID());
			App::ct->objCont->setLightDialogRefreshFlag();
		}
	}
}

void CPathEdition::addMenu(VMenu* menu,C3DObject* viewableObject)
{
	int selSize=App::ct->objCont->editModeBuffer.size();
	int buffSize=App::ct->objCont->editBufferPathPointsCopy.size();
	VMenubar::appendMenuItem(menu,selSize>0,false,PATH_POINT_COPY_CMD,IDS_COPY_SELECTED_PATH_POINTS_MENU_ITEM);
	if (selSize>=1)
		VMenubar::appendMenuItem(menu,(buffSize>0)&&(selSize==1),false,PASTE_PATH_POINT_CMD,IDS_PASTE_PATH_POINTS_MENU_ITEM);
	else
		VMenubar::appendMenuItem(menu,buffSize>0,false,PASTE_PATH_POINT_CMD,IDS_PASTE_PATH_POINTS_AT_BEGINNING_MENU_ITEM);
	VMenubar::appendMenuItem(menu,selSize>0,false,DELETE_PATH_POINT_CMD,IDS_DELETE_SELECTED_PATH_POINTS_MENU_ITEM);
	VMenubar::appendMenuItem(menu,selSize>0,false,PATH_POINT_CUT_CMD,IDS_CUT_SELECTED_PATH_POINTS_MENU_ITEM);
	VMenubar::appendMenuSeparator(menu);
	if (selSize>=1)
		VMenubar::appendMenuItem(menu,selSize==1,false,INSERT_NEW_PATH_POINT_CMD,IDS_INSERT_NEW_PATH_POINT_MENU_ITEM);
	else
		VMenubar::appendMenuItem(menu,true,false,INSERT_NEW_PATH_POINT_CMD,IDS_INSERT_NEW_PATH_POINT_AT_BEGINNING_MENU_ITEM);
	VMenubar::appendMenuItem(menu,(viewableObject!=NULL)&&(viewableObject->getObjectType()==sim_object_camera_type),false,APPEND_NEW_PATH_POINT_FROM_CAMERA_CMD,IDS_APPEND_NEW_PATH_POINT_FROM_CAMERA_MENU_ITEM);
	VMenubar::appendMenuItem(menu,App::ct->objCont->_editionPath->getBezierPathPointCount()>1,false,MAKE_PATH_FROM_BEZIER_PATH_CMD,IDS_MAKE_PATH_FROM_BEZIER_PATH_MENU_ITEM);
	VMenubar::appendMenuItem(menu,App::ct->objCont->_editionPath->getBezierPathPointCount()>1,false,KEEP_ALL_X_AND_ALIGN_Z_FOR_PATH_POINTS_CMD,"TEST FUNCTION");
	VMenubar::appendMenuSeparator(menu);
	VMenubar::appendMenuItem(menu,true,false,SELECT_ALL_PATH_POINTS_CMD,IDSN_SELECT_ALL_MENU_ITEM);
	VMenubar::appendMenuSeparator(menu);
	VMenubar::appendMenuItem(menu,true,false,LEAVE_EDIT_MODE_CANCEL_CHANGES_CMD,IDS_LEAVE_EDIT_MODE_AND_CANCEL_CHANGES_MENU_ITEM);
	VMenubar::appendMenuItem(menu,true,false,LEAVE_EDIT_MODE_CMD,IDS_LEAVE_EDIT_MODE_AND_APPLY_CHANGES_MENU_ITEM);
}

bool CPathEdition::evaluateMenuSelection(int commandID,C3DObject* viewableObject)
{ // Return value is true if the command belonged to object edition menu and was executed
	if (commandID==SELECT_ALL_PATH_POINTS_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("SELECT_ALL_PATH_POINTS_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_SELECTING_ALL_PATH_POINTS);
			for (int i=0;i<int(App::ct->objCont->_editionPath->getSimplePathPointCount());i++)
				App::ct->objCont->addItemToEditModeBuffer(i);
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==PATH_POINT_COPY_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("PATH_POINT_COPY_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
				sel.push_back(App::ct->objCont->editModeBuffer[i]);
			App::uiThread->addStatusLine(IDSNS_COPYING_SELECTION);
			_copySelection(&sel);
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==PASTE_PATH_POINT_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("PASTE_PATH_POINT_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
				sel.push_back(App::ct->objCont->editModeBuffer[i]);
			if (sel.size()<2)
			{
				App::uiThread->addStatusLine(IDSNS_PASTING_BUFFER);
				if (sel.size()==0)
					_paste(-1);
				else
					_paste(sel[0]);
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
		}
		return(true);
	}
	if (commandID==INSERT_NEW_PATH_POINT_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("INSERT_NEW_PATH_POINT_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
				sel.push_back(App::ct->objCont->editModeBuffer[i]);
			if (sel.size()<2)
			{
				App::uiThread->addStatusLine(IDSNS_INSERTING_NEW_PATH_POINT);
				if (sel.size()==0)
					_insertNewPoint(-1);
				else
					_insertNewPoint(sel[0]);
				App::uiThread->addStatusLine(IDSNS_DONE);
			}
		}
		return(true);
	}
	if (commandID==APPEND_NEW_PATH_POINT_FROM_CAMERA_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("APPEND_NEW_PATH_POINT_FROM_CAMERA_CMD")
		{
			if ((viewableObject!=NULL)&&(viewableObject->getObjectType()==sim_object_camera_type))
			{
				if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
				{
					App::uiThread->addStatusLine(IDSNS_APPENDING_NEW_PATH_POINT_FROM_CAMERA_CONFIGURATION);
					App::ct->objCont->deselectEditModeBuffer();
					CSimplePathPoint* it=new CSimplePathPoint();
					C3DObject* path=App::ct->objCont->getObject(App::ct->objCont->getEditModeObjectID());
					C7Vector pathInv(path->getCumulativeTransformation().getInverse());
					it->setTransformation(pathInv*viewableObject->getCumulativeTransformationPart1(),App::ct->objCont->_editionPath->getAttributes());
					App::ct->objCont->_editionPath->insertSimplePathPoint(it,App::ct->objCont->_editionPath->getSimplePathPointCount());
					App::ct->objCont->addItemToEditModeBuffer(App::ct->objCont->_editionPath->getSimplePathPointCount()-1);
					App::ct->objCont->_editionPath->actualizePath();
					App::ct->objCont->setFullDialogRefreshFlag();
					App::uiThread->addStatusLine(IDSNS_DONE);
				}
			}
		}
		return(true);
	}
	if (commandID==DELETE_PATH_POINT_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("DELETE_PATH_POINT_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
				sel.push_back(App::ct->objCont->editModeBuffer[i]);
			App::uiThread->addStatusLine(IDSNS_DELETING_SELECTION);
			_deleteSelection(&sel);
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==PATH_POINT_CUT_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("PATH_POINT_CUT_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
				sel.push_back(App::ct->objCont->editModeBuffer[i]);
			App::uiThread->addStatusLine(IDSNS_CUTTING_SELECTION);
			_cutSelection(&sel);
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==KEEP_ALL_X_AND_ALIGN_Z_FOR_PATH_POINTS_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("KEEP_ALL_X_AND_ALIGN_Z_FOR_PATH_POINTS_CMD")
		{
			std::vector<int> sel;
			for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
				sel.push_back(App::ct->objCont->editModeBuffer[i]);
			if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
			{
				if (App::ct->objCont->_editionPath->getBezierPathPointCount()>=2)
				{
					App::uiThread->addStatusLine(IDSNS_KEEPING_CONTROL_POINT_X_AXIS_AND_ALIGNING_CONTROL_POINT_Z_AXIS_WITH_LAST_SELECTION);
					_keepXAxisAndAlignZAxis(&sel);
					App::uiThread->addStatusLine(IDSNS_DONE);
				}
			}
		}
		return(true);
	}
	if (commandID==MAKE_PATH_FROM_BEZIER_PATH_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("MAKE_PATH_FROM_BEZIER_PATH_CMD")
		{
			if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
			{
				bool proceed=true;
				if (App::ct->objCont->_editionPath->getBezierPathPointCount()>50)
					proceed=(VMessageBox::REPLY_YES==App::uiThread->messageBox_warning(App::mainWindow,strTranslate(IDSN_PATH),strTranslate(IDS_LARGE_QUANTITY_OF_PATH_POINTS_WARNING),VMessageBox::YES_NO));
				if (App::ct->objCont->_editionPath->getBezierPathPointCount()<2)
					proceed=false;
				if (proceed)
				{
					App::uiThread->addStatusLine(IDSNS_GENERATING_NEW_PATH);
					_generatePath();
					App::uiThread->addStatusLine(IDSNS_DONE);
				}
			}
		}
		return(true);
	}
	if (commandID==LEAVE_EDIT_MODE_CANCEL_CHANGES_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("LEAVE_EDIT_MODE_CANCEL_CHANGES_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_ENDING_EDIT_MODE_AND_CANCELLING_CHANGES);
			App::ct->objCont->endEditMode(true);
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	if (commandID==LEAVE_EDIT_MODE_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("LEAVE_EDIT_MODE_CMD")
		{
			App::uiThread->addStatusLine(IDSNS_ENDING_EDIT_MODE_AND_APPLYING_CHANGES);
			App::ct->objCont->endEditMode(false);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			App::uiThread->addStatusLine(IDSNS_DONE);
		}
		return(true);
	}
	return(false);
}
