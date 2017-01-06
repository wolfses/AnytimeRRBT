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
#include "v_rep_internal.h"
#include "GeometricConstraintSolverInt.h"
#include "GeometricConstraintSolver.h"
#include "3DObject.h"
#include "ObjCont.h"
#include "App.h"
#include "IloIlo.h"
#include "Tt.h"
#include "IKGraphObject.h"
#include "IKGraphJoint.h"
#include "IKGraphObjCont.h"
#include "GCSDefs.h"
#include <algorithm>

CGeometricConstraintSolverInt::CGeometricConstraintSolverInt(int threeDObjID,int constraintSolverObjectID)
{ // constraintSolverObjectID can be NULL (but then 'solve' can't be called!)
	_constraintSolverObjectID=constraintSolverObjectID;
	C3DObject* theObject=App::ct->objCont->getObject(threeDObjID);
	if (theObject==NULL)
		graphContainer=NULL;
	else
	{
		graphContainer=new CIKGraphObjCont();
		for (int i=0;i<int(treeBases.size());i++)
			delete treeBases[i];
		treeBases.clear();
		lastMechanismID=createGraph(*graphContainer,theObject,treeBases);
	}
}

int CGeometricConstraintSolverInt::getMechanismID()
{
	return(lastMechanismID);
}

CGeometricConstraintSolverInt::~CGeometricConstraintSolverInt()
{
	if (graphContainer!=NULL)
		delete graphContainer;
}

bool CGeometricConstraintSolverInt::solve()
{
	bool retVal=false;
	CConstraintSolverObject* gcsObj=App::ct->constraintSolver->getObject(_constraintSolverObjectID);
	if (gcsObj!=NULL)
	{
		// 1. We solve the graph:
		SGeomConstrSolverParam p;
		p.generalDamping=gcsObj->getGeneralDamping();
		p.interpolation=gcsObj->getInterpolation();
		p.maxAngularVariation=gcsObj->getMaxAngleVariation();
		p.maxLinearVariation=gcsObj->getMaxLinearVariation();
		p.maxIterations=gcsObj->getMaximumIterations();
		p.loopClosurePositionTolerance=gcsObj->getLoopClosurePositionTolerance();
		p.loopClosureOrientationTolerance=gcsObj->getLoopClosureOrientationTolerance();
		retVal=CGeometricConstraintSolver::solve(*graphContainer,p);
		if (retVal)
		{
			// 2. We put the individual trees in place:
			for (int i=0;i<int(treeBases.size());i++)
			{
				CIKGraphNode* baseTree=treeBases[i];
				C3DObject* it=App::ct->objCont->getObject(baseTree->userData1);
				C7Vector newABS;
				if (baseTree->type==IK_GRAPH_JOINT_TYPE)
					newABS=((CIKGraphJoint*)baseTree)->downObject->cumulativeTransformation;
				else
					newABS=((CIKGraphObject*)baseTree)->cumulativeTransformation;
				C7Vector oldABS(it->getCumulativeTransformationPart1());
				C7Vector diff(newABS*oldABS.getInverse());
				while (it->getParent()!=NULL)
					it=it->getParent();
				it->setLocalTransformation(diff*it->getLocalTransformationPart1());
			}
			// 3. We set the correct joint values:
			for (int i=0;i<int(graphContainer->container.size());i++)
			{
				CIKGraphNode* it=graphContainer->container[i];
				if (it->type==IK_GRAPH_JOINT_TYPE)
				{
					CJoint* act=App::ct->objCont->getJoint(it->userData1);
					CIKGraphJoint* joint=(CIKGraphJoint*)it;
					if (act->getJointType()==sim_joint_spherical_subtype)
						act->setSphericalTransformation(joint->sphericalTransformation);
					else
						act->setPosition(joint->parameter);
				}
			}
		}
	}
	return(retVal);
}

