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
#include "porting.h"

class CInConnection
{
public:
	CInConnection(int theConnectionPort,int maxPacketSize,bool newVersion);
	virtual ~CInConnection();

	bool connectToClient();
	char* receiveMessage(int& messageSize);
	bool replyToReceivedMessage(char* message,int messageSize);

	std::string getConnectedMachineIP();
	bool isOtherSideBigEndian();
	void stopWaitingForConnection();

protected:
	bool _sendSimplePacket(char* packet,int packetLength,WORD packetsLeft);
	int _receiveSimplePacket(std::vector<char>& packet);

	bool _send_sharedMem(const char* data,int dataLength);
	char* _receive_sharedMem(int& dataLength);

	_timeval		_socketTimeOut;
	int				_socketConnectionPort;
	std::string		_socketConnectedMachineIP;
	#ifdef _WIN32
		WSADATA			_socketWsaData;
		HANDLE			_mapFileHandle;
	#endif /* _WIN32 */

	int				_maxPacketSize;
	bool			_otherSideIsBigEndian;
	bool			_connected;
	bool			_newVersion;
	bool			_usingSharedMem;
	bool			_leaveConnectionWait;


	// old version:
	_SOCKET				_socketServer;
	_SOCKET				_socketClient;
	struct sockaddr_in	_socketLocal;
	fd_set				_socketTheSet;

	// new version:
	_SOCKET				_local_socket;
	_SOCKET				_accepted_socket;
	struct sockaddr_in	_address;
	fd_set				_read_fd;
	bool				_listening;
};
