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
#include "Mill.h"
#include "Tt.h"
#include "OGL.h"

#include "CuttingRoutine.h"
#include "VDateTime.h"
#include "IloIlo.h"
#include "ttUtil.h"
#include "easyLock.h"
#include "App.h"
#include "collDistInterface.h"

CMill::CMill(int theType)
{
	commonInit();
	setMillType(theType);
}

CMill::CMill()
{ // needed by the serialization routine only!
	commonInit();
}

CMill::~CMill()
{
	delete convexVolume;
}

std::string CMill::getObjectTypeInfo()
{
	return("Mill");
}

std::string CMill::getObjectTypeInfoExtended()
{
	if (_millType==sim_mill_pyramid_subtype)
		return("Mill (pyramid)");
	if (_millType==sim_mill_cylinder_subtype)
		return("Mill (cylinder)");
	if (_millType==sim_mill_disc_subtype)
		return("Mill (disc)");
	if (_millType==sim_mill_cone_subtype)
		return("Mill (cone)");
	return("ERROR");
}

void CMill::commonInit()
{
	convexVolume=new CConvexVolume();
	_explicitHandling=false;
	_objectType=sim_object_mill_type;
	_size=0.01f;
	_showMillBase=true;
	_mainPropertyLocal=0;
	_millableObject=-1;
	_millDataValid=false;
	_calcTimeInMs=0;
	_milledSurface=0.0f;
	_milledVolume=0.0f;
	_milledObjectCount=0;

	_initialValuesInitialized=false;

	passiveVolumeColor.setDefaultValues();
	passiveVolumeColor.setColor(0.0f,0.35f,0.6f,AMBIENT_MODE);
	activeVolumeColor.setDefaultValues();
	activeVolumeColor.setColor(1.0f,0.0f,0.0f,AMBIENT_MODE);
	layer=MILL_LAYER;
	_objectName="Mill";
}

void CMill::setExplicitHandling(bool setExplicit)
{
	_explicitHandling=setExplicit;
}

bool CMill::getExplicitHandling()
{
	return(_explicitHandling);
}

int CMill::getMillableObject()
{
	return(_millableObject);
}

void CMill::setMillableObject(int objectID)
{
	_millableObject=objectID;
}

bool CMill::getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices)
{
	vertices.clear();
	indices.clear();
	return(false); // for now
}

void CMill::display(CViewableBase* renderingObject,int displayAttrib)
{
	FUNCTION_INSIDE_DEBUG("CMill::display");
	EASYLOCK(_objectMutex);
	// At the beginning of every 3DObject display routine:
	commonStart(displayAttrib);

	// Display the bounding box:
	if (displayAttrib&sim_displayattribute_renderpass) 
		displayBoundingBox(displayAttrib);

	// Display the object:
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
		App::ct->objCont->enableAuxClippingPlanes(_objectID);
		bool wire=false;
		if ( (displayAttrib&sim_displayattribute_forcewireframe)&&(displayAttrib&sim_displayattribute_renderpass) )
		{
			wire=true;
			glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);
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


		if (_millDataValid&&(_milledObjectCount>0))
			activeVolumeColor.makeCurrentColor();
		else
			passiveVolumeColor.makeCurrentColor();

		if (_showMillBase)
			ogl::displayBox(_size/2.0f,_size/2.0f,_size/2.0f,!wire);

		glBegin(GL_LINES);
		for (int i=0;i<int(convexVolume->volumeEdges.size())/6;i++)
		{
			glVertex3fv(&convexVolume->volumeEdges[6*i+0]);
			glVertex3fv(&convexVolume->volumeEdges[6*i+3]);
		}
		glEnd();

		ogl::setColorsAllBlack();
		ogl::setColor(1.0f,0.0f,0.0f,EMISSION_MODE);
		glBegin(GL_LINES);
		for (int i=0;i<int(convexVolume->normalsInside.size())/6;i++)
		{
			glVertex3fv(&convexVolume->normalsInside[6*i+0]);
			glVertex3fv(&convexVolume->normalsInside[6*i+3]);
		}
		glEnd();
		App::ct->objCont->disableAuxClippingPlanes();
	}

	// At the end of every 3DObject display routine:
	commonFinish();
}

