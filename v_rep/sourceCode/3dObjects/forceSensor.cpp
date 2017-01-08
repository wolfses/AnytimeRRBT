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
#include "forceSensor.h"
#include "v_rep_internal.h"
#include "OGL.h"
#include "Tt.h"

#include "v_repStrings.h"
#include <algorithm>
#include "IloIlo.h"
#include "ttUtil.h"
#include "easyLock.h"
#include "App.h"

CForceSensor::CForceSensor()
{
	commonInit();
}

void CForceSensor::commonInit()
{
	_objectType=sim_object_forcesensor_type;
	_initialValuesInitialized=false;
	_forceSensorIsBroken=false;

	_forceThreshold=100.0f;
	_torqueThreshold=10.0f;
	_forceThresholdEnabled=false;
	_torqueThresholdEnabled=false;
	_consecutiveThresholdViolationsForBreaking=10;
	_currentThresholdViolationCount=0;

	// Dynamic values:
	_dynamicSecondPartIsValid=false;
	_dynamicSecondPartLocalTransform.setIdentity();
	_filteredDynamicForces.clear();
	_filteredDynamicTorques.clear();

	_cumulativeForcesTmp.clear();
	_cumulativeTorquesTmp.clear();

	_forceSensorSize=0.05f;

	_valueCountForFilter=1;
	_filterType=0; // average
	_filteredValuesAreValid=false;

	colorPart1.setDefaultValues();
	colorPart1.setColor(0.15f,0.6f,0.3f,AMBIENT_MODE);
	colorPart2.setDefaultValues();
	colorPart2.setColor(0.15f,0.15f,0.15f,AMBIENT_MODE);
	layer=FORCE_SENSOR_LAYER;
	_mainPropertyLocal=0;
	_objectName=IDSOGL_FORCE_SENSOR;
}

CForceSensor::~CForceSensor()
{

}

void CForceSensor::setValueCountForFilter(int c)
{
	_valueCountForFilter=tt::getLimitedInt(1,100,c);
}

int CForceSensor::getValueCountForFilter()
{
	return(_valueCountForFilter);
}

void CForceSensor::setFilterType(int t)
{
	_filterType=t;
}

int CForceSensor::getFilterType()
{
	return(_filterType);
}
void CForceSensor::setForceThreshold(float t)
{
	_forceThreshold=tt::getLimitedFloat(0.0f,10000000000.0f,t);
}
float CForceSensor::getForceThreshold()
{
	return(_forceThreshold);
}
void CForceSensor::setTorqueThreshold(float t)
{
	_torqueThreshold=tt::getLimitedFloat(0.0f,10000000000.0f,t);
}
float CForceSensor::getTorqueThreshold()
{
	return(_torqueThreshold);
}
void CForceSensor::setEnableForceThreshold(bool e)
{
	_forceThresholdEnabled=e;
}
bool CForceSensor::getEnableForceThreshold()
{
	return(_forceThresholdEnabled);
}
void CForceSensor::setEnableTorqueThreshold(bool e)
{
	_torqueThresholdEnabled=e;
}
bool CForceSensor::getEnableTorqueThreshold()
{
	return(_torqueThresholdEnabled);
}
void CForceSensor::setConsecutiveThresholdViolationsForBreaking(int count)
{
	_consecutiveThresholdViolationsForBreaking=tt::getLimitedInt(1,10000,count);
}
int CForceSensor::getConsecutiveThresholdViolationsForBreaking()
{
	return(_consecutiveThresholdViolationsForBreaking);
}

void CForceSensor::setDynamicSecondPartIsValid(bool v)
{
	_dynamicSecondPartIsValid=v;
}

void CForceSensor::setDynamicSecondPartLocalTransform(const C7Vector& tr)
{
	_dynamicSecondPartLocalTransform=tr;
}

C7Vector CForceSensor::getDynamicSecondPartLocalTransform()
{
	return(_dynamicSecondPartLocalTransform);
}

