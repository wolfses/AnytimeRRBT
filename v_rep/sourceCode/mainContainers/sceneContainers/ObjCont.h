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

// This file needs serious refactoring!!

#pragma once

#include "Shape.h"
#include "proximitySensor.h"
#include "visionSensor.h"
#include "Camera.h"
#include "Graph.h"
#include "Path.h"
#include "Mirror.h"
#include "RegGroup.h"
#include "RegColl.h"
#include "RegDist.h"
#include <vector>
#include "PathPlanningTask.h"
#include "MotionPlanningTask.h"
#include "ButtonBlock.h"
#include "LuaScriptObject.h"
#include "EdgeCont.h"
#include "PathCont.h"
#include "ConstraintSolverObject.h"
#include "VolumeObject.h"
#include "Mill.h"
#include "forceSensor.h"
#include "3DObject.h"
#include "jointObject.h"
#include "ikGroup.h"
#include "MainCont.h"

class CObjCont : public CMainCont 
{
public:
	CObjCont();
	virtual ~CObjCont();

	void simulationAboutToStart();
	void simulationEnded();
	void renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib);

	C3DObject* getSelectedObject();

	CMirror* getMirror(int identifier);
	CShape* getShape(int identifier);
	CProxSensor* getProximitySensor(int identifier);
	CVisionSensor* getVisionSensor(int identifier);
	CCamera* getCamera(int identifier);
	CLight* getLight(int identifier);
	CGraph* getGraph(int identifier);
	CPath* getPath(int identifier);
	CVolumeObject* getVolume(int identifier);
	CMill* getMill(int identifier);
	CForceSensor* getForceSensor(int identifier);

	C3DObject* getObjectWithUniqueID(int uniqueID);

	std::string getSimilarNameWithHighestSuffix(std::string objectName,bool dash);

	bool addObjectToScene(C3DObject* newObject,bool objectIsACopy);
	bool addObjectToSceneWithSuffixOffset(C3DObject* newObject,bool objectIsACopy,int suffixOffset);
	void getMinAndMaxNameSuffixes(int& minSuffix,int& maxSuffix);
	bool canSuffix1BeSetToSuffix2(int suffix1,int suffix2);
	void setSuffix1ToSuffix2(int suffix1,int suffix2);

	void cleanupDashNames(int suffix);
	void _getAllObjectsSmallestAndBiggestSuffix(int& smallestSuffix,int& biggestSuffix);
	bool _canSuffix1BeSetToSuffix2(int suffix1,int suffix2);
	void _setSuffix1ToSuffix2(int suffix1,int suffix2);

	void addObjectsToSceneAndPerformMappings(std::vector<C3DObject*>* loadedObjectList,
													std::vector<CRegGroup*>* loadedGroupList,
													std::vector<CRegColl*>* loadedCollisionList,
													std::vector<CRegDist*>* loadedDistanceList,
													std::vector<CikGroup*>* loadedIkGroupList,
													std::vector<CPathPlanningTask*>* loadedPathPlanningTaskList,
													std::vector<CMotionPlanningTask*>* loadedMotionPlanningTaskList,
													std::vector<CButtonBlock*>* loadedButtonBlockList,
													std::vector<CLuaScriptObject*>* loadedLuaScriptList,
													std::vector<CConstraintSolverObject*>* loadedConstraintSolverObjectList,
													std::vector<CTextureObject*>& loadedTextureObjectList,
													std::vector<CDynMaterialObject*>& loadedDynMaterialObjectList,
													bool model);

	int getSuffixOffsetForObjectToAdd(std::vector<C3DObject*>* loadedObjectList,
		std::vector<CRegGroup*>* loadedGroupList,
		std::vector<CRegColl*>* loadedCollisionList,
		std::vector<CRegDist*>* loadedDistanceList,
		std::vector<CikGroup*>* loadedIkGroupList,
		std::vector<CPathPlanningTask*>* loadedPathPlanningTaskList,
		std::vector<CMotionPlanningTask*>* loadedMotionPlanningTaskList,
		std::vector<CButtonBlock*>* loadedButtonBlockList,
		std::vector<CLuaScriptObject*>* loadedLuaScriptList,
		std::vector<CConstraintSolverObject*>* loadedConstraintSolverObjectList);

	// Object selection routines:
	bool selectObject(int objectIdentifier);
	void deselectObjects();
	void addObjectToSelection(int identifier);
	void removeObjectFromSelection(int identifier);
	void xorAddObjectToSelection(int identifier);
	bool isObjectSelected(int identifier);
	void selectAllObjects();

	// Following 3 since 2010/09/01:
	std::vector<int>* getPointerToSelectedPathPointIndices_nonEditMode();
	void clearPathPointIndices_nonEditMode();
	bool isPathPointIndexSelected_nonEditMode(int index,bool removeIfYes);
	void setUniqueSelectedPathID(int id);

	void actualizeMechanismIDs();

	int getSelSize();
	int getSelID(int i);

	C3DObject* load3DObject(CSer& ar,std::string theName,bool &noHit);
	void store3DObject(CSer& ar,C3DObject* it);
	bool loadScene(CSer& ar,bool forUndoRedoOperation);
	void saveScene(CSer& ar);	
	void exportIkContent(CExtIkSer& ar);	
	bool loadModel(CSer& ar,bool justLoadThumbnail);
	bool loadModelOrScene(CSer& ar,bool selectLoaded,bool isScene,bool justLoadThumbnail);
	void setAbsoluteAngle(int identifier,float angle,int index);
	void setAbsolutePosition(int identifier,float pos,int index);
	void setAbsoluteAngles(int identifier,const C3Vector& euler);
	void setAbsolutePosition(int identifier,const C3Vector& p);

	bool getAllShapesAndDummiesFromScene(std::vector<C3DObject*>& objects,int propMask,bool pathPlanningRoutineCalling);
	bool getAllShapesAndVolumesFromScene(std::vector<C3DObject*>& objects,int propMask,bool pathPlanningRoutineCalling);

	bool getAllShapesAndDummiesFromSceneExcept(std::vector<C3DObject*>& exceptionObjects,std::vector<C3DObject*>& objects,int propMask,bool pathPlanningRoutineCalling);
	bool getAllShapesFromSceneExcept(std::vector<C3DObject*>& exceptionObjects,std::vector<C3DObject*>& objects,int propMask,bool pathPlanningRoutineCalling);

	void prepareFastLoadingMapping(std::vector<int>& map);

	static void setDefaultSceneID(float id);
	static float getDefaultSceneID();
	float computeSceneID();


	// Edit mode selection routines:
	void addItemToEditModeBuffer(int item,bool disableEdgeFollowing=true);
	void xorAddItemToEditModeBuffer(int item,bool disableEdgeFollowing=true);
	void deselectEditModeBuffer();
	bool alreadyInEditModeBuffer(int item);
	void removeItemFromEditModeBuffer(int item);
	bool isEditModeItemAValidItem(int item);

	// Other edit mode routines:
	void actualizeEditModeEditionEdges();
	float getEdgeAngle(int edgeID);
	void getNextEdges(int rearVertex,int frontVertex,std::vector<int>& edgeList);


	void actualizeLights(CViewableBase* viewable,bool forDynamicContentOnly);

	void prepareAuxClippingPlanes();
	void enableAuxClippingPlanes(int objID);
	void _prepareOrEnableAuxClippingPlanes(bool prepare,int objID);
	void disableAuxClippingPlanes();

	// Object selection info routines:
	int getMirrorNumberInSelection();
	int getShapeNumberInSelection();
	int getSimpleShapeNumberInSelection();
	int getGroupingNumberInSelection();
	int getJointNumberInSelection();
	int getGraphNumberInSelection();
	int getCameraNumberInSelection();
	int getLightNumberInSelection();
	int getDummyNumberInSelection();
	int getProxSensorNumberInSelection();
	int getVisionSensorNumberInSelection();
	int getPathNumberInSelection();
	int getVolumeNumberInSelection();
	int getMillNumberInSelection();
	int getForceSensorNumberInSelection();
	bool isLastSelectionAMirror();
	bool isLastSelectionAShape();
	bool isLastSelectionASimpleShape();
	bool isLastSelectionAGrouping();
	bool isLastSelectionAJoint();
	bool isLastSelectionAGraph();
	bool isLastSelectionACamera();
	bool isLastSelectionALight();
	bool isLastSelectionADummy();
	bool isLastSelectionAProxSensor();
	bool isLastSelectionAVisionSensor();
	bool isLastSelectionAPath();
	bool isLastSelectionAVolume();
	bool isLastSelectionAMill();
	bool isLastSelectionAForceSensor();
	C3DObject* getLastSelection();
	int getLastSelectionID();
	int getMirrorNumberInSelection(std::vector<int>* selection);
	int getShapeNumberInSelection(std::vector<int>* selection);
	int getSimpleShapeNumberInSelection(std::vector<int>* selection);
	int getGroupingNumberInSelection(std::vector<int>* selection);
	int getJointNumberInSelection(std::vector<int>* selection);
	int getGraphNumberInSelection(std::vector<int>* selection);
	int getCameraNumberInSelection(std::vector<int>* selection);
	int getLightNumberInSelection(std::vector<int>* selection);
	int getDummyNumberInSelection(std::vector<int>* selection);
	int getProxSensorNumberInSelection(std::vector<int>* selection);
	int getVisionSensorNumberInSelection(std::vector<int>* selection);
	int getPathNumberInSelection(std::vector<int>* selection);
	int getVolumeNumberInSelection(std::vector<int>* selection);
	int getMillNumberInSelection(std::vector<int>* selection);
	int getForceSensorNumberInSelection(std::vector<int>* selection);
	bool isLastSelectionAMirror(std::vector<int>* selection);
	bool isLastSelectionAShape(std::vector<int>* selection);
	bool isLastSelectionASimpleShape(std::vector<int>* selection);
	bool isLastSelectionAGrouping(std::vector<int>* selection);
	bool isLastSelectionAJoint(std::vector<int>* selection);
	bool isLastSelectionAGraph(std::vector<int>* selection);
	bool isLastSelectionACamera(std::vector<int>* selection);
	bool isLastSelectionALight(std::vector<int>* selection);
	bool isLastSelectionADummy(std::vector<int>* selection);
	bool isLastSelectionAProxSensor(std::vector<int>* selection);
	bool isLastSelectionAVisionSensor(std::vector<int>* selection);
	bool isLastSelectionAPath(std::vector<int>* selection);
	bool isLastSelectionAVolume(std::vector<int>* selection);
	bool isLastSelectionAMill(std::vector<int>* selection);
	bool isLastSelectionAForceSensor(std::vector<int>* selection);
	C3DObject* getLastSelection(std::vector<int>* selection);
	int getLastSelectionID(std::vector<int>* selection);
	void getSelectedObjects(std::vector<C3DObject*>& selection);
	void getSelectedObjects(std::vector<int>& selection);
	void setSelectedObjects(std::vector<int>& selection);

	void setLightDialogRefreshFlag();
	void setFullDialogRefreshFlag();
	int getDialogRefreshDegree();
	void resetDialogRefreshFlags();

	void announceGroupWillBeErased(int groupID);
	void announceCollisionWillBeErased(int collisionID);
	void announceDistanceWillBeErased(int distanceID);
	void announceGcsObjectWillBeErased(int gcsObjectID);
	void announce2DElementWillBeErased(int elementID);
	void announce2DElementButtonWillBeErased(int elementID,int buttonID);

	bool isObjectInSelection(int objectID);
	bool isObjectInSelection(int objectID,std::vector<int>* selection);
	void removeFromSelectionAllExceptModelBase(bool keepObjectsSelectedThatAreNotBuiltOnAModelBase);

	bool isSelectionSame(std::vector<int>& sel,bool actualize);

	int getRenderingPosition(int objID);
	void changePoseAndKeepRestInPlace(int identifier,C7Vector& m,bool changePositionX,bool changePositionY,bool changePositionZ,bool changeOrientation);


	// object, orphans, joints and dummies are further below
	std::vector<int> mirrorList;
	std::vector<int> graphList;
	std::vector<int> lightList;
	std::vector<int> cameraList;
	std::vector<int> proximitySensorList;
	std::vector<int> visionSensorList;
	std::vector<int> shapeList;
	std::vector<int> pathList;
	std::vector<int> volumeList;
	std::vector<int> millList;
	std::vector<int> forceSensorList;

