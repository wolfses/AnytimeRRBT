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

#include "vrepPrecompiledHeader.h"
#include "Tt.h"
#include "ObjCont.h"

#include "Global.h"
#include "v_rep_internal.h"
#include "OGL.h"
#include "IloIlo.h"
#include "ShapeComponent.h"
#include "OglSurface.h"
#include "DlgCont.h"
#include "GeometricConstraintSolverInt.h"
#include "pluginContainer.h"
#include "geometric.h"
#include "MeshManip.h"
#include "v_repStrings.h"
#include "App.h"

CObjCont::CObjCont()
{
	_editionPath=NULL;
	showHiddenVerticeAndEdges=false;
	automaticallyFollowEdges=true;
	edgeMaxAngle=135.0f*degToRad;
	edgeDirectionChangeMaxAngle=45.0f*degToRad;

	setFullDialogRefreshFlag();
	dialogRefreshDontPublishFlag=false;
	toolbarRefreshFlag=true;
	_editModeType=NO_EDIT_MODE;
	_mainPropertyOverrideValidityNumber=0;
	_objectActualizationEnabled=true;
	_nextObjectHandle=SIM_IDSTART_3DOBJECT;

	newSceneProcedure();
}

CObjCont::~CObjCont()
{
	_ikManipulationObjectID=-1;
	delete _editionPath;
	removeAllObjects();
}


float CObjCont::_defaultSceneID=0.0f;

void CObjCont::setDefaultSceneID(float id)
{
	_defaultSceneID=id;
}

float CObjCont::getDefaultSceneID()
{
	return(_defaultSceneID);
}

float CObjCont::computeSceneID()
{ // there is no guarantee at all that two same ids really represent the same scene! Should just be used as an indication!
	float id=0.0f;
	for (int i=0;i<int(objectList.size());i++)
		id+=float(objectList[i]);
	if (objectList.size()!=0)
		id/=float(objectList.size());
	return(id);
}

int CObjCont::getMainPropertyOverrideValidityNumber()
{ // the _mainPropertyOverrideValidityNumber should be incremented every time an object is erased or the hierarchy changes!
	return(_mainPropertyOverrideValidityNumber);
}

void CObjCont::incrementMainPropertyOverrideValidityNumber()
{
	_mainPropertyOverrideValidityNumber++;
}

void CObjCont::simulationAboutToStart()
{
	_selectedPathPointIndices_nonEditMode.clear();
	incrementMainPropertyOverrideValidityNumber();
	for (int i=0;i<int(objectList.size());i++)
		getObject(objectList[i])->simulationAboutToStart();
}

void CObjCont::simulationEnded()
{
	for (int i=0;i<int(objectList.size());i++)
		getObject(objectList[i])->simulationEnded();
	incrementMainPropertyOverrideValidityNumber();
}

void CObjCont::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

void CObjCont::setLightDialogRefreshFlag()
{
	_lightDialogRefreshFlag=true;
}

void CObjCont::setFullDialogRefreshFlag()
{
	_fullDialogRefreshFlag=true;
}

int CObjCont::getDialogRefreshDegree()
{
	int retVal=-1;
	if (_lightDialogRefreshFlag)
		retVal=0;
	if (_fullDialogRefreshFlag)
		retVal=2;
	return(retVal);
}

void CObjCont::resetDialogRefreshFlags()
{
	_lightDialogRefreshFlag=false;
	_fullDialogRefreshFlag=false;
}

bool CObjCont::loadModelOrScene(CSer& ar,bool selectLoaded,bool isScene,bool justLoadThumbnail)
{	// return val is success or fail
	// Should always be called through 'loadModel' or 'loadScene'!!!!

	deselectObjects();

	std::vector<C3DObject*> loadedObjectList;
	std::vector<CTextureObject*> loadedTextureList;
	std::vector<CDynMaterialObject*> loadedDynMaterialList;
	std::vector<CRegGroup*> loadedGroupList;
	std::vector<CRegColl*> loadedCollisionList;
	std::vector<CRegDist*> loadedDistanceList;
	std::vector<CikGroup*> loadedIkGroupList;
	std::vector<CPathPlanningTask*> pathPlanningTaskList;
	std::vector<CMotionPlanningTask*> motionPlanningTaskList;
	std::vector<CButtonBlock*> loadedButtonBlockList;
	std::vector<CLuaScriptObject*> loadedLuaScriptList;
	std::vector<CConstraintSolverObject*> loadedConstraintSolverObjectList;
	std::vector<int> renderingOrderList;


	int byteQuantity;
	std::string theName="";
	bool hasThumbnail=false;
	while (theName.compare(SER_END_OF_FILE)!=0)
	{
		theName=ar.readDataName();
		if (theName.compare(SER_END_OF_FILE)!=0)
		{
			bool noHit=true;
			if (theName.compare(SER_MODEL_THUMBNAIL)==0)
			{
				ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
				App::ct->environment->modelThumbnail_notSerializedHere.serialize(ar);
				noHit=false;
				if (justLoadThumbnail)
					return(true);
				hasThumbnail=true;
			}
			else
			{ // This happens with older files (before 2012/03/06)
				if (theName.compare(SER_MODEL_THUMBNAIL_OLD_COMPATIBILITY_2012_03_06)==0)
				{
					ar >> byteQuantity;
					int l;
					ar >> l;
					char t;
					std::vector<char> modelThumbnailBuffer;
					for (int i=0;i<l;i++)
					{
						ar >> t;
						modelThumbnailBuffer.push_back(t);
					}
					if (modelThumbnailBuffer.size()!=0)
						App::ct->environment->modelThumbnail_notSerializedHere.setCompressedThumbnailImage(&modelThumbnailBuffer[0]);
					else
						App::ct->environment->modelThumbnail_notSerializedHere.clearThumbnailImage();
					noHit=false;
					if (justLoadThumbnail)
						return(true);
					hasThumbnail=true;
				}
				else
				{ // This happens with older files (before 2010/07/25)
					if (!hasThumbnail)
						App::ct->environment->modelThumbnail_notSerializedHere.clearThumbnailImage();
					if (justLoadThumbnail)
						return(true);
				}
			}

			C3DObject* it=load3DObject(ar,theName,noHit);
			if (it!=NULL) 
			{
				loadedObjectList.push_back(it);
				noHit=false;
			}

			CTextureObject* theTextureData=App::ct->textureCont->loadTextureObject(ar,theName,noHit);
			if (theTextureData!=NULL)
			{
				loadedTextureList.push_back(theTextureData);
				noHit=false;
			}
			CDynMaterialObject* theDynMaterialData=App::ct->dynMaterialCont->loadDynMaterialObject(ar,theName,noHit);
			if (theDynMaterialData!=NULL)
			{
				loadedDynMaterialList.push_back(theDynMaterialData);
				noHit=false;
			}
			if (theName.compare(SER_GHOSTS)==0)
			{
				ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
				App::ct->ghostObjectCont->serialize(ar);
				noHit=false;
			}
			if (theName.compare(SER_ENVIRONMENT)==0)
			{
				ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
				App::ct->environment->serialize(ar,true);
				noHit=false;
			}
			if (theName.compare(SER_SETTINGS)==0)
			{
				ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
				App::ct->mainSettings->serialize(ar);
				noHit=false;
			}
			if (theName.compare(SER_DYNAMICS)==0)
			{
				ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
				App::ct->dynamicsContainer->serialize(ar);
				noHit=false;
			}
			if (theName.compare(SER_SIMULATION)==0)
			{
				ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
				App::ct->simulation->serialize(ar);
				noHit=false;
				
				// For backward compatibility (3/1/2012):
				//************************************************
				if (App::ct->mainSettings->forBackwardCompatibility_03_01_2012_stillUsingStepSizeDividers)
				{ // This needs to be done AFTER simulation settings are loaded!
					float step=App::ct->simulation->getSimulationTimeStep_speedModified();
					float bulletStepSize=step/float(App::ct->mainSettings->dynamicsBULLETStepSizeDivider_forBackwardCompatibility_03_01_2012);
					float odeStepSize=step/float(App::ct->mainSettings->dynamicsODEStepSizeDivider_forBackwardCompatibility_03_01_2012);
					if (fabs(step-0.05f)>0.002f)
						App::ct->dynamicsContainer->setUseDynamicDefaultCalculationParameters(4); // use custom settings
					// Following has an effect only when using custom parameters (custom parameters might already be enabled before above line!):
					App::ct->dynamicsContainer->setDynamicsStepSize(bulletStepSize,false);
					App::ct->dynamicsContainer->setODEDynamicsStepSize(odeStepSize,false);
				}
				//************************************************
			}
			if (theName.compare(SER_VIEWS)==0)
			{
				ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
				App::ct->oglSurface->pageCont->serialize(ar);
				noHit=false;
			}
			if (theName.compare(SER_MODEL_ACKNOWLEDGEMENTOLD_20100714)==0)
			{
				ar >> byteQuantity;
				std::string ack;
				ar >> ack;
				CIloIlo::setModelAcknowledgementOLD_20100714(ack);
				noHit=false;
			}
			if (theName.compare(SER_GROUP)==0)
			{
				ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
				CRegGroup* it=new CRegGroup("Default");
				it->serialize(ar);
				loadedGroupList.push_back(it);
				noHit=false;
			}
			if (theName.compare(SER_BUTTON_BLOCK)==0)
			{
				ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
				CButtonBlock* it=new CButtonBlock(1,1,10,10,0);
				it->serialize(ar);
				loadedButtonBlockList.push_back(it);
				noHit=false;
			}
			if (theName.compare(SER_LUA_SCRIPT)==0)
			{
				ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
				CLuaScriptObject* it=new CLuaScriptObject(true);
				it->serialize(ar);
				loadedLuaScriptList.push_back(it);
				noHit=false;
			}
			if (theName.compare(SER_GEOMETRIC_CONSTRAINT_OBJECT)==0)
			{
				ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
				CConstraintSolverObject* it=new CConstraintSolverObject();
				it->serialize(ar);
				loadedConstraintSolverObjectList.push_back(it);
				noHit=false;
			}
			if (theName.compare(SER_SCENE_CUSTOM_DATA)==0)
			{
				ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
				App::ct->customSceneData->serializeData(ar);
				noHit=false;
			}
			if (theName.compare(SER_COLLISION)==0)
			{
				ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
				CRegColl* it=new CRegColl(0,0,"",0);
				it->serialize(ar);
				loadedCollisionList.push_back(it);
				noHit=false;
			}
			if (theName.compare(SER_DISTANCE)==0)
			{
				ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
				CRegDist* it=new CRegDist(0,0,"",0);
				it->serialize(ar);
				loadedDistanceList.push_back(it);
				noHit=false;
			}
			if (theName.compare(SER_IK)==0)
			{
				ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
				CikGroup* it=new CikGroup();
				it->serialize(ar);
				loadedIkGroupList.push_back(it);
				noHit=false;
			}
			if (theName==SER_PATH_PLANNING)
			{
				ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
				CPathPlanningTask* it=new CPathPlanningTask();
				it->serialize(ar);
				pathPlanningTaskList.push_back(it);
				noHit=false;
			}
			if (theName==SER_MOTION_PLANNING)
			{
				ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
				CMotionPlanningTask* it=new CMotionPlanningTask();
				it->serialize(ar);
				motionPlanningTaskList.push_back(it);
				noHit=false;
			}
			if (noHit)
				ar.loadUnknownData();
		}
	}

	// All object have been loaded and are in:
	// loadedObjectList
	// loadedGroupList
	// ...

	addObjectsToSceneAndPerformMappings(&loadedObjectList,
										&loadedGroupList,
										&loadedCollisionList,
										&loadedDistanceList,
										&loadedIkGroupList,
										&pathPlanningTaskList,
										&motionPlanningTaskList,
										&loadedButtonBlockList,
										&loadedLuaScriptList,
										&loadedConstraintSolverObjectList,
										loadedTextureList,
										loadedDynMaterialList,
										!isScene);

	// Following to avoid the flickering when loading something (also during undo/redo):
	for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
		it->bufferMainDisplayStateVariables();
		it->bufferedMainDisplayStateVariablesToDisplay();
	}

	return(true);
}

void CObjCont::addObjectsToSceneAndPerformMappings(std::vector<C3DObject*>* loadedObjectList,
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
													bool model)
{
	FUNCTION_DEBUG;
	// We check what suffix offset is needed for this model (in case of a scene, the offset is ignored since we won't introduce the objects as copies!):
	int suffixOffset=getSuffixOffsetForObjectToAdd(loadedObjectList,
		loadedGroupList,
		loadedCollisionList,
		loadedDistanceList,
		loadedIkGroupList,
		loadedPathPlanningTaskList,
		loadedMotionPlanningTaskList,
		loadedButtonBlockList,
		loadedLuaScriptList,
		loadedConstraintSolverObjectList);


	// We have 3 cases:
	// 1. We are loading a scene, 2. We are loading a model, 3. We are pasting objects
	// We add objects to the scene as copies only if we also add at least one child script and we don't have a scene. Otherwise objects are added
	// and no '#' (or no modified suffix) will appear in their names.
	// Following line summarizes this:
	bool objectIsACopy=((loadedLuaScriptList->size()!=0)&&model); // scenes are not treated like copies!

	std::vector<int> allInitAndEndSuffix;
	std::vector<std::string> allNamesWithTypes;

	// Texture data:
	std::vector<int> textureMapping;
	for (int i=0;i<int(loadedTextureObjectList.size());i++)
	{
		textureMapping.push_back(loadedTextureObjectList[i]->getObjectID());
		CTextureObject* handler=App::ct->textureCont->getObject(App::ct->textureCont->addObjectWithSuffixOffset(loadedTextureObjectList[i],objectIsACopy,suffixOffset)); // if a same object is found, the object is destroyed in addObject!
		if (handler!=loadedTextureObjectList[i])
			loadedTextureObjectList[i]=handler; // this happens when a similar object is already present
		textureMapping.push_back(handler->getObjectID());
	}
	prepareFastLoadingMapping(textureMapping);

	// Dyn Material data:
	std::vector<int> dynMaterialMapping;
	for (int i=0;i<int(loadedDynMaterialObjectList.size());i++)
	{
		dynMaterialMapping.push_back(loadedDynMaterialObjectList[i]->getObjectID());
		CDynMaterialObject* handler=App::ct->dynMaterialCont->getObject(App::ct->dynMaterialCont->addObject(loadedDynMaterialObjectList[i],false)); // if a same object is found, the object is destroyed in addObject!
		if (handler!=loadedDynMaterialObjectList[i])
			loadedDynMaterialObjectList[i]=handler; // this happens when a similar object is already present
		dynMaterialMapping.push_back(handler->getObjectID());
	}
	prepareFastLoadingMapping(dynMaterialMapping);

	// We add all 3DObjects:
	enableObjectActualization(false);
	std::vector<int> objectMapping;
	for (int i=0;i<int(loadedObjectList->size());i++)
	{
		objectMapping.push_back(loadedObjectList->at(i)->getID()); // Old ID
		std::string initName(loadedObjectList->at(i)->getName());
		addObjectToSceneWithSuffixOffset(loadedObjectList->at(i),objectIsACopy,suffixOffset);
		objectMapping.push_back(loadedObjectList->at(i)->getID()); // New ID
	}
	prepareFastLoadingMapping(objectMapping);
	enableObjectActualization(true);
	actualizeObjectInformation();

	// We add all the collections:
	std::vector<int> groupMapping;
	for (int i=0;i<int(loadedGroupList->size());i++)
	{
		groupMapping.push_back(loadedGroupList->at(i)->getGroupID()); // Old ID
		std::string initName(loadedGroupList->at(i)->getGroupName());
		App::ct->collections->addGroupWithSuffixOffset(loadedGroupList->at(i),objectIsACopy,suffixOffset);
		groupMapping.push_back(loadedGroupList->at(i)->getGroupID()); // New ID
	}
	prepareFastLoadingMapping(groupMapping);

	// We add all the collisions:
	std::vector<int> collisionMapping;
	for (int i=0;i<int(loadedCollisionList->size());i++)
	{
		collisionMapping.push_back(loadedCollisionList->at(i)->getObjectID()); // Old ID
		std::string initName(loadedCollisionList->at(i)->getObjectName());
		App::ct->collisions->addObjectWithSuffixOffset(loadedCollisionList->at(i),objectIsACopy,suffixOffset);
		collisionMapping.push_back(loadedCollisionList->at(i)->getObjectID()); // New ID
	}
	prepareFastLoadingMapping(collisionMapping);

	// We add all the distances:
	std::vector<int> distanceMapping;
	for (int i=0;i<int(loadedDistanceList->size());i++)
	{
		distanceMapping.push_back(loadedDistanceList->at(i)->getObjectID()); // Old ID
		std::string initName(loadedDistanceList->at(i)->getObjectName());
		App::ct->distances->addObjectWithSuffixOffset(loadedDistanceList->at(i),objectIsACopy,suffixOffset);
		distanceMapping.push_back(loadedDistanceList->at(i)->getObjectID()); // New ID
	}
	prepareFastLoadingMapping(distanceMapping);

	// We add all the ik groups:
	std::vector<int> ikGroupMapping;
	for (int i=0;i<int(loadedIkGroupList->size());i++)
	{
		ikGroupMapping.push_back(loadedIkGroupList->at(i)->getObjectID()); // Old ID
		std::string initName(loadedIkGroupList->at(i)->getObjectName());
		App::ct->ikGroups->addIkGroupWithSuffixOffset(loadedIkGroupList->at(i),objectIsACopy,suffixOffset);
		ikGroupMapping.push_back(loadedIkGroupList->at(i)->getObjectID()); // New ID
	}
	prepareFastLoadingMapping(ikGroupMapping);

	// We add all the path planning tasks:
	std::vector<int> pathPlanningTaskMapping;
	for (int i=0;i<int(loadedPathPlanningTaskList->size());i++)
	{
		pathPlanningTaskMapping.push_back(loadedPathPlanningTaskList->at(i)->getObjectID()); // Old ID
		std::string initName(loadedPathPlanningTaskList->at(i)->getObjectName());
		App::ct->pathPlanning->addObjectWithSuffixOffset(loadedPathPlanningTaskList->at(i),objectIsACopy,suffixOffset);
		pathPlanningTaskMapping.push_back(loadedPathPlanningTaskList->at(i)->getObjectID()); // New ID
	}
	prepareFastLoadingMapping(pathPlanningTaskMapping);

	// We add all the motion planning tasks:
	std::vector<int> motionPlanningTaskMapping;
	for (int i=0;i<int(loadedMotionPlanningTaskList->size());i++)
	{
		motionPlanningTaskMapping.push_back(loadedMotionPlanningTaskList->at(i)->getObjectID()); // Old ID
		std::string initName(loadedMotionPlanningTaskList->at(i)->getObjectName());
		App::ct->motionPlanning->addObjectWithSuffixOffset(loadedMotionPlanningTaskList->at(i),objectIsACopy,suffixOffset);
		motionPlanningTaskMapping.push_back(loadedMotionPlanningTaskList->at(i)->getObjectID()); // New ID
	}
	prepareFastLoadingMapping(motionPlanningTaskMapping);

	// We add all the button blocks:
	std::vector<int> buttonBlockMapping;
	for (int i=0;i<int(loadedButtonBlockList->size());i++)
	{
		buttonBlockMapping.push_back(loadedButtonBlockList->at(i)->getBlockID()); // Old ID
		std::string initName(loadedButtonBlockList->at(i)->getBlockName());
		App::ct->buttonBlockContainer->insertBlockWithSuffixOffset(loadedButtonBlockList->at(i),objectIsACopy,suffixOffset);
		buttonBlockMapping.push_back(loadedButtonBlockList->at(i)->getBlockID()); // New ID
	}
	prepareFastLoadingMapping(buttonBlockMapping);

	// We add all the scripts:
	std::vector<int> luaScriptMapping;
	for (int i=0;i<int(loadedLuaScriptList->size());i++)
	{
		luaScriptMapping.push_back(loadedLuaScriptList->at(i)->getScriptID()); // Old ID
		App::ct->luaScriptContainer->insertScript(loadedLuaScriptList->at(i));
		luaScriptMapping.push_back(loadedLuaScriptList->at(i)->getScriptID()); // New ID
	}
	prepareFastLoadingMapping(luaScriptMapping);

	// We add all the GCS objects:
	std::vector<int> constraintSolverObjectMapping;
	for (int i=0;i<int(loadedConstraintSolverObjectList->size());i++)
	{
		constraintSolverObjectMapping.push_back(loadedConstraintSolverObjectList->at(i)->getObjectID()); // Old ID
		std::string initName(loadedConstraintSolverObjectList->at(i)->getObjectName());
		App::ct->constraintSolver->addObjectWithSuffixOffset(loadedConstraintSolverObjectList->at(i),objectIsACopy,suffixOffset);
		constraintSolverObjectMapping.push_back(loadedConstraintSolverObjectList->at(i)->getObjectID()); // New ID
	}
	prepareFastLoadingMapping(constraintSolverObjectMapping);


	enableObjectActualization(false);

	// We do the mapping for the 3DObjects:
	for (int i=0;i<int(loadedObjectList->size());i++)
	{
		C3DObject* it=loadedObjectList->at(i);
		it->performObjectLoadingMapping(&objectMapping);
		it->performGroupLoadingMapping(&groupMapping);
		it->performCollisionLoadingMapping(&collisionMapping);
		it->performDistanceLoadingMapping(&distanceMapping);
		it->performIkLoadingMapping(&ikGroupMapping);
		it->performGcsLoadingMapping(&constraintSolverObjectMapping);
		it->performTextureObjectLoadingMapping(&textureMapping);
		it->performDynMaterialObjectLoadingMapping(&dynMaterialMapping);
	}

	// We do the mapping for the collections:
	for (int i=0;i<int(loadedGroupList->size());i++)
	{
		CRegGroup* it=loadedGroupList->at(i);
		it->performObjectLoadingMapping(&objectMapping);
	}
	// We do the mapping for the collisions:
	for (int i=0;i<int(loadedCollisionList->size());i++)
	{
		CRegColl* it=loadedCollisionList->at(i);
		it->performObjectLoadingMapping(&objectMapping);
		it->performGroupLoadingMapping(&groupMapping);
	}
	// We do the mapping for the distances:
	for (int i=0;i<int(loadedDistanceList->size());i++)
	{
		CRegDist* it=loadedDistanceList->at(i);
		it->performObjectLoadingMapping(&objectMapping);
		it->performGroupLoadingMapping(&groupMapping);
	}
	// We do the mapping for the ik groups:
	for (int i=0;i<int(loadedIkGroupList->size());i++)
	{
		CikGroup* it=loadedIkGroupList->at(i);
		it->performObjectLoadingMapping(&objectMapping);
		it->performGroupLoadingMapping(&groupMapping);
		it->performIkGroupLoadingMapping(&ikGroupMapping);
	}
	// We do the mapping for the path planning tasks:
	for (int i=0;i<int(loadedPathPlanningTaskList->size());i++)
	{
		CPathPlanningTask* it=loadedPathPlanningTaskList->at(i);
		it->performObjectLoadingMapping(&objectMapping);
		it->performGroupLoadingMapping(&groupMapping);
	}
	// We do the mapping for the motion planning tasks:
	for (int i=0;i<int(loadedMotionPlanningTaskList->size());i++)
	{
		CMotionPlanningTask* it=loadedMotionPlanningTaskList->at(i);
		it->performObjectLoadingMapping(&objectMapping);
		it->performGroupLoadingMapping(&groupMapping);
		it->performIkGroupLoadingMapping(&ikGroupMapping);
	}
	// We do the mapping for the 2D Elements:
	for (int i=0;i<int(loadedButtonBlockList->size());i++)
	{
		CButtonBlock* it=loadedButtonBlockList->at(i);
		it->perform3DObjectLoadingMapping(&objectMapping);
		it->performTextureObjectLoadingMapping(&textureMapping);
	}
	// We do the mapping for the Lua scripts:
	for (int i=0;i<int(loadedLuaScriptList->size());i++)
	{
		CLuaScriptObject* it=loadedLuaScriptList->at(i);
		it->perform3DObjectLoadingMapping(&objectMapping);
	}
	// We do the mapping for the GCS objects:
	for (int i=0;i<int(loadedConstraintSolverObjectList->size());i++)
	{
		CConstraintSolverObject* it=loadedConstraintSolverObjectList->at(i);
		it->performObjectLoadingMapping(&objectMapping);
	}

	// We do the mapping for the ghost objects:
	if (!model)
		App::ct->ghostObjectCont->performObjectLoadingMapping(&objectMapping);

	// Backward compatibility (12/9/2013): before V-REP 3.0.5, each shape had individual material properties
	// Since 3.0.5, each shape points onto a material property, thus, material properties can be shared.
	// Here we need to take care of shapes that were saved in a version before 3.0.5:
	for (int i=0;i<int(loadedObjectList->size());i++)
	{
		C3DObject* it=loadedObjectList->at(i);
		if (it->getObjectType()==sim_object_shape_type)
		{
			CShape* shape=(CShape*)it;
			int dynMatId=shape->geomData->geomInfo->getDynMaterialId();
			if (dynMatId==-1)
			{ // that shape hasn't any material yet!
				CDynMaterialObject* material=shape->geomData->geomInfo->createDynMaterialObject();
				int dynMatObjId=App::ct->dynMaterialCont->addObject(material,false);
				shape->geomData->geomInfo->setDynMaterialId(dynMatObjId);
			}
		}
	}


	// We set ALL texture object dependencies (not just for loaded objects):
	// We cannot use App::ct->textureCont->updateAllDependencies, since the shape list is not yet actualized!
	App::ct->textureCont->clearAllDependencies();
	App::ct->buttonBlockContainer->setTextureDependencies();
	for (int i=0;i<int(_objectIndex.size());i++) // here we cannot use shapeList, because not yet actualized!!
	{
		if ( (_objectIndex[i]!=NULL)&&(_objectIndex[i]->getObjectType()==sim_object_shape_type) )
			((CShape*)_objectIndex[i])->geomData->setTextureDependencies(_objectIndex[i]->getID());
	}


	enableObjectActualization(true);
	actualizeObjectInformation();

	if (!model)
		App::ct->oglSurface->pageCont->performObjectLoadingMapping(&objectMapping);

	actualizeObjectInformation();

	// Now clean-up suffixes equal or above those added (global suffix clean-up can be done in the environment dialog):
	cleanupDashNames(suffixOffset-1);

/* Until 4/10/2013. Global suffix name clean-up. This was confusing!
	if (App::ct->simulation->isSimulationStopped()) // added on 2010/02/20 (otherwise objects can get automatically renamed during simulation!!)
		cleanupDashNames(-1);
*/

//************ We need to initialize all object types (also because an object copied during simulation hasn't the simulationEnded routine called!)
	bool simulationRunning=!App::ct->simulation->isSimulationStopped();
	for (int i=0;i<int(loadedObjectList->size());i++)
		loadedObjectList->at(i)->initializeInitialValues(simulationRunning);
	for (int i=0;i<int(loadedButtonBlockList->size());i++)
		loadedButtonBlockList->at(i)->initializeInitialValues(simulationRunning);
	for (int i=0;i<int(loadedCollisionList->size());i++)
		loadedCollisionList->at(i)->initializeInitialValues(simulationRunning);
	for (int i=0;i<int(loadedDistanceList->size());i++)
		loadedDistanceList->at(i)->initializeInitialValues(simulationRunning);
	for (int i=0;i<int(loadedGroupList->size());i++)
		loadedGroupList->at(i)->initializeInitialValues(simulationRunning);
	for (int i=0;i<int(loadedIkGroupList->size());i++)
		loadedIkGroupList->at(i)->initializeInitialValues(simulationRunning);
	for (int i=0;i<int(loadedPathPlanningTaskList->size());i++)
		loadedPathPlanningTaskList->at(i)->initializeInitialValues(simulationRunning);
	for (int i=0;i<int(loadedMotionPlanningTaskList->size());i++)
		loadedMotionPlanningTaskList->at(i)->initializeInitialValues(simulationRunning);
	for (int i=0;i<int(loadedLuaScriptList->size());i++)
		loadedLuaScriptList->at(i)->initializeInitialValues(simulationRunning);
	for (int i=0;i<int(loadedConstraintSolverObjectList->size());i++)
		loadedConstraintSolverObjectList->at(i)->initializeInitialValues(simulationRunning);
	// Here we call the initializeInitialValues for all pages & views
	if (App::ct->oglSurface!=NULL)
	{
		for (int i=0;i<int(loadedObjectList->size());i++)
			App::ct->oglSurface->initializeInitialValues(simulationRunning,loadedObjectList->at(i)->getID());
	}
//**************************************************************************************

	// We select what was loaded if we have a model loaded through the GUI:
	deselectObjects();
	if (model)
	{
		for (int i=0;i<int(loadedObjectList->size());i++)
			addObjectToSelection(loadedObjectList->at(i)->getID());
	}
}

