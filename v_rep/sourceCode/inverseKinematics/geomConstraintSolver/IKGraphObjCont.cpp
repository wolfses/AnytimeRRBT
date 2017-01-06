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
#include "IKGraphObjCont.h"
#include "IKGraphObject.h"
#include "IKGraphJoint.h"
#include "GCSDefs.h"

CIKGraphObjCont::CIKGraphObjCont()
{
	nodeIDCounter=0;
	numberOfElementIDs=0;
	emptyContainer();
}

CIKGraphObjCont::~CIKGraphObjCont()
{
	emptyContainer();
}

void CIKGraphObjCont::insertNode(CIKGraphNode* aNode)
{
	container.push_back(aNode);
	aNode->nodeID=nodeIDCounter;
	nodeIDCounter++;
}

CIKGraphNode* CIKGraphObjCont::getNodeFromUserData0(int userData)
{
	for (int i=0;i<int(container.size());i++)
	{
		if (container[i]->userData0==userData)
			return(container[i]);
	}
	return(NULL);
}

CIKGraphNode* CIKGraphObjCont::getNodeFromUserData1(int userData)
{
	for (int i=0;i<int(container.size());i++)
	{
		if (container[i]->userData1==userData)
			return(container[i]);
	}
	return(NULL);
}

CIKGraphNode* CIKGraphObjCont::getNodeFromNodeID(int nodeID)
{
	for (int i=0;i<int(container.size());i++)
	{
		if (container[i]->nodeID==nodeID)
			return(container[i]);
	}
	return(NULL);
}

void CIKGraphObjCont::resetExplorationIDs()
{
	for (int i=0;i<int(container.size());i++)
		container[i]->explorationID=-1;
}

CIKGraphObject* CIKGraphObjCont::insertPassiveObjectNode(C7Vector& transformation)
{
	CIKGraphObject* it=new CIKGraphObject(transformation);
	insertNode(it);
	return(it);
}
CIKGraphObject* CIKGraphObjCont::insertTipObjectNode(const C7Vector& transformation,const C7Vector& targetTransformation)
{
	CIKGraphObject* it=new CIKGraphObject(transformation,targetTransformation);
	insertNode(it);
	return(it);
}
CIKGraphJoint* CIKGraphObjCont::insertRevoluteJointNode(C7Vector& transformation,float parameter,float minVal,float range,float screwPitch,bool isCyclic,float weight)
{
	CIKGraphObject* down=new CIKGraphObject(transformation);
	insertNode(down);
	CIKGraphObject* top=new CIKGraphObject(transformation);
	insertNode(top);
	CIKGraphJoint* joint=new CIKGraphJoint(true,parameter,minVal,range,screwPitch,isCyclic,weight);
	insertNode(joint);
	down->linkWithJoint(joint,false);
	top->linkWithJoint(joint,true);
	top->cumulativeTransformation=transformation*joint->getDownToTopTransformation();
	return(joint);
}
CIKGraphJoint* CIKGraphObjCont::insertPrismaticJointNode(C7Vector& transformation,float parameter,float minVal,float range,float weight)
{
	CIKGraphObject* down=new CIKGraphObject(transformation);
	insertNode(down);
	CIKGraphObject* top=new CIKGraphObject(transformation);
	insertNode(top);
	CIKGraphJoint* joint=new CIKGraphJoint(false,parameter,minVal,range,0.0f,false,weight);
	insertNode(joint);
	down->linkWithJoint(joint,false);
	top->linkWithJoint(joint,true);
	top->cumulativeTransformation=transformation*joint->getDownToTopTransformation();
	return(joint);
}
CIKGraphJoint* CIKGraphObjCont::insertBallJointNode(C7Vector& transformation,C4Vector sphericalTransformation,float range,float weight)
{
	CIKGraphObject* down=new CIKGraphObject(transformation);
	insertNode(down);
	CIKGraphObject* top=new CIKGraphObject(transformation);
	insertNode(top);
	CIKGraphJoint* joint=new CIKGraphJoint(sphericalTransformation,range,weight);
	insertNode(joint);
	down->linkWithJoint(joint,false);
	top->linkWithJoint(joint,true);
	top->cumulativeTransformation=transformation*joint->getDownToTopTransformation();
	return(joint);
}

void CIKGraphObjCont::emptyContainer()
{
	for (int i=0;i<int(container.size());i++)
		delete container[i];
	container.clear();
	baseObjectID=-1;
}

