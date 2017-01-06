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

// FULLY STATIC CLASS
class CDistanceRoutine  
{
public:
	CDistanceRoutine();
	virtual ~CDistanceRoutine();

	// The one and only general routine:
	static bool getDistanceBetweenEntitiesIfSmaller(int entity1ID,int entity2ID,
		float& dist,float ray[7],int buffer[4],bool overrideMeasurableFlagIfNonCollection1,bool overrideMeasurableFlagIfNonCollection2,bool pathPlanningRoutineCalling,bool handledByWorkThread);
	// This one is used by the IK routine for obstacle avoidance:
	static bool getObjectObstacleDistanceIfSmaller(C3DObject* object,C3DObject* obstacle,
		float& dist,float ray[7],bool overrideMeasurableFlagObject1,bool overrideMeasurableFlagObject2);

private:
	static bool _getDummyDummyDistanceIfSmaller(int dummy1ID,int dummy2ID,
						float& dist,float ray[7],bool overrideMeasurableFlagDummy1,bool overrideMeasurableFlagDummy2,bool pathPlanningRoutineCalling);
	static bool _getShapeShapeDistanceIfSmaller(int shape1ID,int shape2ID,
						float& dist,float ray[7],int buffer[2],bool overrideMeasurableFlagShape1,bool overrideMeasurableFlagShape2,bool pathPlanningRoutineCalling);
	static bool _getGroupShapeDistanceIfSmaller(int groupID,int shapeID,
						float& dist,float ray[7],int buffer[3],bool overrideMeasurableFlagShape,bool pathPlanningRoutineCalling);
	static bool _getGroupGroupDistanceIfSmaller(int group1ID,int group2ID,
						float& dist,float ray[7],int buffer[4],bool pathPlanningRoutineCalling);
	static bool _getShapeDummyDistanceIfSmaller(int shapeID,int dummyID,
						float& dist,float ray[7],int& buffer,bool overrideMeasurableFlagShape,bool overrideMeasurableFlagDummy,bool pathPlanningRoutineCalling);
	static bool _getGroupDummyDistanceIfSmaller(int groupID,int dummyID,
						float& dist,float ray[7],int buffer[2],bool overrideMeasurableFlagDummy,bool pathPlanningRoutineCalling);

	// The following are just to functions calling their opposite then inverting the results
	static bool _getShapeGroupDistanceIfSmaller(int shapeID,int groupID,
						float& dist,float ray[7],int buffer[3],bool overrideMeasurableFlagShape,bool pathPlanningRoutineCalling);
	static bool _getDummyShapeDistanceIfSmaller(int dummyID,int shapeID,
						float& dist,float ray[7],int& buffer,bool overrideMeasurableFlagDummy,bool overrideMeasurableFlagShape,bool pathPlanningRoutineCalling);
	static bool _getDummyGroupDistanceIfSmaller(int dummyID,int groupID,
						float& dist,float ray[7],int buffer[2],bool overrideMeasurableFlagDummy,bool pathPlanningRoutineCalling);

// Used for caching distances:
	static bool _getBufferedDistance_IfSmaller(CShape* shape,CDummy* dummy,
						int buffer,float& dist,float ray[7]);
	static bool _getBufferedDistance_IfSmaller(CShape* shape1,CShape* shape2,
						int buffer[2],float& dist,float ray[7]);
// Others
	static void _invertRay(float ray[7]);
};