int CObjCont::getSuffixOffsetForObjectToAdd(std::vector<C3DObject*>* loadedObjectList,
												 std::vector<CRegGroup*>* loadedGroupList,
												 std::vector<CRegColl*>* loadedCollisionList,
												 std::vector<CRegDist*>* loadedDistanceList,
												 std::vector<CikGroup*>* loadedIkGroupList,
												 std::vector<CPathPlanningTask*>* loadedPathPlanningTaskList,
												 std::vector<CMotionPlanningTask*>* loadedMotionPlanningTaskList,
												 std::vector<CButtonBlock*>* loadedButtonBlockList,
												 std::vector<CLuaScriptObject*>* loadedLuaScriptList,
												 std::vector<CConstraintSolverObject*>* loadedConstraintSolverObjectList)
{
	// 1. We find out about the smallest suffix to paste:
	int smallestSuffix=SIM_MAX_INT;
	// 3DObjects:
	for (int i=0;i<int(loadedObjectList->size());i++)
	{
		int s=tt::getNameSuffixNumber(loadedObjectList->at(i)->getName().c_str(),true);
		if (i==0)
			smallestSuffix=s;
		else
		{
			if (s<smallestSuffix)
				smallestSuffix=s;
		}
	}
	// Collections:
	for (int i=0;i<int(loadedGroupList->size());i++)
	{
		int s=tt::getNameSuffixNumber(loadedGroupList->at(i)->getGroupName().c_str(),true);
		if (s<smallestSuffix)
			smallestSuffix=s;
	}
	// Collisions:
	for (int i=0;i<int(loadedCollisionList->size());i++)
	{
		int s=tt::getNameSuffixNumber(loadedCollisionList->at(i)->getObjectName().c_str(),true);
		if (s<smallestSuffix)
			smallestSuffix=s;
	}
	// Distances:
	for (int i=0;i<int(loadedDistanceList->size());i++)
	{
		int s=tt::getNameSuffixNumber(loadedDistanceList->at(i)->getObjectName().c_str(),true);
		if (s<smallestSuffix)
			smallestSuffix=s;
	}
	// IK Groups:
	for (int i=0;i<int(loadedIkGroupList->size());i++)
	{
		int s=tt::getNameSuffixNumber(loadedIkGroupList->at(i)->getObjectName().c_str(),true);
		if (s<smallestSuffix)
			smallestSuffix=s;
	}
	// Path planning tasks:
	for (int i=0;i<int(loadedPathPlanningTaskList->size());i++)
	{
		int s=tt::getNameSuffixNumber(loadedPathPlanningTaskList->at(i)->getObjectName().c_str(),true);
		if (s<smallestSuffix)
			smallestSuffix=s;
	}
	// Motion planning tasks:
	for (int i=0;i<int(loadedMotionPlanningTaskList->size());i++)
	{
		int s=tt::getNameSuffixNumber(loadedMotionPlanningTaskList->at(i)->getObjectName().c_str(),true);
		if (s<smallestSuffix)
			smallestSuffix=s;
	}
	// 2D Elements:
	for (int i=0;i<int(loadedButtonBlockList->size());i++)
	{
		int s=tt::getNameSuffixNumber(loadedButtonBlockList->at(i)->getBlockName().c_str(),true);
		if (s<smallestSuffix)
			smallestSuffix=s;
	}
	// GCS objects:
	for (int i=0;i<int(loadedConstraintSolverObjectList->size());i++)
	{
		int s=tt::getNameSuffixNumber(loadedConstraintSolverObjectList->at(i)->getObjectName().c_str(),true);
		if (s<smallestSuffix)
			smallestSuffix=s;
	}

	// 2. Now we find out about the highest suffix among existing objects (already in the scene):
	int biggestSuffix,smallestSuffixDummy;

	_getAllObjectsSmallestAndBiggestSuffix(smallestSuffixDummy,biggestSuffix);
	return(biggestSuffix-smallestSuffix+1);
}

bool CObjCont::loadModel(CSer& ar,bool justLoadThumbnail)
{	// return value is success or fail
	bool retVal=loadModelOrScene(ar,true,false,justLoadThumbnail);
	if (!justLoadThumbnail)
	{
		void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_modelloaded,NULL,NULL,NULL);
		delete[] (char*)returnVal;
		returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_model_loaded,NULL,NULL,NULL); // for backward compatibility
		delete[] (char*)returnVal;

		App::ct->outsideCommandQueue->addCommand(sim_message_model_loaded,0,0,0,0,NULL,0); // only for Lua
		App::ct->setModificationFlag(4); // model loaded
	}
	return(retVal);
}

C3DObject* CObjCont::load3DObject(CSer& ar,std::string theName,bool &noHit)
{
	int byteQuantity;
	if (theName.compare(SER_SHAPE)==0)
	{
		ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
		CShape* myNewObject=new CShape();
		myNewObject->serialize(ar);
		noHit=false;
		return(myNewObject);
	}
	if (theName.compare(SER_JOINT)==0)
	{
		ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
		CJoint* myNewObject=new CJoint();
		myNewObject->serialize(ar);
		noHit=false;
		return(myNewObject);
	}
	if (theName.compare(SER_GRAPH)==0)
	{
		ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
		CGraph* myNewObject=new CGraph();
		myNewObject->serialize(ar);
		noHit=false;
		return(myNewObject);
	}
	if (theName.compare(SER_CAMERA)==0)
	{
		ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
		CCamera* myNewObject=new CCamera();
		myNewObject->serialize(ar);
		noHit=false;
		return(myNewObject);
	}
	if (theName.compare(SER_LIGHT)==0)
	{
		ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
		CLight* myNewObject=new CLight();
		myNewObject->serialize(ar);
		noHit=false;
		return(myNewObject);
	}
	if (theName.compare(SER_MIRROR)==0)
	{
		ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
		CMirror* myNewObject=new CMirror();
		myNewObject->serialize(ar);
		noHit=false;
		return(myNewObject);
	}
	if (theName.compare(SER_DUMMY)==0)
	{
		ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
		CDummy* myNewObject=new CDummy();
		myNewObject->serialize(ar);
		noHit=false;
		return(myNewObject);
	}
	if (theName.compare(SER_PROXIMITYSENSOR)==0)
	{
		ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
		CProxSensor* myNewObject=new CProxSensor();
		myNewObject->serialize(ar);
		noHit=false;
		return(myNewObject);
	}
	if (theName.compare(SER_VISIONSENSOR)==0)
	{
		ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
		CVisionSensor* myNewObject=new CVisionSensor();
		myNewObject->serialize(ar);
		noHit=false;
		return(myNewObject);
	}
	if (theName.compare(SER_PATH)==0)
	{
		ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
		CPath* myNewObject=new CPath();
		myNewObject->serialize(ar);
		noHit=false;
		return(myNewObject);
	}
	if (theName.compare(SER_VOLUME)==0)
	{
		ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
		CVolumeObject* myNewObject=new CVolumeObject();
		myNewObject->serialize(ar);
		noHit=false;
		return(myNewObject);
	}
	if (theName.compare(SER_MILL)==0)
	{
		ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
		CMill* myNewObject=new CMill();
		myNewObject->serialize(ar);
		noHit=false;
		return(myNewObject);
	}
	if (theName.compare(SER_FORCESENSOR)==0)
	{ 
		ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
		CForceSensor* myNewObject=new CForceSensor();
		myNewObject->serialize(ar);
		noHit=false;
		return(myNewObject);
	}
	// If we arrived here it means that maybe we have a new 3DObject type that this V-REP doesn't understand yet. 
	// We try to replace it with a dummy (2009/12/09):
	BYTE dat[14];
	if (ar.readBytesButKeepPointerUnchanged(dat,14)!=14)
		return(NULL); // No, this is not a 3DObject! (not enough to read)
	if ((dat[4]=='3')&&(dat[5]=='d')&&(dat[6]=='o')&&(dat[11]==57)&&(dat[12]==58)&&(dat[13]==59))
	{ // yes we have a 3DObject of an unknown type!
		ar >> byteQuantity; // Undo/redo will never arrive here
		CDummy* newUnknownType=new CDummy();
		newUnknownType->loadUnknownObjectType(ar);
		noHit=false;
		return(newUnknownType);
	}
	return(NULL); // No, this is not a 3DObject!
}

bool CObjCont::loadScene(CSer& ar,bool forUndoRedoOperation)
{ // return val is success or fail
	removeAllObjects();
	bool retVal=loadModelOrScene(ar,false,true,false);
	if (!forUndoRedoOperation)
	{
		void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_sceneloaded,NULL,NULL,NULL);
		delete[] (char*)returnVal;
		App::ct->setModificationFlag(8); // scene loaded
		App::ct->outsideCommandQueue->addCommand(sim_message_scene_loaded,0,0,0,0,NULL,0);
	}
	return(retVal);
}

void CObjCont::store3DObject(CSer& ar,C3DObject* it)
{
	if (it->getObjectType()==sim_object_shape_type)
		ar.storeDataName(SER_SHAPE);
	if (it->getObjectType()==sim_object_joint_type)
		ar.storeDataName(SER_JOINT);
	if (it->getObjectType()==sim_object_graph_type)
		ar.storeDataName(SER_GRAPH);
	if (it->getObjectType()==sim_object_camera_type)
		ar.storeDataName(SER_CAMERA);
	if (it->getObjectType()==sim_object_light_type)
		ar.storeDataName(SER_LIGHT);
	if (it->getObjectType()==sim_object_mirror_type)
		ar.storeDataName(SER_MIRROR);
	if (it->getObjectType()==sim_object_dummy_type)
		ar.storeDataName(SER_DUMMY);
	if (it->getObjectType()==sim_object_proximitysensor_type)
		ar.storeDataName(SER_PROXIMITYSENSOR);
	if (it->getObjectType()==sim_object_visionsensor_type)
		ar.storeDataName(SER_VISIONSENSOR);
	if (it->getObjectType()==sim_object_path_type)
		ar.storeDataName(SER_PATH);
	if (it->getObjectType()==sim_object_volume_type)
		ar.storeDataName(SER_VOLUME);
	if (it->getObjectType()==sim_object_mill_type)
		ar.storeDataName(SER_MILL);
	if (it->getObjectType()==sim_object_forcesensor_type)
		ar.storeDataName(SER_FORCESENSOR);
	ar.setCountingMode();
	it->serialize(ar);
	if (ar.setWritingMode())
		it->serialize(ar);
}

C3DObject* CObjCont::getObjectWithUniqueID(int uniqueID)
{
	for (int i=0;i<int(objectList.size());i++)
	{
		C3DObject* it=getObject(objectList[i]);
		if (it!=NULL)
		{
			if (it->getUniqueID()==uniqueID)
				return(it);
		}
	}
	return(NULL);
}

std::string CObjCont::getSimilarNameWithHighestSuffix(std::string objectName,bool dash)
{
	std::string nameWithoutSuffix(tt::getNameWithoutSuffixNumber(objectName.c_str(),dash));
	int highestSuffixNb=-2;
	std::string highestSuffixName;
	for (int i=0;i<int(objectList.size());i++)
	{
		C3DObject* it=getObject(objectList[i]);
		if (it!=NULL)
		{
			std::string nm(it->getName());
			if (tt::getNameWithoutSuffixNumber(nm.c_str(),dash).compare(nameWithoutSuffix)==0)
			{
				int suffixNb=tt::getNameSuffixNumber(nm.c_str(),dash);
				if (suffixNb>highestSuffixNb)
				{
					highestSuffixNb=suffixNb;
					highestSuffixName=nm;
				}
			}
		}
	}
	if (highestSuffixNb==-2)
		return(""); // none
	return(highestSuffixName);
}

