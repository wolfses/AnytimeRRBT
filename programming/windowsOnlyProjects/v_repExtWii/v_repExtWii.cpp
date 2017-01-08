// V-REP plugin "Wii" by Eric Rohmer, March 2011

#include "stdafx.h"
#include "v_repLib.h"
#include <iostream>
#include "wiimote.h"
#include "Mmsystem.h"
#include <math.h>
#include <shlwapi.h> // for the "PathRemoveFileSpec" function

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
#define LUA_START "simExtWiiStart"
#define LUA_SET "simExtWiiSet"
#define LUA_GET "simExtWiiGet"
#define LUA_END "simExtWiiEnd"
#define DEGREE_TO_RAD 0.0174533f

struct SWiiData
{
	WORD buttonStates;
	BYTE batteryState;
	float accelerationX;
	float accelerationY;
	float accelerationZ;
	float pitch;
	float roll;
	bool rumble;
	BYTE leds;
	bool initialized;
};

int startCountOverall=0;
int startCountPerDevice[4]={0,0,0,0}; // for 4 devices
volatile bool _wiiThreadLaunched=false;
wiimote* _allDevices[4]={NULL,NULL,NULL,NULL};
volatile struct SWiiData _allDeviceData[4];
CRITICAL_SECTION m_cs;
bool displayAcknowledgment=false;

LIBRARY vrepLib;

void on_state_change (wiimote &remote,state_change_flags changed,const wiimote_state &new_state)
{
	if (changed&CONNECTED)
	{
		if (new_state.ExtensionType!=wiimote::BALANCE_BOARD)
		{
			if (new_state.bExtension)
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT);
			else
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);
		}
	}
	if (changed&MOTIONPLUS_DETECTED)
	{
		if (remote.ExtensionType==wiimote_state::NONE)
		{
			bool res=remote.EnableMotionPlus();
			_ASSERT(res);
		}
	}
	else if (changed&MOTIONPLUS_EXTENSION_CONNECTED)
	{
		if (remote.MotionPlusEnabled())
			remote.DisableMotionPlus();
	}
	else if (changed&MOTIONPLUS_EXTENSION_DISCONNECTED)
	{
		if (remote.MotionPlusConnected())
			remote.EnableMotionPlus();
	}
	else if (changed&EXTENSION_CONNECTED)
	{
		if (!remote.IsBalanceBoard())
			remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT);
	}
	else if (changed&EXTENSION_DISCONNECTED)
	{
		remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);
	}
}

void killThread()
{
	_wiiThreadLaunched=false;
	while (!_wiiThreadLaunched)
		Sleep(2);
	_wiiThreadLaunched=false;
}

DWORD WINAPI _wiiThread(LPVOID lpParam)
{
	_wiiThreadLaunched=true;
	while (_wiiThreadLaunched)
	{
		static bool firstHere=true;
		if (firstHere)
			displayAcknowledgment=true;
		firstHere=false;
		EnterCriticalSection(&m_cs);
		for (int i=0;i<4;i++)
		{
			if (_allDevices[i]!=NULL)
			{
				if (!_allDeviceData[i].initialized)
				{ // If we create and connect to the device from the other thread, acceleration data won't actualize somehow...
					// So we create the object again:
					_allDevices[i]->Disconnect();
					delete _allDevices[i];
					_allDevices[i]=new wiimote();
					_allDevices[i]->ChangedCallback=on_state_change;
					_allDevices[i]->CallbackTriggerFlags=(state_change_flags)(CONNECTED|EXTENSION_CHANGED|MOTIONPLUS_CHANGED);
					_allDevices[i]->Connect(i+1);
					_allDeviceData[i].initialized=true;
				}
				_allDevices[i]->RefreshState();
				if (_allDevices[i]->ConnectionLost())
				{ // We kill the connection (and let the user reconnect if desired)
					delete _allDevices[i];
					_allDevices[i]=NULL;
					for (int j=0;j<4;j++)
					{
						startCountOverall-=startCountPerDevice[4*j+i];
						startCountPerDevice[4*j+i]=0;
					}
					if (startCountOverall==0)
					{
						LeaveCriticalSection(&m_cs);
						return(0);
					}
				}
				else
				{
					_allDeviceData[i].buttonStates=_allDevices[i]->Button.Bits;
					_allDeviceData[i].batteryState=_allDevices[i]->BatteryPercent;
					_allDeviceData[i].accelerationX=_allDevices[i]->Acceleration.X;
					_allDeviceData[i].accelerationY=_allDevices[i]->Acceleration.Y;
					_allDeviceData[i].accelerationZ=_allDevices[i]->Acceleration.Z;
					if ( (_allDevices[i]->Acceleration.Orientation.Pitch<85.0f)&&(_allDevices[i]->Acceleration.Orientation.Pitch>-85.0f) )
					{
						_allDeviceData[i].pitch=_allDevices[i]->Acceleration.Orientation.Pitch;
						_allDeviceData[i].roll=_allDevices[i]->Acceleration.Orientation.Roll;
					}
					_allDevices[i]->SetRumble(_allDeviceData[i].rumble);
					_allDevices[i]->SetLEDs(_allDeviceData[i].leds);
				}
			}
		}
		LeaveCriticalSection(&m_cs);
		Sleep(10);
	}
	for (int i=0;i<4;i++)
	{
		if (_allDevices[i]!=NULL)
		{
			_allDevices[i]->Disconnect();
			delete _allDevices[i];
			_allDevices[i]=NULL;
		}
	}
	_wiiThreadLaunched=true; // to indicate we left the thread
	return(0);
}


