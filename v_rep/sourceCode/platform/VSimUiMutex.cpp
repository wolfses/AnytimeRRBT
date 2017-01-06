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
#include "VSimUiMutex.h"

VSimUiMutex::VSimUiMutex()
{
	_lockLevel=0;
}

VSimUiMutex::~VSimUiMutex()
{
}

void VSimUiMutex::lock(int threadType)
{
	tryLock(threadType,-1); // negative time out --> no time out!
}

bool VSimUiMutex::tryLock(int threadType,int timeOut/*=0*/)
{ // if timeOut is negative, there is no time out!
	bool retVal=false;

	_cs_aux.lock();

	bool doTheRealLock=false;
	if (_lockLevel==0)
	{ // not locked
		doTheRealLock=true;
	}
	else
	{
		if (threadType==_threadType)
		{ // already locked by self
			_lockLevel++;
			retVal=true;
		}
		else
		{ // already locked by other
			doTheRealLock=true;
		}
	}

	if (doTheRealLock)
	{
		_cs_aux.unlock();
		retVal=_cs.tryLock(timeOut);
		_cs_aux.lock();
		if (retVal)
		{
			_lockLevel=1;
			_threadType=threadType;
		}
	}

	_cs_aux.unlock();

	return(retVal);
}

void VSimUiMutex::unlock()
{
	_cs_aux.lock();
	_lockLevel--;
	if (_lockLevel==0)
		_cs.unlock();
	_cs_aux.unlock();
}

