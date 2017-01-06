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

#include "v_repExtRemoteApi.h"
#include "v_repLib.h"
#include <iostream>
#include "simxConnections.h"
#include "confReader.h"
#include <sstream>

#ifdef _WIN32
	#ifdef QT_COMPIL
		#include <direct.h>
	#else
		#include <shlwapi.h> // required for PathRemoveFileSpec function
		#pragma comment(lib, "Shlwapi.lib")
	#endif
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
	#include <unistd.h>
	#define WIN_AFX_MANAGE_STATE
#endif /* __linux || __APPLE__ */


#define CONCAT(x,y,z) x y z
#define strConCat(x,y,z)	CONCAT(x,y,z)
#define LUA_START_COMMAND "simExtRemoteApiStart"
#define LUA_STOP_COMMAND "simExtRemoteApiStop"
#define LUA_RESET_COMMAND "simExtRemoteApiReset"
#define LUA_STATUS_COMMAND "simExtRemoteApiStatus"

LIBRARY vrepLib;
CSimxConnections allConnections;

void LUA_STATUS_COMMAND_CALLBACK(SLuaCallBack* p)
{ // the callback function of the new Lua command
	int result=-1; // error or no connection at given index
	int info[5];
	int clientVersion=-1;

	if (p->inputArgCount>0)
	{ // Ok, we have at least 1 input argument
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{ // Ok, we have (at least) 1 int as argument
			CSimxSocket* s=allConnections.getConnectionFromPort(p->inputInt[0]);
//			CSimxSocket* s=allConnections.getConnectionAtIndex(p->inputInt[0]);
			if (s!=NULL)
			{
				result=s->getStatus();
				s->getInfo(info);
				clientVersion=s->getClientVersion();
			}
		}
		else
			simSetLastError(LUA_STATUS_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_STATUS_COMMAND,"Not enough arguments."); // output an error

	// Now we prepare the return value:
	if (result>-1)
	{
		p->outputArgCount=4; // 4 return values
		p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
		p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
		p->outputArgTypeAndSize[2*0+1]=1;				// Not used (table size if the return value was a table)
		p->outputArgTypeAndSize[2*1+0]=sim_lua_arg_int|sim_lua_arg_table;	// The return value is an int table
		p->outputArgTypeAndSize[2*1+1]=5;				// Table size
		p->outputArgTypeAndSize[2*2+0]=sim_lua_arg_int;	// The return value is an int
		p->outputArgTypeAndSize[2*2+1]=1;				// Not used (table size if the return value was a table)
		p->outputArgTypeAndSize[2*3+0]=sim_lua_arg_int;	// The return value is an int
		p->outputArgTypeAndSize[2*3+1]=1;				// Not used (table size if the return value was a table)
		p->outputInt=(simInt*)simCreateBuffer(8*sizeof(result)); // total of 8 ints
		p->outputInt[0]=result; // This is the int value we want to return (arg 1)
		// Now the table values:
		for (int i=0;i<5;i++)
			p->outputInt[1+i]=info[i];
		p->outputInt[6]=SIMX_VERSION; // This is the server version info (arg 3)
		p->outputInt[7]=clientVersion; // This is the client version info (arg 4)
	}
	else
	{
		p->outputArgCount=4; // 4 return values
		p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
		p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
		p->outputArgTypeAndSize[2*0+1]=1;				// Not used (table size if the return value was a table)
		p->outputArgTypeAndSize[2*1+0]=sim_lua_arg_nil;	// The return value is nil
		p->outputArgTypeAndSize[2*1+1]=1;				// Not used (table size if the return value was a table)
		p->outputArgTypeAndSize[2*2+0]=sim_lua_arg_int;	// The return value is an int
		p->outputArgTypeAndSize[2*2+1]=1;				// Not used (table size if the return value was a table)
		p->outputArgTypeAndSize[2*3+0]=sim_lua_arg_int;	// The return value is an int
		p->outputArgTypeAndSize[2*3+1]=1;				// Not used (table size if the return value was a table)
		p->outputInt=(simInt*)simCreateBuffer(3*sizeof(result)); // 3 int return value
		p->outputInt[0]=result; // This is the int value we want to return
		p->outputInt[1]=SIMX_VERSION; // This is the second int value we want to return
		p->outputInt[2]=clientVersion; // This is the third int value we want to return
	}
}

void LUA_START_COMMAND_CALLBACK(SLuaCallBack* p)
{ // the callback function of the new Lua command
	int result=-1; // error

	if (p->inputArgCount>0)
	{ // Ok, we have at least 1 input argument
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{ // Ok, we have (at least) 1 int as argument
			bool err=false;
			int maxPacketSize=1300;
			if (p->inputInt[0]<0)
				maxPacketSize=3200000; // when using shared memory
			bool debug=false;
			bool triggerPreEnabled=false; // 3/3/2014
			if (p->inputArgCount>1)
			{
				if (p->inputArgTypeAndSize[1*2+0]==sim_lua_arg_int)
				{
					maxPacketSize=p->inputInt[1];
					if (p->inputInt[0]<0)
					{ // when using shared memory
						if (maxPacketSize<1000)
							maxPacketSize=1000;
						if (maxPacketSize>32000000)
							maxPacketSize=32000000;
					}
					else
					{ // when using sockets
						if (maxPacketSize<200)
							maxPacketSize=200;
						if (maxPacketSize>30000)
							maxPacketSize=30000;
					}
				}
				else
					err=true;
				if (p->inputArgCount>2)
				{
					if (p->inputArgTypeAndSize[2*2+0]==sim_lua_arg_bool)
						debug=(p->inputBool[0]!=0);
					else
						err=true;

					// 3/3/2014
					if (p->inputArgCount>3)
					{
						if (p->inputArgTypeAndSize[3*2+0]==sim_lua_arg_bool)
							triggerPreEnabled=(p->inputBool[1]!=0);
						else
							err=true;
					}
				}
			}
			if (!err)
			{ 
				CSimxSocket* s=allConnections.getConnectionFromPort(p->inputInt[0]);
				if (s==NULL)
				{
					int prop,obj;
					if (-1!=simGetScriptProperty(p->scriptID,&prop,&obj))
					{
						int scriptType=((prop|sim_scripttype_threaded)-sim_scripttype_threaded);
						bool destroyAtSimulationEnd=( (scriptType==sim_scripttype_mainscript)||(scriptType==sim_scripttype_childscript)||(scriptType==sim_scripttype_jointctrlcallback)||(scriptType==sim_scripttype_contactcallback) );
						CSimxSocket* oneSocketConnection=new CSimxSocket(p->inputInt[0],false,destroyAtSimulationEnd,debug,maxPacketSize,triggerPreEnabled); // 3/3/2014
						oneSocketConnection->start();
						allConnections.addSocketConnection(oneSocketConnection);
						result=1;
					}
					else
						simSetLastError(LUA_START_COMMAND,"Unknown error."); // output an error
				}
				else
					simSetLastError(LUA_START_COMMAND,"Invalid port number."); // output an error
			}
			else
				simSetLastError(LUA_START_COMMAND,"Wrong argument type/size."); // output an error
		}
		else
			simSetLastError(LUA_START_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_START_COMMAND,"Not enough arguments."); // output an error

	// Now we prepare the return value:
	p->outputArgCount=1; // 1 return value
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
	p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
	p->outputArgTypeAndSize[2*0+1]=1;				// Not used (table size if the return value was a table)
	p->outputInt=(simInt*)simCreateBuffer(1*sizeof(result)); // 1 int return value
	p->outputInt[0]=result; // This is the int value we want to return
}

void LUA_STOP_COMMAND_CALLBACK(SLuaCallBack* p)
{ // the callback function of the new Lua command
	int result=-1; // error

	if (p->inputArgCount>0)
	{ // Ok, we have at least 1 input argument
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{ // Ok, we have (at least) 1 int as argument
			CSimxSocket* s=allConnections.getConnectionFromPort(p->inputInt[0]);
			if (s!=NULL)
			{
				if (!s->getContinuousService())
				{
					allConnections.removeSocketConnection(s);
					result=1;
				}
				else
					simSetLastError(LUA_STOP_COMMAND,"Can't stop a continuous remote API server service."); // output an error
			}
			else
				simSetLastError(LUA_STOP_COMMAND,"Invalid port number."); // output an error
		}
		else
			simSetLastError(LUA_STOP_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_STOP_COMMAND,"Not enough arguments."); // output an error

	// Now we prepare the return value:
	p->outputArgCount=1; // 1 return value
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
	p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
	p->outputArgTypeAndSize[2*0+1]=1;				// Not used (table size if the return value was a table)
	p->outputInt=(simInt*)simCreateBuffer(1*sizeof(result)); // 1 int return value
	p->outputInt[0]=result; // This is the int value we want to return
}

void LUA_RESET_COMMAND_CALLBACK(SLuaCallBack* p)
{ // the callback function of the new Lua command
	int result=-1; // error

	if (p->inputArgCount>0)
	{ // Ok, we have at least 1 input argument
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{ // Ok, we have (at least) 1 int as argument
			CSimxSocket* s=allConnections.getConnectionFromPort(p->inputInt[0]);
			if (s!=NULL)
			{
				bool simulOnly=s->getActiveOnlyDuringSimulation();
				bool continuous=s->getContinuousService();
				bool debug=s->getDebug();
				int maxPacketS=s->getMaxPacketSize();
				bool triggerPreEnabled=s->getWaitForTriggerAuthorized();

				// Kill the thread/connection:
				allConnections.removeSocketConnection(s);
				
				// Now create a similar thread/connection:
				CSimxSocket* oneSocketConnection=new CSimxSocket(p->inputInt[0],continuous,simulOnly,debug,maxPacketS,triggerPreEnabled);
				oneSocketConnection->start();
				allConnections.addSocketConnection(oneSocketConnection);
				
				result=1;
			}
			else
				simSetLastError(LUA_RESET_COMMAND,"Invalid port number."); // output an error
		}
		else
			simSetLastError(LUA_RESET_COMMAND,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_RESET_COMMAND,"Not enough arguments."); // output an error

	// Now we prepare the return value:
	p->outputArgCount=1; // 1 return value
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
	p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
	p->outputArgTypeAndSize[2*0+1]=1;				// Not used (table size if the return value was a table)
	p->outputInt=(simInt*)simCreateBuffer(1*sizeof(result)); // 1 int return value
	p->outputInt[0]=result; // This is the int value we want to return
}


// This is the plugin start routine:
VREP_DLLEXPORT unsigned char v_repStart(void* reservedPointer,int reservedInt)
{ // This is called just once, at the start of V-REP
	// Dynamically load and bind V-REP functions:
	char curDirAndFile[1024];
#ifdef _WIN32
	#ifdef QT_COMPIL
		_getcwd(curDirAndFile, sizeof(curDirAndFile));
	#else
		GetModuleFileName(NULL,curDirAndFile,1023);
		PathRemoveFileSpec(curDirAndFile);
	#endif
#elif defined (__linux) || defined (__APPLE__)
	getcwd(curDirAndFile, sizeof(curDirAndFile));
#endif

	std::string currentDirAndPath(curDirAndFile);
	std::string temp(currentDirAndPath);

#ifdef _WIN32
	temp+="\\v_rep.dll";
#elif defined (__linux)
	temp+="/libv_rep.so";
#elif defined (__APPLE__)
	temp+="/libv_rep.dylib";
#endif /* __linux || __APPLE__ */

	vrepLib=loadVrepLibrary(temp.c_str());
	if (vrepLib==NULL)
	{
		std::cout << "Error, could not find or correctly load the V-REP library. Cannot start 'RemoteApi' plugin.\n";
		return(0); // Means error, V-REP will unload this plugin
	}
	if (getVrepProcAddresses(vrepLib)==0)
	{
		std::cout << "Error, could not find all required functions in the V-REP library. Cannot start 'RemoteApi' plugin.\n";
		unloadVrepLibrary(vrepLib);
		return(0); // Means error, V-REP will unload this plugin
	}

	int vrepVer;
	simGetIntegerParameter(sim_intparam_program_version,&vrepVer);
	if (vrepVer<30200) // if V-REP version is smaller than 3.02.00
	{
		std::cout << "Sorry, your V-REP copy is somewhat old. Cannot start 'RemoteApi' plugin.\n";
		unloadVrepLibrary(vrepLib);
		return(0); // Means error, V-REP will unload this plugin
	}

	// Register 3 new Lua commands:
	int inArgs1[]={1,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_STATUS_COMMAND,strConCat("number status,table_5 info=",LUA_STATUS_COMMAND,"(number socketPort)"),inArgs1,LUA_STATUS_COMMAND_CALLBACK);
	int inArgs2[]={4,sim_lua_arg_int,sim_lua_arg_int,sim_lua_arg_bool,sim_lua_arg_bool}; // 3/3/2014
	simRegisterCustomLuaFunction(LUA_START_COMMAND,strConCat("number result=",LUA_START_COMMAND,"(number socketPort,number maxPacketSize=1300,boolean debug=false,boolean preEnableTrigger=false)"),inArgs2,LUA_START_COMMAND_CALLBACK); // 3/3/2014
	simRegisterCustomLuaFunction(LUA_STOP_COMMAND,strConCat("number result=",LUA_STOP_COMMAND,"(number socketPort)"),inArgs1,LUA_STOP_COMMAND_CALLBACK);
	simRegisterCustomLuaFunction(LUA_RESET_COMMAND,strConCat("number result=",LUA_RESET_COMMAND,"(number socketPort)"),inArgs1,LUA_RESET_COMMAND_CALLBACK);

	// Read the configuration file to prepare socket connections:
	CConfReader conf;
	temp=currentDirAndPath;

#ifdef _WIN32
	temp+="\\remoteApiConnections.txt";
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
	temp+="/remoteApiConnections.txt";
#endif /* __linux || __APPLE__ */

	conf.readConfiguration(temp.c_str());
	conf.getBoolean("useAlternateSocketRoutines",CSimxSocket::useAlternateSocketRoutines);

	int index=1;
	while (true)
	{
		std::stringstream strStream;
		strStream << index;
		std::string variableNameBase("portIndex");
		variableNameBase+=strStream.str();
		std::string variableName=variableNameBase+"_port";
		int portNb;
		if (conf.getInteger(variableName.c_str(),portNb))
		{
			bool debug=false;
			int maxPacketSize=1300;
			if (portNb<0)
				maxPacketSize=3200000;

			bool synchronousTrigger=false;
			variableName=variableNameBase+"_maxPacketSize";
			conf.getInteger(variableName.c_str(),maxPacketSize);
			variableName=variableNameBase+"_debug";
			conf.getBoolean(variableName.c_str(),debug);
			variableName=variableNameBase+"_syncSimTrigger";
			conf.getBoolean(variableName.c_str(),synchronousTrigger);

			if (portNb<0)
			{ // when using shared memory
				if (maxPacketSize<1000)
					maxPacketSize=1000;
				if (maxPacketSize>32000000)
					maxPacketSize=32000000;
			}
			else
			{ // when using sockets
				if (maxPacketSize<200)
					maxPacketSize=200;
				if (maxPacketSize>30000)
					maxPacketSize=30000;
			}

			if (allConnections.getConnectionFromPort(portNb)==NULL)
			{
// 3/3/2014				synchronousTrigger&=(allConnections.getSynchronousSimulationTriggerConnection()==NULL);
				CSimxSocket* oneSocketConnection=new CSimxSocket(portNb,true,false,debug,maxPacketSize,synchronousTrigger);
				oneSocketConnection->start();
				allConnections.addSocketConnection(oneSocketConnection);
// 3/3/2014				if (synchronousTrigger)
// 3/3/2014					allConnections.setSynchronousSimulationTriggerConnection(oneSocketConnection); // only one connection can act as trigger!
				std::cout << "Starting a remote API server on port " << portNb << std::endl;
			}
			else
				std::cout << "Failed starting a remote API server on port " << portNb << std::endl;
			index++;
		}
		else
			break;
	}

	return(SIMX_VERSION); // initialization went fine, we return the version number of this extension module (can be queried with simGetModuleName)
}

// This is the plugin end routine:
VREP_DLLEXPORT void v_repEnd()
{ // This is called just once, at the end of V-REP
	allConnections.removeAllConnections();

	unloadVrepLibrary(vrepLib); // release the library
}

// This is the plugin messaging routine (i.e. V-REP calls this function very often, with various messages):
VREP_DLLEXPORT void* v_repMessage(int message,int* auxiliaryData,void* customData,int* replyData)
{ // This is called quite often. Just watch out for messages/events you want to handle

	// This function should not generate any error messages:
	int errorModeSaved;
	simGetIntegerParameter(sim_intparam_error_report_mode,&errorModeSaved);
	simSetIntegerParameter(sim_intparam_error_report_mode,sim_api_errormessage_ignore);

	void* retVal=NULL;

	if (message==sim_message_eventcallback_instancepass)
	{ // This message is sent each time the scene was rendered (well, shortly after) (very often)
		allConnections.instancePass();
	}

	if (message==sim_message_eventcallback_mainscriptabouttobecalled)
	{ // Main script is about to be run
		if (allConnections.thereWasARequestToCallTheMainScript())
			replyData[0]=0; // this tells V-REP that we don't wanna execute the main script
		else
			allConnections.mainScriptWillBeCalled(); // this simply tells all remote API server services to reactivate their triggers (if that function is enabled)
	}

	if (message==sim_message_eventcallback_simulationended)
	{ // Simulation just ended
		allConnections.simulationEnded();
	}

	simSetIntegerParameter(sim_intparam_error_report_mode,errorModeSaved); // restore previous settings
	return(retVal);
}

