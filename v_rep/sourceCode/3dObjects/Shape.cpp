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
#include "Shape.h"
#include "OGL.h"
#include "Tt.h"
#include "algos.h"
#include "IloIlo.h"
#include "StatCollObj.h"
#include "StatDistObj.h"
#include "ShapeComponent.h"
#include "GeomObject.h"
#include "App.h"
#include "geomWrap.h"
#include "geometric.h"
#include "collDistInterface.h"
#include "easyLock.h"

CShape::CShape()
{
	geomData=NULL;
	commonInit();
}

CShape::~CShape()
{
	delete geomData;
}

C3Vector CShape::getInitialDynamicVelocity()
{
	return(_initialDynamicVelocity);
}

void CShape::setInitialDynamicVelocity(const C3Vector& vel)
{
	_initialDynamicVelocity=vel;
}

C3Vector CShape::getInitialDynamicAngularVelocity()
{
	return(_initialDynamicAngularVelocity);
}

void CShape::setInitialDynamicAngularVelocity(const C3Vector& vel)
{
	_initialDynamicAngularVelocity=vel;
}

void CShape::setRigidBodyWasAlreadyPutToSleepOnce(bool s)
{
	_rigidBodyWasAlreadyPutToSleepOnce=s;
}

bool CShape::getRigidBodyWasAlreadyPutToSleepOnce()
{
	return(_rigidBodyWasAlreadyPutToSleepOnce);
}

void CShape::actualizeContainsTransparentComponent()
{
	if (geomData!=NULL) // important
		_containsTransparentComponents=geomData->geomInfo->getContainsTransparentComponents();
}

bool CShape::getContainsTransparentComponent()
{
	return(_containsTransparentComponents);
}

void CShape::setShapeIsStaticAndNotRespondableButDynamicTag(bool f)
{
	_shapeIsStaticAndNotRespondableButDynamicTag=f;
}

bool CShape::getShapeIsStaticAndNotRespondableButDynamicTag()
{
	return(_shapeIsStaticAndNotRespondableButDynamicTag);
}

void CShape::setDynamicCollisionMask(WORD m)
{
	_dynamicCollisionMask=m;
}

WORD CShape::getDynamicCollisionMask()
{
	return(_dynamicCollisionMask);
}

C3DObject* CShape::getLastParentForLocalGlobalCollidable()
{
	if (_lastParentForLocalGlobalCollidable!=NULL)
		return(_lastParentForLocalGlobalCollidable);
	C3DObject* it=this;
	while (it->getParent()!=NULL)
		it=it->getParent();
	_lastParentForLocalGlobalCollidable=it;
	return(_lastParentForLocalGlobalCollidable);
}

void CShape::clearLastParentForLocalGlobalCollidable()
{
	_lastParentForLocalGlobalCollidable=NULL;
}

void CShape::setParentFollowsDynamic(bool f)
{ // careful, is also called from makeObjectChildOf routine
	_parentFollowsDynamic=f;
}

bool CShape::getParentFollowsDynamic()
{
	return(_parentFollowsDynamic);
}

std::string CShape::getObjectTypeInfo()
{
	return("Shape");
}

std::string CShape::getObjectTypeInfoExtended()
{
	if (geomData->geomInfo->isGeometric())
	{
		int pureType=((CGeometric*)geomData->geomInfo)->getPurePrimitiveType();
		if (pureType==sim_pure_primitive_none)
			return("Shape (simple, non-pure)");
		if (pureType==sim_pure_primitive_plane)
			return("Shape (simple, pure (plane))");
		if (pureType==sim_pure_primitive_disc)
			return("Shape (simple, pure (disc))");
		if (pureType==sim_pure_primitive_cuboid)
			return("Shape (simple, pure (cuboid))");
		if (pureType==sim_pure_primitive_spheroid)
			return("Shape (simple, pure (sphere))");
		if (pureType==sim_pure_primitive_cylinder)
			return("Shape (simple, pure (cylinder))");
		if (pureType==sim_pure_primitive_cone)
			return("Shape (simple, pure (cone))");
		if (pureType==sim_pure_primitive_heightfield)
			return("Shape (simple, pure (heightfield))");
	}
	else
	{
		if (!geomData->geomInfo->isPure())
			return("Shape (multishape, non-pure)");
		else
			return("Shape (multishape, pure)");
	}
	return("ERROR");
}

