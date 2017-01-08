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

#include "InConnection.h"
#include <Mmsystem.h>


#define HEADER_LENGTH 6 // byte1=id1, byte2=id2, byte3+byte4=packetSize, byte5+byte6=packetsLeftToRead

CInConnection::CInConnection(int theConnectionPort,WORD maxPacketSize/*=250*/,char headerID1/*=59*/,char headerID2/*=57*/)
{
	_socketConnectionPort=theConnectionPort;
	_socketConnectWasOk=false;
	_usingSockets=true;
	_headerByte1=headerID1;
	_headerByte2=headerID2;
	_maxPacketSize=maxPacketSize;
}

CInConnection::CInConnection(const char* thePipeName,WORD maxPacketSize/*=1024*/,char headerID1/*=59*/,char headerID2/*=57*/)
{
	_pipeName=thePipeName;
	_pipeConnectWasOk=false;
	_usingSockets=false;
	_headerByte1=headerID1;
	_headerByte2=headerID2;
	_maxPacketSize=maxPacketSize;
}

CInConnection::~CInConnection()
{
	if (_usingSockets)
	{
		if (_socketConnectWasOk)
		{
			shutdown(_socketClient,SD_BOTH);
			closesocket(_socketServer);
			WSACleanup();
		}
	}
	else
	{ // pipes
		if (_pipeConnectWasOk)
			DisconnectNamedPipe(_pipeThePipe);
	}
}

bool CInConnection::connectToClient()
{
	if (_usingSockets)
	{
		// 1. connect to port:
		if (WSAStartup(0x101,&_socketWsaData)!=0)
			return(false);	 // WSAStartup failed.
		_socketLocal.sin_family=AF_INET;
		_socketLocal.sin_addr.s_addr=INADDR_ANY;
		_socketLocal.sin_port=htons((u_short)_socketConnectionPort);
		_socketServer=socket(AF_INET,SOCK_STREAM,0);
		if (_socketServer==INVALID_SOCKET)
			return(false); // socket failed.
		if (bind(_socketServer,(sockaddr*)&_socketLocal,sizeof(_socketLocal))!=0)
			return(false); // bind failed.
		if (listen(_socketServer,10)!=0)
			return(false); // listen failed.
		// 2. accept client:
		sockaddr_in from;
		int fromlen=sizeof(from);
		_socketClient=accept(_socketServer,(struct sockaddr*)&from,&fromlen);
		_socketConnectedMachineIP=inet_ntoa(from.sin_addr);
		_socketTimeOut.tv_sec=1; // 1 second max between successive receive for the same packet
		_socketTimeOut.tv_usec=0;
		FD_ZERO(&_socketTheSet); 
		FD_SET(_socketClient,&_socketTheSet); 
		_socketConnectWasOk=true;
		return(true);
	}
	else
	{ // pipes
		_pipeThePipe=CreateNamedPipe(_pipeName.c_str(),PIPE_ACCESS_DUPLEX,PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,_maxPacketSize,_maxPacketSize,NMPWAIT_USE_DEFAULT_WAIT,NULL);
		if (INVALID_HANDLE_VALUE!=_pipeThePipe)
		{
			bool reallyConnected=true;
			if (ConnectNamedPipe(_pipeThePipe,NULL)==0)
			{
				DWORD err=GetLastError();
				if (err!=ERROR_PIPE_CONNECTED)
					reallyConnected=false;
			}
			if (reallyConnected)
			{
				_pipeConnectWasOk=true;
				return(true);
			}
		}
		return(false);
	}
}

char* CInConnection::receiveData(int& dataSize)
{ // Returns the data size if >0, 0=we had a read time out, -1=we have an error
	if (_usingSockets)
	{
		if (!_socketConnectWasOk)
		{
			dataSize=-1; // error
			return(NULL);
		}
	}
	else
	{ // pipes
		if (!_pipeConnectWasOk)
		{
			dataSize=-1; // error
			return(NULL);
		}
	}
	std::vector<char> receivedData;
	while (true)
	{
		std::vector<char> inDat;
		int result=_receiveSimplePacket(inDat);
		if (result<0)
		{
			dataSize=result+1; // error or read time-out
			return(NULL);
		}
		receivedData.insert(receivedData.end(),inDat.begin(),inDat.end());
		if (result==0)
		{ // success
			dataSize=int(receivedData.size());
			char* retBuff=new char[dataSize];
			for (int i=0;i<dataSize;i++)
				retBuff[i]=receivedData[i];
			return(retBuff);
		}
	}
}

bool CInConnection::replyToReceivedData(char* data,int dataSize)
{
	if (_usingSockets)
	{
		if (!_socketConnectWasOk)
			return(false);
	}
	else
	{ // pipes
		if (!_pipeConnectWasOk)
			return(false);
	}
	if (dataSize==0)
		return(false);

	// In Following we make sure we don't send too big packets (we might send the data in several packets)
	int packetCount=0;
	int s=dataSize;
	while (s!=0)
	{
		packetCount++;
		if (s>_maxPacketSize-HEADER_LENGTH)
			s-=_maxPacketSize-HEADER_LENGTH;
		else
			s=0;
	}

	s=dataSize;
	int ptr=0;
	while (s!=0)
	{
		packetCount--;
		int sizeToSend=s;
		if (s>_maxPacketSize-HEADER_LENGTH)
			sizeToSend=_maxPacketSize-HEADER_LENGTH;
		s-=sizeToSend;
		if (!_sendSimplePacket(data+ptr,sizeToSend,packetCount))
			return(false);
		ptr+=sizeToSend;
	}
	return(true);
}

