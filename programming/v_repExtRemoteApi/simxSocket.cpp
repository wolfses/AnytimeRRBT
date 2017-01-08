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

#include "simxSocket.h"
#include "simxUtils.h"
#include "v_repLib.h"
#include <iostream>
#include <sstream>

bool CSimxSocket::useAlternateSocketRoutines=false;


CSimxSocket::CSimxSocket(int portNb,bool continuousService,bool simulationOnly,bool debug,int maxPacketSize,bool waitForTriggerFunctionAuthorized)
{
	_commThreadLaunched=false;
	_commThreadEnded=true;
	clientIsConnected=false;
	otherSideIsBigEndian=false;
	_portNb=portNb;
	_simulationOnly=simulationOnly;
	_continuousService=continuousService;
	_debug=debug;
	_maxPacketSize=maxPacketSize;
	_auxConsoleHandle=-1;
	_waitForTrigger=true;
	_waitForTriggerFunctionEnabled=false;
	_waitForTriggerFunctionAuthorized=waitForTriggerFunctionAuthorized;
	if (debug)
	{
		int options=4;
		if (_simulationOnly)
			options|=1;
		else
			options|=16;
		std::stringstream strStream;
		strStream << "Remote Api monitor on port " << _portNb;
		_auxConsoleHandle=simAuxiliaryConsoleOpen(strStream.str().c_str(),100,options,NULL,NULL,NULL,NULL);
	}

	_receivedCommands=new CSimxContainer(true);
	_dataToSend=new CSimxContainer(false);
	_lockLevel=0;
#ifdef _WIN32
	_mutex=CreateMutex(0,FALSE,0);
	_mutexAux=CreateMutex(0,FALSE,0);
#elif defined (__linux) || defined (__APPLE__)
	pthread_mutex_init(&_mutex, NULL);
	pthread_mutex_init(&_mutexAux, NULL);
#endif /* __linux || __APPLE__ */
}

CSimxSocket::~CSimxSocket()
{
	_stop();
	if (_debug)
		simAuxiliaryConsoleClose(_auxConsoleHandle);
	delete _receivedCommands;
	delete _dataToSend;
#ifdef _WIN32
	CloseHandle(_mutexAux);
	CloseHandle(_mutex);
#elif defined (__linux) || defined (__APPLE__)
	pthread_mutex_destroy(&_mutexAux);
	pthread_mutex_destroy(&_mutex);
#endif /* __linux || __APPLE__ */
}

void CSimxSocket::setWaitForTrigger(bool w)
{
	if (_waitForTriggerFunctionEnabled)
		_waitForTrigger=w;
}

bool CSimxSocket::getWaitForTrigger()
{
	// make sure we override the external trigger if the user presses stop:
	int ver;
	simGetIntegerParameter(sim_intparam_program_version,&ver);
	int req;
	simGetIntegerParameter(sim_intparam_stop_request_counter,&req);
	if ((req!=_stopSimulationRequestCounter)&&(ver>=30006)) // stop in stepped mode only supported from V-REP 3.0.6 on!
	{
		_waitForTrigger=false;
		_waitForTriggerFunctionEnabled=false;
	}
	return(_waitForTriggerFunctionEnabled&&_waitForTrigger);
}

void CSimxSocket::setWaitForTriggerEnabled(bool e)
{
	if (_waitForTriggerFunctionAuthorized)
	{
		_waitForTriggerFunctionEnabled=e;
		_waitForTrigger=e;
		// initialize the stop request counter:
		simGetIntegerParameter(sim_intparam_stop_request_counter,&_stopSimulationRequestCounter);
	}
}

bool CSimxSocket::getWaitForTriggerEnabled()
{
	return(_waitForTriggerFunctionAuthorized&&_waitForTriggerFunctionEnabled);
}

bool CSimxSocket::getWaitForTriggerAuthorized()
{
	return(_waitForTriggerFunctionAuthorized);
}

#ifdef _WIN32
DWORD WINAPI CSimxSocket::_staticThreadProc(LPVOID arg)
{
	reinterpret_cast<CSimxSocket*>(arg)->_run();
	return(0);
}
#endif /* _WIN32 */

#if defined (__linux) || defined (__APPLE__)
void* CSimxSocket::_staticThreadProc(void *arg)
{
	return(reinterpret_cast<CSimxSocket*>(arg)->_run());
}
#endif /* __linux || __APPLE__ */

