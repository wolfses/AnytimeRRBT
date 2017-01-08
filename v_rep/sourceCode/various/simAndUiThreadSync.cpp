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
#include "simAndUiThreadSync.h"
#include "App.h"
#include "VDateTime.h"
#include "debugLogFile.h"
#include "Tt.h"

VSimUiMutex CSimAndUiThreadSync::_uiReadPermission;
VSimUiMutex CSimAndUiThreadSync::_uiWritePermission;

int CSimAndUiThreadSync::_ui_readLevel=0;
int CSimAndUiThreadSync::_ui_writeLevel=0;
int CSimAndUiThreadSync::_sim_readLevel=0;
int CSimAndUiThreadSync::_sim_writeLevel=0;
int CSimAndUiThreadSync::_sim_readLevel_temp=0;
int CSimAndUiThreadSync::_sim_writeLevel_temp=0;

volatile bool CSimAndUiThreadSync::_ui_writeRequest=false;

bool CSimAndUiThreadSync::_showActivityInConsole=false;
bool CSimAndUiThreadSync::_showLockFailsInConsole=false;

int CSimAndUiThreadSync::_nextHandleValue=0;

bool CSimAndUiThreadSync::_processAllMessages_tweak3=false;

void CSimAndUiThreadSync::setProcessAllMessages_tweak3(bool all)
{
	_processAllMessages_tweak3=all;
}

CSimAndUiThreadSync::CSimAndUiThreadSync(const char* functionName)
{
	_lockFunctionResult=-1;
	_lockType=-1;
	_functionName=functionName;
	_handle=_nextHandleValue++;
}

