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

#pragma once

	#include "VisualParam.h"
	class CShape;
	class CCustomData;
	class CViewableBase;

#include "extIkSer.h"
#include "3Vector.h"
#include "4Vector.h"
#include "7Vector.h"
#include "VMutex.h"

class C3DObject  
{
public:

	C3DObject();
	virtual ~C3DObject();

	// Following functions need to be implemented in each class derived from 3DObject
	virtual C3DObject* copyYourself();
	virtual void display(CViewableBase* renderingObject,int displayAttrib);
	virtual void scaleObject(float scalingFactor);
	virtual void serialize(CSer& ar);
	virtual void serializeWExtIk(CExtIkSer& ar);
	virtual void announceGroupWillBeErased(int groupID,bool copyBuffer);
	virtual void announceCollisionWillBeErased(int collisionID,bool copyBuffer);
	virtual void announceDistanceWillBeErased(int distanceID,bool copyBuffer);
	virtual void announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer);
	virtual void performGroupLoadingMapping(std::vector<int>* map);
	virtual void performCollisionLoadingMapping(std::vector<int>* map);
	virtual void performDistanceLoadingMapping(std::vector<int>* map);
	virtual void performGcsLoadingMapping(std::vector<int>* map);
	virtual	void performTextureObjectLoadingMapping(std::vector<int>* map);
	virtual	void performDynMaterialObjectLoadingMapping(std::vector<int>* map);
	virtual void performIkLoadingMapping(std::vector<int>* map);
	virtual void bufferMainDisplayStateVariables();
	virtual void bufferedMainDisplayStateVariablesToDisplay();

	virtual void simulationAboutToStart();
	virtual void simulationEnded();
	virtual void initializeInitialValues(bool simulationIsRunning);
	virtual bool getFullBoundingBox(C3Vector& minV,C3Vector& maxV);
	virtual bool getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV);
	virtual bool getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices);

	virtual std::string getObjectTypeInfo();
	virtual std::string getObjectTypeInfoExtended();

	// Following functions need to be called from the derived classes
	C3DObject* copyYourselfMain();
	void scaleObjectMain(float scalingFactor);
	void serializeMain(CSer& ar);
	void serializeWExtIkMain(CExtIkSer& ar);
	void simulationEndedMain();
	void initializeInitialValuesMain(bool simulationIsRunning);
	void announceGroupWillBeErasedMain(int groupID,bool copyBuffer);
	void announceCollisionWillBeErasedMain(int collisionID,bool copyBuffer);
	void announceDistanceWillBeErasedMain(int distanceID,bool copyBuffer);
	void announceGcsObjectWillBeErasedMain(int gcsObjectID,bool copyBuffer);
	void performGroupLoadingMappingMain(std::vector<int>* map);
	void performCollisionLoadingMappingMain(std::vector<int>* map);
	void performDistanceLoadingMappingMain(std::vector<int>* map);
	void performGcsLoadingMappingMain(std::vector<int>* map);
	void performTextureObjectLoadingMappingMain(std::vector<int>* map);
	void performDynMaterialObjectLoadingMappingMain(std::vector<int>* map);
	void performIkLoadingMappingMain(std::vector<int>* map);
	void bufferMainDisplayStateVariablesMain();
	void bufferedMainDisplayStateVariablesToDisplayMain();


	// 3D transformation functions
	void performOGLTransformation(int displayAttrib);
	bool setLocalTransformationFromObjectRotationMode(const C4X4Matrix& cameraAbsConf,float rotationAmount,bool perspective,int eventID);
	bool setLocalTransformationFromObjectTranslationMode(const C4X4Matrix& cameraAbsConf,const C3Vector& clicked3DPoint,float prevPos[2],float pos[2],float screenHalfSizes[2],float halfSizes[2],bool perspective,int eventID);


	// These two functions have to be called by each derived class at the beginning
	// and the end respectively of their display function
	void commonStart(int displayAttrib);
	void commonFinish();
	
	// Various functions
	void scalePosition(float scalingFactor);
	void getAllShapeObjectsRecursive(std::vector<CShape*>* shapeList,bool baseIncluded=true,bool start=true);
	void getAllObjectsRecursive(std::vector<C3DObject*>* objectList,bool baseIncluded=true,bool start=true);
	void getChain(std::vector<C3DObject*>& objectList,bool tipIncluded=true,bool start=true);
	int getUniqueID();

	C3DObject* getFirstParentInSelection(std::vector<C3DObject*>* sel);
	void setObjectType(int theType);
	void setDynamicObjectFlag_forVisualization(int isDynamicallySimulatedFlag);
	int getDynamicObjectFlag_forVisualization();
	int getDynamicObjectFlag_forVisualization_forDisplay(bool guiIsRendering);


	int getLifeID();

	void displayBoundingBox(int displayAttrib,bool displRef=true,float refSize=0.0f);
	void drawReference(float refSize=0.0);
	bool getGlobalMarkingBoundingBox(const C7Vector& baseCoordInv,C3Vector& min,C3Vector& max,bool& minMaxNotYetDefined,bool guiIsRendering);
	void getBoundingBoxEncompassingBoundingBox(const C7Vector& baseCoordInv,C3Vector& min,C3Vector& max,bool guiIsRendering);

	int getModelSelectionID(bool firstObject=true);

	void setObjectProperty(int p);
	int getObjectProperty();
	void setModelBase(bool m);
	bool getModelBase();

	void getChildScriptsToRun(std::vector<int>& childScriptIDs);
	void setObjectManipulationModePermissions(int p);
	int getObjectManipulationModePermissions();
	void setObjectManipulationDisabledDuringSimulation(bool d);
	bool getObjectManipulationDisabledDuringSimulation();
	void setObjectManipulationDisabledDuringNonSimulation(bool d);
	bool getObjectManipulationDisabledDuringNonSimulation();

	void setObjectManipulationTranslationRelativeTo(int p);
	int getObjectManipulationTranslationRelativeTo();
	void setObjectManipulationRotationRelativeTo(int p);
	int getObjectManipulationRotationRelativeTo();
	void setNonDefaultTranslationStepSize(float s);
	float getNonDefaultTranslationStepSize();
	void setNonDefaultRotationStepSize(float s);
	float getNonDefaultRotationStepSize();

	// Following similar to above's functions, but using the dialog settings (and not the object's settings!):
	bool getObjectManipulationDisabledDuringSimulation_currentSettings();
	bool getObjectManipulationDisabledDuringNonSimulation_currentSettings();
	int getObjectManipulationModePermissions_currentSettings();
	int getObjectManipulationTranslationRelativeTo_currentSettings();
	int getObjectManipulationRotationRelativeTo_currentSettings();
	float getNonDefaultTranslationStepSize_currentSettings();
	float getNonDefaultRotationStepSize_currentSettings();

	void setObjectCustomDataObject(CCustomData* theData);

	void setObjectCustomData(int header,const char* data,int dataLength);
	int getObjectCustomDataLength(int header);
	void getObjectCustomData(int header,char* data);
	bool getObjectCustomDataHeader(int index,int& header);

	int getParentCount();

	void setMainPropertyLocal(int prop);
	int getMainPropertyLocal();
	void setMainPropertyOverride(int prop);
	int getMainPropertyOverride();
	int getCumulativeMainPropertyOverride();
	int getMainProperty();

	int getTreeDynamicProperty(); // combination of OBJECT_DYNAMIC_PROPERTY_DYNAMIC and OBJECT_DYNAMIC_PROPERTY_RESPONDABLE

	void disableDynamicTreeForManipulation(bool d);


	void setCreationInfo(DWORD info[4]);
	void getCreationInfo(DWORD info[4]);

	void setMechanismID(int id);
	int getMechanismID();

	// Following two needed when a shape is edited during simulation
	void incrementMemorizedConfigurationValidCounter();
	int getMemorizedConfigurationValidCounter();

	void setSizeFactor(float f);
	float getSizeFactor();

	void setDynamicsFullRefreshFlag(bool refresh);
	bool getDynamicsFullRefreshFlag();

	void setDynamicSimulationIconCode(int c);
	int getDynamicSimulationIconCode();

	void clearManipulationModeOverlayGridFlag();
	void displayManipulationModeOverlayGrid(bool transparentAndOverlay);

	bool isObjectPartOfInvisibleModel();

	void setModelAcknowledgement(const std::string& a);
	std::string getModelAcknowledgement();

	void setIgnoredByViewFitting(bool ignored);
	bool getIgnoredByViewFitting();

	bool getShouldObjectBeDisplayed(int displayAttrib);

	void setAssemblingLocalTransformation(const C7Vector& tr);
	C7Vector getAssemblingLocalTransformation();

	C7Vector getParentCumulativeTransformation_forDisplay(bool guiIsRendering);
	C7Vector getCumulativeTransformation_forDisplay(bool guiIsRendering);
	C7Vector getLocalTransformation_forDisplay(bool guiIsRendering);
	C7Vector getCumulativeTransformationPart1_forDisplay(bool guiIsRendering);
	C7Vector& getLocalTransformationPart1_forDisplay(bool guiIsRendering);

	void setAssemblyMatchValue(bool asChild,int mv);
	int getAssemblyMatchValue(bool asChild);
	void setAssemblyRole(bool childRole,bool roleEnabled);
	bool getAssemblyRole(bool childRole);

	void generateUniqueUpdatableString();
	std::string getUniqueUpdatableString();

	void setHierarchyColorIndex(int c);
	int getHierarchyColorIndex();

	void setCollectionSelfCollisionIndicator(int c);
	int getCollectionSelfCollisionIndicator();

	void measureVelocity(float dt); // should be called just after the main script was called!!!
	float getMeasuredAngularVelocity();
	C3Vector getMeasuredLinearVelocity();
	C3Vector getMeasuredAngularVelocity3();

	// Variables which need to be serialized & copied
	WORD layer;