bool CShape::getFullBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	maxV=((CShape*)this)->geomData->getBoundingBoxHalfSizes();
	minV=maxV*-1.0f;
	return(true);
}

bool CShape::getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	return(getFullBoundingBox(minV,maxV));
}

void CShape::commonInit()
{
	setObjectType(sim_object_shape_type);
	_initialValuesInitialized=false;
	_containsTransparentComponents=false;
	_startInDynamicSleeping=false;
	_shapeIsDynamicallyStatic=true;
	_setAutomaticallyToNonStaticIfGetsParent=false;
	_shapeIsDynamicallyRespondable=false; // keep false, otherwise too many "default" problems
	_dynamicCollisionMask=0xffff;
	_parentFollowsDynamic=false;
	_lastParentForLocalGlobalCollidable=NULL;
	_initialDynamicVelocity.clear();
	_initialDynamicAngularVelocity.clear();

	layer=SHAPE_LAYER;
	_mainPropertyLocal=sim_objectspecialproperty_collidable|sim_objectspecialproperty_measurable|sim_objectspecialproperty_detectable_all|sim_objectspecialproperty_renderable;// by DEFAULT not cuttable! |sim_objectspecialproperty_cuttable;
	_objectName="Shape";

	_dynamicLinearVelocity.clear();
	_dynamicAngularVelocity.clear();
	_additionalForce.clear();
	_additionalTorque.clear();
}

void CShape::setDynamicVelocity(const C3Vector& linearV,const C3Vector& angularV)
{
	_dynamicLinearVelocity=linearV;
	_dynamicAngularVelocity=angularV;
}

C3Vector CShape::getDynamicLinearVelocity()
{
	return(_dynamicLinearVelocity);
}

C3Vector CShape::getDynamicAngularVelocity()
{
	return(_dynamicAngularVelocity);
}

void CShape::addAdditionalForceAndTorque(const C3Vector& f,const C3Vector& t)
{
	_additionalForce+=f;
	_additionalTorque+=t;
}

void CShape::clearAdditionalForceAndTorque()
{
	_additionalForce.clear();
	_additionalTorque.clear();
}

C3Vector CShape::getAdditionalForce()
{
	return(_additionalForce);
}

C3Vector CShape::getAdditionalTorque()
{
	return(_additionalTorque);
}

void CShape::setRespondable(bool r)
{
	_shapeIsDynamicallyRespondable=r;
}

bool CShape::getRespondable()
{
	return(_shapeIsDynamicallyRespondable);
}



bool CShape::getSetAutomaticallyToNonStaticIfGetsParent()
{
	return(_setAutomaticallyToNonStaticIfGetsParent);
}

void CShape::setSetAutomaticallyToNonStaticIfGetsParent(bool autoNonStatic)
{
	_setAutomaticallyToNonStaticIfGetsParent=autoNonStatic;
}

bool CShape::getStartInDynamicSleeping()
{
	return(_startInDynamicSleeping);
}

void CShape::setStartInDynamicSleeping(bool sleeping)
{
	_startInDynamicSleeping=sleeping;
}

bool CShape::getShapeIsDynamicallyStatic()
{
	return(_shapeIsDynamicallyStatic);
}

void CShape::setShapeIsDynamicallyStatic(bool sta)
{
	_shapeIsDynamicallyStatic=sta;
	if (!sta)
		_setAutomaticallyToNonStaticIfGetsParent=false;
}

void CShape::setInsideAndOutsideFacesSameColor(bool s)
{
	if ((geomData!=NULL)&&(geomData->geomInfo->isGeometric()))
		((CGeometric*)geomData->geomInfo)->setInsideAndOutsideFacesSameColor(s);
}
bool CShape::getInsideAndOutsideFacesSameColor()
{
	if ((geomData!=NULL)&&(geomData->geomInfo->isGeometric()))
		return(((CGeometric*)geomData->geomInfo)->getInsideAndOutsideFacesSameColor());
	return(true);
}