//--------------------------------- for shape/path edition -----------------------------------
	
	std::vector<int> editModeBuffer;
	std::vector<float> editBufferVerticesCopy;
	std::vector<int> editBufferIndicesCopy;
	std::vector<float> editBufferNormalsCopy;
	std::vector<float> editBufferTextureCoordsCopy;

	std::vector<CSimplePathPoint*> editBufferPathPointsCopy;

	// For edit-mode:
	std::vector<float> _editionVertices;		// Used for edition
	std::vector<int> _editionIndices;			// Used for edition
	std::vector<float> _editionNormals;		// Used for edition
	std::vector<float> _editionTextureCoords; // Used for edition
	CTextureProperty* _editionTextureProperty; // Used for edition
	std::vector<unsigned char> _editionTexture; // Used for edition
	CEdgeCont _edgeCont;					// Used for edition
	std::vector<int> _editionIndicesToEdgesIndex; // Used for edition
	std::vector<int> objectsAddedDuringEditMode;
	bool showHiddenVerticeAndEdges;			// Used for edition
	bool automaticallyFollowEdges;			// Used for edition
	float edgeMaxAngle;
	float edgeDirectionChangeMaxAngle;

	CPathCont* _editionPath;				// Used for edition

	void displayVertices(int displayAttrib);
	void displayFaceOrientation(int displayAttrib);
	void displayEdgeEditMode(int displayAttrib);
	void flipTriangle(int index);
	void addTriangle(int ind1,int ind2,int ind3);
	C3Vector getRelativeVertexPosition(int index);
	C3Vector getAbsoluteVertexPosition(int index);
	void setAbsoluteVertexPosition(int index,const C3Vector& v);
	void setRelativeVertexPosition(int index,const C3Vector& v);

	void selectionFromTriangleToVertexEditMode(std::vector<int>* newVertexSel=NULL);
	void selectionFromVertexToTriangleEditMode(std::vector<int>* newTriangleSel=NULL);
	void selectionFromEdgeToTriangleEditMode(std::vector<int>* newTriangleSel=NULL);
	void selectionFromEdgeToVertexEditMode(std::vector<int>* newTriangleSel=NULL);
	void selectionFromTriangleToEdgeEditMode(std::vector<int>* newEdgeSel=NULL);
	void selectionFromVertexToEdgeEditMode(std::vector<int>* newEdgeSel=NULL);
	bool enterEditMode(int objID,int modeType);
	void endEditMode(bool cancelChanges);

	int getMainPropertyOverrideValidityNumber();
	void incrementMainPropertyOverrideValidityNumber();

	int getEditModeObjectID();
	int getEditModeType();
	void swapShapeEditModeType(int theType);
	int _editModeObject;
	int _editModeType;
	bool _modelBrowserVisibilityBeforeEditMode;