void CObjCont::saveScene(CSer& ar)
{
	// **** Following needed to save existing calculation structures:
	App::ct->environment->setSaveExistingCalculationStructuresTemp(false);
	if (!App::ct->undoBufferContainer->isUndoSavingOrRestoringUnderWay())
	{ // real saving!
		if (App::ct->environment->getSaveExistingCalculationStructures())
		{
			App::ct->environment->setSaveExistingCalculationStructures(false); // we clear that flag
			App::ct->environment->setSaveExistingCalculationStructuresTemp(true);
		}
	}
	// ************************************************************

	selectedObjectsWhenSaving.clear();

	//***************************************************
	ar.storeDataName(SER_MODEL_THUMBNAIL);
	ar.setCountingMode();
	App::ct->environment->modelThumbnail_notSerializedHere.serialize(ar,false);
	if (ar.setWritingMode())
		App::ct->environment->modelThumbnail_notSerializedHere.serialize(ar,false);
	//****************************************************

	// Textures:
	int textCnt=0;
	while (App::ct->textureCont->getObjectAtIndex(textCnt)!=NULL)
	{
		CTextureObject* it=App::ct->textureCont->getObjectAtIndex(textCnt);
		App::ct->textureCont->storeTextureObject(ar,it);
		textCnt++;
	}

	// DynMaterial objects:
	// We only save what is being used!
	//------------------------------------------------------------
	for (int i=0;i<int(App::ct->dynMaterialCont->_allDynMaterialObjects.size());i++)
		App::ct->dynMaterialCont->_allDynMaterialObjects[i]->setUsedTag(false);
	for (int i=0;i<int(objectList.size());i++)
	{
		CShape* shape=getShape(objectList[i]);
		if (shape!=NULL)
			shape->geomData->geomInfo->getDynMaterialObject()->setUsedTag(true);
	}
	for (int i=0;i<int(App::ct->dynMaterialCont->_allDynMaterialObjects.size());i++)
	{
		if (App::ct->dynMaterialCont->_allDynMaterialObjects[i]->getUsedTag())
			App::ct->dynMaterialCont->storeDynMaterialObject(ar,App::ct->dynMaterialCont->_allDynMaterialObjects[i]);
	}
	//------------------------------------------------------------

	for (int i=0;i<int(objectList.size());i++)
	{
		C3DObject* it=getObject(objectList[i]);
		store3DObject(ar,it);
	}

	ar.storeDataName(SER_GHOSTS);
	ar.setCountingMode();
	App::ct->ghostObjectCont->serialize(ar);
	if (ar.setWritingMode())
		App::ct->ghostObjectCont->serialize(ar);

	ar.storeDataName(SER_ENVIRONMENT);
	ar.setCountingMode();
	App::ct->environment->serialize(ar,true);
	if (ar.setWritingMode())
		App::ct->environment->serialize(ar,true);

	for (int i=0;i<int(App::ct->collisions->collisionObjects.size());i++)
	{
		ar.storeDataName(SER_COLLISION);
		ar.setCountingMode();
		App::ct->collisions->collisionObjects[i]->serialize(ar);
		if (ar.setWritingMode())
			App::ct->collisions->collisionObjects[i]->serialize(ar);
	}
	for (int i=0;i<int(App::ct->distances->distanceObjects.size());i++)
	{
		ar.storeDataName(SER_DISTANCE);
		ar.setCountingMode();
		App::ct->distances->distanceObjects[i]->serialize(ar);
		if (ar.setWritingMode())
			App::ct->distances->distanceObjects[i]->serialize(ar);
	}
	for (int i=0;i<int(App::ct->ikGroups->ikGroups.size());i++)
	{
		ar.storeDataName(SER_IK);
		ar.setCountingMode();
		App::ct->ikGroups->ikGroups[i]->serialize(ar);
		if (ar.setWritingMode())
			App::ct->ikGroups->ikGroups[i]->serialize(ar);
	}
	for (int i=0;i<int(App::ct->pathPlanning->allObjects.size());i++)
	{
		ar.storeDataName(SER_PATH_PLANNING);
		ar.setCountingMode();
		App::ct->pathPlanning->allObjects[i]->serialize(ar);
		if (ar.setWritingMode())
			App::ct->pathPlanning->allObjects[i]->serialize(ar);
	}
	for (int i=0;i<int(App::ct->motionPlanning->allObjects.size());i++)
	{
		ar.storeDataName(SER_MOTION_PLANNING);
		ar.setCountingMode();
		App::ct->motionPlanning->allObjects[i]->serialize(ar);
		if (ar.setWritingMode())
			App::ct->motionPlanning->allObjects[i]->serialize(ar);
	}

	ar.storeDataName(SER_SETTINGS);
	ar.setCountingMode();
	App::ct->mainSettings->serialize(ar);
	if (ar.setWritingMode())
		App::ct->mainSettings->serialize(ar);

	ar.storeDataName(SER_DYNAMICS);
	ar.setCountingMode();
	App::ct->dynamicsContainer->serialize(ar);
	if (ar.setWritingMode())
		App::ct->dynamicsContainer->serialize(ar);

	ar.storeDataName(SER_SIMULATION);
	ar.setCountingMode();
	App::ct->simulation->serialize(ar);
	if (ar.setWritingMode())
		App::ct->simulation->serialize(ar);

	ar.storeDataName(SER_SCENE_CUSTOM_DATA);
	ar.setCountingMode();
	App::ct->customSceneData->serializeData(ar);
	if (ar.setWritingMode())
		App::ct->customSceneData->serializeData(ar);

	ar.storeDataName(SER_VIEWS);
	ar.setCountingMode();
	App::ct->oglSurface->pageCont->serialize(ar);
	if (ar.setWritingMode())
		App::ct->oglSurface->pageCont->serialize(ar);

	// ******* FOLLOWING FOR FORWARD COMPATIBILITY, SO THAT OLD V_REP VERSIONS CAN READ NEWER FILES (2010/07/14) *******************
	// We serialize the rendering order:
	ar.storeDataName(SER_ORDER_OLD_FORWARD_COMPATIBILITY_2010_07_14);
	for (int i=0;i<int(objectList.size());i++)
		ar << objectList[i];
	ar.flush();
	// ******************************************************************************************************************************
	
	// We serialize all groups:
	for (int i=0;i<int(App::ct->collections->allGroups.size());i++)
	{
		ar.storeDataName(SER_GROUP);
		ar.setCountingMode();
		App::ct->collections->allGroups[i]->serialize(ar);
		if (ar.setWritingMode())
			App::ct->collections->allGroups[i]->serialize(ar);
	}

	// We serialize the buttonBlocks (non-system):
	for (int i=0;i<int(App::ct->buttonBlockContainer->allBlocks.size());i++)
	{
		CButtonBlock* bblk=App::ct->buttonBlockContainer->allBlocks[i];
		if ((bblk->getAttributes()&sim_ui_property_systemblock)==0)
		{
			ar.storeDataName(SER_BUTTON_BLOCK);
			ar.setCountingMode();
			bblk->serialize(ar);
			if (ar.setWritingMode())
				bblk->serialize(ar);
		}
	}

	// We serialize the lua script objects (not the add-on scripts!):
	for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->allScripts[i];
		if ( (it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript)||(it->getScriptType()==sim_scripttype_jointctrlcallback)||(it->getScriptType()==sim_scripttype_contactcallback) )
		{
			ar.storeDataName(SER_LUA_SCRIPT);
			ar.setCountingMode();
			it->serialize(ar);
			if (ar.setWritingMode())
				it->serialize(ar);
		}
	}

	// We serialize the GCS objects (all of them):
	for (int i=0;i<int(App::ct->constraintSolver->allGcsObjects.size());i++)
	{
		CConstraintSolverObject* it=App::ct->constraintSolver->allGcsObjects[i];
		ar.storeDataName(SER_GEOMETRIC_CONSTRAINT_OBJECT);
		ar.setCountingMode();
		it->serialize(ar);
		if (ar.setWritingMode())
			it->serialize(ar);
	}

	ar.storeDataName(SER_END_OF_FILE);
}

void CObjCont::exportIkContent(CExtIkSer& ar)
{
	ar.writeInt(0); // this is the ext IK serialization version. Not forward nor backward compatible!

	ar.writeInt(int(objectList.size()));

	for (int i=0;i<int(objectList.size());i++)
	{
		C3DObject* it=getObject(objectList[i]);

		ar.writeInt(it->getObjectType());

		if (it->getObjectType()==sim_object_shape_type)
			((CShape*)it)->serializeWExtIk(ar);
		if (it->getObjectType()==sim_object_joint_type)
			((CJoint*)it)->serializeWExtIk(ar);
		if (it->getObjectType()==sim_object_graph_type)
			((CGraph*)it)->serializeWExtIk(ar);
		if (it->getObjectType()==sim_object_camera_type)
			((CCamera*)it)->serializeWExtIk(ar);
		if (it->getObjectType()==sim_object_dummy_type)
			((CDummy*)it)->serializeWExtIk(ar);
		if (it->getObjectType()==sim_object_proximitysensor_type)
			((CProxSensor*)it)->serializeWExtIk(ar);
		if (it->getObjectType()==sim_object_path_type)
			((CPath*)it)->serializeWExtIk(ar);
		if (it->getObjectType()==sim_object_visionsensor_type)
			((CVisionSensor*)it)->serializeWExtIk(ar);
		if (it->getObjectType()==sim_object_mill_type)
			((CMill*)it)->serializeWExtIk(ar);
		if (it->getObjectType()==sim_object_forcesensor_type)
			((CForceSensor*)it)->serializeWExtIk(ar);
		if (it->getObjectType()==sim_object_light_type)
			((CLight*)it)->serializeWExtIk(ar);
		if (it->getObjectType()==sim_object_mirror_type)
			((CMirror*)it)->serializeWExtIk(ar);
	}

	ar.writeInt(int(App::ct->ikGroups->ikGroups.size()));

	for (int i=0;i<int(App::ct->ikGroups->ikGroups.size());i++)
		App::ct->ikGroups->ikGroups[i]->serializeWExtIk(ar);

}

bool CObjCont::getAllShapesAndVolumesFromScene(std::vector<C3DObject*>& objects,int propMask,bool pathPlanningRoutineCalling)
{
	for (int i=0;i<int(shapeList.size());i++)
	{
		CShape* it=getShape(shapeList[i]);
		if ( (it->getMainProperty()&propMask)||(propMask==-1) )
		{
			if ( (!pathPlanningRoutineCalling)||((it->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)==0) )
				objects.push_back(it);
		}
	}
	for (int i=0;i<int(volumeList.size());i++)
	{
		CVolumeObject* it=getVolume(volumeList[i]);
		if ( (it->getMainProperty()&propMask)||(propMask==-1) )
		{
			if ( (!pathPlanningRoutineCalling)||((it->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)==0) )
				objects.push_back(it);
		}
	}
	return(true);
}

bool CObjCont::getAllShapesAndDummiesFromScene(std::vector<C3DObject*>& objects,int propMask,bool pathPlanningRoutineCalling)
{
	for (int i=0;i<int(dummyList.size());i++)
	{
		CDummy* it=getDummy(dummyList[i]);
		if ( (it->getMainProperty()&propMask)||(propMask==-1) )
		{
			if ( (!pathPlanningRoutineCalling)||((it->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)==0) )
				objects.push_back(it);
		}
	}
	for (int i=0;i<int(shapeList.size());i++)
	{
		CShape* it=getShape(shapeList[i]);
		if ( (it->getMainProperty()&propMask)||(propMask==-1) )
		{
			if ( (!pathPlanningRoutineCalling)||((it->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)==0) ) 
				objects.push_back(it);
		}
	}
	return(true);
}

bool CObjCont::getAllShapesAndDummiesFromSceneExcept(std::vector<C3DObject*>& exceptionObjects,std::vector<C3DObject*>& objects,int propMask,bool pathPlanningRoutineCalling)
{
	for (int i=0;i<int(dummyList.size());i++)
	{
		CDummy* it=getDummy(dummyList[i]);
		if ( (it->getMainProperty()&propMask)||(propMask==-1) )
		{ // Make sure we don't have it in the exception list:
			bool okToAdd=(!pathPlanningRoutineCalling)||((it->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)==0); // Was simple "true" before 2010/08/25
			for (int j=0;j<int(exceptionObjects.size());j++)
			{
				if (exceptionObjects[j]==it)
				{
					okToAdd=false;
					break;
				}
			}
			if (okToAdd)
				objects.push_back(it);
		}
	}
	for (int i=0;i<int(shapeList.size());i++)
	{
		CShape* it=getShape(shapeList[i]);
		if ( (it->getMainProperty()&propMask)||(propMask==-1) )
		{ // Make sure we don't have it in the exception list:
			bool okToAdd=(!pathPlanningRoutineCalling)||((it->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)==0); // Was simple "true" before 2010/08/25
			for (int j=0;j<int(exceptionObjects.size());j++)
			{
				if (exceptionObjects[j]==it)
				{
					okToAdd=false;
					break;
				}
			}
			if (okToAdd)
				objects.push_back(it);
		}
	}
	return(true);
}

bool CObjCont::getAllShapesFromSceneExcept(std::vector<C3DObject*>& exceptionObjects,std::vector<C3DObject*>& objects,int propMask,bool pathPlanningRoutineCalling)
{
	for (int i=0;i<int(shapeList.size());i++)
	{
		CShape* it=getShape(shapeList[i]);
		if ( (it->getMainProperty()&propMask)||(propMask==-1) )
		{ // Make sure we don't have it in the exception list:
			bool okToAdd=(!pathPlanningRoutineCalling)||((it->getMainProperty()&sim_objectspecialproperty_pathplanning_ignored)==0); // Was simple "true" before 2010/08/25
			for (int j=0;j<int(exceptionObjects.size());j++)
			{
				if (exceptionObjects[j]==it)
				{
					okToAdd=false;
					break;
				}
			}
			if (okToAdd)
				objects.push_back(it);
		}
	}
	return(true);
}

void CObjCont::getSelectedObjects(std::vector<C3DObject*>& selection)
{
	selection.clear();
	for (int i=0;i<int(_selectedObjectIDs.size());i++)
	{
		C3DObject* it=getObject(_selectedObjectIDs[i]);
		if (it!=NULL)
			selection.push_back(it);
	}
}

void CObjCont::getSelectedObjects(std::vector<int>& selection)
{
	selection.clear();
	selection.assign(_selectedObjectIDs.begin(),_selectedObjectIDs.end());
}

void CObjCont::setSelectedObjects(std::vector<int>& selection)
{
	deselectObjects();
	for (int i=0;i<int(selection.size());i++)
		addObjectToSelection(selection[i]);
}

C3DObject* CObjCont::getSelectedObject()
{
	if (getSelSize()!=0)
		return(_objectIndex[getSelID(getSelSize()-1)]);
	return(NULL);
}


bool CObjCont::alreadyInEditModeBuffer(int item)
{
	if ( getEditModeType()&SHAPE_OR_PATH_EDIT_MODE )
	{
		for (int i=0;i<int(editModeBuffer.size());i++)
			if (editModeBuffer[i]==item) 
				return(true);
	}
	return(false);
}

void CObjCont::xorAddObjectToSelection(int identifier)
{
	if (identifier!=-1)
	{
		if (identifier>=NON_OBJECT_PICKING_ID_PATH_PTS_START) // individual path points!
		{ // Condition since 2010/09/01, for non path edit mode path point edition!
			if ( (_selectedObjectIDs.size()==0)&&(_uniqueSelectedPathID!=-1) )
				addObjectToSelection(_uniqueSelectedPathID); // special!!
			if ( (_selectedObjectIDs.size()==1)&&(getPath(_selectedObjectIDs[0])!=NULL) )
			{ // Ok, we have one path selected
				if (!isPathPointIndexSelected_nonEditMode(identifier-NON_OBJECT_PICKING_ID_PATH_PTS_START,true))
					_selectedPathPointIndices_nonEditMode.push_back(identifier-NON_OBJECT_PICKING_ID_PATH_PTS_START);
			}
			else
				_selectedPathPointIndices_nonEditMode.clear();
		}
		else
		{
			if (_uniqueSelectedPathID!=identifier)
				_selectedPathPointIndices_nonEditMode.clear();

			if (!isObjectSelected(identifier))
			{
				_selectedObjectIDs.push_back(identifier);
				_selectedObjectsBool[identifier>>3]=_selectedObjectsBool[identifier>>3]|(1<<(identifier&7));
				App::ct->collections->mark(identifier);	//For groups
				App::ct->buttonBlockContainer->a3DObjectWasSelected(identifier);
			}
			else
			{
				int i;
				for (i=0;i<getSelSize();i++)
				{
					if (getSelID(i)==identifier)
						break;
				}
				_selectedObjectIDs.erase(_selectedObjectIDs.begin()+i);
				_selectedObjectsBool[identifier>>3]=_selectedObjectsBool[identifier>>3]&(255-(1<<(identifier&7)));
				App::ct->collections->unmark(identifier);	//For groups	
			}
		}
	}
	else
	{
		_selectedObjectIDs.clear();
		for (int i=0;i<int(_selectedObjectsBool.size());i++)
			_selectedObjectsBool[i]=0;
		App::ct->collections->unmarkAll();		//For groups
		_selectedPathPointIndices_nonEditMode.clear();
	}
	setLightDialogRefreshFlag();
}

void CObjCont::xorAddItemToEditModeBuffer(int item,bool disableEdgeFollowing)
{// disableEdgeFollowing is true by default
	if (isEditModeItemAValidItem(item))
	{
		if (!alreadyInEditModeBuffer(item))
			addItemToEditModeBuffer(item,disableEdgeFollowing);
		else
			removeItemFromEditModeBuffer(item);
	}
	else
		editModeBuffer.clear();
	setLightDialogRefreshFlag();
}


std::vector<int>* CObjCont::getPointerToSelectedPathPointIndices_nonEditMode()
{
	return(&_selectedPathPointIndices_nonEditMode);
}

void CObjCont::clearPathPointIndices_nonEditMode()
{
	_selectedPathPointIndices_nonEditMode.clear();
}

bool CObjCont::isPathPointIndexSelected_nonEditMode(int index,bool removeIfYes)
{
	for (int i=0;i<int(_selectedPathPointIndices_nonEditMode.size());i++)
	{
		if (index==_selectedPathPointIndices_nonEditMode[i])
		{
			if (!removeIfYes)
				return(true);
			_selectedPathPointIndices_nonEditMode.erase(_selectedPathPointIndices_nonEditMode.begin()+i);
			return(true);
		}
	}
	return(false);
}

void CObjCont::setUniqueSelectedPathID(int id)
{
	_uniqueSelectedPathID=id;
}

void CObjCont::addObjectToSelection(int identifier)
{
	if (identifier>=0)
	{
		if (identifier>=NON_OBJECT_PICKING_ID_PATH_PTS_START) // individual path points!
		{ // Condition since 2010/09/01, for non path edit mode path point edition!
			if ( (_selectedObjectIDs.size()==0)&&(_uniqueSelectedPathID!=-1) )
				addObjectToSelection(_uniqueSelectedPathID); // special!!
			if ( (_selectedObjectIDs.size()==1)&&(getPath(_selectedObjectIDs[0])!=NULL) )
			{ // Ok, we have one path selected
				if (!isPathPointIndexSelected_nonEditMode(identifier-NON_OBJECT_PICKING_ID_PATH_PTS_START,false))
					_selectedPathPointIndices_nonEditMode.push_back(identifier-NON_OBJECT_PICKING_ID_PATH_PTS_START);
			}
			else
				_selectedPathPointIndices_nonEditMode.clear();
			return;
		}
		if (getObject(identifier)==NULL)
			return;
		if (_uniqueSelectedPathID!=identifier)
			_selectedPathPointIndices_nonEditMode.clear();
		if (!isObjectSelected(identifier))
		{
			_selectedObjectIDs.push_back(identifier);
			_selectedObjectsBool[identifier>>3]=_selectedObjectsBool[identifier>>3]|(1<<(identifier&7));
			App::ct->collections->mark(identifier);	//For groups
			App::ct->buttonBlockContainer->a3DObjectWasSelected(identifier);
		}
	}
	setLightDialogRefreshFlag();
}

bool CObjCont::isEditModeItemAValidItem(int item)
{ 
	if (item<0)
		return(false);
	if (getEditModeType()&VERTEX_EDIT_MODE)
	{
		if (item>=int(_editionVertices.size())/3)
			return(false);
	}
	if (getEditModeType()&TRIANGLE_EDIT_MODE)
	{
		if (item>=int(_editionIndices.size())/3)
			return(false);
	}
	if (getEditModeType()&EDGE_EDIT_MODE)
	{
		if (item>=int(_edgeCont.allEdges.size())/2)
			return(false);
	}
	return(true);
}

