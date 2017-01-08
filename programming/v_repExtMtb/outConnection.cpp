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

#include "outConnection.h"
#include <QTime>

#define HEADER_LENGTH 6 // byte1=id1, byte2=id2, byte3+byte4=packetSize, byte5+byte6=packetsLeftToRead

COutConnection::COutConnection(const char* theConnectionAddress,int theConnectionPort,unsigned short maxPacketSize/*=250*/,char headerID1/*=59*/,char headerID2/*=57*/)
{
	_socketConnectionAddress=theConnectionAddress;
	_socketConnectionPort=theConnectionPort;
	_socket=NULL;
	_headerByte1=headerID1;
	_headerByte2=headerID2;
	_maxPacketSize=maxPacketSize;
}

COutConnection::~COutConnection()
{
	if (_socket!=NULL)
		delete _socket;
}

int COutConnection::connectToServer()
{
	_socket=new QTcpSocket();
	_socket->connectToHost(_socketConnectionAddress.c_str(),_socketConnectionPort);
	if (!_socket->waitForConnected())
	{
		delete _socket;
		_socket=NULL;
		return(-1);
	}
	return(1);
}

bool COutConnection::sendData(char* data,int dataSize)
{
	if (_socket==NULL)
		return(false);

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

char* COutConnection::receiveReplyData(int& dataSize)
{ // Returns the data size if >0, otherwise error
	if (_socket==NULL)
	{
		dataSize=-2; // error
		return(NULL);
	}
	_socket->waitForReadyRead();

	std::vector<char> receivedData;
	while (true)
	{
		std::vector<char> inDat;
		int result=_receiveSimplePacket(inDat);
		if (result<0)
		{
			dataSize=0; // error
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

bool COutConnection::_sendSimplePacket(char* packet,int packetLength,unsigned short packetsLeft)
{
	if (packetLength==0)
		return(false);
	// Insert the header:
	unsigned short s=(unsigned short)packetLength;
	char header[HEADER_LENGTH];
	header[0]=_headerByte1;
	header[1]=_headerByte2;
	((unsigned short*)(header+2))[0]=s;
	((unsigned short*)(header+2))[1]=packetsLeft;

	std::vector<char> toSend;
	for (int i=0;i<HEADER_LENGTH;i++)
		toSend.push_back(header[i]);
	for (int i=0;i<packetLength;i++)
		toSend.push_back(packet[i]);
	// Send the packet:
	int dl=_socket->write(&toSend[0],packetLength+HEADER_LENGTH);
	if (dl==packetLength+HEADER_LENGTH)
	{
		_socket->waitForBytesWritten(); // Important
		return(true);
	}
	return(false);
}

int COutConnection::_receiveSimplePacket(std::vector<char>& packet)
{ // Returns the number of packets left to read if >=0, otherwise error
	//1. Read the header and packet size:
	char headerAndSize[HEADER_LENGTH];
	int totalReceived=0;
	unsigned short startT=_getTimeInMs();
	while(totalReceived!=HEADER_LENGTH)
	{
		int nb=_socket->bytesAvailable();
		if (nb>HEADER_LENGTH-totalReceived)
			nb=HEADER_LENGTH-totalReceived;
		if (nb>0)
			_socket->read(headerAndSize+totalReceived,nb);
		totalReceived+=nb;
		if (_getTimeInMs()-startT>3000)
			break;
	}
	// 2. Check if the header is consistent:
	if (totalReceived!=HEADER_LENGTH)
		return(-1); // Error reading
	if ( (headerAndSize[0]!=_headerByte1)||(headerAndSize[1]!=_headerByte2) )
		return(-1); // Error, wrong header
	unsigned short dataLength=((unsigned short*)(headerAndSize+2))[0];
	// 3. Read the data with correct length:
	packet.clear();
	packet.resize(dataLength,0);
	totalReceived=0;
	startT=_getTimeInMs();
	while(totalReceived!=dataLength)
	{
		int nb=_socket->bytesAvailable();
		if (nb>dataLength-totalReceived)
			nb=dataLength-totalReceived;
		if (nb>0)
			_socket->read(&packet[0]+totalReceived,nb);
		totalReceived+=nb;
		if (_getTimeInMs()-startT>3000)
			break;
	}
	if (totalReceived!=dataLength)
		return(-1); // wrong size or nothing received
	return(int(((unsigned short*)(headerAndSize+2))[1]));
}

int COutConnection::_getTimeInMs()
{
	static bool first=true;
	static QTime firstTime;
	if (first)
	{
	firstTime.start();
	first=false;
	}
	return(firstTime.elapsed());
}

