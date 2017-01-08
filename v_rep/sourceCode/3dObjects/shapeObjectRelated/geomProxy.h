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

#include "geomWrap.h"
#include "3Vector.h"
#include "7Vector.h"
#include <vector>

class CGeomProxy   
{
public:
	CGeomProxy();
	virtual ~CGeomProxy();
	CGeomProxy(const std::vector<float>& allHeights,int xSize,int ySize,float dx,float zSize); // For heightfield creation
	CGeomProxy(const C7Vector* transformation,const std::vector<float>& vert,const std::vector<int>& ind,const std::vector<float>* normals,const std::vector<float>* textCoord);
	CGeomProxy(const C7Vector& transformation,CGeomWrap* newGeomInfo);

	void perform3DObjectLoadingMapping(std::vector<int>* map);
	void performTextureObjectLoadingMapping(std::vector<int>* map);
	void announce3DObjectWillBeErased(int objectID);
	void removeCollisionInformation();
	C3Vector getBoundingBoxHalfSizes();
	void invertFrontBack();
	void initializeCalculationStructureIfNeeded();
//	void initializeCollisionInformation();
	bool isCollisionInformationInitialized();
	void computeBoundingBox();
	void scale(float xVal,float yVal,float zVal);
	void setDynamicsFullRefreshFlag(bool refresh);
	bool getDynamicsFullRefreshFlag();
	int getGeomDataModificationCounter();
	void setTextureDependencies(int shapeID);
	C7Vector recomputeOrientation(C7Vector& m,bool alignWithMainAxis);
	C7Vector recomputeTubeOrCuboidOrientation(C7Vector& m,bool tube,bool& error);
	CGeomProxy* copyYourself();
	void serialize(CSer& ar);
	C7Vector getCreationTransformation();
	void setCreationTransformation(const C7Vector& tr);
	bool applyCuttingChanges(const C7Vector& shapeCTM);
	void acceptNewGeometry(const std::vector<float>& vert,const std::vector<int>& ind,const std::vector<float>* textCoord,const std::vector<float>* norm);

	CGeomWrap* geomInfo;

	void* collInfo;

protected:
	void _commonInit();
	static bool _getTubeReferenceFrame(const std::vector<float>& v,C7Vector& tr);
	static bool _getCuboidReferenceFrame(const std::vector<float>& v,const std::vector<int>& ind,C7Vector& tr);

	C7Vector _creationTransf;
	C3Vector _boundingBoxHalfSizes;

	bool _dynamicsFullRefreshFlag;
	int _geomDataModificationCounter;
};
