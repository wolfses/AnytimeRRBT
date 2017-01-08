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
#include "proxSensorRoutine.h"
#include "collDistInterface.h"
#include "App.h"
#include "IloIlo.h"
#include "sensingThreadPool.h"

std::vector<SOcclusionCheckDat> CProxSensorRoutine::_occlusionCheckData;

bool CProxSensorRoutine::detectEntity(int sensorID,int entityID,
						 bool closestFeatureMode,bool angleLimitation,float maxAngle,C3Vector& detectedPt,
						 float& dist,bool frontFace,bool backFace,int& detectedObject,float minThreshold,C3Vector& triNormal,bool overrideDetectableFlagIfNonCollection,bool checkOcclusions,bool handledByWorkThread)
{ // entityID==-1 --> checks all objects in the scene
	bool returnValue=false;
	detectedObject=-1;
	C3DObject* object=App::ct->objCont->getObject(entityID);
	CProxSensor* sens=App::ct->objCont->getProximitySensor(sensorID);
	if (sens==NULL)
		return(false); // should never happen!
	if (!handledByWorkThread)
		App::ct->infoDisplay->proximitySensorSimulationStart();

	C3Vector minV,maxV;
	sens->getSensingVolumeBoundingBox(minV,maxV);
	C4X4Matrix sensingVolumeBox;
	sensingVolumeBox.setIdentity();
	sensingVolumeBox.X=(maxV+minV)*0.5f;
	C3Vector sensingVolumeSize(maxV-minV);

	if (object==NULL)
	{
		// Prepare some sort of callback if needed:
		if ( angleLimitation&&checkOcclusions )
			_prepareGroupOcclusionCheck(sensorID,entityID,frontFace,backFace);

		returnValue=_detectGroup(sensorID,entityID,closestFeatureMode, // Collections
			angleLimitation,maxAngle,detectedPt,dist,frontFace,backFace,detectedObject,minThreshold,sensingVolumeBox,sensingVolumeSize,triNormal);

		// Reset the sort of callback:
		if ( angleLimitation&&checkOcclusions )
			_clearOcclusionCheckData();

		if (returnValue)
			triNormal.normalize();
	}
	else
	{ // Detecting objects:
		if (object->getObjectType()==sim_object_dummy_type)
		{
			if (_detectDummy(sensorID,entityID,detectedPt,dist,triNormal,overrideDetectableFlagIfNonCollection))
			{
				if (dist>=minThreshold)
				{
					detectedObject=entityID;
					returnValue=true;
				}
				else
				{
					detectedObject=-1;
					returnValue=false; // We are under the minimum threshold
				}
			}
			else
			{
				detectedObject=-1;
				returnValue=false;
			}
		}
		if (object->getObjectType()==sim_object_shape_type)
		{

			// Prepare some sort of callback if needed:
			if ( angleLimitation&&checkOcclusions )
				_prepareShapeOcclusionCheck(sensorID,entityID,frontFace,backFace,overrideDetectableFlagIfNonCollection);


			if (_detectShape(sensorID,entityID,closestFeatureMode,
				angleLimitation,maxAngle,detectedPt,dist,frontFace,backFace,triNormal,overrideDetectableFlagIfNonCollection))
			{
				if (dist>=minThreshold)
				{
					detectedObject=entityID;
					returnValue=true;
				}
				else
				{
					detectedObject=-1;
					returnValue=false; // We are under the minimum threshold
				}
				triNormal.normalize();
			}
			else
			{
				detectedObject=-1;
				returnValue=false;
			}
			// Reset the sort of callback:
			if ( angleLimitation&&checkOcclusions )
				_clearOcclusionCheckData();
		}
	}
	if (!handledByWorkThread)
		App::ct->infoDisplay->proximitySensorSimulationEnd(returnValue);
	return(returnValue);
}

bool CProxSensorRoutine::_detectDummy(int sensorID,int dummyID,C3Vector& detectedPt,float& dist,C3Vector& triNormalNotNormalized,bool overrideDummyDetectableFlag)
{	// Return value is false if nothing was detected
	// Get the objects:
	CProxSensor* sensor=App::ct->objCont->getProximitySensor(sensorID);
	if (sensor==NULL)
		return(false);
	CDummy* dummy=App::ct->objCont->getDummy(dummyID);
	if (dummy==NULL)
		return(false);
	if ( ((dummy->getMainProperty()&sensor->getSensableType())==0)&&(!overrideDummyDetectableFlag) )
		return(false);
	if (sensor->getSensorType()==sim_proximitysensor_ray_subtype)
		return(false); // ray sensors never detect dummies

	C7Vector inv(sensor->getCumulativeTransformation().getInverse());
	C3Vector sensorPoint(0.0f,0.0f,0.0f);
	C4X4Matrix dummyCTM((inv*dummy->getCumulativeTransformation()).getMatrix());
	C3Vector dummyPos(dummyCTM.X);
	float theDistance=dummyPos.getLength();
	if (CCollDistInterface::isPointInsideVolume1AndOutsideVolume2(dummyPos,&sensor->convexVolume->planesInside,&sensor->convexVolume->planesOutside) )
	{
		if (theDistance<dist)
		{
			dist=theDistance;
			detectedPt=dummyPos;
			return(true);
		}
	}
	return(false);
}