void CForceSensor::addCumulativeForcesAndTorques(const C3Vector& f,const C3Vector& t,int countForAverage)
{ // the countForAverage mechanism is needed because we need to average all values in a simulation time step (but this is called every dynamic simulation time step!!)
	_cumulativeForcesTmp+=f;
	_cumulativeTorquesTmp+=t;
	if (countForAverage>0)
	{
		_cumulatedForces.push_back(_cumulativeForcesTmp/float(countForAverage));
		_cumulatedTorques.push_back(_cumulativeTorquesTmp/float(countForAverage));
		_cumulativeForcesTmp.clear();
		_cumulativeTorquesTmp.clear();
		if (int(_cumulatedForces.size())>_valueCountForFilter)
		{
			_cumulatedForces.erase(_cumulatedForces.begin(),_cumulatedForces.begin()+1);
			_cumulatedTorques.erase(_cumulatedTorques.begin(),_cumulatedTorques.begin()+1);
		}
		_computeFilteredValues();
		_handleSensorBreaking();
	}

}

void CForceSensor::_computeFilteredValues()
{
	if (int(_cumulatedForces.size())>=_valueCountForFilter)
	{
		_filteredValuesAreValid=true;
		if (_filterType==0)
		{ // average filter
			C3Vector fo;
			C3Vector to;
			fo.clear();
			to.clear();
			for (int i=0;i<_valueCountForFilter;i++)
			{
				fo+=_cumulatedForces[i];
				to+=_cumulatedTorques[i];
			}
			_filteredDynamicForces=fo/float(_valueCountForFilter);
			_filteredDynamicTorques=to/float(_valueCountForFilter);
		}
		if (_filterType==1)
		{
			std::vector<float> fx;
			std::vector<float> fy;
			std::vector<float> fz;
			std::vector<float> tx;
			std::vector<float> ty;
			std::vector<float> tz;
			for (int i=0;i<_valueCountForFilter;i++)
			{
				fx.push_back(_cumulatedForces[i](0));
				fy.push_back(_cumulatedForces[i](1));
				fz.push_back(_cumulatedForces[i](2));
				tx.push_back(_cumulatedTorques[i](0));
				ty.push_back(_cumulatedTorques[i](1));
				tz.push_back(_cumulatedTorques[i](2));
			}
			std::sort(fx.begin(),fx.end());
			std::sort(fy.begin(),fy.end());
			std::sort(fz.begin(),fz.end());
			std::sort(tx.begin(),tx.end());
			std::sort(ty.begin(),ty.end());
			std::sort(tz.begin(),tz.end());
			int ind=_valueCountForFilter/2;
			_filteredDynamicForces(0)=fx[ind];
			_filteredDynamicForces(1)=fy[ind];
			_filteredDynamicForces(2)=fz[ind];
			_filteredDynamicTorques(0)=tx[ind];
			_filteredDynamicTorques(1)=ty[ind];
			_filteredDynamicTorques(2)=tz[ind];
		}
	}
	else
		_filteredValuesAreValid=false;
}

bool CForceSensor::getDynamicForces(C3Vector& f)
{
	if ( (!_filteredValuesAreValid)||(!_dynamicSecondPartIsValid) )
		return(false);
	f=_filteredDynamicForces;
	return(true);
}

bool CForceSensor::getDynamicTorques(C3Vector& t)
{
	if ( (!_filteredValuesAreValid)||(!_dynamicSecondPartIsValid) )
		return(false);
	t=_filteredDynamicTorques;
	return(true);
}

void CForceSensor::_handleSensorBreaking()
{
	if ( (!_filteredValuesAreValid)||(!_dynamicSecondPartIsValid) )
		return;
	bool trigger=false;
	if (_forceThresholdEnabled&&(_filteredDynamicForces.getLength()>=_forceThreshold))
		trigger=true;
	if (_torqueThresholdEnabled&&(_filteredDynamicTorques.getLength()>=_torqueThreshold))
		trigger=true;
	if (trigger)
		_currentThresholdViolationCount++;
	else
		_currentThresholdViolationCount=0;
	if (_currentThresholdViolationCount>=_consecutiveThresholdViolationsForBreaking)
	{ // we need to break something!
		setForceSensorIsBroken();
		_currentThresholdViolationCount=0;
	}
}

void CForceSensor::setForceSensorIsBroken()
{
	_forceSensorIsBroken=true;
	App::ct->hierarchy->setRefreshViewFlag();
}
bool CForceSensor::getForceSensorIsBroken()
{
	return(_forceSensorIsBroken);
}

