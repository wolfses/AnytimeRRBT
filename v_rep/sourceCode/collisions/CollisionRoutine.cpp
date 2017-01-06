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
#include "CollisionRoutine.h"
#include "DistanceRoutine.h"
#include "App.h"
#include "collDistInterface.h"
#include "IloIlo.h"


//---------------------------- GENERAL COLLISION QUERIES ---------------------------

bool CCollisionRoutine::doEntitiesCollide(int entity1ID,int entity2ID,
							std::vector<float>* intersections,int buffer[4],bool overrideCollidableFlagIfShape1,bool overrideCollidableFlagIfShape2,bool pathOrMotionPlanningRoutineCalling,bool handledByWorkThread)
{	// if entity2ID==-1, then all collidable objects are tested against entity1
	// We first check if objects are valid:
	C3DObject* object1=App::ct->objCont->getObject(entity1ID);
	C3DObject* object2=App::ct->objCont->getObject(entity2ID);
	CRegGroup* collection1=NULL;
	CRegGroup* collection2=NULL;
	if (object1==NULL)
	{
		collection1=App::ct->collections->getGroup(entity1ID);
		if (collection1==NULL)
			return(false);
	}
	if (object2==NULL)
	{
		collection2=App::ct->collections->getGroup(entity2ID);
		if ( (collection2==NULL)&&(entity2ID!=-1) )
			return(false);
	}
	if (!handledByWorkThread)
		App::ct->infoDisplay->collisionDetectionStart();
	bool collisionResult=false;
	if (object1!=NULL)
	{ // Here we have an object against..
		if (object2!=NULL)
		{ //...another object
			if ( (object1->getObjectType()==sim_object_shape_type)&&(object2->getObjectType()==sim_object_shape_type) )
			{
				collisionResult=_doesShapeCollideWithShape(
					entity1ID,entity2ID,intersections,buffer,overrideCollidableFlagIfShape1,overrideCollidableFlagIfShape2,pathOrMotionPlanningRoutineCalling);
			}
		}
		else
		{ //...a group or all other objects
			if (object1->getObjectType()==sim_object_shape_type)
			{ // Here entity2ID could be -1
				collisionResult=_doesShapeCollideWithGroup(entity1ID,entity2ID,intersections,buffer,overrideCollidableFlagIfShape1,pathOrMotionPlanningRoutineCalling);
			}
		}
	}
	else
	{ // Here we have a group against...
		if (object2!=NULL)
		{ // ...an object
			if (object2->getObjectType()==sim_object_shape_type)
			{
				collisionResult=_doesGroupCollideWithShape(entity1ID,entity2ID,intersections,buffer,overrideCollidableFlagIfShape2,pathOrMotionPlanningRoutineCalling);
			}
		}
		else
		{ // ...another group (or all other objects) (entity2ID could be -1)
			collisionResult=_doesGroupCollideWithGroup(entity1ID,entity2ID,intersections,buffer,pathOrMotionPlanningRoutineCalling);
		}
	}
	if (!handledByWorkThread)
		App::ct->infoDisplay->collisionDetectionEnd(collisionResult);
	return(collisionResult);
}

//----------------------------------------------------------------------------------


bool CCollisionRoutine::_doesShapeCollideWithGroup(int shapeID,int groupID,
						std::vector<float>* intersections,int buffer[3],bool overrideShapeCollidableFlag,bool pathOrMotionPlanningRoutineCalling)
{ // groupID can be -1, in which case all collidable objects are tested against the shape
	int auxBuffer[3]={buffer[1],buffer[2],buffer[0]};
	if (_doesGroupCollideWithShape(groupID,shapeID,intersections,auxBuffer,overrideShapeCollidableFlag,pathOrMotionPlanningRoutineCalling))
	{
		buffer[0]=auxBuffer[2];
		buffer[1]=auxBuffer[0];
		buffer[2]=auxBuffer[1];
		return(true);
	}
	return(false);
}