bool CShape::isGrouping()
{
	return(!geomData->geomInfo->isGeometric());
}

int CShape::getEdgeWidth()
{
	if ((geomData!=NULL)&&(geomData->geomInfo->isGeometric()))
		return(((CGeometric*)geomData->geomInfo)->getEdgeWidth());
	return(0);
}

void CShape::setEdgeWidth(int w)
{
	w=tt::getLimitedInt(1,4,w);
	if ((geomData!=NULL)&&(geomData->geomInfo->isGeometric()))
		((CGeometric*)geomData->geomInfo)->setEdgeWidth(w);
}

bool CShape::getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices)
{
	vertices.clear();
	indices.clear();
	if (index==0)
	{
		std::vector<float> visibleVertices;
		std::vector<int> visibleIndices;
		geomData->geomInfo->getCumulativeMeshes(visibleVertices,&visibleIndices,NULL);

		C7Vector m(getCumulativeTransformationPart1());
		C3Vector v;
		for (int j=0;j<int(visibleVertices.size())/3;j++)
		{
			v(0)=visibleVertices[3*j+0];
			v(1)=visibleVertices[3*j+1];
			v(2)=visibleVertices[3*j+2];
			v=m*v;
			vertices.push_back(v(0));
			vertices.push_back(v(1));
			vertices.push_back(v(2));
		}
		indices.assign(visibleIndices.begin(),visibleIndices.end());
		return(true);
	}
	return(App::ct->drawingCont->getExportableMeshAtIndex(getID(),index-1,vertices,indices));
}

