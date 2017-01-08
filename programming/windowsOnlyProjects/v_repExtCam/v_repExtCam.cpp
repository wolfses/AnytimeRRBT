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

#include "stdafx.h"
#include "escapi.h"
#include "Mmsystem.h"
#include <shlwapi.h> // for the "PathRemoveFileSpec" function
#include <vector>
#include <iostream>
#include "v_repLib.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

#define VREP_DLLEXPORT extern "C" __declspec(dllexport)
#define strConCat(x,y,z)	x##y##z
#define LUA_START "simExtCamStart"
#define LUA_END "simExtCamEnd"
#define LUA_INFO "simExtCamInfo"
#define LUA_GRAB "simExtCamGrab"

int startCountOverall=0;
int deviceCount=0;
int startCountPerDevice[4]={0,0,0,0}; // first 4 are for devices 0-3
volatile bool openCaptureDevices[4]={false,false,false,false}; // The capture devices (out of a max of 4) that have been initialized
volatile bool _camThreadLaunched=false;
struct SimpleCapParams captureInfo[4];
CRITICAL_SECTION m_cs;
bool displayAcknowledgment=false;

LIBRARY vrepLib;

void killThread()
{
	_camThreadLaunched=false;
	while (!_camThreadLaunched)
		Sleep(2);
	_camThreadLaunched=false;
}

DWORD WINAPI _camThread(LPVOID lpParam)
{
	_camThreadLaunched=true;
	while (_camThreadLaunched)
	{
		static bool firstHere=true;
		if (firstHere)
			displayAcknowledgment=true;
		firstHere=false;
		EnterCriticalSection(&m_cs);
		for (int i=0;i<4;i++)
		{
			if (openCaptureDevices[i])
			{
				if (isCaptureDone(i))
					doCapture(i);
			}
		}
		LeaveCriticalSection(&m_cs);
		Sleep(10);
	}
	for (int i=0;i<4;i++)
	{
		if (openCaptureDevices[i])
		{
			deinitCapture(i);
			delete[] captureInfo[i].mTargetBuf;
		}
		openCaptureDevices[i]=false;
	}
	_camThreadLaunched=true;
	return(0);
}