CSimAndUiThreadSync::~CSimAndUiThreadSync()
{
	if (_lockType==0)
	{
		if (_lockFunctionResult>0)
		{
			_ui_readLevel--;
			_uiReadPermission.unlock(); // release the read permission for the UI thread
			if (_showActivityInConsole)
				CDebugLogFile::addDebugText(false,_getLevelsString("$$R ").c_str()," (GUI) 'unlock for read' (",_functionName.c_str(),")\n");
		}
	}


	if (_lockType==1)
	{
		if (_lockFunctionResult>0)
		{
			_ui_writeLevel--;

			if (_ui_writeLevel==0)
			{ // following to avoid visual artifacts:
				for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
				{
					C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
					it->bufferMainDisplayStateVariables();
				}
			}

			_uiWritePermission.unlock(); // release the write permission for the UI thread

			if (_showActivityInConsole)
			{
				CDebugLogFile::addDebugText(false,_getLevelsString("$$W ").c_str()," (GUI) 'unlock for write' (",_functionName.c_str(),")\n");
				if (_ui_writeLevel==0)
				{
					outputNakedDebugMessage("$$W -------------------------------------------------------\n");
					outputNakedDebugMessage("$$W -------------------------------------------------------\n");
					outputNakedDebugMessage("$$W\n");
				}
			}
		}
	}

	if (_lockType==2)
	{
		_sim_writeLevel--;
		_uiReadPermission.unlock(); // release the write permission for the SIM thread (i.e. allow the UI thread to read again)
		if (_showActivityInConsole)
		{
			CDebugLogFile::addDebugText(false,_getLevelsString("$$S ").c_str()," (SIM) 'unlock for write' (",_functionName.c_str(),")\n");
			if (_sim_writeLevel==0)
			{
				outputNakedDebugMessage("$$S +++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
				outputNakedDebugMessage("$$S +++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
				outputNakedDebugMessage("$$S\n");
			}
		}
	}

	if (_lockType==3)
	{
		for (int i=0;i<_sim_readLevel_temp;i++)
			_uiWritePermission.lock(VSimUiMutex::nonUi);
		_sim_readLevel=_sim_readLevel_temp;
		_sim_readLevel_temp=0;


		for (int i=0;i<_sim_writeLevel_temp;i++)
			_uiReadPermission.lock(VSimUiMutex::nonUi);
		_sim_writeLevel=_sim_writeLevel_temp;
		_sim_writeLevel_temp=0;

		if (_showActivityInConsole)
		{
			CDebugLogFile::addDebugText(false,_getLevelsString("$$T ").c_str()," (SIM) 'restore previous locks' (",_functionName.c_str(),")\n");
			outputNakedDebugMessage("$$T #######################################################\n");
			outputNakedDebugMessage("$$T #######################################################\n");
			outputNakedDebugMessage("$$T\n");
		}
	}
}

bool CSimAndUiThreadSync::uiThread_tryToLockForUiEventRead()
{ // called by the UI thread only!
	_lockType=0; // UI READ

	if (!VThread::isCurrentThreadTheUiThread())
	{ // we are NOT in the UI thread. This is a bug!
		CDebugLogFile::addDebugText(true,"$$$ERROR in uiThread_tryToLockForUiEventRead (",_functionName.c_str(),"): not called from the UI thread.\n");
		VREP_BEEP;
	}

	if (_lockFunctionResult!=-1)
		return(false); // this condition is important, since this function will be called twice for the same object, and the second time it should return false!

	// We will try to take possession of the _uiReadPermission lock.
	// This will only succeed if the SIM thread is not writing.

	_lockFunctionResult=0; // means lock failed

	if (_showActivityInConsole)
		CDebugLogFile::addDebugText(false,_getLevelsString("$$R ").c_str()," (GUI) 'try to lock for read' (",_functionName.c_str(),")\n");

	int startTime=VDateTime::getTimeInMs();
	while (VDateTime::getTimeDiffInMs(startTime)<5)
	{
		if (_uiReadPermission.tryLock(VSimUiMutex::ui,1))
		{ // the lock succeeded.
			_lockFunctionResult=1;
			_ui_readLevel++;
			break;
		}
		// Following instruction can be important (but not critical if not present, because the lock will simply fail):
		// if that instruction is not present, then the SIM thread might be waiting for the UI
		// thread to do something (e.g. handle a vision sensor that needs to run in the UI thread).
		// While the SIM thread waits, the lock here will never succeed, unless we process the other
		// UI events here:

		// If we do not process all events, then the QFileDialog sometimes fails displaying and can
		// lead to application hang (on Linux, not on Windows. Mac?). We keep 2 tweaks here just in case!
		if (App::userSettings->applicationTweak3==1)
		{
			if (_processAllMessages_tweak3)
				App::qtApp->processEvents();
			else
				App::qtApp->processEvents(QEventLoop::ExcludeUserInputEvents|QEventLoop::ExcludeSocketNotifiers);
		}
		else
		{
			if (App::userSettings->applicationTweak2==1)
				App::qtApp->processEvents(QEventLoop::ExcludeUserInputEvents|QEventLoop::ExcludeSocketNotifiers);
			else
				App::qtApp->processEvents(); // default
		}
	}

	if (_lockFunctionResult==1)
	{ // the SIM thread doesn't write data. This means that the UI thread can read data.
		if (_showActivityInConsole)
			CDebugLogFile::addDebugText(false,_getLevelsString("$$R ").c_str()," (GUI) 'locked for read' (",_functionName.c_str(),")\n");
	}
	else
	{ // the SIM thread is writing data. This means that the UI thread CANNOT read data.
		if (_showActivityInConsole||_showLockFailsInConsole)
		{
			outputNakedDebugMessage("$$R >>>>>>>>>>>>>>>>>>>>>>>\n");
			CDebugLogFile::addDebugText(false,_getLevelsString("$$R ").c_str()," (GUI) 'could not lock for read' (",_functionName.c_str(),")\n");
			outputNakedDebugMessage("$$R >>>>>>>>>>>>>>>>>>>>>>>\n");
		}
	}

	return(_lockFunctionResult>0);
}

bool CSimAndUiThreadSync::uiThread_tryToLockForUiEventWrite()
{ // called by the UI thread only!
	_lockType=1; // UI WRITE

	if (!VThread::isCurrentThreadTheUiThread())
	{ // we are NOT in the UI thread. This is a bug!
		CDebugLogFile::addDebugText(true,"$$$ERROR in uiThread_tryToLockForUiEventWrite (",_functionName.c_str(),"): not called from the UI thread.\n");
		VREP_BEEP;
	}

	if (_lockFunctionResult!=-1)
		return(false); // this condition is important, since this function will be called twice for the same object, and the second time it should return false!

	// We will try to take possession of the _uiWritePermission lock.
	// This will only succeed if the SIM thread has released that lock (happens only in specific code sections).

	_lockFunctionResult=0; // means lock failed

	if (_showActivityInConsole)
	{
		CDebugLogFile::addDebugText(false,_getLevelsString("$$W ").c_str()," (GUI) 'try to lock for write' (",_functionName.c_str(),")\n");
	}

	_ui_writeRequest=true; // so that the SIM thread waits in specific code sections (if not already there)

	int startTime=VDateTime::getTimeInMs();
	while (VDateTime::getTimeDiffInMs(startTime)<800)
	{
		if (_uiWritePermission.tryLock(VSimUiMutex::ui,10))
		{ // the lock succeeded.
			_lockFunctionResult=1;
			_ui_writeLevel++;
			break;
		}
		// Following instruction can be important (but not critical if not present, because the lock will simply fail):
		// if that instruction is not present, then the SIM thread might be waiting for the UI
		// thread to do something (e.g. handle a vision sensor that needs to run in the UI thread).
		// While the SIM thread waits, the lock here will never succeed, unless we process the other
		// UI events here:

		// If we do not process all events, then the QFileDialog sometimes fails displaying and can
		// lead to application hang (on Linux, not on Windows. Mac?). We keep 2 tweaks here just in case!
		if (App::userSettings->applicationTweak3==1)
		{
			if (_processAllMessages_tweak3)
				App::qtApp->processEvents();
			else
				App::qtApp->processEvents(QEventLoop::ExcludeUserInputEvents|QEventLoop::ExcludeSocketNotifiers);
		}
		else
		{
			if (App::userSettings->applicationTweak2==1)
				App::qtApp->processEvents(QEventLoop::ExcludeUserInputEvents|QEventLoop::ExcludeSocketNotifiers);
			else
				App::qtApp->processEvents(); // default
		}
	}

	_ui_writeRequest=false; // reset the write request

	if (_showActivityInConsole)
	{
		if (_lockFunctionResult>0)
		{
			if (_ui_writeLevel==1)
			{
				outputNakedDebugMessage("$$W\n");
				outputNakedDebugMessage("$$W -------------------------------------------------------\n");
				outputNakedDebugMessage("$$W -------------------------------------------------------\n");
			}
			CDebugLogFile::addDebugText(false,_getLevelsString("$$W ").c_str()," (GUI) 'locked for write' (",_functionName.c_str(),")\n");
		}
	}
	if (_showActivityInConsole||_showLockFailsInConsole)
	{
		if (_lockFunctionResult<=0)
		{
			outputNakedDebugMessage("$$W >>>>>>>>>>>>>>>>>>>>>>>\n");
			CDebugLogFile::addDebugText(false,_getLevelsString("$$W ").c_str()," (GUI) 'could not lock for write' (",_functionName.c_str(),")\n");
			outputNakedDebugMessage("$$W >>>>>>>>>>>>>>>>>>>>>>>\n");
		}
	}

	return(_ui_writeLevel>0);
}

void CSimAndUiThreadSync::simThread_lockForSimThreadWrite()
{ // called by the SIM thread only!
	_lockType=2; // SIM THREAD WRITE

	if (VThread::isCurrentThreadTheUiThread())
	{ // we are NOT in the UI thread. This is a bug!
		CDebugLogFile::addDebugText(true,"$$SERROR in simThread_lockForSimThreadWrite (",_functionName.c_str(),"): called from a the UI thread.\n");
		VREP_BEEP;
	}

	if (_lockFunctionResult!=-1)
		return; // should not happen!

	// We will try to take possession of the _uiReadPermission lock.
	// This will succeed once the UI thread is not reading anymore.

	_lockFunctionResult=1; // lock in this function will always succeed (blocking)

	if (_showActivityInConsole)
		CDebugLogFile::addDebugText(false,_getLevelsString("$$S ").c_str()," (SIM) 'locking for write' (",_functionName.c_str(),")\n");

	_uiReadPermission.lock(VSimUiMutex::nonUi);
	_sim_writeLevel++;

	if (_showActivityInConsole)
	{
		if (_sim_writeLevel==1)
		{
			outputNakedDebugMessage("$$S\n");
			outputNakedDebugMessage("$$S +++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
			outputNakedDebugMessage("$$S +++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
		}
		CDebugLogFile::addDebugText(false,_getLevelsString("$$S ").c_str()," (SIM) 'locked for write' (",_functionName.c_str(),")\n");
	}
}

bool CSimAndUiThreadSync::simOrUiThread_tryToLockForWrite_cApi()
{ // called by the SIM or UI thread, from the C API!
	if (VThread::isCurrentThreadTheUiThread())
	{
		return(uiThread_tryToLockForUiEventWrite());
	}
	else
	{
		if (_lockFunctionResult!=-1)
			return(false); // this condition is important, since this function will be called twice for the same object, and the second time it should return false!
		simThread_lockForSimThreadWrite();
		return(true);
	}
}

bool CSimAndUiThreadSync::simOrUiThread_tryToLockForRead_cApi()
{ // called by the SIM or UI thread, from the C API!
	if (VThread::isCurrentThreadTheUiThread())
	{
		return(uiThread_tryToLockForUiEventRead());
	}
	else
	{
		if (_lockFunctionResult!=-1)
			return(false); // this condition is important, since this function will be called twice for the same object, and the second time it should return false!
		_lockType=-1;
		_lockFunctionResult=1;
		return(true);
	}
}

/*
bool CSimAndUiThreadSync::simThread_shouldWaitToAllowUiThreadToWrite()
{ // this function is static!
	return(_ui_writeRequest);
}
*/

bool CSimAndUiThreadSync::simThread_forbidUiThreadToWrite(bool blocking)
{ // this function is static!
	if (blocking)
	{
//		App::setUiMessagesToProcess(1); // allow processing of all types of messages, except for UI entries
//		while (!App::getUiMessagesToProcessAreApplied())
//			VThread::sleep(1);
		_uiWritePermission.lock(VSimUiMutex::nonUi);
		_sim_readLevel++;
		if (_sim_readLevel!=1)
		{ // should not happen!
			CDebugLogFile::addDebugText(true,"$$SERROR in simThread_forbidUiThreadToWrite: multiple SIM read locks.\n");
			VREP_BEEP;
		}

		return(true);
	}
	else
	{
		if (_ui_writeRequest)
			return(false);

//		App::setUiMessagesToProcess(1); // allow processing of all types of messages, except for UI entries
//		while (!App::getUiMessagesToProcessAreApplied())
//			VThread::sleep(1);

		bool retVal=_uiWritePermission.tryLock(VSimUiMutex::nonUi);
		if (retVal)
		{
			_sim_readLevel++;
			if (_sim_readLevel!=1)
			{ // should not happen!
				CDebugLogFile::addDebugText(true,"$$SERROR in simThread_forbidUiThreadToWrite: multiple SIM read locks.\n");
				VREP_BEEP;
			}
		}
		return(retVal);
	}
}

void CSimAndUiThreadSync::simThread_allowUiThreadToWrite()
{ // this function is static!
	_sim_readLevel--;
	if (_sim_readLevel!=0)
	{ // should not happen!
		CDebugLogFile::addDebugText(true,"$$SERROR in simThread_allowUiThreadToWrite: multiple SIM read locks.\n");
		VREP_BEEP;
	}
	_uiWritePermission.unlock();

//	App::setUiMessagesToProcess(2); // allow processing of all types of messages
}


void CSimAndUiThreadSync::simThread_temporarilyAllowUiThreadToReadAndWrite()
{	_lockType=3; // SIM THREAD TEMPORARILY ALLOWS THE UI THREAD TO READ AND WRITE

	if (VThread::isCurrentThreadTheUiThread())
	{ // we are NOT in the UI thread. This is a bug!
		CDebugLogFile::addDebugText(true,"$$T ERROR in simThread_temporarilyAllowUiThreadToReadAndWrite (",_functionName.c_str(),"): called from a the UI thread.\n");
		VREP_BEEP;
	}

	// We will release possession of the _uiReadPermission and _uiWritePermission locks.
	// This will succeed once the UI thread is not reading anymore.

	_lockFunctionResult=1; // lock in this function will always succeed (blocking)

	_sim_writeLevel_temp=_sim_writeLevel;
	_sim_writeLevel=0;
	for (int i=0;i<_sim_writeLevel_temp;i++)
		_uiReadPermission.unlock();

	_sim_readLevel_temp=_sim_readLevel;
	_sim_readLevel=0;
	for (int i=0;i<_sim_readLevel_temp;i++)
		_uiWritePermission.unlock();

	if (_showActivityInConsole)
	{
		outputNakedDebugMessage("$$T\n");
		outputNakedDebugMessage("$$T #######################################################\n");
		outputNakedDebugMessage("$$T #######################################################\n");
		CDebugLogFile::addDebugText(false,_getLevelsString("$$T ").c_str()," (SIM) 'temporarily release locks' (",_functionName.c_str(),")\n");
	}
}

std::string CSimAndUiThreadSync::_getLevelsString(const char* abr)
{
	std::string retString(abr);
	retString+=tt::FNb(0,_ui_readLevel);
	retString+=",";
	retString+=tt::FNb(0,_ui_writeLevel);
	retString+=",";
	retString+=tt::FNb(0,_sim_readLevel);
	retString+=",";
	retString+=tt::FNb(0,_sim_writeLevel);
	return(retString);
}

void CSimAndUiThreadSync::outputDebugMessage(const char* callLocation,const char* msg)
{ // function is static!
	if (_showActivityInConsole)
		CDebugLogFile::addDebugText(false,_getLevelsString("$$$ ").c_str()," 'aux debug msg' (",callLocation,"): ",msg,"\n");
}

void CSimAndUiThreadSync::outputNakedDebugMessage(const char* msg)
{ // function is static!
	if (_showActivityInConsole)
		CDebugLogFile::addDebugText(false,msg);
}

bool CSimAndUiThreadSync::getShowActivityInConsole()
{
	return(_showActivityInConsole);
}

void CSimAndUiThreadSync::setShowActivityInConsole(bool show)
{
	_showActivityInConsole=show;
}

bool CSimAndUiThreadSync::getShowLockFailsActivityInConsole()
{
	return(_showLockFailsInConsole);
}

void CSimAndUiThreadSync::setShowLockFailsActivityInConsole(bool show)
{
	_showLockFailsInConsole=show;
}

int CSimAndUiThreadSync::getObjectHandle()
{
	return(_handle);
}