void CObjCont::addItemToEditModeBuffer(int item,bool disableEdgeFollowing)
{ // disableEdgeFollowing is true by default
	if (isEditModeItemAValidItem(item))
	{
		if (!alreadyInEditModeBuffer(item))
		{
			if (getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)
			{
				editModeBuffer.push_back(item);
				while (automaticallyFollowEdges&&(getEditModeType()&EDGE_EDIT_MODE)&&(!disableEdgeFollowing))
				{
					int lowVertex;
					int highVertex;
					if (editModeBuffer.size()==1)
					{
						lowVertex=_edgeCont.allEdges[2*item+0];
						highVertex=_edgeCont.allEdges[2*item+1];
					}
					else
					{
						int prevItem=editModeBuffer[editModeBuffer.size()-2];
						if ( (_edgeCont.allEdges[2*item+0]==_edgeCont.allEdges[2*prevItem+0])||
							(_edgeCont.allEdges[2*item+0]==_edgeCont.allEdges[2*prevItem+1]) )
						{
							lowVertex=_edgeCont.allEdges[2*item+0];
							highVertex=_edgeCont.allEdges[2*item+1];
						}
						else
						{
							lowVertex=_edgeCont.allEdges[2*item+1];
							highVertex=_edgeCont.allEdges[2*item+0];
						}
					}
					std::vector<int> nextEdges;
					getNextEdges(lowVertex,highVertex,nextEdges);
					int highestAngleIndex=0;
					float highestAngleValue=0.0f;
					bool foundValid=false;
					for (int i=0;i<int(nextEdges.size());i++)
					{
						// 1. We need the edge angle:
						float a=getEdgeAngle(nextEdges[i]);
//						printf("Angle: %f\n",a*radToDeg);
						// 2. and we need the angle between previous and next edge: (added on 2009/03/14)
						int ind[2]={_edgeCont.allEdges[2*nextEdges[i]+0],_edgeCont.allEdges[2*nextEdges[i]+1]};
						if (ind[1]==highVertex)
							ind[1]=ind[0];
						C3Vector v0(&_editionVertices[3*lowVertex+0]);
						C3Vector v1(&_editionVertices[3*highVertex+0]);
						C3Vector v2(&_editionVertices[3*ind[1]+0]);
						float a2=(v1-v0).getAngle(v2-v1);
//						printf("Dir Angle: %f\n",a2*radToDeg);
						if ( (a>highestAngleValue)&&(a2<edgeDirectionChangeMaxAngle)&&(a>(piValue-edgeMaxAngle)) ) // a>20.0... added on 2009/05/08
						{
							highestAngleValue=a;
							highestAngleIndex=i;
							foundValid=true;
						}
					}
					if (!foundValid)
						break; // we didn't find an appropriate edge
					item=nextEdges[highestAngleIndex];
					int ind[2]={_edgeCont.allEdges[2*item+0],_edgeCont.allEdges[2*item+1]};
					int newVertex=ind[0];
					if (newVertex==highVertex)
						newVertex=ind[1];
					// We make sure that this edge is not yet present in the selection buffer:
					if (alreadyInEditModeBuffer(item))
						break;
					// now we check if that vertex already exists in the selection:
					bool alreadyExists=false;
					for (int i=0;i<int(editModeBuffer.size());i++)
					{
						ind[0]=_edgeCont.allEdges[2*editModeBuffer[i]+0];
						ind[1]=_edgeCont.allEdges[2*editModeBuffer[i]+1];
						if ( (ind[0]==newVertex)||(ind[1]==newVertex) )
						{
							alreadyExists=true;
							break;
						}
					}
					editModeBuffer.push_back(item);
					if (alreadyExists)
						break;
				}
			}
		}
	}
	setLightDialogRefreshFlag();
}

void CObjCont::removeObjectFromSelection(int identifier)
{
	for (int i=0;i<getSelSize();i++)
	{
		if (getSelID(i)==identifier)
		{
			_selectedObjectIDs.erase(_selectedObjectIDs.begin()+i);
			_selectedObjectsBool[identifier>>3]=_selectedObjectsBool[identifier>>3]&(255-(1<<(identifier&7)));
			App::ct->collections->unmark(identifier);	//For groups
			break;
		}
	}
	setLightDialogRefreshFlag();
}

void CObjCont::removeItemFromEditModeBuffer(int item)
{
	if (getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)
	{
		for (int i=0;i<int(editModeBuffer.size());i++)
		{
			if (editModeBuffer[i]==item)
			{
				editModeBuffer.erase(editModeBuffer.begin()+i);
				break;
			}
		}
	}
	setLightDialogRefreshFlag();
}

int CObjCont::getSelSize()
{
	return(_selectedObjectIDs.size());
}
int CObjCont::getSelID(int i)
{
	return(_selectedObjectIDs[i]);
}


bool CObjCont::selectObject(int objectIdentifier)
// No checking if object really exists.
// -1 is a valid parameter (no object is selected)
{
	_selectedObjectIDs.clear();
	for (int i=0;i<int(_selectedObjectsBool.size());i++)
		_selectedObjectsBool[i]=0;
	App::ct->collections->unmarkAll();			//For groups
	if (objectIdentifier==-1)
	{
		setLightDialogRefreshFlag();
		return(false);
	}
	else
	{
		_selectedObjectIDs.push_back(objectIdentifier);
		_selectedObjectsBool[objectIdentifier>>3]=_selectedObjectsBool[objectIdentifier>>3]|(1<<(objectIdentifier&7));
		App::ct->collections->mark(objectIdentifier);		//For groups
		setLightDialogRefreshFlag();
		return(true);
	}
}

void CObjCont::selectAllObjects()
{
	_selectedObjectIDs.clear();
	for (int i=0;i<int(_selectedObjectsBool.size());i++)
		_selectedObjectsBool[i]=0;
	App::ct->collections->unmarkAll();
	for (int i=0;i<int(objectList.size());i++)
	{
		_selectedObjectIDs.push_back(objectList[i]);
		_selectedObjectsBool[objectList[i]>>3]=_selectedObjectsBool[objectList[i]>>3]|(1<<(objectList[i]&7));
		App::ct->collections->mark(objectList[i]);
	}
	setLightDialogRefreshFlag();
}

void CObjCont::deselectObjects()
{
	_selectedObjectIDs.clear();
	for (int i=0;i<int(_selectedObjectsBool.size());i++)
		_selectedObjectsBool[i]=0;
	App::ct->collections->unmarkAll();
	_selectedPathPointIndices_nonEditMode.clear(); 
	setLightDialogRefreshFlag();
}

bool CObjCont::isSelectionSame(std::vector<int>& sel,bool actualize)
{
	if (_selectedObjectIDs.size()!=sel.size())
	{
		if (actualize)
		{
			sel.clear();
			sel.assign(_selectedObjectIDs.begin(),_selectedObjectIDs.end());
		}
		return(false);
	}
	for (int i=0;i<int(_selectedObjectIDs.size());i++)
	{
		if (_selectedObjectIDs[i]!=sel[i])
		{
			if (actualize)
			{
				sel.clear();
				sel.assign(_selectedObjectIDs.begin(),_selectedObjectIDs.end());
			}
			return(false);
		}
	}
	return(true);
}

void CObjCont::deselectEditModeBuffer()
{
	editModeBuffer.clear();
	setLightDialogRefreshFlag();
}

CShape* CObjCont::getShape(int identifier)
{
	if (identifier<0)
		return(NULL);
	if (identifier>=int(_objectIndex.size()))
		return(NULL);
	if (_objectIndex[identifier]==NULL)
		return(NULL);
	if (_objectIndex[identifier]->getObjectType()!=sim_object_shape_type)
		return(NULL);
	return((CShape*)_objectIndex[identifier]);
}

CMirror* CObjCont::getMirror(int identifier)
{
	if (identifier<0)
		return(NULL);
	if (identifier>=int(_objectIndex.size()))
		return(NULL);
	if (_objectIndex[identifier]==NULL)
		return(NULL);
	if (_objectIndex[identifier]->getObjectType()!=sim_object_mirror_type)
		return(NULL);
	return((CMirror*)_objectIndex[identifier]);
}

CProxSensor* CObjCont::getProximitySensor(int identifier)
{
	if (identifier<0)
		return(NULL);
	if (identifier>=int(_objectIndex.size()))
		return(NULL);
	if (_objectIndex[identifier]==NULL)
		return(NULL);
	if (_objectIndex[identifier]->getObjectType()!=sim_object_proximitysensor_type)
		return(NULL);
	return((CProxSensor*)_objectIndex[identifier]);
}
CVisionSensor* CObjCont::getVisionSensor(int identifier)
{
	if (identifier<0)
		return(NULL);
	if (identifier>=int(_objectIndex.size()))
		return(NULL);
	if (_objectIndex[identifier]==NULL)
		return(NULL);
	if (_objectIndex[identifier]->getObjectType()!=sim_object_visionsensor_type)
		return(NULL);
	return((CVisionSensor*)_objectIndex[identifier]);
}
CPath* CObjCont::getPath(int identifier)
{
	if (identifier<0)
		return(NULL);
	if (identifier>=int(_objectIndex.size()))
		return(NULL);
	if (_objectIndex[identifier]==NULL)
		return(NULL);
	if (_objectIndex[identifier]->getObjectType()!=sim_object_path_type)
		return(NULL);
	return((CPath*)_objectIndex[identifier]);
}
CVolumeObject* CObjCont::getVolume(int identifier)
{
	if (identifier<0)
		return(NULL);
	if (identifier>=int(_objectIndex.size()))
		return(NULL);
	if (_objectIndex[identifier]==NULL)
		return(NULL);
	if (_objectIndex[identifier]->getObjectType()!=sim_object_volume_type)
		return(NULL);
	return((CVolumeObject*)_objectIndex[identifier]);
}
CMill* CObjCont::getMill(int identifier)
{
	if (identifier<0)
		return(NULL);
	if (identifier>=int(_objectIndex.size()))
		return(NULL);
	if (_objectIndex[identifier]==NULL)
		return(NULL);
	if (_objectIndex[identifier]->getObjectType()!=sim_object_mill_type)

		return(NULL);
	return((CMill*)_objectIndex[identifier]);
}
CForceSensor* CObjCont::getForceSensor(int identifier)
{
	if (identifier<0)
		return(NULL);
	if (identifier>=int(_objectIndex.size()))
		return(NULL);
	if (_objectIndex[identifier]==NULL)
		return(NULL);
	if (_objectIndex[identifier]->getObjectType()!=sim_object_forcesensor_type)
		return(NULL);
	return((CForceSensor*)_objectIndex[identifier]);
}
CCamera* CObjCont::getCamera(int identifier)
{
	if (identifier<0)
		return(NULL);
	if (identifier>=int(_objectIndex.size()))
		return(NULL);
	if (_objectIndex[identifier]==NULL)
		return(NULL);
	if (_objectIndex[identifier]->getObjectType()!=sim_object_camera_type)
		return(NULL);
	return((CCamera*)_objectIndex[identifier]);
}
CLight* CObjCont::getLight(int identifier)
{
	if (identifier<0)
		return(NULL);
	if (identifier>=int(_objectIndex.size()))
		return(NULL);
	if (_objectIndex[identifier]==NULL)
		return(NULL);
	if (_objectIndex[identifier]->getObjectType()!=sim_object_light_type)
		return(NULL);
	return((CLight*)_objectIndex[identifier]);
}

CGraph* CObjCont::getGraph(int identifier)
{
	if (identifier<0)
		return(NULL);
	if (identifier>=int(_objectIndex.size()))
		return(NULL);
	if (_objectIndex[identifier]==NULL)
		return(NULL);
	if (_objectIndex[identifier]->getObjectType()!=sim_object_graph_type)
		return(NULL);
	return((CGraph*)_objectIndex[identifier]);
}

//------------------ Object destruction announcement -------------------------

void CObjCont::announceGroupWillBeErased(int groupID)
{
	for (int i=0;i<int(objectList.size());i++)
		getObject(objectList[i])->announceGroupWillBeErased(groupID,false); // this never triggers 3DObject destruction!
	App::ct->collisions->announceGroupWillBeErased(groupID); // This can trigger a collision destruction!
	App::ct->distances->announceGroupWillBeErased(groupID); // This can trigger a distance destruction!
	App::ct->ikGroups->announceGroupWillBeErased(groupID); // This will never trigger an IK group destruction! (at least for now!)
	App::ct->pathPlanning->announceGroupWillBeErased(groupID); // This can trigger a path planning object destruction!
	App::ct->motionPlanning->announceGroupWillBeErased(groupID); // This can trigger a motion planning object destruction!
}

void CObjCont::announceGcsObjectWillBeErased(int gcsObjectID)
{
	for (int i=0;i<int(objectList.size());i++)
		getObject(objectList[i])->announceGcsObjectWillBeErased(gcsObjectID,false); // this never triggers 3DObject destruction!
}

void CObjCont::announceCollisionWillBeErased(int collisionID)
{
	for (int i=0;i<int(objectList.size());i++)
		getObject(objectList[i])->announceCollisionWillBeErased(collisionID,false); // this never triggers 3DObject destruction!
}
void CObjCont::announceDistanceWillBeErased(int distanceID)
{
	for (int i=0;i<int(objectList.size());i++)
		getObject(objectList[i])->announceDistanceWillBeErased(distanceID,false); // this never triggers 3DObject destruction!
}

void CObjCont::announce2DElementWillBeErased(int elementID)
{
	if (App::ct->textureCont!=NULL)
		App::ct->textureCont->announceGeneralObjectWillBeErased(elementID,-1);
}

void CObjCont::announce2DElementButtonWillBeErased(int elementID,int buttonID)
{
	if (App::ct->textureCont!=NULL)
		App::ct->textureCont->announceGeneralObjectWillBeErased(elementID,buttonID);
}

void CObjCont::actualizeMechanismIDs()
{
	for (int i=0;i<int(objectList.size());i++)
	{
		C3DObject* it=getObject(objectList[i]);
		if (it->getParent()==NULL)
		{
			CGeometricConstraintSolverInt work(it->getID(),-1); 
			int mechanismID=work.getMechanismID();
			std::vector<C3DObject*> childrenAndSelf;
			it->getAllObjectsRecursive(&childrenAndSelf,true);
			for (int j=0;j<int(childrenAndSelf.size());j++)
				childrenAndSelf[j]->setMechanismID(mechanismID);
		}
	}
}

//----------------------------------------------------------------------------
void CObjCont::setAbsoluteAngle(int identifier,float angle,int index)
{ // Angle is specified in radian!
	C3DObject* it=getObject(identifier);
	if (it==NULL)
		return;
	C7Vector cumul(it->getCumulativeTransformationPart1());
	C7Vector parentInverse(it->getParentCumulativeTransformation().getInverse());
	C3Vector euler=cumul.Q.getEulerAngles();
	euler(index)=angle;
	cumul.Q.setEulerAngles(euler);
	it->setLocalTransformation(parentInverse*cumul);
}

void CObjCont::setAbsolutePosition(int identifier,float pos,int index)
{
	C3DObject* it=getObject(identifier);
	if (it==NULL)
		return;
	C7Vector cumul(it->getCumulativeTransformationPart1());
	C7Vector parentInverse(it->getParentCumulativeTransformation().getInverse());
	cumul.X(index)=pos;
	it->setLocalTransformation(parentInverse*cumul);
}

void CObjCont::setAbsolutePosition(int identifier,const C3Vector& p)
{
	C3DObject* it=getObject(identifier);
	if (it==NULL)
		return;
	C7Vector cumul(it->getCumulativeTransformationPart1());
	C7Vector parentInverse(it->getParentCumulativeTransformation().getInverse());
	cumul.X=p;
	it->setLocalTransformation(parentInverse*cumul);
}

void CObjCont::setAbsoluteAngles(int identifier,const C3Vector& euler)
{	// Here euler elements are in radians!!!
	C3DObject* it=getObject(identifier);
	if (it==NULL)
		return;
	C7Vector cumul(it->getCumulativeTransformationPart1());
	C7Vector parentInverse(it->getParentCumulativeTransformation().getInverse());
	cumul.Q.setEulerAngles(euler);
	it->setLocalTransformation(parentInverse*cumul);
}