bool CCollisionRoutine::_doesShapeCollideWithShape(int shape1ID,int shape2ID,
						std::vector<float>* intersections,int buffer[2],bool overrideShape1CollidableFlag,bool overrideShape2CollidableFlag,bool pathOrMotionPlanningRoutineCalling)
{	// if intersections is different from NULL we check for all collisions and
	// append intersection segments to the vector.
	CShape* shape1=App::ct->objCont->getShape(shape1ID);
	CShape* shape2=App::ct->objCont->getShape(shape2ID);
	if ( (shape1==NULL)||(shape2==NULL) )
		return(false);
	// We never check a shape against itself!!
	if (shape1==shape2)
		return(false);
	if ( ( (shape1->getMainProperty()&sim_objectspecialproperty_collidable)==0 )&&(!overrideShape1CollidableFlag) )
		return(false);
	if ( ( (shape2->getMainProperty()&sim_objectspecialproperty_collidable)==0 )&&(!overrideShape2CollidableFlag) )
		return(false);
	if (pathOrMotionPlanningRoutineCalling)
	{ 
		if ( ((shape1->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)&&(!overrideShape1CollidableFlag))||((shape2->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)&&(!overrideShape2CollidableFlag)) )
			return(false);
	}
	// Build the collision nodes only when needed. So do it right here!
	shape1->initializeCalculationStructureIfNeeded();
	shape2->initializeCalculationStructureIfNeeded();
	if (intersections==NULL)
	{ // We don't want all intersections, we try to use the buffered triangles:
		if (_getBufferedCollision(shape1,shape2,buffer))
			return(true);
	}
	return(shape1->doesShapeCollideWithShape(shape2,intersections,buffer));
}

bool CCollisionRoutine::_doesGroupCollideWithShape(int groupID,int shapeID,
						std::vector<float>* intersections,int buffer[3],bool overrideShapeCollidableFlag,bool pathOrMotionPlanningRoutineCalling)
{	// if intersections is different from NULL we check for all collisions and
	// append intersection segments to the vector.
	// groupID can be -1, in which case all collidable objects are tested against shapeID
	int propMask=sim_objectspecialproperty_collidable;
	// We get the single shape and all shapes in the group:
	CShape* shape=App::ct->objCont->getShape(shapeID);
	if (shape==NULL)
		return(false);
	if ( ( (shape->getMainProperty()&sim_objectspecialproperty_collidable)==0 )&&(!overrideShapeCollidableFlag) )
		return(false);
	if (pathOrMotionPlanningRoutineCalling&&(shape->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)&&(!overrideShapeCollidableFlag))
		return(false);
	std::vector<C3DObject*> group;
	if (groupID==-1)
	{ // Special group here (all objects except the shape):
		std::vector<C3DObject*> exceptionShape;
		exceptionShape.push_back(shape);
		App::ct->objCont->getAllShapesFromSceneExcept(exceptionShape,group,propMask,pathOrMotionPlanningRoutineCalling);
	}
	else
	{ // Regular group here:
		if (!App::ct->collections->getShapesFromGroup(groupID,&group,propMask,pathOrMotionPlanningRoutineCalling))
			return(false);
	}
	// Build the collision nodes only when needed. So do it right here!
	for (int i=0;i<int(group.size());i++)
	{
		if (group[i]->getObjectType()==sim_object_shape_type)
			((CShape*)group[i])->initializeCalculationStructureIfNeeded();
	}
	shape->initializeCalculationStructureIfNeeded();
	
	// This part is for handling the cached collision:
	if (intersections==NULL)
	{ // We don't want all intersections
		C3DObject* buffTree=App::ct->objCont->getObject(buffer[0]);
		if (buffTree!=NULL)
		{	// We check if buffTree is in the group:
			for (int i=0;i<int(group.size());i++)
			{
				if (group[i]==buffTree)
				{ // Is in the group
					if ( (buffTree->getObjectType()==sim_object_shape_type)&&(buffTree!=shape) )
					{ // we never check a shape against itself. Shouldn't happen normally if
						// buffer is initialized at -1!
						if (_getBufferedCollision((CShape*)buffTree,shape,buffer+1))
							return(true);
						if (((CShape*)buffTree)->doesShapeCollideWithShape(shape,intersections,buffer))
							return(true);
					}
					group.erase(group.begin()+i); // We remove the checked element...
					break;	// ...and leave here
				}
			}
		}
	}

	bool returnValue=false;
	for (int i=0;i<int(group.size());i++)
	{
		C3DObject* it=group[i];
		if ( (it->getObjectType()==sim_object_shape_type)&&(it!=shape) )
		{ // Shape - shape (a shape is never checked against itself!)
			if (((CShape*)it)->doesShapeCollideWithShape(shape,intersections,buffer+1))
			{
				buffer[0]=it->getID();
				if (intersections==NULL)
					return(true);
				returnValue=true;
			}
		}
	}
	return(returnValue);
}