bool CProxSensorRoutine::_detectShape(int sensorID,int shapeID,
			bool closestFeatureMode,bool angleLimitation,float maxAngle,C3Vector& detectedPt,float& dist,
			bool frontFace,bool backFace,C3Vector& triNormalNotNormalized,bool overrideShapeDetectableFlag)
{	// Return value is false if nothing was detected

	// Get the objects:
	CProxSensor* sensor=App::ct->objCont->getProximitySensor(sensorID);
	if (sensor==NULL)
		return(false);
	CShape* shape=App::ct->objCont->getShape(shapeID);
	if (shape==NULL)
		return(false);
	if ( ((shape->getMainProperty()&sensor->getSensableType())==0)&&(!overrideShapeDetectableFlag) )
		return(false);

	// Is shape initialized for detection?
	shape->initializeCalculationStructureIfNeeded();

	C7Vector inv(sensor->getCumulativeTransformation().getInverse());
	C4X4Matrix shapeRTM((inv*shape->getCumulativeTransformation()).getMatrix());

	float cosAngle=(float)cos(maxAngle);
	if (!angleLimitation)
		cosAngle=2.0f; // This means we don't want to check for a max angle!

	if (sensor->getSensorType()==sim_proximitysensor_ray_subtype)
	{ // ray sensor here:
		C3Vector lp(0.0f,0.0f,sensor->convexVolume->getOffset());
		C3Vector lvFar(0.0f,0.0f,sensor->convexVolume->getRange());
		char* closeDetectionTriggered=NULL;
		char dummy=0;
		if (sensor->convexVolume->getSmallestDistanceEnabled())
			closeDetectionTriggered=&dummy;

		bool result=CCollDistInterface::getRayProxSensorDistance_ifSmaller(
				shape->geomData->collInfo,shapeRTM,dist,lp,sensor->convexVolume->getSmallestDistanceAllowed(),lvFar,cosAngle,
				detectedPt,!closestFeatureMode,frontFace,backFace,closeDetectionTriggered,triNormalNotNormalized,(void*)CProxSensorRoutine::checkForOcclusion);
		if ((closeDetectionTriggered!=NULL)&&(closeDetectionTriggered[0]!=0))
		{ // We triggered the sensor in the forbiden zone: to inform the calling routine, we do following:
			dist=0.0f;
			return(true);
		}
		return(result);
	}
	else
	{ // regular sensor here:
		std::vector<float>* edges=NULL;
		if (CIloIlo::debugSensorCutting)
			edges=&sensor->cutEdges;
		return( CCollDistInterface::getProxSensorDistance_ifSmaller(
				shape->geomData->collInfo,shapeRTM,dist,&sensor->convexVolume->planesInside,
				&sensor->convexVolume->planesOutside,cosAngle,
				detectedPt,!closestFeatureMode,frontFace,backFace,edges,triNormalNotNormalized,(void*)CProxSensorRoutine::checkForOcclusion) );
	}
}


