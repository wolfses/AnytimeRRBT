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
#include "SerialPort.h"

#ifdef WIN_VREP
#include "auxLib.h"
#else
#include "serial_mac_linux.h"
#endif

CSerialPort::CSerialPort(bool wasOpenedFromAScript)
{
	_portHandle=-1;
	_portNumber_backwardCompatibility=-1;
	_wasOpenedFromAScript=wasOpenedFromAScript;
}

CSerialPort::~CSerialPort()
{
	close();
}

void CSerialPort::setPortNumber_backwardCompatibility(int nb)
{
	_portNumber_backwardCompatibility=nb;
}

int CSerialPort::getPortNumber_backwardCompatibility()
{
	return(_portNumber_backwardCompatibility);
}

int CSerialPort::getPortHandle()
{
	return(_portHandle);
}

bool CSerialPort::wasOpenedFromAScript()
{
	return(_wasOpenedFromAScript);
}

bool CSerialPort::open(const char* portString,int nBaud)
{
#ifdef WIN_VREP
	_portHandle=CAuxLib::vrepWin_serialOpen((char*)portString,nBaud);
	return (_portHandle>=0);
#else
	_portHandle=serialOpen(portString,nBaud);
	return (_portHandle>=0);
#endif
}

bool CSerialPort::close()
{
#ifdef WIN_VREP
	return(CAuxLib::vrepWin_serialClose(_portHandle)!=0);
#else
	return(serialClose(_portHandle)==0);
#endif
}

int CSerialPort::sendData(const char *buffer,int size)
{
#ifdef WIN_VREP
	return(CAuxLib::vrepWin_serialWrite(_portHandle,buffer,size));
#else
	return(serialWrite(_portHandle,buffer,size));
#endif
}


int CSerialPort::readDataWaiting( void )
{
#ifdef WIN_VREP
	return(CAuxLib::vrepWin_serialCheck(_portHandle));
#else
	return(serialCheck(_portHandle));
#endif
}

int CSerialPort::readData(void *buffer,int limit)
{
#ifdef WIN_VREP
	return(CAuxLib::vrepWin_serialRead(_portHandle,(char*)buffer,limit));
#else
	return(serialRead(_portHandle,(char*)buffer,limit));
#endif
}

