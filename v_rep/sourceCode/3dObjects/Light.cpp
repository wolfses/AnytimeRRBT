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
#include "Light.h"
#include "v_rep_internal.h"
#include "Tt.h"
#include "OGL.h"

#include "v_repStrings.h"
#include "IloIlo.h"
#include "ttUtil.h"
#include "easyLock.h"
#include "App.h"

CLight::CLight()
{
	_commonInit();
}

CLight::CLight(int theType)
{
	_lightType=theType;
	_commonInit();
}

std::string CLight::getObjectTypeInfo()
{
	return("Light");
}
std::string CLight::getObjectTypeInfoExtended()
{
	if (_lightType==sim_light_omnidirectional_subtype)
		return("Light (omnidirectional)");
	if (_lightType==sim_light_spot_subtype)
		return("Light (spot light)");
	if (_lightType==sim_light_directional_subtype)
		return("Light (directional)");
	return("ERROR");
}

void CLight::_commonInit()
{
	_initialValuesInitialized=false;
	setObjectType(sim_object_light_type);
	_lightSize=0.10f;
	_spotExponent=5;
	_spotCutoffAngle=90.0f*degToRad;
	layer=CAMERA_LIGHT_LAYER;
	_mainPropertyLocal=0;
	_setDefaultColors();	
	constantAttenuation=0.25f;
	linearAttenuation=0.0f;
	quadraticAttenuation=0.1f;
	lightActive=true;
	_lightIsLocal=false;

	if (_lightType==sim_light_omnidirectional_subtype)
		_objectName=IDSOGL_OMNIDIRECTIONAL_LIGHT;
	if (_lightType==sim_light_spot_subtype)
		_objectName=IDSOGL_SPOTLIGHT;
	if (_lightType==sim_light_directional_subtype)
		_objectName=IDSOGL_DIRECTIONAL_LIGHT;
}

bool CLight::getFullBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	if (_lightType==sim_light_omnidirectional_subtype)
	{
		minV(0)=-0.5f*_lightSize;
		maxV(0)=0.5f*_lightSize;
		minV(1)=-0.5f*_lightSize;
		maxV(1)=0.5f*_lightSize;
		minV(2)=-0.5f*_lightSize;
		maxV(2)=0.5f*_lightSize;
	}
	if (_lightType==sim_light_spot_subtype)
	{
		minV(0)=-0.8f*_lightSize;
		maxV(0)=0.8f*_lightSize;
		minV(1)=-0.8f*_lightSize;
		maxV(1)=0.8f*_lightSize;
		minV(2)=-1.5f*_lightSize;
		maxV(2)=0.5f*_lightSize;
	}
	if (_lightType==sim_light_directional_subtype)
	{
		minV(0)=-_lightSize*0.5f;
		maxV(0)=_lightSize*0.5f;
		minV(1)=-_lightSize*0.5f;
		maxV(1)=_lightSize*0.5f;
		minV(2)=-0.5f*_lightSize;
		maxV(2)=0.5f*_lightSize;
	}
	return(true);
}

bool CLight::getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	return(getFullBoundingBox(minV,maxV));
}

void CLight::_setDefaultColors()
{
	if (_lightType==sim_light_omnidirectional_subtype)
	{
		objectColor.setDefaultValues();
		lightColor.setDefaultValues();
		lightColor.setColor(0.33f,0.33f,0.33f,AMBIENT_MODE);
		lightColor.setColor(0.05f,0.05f,0.05f,AMBIENT_MODE);
		lightColor.setColor(0.5f,0.5f,0.5f,DIFFUSE_MODE);
		lightColor.setColor(0.5f,0.5f,0.5f,SPECULAR_MODE);
	}
	if (_lightType==sim_light_spot_subtype)
	{
		objectColor.setDefaultValues();
		objectColor.setColor(1.0f,0.375f,0.25f,AMBIENT_MODE);
		lightColor.setDefaultValues();
		lightColor.setColor(0.22f,0.22f,0.22f,AMBIENT_MODE);
		lightColor.setColor(0.5f,0.5f,0.5f,DIFFUSE_MODE);
		lightColor.setColor(0.5f,0.5f,0.5f,SPECULAR_MODE);
	}
	if (_lightType==sim_light_directional_subtype)
	{
		objectColor.setDefaultValues();
		objectColor.setColor(0.45f,0.45f,0.75f,AMBIENT_MODE);
		lightColor.setDefaultValues();
		lightColor.setColor(0.22f,0.22f,0.22f,AMBIENT_MODE);
		lightColor.setColor(0.5f,0.5f,0.5f,DIFFUSE_MODE);
		lightColor.setColor(0.5f,0.5f,0.5f,SPECULAR_MODE);
	}
	objectColor.setColor(0.25f,0.25f,0.25f,DIFFUSE_MODE);
	objectColor.setColor(0.25f,0.25f,0.25f,SPECULAR_MODE);
}

