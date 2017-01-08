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

#include "3Vector.h"
#include "7Vector.h"
#include "4X4Matrix.h"
#include <vector>
#ifndef COMPILING_EXTERNAL_COLL_DIST_DLL
#include "VMutex.h"
#include "vrepConfig.h"
#else
#include "meshCalcConfig.h"
#endif

#ifndef COMPILING_EXTERNAL_COLL_DIST_DLL
typedef void* (*ptr_createCollisionInformationStructure)(const float* cumulMeshVertices,int cumulMeshVerticesSize,const int* cumulMeshIndices,int cumulMeshIndicesSize,float maxTriSize,float edgeAngle,int maxTriCount);
typedef void* (*ptr_copyCollisionInformationStructure)(const void* collInfo);
typedef void (*ptr_destroyCollisionInformationStructure)(void* collInfo);
typedef void (*ptr_scaleCollisionInformationStructure)(void* collInfo,float scaleFactor);
typedef unsigned char* (*ptr_getCollisionInformationStructureSerializationData)(const void* collInfo,int* dataSize);
typedef void* (*ptr_getCollisionInformationStructureFromSerializationData)(const unsigned char* data,const float* cumulMeshVertices,int cumulMeshVerticesSize,const int* cumulMeshIndices,int cumulMeshIndicesSize);
typedef void (*ptr_releaseBuffer)(void* buffer);
typedef char (*ptr_getCutMesh)(const void* collInfo,const float* tr,float** vertices,int* verticesSize,int** indices,int* indicesSize,int options);
typedef int (*ptr_getCalculatedTriangleCount)(const void* collInfo);
typedef int* (*ptr_getCalculatedTrianglesPointer)(const void* collInfo);
typedef int (*ptr_getCalculatedVerticeCount)(const void* collInfo);
typedef float* (*ptr_getCalculatedVerticesPointer)(const void* collInfo);
typedef int (*ptr_getCalculatedSegmentCount)(const void* collInfo);
typedef int* (*ptr_getCalculatedSegmentsPointer)(const void* collInfo);
typedef int (*ptr_getCalculatedPolygonCount)(const void* collInfo);
typedef int (*ptr_getCalculatedPolygonSize)(const void* collInfo,int polygonIndex);
typedef int* (*ptr_getCalculatedPolygonArrayPointer)(const void* collInfo,int polygonIndex);
typedef char (*ptr_getCalculatedTriangleAt)(const void* collInfo,float* a0,float* a1,float* a2,int ind);
typedef char (*ptr_getMeshMeshCollision)(const void* collInfo1,const void* collInfo2,const float* collObjMatr1,const float* collObjMatr2,const void* collInfos[2],char inverseExploration,float** intersections,int* intersectionsSize,int caching[2]);
typedef int (*ptr_getTriangleTriangleCollision)(const float* a0,const float* e0,const float* e1,const float* b0,const float* f0,const float* f1,float* intSegPart0,float* intSegPart1,char getIntersection);
typedef void (*ptr_getMeshMeshDistance)(const void* collInfo1,const void* collInfo2,const float* distObjMatr1,const float* distObjMatr2,const void* collInfos[2],char inverseExploration,float distances[7],int caching[2]);
typedef char (*ptr_getDistanceAgainstDummy_ifSmaller)(const void* collInfo,const float* dummyPos,const float* itPCTM,float* dist,float* ray0,float* ray1,int* itBuff);
typedef float (*ptr_getBoxPointDistance)(const float* t1,const float* s1,const float* dummyPos);
typedef float (*ptr_getApproximateBoxBoxDistance)(const float* t1,const float* s1,const float* t2,const float* s2);
typedef char (*ptr_getTriangleTriangleDistance_ifSmaller)(const float* a0,const float* e0,const float* e1,const float* b0,const float* f0,const float* f1,float* dist,float* segA,float* segB);
typedef char (*ptr_getTrianglePointDistance_ifSmaller)(const float* a0,const float* e0,const float* e1,const float* dummyPos,float* dist,float* segA);
typedef char (*ptr_getRayProxSensorDistance_ifSmaller)(const void* collInfo,const float* selfPCTM,float* dist,const float* lp,float closeThreshold,const float* lvFar,float cosAngle,float* detectPoint,bool fast,bool frontFace,bool backFace,char* closeDetectionTriggered,float* triNormalNotNormalized,void* theOcclusionCheckCallback);
typedef char (*ptr_isPointInsideVolume1AndOutsideVolume2)(const float* p,const float* planes,int planesSize,const float* planesOutside,int planesOutsideSize);
typedef char (*ptr_isPointTouchingVolume)(const float* p,const float* planes,int planesSize);
typedef char (*ptr_getProxSensorDistance_ifSmaller)(const void* collInfo,const float* itPCTM,float* dist,const float* planes,int planesSize,const float* planesOutside,int planesOutsideSize,float cosAngle,float* detectPoint,bool fast,bool frontFace,bool backFace,float* triNormalNotNormalized,void* theOcclusionCheckCallback);
typedef char (*ptr_getProxSensorDistanceToSegment_ifSmaller)(const float* p0,const float* p1,float* dist,const float* planes,int planesSize,const float* planesOutside,int planesOutsideSize,float maxAngle,float* detectPoint);
typedef char (*ptr_getProxSensorDistanceToTriangle_ifSmaller)(const float* a0,const float* e0,const float* e1,float* dist,const float* planes,int planesSize,const float* planesOutside,int planesOutsideSize,float cosAngle,float* detectPoint,bool frontFace,bool backFace,float* triNormalNotNormalized);
typedef float (*ptr_cutNodeWithVolume)(void* collInfo,const float* itPCTM,const float* planes,int planesSize);
#endif

