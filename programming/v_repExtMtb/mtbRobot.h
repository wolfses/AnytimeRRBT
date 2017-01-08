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

#pragma once

#include <vector>
#include "outConnection.h"

class CMtbRobot
{
public:
	CMtbRobot();
	virtual ~CMtbRobot();

	void setID(int newID);
	int getID();
	void setAssociatedObject(int objID,int objUniqueID);
	int getAssociatedObject();
	int getAssociatedObjectUniqueId();

	void setProgram(const std::string& prg);
	std::string getProgram();
	void setOptions(char e);
	char getOptions();

	// Following 2 needed to keep a scene object's custom data synchronized with CMtbRobot's data 
	void attachCustomDataToSceneObject();
	void readCCustomDataFromSceneObject();

	void startOfSimulation();
	void endOfSimulation();

	int reset(float jointsInitial[4],float jointVelocityInitial[2]);
	int run(float timeStep,std::string& message);

	bool readOutput(int data[4]);
	bool readInput(int data[4]);
	bool writeInput(int data[4]);
	bool getJoints(float j[4]);

	bool addOutputInputConnection(int inputBit,int outputRobotHandle,int outputBit,int connectionType);
	void removeOutputInputConnection(int inputBit);

protected:
	bool _connect();
	void _disconnect();
	bool _SEND_compile(const std::string& inputCode,float initialJointPosition[4],float initialJointVelocityWhenMoving[2],std::string& retString);
	bool _SEND_run(unsigned char inputData[4],float deltaTime,std::string& retString);

	void _updateInputsFromConnections();

	int _id;						// to identify individual CMtbRobots
	int _associatedObjectUniqueID;	// the unique ID of the scene object associated with this CMtbRobot
	int _associatedObjectID;		// the handle of the scene object associated with this CMtbRobot

	float jointPosition_localCopy[4]; // robot joints. Those are just copies of the last values.
	unsigned char robotOutput_localCopy[4]; // output from the robot (the robot will write those), but those are just copies of the last values
	unsigned char robotInput[4]; // input to the robot (the robot can read this)

	// Following 2 used during compilation:
	float _initialJointPosition[4];
	float _initialJointVelocity[2];

	std::string _robotLanguageProgram;
	char _options;

	std::string _compilationErrorMessage;
	bool _connectionAttemptPerformed;
	bool _compiled;

	COutConnection* _connection; // socket connection

	std::vector<int> _inputOutputConnections; // indicates which input buffer bit is linked with what output buffer bit:
												// value[0+4*n] = input bit number (1-32) of this robot 
												// value[1+4*n] = handle of the robot on which we want to connect an output
												// value[2+4*n] = output bit number (1-32) of the other robot (see above) 
												// value[3+4*n] = connection type (0=same, 1=inverted) 
};
