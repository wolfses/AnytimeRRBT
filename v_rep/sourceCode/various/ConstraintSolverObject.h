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

#include <string>
#include <vector>
#include "Ser.h"

class CConstraintSolverObject
{
public:
	CConstraintSolverObject();
	virtual ~CConstraintSolverObject();

	void initializeInitialValues(bool simulationIsRunning);
	void simulationAboutToStart();
	void simulationEnded();

	void setObjectID(int newID);
	int getObjectID();
	void setObjectName(const char* newName);
	std::string getObjectName();


	void performObjectLoadingMapping(std::vector<int>* map);
	bool announceObjectWillBeErased(int objID,bool copyBuffer);

	CConstraintSolverObject* copyYourself();
	void serialize(CSer& ar);

	void setMaximumIterations(int maxIt);
	int getMaximumIterations();
	void setInterpolation(float interpol);
	float getInterpolation();
	void setGeneralDamping(float damping);
	float getGeneralDamping();
	void setMaxAngleVariation(float maxVar);
	float getMaxAngleVariation();
	void setMaxLinearVariation(float maxVar);
	float getMaxLinearVariation();
	void setLoopClosurePositionTolerance(float tol);
	float getLoopClosurePositionTolerance();
	void setLoopClosureOrientationTolerance(float tol);
	float getLoopClosureOrientationTolerance();
	void setBase3DObject(int objID);
	int getBase3DObject();
	void setExplicitHandling(bool explHandl);
	bool getExplicitHandling();
	void selectAllObjectsInMechanism();
	float getCalculationTime();

	bool computeGcs();

private:
	int _objectID;
	std::string _objectName;
	int _base3DObject;
	bool _explicitHandling;
	int _maxIterations;
	float _interpolation;
	float _generalDamping;
	float _maxAngleVariation;
	float _maxLinearVariation;
	float _loopClosurePositionTolerance;
	float _loopClosureOrientationTolerance;
	int _calcTimeInMs;

	bool _initialValuesInitialized;
	bool _initialExplicitHandling;
};
