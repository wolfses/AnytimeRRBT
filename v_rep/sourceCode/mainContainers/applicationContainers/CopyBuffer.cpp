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
#include "CopyBuffer.h"
#include "Tt.h"

#include "IloIlo.h"
#include "v_repStrings.h"
#include "App.h"

CCopyBuffer::CCopyBuffer()
{
	clearBuffer();
	_copyIsForPasting=false;
}

CCopyBuffer::~CCopyBuffer()
{
	clearMemorizedBuffer();
	clearBuffer();
}

bool CCopyBuffer::isCopyForPasting()
{
	return(_copyIsForPasting);
}

void CCopyBuffer::clearBuffer()
{
	// Here we delete all objects contained in the buffers. We don't
	// have to (and we shouldn't!!) call 'announceObjectWillBeErased'
	// since all objects are erased at the same time!
	for (int i=0;i<int(objectBuffer.size());i++)
		delete objectBuffer[i];
	objectBuffer.clear();
	for (int i=0;i<int(groupBuffer.size());i++)
		delete groupBuffer[i];
	groupBuffer.clear();
	for (int i=0;i<int(collisionBuffer.size());i++)
		delete collisionBuffer[i];
	collisionBuffer.clear();
	for (int i=0;i<int(distanceBuffer.size());i++)
		delete distanceBuffer[i];
	distanceBuffer.clear();
	for (int i=0;i<int(ikGroupBuffer.size());i++)
		delete ikGroupBuffer[i];
	ikGroupBuffer.clear();
	for (int i=0;i<int(pathPlanningTaskBuffer.size());i++)
		delete pathPlanningTaskBuffer[i];
	pathPlanningTaskBuffer.clear();
	for (int i=0;i<int(motionPlanningTaskBuffer.size());i++)
		delete motionPlanningTaskBuffer[i];
	motionPlanningTaskBuffer.clear();
	for (int i=0;i<int(buttonBlockBuffer.size());i++)
		delete buttonBlockBuffer[i];
	buttonBlockBuffer.clear();
	for (int i=0;i<int(luaScriptBuffer.size());i++)
		delete luaScriptBuffer[i];
	luaScriptBuffer.clear();
	for (int i=0;i<int(constraintSolverBuffer.size());i++)
		delete constraintSolverBuffer[i];
	constraintSolverBuffer.clear();
	for (int i=0;i<int(textureObjectBuffer.size());i++)
		delete textureObjectBuffer[i];
	textureObjectBuffer.clear();
	for (int i=0;i<int(dynMaterialObjectBuffer.size());i++)
		delete dynMaterialObjectBuffer[i];
	dynMaterialObjectBuffer.clear();
}

void CCopyBuffer::clearMemorizedBuffer()
{
	for (int i=0;i<int(objectBuffer_memorized.size());i++)
		delete objectBuffer_memorized[i];
	objectBuffer_memorized.clear();
	for (int i=0;i<int(groupBuffer_memorized.size());i++)
		delete groupBuffer_memorized[i];
	groupBuffer_memorized.clear();
	for (int i=0;i<int(collisionBuffer_memorized.size());i++)
		delete collisionBuffer_memorized[i];
	collisionBuffer_memorized.clear();
	for (int i=0;i<int(distanceBuffer_memorized.size());i++)
		delete distanceBuffer_memorized[i];
	distanceBuffer_memorized.clear();
	for (int i=0;i<int(ikGroupBuffer_memorized.size());i++)
		delete ikGroupBuffer_memorized[i];
	ikGroupBuffer_memorized.clear();
	for (int i=0;i<int(pathPlanningTaskBuffer_memorized.size());i++)
		delete pathPlanningTaskBuffer_memorized[i];
	pathPlanningTaskBuffer_memorized.clear();
	for (int i=0;i<int(motionPlanningTaskBuffer_memorized.size());i++)
		delete motionPlanningTaskBuffer_memorized[i];
	motionPlanningTaskBuffer_memorized.clear();
	for (int i=0;i<int(buttonBlockBuffer_memorized.size());i++)
		delete buttonBlockBuffer_memorized[i];
	buttonBlockBuffer_memorized.clear();
	for (int i=0;i<int(luaScriptBuffer_memorized.size());i++)
		delete luaScriptBuffer_memorized[i];
	luaScriptBuffer_memorized.clear();
	for (int i=0;i<int(constraintSolverBuffer_memorized.size());i++)
		delete constraintSolverBuffer_memorized[i];
	constraintSolverBuffer_memorized.clear();
	for (int i=0;i<int(textureObjectBuffer_memorized.size());i++)
		delete textureObjectBuffer_memorized[i];
	textureObjectBuffer_memorized.clear();
	for (int i=0;i<int(dynMaterialObjectBuffer_memorized.size());i++)
		delete dynMaterialObjectBuffer_memorized[i];
	dynMaterialObjectBuffer_memorized.clear();
}

