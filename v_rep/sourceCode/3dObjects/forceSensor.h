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

class CForceSensor : public C3DObject  
{
public:
	CForceSensor();
	virtual ~CForceSensor();

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
	bool getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV);
	bool getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices);
	std::string getObjectTypeInfo();
	std::string getObjectTypeInfoExtended();

	void commonInit();

	// Dynamic routines:
	void setDynamicSecondPartIsValid(bool v);
	void setDynamicSecondPartLocalTransform(const C7Vector& tr);

	void addCumulativeForcesAndTorques(const C3Vector& f,const C3Vector& t,int countForAverage);

	bool getDynamicForces(C3Vector& f);
	bool getDynamicTorques(C3Vector& t);

	void setForceSensorIsBroken();
	bool getForceSensorIsBroken();
	float getPositionError();
	float getOrientationError();
	
	void setForceThreshold(float t);
	float getForceThreshold();
	void setTorqueThreshold(float t);
	float getTorqueThreshold();
	void setEnableForceThreshold(bool e);
	bool getEnableForceThreshold();
	void setEnableTorqueThreshold(bool e);
	bool getEnableTorqueThreshold();
	void setConsecutiveThresholdViolationsForBreaking(int count);
	int getConsecutiveThresholdViolationsForBreaking();
	C7Vector getDynamicSecondPartLocalTransform();

	void setValueCountForFilter(int c);
	int getValueCountForFilter();
	void setFilterType(int t);
	int getFilterType();

	// Various
	void setSize(float s);
	float getSize();
	void displayForceSensor(int displayAttrib,bool partOne,float sizeParam);

	// Variables which need to be serialized & copied
	CVisualParam colorPart1;
	CVisualParam colorPart2;

protected:
	void _computeFilteredValues();
	void _handleSensorBreaking();
	// Variables needed for the reset procedure of the joint. Do not serialize
	bool _initialValuesInitialized;

	bool _dynamicSecondPartIsValid;
	float _forceThreshold;
	float _torqueThreshold;
	int _valueCountForFilter;
	int _filterType; //0=average, 1=median
	bool _forceThresholdEnabled;
	bool _torqueThresholdEnabled;

	bool _forceSensorIsBroken; // if false, then _unbrokenConstraintState is applied, else _brokenConstraintState is applied

	int _consecutiveThresholdViolationsForBreaking;
	int _currentThresholdViolationCount;
	C7Vector _dynamicSecondPartLocalTransform;

	// Variables which need to be serialized & copied
	// Visual attributes:
	float _forceSensorSize;

	// Dynamic values:
	std::vector<C3Vector> _cumulatedForces;
	std::vector<C3Vector> _cumulatedTorques;

	C3Vector _cumulativeForcesTmp;
	C3Vector _cumulativeTorquesTmp;

	C3Vector _filteredDynamicForces; // do not serialize! (but initialize appropriately)
	C3Vector _filteredDynamicTorques; // do not serialize! (but initialize appropriately)
	bool _filteredValuesAreValid;
};
