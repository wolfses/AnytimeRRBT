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
#include "VMenubar.h"
#include "App.h"

QMainWindow* VMenubar::_relatedWindow=NULL;
int VMenubar::memorizedCommand=-1;
bool VMenubar::doNotExecuteCommandButMemorizeIt=false;

VMenubar::VMenubar(QMainWindow* relatedWindow)
{
	_relatedWindow=relatedWindow;
}

VMenubar::~VMenubar()
{
	_relatedWindow->menuBar()->clear();
}

VMenubarType* VMenubar::createMenu()
{
	VMenubarType* menu=_relatedWindow->menuBar();
	#ifdef MAC_VREP
		menu->setNativeMenuBar(false); // otherwise we have plenty of crashes!!! The result with this instruction is that the menu bar is not in the Mac top bar, but in the main window as in Windows!
	#endif
	return(menu);
}

VMenu* VMenubar::createPopupMenu()
{
	VMenu* menu=new VMenu();
	return(menu);
}

VMENU_HANDLE VMenubar::appendMenuAndDetach(VMenu* parentMenu,VMenu* childMenu,bool enabled,const std::string& label)
{
	childMenu->setTitle(label.c_str());
	childMenu->setEnabled(enabled);
	parentMenu->addMenu(childMenu);
	return(childMenu);
}

VMENU_HANDLE VMenubar::appendMenuAndDetach(VMenubarType* parentMenu,VMenu* childMenu,bool enabled,const std::string& label)
{
	childMenu->setTitle(label.c_str());
	childMenu->setEnabled(enabled);
	parentMenu->addMenu(childMenu);
	return(childMenu);
}

void VMenubar::appendMenuItem(VMenu* menu,bool enabled,bool checkMark,int commandID,const std::string& label,bool showCheckmarkSpot)
{
	QAction* action=menu->addAction(label.c_str());
	action->setEnabled(enabled);
	action->setCheckable(showCheckmarkSpot);
	action->setChecked(checkMark);
	_relatedWindow->connect(action,SIGNAL(triggered()),App::mainWindow->popupSignalMapper,SLOT(map()));
	App::mainWindow->popupSignalMapper->setMapping(action,commandID);
}

void VMenubar::appendMenuSeparator(VMenu* menu)
{
	menu->addSeparator();
}

void VMenubar::setMenuAndDetach(VMenubarType* menu)
{
}

int VMenubar::trackPopupMenu(VMenu* menu,int posX,int posY,QWidget* relatedWindow)
{
	doNotExecuteCommandButMemorizeIt=true;
	menu->exec(QCursor::pos());
	if (!doNotExecuteCommandButMemorizeIt)
		return(memorizedCommand);
	doNotExecuteCommandButMemorizeIt=false;
	return(-1);
}