void CCopyBuffer::memorizeBuffer()
{
	// 1. We delete previously memorized objects:
	for (int i=0;i<int(objectBuffer_memorized.size());i++)
		delete objectBuffer_memorized[i];
	objectBuffer_memorized.clear();
	for (int i=0;i<int(groupBuffer_memorized.size());i++)
		delete groupBuffer_memorized[i];
	groupBuffer_memorized.clear();
	for (int i=0;i<int(collisionBuffer_memorized.size());i++)
		delete collisionBuffer_memorized[i];
	collisionBuffer_memorized.clear();
	for (int i=0;i<int(distanceBuffer_memorized.size());i++)
		delete distanceBuffer_memorized[i];
	distanceBuffer_memorized.clear();
	for (int i=0;i<int(ikGroupBuffer_memorized.size());i++)
		delete ikGroupBuffer_memorized[i];
	ikGroupBuffer_memorized.clear();
	for (int i=0;i<int(pathPlanningTaskBuffer_memorized.size());i++)
		delete pathPlanningTaskBuffer_memorized[i];
	pathPlanningTaskBuffer_memorized.clear();
	for (int i=0;i<int(motionPlanningTaskBuffer_memorized.size());i++)
		delete motionPlanningTaskBuffer_memorized[i];
	motionPlanningTaskBuffer_memorized.clear();
	for (int i=0;i<int(buttonBlockBuffer_memorized.size());i++)
		delete buttonBlockBuffer_memorized[i];
	buttonBlockBuffer_memorized.clear();
	for (int i=0;i<int(luaScriptBuffer_memorized.size());i++)
		delete luaScriptBuffer_memorized[i];
	luaScriptBuffer_memorized.clear();
	for (int i=0;i<int(constraintSolverBuffer_memorized.size());i++)
		delete constraintSolverBuffer_memorized[i];
	constraintSolverBuffer_memorized.clear();
	for (int i=0;i<int(textureObjectBuffer_memorized.size());i++)
		delete textureObjectBuffer_memorized[i];
	textureObjectBuffer_memorized.clear();
	for (int i=0;i<int(dynMaterialObjectBuffer_memorized.size());i++)
		delete dynMaterialObjectBuffer_memorized[i];
	dynMaterialObjectBuffer_memorized.clear();

	// 2. we copy all objects to the memorized buffer:
	for (int i=0;i<int(objectBuffer.size());i++)
		objectBuffer_memorized.push_back(objectBuffer[i]->copyYourself());
	for (int i=0;i<int(groupBuffer.size());i++)
		groupBuffer_memorized.push_back(groupBuffer[i]->copyYourself());
	for (int i=0;i<int(collisionBuffer.size());i++)
		collisionBuffer_memorized.push_back(collisionBuffer[i]->copyYourself());
	for (int i=0;i<int(distanceBuffer.size());i++)
		distanceBuffer_memorized.push_back(distanceBuffer[i]->copyYourself());
	for (int i=0;i<int(ikGroupBuffer.size());i++)
		ikGroupBuffer_memorized.push_back(ikGroupBuffer[i]->copyYourself());
	for (int i=0;i<int(pathPlanningTaskBuffer.size());i++)
		pathPlanningTaskBuffer_memorized.push_back(pathPlanningTaskBuffer[i]->copyYourself());
	for (int i=0;i<int(motionPlanningTaskBuffer.size());i++)
		motionPlanningTaskBuffer_memorized.push_back(motionPlanningTaskBuffer[i]->copyYourself());
	for (int i=0;i<int(buttonBlockBuffer.size());i++)
		buttonBlockBuffer_memorized.push_back(buttonBlockBuffer[i]->copyYourself());
	for (int i=0;i<int(luaScriptBuffer.size());i++)
		luaScriptBuffer_memorized.push_back(luaScriptBuffer[i]->copyYourself());
	for (int i=0;i<int(constraintSolverBuffer.size());i++)
		constraintSolverBuffer_memorized.push_back(constraintSolverBuffer[i]->copyYourself());
	for (int i=0;i<int(textureObjectBuffer.size());i++)
		textureObjectBuffer_memorized.push_back(textureObjectBuffer[i]->copyYourself());
	for (int i=0;i<int(dynMaterialObjectBuffer.size());i++)
		dynMaterialObjectBuffer_memorized.push_back(dynMaterialObjectBuffer[i]->copyYourself());

	_bufferIsFromLockedScene_memorized=_bufferIsFromLockedScene;
}

void CCopyBuffer::restoreBuffer()
{
	// 1. We clear the buffer:
	clearBuffer();
	// 2. We copy all memorized objects to the buffer:
	for (int i=0;i<int(objectBuffer_memorized.size());i++)
		objectBuffer.push_back(objectBuffer_memorized[i]->copyYourself());
	for (int i=0;i<int(groupBuffer_memorized.size());i++)
		groupBuffer.push_back(groupBuffer_memorized[i]->copyYourself());
	for (int i=0;i<int(collisionBuffer_memorized.size());i++)
		collisionBuffer.push_back(collisionBuffer_memorized[i]->copyYourself());
	for (int i=0;i<int(distanceBuffer_memorized.size());i++)
		distanceBuffer.push_back(distanceBuffer_memorized[i]->copyYourself());
	for (int i=0;i<int(ikGroupBuffer_memorized.size());i++)
		ikGroupBuffer.push_back(ikGroupBuffer_memorized[i]->copyYourself());
	for (int i=0;i<int(pathPlanningTaskBuffer_memorized.size());i++)
		pathPlanningTaskBuffer.push_back(pathPlanningTaskBuffer_memorized[i]->copyYourself());
	for (int i=0;i<int(motionPlanningTaskBuffer_memorized.size());i++)
		motionPlanningTaskBuffer.push_back(motionPlanningTaskBuffer_memorized[i]->copyYourself());
	for (int i=0;i<int(buttonBlockBuffer_memorized.size());i++)
		buttonBlockBuffer.push_back(buttonBlockBuffer_memorized[i]->copyYourself());
	for (int i=0;i<int(luaScriptBuffer_memorized.size());i++)
		luaScriptBuffer.push_back(luaScriptBuffer_memorized[i]->copyYourself());
	for (int i=0;i<int(constraintSolverBuffer_memorized.size());i++)
		constraintSolverBuffer.push_back(constraintSolverBuffer_memorized[i]->copyYourself());
	for (int i=0;i<int(textureObjectBuffer_memorized.size());i++)
		textureObjectBuffer.push_back(textureObjectBuffer_memorized[i]->copyYourself());
	for (int i=0;i<int(dynMaterialObjectBuffer_memorized.size());i++)
		dynMaterialObjectBuffer.push_back(dynMaterialObjectBuffer_memorized[i]->copyYourself());

	_bufferIsFromLockedScene=_bufferIsFromLockedScene_memorized;
}