float CForceSensor::getPositionError()
{
	float retVal=0.0f;
	if (_dynamicSecondPartIsValid)
		retVal=_dynamicSecondPartLocalTransform.X.getLength();
	return(retVal);
}

float CForceSensor::getOrientationError()
{
	float retVal=0.0f;
	if (_dynamicSecondPartIsValid)
	{
		C4Vector idQuat;
		idQuat.setIdentity();
		retVal=_dynamicSecondPartLocalTransform.Q.getAngleBetweenQuaternions(idQuat);
	}
	return(retVal);
}

void CForceSensor::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	initializeInitialValuesMain(simulationIsRunning);
	_initialValuesInitialized=simulationIsRunning;
	if (simulationIsRunning)
	{
		_filteredDynamicForces.clear();
		_filteredDynamicTorques.clear();
		_currentThresholdViolationCount=0;
		_filteredValuesAreValid=false;
		_cumulatedForces.clear();
		_cumulatedTorques.clear();
		_cumulativeForcesTmp.clear();
		_cumulativeTorquesTmp.clear();
	}
	else
	{
		_dynamicSecondPartIsValid=false;
		_forceSensorIsBroken=false;
	}
}

void CForceSensor::simulationAboutToStart()
{
	initializeInitialValues(true);
	_dynamicSecondPartIsValid=false;
	_forceSensorIsBroken=false;
}

void CForceSensor::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd()&&((getCumulativeMainPropertyOverride()&sim_modelproperty_not_reset)==0))
	{
	}
	_dynamicSecondPartIsValid=false;
	_forceSensorIsBroken=false;
	_initialValuesInitialized=false;
	_filteredValuesAreValid=false;
	_cumulatedForces.clear();
	_cumulatedTorques.clear();
	_cumulativeForcesTmp.clear();
	_cumulativeTorquesTmp.clear();
	simulationEndedMain();
}

std::string CForceSensor::getObjectTypeInfo()
{
	return(IDSOGL_FORCE_SENSOR);
}
std::string CForceSensor::getObjectTypeInfoExtended()
{
	if (_forceSensorIsBroken)
		return(IDSOGL_FORCE_SENSOR_BROKEN);
	return(IDSOGL_FORCE_SENSOR);
}

bool CForceSensor::getFullBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	maxV(0)=maxV(1)=maxV(2)=_forceSensorSize*0.5f;
	minV(0)=minV(1)=minV(2)=-_forceSensorSize*0.5f;
	return(true);
}

bool CForceSensor::getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	return(getFullBoundingBox(minV,maxV));
}

bool CForceSensor::getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices)
{
	vertices.clear();
	indices.clear();
	return(false); // for now
}

void CForceSensor::display(CViewableBase* renderingObject,int displayAttrib)
{
	FUNCTION_INSIDE_DEBUG("CForceSensor::display");
	EASYLOCK(_objectMutex);
	// At the beginning of every 3DObject display routine:
	commonStart(displayAttrib);

	// Bounding box display:
	if (displayAttrib&sim_displayattribute_renderpass) 
		displayBoundingBox(displayAttrib,true,_forceSensorSize);
	
	// Object display:
	if (getShouldObjectBeDisplayed(displayAttrib))
	{
		App::ct->objCont->enableAuxClippingPlanes(_objectID);
		if (displayAttrib&sim_displayattribute_dynamiccontentonly)
		{
			const float col1[12]={0.0f,0.4f,0.0f,0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f};
			ogl::setColor(col1,48,0);
		}
		else
			colorPart1.makeCurrentColor();
		displayForceSensor(displayAttrib,true,_forceSensorSize);
		if (displayAttrib&sim_displayattribute_dynamiccontentonly)
		{
			const float col2[12]={0.0f,0.0f,0.0f,0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f};
			ogl::setColor(col2,48,0);
		}
		else
			colorPart2.makeCurrentColor();
		displayForceSensor(displayAttrib,false,_forceSensorSize);
		App::ct->objCont->disableAuxClippingPlanes();
	}

	// At the end of every 3DObject display routine:
	commonFinish();
}
	
