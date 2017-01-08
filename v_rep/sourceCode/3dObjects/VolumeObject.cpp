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
#include "VolumeObject.h"
#include "App.h"
#include "Tt.h"
#include "OGL.h"
#include "easyLock.h"

CVolumeObject::CVolumeObject()
{
	_commonInit();
}

CVolumeObject::CVolumeObject(C3Vector size,float resolution)
{
	_commonInit();
	resolution=tt::getLimitedFloat(0.0001f,0.1f,resolution);
	size(0)=tt::getLimitedFloat(resolution,50000*resolution,size(0));
	size(1)=tt::getLimitedFloat(resolution,50000*resolution,size(1));
	size(2)=tt::getLimitedFloat(resolution,50000*resolution,size(2));
	_originalSize=size;
	_sideSize=SIM_MAX(size(0),SIM_MAX(size(1),size(2)));
	_resolution=resolution;
	_volumeNode=new CVolumeNode(_sideSize,_sideSize);
	// We now cut the volume to size:
	for (int i=0;i<3;i++)
	{
		if (_originalSize(i)!=_sideSize)
		{
			std::vector<float> planes;
			C3Vector n;
			n.clear();
			n(i)=1.0f;
			for (int j=0;j<3;j++)
				planes.push_back(n(j));
			planes.push_back(-(-_sideSize*0.5f+_originalSize(i)));
			cutVolume(planes);
		}
	}
	// We set the center position (has to be done after the previous cutting!)
	_centerPosition=C3Vector(1.0f,1.0f,1.0f)*_sideSize-_originalSize;
}

void CVolumeObject::_commonInit()
{
	setObjectType(sim_object_volume_type);
	color.setDefaultValues();
	color.setColor(0.0f,0.3f,0.0f,AMBIENT_MODE);
	layer=VOLUME_LAYER;
	_mainPropertyLocal=sim_objectspecialproperty_detectable_all|sim_objectspecialproperty_renderable|sim_objectspecialproperty_cuttable; // for now
	_objectName="Volume";
	_sideSize=0.1f;
	_originalSize.set(0.1f,0.1f,0.1f);
	_centerPosition=C3Vector::zeroVector;
	_resolution=0.001f;
	_volumeNode=NULL;
}

CVolumeObject::~CVolumeObject()
{
	delete _volumeNode;
}

std::string CVolumeObject::getObjectTypeInfo()
{
	return("Volume");
}
std::string CVolumeObject::getObjectTypeInfoExtended()
{
	return("Volume");
}

bool CVolumeObject::getFullBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	if (_volumeNode==NULL)
		return(false);
	bool first=true;
	_volumeNode->getVisualBoundingBox(first,_sideSize,_centerPosition,4,minV,maxV);
	return(true);
}

bool CVolumeObject::getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	return(getFullBoundingBox(minV,maxV));
}

bool CVolumeObject::getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices)
{
	vertices.clear();
	indices.clear();
	return(false); // for now
}