int CCopyBuffer::pasteBuffer(bool intoLockedScene)
{ // return -1 means the operation cannot procceed because the scene is not locked (but buffer is), 0=empty buffer, 1=successful
	// This function is very similar to a model-loading operation:
	// Everything is inserted (3DObjects, groups, etc. ) and then
	// the mapping is performed
	FUNCTION_DEBUG;
	if (isBufferEmpty())
		return(0);
	if (_bufferIsFromLockedScene)
	{
		if (!intoLockedScene)
			return(-1);
	}
//-------------------------------- Buffer copy -----------------------------------------	
	_copyIsForPasting=true;
	// First we need to copy the copy-buffers:
	std::vector<C3DObject*> objectCopy;
	objectCopy.reserve(objectBuffer.size());
	objectCopy.clear();
	for (int i=0;i<int(objectBuffer.size());i++)
		objectCopy.push_back(objectBuffer[i]->copyYourself());

	std::vector<CRegGroup*> groupCopy;
	groupCopy.reserve(groupBuffer.size());
	groupCopy.clear();
	for (int i=0;i<int(groupBuffer.size());i++)
		groupCopy.push_back(groupBuffer[i]->copyYourself());

	std::vector<CRegColl*> collisionCopy;
	collisionCopy.reserve(collisionBuffer.size());
	collisionCopy.clear();
	for (int i=0;i<int(collisionBuffer.size());i++)
		collisionCopy.push_back(collisionBuffer[i]->copyYourself());

	std::vector<CRegDist*> distanceCopy;
	distanceCopy.reserve(distanceBuffer.size());
	distanceCopy.clear();
	for (int i=0;i<int(distanceBuffer.size());i++)
		distanceCopy.push_back(distanceBuffer[i]->copyYourself());

	std::vector<CikGroup*> ikGroupCopy;
	ikGroupCopy.reserve(ikGroupBuffer.size());
	ikGroupCopy.clear();
	for (int i=0;i<int(ikGroupBuffer.size());i++)
		ikGroupCopy.push_back(ikGroupBuffer[i]->copyYourself());

	std::vector<CPathPlanningTask*> pathPlanningTaskCopy;
	pathPlanningTaskCopy.reserve(pathPlanningTaskBuffer.size());
	pathPlanningTaskCopy.clear();
	for (int i=0;i<int(pathPlanningTaskBuffer.size());i++)
		pathPlanningTaskCopy.push_back(pathPlanningTaskBuffer[i]->copyYourself());

	std::vector<CMotionPlanningTask*> motionPlanningTaskCopy;
	motionPlanningTaskCopy.reserve(motionPlanningTaskBuffer.size());
	motionPlanningTaskCopy.clear();
	for (int i=0;i<int(motionPlanningTaskBuffer.size());i++)
		motionPlanningTaskCopy.push_back(motionPlanningTaskBuffer[i]->copyYourself());

	std::vector<CButtonBlock*> buttonBlockCopy;
	buttonBlockCopy.reserve(buttonBlockBuffer.size());
	buttonBlockCopy.clear();
	for (int i=0;i<int(buttonBlockBuffer.size());i++)
		buttonBlockCopy.push_back(buttonBlockBuffer[i]->copyYourself());

	std::vector<CLuaScriptObject*> luaScriptCopy;
	luaScriptCopy.reserve(luaScriptBuffer.size());
	luaScriptCopy.clear();
	for (int i=0;i<int(luaScriptBuffer.size());i++)
		luaScriptCopy.push_back(luaScriptBuffer[i]->copyYourself());

	std::vector<CConstraintSolverObject*> constraintSolverCopy;
	constraintSolverCopy.reserve(constraintSolverBuffer.size());
	constraintSolverCopy.clear();
	for (int i=0;i<int(constraintSolverBuffer.size());i++)
		constraintSolverCopy.push_back(constraintSolverBuffer[i]->copyYourself());

	std::vector<CTextureObject*> textureObjectCopy;
	textureObjectCopy.reserve(textureObjectBuffer.size());
	textureObjectCopy.clear();
	for (int i=0;i<int(textureObjectBuffer.size());i++)
		textureObjectCopy.push_back(textureObjectBuffer[i]->copyYourself());

	std::vector<CDynMaterialObject*> dynMaterialObjectCopy;
	dynMaterialObjectCopy.reserve(dynMaterialObjectBuffer.size());
	dynMaterialObjectCopy.clear();
	for (int i=0;i<int(dynMaterialObjectBuffer.size());i++)
		dynMaterialObjectCopy.push_back(dynMaterialObjectBuffer[i]->copyYourself());

	_copyIsForPasting=false;
	
//--------------------------------------------------------------------------------------

	// We first remove all parents for 3DObjects (the parenting info is stored
	// in _parentID)
	for (int i=0;i<int(objectCopy.size());i++)
		objectCopy[i]->setParent(NULL,false);

	// And we add everything to the scene:
	App::ct->objCont->addObjectsToSceneAndPerformMappings(&objectCopy,
													&groupCopy,
													&collisionCopy,
													&distanceCopy,
													&ikGroupCopy,
													&pathPlanningTaskCopy,
													&motionPlanningTaskCopy,
													&buttonBlockCopy,
													&luaScriptCopy,
													&constraintSolverCopy,
													textureObjectCopy,
													dynMaterialObjectCopy,
													true);
	return(1);
}
bool CCopyBuffer::isBufferEmpty()
{
	return(objectBuffer.size()==0);
}
void CCopyBuffer::copyCurrentSelection(std::vector<int>* sel,bool fromLockedScene)
{	
	// We copy the current selection in a way that the copied data (3DObjects,
	// Groups, collisions, etc.) is self-consistent: Should the entire scene be
	// cleared, then the buffer could be inserted into the scene without any
	// modification (except for different object handles).

	if (sel->size()==0)
		return;

	clearBuffer();
	_bufferIsFromLockedScene=fromLockedScene;
	
	std::vector<C3DObject*> selObj;
	selObj.reserve(sel->size());
	selObj.clear();
	for (int i=0;i<int(sel->size());i++)
		selObj.push_back(App::ct->objCont->getObject(sel->at(i)));
	objectBuffer.reserve(selObj.size());
	objectBuffer.clear();
	for (int i=0;i<int(selObj.size());i++)
	{
		C3DObject* it=selObj[i]->copyYourself();
		it->setParent(NULL,false); // Parenting is handled elsewhere
		objectBuffer.push_back(it);
	}
	// 3DObjects are copied. We need to prepare the parenting info:
	for (int i=0;i<int(selObj.size());i++)
	{
		C3DObject* original=selObj[i];
		C3DObject* originalParent=original->getParent();
		C3DObject* newParent=original->getFirstParentInSelection(&selObj);
		if (originalParent!=newParent)
		{ // We have to change position/orientation
			C7Vector cumul(original->getCumulativeTransformationPart1());
			C7Vector nParentInv;
			if (newParent==NULL)
				nParentInv.setIdentity();
			else
				nParentInv=newParent->getCumulativeTransformation().getInverse();
			objectBuffer[i]->setLocalTransformation(nParentInv*cumul);
		}
		// Now we prepare the index of the new parent (used later)
		if (newParent!=NULL)
		{
			bool found=false;
			for (int j=0;j<int(selObj.size());j++)
			{
				if (selObj[j]->getID()==newParent->getID())
				{
					objectBuffer[i]->setParentIdentifierLoading(objectBuffer[j]->getID());
					// The following is important for model-serialization!!!
					objectBuffer[i]->setParent(objectBuffer[j],false);
					found=true;
					break;
				}
			}
		}
		else
		{
			objectBuffer[i]->setParentIdentifierLoading(-1);
			objectBuffer[i]->setParent(NULL,false); // Important for model-serialization!!
		}
	}

//--------------------------- other object copy -------------------------------------------
// Here we have to copy (entirely) all collision, distances, groups and iks and path planning objects:
	for (int i=0;i<int(App::ct->collections->allGroups.size());i++)
		groupBuffer.push_back(App::ct->collections->allGroups[i]->copyYourself());
	for (int i=0;i<int(App::ct->collisions->collisionObjects.size());i++)
		collisionBuffer.push_back(App::ct->collisions->collisionObjects[i]->copyYourself());
	for (int i=0;i<int(App::ct->distances->distanceObjects.size());i++)
		distanceBuffer.push_back(App::ct->distances->distanceObjects[i]->copyYourself());
	for (int i=0;i<int(App::ct->ikGroups->ikGroups.size());i++)
		ikGroupBuffer.push_back(App::ct->ikGroups->ikGroups[i]->copyYourself());
	for (int i=0;i<int(App::ct->pathPlanning->allObjects.size());i++)
		pathPlanningTaskBuffer.push_back(App::ct->pathPlanning->allObjects[i]->copyYourself());
	for (int i=0;i<int(App::ct->motionPlanning->allObjects.size());i++)
		motionPlanningTaskBuffer.push_back(App::ct->motionPlanning->allObjects[i]->copyYourself());
	for (int i=0;i<int(App::ct->buttonBlockContainer->allBlocks.size());i++)
	{ // only copying non-system blocks and blocks that might be attached to 3DObjects:
		if ( ((App::ct->buttonBlockContainer->allBlocks[i]->getAttributes()&sim_ui_property_systemblock)==0)&&(App::ct->buttonBlockContainer->allBlocks[i]->getObjectIDAttachedTo()!=-1) )
			buttonBlockBuffer.push_back(App::ct->buttonBlockContainer->allBlocks[i]->copyYourself());
	}
	for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
	{ // Copy only child scripts that are active, or callback scripts!:
		if (App::ct->luaScriptContainer->allScripts[i]->getScriptType()==sim_scripttype_childscript)
		{ // don't copy the passive scripts:
			if (App::ct->luaScriptContainer->allScripts[i]->getObjectIDThatScriptIsAttachedTo()!=-1)
				luaScriptBuffer.push_back(App::ct->luaScriptContainer->allScripts[i]->copyYourself());
		}
		if (App::ct->luaScriptContainer->allScripts[i]->getScriptType()==sim_scripttype_jointctrlcallback)
			luaScriptBuffer.push_back(App::ct->luaScriptContainer->allScripts[i]->copyYourself());
	}
	for (int i=0;i<int(App::ct->constraintSolver->allGcsObjects.size());i++)
		constraintSolverBuffer.push_back(App::ct->constraintSolver->allGcsObjects[i]->copyYourself());
	for (int i=0;i<int(App::ct->textureCont->_allTextureObjects.size());i++)
		textureObjectBuffer.push_back(App::ct->textureCont->_allTextureObjects[i]->copyYourself());


	 // we only add a dyn material object if it is used:
	for (int i=0;i<int(App::ct->dynMaterialCont->_allDynMaterialObjects.size());i++)
		App::ct->dynMaterialCont->_allDynMaterialObjects[i]->setUsedTag(false);
	for (int i=0;i<int(selObj.size());i++)
	{
		C3DObject* it=selObj[i];
		if (it->getObjectType()==sim_object_shape_type)
		{
			CShape* shape=(CShape*)it;
			shape->geomData->geomInfo->getDynMaterialObject()->setUsedTag(true);
		}
	}
	for (int i=0;i<int(App::ct->dynMaterialCont->_allDynMaterialObjects.size());i++)
	{
		if (App::ct->dynMaterialCont->_allDynMaterialObjects[i]->getUsedTag())
			dynMaterialObjectBuffer.push_back(App::ct->dynMaterialCont->_allDynMaterialObjects[i]->copyYourself());
	}


// Here we prepare the rendering order and a vector containing unselected objects:
	std::vector<int> unselected;
	unselected.reserve(App::ct->objCont->objectList.size()-sel->size());
	unselected.clear();
	for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
	{
		if (!App::ct->objCont->isObjectInSelection(App::ct->objCont->objectList[i],sel))
			unselected.push_back(App::ct->objCont->objectList[i]);
	}
//-----------------------------------------------------------------------------------------	
	
	// Now we make sure the linked info is consistent: we announce to the selected objects
	// (in the copy buffer) that all unselected objects will be erased:
	// This will in turn also erase general objects (which might trigger other object erase)
	for (int i=0;i<int(unselected.size());i++)
		_announceObjectWillBeErased(unselected[i]);

	// We deselect all objects and refresh the dialogs:
	App::ct->objCont->deselectObjects();
	App::ct->objCont->setFullDialogRefreshFlag();
}