C3DObject* CMill::copyYourself()
{	
	CMill* newMill=(CMill*)copyYourselfMain();

	newMill->_millableObject=_millableObject;
	newMill->_milledSurface=_milledSurface;
	newMill->_milledVolume=_milledVolume;
	newMill->_milledObjectCount=_milledObjectCount;
	newMill->_millDataValid=false;
	newMill->_size=_size;
	newMill->_showMillBase=_showMillBase;
	newMill->_explicitHandling=_explicitHandling;
	newMill->_millType=_millType;

	delete newMill->convexVolume;
	newMill->convexVolume=convexVolume->copyYourself();

	// Colors:
	passiveVolumeColor.copyYourselfInto(&newMill->passiveVolumeColor);
	activeVolumeColor.copyYourselfInto(&newMill->activeVolumeColor);

	newMill->_initialValuesInitialized=_initialValuesInitialized;
	newMill->_initialExplicitHandling=_initialExplicitHandling;

	return(newMill);
}

bool CMill::announceObjectWillBeErased(int objID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	// This routine can be called for objCont-objects, but also for objects
	// in the copy-buffer!! So never make use of any 
	// 'ct::objCont->getObject(id)'-call or similar
	// Return value true means 'this' has to be erased too!
	announceObjectWillBeErasedMain(objID,copyBuffer);
	if (_millableObject==objID)
		_millableObject=-1;
	return(false);
}

void CMill::announceGroupWillBeErased(int groupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGroupWillBeErasedMain(groupID,copyBuffer);
	if (_millableObject==groupID)
		_millableObject=-1;
}
void CMill::announceCollisionWillBeErased(int collisionID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceCollisionWillBeErasedMain(collisionID,copyBuffer);
}
void CMill::announceDistanceWillBeErased(int distanceID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceDistanceWillBeErasedMain(distanceID,copyBuffer);
}
void CMill::announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceIkObjectWillBeErasedMain(ikGroupID,copyBuffer);
}
void CMill::announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGcsObjectWillBeErasedMain(gcsObjectID,copyBuffer);
}
void CMill::performObjectLoadingMapping(std::vector<int>* map)
{
	performObjectLoadingMappingMain(map);
	if (_millableObject<SIM_IDSTART_COLLECTION)
		_millableObject=App::ct->objCont->getLoadingMapping(map,_millableObject);
}
void CMill::performGroupLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performGroupLoadingMappingMain(map);
	if (_millableObject>=SIM_IDSTART_COLLECTION)
		_millableObject=App::ct->objCont->getLoadingMapping(map,_millableObject);
}
void CMill::performCollisionLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performCollisionLoadingMappingMain(map);
}
void CMill::performDistanceLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performDistanceLoadingMappingMain(map);
}
void CMill::performIkLoadingMapping(std::vector<int>* map)
{
	performIkLoadingMappingMain(map);
}
void CMill::performGcsLoadingMapping(std::vector<int>* map)
{
	performGcsLoadingMappingMain(map);
}

void CMill::performTextureObjectLoadingMapping(std::vector<int>* map)
{
	performTextureObjectLoadingMappingMain(map);
}

void CMill::performDynMaterialObjectLoadingMapping(std::vector<int>* map)
{
	performDynMaterialObjectLoadingMappingMain(map);
}

void CMill::bufferMainDisplayStateVariables()
{
	bufferMainDisplayStateVariablesMain();
}

void CMill::bufferedMainDisplayStateVariablesToDisplay()
{
	bufferedMainDisplayStateVariablesToDisplayMain();
}

void CMill::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	initializeInitialValuesMain(simulationIsRunning);
	_initialValuesInitialized=simulationIsRunning;
	if (simulationIsRunning)
	{
		_initialExplicitHandling=_explicitHandling;
	}
}

void CMill::simulationAboutToStart()
{
	initializeInitialValues(true);
}

void CMill::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd()&&((getCumulativeMainPropertyOverride()&sim_modelproperty_not_reset)==0))
	{
		_explicitHandling=_initialExplicitHandling;
	}
	_initialValuesInitialized=false;
	simulationEndedMain();
}

void CMill::serialize(CSer& ar)
{
	serializeMain(ar);
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Snt");
		ar << _millType;
		ar.flush();

		ar.storeDataName("Vod");
		ar.setCountingMode();
		convexVolume->serialize(ar);
		if (ar.setWritingMode())
			convexVolume->serialize(ar);

		ar.storeDataName("Sns");
		ar << _size;
		ar.flush();

		ar.storeDataName("Pra");
		BYTE nothing=0;
		// 12/12/2011 SIM_SET_CLEAR_BIT(nothing,0,_millAllCuttable);
		SIM_SET_CLEAR_BIT(nothing,1,_showMillBase);
		SIM_SET_CLEAR_BIT(nothing,2,_explicitHandling);
		ar << nothing;
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

		ar.storeDataName("Sox");
		ar << _millableObject;
		ar.flush();

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
				if (theName.compare("Snt")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _millType;
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
					ar >> _size;
				}
				if (theName.compare("Sox")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _millableObject;
				}
				if (theName=="Pra")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_showMillBase=SIM_IS_BIT_SET(nothing,1);
					_explicitHandling=SIM_IS_BIT_SET(nothing,2);
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
				if (noHit)
					ar.loadUnknownData();
			}
		}
		if ((ar.getSerializationVersionThatWroteThisFile()<17)&&App::userSettings->correctColorsAndLights)
		{ // on 29/08/2013 we corrected all default lights. So we need to correct for that change:
			CTTUtil::scaleColorUp_(passiveVolumeColor.colors);
			CTTUtil::scaleColorUp_(activeVolumeColor.colors);
		}
	}
}