bool CProxSensorRoutine::_detectGroup(int sensorID,int groupID,
			bool closestFeatureMode,bool angleLimitation,float maxAngle,C3Vector& detectedPt,float& dist,
			bool frontFace,bool backFace,int& detectedObject,float minThreshold,C4X4Matrix& sensingVolumeBox,C3Vector& sensingVolumeSize,C3Vector& triNormalNotNormalized)
{	// Return value is false if nothing was detected. If groupID==-1, all objects in the scene will be detected
	detectedObject=-1;
	// Get the objects:
	CProxSensor* sensor=App::ct->objCont->getProximitySensor(sensorID);
	if (sensor==NULL)
		return(false);
	int propMask=sensor->getSensableType();
	std::vector<C3DObject*> group;
	if (!App::ct->collections->getShapesAndDummiesFromGroup(groupID,&group,propMask,false))
	{ // Here we probably want to check all objects in the scene:
		if (groupID!=-1)
			return(false);
		App::ct->objCont->getAllShapesAndDummiesFromScene(group,propMask,false);
	}
	// Build the collision nodes only when needed. So do it right here!
	for (int i=0;i<int(group.size());i++)
	{
		if (group[i]->getObjectType()==sim_object_shape_type)
			((CShape*)group[i])->initializeCalculationStructureIfNeeded();
	}
	bool somethingWasDetected=false;

	// We first process all dummies because it's so fast:
	C7Vector inv(sensor->getCumulativeTransformation().getInverse());
	C3Vector sensorPoint(0.0f,0.0f,0.0f);
	int i=0;
	while (i<int(group.size()))
	{
		if (group[i]->getObjectType()==sim_object_dummy_type)
		{
			if (sensor->getSensorType()!=sim_proximitysensor_ray_subtype)
			{ // ray sensors cannot detect dummies!
				C3Vector dummyPos((inv*group[i]->getCumulativeTransformation()).X);
				float theDistance=dummyPos.getLength();
				if (CCollDistInterface::isPointInsideVolume1AndOutsideVolume2(dummyPos,&sensor->convexVolume->planesInside,&sensor->convexVolume->planesOutside) )
				{
					if (theDistance<dist)
					{
						if (theDistance<minThreshold)
							return(false); // we are under the minimum threshold, all detection stops here!
						dist=theDistance;
						detectedPt=dummyPos;
						somethingWasDetected=true;
						detectedObject=group[i]->getID();
						if (!closestFeatureMode)
							return(true);
					}
				}
			}
			group.erase(group.begin()+i);
		}
		else
			i++;
	}
	// Here group doesn't contain any dummy anymore
	float cosMaxAngle=(float)cos(maxAngle);
	if (!angleLimitation)
	{ // We don't care about the angle!
		maxAngle=1000.0f;
		cosMaxAngle=2.0f;
	}

	// We proceed with detection (no ordering):
	float cosAngle=(float)cos(maxAngle);
	if (!angleLimitation)
		cosAngle=2.0f; // This means we don't want to check for a max angle!
	
	for (int i=0;i<int(group.size());i++)
	{
		if (group[i]->getObjectType()==sim_object_shape_type)
		{
			CShape* shape=(CShape*)group[i];
			C4X4Matrix m((inv*shape->getCumulativeTransformation()).getMatrix());

			if (sensor->getSensorType()==sim_proximitysensor_ray_subtype)
			{ // ray sensor here:
				C3Vector lp(0.0f,0.0f,sensor->convexVolume->getOffset());
				C3Vector lvFar(0.0f,0.0f,sensor->convexVolume->getRange());
				char* closeDetectionTriggered=NULL;
				char dummy=0;
				if (sensor->convexVolume->getSmallestDistanceEnabled())
					closeDetectionTriggered=&dummy;

				if (CCollDistInterface::getRayProxSensorDistance_ifSmaller(
					shape->geomData->collInfo,m,dist,lp,sensor->convexVolume->getSmallestDistanceAllowed(),lvFar,cosAngle,
					detectedPt,!closestFeatureMode,frontFace,backFace,closeDetectionTriggered,triNormalNotNormalized,(void*)CProxSensorRoutine::checkForOcclusion))
				{
					somethingWasDetected=true;
					detectedObject=shape->getID();
				}

				if ((closeDetectionTriggered!=NULL)&&(closeDetectionTriggered[0]!=0))
				{ // We triggered the sensor in the forbiden zone: all detection stops here
					return(false);
				}
				if (somethingWasDetected&&(!closestFeatureMode))
					return(true); // fast mode
			}
			else
			{ // no ray sensor
// We first check if the sensor volume OBB is colliding with the shape OBB: (doesn't really make a difference!) 2009/07/21
//				if (CAlgos::boxBoxCollisionStatic(shapeBox,shapeSize,sensingVolumeBox,sensingVolumeSize))
				{

					std::vector<float>* edges=NULL;
					if (CIloIlo::debugSensorCutting)
						edges=&sensor->cutEdges;
					if (CCollDistInterface::getProxSensorDistance_ifSmaller(
						shape->geomData->collInfo,m,dist,&sensor->convexVolume->planesInside,
						&sensor->convexVolume->planesOutside,cosAngle,
						detectedPt,!closestFeatureMode,frontFace,backFace,edges,triNormalNotNormalized,(void*)CProxSensorRoutine::checkForOcclusion) )
					{
						if (dist<minThreshold)
							return(false); // we are under the minimum threshold, all detection stops here!
						somethingWasDetected=true;
						detectedObject=shape->getID();
						if (!closestFeatureMode)
							return(true);
					}
				}
			}
		}
	}

	return(somethingWasDetected);
}

/*
void CProxSensorRoutine::_prepareGroupOcclusionCheck(int sensorID,int groupID,bool frontFace,bool backFace)
{
	CProxSensor* sensor=App::ct->objCont->getProximitySensor(sensorID);
	if (sensor==NULL)
		return;
	int propMask=sensor->getSensableType();
	std::vector<C3DObject*> group;
	if (!App::ct->collections->getShapesFromGroup(groupID,&group,propMask,false))
	{ // Here we probably want to check all objects in the scene:
		if (groupID!=-1)
			return;
		std::vector<C3DObject*> exceptionListEmpty;
		App::ct->objCont->getAllShapesFromSceneExcept(exceptionListEmpty,group,propMask,false);
		if (group.size()==0)
			return;
	}
	_occlusionCheck=true;

	// Build the collision nodes only when needed. So do it right here!
	for (int i=0;i<int(group.size());i++)
	{
		if (group[i]->getObjectType()==sim_object_shape_type)
			((CShape*)group[i])->initializeCalculationStructureIfNeeded();
	}
	C7Vector inv(sensor->getCumulativeTransformation().getInverse());

	for (int i=0;i<int(group.size());i++)
	{
		CShape* shape=(CShape*)group[i];
		C4X4Matrix m((inv*shape->getCumulativeTransformation()).getMatrix());

		_shapesForOcclusionCheck.push_back(shape);
		_shapeMatricesForOcclusionCheck.push_back(m);
	}

	_sensorCloseForOcclusionCheck=0.0f;
	if ( (sensor->getSensorType()==sim_proximitysensor_cone_subtype)||(sensor->getSensorType()==sim_proximitysensor_disc_subtype) )
		_sensorCloseForOcclusionCheck=sensor->convexVolume->getRadius();
	else
	{
		if ( (sensor->getSensorType()==sim_proximitysensor_ray_subtype)&&sensor->getRandomizedDetection() )
			_sensorCloseForOcclusionCheck=sensor->convexVolume->getRadius(); // Special!!
		else
			_sensorCloseForOcclusionCheck=sensor->convexVolume->getOffset();
	}
	_frontFaceForOcclusionCheck=frontFace;
	_backFaceForOcclusionCheck=backFace;
}
*/