// FULLY STATIC CLASS
class CCollDistInterface  
{
public:
#ifndef COMPILING_EXTERNAL_COLL_DIST_DLL
	
	static bool initializeFunctionsIfNeeded();
	static void setSystemPluginsLoadPhaseOver();
	static void lockUnlock(bool lock);

	static void* createCollisionInformationStructure(const float* cumulMeshVertices,int cumulMeshVerticesSize,const int* cumulMeshIndices,int cumulMeshIndicesSize,float maxTriSize,float edgeAngle,int maxTriCount);
	static void* copyCollisionInformationStructure(const void* collInfo);
	static void destroyCollisionInformationStructure(void* collInfo);
	static void scaleCollisionInformationStructure(void* collInfo,float scaleFactor);
	static unsigned char* getCollisionInformationStructureSerializationData(const void* collInfo,int& dataSize);
	static void* getCollisionInformationStructureFromSerializationData(const unsigned char* data,const float* cumulMeshVertices,int cumulMeshVerticesSize,const int* cumulMeshIndices,int cumulMeshIndicesSize);
	static void releaseBuffer(void* buffer);
	static bool getCutMesh(const void* collInfo,const C7Vector* tr,float** vertices,int* verticesSize,int** indices,int* indicesSize,int options);

	static int getCalculatedTriangleCount(const void* collInfo);
	static int* getCalculatedTrianglesPointer(const void* collInfo);
	static int getCalculatedVerticeCount(const void* collInfo);
	static float* getCalculatedVerticesPointer(const void* collInfo);
	static int getCalculatedSegmentCount(const void* collInfo);
	static int* getCalculatedSegmentsPointer(const void* collInfo);
	static int getCalculatedPolygonCount(const void* collInfo);
	static int getCalculatedPolygonSize(const void* collInfo,int polygonIndex);
	static int* getCalculatedPolygonArrayPointer(const void* collInfo,int polygonIndex);
	static bool getCalculatedTriangleAt(const void* collInfo,C3Vector& a0,C3Vector& a1,C3Vector& a2,int ind);

