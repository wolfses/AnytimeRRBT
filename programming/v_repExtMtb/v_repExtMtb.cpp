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

#include "v_repExtMtb.h"
#include "v_repLib.h"
#include "Access.h"
#include <iostream>

#ifdef _WIN32
	#include <direct.h>
#endif /* _WIN32 */

#if defined (__linux) || defined (__APPLE__)
	#include <unistd.h>
	#include <string.h>
	#define _stricmp(x,y) strcasecmp(x,y)
#endif

#define LUA_RESET "simExtMtbReset"
#define LUA_RESET_TIPS 	"number result=" LUA_RESET "(number objectHandle,table_4 jointValues,table_2 initialJointVelocity)"
#define LUA_RUN "simExtMtbRun"
#define LUA_RUN_TIPS "number result,string info=" LUA_RUN "(number objectHandle,number deltaTime)"
#define LUA_GET_INPUT "simExtMtbGetInput"
#define LUA_GET_INPUT_TIPS "table_4 inputValues=" LUA_GET_INPUT "(number objectHandle)"
#define LUA_SET_INPUT "simExtMtbSetInput"
#define LUA_SET_INPUT_TIPS "number result=" LUA_SET_INPUT "(number objectHandle,table_4 inputValues)"
#define LUA_GET_OUTPUT "simExtMtbGetOutput"
#define LUA_GET_OUTPUT_TIPS "table_4 outputValues=" LUA_GET_OUTPUT "(number objectHandle)"
#define LUA_GET_JOINTS "simExtMtbGetJoints"
#define LUA_GET_JOINTS_TIPS "table_4 jointValues=" LUA_GET_JOINTS "(number objectHandle)"
#define LUA_GET_AUTO_START "simExtMtbGetAutoStart"
#define LUA_GET_AUTO_START_TIPS "boolean autoStart=" LUA_GET_AUTO_START "(number objectHandle)"
#define LUA_CONNECT_INPUT "simExtMtbConnectInput"
#define LUA_CONNECT_INPUT_TIPS "boolean result=" LUA_CONNECT_INPUT "(number inputRobotHandle,number inputBitNumber,number outputRobotHandle,number outputBitNumber,number connectionType)"
#define LUA_DISCONNECT_INPUT "simExtMtbDisconnectInput"
#define LUA_DISCONNECT_INPUT_TIPS "boolean result=" LUA_DISCONNECT_INPUT "(number inputRobotHandle,number inputBitNumber)"


LIBRARY vrepLib;

