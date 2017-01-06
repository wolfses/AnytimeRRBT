// This file is part of the REMOTE API
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The REMOTE API is licensed under the terms of GNU GPL:
// 
// -------------------------------------------------------------------
// The REMOTE API is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// THE REMOTE API IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with the REMOTE API.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#pragma once

#include <vector>
#include "inConnection.h"
#include "simxContainer.h"
#include "porting.h"

#if defined (__linux) || defined (__APPLE__)
	#include <pthread.h>
#endif /* __linux || __APPLE__ */

class CSimxSocket
{
public:
	CSimxSocket(int portNb,bool continuousService,bool simulationOnly,bool debug,int maxPacketSize,bool waitForTriggerFunctionAuthorized);
	virtual ~CSimxSocket();

	void start();

	void thereWasARequestToCallTheMainScript();
	void instancePass();

	void getInfo(int info[5]);
	int getClientVersion();
	int getStatus();
	int getPortNb();
	bool getActiveOnlyDuringSimulation();
	bool getContinuousService();
	bool getDebug();
	int getMaxPacketSize();


	void setWaitForTrigger(bool w);
	bool getWaitForTrigger();
	void setWaitForTriggerEnabled(bool e);
	bool getWaitForTriggerEnabled();
	bool getWaitForTriggerAuthorized();

	void addErrorString(const char* err);
	char* getAndClearFullErrorStringBuffer(int& size);

	static bool useAlternateSocketRoutines;

protected:
	void* _run();
	void _lock();
	void _unlock();

	void _stop();
	void _executeCommands();

	volatile bool _commThreadLaunched;
	volatile bool _commThreadEnded;
	volatile bool clientIsConnected;
	bool otherSideIsBigEndian;
	int _portNb;
	bool _simulationOnly;
	bool _continuousService;
	bool _debug;
	int _maxPacketSize;
	int _auxConsoleHandle;
	bool _waitForTrigger;
	bool _waitForTriggerFunctionEnabled;
	bool _waitForTriggerFunctionAuthorized;
	int _stopSimulationRequestCounter;

	int _lastReceivedMessage_time;
	int _lastReceivedMessage_clientVersion;
	int _lastSentMessage_time;
	int _successiveReception_time;
	int _lastReceivedMessage_cmdCnt;
	int _lastSentMessage_cmdCnt;

	std::vector<std::string> _textToPrintToConsole;
	std::vector<std::string> _last50Errors;
	
	CSimxContainer* _receivedCommands;
	CSimxContainer* _dataToSend;

	CInConnection* connection;

	int _lockLevel;
#ifdef _WIN32
	static DWORD WINAPI _staticThreadProc(LPVOID arg);
	#define MUTEX_HANDLE HANDLE
	#define MUTEX_HANDLE_X MUTEX_HANDLE
	#define THREAD_ID DWORD
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
	static void* _staticThreadProc(void *arg);
	#define MUTEX_HANDLE pthread_mutex_t
	#define MUTEX_HANDLE_X MUTEX_HANDLE*
	#define THREAD_ID pthread_t
	pthread_t _theThread;
#endif /* __linux || __APPLE__ */

	void _simpleLock(MUTEX_HANDLE_X mutex);
	void _simpleUnlock(MUTEX_HANDLE_X mutex);

	MUTEX_HANDLE _mutex;
	MUTEX_HANDLE _mutexAux;
	THREAD_ID _lockThreadId;
};
