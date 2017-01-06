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
#include "DistanceRoutine.h"
#include "App.h"
#include "collDistInterface.h"
#include "IloIlo.h"

//---------------------------- GENERAL DISTANCE QUERIES ---------------------------

bool CDistanceRoutine::getDistanceBetweenEntitiesIfSmaller(int entity1ID,int entity2ID,
						float& dist,float ray[7],int buffer[4],bool overrideMeasurableFlagIfNonCollection1,bool overrideMeasurableFlagIfNonCollection2,bool pathPlanningRoutineCalling,bool handledByWorkThread)
{ // entity2ID can be -1, in which case all objects are tested against entity1
	bool returnValue=false;
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
		App::ct->infoDisplay->distanceCalculationStart();
	if (object1!=NULL)
	{ // We have an object against...
		if (object2!=NULL)
		{ // ...another object
			if ( (object1->getObjectType()==sim_object_dummy_type)&&(object2->getObjectType()==sim_object_dummy_type) )
				returnValue=_getDummyDummyDistanceIfSmaller(entity1ID,entity2ID,dist,ray,overrideMeasurableFlagIfNonCollection1,overrideMeasurableFlagIfNonCollection2,pathPlanningRoutineCalling);
			if ( (object1->getObjectType()==sim_object_shape_type)&&(object2->getObjectType()==sim_object_shape_type) )
				returnValue=_getShapeShapeDistanceIfSmaller(entity1ID,entity2ID,dist,ray,buffer,overrideMeasurableFlagIfNonCollection1,overrideMeasurableFlagIfNonCollection2,pathPlanningRoutineCalling);
			if ( (object1->getObjectType()==sim_object_shape_type)&&(object2->getObjectType()==sim_object_dummy_type) )
			{
				int cache=buffer[0];
				returnValue=_getShapeDummyDistanceIfSmaller(entity1ID,entity2ID,dist,ray,cache,overrideMeasurableFlagIfNonCollection1,overrideMeasurableFlagIfNonCollection2,pathPlanningRoutineCalling);
				buffer[0]=cache;
			}
			if ( (object1->getObjectType()==sim_object_dummy_type)&&(object2->getObjectType()==sim_object_shape_type) )
			{
				int cache=buffer[0];
				returnValue=_getDummyShapeDistanceIfSmaller(entity1ID,entity2ID,dist,ray,cache,overrideMeasurableFlagIfNonCollection1,overrideMeasurableFlagIfNonCollection2,pathPlanningRoutineCalling);
				buffer[0]=cache;
			}
		}
		else
		{ // ...a group or all other objects (entity2ID could be -1)
			if (object1->getObjectType()==sim_object_dummy_type)
				returnValue=_getDummyGroupDistanceIfSmaller(entity1ID,entity2ID,dist,ray,buffer,overrideMeasurableFlagIfNonCollection1,pathPlanningRoutineCalling);
			if (object1->getObjectType()==sim_object_shape_type)
				returnValue=_getShapeGroupDistanceIfSmaller(entity1ID,entity2ID,dist,ray,buffer,overrideMeasurableFlagIfNonCollection1,pathPlanningRoutineCalling);
		}
	}
	else
	{ // We have a group against...
		if (object2!=NULL)
		{ //...an object
			if (object2->getObjectType()==sim_object_dummy_type)
				returnValue=_getGroupDummyDistanceIfSmaller(entity1ID,entity2ID,dist,ray,buffer,overrideMeasurableFlagIfNonCollection2,pathPlanningRoutineCalling);
			if (object2->getObjectType()==sim_object_shape_type)
				returnValue=_getGroupShapeDistanceIfSmaller(entity1ID,entity2ID,dist,ray,buffer,overrideMeasurableFlagIfNonCollection2,pathPlanningRoutineCalling);
		} // ...another group or all other objects (entity2ID could be -1)
		else
			returnValue=_getGroupGroupDistanceIfSmaller(entity1ID,entity2ID,dist,ray,buffer,pathPlanningRoutineCalling);
	}
	if (!handledByWorkThread)
		App::ct->infoDisplay->distanceCalculationEnd();
	return(returnValue);
}

//----------------------------------------------------------------------------------



//---------------------------- INVERSED DISTANCE QUERIES ---------------------------

bool CDistanceRoutine::_getShapeGroupDistanceIfSmaller(int shapeID,int groupID,
						float& dist,float ray[7],int buffer[3],bool overrideMeasurableFlagShape,bool pathPlanningRoutineCalling)
{ // groupID can be -1 in which case all objects are tested against the shape
	int bufferAux[3]={buffer[1],buffer[2],buffer[0]};
	if (_getGroupShapeDistanceIfSmaller(groupID,shapeID,dist,ray,bufferAux,overrideMeasurableFlagShape,pathPlanningRoutineCalling))
	{
		buffer[0]=bufferAux[2];
		buffer[1]=bufferAux[0];
		buffer[2]=bufferAux[1];
		_invertRay(ray);
		return(true);
	}
	return(false);
}
bool CDistanceRoutine::_getDummyShapeDistanceIfSmaller(int dummyID,int shapeID,
						float& dist,float ray[7],int& buffer,bool overrideMeasurableFlagDummy,bool overrideMeasurableFlagShape,bool pathPlanningRoutineCalling)
{
	if (_getShapeDummyDistanceIfSmaller(shapeID,dummyID,dist,ray,buffer,overrideMeasurableFlagShape,overrideMeasurableFlagDummy,pathPlanningRoutineCalling))
	{
		_invertRay(ray);
		return(true);
	}
	return(false);

}
bool CDistanceRoutine::_getDummyGroupDistanceIfSmaller(int dummyID,int groupID,
						float& dist,float ray[7],int buffer[2],bool overrideMeasurableFlagDummy,bool pathPlanningRoutineCalling)
{ // groupID can be -1 in which case all objects are tested against the dummy
	if (_getGroupDummyDistanceIfSmaller(groupID,dummyID,dist,ray,buffer,overrideMeasurableFlagDummy,pathPlanningRoutineCalling))
	{
		_invertRay(ray);
		return(true);
	}
	return(false);

}
//----------------------------------------------------------------------------------

