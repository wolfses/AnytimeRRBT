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
#include "CustomMenuBarItem.h"
#include "App.h"
#include "OGL.h"
#include "v_rep_internal.h"
#include "IloIlo.h"
#include "pluginContainer.h"

CCustomMenuBarItem::CCustomMenuBarItem(const char* menuBarText)
{
	_menuBarText=menuBarText;
}

CCustomMenuBarItem::~CCustomMenuBarItem()
{
}

void CCustomMenuBarItem::addItem(int cmdID)
{
	std::string tmp("");
	_labels.push_back(tmp);
	_states.push_back(0);
	_commandIDs.push_back(cmdID);
}

void CCustomMenuBarItem::addMenus(VMenu* myMenu)
{
	if (_menuBarText!="")
	{
		VMenu* prim=VMenubar::createPopupMenu();
		for (unsigned int i=0;i<_commandIDs.size();i++)
		{
			if (_labels[i]=="")
				VMenubar::appendMenuSeparator(prim);
			else
				VMenubar::appendMenuItem(prim,(_states[i]&2)!=0,(_states[i]&1)!=0,_commandIDs[i],_labels[i],(_states[i]&1)!=0);
		}
		VMenubar::appendMenuAndDetach(myMenu,prim,true,_menuBarText);
	}
	else
	{
		if (_commandIDs.size()!=0)
			VMenubar::appendMenuItem(myMenu,(_states[0]&2)!=0,(_states[0]&1)!=0,_commandIDs[0],_labels[0],(_states[0]&1)!=0);
	}
}

std::string CCustomMenuBarItem::getMenuBarLabel()
{
	return(_menuBarText);
}

int CCustomMenuBarItem::getItemCount()
{
	return(int(_commandIDs.size()));
}

void CCustomMenuBarItem::setCommandID(int index,int theCommand)
{
	_commandIDs[index]=theCommand;
}

bool CCustomMenuBarItem::setItemState(int commandID,bool checked,bool enabled,const char* newLabel)
{
	for (unsigned int i=0;i<_commandIDs.size();i++)
	{
		if (_commandIDs[i]==commandID)
		{
			if (newLabel!=NULL)
				_labels[i]=newLabel;
			_states[i]=0;
			if (checked)
				_states[i]|=1;
			if (enabled)
				_states[i]|=2;
			App::ct->objCont->setFullDialogRefreshFlag();
			return(true);
		}
	}
	return(false);
}

bool CCustomMenuBarItem::evaluateMenuSelection(int commandID)
{ // Return value is true if the command belonged to object edition menu and was executed
	for (unsigned int i=0;i<_commandIDs.size();i++)
	{
		if (_commandIDs[i]==commandID)
		{ // We have to post a message here!!
			IF_UI_EVENT_CAN_WRITE_DATA_CMD("CCustomMenuBarItem::evaluateMenuSelection")
			{
				int data[4]={commandID,_states[i],0,0};
				void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_menuitemselected,data,NULL,NULL);
				delete[] (char*)returnVal;
			}
			return(true);
		}
	}
	return(false);
}