// This is the callback function for the "simExtMtbReset" custom Lua function:
void LUA_RESET_CALLBACK(SLuaCallBack* p)
{
	int result=-1; // negative number means error
	if (p->inputArgCount>2)
	{ // Ok, we have at least 3 input argument (we expect an object handle, a joint table (4 values) and a joint velocity table (2 values))
		if ( (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)&&( (p->inputArgTypeAndSize[1*2+0]==(sim_lua_arg_float|sim_lua_arg_table))&&(p->inputArgTypeAndSize[1*2+1]>=4) )&&((p->inputArgTypeAndSize[2*2+0]==(sim_lua_arg_float|sim_lua_arg_table))&&(p->inputArgTypeAndSize[2*2+1]>=2) ))
		{ // Ok, we have for argument 1 an int, for arg 2 a table with at least 4 floats, for arg 3 a table with at least 2 floats
			// Now get the MTB robot object associated with the MTB model:
			CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromAssociatedObject(p->inputInt[0]);
			if (mtbRobot!=NULL)
			{ // Ok we found the associated CMtbRobot
				mtbRobot->reset(p->inputFloat,p->inputFloat+4);
				result=1;
			}
			else
				simSetLastError(LUA_RESET,"Object or MTB tag was not found."); // output an error
		}
		else
			simSetLastError(LUA_RESET,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_RESET,"Not enough arguments."); // output an error

	// Now we prepare the return value(s):
	p->outputArgCount=1; // 1 return value
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
	p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
	p->outputArgTypeAndSize[2*0+1]=1;					// Not used (table size if the return value was a table)
	p->outputInt=(simInt*)simCreateBuffer(1*sizeof(result)); // 1 int return value
	p->outputInt[0]=result; // This is the integer value we want to return
}


// This is the callback function for the "simExtMtbRun" custom Lua function:
void LUA_RUN_CALLBACK(SLuaCallBack* p)
{
	int result=-1; // negative number means command syntax error, 0 means program compilation error (message indicates the line), 1 means no error (message indicates current program command), 2 means program end
	std::string message; // compilation error string (when result is 0) or current program command (when result is 1)
	if (p->inputArgCount>1)
	{ // Ok, we have at least 2 input argument
		if ( (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)&&(p->inputArgTypeAndSize[1*2+0]==sim_lua_arg_float) )
		{ // Ok, we have for argument 1 an int, and for argument 2 a float
			// Now get the MTB robot object associated with the MTB model:
			CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromAssociatedObject(p->inputInt[0]);
			if (mtbRobot!=NULL)
			{ // Ok we found the associated CMtbRobot
				result=mtbRobot->run(p->inputFloat[0],message);
			}
			else
				simSetLastError(LUA_RUN,"Object or MTB tag was not found."); // output an error
		}
		else
			simSetLastError(LUA_RUN,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_RUN,"Not enough arguments."); // output an error

	// Now we prepare the return value(s):
	if (result==-1)
	{
		p->outputArgCount=1; // 1 return value
		p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
		p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
		p->outputArgTypeAndSize[2*0+1]=1;					// Not used (table size if the return value was a table)
		p->outputInt=(simInt*)simCreateBuffer(1*sizeof(int)); // 1 int return value
		p->outputInt[0]=result; // This is the integer value we want to return
	}
	else
	{
		p->outputArgCount=2; // 2 return value
		p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*4*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
		p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The first return value is an int
		p->outputArgTypeAndSize[2*0+1]=1;					// Not used (table size if the return value was a table)
		p->outputArgTypeAndSize[2*1+0]=sim_lua_arg_string;	// The second return value is string
		p->outputArgTypeAndSize[2*1+1]=1;					// Not used (table size if the return value was a table)
		p->outputInt=(simInt*)simCreateBuffer(1*sizeof(int)); // first return value
		p->outputInt[0]=result; // This is the integer value we want to return as first value
		p->outputChar=(simChar*)simCreateBuffer(simInt((message.length()+1)*sizeof(simChar))); // second return value
		for (unsigned int i=0;i<message.length();i++)
			p->outputChar[i]=message[i]; // This is the string we want to return as second value
		p->outputChar[message.length()]=0; // terminal zero
	}
}

void LUA_GET_JOINTS_CALLBACK(SLuaCallBack* p)
{
	bool result=false;
	float jointValues[4];
	if (p->inputArgCount>0)
	{ // Ok, we have at least 1 input argument (we expect an object handle)
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{ // Ok, we have for argument 1 an int
			// Now get the MTB robot object associated with the MTB model:
			CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromAssociatedObject(p->inputInt[0]);
			if (mtbRobot!=NULL)
			{ // Ok we found the associated CMtbRobot
				result=mtbRobot->getJoints(jointValues);
			}
			else
				simSetLastError(LUA_GET_JOINTS,"Object or MTB tag was not found."); // output an error
		}
		else
			simSetLastError(LUA_GET_JOINTS,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_GET_JOINTS,"Not enough arguments."); // output an error

	// Now we prepare the return value:
	if (result)
	{ // we return the two values in a table:
		p->outputArgCount=1; // 1 return value
		p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
		p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_float|sim_lua_arg_table;	// The return value is a table of floats
		p->outputArgTypeAndSize[2*0+1]=4;					// the table size is 4
		p->outputFloat=(simFloat*)simCreateBuffer(4*sizeof(float)); // 4 float return value in a table
		p->outputFloat[0]=jointValues[0];
		p->outputFloat[1]=jointValues[1];
		p->outputFloat[2]=jointValues[2];
		p->outputFloat[3]=jointValues[3];
	}
	else
		p->outputArgCount=0; // 0 return values --> nil (error)
}

// This is the callback function for the "simExtMtbGetOutput" custom Lua function:
void LUA_GET_OUTPUT_CALLBACK(SLuaCallBack* p)
{
	bool result=false;
	int output[4];

	if (p->inputArgCount>0)
	{ // Ok, we have at least 1 input argument
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{ // Ok, we have for argument 1 an int
			// Now get the MTB robot object associated with the MTB model:
			CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromAssociatedObject(p->inputInt[0]);
			if (mtbRobot!=NULL)
			{ // Ok we found the associated CMtbRobot
				result=mtbRobot->readOutput(output);
			}
			else
				simSetLastError(LUA_GET_OUTPUT,"Object or MTB tag was not found."); // output an error
		}
		else
			simSetLastError(LUA_GET_OUTPUT,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_GET_OUTPUT,"Not enough arguments."); // output an error

	if (result)
	{ // the function succeeded. We return a table containing 4 integers:
		p->outputArgCount=1; // 1 return value
		p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
		p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int|sim_lua_arg_table;	// The return value is a table of integers
		p->outputArgTypeAndSize[2*0+1]=4;				// he table contains 4 elements
		p->outputInt=(simInt*)simCreateBuffer(4*sizeof(int)); // 4 int return values
		for (int i=0;i<4;i++)
			p->outputInt[i]=output[i]; // Those are the 4 integers for the table
	}
	else
	{ // the function failed. We don't return anything (nil)
		p->outputArgCount=0; // 0 return values
	}
}


// This is the callback function for the "simExtMtbGetinput" custom Lua function:
void LUA_GET_INPUT_CALLBACK(SLuaCallBack* p)
{
	bool result=false;
	int input[4];

	if (p->inputArgCount>0)
	{ // Ok, we have at least 1 input argument
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{ // Ok, we have for argument 1 an int
			// Now get the MTB robot object associated with the MTB model:
			CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromAssociatedObject(p->inputInt[0]);
			if (mtbRobot!=NULL)
			{ // Ok we found the associated CMtbRobot
				result=mtbRobot->readInput(input);
			}
			else
				simSetLastError(LUA_GET_INPUT,"Object or MTB tag was not found."); // output an error
		}
		else
			simSetLastError(LUA_GET_INPUT,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_GET_INPUT,"Not enough arguments."); // output an error

	if (result)
	{ // the function succeeded. We return a table containing 4 integers:
		p->outputArgCount=1; // 1 return value
		p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
		p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int|sim_lua_arg_table;	// The return value is a table of integers
		p->outputArgTypeAndSize[2*0+1]=4;				// he table contains 4 elements
		p->outputInt=(simInt*)simCreateBuffer(4*sizeof(int)); // 4 int return values
		for (int i=0;i<4;i++)
			p->outputInt[i]=input[i]; // Those are the 4 integers for the table
	}
	else
	{ // the function failed. We don't return anything (nil)
		p->outputArgCount=0; // 0 return values
	}
}


// This is the callback function for the "simExtMtbSetinput" custom Lua function:
void LUA_SET_INPUT_CALLBACK(SLuaCallBack* p)
{
	int result=-1; // negative number means error

	if (p->inputArgCount>1)
	{ // Ok, we have at least 2 input argument
		if ( (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)&&((p->inputArgTypeAndSize[1*2+0]==(sim_lua_arg_int|sim_lua_arg_table))&&(p->inputArgTypeAndSize[1*2+1]>3) ) )
		{ // Ok, we have for argument 1 an int, and for arg 2 a table with at least 4 ints
			// Now get the MTB robot object associated with the MTB model:
			CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromAssociatedObject(p->inputInt[0]);
			if (mtbRobot!=NULL)
			{ // Ok we found the associated CMtbRobot
				if (mtbRobot->writeInput(p->inputInt+1))
					result=1;
			}
			else
				simSetLastError(LUA_SET_INPUT,"Object or MTB tag was not found."); // output an error
		}
		else
			simSetLastError(LUA_SET_INPUT,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_SET_INPUT,"Not enough arguments."); // output an error

	p->outputArgCount=1; // 1 return value
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
	p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
	p->outputArgTypeAndSize[2*0+1]=1;					// Not used (table size if the return value was a table)
	p->outputInt=(simInt*)simCreateBuffer(1*sizeof(int)); // 1 int return value
	p->outputInt[0]=result; // This is the integer value we want to return
}

// This is the callback function for the "simExtMtbGetAutoStart" custom Lua function:
void LUA_GET_AUTO_START_CALLBACK(SLuaCallBack* p)
{
	bool result=false;

	if (p->inputArgCount>0)
	{ // Ok, we have at least 1 input argument
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{ // Ok, we have for argument 1 an int
			// Now get the MTB robot object associated with the MTB model:
			CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromAssociatedObject(p->inputInt[0]);
			if (mtbRobot!=NULL)
			{ // Ok we found the associated CMtbRobot
				result=mtbRobot->getOptions()&1;
			}
			else
				simSetLastError(LUA_GET_AUTO_START,"Object or MTB tag was not found."); // output an error
		}
		else
			simSetLastError(LUA_GET_AUTO_START,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_GET_AUTO_START,"Not enough arguments."); // output an error

	// We just return a boolean:
	p->outputArgCount=1; // 1 return value
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
	p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_bool;	// The return value is a bool
	p->outputArgTypeAndSize[2*0+1]=1;					// Not used (table size if the return value was a table)
	p->outputBool=(simBool*)simCreateBuffer(1*sizeof(simBool)); // 1 bool return value (returned as char)
	p->outputBool[0]=result; // This is the bool value we want to return (returned as char)
}

// This is the callback function for the "simExtMtbConnectinput" custom Lua function:
void LUA_CONNECT_INPUT_CALLBACK(SLuaCallBack* p)
{
	bool result=false; 

	if (p->inputArgCount>4)
	{ // Ok, we have at least 5 input argument
		if ( (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)&&(p->inputArgTypeAndSize[1*2+0]==sim_lua_arg_int)&&(p->inputArgTypeAndSize[2*2+0]==sim_lua_arg_int)&&(p->inputArgTypeAndSize[3*2+0]==sim_lua_arg_int)&&(p->inputArgTypeAndSize[4*2+0]==sim_lua_arg_int) )
		{ // Ok, we have 5 int arguments
			// Now get the MTB robot object associated with the MTB model:
			CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromAssociatedObject(p->inputInt[0]);
			if (mtbRobot!=NULL)
			{ // Ok we found the associated CMtbRobot
				result=mtbRobot->addOutputInputConnection(p->inputInt[1],p->inputInt[2],p->inputInt[3],p->inputInt[4]);
			}
			else
				simSetLastError(LUA_CONNECT_INPUT,"Object or MTB tag was not found."); // output an error
		}
		else
			simSetLastError(LUA_CONNECT_INPUT,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_CONNECT_INPUT,"Not enough arguments."); // output an error

	// We just return a boolean:
	p->outputArgCount=1; // 1 return value
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
	p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_bool;	// The return value is a bool
	p->outputArgTypeAndSize[2*0+1]=1;					// Not used (table size if the return value was a table)
	p->outputBool=(simBool*)simCreateBuffer(1*sizeof(simBool)); // 1 bool return value (returned as char)
	p->outputBool[0]=result; // This is the bool value we want to return (returned as char)
}

// This is the callback function for the "simExtMtbDisconnectinput" custom Lua function:
void LUA_DISCONNECT_INPUT_CALLBACK(SLuaCallBack* p)
{
	bool result=false; 

	if (p->inputArgCount>1)
	{ // Ok, we have at least 2 input argument
		if ( (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)&&(p->inputArgTypeAndSize[1*2+0]==sim_lua_arg_int) )
		{ // Ok, we have 2 int arguments
			// Now get the MTB robot object associated with the MTB model:
			CMtbRobot* mtbRobot=CAccess::mtbRobotContainer->getFromAssociatedObject(p->inputInt[0]);
			if (mtbRobot!=NULL)
			{ // Ok we found the associated CMtbRobot
				mtbRobot->removeOutputInputConnection(p->inputInt[1]);
				result=true;
			}
			else
				simSetLastError(LUA_DISCONNECT_INPUT,"Object or MTB tag was not found."); // output an error
		}
		else
			simSetLastError(LUA_DISCONNECT_INPUT,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_DISCONNECT_INPUT,"Not enough arguments."); // output an error

	// We just return a boolean:
	p->outputArgCount=1; // 1 return value
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
	p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_bool;	// The return value is a bool
	p->outputArgTypeAndSize[2*0+1]=1;					// Not used (table size if the return value was a table)
	p->outputBool=(simBool*)simCreateBuffer(1*sizeof(simBool)); // 1 bool return value (returned as char)
	p->outputBool[0]=result; // This is the bool value we want to return (returned as char)
}


VREP_DLLEXPORT unsigned char v_repStart(void* reservedPointer,int reservedInt)
{ // This is called just once, at the start of V-REP.

	// Dynamically load and bind V-REP functions:
	 // ******************************************
	 // 1. Figure out this plugin's directory:
	 char curDirAndFile[1024];
 #ifdef _WIN32
	 _getcwd(curDirAndFile, sizeof(curDirAndFile));
 #elif defined (__linux) || defined (__APPLE__)
	 getcwd(curDirAndFile, sizeof(curDirAndFile));
 #endif
	 std::string currentDirAndPath(curDirAndFile);
	 CAccess::currentDirAndPath=currentDirAndPath;
	 // 2. Append the V-REP library's name:
	 std::string temp(currentDirAndPath);
 #ifdef _WIN32
	 temp+="/v_rep.dll";
 #elif defined (__linux)
	 temp+="/libv_rep.so";
 #elif defined (__APPLE__)
	 temp+="/libv_rep.dylib";
 #endif /* __linux || __APPLE__ */
	 // 3. Load the V-REP library:
	 vrepLib=loadVrepLibrary(temp.c_str());
	 if (vrepLib==NULL)
	 {
		 std::cout << "Error, could not find or correctly load the V-REP library. Cannot start 'Mtb' plugin.\n";
		 return(0); // Means error, V-REP will unload this plugin
	 }
	 if (getVrepProcAddresses(vrepLib)==0)
	 {
		 std::cout << "Error, could not find all required functions in the V-REP library. Cannot start 'Mtb' plugin.\n";
		 unloadVrepLibrary(vrepLib);
		 return(0); // Means error, V-REP will unload this plugin
	 }
	 // ******************************************

	 // Check the version of V-REP:
	 // ******************************************
	 int vrepVer;
	 simGetIntegerParameter(sim_intparam_program_version,&vrepVer);
	 if (vrepVer<20604) // if V-REP version is smaller than 2.06.04
	 {
		 std::cout << "Sorry, your V-REP copy is somewhat old. Cannot start 'Mtb' plugin.\n";
		 unloadVrepLibrary(vrepLib);
		 return(0); // Means error, V-REP will unload this plugin
	 }
	 // ******************************************

	CAccess::createNonGui();
	CAccess::createGui();

	// Register new Lua commands:
	int inArgs2[]={3,sim_lua_arg_int,sim_lua_arg_float|sim_lua_arg_table,sim_lua_arg_float|sim_lua_arg_table};
	simRegisterCustomLuaFunction(LUA_RESET,LUA_RESET_TIPS,inArgs2,LUA_RESET_CALLBACK);

	int inArgs3[]={2,sim_lua_arg_int,sim_lua_arg_float};
	simRegisterCustomLuaFunction(LUA_RUN,LUA_RUN_TIPS,inArgs3,LUA_RUN_CALLBACK);

	int inArgs4[]={1,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_GET_JOINTS,LUA_GET_JOINTS_TIPS,inArgs4,LUA_GET_JOINTS_CALLBACK);

	int inArgs5[]={1,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_GET_OUTPUT,LUA_GET_OUTPUT_TIPS,inArgs5,LUA_GET_OUTPUT_CALLBACK);

	int inArgs6[]={1,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_GET_INPUT,LUA_GET_INPUT_TIPS,inArgs6,LUA_GET_INPUT_CALLBACK);

	int inArgs7[]={2,sim_lua_arg_int,sim_lua_arg_int|sim_lua_arg_table};
	simRegisterCustomLuaFunction(LUA_SET_INPUT,LUA_SET_INPUT_TIPS,inArgs7,LUA_SET_INPUT_CALLBACK);

	int inArgs8[]={1,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_GET_AUTO_START,LUA_GET_AUTO_START_TIPS,inArgs8,LUA_GET_AUTO_START_CALLBACK);

	int inArgs9[]={5,sim_lua_arg_int,sim_lua_arg_int,sim_lua_arg_int,sim_lua_arg_int,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_CONNECT_INPUT,LUA_CONNECT_INPUT_TIPS,inArgs9,LUA_CONNECT_INPUT_CALLBACK);

	int inArgs10[]={2,sim_lua_arg_int,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_DISCONNECT_INPUT,LUA_DISCONNECT_INPUT_TIPS,inArgs10,LUA_DISCONNECT_INPUT_CALLBACK);

	return(6);	// initialization went fine, we return the version number of this plugin (can be queried with simGetModuleName)
				// version 1 was for V-REP 2.5.11 or earlier
				// version 2 was for V-REP versions before V-REP 2.6.0
				// version 3 was for V-REP versions before V-REP 2.6.7
				// version 4 is the Qt version
				// version 5 is from 10/1/2014
				// version 6 is after V-REP 3.1.3
}

VREP_DLLEXPORT void v_repEnd()
{ // This is called just once, at the end of V-REP
	CAccess::destroyGui();
	CAccess::destroyNonGui();
	unloadVrepLibrary(vrepLib); // release the library
}

VREP_DLLEXPORT void* v_repMessage(int message,int* auxiliaryData,void* customData,int* replyData)
{ // This is called quite often. Just watch out for messages/events you want to handle
	static bool refreshDlgFlag=true;

	// This function should not generate any error messages:
	int errorModeSaved;
	simGetIntegerParameter(sim_intparam_error_report_mode,&errorModeSaved);
	simSetIntegerParameter(sim_intparam_error_report_mode,sim_api_errormessage_ignore);

	void* retVal=NULL;
	if (message==sim_message_eventcallback_refreshdialogs)
		refreshDlgFlag=true; // V-REP dialogs were refreshed. Maybe a good idea to refresh this plugin's dialog too

	if (message==sim_message_eventcallback_menuitemselected)
	{ // A custom menu bar entry was selected..
		if (auxiliaryData[0]==CAccess::mtbRobotDialog->dialogMenuItemHandle)
			CAccess::mtbRobotDialog->makeVisible(!CAccess::mtbRobotDialog->getVisible()); // Toggle visibility of the dialog
	}

	if (message==sim_message_eventcallback_instancepass)
	{ // It is important to always correctly react to events in V-REP. This message is the most convenient way to do so:

		CAccess::mtbRobotDialog->handleCommands();
		CAccess::mtbRobotDialog->setState(simGetSimulationState()==sim_simulation_stopped,simGetObjectLastSelection());

		int flags=auxiliaryData[0];
		bool sceneContentChanged=((flags&(1+2+4+8+16+32+64+256))!=0); // object erased, created, model or scene loaded, und/redo called, instance switched, or object scaled since last sim_message_eventcallback_instancepass message 
		bool instanceSwitched=((flags&64)!=0);

		if (instanceSwitched)
		{
			// React to an instance switch here!!
		}

		if (sceneContentChanged)
		{ // we actualize plugin objects for changes in the scene
			CAccess::mtbRobotContainer->actualizeForSceneContent(); 
			refreshDlgFlag=true;
		}
	}

	if (message==sim_message_eventcallback_simulationabouttostart)
	{ // Simulation is about to start
		CAccess::mtbRobotContainer->startOfSimulation();
	}

	if (message==sim_message_eventcallback_simulationended)
	{ // Simulation just ended
		CAccess::mtbRobotContainer->endOfSimulation();
	}

	if ((message==sim_message_eventcallback_guipass)&&refreshDlgFlag)
	{ // handle refresh of the plugin's dialog:
		CAccess::mtbRobotDialog->refresh(); // Refresh the dialog
		refreshDlgFlag=false;
	}

	simSetIntegerParameter(sim_intparam_error_report_mode,errorModeSaved); // restore previous settings
	return(retVal);
}