void CProxSensorRoutine::_prepareShapeOcclusionCheck(int sensorID,int shapeID,bool frontFace,bool backFace,bool overrideShapeDetectableFlag)
{
	CSensingThreadPool::setProtectedAccess(true);
	SOcclusionCheckDat dat;
	dat.freeSlot=false;
	dat.occlusionCheck=false;
	dat.threadID=VThread::getCurrentThreadId();
	
	CProxSensor* sensor=App::ct->objCont->getProximitySensor(sensorID);
	CShape* shape=App::ct->objCont->getShape(shapeID);
	if ((sensor!=NULL)&&(shape!=NULL))
	{
		if ( ((shape->getMainProperty()&sensor->getSensableType())==0)&&(!overrideShapeDetectableFlag) )
		{ // no occlusion check here!

		}
		else
		{ // We wanna do occlusion check!
			dat.occlusionCheck=true;
			// Is shape initialized for detection?
			shape->initializeCalculationStructureIfNeeded();

			C7Vector inv(sensor->getCumulativeTransformation().getInverse());
			C4X4Matrix shapeRTM((inv*shape->getCumulativeTransformation()).getMatrix());
			
			dat.shapes.push_back(shape);
			dat.shapeMatrices.push_back(shapeRTM);
			dat.sensorClose=0.0f;

			if ( (sensor->getSensorType()==sim_proximitysensor_cone_subtype)||(sensor->getSensorType()==sim_proximitysensor_disc_subtype) )
				dat.sensorClose=sensor->convexVolume->getRadius();
			else
			{
				if ( (sensor->getSensorType()==sim_proximitysensor_ray_subtype)&&sensor->getRandomizedDetection() )
					dat.sensorClose=sensor->convexVolume->getRadius(); // Special!!
				else
					dat.sensorClose=sensor->convexVolume->getOffset();
			}
			dat.frontFace=frontFace;
			dat.backFace=backFace;
		}
	}

	bool foundFreeSlot=false;
	for (int i=0;i<int(_occlusionCheckData.size());i++)
	{
		if (_occlusionCheckData[i].freeSlot)
		{
			foundFreeSlot=true;
			_occlusionCheckData[i]=dat;
			break;
		}
	}
	if (!foundFreeSlot)
		_occlusionCheckData.push_back(dat);
	CSensingThreadPool::setProtectedAccess(false);
}


void CProxSensorRoutine::_prepareGroupOcclusionCheck(int sensorID,int groupID,bool frontFace,bool backFace)
{
	CSensingThreadPool::setProtectedAccess(true);
	SOcclusionCheckDat dat;
	dat.freeSlot=false;
	dat.occlusionCheck=false;
	dat.threadID=VThread::getCurrentThreadId();

	CProxSensor* sensor=App::ct->objCont->getProximitySensor(sensorID);
	int propMask=sensor->getSensableType();
	std::vector<C3DObject*> group;
	bool doTheCheck=false;
	if (sensor!=NULL)
	{
		if (!App::ct->collections->getShapesFromGroup(groupID,&group,propMask,false))
		{ // Here we probably want to check all objects in the scene:
			doTheCheck=true;
			if (groupID!=-1)
				doTheCheck=false;
			else
			{
				std::vector<C3DObject*> exceptionListEmpty;
				App::ct->objCont->getAllShapesFromSceneExcept(exceptionListEmpty,group,propMask,false);
				doTheCheck=(group.size()!=0);
			}
		}
		else
			doTheCheck=true;
	}

	if (doTheCheck)
	{
		dat.occlusionCheck=true;
		// Build the collision nodes only when needed. So do it right here!
		for (int i=0;i<int(group.size());i++)
		{
			if (group[i]->getObjectType()==sim_object_shape_type)
				((CShape*)group[i])->initializeCalculationStructureIfNeeded();
		}
		C7Vector inv(sensor->getCumulativeTransformation().getInverse());

		for (int i=0;i<int(group.size());i++)
		{
			CShape* shape=(CShape*)group[i];
			C4X4Matrix m((inv*shape->getCumulativeTransformation()).getMatrix());

			dat.shapes.push_back(shape);
			dat.shapeMatrices.push_back(m);
		}

		dat.sensorClose=0.0f;
		if ( (sensor->getSensorType()==sim_proximitysensor_cone_subtype)||(sensor->getSensorType()==sim_proximitysensor_disc_subtype) )
			dat.sensorClose=sensor->convexVolume->getRadius();
		else
		{
			if ( (sensor->getSensorType()==sim_proximitysensor_ray_subtype)&&sensor->getRandomizedDetection() )
				dat.sensorClose=sensor->convexVolume->getRadius(); // Special!!
			else
				dat.sensorClose=sensor->convexVolume->getOffset();
		}

		dat.frontFace=frontFace;
		dat.backFace=backFace;
	}


	bool foundFreeSlot=false;
	for (int i=0;i<int(_occlusionCheckData.size());i++)
	{
		if (_occlusionCheckData[i].freeSlot)
		{
			foundFreeSlot=true;
			_occlusionCheckData[i]=dat;
			break;
		}
	}
	if (!foundFreeSlot)
		_occlusionCheckData.push_back(dat);
	CSensingThreadPool::setProtectedAccess(false);
}