void CCopyBuffer::serializeCurrentSelection(CSer &ar,std::vector<int>* sel)
{	// This is used when saving a model. When saving a model, we basically perform
	// the same operations as for copying a selection. Since we will make use of the
	// copy buffers, we have to save them first (in case they are not empty) in
	// temporary buffers:
	std::vector<C3DObject*> objectTemp(objectBuffer);
	objectBuffer.clear();
	std::vector<CRegGroup*> groupTemp(groupBuffer);
	groupBuffer.clear();
	std::vector<CRegColl*> collisionTemp(collisionBuffer);
	collisionBuffer.clear();
	std::vector<CRegDist*> distanceTemp(distanceBuffer);
	distanceBuffer.clear();
	std::vector<CikGroup*> ikGroupTemp(ikGroupBuffer);
	ikGroupBuffer.clear();
	std::vector<CPathPlanningTask*> pathPlanningTaskTemp(pathPlanningTaskBuffer);
	pathPlanningTaskBuffer.clear();
	std::vector<CMotionPlanningTask*> motionPlanningTaskTemp(motionPlanningTaskBuffer);
	motionPlanningTaskBuffer.clear();
	std::vector<CButtonBlock*> buttonBlockTemp(buttonBlockBuffer);
	buttonBlockBuffer.clear();
	std::vector<CLuaScriptObject*> luaScriptTemp(luaScriptBuffer);
	luaScriptBuffer.clear();
	std::vector<CConstraintSolverObject*> constraintSolverTemp(constraintSolverBuffer);
	constraintSolverBuffer.clear();
	std::vector<CTextureObject*> textureObjectTemp(textureObjectBuffer);
	textureObjectBuffer.clear();
	std::vector<CDynMaterialObject*> dynMaterialObjectTemp(dynMaterialObjectBuffer);
	dynMaterialObjectBuffer.clear();

	copyCurrentSelection(sel,false); // here we indicate that the scene is not locked,it doesn't matter (should have been checked before anyway)

//--------------------------- Here we serialize the buffer content -------------------

	// **** Following needed to save existing calculation structures:
	App::ct->environment->setSaveExistingCalculationStructuresTemp(false);
	if (App::ct->environment->getSaveExistingCalculationStructures())
	{
		App::ct->environment->setSaveExistingCalculationStructures(false); // we clear that flag
		App::ct->environment->setSaveExistingCalculationStructuresTemp(true);
	}
	// ************************************************************



	//***************************************************
	ar.storeDataName(SER_MODEL_THUMBNAIL);
	ar.setCountingMode();
	App::ct->environment->modelThumbnail_notSerializedHere.serialize(ar,false);
	if (ar.setWritingMode())
		App::ct->environment->modelThumbnail_notSerializedHere.serialize(ar,false);

	// We store dynamic material property objects:
	for (int i=0;i<int(dynMaterialObjectBuffer.size());i++)
		App::ct->dynMaterialCont->storeDynMaterialObject(ar,dynMaterialObjectBuffer[i]);

	// We store texture objects:
	for (int i=0;i<int(textureObjectBuffer.size());i++)
		App::ct->textureCont->storeTextureObject(ar,textureObjectBuffer[i]);

	// Now we store all 3DObjects:
	for (int i=0;i<int(objectBuffer.size());i++)
		App::ct->objCont->store3DObject(ar,objectBuffer[i]);
	
	// Here we store the groups:
	for (int i=0;i<int(groupBuffer.size());i++)
	{
		ar.storeDataName(SER_GROUP);
		ar.setCountingMode();
		groupBuffer[i]->serialize(ar);
		if (ar.setWritingMode())
			groupBuffer[i]->serialize(ar);
	}
	// Here we store the collision objects:
	for (int i=0;i<int(collisionBuffer.size());i++)
	{
		ar.storeDataName(SER_COLLISION);
		ar.setCountingMode();
		collisionBuffer[i]->serialize(ar);
		if (ar.setWritingMode())
			collisionBuffer[i]->serialize(ar);
	}
	// Here we store the distance objects:
	for (int i=0;i<int(distanceBuffer.size());i++)
	{
		ar.storeDataName(SER_DISTANCE);
		ar.setCountingMode();
		distanceBuffer[i]->serialize(ar);
		if (ar.setWritingMode())
			distanceBuffer[i]->serialize(ar);
	}
	// Here we store the ik objects:
	for (int i=0;i<int(ikGroupBuffer.size());i++)
	{
		ar.storeDataName(SER_IK);
		ar.setCountingMode();
		ikGroupBuffer[i]->serialize(ar);
		if (ar.setWritingMode())
			ikGroupBuffer[i]->serialize(ar);
	}
	// Here we store the path planning objects:
	for (int i=0;i<int(pathPlanningTaskBuffer.size());i++)
	{
		ar.storeDataName(SER_PATH_PLANNING);
		ar.setCountingMode();
		pathPlanningTaskBuffer[i]->serialize(ar);
		if (ar.setWritingMode())
			pathPlanningTaskBuffer[i]->serialize(ar);
	}
	// Here we store the motion planning objects:
	for (int i=0;i<int(motionPlanningTaskBuffer.size());i++)
	{
		ar.storeDataName(SER_MOTION_PLANNING);
		ar.setCountingMode();
		motionPlanningTaskBuffer[i]->serialize(ar);
		if (ar.setWritingMode())
			motionPlanningTaskBuffer[i]->serialize(ar);
	}
	// Here we store the button block objects:
	for (int i=0;i<int(buttonBlockBuffer.size());i++)
	{
		ar.storeDataName(SER_BUTTON_BLOCK);
		ar.setCountingMode();
		buttonBlockBuffer[i]->serialize(ar);
		if (ar.setWritingMode())
			buttonBlockBuffer[i]->serialize(ar);
	}
	// Here we store the script objects:
	for (int i=0;i<int(luaScriptBuffer.size());i++)
	{
		ar.storeDataName(SER_LUA_SCRIPT);
		ar.setCountingMode();
		luaScriptBuffer[i]->serialize(ar);
		if (ar.setWritingMode())
			luaScriptBuffer[i]->serialize(ar);
	}
	// Here we store the constraintSolver objects:
	for (int i=0;i<int(constraintSolverBuffer.size());i++)
	{
		ar.storeDataName(SER_GEOMETRIC_CONSTRAINT_OBJECT);
		ar.setCountingMode();
		constraintSolverBuffer[i]->serialize(ar);
		if (ar.setWritingMode())
			constraintSolverBuffer[i]->serialize(ar);
	}

	// Here we have finished:
	ar.storeDataName(SER_END_OF_FILE);

	// We empty the used buffers (not needed anymore, since it was saved):
	clearBuffer();
	// Now we copy the original content of the buffers back:
	objectBuffer.reserve(objectTemp.size());
	for (int i=0;i<int(objectTemp.size());i++)
		objectBuffer.push_back(objectTemp[i]);
	groupBuffer.reserve(groupTemp.size());
	for (int i=0;i<int(groupTemp.size());i++)
		groupBuffer.push_back(groupTemp[i]);
	collisionBuffer.reserve(collisionTemp.size());
	for (int i=0;i<int(collisionTemp.size());i++)
		collisionBuffer.push_back(collisionTemp[i]);
	distanceBuffer.reserve(distanceTemp.size());
	for (int i=0;i<int(distanceTemp.size());i++)
		distanceBuffer.push_back(distanceTemp[i]);
	ikGroupBuffer.reserve(ikGroupTemp.size());
	for (int i=0;i<int(ikGroupTemp.size());i++)
		ikGroupBuffer.push_back(ikGroupTemp[i]);
	pathPlanningTaskBuffer.reserve(pathPlanningTaskTemp.size());
	for (int i=0;i<int(pathPlanningTaskTemp.size());i++)
		pathPlanningTaskBuffer.push_back(pathPlanningTaskTemp[i]);
	motionPlanningTaskBuffer.reserve(motionPlanningTaskTemp.size());
	for (int i=0;i<int(motionPlanningTaskTemp.size());i++)
		motionPlanningTaskBuffer.push_back(motionPlanningTaskTemp[i]);
	buttonBlockBuffer.reserve(buttonBlockTemp.size());
	for (int i=0;i<int(buttonBlockTemp.size());i++)
		buttonBlockBuffer.push_back(buttonBlockTemp[i]);
	luaScriptBuffer.reserve(luaScriptTemp.size());
	for (int i=0;i<int(luaScriptTemp.size());i++)
		luaScriptBuffer.push_back(luaScriptTemp[i]);
	constraintSolverBuffer.reserve(constraintSolverTemp.size());
	for (int i=0;i<int(constraintSolverTemp.size());i++)
		constraintSolverBuffer.push_back(constraintSolverTemp[i]);
	textureObjectBuffer.reserve(textureObjectTemp.size());
	for (int i=0;i<int(textureObjectTemp.size());i++)
		textureObjectBuffer.push_back(textureObjectTemp[i]);
	dynMaterialObjectBuffer.reserve(dynMaterialObjectTemp.size());
	for (int i=0;i<int(dynMaterialObjectTemp.size());i++)
		dynMaterialObjectBuffer.push_back(dynMaterialObjectTemp[i]);
}

