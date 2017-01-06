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
#include "Tt.h"
#include "Shape.h"
#include "Camera.h"
#include "Graph.h"
#include "OGL.h"
#include "IloIlo.h"
#include "Path.h"
#include "OglSurface.h"
#include "CustomData.h"
#include "visionSensor.h"
#include "ShapeComponent.h"
#include "VolumeObject.h"
#include "Mill.h"
#include "Light.h"
#include "GV.h"
#include "VDateTime.h"
#include "ttUtil.h"
#include <boost/lexical_cast.hpp>
#include "App.h"
#include "easyLock.h"
#include "qdlgtranslations.h"
#include "qdlgrotations.h"
#include "jointObject.h"
#include "3DObject.h"
#include "Dummy.h"

#include "Global.h"

C3DObject::C3DObject()
{
	_parentObject=NULL;
	_parentID=-1;
	_objectID=0;
	_transformation.setIdentity();
	generateUniqueUpdatableString();
	_uniqueUpdatableString.clear();
	_assemblingLocalTransformation.setIdentity();
	_lifeID=-1;

	_assemblyMatchValueChild=0;
	_assemblyMatchValueParent=0;
	_assemblyCanHaveChildRole=true;
	_assemblyCanHaveParentRole=true;

	_objectProperty=sim_objectproperty_selectable;
	_hierarchyColorIndex=-1;
	_collectionSelfCollisionIndicator=0;

	_modelBase=false;
	_dynamicObjectFlag_forVisualization=0;

	_measuredAngularVelocity_velocityMeasurement=0.0f;
	_measuredLinearVelocity_velocityMeasurement.clear();
	_measuredAngularVelocity3_velocityMeasurement.clear();


	_dynamicsTemporarilyDisabled=false;
	_initialValuesInitializedMain=false;
	_initialConfigurationMemorized=false;
	_objectManipulationDisabledDuringNonSimulation=false;
	_objectManipulationDisabledDuringSimulation=false;
	_objectManipulationModePermissions=0x023; // about Z and in the X-Y plane!       0x03f; // full
	_objectManipulationModeEventId=-1;
	_objectManipulationTranslationRelativeTo=0; // relative to world by default
	_objectTranslationNonDefaultStepSize=0.0f; // 0.0 means: use the default
	_objectManipulationRotationRelativeTo=2; // relative to own frame by default
	_objectRotationNonDefaultStepSize=0.0f; // 0.0 means: use the default

	_customObjectData=NULL;
	_mainPropertyLocal=0;
	_mainPropertyOverride=0; // By default, the main properties are not overriden! (0 means we inherit from parents)
	_mainPropertyOverrideCombinedWithParents=0;
	_mainPropertyOverrideCombinedWithParentsValidityNumber=-2;

	_memorizedConfigurationValidCounter=0;

	_dynamicSimulationIconCode=DYNAMIC_SIMULATION_ICON_NONE;

	_uniqueID=_uniqueIDCounter++;
	_creationInfo[0]=123456;
	_creationInfo[1]=0;
	int year,month,day;
	VDateTime::getYearMonthDayHourMinuteSecond(&year,&month,&day,NULL,NULL,NULL);
	((WORD*)_creationInfo)[4]=WORD(year);
	((BYTE*)_creationInfo)[10]=BYTE(month);
	((BYTE*)_creationInfo)[11]=BYTE(day);
	_creationInfo[3]=0; //FREEEEEEEEEE!!!
	_modelAcknowledgement="";

	_sizeFactor=1.0f;
	_dynamicsFullRefreshFlag=true;
	_ignoredByViewFitting=false;
}

C3DObject::~C3DObject() 
{
	CLifeControl::announceDeath(_lifeID);
	delete _customObjectData;
}

void C3DObject::measureVelocity(float dt)
{
	C7Vector abs(getCumulativeTransformationPart1());
	_measuredLinearVelocity_velocityMeasurement=(abs.X-_previousAbsTransf_velocityMeasurement.X)*(1.0f/dt);
	_measuredAngularVelocity_velocityMeasurement=abs.Q.getAngleBetweenQuaternions(_previousAbsTransf_velocityMeasurement.Q)/dt;

// Following bug corrected on 17/9/2013:
//	_measuredAngularVelocity3_velocityMeasurement=(_previousAbsTransf_velocityMeasurement.getInverse()*abs).Q.getEulerAngles()*(1.0f/dt);
	_measuredAngularVelocity3_velocityMeasurement=(abs*_previousAbsTransf_velocityMeasurement.getInverse()).Q.getEulerAngles()*(1.0f/dt);

	_previousAbsTransf_velocityMeasurement=abs;
}

float C3DObject::getMeasuredAngularVelocity()
{
	return(_measuredAngularVelocity_velocityMeasurement);
}

C3Vector C3DObject::getMeasuredLinearVelocity()
{
	return(_measuredLinearVelocity_velocityMeasurement);
}

C3Vector C3DObject::getMeasuredAngularVelocity3()
{
	return(_measuredAngularVelocity3_velocityMeasurement);
}


void C3DObject::setHierarchyColorIndex(int c)
{
	_hierarchyColorIndex=c;
}

int C3DObject::getHierarchyColorIndex()
{
	return(_hierarchyColorIndex);
}

void C3DObject::setCollectionSelfCollisionIndicator(int c)
{
	_collectionSelfCollisionIndicator=c;
}

int C3DObject::getCollectionSelfCollisionIndicator()
{
	return(_collectionSelfCollisionIndicator);
}

void C3DObject::setDynamicObjectFlag_forVisualization(int isDynamicallySimulatedFlag)
{
	_dynamicObjectFlag_forVisualization=isDynamicallySimulatedFlag;
}

int C3DObject::getDynamicObjectFlag_forVisualization()
{
	return(_dynamicObjectFlag_forVisualization);
}

int C3DObject::getDynamicObjectFlag_forVisualization_forDisplay(bool guiIsRendering)
{
	if (guiIsRendering)
		return(_dynamicObjectFlag_forVisualization_forDisplay);
	else
		return(_dynamicObjectFlag_forVisualization);
}

bool C3DObject::getShouldObjectBeDisplayed(int displayAttrib)
{
	if (((displayAttrib&sim_displayattribute_pickpass)==0)||((_objectProperty&sim_objectproperty_selectinvisible)==0))
	{ // ok, no pickpass and select invisible
		if (displayAttrib&sim_displayattribute_dynamiccontentonly)
		{
			if ((displayAttrib&sim_displayattribute_forvisionsensor)==0)
				return(_dynamicObjectFlag_forVisualization_forDisplay!=0);
			else
				return(_dynamicObjectFlag_forVisualization!=0);
		}
		else
			return( (!isObjectPartOfInvisibleModel())&&((App::ct->mainSettings->activeLayers&layer)||(displayAttrib&sim_displayattribute_ignorelayer)) );
	}
	else
		return(false);
}

void C3DObject::setIgnoredByViewFitting(bool ignored)
{
	_ignoredByViewFitting=ignored;
}

bool C3DObject::getIgnoredByViewFitting()
{
	return(_ignoredByViewFitting);
}

void C3DObject::setModelAcknowledgement(const std::string& a)
{
	_modelAcknowledgement=a;
	if (_modelAcknowledgement.length()>3000)
		_modelAcknowledgement.erase(_modelAcknowledgement.begin()+2999,_modelAcknowledgement.end());
}

std::string C3DObject::getModelAcknowledgement()
{
	return(_modelAcknowledgement);
}

void C3DObject::setDynamicsFullRefreshFlag(bool refresh)
{
	_dynamicsFullRefreshFlag=refresh;
}

bool C3DObject::getDynamicsFullRefreshFlag()
{
	return(_dynamicsFullRefreshFlag);
}

void C3DObject::setObjectProperty(int p)
{
	// We probably can now remove following 2 lines, no?
	if ((p&0x0003)==0x0003) // we can't have those two flags set at the same time!
		p=(p|0x0003)-0x0003;

	_objectProperty=p;
}

int C3DObject::getObjectProperty()
{
	return(_objectProperty);
}

void C3DObject::setModelBase(bool m)
{ // is also called from the ungroup/divide shape routines!!
	_modelBase=m;
	_mainPropertyOverride=0; // Nothing is overridden!
	_modelAcknowledgement="";
	App::ct->objCont->incrementMainPropertyOverrideValidityNumber();
}

bool C3DObject::getModelBase()
{
	return(_modelBase);
}

void C3DObject::setSizeFactor(float f)
{
	_sizeFactor=f;
}

float C3DObject::getSizeFactor()
{
	return(_sizeFactor);
}

int C3DObject::getParentCount()
{
	if (getParent()==NULL)
		return(0);
	return(1+getParent()->getParentCount());
}

int C3DObject::getLifeID()
{
	if (_lifeID==-1)
		_lifeID=CLifeControl::getLifeID();
	return(_lifeID);
}

int C3DObject::getUniqueID()
{
	return(_uniqueID);
}

void C3DObject::incrementMemorizedConfigurationValidCounter()
{
	_memorizedConfigurationValidCounter++;
}

int C3DObject::getMemorizedConfigurationValidCounter()
{
	return(_memorizedConfigurationValidCounter);
}

void C3DObject::display(CViewableBase* renderingObject,int displayAttrib)
{
}

bool C3DObject::getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices)
{
	return(false);
}

C3DObject* C3DObject::copyYourself() 
{
	return(NULL);
}

std::string C3DObject::getObjectTypeInfo()
{
	return("");
}
std::string C3DObject::getObjectTypeInfoExtended()
{
	return("");
}

void C3DObject::setCreationInfo(DWORD info[4])
{
	for (int i=0;i<4;i++)
		_creationInfo[i]=info[i];
}

void C3DObject::getCreationInfo(DWORD info[4])
{
	for (int i=0;i<4;i++)
		info[i]=_creationInfo[i];
}

void C3DObject::setMainPropertyLocal(int prop)
{
	_mainPropertyLocal=prop;
	int allowed=0;
	if (_objectType==sim_object_shape_type)
	{
		allowed=sim_objectspecialproperty_collidable|sim_objectspecialproperty_measurable|sim_objectspecialproperty_detectable_all|sim_objectspecialproperty_renderable|sim_objectspecialproperty_pathplanning_ignored;
		if ( (!((CShape*)this)->isGrouping())&&(!((CShape*)this)->geomData->geomInfo->isPure()) )
			allowed|=sim_objectspecialproperty_cuttable;
	}
	if (_objectType==sim_object_joint_type)
		allowed=0;
	if (_objectType==sim_object_graph_type)
		allowed=sim_objectspecialproperty_renderable;
	if (_objectType==sim_object_path_type)
		allowed=sim_objectspecialproperty_renderable;
	if (_objectType==sim_object_proximitysensor_type)
		allowed=0;
	if (_objectType==sim_object_visionsensor_type)
		allowed=0;
	if (_objectType==sim_object_camera_type)
		allowed=0;
	if (_objectType==sim_object_light_type)
		allowed=0;
	if (_objectType==sim_object_mirror_type)
		allowed=sim_objectspecialproperty_renderable;
	if (_objectType==sim_object_mill_type)
		allowed=0;
	if (_objectType==sim_object_forcesensor_type)
		allowed=0;
	if (_objectType==sim_object_dummy_type)
		allowed=sim_objectspecialproperty_measurable|sim_objectspecialproperty_detectable_all|sim_objectspecialproperty_pathplanning_ignored;
	if (_objectType==sim_object_volume_type)
		allowed=sim_objectspecialproperty_detectable_all|sim_objectspecialproperty_renderable|sim_objectspecialproperty_cuttable;
	_mainPropertyLocal&=allowed;
}

int C3DObject::getMainPropertyLocal()
{
	return(_mainPropertyLocal);
}

void C3DObject::setMainPropertyOverride(int prop)
{
	_mainPropertyOverride=prop;
	App::ct->objCont->incrementMainPropertyOverrideValidityNumber();
}

int C3DObject::getMainPropertyOverride()
{
	return(_mainPropertyOverride);
}

int C3DObject::getCumulativeMainPropertyOverride()
{
	// 1. Check if the _mainPropertyOverrideCombinedWithParentsValidityNumber is up-to-date:
	int vn=App::ct->objCont->getMainPropertyOverrideValidityNumber();
	if (vn!=_mainPropertyOverrideCombinedWithParentsValidityNumber)
	{ // no
		if (getParent()==NULL)
			_mainPropertyOverrideCombinedWithParents=_mainPropertyOverride;
		else
		{
			int parentCumul=getParent()->getCumulativeMainPropertyOverride();
			if ( (parentCumul&sim_modelproperty_not_respondable)&&((parentCumul&sim_modelproperty_not_dynamic)==0) )
				parentCumul-=sim_modelproperty_not_respondable; // this is a special condition that again fully enables the dynamics
			if ( (_mainPropertyOverride&sim_modelproperty_not_respondable)&&((_mainPropertyOverride&sim_modelproperty_not_dynamic)==0) )
				_mainPropertyOverride-=sim_modelproperty_not_respondable; // this is a special condition that again fully enables the dynamics
			_mainPropertyOverrideCombinedWithParents=_mainPropertyOverride|parentCumul;
		}
		if (_dynamicsTemporarilyDisabled)
			_mainPropertyOverrideCombinedWithParents|=sim_modelproperty_not_dynamic;

		_mainPropertyOverrideCombinedWithParentsValidityNumber=vn;
	}
	return(_mainPropertyOverrideCombinedWithParents);
}