int CIKGraphObjCont::identifyElements()
{ // returns the number of active joints
	if (container.size()==0)
		return(0);
	for (int i=0;i<int(container.size());i++)
	{
		CIKGraphNode* it=container[i];
		it->elementID=-1;
	}
	CIKGraphNode* iterat=getBaseObject();
	if (iterat==NULL)
	{
		iterat=container[0];
		setBaseObject(container[0]->nodeID);
	}
	int currentElementID=0;
	std::vector<CIKGraphNode*> startOfNewElement;
	startOfNewElement.push_back(iterat);
	int activeJointCount=0; 
	while (startOfNewElement.size()!=0)
	{
		iterat=startOfNewElement.back();
		startOfNewElement.pop_back();
		if (iterat->elementID==-1)
		{
			std::vector<CIKGraphNode*> toExplore;
			toExplore.push_back(iterat);
			while (toExplore.size()!=0)
			{
				iterat=toExplore.back();
				toExplore.pop_back();
				iterat->elementID=currentElementID;
				if (iterat->type==IK_GRAPH_OBJECT_TYPE)
				{
					CIKGraphObject* it=(CIKGraphObject*)iterat;
					if (it->linkPartner!=NULL)
						startOfNewElement.push_back(it->linkPartner);
				}
				else 
					activeJointCount++;
				int neighNB=iterat->getConnectionNumber();
				for (int i=0;i<neighNB;i++)
				{
					CIKGraphNode* candidate=iterat->getNeighbour(i,true);
					if ( (candidate!=NULL)&&(candidate->elementID==-1) )
						toExplore.push_back(candidate);
				}
			}
			currentElementID++;
		}
	}
	numberOfElementIDs=currentElementID;
	return(activeJointCount);
}

void CIKGraphObjCont::putElementsInPlace()
{
	// Objects with elementID 0 stay in place.
	// 1. We save all element IDs:
	std::vector<int> elementsSave;
	for (int i=0;i<int(container.size());i++)
		elementsSave.push_back(container[i]->elementID);
	// 2. We put elements in place:
	std::vector<CIKGraphObject*> links;
	getLinkObjectsWithElementID(0,links);
	while (links.size()!=0)
	{
		CIKGraphObject* it=links.back();
		links.pop_back();
		if (it->linkPartner->elementID!=0)
		{
			C7Vector diff(it->cumulativeTransformation*it->linkPartner->cumulativeTransformation.getInverse());
			preMultiplyObjectsWithElementID(it->linkPartner->elementID,diff);
			replaceElementIDWithAnother(it->linkPartner->elementID,0);
			links.clear();
			getLinkObjectsWithElementID(0,links);
		}
	}
	// 3. We restore all element IDs:
	for (int i=0;i<int(container.size());i++)
		container[i]->elementID=elementsSave[i];
}

void CIKGraphObjCont::preMultiplyObjectsWithElementID(int elementID,C7Vector& transform)
{
	for (int i=0;i<int(container.size());i++)
	{
		if ( (container[i]->type==IK_GRAPH_OBJECT_TYPE)&&(container[i]->elementID==elementID) )
		{
			CIKGraphObject* it=(CIKGraphObject*)container[i];
			it->cumulativeTransformation=transform*it->cumulativeTransformation;
		}
	}
}

void CIKGraphObjCont::getLinkObjectsWithElementID(int elementID,std::vector<CIKGraphObject*>& links)
{
	links.clear();
	for (int i=0;i<int(container.size());i++)
	{
		if ( (container[i]->elementID==elementID)&&(container[i]->type==IK_GRAPH_OBJECT_TYPE) )
		{
			CIKGraphObject* it=(CIKGraphObject*)container[i];
			if (it->objectType==IK_GRAPH_LINK_OBJECT_TYPE)
				links.push_back(it);
		}
	}
}

void CIKGraphObjCont::replaceElementIDWithAnother(int oldID,int newID)
{
	for (int i=0;i<int(container.size());i++)
	{
		if (container[i]->elementID==oldID)
			container[i]->elementID=newID;
	}
}

void CIKGraphObjCont::setBaseObject(int nodeID)
{
	CIKGraphNode* it=getNodeFromNodeID(nodeID);
	if (it!=NULL)
	{
		if (it->type==IK_GRAPH_OBJECT_TYPE)
			baseObjectID=nodeID;
	}
}

CIKGraphObject* CIKGraphObjCont::getBaseObject()
{
	CIKGraphNode* it=getNodeFromNodeID(baseObjectID);
	return((CIKGraphObject*)it);
}

