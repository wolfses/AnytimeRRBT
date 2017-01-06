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
#include "proximitySensor.h"
#include "Tt.h"
#include "OGL.h"
#include "IloIlo.h"
#include "proxSensorRoutine.h"

#include "VDateTime.h"
#include "sensingThreadPool.h"
#include "ttUtil.h"
#include "easyLock.h"
#include "App.h"
#include "collDistInterface.h"

CProxSensor::CProxSensor(int theType)
{
	commonInit();
	setSensorType(theType);
}

CProxSensor::CProxSensor()
{ // needed by the serialization routine only!
	commonInit();
}

CProxSensor::~CProxSensor()
{
	delete convexVolume;
}

void CProxSensor::setCheckOcclusions(bool c)
{
	_checkOcclusions=c;
}

bool CProxSensor::getCheckOcclusions()
{
	return(_checkOcclusions);
}

void CProxSensor::setRandomizedDetection(bool enable)
{
	if ( (sensorType==sim_proximitysensor_ray_subtype)&&(enable!=_randomizedDetection) )
	{
		_randomizedDetection=enable;
		float off=convexVolume->getOffset();
		float radius=convexVolume->getRadius();
		float range=convexVolume->getRange();
		bool forbiddenDistEnable=convexVolume->getSmallestDistanceEnabled();
		float forbiddenDist=convexVolume->getSmallestDistanceAllowed();

		if (enable)
		{
			convexVolume->setVolumeType(CONE_TYPE_CONVEX_VOLUME,_objectType,size);
			convexVolume->setOffset(0.0f);
			convexVolume->setRadius(off);
			convexVolume->setRange(range);
			convexVolume->setSmallestDistanceEnabled(forbiddenDistEnable);
			convexVolume->setSmallestDistanceAllowed(forbiddenDist);
			convexVolume->setAngle(60.0f*degToRad);
			convexVolume->setFaceNumber(32);
			convexVolume->setSubdivisions(3);
			convexVolume->setSubdivisionsFar(16);
			convexVolume->setInsideAngleThing(0.0f);
		}
		else
		{
			convexVolume->setVolumeType(RAY_TYPE_CONVEX_VOLUME,_objectType,size);
			convexVolume->setOffset(radius);
			convexVolume->setRange(range);
			convexVolume->setSmallestDistanceEnabled(forbiddenDistEnable);
			convexVolume->setSmallestDistanceAllowed(forbiddenDist);
		}
	}
}

bool CProxSensor::getRandomizedDetection()
{
	return(_randomizedDetection);
}

void CProxSensor::setRandomizedDetectionSampleCount(int c)
{
	_randomizedDetectionSampleCount=tt::getLimitedInt(1,5000,c);
	_randomizedDetectionCountForDetection=tt::getLimitedInt(1,_randomizedDetectionSampleCount,_randomizedDetectionCountForDetection);
}

int CProxSensor::getRandomizedDetectionSampleCount()
{
	return(_randomizedDetectionSampleCount);
}

void CProxSensor::setRandomizedDetectionCountForDetection(int c)
{
	_randomizedDetectionCountForDetection=tt::getLimitedInt(1,_randomizedDetectionSampleCount,c);
}

int CProxSensor::getRandomizedDetectionCountForDetection()
{
	return(_randomizedDetectionCountForDetection);
}

void CProxSensor::setShowVolumeWhenNotDetecting(bool s)
{
	_showVolumeWhenNotDetecting=s;
}
bool CProxSensor::getShowVolumeWhenNotDetecting()
{
	return(_showVolumeWhenNotDetecting);
}
void CProxSensor::setShowVolumeWhenDetecting(bool s)
{
	_showVolumeWhenDetecting=s;
}
bool CProxSensor::getShowVolumeWhenDetecting()
{
	return(_showVolumeWhenDetecting);
}

std::string CProxSensor::getObjectTypeInfo()
{
	return("Proximity sensor");
}

std::string CProxSensor::getObjectTypeInfoExtended()
{
	if (sensorType==sim_proximitysensor_pyramid_subtype)
		return("Proximity sensor (pyramid)");
	if (sensorType==sim_proximitysensor_cylinder_subtype)
		return("Proximity sensor (cylinder)");
	if (sensorType==sim_proximitysensor_disc_subtype)
		return("Proximity sensor (disc)");
	if (sensorType==sim_proximitysensor_cone_subtype)
		return("Proximity sensor (cone)");
	if (sensorType==sim_proximitysensor_ray_subtype)
		return("Proximity sensor (ray)");
	return("ERROR");
}

