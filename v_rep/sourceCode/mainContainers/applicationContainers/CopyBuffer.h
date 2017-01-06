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

#include "3DObject.h"
#include "RegGroup.h"
#include "RegColl.h"
#include "RegDist.h"
#include "ikGroup.h"
#include "ConstraintSolverObject.h"
#include <vector>
#include "PathPlanningTask.h"
#include "MotionPlanningTask.h"
#include "ButtonBlock.h"
#include "LuaScriptObject.h"
#include "ConstraintSolverObject.h"
#include "DynMaterialObject.h"

class CCopyBuffer  
{
public:
	CCopyBuffer();
	virtual ~CCopyBuffer();

	void clearBuffer();
	int pasteBuffer(bool intoLockedScene);
	bool isBufferEmpty();
	void copyCurrentSelection(std::vector<int>* sel,bool fromLockedScene);
	void serializeCurrentSelection(CSer& ar,std::vector<int>* sel);
	bool isCopyForPasting();

	void memorizeBuffer();
	void restoreBuffer();
	void clearMemorizedBuffer();

private:

	void _announceObjectWillBeErased(int objectID);
	void _announceGroupWillBeErased(int groupID);
	void _announceIkGroupWillBeErased(int ikGroupID);
	void _announceGcsObjectWillBeErased(int gcsObjectID);
	void _announceCollisionWillBeErased(int collisionID);
	void _announceDistanceWillBeErased(int distanceID);
	void _announce2DElementWillBeErased(int elementID);

	void _eraseObjectInBuffer(int objectID);
	void _eraseGcsObjectInBuffer(int objectID);
	void _erase2DElementInBuffer(int objectID);
	void _eraseLuaScriptInBuffer(int objectID);
	void _erasePathPlanningTaskInBuffer(int objectID);
	void _eraseMotionPlanningTaskInBuffer(int objectID);
	void _eraseCollisionInBuffer(int objectID);
	void _eraseDistanceInBuffer(int objectID);
	void _eraseCollectionInBuffer(int objectID);
	void _eraseIkObjectInBuffer(int objectID);
	void _eraseTextureObjectInBuffer(int objectID); 
	void _eraseDynMaterialObjectInBuffer(int objectID); 

	bool _copyIsForPasting;


	bool _bufferIsFromLockedScene;
	std::vector<C3DObject*> objectBuffer;
	std::vector<CRegGroup*> groupBuffer;
	std::vector<CRegColl*> collisionBuffer;
	std::vector<CRegDist*> distanceBuffer;
	std::vector<CikGroup*> ikGroupBuffer;
	std::vector<CPathPlanningTask*> pathPlanningTaskBuffer;
	std::vector<CMotionPlanningTask*> motionPlanningTaskBuffer;
	std::vector<CButtonBlock*> buttonBlockBuffer;
	std::vector<CLuaScriptObject*> luaScriptBuffer;
	std::vector<CConstraintSolverObject*> constraintSolverBuffer;
	std::vector<CTextureObject*> textureObjectBuffer;
	std::vector<CDynMaterialObject*> dynMaterialObjectBuffer;



	bool _bufferIsFromLockedScene_memorized;
	std::vector<C3DObject*> objectBuffer_memorized;
	std::vector<CRegGroup*> groupBuffer_memorized;
	std::vector<CRegColl*> collisionBuffer_memorized;
	std::vector<CRegDist*> distanceBuffer_memorized;
	std::vector<CikGroup*> ikGroupBuffer_memorized;
	std::vector<CPathPlanningTask*> pathPlanningTaskBuffer_memorized;
	std::vector<CMotionPlanningTask*> motionPlanningTaskBuffer_memorized;
	std::vector<CButtonBlock*> buttonBlockBuffer_memorized;
	std::vector<CLuaScriptObject*> luaScriptBuffer_memorized;
	std::vector<CConstraintSolverObject*> constraintSolverBuffer_memorized;
	std::vector<CTextureObject*> textureObjectBuffer_memorized;
	std::vector<CDynMaterialObject*> dynMaterialObjectBuffer_memorized;
};
