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
#include "extIkSer.h"
#include "MyMath.h"

class CikEl  
{
public:
	CikEl();
	CikEl(int theTooltip);
	virtual ~CikEl();

	void initializeInitialValues(bool simulationIsRunning);
	void simulationEnded();
	CikEl* copyYourself();
	void serialize(CSer& ar);
	void serializeWExtIk(CExtIkSer& ar);
	void performGroupLoadingMapping(std::vector<int>* map);
	bool announceGroupWillBeErased(int groupID);
	void setObjectID(int newID);
	void setTooltip(int newTooltip);
	void setAlternativeBaseForConstraints(int b); 

public:
	void commonInit();
	bool announceObjectWillBeErased(int objID,bool copyBuffer);
	void performObjectLoadingMapping(std::vector<int>* map);
	int getObjectID();
	int getTooltip();
	int getBase();
	bool getActive();
	void setActive(bool isActive);
	void setBase(int newBase);
	int getAlternativeBaseForConstraints(); 
	int getTarget();
	float getMinAngularPrecision();
	float getMinLinearPrecision();
	void setMinAngularPrecision(float prec);
	void setMinLinearPrecision(float prec);
	void setConstraints(int constr);
	int getConstraints();
	void setPositionWeight(float weight);
	float getPositionWeight();
	void setOrientationWeight(float weight);
	float getOrientationWeight();

	void checkIfWithinTolerance(bool& position,bool& orientation,bool useTempValues);
	void prepareIkEquations(float interpolFact);
	void removeIkEquations();
	void getError(const C4X4Matrix& m1,const C4X4Matrix& m2,float err[2],bool xC,bool yC,bool zC,bool abC,bool gC);

	// various:
	CMatrix* matrix;
	CMatrix* errorVector;
	std::vector<int>* rowJointIDs;
	std::vector<int>* rowJointStages;

private:
	// Variables which need to be serialized and copied:
	int objectID;
	int tooltip;
	int base;
	int alternativeBaseForConstraints;
	float minAngularPrecision;
	float minLinearPrecision;
	bool active;
	int constraints; // Bit-coded: use sim_x_constraint, etc. values!
					// sim_avoidance_conjstraint is reserved for collections! Don't use it here!!
	float positionWeight;
	float orientationWeight;


};
