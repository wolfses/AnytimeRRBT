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

class CK3
{
public:
	CK3();
	virtual ~CK3();

	void setID(int newID);
	int getID();
	void setAssociatedObject(int objID,int objUniqueID);
	int getAssociatedObject();
	int getAssociatedObjectUniqueId();

	// Following 3 needed to keep a scene object's custom data synchronized with CK3's data 
	void synchronizeSceneObject();
	void synchronize();
	static void putK3TagToSceneObject(int objectHandle,int modelVersion,float maxVelocity,float maxAccel,float maxArmAccel);

	float getMaxVelocity();
	void setMaxVelocity(float maxVel);
	int getModelVersion();
	void setModelVersion(int version);
	float getMaxAcceleration();
	void setMaxAcceleration(float maxAccel);
	float getMaxArmAcceleration();
	void setMaxArmAcceleration(float maxAccel);

	void startOfSimulation();
	bool endOfSimulation();
	void handleSimulation();
	void checkScale();

	float getInfrared(int index);
	float getUltrasonic(int index);
	float getLineSensor(int index);
	void setVelocity(float leftWheel,float rightWheel);
	void setArmPosition(int position);
	void setGripperGap(int gap);
	float getFingerProxSensor(int index);
	int getEncoder(int index);
	void setEncoders(int leftSide,int rightSide);

protected:
	void _initialize();
	float _scaling;

	void _updateStateVisualization();
	
	int _id;						// to identify individual CK3's in the same container
	int _associatedObjectUniqueID;	// the unique ID of the scene object associated with this CK3
	int _associatedObjectID;		// the handle of the scene object associated with this CK3

	float _maxVelocity;
	float _maxAcceleration;
	float _maxArmAcceleration;
	int _modelVersion;

	float _targetVelocities[2];
	float _currentVelocities[2];
	float _cumulativeMotorAngles[2];
	float _previousMotorAngles[2];

	float _targetArmPosition;
	float _currentArmPosition;
	float _currentArmVelocity;
	float _targetGripperGap_unscaled;
	float _currentGripperGap_unscaled;

	bool _initialized;					// Whether following values are initialized:					
	int _k3MotorHandles[2];
	int _k3ColorSensorHandles[2];
	int _k3IrSensorHandles[9];
	int _k3UsSensorHandles[5];
	int _k3GripperArmMotorHandles[6];
	int _k3GripperFingerMotorHandles[3];
	int _k3GripperDistanceSensorHandles[2];
	int _k3GripperColorSensorHandles[2];
	int _k3DisplayHandle;
};