int C3DObject::getMainProperty()
{ // returns the total
	int o=getCumulativeMainPropertyOverride();
	if (o==0)
		return(_mainPropertyLocal); // nothing is overridden!
	int p=_mainPropertyLocal;
	if (o&sim_modelproperty_not_collidable)
		p=(p|sim_objectspecialproperty_collidable)-sim_objectspecialproperty_collidable;
	if (o&sim_modelproperty_not_measurable)
		p=(p|sim_objectspecialproperty_measurable)-sim_objectspecialproperty_measurable;
	if (o&sim_modelproperty_not_renderable)
		p=(p|sim_objectspecialproperty_renderable)-sim_objectspecialproperty_renderable;
	if (o&sim_modelproperty_not_detectable)
		p=(p|sim_objectspecialproperty_detectable_all)-sim_objectspecialproperty_detectable_all;
	if (o&sim_modelproperty_not_cuttable)
		p=(p|sim_objectspecialproperty_cuttable)-sim_objectspecialproperty_cuttable;

	return(p);
}

bool C3DObject::isObjectPartOfInvisibleModel()
{
	return((getCumulativeMainPropertyOverride()&sim_modelproperty_not_visible)!=0);
}

int C3DObject::getTreeDynamicProperty() // combination of OBJECT_DYNAMIC_PROPERTY_DYNAMIC and OBJECT_DYNAMIC_PROPERTY_RESPONDABLE
{ // returns the total
	int o=getCumulativeMainPropertyOverride();
	if (o==0)
		return(OBJECT_DYNAMIC_PROPERTY_DYNAMIC|OBJECT_DYNAMIC_PROPERTY_RESPONDABLE); // nothing is overridden!
	int ret=0;
	if ((o&sim_modelproperty_not_dynamic)==0)
		ret|=OBJECT_DYNAMIC_PROPERTY_DYNAMIC;
	if ((o&sim_modelproperty_not_respondable)==0)
		ret|=OBJECT_DYNAMIC_PROPERTY_RESPONDABLE;
	return(ret);
}

void C3DObject::getChildScriptsToRun(std::vector<int>& childScriptIDs)
{
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo(getID());
	if (it!=NULL)
		childScriptIDs.push_back(it->getScriptID());
	else
	{ // we have to explore the children:
		for (int i=0;i<int(childList.size());i++)
			childList[i]->getChildScriptsToRun(childScriptIDs);
	}
}

int C3DObject::getModelSelectionID(bool firstObject)
{ // firstObject is true by default

	// Following 2 lines since 2010/10/12 so that we can override model selection with shift+ctrl down at the same time!
	if ((App::mainWindow!=NULL)&&App::mainWindow->ctrlKeyDown&&App::mainWindow->shiftKeyDown)
		return(getID());

	if (_modelBase)
	{
		if ( ((_objectProperty&sim_objectproperty_selectmodelbaseinstead)==0) )
			return(getID());
		if (getParent()==NULL)
			return(getID());
		int retV=getParent()->getModelSelectionID(false);
		if (retV==-1)
			retV=getID();
		return(retV);
	}
	else
	{
		if (getParent()==NULL)
		{
			if (firstObject)
				return(getID());
			return(-1);
		}
		int retV=getParent()->getModelSelectionID(false);
		if (retV!=-1)
			return(retV);
		if (firstObject)
			return(getID());
		return(-1);
	}
}

void C3DObject::commonStart(int displayAttrib)
{
	glPushMatrix();
	glPushAttrib(GL_POLYGON_BIT);
	performOGLTransformation(displayAttrib);
}

void C3DObject::commonFinish()
{
	glPopAttrib();
	glPopMatrix();
	ogl::lockColors(false);
	ogl::setBlending(false);
	glDisable(GL_CULL_FACE);
}

void C3DObject::performOGLTransformation(int displayAttrib) 
{
	C7Vector tr=getCumulativeTransformationPart1_forDisplay((displayAttrib&sim_displayattribute_forvisionsensor)==0);
	glTranslatef(tr.X(0),tr.X(1),tr.X(2));
	C4Vector axis=tr.Q.getAngleAndAxisNoChecking();
	glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));
}

void C3DObject::clearManipulationModeOverlayGridFlag()
{
	_objectManipulationMode_flaggedForGridOverlay=0;
}

void C3DObject::displayManipulationModeOverlayGrid(bool transparentAndOverlay)
{
	if (_objectManipulationMode_flaggedForGridOverlay==0)
		return;
	App::ct->objCont->setLightDialogRefreshFlag(); // to actualize the position and orientation dialogs!
	bool isPath=false;
	bool isPathPoints=false;
	float sizeValueForPath=0.0f;
	C3Vector localPositionOnPath;
	localPositionOnPath.clear();
	if (_objectType==sim_object_path_type)
	{
		isPath=true;
		std::vector<int> pathPointsToTakeIntoAccount;
		CPathCont* pc;
		if ( ( (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)||(App::ct->objCont->getPointerToSelectedPathPointIndices_nonEditMode()->size()!=0) )&&((_objectManipulationMode_flaggedForGridOverlay&8)==0) )

		{ // (path is in edition or path points are selected) and no rotation
			isPathPoints=true;
			if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
			{ // Path is in edition
				pc=App::ct->objCont->_editionPath;
				pathPointsToTakeIntoAccount.assign(App::ct->objCont->editModeBuffer.begin(),App::ct->objCont->editModeBuffer.end());
			}
			else
			{ // Path points are selected (but not in path edit mode)
				pc=((CPath*)this)->pathContainer;
				pathPointsToTakeIntoAccount.assign(App::ct->objCont->getPointerToSelectedPathPointIndices_nonEditMode()->begin(),App::ct->objCont->getPointerToSelectedPathPointIndices_nonEditMode()->end());
			}
		}
		else
		{ // Path is not in edition and no path points are selected
			pc=((CPath*)this)->pathContainer;
			int cnt=pc->getSimplePathPointCount();
			for (int i=0;i<cnt;i++)
				pathPointsToTakeIntoAccount.push_back(i);
		}
		C3Vector minCoord,maxCoord;
		for (int i=0;i<int(pathPointsToTakeIntoAccount.size());i++)
		{
			CSimplePathPoint* aPt=pc->getSimplePathPoint(pathPointsToTakeIntoAccount[i]);
			C3Vector c(aPt->getTransformation().X);
			if (i==0)
			{
				minCoord=c;
				maxCoord=c;
			}
			else
			{
				minCoord.keepMin(c);
				maxCoord.keepMax(c);
			}
			localPositionOnPath+=c;
		}
		if (pathPointsToTakeIntoAccount.size()!=0)
			localPositionOnPath/=float(pathPointsToTakeIntoAccount.size());
		else
			return; // Should normally never happen
		sizeValueForPath=SIM_MAX((maxCoord-minCoord).getLength()/3.0f,pc->getSquareSize()*2.0f);
	}

	C4X4Matrix tr;
	int axisInfo;
	if (_objectManipulationMode_flaggedForGridOverlay&8)
	{ // rotation.
		axisInfo=_objectManipulationMode_flaggedForGridOverlay-8;
		tr=getCumulativeTransformationPart1().getMatrix();

		if (getObjectManipulationRotationRelativeTo_currentSettings()==0) // world
			tr.M.setIdentity();
		if (getObjectManipulationRotationRelativeTo_currentSettings()==1) // parent frame
			tr.M=getParentCumulativeTransformation().getMatrix().M;
		if ((getObjectManipulationRotationRelativeTo_currentSettings()==2)||isPathPoints) // own frame
			tr.M=getCumulativeTransformationPart1().getMatrix().M;
	}
	else
	{ // translation
		axisInfo=_objectManipulationMode_flaggedForGridOverlay-16;
		tr=getCumulativeTransformationPart1().getMatrix();
		if (getObjectManipulationTranslationRelativeTo_currentSettings()==0) // world
			tr.M.setIdentity();
		if (getObjectManipulationTranslationRelativeTo_currentSettings()==1) // parent frame
			tr.M=getParentCumulativeTransformation().getMatrix().M;
		if ((getObjectManipulationTranslationRelativeTo_currentSettings()==2)||isPathPoints) // own frame
			tr.M=getCumulativeTransformationPart1().getMatrix().M;
		if (isPathPoints)
			tr.X=tr*localPositionOnPath;
	}
	// Get the average bounding box size (or model bounding box size):
	C3Vector bbMin,bbMax;
	bbMin.clear();
	bbMax.clear();
	if (_modelBase)
	{
		C7Vector ctmi;
		if (_objectManipulationMode_flaggedForGridOverlay&8)
			ctmi=tr.getInverse();
		else
			ctmi=getCumulativeTransformationPart1().getInverse(); // actually maybe tr.getInverse would even be better?

		bool b=true;
		if (!getGlobalMarkingBoundingBox(ctmi,bbMin,bbMax,b,true))
			bbMax=C3Vector(0.1f,0.1f,0.1f); // shouldn't happen!
	}
	else
	{
		if (!getMarkingBoundingBox(bbMin,bbMax))
			bbMax=C3Vector(0.1f,0.1f,0.1f); // shouldn't happen!
	}
	C3Vector bbs(bbMax-bbMin);

	float halfSize=0.0f;
	// add the average size of the bounding box (important for models)
	C3Vector bbsavg((bbMax+bbMin)*0.5f);
	if (_objectManipulationMode_flaggedForGridOverlay&8)
	{ // rotation
		// set the orientation according to the rotation axis:
		C3X3Matrix rot;
		if (axisInfo==0)
		{ // rotation around the x-axis
			rot.buildYRotation(piValD2); 
			bbsavg(1)=0.0f;
			bbsavg(2)=0.0f;
			halfSize=1.5f*SIM_MAX(bbs(1),bbs(2))/2.0f;
		}
		if (axisInfo==1)
		{ // rotation around the y-axis
			rot.buildXRotation(-piValD2); 
			bbsavg(0)=0.0f;
			bbsavg(2)=0.0f;
			halfSize=1.5f*SIM_MAX(bbs(0),bbs(2))/2.0f;
		}
		if (axisInfo==2)
		{ // rotation around the z-axis
			rot.setIdentity();
			bbsavg(0)=0.0f;
			bbsavg(1)=0.0f;
			halfSize=1.5f*SIM_MAX(bbs(0),bbs(1))/2.0f;
		}

		if (isPath)
			halfSize=sizeValueForPath;

		// adjust the z-position of the rotation center:
		tr.X+=tr.M*bbsavg;
		// now adjust the orientation
		tr.M*=rot;
	}
	else
	{
		if (!isPath)
			halfSize=1.5f*(bbs(0)+bbs(1)+bbs(2))/6.0f;
		else
			halfSize=sizeValueForPath;

		tr.X+=getCumulativeTransformationPart1().Q*bbsavg;
	}

	glPushMatrix();

	if (transparentAndOverlay)
	{
		glDisable(GL_DEPTH_TEST);
		ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}

	if (_objectManipulationMode_flaggedForGridOverlay&8)
	{ // rotation
	// Do the OGL transformation:
		glTranslatef(tr.X(0),tr.X(1),tr.X(2));
		C3X3Matrix rrot;
		if (getObjectManipulationRotationRelativeTo_currentSettings()==2) // own frame
			rrot=tr.M;
		else
		{
			rrot.buildZRotation(_objectManipulationModeTotalRotation);
			rrot=tr.M*rrot;
		}
		C4Vector axis=rrot.getQuaternion().getAngleAndAxisNoChecking();
		glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));

		float a=5.0f*piValue/180.0f-_objectManipulationModeTotalRotation;
		float oldX=cos(-_objectManipulationModeTotalRotation);
		float oldY=sin(-_objectManipulationModeTotalRotation);
		float h=halfSize*0.9f;
		float ha=halfSize*0.95f;
		int cnt=1;

		// First the flat green circle:
		glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,ogl::MANIPULATION_MODE_OVERLAY_COLOR);
		glPolygonOffset(0.5f,0.0f); // Second argument set to 0.0 on 2009.01.05 (otherwise strange effects on some graphic cards)
		glEnable(GL_POLYGON_OFFSET_FILL);
		glBegin(GL_QUADS);
		for (int i=0;i<72;i++)
		{
			glVertex3f(oldX*h,oldY*h,0.0f);
			glVertex3f(oldX*halfSize,oldY*halfSize,0.0f);
			oldX=cos(a);
			oldY=sin(a);
			a+=5.0f*piValue/180.0f;
			glVertex3f(oldX*halfSize,oldY*halfSize,0.0f);
			glVertex3f(oldX*h,oldY*h,0.0f);
		}
		glEnd();
		glDisable(GL_POLYGON_OFFSET_FILL);

		// Now the graduation:
		glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,ogl::MANIPULATION_MODE_OVERLAY_GRID_COLOR);
		a=5.0f*piValue/180.0f-_objectManipulationModeTotalRotation;
		oldX=cos(-_objectManipulationModeTotalRotation);
		oldY=sin(-_objectManipulationModeTotalRotation);
		for (int i=0;i<72;i++)
		{
			glBegin(GL_LINE_STRIP);
			glVertex3f(oldX*halfSize,oldY*halfSize,0.0f);
			oldX=cos(a);
			oldY=sin(a);
			a+=5.0f*piValue/180.0f;
			glVertex3f(oldX*halfSize,oldY*halfSize,0.0f);
			if (cnt==0)
				glVertex3f(oldX*h,oldY*h,0.0f);
			else
				glVertex3f(oldX*ha,oldY*ha,0.0f);
			cnt++;
			if (cnt==3)
				cnt=0;
			glEnd();
		}
		// Now the moving part:
		glLineWidth(3.0f);
		float h2=halfSize*0.8f;
		glBegin(GL_LINE_STRIP);
		glVertex3f(-h,0.0f,0.0f);
		glVertex3f(h,0.0f,0.0f);
		glVertex3f(cos(0.1f)*h2,sin(0.1f)*h2,0.0f);
		glEnd();
		glBegin(GL_LINES);
		glVertex3f(h,0.0f,0.0f);
		glVertex3f(cos(-0.1f)*h2,sin(-0.1f)*h2,0.0f);
		glEnd();
		std::string s(gv::getAngleStr(true,_objectManipulationModeTotalRotation));
		float h3=halfSize*1.1f;

		if (transparentAndOverlay)				
			ogl::setBlending(false);
		ogl::drawBitmapTextIntoScene(h3,0.0f,halfSize*0.05f,s.c_str());
		if (transparentAndOverlay)
			ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glBegin(GL_LINES);
		glVertex3f(0.0f,-h,0.0f);
		glVertex3f(0.0f,h,0.0f);
		glEnd();
		glLineWidth(1.0f);
	}
	else
	{ // translation
		C3X3Matrix rot;
		bool xAxisOnly=false;
		if (axisInfo==0)
		{ // y-z plane
			C3X3Matrix r1;
			r1.buildZRotation(piValD2);
			C3X3Matrix r2;
			r2.buildXRotation(piValD2);
			rot=r1*r2;
		}
		if (axisInfo==1)
			rot.buildXRotation(piValD2); // z-x plane
		if (axisInfo==2)
			rot.setIdentity(); // x-y plane
		if (axisInfo==3)
		{ // x-axis
			rot.setIdentity();
			xAxisOnly=true;
		}
		if (axisInfo==4)
		{ // y-axis
			rot.buildZRotation(-piValD2);
			xAxisOnly=true;
		}
		if (axisInfo==5)
		{ // y-axis
			rot.buildYRotation(piValD2);
			xAxisOnly=true;
		}
		tr.M*=rot;
		C3Vector totTransl(rot.getTranspose()*_objectManipulationModeTotalTranslation);
		glTranslatef(tr.X(0),tr.X(1),tr.X(2));
		C4Vector axis=tr.M.getQuaternion().getAngleAndAxisNoChecking();
		glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));

		glTranslatef(-totTransl(0),-totTransl(1),-totTransl(2));
		C3Vector dir[2]={C3Vector::unitXVector,C3Vector::unitYVector};
		C3Vector perp[2]={C3Vector::unitYVector,C3Vector::unitXVector};
		float unt=0.001f;
		float h=halfSize*2.0f;
		if (h/unt>20)
			unt=0.002f;
		if (h/unt>20)
			unt=0.005f;
		if (h/unt>20)
			unt=0.01f;
		if (h/unt>20)
			unt=0.02f;
		if (h/unt>20)
			unt=0.05f;
		if (h/unt>20)
			unt=0.1f;
		if (h/unt>20)
			unt=0.2f;
		if (h/unt>20)
			unt=0.5f;

		int grdCnt=int(h/unt);
		C3Vector v;

		// First the green bands:
		glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,ogl::MANIPULATION_MODE_OVERLAY_COLOR);
		glPolygonOffset(0.5f,0.0f); // Second argument set to 0.0 on 2009.01.05 (otherwise strange effects on some graphic cards)
		glEnable(GL_POLYGON_OFFSET_FILL);
		glBegin(GL_QUADS);
		for (int axis=0;axis<2;axis++)
		{
			v=dir[axis]*-unt*float(grdCnt+1);
			C3Vector w(perp[axis]*-unt*0.6f);
			glVertex3f(v(0)+w(0),v(1)+w(1),v(2)+w(2));
			glVertex3f(v(0)-w(0),v(1)-w(1),v(2)-w(2));
			glVertex3f(-v(0)-w(0),-v(1)-w(1),-v(2)-w(2));
			glVertex3f(-v(0)+w(0),-v(1)+w(1),-v(2)+w(2));
			if (xAxisOnly)
				break;
		}
		glEnd();
		glDisable(GL_POLYGON_OFFSET_FILL);

		// Now the graduation:
		glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,ogl::MANIPULATION_MODE_OVERLAY_GRID_COLOR);
		glBegin(GL_LINES);
		for (int axis=0;axis<2;axis++)
		{
			v=dir[axis]*-unt*float(grdCnt+1);
			glVertex3f(v(0),v(1),v(2));
			v*=-1.0f;
			glVertex3f(v(0),v(1),v(2));
			for (int i=-grdCnt;i<=grdCnt;i++)
			{
				C3Vector w(dir[axis]*-unt*float(i));
				v=perp[axis]*-unt*0.6f;
				glVertex3f(v(0)+w(0),v(1)+w(1),v(2)+w(2));
				v*=-1.0f;
				glVertex3f(v(0)+w(0),v(1)+w(1),v(2)+w(2));
			}
			if (xAxisOnly)
				break;
		}
		glEnd();

		// Now the moving part:
		glTranslatef(totTransl(0),totTransl(1),totTransl(2));
		glLineWidth(3.0f);
		for (int axis=0;axis<2;axis++)
		{
			C3Vector w,s;
			w=perp[axis]*-unt*0.8f;
			s=dir[axis]*unt*0.8f;
			v=dir[axis]*-unt*float(grdCnt+1);
			glBegin(GL_LINES);
			glVertex3f(v(0),v(1),v(2));
			v*=-1.0f;
			glVertex3f(v(0)+s(0),v(1)+s(1),v(2)+s(2));
			glVertex3f(v(0)+s(0),v(1)+s(1),v(2)+s(2));
			glVertex3f(v(0)+w(0),v(1)+w(1),v(2)+w(2));
			glVertex3f(v(0)+s(0),v(1)+s(1),v(2)+s(2));
			glVertex3f(v(0)-w(0),v(1)-w(1),v(2)-w(2));
			glEnd();
			std::string st(gv::getSizeStr(true,totTransl(axis),0));
			if (transparentAndOverlay)
				ogl::setBlending(false);
			ogl::drawBitmapTextIntoScene(v(0)+s(0)*2.0f+w(0)*2.0f,v(1)+s(1)*2.0f+w(1)*2.0f,v(2)+s(2)*2.0f+w(2)*2.0f,st.c_str());
			if (transparentAndOverlay)
				ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			if (xAxisOnly)
				break;
		}
		glLineWidth(1.0f);
	}
	glPopMatrix();

	if (transparentAndOverlay)
	{
		glEnable(GL_DEPTH_TEST);
		ogl::setBlending(false);
	}
}

