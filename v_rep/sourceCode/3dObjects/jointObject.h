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

class CJoint : public C3DObject  
{
public:
	CJoint(); // default, use only during serialization!
	CJoint(int jointType);
	virtual ~CJoint();

	// Following functions are inherited from 3DObject
	void display(CViewableBase* renderingObject,int displayAttrib);
	C3DObject* copyYourself();
	void scaleObject(float scalingFactor);
	void serialize(CSer& ar);
	void serializeWExtIk(CExtIkSer& ar);
	void announceGroupWillBeErased(int groupID,bool copyBuffer);
	void announceCollisionWillBeErased(int collisionID,bool copyBuffer);
	void announceDistanceWillBeErased(int distanceID,bool copyBuffer);
	void announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer);
	void performIkLoadingMapping(std::vector<int>* map);
	void performGroupLoadingMapping(std::vector<int>* map);
	void performCollisionLoadingMapping(std::vector<int>* map);
	void performDistanceLoadingMapping(std::vector<int>* map);
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

	void setPositionIntervalMin(float min);	
	void setPositionIntervalRange(float range);

	void setScrewPitch(float pitch);

	void setPositionIsCyclic(bool isCyclic);

	void setExplicitHandling(bool explicitHandl);
	bool getExplicitHandling();
	void resetJoint();
	void handleJoint(float deltaTime);
	void setUnlimitedAcceleration(bool unlimited);
	bool getUnlimitedAcceleration();
	void setInvertTargetVelocityAtLimits(bool invert);
	bool getInvertTargetVelocityAtLimits();

	void setMaxAcceleration(float maxAccel);
	float getMaxAcceleration();
	void setVelocity(float vel);
	float getVelocity();
	void setTargetVelocity(float vel);
	float getTargetVelocity();

	void measureJointVelocity(float dt); // should be called just after the main script was called!!!
	float getMeasuredJointVelocity();

	// Dynamic routines:
	void setEnableDynamicMotor(bool e);
	bool getEnableDynamicMotor();
	void setDynamicMotorTargetVelocity(float v);
	float getDynamicMotorTargetVelocity();
	void setDynamicMotorUpperLimitVelocity(float v);
	float getDynamicMotorUpperLimitVelocity();
	void setDynamicMotorMaximumForce(float f);
	float getDynamicMotorMaximumForce();
	void setDynamicSecondPartIsValid(bool v);
	void setDynamicSecondPartLocalTransform(const C7Vector& tr);
	void getDynamicJointErrors(float& linear,float& angular);
	void setEnableDynamicMotorControlLoop(bool p);
	bool getEnableDynamicMotorControlLoop();

	void setEnableDynamicMotorCustomControl(bool c,const char* scriptContent);
	bool getEnableDynamicMotorCustomControl();

	void handleDynJointControl(bool init,int loopCnt,int totalLoops,float currentPos,float effort,float dynStepSize,float errorV,float& velocity,float& forceTorque);

	void setJointCallbackCallOrder(int order);
	int getJointCallbackCallOrder();

	void setEnableTorqueModulation(bool p);
	bool getEnableTorqueModulation();

	bool getHybridFunctionality();

	void setDynamicMotorPositionControlParameters(float p_param,float i_param,float d_param);
	void getDynamicMotorPositionControlParameters(float& p_param,float& i_param,float& d_param);

	void setDynamicMotorSpringControlParameters(float k_param,float c_param);
	void getDynamicMotorSpringControlParameters(float& k_param,float& c_param);


	void setDynamicMotorReflectedPosition_useOnlyFromDynamicPart(float rfp);
	void setDynamicMotorPositionControlTargetPosition(float pos);
	float getDynamicMotorPositionControlTargetPosition();
	void setHybridFunctionality(bool h);

	void addCumulativeForceOrTorque(float forceOrTorque,int countForAverage);
	bool getDynamicForceOrTorque(float& forceOrTorque);

	void setLength(float l);
	float getLength();
	void setDiameter(float d);
	float getDiameter();
	void displayJoint(int displayAttrib,bool partOne,float sizeParam);

	float getPosition_forDisplay(bool guiIsRendering);
	C4Vector getSphericalTransformation_forDisplay(bool guiIsRendering) const;



	void setDependencyJointID(int depJointID);
	void setDependencyJointCoeff(float coeff);
	void setDependencyJointFact(float fact);

	// Engine properties
	// ---------------------
	void getOdeParameters(float& odeStopERP,float& odeStopCFM,float& odeBounce,float& odeFudgeFactor,float& odeNormalCFM);
	void setOdeParameters(float odeStopERP,float odeStopCFM,float odeBounce,float odeFudgeFactor,float odeNormalCFM);

	void getBulletParameters(float& bulletStopERP,float& bulletStopCFM,float& bulletNormalCFM);
	void setBulletParameters(float bulletStopERP,float bulletStopCFM,float bulletNormalCFM);

	void getVortexFloatParams(std::vector<float>& p);
	void setVortexFloatParams(const std::vector<float>& p);
	void getVortexIntParams(std::vector<int>& p);
	void setVortexIntParams(const std::vector<int>& p);

	void copyEnginePropertiesTo(CJoint* target);

	// ---------------------

	// Variables which need to be serialized & copied
	CVisualParam colorPart1;
	CVisualParam colorPart2;