int CGeometricConstraintSolverInt::createGraph(CIKGraphObjCont& graphContainer,C3DObject* partOfMechanism,std::vector<CIKGraphNode*>& treeBases)
{ // The base object is the first object in the container. Return value is the mechanism ID
	std::vector<C3DObject*> exploredObjs;
	std::vector<C3DObject*> dummiesToFollow;
	dummiesToFollow.push_back(partOfMechanism);
	int mechanismID=-1;
	while (dummiesToFollow.size()!=0)
	{ 
		C3DObject* iterat=dummiesToFollow.back();
		dummiesToFollow.pop_back();
		if (std::find(exploredObjs.begin(),exploredObjs.end(),iterat)==exploredObjs.end())
		{ // That tree doesn't yet exist! We create it:
			int baseObjectID;
			treeBases.push_back(createTree(graphContainer,iterat,exploredObjs,dummiesToFollow,false,baseObjectID));
			if (baseObjectID>mechanismID)
				mechanismID=baseObjectID;
		}
	}
	// Now we have to link the individual parts:
	for (int i=0;i<int(graphContainer.container.size());i++)
	{
		CIKGraphNode* aNode=graphContainer.container[i];
		if (aNode->type==IK_GRAPH_OBJECT_TYPE)
		{
			CIKGraphObject* anObject=(CIKGraphObject*)aNode;
			if ( (anObject->userData0!=-1)&&(anObject->objectType==IK_GRAPH_PASSIVE_OBJECT_TYPE) )
			{
				for (int j=i+1;j<int(graphContainer.container.size());j++)
				{
					CIKGraphNode* aSNode=graphContainer.container[j];
					if (aSNode->type==IK_GRAPH_OBJECT_TYPE)
					{
						CIKGraphObject* aSObject=(CIKGraphObject*)aSNode;
						if ( (aSObject->userData0==anObject->userData0)&&(anObject->objectType==IK_GRAPH_PASSIVE_OBJECT_TYPE) )
						{ // These will become links
							anObject->elasticLinkWithObject(aSObject);
							anObject->userData0=-1;
							aSObject->userData0=-1;
							break;
						}
					}
				}
			}
		}
	}
	// Now we need to take care of joint linking through linear equations:
	for (int i=0;i<int(graphContainer.container.size());i++)
	{
		CIKGraphNode* aNode=graphContainer.container[i];
		if (aNode->type==IK_GRAPH_JOINT_TYPE)
		{
			CIKGraphJoint* aJoint=(CIKGraphJoint*)aNode;
			CJoint* theAct=App::ct->objCont->getJoint(aJoint->userData1);
			int dep=theAct->getDependencyJointID();
			CJoint* theDepAct=App::ct->objCont->getJoint(dep);
			if ( (theDepAct!=NULL)&&(theAct->getJointType()!=sim_joint_spherical_subtype)&&(theDepAct->getJointType()!=sim_joint_spherical_subtype) )
			{ // theAct is linearly dependent of the DepAct. We check if theDepAct is
				// represented in the graph:
				bool foundIt=false;
				for (int j=0;j<int(graphContainer.container.size());j++)
				{
					CIKGraphNode* a2Node=graphContainer.container[j];
					if (a2Node->userData1==dep)
					{ // Yes, present!
						float constVal=theAct->getDependencyJointFact();
						float coeff=theAct->getDependencyJointCoeff();
						aJoint->constrainJointToOtherJoint((CIKGraphJoint*)a2Node,constVal,coeff);
						foundIt=true;
						break;
					}
				}
				if (!foundIt)
				{ // The dependence graph object is not present: we fix this joint:
					float constVal=theAct->getDependencyJointFact();
					float coeff=theAct->getDependencyJointCoeff();
					float depParam=theDepAct->getPosition();
					aJoint->parameter=constVal+coeff*depParam;
					if ((!aJoint->cyclic)||(aJoint->jointType!=IK_GRAPH_REVOLUTE_JOINT_TYPE)) 
					{
						if (aJoint->parameter<aJoint->minValue)
							aJoint->parameter=aJoint->minValue;
						if (aJoint->parameter>(aJoint->minValue+aJoint->range))
							aJoint->parameter=aJoint->minValue+aJoint->range;
					}
					aJoint->disabled=true;
				}
			}
		}
	}
	return(mechanismID);
}

