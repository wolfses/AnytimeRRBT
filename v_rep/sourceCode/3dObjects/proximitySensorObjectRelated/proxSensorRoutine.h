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

#pragma once

#include "Shape.h"

typedef bool (*OCCLUSION_CHECK_CALLBACK)(const float pt[3]);

struct SOcclusionCheckDat
{
	bool freeSlot;
	VTHREAD_ID_TYPE threadID;
	bool occlusionCheck;
	float sensorClose;
	bool frontFace;
	bool backFace;
	std::vector<CShape*> shapes;
	std::vector<C4X4Matrix> shapeMatrices;
};

//FULLY STATIC CLASS
class CProxSensorRoutine  
{
public:
	// The main general routine:
	static bool detectEntity(int sensorID,int entityID,
		bool closestFeatureMode,bool angleLimitation,float maxAngle,C3Vector& detectedPt,
		float& dist,bool frontFace,bool backFace,int& detectedObject,float minThreshold,C3Vector& triNormal,bool overrideDetectableFlagIfNonCollection,bool checkOcclusions,bool handledByWorkThread);

	static bool randomizedDetectEntity(int sensorID,int entityID,
		bool closestFeatureMode,bool angleLimitation,float maxAngle,C3Vector& detectedPt,
		float& dist,bool frontFace,bool backFace,int& detectedObject,float minThreshold,C3Vector& triNormal,bool overrideDetectableFlagIfNonCollection,bool checkOcclusions,const std::vector<C3Vector>& normalizedRays,std::vector<float>& individualRayDetectionState,int requiredDetectionCount,bool handledByWorkThread);


	static bool detectPrimitive(int sensorID,float* vertexPointer,int itemType,int itemCount,
		bool closestFeatureMode,bool angleLimitation,float maxAngle,C3Vector& detectedPt,
		float& dist,bool frontFace,bool backFace,float minThreshold,C3Vector& triNormal);

	static bool randomizedDetectPrimitive(int sensorID,float* vertexPointer,int itemType,int itemCount,
		bool closestFeatureMode,bool angleLimitation,float maxAngle,C3Vector& detectedPt,
		float& dist,bool frontFace,bool backFace,float minThreshold,C3Vector& triNormal,const std::vector<C3Vector>& normalizedRays,std::vector<float>& individualRayDetectionState,int requiredDetectionCount);

	static bool checkForOcclusion(const float _detectedPt[3]);

private:

	static bool _detectDummy(int sensorID,int dummyID,C3Vector& detectedPt,float& dist,C3Vector& triNormalNotNormalized,bool overrideDummyDetectableFlag);
	static bool _detectShape(int sensorID,int shapeID,bool closestFeatureMode,
			bool angleLimitation,float maxAngle,C3Vector& detectedPt,float& dist,bool frontFace,bool backFace,C3Vector& triNormalNotNormalized,bool overrideShapeDetectableFlag);
	static bool _detectGroup(int sensorID,int groupID,bool closestFeatureMode,
			bool angleLimitation,float maxAngle,C3Vector& detectedPt,float& dist,bool frontFace,bool backFace,int& detectedObject,float minThreshold,C4X4Matrix& sensingVolumeBox,C3Vector& sensingVolumeSize,C3Vector& triNormalNotNormalized);



	static void _prepareShapeOcclusionCheck(int sensorID,int shapeID,bool frontFace,bool backFace,bool overrideShapeDetectableFlag);
	static void _prepareGroupOcclusionCheck(int sensorID,int groupID,bool frontFace,bool backFace);
	static void _clearOcclusionCheckData();

	static std::vector<SOcclusionCheckDat> _occlusionCheckData;
/*
	static bool _occlusionCheck;
	static float _sensorCloseForOcclusionCheck;
	static bool _frontFaceForOcclusionCheck;
	static bool _backFaceForOcclusionCheck;
	static std::vector<CShape*> _shapesForOcclusionCheck;
	static std::vector<C4X4Matrix> _shapeMatricesForOcclusionCheck;
	*/
};