//------------------------------ DIRECT DISTANCE QUERIES ---------------------------

bool CDistanceRoutine::_getDummyDummyDistanceIfSmaller(int dummy1ID,int dummy2ID,
						float& dist,float ray[7],bool overrideMeasurableFlagDummy1,bool overrideMeasurableFlagDummy2,bool pathPlanningRoutineCalling)
{	// Distance is measured from dummy1 to dummy2
	// If the distance is smaller than 'dist', 'dist' is replaced and the return value is true
	// If the distance is bigger, 'dist' doesn't change and the return value is false
	// ray contains the point on object1 (0-2), the point on object2 (3-5) and the distance (6)
	CDummy* dummy1=App::ct->objCont->getDummy(dummy1ID);
	CDummy* dummy2=App::ct->objCont->getDummy(dummy2ID);
	if ( (dummy1==NULL)||(dummy2==NULL) )
		return(false);
	if ( (((dummy1->getMainProperty()&sim_objectspecialproperty_measurable)==0)&&(!overrideMeasurableFlagDummy1))||(((dummy2->getMainProperty()&sim_objectspecialproperty_measurable)==0)&&(!overrideMeasurableFlagDummy2)) )
		return(false);
	if (pathPlanningRoutineCalling)
	{ 
		if ( ((dummy1->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)&&(!overrideMeasurableFlagDummy1))||((dummy2->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)&&(!overrideMeasurableFlagDummy2)) )
			return(false);
	}

	// We never measure a dummy against itself!!
	if (dummy1==dummy2)
		return(false);
	C3Vector v0(dummy1->getCumulativeTransformationPart1().X);
	C3Vector v1(dummy2->getCumulativeTransformationPart1().X);
	float newDist=(v0-v1).getLength();
	if (newDist<dist)
	{
		ray[0]=v0(0);
		ray[1]=v0(1);
		ray[2]=v0(2);
		ray[3]=v1(0);
		ray[4]=v1(1);
		ray[5]=v1(2);
		ray[6]=newDist;
		dist=newDist;
		return(true);
	}
	return(false);	
}


bool CDistanceRoutine::_getShapeShapeDistanceIfSmaller(int shape1ID,int shape2ID,
						float& dist,float ray[7],int buffer[2],bool overrideMeasurableFlagShape1,bool overrideMeasurableFlagShape2,bool pathPlanningRoutineCalling)
{	// Distance is measured from shape1 to shape2
	// If the distance is smaller than 'dist', 'dist' is replaced and the return value is true
	// If the distance is bigger, 'dist' doesn't change and the return value is false
	// ray contains the point on object1 (0-2), the point on object2 (3-5) and the distance (6)

	//	buffer[0]=-1;	// Artificially disabling caching for tests

	CShape* shape1=App::ct->objCont->getShape(shape1ID);
	CShape* shape2=App::ct->objCont->getShape(shape2ID);
	if ( (shape1==NULL)||(shape2==NULL) )
		return(false);
	// We never measure a shape against itself!!
	if (shape1==shape2)
		return(false);
	if ( (((shape1->getMainProperty()&sim_objectspecialproperty_measurable)==0)&&(!overrideMeasurableFlagShape1))||(((shape2->getMainProperty()&sim_objectspecialproperty_measurable)==0)&&(!overrideMeasurableFlagShape2)) )
		return(false);
	if (pathPlanningRoutineCalling)
	{
		if ( ((shape1->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)&&(!overrideMeasurableFlagShape1))||((shape2->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)&&(!overrideMeasurableFlagShape2)) )
			return(false);
	}

	// Build the collision nodes only when needed. So do it right here!
	shape1->initializeCalculationStructureIfNeeded();
	shape2->initializeCalculationStructureIfNeeded();

	bool retValue=_getBufferedDistance_IfSmaller(shape1,shape2,buffer,dist,ray);
	if (shape1->getShapeShapeDistance_IfSmaller(shape2,dist,ray,buffer))
		retValue=true;
	return(retValue);
}