bool CCollisionRoutine::_doesGroupCollideWithGroup(int group1ID,int group2ID,
						std::vector<float>* intersections,int buffer[4],bool pathOrMotionPlanningRoutineCalling)
{	// if intersections is different from NULL we check for all collisions and
	// append intersection segments to the vector.
	// group2ID can be -1, in which case all collidable objects are tested against group1ID
	int propMask=sim_objectspecialproperty_collidable;
	// We get all shapes in both groups:
	std::vector<C3DObject*> group1;
	std::vector<C3DObject*> group2;
	if (!App::ct->collections->getShapesFromGroup(group1ID,&group1,propMask,pathOrMotionPlanningRoutineCalling))
		return(false);
	if (group2ID==-1)
	{ // Special group here: all objects except the group1
		App::ct->objCont->getAllShapesFromSceneExcept(group1,group2,propMask,pathOrMotionPlanningRoutineCalling);
	}
	else
	{ // We have a regular group here:
		if (!App::ct->collections->getShapesFromGroup(group2ID,&group2,propMask,pathOrMotionPlanningRoutineCalling))
			return(false);
	}
	// Build the collision nodes only when needed. So do it right here!
	for (int i=0;i<int(group1.size());i++)
	{
		if (group1[i]->getObjectType()==sim_object_shape_type)
			((CShape*)group1[i])->initializeCalculationStructureIfNeeded();
	}
	for (int i=0;i<int(group2.size());i++)
	{
		if (group2[i]->getObjectType()==sim_object_shape_type)
			((CShape*)group2[i])->initializeCalculationStructureIfNeeded();
	}

	std::vector<C3DObject*> objPairs; // Object pairs we need to check
	for (int i=0;i<int(group1.size());i++)
	{
		C3DObject* obj1=group1[i];
		int csci1=obj1->getCollectionSelfCollisionIndicator();
		for (int j=0;j<int(group2.size());j++)
		{
			C3DObject* obj2=group2[j];
			int csci2=obj2->getCollectionSelfCollisionIndicator();
			if (obj1!=obj2)
			{ // We never check an object against itself!
				if ( (group1ID!=group2ID)||(abs(csci1-csci2)!=1) )
				{ // we do not have the same collection, or the collection self collision indicators differences is not 1
					// We now check if these partners are already present in objPairs, but only if the two groups are same.
					// (otherwise it takes too much time!)
					bool absent=true;
					if (group1ID==group2ID)
					{
						for (int k=0;k<int(objPairs.size())/2;k++)
						{
							if ( ( (objPairs[2*k+0]==obj1)&&(objPairs[2*k+1]==obj2) ) ||
								( (objPairs[2*k+0]==obj2)&&(objPairs[2*k+1]==obj1) ) )
							{
								absent=false;
								break;
							}
						}
					}
					if (absent)
					{
						objPairs.push_back(obj1);
						objPairs.push_back(obj2);
					}
				}
			}
		}
	}
	
	// This part is handling the cached collision:
	if (intersections==NULL)
	{ // We don't want all intersections
		C3DObject* buffTree1=App::ct->objCont->getObject(buffer[0]);
		C3DObject* buffTree2=App::ct->objCont->getObject(buffer[2]);
		if ( (buffTree1!=NULL)&&(buffTree2!=NULL)&&(buffTree1!=buffTree2) )
		{	//We check if buffTree1 and buffTree2 are in objPairs:
			bool present=false;
			for (int kl=0;kl<int(objPairs.size())/2;kl++)
			{
				if ( (objPairs[2*kl+0]==buffTree1)&&(objPairs[2*kl+1]==buffTree2) )
				{
					present=true;
					// We remove them from the list because they will be tested right after:
					objPairs[2*kl+0]=NULL;
					break;
				}
			}
			if (present)
			{
				if (buffTree1->getObjectType()==sim_object_shape_type)
				{
					CShape* shape1=(CShape*)buffTree1;
					if (buffTree2->getObjectType()==sim_object_shape_type)
					{
						CShape* shape2=(CShape*)buffTree2;
						int auxBuffer[2]={buffer[1],buffer[3]};
						if (_getBufferedCollision(shape1,shape2,auxBuffer))
							return(true);
						// We check these shapes now:
						if (shape1->doesShapeCollideWithShape(shape2,intersections,auxBuffer))
						{
							buffer[1]=auxBuffer[0];
							buffer[3]=auxBuffer[1];
							return(true);
						}
					}
				}
			}
		}
	}

	// Here we check all shapes from the two groups against each other
	bool returnValue=false;
	for (int i=0;i<int(objPairs.size())/2;i++)
	{
		C3DObject* obj1=objPairs[2*i+0];
		C3DObject* obj2=objPairs[2*i+1];
		if (obj1!=NULL) // to take into account a removed pair
		{
			int auxBuffer[2];
			if (obj1->getObjectType()==sim_object_shape_type)
			{
				CShape* shape1=(CShape*)obj1;
				if (obj2->getObjectType()==sim_object_shape_type)
				{ // shape - shape
					CShape* shape2=(CShape*)obj2;
					if (shape1->doesShapeCollideWithShape(shape2,intersections,auxBuffer))
					{	
						buffer[0]=shape1->getID();
						buffer[1]=auxBuffer[0];
						buffer[2]=shape2->getID();
						buffer[3]=auxBuffer[1];
						returnValue=true;
						if (intersections==NULL)
							break;
					}
				}
			}
		}
	}
	return(returnValue);
}