void CProxSensor::commonInit()
{
	convexVolume=new CConvexVolume();
	explicitHandling=false;
	displayNormals=false;
	_objectType=sim_object_proximitysensor_type;
	frontFaceDetection=true;
	backFaceDetection=true;
	closestObjectMode=true;
	normalCheck=false;
	allowedNormal=45.0f*degToRad;
	_checkOcclusions=true;
	_initialValuesInitialized=false;

	_randomizedDetection=false;
	_randomizedDetectionSampleCount=20;
	_randomizedDetectionCountForDetection=5;

	size=0.01f;
	_showVolumeWhenNotDetecting=true;
	_showVolumeWhenDetecting=true;
	_mainPropertyLocal=0;

	_sensableObject=-1;
	_sensableType=sim_objectspecialproperty_detectable_ultrasonic;
	_detectedPointValid=false;
	_calcTimeInMs=0;

	passiveVolumeColor.setColorsAllBlack();
	passiveVolumeColor.setColor(0.6f,0.0f,0.345f,AMBIENT_MODE);
	activeVolumeColor.setColorsAllBlack();
	activeVolumeColor.setColor(1.0f,0.1f,0.5f,AMBIENT_MODE);
	detectionRayColor.setColorsAllBlack();
	detectionRayColor.setColor(1.0f,1.0f,0.0f,EMISSION_MODE);
	detectionRayColor.setFlash(true);
	closestDistanceVolumeColor.setColorsAllBlack();
	closestDistanceVolumeColor.setColor(0.0f,0.0f,0.6f,AMBIENT_MODE);
	layer=PROXIMITY_SENSOR_LAYER;
	_objectName="Proximity_sensor";
}

void CProxSensor::setSensableType(int theType)
{
	_sensableType=theType;
}

int CProxSensor::getSensableType()
{
	return(_sensableType);
}

void CProxSensor::setExplicitHandling(bool setExplicit)
{
	explicitHandling=setExplicit;
}

bool CProxSensor::getExplicitHandling()
{
	return(explicitHandling);
}

int CProxSensor::getSensableObject()
{
	return(_sensableObject);
}

void CProxSensor::setSensableObject(int objectID)
{
	_sensableObject=objectID;
}

bool CProxSensor::getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices)
{
	vertices.clear();
	indices.clear();
	return(false); // for now
}

