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

#include "extIk.h"
#include <stdio.h>
#include <stdlib.h>

extern "C" {
    #include "extApi.h"
}

int main(int argc, char* argv[])
{
	int portNb=0;
	char* motorSignalNames[3];
	if (argc>=5)
	{
		portNb=atoi(argv[1]);
		motorSignalNames[0]=argv[2];
		motorSignalNames[1]=argv[3];
		motorSignalNames[2]=argv[4];
	}
	else
	{
		printf("Indicate following arguments: 'portNumber motor1SignalName motor2SignalName motor3SignalName'!\n");
		extApi_sleepMs(5000);
		return 0;
	}

	FILE *file;
	file=fopen("irb360.ik","rb");
	unsigned char* data=NULL;
	int dataLength=0;
	if (file)
	{
		fseek(file,0,SEEK_END);
		unsigned long fl=ftell(file);
		dataLength=(int)fl;
		fseek(file,0,SEEK_SET);
		data=new unsigned char[dataLength];
		fread((char*)data,dataLength,1,file);
		fclose(file);
	}
	else
	{
		printf("The kinematic content file 'irb360.ik' could not be read!\n");
		extApi_sleepMs(5000);
		return 0;
	}

	std::string motorPosSignalName("ikDemoMotor");
	motorPosSignalName+=argv[1];

	std::string acknSignalName("ikDemoAckn");
	acknSignalName+=argv[1];

	int clientID=simxStart("127.0.0.1",portNb,true,true,2000,5);
	if (clientID!=-1)
	{
		// Initialize the embedded robot model:
		simEmbLaunch();
		int objectCount=simEmbStart(data,dataLength);
		int embeddedModelMotorHandles[3];
		int embeddedModelIkLinMotorHandles[3];
		embeddedModelMotorHandles[0]=simEmbGetObjectHandle("irb360_drivingJoint1");
		embeddedModelMotorHandles[1]=simEmbGetObjectHandle("irb360_drivingJoint2");
		embeddedModelMotorHandles[2]=simEmbGetObjectHandle("irb360_drivingJoint3");
		embeddedModelIkLinMotorHandles[0]=simEmbGetObjectHandle("irb360_cartesianX");
		embeddedModelIkLinMotorHandles[1]=simEmbGetObjectHandle("irb360_cartesianY");
		embeddedModelIkLinMotorHandles[2]=simEmbGetObjectHandle("irb360_cartesianZ");
		float embeddedModelMotorPositions[3];
		float v=0.0f;
		float a=0.0f;

		int dummyInt;
		simxGetIntegerSignal(clientID,acknSignalName.c_str(),&dummyInt,simx_opmode_streaming); // enable streaming for that signal
		int cnt=0;
		while (simxGetConnectionId(clientID)!=-1)
		{
			// Set the desired tip position:
			float x=sin(v)*0.5f;
			float y=cos(v)*0.5f;
			float z=sin(v*3.0f)*0.1f;
			simEmbSetJointPosition(embeddedModelIkLinMotorHandles[0],x*a*1.1f);
			simEmbSetJointPosition(embeddedModelIkLinMotorHandles[1],y*a*1.1f);
			simEmbSetJointPosition(embeddedModelIkLinMotorHandles[2],z*(1.0f-a));
			// calculate IK:
			simEmbHandleIkGroup(sim_handle_all);
			// Read the corresponding motor angles:
			simEmbGetJointPosition(embeddedModelMotorHandles[0],embeddedModelMotorPositions);
			simEmbGetJointPosition(embeddedModelMotorHandles[1],embeddedModelMotorPositions+1);
			simEmbGetJointPosition(embeddedModelMotorHandles[2],embeddedModelMotorPositions+2);
			// Stream motor angles:
			simxAppendStringSignal(clientID,motorPosSignalName.c_str(),(simxUChar*)embeddedModelMotorPositions,3*4,simx_opmode_oneshot);
			// Check if the other side is reading the buffer:
			cnt++;
			if (cnt>2000)
			{
				if (simxGetIntegerSignal(clientID,acknSignalName.c_str(),&dummyInt,simx_opmode_buffer)!=simx_return_ok)
					simxClearStringSignal(clientID,motorPosSignalName.c_str(),simx_opmode_oneshot); // avoid having the buffer grow and grow when nobody is reading!
				else
					simxClearIntegerSignal(clientID,acknSignalName.c_str(),simx_opmode_oneshot);
				cnt=0;
			}


			v=v+0.04f;
			if (v>10.0f)
			{
				a=a+0.0005f;
				if (a>1.0f)
					a=1.0f;
			}
			extApi_sleepMs(5);
		}
		simEmbShutDown();
		simxFinish(clientID);
	}
	return(0);
}

