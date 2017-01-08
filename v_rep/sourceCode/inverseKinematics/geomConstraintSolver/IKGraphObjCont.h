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

#include "IKGraphNode.h"
#include "IKGraphObject.h"
#include "MyMath.h"

class CIKGraphObjCont  
{
public:
	CIKGraphObjCont();
	virtual ~CIKGraphObjCont();
	CIKGraphNode* getNodeFromUserData0(int userData);
	CIKGraphNode* getNodeFromUserData1(int userData);
	CIKGraphNode* getNodeFromNodeID(int nodeID);
	CIKGraphObject* insertPassiveObjectNode(C7Vector& transformation);
	CIKGraphObject* insertTipObjectNode(const C7Vector& transformation,const C7Vector& targetTransformation);
	CIKGraphJoint* insertRevoluteJointNode(C7Vector& transformation,float parameter,float minVal,float range,float screwPitch,bool isCyclic,float weight);
	CIKGraphJoint* insertPrismaticJointNode(C7Vector& transformation,float parameter,float minVal,float range,float weight);
	CIKGraphJoint* insertBallJointNode(C7Vector& transformation,C4Vector sphericalTransformation,float range,float weight);
	void resetExplorationIDs();
	int identifyElements();
	void putElementsInPlace();
	void replaceElementIDWithAnother(int oldID,int newID);
	void getLinkObjectsWithElementID(int elementID,std::vector<CIKGraphObject*>& links);
	void preMultiplyObjectsWithElementID(int elementID,C7Vector& transform);
	void actualizeAllTransformations();
	void setBaseObject(int nodeID);
	CIKGraphObject* getBaseObject();

	int baseObjectID;
	int nodeIDCounter;
	int numberOfElementIDs;
	void emptyContainer();

	std::vector<CIKGraphNode*> container;

private:
	void insertNode(CIKGraphNode* aNode);
	void actualizeTransformationsWithElementID(int elementID);
};