void CProxSensor::display(CViewableBase* renderingObject,int displayAttrib)
{
	FUNCTION_INSIDE_DEBUG("CProxSensor::display");
	EASYLOCK(_objectMutex);
	// At the beginning of every 3DObject display routine:
	commonStart(displayAttrib);

	// Display the bounding box:
	if (displayAttrib&sim_displayattribute_renderpass) 
		displayBoundingBox(displayAttrib);

	// Display the object:
	if (getShouldObjectBeDisplayed(displayAttrib))
	{
		App::ct->objCont->enableAuxClippingPlanes(_objectID);
		if ((App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
		{
			if (_objectProperty&sim_objectproperty_selectmodelbaseinstead)
				glLoadName(getModelSelectionID());
			else
				glLoadName(getID());
		}
		else
			glLoadName(-1);
		if ( (displayAttrib&sim_displayattribute_forcewireframe)&&(displayAttrib&sim_displayattribute_renderpass) )
			glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);

		if (CIloIlo::debugSensorCutting&&(sensorType!=sim_proximitysensor_ray_subtype))
		{
			ogl::setColor(1.0f,1.0f,1.0f,EMISSION_MODE);
			ogl::setBlending(true);
			glDepthMask(GL_FALSE);
			glBegin(GL_LINE_STRIP);
			for (int klm=0;klm<int(cutEdges.size())/3;klm++)
				glVertex3f(cutEdges[3*klm+0],cutEdges[3*klm+1],cutEdges[3*klm+2]);	
			glEnd();
			ogl::setBlending(false);
			glDepthMask(GL_TRUE);
		}

		for (int i=0;i<int(convexVolume->firstVolumeCorners.size());i++)
		{
			ogl::setColor(1.0f,1.0f,1.0f,AMBIENT_MODE);
			glPointSize(4.0f);
			glBegin(GL_POINTS);
			glVertex3fv(convexVolume->firstVolumeCorners[i].data);
			glEnd();
			glPointSize(1.0f);
		}

		if (_detectedPointValid)
		{
			detectionRayColor.makeCurrentColor();
			glLineWidth(3.0f);
			glBegin(GL_LINES);
			glVertex3f(0.0f,0.0f,0.0f);
			glVertex3f(_detectedPoint(0),_detectedPoint(1),_detectedPoint(2));
			glEnd();
			glLineWidth(1.0f);

			if ( (sensorType==sim_proximitysensor_ray_subtype)&&_randomizedDetection )
			{
				glBegin(GL_LINES);
				for (int i=0;i<int(_randomizedVectors.size());i++)
				{
					if (_randomizedVectorDetectionStates[i]!=0.0f)
					{
						glVertex3f(0.0f,0.0f,0.0f);
						glVertex3f(_randomizedVectors[i](0)*_randomizedVectorDetectionStates[i],_randomizedVectors[i](1)*_randomizedVectorDetectionStates[i],_randomizedVectors[i](2)*_randomizedVectorDetectionStates[i]);
					}
				}
				glEnd();
			}
			activeVolumeColor.makeCurrentColor();
		}
		else
			passiveVolumeColor.makeCurrentColor();

		if ((_showVolumeWhenDetecting&&_detectedPointValid)||(_showVolumeWhenNotDetecting&&(!_detectedPointValid)))
		{
			ogl::displaySphere(getSize()/2.0f);
			glBegin(GL_LINES);
			for (int i=0;i<int(convexVolume->volumeEdges.size())/6;i++)
			{
				glVertex3fv(&convexVolume->volumeEdges[6*i+0]);
				glVertex3fv(&convexVolume->volumeEdges[6*i+3]);
			}
			glEnd();
			closestDistanceVolumeColor.makeCurrentColor();
			glBegin(GL_LINES);
			for (int i=0;i<int(convexVolume->nonDetectingVolumeEdges.size())/6;i++)
			{
				glVertex3fv(&convexVolume->nonDetectingVolumeEdges[6*i+0]);
				glVertex3fv(&convexVolume->nonDetectingVolumeEdges[6*i+3]);
			}
			glEnd();
		}

		ogl::setColorsAllBlack();
		ogl::setColor(1.0f,0.0f,0.0f,EMISSION_MODE);
		glBegin(GL_LINES);
		for (int i=0;i<int(convexVolume->normalsInside.size())/6;i++)
		{
			glVertex3fv(&convexVolume->normalsInside[6*i+0]);
			glVertex3fv(&convexVolume->normalsInside[6*i+3]);
		}
		glEnd();
		ogl::setColor(0.0f,0.0f,1.0f,EMISSION_MODE);
		glBegin(GL_LINES);
		for (int i=0;i<int(convexVolume->normalsOutside.size())/6;i++)
		{
			glVertex3fv(&convexVolume->normalsOutside[6*i+0]);
			glVertex3fv(&convexVolume->normalsOutside[6*i+3]);
		}
		glEnd();
		App::ct->objCont->disableAuxClippingPlanes();
	}

	// At the end of every 3DObject display routine:
	commonFinish();
}

C3DObject* CProxSensor::copyYourself()
{	
	CProxSensor* newSensor=(CProxSensor*)copyYourselfMain();

	newSensor->_sensableObject=_sensableObject;
	newSensor->_detectedPoint=_detectedPoint;
	newSensor->_detectedPointValid=false;
	newSensor->allowedNormal=allowedNormal;
	newSensor->size=size;
	newSensor->normalCheck=normalCheck;
	newSensor->closestObjectMode=closestObjectMode;
	newSensor->frontFaceDetection=frontFaceDetection;
	newSensor->backFaceDetection=backFaceDetection;
	newSensor->explicitHandling=explicitHandling;
	newSensor->sensorType=sensorType;
	newSensor->_sensableType=_sensableType;
	newSensor->displayNormals=displayNormals;
	newSensor->_showVolumeWhenNotDetecting=_showVolumeWhenNotDetecting;
	newSensor->_showVolumeWhenDetecting=_showVolumeWhenDetecting;
	newSensor->_checkOcclusions=_checkOcclusions;

	newSensor->_randomizedDetection=_randomizedDetection;
	newSensor->_randomizedDetectionSampleCount=_randomizedDetectionSampleCount;
	newSensor->_randomizedDetectionCountForDetection=_randomizedDetectionCountForDetection;

	delete newSensor->convexVolume;
	newSensor->convexVolume=convexVolume->copyYourself();

	// Colors:
	passiveVolumeColor.copyYourselfInto(&newSensor->passiveVolumeColor);
	activeVolumeColor.copyYourselfInto(&newSensor->activeVolumeColor);
	detectionRayColor.copyYourselfInto(&newSensor->detectionRayColor);
	closestDistanceVolumeColor.copyYourselfInto(&newSensor->closestDistanceVolumeColor);

	newSensor->_initialValuesInitialized=_initialValuesInitialized;
	newSensor->_initialExplicitHandling=_initialExplicitHandling;

	return(newSensor);
}

bool CProxSensor::announceObjectWillBeErased(int objID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	// This routine can be called for objCont-objects, but also for objects
	// in the copy-buffer!! So never make use of any 
	// 'ct::objCont->getObject(id)'-call or similar
	// Return value true means 'this' has to be erased too!
	announceObjectWillBeErasedMain(objID,copyBuffer);
	if (_sensableObject==objID)
		_sensableObject=-1;
	return(false);
}

void CProxSensor::announceGroupWillBeErased(int groupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGroupWillBeErasedMain(groupID,copyBuffer);
	if (_sensableObject==groupID)
		_sensableObject=-1;
}
void CProxSensor::announceCollisionWillBeErased(int collisionID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceCollisionWillBeErasedMain(collisionID,copyBuffer);
}
void CProxSensor::announceDistanceWillBeErased(int distanceID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceDistanceWillBeErasedMain(distanceID,copyBuffer);
}
void CProxSensor::announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceIkObjectWillBeErasedMain(ikGroupID,copyBuffer);
}
void CProxSensor::announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGcsObjectWillBeErasedMain(gcsObjectID,copyBuffer);
}
void CProxSensor::performObjectLoadingMapping(std::vector<int>* map)
{
	performObjectLoadingMappingMain(map);
	if (_sensableObject<SIM_IDSTART_COLLECTION)
		_sensableObject=App::ct->objCont->getLoadingMapping(map,_sensableObject);
}
void CProxSensor::performGroupLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performGroupLoadingMappingMain(map);
	if (_sensableObject>=SIM_IDSTART_COLLECTION)
		_sensableObject=App::ct->objCont->getLoadingMapping(map,_sensableObject);
}
void CProxSensor::performCollisionLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performCollisionLoadingMappingMain(map);
}
void CProxSensor::performDistanceLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performDistanceLoadingMappingMain(map);
}
void CProxSensor::performIkLoadingMapping(std::vector<int>* map)
{
	performIkLoadingMappingMain(map);
}
void CProxSensor::performGcsLoadingMapping(std::vector<int>* map)
{
	performGcsLoadingMappingMain(map);
}

