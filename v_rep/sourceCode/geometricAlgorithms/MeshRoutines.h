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
#include "3Vector.h"

//FULLY STATIC CLASS
class CMeshRoutines  
{
public:
	static int convexDecompose(const float* vertices,int verticesLength,const int* indices,int indicesLength,std::vector<std::vector<float>*>& verticesList,std::vector<std::vector<int>*>& indicesList,size_t nClusters,double concavity,bool addExtraDistPoints,bool addFacesPoints,double ccConnectDist,size_t targetNTrianglesDecimatedMesh,size_t maxHullVertices,double smallestClusterThreshold);

	static void getEdgeFeatures(float* vertices,int verticesLength,int* indices,int indicesLength,
			std::vector<int>* vertexIDs,std::vector<int>* edgeIDs,std::vector<int>* faceIDs,float angleTolerance,bool forDisplay);
	static bool getConvexHull(std::vector<float>* verticesIn,std::vector<int>* indicesIn,
							std::vector<float>* verticesOut,std::vector<int>* indicesOut);
	static bool getConvexHull(float* verticesIn,int verticesInLength,int* indicesIn,int indicesInLength,
							std::vector<float>* verticesOut,std::vector<int>* indicesOut);
	static bool getConvexHull(std::vector<float>* verticesInOut,std::vector<int>* indicesInOut);

	inline static bool getMinDistBetweenSegmentAndPoint_IfSmaller(const C3Vector& lp0,
							const C3Vector& lv0,const C3Vector& dummyPos,float &dist,C3Vector& segA)
	{	// dist & segA are modified only if the distance is smaller than 'dist' which was input
		// Return value of true means: the dist is smaller
		// The segment can be degenerated (point)
		float d;
		if ((lv0(0)==0.0f)&&(lv0(1)==0.0f)&&(lv0(2)==0.0f))
		{ // We have a degenerated segment here (point):
			C3Vector v(dummyPos-lp0);
			d=v.getLength();
			if (d<dist) 
			{
				dist=d;
				segA=lp0;
				return(true);
			}
			return(false);
		}
		// dist between lines described by segment and dummyPos
		float t=getMinDistPtBetweenPointAndLine(dummyPos,lp0,lv0);
		if ((t>=0.0f)&&(t<=1.0f))
		{
			C3Vector v(dummyPos-lp0-(lv0*t));
			d=v.getLength();
			if (d<dist)
			{
				dist=d;
				segA=lp0+lv0*t;
				return(true);
			}
			return(false);
		}
		// We have to compare point-point distances now
		C3Vector point(lp0-dummyPos);
		d=point.getLength();
		bool smaller=false;
		if (d<dist)
		{
			dist=d;
			segA=lp0;
			smaller=true;
		}
		point=lp0+lv0-dummyPos;
		d=point.getLength();
		if (d<dist)
		{
			dist=d;
			segA=lp0+lv0;
			smaller=true;
		}
		return(smaller);
	}

	inline static float getMinDistPtBetweenPointAndLine(const C3Vector& point,const C3Vector& lp,const C3Vector& lv)
	{
		C3Vector d(point-lp);
		return((d*lv)/(lv*lv));
	};

	static bool checkIfConvex(const std::vector<float>& vertices,const std::vector<int>& indices,float distanceToleranceInPercent);

	static void createCube(std::vector<float>& vertices,std::vector<int>& indices,const C3Vector& sizes,const int subdivisions[3]);
	static void createSphere(std::vector<float>& vertices,std::vector<int>& indices,const C3Vector& sizes,int sides,int faces);
	static void createCylinder(std::vector<float>& vertices,std::vector<int>& indices,const C3Vector& sizes,int sides,int faces,int discDiv,bool open,bool cone);
	static void createAnnulus(std::vector<float>& vertices,std::vector<int>& indices,float Dlarge,float Dsmall,float zShift,int sides,bool faceUp);

private:
	static void _insertEdge(std::vector<std::vector<int>* >& allEdges,int vertexIndex1,int vertexIndex2,int triangleIndex); // used for convex check
	static int _getTriangleIndexFromEdge(std::vector<std::vector<int>* >& allEdges,int vertexIndex1,int vertexIndex2,int triangleIndexToExclude); // used for convex check

};