bool CDistanceRoutine::_getGroupShapeDistanceIfSmaller(int groupID,int shapeID,
						float& dist,float ray[7],int buffer[3],bool overrideMeasurableFlagShape,bool pathPlanningRoutineCalling)
{	// Distance is measured from group to shape
	// If the distance is smaller than 'dist', 'dist' is replaced and the return value is true
	// If the distance is bigger, 'dist' doesn't change and the return value is false
	// ray contains the point on object1 (0-2), the point on object2 (3-5) and the distance (6)
	// groupID can be -1 in which case all objects are tested against the shape!

	//	buffer[0]=-1;	// Artificially disabling caching for tests
	int propMask=sim_objectspecialproperty_measurable;
	
	// We get the single shape and all shapes/dummies in the group:
	CShape* shape=App::ct->objCont->getShape(shapeID);
	if (shape==NULL)
		return(false);
	if ( ((shape->getMainProperty()&sim_objectspecialproperty_measurable)==0)&&(!overrideMeasurableFlagShape) )
		return(false);
	if (pathPlanningRoutineCalling&&(shape->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)&&(!overrideMeasurableFlagShape))
		return(false);

	std::vector<C3DObject*> group;
	if (groupID==-1)
	{ // Special case:
		std::vector<C3DObject*> exceptionObject;
		exceptionObject.push_back(shape);
		App::ct->objCont->getAllShapesAndDummiesFromSceneExcept(exceptionObject,group,propMask,pathPlanningRoutineCalling);
	}
	else
	{ // regular case:
		if (!App::ct->collections->getShapesAndDummiesFromGroup(groupID,&group,propMask,pathPlanningRoutineCalling))
			return(false);
	}
	// Build the collision nodes only when needed. So do it right here!
	for (int i=0;i<int(group.size());i++)
	{
		if (group[i]->getObjectType()==sim_object_shape_type)
			((CShape*)group[i])->initializeCalculationStructureIfNeeded();
	}
	shape->initializeCalculationStructureIfNeeded();

	bool returnValue=false;

	// This part is for handling the cached distance:
	C3DObject* buffTree=App::ct->objCont->getObject(buffer[0]);
	if (buffTree!=NULL)
	{	//We check if buffTree is in the group:
		for (int i=0;i<int(group.size());i++)
		{
			if (group[i]==buffTree)
			{
				group.erase(group.begin()+i); // We remove that element because we measure it here
				if (buffTree->getObjectType()==sim_object_shape_type)
				{ // We have a shape
					if (buffTree!=shape)
					{ // We never measure a shape against itself! (shouldn't anyway happen here)
						returnValue=_getBufferedDistance_IfSmaller((CShape*)buffTree,shape,buffer+1,dist,ray);
						// Now we check these two shapes:
						if (((CShape*)buffTree)->getShapeShapeDistance_IfSmaller(shape,dist,ray,buffer+1))
							returnValue=true;
					}
					else
						buffer[0]=-1;
				}
				if (buffTree->getObjectType()==sim_object_dummy_type)
				{ // Here we have a dummy
					int auxBuff=buffer[2];
					returnValue=_getBufferedDistance_IfSmaller(shape,(CDummy*)buffTree,auxBuff,dist,ray);
					if (shape->getDistanceToDummy_IfSmaller((CDummy*)buffTree,dist,ray,auxBuff))
						returnValue=true;
					if (returnValue)
					{ 
						buffer[2]=auxBuff;
						_invertRay(ray);
					}
				}
				break;
			}
		}
	}

//	Here we process all dummy-shape distances (ordered)
	std::vector<C3DObject*> exploringOrder;
	std::vector<float> approxDistances;
	exploringOrder.reserve(group.size());
	exploringOrder.clear();
	approxDistances.reserve(group.size());
	approxDistances.clear();
	C4X4Matrix shapeCM(shape->getCumulativeTransformation().getMatrix());
	C3Vector shapeSize(shape->geomData->getBoundingBoxHalfSizes());
	int i=0;
	while (i<int(group.size()))
	{
		if (group[i]->getObjectType()==sim_object_dummy_type)
		{
			float testDist;
			C3Vector dummyPos(group[i]->getCumulativeTransformation().X);
			testDist=CCollDistInterface::getBoxPointDistance(shapeCM,shapeSize,dummyPos);
			// We have the EXACT (not approx anymore!) distance and put it in the ordered list:
			int k;
			for (k=0;k<int(approxDistances.size());k++)
			{
				if (testDist<approxDistances[k]) 
					break;
			}
			approxDistances.insert(approxDistances.begin()+k,testDist);
			exploringOrder.insert(exploringOrder.begin()+k,group[i]);
			group.erase(group.begin()+i);
		}
		else
			i++;
	}
	// Now find the smallest distance (dummy-shape):
	for (int i=0;i<int(approxDistances.size());i++)
	{
		if (approxDistances[i]>dist) 
			break;
		int auxBuffer;
		if (shape->getDistanceToDummy_IfSmaller((CDummy*)exploringOrder[i],dist,ray,auxBuffer))
		{
			buffer[0]=exploringOrder[i]->getID();
			buffer[2]=auxBuffer;
			_invertRay(ray);
			returnValue=true;
		}
	}
	// group doesn't contain any dummies anymore

	// group contains only shapes!

	// Now go through all shapes and in the group and order them according to
	// their approximate distance to shape (from smallest to biggest):
	exploringOrder.clear();
	approxDistances.clear();
	for (int i=0;i<int(group.size());i++)
	{
		if (group[i]->getObjectType()==sim_object_shape_type)
		{ // Just in case... (should anyway contain only shapes!)
			C4X4Matrix groupCM(group[i]->getCumulativeTransformation().getMatrix());
			C3Vector groupSize(((CShape*)group[i])->geomData->getBoundingBoxHalfSizes());
			float testDist=CCollDistInterface::getApproximateBoxBoxDistance(groupCM,groupSize,shapeCM,shapeSize);
			// We have the approx distance and put it in the ordered list:
			int k;
			for (k=0;k<int(approxDistances.size());k++)
			{
				if (testDist<approxDistances[k]) 
					break;
			}
			approxDistances.insert(approxDistances.begin()+k,testDist);
			exploringOrder.insert(exploringOrder.begin()+k,group[i]);
		}
	}

	// Now find the smallest distance (shape-shape):
	for (int i=0;i<int(approxDistances.size());i++)
	{
		if (approxDistances[i]>dist) 
			break;
		if (exploringOrder[i]!=shape)
		{ // We never measure a shape against itself!!
			if (((CShape*)exploringOrder[i])->getShapeShapeDistance_IfSmaller(shape,dist,ray,buffer+1))
			{
				buffer[0]=exploringOrder[i]->getID();
				returnValue=true;
			}
		}
	}
	return(returnValue);
}


