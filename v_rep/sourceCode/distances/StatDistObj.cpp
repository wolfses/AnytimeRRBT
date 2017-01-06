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
#include "StatDistObj.h"
#include "collDistInterface.h"
#include "DistanceRoutine.h"

CStatDistObj::CStatDistObj(CShape* theShapeA,CShape* theShapeB)
{
	shapeA=theShapeA;
	shapeB=theShapeB;
	shapeACTM=shapeA->getCumulativeTransformation().getMatrix();
	shapeBCTM=shapeB->getCumulativeTransformation().getMatrix();
	shapeA->initializeCalculationStructureIfNeeded();
	shapeB->initializeCalculationStructureIfNeeded();
}

CStatDistObj::~CStatDistObj()
{

}

bool CStatDistObj::measure(float& dist,int theCaching[2])
{ // Return value true means distance (and dist also) where modified. It means that
	// the measured distance is smaller than the original 'distance' variable
	// caching can be NULL.
	// We we start by exploring the smallest shape:
	float distances[7];
	distances[6]=dist;

	// We first check if the transformation matrices are valid:
	if (!shapeACTM.isValid())
		return(false);
	if (!shapeBCTM.isValid())
		return(false);

	C4X4Matrix distObjMatr[2]={shapeACTM,shapeBCTM};
	const void* collInfos[2]={shapeA->geomData->collInfo,shapeB->geomData->collInfo};
	int shapeATri=CCollDistInterface::getCalculatedTriangleCount(shapeA->geomData->collInfo)*3;
	int shapeBTri=CCollDistInterface::getCalculatedTriangleCount(shapeB->geomData->collInfo)*3;
	if (shapeATri<shapeBTri)
		CCollDistInterface::getMeshMeshDistance(shapeA->geomData->collInfo,shapeB->geomData->collInfo,distObjMatr,collInfos,false,distances,theCaching);
	else
		CCollDistInterface::getMeshMeshDistance(shapeB->geomData->collInfo,shapeA->geomData->collInfo,distObjMatr,collInfos,true,distances,theCaching);


	if (distances[6]<dist)
	{
		dist=distances[6];
		ptOnShapeA.set(distances+0);
		ptOnShapeB.set(distances+3);
		return(true);
	}
	return(false);
}