//void CProxSensorRoutine::_prepareShapeOcclusionCheck(int sensorID,int shapeID,bool frontFace,bool backFace,bool overrideShapeDetectableFlag)
//{
//	CProxSensor* sensor=App::ct->objCont->getProximitySensor(sensorID);
//	if (sensor==NULL)
//		return;
//	CShape* shape=App::ct->objCont->getShape(shapeID);
//	if (shape==NULL)
//		return;
//	if ( ((shape->getMainProperty()&sensor->getSensableType())==0)&&(!overrideShapeDetectableFlag) )
//		return;
//	_occlusionCheck=true;
//
//	// Is shape initialized for detection?
//	shape->initializeCalculationStructureIfNeeded();
//
//	C7Vector inv(sensor->getCumulativeTransformation().getInverse());
//	C4X4Matrix shapeRTM((inv*shape->getCumulativeTransformation()).getMatrix());
//
//	_shapesForOcclusionCheck.push_back(shape);
//	_shapeMatricesForOcclusionCheck.push_back(shapeRTM);
//
//	_sensorCloseForOcclusionCheck=0.0f;
//	if ( (sensor->getSensorType()==sim_proximitysensor_cone_subtype)||(sensor->getSensorType()==sim_proximitysensor_disc_subtype) )
//		_sensorCloseForOcclusionCheck=sensor->convexVolume->getRadius();
//	else
//	{
//		if ( (sensor->getSensorType()==sim_proximitysensor_ray_subtype)&&sensor->getRandomizedDetection() )
//			_sensorCloseForOcclusionCheck=sensor->convexVolume->getRadius(); // Special!!
//		else
//			_sensorCloseForOcclusionCheck=sensor->convexVolume->getOffset();
//	}
//	_frontFaceForOcclusionCheck=frontFace;
//	_backFaceForOcclusionCheck=backFace;
//}

/*
bool CProxSensorRoutine::checkForOcclusion(const float _detectedPt[])
{ // True means there are occlusions
	if (!_occlusionCheck)
		return(false);
	C3Vector detectedPt(_detectedPt);
	_occlusionCheck=false; // Disable the flag temporarily, otherwise this routine might call itself infinitely!
	C3Vector lp(0.0f,0.0f,0.0f);
	C3Vector lvFar(detectedPt);
	float l=detectedPt.getLength();
	if (l!=0.0f)
		lp=detectedPt*(_sensorCloseForOcclusionCheck/l);
	lvFar-=lp;

	C3Vector triNormalNotNormalized;
	C3Vector detectedPtTmp;

	for (int i=0;i<int(_shapesForOcclusionCheck.size());i++)
	{
		float dist=l;
		bool result=CCollDistInterface::getRayProxSensorDistance_ifSmaller(
				_shapesForOcclusionCheck[i]->geomData->collInfo,_shapeMatricesForOcclusionCheck[i],
				dist,lp,0.0f,lvFar,2.0f,detectedPtTmp,false,_frontFaceForOcclusionCheck,_backFaceForOcclusionCheck,NULL,triNormalNotNormalized,(void*)CProxSensorRoutine::checkForOcclusion);
		if (result)
		{
			if (dist<l*0.999f)
			{
				_occlusionCheck=true; // Restore the flag before leaving
				return(true);
			}
		}
	}
	_occlusionCheck=true; // Restore the flag before leaving
	return(false);
}
*/

bool CProxSensorRoutine::checkForOcclusion(const float _detectedPt[])
{ // True means there are occlusions
	bool retVal=false;
	CSensingThreadPool::setProtectedAccess(true);
	VTHREAD_ID_TYPE thisThreadID=VThread::getCurrentThreadId();
	for (int i=0;i<int(_occlusionCheckData.size());i++)
	{
//		printf("%i\n",i);
		if ( (!_occlusionCheckData[i].freeSlot)&&VThread::areThreadIDsSame(_occlusionCheckData[i].threadID,thisThreadID) )
		{
			if (!_occlusionCheckData[i].occlusionCheck)
				break;

			C3Vector detectedPt(_detectedPt);
			_occlusionCheckData[i].occlusionCheck=false; // Disable the flag temporarily, otherwise this routine might call itself infinitely!
			C3Vector lp(0.0f,0.0f,0.0f);
			C3Vector lvFar(detectedPt);
			float l=detectedPt.getLength();
			if (l!=0.0f)
				lp=detectedPt*(_occlusionCheckData[i].sensorClose/l);
			lvFar-=lp;

			C3Vector triNormalNotNormalized;
			C3Vector detectedPtTmp;

			CSensingThreadPool::setProtectedAccess(false); // make sure to unlock!
			for (int j=0;j<int(_occlusionCheckData[i].shapes.size());j++)
			{
				float dist=l;
				bool result=CCollDistInterface::getRayProxSensorDistance_ifSmaller(
						_occlusionCheckData[i].shapes[j]->geomData->collInfo,_occlusionCheckData[i].shapeMatrices[j],
						dist,lp,0.0f,lvFar,2.0f,detectedPtTmp,false,_occlusionCheckData[i].frontFace,_occlusionCheckData[i].backFace,NULL,triNormalNotNormalized,(void*)CProxSensorRoutine::checkForOcclusion);
				if (result)
				{
					if (dist<l*0.999f)
					{
						retVal=true;
						break;
					}
				}
			}
			CSensingThreadPool::setProtectedAccess(true); // make sure to lock again!
			_occlusionCheckData[i].occlusionCheck=true; // Restore the flag before leaving
			break;
		}
	}

	CSensingThreadPool::setProtectedAccess(false);
	return(retVal);
}

