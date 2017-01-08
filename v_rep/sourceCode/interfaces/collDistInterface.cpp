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

#include "collDistInterface.h"

#ifdef COMPILING_EXTERNAL_COLL_DIST_DLL
#include "collDistAlgos.h"
#else
#include "pluginContainer.h"
#include "v_repConst.h"
#endif

#ifndef COMPILING_EXTERNAL_COLL_DIST_DLL

ptr_createCollisionInformationStructure CCollDistInterface::_createCollisionInformationStructure;
ptr_copyCollisionInformationStructure CCollDistInterface::_copyCollisionInformationStructure;
ptr_destroyCollisionInformationStructure CCollDistInterface::_destroyCollisionInformationStructure;
ptr_scaleCollisionInformationStructure CCollDistInterface::_scaleCollisionInformationStructure;
ptr_getCollisionInformationStructureSerializationData CCollDistInterface::_getCollisionInformationStructureSerializationData;
ptr_getCollisionInformationStructureFromSerializationData CCollDistInterface::_getCollisionInformationStructureFromSerializationData;
ptr_releaseBuffer CCollDistInterface::_releaseBuffer;
ptr_getCutMesh CCollDistInterface::_getCutMesh;
ptr_getCalculatedTriangleCount CCollDistInterface::_getCalculatedTriangleCount;
ptr_getCalculatedTrianglesPointer CCollDistInterface::_getCalculatedTrianglesPointer;
ptr_getCalculatedVerticeCount CCollDistInterface::_getCalculatedVerticeCount;
ptr_getCalculatedVerticesPointer CCollDistInterface::_getCalculatedVerticesPointer;
ptr_getCalculatedSegmentCount CCollDistInterface::_getCalculatedSegmentCount;
ptr_getCalculatedSegmentsPointer CCollDistInterface::_getCalculatedSegmentsPointer;
ptr_getCalculatedPolygonCount CCollDistInterface::_getCalculatedPolygonCount;
ptr_getCalculatedPolygonSize CCollDistInterface::_getCalculatedPolygonSize;
ptr_getCalculatedPolygonArrayPointer CCollDistInterface::_getCalculatedPolygonArrayPointer;
ptr_getCalculatedTriangleAt CCollDistInterface::_getCalculatedTriangleAt;
ptr_getMeshMeshCollision CCollDistInterface::_getMeshMeshCollision;
ptr_getTriangleTriangleCollision CCollDistInterface::_getTriangleTriangleCollision;
ptr_getMeshMeshDistance CCollDistInterface::_getMeshMeshDistance;
ptr_getDistanceAgainstDummy_ifSmaller CCollDistInterface::_getDistanceAgainstDummy_ifSmaller;
ptr_getBoxPointDistance CCollDistInterface::_getBoxPointDistance;
ptr_getApproximateBoxBoxDistance CCollDistInterface::_getApproximateBoxBoxDistance;
ptr_getTriangleTriangleDistance_ifSmaller CCollDistInterface::_getTriangleTriangleDistance_ifSmaller;
ptr_getTrianglePointDistance_ifSmaller CCollDistInterface::_getTrianglePointDistance_ifSmaller;
ptr_getRayProxSensorDistance_ifSmaller CCollDistInterface::_getRayProxSensorDistance_ifSmaller;
ptr_isPointInsideVolume1AndOutsideVolume2 CCollDistInterface::_isPointInsideVolume1AndOutsideVolume2;
ptr_isPointTouchingVolume CCollDistInterface::_isPointTouchingVolume;
ptr_getProxSensorDistance_ifSmaller CCollDistInterface::_getProxSensorDistance_ifSmaller;
ptr_getProxSensorDistanceToSegment_ifSmaller CCollDistInterface::_getProxSensorDistanceToSegment_ifSmaller;
ptr_getProxSensorDistanceToTriangle_ifSmaller CCollDistInterface::_getProxSensorDistanceToTriangle_ifSmaller;
ptr_cutNodeWithVolume CCollDistInterface::_cutNodeWithVolume;

bool CCollDistInterface::_collDistInterfaceInitialized=false;
bool CCollDistInterface::_collDistSystemPluginsLoadPhaseOver=false;

VMutex CCollDistInterface::_objectMutex;


void CCollDistInterface::setSystemPluginsLoadPhaseOver()
{
	_collDistSystemPluginsLoadPhaseOver=true;
}

bool CCollDistInterface::initializeFunctionsIfNeeded()
{
	C_API_FUNCTION_DEBUG;
	static bool alreadyTried=false;
	if (_collDistInterfaceInitialized)
		return(true);
	if (alreadyTried&&_collDistSystemPluginsLoadPhaseOver)
		return(false);
	alreadyTried=true;
	CPlugin* plugin=CPluginContainer::getPluginFromName("MeshCalc");
	if (plugin==NULL)
		return(false); // plugin not there!
	// Get the version first:
	int auxVals[4]={0,0,0,0};
	int retVals[4];
	plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,retVals);