bool C3DObject::setLocalTransformationFromObjectRotationMode(const C4X4Matrix& cameraAbsConf,float rotationAmount,bool perspective,int eventID)
{ // bits 0-2: position x,y,z (relative to parent frame), bits 3-5: Euler e9,e1,e2 (relative to own frame)
	if ( (!App::ct->simulation->isSimulationStopped())&&getObjectManipulationDisabledDuringSimulation_currentSettings())
	{
		_objectManipulationMode_flaggedForGridOverlay=0;
		_objectManipulationModeEventId=-1;
		return(false);
	}
	if (App::ct->simulation->isSimulationStopped()&&getObjectManipulationDisabledDuringNonSimulation_currentSettings())
	{
		_objectManipulationMode_flaggedForGridOverlay=0;
		_objectManipulationModeEventId=-1;
		return(false);
	}
	static int  otherAxisMemorized=0;
	bool ctrlKeyDown=((App::mainWindow!=NULL)&&App::mainWindow->ctrlKeyDown);
	if ( (!ctrlKeyDown)&&((getObjectManipulationModePermissions_currentSettings()&56)==0) )
	{ // This is special so that, when no manip is allowed but we held down the ctrl key and released it, the green manip disc doesn't appear
		_objectManipulationModeAxisIndex=-1;
		_objectManipulationMode_flaggedForGridOverlay=0;
		_objectManipulationModePermissionsPreviousCtrlKeyDown=ctrlKeyDown;
		return(false);
	}
	if ( (eventID!=_objectManipulationModeEventId)||(ctrlKeyDown!=_objectManipulationModePermissionsPreviousCtrlKeyDown) )
	{
		if ( (otherAxisMemorized>1)&&((getObjectManipulationModePermissions_currentSettings()&56)!=0) )
			otherAxisMemorized=0;
		if (otherAxisMemorized>2)
			otherAxisMemorized=0;
		_objectManipulationModeSubTranslation.clear();
		_objectManipulationModeSubRotation=0.0f;
		_objectManipulationModeEventId=eventID;
		_objectManipulationModeTotalTranslation.clear();
		_objectManipulationModeTotalRotation=0.0f;
		// Let's first see around which axis we wanna rotate:
		int _objectManipulationModePermissionsTEMP=getObjectManipulationModePermissions_currentSettings();
		bool specialMode=false;
		if (ctrlKeyDown)
			specialMode=true;
		C4X4Matrix objAbs(getCumulativeTransformationPart1().getMatrix());
		C3X3Matrix rotAxes;
		if (getObjectManipulationRotationRelativeTo_currentSettings()==2)
			rotAxes=objAbs.M; // own frame
		if (getObjectManipulationRotationRelativeTo_currentSettings()==1)
			rotAxes=getParentCumulativeTransformation().getMatrix().M; // parent frame
		if (getObjectManipulationRotationRelativeTo_currentSettings()==0)
			rotAxes.setIdentity(); // absolute frame

		float ml=0.0f;
		_objectManipulationModeAxisIndex=-1;
		for (int i=0;i<3;i++)
		{
			float l;
			if (perspective)
				l=(cameraAbsConf.X-objAbs.X) *rotAxes.axis[i];
			else
				l=cameraAbsConf.M.axis[2]*rotAxes.axis[i];
			if ( (fabs(l)>=ml)&&(_objectManipulationModePermissionsTEMP&(8<<i)) )
			{
				ml=fabs(l);
				_objectManipulationModeAxisIndex=i;
			}
		}
		if (specialMode)
		{
			int ax=_objectManipulationModeAxisIndex;
			if (_objectManipulationModeAxisIndex==-1)
				ax=otherAxisMemorized;
			if (_objectManipulationModeAxisIndex==0)
				ax+=1+otherAxisMemorized;
			if (_objectManipulationModeAxisIndex==1)
			{
				if (otherAxisMemorized==0)
					ax=0;
				else
					ax=2;
			}
			if (_objectManipulationModeAxisIndex==2)
				ax-=(1+otherAxisMemorized);
			_objectManipulationModeAxisIndex=ax;
			otherAxisMemorized++;
		}
	}

	_objectManipulationModePermissionsPreviousCtrlKeyDown=ctrlKeyDown;

	if (_objectManipulationModeAxisIndex==-1)
		return(false); //rotation not allowed

	float ss=getNonDefaultRotationStepSize_currentSettings();
	if (ss==0.0f)
		ss=App::userSettings->getRotationStepSize();
	if ((App::mainWindow!=NULL)&&App::mainWindow->shiftKeyDown)
	{
		ss=0.1f*degToRad;
		rotationAmount/=5.0f;
	}
	float axisEffectiveRotationAmount=0.0f;
	_objectManipulationModeSubRotation+=rotationAmount;
	float w=fmod(_objectManipulationModeSubRotation,ss);
	axisEffectiveRotationAmount=_objectManipulationModeSubRotation-w;
	_objectManipulationModeTotalRotation+=axisEffectiveRotationAmount;
	_objectManipulationModeSubRotation=w;

	C3Vector euler;
	euler.clear();
	euler(_objectManipulationModeAxisIndex)=axisEffectiveRotationAmount;
	C4Vector rot(euler);
	if (getObjectManipulationRotationRelativeTo_currentSettings()==2)
		_transformation.Q*=rot; // relative to own frame
	if (getObjectManipulationRotationRelativeTo_currentSettings()==1)
		_transformation.Q=rot*_transformation.Q; // relative to parent frame
	if (getObjectManipulationRotationRelativeTo_currentSettings()==0)
	{ // relative to world frame
		C4Vector trq(getCumulativeTransformationPart1().Q);
		trq=rot*trq;
		C4Vector pinv(getParentCumulativeTransformation().Q.getInverse());
		_transformation.Q=pinv*trq;
	}
	_objectManipulationMode_flaggedForGridOverlay=_objectManipulationModeAxisIndex+8;
	return(true);
}