/*
void CProxSensorRoutine::_clearOcclusionCheckData()
{
	_occlusionCheck=false;
	_shapesForOcclusionCheck.clear();
	_shapeMatricesForOcclusionCheck.clear();
}
*/

void CProxSensorRoutine::_clearOcclusionCheckData()
{
	CSensingThreadPool::setProtectedAccess(true);
	VTHREAD_ID_TYPE thisThreadID=VThread::getCurrentThreadId();
	for (int i=0;i<int(_occlusionCheckData.size());i++)
	{
		if (VThread::areThreadIDsSame(thisThreadID,_occlusionCheckData[i].threadID))
		{
			_occlusionCheckData[i].freeSlot=true;
			_occlusionCheckData[i].occlusionCheck=false;
			_occlusionCheckData[i].shapes.clear();
			_occlusionCheckData[i].shapeMatrices.clear();
			break;
		}
	}
	CSensingThreadPool::setProtectedAccess(false);
}

bool CProxSensorRoutine::randomizedDetectEntity(int sensorID,int entityID,
						 bool closestFeatureMode,bool angleLimitation,float maxAngle,C3Vector& detectedPt,
						 float& dist,bool frontFace,bool backFace,int& detectedObject,float minThreshold,C3Vector& triNormal,bool overrideDetectableFlagIfNonCollection,bool checkOcclusions,const std::vector<C3Vector>& normalizedRays,std::vector<float>& individualRayDetectionState,int requiredDetectionCount,bool handledByWorkThread)
{ // entityID==-1 --> checks all objects in the scene
	bool returnValue=false;
	detectedObject=-1;
	C3DObject* object=App::ct->objCont->getObject(entityID);
	CProxSensor* sens=App::ct->objCont->getProximitySensor(sensorID);
	if (sens==NULL)
		return(false); // should never happen!
	if (sens->getSensorType()!=sim_proximitysensor_ray_subtype)
		return(false);
	if (!sens->getRandomizedDetection())
		return(false);

	if (!handledByWorkThread)
		App::ct->infoDisplay->proximitySensorSimulationStart();

	std::vector<C3DObject*> shapesToCheck;
	std::vector<C4X4Matrix> shapeMatricesToCheck;

	// First get all shapes to check:
	if (object==NULL)
	{
		// Prepare some sort of callback if needed:
		if ( angleLimitation&&checkOcclusions )
			_prepareGroupOcclusionCheck(sensorID,entityID,frontFace,backFace);

		// Now get the shapes:
		int propMask=sens->getSensableType();
		if (!App::ct->collections->getShapesFromGroup(entityID,&shapesToCheck,propMask,false))
		{ // Here we probably want to check all objects in the scene:
			if (entityID==-1)
			{
				std::vector<C3DObject*> exceptionListEmpty;
				App::ct->objCont->getAllShapesFromSceneExcept(exceptionListEmpty,shapesToCheck,propMask,false);
			}
		}
	}
	else
	{
		if (object->getObjectType()==sim_object_shape_type)
		{
			if ( (object->getMainProperty()&sens->getSensableType())||overrideDetectableFlagIfNonCollection )
			{
				// Prepare some sort of callback if needed:
				if ( angleLimitation&&checkOcclusions )
					_prepareShapeOcclusionCheck(sensorID,entityID,frontFace,backFace,overrideDetectableFlagIfNonCollection);

				shapesToCheck.push_back(object);
			}
		}
	}

	// Now get all matrices from the shapes and build the collision nodes (if needed):
	C7Vector inv(sens->getCumulativeTransformation().getInverse());
	for (int i=0;i<int(shapesToCheck.size());i++)
	{
		C4X4Matrix m((inv*shapesToCheck[i]->getCumulativeTransformation()).getMatrix());
		shapeMatricesToCheck.push_back(m);
		((CShape*)shapesToCheck[i])->initializeCalculationStructureIfNeeded();
	}

	// Now prepare for detection
	float cosAngle=(float)cos(maxAngle);
	if (!angleLimitation)
		cosAngle=2.0f; // This means we don't want to check for a max angle!
	char* closeDetectionTriggered=NULL;
	char dummyVal=0;
	if (sens->convexVolume->getSmallestDistanceEnabled())
		closeDetectionTriggered=&dummyVal;
	int closeDetectionCnt=0;
	int normalDetectionCnt=0;
	C3Vector averageDetectionVector;
	C3Vector averageNormalVector;
	averageDetectionVector.clear();
	float averageDetectionDist=0.0f;

	// Now detect!
	for (int i=0;i<int(normalizedRays.size());i++)
	{
		C3Vector lp(normalizedRays[i]*sens->convexVolume->getRadius()); // Here we have radius instead of offset! Special with randomized detection!!
		C3Vector lvFar(normalizedRays[i]*sens->convexVolume->getRange());

		float distTmp=dist;
		C3Vector detectedPtTmp;
		C3Vector triNormalNotNormalizedTmp;
		int detectedObjectTmp=0; // '=0' not needed, but to avoid having a warning
		bool detec=false;
		for (int j=0;j<int(shapesToCheck.size());j++)
		{
			bool result=CCollDistInterface::getRayProxSensorDistance_ifSmaller(
					((CShape*)shapesToCheck[j])->geomData->collInfo,shapeMatricesToCheck[j],distTmp,lp,sens->convexVolume->getSmallestDistanceAllowed(),lvFar,cosAngle,
					detectedPtTmp,!closestFeatureMode,frontFace,backFace,closeDetectionTriggered,triNormalNotNormalizedTmp,(void*)CProxSensorRoutine::checkForOcclusion);
			if ((closeDetectionTriggered!=NULL)&&(closeDetectionTriggered[0]!=0))
			{ // We triggered the sensor in the forbiden zone
				closeDetectionCnt++;
				break;
			}
			if (result)
			{ // We triggered the sensor normally
				detec=true;
				detectedObjectTmp=shapesToCheck[j]->getID();
				if (!closestFeatureMode)
					break;
			}
		}
		if (detec)
		{
			normalDetectionCnt++;
			individualRayDetectionState[i]=distTmp;
			averageDetectionVector+=detectedPtTmp;
			averageDetectionDist+=distTmp;
			detectedObject=detectedObjectTmp;
			averageNormalVector+=triNormalNotNormalizedTmp.getNormalized();
		}
		if (closeDetectionCnt>=requiredDetectionCount)
		{ // We triggered too often in the forbidden zone --> we set dist to zero to inform the calling routine:
			dist=0.0f;
			break;
		}
		if (normalDetectionCnt>=requiredDetectionCount)
			break; // We finally triggered the sensor and leave now
	}

	if (normalDetectionCnt>=requiredDetectionCount)
	{
		returnValue=true;
		dist=averageDetectionDist/float(normalDetectionCnt);
		detectedPt=averageDetectionVector/float(normalDetectionCnt);
		triNormal=averageNormalVector/float(normalDetectionCnt);
	}

	if ( angleLimitation&&checkOcclusions )
		_clearOcclusionCheckData();

	if (!handledByWorkThread)
		App::ct->infoDisplay->proximitySensorSimulationEnd(returnValue);
	return(returnValue);
}

