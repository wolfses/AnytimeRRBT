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
#include "ThreadPool.h"
#include "IloIlo.h"
#include "App.h"
#include "VDateTime.h"
#include "debugLogFile.h"
#include <boost/lexical_cast.hpp>

#ifdef MAC_VREP
	#include <curses.h>
#endif

#define STOP_REQUEST_DELAY_IN_MS 1000

int CThreadPool::_processorCoreAffinity=0;
int CThreadPool::_lockStage=0;
std::vector<CVThreadData*> CThreadPool::_allThreadData;
std::vector<VTHREAD_ID_TYPE> CThreadPool::_threadQueue;
std::vector<int> CThreadPool::_threadStartTime;
std::vector<int> CThreadPool::_instancesAndTheirStopRequestTimes;
std::vector<int> CThreadPool::_instancesThatRequestedEmergencyStop;
VTHREAD_START_ADDRESS CThreadPool::_threadStartAdd=NULL;
VMutex CThreadPool::_threadPoolMutex;

bool CThreadPool::_showThreadSwitches=false;

//std::vector<QOpenGLContext*> CThreadPool::_glContextToThreadOrder_context;
//std::vector<QThread*> CThreadPool::_glContextToThreadOrder_thread;
//std::vector<VTHREAD_ID_TYPE> CThreadPool::_glContextToThreadOrder_threadId;


VTHREAD_ID_TYPE CThreadPool::createNewThread(VTHREAD_START_ADDRESS threadStartAddress)
{
	_lock(0);

	if (_allThreadData.size()==0)
	{ // We first need to add the main thread! executiontime: -1 --> not yet executed
		CVThreadData* dat=new CVThreadData(NULL,VThread::getCurrentThreadId(),App::ct->getCurrentInstanceIndex());
		_allThreadData.push_back(dat);

		_threadQueue.push_back(VThread::getCurrentThreadId());
		_threadStartTime.push_back(VDateTime::getTimeInMs());
		VThread::setProcessorCoreAffinity(_processorCoreAffinity);
	}

	_threadStartAdd=threadStartAddress;
	if (_showThreadSwitches)
	{
		std::string tmp("==* Launching thread (from threadID: ");
		tmp+=boost::lexical_cast<std::string>((unsigned long)VThread::getCurrentThreadId());
		tmp+=")\n";
		CDebugLogFile::addDebugText(false,tmp.c_str());
//		printf("Launching thread (from threadID: %lu)\n",(unsigned long)VThread::getCurrentThreadId());
	}
	VThread::launchThread(_intermediateThreadStartPoint,VThread::VTHREAD_NORMAL_PRIORITY,true);
	while (_threadStartAdd!=NULL)
		VThread::sleep(1); // We wait until the thread could set its thread ID ************* TODO: don't use sleep!!!
	VTHREAD_ID_TYPE newID=(VTHREAD_ID_TYPE)_allThreadData[_allThreadData.size()-1]->threadID;
	if (_showThreadSwitches)
	{
		std::string tmp("==* Thread was created with ID: ");
		tmp+=boost::lexical_cast<std::string>((unsigned long)newID);
		tmp+=" (from thread ID: ";
		tmp+=boost::lexical_cast<std::string>((unsigned long)_threadQueue[_threadQueue.size()-1]);
		tmp+=")\n";
		CDebugLogFile::addDebugText(false,tmp.c_str());
//		printf("Thread was created with ID: %lu (from thread ID: %lu)\n",(unsigned long)newID,(unsigned long)_threadQueue[_threadQueue.size()-1]);
	}
	_unlock(0);
	return(newID);
}

bool CThreadPool::setThreadSwitchTiming(int timeInMs)
{
	_lock(1);
	bool retVal=false;
	int fqs=int(_threadQueue.size());
	if (fqs>1)
	{
		VTHREAD_ID_TYPE fID=_threadQueue[fqs-1];
		for (int i=0;i<int(_allThreadData.size());i++)
		{
			if (VThread::areThreadIDsSame(_allThreadData[i]->threadID,fID))
			{
				if (timeInMs<0)
					timeInMs=0;
				if (timeInMs>200)
					timeInMs=200;
				_allThreadData[i]->threadDesiredTiming=timeInMs;
				retVal=true;
				break;
			}
		}
	}
	_unlock(1);
	return(retVal);
}