CLight::~CLight()
{
}

bool CLight::getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices)
{
	vertices.clear();
	indices.clear();
	return(false); // for now
}

void CLight::display(CViewableBase* renderingObject,int displayAttrib)
{
	FUNCTION_INSIDE_DEBUG("CLight::display");
	EASYLOCK(_objectMutex);
	// At the beginning of every 3DObject display routine:
	commonStart(displayAttrib);

	// Bounding box display:
	if (displayAttrib&sim_displayattribute_renderpass) 
		displayBoundingBox(displayAttrib);

	// Object display:
	if (getShouldObjectBeDisplayed(displayAttrib))
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
		if ( (displayAttrib&sim_displayattribute_forcewireframe)&&(displayAttrib&sim_displayattribute_renderpass) )
			glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);

		App::ct->objCont->enableAuxClippingPlanes(_objectID);
		C3Vector normalizedAmbientColor(lightColor.colors);
		float m=SIM_MAX(SIM_MAX(normalizedAmbientColor(0),normalizedAmbientColor(1)),normalizedAmbientColor(2));
		if (m>0.00001f)
			normalizedAmbientColor/=m;
		C3Vector normalizedDiffuseColor(lightColor.colors+3);
		m=SIM_MAX(SIM_MAX(normalizedDiffuseColor(0),normalizedDiffuseColor(1)),normalizedDiffuseColor(2));
		if (m>0.00001f)
			normalizedDiffuseColor/=m;
		C3Vector normalizedSpecularColor(lightColor.colors+6);
		m=SIM_MAX(SIM_MAX(normalizedSpecularColor(0),normalizedSpecularColor(1)),normalizedSpecularColor(2));
		if (m>0.00001f)
			normalizedSpecularColor/=m;
		normalizedAmbientColor+=normalizedDiffuseColor*0.2f+normalizedSpecularColor*0.1f;
		m=SIM_MAX(SIM_MAX(normalizedAmbientColor(0),normalizedAmbientColor(1)),normalizedAmbientColor(2));
		if (m>0.00001f)
			normalizedAmbientColor/=m;
		float black[3]={0.0f,0.0f,0.0f};
		float grey[3]={0.2f,0.2f,0.2f};
		if (_lightType==sim_light_omnidirectional_subtype)
		{	// Omnidirectional light
			if (getLightActive())
				ogl::setColor(black,black,black,normalizedAmbientColor.data,12);
			else
				ogl::setColor(grey,grey,grey,black,12);
			ogl::displaySphere(0.5f*_lightSize,20,10);
		}
		if (_lightType==sim_light_spot_subtype)
		{	//spotLight
			objectColor.makeCurrentColor();
			glPushMatrix();
				glTranslatef(0.0f,0.0f,-0.5f*_lightSize);
				ogl::displayCone(1.6f*_lightSize,2.0f*_lightSize,20,true);
				glTranslatef(0.0f,0.0f,0.5f*_lightSize);
				if (getLightActive())
					ogl::setColor(black,black,black,normalizedAmbientColor.data,12);
				else
					ogl::setColor(grey,grey,grey,black,12);
				ogl::displaySphere(0.5f*_lightSize,20,10);
			glPopMatrix();
		}
		if (_lightType==sim_light_directional_subtype)
		{	// Directional light
			glPushMatrix();
				glTranslatef(0.0f,0.0f,0.5f*_lightSize);
				if (getLightActive())
					ogl::setColor(black,black,black,normalizedAmbientColor.data,12);
				else
					ogl::setColor(grey,grey,grey,black,12);
				ogl::displayDisk(_lightSize,20);
				objectColor.makeCurrentColor();
				glTranslatef(0.0f,0.0f,-_lightSize/2.0f);
				ogl::displayCylinder(_lightSize,_lightSize,20,1);

			glPopMatrix();
		}
		App::ct->objCont->disableAuxClippingPlanes();
	}

	// At the end of every 3DObject display routine:
	commonFinish();
}