void CProxSensor::performTextureObjectLoadingMapping(std::vector<int>* map)
{
	performTextureObjectLoadingMappingMain(map);
}

void CProxSensor::performDynMaterialObjectLoadingMapping(std::vector<int>* map)
{
	performDynMaterialObjectLoadingMappingMain(map);
}

void CProxSensor::bufferMainDisplayStateVariables()
{
	bufferMainDisplayStateVariablesMain();
}

void CProxSensor::bufferedMainDisplayStateVariablesToDisplay()
{
	bufferedMainDisplayStateVariablesToDisplayMain();
}

void CProxSensor::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	initializeInitialValuesMain(simulationIsRunning);
	_initialValuesInitialized=simulationIsRunning;
	if (simulationIsRunning)
	{
		_initialExplicitHandling=explicitHandling;
	}
}

void CProxSensor::simulationAboutToStart()
{
	initializeInitialValues(true);
}

void CProxSensor::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd()&&((getCumulativeMainPropertyOverride()&sim_modelproperty_not_reset)==0))
	{
		explicitHandling=_initialExplicitHandling;
	}
	_initialValuesInitialized=false;
	simulationEndedMain();
}

void CProxSensor::serialize(CSer& ar)
{
	serializeMain(ar);
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Snt");
		ar << sensorType;
		ar.flush();

		ar.storeDataName("Vod");
		ar.setCountingMode();
		convexVolume->serialize(ar);
		if (ar.setWritingMode())
			convexVolume->serialize(ar);

		ar.storeDataName("Sns");
		ar << size;
		ar.flush();

		ar.storeDataName("Al2");
		ar << allowedNormal;
		ar.flush();

		ar.storeDataName("Pr4");
		BYTE nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,_showVolumeWhenNotDetecting);
		SIM_SET_CLEAR_BIT(nothing,1,closestObjectMode);
		SIM_SET_CLEAR_BIT(nothing,2,normalCheck);