bool CThreadPool::getThreadSwitchTiming(int& timeInMs)
{
	_lock(2);
	bool retVal=false;
	int fqs=int(_threadQueue.size());
	if (fqs>1)
	{
		VTHREAD_ID_TYPE fID=_threadQueue[fqs-1];
		for (int i=0;i<int(_allThreadData.size());i++)
		{
			if (VThread::areThreadIDsSame(_allThreadData[i]->threadID,fID))
			{
				timeInMs=_allThreadData[i]->threadDesiredTiming;
				retVal=true;
				break;
			}
		}
	}
	_unlock(2);
	return(retVal);
}

bool CThreadPool::setThreadAutomaticSwitch(bool switchIsAutomatic)
{
	_lock(1);
	bool retVal=false;
	int fqs=int(_threadQueue.size());
	if (fqs>1)
	{
		VTHREAD_ID_TYPE fID=_threadQueue[fqs-1];
		for (int i=0;i<int(_allThreadData.size());i++)
		{
			if (VThread::areThreadIDsSame(_allThreadData[i]->threadID,fID))
			{
				_allThreadData[i]->threadShouldNotSwitch=!switchIsAutomatic;
				retVal=true;
				break;
			}
		}
	}
	_unlock(1);
	return(retVal);
}

bool CThreadPool::setThreadResumeLocation(int location,int order)
{
	_lock(99);
	bool retVal=false;
	int fqs=int(_threadQueue.size());
	if (fqs>1)
	{
		VTHREAD_ID_TYPE fID=_threadQueue[fqs-1];
		for (int i=0;i<int(_allThreadData.size());i++)
		{
			if (VThread::areThreadIDsSame(_allThreadData[i]->threadID,fID))
			{
				if (order<0)
					order=-1;
				if (order>0)
					order=1;
				_allThreadData[i]->threadResumeLocationAndOrder=location*3+(order+1);
				retVal=true;
				break;
			}
		}
	}
	_unlock(1);
	return(retVal);
}


