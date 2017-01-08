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
#include "ikEl.h"
#include "3DObject.h"
#include "jointObject.h"
#include "Dummy.h"

class CikGroup  
{
public:
	CikGroup();
	virtual ~CikGroup();


public:
	bool announceObjectWillBeErased(int objID,bool copyBuffer);
	bool announceIkGroupWillBeErased(int ikGroupID,bool copyBuffer);
	void performObjectLoadingMapping(std::vector<int>* map);

	CikEl* getIkElement(int ikElementID);
	CikEl* getIkElementWithTooltipID(int tooltipID);
	void removeIkElement(int elementID);

	void setExplicitHandling(bool explicitHandl);
	bool getExplicitHandling();

	int getObjectID();
	std::string getObjectName();
	void setObjectName(std::string newName);
	void setMaxIterations(int maxIter);
	int getMaxIterations();
	bool getActive();
	void setDlsFactor(float theFactor);
	float getDlsFactor();
	void setCalculationMethod(int theMethod);
	int getCalculationMethod();
	bool getRestoreIfPositionNotReached();
	bool getRestoreIfOrientationNotReached();

	int getDoOnFailOrSuccessOf();
	bool getDoOnFail();
	bool getDoOnPerformed();
	void setConstraints(int constr);
	int getConstraints();
	void setJointLimitWeight(float weight);
	float getJointLimitWeight();

	float getJointTreshholdAngular();
	float getJointTreshholdLinear();
	int computeGroupIk(bool forMotionPlanning);
	void getAllActiveJoints(std::vector<CJoint*>& jointList);
	void getTipAndTargetLists(std::vector<CDummy*>& tipList,std::vector<CDummy*>& targetList);

	bool getIgnoreMaxStepSizes();
	void resetCalculationResult();
	void setCalculationResult(int res,int calcTimeInMs);
	int getCalculationResult();
	void setCorrectJointLimits(bool c);
	bool getCorrectJointLimits();

	void setActive(bool isActive);

	float* getLastJacobianData(int matrixSize[2]);
	float* getLastManipulabilityValue(int matrixSize[2]);
	float getDeterminant(const CMatrix& m,const std::vector<int>* activeRows,const std::vector<int>* activeColumns);

	// Variables which need to be serialized and copied:
	std::vector<CikEl*> ikElements;

private:
	void _resetTemporaryParameters();
	void _applyTemporaryParameters();

	int performOnePass(std::vector<CikEl*>* validElements,bool& limitOrAvoidanceNeedMoreCalculation,float interpolFact,bool forMotionPlanning);

	// Variables which need to be serialized and copied:
	int objectID;
	std::string objectName;
	int maxIterations;
	bool active;
	bool _correctJointLimits;
	float dlsFactor;
	int calculationMethod;
	bool restoreIfPositionNotReached;
	bool restoreIfOrientationNotReached;
	int doOnFailOrSuccessOf; // group identifier which success/fail will be evaluated
	bool doOnFail;
	bool doOnPerformed;
	int constraints; // only sim_avoidance_constraint is valid!
	float jointLimitWeight;
	float jointTreshholdAngular;	// in radian
	float jointTreshholdLinear;		// in meter

	bool ignoreMaxStepSizes;
	int _calculationResult;

	CMatrix* _lastJacobian;

	bool _explicitHandling;

public:
	void serializeRExtIk(CExtIkSer& ar);
};
