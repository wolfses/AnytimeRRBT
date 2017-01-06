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

#include <stdio.h>
#include <stdlib.h>
#include <ros/ros.h>
#include "../include/v_repConst.h"

// Used data structures:
#include "vrep_common/ProximitySensorData.h"
#include "vrep_common/VrepInfo.h"
#include "vrep_common/JointSetStateData.h"

// Used API services:
#include "vrep_common/simRosEnablePublisher.h"
#include "vrep_common/simRosEnableSubscriber.h"

// Global variables (modified by topic subscribers):
bool simulationRunning=true;
bool sensorTrigger=false;
float simulationTime=0.0f;

// Topic subscriber callbacks:
void infoCallback(const vrep_common::VrepInfo::ConstPtr& info)
{
	simulationTime=info->simulationTime.data;
	simulationRunning=(info->simulatorState.data&1)!=0;
}

void sensorCallback(const vrep_common::ProximitySensorData::ConstPtr& sens)
{
	// We don't care about the detected distance here, we just trigger!
	sensorTrigger=true;
}

// Main code:
int main(int argc,char* argv[])
{
	// The joint handles and proximity sensor handles are given in the argument list
	// (when V-REP launches this executable, V-REP will also provide the argument list)
	int leftMotorHandle;
	int rightMotorHandle;
	int sensorHandle;
	if (argc>=4)
	{
		leftMotorHandle=atoi(argv[1]);
		rightMotorHandle=atoi(argv[2]);
		sensorHandle=atoi(argv[3]);
	}
	else
	{
		printf("Indicate following arguments: 'leftMotorHandle rightMotorHandle sensorHandle'!\n");
		sleep(5000);
		return 0;
	}

	// Create a ROS node. The name has a random component: 
	int _argc = 0;
	char** _argv = NULL;
	struct timeval tv;
	unsigned int timeVal=0;
	if (gettimeofday(&tv,NULL)==0)
		timeVal=(tv.tv_sec*1000+tv.tv_usec/1000)&0x00ffffff;
	std::string nodeName("rosBubbleRob");
	std::string randId(boost::lexical_cast<std::string>(timeVal+int(999999.0f*(rand()/(float)RAND_MAX))));
	nodeName+=randId;		
	ros::init(_argc,_argv,nodeName.c_str());

	if(!ros::master::check())
		return(0);
	
	ros::NodeHandle node("~");	
	printf("rosBubbleRob just started with node name %s\n",nodeName.c_str());

	// 1. Let's subscribe to V-REP's info stream (that stream is the only one enabled by default,
	// and the only one that can run while no simulation is running):
	ros::Subscriber subInfo=node.subscribe("/vrep/info",1,infoCallback);

	// 2. Request V-REP to launch a publisher for the BubbleRob's proximity sensor:
	ros::ServiceClient client_enablePublisher=node.serviceClient<vrep_common::simRosEnablePublisher>("/vrep/simRosEnablePublisher");
	vrep_common::simRosEnablePublisher srv_enablePublisher;
	srv_enablePublisher.request.topicName="proxData"+randId; // the requested topic name
	srv_enablePublisher.request.queueSize=1; // the requested publisher queue size (on V-REP side)
	srv_enablePublisher.request.streamCmd=simros_strmcmd_read_proximity_sensor; // the requested publisher type
	srv_enablePublisher.request.auxInt1=sensorHandle; // some additional information the publisher needs (what proximity sensor)
    if ( client_enablePublisher.call(srv_enablePublisher)&&(srv_enablePublisher.response.effectiveTopicName.length()!=0) )
	{ 	// ok, the service call was ok, and the publisher was succesfully started on V-REP side
		// V-REP is now streaming the proximity sensor data!

		// 3. Let's subscribe to that proximity sensor data:
		std::string topicName("/vrep/");
		topicName+=srv_enablePublisher.response.effectiveTopicName; // Make sure to use the returned topic name, not the requested one (can be same)
		ros::Subscriber sub=node.subscribe(topicName.c_str(),1,sensorCallback);

		// 4. Let's tell V-REP to subscribe to the motor speed topic (publisher to that topic will be created further down):
		ros::ServiceClient client_enableSubscriber=node.serviceClient<vrep_common::simRosEnableSubscriber>("/vrep/simRosEnableSubscriber");
		vrep_common::simRosEnableSubscriber srv_enableSubscriber;

		srv_enableSubscriber.request.topicName="/"+nodeName+"/wheels"; // the topic name
		srv_enableSubscriber.request.queueSize=1; // the subscriber queue size (on V-REP side)
		srv_enableSubscriber.request.streamCmd=simros_strmcmd_set_joint_state; // the subscriber type
		if ( client_enableSubscriber.call(srv_enableSubscriber)&&(srv_enableSubscriber.response.subscriberID!=-1) )
		{	// ok, the service call was ok, and the subscriber was succesfully started on V-REP side
			// V-REP is now listening to the desired motor joint states

			// 5. Let's prepare a publisher of those motor speeds:
			ros::Publisher motorSpeedPub=node.advertise<vrep_common::JointSetStateData>("wheels",1);

			// 6. Finally we have the control loop:
			float driveBackStartTime=-99.0f;
			while (ros::ok()&&simulationRunning)
			{ // this is the control loop (very simple, just as an example)
				vrep_common::JointSetStateData motorSpeeds;
				float desiredLeftMotorSpeed;
				float desiredRightMotorSpeed;
				if (simulationTime-driveBackStartTime<3.0f)
				{ // driving backwards while slightly turning:
					desiredLeftMotorSpeed=-3.1415*0.5;
					desiredRightMotorSpeed=-3.1415*0.25;
				}
				else
				{ // going forward:
					desiredLeftMotorSpeed=3.1415;
					desiredRightMotorSpeed=3.1415;
					if (sensorTrigger)
						driveBackStartTime=simulationTime; // We detected something, and start the backward mode
					sensorTrigger=false;
				}

				// publish the motor speeds:
				motorSpeeds.handles.data.push_back(leftMotorHandle);
				motorSpeeds.handles.data.push_back(rightMotorHandle);
				motorSpeeds.setModes.data.push_back(2); // 2 is the speed mode
				motorSpeeds.setModes.data.push_back(2);
				motorSpeeds.values.data.push_back(desiredLeftMotorSpeed);
				motorSpeeds.values.data.push_back(desiredRightMotorSpeed);
				motorSpeedPub.publish(motorSpeeds);

				// handle ROS messages:
				ros::spinOnce();

				// sleep a bit:
				usleep(5000);
			}
		}
	}
	ros::shutdown();
	printf("rosBubbleRob just ended!\n");
	return(0);
}