// From here we have the primitives detection:
bool CProxSensorRoutine::detectPrimitive(int sensorID,float* vertexPointer,int itemType,int itemCount,
		bool closestFeatureMode,bool angleLimitation,float maxAngle,C3Vector& detectedPt,
		float& dist,bool frontFace,bool backFace,float minThreshold,C3Vector& triNormal)
{
	bool returnValue=false;
	CProxSensor* sens=App::ct->objCont->getProximitySensor(sensorID);
	if (sens==NULL)
		return(false); // should never happen!
	App::ct->infoDisplay->proximitySensorSimulationStart();

	// Now prepare for detection
	float cosAngle=(float)cos(maxAngle);
	if (!angleLimitation)
		cosAngle=2.0f; // This means we don't want to check for a max angle!
	C7Vector sensInv(sens->getCumulativeTransformationPart1().getInverse());

	for (int i=0;i<itemCount;i++)
	{
		bool detect=false;
		if (itemType==0)
		{ // detecting points
			C3Vector p0(vertexPointer[6*i+0],vertexPointer[6*i+1],vertexPointer[6*i+2]);
			p0*=sensInv;
			float l=p0.getLength();
			if (l<=dist)
			{ // ok, the point is closer..
				detect=CCollDistInterface::isPointInsideVolume1AndOutsideVolume2(p0,&sens->convexVolume->planesInside,&sens->convexVolume->planesOutside);
				if (detect)
					detectedPt=p0;
			}
		}
		if (itemType==1)
		{ // detecting segments
			C3Vector p0(vertexPointer[6*i+0],vertexPointer[6*i+1],vertexPointer[6*i+2]);
			C3Vector p1(vertexPointer[6*i+3],vertexPointer[6*i+4],vertexPointer[6*i+5]);
			p0*=sensInv;
			p1*=sensInv;
			detect=CCollDistInterface::getProxSensorDistanceToSegment_ifSmaller(p0,p1,dist,&sens->convexVolume->planesInside,&sens->convexVolume->planesOutside,cosAngle,detectedPt);
		}
		if (itemType==2)
		{ // detecting triangles
			C3Vector a0(vertexPointer[9*i+0],vertexPointer[9*i+1],vertexPointer[9*i+2]);
			C3Vector e0(vertexPointer[9*i+3],vertexPointer[9*i+4],vertexPointer[9*i+5]);
			C3Vector e1(vertexPointer[9*i+6],vertexPointer[9*i+7],vertexPointer[9*i+8]);
			a0*=sensInv;
			e0*=sensInv;
			e1*=sensInv;
			e0-=a0;
			e1-=a0;
			detect=CCollDistInterface::getProxSensorDistanceToTriangle_ifSmaller(a0,e0,e1,dist,&sens->convexVolume->planesInside,&sens->convexVolume->planesOutside,cosAngle,detectedPt,frontFace,backFace,triNormal);
		}
		if (detect)
		{
			returnValue=true;
			if (sens->convexVolume->getSmallestDistanceEnabled())
			{
				if (dist<sens->convexVolume->getSmallestDistanceAllowed())
				{ // We triggered the sensor in the forbiden zone
					returnValue=false;
					break;
				}
			}
			if (!closestFeatureMode)
				break; // No fast detection, we leave here..
		}
	}

	if (returnValue)
		triNormal.normalize();

		
	App::ct->infoDisplay->proximitySensorSimulationEnd(returnValue);
	return(returnValue);
}