void CMill::serializeWExtIk(CExtIkSer& ar)
{
	serializeWExtIkMain(ar);
	CDummy::serializeWExtIkStatic(ar);
}

bool CMill::getMillingVolumeBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	return(convexVolume->getVolumeBoundingBox(minV,maxV));
}

bool CMill::getFullBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	getMillingVolumeBoundingBox(minV,maxV);
	C3Vector m(_size*0.25f,_size*0.25f,_size*0.25f); // mill base
	C3Vector n(-_size*0.25f,-_size*0.25f,-_size*0.25f);
	minV.keepMin(n);
	maxV.keepMax(m);
	return(true);
}

bool CMill::getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	return (getFullBoundingBox(minV,maxV));
}

void CMill::resetMill(bool exceptExplicitHandling)
{
	if ( (!exceptExplicitHandling)||(!getExplicitHandling()) )
	{
		_millDataValid=false;
		_calcTimeInMs=0;
	}
}

int CMill::handleMill(bool exceptExplicitHandling,float& milledSurface,float& milledVolume,bool justForInitialization)
{
	if (exceptExplicitHandling&&getExplicitHandling())
		return(0); // We don't want to handle those
	_millDataValid=false;
	_calcTimeInMs=0;
	if (!App::ct->mainSettings->millsEnabled)
		return(0);
	if (!CCollDistInterface::initializeFunctionsIfNeeded())
		return(0);

	int stTime=VDateTime::getTimeInMs();

	// For now:
	int cutObjectHandle=-1;
	milledSurface=0.0f;
	milledVolume=0.0f;
	_milledObjectCount=CCuttingRoutine::cutEntity(getID(),_millableObject,cutObjectHandle,milledSurface,milledVolume,justForInitialization,false);
	_milledSurface=milledSurface;
	_milledVolume=milledVolume;
	_calcTimeInMs=VDateTime::getTimeDiffInMs(stTime);
	_millDataValid=true;
	return(_milledObjectCount);
}

float CMill::getCalculationTime()
{
	return(float(_calcTimeInMs)*0.001f);
}

void CMill::setSize(float newSize)
{
	tt::limitValue(0.0001f,10.0f,newSize);
	_size=newSize;
}
float CMill::getSize()
{
	return(_size);
}

bool CMill::getMilledSurface(float& surf)
{
	if (!_millDataValid)
		return(false);
	surf=_milledSurface;
	return(true);
}

bool CMill::getMilledVolume(float& vol)
{
	if (!_millDataValid)
		return(false);
	vol=_milledVolume;
	return(true);
}

bool CMill::getMilledCount(int& milledCount)
{
	if (!_millDataValid)
		return(false);
	milledCount=_milledObjectCount;
	return(true);
}

void CMill::scaleObject(float scalingFactor)
{
	_size*=scalingFactor;
	convexVolume->scaleVolume(scalingFactor);
	scaleObjectMain(scalingFactor);
}

void CMill::setMillType(int theType)
{
	_millType=theType;
	if (theType==sim_mill_cylinder_subtype)
		convexVolume->setVolumeType(CYLINDER_TYPE_CONVEX_VOLUME,_objectType,_size);
	if (theType==sim_mill_disc_subtype)
		convexVolume->setVolumeType(DISC_TYPE_CONVEX_VOLUME,_objectType,_size);
	if (theType==sim_mill_pyramid_subtype)
		convexVolume->setVolumeType(PYRAMID_TYPE_CONVEX_VOLUME,_objectType,_size);
	if (theType==sim_mill_cone_subtype)
		convexVolume->setVolumeType(CONE_TYPE_CONVEX_VOLUME,_objectType,_size);
}
int CMill::getMillType()
{
	return(_millType);
}

void CMill::setShowMillBase(bool show)
{
	_showMillBase=show;
}

bool CMill::getShowMillBase()
{
	return(_showMillBase);
}