void CVolumeObject::display(CViewableBase* renderingObject,int displayAttrib)
{
	FUNCTION_INSIDE_DEBUG("CVolumeObject::display");
	EASYLOCK(_objectMutex);
	if (_volumeNode==NULL)
		return;
	// At the beginning of every 3DObject display routine:
	commonStart(displayAttrib);
	//if ( (_objectProperty&sim_objectproperty_wireframe)&&((displayAttrib&sim_displayattribute_forbidwireframe)==0) )
	//	displayAttrib|=sim_displayattribute_forcewireframe;

	// Bounding box display:
	if (displayAttrib&sim_displayattribute_renderpass)
		displayBoundingBox(displayAttrib);

	// Object display:
	if ((!isObjectPartOfInvisibleModel())&&((App::ct->mainSettings->activeLayers&layer)||(displayAttrib&sim_displayattribute_ignorelayer)))
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
		bool wire=false;
		if ( (displayAttrib&sim_displayattribute_forcewireframe)&&(displayAttrib&sim_displayattribute_renderpass) )
		{
			wire=true;
			glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);
		}
		else
			glEnable(GL_CULL_FACE);

		color.makeCurrentColor();

		int collColl=App::ct->collisions->getCollisionColor(_objectID);
		for (int i=0;i<int(App::ct->collections->allGroups.size());i++)
		{
			if (App::ct->collections->allGroups[i]->isObjectInGroup(_objectID))
				collColl|=App::ct->collisions->getCollisionColor(App::ct->collections->allGroups[i]->getGroupID());
		}
		if (displayAttrib&sim_displayattribute_originalcolors)
			collColl=0;
		if (collColl==1)
			App::ct->mainSettings->collisionColor.makeCurrentColor();
		if (collColl==2)
			App::ct->mainSettings->collisionColor.makeCurrentColor();
		if (collColl==3)
			App::ct->mainSettings->collisionColor.makeCurrentColor();

		std::vector<float> relViewFrustum;
		C7Vector tr(getCumulativeTransformation().getInverse());
		renderingObject->computeViewFrustumIfNeeded(); // important!
		for (int i=0;i<int(renderingObject->viewFrustum.size())/4;i++)
		{
			C3Vector n(&renderingObject->viewFrustum[4*i+0]);
			float d=renderingObject->viewFrustum[4*i+3];
			C3Vector pt(n*(d*-1.0f));
			pt*=tr;
			n=tr.Q*n; // orientation only
			d=-(pt*n);
			relViewFrustum.push_back(n(0));
			relViewFrustum.push_back(n(1));
			relViewFrustum.push_back(n(2));
			relViewFrustum.push_back(d);
		}

		_volumeNode->display(_sideSize,0.0001f,_centerPosition,relViewFrustum);
		
		glDisable(GL_CULL_FACE);
	}

	// At the end of every 3DObject display routine:
	commonFinish();
}

void CVolumeObject::scaleObject(float scalingFactor)
{
	_sideSize*=scalingFactor;
	_resolution*=scalingFactor;
	_centerPosition*=scalingFactor;
	_originalSize*=scalingFactor;
	scaleObjectMain(scalingFactor);
}

C3DObject* CVolumeObject::copyYourself()
{	
	CVolumeObject* newVolumeObject=(CVolumeObject*)copyYourselfMain();
	// Colors
	color.copyYourselfInto(&newVolumeObject->color);
	// Various:
	newVolumeObject->_volumeNode=_volumeNode->copyYourself();
	newVolumeObject->_sideSize=_sideSize;
	newVolumeObject->_resolution=_resolution;
	newVolumeObject->_originalSize=_originalSize;
	newVolumeObject->_centerPosition=_centerPosition;
	
	return(newVolumeObject);
}

bool CVolumeObject::announceObjectWillBeErased(int objID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	// This routine can be called for objCont-objects, but also for objects
	// in the copy-buffer!! So never make use of any 
	// 'ct::objCont->getObject(id)'-call or similar
	// Return value true means 'this' has to be erased too!
	announceObjectWillBeErasedMain(objID,copyBuffer);
	return(false);
}

