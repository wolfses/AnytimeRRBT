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
#include "IKGraphObject.h"
#include "IKGraphJoint.h"
#include "GCSDefs.h"

CIKGraphObject::CIKGraphObject(const C7Vector& cumulTransf,const C7Vector& targetCumulTransf)
{
	type=IK_GRAPH_OBJECT_TYPE;
	objectType=IK_GRAPH_TIP_OBJECT_TYPE;
	linkPartner=NULL;
	jointTop=false;
	jointDown=false;
	cumulativeTransformation=cumulTransf;
	targetCumulativeTransformation=targetCumulTransf;
}

CIKGraphObject::CIKGraphObject(const C7Vector& cumulTransf)
{
	type=IK_GRAPH_OBJECT_TYPE;
	objectType=IK_GRAPH_PASSIVE_OBJECT_TYPE;
	linkPartner=NULL;
	jointTop=false;
	jointDown=false;
	cumulativeTransformation=cumulTransf;
}

CIKGraphObject::~CIKGraphObject()
{

}

CIKGraphNode* CIKGraphObject::getUnexplored(int pos)
{
	for (int i=0;i<int(neighbours.size());i++)
	{
		if (neighbours[i]->explorationID==-1)
		{
			if (pos==0)
				return(neighbours[i]);
			pos--;
		}
	}
	if (linkPartner!=NULL)
	{
		if (linkPartner->explorationID==-1)
		{
			if (pos==0)
				return(linkPartner);
			pos--;
		}
	}
	return(NULL);
}

CIKGraphNode* CIKGraphObject::getNeighbour(int pos,bool noLinkNeighbour)
{
	for (int i=0;i<int(neighbours.size());i++)
	{
		if (pos==0)
			return(neighbours[i]);
		pos--;
	}
	if ( (linkPartner!=NULL)&&(!noLinkNeighbour) )
	{
		if (pos==0)
			return(linkPartner);
		pos--;
	}
	return(NULL);
}

int CIKGraphObject::getNumberOfUnexplored()
{	
	int retVal=0;
	for (int i=0;i<int(neighbours.size());i++)
	{
		if (neighbours[i]->explorationID==-1)
			retVal++;
	}
	if (linkPartner!=NULL)
	{
		if (linkPartner->explorationID==-1)
			retVal++;
	}
	return(retVal);
}

CIKGraphNode* CIKGraphObject::getNeighbourWithExplorationID(int theID)
{
	for (int i=0;i<int(neighbours.size());i++)
	{
		if (neighbours[i]->explorationID==theID)
			return(neighbours[i]);
	}
	if (linkPartner!=NULL)
	{
		if (linkPartner->explorationID==theID)
			return(linkPartner);
	}
	return(NULL);
}


CIKGraphNode* CIKGraphObject::getExploredWithSmallestExplorationID()
{
	int smallest=999999;
	CIKGraphNode* retVal=NULL;
	for (int i=0;i<int(neighbours.size());i++)
	{
		if (neighbours[i]->explorationID!=-1)
		{
			if (neighbours[i]->explorationID<smallest)
			{
				smallest=neighbours[i]->explorationID;
				retVal=neighbours[i];
			}
		}
	}
	if (linkPartner!=NULL)
	{
		if (linkPartner->explorationID!=-1)
		{
			if (linkPartner->explorationID<smallest)
			{
				smallest=linkPartner->explorationID;
				retVal=linkPartner;
			}
		}
	}
	return(retVal);
}

int CIKGraphObject::getConnectionNumber()
{
	int retVal=0;
	retVal+=neighbours.size();
	if (linkPartner!=NULL)
		retVal++;
	return(retVal);
}

bool CIKGraphObject::linkWithObject(CIKGraphObject* partner)
{
	for (int i=0;i<int(neighbours.size());i++)
	{
		if (neighbours[i]==partner)
			return(true);
	}
	if (linkPartner==partner)
		return(false);
	neighbours.push_back(partner);
	partner->neighbours.push_back(this);
	return(true);
}

bool CIKGraphObject::elasticLinkWithObject(CIKGraphObject* partner)
{
	for (int i=0;i<int(neighbours.size());i++)
	{
		if (neighbours[i]==partner)
			return(false);
	}
	if (linkPartner==partner)
		return(true);
	if (linkPartner!=NULL)
		return(false);
	linkPartner=partner;
	partner->linkPartner=this;
	objectType=IK_GRAPH_LINK_OBJECT_TYPE;
	partner->objectType=IK_GRAPH_LINK_OBJECT_TYPE;
	return(true);
}

bool CIKGraphObject::linkWithJoint(CIKGraphJoint* joint,bool top)
{
	if (top)
	{
		if (joint->topObject==this)
			return(true);
		if (joint->topObject!=NULL)
			return(false);
		if (joint->downObject==this)
			return(false);
		joint->topObject=this;
		neighbours.push_back(joint);
		jointTop=true;
		jointDown=false;
		return(true);
	}
	else
	{
		if (joint->downObject==this)
			return(true);
		if (joint->downObject!=NULL)
			return(false);
		if (joint->topObject==this)
			return(false);
		joint->downObject=this;
		neighbours.push_back(joint);
		jointTop=false;
		jointDown=true;
		return(true);
	}
}