// 12/12/2011		SIM_SET_CLEAR_BIT(nothing,3,_detectAllDetectable);
		SIM_SET_CLEAR_BIT(nothing,4,!frontFaceDetection);
		SIM_SET_CLEAR_BIT(nothing,5,!backFaceDetection);
		SIM_SET_CLEAR_BIT(nothing,6,_showVolumeWhenDetecting);
		SIM_SET_CLEAR_BIT(nothing,7,explicitHandling);
		ar << nothing;
		ar.flush();

		ar.storeDataName("Pr5");
		nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,_checkOcclusions);
		SIM_SET_CLEAR_BIT(nothing,1,_randomizedDetection);
		ar << nothing;
		ar.flush();

		ar.storeDataName("Rad");
		ar << _randomizedDetectionSampleCount << _randomizedDetectionCountForDetection;
		ar.flush();

		ar.storeDataName("Cl1");
		ar.setCountingMode();
		passiveVolumeColor.serialize(ar);
		if (ar.setWritingMode())
			passiveVolumeColor.serialize(ar);

		ar.storeDataName("Cl2");
		ar.setCountingMode();
		activeVolumeColor.serialize(ar);
		if (ar.setWritingMode())
			activeVolumeColor.serialize(ar);

		ar.storeDataName("Cl3");
		ar.setCountingMode();
		detectionRayColor.serialize(ar);
		if (ar.setWritingMode())
			detectionRayColor.serialize(ar);

		ar.storeDataName("Cl4");
		ar.setCountingMode();
		closestDistanceVolumeColor.serialize(ar);
		if (ar.setWritingMode())
			closestDistanceVolumeColor.serialize(ar);

		ar.storeDataName("Sox");
		ar << _sensableObject;
		ar.flush();

		ar.storeDataName("Sst");
		ar << _sensableType;
		ar.flush();

		ar.storeDataName(SER_END_OF_OBJECT);
	}
	else
	{		// Loading
		int byteQuantity;
		std::string theName="";
		bool occlusionCheckThingWasLoaded_backwardCompatibility2010_08_09=false;
		while (theName.compare(SER_END_OF_OBJECT)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_END_OF_OBJECT)!=0)
			{
				bool noHit=true;
				if (theName.compare("Snt")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> sensorType;
				}
				if (theName.compare("Vod")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					if (convexVolume!=NULL)
						delete convexVolume;
					convexVolume=new CConvexVolume();
					convexVolume->serialize(ar);
				}
				if (theName.compare("Sns")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> size;
				}
				if (theName.compare("Sox")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _sensableObject;
				}
				if (theName.compare("Sst")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _sensableType;
				}
				if (theName.compare("Al2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> allowedNormal;
				}
				if (theName=="Pr4")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_showVolumeWhenNotDetecting=SIM_IS_BIT_SET(nothing,0);
					closestObjectMode=SIM_IS_BIT_SET(nothing,1);
					normalCheck=SIM_IS_BIT_SET(nothing,2);
					frontFaceDetection=!SIM_IS_BIT_SET(nothing,4);
					backFaceDetection=!SIM_IS_BIT_SET(nothing,5);
					_showVolumeWhenDetecting=SIM_IS_BIT_SET(nothing,6);
					explicitHandling=SIM_IS_BIT_SET(nothing,7);
				}
				if (theName=="Pr5")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_checkOcclusions=SIM_IS_BIT_SET(nothing,0);
					_randomizedDetection=SIM_IS_BIT_SET(nothing,1);
					occlusionCheckThingWasLoaded_backwardCompatibility2010_08_09=true;
				}
				if (theName.compare("Rad")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _randomizedDetectionSampleCount >> _randomizedDetectionCountForDetection;
				}
				if (theName.compare("Cl1")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					passiveVolumeColor.serialize(ar);
				}
				if (theName.compare("Cl2")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					activeVolumeColor.serialize(ar);
				}
				if (theName.compare("Cl3")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					detectionRayColor.serialize(ar);
				}
				if (theName.compare("Cl4")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					closestDistanceVolumeColor.serialize(ar);
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
		if (!occlusionCheckThingWasLoaded_backwardCompatibility2010_08_09)
			_checkOcclusions=false;

		if ((ar.getSerializationVersionThatWroteThisFile()<17)&&App::userSettings->correctColorsAndLights)
		{ // on 29/08/2013 we corrected all default lights. So we need to correct for that change:
			CTTUtil::scaleColorUp_(passiveVolumeColor.colors);
			CTTUtil::scaleColorUp_(activeVolumeColor.colors);
			CTTUtil::scaleColorUp_(detectionRayColor.colors);
			CTTUtil::scaleColorUp_(closestDistanceVolumeColor.colors);
		}
	}
}