void CForceSensor::displayForceSensor(int displayAttrib,bool partOne,float sizeParam)
{
	if ((App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
	{
		if (_objectProperty&sim_objectproperty_selectmodelbaseinstead)
			glLoadName(getModelSelectionID());
		else
			glLoadName(getID());
	}
	else
		glLoadName(-1);

	glPushAttrib(GL_POLYGON_BIT);
	if ((displayAttrib&sim_displayattribute_forcewireframe)&&(displayAttrib&sim_displayattribute_renderpass))
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	glPushMatrix();
	if (partOne)
		ogl::displayCylinder(_forceSensorSize,_forceSensorSize*0.5f,16);
	else
	{
		if (_dynamicSecondPartIsValid&&(!App::ct->simulation->isSimulationStopped()))
		{ // for dynamic mode
			glTranslatef(_dynamicSecondPartLocalTransform.X(0),_dynamicSecondPartLocalTransform.X(1),_dynamicSecondPartLocalTransform.X(2));
			C4Vector axis=_dynamicSecondPartLocalTransform.Q.getAngleAndAxisNoChecking();
			glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));
		}

		ogl::displayCylinder(_forceSensorSize/5.0f,_forceSensorSize,16);
		if (displayAttrib&sim_displayattribute_selected)
			drawReference(sizeParam);
	}
	glPopAttrib();
	glPopMatrix();
}

void CForceSensor::setSize(float s)
{
	tt::limitValue(0.001f,10.0f,s);
	_forceSensorSize=s;
}

float CForceSensor::getSize()
{
	return(_forceSensorSize);
}

void CForceSensor::scaleObject(float scalingFactor)
{
	_forceSensorSize*=scalingFactor;
	_forceThreshold*=scalingFactor*scalingFactor*scalingFactor;//*scalingFactor; removed one on 2010/02/17 b/c often working against gravity which doesn't change
	_torqueThreshold*=scalingFactor*scalingFactor*scalingFactor*scalingFactor;//*scalingFactor; removed one on 2010/02/17 b/c often working against gravity which doesn't change

	scaleObjectMain(scalingFactor);
	// We have to reconstruct a part of the dynamics world:
	_dynamicsFullRefreshFlag=true; // yes, because we might have a position scaling too!
}

C3DObject* CForceSensor::copyYourself()
{	
	CForceSensor* newForceSensor=(CForceSensor*)copyYourselfMain();

	newForceSensor->_forceSensorSize=_forceSensorSize;
	newForceSensor->_forceSensorIsBroken=_forceSensorIsBroken;
	newForceSensor->_dynamicSecondPartIsValid=_dynamicSecondPartIsValid;

	newForceSensor->_forceThreshold=_forceThreshold;
	newForceSensor->_torqueThreshold=_torqueThreshold;
	newForceSensor->_forceThresholdEnabled=_forceThresholdEnabled;
	newForceSensor->_torqueThresholdEnabled=_torqueThresholdEnabled;
	newForceSensor->_consecutiveThresholdViolationsForBreaking=_consecutiveThresholdViolationsForBreaking;
	newForceSensor->_currentThresholdViolationCount=_currentThresholdViolationCount;

	newForceSensor->_valueCountForFilter=_valueCountForFilter;
	newForceSensor->_filterType=_filterType;

	colorPart1.copyYourselfInto(&newForceSensor->colorPart1);
	colorPart2.copyYourselfInto(&newForceSensor->colorPart2);

	newForceSensor->_dynamicSecondPartLocalTransform=_dynamicSecondPartLocalTransform; // needed when copying a broken sensor!

	return(newForceSensor);
}

bool CForceSensor::announceObjectWillBeErased(int objID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	// This routine can be called for objCont-objects, but also for objects
	// in the copy-buffer!! So never make use of any 
	// 'ct::objCont->getObject(id)'-call or similar
	// Return value true means 'this' has to be erased too!
	announceObjectWillBeErasedMain(objID,copyBuffer);
	return(false);
}