void CShape::display(CViewableBase* renderingObject,int displayAttrib)
{
	FUNCTION_INSIDE_DEBUG("CShape::display");
	EASYLOCK(_objectMutex);
	// At the beginning of every 3DObject display routine:
	commonStart(displayAttrib);

	bool guiIsRendering=((displayAttrib&sim_displayattribute_forvisionsensor)==0);

	// Display the bounding box:
	if (displayAttrib&sim_displayattribute_renderpass)
		displayBoundingBox(displayAttrib);

	bool editNormals=((App::ct->objCont->getEditModeObjectID()==getID())&&(!isGrouping())&&(App::ct->objCont->getEditModeType()&TRIANGLE_EDIT_MODE));
	bool editVertices=((App::ct->objCont->getEditModeObjectID()==getID())&&(!isGrouping())&&(App::ct->objCont->getEditModeType()&VERTEX_EDIT_MODE));
	bool editEdges=((App::ct->objCont->getEditModeObjectID()==getID())&&(!isGrouping())&&(App::ct->objCont->getEditModeType()&EDGE_EDIT_MODE));
	bool editMode=editNormals||editVertices||editEdges;

	if (getShouldObjectBeDisplayed(displayAttrib)||editMode)
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

		if (editNormals)
		{
			if ((displayAttrib&sim_displayattribute_renderpass)!=0)
				drawReference();
			App::ct->objCont->displayFaceOrientation(displayAttrib);
		} 
		else if (editVertices)
		{
			if ((displayAttrib&sim_displayattribute_renderpass)!=0)
				drawReference();
			App::ct->objCont->displayVertices(displayAttrib);
		}
		else if (editEdges)
		{
			if ((displayAttrib&sim_displayattribute_renderpass)!=0)
				drawReference();
			App::ct->objCont->displayEdgeEditMode(displayAttrib);
		}
		else
		{
			App::ct->objCont->enableAuxClippingPlanes(_objectID);

			// Display the mass and inertia:
			if ( (displayAttrib&(sim_displayattribute_renderpass|sim_displayattribute_pickpass|sim_displayattribute_depthpass))&&App::ct->environment->getShowingInertias()&&(!_shapeIsDynamicallyStatic)&&(!editMode)&&((displayAttrib&sim_displayattribute_forvisionsensor)==0)&&((displayAttrib&sim_displayattribute_dynamiccontentonly)==0) )
			{
				C3Vector v(geomData->getBoundingBoxHalfSizes()*2.0f);
				geomData->geomInfo->displayInertia(sqrt(v*v));
			}

			int collColl=App::ct->collisions->getCollisionColor(_objectID);
			for (int i=0;i<int(App::ct->collections->allGroups.size());i++)
			{
				if (App::ct->collections->allGroups[i]->isObjectInGroup(_objectID))
					collColl|=App::ct->collisions->getCollisionColor(App::ct->collections->allGroups[i]->getGroupID());
			}
			if (displayAttrib&sim_displayattribute_originalcolors)
				collColl=0;
			CVisualParam collisionColor;
			if (collColl==1)
				App::ct->mainSettings->collisionColor.copyYourselfInto(&collisionColor);
			if (collColl==2)
				App::ct->mainSettings->collisionColor.copyYourselfInto(&collisionColor);
			if (collColl==3)
				App::ct->mainSettings->collisionColor.copyYourselfInto(&collisionColor);

			CVisualParam* collCol=NULL;
			if (collColl)
				collCol=&collisionColor;

			if (geomData->isCollisionInformationInitialized()&&(CCollDistInterface::getCalculatedPolygonCount(geomData->collInfo)!=0))
			{
				((CGeometric*)geomData->geomInfo)->displayForCutting(geomData,displayAttrib,collCol);
			}
			else
			{
				if (renderingObject->isObjectInsideView(getCumulativeTransformation(),geomData->getBoundingBoxHalfSizes()))
				{ // the bounding box is inside of the view (at least some part of it!)
					if (displayAttrib&sim_displayattribute_simplifyasboundingbox)
					{
						C3Vector bbMin,bbMax;
						getFullBoundingBox(bbMin,bbMax);
						ogl::setColorsAllBlack();
						if (displayAttrib&sim_displayattribute_forcewireframe)
						{
							glLineStipple(1,0x5555);
							glEnable(GL_LINE_STIPPLE);
							ogl::displayBox(bbMax(0)*2.0f,bbMax(1)*2.0f,bbMax(2)*2.0f,(displayAttrib&sim_displayattribute_renderpass)==0);
							glLineStipple(1,0xaaaa);
							ogl::setColor(1.0f,1.0f,1.0f,EMISSION_MODE);
							ogl::displayBox(bbMax(0)*2.0f,bbMax(1)*2.0f,bbMax(2)*2.0f,(displayAttrib&sim_displayattribute_renderpass)==0);
							glDisable(GL_LINE_STIPPLE);
						}
						else
						{
							glPolygonOffset(0.5f,0.0f); // Second argument set to 0.0 on 2009.01.05 (otherwise strange effects on some graphic cards)
							glEnable(GL_POLYGON_OFFSET_FILL);
							ogl::setColor(0.75f,0.6f,0.45f,AMBIENT_MODE);
							ogl::setColor(0.4f,0.4f,0.4f,DIFFUSE_MODE);
							ogl::setColor(0.4f,0.4f,0.4f,SPECULAR_MODE);
							ogl::displayBox(bbMax(0)*2.0f,bbMax(1)*2.0f,bbMax(2)*2.0f,true);
							ogl::setColorsAllBlack();
							glDisable(GL_POLYGON_OFFSET_FILL);
							ogl::displayBox(bbMax(0)*2.0f,bbMax(1)*2.0f,bbMax(2)*2.0f,false);
						}
					}
					else
					{
						if ((displayAttrib&sim_displayattribute_colorcoded)==0)
						{
							if (_containsTransparentComponents)
							{
								geomData->geomInfo->display(geomData,displayAttrib,collCol,getDynamicObjectFlag_forVisualization_forDisplay(guiIsRendering),2);
								geomData->geomInfo->display(geomData,displayAttrib,collCol,getDynamicObjectFlag_forVisualization_forDisplay(guiIsRendering),1);
							}
							else
								geomData->geomInfo->display(geomData,displayAttrib,collCol,getDynamicObjectFlag_forVisualization_forDisplay(guiIsRendering),0);
						}
						else
							geomData->geomInfo->display_colorCoded(geomData,_objectID);
					}
				}
			}
			App::ct->objCont->disableAuxClippingPlanes();
		}
	}

	// At the end of every 3DObject display routine:
	commonFinish();
}

