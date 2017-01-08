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
#include "CustomMenuBarItemContainer.h"
#include "Global.h"

CCustomMenuBarItemContainer::CCustomMenuBarItemContainer()
{
	_nextSmallItemCommandID=ADDITIONAL_TOOL_MENU_ITEM_START_CMD;
	_menuHandle=NULL;
}

CCustomMenuBarItemContainer::~CCustomMenuBarItemContainer()
{
	for (unsigned int i=0;i<allItems.size();i++)
		delete allItems[i];
}

bool CCustomMenuBarItemContainer::setItemState(int commandID,bool checked,bool enabled,const char* newLabel)
{
	for (unsigned int i=0;i<allItems.size();i++)
	{
		if (allItems[i]->setItemState(commandID,checked,enabled,newLabel))
			return(true);
	}
	return(false);
}

bool CCustomMenuBarItemContainer::addMenuBarItem(const char* title,int subItemCount,std::vector<int>& commandIDs)
{
	commandIDs.clear();
	if (subItemCount+_nextSmallItemCommandID-1>ADDITIONAL_TOOL_MENU_ITEM_END_CMD)
		return(false); // Too many!!
	CCustomMenuBarItem* it=NULL;
	for (int i=0;i<int(allItems.size());i++)
	{
		if ( (allItems[i]->getMenuBarLabel()==title)&&(strlen(title)!=0) )
		{
			it=allItems[i];
			break;
		}
	}
	if (it==NULL)
	{ // That menu doesn't yet exist! We create it
		it=new CCustomMenuBarItem(title);
		allItems.push_back(it);
	}
	for (int i=0;i<subItemCount;i++)
	{
		commandIDs.push_back(_nextSmallItemCommandID);
		it->addItem(_nextSmallItemCommandID++);
	}
	return(true);
}

bool CCustomMenuBarItemContainer::addMenus(VMENU_HANDLE menuHandle,VMenu* myMenu)
{
	// From Qt this is only called if the custom menu was clicked (unlike in Windows)
	for (unsigned int i=0;i<allItems.size();i++)
		allItems[i]->addMenus(myMenu);
	return(true);
	return(false);
}

bool CCustomMenuBarItemContainer::evaluateMenuSelection(int commandID)
{ // Return value is true if the command belonged to object edition menu and was executed
	for (unsigned int i=0;i<allItems.size();i++)
	{
		if (allItems[i]->evaluateMenuSelection(commandID))
			return(true);
	}
	return(false);

}

