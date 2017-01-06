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
#include "Mirror.h"
#include "v_rep_internal.h"
#include "Tt.h"
#include "OGL.h"

#include "v_repStrings.h"
#include "IloIlo.h"
#include "ttUtil.h"
#include "easyLock.h"
#include "App.h"

int CMirror::currentMirrorContentBeingRendered=-1;

CMirror::CMirror()
{
	_commonInit();
}

std::string CMirror::getObjectTypeInfo()
{
	return("Mirror");
}
std::string CMirror::getObjectTypeInfoExtended()
{
	return("Mirror");
}

void CMirror::_commonInit()
{
	_initialValuesInitialized=false;
	setObjectType(sim_object_mirror_type);
	_mirrorWidth=0.5f;
	_mirrorHeight=1.0f;
	_mirrorReflectance=0.75f;
	mirrorColor[0]=0.0f;
	mirrorColor[1]=0.0f;
	mirrorColor[2]=0.0f;

	clipPlaneColor.setDefaultValues();
	clipPlaneColor.setColor(0.0f,0.525f,0.6f,AMBIENT_MODE);
	clipPlaneColor.translucid=true;
	clipPlaneColor.transparencyFactor=0.4f;

	_active=true;
	_isMirror=true;
	_clippingObjectOrCollection=-1; // clipping all
	layer=CAMERA_LIGHT_LAYER;
	_mainPropertyLocal=sim_objectspecialproperty_renderable;

	_objectName=IDSOGL_MIRROR;
}

bool CMirror::getContainsTransparentComponent()
{
	if (_isMirror)
		return(false);
	return(clipPlaneColor.translucid);
}

bool CMirror::getFullBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	minV(0)=-0.5f*_mirrorWidth;
	maxV(0)=0.5f*_mirrorWidth;
	minV(1)=-0.5f*_mirrorHeight;
	maxV(1)=0.5f*_mirrorHeight;
	minV(2)=0.0f;
	maxV(2)=0.0f;
	return(true);
}

bool CMirror::getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	return(getFullBoundingBox(minV,maxV));
}

CMirror::~CMirror()
{
}

bool CMirror::getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices)
{
	vertices.clear();
	indices.clear();
	return(false); // for now
}

void CMirror::display(CViewableBase* renderingObject,int displayAttrib)
{
	FUNCTION_INSIDE_DEBUG("CMirror::display");
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
		if (((displayAttrib&sim_displayattribute_pickpass)||((currentMirrorContentBeingRendered!=_objectID)&&(currentMirrorContentBeingRendered!=-1)))&&_isMirror)
		{
			C3Vector v0(+_mirrorWidth*0.5f,-_mirrorHeight*0.5f,0.0f);
			C3Vector v1(+_mirrorWidth*0.5f,+_mirrorHeight*0.5f,0.0f);
			C3Vector v2(-_mirrorWidth*0.5f,+_mirrorHeight*0.5f,0.0f);
			C3Vector v3(-_mirrorWidth*0.5f,-_mirrorHeight*0.5f,0.0f);

			glDisable(GL_LIGHTING);
			glColor4f(mirrorColor[0],mirrorColor[1],mirrorColor[2],1.0f);
			glBegin (GL_QUADS);
			glVertex3fv(v0.data);
			glVertex3fv(v1.data);
			glVertex3fv(v2.data);
			glVertex3fv(v3.data);
			glEnd ();
			glEnable(GL_LIGHTING);
		}
		App::ct->objCont->disableAuxClippingPlanes();
		// For this part we want the clipping planes disabled:
		if (!_isMirror)
		{
			clipPlaneColor.makeCurrentColor();
			C3Vector v0(+_mirrorWidth*0.5f,-_mirrorHeight*0.5f,0.0f);
			C3Vector v1(+_mirrorWidth*0.5f,+_mirrorHeight*0.5f,0.0f);
			C3Vector v2(-_mirrorWidth*0.5f,+_mirrorHeight*0.5f,0.0f);
			C3Vector v3(-_mirrorWidth*0.5f,-_mirrorHeight*0.5f,0.0f);
			glBegin (GL_QUADS);
			glVertex3fv(v0.data);
			glVertex3fv(v1.data);
			glVertex3fv(v2.data);
			glVertex3fv(v3.data);
			glEnd ();
		}
	}

	// At the end of every 3DObject display routine:
	commonFinish();
}