void CProxSensor::serializeWExtIk(CExtIkSer& ar)
{
	serializeWExtIkMain(ar);
	CDummy::serializeWExtIkStatic(ar);
}

bool CProxSensor::getSensingVolumeBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	return(convexVolume->getVolumeBoundingBox(minV,maxV));
}

bool CProxSensor::getFullBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	getSensingVolumeBoundingBox(minV,maxV);
	C3Vector m(size*0.5f,size*0.5f,size*0.5f); // sensing sphere
	C3Vector n(-size*0.5f,-size*0.5f,-size*0.5f);
	minV.keepMin(n);
	maxV.keepMax(m);
	return(true);
}

bool CProxSensor::getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	return (getFullBoundingBox(minV,maxV));
}

void CProxSensor::calculateFreshRandomizedRays()
{
	_randomizedVectors.clear();
	_randomizedVectorDetectionStates.clear();
	// Build the random rays (only direction)
	for (int i=0;i<_randomizedDetectionSampleCount;i++)
	{
		float rZ=convexVolume->getAngle()*acos(1.0f-SIM_RAND_FLOAT)/piValue;
		float sZ=sin(rZ);
		float cZ=cos(rZ);
		float rXY=SIM_RAND_FLOAT*piValTimes2;
		float sXY=sin(rXY);
		float cXY=cos(rXY);
		C3Vector v(sZ*cXY,sZ*sXY,cZ);
		_randomizedVectors.push_back(v);
		_randomizedVectorDetectionStates.push_back(0.0f);
	}
}

std::vector<C3Vector>* CProxSensor::getPointerToRandomizedRays()
{
	return(&_randomizedVectors);
}

std::vector<float>* CProxSensor::getPointerToRandomizedRayDetectionStates()
{
	return(&_randomizedVectorDetectionStates);
}

void CProxSensor::resetSensor(bool exceptExplicitHandling)
{
	if ( (!exceptExplicitHandling)||(!getExplicitHandling()) )
	{
		_detectedPointValid=false;
		_sensorResultValid=false;
		_calcTimeInMs=0;
	}
}

bool CProxSensor::handleSensor(bool handledByWorkThread,bool exceptExplicitHandling,int& detectedObjectHandle,C3Vector& detectedNormalVector)
{
	if (exceptExplicitHandling&&getExplicitHandling())
		return(false); // We don't want to handle those
	_sensorResultValid=false;
	_detectedPointValid=false;
	_calcTimeInMs=0;
	if (!App::ct->mainSettings->proximitySensorsEnabled)
		return(false);
	CSensingThreadPool::setProtectedAccess(true);
	if (!CCollDistInterface::initializeFunctionsIfNeeded()) // function is not reentrant!
	{
		CSensingThreadPool::setProtectedAccess(false);
		return(false);
	}
	CSensingThreadPool::setProtectedAccess(false);

	_sensorResultValid=true;

	int stTime=VDateTime::getTimeInMs();

	float treshhold=SIM_MAX_FLOAT;
	float minThreshold=-1.0f;
	if (convexVolume->getSmallestDistanceEnabled())
		minThreshold=convexVolume->getSmallestDistanceAllowed();

	_randomizedVectors.clear();
	_randomizedVectorDetectionStates.clear();
	if (_randomizedDetection)
	{ // randomized detection
		calculateFreshRandomizedRays();
		_detectedPointValid=CProxSensorRoutine::randomizedDetectEntity(getID(),_sensableObject,getClosestObjectMode(),getNormalCheck(),
			getAllowedNormal(),_detectedPoint,treshhold,getFrontFaceDetection(),getBackFaceDetection(),detectedObjectHandle,minThreshold,detectedNormalVector,false,_checkOcclusions,_randomizedVectors,_randomizedVectorDetectionStates,_randomizedDetectionCountForDetection,handledByWorkThread);
	}
	else
	{ // normal detection
		_detectedPointValid=CProxSensorRoutine::detectEntity(getID(),_sensableObject,getClosestObjectMode(),getNormalCheck(),
			getAllowedNormal(),_detectedPoint,treshhold,getFrontFaceDetection(),getBackFaceDetection(),detectedObjectHandle,minThreshold,detectedNormalVector,false,_checkOcclusions,handledByWorkThread);
	}
	_detectedObjectHandle=detectedObjectHandle;
	_detectedNormalVector=detectedNormalVector;
	if (handledByWorkThread)
		_calcTimeInMs=0;
	else
		_calcTimeInMs=VDateTime::getTimeDiffInMs(stTime);
	return(_detectedPointValid);
}