bool CProxSensorRoutine::randomizedDetectPrimitive(int sensorID,float* vertexPointer,int itemType,int itemCount,
		bool closestFeatureMode,bool angleLimitation,float maxAngle,C3Vector& detectedPt,
		float& dist,bool frontFace,bool backFace,float minThreshold,C3Vector& triNormal,const std::vector<C3Vector>& normalizedRays,std::vector<float>& individualRayDetectionState,int requiredDetectionCount)
{
	bool returnValue=false;
	CProxSensor* sens=App::ct->objCont->getProximitySensor(sensorID);
	if (sens==NULL)
		return(false); // should never happen!
	if (sens->getSensorType()!=sim_proximitysensor_ray_subtype)
		return(false);
	if (!sens->getRandomizedDetection())
		return(false);
	App::ct->infoDisplay->proximitySensorSimulationStart();

	// Now prepare for detection
	float cosAngle=(float)cos(maxAngle);
	if (!angleLimitation)
		cosAngle=2.0f; // This means we don't want to check for a max angle!
	C7Vector sensInv(sens->getCumulativeTransformationPart1().getInverse());

	int closeDetectionCnt=0;
	int normalDetectionCnt=0;
	C3Vector averageDetectionVector;
	C3Vector averageNormalVector;
	averageDetectionVector.clear();
	float averageDetectionDist=0.0f;

	// Now detect!
	for (int j=0;j<int(normalizedRays.size());j++)
	{
		C3Vector lp(normalizedRays[j]*sens->convexVolume->getRadius()); // Here we have radius instead of offset! Special with randomized detection!!
		C3Vector lvFar(normalizedRays[j]*sens->convexVolume->getRange());

		float distTmp=dist;
		C3Vector detectedPtTmp;
		C3Vector triNormalNotNormalizedTmp;
		bool detec=false;


		for (int i=0;i<itemCount;i++)
		{
			bool itemDetect=false;
			if (itemType==0)
			{ // detecting points
				C3Vector p0(vertexPointer[6*i+0],vertexPointer[6*i+1],vertexPointer[6*i+2]);
				p0*=sensInv;
				float l=p0.getLength();
				if (l<=dist)
				{ // ok, the point is closer..
					itemDetect=CCollDistInterface::isPointInsideVolume1AndOutsideVolume2(p0,&sens->convexVolume->planesInside,&sens->convexVolume->planesOutside);
					if (itemDetect)
						detectedPt=p0;
				}
			}
			if (itemType==1)
			{ // detecting segments
				C3Vector p0(vertexPointer[6*i+0],vertexPointer[6*i+1],vertexPointer[6*i+2]);
				C3Vector p1(vertexPointer[6*i+3],vertexPointer[6*i+4],vertexPointer[6*i+5]);
				p0*=sensInv;
				p1*=sensInv;
				itemDetect=CCollDistInterface::getProxSensorDistanceToSegment_ifSmaller(p0,p1,dist,&sens->convexVolume->planesInside,&sens->convexVolume->planesOutside,cosAngle,detectedPt);
			}
			if (itemType==2)
			{ // detecting triangles
				C3Vector a0(vertexPointer[9*i+0],vertexPointer[9*i+1],vertexPointer[9*i+2]);
				C3Vector e0(vertexPointer[9*i+3],vertexPointer[9*i+4],vertexPointer[9*i+5]);
				C3Vector e1(vertexPointer[9*i+6],vertexPointer[9*i+7],vertexPointer[9*i+8]);
				a0*=sensInv;
				e0*=sensInv;
				e1*=sensInv;
				e0-=a0;
				e1-=a0;
				itemDetect=CCollDistInterface::getProxSensorDistanceToTriangle_ifSmaller(a0,e0,e1,dist,&sens->convexVolume->planesInside,&sens->convexVolume->planesOutside,cosAngle,detectedPt,frontFace,backFace,triNormal);
			}
			if (itemDetect)
			{
				detec=true;
				if (sens->convexVolume->getSmallestDistanceEnabled())
				{
					if (dist<sens->convexVolume->getSmallestDistanceAllowed())
					{ // We triggered the sensor in the forbiden zone
						closeDetectionCnt++;
						detec=false;
						break;
					}
				}
				if (!closestFeatureMode)
					break; // No fast detection, we leave here..
			}
		}


		if (detec)
		{
			normalDetectionCnt++;
			individualRayDetectionState[j]=distTmp;
			averageDetectionVector+=detectedPtTmp;
			averageDetectionDist+=distTmp;
			averageNormalVector+=triNormalNotNormalizedTmp.getNormalized();
		}
		if (closeDetectionCnt>=requiredDetectionCount)
		{ // We triggered too often in the forbidden zone --> we set dist to zero to inform the calling routine:
			dist=0.0f;
			break;
		}
		if (normalDetectionCnt>=requiredDetectionCount)
			break; // We finally triggered the sensor and leave now
	}

	if (normalDetectionCnt>=requiredDetectionCount)
	{
		returnValue=true;
		dist=averageDetectionDist/float(normalDetectionCnt);
		detectedPt=averageDetectionVector/float(normalDetectionCnt);
		triNormal=averageNormalVector/float(normalDetectionCnt);
	}

	App::ct->infoDisplay->proximitySensorSimulationEnd(returnValue);
	return(returnValue);
}