void CThreadPool::switchToThread(VTHREAD_ID_TYPE threadID)
{
	_lock(3);
	int fql=int(_threadQueue.size());
	if (!VThread::areThreadIDsSame(_threadQueue[fql-1],VThread::getCurrentThreadId()))
	{
		_unlock(3);
		return; // We have a free-running thread here (cannot be stopped if it doesn't itself disable the free-running mode)
	}
	for (int i=0;i<int(_allThreadData.size());i++)
	{
		if (VThread::areThreadIDsSame(_allThreadData[i]->threadID,threadID))
		{
			VTHREAD_ID_TYPE oldFiberID=_threadQueue[fql-1];
			if ((fql>1)&&VThread::areThreadIDsSame(threadID,_threadQueue[fql-2]))
			{ // we switch back to a previous (calling) fiber
				CVThreadData* it=NULL;
				for (int j=0;j<int(_allThreadData.size());j++)
				{
					if (VThread::areThreadIDsSame(_allThreadData[j]->threadID,oldFiberID))
					{
						it=_allThreadData[j];
						break;
					}
				}
				// Before we proceed, there is a special case to handle here:
				// When a thread was marked for termination, its fiberOrThreadID is 0xfffffff, and in
				// consequence, it would be NULL here.

				if ( (it==NULL)||(!it->threadSwitchShouldTriggerNoOtherThread) )
				{ // Regular situation
					_threadQueue.pop_back();
					int totalTimeInMs=VDateTime::getTimeDiffInMs(_threadStartTime[fql-1]);
					_threadStartTime.pop_back();
					_threadStartTime[fql-2]+=totalTimeInMs; // We have to "remove" the time spent in the called fiber!
					if (_showThreadSwitches)
					{
						std::string tmp("==< Switching backward from threadID: ");
						tmp+=boost::lexical_cast<std::string>((unsigned long)oldFiberID);
						tmp+=" to threadID: ";
						tmp+=boost::lexical_cast<std::string>((unsigned long)_allThreadData[i]->threadID);
						tmp+="\n";
						CDebugLogFile::addDebugText(false,tmp.c_str());
//						printf("Switching backward from threadID: %lu to threadID: %lu\n",(unsigned long)oldFiberID,(unsigned long)_allThreadData[i]->threadID);
					}
					_allThreadData[i]->threadWantsResumeFromYield=true; // We mark the next thread for resuming
				}
				else
				{ // Happens when a thread that used to be free running (or that still is) comes through here
					_threadQueue.pop_back();
					_threadStartTime.pop_back();
					if ( (it!=NULL)&&(!it->threadShouldRunFreely)&&_showThreadSwitches)
					{
						std::string tmp("==< Switching backward from previously free-running thread with ID: ");
						tmp+=boost::lexical_cast<std::string>((unsigned long)oldFiberID);
						tmp+="\n";
						CDebugLogFile::addDebugText(false,tmp.c_str());
//						printf("Switching backward from previously free-running thread with ID: %lu\n",(unsigned long)oldFiberID);
					}
				}

				_unlock(3); // make sure to unlock here
				if (it!=NULL)
				{
					it->threadSwitchShouldTriggerNoOtherThread=false; // We have to reset this one
					// Now we wait here until this thread gets flagged as threadWantsResumeFromYield:
					if (_showThreadSwitches)
					{
						std::string tmp("==< Backward switch part, threadID ");
						tmp+=boost::lexical_cast<std::string>((unsigned long)it->threadID);
						tmp+=" (";
						tmp+=boost::lexical_cast<std::string>((unsigned long)VThread::getCurrentThreadId());
						tmp+=") is waiting...\n";
						CDebugLogFile::addDebugText(false,tmp.c_str());
//						printf("Backward switch part, threadID %lu (%lu) is waiting...\n",(unsigned long)it->threadID,(unsigned long)VThread::getCurrentThreadId());
					}
					while (!it->threadWantsResumeFromYield)
					{
//						_handleOpenGlContextMoves();
//						App::qtApp->processEvents();
						VThread::switchThread();
					}
					if (_showThreadSwitches)
					{
						std::string tmp("==< Backward switch part, threadID ");
						tmp+=boost::lexical_cast<std::string>((unsigned long)it->threadID);
						tmp+=" (";
						tmp+=boost::lexical_cast<std::string>((unsigned long)VThread::getCurrentThreadId());
						tmp+=") NOT waiting anymore...\n";
						CDebugLogFile::addDebugText(false,tmp.c_str());
//						printf("Backward switch part, threadID %lu (%lu) NOT waiting anymore...\n",(unsigned long)it->threadID,(unsigned long)VThread::getCurrentThreadId());
					}
					// If we arrived here, it is because CThreadPool::switchToFiberOrThread was called for this thread from another thread
					it->threadWantsResumeFromYield=false; // We reset it
					// Now this thread resumes!
				}
				return;
			}
			else
			{ // we switch forward to an auxiliary thread
				_threadQueue.push_back(threadID);
				_threadStartTime.push_back(VDateTime::getTimeInMs());
				if (_showThreadSwitches)
				{
					std::string tmp("==> Switching forward from threadID: ");
					tmp+=boost::lexical_cast<std::string>((unsigned long)_threadQueue[_threadQueue.size()-2]);
					tmp+=" to threadID: ";
					tmp+=boost::lexical_cast<std::string>((unsigned long)threadID);
					tmp+="\n";
					CDebugLogFile::addDebugText(false,tmp.c_str());
//					printf("Switching forward from threadID: %lu to threadID: %lu\n",(unsigned long)_threadQueue[_threadQueue.size()-2],(unsigned long)threadID);
				}
				_allThreadData[i]->threadWantsResumeFromYield=true; // We mark the next thread for resuming
				// We do not need to idle this thread since it is already flagged as such
				CVThreadData* it=NULL;
				for (int j=0;j<int(_allThreadData.size());j++)
				{
					if (VThread::areThreadIDsSame(_allThreadData[j]->threadID,oldFiberID))
					{
						it=_allThreadData[j];
						break;
					}
				}
				// Now we wait here until this thread gets flagged as threadWantsResumeFromYield:
				_unlock(3);
				if (_showThreadSwitches)
				{
					std::string tmp("==> Forward switch part, threadID ");
					tmp+=boost::lexical_cast<std::string>((unsigned long)it->threadID);
					tmp+=" (";
					tmp+=boost::lexical_cast<std::string>((unsigned long)VThread::getCurrentThreadId());
					tmp+=") is waiting...\n";
					CDebugLogFile::addDebugText(false,tmp.c_str());
//					printf("Forward switch part, threadID %lu (%lu) is waiting...\n",(unsigned long)it->threadID,(unsigned long)VThread::getCurrentThreadId());
				}
				while (!it->threadWantsResumeFromYield)
				{
//					_handleOpenGlContextMoves();
//					App::qtApp->processEvents();
					VThread::switchThread();
				}
				if (_showThreadSwitches)
				{
					std::string tmp("==> Forward switch part, threadID ");
					tmp+=boost::lexical_cast<std::string>((unsigned long)it->threadID);
					tmp+=" (";
					tmp+=boost::lexical_cast<std::string>((unsigned long)VThread::getCurrentThreadId());
					tmp+=") NOT waiting anymore...\n";
					CDebugLogFile::addDebugText(false,tmp.c_str());
//					printf("Forward switch part, threadID %lu (%lu) NOT waiting anymore...\n",(unsigned long)it->threadID,(unsigned long)VThread::getCurrentThreadId());
				}
				// If we arrived here, it is because CThreadPool::switchToFiberOrThread was called for this thread from another thread
				it->threadWantsResumeFromYield=false; // We reset it
				// Now this thread resumes!
				_cleanupTerminatedThreads(); // This routine will perform the locking unlocking itself
				return;
			}
		}
	}
	_unlock(3);
}

