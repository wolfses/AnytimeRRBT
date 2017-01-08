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

#include "Shape.h"

//FULLY STATIC CLASS
class CCollisionRoutine  
{
public:
	CCollisionRoutine();
	virtual ~CCollisionRoutine();

	// The one and only general routine:
	static bool doEntitiesCollide(int entity1ID,int entity2ID,std::vector<float>* intersections,int buffer[4],bool overrideCollidableFlagIfShape1,bool overrideCollidableFlagIfShape2,bool pathOrMotionPlanningRoutineCalling,bool handledByWorkThread);
//	// This one is needed for path planning:
//	static void getNonCollidingPairs(int collectionID,std::vector<int>& pairs,bool pathPlanningRoutineCalling);

private:

//-----------------------------------------------------------------------------------------
	static bool _doesShapeCollideWithShape(int shape1ID,int shape2ID,
						std::vector<float>* intersections,int buffer[2],bool overrideShape1CollidableFlag,bool overrideShape2CollidableFlag,bool pathOrMotionPlanningRoutineCalling);
	static bool _doesGroupCollideWithShape(int groupID,int shapeID,
						std::vector<float>* intersections,int buffer[3],bool overrideShapeCollidableFlag,bool pathOrMotionPlanningRoutineCalling);
	static bool _doesGroupCollideWithGroup(int group1ID,int group2ID,
						std::vector<float>* intersections,int buffer[4],bool pathOrMotionPlanningRoutineCalling);
	// The following are just functions calling their opposite then inverting the buffers:
	static bool _doesShapeCollideWithGroup(int shapeID,int groupID,
						std::vector<float>* intersections,int buffer[3],bool overrideShapeCollidableFlag,bool pathOrMotionPlanningRoutineCalling);

// Used for caching distances:
//-----------------------------------------------------------------------------------------
	static bool _getBufferedCollision(CShape* shape1,CShape* shape2,int buffer[2]);
//-----------------------------------------------------------------------------------------

	// Others:
	static int _getInversedCollisionType(int collType);
};
