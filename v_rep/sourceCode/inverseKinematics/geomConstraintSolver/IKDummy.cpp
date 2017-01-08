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

#include "vrepPrecompiledHeader.h"
#include "IKDummy.h"
#include "IKJoint.h"
#include "GCSDefs.h"

CIKDummy::CIKDummy(C7Vector& tipLocalTransformation,C7Vector& targetCumulTransformation)
{
	objectType=IK_DUMMY_TYPE;
	targetCumulativeTransformation=targetCumulTransformation;
	baseReorient.setIdentity();
	transformation=tipLocalTransformation;
	constraints=(IK_X_CONSTRAINT|IK_Y_CONSTRAINT|IK_Z_CONSTRAINT|IK_ALPHA_BETA_CONSTRAINT|IK_GAMMA_CONSTRAINT);
	dampingFactor=0.0f;
	loopClosureDummy=true;
	chainSize=0;
}

CIKDummy::~CIKDummy()
{

}

int CIKDummy::computeChainSize()
{
	chainSize=0;
	CIKJoint* it=parent;
	while (it!=NULL)
	{
		if (it->objectType==IK_JOINT_TYPE)
		{
			if (((CIKJoint*)it)->active)
				chainSize++;
		}
		it=it->parent;
	}
	return(chainSize);
}

bool CIKDummy::withinTolerance(float positionTolerance,float orientationTolerance)
{
	// We need to take into account a possible reorientation of the base
	C4X4Matrix m1((baseReorient*getCumulativeTransformation(true)).getMatrix());
	C4X4Matrix m2((baseReorient*targetCumulativeTransformation).getMatrix());
	C3Vector p(m1.X(0)-m2.X(0),m1.X(1)-m2.X(1),m1.X(2)-m2.X(2));
	C3Vector posConstr(0.0f,0.0f,0.0f);
	if ((constraints&IK_X_CONSTRAINT)!=0)
		posConstr(0)=1.0f;
	if ((constraints&IK_Y_CONSTRAINT)!=0)
		posConstr(1)=1.0f;
	if ((constraints&IK_Z_CONSTRAINT)!=0)
		posConstr(2)=1.0f;
	p(0)*=p(0);
	p(1)*=p(1);
	p(2)*=p(2);
	float posError=sqrtf(p*posConstr);
	if ( ((constraints&(IK_X_CONSTRAINT|IK_Y_CONSTRAINT|IK_Z_CONSTRAINT))!=0)&&(posError>positionTolerance) )
		return(false);

	float ori[2]={0.0f,0.0f};
	if ((constraints&IK_ALPHA_BETA_CONSTRAINT)!=0)
	{
		float z=m1.M.axis[2]*m2.M.axis[2];
		CMath::limitValue(-0.99999f,0.99999f,z);
		ori[0]=CMath::robustAcos(z);
	}
	if ((constraints&IK_GAMMA_CONSTRAINT)!=0)
	{
		float x=m1.M.axis[0]*m2.M.axis[0];
		CMath::limitValue(-0.99999f,0.99999f,x);
		ori[1]=CMath::robustAcos(x);
	}
	float orError=sqrtf(ori[0]*ori[0]+ori[1]*ori[1]);
	if ( ((constraints&(IK_ALPHA_BETA_CONSTRAINT|IK_GAMMA_CONSTRAINT))!=0)&&(orError>orientationTolerance) )
		return(false);
	return(true);
}