protected:
	int _dynamicSimulationIconCode;

	C7Vector _assemblingLocalTransformation; // When assembling this object 

	int _uniqueID; // valid for a given scene instance (non-persistent)

	int _assemblyMatchValueChild;
	int _assemblyMatchValueParent;
	bool _assemblyCanHaveChildRole;
	bool _assemblyCanHaveParentRole;
	std::string _uniqueUpdatableString;

	int _objectProperty;
	bool _modelBase;
	bool _ignoredByViewFitting;
	int _hierarchyColorIndex;
	int _collectionSelfCollisionIndicator;

	int _mainPropertyLocal;
	int _mainPropertyOverride;
	int _mainPropertyOverrideCombinedWithParents;
	int _mainPropertyOverrideCombinedWithParentsValidityNumber;

	int _objectManipulationModePermissions;
	bool _objectManipulationDisabledDuringSimulation;
	bool _objectManipulationDisabledDuringNonSimulation;
	bool _objectManipulationModePermissionsPreviousCtrlKeyDown;
	float _objectTranslationNonDefaultStepSize;
	float _objectRotationNonDefaultStepSize;
	int _objectManipulationTranslationRelativeTo; //0=world, 1=parent, 2=own frame
	int _objectManipulationRotationRelativeTo; //0=world, 1=parent, 2=own frame
	int _memorizedConfigurationValidCounter;
	float _sizeFactor; // just used so that scripts can also adjust for scaling
	int _dynamicProperty; // modified in the dynamics routines. Not saved nor copied!
	bool _dynamicsTemporarilyDisabled;
	bool _dynamicsFullRefreshFlag;