void CSimxSocket::start()
{
	if (!_commThreadLaunched)
	{
		_lastReceivedMessage_clientVersion=-1;
		_lastReceivedMessage_time=0;
		_lastSentMessage_time=0;
		_successiveReception_time=0;
		_lastReceivedMessage_cmdCnt=0;
		_lastSentMessage_cmdCnt=0;

		_commThreadEnded=false;
#ifdef _WIN32
		CreateThread(NULL,0,&CSimxSocket::_staticThreadProc,this,THREAD_PRIORITY_NORMAL,NULL);
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
		pthread_create(&_theThread,NULL,&CSimxSocket::_staticThreadProc,this);
#endif /* __linux || __APPLE__ */
		while ( (!_commThreadLaunched)&&(!_commThreadEnded) )
			Sleep(10);
	}
}

void CSimxSocket::_stop()
{
	// Terminate the communication thread if needed:
	if (_commThreadLaunched)
	{
		// connection might already have been destroyed!!!
		// connection->stopWaitingForConnection();

		_commThreadLaunched=false; // signal the thread we wanna end

		// First try if we can bring the communication thread to an end:
		DWORD startT=getTimeInMs();
		while (getTimeDiffInMs(startT)<20)
		{
			Sleep(1);
			if (_commThreadEnded)
				break; // this usually happens when a client was connected and the communication was running
		}

		if (!_commThreadEnded)
		{	// no, the communication thread is probably waiting for a connection (blocked while accepting)

			// Make a fake connection to this socket, to unblock it (the thread might be trapped in the "accept" function)
			if (_portNb>=0)
			{ // sockets
				struct hostent *hp;
				unsigned int addr;
				struct sockaddr_in _socketServer;
#ifdef _WIN32
				WSADATA	_socketWsaData;
				if (WSAStartup(0x101,&_socketWsaData)==0)
#endif // _WIN32
				{
					_SOCKET _socketConn=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
					if(_socketConn!=INVALID_SOCKET)
					{
						addr=inet_addr("127.0.0.1");
						hp=gethostbyaddr((char*)&addr,sizeof(addr),AF_INET);
						if(hp!=NULL)
						{
							_socketServer.sin_addr.s_addr=*((unsigned long*)hp->h_addr);
							_socketServer.sin_family=AF_INET;
							_socketServer.sin_port=htons(_portNb);
							connect(_socketConn,(struct sockaddr*)&_socketServer,sizeof(_socketServer));
						}
#ifdef _WIN32
						closesocket(_socketConn);
#elif defined (__linux) || defined (__APPLE__)
						close(_socketConn);
#endif // _WIN32
					}
#ifdef _WIN32
					WSACleanup();
#endif // _WIN32
				}
			}

			// Now wait until the thread signals that it ended (or we reached a timeout: that probably happens when a firewall forbids self-connections)
			startT=getTimeInMs();
			while (getTimeDiffInMs(startT)<1000)
			{
				Sleep(1);
				if (_commThreadEnded)
					break; // ok, the thread ended
			}

		}

	}

	// Do some other clean-up:
	_receivedCommands->clearAll();
	_dataToSend->clearAll();
}

void CSimxSocket::getInfo(int info[5])
{
	info[0]=_lastReceivedMessage_time;
	info[1]=_lastSentMessage_time;
	info[2]=_successiveReception_time;
	info[3]=_lastReceivedMessage_cmdCnt;
	info[4]=_lastSentMessage_cmdCnt;
}

int CSimxSocket::getClientVersion()
{
	return(_lastReceivedMessage_clientVersion);
}

int CSimxSocket::getStatus()
{
	int retVal=0;
	if (_commThreadLaunched)
		retVal|=1;
	if (clientIsConnected)
		retVal|=2;
	return(retVal);
}

int CSimxSocket::getPortNb()
{
	return(_portNb);
}

bool CSimxSocket::getActiveOnlyDuringSimulation()
{
	return(_simulationOnly);
}

bool CSimxSocket::getContinuousService()
{
	return(_continuousService);
}

bool CSimxSocket::getDebug()
{
	return(_debug);
}

int CSimxSocket::getMaxPacketSize()
{
	return(_maxPacketSize);
}

void CSimxSocket::_simpleLock(MUTEX_HANDLE_X mutex)
{
#ifdef _WIN32
	while (WaitForSingleObject(mutex,INFINITE)!=WAIT_OBJECT_0)
		Sleep(1);
#elif defined (__linux) || defined (__APPLE__)
	while (pthread_mutex_lock(mutex)==-1)
		Sleep(1);
#endif
}