bool C3DObject::setLocalTransformationFromObjectTranslationMode(const C4X4Matrix& cameraAbsConf,const C3Vector& clicked3DPoint,float prevPos[2],float pos[2],float screenHalfSizes[2],float halfSizes[2],bool perspective,int eventID)
{ // bits 0-2: position x,y,z (relative to parent frame), bits 3-5: Euler e9,e1,e2 (relative to own frame) 

	if ( (!App::ct->simulation->isSimulationStopped())&&getObjectManipulationDisabledDuringSimulation_currentSettings())
	{
		_objectManipulationMode_flaggedForGridOverlay=0;
		_objectManipulationModeEventId=-1;
		return(false);
	}
	if (App::ct->simulation->isSimulationStopped()&&getObjectManipulationDisabledDuringNonSimulation_currentSettings())
	{
		_objectManipulationMode_flaggedForGridOverlay=0;
		_objectManipulationModeEventId=-1;
		return(false);
	}

	C4X4Matrix objAbs;
	objAbs.X=getCumulativeTransformationPart1().X;
	if (getObjectManipulationTranslationRelativeTo_currentSettings()==0)
		objAbs.M.setIdentity();
	if (getObjectManipulationTranslationRelativeTo_currentSettings()==1)
		objAbs.M=getParentCumulativeTransformation().getMatrix().M;
	if (getObjectManipulationTranslationRelativeTo_currentSettings()==2)
		objAbs.M=getCumulativeTransformationPart1().getMatrix().M;
	static int  otherAxisMemorized=0;
	bool ctrlKeyDown=((App::mainWindow!=NULL)&&App::mainWindow->ctrlKeyDown);
	if ( (!ctrlKeyDown)&&((getObjectManipulationModePermissions_currentSettings()&7)==0) )
	{ // This is special so that, when no manip is allowed but we held down the ctrl key and released it, the green manip bars don't appear
		_objectManipulationModeAxisIndex=-1;
		_objectManipulationMode_flaggedForGridOverlay=0;
		_objectManipulationModePermissionsPreviousCtrlKeyDown=ctrlKeyDown;
		return(false);
	}
	if (eventID!=_objectManipulationModeEventId)
		_objectManipulationModeRelativePositionOfClickedPoint=clicked3DPoint-objAbs.X; // Added on 2010/07/29
	if ( (eventID!=_objectManipulationModeEventId)||(ctrlKeyDown!=_objectManipulationModePermissionsPreviousCtrlKeyDown) )
	{
		if (otherAxisMemorized>1)
			otherAxisMemorized=0;
		_objectManipulationModeSubTranslation.clear();
		_objectManipulationModeSubRotation=0.0f;
		_objectManipulationModeEventId=eventID;
		_objectManipulationModeTotalTranslation.clear();
		_objectManipulationModeTotalRotation=0.0f;
		// Let's first see on which plane we wanna translate:
		int _objectManipulationModePermissionsTEMP=getObjectManipulationModePermissions_currentSettings();
		bool specialMode=false;
		bool specialMode2=false;
		if (ctrlKeyDown)
		{
			if ((_objectManipulationModePermissionsTEMP&7)!=7)
			{
				_objectManipulationModePermissionsTEMP^=7;
				specialMode2=((_objectManipulationModePermissionsTEMP&7)==7);
			}
			else
				specialMode=true;
		}
		float ml=0.0f;
		_objectManipulationModeAxisIndex=-1;
		unsigned char planeComb[3]={6,5,3};
		for (int i=0;i<3;i++)
		{
			float l;
			if (perspective)
				l=(cameraAbsConf.X-objAbs.X)*objAbs.M.axis[i];
			else
				l=cameraAbsConf.M.axis[2]*objAbs.M.axis[i];
			if ( (fabs(l)>=ml)&&((_objectManipulationModePermissionsTEMP&(planeComb[i]))==planeComb[i]) )
			{
				ml=fabs(l);
				_objectManipulationModeAxisIndex=i;
			}
		}
		if (_objectManipulationModeAxisIndex==-1)
		{ // maybe we are constrained to stay on a line?
			for (int i=0;i<3;i++)
			{
				if (_objectManipulationModePermissionsTEMP&(1<<i))
					_objectManipulationModeAxisIndex=3+i;
			}
		}
		else
		{
			if (specialMode)
				_objectManipulationModeAxisIndex+=3;
		}
		if (specialMode2)
		{
			if (otherAxisMemorized!=0)
				_objectManipulationModeAxisIndex+=3;
			otherAxisMemorized++;
		}
	}

	_objectManipulationModePermissionsPreviousCtrlKeyDown=ctrlKeyDown;

	if (_objectManipulationModeAxisIndex==-1)
		return(false); //rotation not allowed
	C4X4Matrix originalPlane(objAbs); // x-y plane
	originalPlane.X+=_objectManipulationModeRelativePositionOfClickedPoint; // Added on 2010/07/29
	if (_objectManipulationModeAxisIndex==0)
	{ // y-z plane
		C3X3Matrix rot;
		rot.buildYRotation(piValD2);
		originalPlane.M*=rot; 
	}
	if (_objectManipulationModeAxisIndex==1)
	{ // z-x plane
		C3X3Matrix rot;
		rot.buildXRotation(piValD2);
		originalPlane.M*=rot; 
	}
	bool projectOntoXAxis=false;
	if (_objectManipulationModeAxisIndex==3)
	{ // x axis
		projectOntoXAxis=true;
	}
	if (_objectManipulationModeAxisIndex==4)
	{ // y axis
		projectOntoXAxis=true;
		C3X3Matrix rot;
		rot.buildZRotation(piValD2);
		originalPlane.M*=rot; 
	}
	if (_objectManipulationModeAxisIndex==5)
	{ // z axis
		projectOntoXAxis=true;
		C3X3Matrix rot;
		rot.buildYRotation(piValD2);
		originalPlane.M*=rot; 
	}

	C4X4Matrix plane(originalPlane);
	C3Vector p[2]; // previous and current point on the plane
	float d=-(plane.X*plane.M.axis[2]);
	float screenP[2]={prevPos[0],prevPos[1]};
	C4X4Matrix cam(cameraAbsConf);
	bool singularityProblem=false;

	for (int pass=0;pass<2;pass++)
	{
		float tt[2];
		for (int i=0;i<2;i++)
		{
			if (i==1)
			{
				screenP[0]=pos[0];
				screenP[1]=pos[1];
			}
			C3Vector pp(cam.X);
			if (!perspective)
			{
				if (fabs(cam.M.axis[2]*plane.M.axis[2])<0.05f)
				{
					singularityProblem=true;
					break;
				}
				pp-=cam.M.axis[0]*halfSizes[0]*(screenP[0]/screenHalfSizes[0]);
				pp+=cam.M.axis[1]*halfSizes[1]*(screenP[1]/screenHalfSizes[1]);
				float t=(-d-(plane.M.axis[2]*pp))/(cam.M.axis[2]*plane.M.axis[2]);
				p[i]=pp+cam.M.axis[2]*t;
			}
			else
			{
				C3Vector v(cam.M.axis[2]+cam.M.axis[0]*tan(-screenP[0])+cam.M.axis[1]*tan(screenP[1]));
				v.normalize();
				pp+=v;
				if (fabs(v*plane.M.axis[2])<0.05f)
				{
					singularityProblem=true;
					break;
				}
				float t=(-d-(plane.M.axis[2]*pp))/(v*plane.M.axis[2]);
				tt[i]=t;
				p[i]=pp+v*t;
			}
		}
		if (!singularityProblem)
		{
			if ((!perspective)||(tt[0]*tt[1]>0.0f))
				break;
			singularityProblem=true;
		}
		plane.M=cam.M;
	}
	if (projectOntoXAxis)
	{
		C4X4Matrix inv(originalPlane.getInverse());
		p[0]*=inv;
		p[1]*=inv;
		p[0](1)=0.0f;
		p[0](2)=0.0f;
		p[1](1)=0.0f;
		p[1](2)=0.0f;
		p[0]*=originalPlane;
		p[1]*=originalPlane;
	}
	else
	{
		if (singularityProblem)
		{ // we have to project the coordinates onto the original plane:
			C4X4Matrix inv(originalPlane.getInverse());
			p[0]*=inv;
			p[1]*=inv;
			p[0](2)=0.0f;
			p[1](2)=0.0f;
			p[0]*=originalPlane;
			p[1]*=originalPlane;
		}
	}

	// We snap the translation!
	C3Vector v(p[1]-p[0]);
	v=objAbs.getInverse().M*v;
	_objectManipulationModeSubTranslation+=v;
	for (int i=0;i<3;i++)
	{
		float ss=getNonDefaultTranslationStepSize_currentSettings();
		if (ss==0.0f)
			ss=App::userSettings->getTranslationStepSize();
		if ((App::mainWindow!=NULL)&&App::mainWindow->shiftKeyDown)
			ss=0.001f;
		float w=fmod(_objectManipulationModeSubTranslation(i),ss);
		v(i)=_objectManipulationModeSubTranslation(i)-w;
		_objectManipulationModeTotalTranslation(i)+=_objectManipulationModeSubTranslation(i)-w;
		_objectManipulationModeSubTranslation(i)=w;
	}
	v=objAbs.M*v;
	C4X4Matrix m(getCumulativeTransformationPart1());
	m.X+=v;
	_transformation=getParentCumulativeTransformation().getInverse().getMatrix()*m;
	_objectManipulationMode_flaggedForGridOverlay=_objectManipulationModeAxisIndex+16;
	return(true);
}

void C3DObject::scaleObject(float scalingFactor)
{
}

void C3DObject::scaleObjectMain(float scalingFactor)
{
	_sizeFactor*=scalingFactor;
	incrementMemorizedConfigurationValidCounter();
	App::ct->drawingCont->adjustForScaling(_objectID,scalingFactor,scalingFactor,scalingFactor);
	App::ct->setModificationFlag(256); // object scaled
}

void C3DObject::scalePosition(float scalingFactor) 
{ // This routine will scale an object's position. The object itself keeps the same size.
	C7Vector local(getLocalTransformationPart1());
	setLocalTransformation(local.X*scalingFactor);
	_assemblingLocalTransformation.X=_assemblingLocalTransformation.X*scalingFactor;
}

bool C3DObject::getGlobalMarkingBoundingBox(const C7Vector& baseCoordInv,C3Vector& min,C3Vector& max,bool& minMaxNotYetDefined,bool guiIsRendering)
{ // For root selection display! Return value false means there is no global marking bounding box and min/max values are not valid
	bool retVal=false;
	if ((_objectProperty&sim_objectproperty_dontshowasinsidemodel)==0)
	{
		C3Vector smi,sma;
		if (getMarkingBoundingBox(smi,sma))
		{
			retVal=true;
			C3Vector sm;
			C7Vector ctm;
			ctm=getCumulativeTransformationPart1_forDisplay(guiIsRendering);
			ctm=baseCoordInv*ctm;
			for (int i=0;i<2;i++)
			{
				sm(0)=smi(0)*i+sma(0)*(1-i);
				for (int j=0;j<2;j++)
				{
					sm(1)=smi(1)*j+sma(1)*(1-j);
					for (int k=0;k<2;k++)
					{
						sm(2)=smi(2)*k+sma(2)*(1-k);
						if (minMaxNotYetDefined)
						{
							max=ctm*sm;
							min=max;
							minMaxNotYetDefined=false;
						}
						else
						{
							max.keepMax(ctm*sm);
							min.keepMin(ctm*sm);
						}
					}
				}
			}
		}
	}
	for (int i=0;i<int(childList.size());i++)
	{
		if (childList[i]->getGlobalMarkingBoundingBox(baseCoordInv,min,max,minMaxNotYetDefined,guiIsRendering))
			retVal=true;
	}
	return(retVal);
}

void C3DObject::getBoundingBoxEncompassingBoundingBox(const C7Vector& baseCoordInv,C3Vector& min,C3Vector& max,bool guiIsRendering)
{
	C3Vector smi,sma;
	getFullBoundingBox(smi,sma);
	bool minMaxNotYetDefined=true;
	C3Vector sm;
	C7Vector ctm;
	ctm=getCumulativeTransformationPart1_forDisplay(guiIsRendering);
	ctm=baseCoordInv*ctm;
	for (int i=0;i<2;i++)
	{
		sm(0)=smi(0)*i+sma(0)*(1-i);
		for (int j=0;j<2;j++)
		{
			sm(1)=smi(1)*j+sma(1)*(1-j);
			for (int k=0;k<2;k++)
			{
				sm(2)=smi(2)*k+sma(2)*(1-k);
				if (minMaxNotYetDefined)
				{
					max=ctm*sm;
					min=max;
					minMaxNotYetDefined=false;
				}
				else
				{
					max.keepMax(ctm*sm);
					min.keepMin(ctm*sm);
				}
			}
		}
	}
}