//	int version=retVals[0];
	// Now get all the function pointers:
	int v=1;
	auxVals[0]=v++;
	_createCollisionInformationStructure=(ptr_createCollisionInformationStructure)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_copyCollisionInformationStructure=(ptr_copyCollisionInformationStructure)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_destroyCollisionInformationStructure=(ptr_destroyCollisionInformationStructure)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_scaleCollisionInformationStructure=(ptr_scaleCollisionInformationStructure)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getCollisionInformationStructureSerializationData=(ptr_getCollisionInformationStructureSerializationData)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getCollisionInformationStructureFromSerializationData=(ptr_getCollisionInformationStructureFromSerializationData)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_releaseBuffer=(ptr_releaseBuffer)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getCutMesh=(ptr_getCutMesh)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getCalculatedTriangleCount=(ptr_getCalculatedTriangleCount)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getCalculatedTrianglesPointer=(ptr_getCalculatedTrianglesPointer)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getCalculatedVerticeCount=(ptr_getCalculatedVerticeCount)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getCalculatedVerticesPointer=(ptr_getCalculatedVerticesPointer)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getCalculatedSegmentCount=(ptr_getCalculatedSegmentCount)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getCalculatedSegmentsPointer=(ptr_getCalculatedSegmentsPointer)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getCalculatedPolygonCount=(ptr_getCalculatedPolygonCount)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getCalculatedPolygonSize=(ptr_getCalculatedPolygonSize)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getCalculatedPolygonArrayPointer=(ptr_getCalculatedPolygonArrayPointer)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getCalculatedTriangleAt=(ptr_getCalculatedTriangleAt)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getMeshMeshCollision=(ptr_getMeshMeshCollision)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getTriangleTriangleCollision=(ptr_getTriangleTriangleCollision)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getMeshMeshDistance=(ptr_getMeshMeshDistance)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getDistanceAgainstDummy_ifSmaller=(ptr_getDistanceAgainstDummy_ifSmaller)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getBoxPointDistance=(ptr_getBoxPointDistance)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getApproximateBoxBoxDistance=(ptr_getApproximateBoxBoxDistance)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getTriangleTriangleDistance_ifSmaller=(ptr_getTriangleTriangleDistance_ifSmaller)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getTrianglePointDistance_ifSmaller=(ptr_getTrianglePointDistance_ifSmaller)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getRayProxSensorDistance_ifSmaller=(ptr_getRayProxSensorDistance_ifSmaller)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_isPointInsideVolume1AndOutsideVolume2=(ptr_isPointInsideVolume1AndOutsideVolume2)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_isPointTouchingVolume=(ptr_isPointTouchingVolume)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getProxSensorDistance_ifSmaller=(ptr_getProxSensorDistance_ifSmaller)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getProxSensorDistanceToSegment_ifSmaller=(ptr_getProxSensorDistanceToSegment_ifSmaller)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_getProxSensorDistanceToTriangle_ifSmaller=(ptr_getProxSensorDistanceToTriangle_ifSmaller)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_cutNodeWithVolume=(ptr_cutNodeWithVolume)plugin->sendEventCallbackMessage(sim_message_eventcallback_meshcalculationplugin,auxVals,NULL,NULL);
	auxVals[0]=v++;
	_collDistInterfaceInitialized=true;
	return(true);
}

void CCollDistInterface::lockUnlock(bool lock)
{
	if (lock)
		_objectMutex.lock();
	else
		_objectMutex.unlock();
}

void* CCollDistInterface::createCollisionInformationStructure(const float* cumulMeshVertices,int cumulMeshVerticesSize,const int* cumulMeshIndices,int cumulMeshIndicesSize,float maxTriSize,float edgeAngle,int maxTriCount)
{
	C_API_FUNCTION_DEBUG;
	// Check first if we cannot simply copy the collInfo from a similar geom object:
	if (!initializeFunctionsIfNeeded())
		return(NULL);
	// we call the plugin function:
	return(_createCollisionInformationStructure(cumulMeshVertices,cumulMeshVerticesSize,cumulMeshIndices,cumulMeshIndicesSize,maxTriSize,edgeAngle,maxTriCount));
}

void CCollDistInterface::destroyCollisionInformationStructure(void* collInfo)
{
	C_API_FUNCTION_DEBUG;
	if (!initializeFunctionsIfNeeded())
		return;
	// we call the plugin function:
	_destroyCollisionInformationStructure(collInfo);
}

void CCollDistInterface::scaleCollisionInformationStructure(void* collInfo,float scaleFactor)
{
	C_API_FUNCTION_DEBUG;
	if (!initializeFunctionsIfNeeded())
		return;
	// we call the plugin function:
	_scaleCollisionInformationStructure(collInfo,scaleFactor);
}

unsigned char* CCollDistInterface::getCollisionInformationStructureSerializationData(const void* collInfo,int& dataSize)
{
	C_API_FUNCTION_DEBUG;
	if (!initializeFunctionsIfNeeded())
		return(NULL);
	// we call the plugin function:
	return(_getCollisionInformationStructureSerializationData(collInfo,&dataSize));
}

void CCollDistInterface::releaseBuffer(void* buffer)
{
	C_API_FUNCTION_DEBUG;
	if (!initializeFunctionsIfNeeded())
		return;
	// we call the plugin function:
	_releaseBuffer(buffer);
}

void* CCollDistInterface::getCollisionInformationStructureFromSerializationData(const unsigned char* data,const float* cumulMeshVertices,int cumulMeshVerticesSize,const int* cumulMeshIndices,int cumulMeshIndicesSize)
{
	C_API_FUNCTION_DEBUG;
	if (!initializeFunctionsIfNeeded())
		return(NULL);
	// we call the plugin function:
	return(_getCollisionInformationStructureFromSerializationData(data,cumulMeshVertices,cumulMeshVerticesSize,cumulMeshIndices,cumulMeshIndicesSize));
}

void* CCollDistInterface::copyCollisionInformationStructure(const void* collInfo)
{
	C_API_FUNCTION_DEBUG;
	if (!initializeFunctionsIfNeeded())
		return(NULL);
	// we call the plugin function:
	return(_copyCollisionInformationStructure(collInfo));
}