	static bool getMeshMeshCollision(const void* collInfo1,const void* collInfo2,const C4X4Matrix collObjMatr[2],const void* collInfos[2],bool inverseExploration,std::vector<float>* intersections,int caching[2]);
	static int getTriangleTriangleCollision(const C3Vector& a0,const C3Vector& e0,const C3Vector& e1,const C3Vector& b0,const C3Vector& f0,const C3Vector& f1,C3Vector* intSegPart0,C3Vector* intSegPart1,bool getIntersection);

	static void getMeshMeshDistance(const void* collInfo1,const void* collInfo2,const C4X4Matrix distObjMatr[2],const void* collInfos[2],bool inverseExploration,float distances[7],int caching[2]);
	static bool getDistanceAgainstDummy_ifSmaller(const void* collInfo,const C3Vector& dummyPos,const C4X4Matrix& itPCTM,float &dist,C3Vector& ray0,C3Vector& ray1,int& itBuff);
	static float getBoxPointDistance(const C4X4Matrix& t1,const C3Vector& s1,const C3Vector& dummyPos);
	static float getApproximateBoxBoxDistance(const C4X4Matrix& t1,const C3Vector& s1,const C4X4Matrix& t2,const C3Vector& s2);
	static bool getTriangleTriangleDistance_ifSmaller(const C3Vector& a0,const C3Vector& e0,const C3Vector& e1,const C3Vector& b0,const C3Vector& f0,const C3Vector& f1,float &dist,C3Vector& segA,C3Vector& segB);
	static bool getTrianglePointDistance_ifSmaller(const C3Vector& a0,const C3Vector& e0,const C3Vector& e1,const C3Vector& dummyPos,float &dist,C3Vector& segA);

	static bool getRayProxSensorDistance_ifSmaller(const void* collInfo,const C4X4Matrix& selfPCTM,float &dist,const C3Vector& lp,float closeThreshold,const C3Vector& lvFar,float cosAngle,C3Vector& detectPoint,bool fast,bool frontFace,bool backFace,char* closeDetectionTriggered,C3Vector& triNormalNotNormalized,void* theOcclusionCheckCallback);
	static bool isPointInsideVolume1AndOutsideVolume2(const C3Vector& p,const std::vector<float>* planes,const std::vector<float>* planesOutside);
	static bool isPointTouchingVolume(const C3Vector& p,const std::vector<float>* planes);
	static bool getProxSensorDistance_ifSmaller(const void* collInfo,const C4X4Matrix& itPCTM,float &dist,const std::vector<float>* planes,const std::vector<float>* planesOutside,float cosAngle,C3Vector& detectPoint,bool fast,bool frontFace,bool backFace,std::vector<float>* cutEdges,C3Vector& triNormalNotNormalized,void* theOcclusionCheckCallback);
	static bool getProxSensorDistanceToSegment_ifSmaller(const C3Vector& p0,const C3Vector& p1,float &dist,const std::vector<float>* planes,const std::vector<float>* planesOutside,float maxAngle,C3Vector& detectPoint);
	static bool getProxSensorDistanceToTriangle_ifSmaller(const C3Vector& a0,const C3Vector& e0,const C3Vector& e1,float &dist,const std::vector<float>* planes,const std::vector<float>* planesOutside,float cosAngle,C3Vector& detectPoint,bool frontFace,bool backFace,C3Vector& triNormalNotNormalized);

	static float cutNodeWithVolume(void* collInfo,const C4X4Matrix& itPCTM,const std::vector<float>* planes);

#else