void LUA_START_CALLBACK(SLuaCallBack* p)
{ // the callback function of the new Lua command
	int deviceIndex=-1; // error

	if (p->inputArgCount>0)
	{ // Ok, we have at least 1 input argument
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{ // Ok, we have (at least) 1 int as argument
			int effectiveDeviceIndex=-1;
			int desiredIndex=p->inputInt[0];
			if (desiredIndex==-1)
			{ // We want any free device (not yet being used)
				// We search for a free slot:
				for (int i=0;i<4;i++)
				{
					if (_allDevices[i]==NULL)
					{
						desiredIndex=i;
						break;
					}
				}
			}
			if ( (desiredIndex>=0)&&(desiredIndex<4) )
			{
				if (_allDevices[desiredIndex]==NULL)
				{ // We connect to a new device!
					wiimote* it=new wiimote();
					it->ChangedCallback=on_state_change;
					it->CallbackTriggerFlags=(state_change_flags)(CONNECTED|EXTENSION_CHANGED|MOTIONPLUS_CHANGED);
					if (it->Connect(desiredIndex+1))
					{ // Success!
						_allDeviceData[desiredIndex].buttonStates=0;
						_allDeviceData[desiredIndex].batteryState=0;
						_allDeviceData[desiredIndex].accelerationX=0.0f;
						_allDeviceData[desiredIndex].accelerationY=0.0f;
						_allDeviceData[desiredIndex].accelerationZ=0.0f;
						_allDeviceData[desiredIndex].pitch=0.0f;
						_allDeviceData[desiredIndex].roll=0.0f;
						_allDeviceData[desiredIndex].rumble=0;
						_allDeviceData[desiredIndex].leds=0;
						_allDeviceData[desiredIndex].initialized=false;
						_allDevices[desiredIndex]=it;
					}
					else
						desiredIndex=-1; // No more devices available!
				}
			}
			if ( (desiredIndex>=0)&&(desiredIndex<4) )
			{
				if (startCountOverall==0)
				{ // Launch the thread!
					_wiiThreadLaunched=false;
					CreateThread(NULL,0,_wiiThread,NULL,THREAD_PRIORITY_NORMAL,NULL);
					while (!_wiiThreadLaunched)
						Sleep(2);
				}
				startCountOverall++;
				startCountPerDevice[desiredIndex]++;
				deviceIndex=desiredIndex;
			}
			else
				simSetLastError(LUA_START,"Invalid device index, or no available device at given index. Is device connected?"); // output an error
		}
		else
			simSetLastError(LUA_START,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_START,"Not enough arguments."); // output an error

	// Now we prepare the return value:
	p->outputArgCount=1; // 1 return value
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
	p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
	p->outputArgTypeAndSize[2*0+1]=1;				// Not used (table size if the return value was a table)
	p->outputInt=(simInt*)simCreateBuffer(1*sizeof(deviceIndex)); // 1 int return value
	p->outputInt[0]=deviceIndex; // This is the int value we want to return
}


void LUA_END_CALLBACK(SLuaCallBack* p)
{ // the callback function of the new Lua command
	int result=-1; // error

	if (p->inputArgCount>0)
	{ // Ok, we have at least 1 input argument
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{ // Ok, we have (at least) 1 int as argument
			if ( (p->inputInt[0]>=0)&&(p->inputInt[0]<4)&&(startCountPerDevice[p->inputInt[0]]>0) )
			{
				startCountOverall--;
				startCountPerDevice[p->inputInt[0]]--;
				if (startCountPerDevice[p->inputInt[0]]==0)
				{ // No one is using this device anymore... we remove it
					EnterCriticalSection(&m_cs);
					_allDevices[p->inputInt[0]]->Disconnect();
					delete _allDevices[p->inputInt[0]];
					_allDevices[p->inputInt[0]]=NULL;
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

void LUA_SET_CALLBACK(SLuaCallBack* p)
{ // the callback function of the new Lua command
	int result=-1; // error

	if (p->inputArgCount>2)
	{ // Ok, we have at least 3 input argument
		if ( (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)&&(p->inputArgTypeAndSize[1*2+0]==sim_lua_arg_int)&&(p->inputArgTypeAndSize[2*2+0]==sim_lua_arg_bool) )
		{ // Ok, we have (at least) 2 ints and a bool as argument
			if ( (p->inputInt[0]>=0)&&(p->inputInt[0]<4)&&(startCountPerDevice[p->inputInt[0]]>0) )
			{ // Ok, we have an "open" device here
				_allDeviceData[p->inputInt[0]].leds=BYTE(p->inputInt[1]);
				_allDeviceData[p->inputInt[0]].rumble=(p->inputBool[1]!=0);
				result=1;
			}
			else
				simSetLastError(LUA_SET,"Invalid device index."); // output an error
		}
		else
			simSetLastError(LUA_SET,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_SET,"Not enough arguments."); // output an error


	// Now we prepare the return value:
	p->outputArgCount=1; // 1 return value
	p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
	p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
	p->outputArgTypeAndSize[2*0+1]=1;				// Not used (table size if the return value was a table)
	p->outputInt=(simInt*)simCreateBuffer(1*sizeof(result)); // 1 int return value
	p->outputInt[0]=result; // This is the int value we want to return
}

void LUA_GET_CALLBACK(SLuaCallBack* p)
{ // the callback function of the new Lua command
	int result=-1; // error

	struct SWiiData returnData;

	if (p->inputArgCount>0)
	{ // Ok, we have at least 1 input argument
		if (p->inputArgTypeAndSize[0*2+0]==sim_lua_arg_int)
		{ // Ok, we have (at least) 1 int as argument
			if ( (p->inputInt[0]>=0)&&(p->inputInt[0]<4)&&(startCountPerDevice[p->inputInt[0]]>0) )
			{ // Ok, we have an "open" device here
				returnData.buttonStates=_allDeviceData[p->inputInt[0]].buttonStates;
				returnData.accelerationX=_allDeviceData[p->inputInt[0]].accelerationX;
				returnData.accelerationY=_allDeviceData[p->inputInt[0]].accelerationY;
				returnData.accelerationZ=_allDeviceData[p->inputInt[0]].accelerationZ;
				returnData.roll=_allDeviceData[p->inputInt[0]].roll;
				returnData.pitch=_allDeviceData[p->inputInt[0]].pitch;
				returnData.batteryState=_allDeviceData[p->inputInt[0]].batteryState;
				result=1;
			}
			else
				simSetLastError(LUA_GET,"Invalid device index."); // output an error
		}
		else
			simSetLastError(LUA_GET,"Wrong argument type/size."); // output an error
	}
	else
		simSetLastError(LUA_GET,"Not enough arguments."); // output an error

	// Now we prepare the return value(s):
	if (result==-1)
	{ // We just return -1 in case of an error
		p->outputArgCount=1; // 1 return value
		p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer
		p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
		p->outputArgTypeAndSize[2*0+1]=1;				// Not used (table size if the return value was a table)
		p->outputInt=(simInt*)simCreateBuffer(1*sizeof(result)); // 1 int return value
		p->outputInt[0]=result; // This is the int value we want to return
	}
	else
	{ // We return: result,buttons,acceleration(table_3),orientation(table_2),battery
		p->outputArgCount=5; // 5 return values
		p->outputArgTypeAndSize=(simInt*)simCreateBuffer(p->outputArgCount*5*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer

		p->outputArgTypeAndSize[2*0+0]=sim_lua_arg_int;	// The return value is an int
		p->outputArgTypeAndSize[2*0+1]=1;				// Not used (table size if the return value was a table)
		p->outputArgTypeAndSize[2*1+0]=sim_lua_arg_int;	// The return value is an int
		p->outputArgTypeAndSize[2*1+1]=1;				// Not used (table size if the return value was a table)
		p->outputArgTypeAndSize[2*2+0]=sim_lua_arg_float|sim_lua_arg_table;	// The return value is a float table
		p->outputArgTypeAndSize[2*2+1]=3;				// table size is 3
		p->outputArgTypeAndSize[2*3+0]=sim_lua_arg_float|sim_lua_arg_table;	// The return value is a float table
		p->outputArgTypeAndSize[2*3+1]=2;				// table size is 2
		p->outputArgTypeAndSize[2*4+0]=sim_lua_arg_int;	// The return value is an int
		p->outputArgTypeAndSize[2*4+1]=1;				// Not used (table size if the return value was a table)

		p->outputInt=(simInt*)simCreateBuffer(3*sizeof(int)); // 1+1+1 int return value
		p->outputInt[0]=result;
		p->outputInt[1]=int(returnData.buttonStates);
		p->outputInt[2]=int(returnData.batteryState);

		p->outputFloat=(simFloat*)simCreateBuffer(5*sizeof(float)); // 3+2 float return value
		p->outputFloat[0]=returnData.accelerationX;
		p->outputFloat[1]=returnData.accelerationY;
		p->outputFloat[2]=returnData.accelerationZ;

		p->outputFloat[3]=returnData.roll*DEGREE_TO_RAD;
		p->outputFloat[4]=returnData.pitch*DEGREE_TO_RAD;
	}
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
		std::cout << "Error, could not find or correctly load v_rep.dll. Cannot start 'Wii' plugin.\n";
		return(0); // Means error, V-REP will unload this plugin
	}
	if (getVrepProcAddresses(vrepLib)==0)
	{
		std::cout << "Error, could not find all required functions in v_rep.dll. Cannot start 'Wii' plugin.\n";
		unloadVrepLibrary(vrepLib);
		return(0); // Means error, V-REP will unload this plugin
	}

	int vrepVer;
	simGetIntegerParameter(sim_intparam_program_version,&vrepVer);
	if (vrepVer<20600) // if V-REP version is smaller than 2.06.00
	{
		printf("Sorry, your V-REP copy is somewhat old, V-REP 2.6.0 or later is required. Cannot start 'Wii' plugin.\n");
		unloadVrepLibrary(vrepLib);
		return(0); // initialization failed!!
	}

	// Register one new Lua command:
	int inArgs1[]={1,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_START,strConCat("number deviceIndex=",LUA_START,"(number deviceIndex)"),inArgs1,LUA_START_CALLBACK);

	int inArgs2[]={1,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_END,strConCat("number result=",LUA_END,"(number deviceIndex)"),inArgs2,LUA_END_CALLBACK);

	int inArgs3[]={3,sim_lua_arg_int,sim_lua_arg_int,sim_lua_arg_bool};
	simRegisterCustomLuaFunction(LUA_SET,strConCat("number result=",LUA_SET,"(number deviceIndex,number ledStates,boolean rumble)"),inArgs3,LUA_SET_CALLBACK);

	int inArgs4[]={1,sim_lua_arg_int};
	simRegisterCustomLuaFunction(LUA_GET,strConCat("number result,number buttons,table_3 accelerations,table_2 rollAndPitch,number battery=",LUA_GET,"(number deviceIndex)"),inArgs4,LUA_GET_CALLBACK);

	InitializeCriticalSection(&m_cs);

	return(3);	// initialization went fine, return the version number of this plugin!
				// version 1 was for V-REP 2.5.11 or earlier
				// version 2 was for V-REP 2.6.0 or earlier
}

VREP_DLLEXPORT void v_repEnd()
{ // This is called just once, at the end of V-REP
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
		simAuxiliaryConsolePrint(auxConsoleHandleForAcknowledgmentDisplay,"The wiimote plugin is courtesy of Eric Rohmer.\n\nThe wiimote plugin contains WiiYourself! wiimote code by gl.tter\nhttp://gl.tter.org");
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