void CLight::scaleObject(float scalingFactor)
{
	_lightSize*=scalingFactor;
	linearAttenuation/=scalingFactor;
	quadraticAttenuation/=scalingFactor*scalingFactor;
	scaleObjectMain(scalingFactor);
}

void CLight::setLightSize(float size)
{
	tt::limitValue(0.001f,100.0f,size);
	_lightSize=size;
}

float CLight::getLightSize()
{
	return(_lightSize);
}

float CLight::getAttenuationFactor(short type)
{
	if (type==CONSTANT_ATTENUATION) return(constantAttenuation);
	if (type==LINEAR_ATTENUATION) return(linearAttenuation);
	if (type==QUADRATIC_ATTENUATION) return(quadraticAttenuation);
	return(0);
}

void CLight::setAttenuationFactor(float value,short type)
{
	tt::limitValue(0.0,1000.0,value);
	if (type==CONSTANT_ATTENUATION) constantAttenuation=value;
	if (type==LINEAR_ATTENUATION) linearAttenuation=value;
	if (type==QUADRATIC_ATTENUATION) quadraticAttenuation=value;
}

void CLight::setLightActive(bool active)
{
	if (active!=lightActive)
		App::ct->hierarchy->setRefreshViewFlag();
	lightActive=active;
}


bool CLight::getLightActive()
{
	return(lightActive);
}

void CLight::setSpotExponent(int e)
{
	_spotExponent=tt::getLimitedInt(0,128,e);
}

int CLight::getSpotExponent()
{
	return(_spotExponent);
}

void CLight::setSpotCutoffAngle(float co)
{
	_spotCutoffAngle=tt::getLimitedFloat(5.0f*degToRad,90.0f*degToRad,co);
}

float CLight::getSpotCutoffAngle()
{
	return(_spotCutoffAngle);
}


int  CLight::getLightType()
{
	return(_lightType);
}

C3DObject* CLight::copyYourself()
{	
	CLight* newLight=(CLight*)copyYourselfMain();

	// Various
	newLight->_lightSize=_lightSize;
	newLight->_lightType=_lightType;
	newLight->_spotExponent=_spotExponent;
	newLight->_spotCutoffAngle=_spotCutoffAngle;
	newLight->lightActive=lightActive;
	newLight->constantAttenuation=constantAttenuation;
	newLight->linearAttenuation=linearAttenuation;
	newLight->quadraticAttenuation=quadraticAttenuation;
	newLight->_lightIsLocal=_lightIsLocal;

	// Colors:
	objectColor.copyYourselfInto(&newLight->objectColor);
	lightColor.copyYourselfInto(&newLight->lightColor);

	return(newLight);
}

bool CLight::announceObjectWillBeErased(int objID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	// This routine can be called for objCont-objects, but also for objects
	// in the copy-buffer!! So never make use of any 
	// 'ct::objCont->getObject(id)'-call or similar
	// Return value true means 'this' has to be erased too!
	announceObjectWillBeErasedMain(objID,copyBuffer);
	return(false);
}

