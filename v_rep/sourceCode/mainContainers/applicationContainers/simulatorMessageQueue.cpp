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
#include "simulatorMessageQueue.h"
#include "App.h"

CSimulatorMessageQueue::CSimulatorMessageQueue()
{
}

CSimulatorMessageQueue::~CSimulatorMessageQueue()
{
	_mutex.lock();
	for (int i=0;i<int(_messages.size());i++)
		delete[] _messages[i].data;
	_mutex.unlock();
}

void CSimulatorMessageQueue::addCommand(int commandID,int auxVal1,int auxVal2,int auxVal3,int auxVal4,char* data,int dataSize)
{
	_mutex.lock();
	SMessageQueueMessage msg;
	msg.messageID=commandID;
	msg.auxValues[0]=auxVal1;
	msg.auxValues[1]=auxVal2;
	msg.auxValues[2]=auxVal3;
	msg.auxValues[3]=auxVal4;
	msg.data=data;
	msg.dataLength=dataSize;
	_messages.push_back(msg);
	_mutex.unlock();
}

char* CSimulatorMessageQueue::extractOneCommand(int& commandID,int auxVals[4],int& dataSize)
{
	_mutex.lock();
	char* retVal=NULL;
	commandID=-1;
	if (_messages.size()!=0)
	{
		commandID=_messages[0].messageID;
		auxVals[0]=_messages[0].auxValues[0];
		auxVals[1]=_messages[0].auxValues[1];
		auxVals[2]=_messages[0].auxValues[2];
		auxVals[3]=_messages[0].auxValues[3];
		dataSize=_messages[0].dataLength;
		retVal=_messages[0].data;
		_messages.erase(_messages.begin());
	}
	_mutex.unlock();
	return(retVal);
}