void LUA_START_CALLBACK(SLuaCallBack* p)
{ // the callback function of the new Lua command
	int result=-1; // error
	int returnResolution[2]={0,0};

	if (p->inputArgCount>2)
	{ // Ok, we have at least 3 input argument
		if ( (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)&&(p->inputArgTypeAndSize[1*2+0]==sim_lua_arg_int)&&(p->inputArgTypeAndSize[2*2+0]==sim_lua_arg_int) )
		{ // Ok, we have (at least) 3 ints as argument
			if ( (countCaptureDevices()>p->inputInt[0])&&(p->inputInt[0]>=0)&&(p->inputInt[0]<4) )
			{
				if (!openCaptureDevices[p->inputInt[0]])
				{ // We can set the new resolution
					bool goOn=true;
					if (startCountOverall==0)
					{ // Launch the thread!
						_camThreadLaunched=false;
						CreateThread(NULL,0,_camThread,NULL,THREAD_PRIORITY_NORMAL,NULL);
						while (!_camThreadLaunched)
							Sleep(2);
						if (deviceCount<1)
						{
							simSetLastError(LUA_START,"ESCAPI initialization failure or no devices found."); // output an error
							killThread();
							goOn=false;
						}
					}

					if (goOn)
					{
						captureInfo[p->inputInt[0]].mWidth=p->inputInt[1];
						captureInfo[p->inputInt[0]].mHeight=p->inputInt[2];
						captureInfo[p->inputInt[0]].mTargetBuf=new int[p->inputInt[1]*p->inputInt[2]];
						if (initCapture(p->inputInt[0],&captureInfo[p->inputInt[0]])!=0)
						{
							doCapture(p->inputInt[0]);
							openCaptureDevices[p->inputInt[0]]=true;
							returnResolution[0]=p->inputInt[1];
							returnResolution[1]=p->inputInt[2];
							result=1; // success!
							startCountOverall++;
							startCountPerDevice[p->inputInt[0]]++;
						}
						else
						{
							delete[] captureInfo[p->inputInt[0]].mTargetBuf;
							simSetLastError(LUA_START,"Device may already be in use."); // output an error
						}
					}
				}
				else
				{ // We have to retrieve the current resolution
					returnResolution[0]=captureInfo[p->inputInt[0]].mWidth;
					returnResolution[1]=captureInfo[p->inputInt[0]].mHeight;
					result=0;
					startCountOverall++;
					startCountPerDevice[p->inputInt[0]]++;
				}
			}
			else
				simSetLastError(LUA_START,"Invalid device index."); // output an error
		}
		else
			simSetLastError(LUA_START,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_START,"Not enough arguments."); // output an error

	// Now we prepare the return value:
	if (result>-1)
	{
		p->outputArgCount=3; // 3 return values
		p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
		p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
		p->outputArgTypeAndSize[2*0+1]=1;				// Not used (table size if the return value was a table)
		p->outputArgTypeAndSize[2*1+0]=sim_lua_arg_int;	// The return value is an int
		p->outputArgTypeAndSize[2*1+1]=1;				// Not used (table size if the return value was a table)
		p->outputArgTypeAndSize[2*2+0]=sim_lua_arg_int;	// The return value is an int
		p->outputArgTypeAndSize[2*2+1]=1;				// Not used (table size if the return value was a table)
		p->outputInt=(simInt*)simCreateBuffer(3*sizeof(result)); // 1 int return value
		p->outputInt[0]=result; // This is the int value we want to return
		p->outputInt[1]=returnResolution[0]; // This is the int value we want to return
		p->outputInt[2]=returnResolution[1]; // This is the int value we want to return
	}
	else
	{
		p->outputArgCount=1; // 1 return value
		p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
		p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
		p->outputArgTypeAndSize[2*0+1]=1;				// Not used (table size if the return value was a table)
		p->outputInt=(simInt*)simCreateBuffer(1*sizeof(result)); // 1 int return value
		p->outputInt[0]=result; // This is the int value we want to return
	}
}


void LUA_END_CALLBACK(SLuaCallBack* p)
{ // the callback function of the new Lua command
	int result=-1; // error

	if (p->inputArgCount>0)
	{ // Ok, we have at least 1 input argument
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{ // Ok, we have (at least) 1 int as argument
			if ( (p->inputInt[0]<4)&&(startCountPerDevice[p->inputInt[0]]>0) )
			{
				startCountOverall--;
				startCountPerDevice[p->inputInt[0]]--;
				if (startCountPerDevice[p->inputInt[0]]==0)
				{
					EnterCriticalSection(&m_cs);
					deinitCapture(p->inputInt[0]);
					delete[] captureInfo[p->inputInt[0]].mTargetBuf;
					openCaptureDevices[p->inputInt[0]]=false;
					LeaveCriticalSection(&m_cs);
				}
				if (startCountOverall==0)
					killThread();
				result=1;
			}
			else
				simSetLastError(LUA_END,"Invalid device index."); // output an error
		}
		else
			simSetLastError(LUA_END,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_END,"Not enough arguments."); // output an error


	// Now we prepare the return value:
	p->outputArgCount=1; // 1 return value
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
	p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
	p->outputArgTypeAndSize[2*0+1]=1;				// Not used (table size if the return value was a table)
	p->outputInt=(simInt*)simCreateBuffer(1*sizeof(result)); // 1 int return value
	p->outputInt[0]=result; // This is the int value we want to return
}



void LUA_INFO_CALLBACK(SLuaCallBack* p)
{ // the callback function of the new Lua command
	char infoString[200];
	infoString[0]=0;

	if (p->inputArgCount>0)
	{ // Ok, we have at least 1 input argument
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{ // Ok, we have (at least) one int as argument
			if ( (countCaptureDevices()>p->inputInt[0])&&(p->inputInt[0]>=0)&&(p->inputInt[0]<4) )
			{
				getCaptureDeviceName(p->inputInt[0],infoString,200);
			}
			else
				simSetLastError(LUA_INFO,"Wrong index."); // output an error
		}
		else
			simSetLastError(LUA_INFO,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_INFO,"Not enough arguments."); // output an error

	// Now we prepare the return value:
	int l=int(strlen(infoString));
	if (l!=0)
	{
		p->outputArgCount=1; // 1 return value
		p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
		p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_string;	// The return value is a string
		p->outputArgTypeAndSize[2*0+1]=1;					// Not used (table size if the return value was a table)
		p->outputChar=(simChar*)simCreateBuffer(l+1); // 1 string return value
		for (int i=0;i<l;i++)
			p->outputChar[i]=infoString[i];
		p->outputChar[l]=0;
	}
	else
		p->outputArgCount=0; // no return values (nil)
}

void LUA_GRAB_CALLBACK(SLuaCallBack* p)
{ // the callback function of the new Lua command
	int retVal=0; // Means error

	if (p->inputArgCount>1)
	{ // Ok, we have at least 2 input argument
		if ( (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)&&(p->inputArgTypeAndSize[1*2+0]==sim_lua_arg_int) )
		{ // Ok, we have (at least) 2 ints as argument
			if ( (countCaptureDevices()>p->inputInt[0])&&(p->inputInt[0]>=0)&&(p->inputInt[0]<4) )
			{
				if (startCountPerDevice[p->inputInt[0]]>0)
				{
					if (openCaptureDevices[p->inputInt[0]])
					{
						int errorModeSaved;
						simGetIntegerParameter(sim_intparam_error_report_mode,&errorModeSaved);
						simSetIntegerParameter(sim_intparam_error_report_mode,sim_api_errormessage_ignore);
						int t=simGetObjectType(p->inputInt[1]);
						simSetIntegerParameter(sim_intparam_error_report_mode,errorModeSaved); // restore previous settings
						if (t==sim_object_visionsensor_type)
						{
							int r[2]={0,0};
							simGetVisionSensorResolution(p->inputInt[1],r);
							if ( (r[0]==captureInfo[p->inputInt[0]].mWidth)&&(r[1]==captureInfo[p->inputInt[0]].mHeight) )
							{
								float* buff=new float[r[0]*r[1]*3];

								for (int i=0;i<r[1];i++)
								{
									int y0=r[0]*i;
									int y1=r[0]*(r[1]-i-1);
									for (int j=0;j<r[0];j++)
									{ // Info is provided as BGR!! (and not RGB)
										buff[3*(y0+j)+0]=float(((BYTE*)captureInfo[p->inputInt[0]].mTargetBuf)[4*(y1+j)+2])/255.0f;
										buff[3*(y0+j)+1]=float(((BYTE*)captureInfo[p->inputInt[0]].mTargetBuf)[4*(y1+j)+1])/255.0f;
										buff[3*(y0+j)+2]=float(((BYTE*)captureInfo[p->inputInt[0]].mTargetBuf)[4*(y1+j)+0])/255.0f;
									}
								}
								simSetVisionSensorImage(p->inputInt[1],buff);
								delete[] buff;
								retVal=1;								
							}
							else
								simSetLastError(LUA_GRAB,"Resolutions do not match."); // output an error
						}
						else
							simSetLastError(LUA_GRAB,"Invalid vision sensor handle."); // output an error
					}
					else
						simSetLastError(LUA_GRAB,"Resolution was not set."); // output an error
				}
				else
					simSetLastError(LUA_GRAB,"simExtCamStart was not called."); // output an error
			}
			else
				simSetLastError(LUA_GRAB,"Wrong index."); // output an error
		}
		else
			simSetLastError(LUA_GRAB,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_GRAB,"Not enough arguments."); // output an error

	// Now we prepare the return value:
	p->outputArgCount=1; // 1 return value
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
	p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
	p->outputArgTypeAndSize[2*0+1]=1;				// Not used (table size if the return value was a table)
	p->outputInt=(simInt*)simCreateBuffer(1*sizeof(retVal)); // 1 int return value
	p->outputInt[0]=retVal; // This is the int value we want to return
}


VREP_DLLEXPORT unsigned char v_repStart(void* reservedPointer,int reservedInt)
{ // This is called just once, at the start of V-REP

	// Dynamically load and bind V-REP functions:
	char curDirAndFile[1024];
	GetModuleFileName(NULL,curDirAndFile,1023);
	PathRemoveFileSpec(curDirAndFile);
	std::string currentDirAndPath(curDirAndFile);
	std::string temp(currentDirAndPath);
	temp+="\\v_rep.dll";
	vrepLib=loadVrepLibrary(temp.c_str());
	if (vrepLib==NULL)
	{
		std::cout << "Error, could not find or correctly load v_rep.dll. Cannot start 'Cam' plugin.\n";
		return(0); // Means error, V-REP will unload this plugin
	}
	if (getVrepProcAddresses(vrepLib)==0)
	{
		std::cout << "Error, could not find all required functions in v_rep.dll. Cannot start 'Cam' plugin.\n";
		unloadVrepLibrary(vrepLib);
		return(0); // Means error, V-REP will unload this plugin
	}

	int vrepVer;
	simGetIntegerParameter(sim_intparam_program_version,&vrepVer);
	if (vrepVer<20600) // if V-REP version is smaller than 2.06.00
	{
		std::cout << "Sorry, your V-REP copy is somewhat old, V-REP 2.6.0 or later is required. Cannot start 'Cam' plugin.\n";
		unloadVrepLibrary(vrepLib);
		return(0); // initialization failed!!
	}

	// Marc modified following function to return -1 in case of initialization error:
	deviceCount=setupESCAPI();
	if (deviceCount<0)
	{
		std::cout << "ESCAPI initialization failed (error code: " << deviceCount << "). Is 'escapi.dll' available? Cannot start 'Cam' plugin.\n";
		unloadVrepLibrary(vrepLib);
		return(0); // initialization failed!!
	}

	// Register one new Lua command:
	int inArgs1[]={3,sim_lua_arg_int,sim_lua_arg_int,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_START,strConCat("number result,number resX,number resY=",LUA_START,"(number deviceIndex,number resX,number resY)"),inArgs1,LUA_START_CALLBACK);

	int inArgs2[]={1,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_END,strConCat("number result=",LUA_END,"(number deviceIndex)"),inArgs2,LUA_END_CALLBACK);

	simRegisterCustomLuaFunction(LUA_INFO,strConCat("string info=",LUA_INFO,"(number index)"),inArgs2,LUA_INFO_CALLBACK);

	int inArgs5[]={2,sim_lua_arg_int,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_GRAB,strConCat("number result=",LUA_GRAB,"(number index,number visionSensorHandle)"),inArgs5,LUA_GRAB_CALLBACK);

	InitializeCriticalSection(&m_cs);

	return(3); // initialization went fine, return the version number of this extension module (can be queried with simGetModuleName)
	// version 1 was for V-REP versions before V-REP 2.5.12
	// version 2 was for V-REP versions before V-REP 2.6.0
}

VREP_DLLEXPORT void v_repEnd()
{ // This is called just once, at the end of V-REP
	// Release resources here..

	// The thread should have been exited alreads (all simulations stopped!)

	unloadVrepLibrary(vrepLib); // release the library
}

VREP_DLLEXPORT void* v_repMessage(int message,int* auxiliaryData,void* customData,int* replyData)
{ // This is called quite often. Just watch out for messages/events you want to handle
	// This function should not generate any error messages:
	int errorModeSaved;
	simGetIntegerParameter(sim_intparam_error_report_mode,&errorModeSaved);
	simSetIntegerParameter(sim_intparam_error_report_mode,sim_api_errormessage_ignore);

	void* retVal=NULL;

	// Acknowledgment message display:
	// *****************************************************************
	static int auxConsoleHandleForAcknowledgmentDisplay=-1;
	static int acknowledgmentDisplayStartTime=0;
	if (displayAcknowledgment)
	{
		auxConsoleHandleForAcknowledgmentDisplay=simAuxiliaryConsoleOpen("Acknowledgments",10,2+4+16,NULL,NULL,NULL,NULL);
		simAuxiliaryConsolePrint(auxConsoleHandleForAcknowledgmentDisplay,"\nThe cam plugin contains the ESCAPI library, which is courtesy of Jari Komppa.");
		acknowledgmentDisplayStartTime=timeGetTime();
		displayAcknowledgment=false;
	}
	if ( (auxConsoleHandleForAcknowledgmentDisplay!=-1)&&(timeGetTime()-acknowledgmentDisplayStartTime>5000) )
	{
		simAuxiliaryConsoleClose(auxConsoleHandleForAcknowledgmentDisplay);
		auxConsoleHandleForAcknowledgmentDisplay=-1;
	}
	// *****************************************************************

	// Clean-up at simulation end:
	// *****************************************************************
	if (message==sim_message_eventcallback_simulationended)
	{
		if (auxConsoleHandleForAcknowledgmentDisplay!=-1)
		{
			simAuxiliaryConsoleClose(auxConsoleHandleForAcknowledgmentDisplay);
			auxConsoleHandleForAcknowledgmentDisplay=-1;
		}
		for (int i=0;i<4;i++) // for the 4 devices
		{
			while (startCountPerDevice[i]>0)
			{
				startCountOverall--;
				startCountPerDevice[i]--;
				if (startCountOverall==0)
					killThread();
			}
		}
	}
	// *****************************************************************

	simSetIntegerParameter(sim_intparam_error_report_mode,errorModeSaved); // restore previous settings
	return(retVal);
}
