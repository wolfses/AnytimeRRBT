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
#include "apiErrors.h"
#include "IloIlo.h"
#include "App.h"
#include "v_rep_internal.h"
#include "LuaScriptFunctions.h"
#include "ThreadPool.h"
#include "v_repStrings.h"
#include <iostream>

int CApiErrors::c_api_mainThread_errorReportingMode=sim_api_errormessage_report;
std::string CApiErrors::last_c_api_mainThread_errorMessage=SIM_API_CALL_NO_ERROR;

int CApiErrors::c_api_guiThread_errorReportingMode=sim_api_errormessage_report;
std::string CApiErrors::last_c_api_guiThread_errorMessage=SIM_API_CALL_NO_ERROR;

CApiErrors::CApiErrors()
{
}

CApiErrors::~CApiErrors()
{
	lastLuaAPICallErrorMessage_processIDs.clear();
	lastLuaAPICallErrorMessage_messages.clear();
	lastLuaAPICallErrorMessage_reportingModes.clear();
}

void CApiErrors::simulationAboutToStart()
{
	lastLuaAPICallErrorMessage_processIDs.clear();
	lastLuaAPICallErrorMessage_messages.clear();
	lastLuaAPICallErrorMessage_reportingModes.clear();
}

void CApiErrors::simulationEnded()
{

}

void CApiErrors::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

VTHREAD_ID_TYPE CApiErrors::_getProcessID()
{
	// We return:
	// 0: if we are in the main thread, and are dealing with the C API
	// 1: if we are in the GUI thread (and obviously are dealing with the C API)
	// 2: if we are in the main thread, and are dealing with the Lua API
	// 3-n: if we are not in the main nor GUI thread (and obviously are dealing with the Lua API)

	if (VThread::isCurrentThreadTheUiThread()||(!VThread::isUiThreadIdSet()))
	{ // We are in the GUI thread
		return((VTHREAD_ID_TYPE)1);
	}
	else
	{ // we are not in the GUI thread
		if (VThread::isCurrentThreadTheMainThread())
		{ // we are in the main thread
			if (App::ct->luaScriptContainer->getMainScriptLaunched()||App::ct->luaScriptContainer->getInAddOnNow()||App::ct->luaScriptContainer->getInLuaCallbackNow())
				return((VTHREAD_ID_TYPE)2); // From main thread and Lua API
			return((VTHREAD_ID_TYPE)0); // from main thread and C API
		}
		else
		{ // we are not in the main thread (and not in the GUI thread)
			return(VThread::getCurrentThreadId()+3);
		}
	}
}

int CApiErrors::_getIndexFromProcessID(VTHREAD_ID_TYPE processID)
{
	for (int i=0;i<int(lastLuaAPICallErrorMessage_processIDs.size());i++)
	{
		if (lastLuaAPICallErrorMessage_processIDs[i]==processID)
			return(i);
	}
	return(-1);
}

std::string CApiErrors::getLastErrorMessage()
{ // can be called by any thread
	VTHREAD_ID_TYPE processID=_getProcessID();
	if (processID==(VTHREAD_ID_TYPE)0)
		return(last_c_api_mainThread_errorMessage); // This is the c-api and main thread!
	if (processID==(VTHREAD_ID_TYPE)1)
		return(last_c_api_guiThread_errorMessage); // This is the c-api and GUI thread!
	return(App::ct->apiErrors->_getLastErrorMessage(processID));
}

std::string CApiErrors::_getLastErrorMessage(VTHREAD_ID_TYPE processID)
{ // should only be called by non-GUI threads
	int theIndex=_getIndexFromProcessID(processID);
	if (theIndex<0)
		return(SIM_API_CALL_NO_ERROR);
	return(lastLuaAPICallErrorMessage_messages[theIndex]);
}

void CApiErrors::setLastErrorMessage(const char* functionName,const char* errMsg)
{ // can be called by any thread
	std::string funcName(functionName);
	if (funcName.size()>9)
	{
		if (funcName.compare(funcName.size()-9,9,"_internal")==0)
			funcName.assign(funcName.begin(),funcName.end()-9);
	}
	VTHREAD_ID_TYPE processID=_getProcessID();
	if (processID==(VTHREAD_ID_TYPE)0)
	{ // This is the C API and main thread!
		if (std::string(errMsg).compare(SIM_API_CALL_NO_ERROR)==0)
			last_c_api_mainThread_errorMessage=SIM_API_CALL_NO_ERROR;
		else
		{
			if (c_api_mainThread_errorReportingMode!=sim_api_errormessage_ignore)
			{
				if ( (c_api_mainThread_errorReportingMode==sim_api_errormessage_report)||(c_api_mainThread_errorReportingMode==sim_api_errormessage_output) )
				{
					std::string errorMsg(errMsg);
//					std::string funcName(functionName);
					last_c_api_mainThread_errorMessage=errorMsg+" ("+funcName+")";
				}
				if (c_api_mainThread_errorReportingMode==sim_api_errormessage_output)
				{
					std::string tmp=IDSNOTR_FUNCTION_CALL_ERROR_C_API+last_c_api_mainThread_errorMessage;
					std::cout << tmp.c_str() << std::endl;
				}
			}
		}
	}
	if (processID==(VTHREAD_ID_TYPE)1)
	{ // This is the C API and GUI thread!
		if (std::string(errMsg).compare(SIM_API_CALL_NO_ERROR)==0)
			last_c_api_guiThread_errorMessage=SIM_API_CALL_NO_ERROR;
		else
		{
			if (c_api_guiThread_errorReportingMode!=sim_api_errormessage_ignore)
			{
				if ( (c_api_guiThread_errorReportingMode==sim_api_errormessage_report)||(c_api_guiThread_errorReportingMode==sim_api_errormessage_output) )
				{
					std::string errorMsg(errMsg);
//					std::string funcName(functionName);
					last_c_api_guiThread_errorMessage=errorMsg+" ("+funcName+")";
				}
				if (c_api_guiThread_errorReportingMode==sim_api_errormessage_output)
				{
					std::string tmp=IDSNOTR_FUNCTION_CALL_ERROR_C_API+last_c_api_guiThread_errorMessage;
					std::cout << tmp.c_str() << std::endl;
				}
			}
		}
	}
	if (processID>(VTHREAD_ID_TYPE)1) // This is the Lua API (and never the GUI thread)
		App::ct->apiErrors->_setLastErrorMessage(processID,funcName.c_str(),errMsg);
//		App::ct->apiErrors->_setLastErrorMessage(processID,functionName,errMsg);
}

