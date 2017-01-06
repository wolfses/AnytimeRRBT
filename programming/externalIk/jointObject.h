// This file is part of the EXTERNAL IK for V-REP
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The EXTERNAL IK is licensed under the terms of EITHER:
//   1. EXTERNAL IK commercial license (contact us for details)
//   2. EXTERNAL IK educational license (see below)
// 
// EXTERNAL IK educational license:
// -------------------------------------------------------------------
// The EXTERNAL IK educational license applies only to EDUCATIONAL
// ENTITIES composed by following people and institutions:
// 
// 1. Hobbyists, students, teachers and professors
// 2. Schools and universities
// 
// EDUCATIONAL ENTITIES do NOT include companies, research institutions,
// non-profit organisations, foundations, etc.
// 
// An EDUCATIONAL ENTITY may use, modify, compile and distribute the
// modified/unmodified EXTERNAL IK under following conditions:
//  
// 1. Distribution should be free of charge.
// 2. Distribution should be to EDUCATIONAL ENTITIES only.
// 3. Usage should be non-commercial.
// 4. Altered source versions must be plainly marked as such and distributed
//    along with any compiled code.
// 5. The origin of the EXTERNAL IK must not be misrepresented. you must
//    not claim that you wrote the original software.
// 
// THE EXTERNAL IK IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#pragma once

#include "extIk.h"
#include "3DObject.h"

class CJoint : public C3DObject  
{
public:
	CJoint(); // default, use only during serialization!
	CJoint(int jointType);
	virtual ~CJoint();


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

	void setJointMode(int theMode);
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



public:
	void serializeRExtIk(CExtIkSer& ar);
};