void CCopyBuffer::_eraseObjectInBuffer(int objectID)
{
	_announceObjectWillBeErased(objectID);
	for (int i=0;i<int(objectBuffer.size());i++)
	{
		if (objectBuffer[i]->getID()==objectID)
		{
			delete objectBuffer[i];
			objectBuffer.erase(objectBuffer.begin()+i);
			break;
		}
	}
}

void CCopyBuffer::_eraseGcsObjectInBuffer(int objectID)
{
	_announceGcsObjectWillBeErased(objectID);
	for (int i=0;i<int(constraintSolverBuffer.size());i++)
	{
		if (constraintSolverBuffer[i]->getObjectID()==objectID)
		{
			delete constraintSolverBuffer[i];
			constraintSolverBuffer.erase(constraintSolverBuffer.begin()+i);
			break;
		}
	}
}

void CCopyBuffer::_erase2DElementInBuffer(int objectID)
{
	_announce2DElementWillBeErased(objectID);
	for (int i=0;i<int(buttonBlockBuffer.size());i++)
	{
		if (buttonBlockBuffer[i]->getBlockID()==objectID)
		{
			delete buttonBlockBuffer[i];
			buttonBlockBuffer.erase(buttonBlockBuffer.begin()+i);
			break;
		}
	}
}