bool CCollDistInterface::getCutMesh(const void* collInfo,const C7Vector* tr,float** vertices,int* verticesSize,int** indices,int* indicesSize,int options)
{
	C_API_FUNCTION_DEBUG;
	if (!initializeFunctionsIfNeeded())
		return(false);
	// we call the plugin function:
	float tr_[7];
	tr->getInternalData(tr_);
	return(_getCutMesh(collInfo,tr_,vertices,verticesSize,indices,indicesSize,options)==0);
}

int CCollDistInterface::getCalculatedTriangleCount(const void* collInfo)
{
	if (!initializeFunctionsIfNeeded())
		return(0);
	// we call the plugin function:
	return(_getCalculatedTriangleCount(collInfo));
}

int* CCollDistInterface::getCalculatedTrianglesPointer(const void* collInfo)
{
	if (!initializeFunctionsIfNeeded())
		return(NULL);
	// we call the plugin function:
	return(_getCalculatedTrianglesPointer(collInfo));
}

int CCollDistInterface::getCalculatedSegmentCount(const void* collInfo)
{
	if (!initializeFunctionsIfNeeded())
		return(0);
	// we call the plugin function:
	return(_getCalculatedSegmentCount(collInfo));
}

int* CCollDistInterface::getCalculatedSegmentsPointer(const void* collInfo)
{
	if (!initializeFunctionsIfNeeded())
		return(NULL);
	// we call the plugin function:
	return(_getCalculatedSegmentsPointer(collInfo));
}

int CCollDistInterface::getCalculatedVerticeCount(const void* collInfo)
{
	C_API_FUNCTION_DEBUG;
	if (!initializeFunctionsIfNeeded())
		return(0);
	// we call the plugin function:
	return(_getCalculatedVerticeCount(collInfo));
}

float* CCollDistInterface::getCalculatedVerticesPointer(const void* collInfo)
{
	if (!initializeFunctionsIfNeeded())
		return(NULL);
	// we call the plugin function:
	return(_getCalculatedVerticesPointer(collInfo));
}

int CCollDistInterface::getCalculatedPolygonCount(const void* collInfo)
{
	if (!initializeFunctionsIfNeeded())
		return(0);
	// we call the plugin function:
	return(_getCalculatedPolygonCount(collInfo));
}

int CCollDistInterface::getCalculatedPolygonSize(const void* collInfo,int polygonIndex)
{
	if (!initializeFunctionsIfNeeded())
		return(0);
	// we call the plugin function:
	return(_getCalculatedPolygonSize(collInfo,polygonIndex));
}

int* CCollDistInterface::getCalculatedPolygonArrayPointer(const void* collInfo,int polygonIndex)
{
	if (!initializeFunctionsIfNeeded())
		return(NULL);
	// we call the plugin function:
	return(_getCalculatedPolygonArrayPointer(collInfo,polygonIndex));
}

bool CCollDistInterface::getCalculatedTriangleAt(const void* collInfo,C3Vector& a0,C3Vector& a1,C3Vector& a2,int ind)
{
	if (!initializeFunctionsIfNeeded())
		return(false);
	// we call the plugin function:
	return(_getCalculatedTriangleAt(collInfo,a0.data,a1.data,a2.data,ind)!=0);
}

int CCollDistInterface::getTriangleTriangleCollision(const C3Vector& a0,const C3Vector& e0,const C3Vector& e1,const C3Vector& b0,const C3Vector& f0,const C3Vector& f1,C3Vector* intSegPart0,C3Vector* intSegPart1,bool getIntersection)
{
	if (!initializeFunctionsIfNeeded())
		return(0);
	// we call the plugin function:
	int retVal;
	if (intSegPart0==NULL)
		retVal=_getTriangleTriangleCollision(a0.data,e0.data,e1.data,b0.data,f0.data,f1.data,NULL,NULL,getIntersection);
	else
	{
		float v0[3];
		float v1[3];
		retVal=_getTriangleTriangleCollision(a0.data,e0.data,e1.data,b0.data,f0.data,f1.data,v0,v1,getIntersection);
		intSegPart0->setInternalData(v0);
		intSegPart1->setInternalData(v1);
	}
	return(retVal);
}

float CCollDistInterface::getBoxPointDistance(const C4X4Matrix& t1,const C3Vector& s1,const C3Vector& dummyPos)
{
	if (!initializeFunctionsIfNeeded())
		return(0.0f);
	// we call the plugin function:
	float _t1[12];
	t1.copyToInterface(_t1);
	return(_getBoxPointDistance(_t1,s1.data,dummyPos.data));
}

float CCollDistInterface::getApproximateBoxBoxDistance(const C4X4Matrix& t1,const C3Vector& s1,const C4X4Matrix& t2,const C3Vector& s2)
{
	if (!initializeFunctionsIfNeeded())
		return(0.0f);
	// we call the plugin function:
	float _t1[12];
	t1.copyToInterface(_t1);
	float _t2[12];
	t2.copyToInterface(_t2);
	return(_getApproximateBoxBoxDistance(_t1,s1.data,_t2,s2.data));
}

