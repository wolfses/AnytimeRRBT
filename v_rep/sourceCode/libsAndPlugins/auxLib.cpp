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
#include "auxLib.h"

#ifdef WIN_VREP

LIBRARY CAuxLib::lib=NULL;

pvrepWin_showConsole CAuxLib::vrepWin_showConsole=0;
pvrepWin_launchThread CAuxLib::vrepWin_launchThread=0;
pvrepWin_setThreadAffinity CAuxLib::vrepWin_setThreadAffinity=0;
pvrepWin_getCurrentThreadId CAuxLib::vrepWin_getCurrentThreadId=0;
pvrepWin_switchThread CAuxLib::vrepWin_switchThread=0;
pvrepWin_sleep CAuxLib::vrepWin_sleep=0;

pvrepWin_serialOpen CAuxLib::vrepWin_serialOpen=0;
pvrepWin_serialClose CAuxLib::vrepWin_serialClose=0;
pvrepWin_serialWrite CAuxLib::vrepWin_serialWrite=0;
pvrepWin_serialCheck CAuxLib::vrepWin_serialCheck=0;
pvrepWin_serialRead CAuxLib::vrepWin_serialRead=0;
pvrepWin_cleanUp CAuxLib::vrepWin_cleanUp=0;

CAuxLib::CAuxLib()
{
}

CAuxLib::~CAuxLib()
{
}


bool CAuxLib::loadLibrary(const char* pathAndFilename)
{
	lib=new QLibrary(pathAndFilename);
	if (!lib->load())
	{
		delete lib;
		lib=NULL;
	}
	return (lib!=NULL);
}

void CAuxLib::unloadLibrary()
{
	if (lib!=0)
	{
		lib->unload();
		delete lib;
	}
	lib=NULL;

	vrepWin_showConsole=0;
	vrepWin_launchThread=0;
	vrepWin_setThreadAffinity=0;
	vrepWin_getCurrentThreadId=0;
	vrepWin_switchThread=0;
	vrepWin_sleep=0;
	vrepWin_serialOpen=0;
	vrepWin_serialClose=0;
	vrepWin_serialWrite=0;
	vrepWin_serialCheck=0;
	vrepWin_serialRead=0;
	vrepWin_cleanUp=0;
}

FARPROC_BLA CAuxLib::_getProcAddress(const char* funcName)
{
	return (void*)lib->resolve(funcName);
}

bool CAuxLib::getVrepAuxLibProcAddresses()
{
	vrepWin_showConsole=(pvrepWin_showConsole)(_getProcAddress("vrepWin_showConsole"));
	vrepWin_launchThread=(pvrepWin_launchThread)(_getProcAddress("vrepWin_launchThread"));
	vrepWin_setThreadAffinity=(pvrepWin_setThreadAffinity)(_getProcAddress("vrepWin_setThreadAffinity"));
	vrepWin_getCurrentThreadId=(pvrepWin_getCurrentThreadId)(_getProcAddress("vrepWin_getCurrentThreadId"));
	vrepWin_switchThread=(pvrepWin_switchThread)(_getProcAddress("vrepWin_switchThread"));
	vrepWin_sleep=(pvrepWin_sleep)(_getProcAddress("vrepWin_sleep"));

	vrepWin_serialOpen=(pvrepWin_serialOpen)(_getProcAddress("vrepWin_serialOpen"));
	vrepWin_serialClose=(pvrepWin_serialClose)(_getProcAddress("vrepWin_serialClose"));
	vrepWin_serialWrite=(pvrepWin_serialWrite)(_getProcAddress("vrepWin_serialWrite"));
	vrepWin_serialCheck=(pvrepWin_serialCheck)(_getProcAddress("vrepWin_serialCheck"));
	vrepWin_serialRead=(pvrepWin_serialRead)(_getProcAddress("vrepWin_serialRead"));
	vrepWin_cleanUp=(pvrepWin_cleanUp)(_getProcAddress("vrepWin_cleanUp"));

	if (vrepWin_showConsole==0) return false;
	if (vrepWin_launchThread==0) return false;
	if (vrepWin_setThreadAffinity==0) return false;
	if (vrepWin_getCurrentThreadId==0) return false;
	if (vrepWin_switchThread==0) return false;
	if (vrepWin_sleep==0) return false;

	if (vrepWin_serialOpen==0) return false;
	if (vrepWin_serialClose==0) return false;
	if (vrepWin_serialWrite==0) return false;
	if (vrepWin_serialCheck==0) return false;
	if (vrepWin_serialRead==0) return false;
	if (vrepWin_cleanUp==0) return false;

	return true;
}

#endif // WIN_VREP
