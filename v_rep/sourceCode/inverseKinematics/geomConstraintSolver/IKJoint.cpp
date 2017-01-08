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
#include "IKJoint.h"
#include "GCSDefs.h"

CIKJoint::CIKJoint(CIKGraphJoint* joint,C7Vector& localTransformationPart1,bool isTopSpherical,bool isSphericalUp)
{
	graphJoint=joint;
	active=true;
	topJoint=NULL;
	objectType=IK_JOINT_TYPE;
	spherical=(joint->jointType==IK_GRAPH_SPHERICAL_JOINT_TYPE);
	revolute=(joint->jointType==IK_GRAPH_REVOLUTE_JOINT_TYPE)||spherical;
	topSpherical=isTopSpherical;
	sphericalUp=isSphericalUp;
	weight=joint->weight;
	cyclic=joint->cyclic;
	if (spherical)
	{
		parameter=0.0f;
		minValue=0.0f;
		range=joint->range;
		screwPitch=0.0f;
	}
	else
	{
		parameter=joint->parameter;
		minValue=joint->minValue;
		range=joint->range;
		if (joint->jointType==IK_GRAPH_REVOLUTE_JOINT_TYPE)
			screwPitch=joint->screwPitch;
		else
			screwPitch=0.0f;
	}
	tempParameter=parameter;
	transformation=localTransformationPart1;
	avatarParent=NULL;
	avatarKids.clear();

}

CIKJoint::~CIKJoint()
{

}

void CIKJoint::addAvatar(CIKJoint* avatar)
{ // This is the avatar parent
	avatarKids.push_back(avatar);
	avatar->avatarParent=this;
}

void CIKJoint::copyStateToAvatarKids()
{
	for (int i=0;i<int(avatarKids.size());i++)
	{
		avatarKids[i]->parameter=parameter;
		avatarKids[i]->tempParameter=tempParameter;
		avatarKids[i]->active=active;
	}
}