bool CCollDistInterface::getTriangleTriangleDistance_ifSmaller(const C3Vector& a0,const C3Vector& e0,const C3Vector& e1,const C3Vector& b0,const C3Vector& f0,const C3Vector& f1,float &dist,C3Vector& segA,C3Vector& segB)
{
	if (!initializeFunctionsIfNeeded())
		return(false);
	// we call the plugin function:
	float _segA[3];
	float _segB[3];
	bool retVal=_getTriangleTriangleDistance_ifSmaller(a0.data,e0.data,e1.data,b0.data,f0.data,f1.data,&dist,_segA,_segB)!=0;
	if (retVal)
	{
		segA.set(_segA);
		segB.set(_segB);
	}
	return(retVal);
}

bool CCollDistInterface::getTrianglePointDistance_ifSmaller(const C3Vector& a0,const C3Vector& e0,const C3Vector& e1,const C3Vector& dummyPos,float &dist,C3Vector& segA)
{
	if (!initializeFunctionsIfNeeded())
		return(false);
	// we call the plugin function:
	float _segA[3];
	bool retVal=_getTrianglePointDistance_ifSmaller(a0.data,e0.data,e1.data,dummyPos.data,&dist,_segA)!=0;
	if (retVal)
		segA.set(_segA);
	return(retVal);
}

bool CCollDistInterface::getRayProxSensorDistance_ifSmaller(const void* collInfo,const C4X4Matrix& selfPCTM,float &dist,const C3Vector& lp,float closeThreshold,const C3Vector& lvFar,float cosAngle,C3Vector& detectPoint,bool fast,bool frontFace,bool backFace,char* closeDetectionTriggered,C3Vector& triNormalNotNormalized,void* theOcclusionCheckCallback)
{
	if (!initializeFunctionsIfNeeded())
		return(false);
	// we call the plugin function:
	float _selfPCTM[12];
	selfPCTM.copyToInterface(_selfPCTM);
	float _detectPoint[3];
	float _triNormalNotNormalized[3];
	bool retVal=_getRayProxSensorDistance_ifSmaller(collInfo,_selfPCTM,&dist,lp.data,closeThreshold,lvFar.data,cosAngle,_detectPoint,fast,frontFace,backFace,closeDetectionTriggered,_triNormalNotNormalized,theOcclusionCheckCallback)!=0;
	if (retVal)
	{
		detectPoint.set(_detectPoint);
		triNormalNotNormalized.set(_triNormalNotNormalized);
	}
	return(retVal);
}

bool CCollDistInterface::isPointInsideVolume1AndOutsideVolume2(const C3Vector& p,const std::vector<float>* planes,const std::vector<float>* planesOutside)
{
	const float* _planes=NULL;
	int _planesSize=0;
	if ((planes!=NULL)&&(planes->size()!=0))
	{
		_planes=&(*planes)[0];
		_planesSize=planes->size();
	}
	const float* _planesOutside=NULL;
	int _planesOutsideSize=0;
	if ((planesOutside!=NULL)&&(planesOutside->size()!=0))
	{
		_planesOutside=&(*planesOutside)[0];
		_planesOutsideSize=planesOutside->size();
	}
	if (!initializeFunctionsIfNeeded())
		return(false);
	// we call the plugin function:
	return(_isPointInsideVolume1AndOutsideVolume2(p.data,_planes,_planesSize,_planesOutside,_planesOutsideSize)!=0);
}

bool CCollDistInterface::getProxSensorDistance_ifSmaller(const void* collInfo,const C4X4Matrix& itPCTM,float &dist,const std::vector<float>* planes,const std::vector<float>* planesOutside,float cosAngle,C3Vector& detectPoint,bool fast,bool frontFace,bool backFace,std::vector<float>* cutEdges,C3Vector& triNormalNotNormalized,void* theOcclusionCheckCallback)
{
	const float* _planes=NULL;
	int _planesSize=0;
	if ((planes!=NULL)&&(planes->size()!=0))
	{
		_planes=&(*planes)[0];
		_planesSize=planes->size();
	}
	const float* _planesOutside=NULL;
	int _planesOutsideSize=0;
	if ((planesOutside!=NULL)&&(planesOutside->size()!=0))
	{
		_planesOutside=&(*planesOutside)[0];
		_planesOutsideSize=planesOutside->size();
	}
	if (!initializeFunctionsIfNeeded())
		return(false);
	// we call the plugin function:
	float _itPCTM[12];
	itPCTM.copyToInterface(_itPCTM);
	float _detectPoint[3];
	float _triNormalNotNormalized[3];
	bool retVal=_getProxSensorDistance_ifSmaller(collInfo,_itPCTM,&dist,_planes,_planesSize,_planesOutside,_planesOutsideSize,cosAngle,_detectPoint,fast,frontFace,backFace,_triNormalNotNormalized,theOcclusionCheckCallback)!=0;
	if (retVal)
	{
		detectPoint.set(_detectPoint);
		triNormalNotNormalized.set(_triNormalNotNormalized);
	}
	return(retVal);
}

bool CCollDistInterface::getProxSensorDistanceToSegment_ifSmaller(const C3Vector& p0,const C3Vector& p1,float &dist,const std::vector<float>* planes,const std::vector<float>* planesOutside,float maxAngle,C3Vector& detectPoint)
{
	const float* _planes=NULL;
	int _planesSize=0;
	if ((planes!=NULL)&&(planes->size()!=0))
	{
		_planes=&(*planes)[0];
		_planesSize=planes->size();
	}
	const float* _planesOutside=NULL;
	int _planesOutsideSize=0;
	if ((planesOutside!=NULL)&&(planesOutside->size()!=0))
	{
		_planesOutside=&(*planesOutside)[0];
		_planesOutsideSize=planesOutside->size();
	}
	if (!initializeFunctionsIfNeeded())
		return(false);
	// we call the plugin function:
	float _detectPoint[3];
	bool retVal=_getProxSensorDistanceToSegment_ifSmaller(p0.data,p1.data,&dist,_planes,_planesSize,_planesOutside,_planesOutsideSize,maxAngle,_detectPoint)!=0;
	if (retVal)
		detectPoint.set(_detectPoint);
	return(retVal);
}

