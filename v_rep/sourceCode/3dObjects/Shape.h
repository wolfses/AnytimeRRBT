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

#include "3DObject.h"
#include "Dummy.h"
#include "geomProxy.h"

class CShape : public C3DObject  
{
public:

	CShape();
	virtual ~CShape();

	// Following functions are inherited from 3DObject
	void display(CViewableBase* renderingObject,int displayAttrib);
	C3DObject* copyYourself();
	void scaleObject(float scalingFactor);
	void serialize(CSer& ar);
	void serializeWExtIk(CExtIkSer& ar);
	bool announceObjectWillBeErased(int objID,bool copyBuffer);
	void announceGroupWillBeErased(int groupID,bool copyBuffer);
	void announceCollisionWillBeErased(int collisionID,bool copyBuffer);
	void announceDistanceWillBeErased(int distanceID,bool copyBuffer);
	void announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer);
	void announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer);
	void performObjectLoadingMapping(std::vector<int>* map);
	void performGroupLoadingMapping(std::vector<int>* map);
	void performCollisionLoadingMapping(std::vector<int>* map);
	void performDistanceLoadingMapping(std::vector<int>* map);
	void performIkLoadingMapping(std::vector<int>* map);
	void performGcsLoadingMapping(std::vector<int>* map);
	void performTextureObjectLoadingMapping(std::vector<int>* map);
	void performDynMaterialObjectLoadingMapping(std::vector<int>* map);
	void bufferMainDisplayStateVariables();
	void bufferedMainDisplayStateVariablesToDisplay();
	void simulationAboutToStart();
	void simulationEnded();
	void initializeInitialValues(bool simulationIsRunning);
	bool getFullBoundingBox(C3Vector& minV,C3Vector& maxV);
	bool getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV);
	bool getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices);
	std::string getObjectTypeInfo();
	std::string getObjectTypeInfoExtended();

	// Various
	void commonInit();
	bool getCulling();
	void setCulling(bool culState);
	bool getVisibleEdges();
	void setVisibleEdges(bool v);
	int getEdgeWidth();
	void setEdgeWidth(int w);
	bool getShapeWireframe();
	void setShapeWireframe(bool w);

	void resetMilling();
	bool applyMilling();

	bool getStartInDynamicSleeping();
	void setStartInDynamicSleeping(bool sleeping);
	bool getShapeIsDynamicallyStatic();
	void setShapeIsDynamicallyStatic(bool sta);

	C3Vector getInitialDynamicVelocity();
	void setInitialDynamicVelocity(const C3Vector& vel);
	C3Vector getInitialDynamicAngularVelocity();
	void setInitialDynamicAngularVelocity(const C3Vector& vel);
	bool isGrouping();
	void setColor(const char* colorName,int colorComponent,const float* rgbData);
	bool getColor(const char* colorName,int colorComponent,float* rgbData);
	void setRespondable(bool r);
	bool getRespondable();
	void setDynamicCollisionMask(WORD m);
	WORD getDynamicCollisionMask();
	void setParentFollowsDynamic(bool f);
	bool getParentFollowsDynamic();
	void setShapeIsStaticAndNotRespondableButDynamicTag(bool f);
	bool getShapeIsStaticAndNotRespondableButDynamicTag();

	bool getSetAutomaticallyToNonStaticIfGetsParent();
	void setSetAutomaticallyToNonStaticIfGetsParent(bool autoNonStatic);



	void setDynamicVelocity(const C3Vector& linearV,const C3Vector& angularV);
	C3Vector getDynamicLinearVelocity();
	C3Vector getDynamicAngularVelocity();
	void addAdditionalForceAndTorque(const C3Vector& f,const C3Vector& t);
	void clearAdditionalForceAndTorque();
	C3Vector getAdditionalForce();
	C3Vector getAdditionalTorque();

	C3DObject* getLastParentForLocalGlobalCollidable();
	void clearLastParentForLocalGlobalCollidable();

	// Distance measurement functions
	bool getShapeShapeDistance_IfSmaller(CShape* it,float &dist,float ray[7],int buffer[2]);
	bool getDistanceToDummy_IfSmaller(CDummy* it,float &dist,float ray[7],int& buffer);

	// Collision detection functions
	bool isCollisionInformationInitialized();
//	void initializeCollisionDetection();
	void initializeCalculationStructureIfNeeded();
	void removeCollisionInformation();
	bool doesShapeCollideWithShape(CShape* collidee,std::vector<float>* intersections,int buffer[2]);

	// Bounding box functions
	void alignBoundingBoxWithMainAxis();
	void alignBoundingBoxWithWorld();
	bool alignTubeBoundingBoxWithMainAxis();
	bool alignCuboidBoundingBoxWithMainAxis();

	void setInsideAndOutsideFacesSameColor(bool s);
	bool getInsideAndOutsideFacesSameColor();

	void actualizeContainsTransparentComponent();
	bool getContainsTransparentComponent();

	void setRigidBodyWasAlreadyPutToSleepOnce(bool s);
	bool getRigidBodyWasAlreadyPutToSleepOnce();

	CGeomProxy* geomData;
protected:

	bool reorientGeometry(int type); // 0=main axis, 1=world, 2=tube, 3=cuboid

	WORD _dynamicCollisionMask;
	C3DObject* _lastParentForLocalGlobalCollidable;

	// Variables which need to be serialized
	bool _startInDynamicSleeping;
	bool _shapeIsDynamicallyStatic;
	bool _shapeIsDynamicallyRespondable;
	bool _shapeIsStaticAndNotRespondableButDynamicTag; // needed when a dynamic joint connects to a static non respondable shape
	bool _parentFollowsDynamic;
	bool _containsTransparentComponents; // to be able to order shapes according to transparency
	bool _setAutomaticallyToNonStaticIfGetsParent;

	// other variables:
	C3Vector _dynamicLinearVelocity;
	C3Vector _dynamicAngularVelocity;
	C3Vector _additionalForce;
	C3Vector _additionalTorque;
	C3Vector _initialDynamicVelocity;
	C3Vector _initialDynamicAngularVelocity;

	bool _rigidBodyWasAlreadyPutToSleepOnce;

	bool _initialValuesInitialized;
};