void CCopyBuffer::_eraseLuaScriptInBuffer(int objectID)
{
	for (int i=0;i<int(luaScriptBuffer.size());i++)
	{
		if (luaScriptBuffer[i]->getScriptID()==objectID)
		{
			delete luaScriptBuffer[i];
			luaScriptBuffer.erase(luaScriptBuffer.begin()+i);
			break;
		}
	}
}

void CCopyBuffer::_erasePathPlanningTaskInBuffer(int objectID)
{
	for (int i=0;i<int(pathPlanningTaskBuffer.size());i++)
	{
		if (pathPlanningTaskBuffer[i]->getObjectID()==objectID)
		{
			delete pathPlanningTaskBuffer[i];
			pathPlanningTaskBuffer.erase(pathPlanningTaskBuffer.begin()+i);
			break;
		}
	}
}

void CCopyBuffer::_eraseMotionPlanningTaskInBuffer(int objectID)
{
	for (int i=0;i<int(motionPlanningTaskBuffer.size());i++)
	{
		if (motionPlanningTaskBuffer[i]->getObjectID()==objectID)
		{
			delete motionPlanningTaskBuffer[i];
			motionPlanningTaskBuffer.erase(motionPlanningTaskBuffer.begin()+i);
			break;
		}
	}
}

void CCopyBuffer::_eraseCollisionInBuffer(int objectID)
{
	_announceCollisionWillBeErased(objectID);
	for (int i=0;i<int(collisionBuffer.size());i++)
	{
		if (collisionBuffer[i]->getObjectID()==objectID)
		{
			delete collisionBuffer[i];
			collisionBuffer.erase(collisionBuffer.begin()+i);
			break;
		}
	}
}