void CIKGraphObjCont::actualizeAllTransformations()
{
	for (int i=0;i<int(container.size());i++)
		container[i]->explorationID=-1;
	for (int i=0;i<numberOfElementIDs;i++)
		actualizeTransformationsWithElementID(i);
}

void CIKGraphObjCont::actualizeTransformationsWithElementID(int elementID)
{
	CIKGraphNode* it=NULL;
	if (elementID==0)
		it=getBaseObject();
	else
	{
		for (int i=0;i<int(container.size());i++)
		{
			if ( (container[i]->elementID==elementID)&&(container[i]->type==IK_GRAPH_OBJECT_TYPE) )
			{
				it=container[i];
				break;
			}
		}
	}
	C7Vector prevToCurrTr;
	prevToCurrTr.setIdentity();
	std::vector<C7Vector> prevToCurrTrs;
	prevToCurrTrs.push_back(prevToCurrTr);
	CIKGraphNode* prevNode=NULL;
	std::vector<CIKGraphNode*> prevNodes;
	prevNodes.push_back(prevNode);
	CIKGraphNode* currentNode=it;
	std::vector<CIKGraphNode*> currentNodes;
	currentNodes.push_back(currentNode);
	while (currentNodes.size()!=0)
	{
		prevToCurrTr=prevToCurrTrs.back();
		prevToCurrTrs.pop_back();
		prevNode=prevNodes.back();
		prevNodes.pop_back();
		currentNode=currentNodes.back();
		currentNodes.pop_back();
		C7Vector futurePrevOldTr;
		if (currentNode->explorationID==-1)
		{
			if ( (prevNode!=NULL)&&(prevNode->type==IK_GRAPH_JOINT_TYPE) )
			{ // We have : joint--->object
				//1. We prepare future explorations first:
				int i=0;
				while (currentNode->getUnexplored(i)!=NULL)
				{
					CIKGraphNode* aNode=currentNode->getUnexplored(i);
					C7Vector currentCTMI(((CIKGraphObject*)currentNode)->cumulativeTransformation.getInverse());
					C7Vector nextCTM(((CIKGraphObject*)aNode)->cumulativeTransformation);
					prevToCurrTrs.push_back(currentCTMI*nextCTM);
					prevNodes.push_back(currentNode);
					currentNodes.push_back(aNode);
					i++;
				}
				//2. We handle this object:
				currentNode->explorationID=0; // We flag this
				CIKGraphJoint* prevJoint=(CIKGraphJoint*)prevNode;
				if (prevJoint->topObject==currentNode)
					prevJoint->topObject->cumulativeTransformation=prevJoint->downObject->cumulativeTransformation*prevJoint->getDownToTopTransformation();
				else
					prevJoint->downObject->cumulativeTransformation=prevJoint->topObject->cumulativeTransformation*prevJoint->getDownToTopTransformation().getInverse();
			}
			else if (currentNode->type==IK_GRAPH_JOINT_TYPE)
			{ // We have : object--->joint
				//1. We prepare future explorations first:
				int i=0;
				while (currentNode->getUnexplored(i)!=NULL)
				{
					CIKGraphNode* aNode=currentNode->getUnexplored(i);
					prevToCurrTrs.push_back(prevToCurrTr); // The content doesn't matter here
					prevNodes.push_back(currentNode);
					currentNodes.push_back(aNode);
					i++;
				}
				//2. We handle this object:
				currentNode->explorationID=0; // We flag this
			}
			else
			{ // We have : object--->object or NULL--->object
				//1. We prepare future explorations first:
				int i=0;
				while (currentNode->getUnexplored(i)!=NULL)
				{
					CIKGraphNode* aNode=currentNode->getUnexplored(i);
					C7Vector currentCTMI(((CIKGraphObject*)currentNode)->cumulativeTransformation.getInverse());
					C7Vector nextCTM;
					if (aNode->type==IK_GRAPH_OBJECT_TYPE)
					{
						nextCTM=(((CIKGraphObject*)aNode)->cumulativeTransformation);
						// In case aNode is a joint type, nextCTM can be anything (above)
					}
					if (aNode->elementID==elementID)
					{ // We follow only same elementIDs!
						prevToCurrTrs.push_back(currentCTMI*nextCTM);
						prevNodes.push_back(currentNode);
						currentNodes.push_back(aNode);
					}
					i++;
				}
				//2. We handle this object:
				currentNode->explorationID=0; // We flag this
				if (prevNode!=NULL)
					((CIKGraphObject*)currentNode)->cumulativeTransformation=((CIKGraphObject*)prevNode)->cumulativeTransformation*prevToCurrTr;
			}
		}
	}
}