protected:
	VMutex _objectMutex;
	C3Vector _objectManipulationModeSubTranslation;
	float _objectManipulationModeSubRotation;
	int _objectManipulationModeEventId;
	int _objectManipulationModeAxisIndex;
	C3Vector _objectManipulationModeRelativePositionOfClickedPoint;
	C3Vector _objectManipulationModeTotalTranslation;
	float _objectManipulationModeTotalRotation;
	BYTE _objectManipulationMode_flaggedForGridOverlay; // is the rotation or translation axis index + 8 if it is a rotation, or +16 if it is a translation
	CCustomData* _customObjectData;
	DWORD _creationInfo[4];
	std::string _modelAcknowledgement;

	C7Vector _transformation_buffered;
	C7Vector _transformation_forDisplay;
	int _dynamicObjectFlag_forVisualization_buffered;
	int _dynamicObjectFlag_forVisualization_forDisplay;


	// Other variables
	int _mechanismID; // don't serialize!
	bool _initialValuesInitializedMain;
	bool _initialConfigurationMemorized;
	int _initialParentUniqueID;
	int _initialMainPropertyOverride;
	int _initialMemorizedConfigurationValidCounter;
	C7Vector _initialLocalTransformationPart1;
	static int _uniqueIDCounter;

	int _dynamicObjectFlag_forVisualization;

	float _measuredAngularVelocity_velocityMeasurement;
	C3Vector _measuredAngularVelocity3_velocityMeasurement;
	C3Vector _measuredLinearVelocity_velocityMeasurement;
	C7Vector _previousAbsTransf_velocityMeasurement;


private:
	int _lifeID;



public:
	virtual bool announceObjectWillBeErased(int objID,bool copyBuffer);
	virtual void announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer);

	void announceObjectWillBeErasedMain(int objID,bool copyBuffer);
	void announceIkObjectWillBeErasedMain(int ikGroupID,bool copyBuffer);

	virtual void performObjectLoadingMapping(std::vector<int>* map);
	void performObjectLoadingMappingMain(std::vector<int>* map);

	void getCumulativeTransformationMatrix(float m[4][4],bool useTempValues=false);
	void getCumulativeTransformationMatrixPart1(float m[4][4],bool useTempValues=false);

	void setLocalTransformation(const C7Vector& v);
	void setLocalTransformation(const C4Vector& q);
	void setLocalTransformation(const C3Vector& x);

	void setAbsoluteTransformation(const C7Vector& v);
	void setAbsoluteTransformation(const C4Vector& q);
	void setAbsoluteTransformation(const C3Vector& x);
	C7Vector getParentCumulativeTransformation(bool useTempValues=false);
	C7Vector getCumulativeTransformation(bool useTempValues=false);
	C7Vector getLocalTransformation(bool useTempValues=false);
	C7Vector getCumulativeTransformationPart1(bool useTempValues=false);
	C7Vector& getLocalTransformationPart1(bool useTempValues=false);

	int getID();
	void setID(int newID);
	void setName(std::string newName);
	std::string getName();
	C3DObject* getParent();
	bool isObjectParentedWith(C3DObject* thePotentialParent);
	int getParentIdentifierLoading();
	void setParentIdentifierLoading(int pID);
	void setParent(C3DObject* newParent,bool actualizeInfo=true);
	int getObjectType();

	std::vector<C3DObject*> childList;

protected:
	// Variables that need to be serialized & copied
	C7Vector _transformation; // Quaternion and position
	int _objectID;
	std::string _objectName;
	// Keep parent stored as pointer. Very important!!! If we would use a simple
	// parent identifier, an object and its copy in the copy buffer would have
	// the exact same parent (which would be retrieved via 'CObjCont::getObject()')
	C3DObject* _parentObject; 
	int _parentID;
	int _objectType;

};