void C3DObject::displayBoundingBox(int displayAttrib,bool displRef,float refSize)
{	// displRef is true by default, refSize is 0.0f by default
	if ((displayAttrib&sim_displayattribute_selected)==0)
		return;
	if (!App::userSettings->displayBoundingBoxeWhenObjectSelected)
		return;
	C3Vector bbMin,bbMax;
	bbMin.clear();
	bbMax.clear();
	if (_modelBase)
	{
		C7Vector ctmi(getCumulativeTransformationPart1_forDisplay((displayAttrib&sim_displayattribute_forvisionsensor)==0).getInverse());
		bool b=true;
		if (!getGlobalMarkingBoundingBox(ctmi,bbMin,bbMax,b,(displayAttrib&sim_displayattribute_forvisionsensor)==0))
			return; // no boundingbox to display!
		glLineStipple(1,0x0F0F);
		glLineWidth(2.0f);
		glEnable(GL_LINE_STIPPLE);
	}
	else
	{
		if (!getMarkingBoundingBox(bbMin,bbMax))
			return; // No bounding box to display!
	}
	C3Vector bbs(bbMax-bbMin);
	// Bounding box is 4% bigger:
	C3Vector dx(bbs(0)*0.02f,bbs(1)*0.02f,bbs(2)*0.02f);
	bool avail=true;
	if ((displayAttrib&sim_displayattribute_mainselection)&&avail)
	{
		ogl::setColorsAllBlack();
		ogl::setColor(1.0f,1.0f,1.0f,EMISSION_MODE);
		avail=false;
	}
	if (((displayAttrib&sim_displayattribute_mainselection)==0)&&avail)
	{
		ogl::setColorsAllBlack();
		ogl::setColor(1.0f,1.0f,0.0f,EMISSION_MODE);
		avail=false;
	}

	if (!avail)
	{
		if (CViewableBase::fogWasActivated)
			glDisable(GL_FOG);
		glBegin(GL_LINE_STRIP);
			glVertex3f(bbMin(0)-dx(0),bbMin(1)-dx(1),bbMax(2)+dx(2));
			glVertex3f(bbMin(0)-dx(0),bbMin(1)-dx(1),bbMin(2)-dx(2));
			glVertex3f(bbMax(0)+dx(0),bbMin(1)-dx(1),bbMin(2)-dx(2));
			glVertex3f(bbMax(0)+dx(0),bbMin(1)-dx(1),bbMax(2)+dx(2));
			glVertex3f(bbMin(0)-dx(0),bbMin(1)-dx(1),bbMax(2)+dx(2));
			glVertex3f(bbMin(0)-dx(0),bbMax(1)+dx(1),bbMax(2)+dx(2));
			glVertex3f(bbMax(0)+dx(0),bbMax(1)+dx(1),bbMax(2)+dx(2));
			glVertex3f(bbMax(0)+dx(0),bbMax(1)+dx(1),bbMin(2)-dx(2));
			glVertex3f(bbMin(0)-dx(0),bbMax(1)+dx(1),bbMin(2)-dx(2));
			glVertex3f(bbMin(0)-dx(0),bbMax(1)+dx(1),bbMax(2)+dx(2));
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(bbMin(0)-dx(0),bbMin(1)-dx(1),bbMin(2)-dx(2));
			glVertex3f(bbMin(0)-dx(0),bbMax(1)+dx(1),bbMin(2)-dx(2));
			glVertex3f(bbMax(0)+dx(0),bbMin(1)-dx(1),bbMin(2)-dx(2));
			glVertex3f(bbMax(0)+dx(0),bbMax(1)+dx(1),bbMin(2)-dx(2));
			glVertex3f(bbMax(0)+dx(0),bbMin(1)-dx(1),bbMax(2)+dx(2));
			glVertex3f(bbMax(0)+dx(0),bbMax(1)+dx(1),bbMax(2)+dx(2));
		glEnd();
		if (CViewableBase::fogWasActivated)
			glEnable(GL_FOG);
		if (true)
		{
			C4Vector r(getCumulativeTransformation_forDisplay((displayAttrib&sim_displayattribute_forvisionsensor)==0).Q);
			C3Vector absV;
			float maxH=0.0f;
			int highestIndex[3];
			for (int i=0;i<2;i++)
			{
				if (i==0)
					absV(0)=bbMin(0);
				else
					absV(0)=bbMax(0);
				for (int j=0;j<2;j++)
				{
					if (j==0)
						absV(1)=bbMin(1);
					else
						absV(1)=bbMax(1);
					for (int k=0;k<2;k++)
					{
						if (k==0)
							absV(2)=bbMin(2);
						else
							absV(2)=bbMax(2);
						float h=(r*absV)(2);
						if (h>(maxH+0.001f)) // added 0.001f to avoid that the label jumps when the box is aligned with the x/y plane
						{
							maxH=h;
							highestIndex[0]=i;
							highestIndex[1]=j;
							highestIndex[2]=k;
						}
					}
				}
			}
			
			C3Vector corner;
			C3Vector corner2;
			for (int i=0;i<3;i++)
			{
				if (highestIndex[i]==0)
					corner(i)=bbMin(i)-dx(i);
				else
					corner(i)=bbMax(i)+dx(i);
				corner2(i)=corner(i)*(1.1f+0.15f*float((_objectID>>((2-i)*2))%4));
			}
			if (CViewableBase::fogWasActivated)
				glDisable(GL_FOG);
			glBegin(GL_LINES);
				glVertex3f(corner(0),corner(1),corner(2));
				glVertex3f(corner2(0),corner2(1),corner2(2));
			glEnd();
			ogl::drawBitmapTextIntoScene(corner2(0),corner2(1),corner2(2),getName());
			if (CViewableBase::fogWasActivated)
				glEnable(GL_FOG);
		}
		glLineWidth(1.0f);
		if (displRef) 
			drawReference(refSize);
	}

	glDisable(GL_LINE_STIPPLE);
	avail=true;
	if ((displayAttrib&sim_displayattribute_groupselection)&&avail)
	{
		App::userSettings->groupSelectionColor.makeCurrentColor();
		avail=false;
	}
	if (!avail)
		ogl::lockColors(true);
}

void C3DObject::drawReference(float refSize)
{	// refSize is 0.0f by default --> size depends on the bounding box
	float s;	
	if (refSize!=0.0f) 
		s=refSize;
	else
	{
		C3Vector minV,maxV;
		if (!getMarkingBoundingBox(minV,maxV))
			return; // no bounding box --> no reference!
		maxV-=minV;
		s=(maxV(0)+maxV(1)+maxV(2))/4.0f;	
	}
	glPushMatrix();
	ogl::drawReference(s);
	glPopMatrix();
}

void C3DObject::announceGroupWillBeErased(int groupID,bool copyBuffer)
{
}
void C3DObject::announceCollisionWillBeErased(int collisionID,bool copyBuffer)
{
}
void C3DObject::announceDistanceWillBeErased(int distanceID,bool copyBuffer)
{
}
void C3DObject::announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer)
{
}

void C3DObject::performIkLoadingMapping(std::vector<int>* map)
{
}
void C3DObject::performGroupLoadingMapping(std::vector<int>* map)
{
}
void C3DObject::performCollisionLoadingMapping(std::vector<int>* map)
{
}
void C3DObject::performDistanceLoadingMapping(std::vector<int>* map)
{
}
void C3DObject::performGcsLoadingMapping(std::vector<int>* map)
{
}

void C3DObject::performTextureObjectLoadingMapping(std::vector<int>* map)
{
}

void C3DObject::performDynMaterialObjectLoadingMapping(std::vector<int>* map)
{
}

void C3DObject::bufferMainDisplayStateVariables()
{
}

void C3DObject::bufferedMainDisplayStateVariablesToDisplay()
{
}

void C3DObject::bufferMainDisplayStateVariablesMain()
{
	_transformation_buffered=_transformation;
	_dynamicObjectFlag_forVisualization_buffered=_dynamicObjectFlag_forVisualization;
}

void C3DObject::bufferedMainDisplayStateVariablesToDisplayMain()
{
	_transformation_forDisplay=_transformation_buffered;
	_dynamicObjectFlag_forVisualization_forDisplay=_dynamicObjectFlag_forVisualization_buffered;
}

void C3DObject::announceGroupWillBeErasedMain(int groupID,bool copyBuffer)
{
	// This routine can be called for objCont-objects, but also for objects
	// in the copy-buffer!! So never make use of any 
	// 'ct::objCont->getObject(id)'-call or similar
}
void C3DObject::announceCollisionWillBeErasedMain(int collisionID,bool copyBuffer)
{
	// This routine can be called for objCont-objects, but also for objects
	// in the copy-buffer!! So never make use of any 
	// 'ct::objCont->getObject(id)'-call or similar
}
void C3DObject::announceDistanceWillBeErasedMain(int distanceID,bool copyBuffer)
{
	// This routine can be called for objCont-objects, but also for objects
	// in the copy-buffer!! So never make use of any 
	// 'ct::objCont->getObject(id)'-call or similar
}
void C3DObject::announceGcsObjectWillBeErasedMain(int gcsObjectID,bool copyBuffer)
{
	// This routine can be called for objCont-objects, but also for objects
	// in the copy-buffer!! So never make use of any 
	// 'ct::objCont->getObject(id)'-call or similar
}

void C3DObject::performIkLoadingMappingMain(std::vector<int>* map)
{
}
void C3DObject::performGroupLoadingMappingMain(std::vector<int>* map)
{
}
void C3DObject::performCollisionLoadingMappingMain(std::vector<int>* map)
{
}
void C3DObject::performDistanceLoadingMappingMain(std::vector<int>* map)
{
}
void C3DObject::performGcsLoadingMappingMain(std::vector<int>* map)
{
}
void C3DObject::performTextureObjectLoadingMappingMain(std::vector<int>* map)
{
}
void C3DObject::performDynMaterialObjectLoadingMappingMain(std::vector<int>* map)
{
}

void C3DObject::getAllShapeObjectsRecursive(std::vector<CShape*>* shapeList,bool baseIncluded,bool start)
{	// baseIncluded and start are true by default.
	// Returns all shape-objects build on this (including children of children of...)
	if ( (baseIncluded||(!start))&&(getObjectType()==sim_object_shape_type) )
		shapeList->push_back((CShape*)this);
	for (int i=0;i<int(childList.size());i++)
		childList[i]->getAllShapeObjectsRecursive(shapeList,true,false);
}

void C3DObject::getAllObjectsRecursive(std::vector<C3DObject*>* objectList,bool baseIncluded,bool start)
{	// baseIncluded and start are true by default.
	// Returns all objects build on this (including children of children of...)
	if (baseIncluded||(!start))
		objectList->push_back(this);
	for (int i=0;i<int(childList.size());i++)
		childList[i]->getAllObjectsRecursive(objectList,true,false);
}

void C3DObject::getChain(std::vector<C3DObject*>& objectList,bool tipIncluded,bool start)
{	// tipIncluded and start are true by default.
	// Returns the chain with this object as tip
	if (tipIncluded||(!start))
		objectList.push_back(this);
	if (getParent()!=NULL)
		getParent()->getChain(objectList,true,false);
}

void C3DObject::setObjectType(int theNewType)
{ // Be very careful with this function!!
	_objectType=theNewType;
}
C3DObject* C3DObject::getFirstParentInSelection(std::vector<C3DObject*>* sel)
{
	C3DObject* it=getParent();
	if (it==NULL) 
		return(NULL);
	for (int i=0;i<int(sel->size());i++)
	{
		if (sel->at(i)==it) 
			return(it);
	}
	return(it->getFirstParentInSelection(sel));
}