void CCopyBuffer::_eraseDistanceInBuffer(int objectID)
{
	_announceDistanceWillBeErased(objectID);
	for (int i=0;i<int(distanceBuffer.size());i++)
	{
		if (distanceBuffer[i]->getObjectID()==objectID)
		{
			delete distanceBuffer[i];
			distanceBuffer.erase(distanceBuffer.begin()+i);
			break;
		}
	}
}

void CCopyBuffer::_eraseCollectionInBuffer(int objectID)
{
	_announceGroupWillBeErased(objectID);
	for (int i=0;i<int(groupBuffer.size());i++)
	{
		if (groupBuffer[i]->getGroupID()==objectID)
		{
			delete groupBuffer[i];
			groupBuffer.erase(groupBuffer.begin()+i);
			break;
		}
	}
}

void CCopyBuffer::_eraseIkObjectInBuffer(int objectID)
{
	_announceIkGroupWillBeErased(objectID);
	for (int i=0;i<int(ikGroupBuffer.size());i++)
	{
		if (ikGroupBuffer[i]->getObjectID()==objectID)
		{
			delete ikGroupBuffer[i];
			ikGroupBuffer.erase(ikGroupBuffer.begin()+i);
			break;
		}
	}
}

void CCopyBuffer::_eraseTextureObjectInBuffer(int objectID)
{
	for (int i=0;i<int(textureObjectBuffer.size());i++)
	{
		if (textureObjectBuffer[i]->getObjectID()==objectID)
		{
			delete textureObjectBuffer[i];
			textureObjectBuffer.erase(textureObjectBuffer.begin()+i);
			break;
		}
	}
}

void CCopyBuffer::_eraseDynMaterialObjectInBuffer(int objectID)
{
	for (int i=0;i<int(dynMaterialObjectBuffer.size());i++)
	{
		if (dynMaterialObjectBuffer[i]->getObjectID()==objectID)
		{
			delete dynMaterialObjectBuffer[i];
			dynMaterialObjectBuffer.erase(dynMaterialObjectBuffer.begin()+i);
			break;
		}
	}
}

//------------------ Object destruction announcement -------------------------
void CCopyBuffer::_announceObjectWillBeErased(int objectID)
{
	// First objects that won't trigger any more destructions:
	int i=0;
	while (i<int(objectBuffer.size()))
	{
		C3DObject* it=objectBuffer[i];
		if (it->announceObjectWillBeErased(objectID,true))
		{ // We should never enter here since one obj destruction cannot trigger another obj destruction (anymore, no more versatiles!) 
			App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSNOTR_APPLICATION_ERROR),strTranslate(IDSNOTR_STRANGE_ERROR7),VMessageBox::OKELI);
			_eraseObjectInBuffer(it->getID()); 
			i=0; // ordering may have changed!
		}
		else
			i++;
	}

	i=0;
	while (i<int(constraintSolverBuffer.size()))
	{
		CConstraintSolverObject* it=constraintSolverBuffer[i];
		if (it->announceObjectWillBeErased(objectID,true))
		{
			_eraseGcsObjectInBuffer(it->getObjectID());
			i=0; // ordering may have changed!
		}
		else
			i++;
	}

	i=0;
	while (i<int(buttonBlockBuffer.size()))
	{
		CButtonBlock* it=buttonBlockBuffer[i];
		if (it->announce3DObjectWillBeErased(objectID,true)||(it->getObjectIDAttachedTo()==-1))
		{
			_erase2DElementInBuffer(it->getBlockID());
			i=0; // Ordering may have changed!
		}
		else
			i++;
	}

	i=0;
	while (i<int(luaScriptBuffer.size()))
	{
		CLuaScriptObject* it=luaScriptBuffer[i];
		if (it->announce3DObjectWillBeErased(objectID,true))
		{
			_eraseLuaScriptInBuffer(it->getScriptID());
			i=0; // Ordering may have changed!
		}
		else
			i++;
	}

	i=0;
	while (i<int(pathPlanningTaskBuffer.size()))
	{
		CPathPlanningTask* it=pathPlanningTaskBuffer[i];
		if (it->announceObjectWillBeErased(objectID,true))
		{
			_erasePathPlanningTaskInBuffer(it->getObjectID());
			i=0; // Ordering may have changed!
		}
		else
			i++;
	}

	i=0;
	while (i<int(motionPlanningTaskBuffer.size()))
	{
		CMotionPlanningTask* it=motionPlanningTaskBuffer[i];
		if (it->announceObjectWillBeErased(objectID,true))
		{
			_eraseMotionPlanningTaskInBuffer(it->getObjectID());
			i=0; // Ordering may have changed!
		}
		else
			i++;
	}

	i=0;
	while (i<int(collisionBuffer.size()))
	{
		CRegColl* it=collisionBuffer[i];
		if (it->announceObjectWillBeErased(objectID,true))
		{
			_eraseCollisionInBuffer(it->getObjectID());
			i=0; // Ordering may have changed!
		}
		else
			i++;
	}

	i=0;
	while (i<int(distanceBuffer.size()))
	{
		CRegDist* it=distanceBuffer[i];
		if (it->announceObjectWillBeErased(objectID,true))
		{
			_eraseDistanceInBuffer(it->getObjectID());
			i=0; // Ordering may have changed!
		}
		else
			i++;
	}
	
	// Now objects that could trigger other destructions:

	i=0;
	while (i<int(groupBuffer.size()))
	{
		CRegGroup* it=groupBuffer[i];
		if (it->announceObjectWillBeErased(objectID,true))
		{
			_eraseCollectionInBuffer(it->getGroupID());
			i=0; // Ordering may have changed!
		}
		else
			i++;
	}

	i=0;
	while (i<int(ikGroupBuffer.size()))
	{
		CikGroup* it=ikGroupBuffer[i];
		if (it->announceObjectWillBeErased(objectID,true))
		{
			_eraseIkObjectInBuffer(it->getObjectID());
			i=0; // Ordering may have changed!
		}
		else
			i++;
	}

	i=0;
	while (i<int(textureObjectBuffer.size()))
	{
		CTextureObject* it=textureObjectBuffer[i];
		if (it->announceGeneralObjectWillBeErased(objectID,-1))
		{
			_eraseTextureObjectInBuffer(it->getObjectID());
			i=0; // Ordering may have changed!
		}
		else
			i++;
	}

	// The dynMaterialObjectBuffer doesn't need to be handled here (uses a different method and was already handled)
}

