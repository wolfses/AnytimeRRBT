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
#include "extIkSer.h"
#include "MyMath.h"

class CikEl  
{
public:
	CikEl();
	CikEl(int theTooltip);
	virtual ~CikEl();


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
	CMatrix* matrix_correctJacobian;
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


public:
	void serializeRExtIk(CExtIkSer& ar);
};