	static void* _createCollisionInformationStructure(const float* cumulMeshVertices,int cumulMeshVerticesSize,const int* cumulMeshIndices,int cumulMeshIndicesSize,float maxTriSize,float edgeAngle,int maxTriCount);
	static void* _copyCollisionInformationStructure(const void* collInfo);
	static void _destroyCollisionInformationStructure(void* collInfo);
	static void _scaleCollisionInformationStructure(void* collInfo,float scaleFactor);
	static unsigned char* _getCollisionInformationStructureSerializationData(const void* collInfo,int* dataSize);
	static void* _getCollisionInformationStructureFromSerializationData(const unsigned char* data,const float* cumulMeshVertices,int cumulMeshVerticesSize,const int* cumulMeshIndices,int cumulMeshIndicesSize);
	static void _releaseBuffer(void* buffer);
	static char _getCutMesh(const void* collInfo,const float* tr,float** vertices,int* verticesSize,int** indices,int* indicesSize,int options);

	static int _getCalculatedTriangleCount(const void* collInfo);
	static int* _getCalculatedTrianglesPointer(const void* collInfo);
	static int _getCalculatedVerticeCount(const void* collInfo);
	static float* _getCalculatedVerticesPointer(const void* collInfo);
	static int _getCalculatedSegmentCount(const void* collInfo);
	static int* _getCalculatedSegmentsPointer(const void* collInfo);
	static int _getCalculatedPolygonCount(const void* collInfo);
	static int _getCalculatedPolygonSize(const void* collInfo,int polygonIndex);
	static int* _getCalculatedPolygonArrayPointer(const void* collInfo,int polygonIndex);
	static char _getCalculatedTriangleAt(const void* collInfo,float* a0,float* a1,float* a2,int ind);

	static char _getMeshMeshCollision(const void* collInfo1,const void* collInfo2,const float* collObjMatr1,const float* collObjMatr2,const void* collInfos[2],char inverseExploration,float** intersections,int* intersectionsSize,int caching[2]);
	static int _getTriangleTriangleCollision(const float* a0,const float* e0,const float* e1,const float* b0,const float* f0,const float* f1,float* intSegPart0,float* intSegPart1,char getIntersection);

	static void _getMeshMeshDistance(const void* collInfo1,const void* collInfo2,const float* distObjMatr1,const float* distObjMatr2,const void* collInfos[2],char inverseExploration,float distances[7],int caching[2]);
	static char _getDistanceAgainstDummy_ifSmaller(const void* collInfo,const float* dummyPos,const float* itPCTM,float* dist,float* ray0,float* ray1,int* itBuff);
	static float _getBoxPointDistance(const float* t1,const float* s1,const float* dummyPos);
	static float _getApproximateBoxBoxDistance(const float* t1,const float* s1,const float* t2,const float* s2);
	static char _getTriangleTriangleDistance_ifSmaller(const float* a0,const float* e0,const float* e1,const float* b0,const float* f0,const float* f1,float* dist,float* segA,float* segB);
	static char _getTrianglePointDistance_ifSmaller(const float* a0,const float* e0,const float* e1,const float* dummyPos,float* dist,float* segA);

	static char _getRayProxSensorDistance_ifSmaller(const void* collInfo,const float* selfPCTM,float* dist,const float* lp,float closeThreshold,const float* lvFar,float cosAngle,float* detectPoint,bool fast,bool frontFace,bool backFace,char* closeDetectionTriggered,float* triNormalNotNormalized,void* theOcclusionCheckCallback);
	static char _isPointInsideVolume1AndOutsideVolume2(const float* p,const float* planes,int planesSize,const float* planesOutside,int planesOutsideSize);
	static char _isPointTouchingVolume(const float* p,const float* planes,int planesSize);
	static char _getProxSensorDistance_ifSmaller(const void* collInfo,const float* itPCTM,float* dist,const float* planes,int planesSize,const float* planesOutside,int planesOutsideSize,float cosAngle,float* detectPoint,bool fast,bool frontFace,bool backFace,float* triNormalNotNormalized,void* theOcclusionCheckCallback);
	static char _getProxSensorDistanceToSegment_ifSmaller(const float* p0,const float* p1,float* dist,const float* planes,int planesSize,const float* planesOutside,int planesOutsideSize,float maxAngle,float* detectPoint);
	static char _getProxSensorDistanceToTriangle_ifSmaller(const float* a0,const float* e0,const float* e1,float* dist,const float* planes,int planesSize,const float* planesOutside,int planesOutsideSize,float cosAngle,float* detectPoint,bool frontFace,bool backFace,float* triNormalNotNormalized);

