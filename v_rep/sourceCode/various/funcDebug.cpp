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

#include "funcDebug.h"
#include "VThread.h"
#include "debugLogFile.h"

int CFuncDebug::_debugMask=0;

CFuncDebug::CFuncDebug(const char* functionName,int debugMaskToShow)
{
	_functionName=functionName;
	_debugMaskToShow=debugMaskToShow;
	if ((_debugMask&_debugMaskToShow)!=0)
	{
		std::string arrow("--> ");
		if (VThread::isUiThreadIdSet())
		{
			if (VThread::isCurrentThreadTheUiThread())
				arrow+="(GUI ";
			else
				arrow+="(SIM ";
		}
		else
			arrow+="(GUI ";


		if (debugMaskToShow==1)
			arrow+="FUNC) ";
		if (debugMaskToShow==2)
			arrow+="C API) ";
		if (debugMaskToShow==4)
			arrow+="LUA API) ";

		CDebugLogFile::addDebugText(false,arrow.c_str(),_functionName.c_str(),"\n");
	}
}

CFuncDebug::~CFuncDebug()
{
	if ((_debugMask&_debugMaskToShow)!=0)
	{
		std::string arrow("<-- ");
		if (VThread::isUiThreadIdSet())
		{
			if (VThread::isCurrentThreadTheUiThread())
				arrow+="(GUI ";
			else
				arrow+="(SIM ";
		}
		else
			arrow+="(GUI ";


		if (_debugMaskToShow==1)
			arrow+="FUNC) ";
		if (_debugMaskToShow==2)
			arrow+="C API) ";
		if (_debugMaskToShow==4)
			arrow+="LUA API) ";

		CDebugLogFile::addDebugText(false,arrow.c_str(),_functionName.c_str(),"\n");
	}
}

void CFuncDebug::print(const char* txt,int debugMaskToShow)
{ // static function
	if ((_debugMask&debugMaskToShow)!=0)
	{
		if (VThread::isUiThreadIdSet())
		{
			if (VThread::isCurrentThreadTheUiThread())
				CDebugLogFile::addDebugText(false,"==> (GUI FUNC) ",txt,"\n");
			else
				CDebugLogFile::addDebugText(false,"==> (SIM FUNC) ",txt,"\n");
		}
		else
			CDebugLogFile::addDebugText(false,"==> (GUI FUNC) ",txt,"\n");
	}
}


void CFuncDebug::setDebugMask(int m)
{
	_debugMask=m;
}

int CFuncDebug::getDebugMask()
{
	return(_debugMask);
}