bool CThreadPool::switchBackToPreviousThread()
{
	_lock(4);
	int fql=int(_threadQueue.size());
	if (fql>1)
	{ // Switch back only if not main thread
		int totalTimeInMs=VDateTime::getTimeDiffInMs(_threadStartTime[fql-1]);
		for (int i=0;i<int(_allThreadData.size());i++)
		{
			if (VThread::areThreadIDsSame(_allThreadData[i]->threadID,_threadQueue[fql-1]))
			{
				_allThreadData[i]->threadExecutionTime=totalTimeInMs;
				break;
			}
		}
		_unlock(4);
		switchToThread(_threadQueue[fql-2]); // has its own locking / unlocking
		return(true);
	}
	_unlock(4);
	return(false);
}

void CThreadPool::switchBackToPreviousThreadIfNeeded()
{
	_lock(5);
	int fql=int(_threadQueue.size());
	if (fql>1)
	{ // Switch back only if not main thread
		int totalTimeInMs=VDateTime::getTimeDiffInMs(_threadStartTime[fql-1]);
		for (int i=0;i<int(_allThreadData.size());i++)
		{
			if (VThread::areThreadIDsSame(_allThreadData[i]->threadID,_threadQueue[fql-1]))
			{
				if (_allThreadData[i]->threadDesiredTiming<=totalTimeInMs)
				{
					if (!_allThreadData[i]->threadShouldNotSwitch) 
					{
						_unlock(5);
						switchBackToPreviousThread(); // Has its own locking / unlocking
						return;
					}
				}
				break;
			}
		}
	}
	_unlock(5);
}

void CThreadPool::_terminateThread()
{
	_lock(6);
	VTHREAD_ID_TYPE nextThreadToSwitchTo=0;
	for (int i=0;i<int(_allThreadData.size());i++)
	{
		int fql=int(_threadQueue.size());
		if (VThread::areThreadIDsSame(_allThreadData[i]->threadID,_threadQueue[fql-1]))
		{
			if (_showThreadSwitches)
			{
				std::string tmp("==q Terminating thread: ");
				tmp+=boost::lexical_cast<std::string>((unsigned long)_allThreadData[i]->threadID);
				tmp+="\n";
				CDebugLogFile::addDebugText(false,tmp.c_str());
//				printf("Terminating thread: %lu\n",(unsigned long)_allThreadData[i]->threadID);
			}
			_allThreadData[i]->threadID=VTHREAD_ID_DEAD; // To indicate we need clean-up
			nextThreadToSwitchTo=_threadQueue[fql-2]; // This will be the next thread we wanna switch to
			break;
		}
	}
	_unlock(6);
	switchToThread(nextThreadToSwitchTo); // We switch to the calling thread (previous thread)
}

void CThreadPool::_cleanupTerminatedThreads()
{
	_lock(7);
	for (int i=0;i<int(_allThreadData.size());i++)
	{
		if (VThread::areThreadIDsSame(_allThreadData[i]->threadID,VTHREAD_ID_DEAD)&&(_allThreadData[i]->threadInstanceIndex==App::ct->getCurrentInstanceIndex()) )
		{ // That thread died or is dying, we clean-up
			delete _allThreadData[i];
			_allThreadData.erase(_allThreadData.begin()+i);
			i--;
		}
	}
	_unlock(7);
}