bool CCollDistInterface::getProxSensorDistanceToTriangle_ifSmaller(const C3Vector& a0,const C3Vector& e0,const C3Vector& e1,float &dist,const std::vector<float>* planes,const std::vector<float>* planesOutside,float cosAngle,C3Vector& detectPoint,bool frontFace,bool backFace,C3Vector& triNormalNotNormalized)
{
	const float* _planes=NULL;
	int _planesSize=0;
	if ((planes!=NULL)&&(planes->size()!=0))
	{
		_planes=&(*planes)[0];
		_planesSize=planes->size();
	}
	const float* _planesOutside=NULL;
	int _planesOutsideSize=0;
	if ((planesOutside!=NULL)&&(planesOutside->size()!=0))
	{
		_planesOutside=&(*planesOutside)[0];
		_planesOutsideSize=planesOutside->size();
	}
	if (!initializeFunctionsIfNeeded())
		return(false);
	// we call the plugin function:
	float _detectPoint[3];
	float _triNormalNotNormalized[3];
	bool retVal=_getProxSensorDistanceToTriangle_ifSmaller(a0.data,e0.data,e1.data,&dist,_planes,_planesSize,_planesOutside,_planesOutsideSize,cosAngle,_detectPoint,frontFace,backFace,_triNormalNotNormalized)!=0;
	if (retVal)
	{
		detectPoint.set(_detectPoint);
		triNormalNotNormalized.set(_triNormalNotNormalized);
	}
	return(retVal);
}

bool CCollDistInterface::getDistanceAgainstDummy_ifSmaller(const void* collInfo,const C3Vector& dummyPos,const C4X4Matrix& itPCTM,float &dist,C3Vector& ray0,C3Vector& ray1,int& itBuff)
{
	if (!initializeFunctionsIfNeeded())
		return(false);
	// we call the plugin function:
	float _itPCTM[12];
	itPCTM.copyToInterface(_itPCTM);
	float _ray0[3];
	float _ray1[3];
	bool retVal=_getDistanceAgainstDummy_ifSmaller(collInfo,dummyPos.data,_itPCTM,&dist,_ray0,_ray1,&itBuff)!=0;
	if (retVal)
	{
		ray0.set(_ray0);
		ray1.set(_ray1);
	}
	return(retVal);
}

float CCollDistInterface::cutNodeWithVolume(void* collInfo,const C4X4Matrix& itPCTM,const std::vector<float>* planes)
{
	C_API_FUNCTION_DEBUG;
	const float* _planes=NULL;
	int _planesSize=0;
	if ((planes!=NULL)&&(planes->size()!=0))
	{
		_planes=&(*planes)[0];
		_planesSize=planes->size();
	}
	if (!initializeFunctionsIfNeeded())
		return(0.0f);
	// we call the plugin function:
	float _itPCTM[12];
	itPCTM.copyToInterface(_itPCTM);
	return(_cutNodeWithVolume(collInfo,_itPCTM,_planes,_planesSize));
}

void CCollDistInterface::getMeshMeshDistance(const void* collInfo1,const void* collInfo2,const C4X4Matrix distObjMatr[2],const void* collInfos[2],bool inverseExploration,float distances[7],int caching[2])
{
	if (!initializeFunctionsIfNeeded())
		return;
	// we call the plugin function:
	float distObjMatr1[12];
	float distObjMatr2[12];
	distObjMatr[0].copyToInterface(distObjMatr1);
	distObjMatr[1].copyToInterface(distObjMatr2);
	_getMeshMeshDistance(collInfo1,collInfo2,distObjMatr1,distObjMatr2,collInfos,inverseExploration,distances,caching);
}

bool CCollDistInterface::getMeshMeshCollision(const void* collInfo1,const void* collInfo2,const C4X4Matrix collObjMatr[2],const void* collInfos[2],bool inverseExploration,std::vector<float>* intersections,int caching[2])
{
	if (!initializeFunctionsIfNeeded())
		return(false);
	// we call the plugin function:
	float collObjMatr1[12];
	float collObjMatr2[12];
	collObjMatr[0].copyToInterface(collObjMatr1);
	collObjMatr[1].copyToInterface(collObjMatr2);
	bool retVal;
	if (intersections==NULL)
		retVal=_getMeshMeshCollision(collInfo1,collInfo2,collObjMatr1,collObjMatr2,collInfos,inverseExploration,NULL,NULL,caching)!=0;
	else
	{
		float* _intersections;
		int _intersectionsSize;
		retVal=_getMeshMeshCollision(collInfo1,collInfo2,collObjMatr1,collObjMatr2,collInfos,inverseExploration,&_intersections,&_intersectionsSize,caching)!=0;
		if (retVal)
			intersections->assign(_intersections,_intersections+_intersectionsSize);
		_releaseBuffer(_intersections);
	}
	return(retVal);
}

bool CCollDistInterface::isPointTouchingVolume(const C3Vector& p,const std::vector<float>* planes)
{
	const float* _planes=NULL;
	int _planesSize=0;
	if ((planes!=NULL)&&(planes->size()!=0))
	{
		_planes=&(*planes)[0];
		_planesSize=planes->size();
	}
	if (!initializeFunctionsIfNeeded())
		return(false);
	// we call the plugin function:
	return(_isPointTouchingVolume(p.data,_planes,_planesSize)!=0);
}


