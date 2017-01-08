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

#include "Ser.h"
#include "ikEl.h"
#include "3DObject.h"
#include "jointObject.h"
#include "Dummy.h"

class CikGroup  
{
public:
	CikGroup();
	virtual ~CikGroup();

	void initializeInitialValues(bool simulationIsRunning);
	void simulationAboutToStart();
	void simulationEnded();

	CikGroup* copyYourself();
	void serialize(CSer& ar);
	void serializeWExtIk(CExtIkSer& ar);
	void performIkGroupLoadingMapping(std::vector<int>* map);
	void performGroupLoadingMapping(std::vector<int>* map);
	bool announceGroupWillBeErased(int groupID,bool copyBuffer);

	bool addIkElement(CikEl* anElement);
	void setObjectID(int newID);

	void setRestoreIfPositionNotReached(bool active);
	void setRestoreIfOrientationNotReached(bool active);
	void setDoOnFailOrSuccessOf(int groupID);
	void setDoOnFail(bool onFail);
	void setDoOnPerformed(bool turnOn);

	void setAvoidanceWeight(float weight);
	float getAvoidanceWeight();

	void setJointTreshholdAngular(float treshhold);
	void setJointTreshholdLinear(float treshhold);

	void setAvoidanceRobot(int universalObject);
	int getAvoidanceRobot();
	void setAvoidanceObstacle(int universalObject);
	int getAvoidanceObstacle();
	void setAvoidanceThreshold(float t);
	float getAvoidanceThreshold();

	void setIgnoreMaxStepSizes(bool ignore);

	float getCalculationTime();

private:
	void getCollisionPartners(std::vector<C3DObject*>* collPartners,std::vector<float>* treshholds,CJoint* bottomJoint,std::vector<CJoint*>* allGroupJoints);

	// Variables which need to be serialized and copied:
	float avoidanceWeight;

	int _avoidanceRobotEntity;
	int _avoidanceObstacleEntity;
	float avoidanceThreshold;

	int _calcTimeInMs;

	bool _initialValuesInitialized;
	bool _initialExplicitHandling;


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
	float getDeterminant(CMatrix& m,std::vector<int>* activeRows,std::vector<int>* activeColumns);

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

};