void CShape::scaleObject(float scalingFactor)
{	
	geomData->scale(scalingFactor,scalingFactor,scalingFactor); // will set the geomObject dynamics full refresh flag!
	scaleObjectMain(scalingFactor);
	// We have to reconstruct a part of the dynamics world:
	_dynamicsFullRefreshFlag=true;
}

bool CShape::announceObjectWillBeErased(int objID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	// This routine can be called for objCont-objects, but also for objects
	// in the copy-buffer!! So never make use of any 
	// 'ct::objCont->getObject(id)'-call or similar
	// Return value true means 'this' has to be erased too!
	announceObjectWillBeErasedMain(objID,copyBuffer);
	geomData->announce3DObjectWillBeErased(objID); // for textures based on vision sensors
	return(false);
}

void CShape::announceGroupWillBeErased(int groupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGroupWillBeErasedMain(groupID,copyBuffer);
}
void CShape::announceCollisionWillBeErased(int collisionID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceCollisionWillBeErasedMain(collisionID,copyBuffer);
}
void CShape::announceDistanceWillBeErased(int distanceID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceDistanceWillBeErasedMain(distanceID,copyBuffer);
}
void CShape::announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceIkObjectWillBeErasedMain(ikGroupID,copyBuffer);
}
void CShape::announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGcsObjectWillBeErasedMain(gcsObjectID,copyBuffer);
}
void CShape::performObjectLoadingMapping(std::vector<int>* map)
{ // New_Object_ID=map[Old_Object_ID]
	performObjectLoadingMappingMain(map);
	geomData->perform3DObjectLoadingMapping(map);
}
void CShape::performGroupLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performGroupLoadingMappingMain(map);
}
void CShape::performCollisionLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performCollisionLoadingMappingMain(map);
}
void CShape::performDistanceLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performDistanceLoadingMappingMain(map);
}
void CShape::performIkLoadingMapping(std::vector<int>* map)
{
	performIkLoadingMappingMain(map);
}
void CShape::performGcsLoadingMapping(std::vector<int>* map)
{
	performGcsLoadingMappingMain(map);
}

void CShape::performTextureObjectLoadingMapping(std::vector<int>* map)
{
	performTextureObjectLoadingMappingMain(map);
	geomData->performTextureObjectLoadingMapping(map);
}

void CShape::performDynMaterialObjectLoadingMapping(std::vector<int>* map)
{
	performDynMaterialObjectLoadingMappingMain(map);
	geomData->geomInfo->performDynMaterialObjectLoadingMapping(map);
}

void CShape::bufferMainDisplayStateVariables()
{
	bufferMainDisplayStateVariablesMain();
}

void CShape::bufferedMainDisplayStateVariablesToDisplay()
{
	bufferedMainDisplayStateVariablesToDisplayMain();
}

void CShape::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	initializeInitialValuesMain(simulationIsRunning);
	_dynamicLinearVelocity.clear();
	_dynamicAngularVelocity.clear();
	_additionalForce.clear();
	_additionalTorque.clear();
	_initialDynamicVelocity.clear();
	_initialDynamicAngularVelocity.clear();
	_initialValuesInitialized=simulationIsRunning;
	if (simulationIsRunning)
	{

	}

	actualizeContainsTransparentComponent(); // added on 2010/11/22 to correct at least each time a simulation starts, when those values where not set correctly
}

void CShape::simulationAboutToStart()
{
	initializeInitialValues(true);
	_rigidBodyWasAlreadyPutToSleepOnce=false;
}

void CShape::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd()&&((getCumulativeMainPropertyOverride()&sim_modelproperty_not_reset)==0))
	{

	}
	_initialValuesInitialized=false;
	_initialDynamicVelocity.clear();
	_initialDynamicAngularVelocity.clear();
	_dynamicLinearVelocity.clear();
	_dynamicAngularVelocity.clear();
	_additionalForce.clear();
	_additionalTorque.clear();
	simulationEndedMain();
}

void CShape::resetMilling()
{
	if ((_mainPropertyLocal&sim_objectspecialproperty_cuttable)&&geomData->isCollisionInformationInitialized())
		geomData->removeCollisionInformation();
}

