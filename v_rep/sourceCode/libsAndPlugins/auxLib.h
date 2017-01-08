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

#pragma once

#ifdef WIN_VREP

#include <QLibrary>
#define LIBRARY QLibrary*
#define FARPROC_BLA void*

typedef void (__cdecl *pvrepWin_showConsole)(char show);
typedef DWORD_PTR (__cdecl *pvrepWin_launchThread)(AFX_THREADPROC_TT startAddress,int priority,DWORD_PTR affinityMask);
typedef DWORD_PTR (__cdecl *pvrepWin_setThreadAffinity)(DWORD_PTR affinity);
typedef DWORD (__cdecl *pvrepWin_getCurrentThreadId)(void);
typedef void (__cdecl *pvrepWin_switchThread)(void);
typedef void (__cdecl *pvrepWin_sleep)(int ms);

typedef int (__cdecl *pvrepWin_serialOpen)(char* portString,int baudrate);
typedef char (__cdecl *pvrepWin_serialClose)(int portHandle);
typedef int (__cdecl *pvrepWin_serialWrite)(int portHandle,const char* buffer,int size);
typedef int (__cdecl *pvrepWin_serialCheck)(int portHandle);
typedef int (__cdecl *pvrepWin_serialRead)(int portHandle,char* buffer,int maxSize);
typedef void (__cdecl *pvrepWin_cleanUp)(void);

class CAuxLib  
{
public:
	CAuxLib();
	virtual ~CAuxLib();

	static bool loadLibrary(const char* pathAndFilename);
	static void unloadLibrary();
	static bool getVrepAuxLibProcAddresses();
	static FARPROC_BLA _getProcAddress(const char* funcName);

	static LIBRARY lib;

	static pvrepWin_showConsole vrepWin_showConsole;
	static pvrepWin_launchThread vrepWin_launchThread;
	static pvrepWin_setThreadAffinity vrepWin_setThreadAffinity;
	static pvrepWin_getCurrentThreadId vrepWin_getCurrentThreadId;
	static pvrepWin_switchThread vrepWin_switchThread;
	static pvrepWin_sleep vrepWin_sleep;

	static pvrepWin_serialOpen vrepWin_serialOpen;
	static pvrepWin_serialClose vrepWin_serialClose;
	static pvrepWin_serialWrite vrepWin_serialWrite;
	static pvrepWin_serialCheck vrepWin_serialCheck;
	static pvrepWin_serialRead vrepWin_serialRead;
	static pvrepWin_cleanUp vrepWin_cleanUp;
};
#endif