//------------------------------------------------------------------------------------------


	bool _lightDialogRefreshFlag;
	bool _fullDialogRefreshFlag;
	bool dialogRefreshDontPublishFlag;
	bool toolbarRefreshFlag;


	std::vector<int> selectedObjectsWhenSaving;

	int _ikManipulationObjectID;
	C3Vector _ikManipulationStartPosRel;
	C3Vector _ikManipulationCurrentPosAbs;

private:
	std::vector<int> _selectedObjectIDs;
	std::vector<unsigned char> _selectedObjectsBool;

	std::vector<int> _selectedPathPointIndices_nonEditMode; 
	int _uniqueSelectedPathID;



	int _mainPropertyOverrideValidityNumber;

	static float _defaultSceneID;


public:
	void newSceneProcedure();
	void removeAllObjects();
	void actualizeObjectInformation();

	int getLoadingMapping(std::vector<int>* map,int oldVal);

	int getObjectIdentifier(const std::string& objectName);
	C3DObject* getObject(int identifier);
	CDummy* getDummy(int identifier);
	CJoint* getJoint(int identifier);
	C3DObject* getObject(const std::string& name);

	bool makeObjectChildOf(C3DObject* childObject,C3DObject* parentObject);
	void setAbsoluteConfiguration(int identifier,const C7Vector& v,bool keepChildrenInPlace);

	int getHighestObjectID();
	bool eraseObject(C3DObject* it);

	void announceObjectWillBeErased(int objectID);
	void announceIkGroupWillBeErased(int ikGroupID);

	std::vector<int> objectsWhichNeedToBeErased;
	void enableObjectActualization(bool e);

	std::vector<C3DObject*> _objectIndex;

	bool _objectActualizationEnabled;

	int _nextObjectHandle;

	std::vector<int> orphanList;
	std::vector<int> objectList;
	std::vector<int> jointList;
	std::vector<int> dummyList;

private:


};

inline bool CObjCont::isObjectSelected(int identifier)
{
	if (getObject(identifier)==NULL)
		return(false);
	return((_selectedObjectsBool[identifier>>3]&(1<<(identifier&7)))!=0);
}