int CObjCont::getSimpleShapeNumberInSelection()
{
	return(getSimpleShapeNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getShapeNumberInSelection()
{
	return(getShapeNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getMirrorNumberInSelection()
{
	return(getMirrorNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getJointNumberInSelection()
{
	return(getJointNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getGroupingNumberInSelection()
{
	return(getGroupingNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getGraphNumberInSelection()
{
	return(getGraphNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getCameraNumberInSelection()
{
	return(getCameraNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getLightNumberInSelection()
{
	return(getLightNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getDummyNumberInSelection()
{
	return(getDummyNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getProxSensorNumberInSelection()
{
	return(getProxSensorNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getVisionSensorNumberInSelection()
{
	return(getVisionSensorNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getPathNumberInSelection()
{
	return(getPathNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getVolumeNumberInSelection()
{
	return(getVolumeNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getMillNumberInSelection()
{
	return(getMillNumberInSelection(&_selectedObjectIDs));
}
int CObjCont::getForceSensorNumberInSelection()
{
	return(getForceSensorNumberInSelection(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAShape()
{
	return(isLastSelectionAShape(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAMirror()
{
	return(isLastSelectionAMirror(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionASimpleShape()
{
	return(isLastSelectionASimpleShape(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAJoint()
{
	return(isLastSelectionAJoint(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAGrouping()
{
	return(isLastSelectionAGrouping(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAGraph()
{
	return(isLastSelectionAGraph(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionACamera()
{
	return(isLastSelectionACamera(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionALight()
{
	return(isLastSelectionALight(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionADummy()
{
	return(isLastSelectionADummy(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAProxSensor()
{
	return(isLastSelectionAProxSensor(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAVisionSensor()
{
	return(isLastSelectionAVisionSensor(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAPath()
{
	return(isLastSelectionAPath(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAVolume()
{
	return(isLastSelectionAVolume(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAMill()
{
	return(isLastSelectionAMill(&_selectedObjectIDs));
}
bool CObjCont::isLastSelectionAForceSensor()
{
	return(isLastSelectionAForceSensor(&_selectedObjectIDs));
}

C3DObject* CObjCont::getLastSelection()
{
	return(getLastSelection(&_selectedObjectIDs));
}

int CObjCont::getLastSelectionID()
{
	return(getLastSelectionID(&_selectedObjectIDs));
}

int CObjCont::getSimpleShapeNumberInSelection(std::vector<int>* selection)
{
	int counter=0;
	for (int i=0;i<int(selection->size());i++)
	{
		CShape* it=getShape(selection->at(i));
		if (it!=NULL)
		{
			if (!it->isGrouping()) 
				counter++;
		}
	}
	return (counter);
}
int CObjCont::getShapeNumberInSelection(std::vector<int>* selection)
{
	int counter=0;
	for (int i=0;i<int(selection->size());i++)
	{
		CShape* it=getShape(selection->at(i));
		if (it!=NULL)
			counter++;
	}
	return (counter);
}
int CObjCont::getMirrorNumberInSelection(std::vector<int>* selection)
{
	int counter=0;
	for (int i=0;i<int(selection->size());i++)
	{
		CMirror* it=getMirror(selection->at(i));
		if (it!=NULL)
			counter++;
	}
	return (counter);
}
int CObjCont::getJointNumberInSelection(std::vector<int>* selection)
{
	int counter=0;
	for (int i=0;i<int(selection->size());i++)
	{
		CJoint* it=getJoint(selection->at(i));
		if (it!=NULL) 
			counter++;
	}
	return (counter);
}
int CObjCont::getGraphNumberInSelection(std::vector<int>* selection)
{
	int counter=0;
	for (int i=0;i<int(selection->size());i++)
	{
		CGraph* it=getGraph(selection->at(i));
		if (it!=NULL) 
			counter++;
	}
	return (counter);
}
int CObjCont::getGroupingNumberInSelection(std::vector<int>* selection)
{
	int counter=0;
	for (int i=0;i<int(selection->size());i++)
	{
		CShape* it=getShape(selection->at(i));
		if (it!=NULL)
		{
			if (it->isGrouping())
				counter++;
		}
	}
	return (counter);
}
int CObjCont::getCameraNumberInSelection(std::vector<int>* selection)
{
	int counter=0;
	for (int i=0;i<int(selection->size());i++)
	{
		CCamera* it=getCamera(selection->at(i));
		if (it!=NULL) 
			counter++;	
	}
	return (counter);
}
int CObjCont::getLightNumberInSelection(std::vector<int>* selection)
{
	int counter=0;
	for (int i=0;i<int(selection->size());i++)
	{
		CLight* it=getLight(selection->at(i));
		if (it!=NULL) 
			counter++;	
	}
	return (counter);
}
int CObjCont::getDummyNumberInSelection(std::vector<int>* selection)
{
	int counter=0;
	for (int i=0;i<int(selection->size());i++)
	{
		CDummy* it=getDummy(selection->at(i));
		if (it!=NULL) 
			counter++;
	}
	return (counter);
}
int CObjCont::getProxSensorNumberInSelection(std::vector<int>* selection)
{
	int counter=0;
	for (int i=0;i<int(selection->size());i++)
	{
		CProxSensor* it=getProximitySensor(selection->at(i));
		if (it!=NULL) 
			counter++;
	}
	return (counter);
}
int CObjCont::getVisionSensorNumberInSelection(std::vector<int>* selection)
{
	int counter=0;
	for (int i=0;i<int(selection->size());i++)
	{
		CVisionSensor* it=getVisionSensor(selection->at(i));
		if (it!=NULL) 
			counter++;
	}
	return (counter);
}
int CObjCont::getPathNumberInSelection(std::vector<int>* selection)
{
	int counter=0;
	for (int i=0;i<int(selection->size());i++)
	{
		CPath* it=getPath(selection->at(i));
		if (it!=NULL) 
			counter++;
	}
	return (counter);
}
int CObjCont::getVolumeNumberInSelection(std::vector<int>* selection)
{
	int counter=0;
	for (int i=0;i<int(selection->size());i++)
	{
		CVolumeObject* it=getVolume(selection->at(i));
		if (it!=NULL) 
			counter++;
	}
	return (counter);
}
int CObjCont::getMillNumberInSelection(std::vector<int>* selection)
{
	int counter=0;
	for (int i=0;i<int(selection->size());i++)
	{
		CMill* it=getMill(selection->at(i));
		if (it!=NULL) 
			counter++;
	}
	return (counter);
}
int CObjCont::getForceSensorNumberInSelection(std::vector<int>* selection)
{
	int counter=0;
	for (int i=0;i<int(selection->size());i++)
	{
		CForceSensor* it=getForceSensor(selection->at(i));
		if (it!=NULL) 
			counter++;
	}
	return (counter);
}
bool CObjCont::isLastSelectionAShape(std::vector<int>* selection)
{
	if (selection->size()==0) 
		return(false);
	CShape* it=getShape(selection->at(selection->size()-1));
	if (it!=NULL)
		return(true);
	return(false);
}
bool CObjCont::isLastSelectionASimpleShape(std::vector<int>* selection)
{
	if (selection->size()==0) 
		return(false);
	CShape* it=getShape(selection->at(selection->size()-1));
	if (it!=NULL)
	{
		if (!it->isGrouping()) 
			return(true);
	}
	return(false);
}
bool CObjCont::isLastSelectionAJoint(std::vector<int>* selection)
{
	if (selection->size()==0) 
		return(false);
	CJoint* it=getJoint(selection->at(selection->size()-1));
	if (it!=NULL) 
		return(true);
	return(false);
}
bool CObjCont::isLastSelectionAMirror(std::vector<int>* selection)
{
	if (selection->size()==0) 
		return(false);
	CMirror* it=getMirror(selection->at(selection->size()-1));
	if (it!=NULL) 
		return(true);
	return(false);
}
bool CObjCont::isLastSelectionAGrouping(std::vector<int>* selection)
{
	if (selection->size()==0)
		return(false);
	CShape* it=getShape(selection->at(selection->size()-1));
	if (it!=NULL)
	{
		if (it->isGrouping()) 
			return(true);
	}
	return(false);
}
bool CObjCont::isLastSelectionAGraph(std::vector<int>* selection)
{
	if (selection->size()==0) 
		return(false);
	CGraph* it=getGraph(selection->at(selection->size()-1));
	if (it!=NULL) 
		return(true);
	return(false);
}
bool CObjCont::isLastSelectionACamera(std::vector<int>* selection)
{
	if (selection->size()==0) 
		return(false);
	CCamera* it=getCamera(selection->at(selection->size()-1));
	if (it!=NULL)
		return(true);
	return(false);
}
bool CObjCont::isLastSelectionALight(std::vector<int>* selection)
{
	if (selection->size()==0) 
		return(false);
	CLight* it=getLight(selection->at(selection->size()-1));
	if (it!=NULL)
		return(true);
	return(false);
}
bool CObjCont::isLastSelectionADummy(std::vector<int>* selection)
{
	if (selection->size()==0) 
		return(false);
	CDummy* it=getDummy(selection->at(selection->size()-1));
	if (it!=NULL) 
		return(true);
	return(false);
}
bool CObjCont::isLastSelectionAProxSensor(std::vector<int>* selection)
{
	if (selection->size()==0) 
		return(false);
	CProxSensor* it=getProximitySensor(selection->at(selection->size()-1));
	if (it!=NULL) 
		return(true);
	return(false);
}
bool CObjCont::isLastSelectionAVisionSensor(std::vector<int>* selection)
{
	if (selection->size()==0) 
		return(false);
	CVisionSensor* it=getVisionSensor(selection->at(selection->size()-1));
	if (it!=NULL) 
		return(true);
	return(false);
}
bool CObjCont::isLastSelectionAPath(std::vector<int>* selection)
{
	if (selection->size()==0) 
		return(false);
	CPath* it=getPath(selection->at(selection->size()-1));
	if (it!=NULL) 
		return(true);
	return(false);
}
bool CObjCont::isLastSelectionAVolume(std::vector<int>* selection)
{
	if (selection->size()==0) 
		return(false);
	CVolumeObject* it=getVolume(selection->at(selection->size()-1));
	if (it!=NULL) 
		return(true);
	return(false);
}
bool CObjCont::isLastSelectionAMill(std::vector<int>* selection)
{
	if (selection->size()==0) 
		return(false);
	CMill* it=getMill(selection->at(selection->size()-1));
	if (it!=NULL) 
		return(true);
	return(false);
}
bool CObjCont::isLastSelectionAForceSensor(std::vector<int>* selection)
{
	if (selection->size()==0) 
		return(false);
	CForceSensor* it=getForceSensor(selection->at(selection->size()-1));
	if (it!=NULL) 
		return(true);
	return(false);
}
C3DObject* CObjCont::getLastSelection(std::vector<int>* selection)
{
	if (selection->size()==0) 
		return(NULL);
	return(_objectIndex[selection->at(selection->size()-1)]);
}

int CObjCont::getLastSelectionID(std::vector<int>* selection)
{
	if (selection->size()==0) 
		return(-1);
	return((*selection)[(selection->size()-1)]);
}

bool CObjCont::isObjectInSelection(int objectID)
{
	return(isObjectInSelection(objectID,&_selectedObjectIDs));
}
bool CObjCont::isObjectInSelection(int objectID,std::vector<int>* selection)
{
	for (int i=0;i<int(selection->size());i++)
	{
		if (selection->at(i)==objectID)
			return(true);
	}
	return(false);
}

int CObjCont::getRenderingPosition(int objID)
{ // The rendering order is the reversed objectList!!
	for (int i=0;i<int(objectList.size());i++)
	{
		if (objectList[i]==objID)
			return(objectList.size()-i-1);
	}
	return(-1);
}

void CObjCont::changePoseAndKeepRestInPlace(int identifier,C7Vector& m,bool changePositionX,
		bool changePositionY,bool changePositionZ,bool changeOrientation)
{	// Changes the orientation/position of an object (identifier) with
	// keeping all its children in place. m is the new pose.
	C3DObject* it=CObjCont::getObject(identifier);
	if (it==NULL) 
		return;
	C7Vector oldRel(it->getLocalTransformationPart1());
	C7Vector oldRelActuatorPartIncl(it->getLocalTransformation());
	C7Vector oldAbs(it->getCumulativeTransformationPart1());
	C7Vector newAbs(oldAbs);
	if (changePositionX)
		newAbs.X(0)=m.X(0);
	if (changePositionY)
		newAbs.X(1)=m.X(1);
	if (changePositionZ)
		newAbs.X(2)=m.X(2);
	if (changeOrientation)
		newAbs.Q=m.Q;
	C7Vector parentInv(it->getParentCumulativeTransformation().getInverse());
	it->setLocalTransformation(parentInv*newAbs);
	// from here we have to compensate the children
	C7Vector newRelActuatorPartIncl(it->getLocalTransformation().getInverse());
	C7Vector coeff(newRelActuatorPartIncl*oldRelActuatorPartIncl);
	for (int i=0;i<int(it->childList.size());i++)
	{
		C7Vector oldChild(it->childList[i]->getLocalTransformationPart1());
		it->childList[i]->setLocalTransformation(coeff*oldChild);
	}
}

void CObjCont::selectionFromTriangleToVertexEditMode(std::vector<int>* newVertexSel)
{ // newVertexSel is NULL by default. In that case, the change is made in the editModeBuffer
	bool inPlace=false;
	if (newVertexSel==NULL)
	{
		newVertexSel=new std::vector<int>;
		inPlace=true;
	}
	newVertexSel->reserve(CObjCont::editModeBuffer.size());
	newVertexSel->clear();
	std::vector<bool> used(_editionVertices.size(),false);
	for (int i=0;i<int(editModeBuffer.size());i++)
	{
		for (int j=0;j<3;j++)
		{
			int vertInd=_editionIndices[3*editModeBuffer[i]+j];
			if (!used[vertInd])
			{
				newVertexSel->push_back(vertInd);
				used[vertInd]=true;
			}
		}
	}

	if (inPlace)
	{
		editModeBuffer.reserve(newVertexSel->size());
		editModeBuffer.clear();
		editModeBuffer.insert(editModeBuffer.begin(),newVertexSel->begin(),newVertexSel->end());
		newVertexSel->clear();
		delete newVertexSel;
	}
}

void CObjCont::selectionFromTriangleToEdgeEditMode(std::vector<int>* newEdgeSel)
{ // newVertexSel is NULL by default. In that case, the change is made in the editModeBuffer
	bool inPlace=false;
	if (newEdgeSel==NULL)
	{
		newEdgeSel=new std::vector<int>;
		inPlace=true;
	}
	newEdgeSel->clear();
	std::vector<bool> used(_edgeCont.allEdges.size(),false);
	for (int i=0;i<int(editModeBuffer.size());i++)
	{
		for (int j=0;j<3;j++)
		{
			int edgeInd=_editionIndicesToEdgesIndex[3*editModeBuffer[i]+j];
			if (!used[edgeInd])
			{
				newEdgeSel->push_back(edgeInd);
				used[edgeInd]=true;
			}
		}
	}
	if (inPlace)
	{
		editModeBuffer.reserve(newEdgeSel->size());
		editModeBuffer.clear();
		editModeBuffer.insert(editModeBuffer.begin(),newEdgeSel->begin(),newEdgeSel->end());
		newEdgeSel->clear();
		delete newEdgeSel;
	}
}

void CObjCont::selectionFromEdgeToVertexEditMode(std::vector<int>* newVertexSel)
{ // newVertexSel is NULL by default. In that case, the change is made in the editModeBuffer
	bool inPlace=false;
	if (newVertexSel==NULL)
	{
		newVertexSel=new std::vector<int>;
		inPlace=true;
	}
	newVertexSel->clear();
	std::vector<bool> used(_editionVertices.size(),false);
	for (int i=0;i<int(editModeBuffer.size());i++)
	{
		for (int j=0;j<2;j++)
		{
			int vertInd=_edgeCont.allEdges[2*editModeBuffer[i]+j];
			if (!used[vertInd])
			{
				newVertexSel->push_back(vertInd);
				used[vertInd]=true;
			}
		}
	}

	if (inPlace)
	{
		editModeBuffer.reserve(newVertexSel->size());
		editModeBuffer.clear();
		editModeBuffer.insert(editModeBuffer.begin(),newVertexSel->begin(),newVertexSel->end());
		newVertexSel->clear();
		delete newVertexSel;
	}
}

void CObjCont::selectionFromVertexToTriangleEditMode(std::vector<int>* newTriangleSel)
{ // newTriangleSel is NULL by default. In that case, the change is made in the editModeBuffer
	std::vector<bool> used(_editionVertices.size(),false);
	for (int i=0;i<int(editModeBuffer.size());i++)
		used[editModeBuffer[i]]=true;
	if (newTriangleSel==NULL)
	{
		editModeBuffer.reserve(3*editModeBuffer.size()); // Max size
		editModeBuffer.clear();
	}
	else
	{
		newTriangleSel->reserve(3*editModeBuffer.size()); // Max size
		newTriangleSel->clear();
	}
	for (int i=0;i<int(_editionIndices.size())/3;i++)
	{
		if (used[_editionIndices[3*i+0]]&&
			used[_editionIndices[3*i+1]]&&
			used[_editionIndices[3*i+2]] )
		{
			if (newTriangleSel==NULL)
				editModeBuffer.push_back(i);
			else
				newTriangleSel->push_back(i);
		}
	}
}

void CObjCont::selectionFromVertexToEdgeEditMode(std::vector<int>* newEdgeSel)
{ // newEdgeSel is NULL by default. In that case, the change is made in the editModeBuffer
	std::vector<bool> used(_editionVertices.size(),false);
	for (int i=0;i<int(editModeBuffer.size());i++)
		used[editModeBuffer[i]]=true;
	if (newEdgeSel==NULL)
	{
		editModeBuffer.reserve(3*editModeBuffer.size()); // Max size
		editModeBuffer.clear();
	}
	else
	{
		newEdgeSel->reserve(3*editModeBuffer.size()); // Max size
		newEdgeSel->clear();
	}
	for (int i=0;i<int(_edgeCont.allEdges.size())/2;i++)
	{
		if (used[_edgeCont.allEdges[2*i+0]]&&
			used[_edgeCont.allEdges[2*i+1]] )
		{
			if (newEdgeSel==NULL)
				editModeBuffer.push_back(i);
			else
				newEdgeSel->push_back(i);
		}
	}
}

void CObjCont::selectionFromEdgeToTriangleEditMode(std::vector<int>* newTriangleSel)
{ // newTriangleSel is NULL by default. In that case, the change is made in the editModeBuffer
	std::vector<bool> used(_edgeCont.allEdges.size(),false);
	for (int i=0;i<int(editModeBuffer.size());i++)
		used[editModeBuffer[i]]=true;
	if (newTriangleSel==NULL)
		editModeBuffer.clear();
	else
		newTriangleSel->clear();
	for (int i=0;i<int(_editionIndicesToEdgesIndex.size())/3;i++)
	{
		if (used[_editionIndicesToEdgesIndex[3*i+0]]&&
			used[_editionIndicesToEdgesIndex[3*i+1]]&&
			used[_editionIndicesToEdgesIndex[3*i+2]] )
		{
			if (newTriangleSel==NULL)
				editModeBuffer.push_back(i);
			else
				newTriangleSel->push_back(i);
		}
	}
}

void CObjCont::actualizeLights(CViewableBase* viewable,bool forDynamicContentOnly)
{ // Actualize the lights positions
	App::ct->environment->activateAmbientLight((viewable->getDisabledColorComponents()&1)==0);
	GLfloat lightPos[]={0.0f,0.0f,0.0f,1.0f};
	GLfloat lightDir[3];
	for (int i=0;i<CIloIlo::maximumOpenGlLights;i++)
		glDisable(GL_LIGHT0+i);
	int activeLightCounter=0;
	bool useLocalLights=viewable->getuseLocalLights();
	for (int i=0;i<int(lightList.size());i++)
	{
		CLight* light=(CLight*)getObject(lightList[i]);
		bool LocalLight=light->getLightIsLocal();
		if ( ((!useLocalLights)&&(!LocalLight))||(useLocalLights&&LocalLight&&light->isObjectParentedWith(viewable)) )
		{
			if ((light->getLightActive())&&(activeLightCounter<CIloIlo::maximumOpenGlLights))
			{
				float m[4][4];
				light->getCumulativeTransformationMatrix(m);
				if (light->getLightType()==sim_light_directional_subtype)
				{
					lightPos[0]=-m[0][2];
					lightPos[1]=-m[1][2];
					lightPos[2]=-m[2][2];
					lightPos[3]=0.0f;
				}
				else 
				{
					lightPos[0]=m[0][3];
					lightPos[1]=m[1][3];
					lightPos[2]=m[2][3];
					lightPos[3]=1.0f;
				}
				lightDir[0]=m[0][2];
				lightDir[1]=m[1][2];
				lightDir[2]=m[2][2];
				glLightfv(GL_LIGHT0+activeLightCounter,GL_POSITION,lightPos);
				glLightfv(GL_LIGHT0+activeLightCounter,GL_SPOT_DIRECTION,lightDir);
				if (light->getLightType()==sim_light_omnidirectional_subtype) 
					glLightf(GL_LIGHT0+activeLightCounter,GL_SPOT_CUTOFF,180.0f);
				if (light->getLightType()==sim_light_directional_subtype) 
					glLightf(GL_LIGHT0+activeLightCounter,GL_SPOT_CUTOFF,90.0f);
				if (light->getLightType()==sim_light_spot_subtype)
				{
					float coa=light->getSpotCutoffAngle()*radToDeg;
					if (coa>89.0f) // 90.0f causes problems on MacOS!!!
						coa=89.0f;
					glLightf(GL_LIGHT0+activeLightCounter,GL_SPOT_CUTOFF,coa); 
				}
				glLightf(GL_LIGHT0+activeLightCounter,GL_SPOT_EXPONENT,float(light->getSpotExponent())); // glLighti & GL_SPOT_EXPONENT causes problems on MacOS!!!

				float black[4]={0.0f,0.0f,0.0f,1.0f};
				if ((viewable->getDisabledColorComponents()&1)==0)
				{
					float ambientLight[4]={light->lightColor.colors[0],light->lightColor.colors[1],light->lightColor.colors[2],1.0f};
					glLightfv(GL_LIGHT0+activeLightCounter,GL_AMBIENT,ambientLight);
				}
				else
					glLightfv(GL_LIGHT0+activeLightCounter,GL_AMBIENT,black);
				if ((viewable->getDisabledColorComponents()&2)==0)
				{
					float diffuseLight[4]={light->lightColor.colors[3],light->lightColor.colors[4],light->lightColor.colors[5],1.0f};
					glLightfv(GL_LIGHT0+activeLightCounter,GL_DIFFUSE,diffuseLight);
				}
				else
					glLightfv(GL_LIGHT0+activeLightCounter,GL_DIFFUSE,black);
				if ((viewable->getDisabledColorComponents()&4)==0)
				{
					float specularLight[4]={light->lightColor.colors[6],light->lightColor.colors[7],light->lightColor.colors[8],1.0f};
					glLightfv(GL_LIGHT0+activeLightCounter,GL_SPECULAR,specularLight);
				}
				else
					glLightfv(GL_LIGHT0+activeLightCounter,GL_SPECULAR,black);
				glLightf(GL_LIGHT0+activeLightCounter,GL_CONSTANT_ATTENUATION,
					light->getAttenuationFactor(CONSTANT_ATTENUATION));
				glLightf(GL_LIGHT0+activeLightCounter,GL_LINEAR_ATTENUATION,
					light->getAttenuationFactor(LINEAR_ATTENUATION));
				glLightf(GL_LIGHT0+activeLightCounter,GL_QUADRATIC_ATTENUATION,
					light->getAttenuationFactor(QUADRATIC_ATTENUATION));
				glEnable(GL_LIGHT0+activeLightCounter);
				activeLightCounter++;
			}
		}
		if (activeLightCounter>=CIloIlo::maximumOpenGlLights)
			break;
	}
	float fog_color[4]={App::ct->environment->fogBackgroundColor[0],
						App::ct->environment->fogBackgroundColor[1],
						App::ct->environment->fogBackgroundColor[2],1.0f};
	GLenum fogType[3]={GL_LINEAR,GL_EXP,GL_EXP2};
	glFogfv(GL_FOG_COLOR,fog_color);
	glFogi(GL_FOG_MODE,fogType[App::ct->environment->fogType]);
	float dd=viewable->getFogStrength();
	float farClipp=viewable->getFarClippingPlane();
	glFogf(GL_FOG_START,App::ct->environment->fogStart*dd+(1.0f-dd)*farClipp);
	glFogf(GL_FOG_END,App::ct->environment->fogEnd*dd+(1.0f-dd)*farClipp);
	glFogf(GL_FOG_DENSITY,App::ct->environment->fogDensity*dd);
	if (App::ct->environment->fogEnabled&&viewable->getShowFogIfAvailable()&&(!forDynamicContentOnly)&&((getEditModeType()&SHAPE_EDIT_MODE)==0)&&(getEditModeType()!=PATH_EDIT_MODE))
	{
		glEnable(GL_FOG);
		CViewableBase::fogWasActivated=true;
	}
	else
	{
		glDisable(GL_FOG);
		CViewableBase::fogWasActivated=false;
	}
}

void CObjCont::prepareAuxClippingPlanes()
{
	_prepareOrEnableAuxClippingPlanes(true,-1);
}

void CObjCont::enableAuxClippingPlanes(int objID)
{
	_prepareOrEnableAuxClippingPlanes(false,objID);
}

void CObjCont::_prepareOrEnableAuxClippingPlanes(bool prepare,int objID)
{
	if (App::ct->mainSettings->clippingPlanesDisabled)
		return;
	int cpi=0;
	for (int i=0;i<int(mirrorList.size());i++)
	{
		if (cpi<5)
		{
			CMirror* it=getMirror(mirrorList[i]);
			if ( (!it->getIsMirror())&&it->getActive() )
			{
				if (prepare)
				{
					C7Vector mtr(it->getCumulativeTransformation());
					C3Vector mtrN(mtr.Q.getMatrix().axis[2]);
					float d=(mtrN*mtr.X);
					double cpv[4]={-mtrN(0),-mtrN(1),-mtrN(2),d};
					glClipPlane(GL_CLIP_PLANE1+cpi,cpv);
				}
				else
				{
					int clipObj=it->getClippingObjectOrCollection();
					bool clipIt=false;
					if ( (clipObj>=SIM_IDSTART_COLLECTION)&&(clipObj<=SIM_IDEND_COLLECTION) )
					{ // collection
						CRegGroup* coll=App::ct->collections->getGroup(clipObj);
						clipIt=coll->isObjectInGroup(objID);
					}
					else
						clipIt=(clipObj==-1)||(clipObj==objID);
					if (clipIt)
						glEnable(GL_CLIP_PLANE1+cpi);
				}
				cpi++;
			}
		}
	}
}

void CObjCont::disableAuxClippingPlanes()
{
	for (int i=0;i<7;i++)
		glDisable(GL_CLIP_PLANE1+i);
}


bool CObjCont::enterEditMode(int objID,int modeType)
{
	deselectObjects();
	if (getEditModeType()!=NO_EDIT_MODE)
		return(false);
	objectsAddedDuringEditMode.clear();

	if (modeType&SHAPE_EDIT_MODE)
	{
		CShape* shape=getShape(objID);
		if (shape==NULL)
			return(false);
		if (shape->isGrouping())
			return(false);
		if (shape->geomData==NULL)
			return(false);
		deselectEditModeBuffer();
		deselectObjects();
		_editModeObject=objID;
		_editModeType=modeType;
		_editionVertices.clear();
		_editionIndices.clear();
		_editionNormals.clear();
		_editionTextureCoords.clear();
		_edgeCont.clearEdges();
		_editionIndicesToEdgesIndex.clear();

		shape->geomData->geomInfo->getCumulativeMeshes(_editionVertices,&_editionIndices,&_editionNormals);
		_editionTextureProperty=((CGeometric*)shape->geomData->geomInfo)->getTextureProperty();
		if (_editionTextureProperty!=NULL)
		{
			if (!((CGeometric*)shape->geomData->geomInfo)->getNonCalculatedTextureCoordinates(_editionTextureCoords))
				_editionTextureProperty=NULL; // texture coordinates are calculated, so we don't care
		}
		if (_editionTextureProperty!=NULL)
		{
			CTextureObject* to=App::ct->textureCont->getObject(_editionTextureProperty->getTextureObjectID());
			if (to!=NULL)
			{
				to->getTextureBuffer(_editionTexture);
				to->lightenUp();
			}
		}
		if (_editionTextureCoords.size()==0)
			_editionTextureCoords.resize(_editionIndices.size()*2,0.0f); // when we work with no textures, we just use dummy texture coordinates

		actualizeEditModeEditionEdges();
		if (App::mainWindow!=NULL)
			App::mainWindow->dlgCont->openOrBringToFront(SHAPE_EDITION_DLG);
	}
	else if (modeType&PATH_EDIT_MODE)
	{
		CPath* path=getPath(objID);
		if (path==NULL)
			return(false);
		deselectEditModeBuffer();
		deselectObjects();
		_editModeObject=objID;
		_editModeType=modeType;
		_editionPath=path->pathContainer->copyYourself();
		if (App::mainWindow!=NULL)
			App::mainWindow->dlgCont->openOrBringToFront(PATH_EDITION_DLG);
	}
	else if (modeType&BUTTON_EDIT_MODE)
	{
		_editModeObject=-1;
		_editModeType=modeType;
		App::ct->buttonBlockContainer->setButtonEditMode(true);
		if (App::mainWindow!=NULL)
			App::mainWindow->dlgCont->openOrBringToFront(CUSTOM_UI_DLG);
	}
	setFullDialogRefreshFlag();

	bool originalState=App::ct->oglSurface->isBrowserEnabled();
	App::ct->oglSurface->setBrowserEnabled(false);
	App::ct->oglSurface->setBrowserEnabled(originalState);

	App::ct->closeTemporarilyNonEditModeDialogs();
	App::ct->hierarchy->setRebuildHierarchyFlag();
	App::ct->hierarchy->setResetViewFlag();
	toolbarRefreshFlag=true;
	if (App::mainWindow!=NULL)
		App::mainWindow->createDefaultMenuBar(false);
	return(true);
}

void CObjCont::endEditMode(bool cancelChanges)
{
	if (getEditModeType()==NO_EDIT_MODE)
		return;
	int objectIDToSelect=-1;
	if (getEditModeType()&SHAPE_EDIT_MODE)
	{
		deselectEditModeBuffer();
		deselectObjects();
		C3DObject* it=getObject(_editModeObject);
		if ((it!=NULL)&&(!cancelChanges))
		{
			if (it->getObjectType()==sim_object_shape_type) // Just in case
			{
				CShape* sh=(CShape*)it;
				C7Vector oldTr(sh->getCumulativeTransformationPart1());
				CGeomProxy* g=sh->geomData;
				CGeometric* gc=(CGeometric*)g->geomInfo;
				gc->setPurePrimitiveType(sim_pure_primitive_none,1.0f,1.0f,1.0f); // disable the pure characteristic
				CMeshManip::checkVerticesIndicesNormalsTexCoords(_editionVertices,_editionIndices,NULL,&_editionTextureCoords,App::userSettings->identicalVerticesCheck,App::userSettings->identicalVerticesTolerance,App::userSettings->identicalTrianglesCheck);

//				_editionNormals.clear(); // we will calculate them
//				_editionNormals.reserve(_editionIndices.size()*3);
				if (_editionVertices.size()!=0)
				{ // The shape is not empty
//					CMeshManip::getNormals(&_editionVertices,&_editionIndices,&_editionNormals); // we get calculated normals anyway
					gc->setMesh(_editionVertices,_editionIndices,NULL,C7Vector::identityTransformation); // will do the convectivity test
					gc->actualizeGouraudShadingAndVisibleEdges();
					g->removeCollisionInformation();
					// handle textures:
					CTextureProperty* tp=gc->getTextureProperty();
					if (tp!=NULL)
					{
						if (tp->getFixedCoordinates())
						{
							if (_editionTextureCoords.size()/2!=_editionIndices.size())
							{ // should normally never happen
								App::ct->textureCont->announceGeneralObjectWillBeErased(sh->getID(),-1);
								delete tp;
								gc->setTextureProperty(NULL);
							}
							else
								tp->setFixedCoordinates(&_editionTextureCoords);
						}
					}

					/*
					// Remove any texture that has fixed coordinates:
					CTextureProperty* tp=gc->getTextureProperty();
					if (tp!=NULL)
					{
						if (tp->getFixedCoordinates())
						{
							App::ct->textureCont->announceGeneralObjectWillBeErased(sh->getID(),-1);
							delete tp;
							gc->setTextureProperty(NULL);
						}
					}
					*/

					sh->alignBoundingBoxWithMainAxis();
					objectIDToSelect=sh->getID();
					sh->incrementMemorizedConfigurationValidCounter(); 
				}
				else
				{ // The shape is empty!!! We have to remove it!
					eraseObject(sh);
				}
			}
		}
		else
		{
			if (it!=NULL)
				objectIDToSelect=it->getID();
		}

		// We empty the edition data
		_editionVertices.clear();
		std::vector<float>(_editionVertices).swap(_editionVertices);
		_editionIndices.clear();
		std::vector<int>(_editionIndices).swap(_editionIndices);
		_editionNormals.clear();
		std::vector<float>(_editionNormals).swap(_editionNormals);
		_editionTextureCoords.clear();
		std::vector<float>(_editionTextureCoords).swap(_editionTextureCoords);
		if (_editionTextureProperty!=NULL)
		{ // reset to original texture
			CTextureObject* to=App::ct->textureCont->getObject(_editionTextureProperty->getTextureObjectID());
			if (to!=NULL)
				to->setTextureBuffer(_editionTexture);
		}
		_editionTexture.clear();
		_editionTextureProperty=NULL;
		_edgeCont.clearEdges();
		std::vector<int>(_edgeCont.allEdges).swap(_edgeCont.allEdges);
		_editionIndicesToEdgesIndex.clear();
		std::vector<int>(_editionIndicesToEdgesIndex).swap(_editionIndicesToEdgesIndex);
		editModeBuffer.clear();
		editBufferVerticesCopy.clear();
		editBufferIndicesCopy.clear();
		editBufferNormalsCopy.clear();
		editBufferTextureCoordsCopy.clear();
		if (App::mainWindow!=NULL)
			App::mainWindow->dlgCont->close(SHAPE_EDITION_DLG);
	}
	else if (getEditModeType()&PATH_EDIT_MODE)
	{
		deselectEditModeBuffer();
		deselectObjects();
		for (int i=0;i<int(editBufferPathPointsCopy.size());i++)
			delete editBufferPathPointsCopy[i];
		editBufferPathPointsCopy.clear();
		CPath* path=getPath(_editModeObject);
		if ((path!=NULL)&&(!cancelChanges))
		{
			deselectEditModeBuffer();
			deselectObjects();
			delete path->pathContainer;
			path->pathContainer=_editionPath;
		}
		else
			delete _editionPath;
		_editionPath=NULL;
		editModeBuffer.clear();
		if (path!=NULL)
			objectIDToSelect=path->getID();
		if (App::mainWindow!=NULL)
			App::mainWindow->dlgCont->close(PATH_EDITION_DLG);
	}
	else if (getEditModeType()&BUTTON_EDIT_MODE)
	{
		App::ct->buttonBlockContainer->setButtonEditMode(false);
		if (App::mainWindow!=NULL)
			App::mainWindow->dlgCont->close(CUSTOM_UI_DLG);
	}
	_editModeType=NO_EDIT_MODE;
	_editModeObject=-1;
	setFullDialogRefreshFlag();

	bool originalState=App::ct->oglSurface->isBrowserEnabled();
	App::ct->oglSurface->setBrowserEnabled(false);
	App::ct->oglSurface->setBrowserEnabled(originalState);

	if (cancelChanges)
	{
		objectsAddedDuringEditMode.clear();
	}

	App::ct->hierarchy->setResetViewFlag(); 
	toolbarRefreshFlag=true;
	selectObject(objectIDToSelect);
	App::ct->reopenTemporarilyClosedNonEditModeDialogs();
	if (App::mainWindow!=NULL)	
		App::mainWindow->createDefaultMenuBar(false);
}

int CObjCont::getEditModeObjectID()
{
	return(_editModeObject);
}
int CObjCont::getEditModeType()
{
	return(_editModeType);
}
void CObjCont::swapShapeEditModeType(int theType)
{
	if (getEditModeType()&SHAPE_EDIT_MODE)
		_editModeType=theType;
}

void CObjCont::displayVertices(int displayAttrib) // all edit mode routines should go somewhere else!!!
{ // Use only in edit mode!!! Solid is true by default
	ogl::setColorsAllBlack();
	if ((displayAttrib&sim_displayattribute_colorcodedpickpass)==0)
	{
		float selColor[3]={1.0f,1.0f,0.0f};
		float nselColor[3]={1.0f,0.0f,0.0f};
		float fSelColor[3]={1.0f,1.0f,1.0f};

		ogl::setColorsAllBlack();
		ogl::setColor(0.5f,0.5f,0.5f,DIFFUSE_MODE);
		ogl::setColor(0.5f,0.5f,0.5f,SPECULAR_MODE);
		ogl::setColor(0.4f,0.4f,1.0f,AMBIENT_MODE);
		glPushAttrib(GL_POLYGON_BIT);


		glPolygonOffset(0.5f,0.0f);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
		glLoadName(-1);

		if (_editionTextureProperty!=NULL)
			_editionTextureProperty->start3DTextureDisplay();

		// Draw the filled triangles
		glBegin(GL_TRIANGLES);
		if (_editionTextureProperty!=NULL)
		{
			for (int i=0;i<int(_editionIndices.size());i++)
			{
				glNormal3fv(&_editionNormals[3*i]);
				glTexCoord2fv(&_editionTextureCoords[2*i]);
				glVertex3fv(&_editionVertices[3*_editionIndices[i]]);
			}
		}
		else
		{
			for (int i=0;i<int(_editionIndices.size());i++)
			{
				glNormal3fv(&_editionNormals[3*i]);
				glVertex3fv(&_editionVertices[3*_editionIndices[i]]);
			}
		}
		glEnd();

		if (_editionTextureProperty!=NULL)
			_editionTextureProperty->end3DTextureDisplay();

		glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
		glDisable(GL_POLYGON_OFFSET_FILL);
		if (displayAttrib&sim_displayattribute_renderpass)
		{
			glLoadName(-1);
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			glDisable(GL_CULL_FACE);
			ogl::setColorsAllBlack();
			glBegin(GL_TRIANGLES);
			for (int i=0;i<int(_editionIndices.size());i++)
				glVertex3f(_editionVertices[3*_editionIndices[i]+0],_editionVertices[3*_editionIndices[i]+1],_editionVertices[3*_editionIndices[i]+2]);
			glEnd();
		}


		// We prepare an array indicating which vertices were already drawn:
		std::vector<bool> drawn(_editionVertices.size()/3,false);

		// First we display the selected vertices:
		glPointSize(4.0f);
		if (showHiddenVerticeAndEdges)
		{
			glDepthMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);
		}

		ogl::setColor(selColor,EMISSION_MODE);
		for (int j=0;j<int(App::ct->objCont->editModeBuffer.size());j++)
		{
			int i=App::ct->objCont->editModeBuffer[j];
			glLoadName(i);
			drawn[i]=true;
			if (j==int(App::ct->objCont->editModeBuffer.size())-1)
				ogl::setColor(fSelColor,EMISSION_MODE); // First selection
			glBegin(GL_POINTS); // Important here because of glLoadName!
			glVertex3f(_editionVertices[3*i+0],_editionVertices[3*i+1],_editionVertices[3*i+2]);
			glEnd();
		}
		// Now we draw all unselected vertices
		ogl::setColor(nselColor,EMISSION_MODE);
		for (int i=0;i<int(_editionVertices.size())/3;i++)
		{
			if (!drawn[i])
			{
				glLoadName(i);
				glBegin(GL_POINTS); // Important here because of glLoadName!
				glVertex3f(_editionVertices[3*i+0],_editionVertices[3*i+1],_editionVertices[3*i+2]);
				glEnd();
			}
		}
		glDisable(GL_POLYGON_OFFSET_POINT);
		glPointSize(1.0f); // Important to reset it
		glLoadName(-1);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glPopAttrib();
	}
	else
	{
		glPushAttrib(GL_POLYGON_BIT);
		glPolygonOffset(0.5f,0.0f);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
		glDisable(GL_DITHER);
		glColor3ub(0,0,0);

		// Draw the filled triangles
		glBegin(GL_TRIANGLES);
		for (int i=0;i<int(_editionIndices.size());i++)
			glVertex3fv(&_editionVertices[3*_editionIndices[i]]);
		glEnd();

		glDisable(GL_POLYGON_OFFSET_FILL);
		glPointSize(4.0f);
		if (showHiddenVerticeAndEdges)
		{
			glDepthMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);
		}

		int j;
		for (int i=0;i<int(_editionVertices.size())/3;i++)
		{
			j=i+1;
			glColor3ub(j&255,(j>>8)&255,(j>>16)&255);
			glBegin(GL_POINTS);
			glVertex3f(_editionVertices[3*i+0],_editionVertices[3*i+1],_editionVertices[3*i+2]);
			glEnd();
		}

		glDisable(GL_POLYGON_OFFSET_POINT);
		glPointSize(1.0f); // Important to reset it
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_DITHER);
		glPopAttrib();
	}
}

void CObjCont::displayFaceOrientation(int displayAttrib) // all edit mode routines should go somewhere else!!!
{ // Use only in edit mode!!!
	ogl::setColorsAllBlack();
	if ((displayAttrib&sim_displayattribute_colorcodedpickpass)==0)
	{
		ogl::setColor(0.5f,0.5f,0.5f,DIFFUSE_MODE);
		ogl::setColor(0.5f,0.5f,0.5f,SPECULAR_MODE);
		glPushAttrib(GL_POLYGON_BIT);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glPolygonOffset(0.5f,0.0f); // Second argument set to 0.0 on 2009.01.05 (otherwise strange effects on some graphic cards)
		glEnable(GL_POLYGON_OFFSET_FILL);

		if (_editionTextureProperty!=NULL)
			_editionTextureProperty->start3DTextureDisplay();

		// Now we display the inside faces...
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		for (int i=0;i<int(_editionIndices.size())/3;i++)
		{
			glLoadName(i);
			if (App::ct->objCont->alreadyInEditModeBuffer(i))
			{
				if (App::ct->objCont->editModeBuffer[App::ct->objCont->editModeBuffer.size()-1]==i)
					ogl::setColor(1.0f,1.0f,1.0f,AMBIENT_MODE); // Last selection
				else
					ogl::setColor(1.0f,1.0f,0.0f,AMBIENT_MODE); // Not last selection
			}
			else
				ogl::setColor(1.0f,0.4f,0.4f,AMBIENT_MODE);

			glBegin(GL_TRIANGLES);
			if (_editionTextureProperty!=NULL)
			{
				glNormal3fv(&_editionNormals[9*i+0]);
				glTexCoord2fv(&_editionTextureCoords[6*i+0]);
				glVertex3fv(&_editionVertices[3*_editionIndices[3*i+0]]);

				glNormal3fv(&_editionNormals[9*i+3]);
				glTexCoord2fv(&_editionTextureCoords[6*i+2]);
				glVertex3fv(&_editionVertices[3*_editionIndices[3*i+1]]);

				glNormal3fv(&_editionNormals[9*i+6]);
				glTexCoord2fv(&_editionTextureCoords[6*i+4]);
				glVertex3fv(&_editionVertices[3*_editionIndices[3*i+2]]);
			}
			else
			{
				glNormal3fv(&_editionNormals[9*i+0]);
				glVertex3fv(&_editionVertices[3*_editionIndices[3*i+0]]);

				glNormal3fv(&_editionNormals[9*i+3]);
				glVertex3fv(&_editionVertices[3*_editionIndices[3*i+1]]);

				glNormal3fv(&_editionNormals[9*i+6]);
				glVertex3fv(&_editionVertices[3*_editionIndices[3*i+2]]);
			}
			glEnd();
		}
		// ...and now the outside faces:
		glCullFace(GL_BACK);
		for (int i=0;i<int(_editionIndices.size())/3;i++)
		{
			glLoadName(i);
			if (App::ct->objCont->alreadyInEditModeBuffer(i))
			{
				if (App::ct->objCont->editModeBuffer[App::ct->objCont->editModeBuffer.size()-1]==i)
					ogl::setColor(1.0f,1.0f,1.0f,AMBIENT_MODE); // Last selection
				else
					ogl::setColor(1.0f,1.0f,0.0f,AMBIENT_MODE); // Not last selection
			}
			else
				ogl::setColor(0.4f,0.4f,1.0f,AMBIENT_MODE);

			glBegin(GL_TRIANGLES);
			if (_editionTextureProperty!=NULL)
			{
				glNormal3fv(&_editionNormals[9*i+0]);
				glTexCoord2fv(&_editionTextureCoords[6*i+0]);
				glVertex3fv(&_editionVertices[3*_editionIndices[3*i+0]]);

				glNormal3fv(&_editionNormals[9*i+3]);
				glTexCoord2fv(&_editionTextureCoords[6*i+2]);
				glVertex3fv(&_editionVertices[3*_editionIndices[3*i+1]]);

				glNormal3fv(&_editionNormals[9*i+6]);
				glTexCoord2fv(&_editionTextureCoords[6*i+4]);
				glVertex3fv(&_editionVertices[3*_editionIndices[3*i+2]]);
			}
			else
			{
				glNormal3fv(&_editionNormals[9*i+0]);
				glVertex3fv(&_editionVertices[3*_editionIndices[3*i+0]]);

				glNormal3fv(&_editionNormals[9*i+3]);
				glVertex3fv(&_editionVertices[3*_editionIndices[3*i+1]]);

				glNormal3fv(&_editionNormals[9*i+6]);
				glVertex3fv(&_editionVertices[3*_editionIndices[3*i+2]]);
			}
			glEnd();
		}

		if (_editionTextureProperty!=NULL)
			_editionTextureProperty->end3DTextureDisplay();

		// We display all edges in black:
		glDisable(GL_POLYGON_OFFSET_FILL);
		if (displayAttrib&sim_displayattribute_renderpass)
		{
			glLoadName(-1);
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			glDisable(GL_CULL_FACE);
			ogl::setColorsAllBlack();
			glBegin(GL_TRIANGLES);
			for (int i=0;i<int(_editionIndices.size());i++)
				glVertex3f(_editionVertices[3*_editionIndices[i]+0],_editionVertices[3*_editionIndices[i]+1],_editionVertices[3*_editionIndices[i]+2]);
			glEnd();
		}
		glPopAttrib();
	}
	else
	{
		glPushAttrib(GL_POLYGON_BIT);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glDisable(GL_DITHER);
		unsigned int j;
		for (int i=0;i<int(_editionIndices.size())/3;i++)
		{
			j=i+1;
			glColor3ub(j&255,(j>>8)&255,(j>>16)&255);
			glBegin(GL_TRIANGLES);
			glVertex3fv(&_editionVertices[3*_editionIndices[3*i+0]]);
			glVertex3fv(&_editionVertices[3*_editionIndices[3*i+1]]);
			glVertex3fv(&_editionVertices[3*_editionIndices[3*i+2]]);
			glEnd();
		}
		glEnable(GL_DITHER);
		glPopAttrib();
	}
}

void CObjCont::displayEdgeEditMode(int displayAttrib) // all edit mode routines should go somewhere else!!!
{ // Use only in edit mode!!! Solid is true by default
	ogl::setColorsAllBlack();
	if ((displayAttrib&sim_displayattribute_colorcodedpickpass)==0)
	{
		float selColor[3]={1.0f,1.0f,0.0f};
		float nselColor[3]={1.0f,0.0f,0.0f};
		ogl::setColor(0.5f,0.5f,0.5f,DIFFUSE_MODE);
		ogl::setColor(0.5f,0.5f,0.5f,SPECULAR_MODE);
		ogl::setColor(0.4f,0.4f,1.0f,AMBIENT_MODE);
		glPushAttrib(GL_POLYGON_BIT);


		glPolygonOffset(0.5f,0.0f); // Second argument set to 0.0 on 2009.01.05 (otherwise strange effects on some graphic cards)
		glEnable(GL_POLYGON_OFFSET_FILL); //
		glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
		glLoadName(-1);

		if (_editionTextureProperty!=NULL)
			_editionTextureProperty->start3DTextureDisplay();

		// Draw the filled triangles
		glBegin(GL_TRIANGLES);
		if (_editionTextureProperty!=NULL)
		{
			for (int i=0;i<int(_editionIndices.size());i++)
			{
				glNormal3fv(&_editionNormals[3*i]);
				glTexCoord2fv(&_editionTextureCoords[2*i]);
				glVertex3fv(&_editionVertices[3*_editionIndices[i]]);
			}
		}
		else
		{
			for (int i=0;i<int(_editionIndices.size());i++)
			{
				glNormal3fv(&_editionNormals[3*i]);
				glVertex3fv(&_editionVertices[3*_editionIndices[i]]);
			}
		}
		glEnd();

		if (_editionTextureProperty!=NULL)
			_editionTextureProperty->end3DTextureDisplay();

		glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);

		glDisable(GL_POLYGON_OFFSET_FILL);
		if (displayAttrib&sim_displayattribute_renderpass)
		{
			glLoadName(-1);
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			glDisable(GL_CULL_FACE);
			ogl::setColorsAllBlack();
			glBegin(GL_TRIANGLES);
			for (int i=0;i<int(_editionIndices.size());i++)
				glVertex3f(_editionVertices[3*_editionIndices[i]+0],_editionVertices[3*_editionIndices[i]+1],_editionVertices[3*_editionIndices[i]+2]);
			glEnd();
		}
		std::vector<bool> usedEdges(_edgeCont.allEdges.size()/2,false);
		if (displayAttrib&sim_displayattribute_pickpass)
			glLineWidth(6.0f);
		else
			glLineWidth(2.0f);
		if (showHiddenVerticeAndEdges)
		{
			glDepthMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);
		}
		// first selected edges:
		ogl::setColor(selColor,EMISSION_MODE);
		for (int i=0;i<int(editModeBuffer.size());i++)
		{
			int ind[2]={_edgeCont.allEdges[2*editModeBuffer[i]+0],_edgeCont.allEdges[2*editModeBuffer[i]+1]};
			usedEdges[editModeBuffer[i]]=true;
			glLoadName(editModeBuffer[i]);
			glBegin(GL_LINES);
				glVertex3f(_editionVertices[3*ind[0]+0],_editionVertices[3*ind[0]+1],_editionVertices[3*ind[0]+2]);
				glVertex3f(_editionVertices[3*ind[1]+0],_editionVertices[3*ind[1]+1],_editionVertices[3*ind[1]+2]);
			glEnd();

		}
		if (displayAttrib&sim_displayattribute_pickpass)
			glLineWidth(6.0f);
		else
			glLineWidth(1.0f);
		ogl::setColor(nselColor,EMISSION_MODE);
		for (int i=0;i<int(_edgeCont.allEdges.size()/2);i++)
		{
			if (!usedEdges[i])
			{
				int ind[2]={_edgeCont.allEdges[2*i+0],_edgeCont.allEdges[2*i+1]};
				glLoadName(i);
				glBegin(GL_LINES);
					glVertex3f(_editionVertices[3*ind[0]+0],_editionVertices[3*ind[0]+1],_editionVertices[3*ind[0]+2]);
					glVertex3f(_editionVertices[3*ind[1]+0],_editionVertices[3*ind[1]+1],_editionVertices[3*ind[1]+2]);
				glEnd();
			}
		}

		glLineWidth(1.0f);

		glLoadName(-1);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glPopAttrib();
	}
	else
	{
		glPushAttrib(GL_POLYGON_BIT);
		glPolygonOffset(0.5f,0.0f); // Second argument set to 0.0 on 2009.01.05 (otherwise strange effects on some graphic cards)
		glEnable(GL_POLYGON_OFFSET_FILL); //
		glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
		glDisable(GL_DITHER);
		glColor3ub(0,0,0);

		// Draw the filled triangles
		glBegin(GL_TRIANGLES);
		for (int i=0;i<int(_editionIndices.size());i++)
			glVertex3fv(&_editionVertices[3*_editionIndices[i]]);
		glEnd();

		glDisable(GL_POLYGON_OFFSET_FILL);
		glLineWidth(6.0f);
		if (showHiddenVerticeAndEdges)
		{
			glDepthMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);
		}

		int j;
		for (int i=0;i<int(_edgeCont.allEdges.size()/2);i++)
		{
			int ind[2]={_edgeCont.allEdges[2*i+0],_edgeCont.allEdges[2*i+1]};
			j=i+1;
			glColor3ub(j&255,(j>>8)&255,(j>>16)&255);
			glBegin(GL_LINES);
				glVertex3f(_editionVertices[3*ind[0]+0],_editionVertices[3*ind[0]+1],_editionVertices[3*ind[0]+2]);
				glVertex3f(_editionVertices[3*ind[1]+0],_editionVertices[3*ind[1]+1],_editionVertices[3*ind[1]+2]);
			glEnd();
		}

		glLineWidth(1.0f);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_DITHER);
		glPopAttrib();
	}
}


void CObjCont::flipTriangle(int index) // all edit mode routines should go somewhere else!!!
{ // Use only in edit mode!!
	int save;
	float normSave;
	save=_editionIndices[3*index+0];
	_editionIndices[3*index+0]=_editionIndices[3*index+1];
	_editionIndices[3*index+1]=save;
	// We have to report the changes to the _editionIndicesToEdgesIndex:
	save=_editionIndicesToEdgesIndex[3*index+0];
	_editionIndicesToEdgesIndex[3*index+0]=_editionIndicesToEdgesIndex[3*index+1];
	_editionIndicesToEdgesIndex[3*index+1]=save;
	// Now the normals:
	normSave=-_editionNormals[3*(3*index+0)+0];
	_editionNormals[3*(3*index+0)+0]=-_editionNormals[3*(3*index+1)+0];
	_editionNormals[3*(3*index+2)+0]=-_editionNormals[3*(3*index+2)+0];
	_editionNormals[3*(3*index+1)+0]=normSave;
	normSave=-_editionNormals[3*(3*index+0)+1];
	_editionNormals[3*(3*index+0)+1]=-_editionNormals[3*(3*index+1)+1];
	_editionNormals[3*(3*index+2)+1]=-_editionNormals[3*(3*index+2)+1];
	_editionNormals[3*(3*index+1)+1]=normSave;
	normSave=-_editionNormals[3*(3*index+0)+2];
	_editionNormals[3*(3*index+0)+2]=-_editionNormals[3*(3*index+1)+2];
	_editionNormals[3*(3*index+2)+2]=-_editionNormals[3*(3*index+2)+2];
	_editionNormals[3*(3*index+1)+2]=normSave;
}

void CObjCont::addTriangle(int ind1,int ind2,int ind3) // all edit mode routines should go somewhere else!!!
{ // Use only in edit mode!!
	_editionIndices.push_back(ind1);
	_editionIndices.push_back(ind2);
	_editionIndices.push_back(ind3);
	int index=_edgeCont.getEdgeIndex(ind1,ind2);
	if (index==-1)
	{ // edge doesn't not yet exist
		index=_edgeCont.addEdge(ind1,ind2);
	}
	_editionIndicesToEdgesIndex.push_back(index);
	index=_edgeCont.getEdgeIndex(ind1,ind3);
	if (index==-1)
	{ // edge doesn't not yet exist
		index=_edgeCont.addEdge(ind1,ind3);
	}
	_editionIndicesToEdgesIndex.push_back(index);
	index=_edgeCont.getEdgeIndex(ind3,ind2);
	if (index==-1)
	{ // edge doesn't not yet exist
		index=_edgeCont.addEdge(ind3,ind2);
	}
	_editionIndicesToEdgesIndex.push_back(index);
	float v[3][3];
	v[0][0]=_editionVertices[3*ind1+0];
	v[0][1]=_editionVertices[3*ind1+1];
	v[0][2]=_editionVertices[3*ind1+2];
	v[1][0]=_editionVertices[3*ind2+0];
	v[1][1]=_editionVertices[3*ind2+1];
	v[1][2]=_editionVertices[3*ind2+2];
	v[2][0]=_editionVertices[3*ind3+0];
	v[2][1]=_editionVertices[3*ind3+1];
	v[2][2]=_editionVertices[3*ind3+2];
	float n[3];
	CMeshManip::calcNormal(v,n);
	for (int i=0;i<3;i++)
	{
		_editionNormals.push_back(n[0]);
		_editionNormals.push_back(n[1]);
		_editionNormals.push_back(n[2]);
	}
	for (int i=0;i<6;i++)
		_editionTextureCoords.push_back(0.0f);
}

C3Vector CObjCont::getRelativeVertexPosition(int index) // all edit mode routines should go somewhere else!!!
{ // Use only in edit mode!!
	return(C3Vector(_editionVertices[3*index+0],_editionVertices[3*index+1],_editionVertices[3*index+2]));
}
void CObjCont::setRelativeVertexPosition(int index,const C3Vector& v) // all edit mode routines should go somewhere else!!!
{ // Use only in edit mode!!
	_editionVertices[3*index+0]=v(0);
	_editionVertices[3*index+1]=v(1);
	_editionVertices[3*index+2]=v(2);
}

													
C3Vector CObjCont::getAbsoluteVertexPosition(int index) // all edit mode routines should go somewhere else!!!
{
	C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->getEditModeObjectID());
	C3Vector v(_editionVertices[3*index+0],
				_editionVertices[3*index+1],
				_editionVertices[3*index+2]);
	if (it!=NULL)
	{
		C7Vector m(it->getCumulativeTransformationPart1());
		return(m*v);
	}
	else
		return(v);
}
void CObjCont::setAbsoluteVertexPosition(int index,const C3Vector& v) // all edit mode routines should go somewhere else!!!
{ // Use only in edit mode!!!
	C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->getEditModeObjectID());
	if (it!=NULL)
	{
		C7Vector m(it->getCumulativeTransformationPart1().getInverse());
		C3Vector w(m*v);
		_editionVertices[3*index+0]=w(0);
		_editionVertices[3*index+1]=w(1);
		_editionVertices[3*index+2]=w(2);
	}
}

void CObjCont::actualizeEditModeEditionEdges() // all edit mode routines should go somewhere else!!!
{
	_edgeCont.clearEdges();
	_editionIndicesToEdgesIndex.clear();
	for (int i=0;i<int(_editionIndices.size()/3);i++)
	{
		int ind[3]={_editionIndices[3*i+0],_editionIndices[3*i+1],_editionIndices[3*i+2]};
		int index=_edgeCont.getEdgeIndex(ind[0],ind[1]);
		if (index==-1)
		{ // edge doesn't yet exist
			index=_edgeCont.addEdge(ind[0],ind[1]);
		}
		_editionIndicesToEdgesIndex.push_back(index);
		index=_edgeCont.getEdgeIndex(ind[0],ind[2]);
		if (index==-1)
		{ // edge doesn't yet exist
			index=_edgeCont.addEdge(ind[0],ind[2]);
		}
		_editionIndicesToEdgesIndex.push_back(index);
		index=_edgeCont.getEdgeIndex(ind[2],ind[1]);
		if (index==-1)
		{ // edge doesn't yet exist
			index=_edgeCont.addEdge(ind[2],ind[1]);
		}
		_editionIndicesToEdgesIndex.push_back(index);
	}
}

float CObjCont::getEdgeAngle(int edgeID) // all edit mode routines should go somewhere else!!!
{
	int ev[2]={_edgeCont.allEdges[2*edgeID+0],_edgeCont.allEdges[2*edgeID+1]};
	std::vector<C3Vector> normalVectors;
	for (int i=0;i<int(_editionIndices.size()/3);i++)
	{
		int ind[3]={_editionIndices[3*i+0],_editionIndices[3*i+1],_editionIndices[3*i+2]};
		if (	((ev[0]==ind[0])&&(ev[1]==ind[1]))||
				((ev[1]==ind[0])&&(ev[0]==ind[1]))||
				((ev[0]==ind[0])&&(ev[1]==ind[2]))||
				((ev[1]==ind[0])&&(ev[0]==ind[2]))||
				((ev[0]==ind[1])&&(ev[1]==ind[2]))||
				((ev[1]==ind[1])&&(ev[0]==ind[2])) )
		{ // that triangle shares the edge!
			// We search for the vertex that is not contained in the edge:
			int ne=-1;
			for (int j=0;j<3;j++)
			{
				if ( (ind[j]!=ev[0])&&(ind[j]!=ev[1]) )
				{
					ne=j;
					break;
				}
			}
			C3Vector v0(&_editionVertices[3*ev[0]+0]);
			C3Vector v1(&_editionVertices[3*ev[1]+0]);
			C3Vector v2(&_editionVertices[3*ind[ne]+0]);
			C3Vector dv0(v1-v0);
			C3Vector dv1(v2-v0);
			C3Vector n(dv1^dv0);
			n.normalize();
			normalVectors.push_back(n);
		}
	}
	if (normalVectors.size()<2)
		return(piValue);
	// now we search for the smallest scalar product (that gives the smallest edge value):
	float smallestScalarProduct=1.0f; // corresponds to 0 degree edge
	for (int i=0;i<int(normalVectors.size()-1);i++)
	{
		C3Vector v(normalVectors[i]);
		for (int j=i+1;j<int(normalVectors.size());j++)
		{
			C3Vector w(normalVectors[j]);
			float sp=fabs(v*w);
			if (sp<smallestScalarProduct)
				smallestScalarProduct=sp;
		}
	}
	return(CMath::robustAcos(smallestScalarProduct));
}

void CObjCont::getNextEdges(int rearVertex,int frontVertex,std::vector<int>& edgeList) // all edit mode routines should go somewhere else!!!
{
	edgeList.clear();
	std::vector<bool> alreadyTaken(_edgeCont.allEdges.size()/2,false);
	for (int i=0;i<int(_editionIndices.size())/3;i++)
	{
		int ind[3]={_editionIndices[3*i+0],_editionIndices[3*i+1],_editionIndices[3*i+2]};
		if (ind[0]==frontVertex)
		{
			if ( (ind[1]!=rearVertex)&&(!alreadyTaken[_editionIndicesToEdgesIndex[3*i+0]]) )
			{
				edgeList.push_back(_editionIndicesToEdgesIndex[3*i+0]);
				alreadyTaken[_editionIndicesToEdgesIndex[3*i+0]]=true;
			}
			if ( (ind[2]!=rearVertex)&&(!alreadyTaken[_editionIndicesToEdgesIndex[3*i+1]]) )
			{
				edgeList.push_back(_editionIndicesToEdgesIndex[3*i+1]);
				alreadyTaken[_editionIndicesToEdgesIndex[3*i+1]]=true;
			}
		}
		else if (ind[1]==frontVertex)
		{
			if ( (ind[0]!=rearVertex)&&(!alreadyTaken[_editionIndicesToEdgesIndex[3*i+0]]) )
			{
				edgeList.push_back(_editionIndicesToEdgesIndex[3*i+0]);
				alreadyTaken[_editionIndicesToEdgesIndex[3*i+0]]=true;
			}
			if ( (ind[2]!=rearVertex)&&(!alreadyTaken[_editionIndicesToEdgesIndex[3*i+2]]) )
			{
				edgeList.push_back(_editionIndicesToEdgesIndex[3*i+2]);
				alreadyTaken[_editionIndicesToEdgesIndex[3*i+2]]=true;
			}
		}
		else if (ind[2]==frontVertex)
		{
			if ( (ind[0]!=rearVertex)&&(!alreadyTaken[_editionIndicesToEdgesIndex[3*i+1]]) )
			{
				edgeList.push_back(_editionIndicesToEdgesIndex[3*i+1]);
				alreadyTaken[_editionIndicesToEdgesIndex[3*i+1]]=true;
			}
			if ( (ind[1]!=rearVertex)&&(!alreadyTaken[_editionIndicesToEdgesIndex[3*i+2]]) )
			{
				edgeList.push_back(_editionIndicesToEdgesIndex[3*i+2]);
				alreadyTaken[_editionIndicesToEdgesIndex[3*i+2]]=true;
			}
		}
	}
}

void CObjCont::removeFromSelectionAllExceptModelBase(bool keepObjectsSelectedThatAreNotBuiltOnAModelBase)
{ // One model base per hierarchy tree!
	FUNCTION_DEBUG;
	std::vector<int> sel(_selectedObjectIDs);
	deselectObjects();
	std::vector<C3DObject*> modelBases;
	std::vector<C3DObject*> nonModelBasesBuildOnNothing;
	for (int i=0;i<int(sel.size());i++)
	{
		C3DObject* it=getObject(sel[i]);
		if (it->getModelBase())
			modelBases.push_back(it);
		else
		{
			C3DObject* objIt=it;
			bool addIt=true;
			while (objIt->getParent()!=NULL)
			{
				objIt=objIt->getParent();
				if (objIt->getModelBase())
				{
					addIt=false;
					break;
				}
			}
			if (addIt)
				nonModelBasesBuildOnNothing.push_back(it);
		}
	}
	for (int i=0;i<int(modelBases.size());i++)
	{
		C3DObject* it=modelBases[i];
		bool isIndependentBase=true;
		while (it->getParent()!=NULL)
		{
			it=it->getParent();
			for (int j=0;j<int(modelBases.size());j++)
			{
				if (modelBases[j]==it)
				{
					isIndependentBase=false;
					break;
				}
			}
		}
		if (isIndependentBase)
			addObjectToSelection(modelBases[i]->getID());
	}
	if (keepObjectsSelectedThatAreNotBuiltOnAModelBase)
	{
		for (int i=0;i<int(nonModelBasesBuildOnNothing.size());i++)
			addObjectToSelection(nonModelBasesBuildOnNothing[i]->getID());
	}
}


void CObjCont::getMinAndMaxNameSuffixes(int& minSuffix,int& maxSuffix)
{
	minSuffix=-1;
	maxSuffix=-1;
	for (int i=0;i<int(objectList.size());i++)
	{
		int s=tt::getNameSuffixNumber(_objectIndex[objectList[i]]->getName().c_str(),true);
		if (i==0)
		{
			minSuffix=s;
			maxSuffix=s;
		}
		else
		{
			if (s<minSuffix)
				minSuffix=s;
			if (s>maxSuffix)
				maxSuffix=s;
		}
	}
}

bool CObjCont::canSuffix1BeSetToSuffix2(int suffix1,int suffix2)
{
	for (int i=0;i<int(objectList.size());i++)
	{
		int s1=tt::getNameSuffixNumber(_objectIndex[objectList[i]]->getName().c_str(),true);
		if (s1==suffix1)
		{
			std::string name1(tt::getNameWithoutSuffixNumber(_objectIndex[objectList[i]]->getName().c_str(),true));
			for (int j=0;j<int(objectList.size());j++)
			{
				int s2=tt::getNameSuffixNumber(_objectIndex[objectList[j]]->getName().c_str(),true);
				if (s2==suffix2)
				{
					std::string name2(tt::getNameWithoutSuffixNumber(_objectIndex[objectList[j]]->getName().c_str(),true));
					if (name1==name2)
						return(false); // NO! We would have a name clash!
				}
			}
		}
	}
	return(true);
}

void CObjCont::setSuffix1ToSuffix2(int suffix1,int suffix2)
{
	for (int i=0;i<int(objectList.size());i++)
	{
		int s1=tt::getNameSuffixNumber(_objectIndex[objectList[i]]->getName().c_str(),true);
		if (s1==suffix1)
		{
			std::string name1(tt::getNameWithoutSuffixNumber(_objectIndex[objectList[i]]->getName().c_str(),true));
			_objectIndex[objectList[i]]->setName(tt::generateNewName_dash(name1,suffix2+1));
		}
	}
}

bool CObjCont::addObjectToScene(C3DObject* newObject,bool objectIsACopy)
{
	return(addObjectToSceneWithSuffixOffset(newObject,objectIsACopy,1));
}

bool CObjCont::addObjectToSceneWithSuffixOffset(C3DObject* newObject,bool objectIsACopy,int suffixOffset)
{
	App::ct->environment->setSceneCanBeDiscardedWhenNewSceneOpened(false); // 4/3/2012

	// Handle cases where two object names are the same, 
	// and replace all spaces and illegal chars:
	std::string newObjName=newObject->getName();
	if (objectIsACopy)
		newObjName=tt::generateNewName_dash(newObjName,suffixOffset);
	else
	{
		while (getObject(newObjName)!=NULL)
			newObjName=tt::generateNewName_noDash(newObjName);
	}
	newObject->setName(newObjName);

	// Give the object a new identifier
	int i=_nextObjectHandle;
	while (_objectIndex[i]!=NULL)
	{
		i++;
		if (i>=(SIM_IDEND_3DOBJECT-SIM_IDSTART_3DOBJECT))
			i=SIM_IDSTART_3DOBJECT;
	}
	// i now points to a free spot. Actualize _nextObjectHandle now:
	_nextObjectHandle=i+1;
	if (_nextObjectHandle>=(SIM_IDEND_3DOBJECT-SIM_IDSTART_3DOBJECT))
		_nextObjectHandle=SIM_IDSTART_3DOBJECT;
	// set the new handle to the object:
	newObject->setID(i);
	objectList.push_back(i); // Added at the end of the list --> rendered first
	// Set the object in the index list:
	_objectIndex[i]=newObject;

	if (newObject->getObjectType()==sim_object_graph_type)
	{ // If the simulation is running, we have to empty the buffer!!! (otherwise we might have old and new data mixed together (e.g. old data in future, new data in present!)
		if ((App::ct->simulation!=NULL)&&(!App::ct->simulation->isSimulationStopped()))
		{
			CGraph* graph=(CGraph*)newObject;
			graph->resetGraph();
		}
	}

	// Actualize the object information
	actualizeObjectInformation();

	App::ct->setModificationFlag(2); // object created
	return(true);
}

void CObjCont::_getAllObjectsSmallestAndBiggestSuffix(int& smallestSuffix,int& biggestSuffix)
{
	smallestSuffix=SIM_MAX_INT;
	biggestSuffix=-1;
	int minS,maxS;
	App::ct->constraintSolver->getMinAndMaxNameSuffixes(minS,maxS);
	if (minS<smallestSuffix)
		smallestSuffix=minS;
	if (maxS>biggestSuffix)
		biggestSuffix=maxS;
	App::ct->buttonBlockContainer->getMinAndMaxNameSuffixes(minS,maxS);
	if (minS<smallestSuffix)
		smallestSuffix=minS;
	if (maxS>biggestSuffix)
		biggestSuffix=maxS;
	App::ct->objCont->getMinAndMaxNameSuffixes(minS,maxS);
	if (minS<smallestSuffix)
		smallestSuffix=minS;
	if (maxS>biggestSuffix)
		biggestSuffix=maxS;
	App::ct->collisions->getMinAndMaxNameSuffixes(minS,maxS);
	if (minS<smallestSuffix)
		smallestSuffix=minS;
	if (maxS>biggestSuffix)
		biggestSuffix=maxS;
	App::ct->distances->getMinAndMaxNameSuffixes(minS,maxS);
	if (minS<smallestSuffix)
		smallestSuffix=minS;
	if (maxS>biggestSuffix)
		biggestSuffix=maxS;
	App::ct->collections->getMinAndMaxNameSuffixes(minS,maxS);
	if (minS<smallestSuffix)
		smallestSuffix=minS;
	if (maxS>biggestSuffix)
		biggestSuffix=maxS;
	App::ct->ikGroups->getMinAndMaxNameSuffixes(minS,maxS);
	if (minS<smallestSuffix)
		smallestSuffix=minS;
	if (maxS>biggestSuffix)
		biggestSuffix=maxS;
	App::ct->pathPlanning->getMinAndMaxNameSuffixes(minS,maxS);
	if (minS<smallestSuffix)
		smallestSuffix=minS;
	if (maxS>biggestSuffix)
		biggestSuffix=maxS;
	App::ct->motionPlanning->getMinAndMaxNameSuffixes(minS,maxS);
	if (minS<smallestSuffix)
		smallestSuffix=minS;
	if (maxS>biggestSuffix)
		biggestSuffix=maxS;
}

bool CObjCont::_canSuffix1BeSetToSuffix2(int suffix1,int suffix2)
{
	if (!App::ct->objCont->canSuffix1BeSetToSuffix2(suffix1,suffix2))
		return(false);
	if (!App::ct->constraintSolver->canSuffix1BeSetToSuffix2(suffix1,suffix2))
		return(false);
	if (!App::ct->buttonBlockContainer->canSuffix1BeSetToSuffix2(suffix1,suffix2))
		return(false);
	if (!App::ct->collisions->canSuffix1BeSetToSuffix2(suffix1,suffix2))
		return(false);
	if (!App::ct->distances->canSuffix1BeSetToSuffix2(suffix1,suffix2))
		return(false);
	if (!App::ct->collections->canSuffix1BeSetToSuffix2(suffix1,suffix2))
		return(false);
	if (!App::ct->ikGroups->canSuffix1BeSetToSuffix2(suffix1,suffix2))
		return(false);
	if (!App::ct->pathPlanning->canSuffix1BeSetToSuffix2(suffix1,suffix2))
		return(false);
	if (!App::ct->motionPlanning->canSuffix1BeSetToSuffix2(suffix1,suffix2))
		return(false);
	return(true);
}

void CObjCont::_setSuffix1ToSuffix2(int suffix1,int suffix2)
{
	App::ct->objCont->setSuffix1ToSuffix2(suffix1,suffix2);
	App::ct->constraintSolver->setSuffix1ToSuffix2(suffix1,suffix2);
	App::ct->buttonBlockContainer->setSuffix1ToSuffix2(suffix1,suffix2);
	App::ct->collisions->setSuffix1ToSuffix2(suffix1,suffix2);
	App::ct->distances->setSuffix1ToSuffix2(suffix1,suffix2);
	App::ct->collections->setSuffix1ToSuffix2(suffix1,suffix2);
	App::ct->ikGroups->setSuffix1ToSuffix2(suffix1,suffix2);
	App::ct->pathPlanning->setSuffix1ToSuffix2(suffix1,suffix2);
	App::ct->motionPlanning->setSuffix1ToSuffix2(suffix1,suffix2);
}

void CObjCont::cleanupDashNames(int suffix)
{ // This function will try to use the lowest dash naming possible (e.g. model#59 --> model and model#67 --> model#0 if possible)
	// if suffix is -1, then all suffixes are cleaned, otherwise only those equal or above 'suffix'

	// 1. we get all object's smallest and biggest suffix: 
	int smallestSuffix,biggestSuffix;
	_getAllObjectsSmallestAndBiggestSuffix(smallestSuffix,biggestSuffix);
	if (suffix<=0)
		suffix=0;
	for (int i=suffix;i<=biggestSuffix;i++)
	{
		for (int j=-1;j<i;j++)
		{
			if (_canSuffix1BeSetToSuffix2(i,j))
			{
				_setSuffix1ToSuffix2(i,j);
				break;
			}
		}
	}
}

void CObjCont::prepareFastLoadingMapping(std::vector<int>& map)
{
	std::vector<int> mapC(map);
	map.clear();
	int minVal=0;
	int maxVal=0;
	for (int i=0;i<int(mapC.size())/2;i++)
	{
		int v=mapC[2*i+0];
		if ( (v<minVal)||(i==0) )
			minVal=v;
		if ( (v>maxVal)||(i==0) )
			maxVal=v;
	}
	map.push_back(minVal);
	if (mapC.size()!=0)
	{
		for (int i=0;i<maxVal-minVal+1;i++)
			map.push_back(-1);
		for (int i=0;i<int(mapC.size())/2;i++)
			map[1+mapC[2*i+0]-minVal]=mapC[2*i+1];
	}
}



int CObjCont::getLoadingMapping(std::vector<int>* map,int oldVal)
{
	// using a fast mapping (lots of RAM)
	if ( (oldVal<0)||((oldVal-map->at(0))>int(map->size())-2) )
		return(-1);
	return(map->at(oldVal+1-map->at(0)));

}

void CObjCont::newSceneProcedure()
{
	removeAllObjects();
}

void CObjCont::removeAllObjects()
{
	endEditMode(true);
	deselectObjects();

	while (objectList.size()!=0)
	{
		C3DObject* it=getObject(objectList[0]);
		if (it!=NULL) 
			eraseObject(it);
	}
	// The above loop destroys all 3DObjects, and normally automatically
	// all resources linked to them. So we don't have to destroy the resources

	deselectObjects();
	objectList.clear();

	_objectIndex.clear();
	_objectIndex.resize(SIM_IDEND_3DOBJECT-SIM_IDSTART_3DOBJECT,NULL);

	_selectedObjectsBool.clear();
	_selectedObjectsBool.resize((SIM_IDEND_3DOBJECT-SIM_IDSTART_3DOBJECT)/8,0);

	mirrorList.clear();
	lightList.clear();
	cameraList.clear();
	graphList.clear();
	proximitySensorList.clear();
	visionSensorList.clear();
	pathList.clear();
	volumeList.clear();
	millList.clear();
	forceSensorList.clear();
	shapeList.clear();
	selectedObjectsWhenSaving.clear();

	jointList.clear();
	dummyList.clear();
	orphanList.clear();
}

bool CObjCont::makeObjectChildOf(C3DObject* childObject,C3DObject* parentObject)
{	// This will trigger an actualization (important! orphanList needs also an update and other things too)
	// Careful: this routine should be able to be used for objects in the object
	// container, but also for objects in the copy buffer!!!! So don't make
	// use of any 'getObject(id)' or similar function!!!!! <-- this is a very old comment. Is it still true??
	if (childObject==NULL) 
		return(false);
	// Check if the child has already his desired parent (so that we don't have to call the actualization (heavy and will also refresh all dialogs) (added on 2009/12/15)
	if (childObject->getParent()==parentObject)
		return(true);
	incrementMainPropertyOverrideValidityNumber();
	if (parentObject==NULL)
	{
		C7Vector oldAbsoluteTransf(childObject->getCumulativeTransformationPart1());
		childObject->setParent(NULL);
		childObject->setLocalTransformation(oldAbsoluteTransf);
		if (childObject->getObjectType()==sim_object_shape_type)
			((CShape*)childObject)->setParentFollowsDynamic(false);
		actualizeObjectInformation();
		return(true);
	}
	// Illegal loop checking:
	if (parentObject->isObjectParentedWith(childObject)) 
		return(false);
	C7Vector oldAbsoluteTransf(childObject->getCumulativeTransformationPart1());
	C7Vector parentInverse(parentObject->getCumulativeTransformation().getInverse());
	childObject->setLocalTransformation(parentInverse*oldAbsoluteTransf);
	childObject->setParent(parentObject);

	if ( (childObject->getObjectType()==sim_object_shape_type)&&((CShape*)childObject)->getSetAutomaticallyToNonStaticIfGetsParent() )
	{
		((CShape*)childObject)->setShapeIsDynamicallyStatic(false);
	}
	
	if ( (childObject->getObjectType()==sim_object_shape_type)&&((CShape*)childObject)->getParentFollowsDynamic() )
	{ // Make sure we don't have more than one child that has the parentFollow tag :
		for (int i=0;i<int(parentObject->childList.size());i++)
		{
			if ( (parentObject->childList[i]->getObjectType()==sim_object_shape_type)&&(parentObject->childList[i]!=childObject) )
			{
				CShape* otherChild=(CShape*)parentObject->childList[i];
				if (otherChild->getParentFollowsDynamic())
				{
					((CShape*)childObject)->setParentFollowsDynamic(false);
					break;
				}
			}
		}
	}

	actualizeObjectInformation();
	return(true);
}

void CObjCont::setAbsoluteConfiguration(int identifier,const C7Vector& v,bool keepChildrenInPlace)
{
	C3DObject* it=getObject(identifier);
	if (it==NULL)
		return;
	C7Vector childPreTr(it->getLocalTransformation());
	C7Vector parentInverse(it->getParentCumulativeTransformation().getInverse());
	it->setLocalTransformation(parentInverse*v);
	if (keepChildrenInPlace)
	{
		childPreTr=it->getLocalTransformation().getInverse()*childPreTr;
		for (int i=0;i<int(it->childList.size());i++)
			it->childList[i]->setLocalTransformation(childPreTr*it->childList[i]->getLocalTransformationPart1());
	}
}


void CObjCont::enableObjectActualization(bool e)
{
	_objectActualizationEnabled=e;
}

void CObjCont::actualizeObjectInformation()
{
	std::vector<int> initialSel;
	getSelectedObjects(initialSel);
	
	deselectObjects();

	if (_objectActualizationEnabled) 
	{
		App::ct->hierarchy->setRebuildHierarchyFlag();

		// Actualize each object's child list
		// Following rewritten on 2009/03/15 to make it faster:
		for (int i=0;i<int(objectList.size());i++)
		{
			C3DObject* it=_objectIndex[objectList[i]];
			it->childList.clear();
		}
		for (int i=0;i<int(objectList.size());i++)
		{
			C3DObject* it=_objectIndex[objectList[i]];
			C3DObject* parent=it->getParent();
			if (parent!=NULL)
				parent->childList.push_back(it);
		}

		// Actualize the light, graph, sensor & joint list
		jointList.clear();
		dummyList.clear();
		orphanList.clear();
		mirrorList.clear();
		lightList.clear();
		cameraList.clear();
		graphList.clear();
		proximitySensorList.clear();
		visionSensorList.clear();
		pathList.clear();
		volumeList.clear();
		millList.clear();
		forceSensorList.clear();
		shapeList.clear();

		for (int i=0;i<int(objectList.size());i++)
		{
			if (_objectIndex[objectList[i]]->getObjectType()==sim_object_joint_type)
				jointList.push_back(objectList[i]);
			if (_objectIndex[objectList[i]]->getObjectType()==sim_object_dummy_type)
				dummyList.push_back(objectList[i]);
			if (_objectIndex[objectList[i]]->getParent()==NULL)
				orphanList.push_back(objectList[i]);
			if (_objectIndex[objectList[i]]->getObjectType()==sim_object_camera_type)
				cameraList.push_back(objectList[i]);
			if (_objectIndex[objectList[i]]->getObjectType()==sim_object_light_type)
				lightList.push_back(objectList[i]);
			if (_objectIndex[objectList[i]]->getObjectType()==sim_object_mirror_type)
				mirrorList.push_back(objectList[i]);
			if (_objectIndex[objectList[i]]->getObjectType()==sim_object_graph_type)
				graphList.push_back(objectList[i]);
			if (_objectIndex[objectList[i]]->getObjectType()==sim_object_proximitysensor_type)
				proximitySensorList.push_back(objectList[i]);
			if (_objectIndex[objectList[i]]->getObjectType()==sim_object_visionsensor_type)
				visionSensorList.push_back(objectList[i]);
			if (_objectIndex[objectList[i]]->getObjectType()==sim_object_path_type)
				pathList.push_back(objectList[i]);
			if (_objectIndex[objectList[i]]->getObjectType()==sim_object_shape_type)
				shapeList.push_back(objectList[i]);
			if (_objectIndex[objectList[i]]->getObjectType()==sim_object_volume_type)
				volumeList.push_back(objectList[i]);
			if (_objectIndex[objectList[i]]->getObjectType()==sim_object_mill_type)
				millList.push_back(objectList[i]);
			if (_objectIndex[objectList[i]]->getObjectType()==sim_object_forcesensor_type)
				forceSensorList.push_back(objectList[i]);
		}
		// We actualize the direct linked joint list of each joint: (2009-01-27)
		for (int i=0;i<int(jointList.size());i++)
		{
			CJoint* it=getJoint(jointList[i]);
			it->directDependentJoints.clear();
			for (int j=0;j<int(jointList.size());j++)
			{
				CJoint* anAct=getJoint(App::ct->objCont->jointList[j]);
				if (anAct!=it)
				{
					if (anAct->getJointMode()==sim_jointmode_dependent) 
					{
						if (anAct->getDependencyJointID()==it->getID())
							it->directDependentJoints.push_back(anAct);
					}
				}
			}
		}

		// We rebuild the group information
		App::ct->collections->actualizeAllGroups();

		// Keep this close to the end (added on 2010/02/09):
		for (int i=0;i<int(shapeList.size());i++)
			getShape(shapeList[i])->clearLastParentForLocalGlobalCollidable();
	}


	setSelectedObjects(initialSel);
	setFullDialogRefreshFlag();
}


int CObjCont::getObjectIdentifier(const std::string& objectName)
{
	for (int i=0;i<int(objectList.size());i++)
	{
		if (_objectIndex[objectList[i]]->getName().compare(objectName)==0)
			return(objectList[i]);
	}
	return(-1);
}

int CObjCont::getHighestObjectID()
{
	int highest=-1;
	for (int i=0;i<int(objectList.size());i++)
	{
		if (objectList[i]>highest)
			highest=objectList[i];
	}
	return(highest);
}

C3DObject* CObjCont::getObject(int identifier)
{
	if (identifier<0)
		return(NULL);
	if (identifier>=int(_objectIndex.size()))
		return(NULL);
	return(_objectIndex[identifier]);
}

CDummy* CObjCont::getDummy(int identifier)
{
	if (identifier<0)
		return(NULL);
	if (identifier>=int(_objectIndex.size()))
		return(NULL);
	if (_objectIndex[identifier]==NULL)
		return(NULL);
	if (_objectIndex[identifier]->getObjectType()!=sim_object_dummy_type)
		return(NULL);
	return((CDummy*)_objectIndex[identifier]);
}

CJoint* CObjCont::getJoint(int identifier)
{
	if (identifier<0)
		return(NULL);
	if (identifier>=int(_objectIndex.size()))
		return(NULL);
	if (_objectIndex[identifier]==NULL)
		return(NULL);
	if (_objectIndex[identifier]->getObjectType()!=sim_object_joint_type)
		return(NULL);
	return((CJoint*)_objectIndex[identifier]);
}

C3DObject* CObjCont::getObject(const std::string& name)
{
	return(getObject(getObjectIdentifier(name)));
}

bool CObjCont::eraseObject(C3DObject* it)
{
	incrementMainPropertyOverrideValidityNumber(); // added on 2010/08/07
	deselectObjects();

	if (it==NULL) 
		return(false);

	// We announce the object will be erased:
	announceObjectWillBeErased(it->getID()); // this may trigger other "eraseObject" calls (not really, since we don't have versatiles anymore!)
	// We remove the object from the object list
	int i;
	for (i=0;i<int(objectList.size());i++)
	{
		if (objectList[i]==it->getID()) 
			break;
	}
	objectList.erase(objectList.begin()+i);
	// Now remove the object from the index
	_objectIndex[it->getID()]=NULL;
//	int idSaved=it->getID();
	delete it;
	actualizeObjectInformation();

	App::ct->setModificationFlag(1); // object erased
	return(true);
}

//------------------ Object destruction announcement -------------------------
void CObjCont::announceObjectWillBeErased(int objectID)
{
	for (int i=0;i<int(objectList.size());i++)
	{
		C3DObject* it=getObject(objectList[i]);
		if (it->getID()!=objectID)
		{
			if (it->announceObjectWillBeErased(objectID,false))
			{ // We should never enter here since one obj destruction cannot trigger another obj destruction (anymore, no more versatiles!) 
				App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSNOTR_APPLICATION_ERROR),strTranslate(IDSNOTR_STRANGE_ERROR5),VMessageBox::OKELI);
				eraseObject(it);
				i=-1; // ordering might have changed!
			}
		}
	}

	// First objects that won't trigger any more destructions:
	App::ct->oglSurface->pageCont->announceObjectWillBeErased(objectID); // might trigger a view destruction!
	App::ct->constraintSolver->announceObjectWillBeErased(objectID);		// Triggers an announceGcsObjectWillBeErased for 3DObjects!
	App::ct->buttonBlockContainer->announceObjectWillBeErased(objectID);
	App::ct->luaScriptContainer->announceObjectWillBeErased(objectID);
	App::ct->pathPlanning->announceObjectWillBeErased(objectID);
	App::ct->motionPlanning->announceObjectWillBeErased(objectID);
	App::ct->collisions->announceObjectWillBeErased(objectID);
	App::ct->distances->announceObjectWillBeErased(objectID);
	App::ct->drawingCont->announceObjectWillBeErased(objectID);
	App::ct->pointCloudCont->announceObjectWillBeErased(objectID);
	App::ct->ghostObjectCont->announceObjectWillBeErased(objectID);
	App::ct->bannerCont->announceObjectWillBeErased(objectID);
	App::ct->textureCont->announceGeneralObjectWillBeErased(objectID,-1);

	// Now objects that could trigger other destructions:
	App::ct->collections->announceObjectWillBeErased(objectID); // can trigger distance, collision and pathPlanning destructions

	App::ct->ikGroups->announceObjectWillBeErased(objectID); // can trigger pathPlanning object destruction
}

void CObjCont::announceIkGroupWillBeErased(int ikGroupID)
{
	for (int i=0;i<int(objectList.size());i++)
		getObject(objectList[i])->announceIkObjectWillBeErased(ikGroupID,false); // this never triggers 3DObject destruction!
	App::ct->ikGroups->announceIkGroupWillBeErased(ikGroupID); // This will never trigger an Ik group destruction
	App::ct->motionPlanning->announceIkGroupWillBeErased(ikGroupID); // This can trigger a motion planning object destruction!
}



