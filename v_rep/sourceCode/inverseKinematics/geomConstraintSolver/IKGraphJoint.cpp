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
#include "IKGraphJoint.h"
#include "GCSDefs.h"

CIKGraphJoint::CIKGraphJoint(bool isRevolute,float theParameter,float theMinVal,float theRange,float theScrewPitch,bool isCyclic,float theWeight)
{
	topObject=NULL;
	downObject=NULL;
	type=IK_GRAPH_JOINT_TYPE;
	if (isRevolute)
		jointType=IK_GRAPH_REVOLUTE_JOINT_TYPE;
	else
		jointType=IK_GRAPH_PRISMATIC_JOINT_TYPE;
	parameter=theParameter;
	minValue=theMinVal;
	range=theRange;
	cyclic=isCyclic;
	weight=theWeight;
	screwPitch=theScrewPitch;
	followedJoint=NULL;
	disabled=false;
}
CIKGraphJoint::CIKGraphJoint(C4Vector& theSphericalTr,float theRange,float theWeight)
{
	topObject=NULL;
	downObject=NULL;
	type=IK_GRAPH_JOINT_TYPE;
	jointType=IK_GRAPH_SPHERICAL_JOINT_TYPE;
	screwPitch=0.0f;
	parameter=0.0f;
	minValue=-piValue;
	range=theRange;
	cyclic=false;
	weight=theWeight;
	sphericalTransformation=theSphericalTr;
	followedJoint=NULL;
	disabled=false;
}

CIKGraphJoint::~CIKGraphJoint()
{
}

C7Vector CIKGraphJoint::getDownToTopTransformation()
{
	C7Vector retVal;
	retVal.setIdentity();
	if (jointType==IK_GRAPH_SPHERICAL_JOINT_TYPE)
		retVal.Q=sphericalTransformation;
	else if (jointType==IK_GRAPH_REVOLUTE_JOINT_TYPE)
		retVal.Q.setAngleAndAxis(parameter,C3Vector(0.0f,0.0f,1.0f));
	else if (jointType==IK_GRAPH_PRISMATIC_JOINT_TYPE)
		retVal.X(2)=parameter;
	return(retVal);
}

CIKGraphObject* CIKGraphJoint::getTopIKGraphObject()
{
	return(topObject);
}

CIKGraphObject* CIKGraphJoint::getDownIKGraphObject()
{
	return(downObject);
}

CIKGraphNode* CIKGraphJoint::getUnexplored(int pos)
{
	if (downObject->explorationID==-1)
	{
		if (pos==0)
			return(downObject);
		pos--;
	}
	if (topObject->explorationID==-1)
	{
		if (pos==0)
			return(topObject);
		pos--;
	}
	return(NULL);
}

CIKGraphNode* CIKGraphJoint::getNeighbour(int pos,bool noLinkNeighbour)
{
	if (pos==0)
		return(downObject);
	if (pos==1)
		return(topObject);
	return(NULL);
}

int CIKGraphJoint::getNumberOfUnexplored()
{	
	int retVal=0;
	if (downObject->explorationID==-1)
		retVal++;
	if (topObject->explorationID==-1)
		retVal++;
	return(retVal);
}

CIKGraphNode* CIKGraphJoint::getNeighbourWithExplorationID(int theID)
{
	if (downObject->explorationID==theID)
		return(downObject);
	if (topObject->explorationID==theID)
		return(topObject);
	return(NULL);
}


CIKGraphNode* CIKGraphJoint::getExploredWithSmallestExplorationID()
{
	int smallest=999999;
	CIKGraphObject* retVal=NULL;
	if (downObject->explorationID!=-1)
	{
		if (downObject->explorationID<smallest)
		{
			smallest=downObject->explorationID;
			retVal=downObject;
		}
	}
	if (topObject->explorationID!=-1)
	{
		if (topObject->explorationID<smallest)
		{
			smallest=topObject->explorationID;
			retVal=topObject;
		}
	}
	return(retVal);
}

int CIKGraphJoint::getConnectionNumber()
{
	return(2);
}

void CIKGraphJoint::constrainJointToOtherJoint(CIKGraphJoint* it,float constantVal,float coefficient)
{
	if (it!=NULL)
	{
		if ( (it->jointType!=IK_GRAPH_SPHERICAL_JOINT_TYPE)&&(jointType!=IK_GRAPH_SPHERICAL_JOINT_TYPE) )
		{
			followedJoint=it;
			constantValue=constantVal;
			coefficientValue=coefficient;
		}
	}
	else
		followedJoint=NULL;
}