#else

void* CCollDistInterface::_createCollisionInformationStructure(const float* cumulMeshVertices,int cumulMeshVerticesSize,const int* cumulMeshIndices,int cumulMeshIndicesSize,float maxTriSize,float edgeAngle,int maxTriCount)
{
	CCollInfo* newCollInfo=CCollDistAlgos::copyFromSimilarCollInfo(cumulMeshVertices,cumulMeshVerticesSize,cumulMeshIndices,cumulMeshIndicesSize,maxTriSize,edgeAngle,maxTriCount);
	if (newCollInfo==NULL)
		newCollInfo=new CCollInfo(cumulMeshVertices,cumulMeshVerticesSize,cumulMeshIndices,cumulMeshIndicesSize,maxTriSize,edgeAngle,maxTriCount);
	CCollDistAlgos::insertCollInfo(newCollInfo);
	return(newCollInfo);
}

void* CCollDistInterface::_copyCollisionInformationStructure(const void* collInfo)
{
	CCollInfo* newCollInfo=((CCollInfo*)collInfo)->copyYourself();
	CCollDistAlgos::insertCollInfo(newCollInfo);
	return(newCollInfo);
}

void CCollDistInterface::_destroyCollisionInformationStructure(void* collInfo)
{
	CCollDistAlgos::eraseCollInfo((CCollInfo*)collInfo);
}

void CCollDistInterface::_scaleCollisionInformationStructure(void* collInfo,float scaleFactor)
{
	((CCollInfo*)collInfo)->scale(scaleFactor);
}

unsigned char* CCollDistInterface::_getCollisionInformationStructureSerializationData(const void* collInfo,int* dataSize)
{
	return(((CCollInfo*)collInfo)->getSerializationData(dataSize[0]));
}

void* CCollDistInterface::_getCollisionInformationStructureFromSerializationData(const unsigned char* data,const float* cumulMeshVertices,int cumulMeshVerticesSize,const int* cumulMeshIndices,int cumulMeshIndicesSize)
{
	CCollInfo* newCollInfo=new CCollInfo();
	newCollInfo->buildFromSerializationData(data,cumulMeshVertices,cumulMeshVerticesSize,cumulMeshIndices,cumulMeshIndicesSize);
	CCollDistAlgos::insertCollInfo(newCollInfo);
	return(newCollInfo);
}

void CCollDistInterface::_releaseBuffer(void* buffer)
{
	delete[] ((char*)buffer);
}

char CCollDistInterface::_getCutMesh(const void* collInfo,const float* tr,float** vertices,int* verticesSize,int** indices,int* indicesSize,int options)
{
	C7Vector tr_;
	tr_.setInternalData(tr);
	if (((CCollInfo*)collInfo)->getCutMesh(&tr_,vertices,verticesSize,indices,indicesSize,options))
		return(0);
	return(1);
}


int CCollDistInterface::_getCalculatedTriangleCount(const void* collInfo)
{
	return(((CCollInfo*)collInfo)->calcIndices.size()/3);
}

int* CCollDistInterface::_getCalculatedTrianglesPointer(const void* collInfo)
{
	CCollInfo* ci=(CCollInfo*)collInfo;
	if (ci->calcIndices.size()==0)
		return(NULL);
	return(&ci->calcIndices[0]);
}

int CCollDistInterface::_getCalculatedVerticeCount(const void* collInfo)
{
	return(((CCollInfo*)collInfo)->calcVertices.size()/3);
}

float* CCollDistInterface::_getCalculatedVerticesPointer(const void* collInfo)
{
	CCollInfo* ci=(CCollInfo*)collInfo;
	if (ci->calcVertices.size()==0)
		return(NULL);
	return(&ci->calcVertices[0]);
}

int CCollDistInterface::_getCalculatedSegmentCount(const void* collInfo)
{
	return(((CCollInfo*)collInfo)->calcSegments.size()/2);
}

int* CCollDistInterface::_getCalculatedSegmentsPointer(const void* collInfo)
{
	CCollInfo* ci=(CCollInfo*)collInfo;
	if (ci->calcSegments.size()==0)
		return(NULL);
	return(&ci->calcSegments[0]);
}

int CCollDistInterface::_getCalculatedPolygonCount(const void* collInfo)
{
	return(((CCollInfo*)collInfo)->calcPolygons.size());
}

int CCollDistInterface::_getCalculatedPolygonSize(const void* collInfo,int polygonIndex)
{
	return(((CCollInfo*)collInfo)->calcPolygons[polygonIndex].size());
}

int* CCollDistInterface::_getCalculatedPolygonArrayPointer(const void* collInfo,int polygonIndex)
{
	CCollInfo* ci=(CCollInfo*)collInfo;
	if (ci->calcPolygons[polygonIndex].size()==0)
		return(NULL);
	return(&ci->calcPolygons[polygonIndex][0]);
}

char CCollDistInterface::_getCalculatedTriangleAt(const void* collInfo,float* a0,float* a1,float* a2,int ind)
{
	if (((CCollInfo*)collInfo)->getCalcTriangleAt(a0,a1,a2,ind))
		return(1);
	return(0);
}

