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

#include "VThread.h"
#include "VSimUiMutex.h"

class CSimAndUiThreadSync
{
public:
	CSimAndUiThreadSync(const char* functionName);
	virtual ~CSimAndUiThreadSync();

	bool uiThread_tryToLockForUiEventRead();
	bool uiThread_tryToLockForUiEventWrite();

	void simThread_lockForSimThreadWrite();
	void simThread_temporarilyAllowUiThreadToReadAndWrite();

	bool simOrUiThread_tryToLockForWrite_cApi();
	bool simOrUiThread_tryToLockForRead_cApi();

	int getObjectHandle();

	static bool simThread_forbidUiThreadToWrite(bool blocking);
	static void simThread_allowUiThreadToWrite();

	static void outputDebugMessage(const char* callLocation,const char* msg);
	static void outputNakedDebugMessage(const char* msg);

	static bool getShowActivityInConsole();
	static void setShowActivityInConsole(bool show);

	static bool getShowLockFailsActivityInConsole();
	static void setShowLockFailsActivityInConsole(bool show);

	static void setProcessAllMessages_tweak3(bool all);

private:

	static std::string _getLevelsString(const char* abr);

	int _lockFunctionResult; // -1=not yet tried to lock, 0=tried to lock but failed, 1=tried to lock and succeeded
	std::string _functionName;
	int	_lockType; // 0=UI READ, 1=UI WRITE, 2=SIM THREAD WRITE, 3=SIM THREAD ALLOWS OVERRIDING READ OR WRITE BAN, 4=UI THREAD GRANTS SPECIAL READ/WRITE RIGHTS
	int _handle;

	static VSimUiMutex _uiReadPermission; // shared between the SIM and UI thread. Locked by UI when UI reads.
	static VSimUiMutex _uiWritePermission; // shared between the SIM and UI thread. Locked by UI when UI writes

	static int _ui_readLevel;
	static int _ui_writeLevel;
	static int _sim_readLevel;
	static int _sim_writeLevel;

	static int _sim_readLevel_temp;
	static int _sim_writeLevel_temp;

	static volatile bool _ui_writeRequest;

	static bool _showActivityInConsole;
	static bool _showLockFailsInConsole;

	static int _nextHandleValue;

	static bool _processAllMessages_tweak3;
};
