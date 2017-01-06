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

#include "Ser.h"
#include "3Vector.h"
#include <vector>

#define PROXSENSOR_MAX_FACE_NUMBER 1024	// Maximum faces of a disk or cone-type sensor
#define PROXSENSOR_MAX_SUBDIVISIONS 1024	// Maximum subdivisions of a cone-type sensor

enum { // IS SERIALIZED!!!!
	PYRAMID_TYPE_CONVEX_VOLUME=0,
	CYLINDER_TYPE_CONVEX_VOLUME,
	DISC_TYPE_CONVEX_VOLUME,
	CONE_TYPE_CONVEX_VOLUME,
	RAY_TYPE_CONVEX_VOLUME,
};

class CConvexVolume
{
public:

	CConvexVolume();
	virtual ~CConvexVolume();

	CConvexVolume* copyYourself();
	void scaleVolume(float scalingFactor);
	void serialize(CSer& ar);
	bool getVolumeBoundingBox(C3Vector& minV,C3Vector& maxV);
	void disableVolumeComputation(bool disableIt);

	void commonInit();
	C4X4Matrix getTheMatrix(const C3Vector& pt0,const C3Vector& pt1,const C3Vector& pt2,bool tri);
	void add3Values(std::vector<float>& vect,const C4X4Matrix& transf,float x,float y,float z);
	void addAPlane(std::vector<float>* volume,std::vector<float>* normals,float nL,const C4X4Matrix& m,bool inside);
	void computeVolumeEdges(std::vector<float>& edges);
	void getCloseAndFarVolumeEdges(std::vector<float>& allEdges,float distance,std::vector<float>& closeEdges,std::vector<float>& farEdges);
	void removeEdgesNotInsideVolume(std::vector<float>& edges,std::vector<float>& planes,bool invertSides);
	void generateSphereEdges(std::vector<float>& edges,float radius);

	void computeVolumes();

	void setVolumeType(int theType,int objectTypeTheVolumeIsFor,float pointSize);
	int getVolumeType();
	void setOffset(float theOffset);
	float getOffset();
	void setRange(float theRange);
	float getRange();
	void setXSize(float theXSize);
	float getXSize();
	void setYSize(float theYSize);
	float getYSize();
	void setXSizeFar(float theXSizeFar);
	float getXSizeFar();
	void setYSizeFar(float theYSizeFar);
	float getYSizeFar();
	void setRadius(float theRadius);
	float getRadius();
	void setRadiusFar(float theRadiusFar);
	float getRadiusFar();
	void setAngle(float theAngle);
	float getAngle();
	void setInsideAngleThing(float theAngleThing);
	float getInsideAngleThing();
	void setFaceNumber(int theFaceNumber);
	int getFaceNumber();
	void setFaceNumberFar(int theFaceNumberFar);
	int getFaceNumberFar();
	void setSubdivisions(int theSubdivisions);
	int getSubdivisions();
	void setSubdivisionsFar(int theSubdivisionsFar);
	int getSubdivisionsFar();
	void setSmallestDistanceEnabled(bool e);
	bool getSmallestDistanceEnabled();
	void setSmallestDistanceAllowed(float d);
	float getSmallestDistanceAllowed();

	void setDefaultVolumeParameters(int objectTypeTheVolumeIsFor,float pointSize);
	void solveInterferences();
	
	// Variables which need to be serialized & copied
	std::vector<float> planesInside;// Detection volume is formed by the inside volume minus the outside volume
	std::vector<float> planesOutside;
	std::vector<float> normalsInside;
	std::vector<float> normalsOutside;
	std::vector<float> volumeEdges;
	std::vector<float> nonDetectingVolumeEdges;
	std::vector<C3Vector> firstVolumeCorners;

protected:
	bool _volumeComputationTemporarilyDisabled;

	// Variables which need to be serialized & copied
	float _smallestDistanceAllowed;
	bool _smallestDistanceEnabled;
	int _volumeType;
	float offset;
	float range;
	float xSize;
	float ySize;
	float xSizeFar;
	float ySizeFar;
	float radius;
	float radiusFar;
	float angle;
	float insideAngleThing;
	int faceNumber;
	int faceNumberFar;
	int subdivisions;
	int subdivisionsFar;
};