CIKGraphNode* CGeometricConstraintSolverInt::createTree(CIKGraphObjCont& graphContainer,C3DObject* objectOnTree,std::vector<C3DObject*>& exploredObjs,std::vector<C3DObject*>& links,bool keepShapes,int& baseObjectID)
{  // Creates a tree of linked objects (linked dummies are not followed!)
	// Return value is the base IKGraphObject of that tree
	CIKGraphNode* toBeReturned=NULL;
	while (objectOnTree->getParent()!=NULL)
		objectOnTree=objectOnTree->getParent();
	baseObjectID=objectOnTree->getID();
	std::vector<C3DObject*> objectsToExplore;
	std::vector<CIKGraphObject*> lastAdded;
	objectsToExplore.push_back(objectOnTree);
	lastAdded.push_back(NULL);
	std::vector<CIKGraphObject*> noParent;
	while (objectsToExplore.size()!=0)
	{
		C3DObject* object=objectsToExplore.back();
		objectsToExplore.pop_back();
		CIKGraphObject* lastAddedNode=lastAdded.back();
		lastAdded.pop_back();
		// 1. We have to insert this object (maybe)
		int insert=-1;
		CJoint* act=NULL;
		CDummy* dum=NULL;
		if ( (object->getObjectType()==sim_object_shape_type) )
			insert=3;
		if (object->getObjectType()==sim_object_joint_type)
		{
			act=(CJoint*)object;
			if ( (act->getJointMode()==sim_jointmode_ik)||(act->getJointMode()==sim_jointmode_ikdependent) )
				insert=0;
		}
		if (object->getObjectType()==sim_object_dummy_type)
		{
			dum=(CDummy*)object;

			if (dum->getLinkedDummyID()!=-1)
			{
				if (dum->getLinkType()==sim_dummy_linktype_gcs_loop_closure)
					insert=1;
				if (dum->getLinkType()==sim_dummy_linktype_gcs_tip)
					insert=2;
			}
		}
		if (insert!=-1)
		{
			CIKGraphObject* justInsertedObject=NULL;
			CIKGraphJoint* justInsertedJoint=NULL;
			C7Vector transf(object->getCumulativeTransformationPart1());
			if (insert==0)
			{
				if (act->getJointType()==sim_joint_revolute_subtype)
					justInsertedJoint=graphContainer.insertRevoluteJointNode(transf,act->getPosition(),act->getPositionIntervalMin(),act->getPositionIntervalRange(),act->getScrewPitch(),act->getPositionIsCyclic(),act->getIKWeight()); // added getPositionIsCyclic on 2009/07/11
				if (act->getJointType()==sim_joint_prismatic_subtype)
					justInsertedJoint=graphContainer.insertPrismaticJointNode(transf,act->getPosition(),act->getPositionIntervalMin(),act->getPositionIntervalRange(),act->getIKWeight());
				if (act->getJointType()==sim_joint_spherical_subtype)
					justInsertedJoint=graphContainer.insertBallJointNode(transf,act->getSphericalTransformation(),act->getPositionIntervalRange(),act->getIKWeight());
			}
			else if (insert==1)
			{ // if we enter in this section, it is sure the dummies are linked and the link type is GCS_LOOP_CLOSURE
				int data=dum->getID();
				if (data>dum->getLinkedDummyID())
					data=dum->getLinkedDummyID();
				justInsertedObject=graphContainer.insertPassiveObjectNode(transf);	
				justInsertedObject->userData0=data;
				links.push_back(App::ct->objCont->getDummy(dum->getLinkedDummyID()));
			}
			else if (insert==2)
			{  // if we enter in this section, it is sure the dummies are linked and the link type is GCS_TIP
				CDummy* targetD=App::ct->objCont->getDummy(dum->getLinkedDummyID());
				justInsertedObject=graphContainer.insertTipObjectNode(transf,targetD->getCumulativeTransformation());	
			}
			else if (insert==3)
			{
				justInsertedObject=graphContainer.insertPassiveObjectNode(transf);
//*********************************** IK Manipulation *********************************
				if (object->getID()==App::ct->objCont->_ikManipulationObjectID)
				{
					C7Vector tipTransf;
					C7Vector targetTransf;
					tipTransf.setIdentity();
					targetTransf.setIdentity();
					tipTransf.X=App::ct->objCont->_ikManipulationStartPosRel;
					tipTransf=object->getCumulativeTransformation()*tipTransf;
					targetTransf.X=App::ct->objCont->_ikManipulationCurrentPosAbs;
					CIKGraphObject* constr=graphContainer.insertTipObjectNode(tipTransf,targetTransf);
					justInsertedObject->linkWithObject(constr);				
				}
//*************************************************************************************
			}
			if (insert!=0)
			{
				justInsertedObject->userData1=object->getID();
				if (lastAddedNode!=NULL)
				{
					lastAddedNode->linkWithObject(justInsertedObject);
					lastAddedNode=justInsertedObject;
				}
				else
				{
					lastAddedNode=justInsertedObject;
					if (toBeReturned==NULL)
						toBeReturned=justInsertedObject;
					noParent.push_back(justInsertedObject);
				}
			}
			else
			{
				justInsertedJoint->userData1=object->getID();
				if (lastAddedNode!=NULL)
				{
					lastAddedNode->linkWithObject(justInsertedJoint->getDownIKGraphObject());
					lastAddedNode=justInsertedJoint->getTopIKGraphObject();
				}
				else
				{
					lastAddedNode=justInsertedJoint->getTopIKGraphObject();
					if (toBeReturned==NULL)
						toBeReturned=justInsertedJoint;
					noParent.push_back(justInsertedJoint->getDownIKGraphObject());
				}
			}
			exploredObjs.push_back(object);
		}
		// 2. We prepare further exploration:
		for (int i=0;i<int(object->childList.size());i++)
		{
			objectsToExplore.push_back(object->childList[i]);
			lastAdded.push_back(lastAddedNode);
		}
	}
	if (noParent.size()>1)
	{ // We have to link those objects aginst each other (happens when the base object is not inserted)
		for (int i=1;i<int(noParent.size());i++)
			noParent[0]->linkWithObject(noParent[i]);
	}
	return(toBeReturned);
}