void CCopyBuffer::_announceGroupWillBeErased(int groupID)
{
	for (int i=0;i<int(objectBuffer.size());i++)
		objectBuffer[i]->announceGroupWillBeErased(groupID,true); // this never triggers 3DObject destruction!

	int i=0;
	while (i<int(collisionBuffer.size()))
	{
		CRegColl* it=collisionBuffer[i];
		if (it->announceGroupWillBeErased(groupID,true))
		{
			_eraseCollisionInBuffer(it->getObjectID());
			i=0; // Ordering may have changed
		}
		else
			i++;
	}

	i=0;
	while (i<int(distanceBuffer.size()))
	{
		CRegDist* it=distanceBuffer[i];
		if (it->announceGroupWillBeErased(groupID,true))
		{
			_eraseDistanceInBuffer(it->getObjectID());
			i=0; // Ordering may have changed
		}
		else
			i++;
	}

	i=0;
	while (i<int(ikGroupBuffer.size()))
	{
		CikGroup* it=ikGroupBuffer[i];
		if (it->announceGroupWillBeErased(groupID,true))
		{
			_eraseIkObjectInBuffer(it->getObjectID());
			i=0; // Ordering may have changed
		}
		else
			i++;
	}

	i=0;
	while (i<int(pathPlanningTaskBuffer.size()))
	{
		CPathPlanningTask* it=pathPlanningTaskBuffer[i];
		if (it->announceGroupWillBeErased(groupID,true))
		{
			_erasePathPlanningTaskInBuffer(it->getObjectID());
			i=0; // Ordering may have changed
		}
		else
			i++;
	}

	i=0;
	while (i<int(motionPlanningTaskBuffer.size()))
	{
		CMotionPlanningTask* it=motionPlanningTaskBuffer[i];
		if (it->announceGroupWillBeErased(groupID,true))
		{
			_eraseMotionPlanningTaskInBuffer(it->getObjectID());
			i=0; // Ordering may have changed
		}
		else
			i++;
	}
}


void CCopyBuffer::_announceIkGroupWillBeErased(int ikGroupID)
{
	for (int i=0;i<int(objectBuffer.size());i++)
		objectBuffer[i]->announceIkObjectWillBeErased(ikGroupID,true); // this never triggers 3DObject destruction!

	int i=0;
	while (i<int(ikGroupBuffer.size()))
	{
		CikGroup* it=ikGroupBuffer[i];
		if (it->announceIkGroupWillBeErased(ikGroupID,true))
		{
			_eraseIkObjectInBuffer(it->getObjectID());
			i=0; // Ordering may have changed
		}
		else
			i++;
	}

	i=0;
	while (i<int(motionPlanningTaskBuffer.size()))
	{
		CMotionPlanningTask* it=motionPlanningTaskBuffer[i];
		if (it->announceIkGroupWillBeErased(ikGroupID,true))
		{
			_eraseMotionPlanningTaskInBuffer(it->getObjectID());
			i=0; // Ordering may have changed
		}
		else
			i++;
	}
}

void CCopyBuffer::_announceGcsObjectWillBeErased(int gcsObjectID)
{
	for (int i=0;i<int(objectBuffer.size());i++)
		objectBuffer[i]->announceGcsObjectWillBeErased(gcsObjectID,true); // Never triggers 3DObject destruction!
}

void CCopyBuffer::_announceCollisionWillBeErased(int collisionID)
{
	for (int i=0;i<int(objectBuffer.size());i++)
		objectBuffer[i]->announceCollisionWillBeErased(collisionID,true); // this never triggers 3DObject destruction!
}

void CCopyBuffer::_announceDistanceWillBeErased(int distanceID)
{
	for (int i=0;i<int(objectBuffer.size());i++)
		objectBuffer[i]->announceDistanceWillBeErased(distanceID,true); // this never triggers 3DObject destruction!
}

void CCopyBuffer::_announce2DElementWillBeErased(int elementID)
{
	int i=0;
	while (i<int(textureObjectBuffer.size()))
	{
		CTextureObject* it=textureObjectBuffer[i];
		if (it->announceGeneralObjectWillBeErased(elementID,-1))
		{
			_eraseTextureObjectInBuffer(it->getObjectID());
			i=0; // Ordering may have changed!
		}
		else
			i++;
	}
}