bool CDistanceRoutine::_getGroupGroupDistanceIfSmaller(int group1ID,int group2ID,
						float& dist,float ray[7],int buffer[4],bool pathPlanningRoutineCalling)
{	// Distance is measured from tree1 to tree2
	// If the distance is smaller than 'dist', 'dist' is replaced and the return value is true
	// If the distance is bigger, 'dist' doesn't change and the return value is false
	// ray contains the point on object1 (0-2), the point on object2 (3-5) and the distance (6)
	// group2ID can be -1, in which case all objects are tested against group1

	//	buffer[0]=-1;	// Artificially disabling caching for tests
	int propMask=sim_objectspecialproperty_measurable;

	// We get all shapes/dummies in the two groups:
	std::vector<C3DObject*> group1;
	if (!App::ct->collections->getShapesAndDummiesFromGroup(group1ID,&group1,propMask,pathPlanningRoutineCalling))
		return(false);
	bool overrideGroup1ObjectMeasurableFlag=App::ct->collections->getGroup(group1ID)->getOverridesObjectMainProperties();
	bool overrideGroup2ObjectMeasurableFlag=false;
	std::vector<C3DObject*> group2;
	if (group2ID==-1)
	{ // special case:
		App::ct->objCont->getAllShapesAndDummiesFromSceneExcept(group1,group2,propMask,pathPlanningRoutineCalling);
	}
	else
	{ // regular case:
		if (!App::ct->collections->getShapesAndDummiesFromGroup(group2ID,&group2,propMask,pathPlanningRoutineCalling))
			return(false);
		overrideGroup2ObjectMeasurableFlag=App::ct->collections->getGroup(group2ID)->getOverridesObjectMainProperties();
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

	// Now we create a list of object pairs which need to be checked. If group1
	// is same as group2 (or partially same), objects are not checked in both
	// directions (only a-b, not a-b and b-a).
	std::vector<C3DObject*> objPairs;
	objPairs.reserve(1024);
	objPairs.clear();
	for (int i=0;i<int(group1.size());i++)
	{
		C3DObject* obj1=group1[i];
		int csci1=obj1->getCollectionSelfCollisionIndicator();
		for (int j=0;j<int(group2.size());j++)
		{
			C3DObject* obj2=group2[j];
			int csci2=obj2->getCollectionSelfCollisionIndicator();
			if (obj1!=obj2)
			{ // We never measure an object against itself!
				if ( (group1ID!=group2ID)||(abs(csci1-csci2)!=1) )
				{ // we do not have the same collection, or the collection self collision indicators differences is not 1
					// We now check if these partners are already present in objPairs, but only if the two groups are same.
					// (otherwise it takes too much time!)
					bool inexistant=true;
					if (group1ID==group2ID)
					{
						for (int k=0;k<int(objPairs.size())/2;k++)
						{
							if ( ( (objPairs[2*k+0]==obj1)&&(objPairs[2*k+1]==obj2) ) ||
								( (objPairs[2*k+0]==obj2)&&(objPairs[2*k+1]==obj1) ) )
							{
								inexistant=false;
								break;
							}
						}
					}
					if (inexistant)
					{ // We can insert this pair:
						objPairs.push_back(obj1);
						objPairs.push_back(obj2);
					}
				}
			}
		}
	}
	group1.clear();
	group2.clear();

	bool returnValue=false;

	// This part is for handling the cached distance:
	C3DObject* buffTree1=App::ct->objCont->getObject(buffer[0]);
	C3DObject* buffTree2=App::ct->objCont->getObject(buffer[2]);
	if ( (buffTree1!=NULL)&&(buffTree2!=NULL)&&(buffTree1!=buffTree2) )
	{	//We check if this pair exists:
		bool exists=false;
		for (int i=0;i<int(objPairs.size())/2;i++)
		{
			if ( (objPairs[2*i+0]==buffTree1)&&(objPairs[2*i+1]==buffTree2) )
			{ // It does exist!
				exists=true;
				objPairs[2*i+0]=NULL; // We remove it from the list
				break;
			}
		}
		if (exists)
		{
			bool retVal2=false;
			if (buffTree1->getObjectType()==sim_object_dummy_type)
			{
				if (buffTree2->getObjectType()==sim_object_shape_type)
				{
					int auxBuffer=buffer[3];
					retVal2=_getBufferedDistance_IfSmaller((CShape*)buffTree2,(CDummy*)buffTree1,auxBuffer,dist,ray);
					if (((CShape*)buffTree2)->getDistanceToDummy_IfSmaller((CDummy*)buffTree1,dist,ray,auxBuffer))
						retVal2=true;
					if (retVal2)
					{
						buffer[3]=auxBuffer;
						_invertRay(ray);
					}
				}
			}
			if (buffTree1->getObjectType()==sim_object_shape_type)
			{
				if (buffTree2->getObjectType()==sim_object_dummy_type)
				{
					int auxBuffer=buffer[1];
					retVal2=_getBufferedDistance_IfSmaller((CShape*)buffTree1,(CDummy*)buffTree2,auxBuffer,dist,ray);
					if (((CShape*)buffTree1)->getDistanceToDummy_IfSmaller((CDummy*)buffTree2,dist,ray,auxBuffer))
						retVal2=true;
					if (retVal2)
						buffer[1]=auxBuffer;
				}
				if (buffTree2->getObjectType()==sim_object_shape_type)
				{
					int auxBuffer[2]={buffer[1],buffer[3]};
					retVal2=_getBufferedDistance_IfSmaller((CShape*)buffTree1,(CShape*)buffTree2,auxBuffer,dist,ray);
					if (((CShape*)buffTree1)->getShapeShapeDistance_IfSmaller((CShape*)buffTree2,dist,ray,auxBuffer))
						retVal2=true;
					if (retVal2)
					{
						buffer[1]=auxBuffer[0];
						buffer[3]=auxBuffer[1];
					}
				}
			}
			returnValue=returnValue||retVal2;
		}
	}

	// First process all dummy-dummy distances because it's so fast (and not buffered):
	for (int i=0;i<int(objPairs.size())/2;i++)
	{
		if (dist==0.0f)
			break;
		if (objPairs[2*i+0]!=NULL)
		{
			if ( (objPairs[2*i+0]->getObjectType()==sim_object_dummy_type)&&
				(objPairs[2*i+1]->getObjectType()==sim_object_dummy_type) )
			{
				CDummy* dummy1=(CDummy*)objPairs[2*i+0];
				CDummy* dummy2=(CDummy*)objPairs[2*i+1];
				objPairs[2*i+0]=NULL; // We remove this pair from the list
				if (_getDummyDummyDistanceIfSmaller(dummy1->getID(),dummy2->getID(),dist,ray,overrideGroup1ObjectMeasurableFlag,overrideGroup2ObjectMeasurableFlag,pathPlanningRoutineCalling))
					returnValue=true;
			}
		}
	}

	// We process now all shape-dummy or dummy-shape pairs in an ordered way:
	std::vector<C3DObject*> exploringOrder;
	exploringOrder.reserve(objPairs.size());
	exploringOrder.clear();
	std::vector<float> approxDistances;
	approxDistances.reserve(objPairs.size()/2);
	approxDistances.clear();
	for (int i=0;i<int(objPairs.size())/2;i++)
	{
		if (objPairs[2*i+0]!=NULL)
		{
			C3DObject* obj1=objPairs[2*i+0];
			C3DObject* obj2=objPairs[2*i+1];
			float approxDistance=-10.0f;
			if ( (obj1->getObjectType()==sim_object_shape_type)&&(obj2->getObjectType()==sim_object_dummy_type) )
			{
				objPairs[2*i+0]=NULL; // We remove these partners from the list
				C3Vector dummyPos(obj2->getCumulativeTransformation().X);
				C4X4Matrix shapeCTM(obj1->getCumulativeTransformation().getMatrix());
				C3Vector shapeSize(((CShape*)obj1)->geomData->getBoundingBoxHalfSizes());
				approxDistance=CCollDistInterface::getBoxPointDistance(shapeCTM,shapeSize,dummyPos);
			}
			if ( (obj1->getObjectType()==sim_object_dummy_type)&&(obj2->getObjectType()==sim_object_shape_type) )
			{
				objPairs[2*i+0]=NULL; // We remove these partners from the list
				C3Vector dummyPos(obj1->getCumulativeTransformation().X);
				C4X4Matrix shapeCTM(obj2->getCumulativeTransformation().getMatrix());
				C3Vector shapeSize(((CShape*)obj2)->geomData->getBoundingBoxHalfSizes());
				approxDistance=CCollDistInterface::getBoxPointDistance(shapeCTM,shapeSize,dummyPos);
			}
			if (approxDistance>-5.0f)
			{
				int k;
				for (k=0;k<int(approxDistances.size());k++)
				{
					if (approxDistance<approxDistances[k]) 
						break;
				}
				approxDistances.insert(approxDistances.begin()+k,approxDistance);
				exploringOrder.insert(exploringOrder.begin()+(2*k+0),obj1);
				exploringOrder.insert(exploringOrder.begin()+(2*k+1),obj2);
			}
		}
	}
	for (int i=0;i<int(approxDistances.size());i++)
	{
		if (approxDistances[i]>dist) 
			break;
		if (dist==0.0f)
			break;
		if (exploringOrder[2*i+0]->getObjectType()==sim_object_shape_type)
		{ // Here we have shape-dummy
			CShape* shape=(CShape*)exploringOrder[2*i+0];
			CDummy* dummy=(CDummy*)exploringOrder[2*i+1];
			int auxBuff;
			if (shape->getDistanceToDummy_IfSmaller(dummy,dist,ray,auxBuff))
			{
				buffer[0]=shape->getID();
				buffer[1]=auxBuff;
				buffer[2]=dummy->getID();
				returnValue=true;
			}
		}
		else
		{ // Here we have dummy-shape
			CDummy* dummy=(CDummy*)exploringOrder[2*i+0];
			CShape* shape=(CShape*)exploringOrder[2*i+1];
			int auxBuff;
			if (shape->getDistanceToDummy_IfSmaller(dummy,dist,ray,auxBuff))
			{
				buffer[0]=dummy->getID();
				buffer[2]=shape->getID();
				buffer[3]=auxBuff;
				_invertRay(ray);
				returnValue=true;
			}
		}
	}
	exploringOrder.clear();
	approxDistances.clear();

	// We process now all shape-shape pairs in an ordered way:
	for (int i=0;i<int(objPairs.size())/2;i++)
	{
		if (objPairs[2*i+0]!=NULL)
		{
			if ( (objPairs[2*i+0]->getObjectType()==sim_object_shape_type)&&(objPairs[2*i+1]->getObjectType()==sim_object_shape_type) )
			{
				CShape* shape1=(CShape*)objPairs[2*i+0];
				CShape* shape2=(CShape*)objPairs[2*i+1];
				C4X4Matrix shape1CM(shape1->getCumulativeTransformation().getMatrix());
				C3Vector shape1Size(shape1->geomData->getBoundingBoxHalfSizes());
				C4X4Matrix shape2CM(shape2->getCumulativeTransformation().getMatrix());
				C3Vector shape2Size(shape2->geomData->getBoundingBoxHalfSizes());
				float approxDistance=CCollDistInterface::getApproximateBoxBoxDistance(shape1CM,shape1Size,shape2CM,shape2Size);
				int k;
				for (k=0;k<int(approxDistances.size());k++)
				{
					if (approxDistance<approxDistances[k]) 
						break;
				}
				approxDistances.insert(approxDistances.begin()+k,approxDistance);
				exploringOrder.insert(exploringOrder.begin()+(2*k+0),shape1);
				exploringOrder.insert(exploringOrder.begin()+(2*k+1),shape2);
			}
		}
	}

	for (int i=0;i<int(approxDistances.size());i++)
	{
		if (approxDistances[i]>dist) 
			break;
		if (dist==0.0f)
			break;
		CShape* shape1=(CShape*)exploringOrder[2*i+0];
		CShape* shape2=(CShape*)exploringOrder[2*i+1];
		int auxBuffer[2];
		if (shape1->getShapeShapeDistance_IfSmaller(shape2,dist,ray,auxBuffer))
		{
			buffer[0]=shape1->getID();
			buffer[1]=auxBuffer[0];
			buffer[2]=shape2->getID();
			buffer[3]=auxBuffer[1];
			returnValue=true;
		}
	}
	return(returnValue);
}



bool CDistanceRoutine::_getShapeDummyDistanceIfSmaller(int shapeID,int dummyID,
						float& dist,float ray[7],int& buffer,bool overrideMeasurableFlagShape,bool overrideMeasurableFlagDummy,bool pathPlanningRoutineCalling)
{	// Distance is measured from shape to dummy
	// If the distance is smaller than 'dist', 'dist' is replaced and the return value is true
	// If the distance is bigger, 'dist' doesn't change and the return value is false
	// ray contains the point on object1 (0-2), the point on object2 (3-5) and the distance (6)

	//	buffer=-1;	// Artificially disabling caching for tests

	CShape* shape=App::ct->objCont->getShape(shapeID);
	CDummy* dummy=App::ct->objCont->getDummy(dummyID);
	if ( (shape==NULL)||(dummy==NULL) )
		return(false);
	if ( (((shape->getMainProperty()&sim_objectspecialproperty_measurable)==0)&&(!overrideMeasurableFlagShape))||(((dummy->getMainProperty()&sim_objectspecialproperty_measurable)==0)&&(!overrideMeasurableFlagDummy)) )
		return(false);

	if (pathPlanningRoutineCalling)
	{ 
		if ( ((shape->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)&&(!overrideMeasurableFlagShape))||((dummy->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)&&(!overrideMeasurableFlagDummy)) )
			return(false);
	}

	// Build the collision nodes only when needed. So do it right here!
	shape->initializeCalculationStructureIfNeeded();
	bool retValue=_getBufferedDistance_IfSmaller(shape,dummy,buffer,dist,ray);
	if (shape->getDistanceToDummy_IfSmaller(dummy,dist,ray,buffer))
		retValue=true;
	return(retValue);
}

bool CDistanceRoutine::_getGroupDummyDistanceIfSmaller(int groupID,int dummyID,
						float& dist,float ray[7],int buffer[2],bool overrideMeasurableFlagDummy,bool pathPlanningRoutineCalling)
{	// Distance is measured from collection to dummy
	// If the distance is smaller than 'dist', 'dist' is replaced and the return value is true
	// If the distance is bigger, 'dist' doesn't change and the return value is false
	// ray contains the point on object1 (0-2), the point on object2 (3-5) and the distance (6)
	// groupID can be -1, in which case all other objects are tested

	//	buffer[0]=-1;	// Artificially disabling caching for tests
	int propMask=sim_objectspecialproperty_measurable;

	// We get the dummy and all shapes/dummies in the group:
	CDummy* dummy=App::ct->objCont->getDummy(dummyID);
	if (dummy==NULL)
		return(false);
	if ( ((dummy->getMainProperty()&sim_objectspecialproperty_measurable)==0)&&(!overrideMeasurableFlagDummy) )
		return(false);
	if (pathPlanningRoutineCalling&&(dummy->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)&&(!overrideMeasurableFlagDummy))
		return(false);

	std::vector<C3DObject*> group;
	bool overrideMeasurableFlagGroupObjects=false;
	if (groupID==-1)
	{ // Special group:
		std::vector<C3DObject*> exceptionObject;
		exceptionObject.push_back(dummy);
		App::ct->objCont->getAllShapesAndDummiesFromSceneExcept(exceptionObject,group,propMask,pathPlanningRoutineCalling);
	}
	else
	{ // regular group
		if (!App::ct->collections->getShapesAndDummiesFromGroup(groupID,&group,propMask,pathPlanningRoutineCalling))
			return(false);
		overrideMeasurableFlagGroupObjects=App::ct->collections->getGroup(groupID)->getOverridesObjectMainProperties();
	}
	// Build the collision nodes only when needed. So do it right here!
	for (int i=0;i<int(group.size());i++)
	{
		if (group[i]->getObjectType()==sim_object_shape_type)
			((CShape*)group[i])->initializeCalculationStructureIfNeeded();
	}
	bool returnValue=false;

	// We first process all dummy-dummy distances because it's so fast (doesn't use caching):
	int i=0;
	while (i<int(group.size()))
	{
		if (group[i]->getObjectType()==sim_object_dummy_type)
		{
			if (group[i]!=dummy) // We never measure a dummy against itself!
			{
				if (_getDummyDummyDistanceIfSmaller(group[i]->getID(),dummy->getID(),dist,ray,overrideMeasurableFlagGroupObjects,overrideMeasurableFlagDummy,pathPlanningRoutineCalling))
					returnValue=true;
			}
			group.erase(group.begin()+i);
		}
		else
			i++;
	}

	// This part is for handling the cached distance:
	C3DObject* buffTree=App::ct->objCont->getObject(buffer[0]);
	if (buffTree!=NULL)
	{	//We check if buffTree is in the group:
		for (int i=0;i<int(group.size());i++)
		{
			if (group[i]==buffTree)
			{
				group.erase(group.begin()+i); // We remove that element because we measure it here
				int auxBuff=buffer[1];
				if (buffTree->getObjectType()==sim_object_shape_type)
				{ // We have a buffered shape here:
					bool retVal=_getBufferedDistance_IfSmaller((CShape*)buffTree,dummy,auxBuff,dist,ray);
					// Now we check this shape & dummy:
					if (((CShape*)buffTree)->getDistanceToDummy_IfSmaller(dummy,dist,ray,auxBuff))
						retVal=true;
					if (retVal)
						buffer[1]=auxBuff;
					returnValue=returnValue||retVal; // ReturnValue may already be true (dummy-dummy were already processed!)
				}
				break;
			}
		}
	}

	// Group now only contains shapes

	// Now go through all shapes in the group and order them according to
	// their approximate distance to dummy (from smallest to biggest):
	std::vector<CShape*> exploringOrder;
	std::vector<float> approxDistances;
	exploringOrder.reserve(group.size());
	approxDistances.reserve(group.size());
	C4X4Matrix groupCM;
	C3Vector groupSize;
	float groupCMAux[4][4];
	dummy->getCumulativeTransformationMatrix(groupCMAux); // We borrow groupCM just for here
	groupCM.set(groupCMAux);
	C3Vector dummyPos(groupCM.X);
	for (int i=0;i<int(group.size());i++)
	{
		if (group[i]->getObjectType()==sim_object_shape_type)
		{ // Just in case... (should anyway contain only shapes!)
			float testDist;
			group[i]->getCumulativeTransformationMatrix(groupCMAux);
			groupSize=((CShape*)group[i])->geomData->getBoundingBoxHalfSizes();
			testDist=CCollDistInterface::getBoxPointDistance(groupCM,groupSize,dummyPos);
			// We have the approx distance and put it in the ordered list:		
			int k;
			for (k=0;k<int(approxDistances.size());k++)
			{
				if (testDist<approxDistances[k]) 
					break;
			}
			approxDistances.insert(approxDistances.begin()+k,testDist);
			exploringOrder.insert(exploringOrder.begin()+k,(CShape*)group[i]);
		}
	}

	// Now find the smallest distance:
	int auxBuffer;
	for (int i=0;i<int(approxDistances.size());i++)
	{
		if (approxDistances[i]>dist) 
			break;
		if (dist==0.0f)
			break;
		if (((CShape*)exploringOrder[i])->getDistanceToDummy_IfSmaller(dummy,dist,ray,auxBuffer))
		{
			buffer[0]=exploringOrder[i]->getID();
			buffer[1]=auxBuffer;
			returnValue=true;
		}
	}
	return(returnValue);
}

bool CDistanceRoutine::getObjectObstacleDistanceIfSmaller(C3DObject* object,C3DObject* obstacle,
												  float& dist,float ray[7],bool overrideMeasurableFlagObject1,bool overrideMeasurableFlagObject2)
{ // Only to be called from IK/GCS routines!!!
	bool returnValue=false;
	App::ct->infoDisplay->distanceCalculationStart();
	// Here we don't use caching: since we have a treshhold, there is almost no speed gain when
	// using caching!!
	if (object->getObjectType()==sim_object_dummy_type)
	{ // Object is a dummy
		if (obstacle->getObjectType()==sim_object_dummy_type)
		{ // Obstacle is a dummy
			returnValue=_getDummyDummyDistanceIfSmaller(object->getID(),obstacle->getID(),dist,ray,overrideMeasurableFlagObject1,overrideMeasurableFlagObject2,false);
		}
		if (obstacle->getObjectType()==sim_object_shape_type)
		{ // Obstacle is a shape
			int buffer=-1;
			returnValue=_getDummyShapeDistanceIfSmaller(object->getID(),obstacle->getID(),dist,ray,buffer,overrideMeasurableFlagObject1,overrideMeasurableFlagObject2,false);
		}
	}
	if (object->getObjectType()==sim_object_shape_type)
	{ // Object is a shape
		if (obstacle->getObjectType()==sim_object_dummy_type)
		{ // Obstacle is a dummy
			int buffer=-1;
			returnValue=_getShapeDummyDistanceIfSmaller(object->getID(),obstacle->getID(),dist,ray,buffer,overrideMeasurableFlagObject1,overrideMeasurableFlagObject2,false);
		}
		if (obstacle->getObjectType()==sim_object_shape_type)
		{ // Obstacle is a shape
			int buffer[2]={-1,-1};
			returnValue=_getShapeShapeDistanceIfSmaller(object->getID(),obstacle->getID(),dist,ray,buffer,overrideMeasurableFlagObject1,overrideMeasurableFlagObject2,false);
		}
	}
	App::ct->infoDisplay->distanceCalculationEnd();
	return(returnValue);
}


//----------------------------------------------------------------------------------

//------------------------------ BUFFERED DISTANCE QUERIES -------------------------

bool CDistanceRoutine::_getBufferedDistance_IfSmaller(CShape* shape1,CShape* shape2,
						int buffer[2],float& dist,float ray[7])
{	// Distance is measured from shape1 to shape2
	// If the distance is smaller than 'dist', 'dist' is replaced and the return value is true
	// If the distance is bigger, 'dist' doesn't change and the return value is false
	// ray contains the point on object1 (0-2), the point on object2 (3-5) and the distance (6)
	if (CIloIlo::distanceCachingOff)
		return(false); // Distance caching is turned off
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
	C3Vector segA;
	C3Vector segB;
	if (CCollDistInterface::getTriangleTriangleDistance_ifSmaller(t1a,t1b,t1c,t2a,t2b,t2c,dist,segA,segB))
	{
		ray[0]=segA(0);
		ray[1]=segA(1);
		ray[2]=segA(2);
		ray[3]=segB(0);
		ray[4]=segB(1);
		ray[5]=segB(2);
		ray[6]=dist;
		return(true);
	}
	return(false);
}

bool CDistanceRoutine::_getBufferedDistance_IfSmaller(CShape* shape,CDummy* dummy,
						int buffer,float& dist,float ray[7])
{	// Distance is measured from shape to dummy
	// If the distance is smaller than 'dist', 'dist' is replaced and the return value is true
	// If the distance is bigger, 'dist' doesn't change and the return value is false
	// ray contains the point on object1 (0-2), the point on object2 (3-5) and the distance (6)
	if (CIloIlo::distanceCachingOff)
		return(false); // Distance caching is turned off
	if (buffer<0)
		return(false);
	if (CCollDistInterface::getCalculatedTriangleCount(shape->geomData->collInfo)*3<=(buffer*3+2))
		return(false); // that index doesn't exist
	C3Vector t1a;
	C3Vector t1b;
	C3Vector t1c;
	if (!CCollDistInterface::getCalculatedTriangleAt(shape->geomData->collInfo,t1a,t1b,t1c,buffer))
		return(false); // triangle was disabled
	C7Vector bCTM(shape->getCumulativeTransformationPart1());
	t1a*=bCTM;
	t1b*=bCTM;
	t1c*=bCTM;
	t1b-=t1a;
	t1c-=t1a;
	C7Vector dCTM(dummy->getCumulativeTransformationPart1());
	C3Vector dummyPos(dCTM.X);
	C3Vector tmpA;
	if (CCollDistInterface::getTrianglePointDistance_ifSmaller(t1a,t1b,t1c,dummyPos,dist,tmpA))
	{
		ray[0]=tmpA(0);
		ray[1]=tmpA(1);
		ray[2]=tmpA(2);
		ray[3]=dummyPos(0);
		ray[4]=dummyPos(1);
		ray[5]=dummyPos(2);
		ray[6]=dist;
		return(true);
	}
	return(false);
}

//----------------------------------------------------------------------------------

void CDistanceRoutine::_invertRay(float ray[7])
{
	float tmp=ray[3];
	ray[3]=ray[0];
	ray[0]=tmp;
	tmp=ray[4];
	ray[4]=ray[1];
	ray[1]=tmp;
	tmp=ray[5];
	ray[5]=ray[2];
	ray[2]=tmp;
}