C3DObject* C3DObject::copyYourselfMain()
{ // This routine should be called in the very first line of function
  // "copyYourself" in every joint, camera, etc. !

	C3DObject* theNewObject=NULL;
	if (getObjectType()==sim_object_shape_type)
		theNewObject=new CShape();
	if (getObjectType()==sim_object_joint_type)
		theNewObject=new CJoint();
	if (getObjectType()==sim_object_graph_type)
		theNewObject=new CGraph();
	if (getObjectType()==sim_object_dummy_type)
		theNewObject=new CDummy();
	if (getObjectType()==sim_object_proximitysensor_type)
		theNewObject=new CProxSensor();
	if (getObjectType()==sim_object_camera_type)
		theNewObject=new CCamera();
	if (getObjectType()==sim_object_light_type)
		theNewObject=new CLight(((CLight*)this)->getLightType());
	if (getObjectType()==sim_object_path_type)
		theNewObject=new CPath();
	if (getObjectType()==sim_object_mirror_type)
		theNewObject=new CMirror();
	if (getObjectType()==sim_object_visionsensor_type)
		theNewObject=new CVisionSensor();
	if (getObjectType()==sim_object_volume_type)
		theNewObject=new CVolumeObject();
	if (getObjectType()==sim_object_mill_type)
		theNewObject=new CMill();
	if (getObjectType()==sim_object_forcesensor_type)
		theNewObject=new CForceSensor();
	
	theNewObject->layer=layer;
	theNewObject->_transformation=_transformation;
	theNewObject->_objectID=_objectID;
	theNewObject->_objectName=_objectName;
	theNewObject->_objectProperty=_objectProperty;
	theNewObject->_hierarchyColorIndex=_hierarchyColorIndex;
	theNewObject->_collectionSelfCollisionIndicator=_collectionSelfCollisionIndicator;
	theNewObject->_modelBase=_modelBase;
	theNewObject->_parentID=_parentID;
	theNewObject->_objectType=_objectType;
	theNewObject->_mainPropertyLocal=_mainPropertyLocal;
	theNewObject->_mainPropertyOverride=_mainPropertyOverride;

	if (_objectProperty&sim_objectproperty_canupdatedna)
		theNewObject->_uniqueUpdatableString=_uniqueUpdatableString;

	theNewObject->_assemblingLocalTransformation=_assemblingLocalTransformation;
//	theNewObject->_useSpecialLocalTransformationWhenAssembling=_useSpecialLocalTransformationWhenAssembling;

	theNewObject->_assemblyMatchValueChild=_assemblyMatchValueChild;
	theNewObject->_assemblyMatchValueParent=_assemblyMatchValueParent;
	theNewObject->_assemblyCanHaveChildRole=_assemblyCanHaveChildRole;
	theNewObject->_assemblyCanHaveParentRole=_assemblyCanHaveParentRole;

	theNewObject->_objectManipulationDisabledDuringNonSimulation=_objectManipulationDisabledDuringNonSimulation;
	theNewObject->_objectManipulationDisabledDuringSimulation=_objectManipulationDisabledDuringSimulation;
	theNewObject->_objectManipulationModePermissions=_objectManipulationModePermissions;
	theNewObject->_objectManipulationTranslationRelativeTo=_objectManipulationTranslationRelativeTo;
	theNewObject->_objectManipulationRotationRelativeTo=_objectManipulationRotationRelativeTo;
	theNewObject->_objectTranslationNonDefaultStepSize=_objectTranslationNonDefaultStepSize;
	theNewObject->_objectRotationNonDefaultStepSize=_objectRotationNonDefaultStepSize;

	theNewObject->_measuredAngularVelocity_velocityMeasurement=_measuredAngularVelocity_velocityMeasurement;
	theNewObject->_measuredAngularVelocity3_velocityMeasurement=_measuredAngularVelocity3_velocityMeasurement;
	theNewObject->_measuredLinearVelocity_velocityMeasurement=_measuredLinearVelocity_velocityMeasurement;
	theNewObject->_previousAbsTransf_velocityMeasurement=_previousAbsTransf_velocityMeasurement;

	theNewObject->_sizeFactor=_sizeFactor;
	theNewObject->_creationInfo[0]=_creationInfo[0];
	theNewObject->_creationInfo[1]=_creationInfo[1];
	theNewObject->_creationInfo[2]=_creationInfo[2];
	theNewObject->_creationInfo[3]=_creationInfo[3];
	delete theNewObject->_customObjectData;
	theNewObject->_customObjectData=NULL;
	theNewObject->_modelAcknowledgement=_modelAcknowledgement;
	theNewObject->_ignoredByViewFitting=_ignoredByViewFitting;

	if (_customObjectData!=NULL)
		theNewObject->_customObjectData=_customObjectData->copyYourself();

	// The following line is important to properly copy the copy buffer!!!
	theNewObject->setParentIdentifierLoading(getParentIdentifierLoading());

	return(theNewObject);
}

void C3DObject::setObjectCustomDataObject(CCustomData* theData)
{
	delete _customObjectData;
	_customObjectData=theData;	
}

void C3DObject::setObjectCustomData(int header,const char* data,int dataLength)
{
	if (_customObjectData==NULL)
		_customObjectData=new CCustomData();
	_customObjectData->setData(header,data,dataLength);
}
int C3DObject::getObjectCustomDataLength(int header)
{
	if (_customObjectData==NULL)
		return(0);
	return(_customObjectData->getDataLength(header));
}
void C3DObject::getObjectCustomData(int header,char* data)
{
	if (_customObjectData==NULL)
		return;
	_customObjectData->getData(header,data);
}

bool C3DObject::getObjectCustomDataHeader(int index,int& header)
{
	if (_customObjectData==NULL)
		return(false);
	return(_customObjectData->getHeader(index,header));
}

void C3DObject::setObjectManipulationDisabledDuringSimulation(bool d)
{
	_objectManipulationDisabledDuringSimulation=d;
}

bool C3DObject::getObjectManipulationDisabledDuringSimulation()
{
	return(_objectManipulationDisabledDuringSimulation);
}

void C3DObject::setObjectManipulationDisabledDuringNonSimulation(bool d)
{
	_objectManipulationDisabledDuringNonSimulation=d;
}

bool C3DObject::getObjectManipulationDisabledDuringNonSimulation()
{
	return(_objectManipulationDisabledDuringNonSimulation);
}

void C3DObject::setObjectManipulationModePermissions(int p)
{ // bits 0-2: position x,y,z (relative to parent frame), bits 3-5: Euler e9,e1,e2 (relative to own frame) 
	p&=0x3f;
	_objectManipulationModePermissions=p;
}

int C3DObject::getObjectManipulationModePermissions()
{ // bits 0-2: position x,y,z (relative to parent frame), bits 3-5: Euler e9,e1,e2 (relative to own frame) 
	return(_objectManipulationModePermissions);
}

void C3DObject::setObjectManipulationTranslationRelativeTo(int p)
{
	_objectManipulationTranslationRelativeTo=p;
}

int C3DObject::getObjectManipulationTranslationRelativeTo()
{
	return(_objectManipulationTranslationRelativeTo);
}

void C3DObject::setObjectManipulationRotationRelativeTo(int p)
{
	_objectManipulationRotationRelativeTo=p;
}

int C3DObject::getObjectManipulationRotationRelativeTo()
{
	return(_objectManipulationRotationRelativeTo);
}

void C3DObject::setNonDefaultTranslationStepSize(float s)
{
	if (s<0.0005)
	{
		_objectTranslationNonDefaultStepSize=0.0f; //default
		return;
	}
	float sc=1.0f;
	if ((s>=0.0075f)&&(s<0.075f))
		sc=10.0f;
	if (s>=0.075f)
		sc=100.0f;
	if (s<0.0015f*sc)
		s=0.001f*sc;
	else
	{
		if (s<0.00375f*sc)
		{
			if (sc<2.0f)
				s=0.002f*sc;
			else
				s=0.0025f*sc;
		}
		else
			s=0.005f*sc;
	}
	_objectTranslationNonDefaultStepSize=s;
}

float C3DObject::getNonDefaultTranslationStepSize()
{
	return(_objectTranslationNonDefaultStepSize);
}

void C3DObject::setNonDefaultRotationStepSize(float s)
{
	if (s<0.05f*degToRad)
	{
		_objectRotationNonDefaultStepSize=0.0f; //default
		return;
	}
	if (s<1.5f*degToRad)
		s=1.0f*degToRad;
	else
	{
		if (s<3.5f*degToRad)
			s=2.0f*degToRad;
		else
		{
			if (s<7.5f*degToRad)
				s=5.0f*degToRad;
			else
			{
				if (s<12.5f*degToRad)
					s=10.0f*degToRad;
				else
				{
					if (s<22.5f*degToRad)
						s=15.0f*degToRad;
					else
					{
						if (s<37.5f*degToRad)
							s=30.0f*degToRad;
						else
							s=45.0f*degToRad;
					}
				}
			}
		}
	}
	_objectRotationNonDefaultStepSize=s;
}

float C3DObject::getNonDefaultRotationStepSize()
{
	return(_objectRotationNonDefaultStepSize);
}

void C3DObject::setMechanismID(int id)
{
	_mechanismID=id;
}

int C3DObject::getMechanismID()
{ // Make sure setMechanismID was called just before!!
	return (_mechanismID);
}

void C3DObject::simulationAboutToStart()
{ // not called, see the ...Main routine instead
}

void C3DObject::simulationEnded()
{ // not called, see the ...Main routine instead
}

void C3DObject::initializeInitialValues(bool simulationIsRunning)
{ // not called, see the ...Main routine instead
}

void C3DObject::initializeInitialValuesMain(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	_dynamicSimulationIconCode=DYNAMIC_SIMULATION_ICON_NONE;
	_initialValuesInitializedMain=simulationIsRunning;
	_mainPropertyOverride=(_mainPropertyOverride|sim_modelproperty_not_reset)-sim_modelproperty_not_reset;
	App::ct->objCont->incrementMainPropertyOverrideValidityNumber();
	_dynamicObjectFlag_forVisualization=0;

	_measuredAngularVelocity_velocityMeasurement=0.0f;
	_measuredAngularVelocity3_velocityMeasurement.clear();
	_measuredLinearVelocity_velocityMeasurement.clear();
	_previousAbsTransf_velocityMeasurement=getCumulativeTransformationPart1();;

	if (simulationIsRunning)
	{
		// this section is special and reserved to local configuration restoration!
		//********************************
		_initialConfigurationMemorized=true;
		_initialMemorizedConfigurationValidCounter=_memorizedConfigurationValidCounter;
		_initialParentUniqueID=-1; // -1 means there was no parent at begin
		C3DObject* p=getParent();
		if (p!=NULL)
			_initialParentUniqueID=p->getUniqueID();
		_initialLocalTransformationPart1=_transformation;
		//********************************

		_initialMainPropertyOverride=_mainPropertyOverride;
	}
}

void C3DObject::simulationEndedMain()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	_dynamicSimulationIconCode=DYNAMIC_SIMULATION_ICON_NONE;
	_dynamicObjectFlag_forVisualization=0;
	if (_initialValuesInitializedMain&&App::ct->simulation->getResetSceneAtSimulationEnd()&&((getCumulativeMainPropertyOverride()&sim_modelproperty_not_reset)==0))
	{
		if (_initialConfigurationMemorized)
		{ // this section is special and reserved to local configuration restoration!
			if (_initialMemorizedConfigurationValidCounter==_memorizedConfigurationValidCounter)
			{ // the object wasn't resized/didn't change frame
				int puid=-1;
				C3DObject* p=getParent();
				if (p!=NULL)
					puid=p->getUniqueID();
				// Changed following on 24/04/2011 (because we also wanna reset the parenting to the initial state!)
				if (puid!=_initialParentUniqueID)
				{ // Not sure following instructions are not problematic here.
					C3DObject* oldParent=App::ct->objCont->getObjectWithUniqueID(_initialParentUniqueID);
					if ( (oldParent!=NULL)||(_initialParentUniqueID==-1) )
					{
						// Inverted following 2 lines on 24/2/2012:
						App::ct->objCont->makeObjectChildOf(this,oldParent);
						_transformation=_initialLocalTransformationPart1;
					}
				}
				else
					_transformation=_initialLocalTransformationPart1;
			}
			_mainPropertyOverride=_initialMainPropertyOverride;
			_initialConfigurationMemorized=false;
		}
	}
	_initialValuesInitializedMain=false;
}

bool C3DObject::getFullBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	return(false);
}

bool C3DObject::getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	return(getFullBoundingBox(minV,maxV));
}

void C3DObject::disableDynamicTreeForManipulation(bool d)
{
	if (d!=_dynamicsTemporarilyDisabled)
		App::ct->objCont->incrementMainPropertyOverrideValidityNumber(); // we want the cumulative values all recalculated
	_dynamicsTemporarilyDisabled=d;
}

void C3DObject::setDynamicSimulationIconCode(int c)
{
	if (c!=_dynamicSimulationIconCode)
		App::ct->hierarchy->setRefreshViewFlag();
	_dynamicSimulationIconCode=c;
}

int C3DObject::getDynamicSimulationIconCode()
{
	return(_dynamicSimulationIconCode);
}

bool C3DObject::getObjectManipulationDisabledDuringSimulation_currentSettings()
{
	if ((App::ct->getMouseMode()&0x0f)==sim_navigation_objectrotate)
	{ // rotation
		return(CQDlgRotations::forbidManipWhenRunning);
	}
	else
	{ // translation
		return(CQDlgTranslations::forbidManipWhenRunning);
	}
}

bool C3DObject::getObjectManipulationDisabledDuringNonSimulation_currentSettings()
{
	if ((App::ct->getMouseMode()&0x0f)==sim_navigation_objectrotate)
	{ // rotation
		return(CQDlgRotations::forbidManipWhenNotRunning);
	}
	else
	{ // translation
		return(CQDlgTranslations::forbidManipWhenNotRunning);
	}
}