VTHREAD_RETURN_TYPE CThreadPool::_intermediateThreadStartPoint(VTHREAD_ARGUMENT_TYPE lpData)
{
	srand(VDateTime::getTimeInMs()+ (((unsigned long)(VThread::getCurrentThreadId()))&0xffffffff) ); // Important: each thread starts with a same seed!!!
	VTHREAD_START_ADDRESS startAdd=_threadStartAdd;
	CVThreadData* it=new CVThreadData(NULL,VThread::getCurrentThreadId(),App::ct->getCurrentInstanceIndex());
	_allThreadData.push_back(it);
	_threadStartAdd=NULL; // To indicate we could set the thread iD (in case of threads)
	it->threadWantsResumeFromYield=false;
	while (!it->threadWantsResumeFromYield)
	{
//		_handleOpenGlContextMoves();
//		App::qtApp->processEvents();
		VThread::switchThread();
	}
	// If we arrived here, it is because CThreadPool::switchToFiberOrThread was called for this thread from another thread
	it->threadWantsResumeFromYield=false; // We reset it
	if (_showThreadSwitches)
	{
		std::string tmp("==* Inside new thread (threadID: ");
		tmp+=boost::lexical_cast<std::string>((unsigned long)VThread::getCurrentThreadId());
		tmp+=")\n";
		CDebugLogFile::addDebugText(false,tmp.c_str());
//		printf("Inside new thread (threadID: %lu)\n",(unsigned long)VThread::getCurrentThreadId());
	}
	VTHREAD_RETURN_TYPE retVal=startAdd(lpData);
	// The thread ended

/*
	_lock(21);
	if (it->openGlContextInThread!=NULL)
	{ // we need to switch the openGl context back to the main thread before leaving this thread!!!
		it->openGlContextInThread->moveToThread(VThread::getMainThread());
	}
	_unlock(21);
*/
	_terminateThread();
	return(retVal);
}

void CThreadPool::prepareAllThreadsForResume_calledBeforeMainScript()
{
	_lock(8);
	for (int i=1;i<int(_allThreadData.size());i++)
	{
		if (_allThreadData[i]->threadInstanceIndex==App::ct->getCurrentInstanceIndex())
		{
			// Following is a special condition to support free-running mode:
			if ( (!_allThreadData[i]->threadShouldRunFreely)&&(!_allThreadData[i]->threadSwitchShouldTriggerNoOtherThread) )
			{
				_allThreadData[i]->threadExecutionTime=-1; // -1 --> not yet executed
				_allThreadData[i]->usedMovementTime=0.0f; // we reset the used movement time at every simulation pass (better results than if we don't do it, tested!)
			}
		}
	}
	_unlock(8);
}

int CThreadPool::handleAllThreads_withResumeLocation(int location)
{
	int retVal=0;
	_lock(8);
	bool doAll=false;
	if (location==-1) // Will resume all unhandled threads (to be called at the end of the main script)
	{
		location=0;
		doAll=true;
	}
	for (int j=3*location;j<3*location+3;j++)
	{
		for (int i=1;i<int(_allThreadData.size());i++)
		{
			if ((_allThreadData[i]->threadResumeLocationAndOrder==j)||doAll)
			{ // We first execute those with 0, then 1, then 2! (then 3 in the sensing phase!)
				if ((_allThreadData[i]->threadExecutionTime==-1)&&(_allThreadData[i]->threadInstanceIndex==App::ct->getCurrentInstanceIndex()) )
				{
					// Following is a special condition to support free-running mode:
					if ( (!_allThreadData[i]->threadShouldRunFreely)&&(!_allThreadData[i]->threadSwitchShouldTriggerNoOtherThread) )
					{
						if (_showThreadSwitches)
						{
							std::string tmp("==. In fiber/thread handling routine (fiberID/threadID: ");
							tmp+=boost::lexical_cast<std::string>((unsigned long)_threadQueue[_threadQueue.size()-1]);
							tmp+=")\n";
							CDebugLogFile::addDebugText(false,tmp.c_str());
//							printf("In fiber/thread handling routine (fiberID/threadID: %lu)\n",(unsigned long)_threadQueue[_threadQueue.size()-1]);
						}
						_unlock(8);
						switchToThread((VTHREAD_ID_TYPE)_allThreadData[i]->threadID);
						_lock(8);
						i=0; // We re-check from the beginning
						retVal++;
					}
				}
			}
		}
		if (doAll)
			break;
	}
	_unlock(8);
	return(retVal);
}

CVThreadData* CThreadPool::getCurrentThreadData()
{
	return(getThreadData(VThread::getCurrentThreadId()));
}

CVThreadData* CThreadPool::getThreadData(VTHREAD_ID_TYPE threadId)
{
	CVThreadData* retVal=NULL;
	_lock(9);
	for (int i=0;i<int(_allThreadData.size());i++)
	{
		if (VThread::areThreadIDsSame(_allThreadData[i]->threadID,threadId))
		{
			retVal=_allThreadData[i];
			break;
		}
	}
	_unlock(9);
	return(retVal);
}