float CIKJoint::getValueOverLimitation(bool deactivate)
{
	if (!active)
	{ // The case (!active)&&spherical&&topSpherical&&(!sphericalUp) should not return here!
		if (!spherical)
			return(-1.0f); // No limitation violation
		if (!topSpherical)
			return(-1.0f);
		if (sphericalUp)
			return(-1.0f);
	}
	if (spherical)
	{
		if (!topSpherical)
			return(-1.0f);
		if (range>179.9f*degToRad)
			return(-1.0f);
		if (sphericalUp)
		{ // Bottom-up
			if (!parent->active)
				return(-1.0f); // Was already deactivated
			C4Vector rotYm90(C3Vector(0.0f,-piValue/2.0f,0.0f));
			C4Vector tr0(C3Vector(parent->parent->tempParameter,parent->tempParameter,0.0f));
			C4Vector tr(C3Vector(parent->parent->tempParameter+parent->parent->probableDeltaValue,parent->tempParameter+parent->probableDeltaValue,0.0f));
			C4Vector sphericalTr(parent->parent->transformation.Q*rotYm90);
			tr0=sphericalTr*tr0;
			tr=sphericalTr*tr;
			C3X3Matrix m0(tr0.getMatrix());
			C3X3Matrix m(tr.getMatrix());
			C3Vector z0(m0(0,2),m0(1,2),m0(2,2));
			C3Vector z(m(0,2),m(1,2),m(2,2));
			C3Vector zVertical(0.0f,0.0f,1.0f);
			C4Vector zVerticalToZ0(zVertical,z0);
			C4Vector zVerticalToZ(zVertical,z);
			C4Vector angleAndAxis0(zVerticalToZ0.getAngleAndAxis());
			C4Vector angleAndAxis(zVerticalToZ.getAngleAndAxis());
			float p=angleAndAxis(0);
			float deltaV=p-angleAndAxis0(0);
			float tolerance=range*0.0001f;
			bool respected=true;
			if (p>range+tolerance)
			{
				p=range;
				respected=false;
			}
			if (!respected)
			{
				float retVal=fabs((p-angleAndAxis0(0)+deltaV)/deltaV);
				if (deactivate)
				{
					tr.setAngleAndAxis(p,C3Vector(angleAndAxis(1),angleAndAxis(2),angleAndAxis(3)));
					tr=sphericalTr.getInverse()*tr;
					C3Vector euler(tr.getEulerAngles());
					parent->tempParameter=euler(1);
					parent->active=false;
					parent->copyStateToAvatarKids();
					parent->parent->tempParameter=euler(0);
					parent->parent->active=false;
					parent->parent->copyStateToAvatarKids();
					parent->parent->parent->active=true;
					parent->parent->parent->copyStateToAvatarKids();
				}
				return(retVal);
			}
		}
		else
		{ // Top-down
			if (active)
				return(-1.0f); // Was already deactivated
			C4Vector rotY180(C3Vector(0.0f,piValue,0.0f));
			C4Vector rotXm90(C3Vector(-piValue/2.0f,0.0f,0.0f));
			C4Vector rotZ90(C3Vector(0.0f,0.0f,piValue/2.0f));
			C4Vector tr0(C3Vector(parent->tempParameter,parent->parent->tempParameter,0.0f));
			C4Vector tr(C3Vector(parent->tempParameter+parent->probableDeltaValue,parent->parent->tempParameter+parent->parent->probableDeltaValue,0.0f));
			C4Vector sphericalTr((rotZ90*rotXm90*transformation.Q*rotY180).getInverse());
			tr0=sphericalTr*tr0;
			tr=sphericalTr*tr;
			C3X3Matrix m0(tr0.getMatrix());
			C3X3Matrix m(tr.getMatrix());
			C3Vector z0(m0(0,2),m0(1,2),m0(2,2));
			C3Vector z(m(0,2),m(1,2),m(2,2));
			C3Vector zVertical(0.0f,0.0f,1.0f);
			C4Vector zVerticalToZ0(zVertical,z0);
			C4Vector zVerticalToZ(zVertical,z);
			C4Vector angleAndAxis0(zVerticalToZ0.getAngleAndAxis());
			C4Vector angleAndAxis(zVerticalToZ.getAngleAndAxis());
			float p=angleAndAxis(0);
			float deltaV=p-angleAndAxis0(0);
			float tolerance=range*0.0001f;
			bool respected=true;
			if (p>range+tolerance)
			{
				p=range;
				respected=false;
			}
			if (!respected)
			{
				float retVal=fabs((p-angleAndAxis0(0)+deltaV)/deltaV);
				if (deactivate)
				{
					tr.setAngleAndAxis(p,C3Vector(angleAndAxis(1),angleAndAxis(2),angleAndAxis(3)));
					tr=sphericalTr.getInverse()*tr;
					C3Vector euler(tr.getEulerAngles());
					active=true;
					copyStateToAvatarKids();
					parent->tempParameter=euler(0);
					parent->active=false;
					parent->copyStateToAvatarKids();
					parent->parent->tempParameter=euler(1);
					parent->parent->active=false;
					parent->parent->copyStateToAvatarKids();
				}
				return(retVal);
			}
		}
		return(-1.0f);
	}
	else
	{
		float p=tempParameter+probableDeltaValue;
		float tolerance=range*0.0001f;
		bool respected=true;
		if (revolute)
		{
			if ( (range<359.9f*degToRad)&&(!cyclic) )
			{
				if (p<minValue-tolerance)
				{
					p=minValue;
					respected=false;
				}
				else if (p>minValue+range+tolerance)
				{
					p=minValue+range;
					respected=false;
				}
			}
		}
		else
		{
			if (p<minValue-tolerance)
			{
				p=minValue;
				respected=false;
			}
			else if (p>minValue+range+tolerance)
			{
				p=minValue+range;
				respected=false;
			}
		}
		if (!respected)
		{
			float retVal=fabs((p-tempParameter+probableDeltaValue)/probableDeltaValue);
			if (deactivate)
			{
				tempParameter=p;
				active=false;
				copyStateToAvatarKids();
			}
			return(retVal);
		}
		return(-1.0f); // Respected
	}
}