int C3DObject::getObjectManipulationModePermissions_currentSettings()
{
	if ((App::ct->getMouseMode()&0x0f)==sim_navigation_objectrotate)
	{ // rotation
		return(CQDlgRotations::manipulationModePermission);
	}
	else
	{ // translation
		return(CQDlgTranslations::manipulationModePermission);
	}
}

int C3DObject::getObjectManipulationTranslationRelativeTo_currentSettings()
{
	return(CQDlgTranslations::manipulationTranslationRelativeTo);
}

int C3DObject::getObjectManipulationRotationRelativeTo_currentSettings()
{
	return(CQDlgRotations::manipulationRotationRelativeTo);
}

float C3DObject::getNonDefaultTranslationStepSize_currentSettings()
{
	return(CQDlgTranslations::manipulationTranslationStepSize);
}

float C3DObject::getNonDefaultRotationStepSize_currentSettings()
{
	return(CQDlgRotations::manipulationRotationStepSize);
}

void C3DObject::setAssemblingLocalTransformation(const C7Vector& tr)
{
	_assemblingLocalTransformation=tr;
}
C7Vector C3DObject::getAssemblingLocalTransformation()
{
	return(_assemblingLocalTransformation);
}
/*
void C3DObject::setUseSpecialLocalTransformationWhenAssembling(bool u)
{
	_useSpecialLocalTransformationWhenAssembling=u;
}
bool C3DObject::getUseSpecialLocalTransformationWhenAssembling()
{
	return(_useSpecialLocalTransformationWhenAssembling);
}
*/
void C3DObject::setAssemblyMatchValue(bool asChild,int mv)
{
	if (asChild)
		_assemblyMatchValueChild=mv;
	else
		_assemblyMatchValueParent=mv;
}
int C3DObject::getAssemblyMatchValue(bool asChild)
{
	if (asChild)
		return(_assemblyMatchValueChild);
	return(_assemblyMatchValueParent);
}
void C3DObject::setAssemblyRole(bool childRole,bool roleEnabled)
{
	if (childRole)
		_assemblyCanHaveChildRole=roleEnabled;
	else
		_assemblyCanHaveParentRole=roleEnabled;

}
bool C3DObject::getAssemblyRole(bool childRole)
{
	if (childRole)
		return(_assemblyCanHaveChildRole);
	return(_assemblyCanHaveParentRole);
}

void C3DObject::generateUniqueUpdatableString()
{
	std::string s("1234567890123456");
	std::string a(tt::FNb(int(SIM_RAND_FLOAT*100000000.0f)));
	while (a.length()<16)
		a=a+"*";
	std::string b(tt::FNb(VDateTime::getTimeInMs()));
	while (b.length()<16)
		b="*"+b;
	std::string c(tt::FNb(int(VDateTime::getSecondsSince1970())));
	while (c.length()<16)
		c=c+"*";
	for (int i=0;i<16;i++)
	{
		s[i]+=a[i]*int(SIM_RAND_FLOAT*4.1f);
		s[i]+=b[i]*int(SIM_RAND_FLOAT*3.1f);
		s[i]+=c[i]*int(SIM_RAND_FLOAT*17.1f);
	}
	_uniqueUpdatableString=s;
}

std::string C3DObject::getUniqueUpdatableString()
{
	return(_uniqueUpdatableString);
}

void C3DObject::serialize(CSer& ar)
{
}