void CVolumeObject::announceGroupWillBeErased(int groupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGroupWillBeErasedMain(groupID,copyBuffer);
}
void CVolumeObject::announceCollisionWillBeErased(int collisionID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceCollisionWillBeErasedMain(collisionID,copyBuffer);
}
void CVolumeObject::announceDistanceWillBeErased(int distanceID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceDistanceWillBeErasedMain(distanceID,copyBuffer);
}
void CVolumeObject::announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceIkObjectWillBeErasedMain(ikGroupID,copyBuffer);
}
void CVolumeObject::announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGcsObjectWillBeErasedMain(gcsObjectID,copyBuffer);
}
void CVolumeObject::performObjectLoadingMapping(std::vector<int>* map)
{ // New_Object_ID=map[Old_Object_ID]
	performObjectLoadingMappingMain(map);
}
void CVolumeObject::performGroupLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performGroupLoadingMappingMain(map);
}
void CVolumeObject::performCollisionLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performCollisionLoadingMappingMain(map);
}
void CVolumeObject::performDistanceLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performDistanceLoadingMappingMain(map);
}
void CVolumeObject::performIkLoadingMapping(std::vector<int>* map)
{
	performIkLoadingMappingMain(map);
}
void CVolumeObject::performGcsLoadingMapping(std::vector<int>* map)
{
	performGcsLoadingMappingMain(map);
}
void CVolumeObject::performTextureObjectLoadingMapping(std::vector<int>* map)
{
	performTextureObjectLoadingMappingMain(map);
}
void CVolumeObject::performDynMaterialObjectLoadingMapping(std::vector<int>* map)
{
	performDynMaterialObjectLoadingMappingMain(map);
}
void CVolumeObject::bufferMainDisplayStateVariables()
{
	bufferMainDisplayStateVariablesMain();
}
void CVolumeObject::bufferedMainDisplayStateVariablesToDisplay()
{
	bufferedMainDisplayStateVariablesToDisplayMain();
}

void CVolumeObject::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	initializeInitialValuesMain(simulationIsRunning);
//	_initialValuesInitialized=simulationIsRunning;
//	if (simulationIsRunning)
//	{
//	}
}

void CVolumeObject::simulationAboutToStart()
{
	initializeInitialValues(true);
}

void CVolumeObject::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
//	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd()&&((getCumulativeMainPropertyOverride()&sim_modelproperty_not_reset)==0))
//	{
//	}
	simulationEndedMain();
}

void CVolumeObject::serialize(CSer& ar)
{
	serializeMain(ar);
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Cl0");
		ar.setCountingMode();
		color.serialize(ar);
		if (ar.setWritingMode())
			color.serialize(ar);

		ar.storeDataName("Siz");
		ar << _sideSize << _resolution;
		ar.flush();

		ar.storeDataName("Si2");
		ar << _originalSize(0) << _originalSize(1) << _originalSize(2);
		ar << _centerPosition(0) << _centerPosition(1) << _centerPosition(2);
		ar.flush();

		if (_volumeNode!=NULL)
		{
			ar.storeDataName("Vnd");
			ar.setCountingMode();
			int nodeSerializationVersion=0;
			ar << nodeSerializationVersion;
			_volumeNode->serialize(ar,nodeSerializationVersion);
			ar.flush(); // special here, don't remove!
			if (ar.setWritingMode())
			{
				ar << nodeSerializationVersion;
				_volumeNode->serialize(ar,nodeSerializationVersion);
				ar.flush(); // special here, don't remove!
			}
		}

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
				if (theName.compare("Cl0")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					color.serialize(ar);
				}
				if (theName.compare("Siz")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _sideSize >> _resolution;
				}
				if (theName.compare("Si2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _originalSize(0) >> _originalSize(1) >> _originalSize(2);
					ar >> _centerPosition(0) >> _centerPosition(1) >> _centerPosition(2);
				}
				if (theName.compare("Vnd")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					ar >> byteQuantity; // special here, don't remove!
					int nodeSerializationVersion;
					ar >> nodeSerializationVersion;
					_volumeNode=new CVolumeNode();
					_volumeNode->serialize(ar,nodeSerializationVersion);
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}

bool CVolumeObject::cutVolume(const std::vector<float>& planes)
{ // return value true means something was cut!
	char somethingCut=0;
	if (_volumeNode!=NULL)
	{
		if (_volumeNode->cutVolume(_sideSize,_resolution,_centerPosition,planes,somethingCut))
		{ // we have to remove the node
			delete _volumeNode;
			_volumeNode=NULL;
		}
	}
	return(somethingCut!=0);
}

void CVolumeObject::resetMilling()
{ // nothing for now...
}

bool CVolumeObject::applyMilling()
{ // nothing for now... (return true means: this volume has to be destroyed)
	return(false);
}