bool CCollisionRoutine::_getBufferedCollision(CShape* shape1,CShape* shape2,int buffer[2])
{
	if (CIloIlo::collisionCachingOff)
		return(false); // Collision caching is turned off
	if ( (buffer[0]<0)||(buffer[1]<0) )
		return(false);
	if (CCollDistInterface::getCalculatedTriangleCount(shape1->geomData->collInfo)*3<=(buffer[0]*3+2))
		return(false); // that index doesn't exist
	if (CCollDistInterface::getCalculatedTriangleCount(shape2->geomData->collInfo)*3<=(buffer[1]*3+2))
		return(false); // that index doesn't exist

	C3Vector t1a;
	C3Vector t1b;
	C3Vector t1c;
	if (!CCollDistInterface::getCalculatedTriangleAt(shape1->geomData->collInfo,t1a,t1b,t1c,buffer[0]))
		return(false); // triangle was disabled
	C3Vector t2a;
	C3Vector t2b;
	C3Vector t2c;
	if (!CCollDistInterface::getCalculatedTriangleAt(shape2->geomData->collInfo,t2a,t2b,t2c,buffer[1]))
		return(false); // triangle was disabled
	C7Vector b1CTM(shape1->getCumulativeTransformationPart1());
	C7Vector b2CTM(shape2->getCumulativeTransformationPart1());
	t1a*=b1CTM;
	t1b*=b1CTM;
	t1c*=b1CTM;
	t2a*=b2CTM;
	t2b*=b2CTM;
	t2c*=b2CTM;
	t1b-=t1a;
	t1c-=t1a;
	t2b-=t2a;
	t2c-=t2a;
	int rres=(CCollDistInterface::getTriangleTriangleCollision(t1a,t1b,t1c,t2a,t2b,t2c,NULL,NULL,false));
	return(rres!=0);
}