void CSimxSocket::_simpleUnlock(MUTEX_HANDLE_X mutex)
{
#ifdef _WIN32
	ReleaseMutex(mutex);
#elif defined (__linux) || defined (__APPLE__)
	pthread_mutex_unlock(mutex);
#endif
}

void CSimxSocket::_lock()
{
#ifdef _WIN32
	_simpleLock(_mutexAux);
	if ( (GetCurrentThreadId()==_lockThreadId) && (_lockLevel>0) )
	{ // Already locked by this thread
		_lockLevel++;
		_simpleUnlock(_mutexAux);
		return;
	}
	// First level lock
	_simpleUnlock(_mutexAux);
	_simpleLock(_mutex);
	_simpleLock(_mutexAux);
	_lockThreadId=GetCurrentThreadId();
	_lockLevel=1;
	_simpleUnlock(_mutexAux);
#elif defined (__linux) || defined (__APPLE__)
	_simpleLock(&_mutexAux);
	if ( (pthread_self()==_lockThreadId) && (_lockLevel>0) )
	{ // Already locked by this thread
		_lockLevel++;
		_simpleUnlock(&_mutexAux);
		return;
	}
	// First level lock
	_simpleUnlock(&_mutexAux);
	_simpleLock(&_mutex);
	_simpleLock(&_mutexAux);
	_lockThreadId=pthread_self();
	_lockLevel=1;
	_simpleUnlock(&_mutexAux);
#endif
}

void CSimxSocket::_unlock()
{
#ifdef _WIN32
	_simpleLock(_mutexAux);
	_lockLevel--;
	if (_lockLevel==0)
	{
		_simpleUnlock(_mutexAux);
		_simpleUnlock(_mutex);
	}
	else
		_simpleUnlock(_mutexAux);
#elif defined (__linux) || defined (__APPLE__)
	_simpleLock(&_mutexAux);
	_lockLevel--;
	if (_lockLevel==0)
	{
		_simpleUnlock(&_mutexAux);
		_simpleUnlock(&_mutex);
	}
	else
		_simpleUnlock(&_mutexAux);
#endif
}

void CSimxSocket::_executeCommands()
{
	_lock();
	_receivedCommands->executeCommands(_dataToSend,this);
	_unlock();
}

