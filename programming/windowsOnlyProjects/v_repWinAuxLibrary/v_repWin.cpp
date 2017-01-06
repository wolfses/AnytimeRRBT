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
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#include "stdafx.h"
#include "v_repWin.h"
#include <vector>
#include "SerialPort.h"

std::vector<CSerialPort*> allOpenedSerialPorts;

VREPWIN_DLLEXPORT void vrepWin_showConsole(char show)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (show!=0)
		ShowWindow(GetConsoleWindow(),SW_SHOW);
	else
		ShowWindow(GetConsoleWindow(),SW_HIDE);
}

VREPWIN_DLLEXPORT DWORD_PTR vrepWin_setThreadAffinity(DWORD_PTR affinity)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DWORD_PTR oldAffinity=SetThreadAffinityMask(GetCurrentThread(),affinity);
	return(oldAffinity);
}

VREPWIN_DLLEXPORT DWORD_PTR vrepWin_launchThread(AFX_THREADPROC_TT startAddress,int priority,DWORD_PTR affinityMask)
{ // priority==0 --> lowest, priority==1 --> normal, priority==2 --> highest. if affinityMask==0, we don't do anything
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int prio=THREAD_PRIORITY_NORMAL;
	if (priority==0)
		prio=THREAD_PRIORITY_LOWEST;
	if (priority==2)
		prio=THREAD_PRIORITY_HIGHEST;

	CWinThread* theWinThreadObject=AfxBeginThread(startAddress,NULL,prio,0,0,NULL);
	if (affinityMask!=0)
	{
		DWORD_PTR oldAffinity=SetThreadAffinityMask(theWinThreadObject->m_hThread,affinityMask);
		return(oldAffinity);
	}
	return(0);
}

VREPWIN_DLLEXPORT DWORD vrepWin_getCurrentThreadId()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return(GetCurrentThreadId());
}

VREPWIN_DLLEXPORT void vrepWin_switchThread()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	SwitchToThread();
}

VREPWIN_DLLEXPORT void vrepWin_sleep(int ms)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	Sleep(ms);
}

CSerialPort* _getOpenedPortFromHandle(int portHandle)
{
	for (int i=0;i<int(allOpenedSerialPorts.size());i++)
	{
		if (allOpenedSerialPorts[i]->getPortHandle()==portHandle)
			return(allOpenedSerialPorts[i]);
	}
	return(NULL);
}

VREPWIN_DLLEXPORT int vrepWin_serialOpen(char* portString,int baudrate)
{
	CSerialPort* p=new CSerialPort();
	if (p->Open(portString,baudrate)!=0)
	{
		allOpenedSerialPorts.push_back(p);
		return(p->getPortHandle());
	}
	delete p;
	return(-1);
}

VREPWIN_DLLEXPORT char vrepWin_serialClose(int portHandle)
{
	for (int i=0;i<int(allOpenedSerialPorts.size());i++)
	{
		if (allOpenedSerialPorts[i]->getPortHandle()==portHandle)
		{
			CSerialPort* p=allOpenedSerialPorts[i];
			p->Close();
			delete p;
			allOpenedSerialPorts.erase(allOpenedSerialPorts.begin()+i);
			return(1);
		}
	}
	return(0);
}

VREPWIN_DLLEXPORT int vrepWin_serialWrite(int portHandle,const char* buffer,int size)
{
	CSerialPort* p=_getOpenedPortFromHandle(portHandle);
	if (p==NULL)
		return(0);
	return(p->SendData(buffer,size));
}

VREPWIN_DLLEXPORT int vrepWin_serialCheck(int portHandle)
{
	CSerialPort* p=_getOpenedPortFromHandle(portHandle);
	if (p==NULL)
		return(0);
	return(p->ReadDataWaiting());
}

VREPWIN_DLLEXPORT int vrepWin_serialRead(int portHandle,char* buffer,int maxSize)
{
	CSerialPort* p=_getOpenedPortFromHandle(portHandle);
	if (p==NULL)
		return(0);
	return(p->ReadData(buffer,maxSize));
}

VREPWIN_DLLEXPORT void vrepWin_cleanUp()
{
	for (int i=0;i<int(allOpenedSerialPorts.size());i++)
		delete allOpenedSerialPorts[i];
}