int CThreadPool::getThreadCountForCurrentInstance()
{ // Doesn't count the main thread!
	int retVal=0;
	_lock(10);
	if (_threadQueue.size()!=0)
	{ // At least one thread was created
		for (int i=1;i<int(_allThreadData.size());i++)
		{
			if (_allThreadData[i]->threadInstanceIndex==App::ct->getCurrentInstanceIndex())
				retVal++;
		}
	}
	_unlock(10);
	return(retVal);
}

void CThreadPool::setEmergencyStopForCurrentInstance(bool stop)
{
	_lock(11);
	int inst=App::ct->getCurrentInstanceIndex();
	bool done=false;
	for (int i=0;i<int(_instancesThatRequestedEmergencyStop.size());i++)
	{
		if (_instancesThatRequestedEmergencyStop[i]==inst)
		{
			if (!stop)
				_instancesThatRequestedEmergencyStop.erase(_instancesThatRequestedEmergencyStop.begin()+i);
			done=true;
			break;
		}
	}
	if (!done)
	{
		if (stop)
			_instancesThatRequestedEmergencyStop.push_back(inst);
	}
	_unlock(11);
}

bool CThreadPool::getEmergencyStopForCurrentInstance()
{
	_lock(12);
	if (_threadQueue.size()!=0)
	{
		// Make sure the thread is not running in free mode. Otherwise we return false
		if (!VThread::areThreadIDsSame(_threadQueue[_threadQueue.size()-1],VThread::getCurrentThreadId()))
		{
			_unlock(12);
			return(false);
		}
	}

	bool retVal=false;
	int inst=App::ct->getCurrentInstanceIndex();
	for (int i=0;i<int(_instancesThatRequestedEmergencyStop.size());i++)
	{
		if (_instancesThatRequestedEmergencyStop[i]==inst)
		{
			retVal=true;
			break;
		}
	}
	_unlock(12);
	return(retVal);
}

void CThreadPool::setRequestStopForCurrentInstance(bool stop)
{
	_lock(13);
	int inst=App::ct->getCurrentInstanceIndex();
	bool done=false;
	for (int i=0;i<int(_instancesAndTheirStopRequestTimes.size())/2;i++)
	{
		if (_instancesAndTheirStopRequestTimes[2*i+0]==inst)
		{
			if (!stop)
				_instancesAndTheirStopRequestTimes.erase(_instancesAndTheirStopRequestTimes.begin()+2*i,_instancesAndTheirStopRequestTimes.begin()+2*i+2);
			done=true;
			break;
		}
	}
	if (!done)
	{
		if (stop)
		{
			_instancesAndTheirStopRequestTimes.push_back(inst);
			_instancesAndTheirStopRequestTimes.push_back(VDateTime::getTimeInMs());
		}
	}
	_unlock(13);
}

bool CThreadPool::getStopForCurrentInstanceRequested()
{
	if (getEmergencyStopForCurrentInstance())
		return(true);
	_lock(14);

	if (_threadQueue.size()!=0)
	{
		// Make sure the thread is not running in free mode. Otherwise we return false
		if (!VThread::areThreadIDsSame(_threadQueue[_threadQueue.size()-1],VThread::getCurrentThreadId()))
		{
			_unlock(14);
			return(false);
		}
	}

	bool retVal=false;
	int inst=App::ct->getCurrentInstanceIndex();
	for (int i=0;i<int(_instancesAndTheirStopRequestTimes.size())/2;i++)
	{
		if (_instancesAndTheirStopRequestTimes[2*i+0]==inst)
		{
			retVal=true;
			break;
		}
	}
	_unlock(14);
	return(retVal);
}

bool CThreadPool::getStopForCurrentInstanceRequestedAndActivated()
{
	if (getEmergencyStopForCurrentInstance())
		return(true);
	_lock(15);

	if (_threadQueue.size()!=0)
	{
		// Make sure the thread is not running in free mode. Otherwise we return false
		if (!VThread::areThreadIDsSame(_threadQueue[_threadQueue.size()-1],VThread::getCurrentThreadId()))
		{
			_unlock(15);
			return(false);
		}
	}

	bool retVal=false;
	int inst=App::ct->getCurrentInstanceIndex();
	for (int i=0;i<int(_instancesAndTheirStopRequestTimes.size())/2;i++)
	{
		if (_instancesAndTheirStopRequestTimes[2*i+0]==inst)
		{
			retVal=VDateTime::getTimeDiffInMs(_instancesAndTheirStopRequestTimes[2*i+1])>STOP_REQUEST_DELAY_IN_MS;
			break;
		}
	}
	_unlock(15);
	return(retVal);
}

