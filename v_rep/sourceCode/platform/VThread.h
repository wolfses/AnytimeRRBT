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

// This file requires some serious refactoring!

#pragma once

#include "vrepPrecompiledHeader.h"
#include "VMutex.h"

#ifdef WIN_VREP
	#include "auxLib.h"
#endif
#include <QTime>
#include <QThread>
#ifdef WIN_VREP
	#define VTHREAD_RETURN_TYPE quint32
#else
	#define VTHREAD_RETURN_TYPE void*
#endif
#define VTHREAD_START_ADDRESS AFX_THREADPROC_TT
#define VTHREAD_ARGUMENT_TYPE void*
typedef unsigned int( __cdecl* SIMPLE_VTHREAD_START_ADDRESS )( void* );
#define SIMPLE_VTHREAD_RETURN_TYPE unsigned int
#define SIMPLE_VTHREAD_ARGUMENT_TYPE void*

// FULLY STATIC CLASS

class VThread  
{
public:
	static void launchThread(VTHREAD_START_ADDRESS startAddress,WORD priority,bool followMainThreadAffinity);
	static void launchSimpleThread(SIMPLE_VTHREAD_START_ADDRESS startAddress,WORD priority);
	static void setProcessorCoreAffinity(int mode); // negative=multicore, 0=any single core, otherwise affinity mask (bit1=core1, bit2=core2, etc.)
	static int getThreadId_apiQueried();

	static inline bool isMainThreadIdSet()
	{
		return(_mainThreadIdSet);
	}

	static inline void setMainThreadId()
	{
		_mainThreadId=VThread::getCurrentThreadId();
		_mainThread=QThread::currentThread();
		_mainThreadIdSet=true;
	}

	static inline QThread* getMainThread()
	{
		return(_mainThread);
	}

	static inline bool isMainThread()
	{
		return(areThreadIDsSame(_mainThreadId,getCurrentThreadId()));
	}

	static inline bool isCurrentThreadTheMainThread()
	{
		return(areThreadIDsSame(_mainThreadId,VThread::getCurrentThreadId()));
	}

	static inline bool isUiThreadIdSet()
	{
		return(_uiThreadIdSet);
	}

	static inline void setUiThreadId()
	{
		_uiThreadId=VThread::getCurrentThreadId();
		_uiThread=QThread::currentThread();
		_uiThreadIdSet=true;
	}

	static inline QThread* getUiThread()
	{
		return(_uiThread);
	}

	static inline bool isUiThread()
	{
		return(areThreadIDsSame(_uiThreadId,getCurrentThreadId()));
	}

	static inline bool isCurrentThreadTheUiThread()
	{
		return(areThreadIDsSame(_uiThreadId,VThread::getCurrentThreadId()));
	}

	static inline bool areThreadIDsSame(VTHREAD_ID_TYPE threadA,VTHREAD_ID_TYPE threadB)
	{
#ifdef WIN_VREP
		return(threadA==threadB); // normally this is the only thread ID comparison place!! Except for another one in VMutex
#else
		return(pthread_equal(threadA,threadB)!=0); // normally this is the only thread ID comparison place!! Except for another one in VMutex (THREAD_ID_COMPARISON)
#endif
	}

	static inline VTHREAD_ID_TYPE getCurrentThreadId()
	{
#ifdef WIN_VREP
		if (CAuxLib::vrepWin_getCurrentThreadId!=0)
			return(CAuxLib::vrepWin_getCurrentThreadId());
		return(0);
#else
		return(pthread_self());
#endif
	}

	static inline void switchThread()
	{
#ifdef WIN_VREP
		CAuxLib::vrepWin_switchThread();
#endif
#ifdef MAC_VREP
		pthread_yield_np();
#endif
#ifdef LIN_VREP
		pthread_yield();
#endif
	}

	static inline void sleep(int ms)
	{
#ifdef WIN_VREP
		CAuxLib::vrepWin_sleep(ms);
#else
		usleep(ms*1000);
#endif
	}

private:
	static bool _mainThreadIdSet;
	static bool _uiThreadIdSet;
	static VTHREAD_ID_TYPE _mainThreadId;
	static VTHREAD_ID_TYPE _uiThreadId;
	static QThread* _mainThread;
	static QThread* _uiThread;
	static QMutex _lock;
	static std::vector<QThread*> _apiQueriedThreadIds;
#ifdef WIN_VREP
	static DWORD_PTR _mainThreadAffinityMask;
#else
	static quint32 _mainThreadAffinityMask;
#endif

public:
	static WORD VTHREAD_LOWEST_PRIORITY;
	static WORD VTHREAD_NORMAL_PRIORITY;
	static WORD VTHREAD_HIGHEST_PRIORITY;
};