char CCollDistInterface::_getMeshMeshCollision(const void* collInfo1,const void* collInfo2,const float* collObjMatr1,const float* collObjMatr2,const void* collInfos[2],char inverseExploration,float** intersections,int* intersectionsSize,int caching[2])
{
	C4X4Matrix shapeACTM;
	C4X4Matrix shapeBCTM;
	shapeACTM.copyFromInterface(collObjMatr1);
	shapeBCTM.copyFromInterface(collObjMatr2);
	C4X4Matrix collObjMatr[2]={shapeACTM,shapeBCTM};
	if (intersections==NULL)
	{
		if (CCollDistAlgos::getCollision_Stat(((const CCollInfo*)collInfo1)->collNode,collObjMatr,(const CCollInfo**)collInfos,((const CCollInfo*)collInfo2)->collNode,inverseExploration!=0,NULL,caching))
			return(1);
		return(0);
	}
	else
	{
		char retVal;
		std::vector<float> _intersections;
		if (CCollDistAlgos::getCollision_Stat(((const CCollInfo*)collInfo1)->collNode,collObjMatr,(const CCollInfo**)collInfos,((const CCollInfo*)collInfo2)->collNode,inverseExploration!=0,&_intersections,caching))
			retVal=1;
		else
			retVal=0;
		intersections[0]=new float[_intersections.size()];
		intersectionsSize[0]=_intersections.size();
		for (int i=0;i<int(_intersections.size());i++)
			intersections[0][i]=_intersections[i];
		return(retVal);
	}
}

int CCollDistInterface::_getTriangleTriangleCollision(const float* a0,const float* e0,const float* e1,const float* b0,const float* f0,const float* f1,float* intSegPart0,float* intSegPart1,char getIntersection)
{
	C3Vector _a0(a0);
	C3Vector _e0(e0);
	C3Vector _e1(e1);
	C3Vector _b0(b0);
	C3Vector _f0(f0);
	C3Vector _f1(f1);
	int retVal;
	if (intSegPart0==NULL)
		retVal=CCollDistAlgos::triangleTriangleCollisionStatic(_a0,_e0,_e1,_b0,_f0,_f1,NULL,NULL,getIntersection!=0);
	else
	{
		C3Vector v0,v1;
		retVal=CCollDistAlgos::triangleTriangleCollisionStatic(_a0,_e0,_e1,_b0,_f0,_f1,&v0,&v1,getIntersection!=0);
		v0.getInternalData(intSegPart0);
		v1.getInternalData(intSegPart1);
	}
	return(retVal);
}

void CCollDistInterface::_getMeshMeshDistance(const void* collInfo1,const void* collInfo2,const float* distObjMatr1,const float* distObjMatr2,const void* collInfos[2],char inverseExploration,float distances[7],int caching[2])
{
	C4X4Matrix shapeACTM;
	C4X4Matrix shapeBCTM;
	shapeACTM.copyFromInterface(distObjMatr1);
	shapeBCTM.copyFromInterface(distObjMatr2);
	C4X4Matrix distObjMatr[2]={shapeACTM,shapeBCTM};
	CCollDistAlgos::getDistance_Stat(((const CCollInfo*)collInfo1)->collNode,distObjMatr,(const CCollInfo**)collInfos,((const CCollInfo*)collInfo2)->collNode,inverseExploration!=0,distances,caching);
}

char CCollDistInterface::_getDistanceAgainstDummy_ifSmaller(const void* collInfo,const float* dummyPos,const float* itPCTM,float* dist,float* ray0,float* ray1,int* itBuff)
{
	C4X4Matrix _itPCTM;
	_itPCTM.copyFromInterface(itPCTM);
	C3Vector _ray0,_ray1;
	char retVal=CCollDistAlgos::getDistanceAgainstDummy_IfSmaller(((CCollInfo*)collInfo)->collNode,dummyPos,(CCollInfo*)collInfo,_itPCTM,dist[0],_ray0,_ray1,itBuff[0]);
	_ray0.getInternalData(ray0);
	_ray1.getInternalData(ray1);
	return(retVal);
}

float CCollDistInterface::_getBoxPointDistance(const float* t1,const float* s1,const float* dummyPos)
{
	C4X4Matrix _t1;
	_t1.copyFromInterface(t1);
	C3Vector _s1(s1);
	C3Vector _dummyPos(dummyPos);
	return(CCollDistAlgos::getBoxPointDistance(_t1,_s1,_dummyPos));

}

float CCollDistInterface::_getApproximateBoxBoxDistance(const float* t1,const float* s1,const float* t2,const float* s2)
{
	C4X4Matrix _t1;
	_t1.copyFromInterface(t1);
	C3Vector _s1(s1);
	C4X4Matrix _t2;
	_t2.copyFromInterface(t2);
	C3Vector _s2(s2);
	return(CCollDistAlgos::getApproxBoxBoxDistance(_t1,_s1,_t2,_s2));
}

char CCollDistInterface::_getTriangleTriangleDistance_ifSmaller(const float* a0,const float* e0,const float* e1,const float* b0,const float* f0,const float* f1,float* dist,float* segA,float* segB)
{
	C3Vector _a0(a0);
	C3Vector _e0(e0);
	C3Vector _e1(e1);
	C3Vector _b0(b0);
	C3Vector _f0(f0);
	C3Vector _f1(f1);
	C3Vector _segA,_segB;
	char retVal=CCollDistAlgos::getTriangleTriangleDistance_IfSmaller(_a0,_e0,_e1,_b0,_f0,_f1,dist[0],_segA,_segB);
	_segA.copyTo(segA);
	_segB.copyTo(segB);
	return(retVal);
}