void CMirror::scaleObject(float scalingFactor)
{
	_mirrorWidth*=scalingFactor;
	_mirrorHeight*=scalingFactor;
	scaleObjectMain(scalingFactor);
}

void CMirror::setMirrorWidth(float w)
{
	_mirrorWidth=tt::getLimitedFloat(0.001f,100.0f,w);
}

float CMirror::getMirrorWidth()
{
	return(_mirrorWidth);
}

void CMirror::setMirrorHeight(float h)
{
	_mirrorHeight=tt::getLimitedFloat(0.001f,100.0f,h);
}

float CMirror::getMirrorHeight()
{
	return(_mirrorHeight);
}

void CMirror::setReflectance(float r)
{
	_mirrorReflectance=tt::getLimitedFloat(0.0f,1.0f,r);
}

float CMirror::getReflectance()
{
	return(_mirrorReflectance);
}

void CMirror::setActive(bool a)
{
	_active=a;
}

bool CMirror::getActive()
{
	return(_active);
}

void CMirror::setIsMirror(bool m)
{
	if (m!=_isMirror)
		App::ct->hierarchy->setRefreshViewFlag();
	_isMirror=m;
}

bool CMirror::getIsMirror()
{
	return(_isMirror);
}

void CMirror::setClippingObjectOrCollection(int co)
{
	_clippingObjectOrCollection=co;
}

int CMirror::getClippingObjectOrCollection()
{
	return(_clippingObjectOrCollection);
}

C3DObject* CMirror::copyYourself()
{	
	CMirror* newMirror=(CMirror*)copyYourselfMain();

	// Various
	newMirror->_mirrorHeight=_mirrorHeight;
	newMirror->_mirrorWidth=_mirrorWidth;
	newMirror->_active=_active;
	newMirror->_isMirror=_isMirror;
	newMirror->_mirrorReflectance=_mirrorReflectance;
	newMirror->_clippingObjectOrCollection=_clippingObjectOrCollection;

	newMirror->mirrorColor[0]=mirrorColor[0];
	newMirror->mirrorColor[1]=mirrorColor[1];
	newMirror->mirrorColor[2]=mirrorColor[2];

	clipPlaneColor.copyYourselfInto(&newMirror->clipPlaneColor);

	newMirror->_initialMirrorActive=_initialMirrorActive;

	return(newMirror);
}

bool CMirror::announceObjectWillBeErased(int objID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	// This routine can be called for objCont-objects, but also for objects
	// in the copy-buffer!! So never make use of any 
	// 'ct::objCont->getObject(id)'-call or similar
	// Return value true means 'this' has to be erased too!
	if (_clippingObjectOrCollection==objID)
	{
		_clippingObjectOrCollection=-1;
		if (!_isMirror)
			_active=false;
	}
	announceObjectWillBeErasedMain(objID,copyBuffer);
	return(false);
}