void CThreadPool::_lock(BYTE debugInfo)
{
	_threadPoolMutex.lock();
	_lockStage++;
//	if (_lockStage==2)
//	{
//		VREP_BEEP;
//		printf("Error in _lock function\n");
//	}
}

void CThreadPool::_unlock(BYTE debugInfo)
{
	_lockStage--;
	_threadPoolMutex.unlock();
}

void CThreadPool::cleanUp()
{ // Make sure to release the main thread data:
	for (int i=0;i<int(_allThreadData.size());i++)
		delete _allThreadData[i];
}

int CThreadPool::getInstanceCurrentThreadIsAssociatedWith()
{
	if (_allThreadData.size()==0)
		return(-1);
	_lock(16);
	VTHREAD_ID_TYPE threadID=VThread::getCurrentThreadId();
	int retVal=-1;
	for (int i=0;i<int(_allThreadData.size());i++)
	{
		if (VThread::areThreadIDsSame(_allThreadData[i]->threadID,threadID))
		{
			retVal=_allThreadData[i]->threadInstanceIndex;
			break;
		}
	}
	_unlock(16);
	return(retVal);
}

bool CThreadPool::setThreadFreeMode(bool freeMode)
{
	bool retVal=false;
	VTHREAD_ID_TYPE thisThreadID=VThread::getCurrentThreadId();
	_lock(17);
	if (freeMode)
	{ // FREE MODE
		int fql=int(_threadQueue.size());
		if ( (fql>=2)&&VThread::areThreadIDsSame(_threadQueue[fql-1],thisThreadID) )
		{
			VTHREAD_ID_TYPE nextThreadID=_threadQueue[fql-2];
			CVThreadData* thisThreadData=NULL;		
			CVThreadData* nextThreadData=NULL;
			for (int i=0;i<int(_allThreadData.size());i++)
			{

				if (VThread::areThreadIDsSame(_allThreadData[i]->threadID,thisThreadID))
					thisThreadData=_allThreadData[i];
				if (VThread::areThreadIDsSame(_allThreadData[i]->threadID,nextThreadID))
					nextThreadData=_allThreadData[i];
			}

			if (_showThreadSwitches)
			{
				std::string tmp("==f Starting thread free-mode (threadID: ");
				tmp+=boost::lexical_cast<std::string>((unsigned long)thisThreadData->threadID);
				tmp+=")\n";
				CDebugLogFile::addDebugText(false,tmp.c_str());
//				printf("Starting thread free-mode (threadID: %lu)\n",(unsigned long)thisThreadData->threadID);
			}
			_threadQueue.pop_back();
			thisThreadData->freeModeSavedThreadStartTime=_threadStartTime[_threadStartTime.size()-1];
			_threadStartTime.pop_back();
			thisThreadData->threadShouldRunFreely=true;
			thisThreadData->threadSwitchShouldTriggerNoOtherThread=true;
			nextThreadData->threadWantsResumeFromYield=true;
			_unlock(17);
			if (_showThreadSwitches)
			{
				std::string tmp("==f Started thread free-mode (threadID: ");
				tmp+=boost::lexical_cast<std::string>((unsigned long)thisThreadData->threadID);
				tmp+=")\n";
				CDebugLogFile::addDebugText(false,tmp.c_str());
//				printf("Started thread free-mode (threadID: %lu)\n",(unsigned long)thisThreadData->threadID);
			}
			return(true);
		}
	}
	else
	{ // NON-FREE MODE
		int fql=int(_threadQueue.size());
		if ( (fql>=1)&&(!VThread::areThreadIDsSame(_threadQueue[fql-1],thisThreadID)) )
		{
			CVThreadData* thisThreadData=NULL;		
			for (int i=1;i<int(_allThreadData.size());i++)
			{
				if (VThread::areThreadIDsSame(_allThreadData[i]->threadID,thisThreadID))
					thisThreadData=_allThreadData[i];
			}

			if (_showThreadSwitches)
			{
				std::string tmp("==e Ending thread free-mode (threadID: ");
				tmp+=boost::lexical_cast<std::string>((unsigned long)thisThreadData->threadID);
				tmp+=")\n";
				CDebugLogFile::addDebugText(false,tmp.c_str());
//				printf("Ending thread free-mode (threadID: %lu)\n",(unsigned long)thisThreadData->threadID);
			}
			thisThreadData->threadShouldRunFreely=false;
			_threadQueue.push_back((VTHREAD_ID_TYPE)thisThreadData->threadID);
			_threadStartTime.push_back((int)thisThreadData->freeModeSavedThreadStartTime);
			_unlock(17);
			switchBackToPreviousThread();
			if (_showThreadSwitches)
			{
				std::string tmp("==e Ended thread free-mode (threadID: ");
				tmp+=boost::lexical_cast<std::string>((unsigned long)thisThreadData->threadID);
				tmp+=")\n";
				CDebugLogFile::addDebugText(false,tmp.c_str());
//				printf("Ended thread free-mode (threadID: %lu)\n",(unsigned long)thisThreadData->threadID);
			}
			return(true);
		}
	}

	if ( (!retVal)&&_showThreadSwitches )
	{
		if (freeMode)
		{
			std::string tmp("==x Failed starting thread free-mode (threadID: ");
			tmp+=boost::lexical_cast<std::string>((unsigned long)VThread::getCurrentThreadId());
			tmp+=")\n";
			CDebugLogFile::addDebugText(false,tmp.c_str());
//			printf("Failed starting thread free-mode (threadID: %lu)\n",(unsigned long)VThread::getCurrentThreadId());
		}
		else
		{
			std::string tmp("==x Failed stopping thread free-mode (threadID: ");
			tmp+=boost::lexical_cast<std::string>((unsigned long)VThread::getCurrentThreadId());
			tmp+=")\n";
			CDebugLogFile::addDebugText(false,tmp.c_str());
//			printf("Failed stopping thread free-mode (threadID: %lu)\n",(unsigned long)VThread::getCurrentThreadId());
		}
	}

	_unlock(17);
	return(retVal);
}