protected:

	C4Vector _sphericalTransformation_buffered;
	float _jointPosition_buffered;
	C4Vector _sphericalTransformation_forDisplay;
	float _jointPosition_forDisplay;


	// Variables needed for the reset procedure of the joint. Do not serialize
	bool _initialValuesInitialized;
	float _initialVelocity;
	float _initialPosition;
	C4Vector _initialSphericalJointTransformation;
	float _initialTargetVelocity; 
	float _initialTargetPosition;
	bool _initialDynamicMotorEnabled;
	float _initialDynamicMotorTargetVelocity;
	float _initialDynamicMotorUpperLimitVelocity;
	float _initialDynamicMotorMaximumForce;
	bool _dynamicSecondPartIsValid;
	C7Vector _dynamicSecondPartLocalTransform;
	bool _initialDynamicMotorControlLoopEnabled;
	float _initialDynamicMotorPositionControl_P;
	float _initialDynamicMotorPositionControl_I;
	float _initialDynamicMotorPositionControl_D;
	float _initialDynamicMotorSpringControl_K;
	float _initialDynamicMotorSpringControl_C;
	int _initialJointMode;
	bool _initialHybridOperation;
	bool _initialExplicitHandling;
	int _initialJointCallbackCallOrder;

	float _measuredJointVelocity_velocityMeasurement;
	float _previousJointPosition_velocityMeasurement;

	// Joint visual attributes:
	float _length;
	float _diameter;

	// Joint state and other:
	float _jointPositionForMotionHandling;
	float _velocity;
	float _targetVelocity;

	bool _explicitHandling;
	bool _unlimitedAcceleration;
	bool _invertTargetVelocityAtLimits;
	float _maxAcceleration;

	int _jointCallbackCallOrder;

	// Dynamic values:
	bool _dynamicMotorEnabled;
	float _dynamicMotorTargetVelocity;
	float _dynamicMotorUpperLimitVelocity;
	float _dynamicMotorMaximumForce;
	bool _dynamicMotorControlLoopEnabled;
	bool _dynamicMotorPositionControl_torqueModulation;
	float _dynamicMotorPositionControl_targetPosition; 
	float _dynamicMotorPositionControl_P;
	float _dynamicMotorPositionControl_I;
	float _dynamicMotorPositionControl_D;
	float _dynamicMotorSpringControl_K;
	float _dynamicMotorSpringControl_C;
	bool _dynamicMotorCustomControl;
	bool _jointHasHybridFunctionality;

	float _dynamicMotorPIDCumulativeErrorForIntegralParameter;
	float _dynamicMotorPIDLastErrorForDerivativeParameter;

	// Engine properties
	// ---------------------
	float _odeStopERP;
	float _odeStopCFM;
	float _odeBounce;
	float _odeFudgeFactor;
	float _odeNormalCFM;

	float _bulletStopERP;
	float _bulletStopCFM;
	float _bulletNormalCFM;

	std::vector<float> _vortexFloatParams;
	std::vector<int> _vortexIntParams;
	// ------------------

	// Dynamic values, not serialized:
	float _cumulatedForceOrTorque;
	float _cumulativeForceOrTorqueTmp;


public:

	void commonInit();

	bool announceObjectWillBeErased(int objID,bool copyBuffer);
	void announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer);

	void performObjectLoadingMapping(std::vector<int>* map);

	float getPosition(bool useTempValues=false);
	void setPosition(float parameter,bool useTempValues=false);
	float getPosition_ratio();
	void setPosition_ratio(float posRatio);

	void initializeParametersForIK(float angularJointLimitationThreshold);
	int getDoFs();
	void getLocalTransformationExPart1(C7Vector& mTr,int index,bool useTempValues=false);
	float getTempParameterEx(int index);
	void setTempParameterEx(float parameter,int index);
	void applyTempParametersEx();
	int getTempSphericalJointLimitations();

	float getScrewPitch() const;
	void setSphericalTransformation(const C4Vector& tr);
	C4Vector getSphericalTransformation() const;
	int getJointType();
	float getPositionIntervalMin();
	float getPositionIntervalRange();

	bool getPositionIsCyclic();

	float getIKWeight();
	void setIKWeight(float newWeight);
	void setMaxStepSize(float stepS);
	float getMaxStepSize();	

	void _rectifyDependentJoints();

	void setJointMode(int theMode,bool correctDynMotorTargetPositions=true);
	int getJointMode();
	int getDependencyJointID();
	float getDependencyJointCoeff();
	float getDependencyJointFact();

	// Various
	std::vector<CJoint*> directDependentJoints;

protected:
	// Variables which need to be serialized & copied
	// Main joint attributes:
	int _jointType;
	C4Vector _sphericalTransformation;
	bool _positionIsCyclic;
	float _screwPitch;
	float _jointMinPosition;
	float _jointPositionRange;

	// Joint state and other:
	float _jointPosition;

	// IK and path planning calculation:
	float _maxStepSize;

	// General IK calculation:
	float _ikWeight;

	int _jointMode;
	int _dependencyJointID;
	float _dependencyJointCoeff;
	float _dependencyJointFact;

	// Temporary values used when doing IK:
	float _jointPosition_tempForIK;
	float _sphericalTransformation_euler1TempForIK;
	float _sphericalTransformation_euler2TempForIK;
	float _sphericalTransformation_euler3TempForIK;
	int _sphericalTransformation_eulerLockTempForIK; // bit-coded, bit0--> _sphericalTransformation_euler1TempForIK, bit1--> _sphericalTransformation_euler2TempForIK, etc.



};