void* CSimxSocket::_run()
{
	_commThreadLaunched=true;
	connection=NULL;
	while (_commThreadLaunched)
	{
		connection=new CInConnection(_portNb,_maxPacketSize,useAlternateSocketRoutines);
		if (_debug)
		{ // We do this in a funny way since we should not access V-REP from a thread not created in V-REP!
			_lock();
			_textToPrintToConsole.push_back("connecting to client...\n");
			_unlock();
		}
//printf("Trying to connect...\n");
		if (connection->connectToClient())
		{
//printf("Connected!\n");
			clientIsConnected=true;
			_lock();
			_receivedCommands->clearAll();
			_dataToSend->clearAll();
			_unlock();
			int _lastLastReceivedMessage_time=0;
			if (_debug)
			{ // We do this in a funny way since we should not access V-REP from a thread not created in V-REP!
				_lock(); // we are not locked here!
				_textToPrintToConsole.push_back("connected to client.\n");
				_unlock();
			}
			while (_commThreadLaunched)
			{
				int dataSize;
//printf("Trying to read...\n");
				char* data=connection->receiveMessage(dataSize);
				if (dataSize>0)
				{ // we received some data. The server ALWAYS replies!
//printf("Read successful!\n");
					_lastReceivedMessage_time=getTimeInMs();
					_successiveReception_time=_lastReceivedMessage_time-_lastLastReceivedMessage_time;
					_lastLastReceivedMessage_time=_lastReceivedMessage_time;

					otherSideIsBigEndian=connection->isOtherSideBigEndian();
					// a) check the CRC:
					WORD crc=littleEndianWordConversion(((WORD*)(data+simx_headeroffset_crc))[0],otherSideIsBigEndian);
					_lock();
					_lastReceivedMessage_cmdCnt=0;
					bool killConnectionCommand=false;
					// CRC calculation represents a bottleneck for large transmissions, and is anyway not needed with tcp or shared memory transmissions
					// if (getCRC(data+2,dataSize-2)==crc)
					if (true)
					{
						_lastReceivedMessage_clientVersion=data[simx_headeroffset_version];
						_receivedCommands->setOtherSideIsBigEndian(otherSideIsBigEndian);
						_dataToSend->setOtherSideIsBigEndian(otherSideIsBigEndian);
						int messageID=littleEndianIntConversion(((int*)(data+simx_headeroffset_message_id))[0],otherSideIsBigEndian);
						int timeStamp=littleEndianIntConversion(((int*)(data+simx_headeroffset_client_time))[0],otherSideIsBigEndian);
						_receivedCommands->setMessageID(messageID);
						_receivedCommands->setDataTimeStamp(timeStamp);
						if (_debug)
						{ // We do this in a funny way since we should not access V-REP from a thread not created in V-REP!
							// resources are already locked here, no need to lock a second time!
							std::stringstream strStream;
							strStream << "data received: " << dataSize << " bytes (message ID = " << messageID << ")\n";
							_textToPrintToConsole.push_back(strStream.str());
						}
						int off=SIMX_HEADER_SIZE;
						while (off<dataSize)
						{
							int cmdSize=littleEndianIntConversion(((int*)(data+off+simx_cmdheaderoffset_mem_size))[0],otherSideIsBigEndian);
							int fullCmdSize=littleEndianIntConversion(((int*)(data+off+simx_cmdheaderoffset_full_mem_size))[0],otherSideIsBigEndian);
							if (cmdSize!=fullCmdSize)
							{
								char* fullCommand=_receivedCommands->addPartialCommand(data+off,otherSideIsBigEndian);
								if (fullCommand!=NULL)
								{
									int localCmdSize=littleEndianIntConversion(((int*)(fullCommand+simx_cmdheaderoffset_mem_size))[0],otherSideIsBigEndian);
									int cmd=littleEndianIntConversion(((int*)(fullCommand+simx_cmdheaderoffset_cmd))[0],otherSideIsBigEndian);
									killConnectionCommand=(cmd==simx_cmd_kill_connection);
									BYTE options=fullCommand[simx_cmdheaderoffset_status]; // bit0 set: do not overwrite this command!
									WORD delayOrSplit=littleEndianWordConversion(((WORD*)(fullCommand+simx_cmdheaderoffset_delay_or_split))[0],otherSideIsBigEndian);
									CSimxCmd* newCmd=new CSimxCmd(cmd,delayOrSplit,localCmdSize-SIMX_SUBHEADER_SIZE,fullCommand+SIMX_SUBHEADER_SIZE);
									_receivedCommands->addCommand(newCmd,options&1);
									delete[] fullCommand;
								}
							}
							else
							{
								int cmd=littleEndianIntConversion(((int*)(data+off+simx_cmdheaderoffset_cmd))[0],otherSideIsBigEndian);
								killConnectionCommand=(cmd==simx_cmd_kill_connection);
								BYTE options=data[off+simx_cmdheaderoffset_status]; // bit0 set: do not overwrite this command!
								WORD delayOrSplit=littleEndianWordConversion(((WORD*)(data+off+simx_cmdheaderoffset_delay_or_split))[0],otherSideIsBigEndian);
								CSimxCmd* newCmd=new CSimxCmd(cmd,delayOrSplit,cmdSize-SIMX_SUBHEADER_SIZE,data+off+SIMX_SUBHEADER_SIZE);
								_receivedCommands->addCommand(newCmd,options&1);
							}
							off+=cmdSize;
							_lastReceivedMessage_cmdCnt++;
						}
					}
					else
					{
						if (_debug)
						{ // We do this in a funny way since we should not access V-REP from a thread not created in V-REP!
							// resources are already locked here, no need to lock a second time!
							_textToPrintToConsole.push_back("data received: error (crc failed)\n");
						}
					}
					delete[] data;
					// Prepare the reply:
					int streamCmdCnt=_dataToSend->getStreamCommandCount();
					std::vector<char> replyData;
					_lastSentMessage_cmdCnt=_dataToSend->getDataString(replyData,otherSideIsBigEndian);
					_lastSentMessage_cmdCnt+=_receivedCommands->getDataStringOfSplitOrGradualCommands(replyData,otherSideIsBigEndian);
					int messageIdToSend=_dataToSend->getMessageID();
					_dataToSend->clearAll();
					_unlock();
					// send the reply, but first add the CRC:
					// CRC calculation represents a bottleneck for large transmissions, and is anyway not needed with tcp or shared memory transmissions
					// crc=getCRC(&replyData[2],int(replyData.size()-2));
					crc=0;
					((WORD*)&replyData[simx_headeroffset_crc])[0]=littleEndianWordConversion(crc,otherSideIsBigEndian);

//printf("Trying to write...\n");
					if (!connection->replyToReceivedMessage(&replyData[0],(int)replyData.size()))
					{
//printf("Write NOT successful!\n");
						if (_debug)
						{ // We do this in a funny way since we should not access V-REP from a thread not created in V-REP!
							_lock(); // important to lock resources!
							_textToPrintToConsole.push_back("failed sending reply.\n");
							_unlock();
						}
						break;
					}
					else
					{
//printf("Write successful!\n");
						_lastSentMessage_time=getTimeInMs();
						if (_debug)
						{ // We do this in a funny way since we should not access V-REP from a thread not created in V-REP!
							_lock(); // important to lock resources!
							std::stringstream strStream;
							strStream << "reply sent: " << replyData.size() << " bytes (message ID = " << messageIdToSend << ", stream cmd cnt = " << streamCmdCnt << ")\n";
							_textToPrintToConsole.push_back(strStream.str());
							_unlock();
						}
					}
					if (killConnectionCommand)
						break; // since 13/12/2013
				}
				else
				{
//printf("Read NOT successful!\n");
					// commented on 13/12/2013 
					// if (dataSize==-1)
					{
						if (_debug)
						{ // We do this in a funny way since we should not access V-REP from a thread not created in V-REP!
							_lock(); // important to lock resources!
							_textToPrintToConsole.push_back("error while receiving data.\n");
							_unlock();
						}
						break;
					}
				}
			}
			if (_debug)
			{ // We do this in a funny way since we should not access V-REP from a thread not created in V-REP!
				_lock(); // important to lock resources!
				_textToPrintToConsole.push_back("disconnected from client.\n");
				_unlock();
			}
			_lock();
			_receivedCommands->clearAll();
			_unlock();
			clientIsConnected=false;
			_waitForTrigger=true;
			_waitForTriggerFunctionEnabled=false;

		}
		else
		{
//printf("Failed connecting!\n");
			Sleep(2000); // 13/12/2013
			if (_debug)
			{ // We do this in a funny way since we should not access V-REP from a thread not created in V-REP!
				_lock(); // important to lock resources!
				_textToPrintToConsole.push_back("failed connecting to client.\n");
				_unlock();
			}
		}
		delete connection;
		connection=NULL;
		Sleep(500); // important, otherwise it reconnects directly. 600 is already too much on some systems somehow. To fix.
	}

	_commThreadLaunched=false;
	_commThreadEnded=true; // confirmation that is needed
	return(NULL);
}

