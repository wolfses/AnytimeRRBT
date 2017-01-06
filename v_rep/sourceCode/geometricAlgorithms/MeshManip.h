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

#include <vector>
#include "EdgeElement.h"
#include "MyMath.h"

class CMeshManip  
{
public:
	CMeshManip(float* vertices,int verticesNb,int* indices,int indicesNb);
	virtual ~CMeshManip();

	std::vector<CEdgeElement*> edges;
	std::vector<C3Vector> faceNormals;

	// Various static functions (some of them are redundant or very old!!):
	static bool extractOneShape(std::vector<float>* vertices,std::vector<int>* indices,std::vector<float>* sVertices,std::vector<int>* sIndices);
	static void useOnlyReferencedVertices(std::vector<float>* vertices,std::vector<float>* sVertices,std::vector<int>* sIndices);
	static void removeNonReferencedVertices(std::vector<float>& vertices,std::vector<int>& indices);
	static bool checkVerticesIndicesNormalsTexCoords(std::vector<float>& vertices,std::vector<int>& indices,std::vector<float>* normals,std::vector<float>* texCoords,bool checkDoubles,float tolerance,bool checkSameTriangles);

	static int removeColinearTriangles(std::vector<float>& vertices,std::vector<int>& indices,std::vector<float>* normals,std::vector<float>* texCoords,float tolerance);
	static void removeDoubleVertices(std::vector<float>& vertices,std::vector<int>& mapping,float tolerance);
	static void removeDoubleIndices(std::vector<float>& vertices,std::vector<int>& indices,bool checkSameWinding);
	static bool reduceTriangleSize(std::vector<float>& vertices,std::vector<int>& indices,std::vector<float>* normals,std::vector<float>* texCoords,float maxEdgeSize,float verticeMergeTolerance);
	static bool correctTriangleWinding(std::vector<float>* vertices,std::vector<int>* indices);
	static bool isInsideOut(std::vector<float>* vertices,std::vector<int>* indices);
	static void setInsideOut(std::vector<int>* indices);
	static void getProjectionOfPointOnLine(float x,float y,float z,float vx,float vy,float vz,float &px,float &py,float &pz);
	static bool mergeWith(std::vector<float>* tVertices,std::vector<int>* tIndices,std::vector<float>* tNormals,
					std::vector<float>* sVertices,std::vector<int>* sIndices,std::vector<float>* sNormals);	static void reduceToUnit(float vector[3]);
	static void calcNormal(float v[3][3],float out[3]);
	static void getSize(std::vector<float>* vertices,float x[2],float y[2],float z[2],bool &start);
	static void centerAndScale(std::vector<float>* vertices,float x,float y,float z,float sf);
	static void getNormals(const std::vector <float>* vertices,const std::vector<int>* indices,std::vector<float>* normals);
	static void getTrianglesFromPolygons(const std::vector<std::vector<int> >& polygons,std::vector<int>& indices);
	static void transposeMatrix_4x4Array(float m[4][4]);

private:
	static void _setExtractionExploration(std::vector<BYTE>* exploration,int index,BYTE bit,int &allBits,int &twoBits);
	static int _reduceTriangleSizePass(std::vector<float>& vertices,std::vector<int>& indices,std::vector<float>* normals,std::vector<float>* texCoords,float maxEdgeSize);
	static int _getNeighbour(int actualTriangle,std::vector<int>* indices,int actualEdge[2],std::vector<std::vector<int>*>* edges,std::vector<BYTE>* exploredState);
};
