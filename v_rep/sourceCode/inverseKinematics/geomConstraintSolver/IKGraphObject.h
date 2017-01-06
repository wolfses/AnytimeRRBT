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
#include "IKGraphNode.h"

class CIKGraphJoint; // Forward declaration

class CIKGraphObject : public CIKGraphNode
{
public:
	CIKGraphObject();
	CIKGraphObject(const C7Vector& cumulTransf,const C7Vector& targetCumulTransf);	// Tip object
	CIKGraphObject(const C7Vector& cumulTransf);								// Passive object
	virtual ~CIKGraphObject();
	int objectType;// IK_GRAPH_LINK_OBJECT_TYPE,IK_GRAPH_TIP_OBJECT_TYPE,IK_GRAPH_PASSIVE_OBJECT_TYPE,IK_GRAPH_MESH_OBJECT_TYPE
	C7Vector cumulativeTransformation;
	std::vector<CIKGraphNode*> neighbours;

	bool linkWithObject(CIKGraphObject* partner);
	bool linkWithJoint(CIKGraphJoint* joint,bool top);
	bool elasticLinkWithObject(CIKGraphObject* partner);

	// Link-type:
	CIKGraphObject* linkPartner;
	
	// Tip-dummy:
	C7Vector targetCumulativeTransformation;

	// Passive object:
	bool jointTop;
	bool jointDown;

	CIKGraphNode* getUnexplored(int pos);
	CIKGraphNode* getNeighbour(int pos,bool noLinkNeighbour);
	CIKGraphNode* getExploredWithSmallestExplorationID();
	CIKGraphNode* getNeighbourWithExplorationID(int theID);
	int getNumberOfUnexplored();
	int getConnectionNumber();
};
