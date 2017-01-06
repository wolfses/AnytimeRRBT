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
#include "sensingThreadPool.h"
#include "RegisteredCollisions.h"
#include "RegisteredDistances.h"
#include "proximitySensor.h"

std::vector<SParallelWorkDat> CSensingThreadPool::_tasks;
volatile int CSensingThreadPool::_threadDeinitCnt=0;
volatile int CSensingThreadPool::_threadIdleCnt=0;
volatile bool CSensingThreadPool::_simulationEndedFlag=false;
volatile int CSensingThreadPool::_taskAddID=0;
int CSensingThreadPool::_threadCount=0;
int CSensingThreadPool::_nextThreadIndex=0;
VMutex CSensingThreadPool::_mutex;
VMutex CSensingThreadPool::_protectedAccessMutex;


void CSensingThreadPool::launchThreads(int desiredThreadCount)
{
	if (desiredThreadCount<=0)
		return;
	if (desiredThreadCount>256)
		desiredThreadCount=256;
	_simulationEndedFlag=false;
	_threadDeinitCnt=0;
	_threadIdleCnt=0;
	_tasks.clear();
	_nextThreadIndex=0;
	_threadCount=desiredThreadCount;
	for (int i=0;i<_threadCount;i++)
		VThread::launchThread(_parallelWorkEntry,VThread::VTHREAD_NORMAL_PRIORITY,false);
}

void CSensingThreadPool::killThreads()
{
	if (_threadCount<=0)
		return;
	waitUntilAllThreadsIdleAndTasksDone();
	_simulationEndedFlag=true;
	_mutex.wakeAll_simple();
	int threadDeinitCnt_copy;
	while (true)
	{ // wait until all threads have ended
		_mutex.lock_simple();
		threadDeinitCnt_copy=_threadDeinitCnt;
		_mutex.unlock_simple();
		if (threadDeinitCnt_copy==_threadCount)
			break;
		VThread::switchThread();
	}
	clearAllTasks();
	_nextThreadIndex=0;
	_threadCount=0;

}

int CSensingThreadPool::getThreadCount()
{
	return(_threadCount);	
}


void CSensingThreadPool::waitUntilAllThreadsIdleAndTasksDone()
{
	while (true)
	{ // wait until all threads have ended
		_mutex.lock_simple();
		if (_threadIdleCnt==_threadCount)
		{	// Ok, we have all threads in the idle section.
			// Now make sure that all tasks have been processed:
			bool ok=true;
			for (int i=0;i<int(_tasks.size());i++)
			{
				if ((!_tasks[i].handlingStarted)&&(!_tasks[i].freeSlot))
				{
					ok=false;
					break;
				}
			}
			if (ok)
			{ // ok, all tasks have been processed!
				_mutex.unlock_simple();
				break; // ok, they are all idle and will stay idle!
			}
		}
		_mutex.unlock_simple();
		VThread::switchThread();
	}
}

void CSensingThreadPool::addTask(const SParallelWorkDat& task)
{
	_mutex.lock_simple();
	// We first try to insert the task into an empty slot:
	bool done=false;
	for (int i=0;i<int(_tasks.size());i++)
	{
		if (_tasks[i].freeSlot)
		{
			_tasks[i]=task;
			done=true;
			break;
		}
	}
	if (!done)
		_tasks.push_back(task); // we have to append the task: no free slot
	_taskAddID++; // to signal the threads there is more to process
	_mutex.unlock_simple();
	_mutex.wakeAll_simple();
}

std::vector<SParallelWorkDat>* CSensingThreadPool::getTasks()
{
	return(&_tasks);
}

void CSensingThreadPool::clearAllTasks()
{
	_tasks.clear();
}

VTHREAD_RETURN_TYPE CSensingThreadPool::_parallelWorkEntry(VTHREAD_ARGUMENT_TYPE lpData)
{ // this is where the threads live and do their work!
	int threadIndex;
	_mutex.lock_simple(); //************************************************** LOCKING ********************************************************
	threadIndex=_nextThreadIndex++;
	int threadTaskID;
	while (true)
	{
		threadTaskID=_taskAddID;
		for (int i=0;i<int(_tasks.size());i++)
		{
			if ((!_tasks[i].handlingStarted)&&(!_tasks[i].freeSlot))
			{
				_tasks[i].handlingStarted=true;
				_mutex.unlock_simple(); //************************************************** UNLOCKING ********************************************************
				if (_tasks[i].objectType==0)
				{ // handle a collision object
					CRegColl* collObj=(CRegColl*)_tasks[i].object;
					if ( (!collObj->getExplicitHandling())||(!_tasks[i].exceptExplicit) )
						collObj->handleCollision(true);
				}
				if (_tasks[i].objectType==1)
				{ // handle a distance object
					CRegDist* distObj=(CRegDist*)_tasks[i].object;
					if ( (!distObj->getExplicitHandling())||(!_tasks[i].exceptExplicit) )
						distObj->handleDistance(true);
				}
				if (_tasks[i].objectType==2)
				{ // handle a proximity sensor
					int detectedObj;
					C3Vector detectedSurf;
					((CProxSensor*)_tasks[i].object)->handleSensor(true,_tasks[i].exceptExplicit,detectedObj,detectedSurf);
				}
				_mutex.lock_simple(); //************************************************** LOCKING ********************************************************
				_tasks[i].freeSlot=true;
				i=-1; // start the loop from the beginning again!
			}
		}

		_threadIdleCnt++;
		while (true)
		{
			if (_simulationEndedFlag)
				break;
			if (threadTaskID!=_taskAddID)
				break;
			_mutex.wait_simple(); //************************************************** UNLOCKING/WAITING/LOCKING ********************************************************
		}
		_threadIdleCnt--;

		if (_simulationEndedFlag)
			break; // simulation ended. All threads have to end too
	}
	_threadDeinitCnt++;	
	_mutex.unlock_simple(); //************************************************** UNLOCKING ********************************************************
	return(0);
}

void CSensingThreadPool::setProtectedAccess(bool p)
{ // can be called in a cascaded way
	if (p)
		_protectedAccessMutex.lock();
	else
		_protectedAccessMutex.unlock();
}