void CApiErrors::_setLastErrorMessage(VTHREAD_ID_TYPE processID,const char* functionName,const char* errMsg)
{ // should not be called by the GUI thread
	int theIndex=_getIndexFromProcessID(processID);
	if (theIndex<0)
	{ // That process wasn't yet registered. We add an entry:
		lastLuaAPICallErrorMessage_processIDs.push_back(processID);
		lastLuaAPICallErrorMessage_messages.push_back(SIM_API_CALL_NO_ERROR); // for now
		lastLuaAPICallErrorMessage_reportingModes.push_back(sim_api_errormessage_output); // default for scripts
		theIndex=lastLuaAPICallErrorMessage_processIDs.size()-1;
	}

	if (std::string(errMsg).compare(SIM_API_CALL_NO_ERROR)==0)
		lastLuaAPICallErrorMessage_messages[theIndex]=SIM_API_CALL_NO_ERROR;
	{
		// Here we can react to errors:
		App::ct->simulation->pauseOnErrorRequested();

		// Do we want to output errors?
		if (lastLuaAPICallErrorMessage_reportingModes[theIndex]!=sim_api_errormessage_ignore)
		{
			std::string errorMsg(errMsg);
			std::string funcName(functionName);
			lastLuaAPICallErrorMessage_messages[theIndex]=errorMsg+" ("+funcName+")";
			if (lastLuaAPICallErrorMessage_reportingModes[theIndex]==sim_api_errormessage_output)
				outputCallErrorMessage(functionName,errMsg,true);
		}
	}
}

void CApiErrors::setErrorReportingMode(int mode)
{ // can be called by any thread
	VTHREAD_ID_TYPE processID=_getProcessID();
	if (processID==(VTHREAD_ID_TYPE)0)
	{ // This is the C API and main thread!
		c_api_mainThread_errorReportingMode=mode;
	}
	if (processID==(VTHREAD_ID_TYPE)1)
	{ // This is the C API and GUI thread!
		c_api_guiThread_errorReportingMode=mode;
	}
	if (processID>(VTHREAD_ID_TYPE)1)
	{ // This is the Lua API
		App::ct->apiErrors->_setErrorReportingMode(processID,mode);
	}
}

int CApiErrors::getErrorReportingMode()
{ // can be called by any thread
	VTHREAD_ID_TYPE processID=_getProcessID();
	if (processID==(VTHREAD_ID_TYPE)0)
	{ // This is the C API and main thread!
		return(c_api_mainThread_errorReportingMode);
	}
	if (processID==(VTHREAD_ID_TYPE)1)
	{ // This is the C API and GUI thread!
		return(c_api_guiThread_errorReportingMode);
	}
	return(App::ct->apiErrors->_getErrorReportingMode(processID));
}

void CApiErrors::_setErrorReportingMode(VTHREAD_ID_TYPE processID,int mode)
{ // should not be called by the GUI thread
	int theIndex=_getIndexFromProcessID(processID);
	if (theIndex<0)
	{ // That process wasn't yet registered. We add an entry:
		lastLuaAPICallErrorMessage_processIDs.push_back(processID);
		lastLuaAPICallErrorMessage_messages.push_back(SIM_API_CALL_NO_ERROR);
		lastLuaAPICallErrorMessage_reportingModes.push_back(sim_api_errormessage_output); // for now
		theIndex=lastLuaAPICallErrorMessage_processIDs.size()-1;
	}
	lastLuaAPICallErrorMessage_reportingModes[theIndex]=mode;
}

int CApiErrors::_getErrorReportingMode(VTHREAD_ID_TYPE processID)
{ // should not be called by the GUI thread
	int theIndex=_getIndexFromProcessID(processID);
	if (theIndex<0)
	{ // That process wasn't yet registered. We add an entry:
		lastLuaAPICallErrorMessage_processIDs.push_back(processID);
		lastLuaAPICallErrorMessage_messages.push_back(SIM_API_CALL_NO_ERROR);
		lastLuaAPICallErrorMessage_reportingModes.push_back(sim_api_errormessage_output); // for now
		theIndex=lastLuaAPICallErrorMessage_processIDs.size()-1;
	}
	return(lastLuaAPICallErrorMessage_reportingModes[theIndex]);
}




