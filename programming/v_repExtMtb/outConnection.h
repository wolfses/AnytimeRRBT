// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// -------------------------------------------------------------------
// THIS FILE IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// You are free to use/modify/distribute this file for whatever purpose!
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#pragma once

#include <vector>
#include <string>
#include <QTcpSocket>

class COutConnection  
{
public:
	COutConnection(const char* theConnectionAddress,int theConnectionPort,unsigned short maxPacketSize=250,char headerID1=59,char headerID2=57);
	virtual ~COutConnection();

	int connectToServer();
	bool sendData(char* data,int dataSize);
	char* receiveReplyData(int& dataSize);

protected:
	bool _sendSimplePacket(char* packet,int packetLength,unsigned short packetsLeft);
	int _receiveSimplePacket(std::vector<char>& packet);

	int _getTimeInMs();

	std::string			_socketConnectionAddress;
	int					_socketConnectionPort;

	QTcpSocket* _socket;

	char _headerByte1;
	char _headerByte2;
	unsigned short _maxPacketSize;
};