bool CShape::applyMilling()
{
	if ((_mainPropertyLocal&sim_objectspecialproperty_cuttable)&&geomData->isCollisionInformationInitialized())
	{
		if (geomData->applyCuttingChanges(getCumulativeTransformation()))
			return(true); // means that this shape has to be destroyed!
		incrementMemorizedConfigurationValidCounter();
		geomData->removeCollisionInformation(); // is probably not needed
		C7Vector newCTM(geomData->getCreationTransformation());
		C7Vector xTr(getCumulativeTransformation().getInverse()*newCTM);
		geomData->setCreationTransformation(C7Vector::identityTransformation);

		setLocalTransformation(getLocalTransformation()*xTr);
		// we have to correct the attached drawing objects:
		App::ct->drawingCont->adjustForFrameChange(getID(),xTr.getInverse());
		for (int j=0;j<int(childList.size());j++) // adjust for the children:
			childList[j]->setLocalTransformation(xTr.getInverse()*childList[j]->getLocalTransformationPart1());
		
	}
	return(false);
}

void CShape::serialize(CSer& ar)
{
	serializeMain(ar);
	if (ar.isStoring())
	{	// Storing
		// Following tags are reserved (11/11/2012): Sco, Sc2, Eco, Ewt

		ar.storeDataName("Ge2");
		ar.setCountingMode();
		geomData->serialize(ar);
		if (ar.setWritingMode())
			geomData->serialize(ar);

		ar.storeDataName("Dc2");
		ar << _dynamicCollisionMask;
		ar.flush();

		ar.storeDataName("Sss");
		BYTE nothing=0;
//		SIM_SET_CLEAR_BIT(nothing,0,_explicitTracing); removed on 13/09/2011
//		SIM_SET_CLEAR_BIT(nothing,1,_visibleEdges); removed on 11/11/2012
//		SIM_SET_CLEAR_BIT(nothing,2,_culling); removed on 11/11/2012
//		SIM_SET_CLEAR_BIT(nothing,3,tracing); removed on 13/09/2011
//		SIM_SET_CLEAR_BIT(nothing,4,_shapeWireframe); removed on 11/11/2012
//		SIM_SET_CLEAR_BIT(nothing,5,_displayInvertedFaces); removed on 2010/04/19
		SIM_SET_CLEAR_BIT(nothing,6,_startInDynamicSleeping);
		SIM_SET_CLEAR_BIT(nothing,7,!_shapeIsDynamicallyStatic);
		ar << nothing;
		ar.flush();

		ar.storeDataName("Ss3");
		nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,_shapeIsDynamicallyRespondable);
//		SIM_SET_CLEAR_BIT(nothing,1,_visualizeInertia); // removed on 16/12/2012
		SIM_SET_CLEAR_BIT(nothing,2,_parentFollowsDynamic);
//		SIM_SET_CLEAR_BIT(nothing,3,!_insideAndOutsideFacesSameColor); // removed on 11/11/2012
//		SIM_SET_CLEAR_BIT(nothing,4,_containsTransparentComponents); // removed on 11/11/2012
		SIM_SET_CLEAR_BIT(nothing,5,_setAutomaticallyToNonStaticIfGetsParent);
		ar << nothing;
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
				if (theName.compare("Geo")==0)
				{ // keep for backward compatibility (nov. 2012). For serialization version 15 and previous
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					CGeomObject_OLD* geomData_OLD_BACKWARD_COMPATIBILITY_NOV_2012=new CGeomObject_OLD();
					geomData_OLD_BACKWARD_COMPATIBILITY_NOV_2012->serialize(ar);
					geomData=geomData_OLD_BACKWARD_COMPATIBILITY_NOV_2012->buildEquivalentGeomProxy();
					delete geomData_OLD_BACKWARD_COMPATIBILITY_NOV_2012;
				}
				if (theName.compare("Ge2")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					geomData=new CGeomProxy();
					geomData->serialize(ar);
				}
				if (theName.compare("Dc2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _dynamicCollisionMask;
				}
				if (theName=="Sss")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_startInDynamicSleeping=SIM_IS_BIT_SET(nothing,6);
					_shapeIsDynamicallyStatic=!SIM_IS_BIT_SET(nothing,7);
				}
				if (theName=="Ss2")
				{ // keep for backward compatibility (2010/07/12)
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_shapeIsDynamicallyRespondable=SIM_IS_BIT_SET(nothing,0);
					_parentFollowsDynamic=SIM_IS_BIT_SET(nothing,2);
				}
				if (theName=="Ss3")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_shapeIsDynamicallyRespondable=SIM_IS_BIT_SET(nothing,0);
					_parentFollowsDynamic=SIM_IS_BIT_SET(nothing,2);
					_setAutomaticallyToNonStaticIfGetsParent=SIM_IS_BIT_SET(nothing,5);
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
		actualizeContainsTransparentComponent();
	}
}