int CProxSensor::readSensor(C3Vector& detectPt,int& detectedObjectHandle,C3Vector& detectedNormalVector)
{
	if (_sensorResultValid)
	{
		if (_detectedPointValid)
		{
			detectPt=_detectedPoint;
			detectedObjectHandle=_detectedObjectHandle;
			detectedNormalVector=_detectedNormalVector;
			return(1);
		}
		return(0);
	}
	return(-1);
}

float CProxSensor::getCalculationTime()
{
	return(float(_calcTimeInMs)*0.001f);
}

bool CProxSensor::getFrontFaceDetection()
{
	return(frontFaceDetection);
}
bool CProxSensor::getBackFaceDetection()
{
	return(backFaceDetection);
}
void CProxSensor::setFrontFaceDetection(bool faceOn)
{
	frontFaceDetection=faceOn;
	if (!faceOn)
		backFaceDetection=true;
}
void CProxSensor::setBackFaceDetection(bool faceOn)
{
	backFaceDetection=faceOn;
	if (!faceOn)
		frontFaceDetection=true;
}

void CProxSensor::setAllowedNormal(float al)
{
	tt::limitValue(0.1f*degToRad,90.0f*degToRad,al);
	allowedNormal=al;
}
float CProxSensor::getAllowedNormal()
{
	return(allowedNormal);
}
void CProxSensor::setNormalCheck(bool check)
{
	normalCheck=check;
}
bool CProxSensor::getNormalCheck()
{
	return(normalCheck);
}
void CProxSensor::setClosestObjectMode(bool closestObjMode)
{
	closestObjectMode=closestObjMode;
	_detectedPointValid=false;
	_calcTimeInMs=0;
}
bool CProxSensor::getClosestObjectMode()
{
	return(closestObjectMode);
}

void CProxSensor::setSize(float newSize)
{
	tt::limitValue(0.0001f,10.0f,newSize);
	size=newSize;
}
float CProxSensor::getSize()
{
	return(size);
}

bool CProxSensor::getSensedData(C3Vector& pt)
{
	if (!_detectedPointValid)
		return(false);
	pt=_detectedPoint;
	return(true);
}

void CProxSensor::scaleObject(float scalingFactor)
{
	size*=scalingFactor;
	convexVolume->scaleVolume(scalingFactor);
	scaleObjectMain(scalingFactor);
}

C3Vector CProxSensor::getDetectedPoint()
{
	return(_detectedPoint);
}


void CProxSensor::setSensorType(int theType)
{
	sensorType=theType;
	if (theType==sim_proximitysensor_ray_subtype)
		convexVolume->setVolumeType(RAY_TYPE_CONVEX_VOLUME,_objectType,size);
	else
		_randomizedDetection=false;

	if (theType==sim_proximitysensor_cylinder_subtype)
		convexVolume->setVolumeType(CYLINDER_TYPE_CONVEX_VOLUME,_objectType,size);
	if (theType==sim_proximitysensor_disc_subtype)
		convexVolume->setVolumeType(DISC_TYPE_CONVEX_VOLUME,_objectType,size);
	if (theType==sim_proximitysensor_pyramid_subtype)
		convexVolume->setVolumeType(PYRAMID_TYPE_CONVEX_VOLUME,_objectType,size);
	if (theType==sim_proximitysensor_cone_subtype)
		convexVolume->setVolumeType(CONE_TYPE_CONVEX_VOLUME,_objectType,size);
}
int CProxSensor::getSensorType()
{
	return(sensorType);
}