void CSimxSocket::addErrorString(const char* err)
{
	if (_last50Errors.size()>=50)
		_last50Errors.erase(_last50Errors.begin());
	_last50Errors.push_back(err);
}

char* CSimxSocket::getAndClearFullErrorStringBuffer(int& size)
{
	size_t letters=0;
	for (unsigned int i=0;i<_last50Errors.size();i++)
		letters+=_last50Errors[i].length()+1;
	char* retBuff=new char[4+letters];
	size=int(4+letters);
	((int*)retBuff)[0]=int(_last50Errors.size());
	size_t off=4;
	for (unsigned int i=0;i<_last50Errors.size();i++)
	{
		for (unsigned int j=0;j<_last50Errors[i].length();j++)
			retBuff[off+j]=_last50Errors[i][j];
		off+=_last50Errors[i].length();
		retBuff[off]=0;
		off++;
	}
	_last50Errors.clear();
	return(retBuff);
}

void CSimxSocket::thereWasARequestToCallTheMainScript()
{
	_executeCommands();
}

void CSimxSocket::instancePass()
{
// 3/3/2014
//	if (!_simulationOnly)
	{
		int simState=simGetSimulationState();
		if ((simState&sim_simulation_advancing)==0)
			_executeCommands();
	}

	if (_debug)
	{ // we should never access the V-REP API from a thread not created in V-REP!
		_lock();
		for (unsigned int i=0;i<_textToPrintToConsole.size();i++)
			simAuxiliaryConsolePrint(_auxConsoleHandle,_textToPrintToConsole[i].c_str());
		_textToPrintToConsole.clear();
		_unlock();
	}
}