char CCollDistInterface::_getTrianglePointDistance_ifSmaller(const float* a0,const float* e0,const float* e1,const float* dummyPos,float* dist,float* segA)
{
	C3Vector _a0(a0);
	C3Vector _e0(e0);
	C3Vector _e1(e1);
	C3Vector _dummyPos(dummyPos);
	C3Vector _segA;
	char retVal=CCollDistAlgos::getTrianglePointDistance_IfSmaller(_a0,_e0,_e1,_dummyPos,dist[0],_segA);
	_segA.copyTo(segA);
	return(retVal);
}

char CCollDistInterface::_getRayProxSensorDistance_ifSmaller(const void* collInfo,const float* selfPCTM,float* dist,const float* lp,float closeThreshold,const float* lvFar,float cosAngle,float* detectPoint,bool fast,bool frontFace,bool backFace,char* closeDetectionTriggered,float* triNormalNotNormalized,void* theOcclusionCheckCallback)
{
	C4X4Matrix _selfPCTM;
	_selfPCTM.copyFromInterface(selfPCTM);
	C3Vector _lp(lp);
	C3Vector _lvFar(lvFar);
	C3Vector _detectPoint;
	C3Vector _triNormalNotNormalized;
	bool retVal=CCollDistAlgos::getRayProxSensorDistance_IfSmaller(((CCollInfo*)collInfo)->collNode,(CCollInfo*)collInfo,_selfPCTM,dist[0],_lp,closeThreshold,_lvFar,cosAngle,_detectPoint,fast,frontFace,backFace,closeDetectionTriggered,_triNormalNotNormalized,(OCCLUSION_CHECK_CALLBACK)theOcclusionCheckCallback);
	if (retVal)
	{
		_detectPoint.copyTo(detectPoint);
		_triNormalNotNormalized.copyTo(triNormalNotNormalized);
		return(1);
	}
	return(0);
}

char CCollDistInterface::_isPointInsideVolume1AndOutsideVolume2(const float* p,const float* planes,int planesSize,const float* planesOutside,int planesOutsideSize)
{
	C3Vector _p(p);
	return(CCollDistAlgos::isPointInsideVolume1AndOutsideVolume2(_p,planes,planesSize,planesOutside,planesOutsideSize));
}

char CCollDistInterface::_isPointTouchingVolume(const float* p,const float* planes,int planesSize)
{
	C3Vector _p(p);
	return(CCollDistAlgos::isPointTouchingVolume(_p,planes,planesSize));
}

char CCollDistInterface::_getProxSensorDistance_ifSmaller(const void* collInfo,const float* itPCTM,float* dist,const float* planes,int planesSize,const float* planesOutside,int planesOutsideSize,float cosAngle,float* detectPoint,bool fast,bool frontFace,bool backFace,float* triNormalNotNormalized,void* theOcclusionCheckCallback)
{
	C4X4Matrix _itPCTM;
	_itPCTM.copyFromInterface(itPCTM);
	C3Vector _detectPoint,_triNormalNotNormalized;
	char retVal=CCollDistAlgos::getProxSensorDistance_IfSmaller(((CCollInfo*)collInfo)->collNode,(CCollInfo*)collInfo,_itPCTM,dist[0],planes,planesSize,planesOutside,planesOutsideSize,cosAngle,_detectPoint,fast,frontFace,backFace,NULL,_triNormalNotNormalized,(OCCLUSION_CHECK_CALLBACK)theOcclusionCheckCallback);
	_detectPoint.copyTo(detectPoint);
	_triNormalNotNormalized.copyTo(triNormalNotNormalized);
	return(retVal);
}

char CCollDistInterface::_getProxSensorDistanceToSegment_ifSmaller(const float* p0,const float* p1,float* dist,const float* planes,int planesSize,const float* planesOutside,int planesOutsideSize,float maxAngle,float* detectPoint)
{
	C3Vector _p0(p0);
	C3Vector _p1(p1);
	C3Vector _detectPoint;
	char retVal=CCollDistAlgos::getProxSensorDistanceToSegment_IfSmaller(_p0,_p1,dist[0],planes,planesSize,planesOutside,planesOutsideSize,maxAngle,_detectPoint);
	_detectPoint.copyTo(detectPoint);
	return(retVal);
}

char CCollDistInterface::_getProxSensorDistanceToTriangle_ifSmaller(const float* a0,const float* e0,const float* e1,float* dist,const float* planes,int planesSize,const float* planesOutside,int planesOutsideSize,float cosAngle,float* detectPoint,bool frontFace,bool backFace,float* triNormalNotNormalized)
{
	C3Vector _a0(a0);
	C3Vector _e0(e0);
	C3Vector _e1(e1);
	C3Vector _detectPoint,_triNormalNotNormalized;
	char retVal=CCollDistAlgos::getProxSensorDistanceToTriangle_IfSmaller(_a0,_e0,_e1,dist[0],planes,planesSize,planesOutside,planesOutsideSize,cosAngle,_detectPoint,frontFace,backFace,NULL,_triNormalNotNormalized);
	_detectPoint.copyTo(detectPoint);
	_triNormalNotNormalized.copyTo(triNormalNotNormalized);
	return(retVal);
}

float CCollDistInterface::_cutNodeWithVolume(void* collInfo,const float* itPCTM,const float* planes,int planesSize)
{
	C4X4Matrix _itPCTM;
	_itPCTM.copyFromInterface(itPCTM);
	return(CCollDistAlgos::cutNodeWithSensor(((CCollInfo*)collInfo)->collNode,(CCollInfo*)collInfo,_itPCTM,planes,planesSize));
}

#endif // COMPILING_EXTERNAL_COLL_DIST_DLL