std::string CInConnection::getConnectedMachineIP()
{
	if (!_socketConnectWasOk)
		return("NONE (reception line is not open)");
	return(_socketConnectedMachineIP);
}

bool CInConnection::_sendSimplePacket(char* packet,int packetLength,WORD packetsLeft)
{
	if (packetLength==0)
		return(false);
	// Insert the header:
	WORD s=WORD(packetLength);
	char header[HEADER_LENGTH];
	header[0]=_headerByte1;
	header[1]=_headerByte2;
	((WORD*)(header+2))[0]=s;
	((WORD*)(header+2))[1]=packetsLeft;

	std::vector<char> toSend;
	for (int i=0;i<HEADER_LENGTH;i++)
		toSend.push_back(header[i]);
	for (int i=0;i<packetLength;i++)
		toSend.push_back(packet[i]);
	// Send the packet:
	if (_usingSockets)
		return(send(_socketClient,&toSend[0],packetLength+HEADER_LENGTH,0)==packetLength+HEADER_LENGTH);
	else
	{ // pipes:
		DWORD bytesWritten=0;
		return( WriteFile(_pipeThePipe,&toSend[0],packetLength+HEADER_LENGTH,&bytesWritten,NULL)&&(packetLength+HEADER_LENGTH==bytesWritten) );
	}
}

int CInConnection::_receiveSimplePacket(std::vector<char>& packet)
{ 
	if (_usingSockets)
	{ // Returns the number of packets left to read if >=0, -2=error, -1=select time out
		_socketTimeOut.tv_sec=10; // 1 second max between successive receive for the same packet
		_socketTimeOut.tv_usec=0;
		FD_ZERO(&_socketTheSet); 
		FD_SET(_socketClient,&_socketTheSet);
		int selectResult=select(0,&_socketTheSet,NULL,NULL,&_socketTimeOut);
		if (selectResult==1)
		{
			//1. Read the header and packet size:
			char headerAndSize[HEADER_LENGTH];
			int totalReceived=0;
			DWORD startT=timeGetTime();
			while(totalReceived!=HEADER_LENGTH)
			{
				int nb=recv(_socketClient,headerAndSize+totalReceived,HEADER_LENGTH-totalReceived,0);
				if (nb<1)
					break;
				totalReceived+=nb;
				if (timeGetTime()-startT>3000)
					break;
			}
			// 2. Check if the header is consistent:
			if (totalReceived!=HEADER_LENGTH)
				return(-2); // Error reading
			if ( (headerAndSize[0]!=_headerByte1)||(headerAndSize[1]!=_headerByte2) )
				return(-2); // Error, wrong header
			WORD dataLength=((WORD*)(headerAndSize+2))[0];
			// 3. Read the data with correct length:
			packet.clear();
			packet.resize(dataLength,0);
			totalReceived=0;
			startT=timeGetTime();
			while(totalReceived!=dataLength)
			{
				int nb=recv(_socketClient,&packet[0]+totalReceived,dataLength-totalReceived,0);
				if (nb<1)
					break;
				totalReceived+=nb;
				if (timeGetTime()-startT>3000)
					break;
			}
			if (totalReceived!=dataLength)
				return(-2); // wrong size or nothing received
			return(int(((WORD*)(headerAndSize+2))[1]));
		}
		if (selectResult==0)
			return(-1);
		return(-2);
	}
	else
	{ // pipes
		//1. Read the header and packet size:
		char headerAndSize[HEADER_LENGTH];
		int totalReceived=0;
		DWORD startT=timeGetTime();
		while(totalReceived!=HEADER_LENGTH)
		{
			int nb=0;
			DWORD bytesRead=0;
			if (ReadFile(_pipeThePipe,headerAndSize+totalReceived,HEADER_LENGTH-totalReceived,&bytesRead,NULL))
				nb=int(bytesRead);
			if (nb<1)
				break;
			totalReceived+=nb;
			if (timeGetTime()-startT>3000)
				break;
		}
		// 2. Check if the header is consistent:
		if (totalReceived!=HEADER_LENGTH)
			return(-2); // Error reading
		if ( (headerAndSize[0]!=_headerByte1)||(headerAndSize[1]!=_headerByte2) )
			return(-2); // Error, wrong header
		WORD dataLength=((WORD*)(headerAndSize+2))[0];
		// 3. Read the data with correct length:
		packet.clear();
		packet.resize(dataLength,0);
		totalReceived=0;
		startT=timeGetTime();
		while(totalReceived!=dataLength)
		{
			int nb=0;
			DWORD bytesRead=0;
			if (ReadFile(_pipeThePipe,&packet[0]+totalReceived,dataLength-totalReceived,&bytesRead,NULL))
				nb=int(bytesRead);
			if (nb<1)
				break;
			totalReceived+=nb;
			if (timeGetTime()-startT>3000)
				break;
		}
		if (totalReceived!=dataLength)
			return(-2); // wrong size or nothing received
		return(int(((WORD*)(headerAndSize+2))[1]));
	}
}
