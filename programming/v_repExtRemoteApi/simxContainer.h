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
#include "simxCmd.h"

class CSimxContainer
{
public:
	CSimxContainer(bool isInputContainer);
	virtual ~CSimxContainer();

	void clearAll();

	void addCommand(CSimxCmd* cmd,bool doNotOverwriteSameCommand);
	char* addPartialCommand(const char* buffer,bool otherSideIsBigEndian);
	int _arePartialCommandsSame(const char* buff1,const char* buff2,bool otherSideIsBigEndian);
	void executeCommands(CSimxContainer* outputContainer,CSimxSocket* sock);
	void setCommandsAlreadyExecuted(bool e);
	bool getCommandsAlreadyExecuted();
	int getDataString(std::vector<char>& dataString,bool otherSideIsBigEndian);
	int getDataStringOfSplitOrGradualCommands(std::vector<char>& dataString,bool otherSideIsBigEndian);
	int getStreamCommandCount();
	void setMessageID(int id);
	int getMessageID();
	void setDataTimeStamp(int ts);
	void setDataServerTimeStamp(int st);
	void setSceneID(WORD id);
	void setServerState(BYTE serverState);

	int getCommandCount();
	void setOtherSideIsBigEndian(bool bigEndian);

protected:
	int _getIndexOfSimilarCommand(CSimxCmd* cmd);
	void _removeNonContinuousCommands();

	int _messageID;
	int _dataTimeStamp; // client time stamp
	int _dataServerTime; // server time stamp
	WORD _sceneID;
	BYTE _serverState;
	bool _isInputContainer;
	bool _otherSideIsBigEndian;
	std::vector<CSimxCmd*> _allCommands;
	std::vector<char*> _partialCommands;
};
