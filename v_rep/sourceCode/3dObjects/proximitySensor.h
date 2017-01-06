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

#include "3DObject.h"
#include "ConvexVolume.h"

class CProxSensor : public C3DObject  
{
public:

	CProxSensor();
	CProxSensor(int theType);
	virtual ~CProxSensor();

	// Following functions are inherited from 3DObject
	void display(CViewableBase* renderingObject,int displayAttrib);
	C3DObject* copyYourself();
	void scaleObject(float scalingFactor);
	void serialize(CSer& ar);
	void serializeWExtIk(CExtIkSer& ar);
	bool announceObjectWillBeErased(int objID,bool copyBuffer);
	void announceGroupWillBeErased(int groupID,bool copyBuffer);
	void announceCollisionWillBeErased(int collisionID,bool copyBuffer);
	void announceDistanceWillBeErased(int distanceID,bool copyBuffer);
	void announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer);
	void announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer);
	void performObjectLoadingMapping(std::vector<int>* map);
	void performGroupLoadingMapping(std::vector<int>* map);
	void performCollisionLoadingMapping(std::vector<int>* map);
	void performDistanceLoadingMapping(std::vector<int>* map);
	void performIkLoadingMapping(std::vector<int>* map);
	void performGcsLoadingMapping(std::vector<int>* map);
	void performTextureObjectLoadingMapping(std::vector<int>* map);
	void performDynMaterialObjectLoadingMapping(std::vector<int>* map);
	void bufferMainDisplayStateVariables();
	void bufferedMainDisplayStateVariablesToDisplay();
	void simulationAboutToStart();
	void simulationEnded();
	void initializeInitialValues(bool simulationIsRunning);
	bool getFullBoundingBox(C3Vector& minV,C3Vector& maxV);
	bool getSensingVolumeBoundingBox(C3Vector& minV,C3Vector& maxV);
	bool getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV);
	bool getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices);
	std::string getObjectTypeInfo();
	std::string getObjectTypeInfoExtended();

	// Various
	void setSensableObject(int objectID);
	int getSensableObject();

	bool handleSensor(bool handledByWorkThread,bool exceptExplicitHandling,int& detectedObjectHandle,C3Vector& detectedNormalVector);
	void resetSensor(bool exceptExplicitHandling);
	int readSensor(C3Vector& detectPt,int& detectedObjectHandle,C3Vector& detectedNormalVector);

	void commonInit();
	bool getSensedData(C3Vector& pt);
	void setClosestObjectMode(bool closestObjMode);
	bool getClosestObjectMode();
	void setSize(float newSize);
	float getSize();

	void setAllowedNormal(float al);
	float getAllowedNormal();
	void setNormalCheck(bool check);
	bool getNormalCheck();
	bool getFrontFaceDetection();
	bool getBackFaceDetection();
	void setFrontFaceDetection(bool faceOn);
	void setBackFaceDetection(bool faceOn);
	bool getShowDetectionVolume();
	void setShowDetectionVolume(bool show);

	void setSensorType(int theType);
	int getSensorType();

	void setExplicitHandling(bool setExplicit);
	bool getExplicitHandling();

	void setSensableType(int theType);
	int getSensableType();

	void setShowVolumeWhenNotDetecting(bool s);
	bool getShowVolumeWhenNotDetecting();
	void setShowVolumeWhenDetecting(bool s);
	bool getShowVolumeWhenDetecting();

	void setCheckOcclusions(bool c);
	bool getCheckOcclusions();

	void setRandomizedDetection(bool enable);
	bool getRandomizedDetection();
	void setRandomizedDetectionSampleCount(int c);
	int getRandomizedDetectionSampleCount();
	void setRandomizedDetectionCountForDetection(int c);
	int getRandomizedDetectionCountForDetection();

	void calculateFreshRandomizedRays();
	std::vector<C3Vector>* getPointerToRandomizedRays();
	std::vector<float>* getPointerToRandomizedRayDetectionStates();

	float getCalculationTime();
	C3Vector getDetectedPoint();

	// Variables which need to be serialized & copied
	CVisualParam passiveVolumeColor;
	CVisualParam activeVolumeColor;
	CVisualParam detectionRayColor;
	CVisualParam closestDistanceVolumeColor;

	std::vector<float> cutEdges; // Only used for sensor cutting debugging

	CConvexVolume* convexVolume;

protected:

	// Variables which need to be serialized & copied
	float allowedNormal;
	float size;
	bool normalCheck;
	bool _checkOcclusions;
	bool closestObjectMode;
	bool frontFaceDetection;
	bool backFaceDetection;
	bool explicitHandling;
	bool _showVolumeWhenNotDetecting;
	bool _showVolumeWhenDetecting;
	int sensorType;
	int _sensableType;
	bool displayNormals;
	int _sensableObject; // 3DObject ID or collection ID
	C3Vector _detectedPoint;
	bool _detectedPointValid;
	bool _sensorResultValid;
	int _detectedObjectHandle;
	C3Vector _detectedNormalVector;
	int _calcTimeInMs;

	bool _randomizedDetection;
	int _randomizedDetectionSampleCount;
	int _randomizedDetectionCountForDetection;

	std::vector<C3Vector> _randomizedVectors;
	std::vector<float> _randomizedVectorDetectionStates;

	bool _initialValuesInitialized;
	bool _initialExplicitHandling;
};