void CShape::serializeWExtIk(CExtIkSer& ar)
{
	serializeWExtIkMain(ar);
	CDummy::serializeWExtIkStatic(ar);
}

void CShape::alignBoundingBoxWithMainAxis()
{
	reorientGeometry(0);
}
void CShape::alignBoundingBoxWithWorld()
{
	reorientGeometry(1);	
}

bool CShape::alignTubeBoundingBoxWithMainAxis()
{
	return(reorientGeometry(2));	
}

bool CShape::alignCuboidBoundingBoxWithMainAxis()
{
	return(reorientGeometry(3));	
}

bool CShape::reorientGeometry(int type)
{ // return value is the success state of the operation
	C7Vector m(getCumulativeTransformationPart1());
	C7Vector mNew;
	bool error=false;
	if (type<2)
		mNew=geomData->recomputeOrientation(m,type==0);
	if (type==2)
		mNew=geomData->recomputeTubeOrCuboidOrientation(m,true,error);
	if (type==3)
		mNew=geomData->recomputeTubeOrCuboidOrientation(m,false,error);

	if (error)
		return(false);

	C7Vector mCorr(m.getInverse()*mNew);
	C7Vector mCorrInv(mCorr.getInverse());

	// Now we have to compute the new local transformation:
	setLocalTransformation(getLocalTransformationPart1()*mCorr);
	// we have to correct the attached drawing objects:
	App::ct->drawingCont->adjustForFrameChange(getID(),mCorrInv);
	incrementMemorizedConfigurationValidCounter();
	for (int i=0;i<int(childList.size());i++)
	{
		childList[i]->setLocalTransformation(mCorrInv*childList[i]->getLocalTransformationPart1());
		childList[i]->incrementMemorizedConfigurationValidCounter();
	}
	App::ct->objCont->setFullDialogRefreshFlag(); // so that textures and other things get updated!
	return(true);
}

void CShape::removeCollisionInformation()
{
	geomData->removeCollisionInformation();
}

/*
void CShape::initializeCollisionDetection()
{
	geomData->initializeCollisionInformation();
}
*/
bool CShape::isCollisionInformationInitialized()
{
	return(geomData->isCollisionInformationInitialized());
}


void CShape::initializeCalculationStructureIfNeeded()
{
	geomData->initializeCalculationStructureIfNeeded();
}

bool CShape::getCulling()
{
	if ((geomData!=NULL)&&(geomData->geomInfo->isGeometric()))
		return(((CGeometric*)geomData->geomInfo)->getCulling());
	return(false);
}

void CShape::setCulling(bool culState)
{
	if ((geomData!=NULL)&&(geomData->geomInfo->isGeometric()))
		((CGeometric*)geomData->geomInfo)->setCulling(culState);
}

bool CShape::getVisibleEdges()
{
	if ((geomData!=NULL)&&(geomData->geomInfo->isGeometric()))
		return(((CGeometric*)geomData->geomInfo)->getVisibleEdges());
	return(false);
}

void CShape::setVisibleEdges(bool v)
{
	if ((geomData!=NULL)&&(geomData->geomInfo->isGeometric()))
		((CGeometric*)geomData->geomInfo)->setVisibleEdges(v);
}

bool CShape::getShapeWireframe()
{
	if ((geomData!=NULL)&&(geomData->geomInfo->isGeometric()))
		return(((CGeometric*)geomData->geomInfo)->getWireframe());
	return(false);
}

