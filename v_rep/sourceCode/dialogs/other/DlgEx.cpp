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
#include "DlgEx.h"
#include "IloIlo.h"
#include "App.h"

int CDlgEx::doTransparencyCounter=0;

CDlgEx::CDlgEx(QWidget* pParent) : VDialog(pParent)
{
	_wasTransparent=false;
	_markedForDestruction=false;
	doTransparencyCounter++;
	_transparencyCounter=doTransparencyCounter;
	initializationEvent();
}
CDlgEx::CDlgEx(QWidget* pParent,Qt::WindowFlags specialFlags) : VDialog(pParent,specialFlags)
{
	_wasTransparent=false;
	_markedForDestruction=false;
	doTransparencyCounter++;
	_transparencyCounter=doTransparencyCounter;
	initializationEvent();
}

bool CDlgEx::event(QEvent* event)
{
	if (App::userSettings->allowTransparentDialogs)
	{
		if (_transparencyCounter!=doTransparencyCounter)
		{
			setWindowOpacity(App::userSettings->dialogTransparencyFactor);
			_transparencyCounter=doTransparencyCounter;
			_wasTransparent=true;
		}
		else
		{
			if (event->type()==QEvent::WindowActivate)//FocusIn)
			{
				setWindowOpacity(1.0f);
				_wasTransparent=false;
			}
			if (event->type()==QEvent::WindowDeactivate)//FocusOut)
			{
				setWindowOpacity(App::userSettings->dialogTransparencyFactor);
				_wasTransparent=true;
			}
		}
	}
	else
	{
		if (_wasTransparent)
			setWindowOpacity(1.0f);
		_wasTransparent=false;
	}
	return(QDialog::event(event));
}

CDlgEx::~CDlgEx()
{
}

void CDlgEx::refresh()
{
}

bool CDlgEx::needsDestruction()
{
	return(_markedForDestruction);
}

bool CDlgEx::doesInstanceSwitchRequireDestruction()
{
	return(false); // default behaviour
}

void CDlgEx::setRefreshDegree(int d)
{
	if (d<refreshDegree) 
		return;
	refreshDegree=d;
}

void CDlgEx::cancelEvent()
{ // We just hide the dialog and destroy it at next rendering pass
#ifndef LIN_VREP
	showDialog(false); // on Linux it seems that once a window is hidden, its position becomes (0,0)!!! So we don't hide it on Linux, we just destroy it later!
#endif
	_markedForDestruction=true;
	App::ct->objCont->toolbarRefreshFlag=true;
}

void CDlgEx::okEvent()
{
}

void CDlgEx::initializationEvent()
{
	defaultDialogInitializationRoutine();
	refresh();
}
