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

class CVisionTransf  
{
public:
	CVisionTransf(int passiveVisionSensorHandle,const int activeVisionSensorHandles[6],float horizontalAngle,float verticalAngle);
	virtual ~CVisionTransf();

	bool isActiveVisionSensorResolutionCorrect();
	bool areActiveVisionSensorsExplicitelyHandled();
	bool doAllObjectsExistAndAreVisionSensors();

	bool isSame(const int activeVisionSensorHandles[6],float horizontalAngle,float verticalAngle);

	int getPassiveVisionSensorHandle();

	void disableSpecularLightComponent(bool d);

	void handleObject();
	void releaseActiveVisionSensorImages();

private:
	void _calculateMapping();

	int _passiveVisionSensorHandle;
	int _activeVisionSensorHandles[6];
	bool _usedActiveVisionSensors[6];
	float _horizontalAngle;
	float _verticalAngle;
	int _passiveVisionSensorResolution[2];
	int _activeVisionSensorResolutionXY;

	float* _activeVisionSensorImages[6];
	float* _passiveVisionSensorImage;
	int* _mapP;
	unsigned char* _mapV;
};