void CForceSensor::announceGroupWillBeErased(int groupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGroupWillBeErasedMain(groupID,copyBuffer);
}
void CForceSensor::announceCollisionWillBeErased(int collisionID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceCollisionWillBeErasedMain(collisionID,copyBuffer);
}
void CForceSensor::announceDistanceWillBeErased(int distanceID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceDistanceWillBeErasedMain(distanceID,copyBuffer);
}
void CForceSensor::announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceIkObjectWillBeErasedMain(ikGroupID,copyBuffer);
}
void CForceSensor::announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGcsObjectWillBeErasedMain(gcsObjectID,copyBuffer);
}
void CForceSensor::performObjectLoadingMapping(std::vector<int>* map)
{ // New_Object_ID=map[Old_Object_ID]
	performObjectLoadingMappingMain(map);
}
void CForceSensor::performGroupLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performGroupLoadingMappingMain(map);
}
void CForceSensor::performCollisionLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performCollisionLoadingMappingMain(map);
}
void CForceSensor::performDistanceLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performDistanceLoadingMappingMain(map);
}
void CForceSensor::performIkLoadingMapping(std::vector<int>* map)
{
	performIkLoadingMappingMain(map);
}
void CForceSensor::performGcsLoadingMapping(std::vector<int>* map)
{
	performGcsLoadingMappingMain(map);
}

void CForceSensor::performTextureObjectLoadingMapping(std::vector<int>* map)
{
	performTextureObjectLoadingMappingMain(map);
}

void CForceSensor::performDynMaterialObjectLoadingMapping(std::vector<int>* map)
{
	performDynMaterialObjectLoadingMappingMain(map);
}

void CForceSensor::bufferMainDisplayStateVariables()
{
	bufferMainDisplayStateVariablesMain();
}

void CForceSensor::bufferedMainDisplayStateVariablesToDisplay()
{
	bufferedMainDisplayStateVariablesToDisplayMain();
}

void CForceSensor::serialize(CSer& ar)
{
	serializeMain(ar);
	if (ar.isStoring())
	{	// Storing.
		ar.storeDataName("Siz");
		ar << _forceSensorSize;
		ar.flush();

		ar.storeDataName("Vab");
		BYTE dummy=0;
		SIM_SET_CLEAR_BIT(dummy,0,_forceThresholdEnabled);
		SIM_SET_CLEAR_BIT(dummy,1,_torqueThresholdEnabled);
//		SIM_SET_CLEAR_BIT(dummy,2,_breaksPositionConstraints); // removed on 2010/02/06
//		SIM_SET_CLEAR_BIT(dummy,3,_breaksOrientationConstraints); // removed on 2010/02/06
		ar << dummy;
		ar.flush();

		ar.storeDataName("Tre");
		ar << _forceThreshold << _torqueThreshold << _consecutiveThresholdViolationsForBreaking;
		ar.flush();

		ar.storeDataName("Fil");
		ar << _valueCountForFilter << _filterType;
		ar.flush();

// "Bus" is reserved keyword since 2010/10/09		ar.storeDataName("Bus");

		ar.storeDataName("Cl1");
		ar.setCountingMode();
		colorPart1.serialize(ar);
		if (ar.setWritingMode())
			colorPart1.serialize(ar);

		ar.storeDataName("Cl2");
		ar.setCountingMode();
		colorPart2.serialize(ar);
		if (ar.setWritingMode())
			colorPart2.serialize(ar);

		ar.storeDataName(SER_END_OF_OBJECT);
	}
	else
	{		// Loading
		int byteQuantity;
		std::string theName="";
		while (theName.compare(SER_END_OF_OBJECT)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_END_OF_OBJECT)!=0)
			{
				bool noHit=true;
				if (theName.compare("Siz")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _forceSensorSize;
				}
				if (theName.compare("Cl1")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					colorPart1.serialize(ar);
				}
				if (theName.compare("Cl2")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					colorPart2.serialize(ar);
				}
				if (theName.compare("Vab")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
					_forceThresholdEnabled=SIM_IS_BIT_SET(dummy,0);
					_torqueThresholdEnabled=SIM_IS_BIT_SET(dummy,1);
				}
				if (theName.compare("Tre")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _forceThreshold >> _torqueThreshold >> _consecutiveThresholdViolationsForBreaking;
				}
				if (theName.compare("Fil")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _valueCountForFilter >> _filterType;
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
		if ((ar.getSerializationVersionThatWroteThisFile()<17)&&App::userSettings->correctColorsAndLights)
		{ // on 29/08/2013 we corrected all default lights. So we need to correct for that change:
			CTTUtil::scaleColorUp_(colorPart1.colors);
			CTTUtil::scaleColorUp_(colorPart2.colors);
		}
	}
}

void CForceSensor::serializeWExtIk(CExtIkSer& ar)
{
	serializeWExtIkMain(ar);
	CDummy::serializeWExtIkStatic(ar);
}