	static float _cutNodeWithVolume(void* collInfo,const float* itPCTM,const float* planes,int planesSize);
#endif // COMPILING_EXTERNAL_COLL_DIST_DLL

#ifndef COMPILING_EXTERNAL_COLL_DIST_DLL
	static ptr_createCollisionInformationStructure _createCollisionInformationStructure;
	static ptr_copyCollisionInformationStructure _copyCollisionInformationStructure;
	static ptr_destroyCollisionInformationStructure _destroyCollisionInformationStructure;
	static ptr_scaleCollisionInformationStructure _scaleCollisionInformationStructure;
	static ptr_getCollisionInformationStructureSerializationData _getCollisionInformationStructureSerializationData;
	static ptr_getCollisionInformationStructureFromSerializationData _getCollisionInformationStructureFromSerializationData;
	static ptr_releaseBuffer _releaseBuffer;
	static ptr_getCutMesh _getCutMesh;
	static ptr_getCalculatedTriangleCount _getCalculatedTriangleCount;
	static ptr_getCalculatedTrianglesPointer _getCalculatedTrianglesPointer;
	static ptr_getCalculatedVerticeCount _getCalculatedVerticeCount;
	static ptr_getCalculatedVerticesPointer _getCalculatedVerticesPointer;
	static ptr_getCalculatedSegmentCount _getCalculatedSegmentCount;
	static ptr_getCalculatedSegmentsPointer _getCalculatedSegmentsPointer;
	static ptr_getCalculatedPolygonCount _getCalculatedPolygonCount;
	static ptr_getCalculatedPolygonSize _getCalculatedPolygonSize;
	static ptr_getCalculatedPolygonArrayPointer _getCalculatedPolygonArrayPointer;
	static ptr_getCalculatedTriangleAt _getCalculatedTriangleAt;
	static ptr_getMeshMeshCollision _getMeshMeshCollision;
	static ptr_getTriangleTriangleCollision _getTriangleTriangleCollision;
	static ptr_getMeshMeshDistance _getMeshMeshDistance;
	static ptr_getDistanceAgainstDummy_ifSmaller _getDistanceAgainstDummy_ifSmaller;
	static ptr_getBoxPointDistance _getBoxPointDistance;
	static ptr_getApproximateBoxBoxDistance _getApproximateBoxBoxDistance;
	static ptr_getTriangleTriangleDistance_ifSmaller _getTriangleTriangleDistance_ifSmaller;
	static ptr_getTrianglePointDistance_ifSmaller _getTrianglePointDistance_ifSmaller;
	static ptr_getRayProxSensorDistance_ifSmaller _getRayProxSensorDistance_ifSmaller;
	static ptr_isPointInsideVolume1AndOutsideVolume2 _isPointInsideVolume1AndOutsideVolume2;
	static ptr_isPointTouchingVolume _isPointTouchingVolume;
	static ptr_getProxSensorDistance_ifSmaller _getProxSensorDistance_ifSmaller;
	static ptr_getProxSensorDistanceToSegment_ifSmaller _getProxSensorDistanceToSegment_ifSmaller;
	static ptr_getProxSensorDistanceToTriangle_ifSmaller _getProxSensorDistanceToTriangle_ifSmaller;
	static ptr_cutNodeWithVolume _cutNodeWithVolume;

	static bool _collDistInterfaceInitialized;
	static bool _collDistSystemPluginsLoadPhaseOver;

	static VMutex _objectMutex;
#endif

};
