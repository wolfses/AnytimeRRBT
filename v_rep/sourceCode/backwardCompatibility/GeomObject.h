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

// This is needed only to be backward compatible with serialization version 15 (Serialization version 16 since Nov. 2012)

#pragma once

#include "ShapeComponent.h"
#include "geomProxy.h"

class C3DObject;

class CGeomObject_OLD   
{
public:
	CGeomObject_OLD();
	virtual ~CGeomObject_OLD();

	void _commonInit();
	CGeomProxy* buildEquivalentGeomProxy();
	void computeBoundingBox();
	void serialize(CSer& ar);

	CShapeComponent_OLD* _geomStruct;
	C7Vector _creationTransf;

	std::vector<float> visibleVertices;
	std::vector<int> visibleIndices;
	std::vector<float> visibleNormals;
	std::vector<unsigned char> visibleEdges;
	std::vector<float> visibleTextureCoords_notCopiedNorSerialized;

	void* collNode;
	std::vector<int> calcEdgeIDs;
	std::vector<int> calcFaceIDs;
	std::vector<int> calcPointIDs;	
	std::vector<float> calcVertices;
	std::vector<int> calcIndices;
	std::vector<int> calcSegments;
	std::vector<std::vector<int> > calcPolygons;

	C3Vector _boundingBoxHalfSizes;
private:
	void loadPackedIntegers(CSer& ar,std::vector<int>& data);
};
