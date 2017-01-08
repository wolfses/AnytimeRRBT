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
#include "SerialPortContainer.h"
#include "v_rep_internal.h"

CSerialPortContainer::CSerialPortContainer()
{

}

CSerialPortContainer::~CSerialPortContainer()
{
	closeAllPorts();
}

void CSerialPortContainer::closeAllPorts()
{ // closes all ports of this instance
	for (int i=0;i<int(_openedSerialPorts.size());i++)
		delete _openedSerialPorts[i];
	_openedSerialPorts.clear();
}

int CSerialPortContainer::addOpenedPort(CSerialPort* port)
{
	_openedSerialPorts.push_back(port);
	return(port->getPortHandle());
}

CSerialPort* CSerialPortContainer::getPortFromPortNumber_backwardCompatibility(int portNb)
{ // for backward compatibility (10/04/2012)
	for (int i=0;i<int(_openedSerialPorts.size());i++)
	{
		if (_openedSerialPorts[i]->getPortNumber_backwardCompatibility()==portNb)
			return(_openedSerialPorts[i]);
	}
	return(NULL);
}


CSerialPort* CSerialPortContainer::getPortFromHandle(int portHandle)
{
	for (int i=0;i<int(_openedSerialPorts.size());i++)
	{
		if (_openedSerialPorts[i]->getPortHandle()==portHandle)
			return(_openedSerialPorts[i]);
	}
	return(NULL);
}

void CSerialPortContainer::closePortFromHandle(int portHandle)
{
	for (int i=0;i<int(_openedSerialPorts.size());i++)
	{
		if (_openedSerialPorts[i]->getPortHandle()==portHandle)
		{
			delete _openedSerialPorts[i];
			_openedSerialPorts.erase(_openedSerialPorts.begin()+i);
			break;
		}
	}
}

void CSerialPortContainer::simulationEnded()
{ // Close only ports that were opened through a script:
	for (int i=0;i<int(_openedSerialPorts.size());i++)
	{
		if (_openedSerialPorts[i]->wasOpenedFromAScript())
		{
			closePortFromHandle(_openedSerialPorts[i]->getPortHandle());
			i=-1; // We have to restart the loop from the beginning!
		}
	}
}
