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

#include "MyMath.h"
#include "IKDummy.h"

class CIKChain  
{
public:
	CIKChain(CIKDummy* tip,float interpolFact,int jointNbToExclude);
	virtual ~CIKChain();

	CIKDummy* tooltip;

	// various:
	CMatrix* matrix;
	CMatrix* errorVector;
	std::vector<CIKJoint*> rowJoints;
	bool chainIsValid;

private:
	CMatrix* getJacobian(CIKDummy* tooltip,C4X4Matrix& tooltipTransf,std::vector<CIKJoint*>& theRowJoints,int jointNbToExclude);
	void multiply(C4X4FullMatrix& d0,C4X4FullMatrix& dp,int index,std::vector<C4X4FullMatrix*>& allMatrices);
	void buildDeltaZRotation(C4X4FullMatrix& d0,C4X4FullMatrix& dp,float screwCoeff);
	void buildDeltaZTranslation(C4X4FullMatrix& d0,C4X4FullMatrix& dp);
};
