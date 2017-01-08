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

#include "vrepPrecompiledHeader.h"
#include "VThread.h"

#include "WThread.h"
QMutex VThread::_lock;
bool VThread::_mainThreadIdSet=false;
bool VThread::_uiThreadIdSet=false;
VTHREAD_ID_TYPE VThread::_mainThreadId=VTHREAD_ID_DEAD;
VTHREAD_ID_TYPE VThread::_uiThreadId=VTHREAD_ID_DEAD;
QThread* VThread::_mainThread=NULL;
QThread* VThread::_uiThread=NULL;
WORD VThread::VTHREAD_LOWEST_PRIORITY=0;
WORD VThread::VTHREAD_NORMAL_PRIORITY=1;
WORD VThread::VTHREAD_HIGHEST_PRIORITY=2;
std::vector<QThread*> VThread::_apiQueriedThreadIds;
#ifdef WIN_VREP
	DWORD_PTR VThread::_mainThreadAffinityMask=0; // Means we use the OS default
#else
	quint32 VThread::_mainThreadAffinityMask=0; // Means we use the OS default
#endif


void VThread::setProcessorCoreAffinity(int mode)
{
#ifdef WIN_VREP
	if (mode<0)
	{
		_mainThreadAffinityMask=0; // Means: we keep what the os choose
		return;
	}
	if (mode>0)
	{ // we (try to) use the mask given in "mode":
		_mainThreadAffinityMask=mode;

		DWORD_PTR oldAffinity=CAuxLib::vrepWin_setThreadAffinity(_mainThreadAffinityMask);
		if (oldAffinity==0)
			_mainThreadAffinityMask=0; // we failed setting what we wanted... we use the os default
		return;
	}
	else
	{
		bool found=false;
		for (int i=0;i<64;i++)
		{
			_mainThreadAffinityMask=(1<<i);
			DWORD_PTR oldAffinity=CAuxLib::vrepWin_setThreadAffinity(_mainThreadAffinityMask);
			if (oldAffinity!=0)
			{ // Means we could set the new affinity!
				found=true;
				break; 
			}
		}
		if (!found)
			_mainThreadAffinityMask=0;
	}
#else
	// not yet implemented
#endif
}

void VThread::launchThread(VTHREAD_START_ADDRESS startAddress,WORD priority,bool followMainThreadAffinity)
{
	int prio=1; // normal
	if (priority==VTHREAD_LOWEST_PRIORITY)
		prio=0;
	if (priority==VTHREAD_HIGHEST_PRIORITY)
		prio=2;
#ifdef WIN_VREP
	DWORD_PTR aff=0;
	if ( (_mainThreadAffinityMask!=0)&&followMainThreadAffinity )
		aff=_mainThreadAffinityMask;
	CAuxLib::vrepWin_launchThread(startAddress,prio,aff);
#else
	quint32 aff=0;
	if ( (_mainThreadAffinityMask!=0)&&followMainThreadAffinity )
		aff=_mainThreadAffinityMask;
	pthread_t th;
	pthread_create(&th,NULL,startAddress,NULL);
#endif
}

void VThread::launchSimpleThread(SIMPLE_VTHREAD_START_ADDRESS startAddress,WORD priority)
{
	_lock.lock();
	Thread* it=new Thread();
	Thread::startAddress=startAddress;
	Thread::startAddressIsFree=false;
	if (priority==VTHREAD_LOWEST_PRIORITY)
	    it->start(QThread::LowestPriority);
	if (priority==VTHREAD_NORMAL_PRIORITY)
	    it->start(QThread::NormalPriority);
	if (priority==VTHREAD_HIGHEST_PRIORITY)
	    it->start(QThread::HighestPriority);
	while (!Thread::startAddressIsFree)
	    QThread::yieldCurrentThread();
	_lock.unlock();
}

int VThread::getThreadId_apiQueried()
{ // this is an artificial ID, just needed externally. 0=GUI thread, 1=main sim thread.
	if ((_apiQueriedThreadIds.size()==0)&&isMainThreadIdSet()&&isUiThreadIdSet())
	{
		_apiQueriedThreadIds.push_back(_uiThread);
		_apiQueriedThreadIds.push_back(_mainThread);
	}
	if (_apiQueriedThreadIds.size()==0)
		return(-1);
	QThread* t=QThread::currentThread();
	for (int i=0;i<int(_apiQueriedThreadIds.size());i++)
	{
		if (_apiQueriedThreadIds[i]==t)
			return(i);
	}
	_apiQueriedThreadIds.push_back(t);
	return(_apiQueriedThreadIds.size()-1);
}