void CMirror::announceGroupWillBeErased(int groupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	if (_clippingObjectOrCollection==groupID)
	{
		_clippingObjectOrCollection=-1;
		if (!_isMirror)
			_active=false;
	}
	announceGroupWillBeErasedMain(groupID,copyBuffer);
}
void CMirror::announceCollisionWillBeErased(int collisionID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceCollisionWillBeErasedMain(collisionID,copyBuffer);
}
void CMirror::announceDistanceWillBeErased(int distanceID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceDistanceWillBeErasedMain(distanceID,copyBuffer);
}
void CMirror::announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceIkObjectWillBeErasedMain(ikGroupID,copyBuffer);
}
void CMirror::announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGcsObjectWillBeErasedMain(gcsObjectID,copyBuffer);
}
void CMirror::performObjectLoadingMapping(std::vector<int>* map)
{ // New_Object_ID=map[Old_Object_ID]
	performObjectLoadingMappingMain(map);
	if (_clippingObjectOrCollection<SIM_IDSTART_COLLECTION)
		_clippingObjectOrCollection=App::ct->objCont->getLoadingMapping(map,_clippingObjectOrCollection);
}
void CMirror::performGroupLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performGroupLoadingMappingMain(map);
	if (_clippingObjectOrCollection>=SIM_IDSTART_COLLECTION)
		_clippingObjectOrCollection=App::ct->objCont->getLoadingMapping(map,_clippingObjectOrCollection);
}
void CMirror::performCollisionLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performCollisionLoadingMappingMain(map);
}
void CMirror::performDistanceLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performDistanceLoadingMappingMain(map);
}
void CMirror::performIkLoadingMapping(std::vector<int>* map)
{
	performIkLoadingMappingMain(map);
}
void CMirror::performGcsLoadingMapping(std::vector<int>* map)
{
	performGcsLoadingMappingMain(map);
}

void CMirror::performTextureObjectLoadingMapping(std::vector<int>* map)
{
	performTextureObjectLoadingMappingMain(map);
}

void CMirror::performDynMaterialObjectLoadingMapping(std::vector<int>* map)
{
	performDynMaterialObjectLoadingMappingMain(map);
}

void CMirror::bufferMainDisplayStateVariables()
{
	bufferMainDisplayStateVariablesMain();
}

void CMirror::bufferedMainDisplayStateVariablesToDisplay()
{
	bufferedMainDisplayStateVariablesToDisplayMain();
}

void CMirror::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	initializeInitialValuesMain(simulationIsRunning);
	_initialValuesInitialized=simulationIsRunning;
	if (simulationIsRunning)
	{
		_initialMirrorActive=_active;
	}
}


void CMirror::simulationAboutToStart()
{
	initializeInitialValues(true);
}

void CMirror::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd()&&((getCumulativeMainPropertyOverride()&sim_modelproperty_not_reset)==0))
	{
		_active=_initialMirrorActive;
	}
	_initialValuesInitialized=false;
	simulationEndedMain();
}

void CMirror::serialize(CSer& ar)
{
	serializeMain(ar);
	if (ar.isStoring())
	{ // Storing
		
		ar.storeDataName("Msz");
		ar << _mirrorWidth << _mirrorHeight;
		ar.flush();

		ar.storeDataName("Cas");
		BYTE nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,_active);
		SIM_SET_CLEAR_BIT(nothing,1,!_isMirror);
		ar << nothing;
		ar.flush();

		ar.storeDataName("Clp");
		ar << _clippingObjectOrCollection;
		ar.flush();

		ar.storeDataName("Mcr");
		ar << _mirrorReflectance << mirrorColor[0] << mirrorColor[1] << mirrorColor[2];
		ar.flush();

		ar.storeDataName("Cpc");
		ar.setCountingMode();
		clipPlaneColor.serialize(ar);
		if (ar.setWritingMode())
			clipPlaneColor.serialize(ar);

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
				if (theName.compare("Msz")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _mirrorWidth >> _mirrorHeight;
				}
				if (theName=="Cas")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_active=SIM_IS_BIT_SET(nothing,0);
					_isMirror=!SIM_IS_BIT_SET(nothing,1);
				}
				if (theName.compare("Clp")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _clippingObjectOrCollection;
				}
				if (theName.compare("Mcr")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _mirrorReflectance >> mirrorColor[0] >> mirrorColor[1] >> mirrorColor[2];
				}
				if (theName.compare("Cpc")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					clipPlaneColor.serialize(ar);
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
		if ((ar.getSerializationVersionThatWroteThisFile()<17)&&App::userSettings->correctColorsAndLights)
		{ // on 29/08/2013 we corrected all default lights. So we need to correct for that change:
			CTTUtil::scaleColorUp_(mirrorColor);
		}
	}
}

void CMirror::serializeWExtIk(CExtIkSer& ar)
{
	serializeWExtIkMain(ar);
	CDummy::serializeWExtIkStatic(ar);
}