int CThreadPool::getProcessorCoreAffinity()
{
	return(_processorCoreAffinity);
}

void CThreadPool::setProcessorCoreAffinity(int affinity)
{
	_processorCoreAffinity=affinity;
}

bool CThreadPool::isThreadInFreeMode()
{
	if ( VThread::isMainThread()||(!VThread::isMainThreadIdSet()) )
		return(false);
	bool retVal=false;
	_lock(18);
	int fql=int(_threadQueue.size());
	if (!VThread::areThreadIDsSame(_threadQueue[fql-1],VThread::getCurrentThreadId()))
		retVal=true;
	_unlock(18);
	return(retVal);
}

bool CThreadPool::getShowThreadSwitches()
{
	return(_showThreadSwitches);
}

void CThreadPool::setShowThreadSwitches(bool show)
{
	_showThreadSwitches=show;
}

/*
void CThreadPool::makeOpenGlContextCurrent(QOpenGLContext* glContext,QOffscreenSurface* _offscreenSurface)
{
	_lock(19);
	if (glContext->thread()!=QThread::currentThread())
	{ // the context is not in the right thread. Place an order for that:
		_glContextToThreadOrder_context.push_back(glContext);
		_glContextToThreadOrder_thread.push_back(QThread::currentThread());
		_glContextToThreadOrder_threadId.push_back(VThread::getCurrentThreadId());
		_unlock(19);
		// Now wait until the current context thread moved the context to this thread:
		while (_glContextToThreadOrder_context.size()!=0)
			VThread::switchThread();
		_lock(19);
		// The other thread now moved the context to this thread!
	}
	_unlock(19);
	// the context is now in the right thread. We simply make it current:
	glContext->makeCurrent(_offscreenSurface);
}

void CThreadPool::_handleOpenGlContextMoves()
{
	_lock(20);
	if (_glContextToThreadOrder_context.size()!=0)
	{
		if (_glContextToThreadOrder_context[0]->thread()==QThread::currentThread())
		{ // ok, we are in the thread the context is currently in.
			// Let's move the context to the desired thread:
			_glContextToThreadOrder_context[0]->moveToThread(_glContextToThreadOrder_thread[0]);
			CVThreadData* data=getThreadData(_glContextToThreadOrder_threadId[0]);
			if (VThread::isMainThread())
			{ // the main thread moved the context to another thread
				// We need to memorize that. In case this thread exits, we need to move the context back to the main thread:
				data->openGlContextInThread=_glContextToThreadOrder_context[0];
			}
			else
			{ // an auxiliary thread moved the context back to the main thread.
				// We need to clear the context in that aux thread data:
				data->openGlContextInThread=NULL;
			}
		}
		_glContextToThreadOrder_context.clear();
		_glContextToThreadOrder_thread.clear();
		_glContextToThreadOrder_threadId.clear();
	}
	_unlock(20);
}
*/