void CLight::announceGroupWillBeErased(int groupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGroupWillBeErasedMain(groupID,copyBuffer);
}
void CLight::announceCollisionWillBeErased(int collisionID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceCollisionWillBeErasedMain(collisionID,copyBuffer);
}
void CLight::announceDistanceWillBeErased(int distanceID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceDistanceWillBeErasedMain(distanceID,copyBuffer);
}
void CLight::announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceIkObjectWillBeErasedMain(ikGroupID,copyBuffer);
}
void CLight::announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGcsObjectWillBeErasedMain(gcsObjectID,copyBuffer);
}
void CLight::performObjectLoadingMapping(std::vector<int>* map)
{ // New_Object_ID=map[Old_Object_ID]
	performObjectLoadingMappingMain(map);
}
void CLight::performGroupLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performGroupLoadingMappingMain(map);
}
void CLight::performCollisionLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performCollisionLoadingMappingMain(map);
}
void CLight::performDistanceLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performDistanceLoadingMappingMain(map);
}
void CLight::performIkLoadingMapping(std::vector<int>* map)
{
	performIkLoadingMappingMain(map);
}
void CLight::performGcsLoadingMapping(std::vector<int>* map)
{
	performGcsLoadingMappingMain(map);
}

void CLight::performTextureObjectLoadingMapping(std::vector<int>* map)
{
	performTextureObjectLoadingMappingMain(map);
}

void CLight::performDynMaterialObjectLoadingMapping(std::vector<int>* map)
{
	performDynMaterialObjectLoadingMappingMain(map);
}

void CLight::bufferMainDisplayStateVariables()
{
	bufferMainDisplayStateVariablesMain();
}

void CLight::bufferedMainDisplayStateVariablesToDisplay()
{
	bufferedMainDisplayStateVariablesToDisplayMain();
}

void CLight::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	initializeInitialValuesMain(simulationIsRunning);
	_initialValuesInitialized=simulationIsRunning;
	if (simulationIsRunning)
	{
		_initialLightActive=lightActive;
	}
}

void CLight::simulationAboutToStart()
{
	initializeInitialValues(true);
}

void CLight::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd()&&((getCumulativeMainPropertyOverride()&sim_modelproperty_not_reset)==0))
	{
		lightActive=_initialLightActive;
	}
	_initialValuesInitialized=false;
	simulationEndedMain();
}

void  CLight::setLightIsLocal(bool l)
{
	_lightIsLocal=l;
}

bool  CLight::getLightIsLocal()
{
	return(_lightIsLocal);
}

void CLight::serialize(CSer& ar)
{
	serializeMain(ar);
	if (ar.isStoring())
	{ // Storing
		
		ar.storeDataName("Cp2");
		ar << _lightType << _spotExponent << _lightSize;
		ar.flush();

		ar.storeDataName("Cp3");
		ar << _spotCutoffAngle;
		ar.flush();

		ar.storeDataName("Caf");
		ar << constantAttenuation << linearAttenuation << quadraticAttenuation;
		ar.flush();

		ar.storeDataName("Cas");
		BYTE nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,lightActive);
		SIM_SET_CLEAR_BIT(nothing,1,_lightIsLocal);
		ar << nothing;
		ar.flush();

		ar.storeDataName("Cl1");
		ar.setCountingMode();
		objectColor.serialize(ar);
		if (ar.setWritingMode())
			objectColor.serialize(ar);

		ar.storeDataName("Cl2");
		ar.setCountingMode();
		lightColor.serialize(ar);
		if (ar.setWritingMode())
			lightColor.serialize(ar);

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
				if (theName.compare("Cp2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _lightType >> _spotExponent >> _lightSize;
				}
				if (theName.compare("Cp3")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _spotCutoffAngle;
				}
				if (theName.compare("Caf")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> constantAttenuation >> linearAttenuation >> quadraticAttenuation;
				}
				if (theName=="Cas")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					lightActive=SIM_IS_BIT_SET(nothing,0);
					_lightIsLocal=SIM_IS_BIT_SET(nothing,1);
				}
				if (theName.compare("Cl1")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					objectColor.serialize(ar);
				}
				if (theName.compare("Cl2")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					lightColor.serialize(ar);
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
		if ((ar.getSerializationVersionThatWroteThisFile()<17)&&App::userSettings->correctColorsAndLights)
		{ // on 29/08/2013 we corrected all default lights. So we need to correct for that change:
			CTTUtil::scaleColorUp_(objectColor.colors);
			CTTUtil::scaleLightDown_(lightColor.colors);
		}
	}
}

void CLight::serializeWExtIk(CExtIkSer& ar)
{
	serializeWExtIkMain(ar);
	CDummy::serializeWExtIkStatic(ar);
}