void C3DObject::serializeMain(CSer& ar)
{
	if (ar.isStoring())
	{		// Storing
		// KEEP FOLLOWING ALWAYS AT THE BEGINNING!!!!!!! (3do)
		ar.storeDataName("3do"); // 3D object identifier, added on 2009/12/09. Needed for forward compatibility when trying to load an object type that doesn't yet exist!
		ar << BYTE(57) << BYTE(58) << BYTE(59);
		ar.flush();

		ar.storeDataName("Cfq");
		C7Vector tr=getLocalTransformationPart1();
		ar << tr.Q(0) << tr.Q(1) << tr.Q(2) << tr.Q(3) << tr.X(0) << tr.X(1) << tr.X(2);
		ar.flush();
	
		ar.storeDataName("Alt");
		ar << _assemblingLocalTransformation.Q(0) << _assemblingLocalTransformation.Q(1) << _assemblingLocalTransformation.Q(2) << _assemblingLocalTransformation.Q(3) << _assemblingLocalTransformation.X(0) << _assemblingLocalTransformation.X(1) << _assemblingLocalTransformation.X(2);
		ar.flush();

		ar.storeDataName("Amv");
		ar << _assemblyMatchValueChild << _assemblyMatchValueParent;
		ar.flush();

		ar.storeDataName("Ids");
		int parentID=-1;
		if (getParent()!=NULL)
			parentID=getParent()->getID();
		ar << getID() << parentID;
		ar.flush();

		ar.storeDataName("Nme");
		ar << getName();
		ar.flush();

		ar.storeDataName("Hci");
		ar << _hierarchyColorIndex;
		ar.flush();

		ar.storeDataName("Sci");
		ar << _collectionSelfCollisionIndicator;
		ar.flush();

		ar.storeDataName("Op2");
		int objProp=_objectProperty|sim_objectproperty_reserved5; // Needed for backward compatibility (still in serialization version 15)
		ar << objProp;
		ar.flush();

		ar.storeDataName("Var");
		BYTE dummy=0;
		SIM_SET_CLEAR_BIT(dummy,0,_modelBase);
		SIM_SET_CLEAR_BIT(dummy,1,_objectManipulationDisabledDuringSimulation);
		SIM_SET_CLEAR_BIT(dummy,2,_objectManipulationDisabledDuringNonSimulation);
		SIM_SET_CLEAR_BIT(dummy,3,_ignoredByViewFitting);
		// reserved since 9/6/2013  SIM_SET_CLEAR_BIT(dummy,4,_useSpecialLocalTransformationWhenAssembling);
		SIM_SET_CLEAR_BIT(dummy,5,!_assemblyCanHaveChildRole);
		SIM_SET_CLEAR_BIT(dummy,6,!_assemblyCanHaveParentRole);
		ar << dummy;
		ar.flush();
	
		ar.storeDataName("Omp");
		ar << _mainPropertyLocal;
		ar.flush();

		ar.storeDataName("Mpo");
		ar << _mainPropertyOverride;
		ar.flush();
		
		ar.storeDataName("Lar");
		ar << layer;
		ar.flush();

		ar.storeDataName("Om5");
		ar << _objectManipulationModePermissions << _objectManipulationTranslationRelativeTo << _objectTranslationNonDefaultStepSize;
		ar.flush();

		ar.storeDataName("Omr");
		ar << _objectManipulationRotationRelativeTo << _objectRotationNonDefaultStepSize;
		ar.flush();

		ar.storeDataName("Sfa");
		ar << _sizeFactor;
		ar.flush();

		ar.storeDataName("Cv2");
		ar << _creationInfo[0];
		ar << _creationInfo[1];
		ar << _creationInfo[2];
		ar << _creationInfo[3];
		ar.flush();

		if (_customObjectData!=NULL)
		{
			ar.storeDataName("Cod");
			ar.setCountingMode();
			_customObjectData->serializeData(ar);
			if (ar.setWritingMode())
				_customObjectData->serializeData(ar);
		}

		ar.storeDataName("Ack");
		ar << _modelAcknowledgement;
		ar.flush();

		ar.storeDataName("Ups");
		ar << _uniqueUpdatableString;
		ar.flush();

		ar.storeDataName(SER_NEXT_STEP);
	}
	else
	{		// Loading
		int byteQuantity;
		std::string theName="";
		while (theName.compare(SER_NEXT_STEP)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_NEXT_STEP)!=0)
			{
				bool noHit=true;
				if (theName.compare("3do")==0) 
				{ // 3D object identifier, added on 2009/12/09. Needed for forward compatibility when trying to load an object type that doesn't yet exist!
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy >> dummy >> dummy;
				}
				if (theName.compare("Cfq")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					C7Vector tr;
					ar >> tr.Q(0) >> tr.Q(1) >> tr.Q(2) >> tr.Q(3) >> tr.X(0) >> tr.X(1) >> tr.X(2);
					setLocalTransformation(tr);
				}
				if (theName.compare("Hci")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _hierarchyColorIndex;
				}
				if (theName.compare("Sci")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _collectionSelfCollisionIndicator;
				}
				if (theName.compare("Alt")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _assemblingLocalTransformation.Q(0) >> _assemblingLocalTransformation.Q(1) >> _assemblingLocalTransformation.Q(2) >> _assemblingLocalTransformation.Q(3) >> _assemblingLocalTransformation.X(0) >> _assemblingLocalTransformation.X(1) >> _assemblingLocalTransformation.X(2);
				}
				if (theName.compare("Amv")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _assemblyMatchValueChild >> _assemblyMatchValueParent;
				}
				if (theName.compare("Ids")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _objectID >> _parentID;
					_parentObject=NULL;
				}
				if (theName.compare("Nme")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _objectName;
				}
				if (theName.compare("Opr")==0)
				{ // keep a while for backward compatibility (2010/07/14)
					noHit=false;
					ar >> byteQuantity;
					ar >> _objectProperty;
					if (_objectProperty&sim_objectproperty_reserved7)
						_modelBase=true;
					_objectProperty|=sim_objectproperty_reserved7;
					_objectProperty-=sim_objectproperty_reserved7;
				}
				if (theName.compare("Op2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _objectProperty;
				}
				if (theName.compare("Var")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
					_modelBase=SIM_IS_BIT_SET(dummy,0);
					_objectManipulationDisabledDuringSimulation=SIM_IS_BIT_SET(dummy,1);
					_objectManipulationDisabledDuringNonSimulation=SIM_IS_BIT_SET(dummy,2);
					_ignoredByViewFitting=SIM_IS_BIT_SET(dummy,3);
					// reserved since 9/6/2013   _useSpecialLocalTransformationWhenAssembling=SIM_IS_BIT_SET(dummy,4);
					_assemblyCanHaveChildRole=!SIM_IS_BIT_SET(dummy,5);
					_assemblyCanHaveParentRole=!SIM_IS_BIT_SET(dummy,6);
				}
				if (theName.compare("Omp")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _mainPropertyLocal;
				}
				if (theName.compare("Mpo")==0)
				{ // from 2010/08/06
					noHit=false;
					ar >> byteQuantity;
					ar >> _mainPropertyOverride;
				}
				if (theName.compare("Lar")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> layer;
				}
				if (theName.compare("Om2")==0)
				{ // Keep a while for backward compatibility (2010/07/11)
					noHit=false;
					ar >> byteQuantity;
					ar >> _objectManipulationModePermissions;
					if (_objectType==sim_object_camera_type)
					{
						if ((_objectManipulationModePermissions&0x07)!=0x07)
						{ // here we probably have one of the 3 ortho cameras in each scene
							if (_objectManipulationModePermissions&0x010) // old around own z
								((CCamera*)this)->setCameraManipulationModePermissions(0x013); // for the z-camera
							else
								((CCamera*)this)->setCameraManipulationModePermissions(0x003); // for the x- and y-camera
							((CCamera*)this)->backwardCompatibility20100713();
						}
						_objectManipulationModePermissions=0x03f; // free for object manipulation
					}
					else
						_objectManipulationModePermissions|=0x038; // free for object rotation
				}
				if (theName.compare("Om3")==0)
				{ // Keep a while for backward compatibility (2010/07/13)
					noHit=false;
					ar >> byteQuantity;
					ar >> _objectManipulationModePermissions;
				}
				if (theName.compare("Om4")==0)
				{ // Keep a while for backward compatibility (2010/07/13)
					noHit=false;
					ar >> byteQuantity;
					ar >> _objectManipulationTranslationRelativeTo;
				}
				if (theName.compare("Om5")==0)
				{ 
					noHit=false;
					ar >> byteQuantity;
					ar >> _objectManipulationModePermissions >> _objectManipulationTranslationRelativeTo >> _objectTranslationNonDefaultStepSize;
				}
				if (theName.compare("Omr")==0)
				{ 
					noHit=false;
					ar >> byteQuantity;
					ar >> _objectManipulationRotationRelativeTo >> _objectRotationNonDefaultStepSize;
				}

				if (theName.compare("Cv2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _creationInfo[0];
					ar >> _creationInfo[1];
					ar >> _creationInfo[2];
					ar >> _creationInfo[3];
				}
				if (theName.compare("Cod")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					_customObjectData=new CCustomData();
					_customObjectData->serializeData(ar);
				}

				if (theName.compare("Sfa")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _sizeFactor;
				}
				if (theName.compare("Ack")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _modelAcknowledgement;
				}
				if (theName.compare("Ups")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _uniqueUpdatableString;
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
		//*************************************************************
		// For backward compatibility 13/09/2011
		if ((_objectProperty&sim_objectproperty_reserved5)==0)
		{ // this used to be the sim_objectproperty_visible property. If it wasn't set in the past, we now try to hide it in a hidden layer:
			if (layer<256)
				layer=layer*256;
		}
		else
			_objectProperty-=sim_objectproperty_reserved5;
		//*************************************************************

		//*************************************************************
		// For old models to support the DNA-thing by default:
		if ( (ar.getVrepVersionThatWroteThisFile()<30003)&&getModelBase() )
		{
			_objectProperty|=sim_objectproperty_canupdatedna;
			// We now create a "unique" id, that is always the same for the same file:
			_uniqueUpdatableString="1234567890123456";
			std::string a(_objectName);
			while (a.length()<16)
				a=a+"*";
			std::string b("1234567890123456");
			b[0]=((BYTE*)_creationInfo)[10];
			b[1]=((BYTE*)_creationInfo)[11];
			int fbp=ar.getFileBufferReadPointer();
			b[2]=((BYTE*)&fbp)[0];
			b[3]=((BYTE*)&fbp)[1];
			b[4]=((BYTE*)&fbp)[2];
			b[5]=((BYTE*)&fbp)[3];
			for (int i=0;i<16;i++)
			{
				_uniqueUpdatableString[i]+=a[i];
				_uniqueUpdatableString[i]+=b[i];
			}
		}
		//*************************************************************
	}
}


void C3DObject::serializeWExtIk(CExtIkSer& ar)
{
}

void C3DObject::serializeWExtIkMain(CExtIkSer& ar)
{
	C7Vector tr=getLocalTransformationPart1();
	ar.writeFloat(tr.Q(0));
	ar.writeFloat(tr.Q(1));
	ar.writeFloat(tr.Q(2));
	ar.writeFloat(tr.Q(3));
	ar.writeFloat(tr.X(0));
	ar.writeFloat(tr.X(1));
	ar.writeFloat(tr.X(2));
	
	int parentID=-1;
	if (getParent()!=NULL)
		parentID=getParent()->getID();
	ar.writeInt(_objectID);
	ar.writeInt(parentID);

	ar.writeString(_objectName.c_str());
}

int C3DObject::_uniqueIDCounter=0;


void C3DObject::performObjectLoadingMapping(std::vector<int>* map)
{
}

void C3DObject::performObjectLoadingMappingMain(std::vector<int>* map)
{
	int newParentID=App::ct->objCont->getLoadingMapping(map,_parentID);
	setParent(App::ct->objCont->getObject(newParentID));
}

std::string C3DObject::getName()
{
	return(_objectName);
}
void C3DObject::setName(std::string newName)
{ 
	_objectName=newName;
}

bool C3DObject::announceObjectWillBeErased(int objID,bool copyBuffer)
{
	// Return value true means 'this' has to be erased too!
	return(false);
}
void C3DObject::announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer)
{
}

void C3DObject::announceObjectWillBeErasedMain(int objID,bool copyBuffer)
{ 
	// This routine can be called for objCont-objects, but also for objects
	// in the copy-buffer!! So never make use of any 
	// 'ct::objCont->getObject(id)'-call or similar

	// if we are in edit mode, we leave edit mode:
	if ( (App::ct->objCont->getEditModeType()!=NO_EDIT_MODE)&&(!copyBuffer) )
	{
		if (App::ct->objCont->getEditModeObjectID()==objID)
			App::ct->objCont->endEditMode(true); // This is if we destroy the object being edited (shouldn't normally happen!)
	}

	// If the object's parent will be erased, make the object child of its grand-parents
	if ( (getParent()!=NULL)&&(!copyBuffer) )
	{
		if (getParent()->getID()==objID)
			App::ct->objCont->makeObjectChildOf(this,getParent()->getParent());
	}
}
void C3DObject::announceIkObjectWillBeErasedMain(int ikGroupID,bool copyBuffer)
{
	// This routine can be called for objCont-objects, but also for objects
	// in the copy-buffer!! So never make use of any 
	// 'ct::objCont->getObject(id)'-call or similar
}

void C3DObject::getCumulativeTransformationMatrix(float m[4][4],bool useTempValues) 
{ // useTempValues is false by default. This is used by the IK-routine
	getCumulativeTransformation(useTempValues).copyTo(m);
}

void C3DObject::getCumulativeTransformationMatrixPart1(float m[4][4],bool useTempValues) 
{ // useTempValues is false by default. This is used by the IK-routine
	getCumulativeTransformationPart1(useTempValues).copyTo(m);
}

C7Vector C3DObject::getParentCumulativeTransformation(bool useTempValues)
{ // useTempValues is false by default. This is used by the IK-routine
	if (getParent()==NULL)
	{
		C7Vector retV;
		retV.setIdentity();
		return(retV);
	}
	else
		return(getParent()->getCumulativeTransformation(useTempValues));
}

C7Vector C3DObject::getParentCumulativeTransformation_forDisplay(bool guiIsRendering)
{
	if (getParent()==NULL)
	{
		C7Vector retV;
		retV.setIdentity();
		return(retV);
	}
	else
		return(getParent()->getCumulativeTransformation_forDisplay(guiIsRendering));
}


C7Vector C3DObject::getCumulativeTransformation(bool useTempValues)
{ // useTempValues is false by default. This is used by the IK-routine
	if (getParent()==NULL)
		return(getLocalTransformation(useTempValues));
	else
		return(getParentCumulativeTransformation(useTempValues)*getLocalTransformation(useTempValues));
}
C7Vector C3DObject::getCumulativeTransformation_forDisplay(bool guiIsRendering)
{
	if (getParent()==NULL)
		return(getLocalTransformation_forDisplay(guiIsRendering));
	else
		return(getParentCumulativeTransformation_forDisplay(guiIsRendering)*getLocalTransformation_forDisplay(guiIsRendering));
}

C7Vector C3DObject::getLocalTransformation(bool useTempValues)
{ // useTempValues is false by default. This is used by the IK-routine
	if (getObjectType()==sim_object_joint_type)
	{
		CJoint* it=(CJoint*)this;
		C7Vector jointTr;
		jointTr.setIdentity();
		float val;
		val=it->getPosition(useTempValues);
		if (it->getJointType()==sim_joint_revolute_subtype)
		{
			jointTr.Q.setAngleAndAxis(val,C3Vector(0.0f,0.0f,1.0f));
			jointTr.X(2)=val*it->getScrewPitch();
		}
		if (it->getJointType()==sim_joint_prismatic_subtype)
			jointTr.X(2)=val;
		if (it->getJointType()==sim_joint_spherical_subtype)
		{
			if (useTempValues)
			{
				if (it->getTempSphericalJointLimitations()==0)
				{ // Used by the IK routine when away from joint limitations
					jointTr.Q.setEulerAngles(0.0f,0.0f,it->getTempParameterEx(2));
					C4Vector q2;
					q2.setEulerAngles(piValD2,0.0f,0.0f);
					jointTr.Q=q2*jointTr.Q;

					q2.setEulerAngles(0.0f,0.0f,it->getTempParameterEx(1));
					jointTr.Q=q2*jointTr.Q;
					q2.setEulerAngles(-piValD2,0.0f,-piValD2);
					jointTr.Q=q2*jointTr.Q;

					q2.setEulerAngles(0.0f,0.0f,it->getTempParameterEx(0));
					jointTr.Q=q2*jointTr.Q;
					q2.setEulerAngles(0.0f,piValD2,0.0f);
					jointTr.Q=q2*jointTr.Q;
					q2=it->getSphericalTransformation();
					jointTr.Q=q2*jointTr.Q;
				}
				else
				{ // Used by the IK routine when close to joint limitations
					jointTr.Q.setEulerAngles(0.0f,0.0f,it->getTempParameterEx(2));
					C4Vector q2;
					q2.setEulerAngles(0.0f,-piValD2,0.0f);
					jointTr.Q=q2*jointTr.Q;

					q2.setEulerAngles(0.0f,0.0f,it->getTempParameterEx(1));
					jointTr.Q=q2*jointTr.Q;
					q2.setEulerAngles(0.0f,piValD2,0.0f);
					jointTr.Q=q2*jointTr.Q;

					q2.setEulerAngles(0.0f,0.0f,it->getTempParameterEx(0));
					jointTr.Q=q2*jointTr.Q;
				}
			}
			else
				jointTr.Q=it->getSphericalTransformation();
		}
		return(_transformation*jointTr);
	}
	else if (getObjectType()==sim_object_dummy_type)
	{
		CDummy* it=(CDummy*)this;
		if (useTempValues)
			return(it->getTempLocalTransformation()); // used for IK when dummy is freely sliding on a path object
		return(_transformation);
	}
	else
		return(_transformation);
}
C7Vector C3DObject::getLocalTransformation_forDisplay(bool guiIsRendering)
{
	if (getObjectType()==sim_object_joint_type)
	{
		CJoint* it=(CJoint*)this;
		C7Vector jointTr;
		jointTr.setIdentity();
		float val;
		val=it->getPosition_forDisplay(guiIsRendering);
		if (it->getJointType()==sim_joint_revolute_subtype)
		{
			jointTr.Q.setAngleAndAxis(val,C3Vector(0.0f,0.0f,1.0f));
			jointTr.X(2)=val*it->getScrewPitch();
		}
		if (it->getJointType()==sim_joint_prismatic_subtype)
			jointTr.X(2)=val;
		if (it->getJointType()==sim_joint_spherical_subtype)
			jointTr.Q=it->getSphericalTransformation_forDisplay(guiIsRendering);
		if (guiIsRendering)
			return(_transformation_forDisplay*jointTr);
		else
			return(_transformation*jointTr);
	}
	if (guiIsRendering)
		return(_transformation_forDisplay);
	else
		return(_transformation);
}

C7Vector C3DObject::getCumulativeTransformationPart1(bool useTempValues)
{ // useTempValues is false by default. This is used by the IK-routine
	if (getObjectType()==sim_object_joint_type)
	{
		if (getParent()==NULL)
			return(getLocalTransformationPart1(useTempValues));
		else
			return(getParentCumulativeTransformation(useTempValues)*getLocalTransformationPart1(useTempValues));
	}
	else
		return(getCumulativeTransformation(useTempValues));
}

C7Vector C3DObject::getCumulativeTransformationPart1_forDisplay(bool guiIsRendering)
{ 
	if (getObjectType()==sim_object_joint_type)
	{
		if (getParent()==NULL)
			return(getLocalTransformationPart1_forDisplay(guiIsRendering));
		else
			return(getParentCumulativeTransformation_forDisplay(guiIsRendering)*getLocalTransformationPart1_forDisplay(guiIsRendering));
	}
	else
		return(getCumulativeTransformation_forDisplay(guiIsRendering));
}

C7Vector& C3DObject::getLocalTransformationPart1(bool useTempValues)
{ // useTempValues is false by default. This is used by the IK-routine
	return(_transformation);
}
C7Vector& C3DObject::getLocalTransformationPart1_forDisplay(bool guiIsRendering)
{
	if (guiIsRendering)
		return(_transformation_forDisplay);
	else
		return(_transformation);
}

void C3DObject::setLocalTransformation(const C7Vector& v)
{
	_transformation=v;
}

void C3DObject::setLocalTransformation(const C4Vector& q)
{
	_transformation.Q=q;
}
void C3DObject::setLocalTransformation(const C3Vector& x)
{
	_transformation.X=x;
}
void C3DObject::setAbsoluteTransformation(const C7Vector& v)
{
	C7Vector pInv(getParentCumulativeTransformation().getInverse());
	_transformation.setMultResult(pInv,v);
}
void C3DObject::setAbsoluteTransformation(const C4Vector& q)
{
	C7Vector pInv(getParentCumulativeTransformation().getInverse());
	_transformation.Q=pInv.Q*q;
}
void C3DObject::setAbsoluteTransformation(const C3Vector& x)
{
	C7Vector pInv(getParentCumulativeTransformation().getInverse());
	_transformation.X=pInv*x;
}

int C3DObject::getID()
{
	return(_objectID);
}
void C3DObject::setID(int newID)
{ // Be very careful with this function!!!
	_objectID=newID;
}

bool C3DObject::isObjectParentedWith(C3DObject* thePotentialParent)
{
	if (getParent()==NULL)
		return(false);
	if (getParent()==thePotentialParent) 
		return(true);
	return(getParent()->isObjectParentedWith(thePotentialParent));
}

int C3DObject::getParentIdentifierLoading()
{
	return(_parentID);
}

void C3DObject::setParentIdentifierLoading(int pID)
{ // Be very careful with this function!!!
	_parentID=pID;
}

void C3DObject::setParent(C3DObject* newParent,bool actualizeInfo)
{ // actualizeInfo is true by default
	if (newParent==this)
		return;
	EASYLOCK(_objectMutex);
	_parentObject=newParent;
	if (actualizeInfo) 
		App::ct->objCont->actualizeObjectInformation();
}

C3DObject* C3DObject::getParent()
{
	return(_parentObject);
}

int C3DObject::getObjectType()
{
	return(_objectType);
}