void CShape::setShapeWireframe(bool w)
{
	if ((geomData!=NULL)&&(geomData->geomInfo->isGeometric()))
		((CGeometric*)geomData->geomInfo)->setWireframe(w);
}

bool CShape::doesShapeCollideWithShape(CShape* collidee,std::vector<float>* intersections,int buffer[2])
{	// If intersections is different from NULL, we check for all intersections and
	// intersection segments are appended to the vector
	CStatCollObj collObject(this,collidee);
	if (collObject.check(buffer,intersections!=NULL))
	{
		if (intersections!=NULL)
		{
			// Make the following nicer later:
			for (int i=0;i<int(collObject.intersection.size());i++)
				intersections->push_back(collObject.intersection[i]);
		}
		return(true);
	}
	return(false);
}

bool CShape::getDistanceToDummy_IfSmaller(CDummy* dummy,float &dist,float ray[7],int& buffer)
{	// Distance is measured from this to dummy
	// If the distance is smaller than 'dist', 'dist' is replaced and the return value is true
	// If the distance is bigger, 'dist' doesn't change and the return value is false
	// Build the node only when needed. So do it right here!
	initializeCalculationStructureIfNeeded();

	C3Vector dummyPos(dummy->getCumulativeTransformation().X);
	C4X4Matrix thisPCTM(getCumulativeTransformation());
	C3Vector rayPart0;
	C3Vector rayPart1;
	if (CCollDistInterface::getDistanceAgainstDummy_ifSmaller(geomData->collInfo,dummyPos,thisPCTM,dist,rayPart0,rayPart1,buffer))
	{
		rayPart0.copyTo(ray);
		rayPart1.copyTo(ray+3);
		ray[6]=dist;
		return(true);
	}
	return(false);
}

bool CShape::getShapeShapeDistance_IfSmaller(CShape* it,float &dist,float ray[7],int buffer[2])
{	// this is shape number 1, 'it' is shape number 2 (for ordering (measured from 1 to 2))
	// If the distance is smaller than 'dist', 'dist' is replaced and the return value is true
	// If the distance is bigger, 'dist' doesn't change and the return value is false

	CStatDistObj distObj(this,it);
	if (distObj.measure(dist,buffer))
	{
		ray[0]=distObj.ptOnShapeA(0);
		ray[1]=distObj.ptOnShapeA(1);
		ray[2]=distObj.ptOnShapeA(2);
		ray[3]=distObj.ptOnShapeB(0);
		ray[4]=distObj.ptOnShapeB(1);
		ray[5]=distObj.ptOnShapeB(2);
		ray[6]=dist;
		return(true);
	}
	return(false);
}

C3DObject* CShape::copyYourself()
{	
	CShape* newShape=(CShape*)copyYourselfMain();

	if (geomData!=NULL)
		newShape->geomData=geomData->copyYourself();

	// Various:
	newShape->_startInDynamicSleeping=_startInDynamicSleeping;
	newShape->_shapeIsDynamicallyStatic=_shapeIsDynamicallyStatic;
	newShape->_shapeIsDynamicallyRespondable=_shapeIsDynamicallyRespondable;
	newShape->_dynamicCollisionMask=_dynamicCollisionMask;
	newShape->_parentFollowsDynamic=_parentFollowsDynamic;
	newShape->_containsTransparentComponents=_containsTransparentComponents;
	newShape->_rigidBodyWasAlreadyPutToSleepOnce=_rigidBodyWasAlreadyPutToSleepOnce;
	newShape->_setAutomaticallyToNonStaticIfGetsParent=_setAutomaticallyToNonStaticIfGetsParent;


	newShape->_initialValuesInitialized=_initialValuesInitialized;

	return(newShape);
}

void CShape::setColor(const char* colorName,int colorComponent,const float* rgbData)
{
	if (geomData!=NULL)
		geomData->geomInfo->setColor(colorName,colorComponent,rgbData);
}

bool CShape::getColor(const char* colorName,int colorComponent,float* rgbData)
{
	if (geomData!=NULL)
		return(geomData->geomInfo->getColor(colorName,colorComponent,rgbData));
	return(false);
}
