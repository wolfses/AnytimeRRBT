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
#include "ViewableBase.h"
#include "Global.h"
#include "Light.h"
#include "SView.h"

const float ORTHO_CAMERA_NEAR_CLIPPING_PLANE=0.0001f;
const float ORTHO_CAMERA_FAR_CLIPPING_PLANE=300.0f;

enum {	DEPTHPASS=0,
		RENDERPASS,
		PICKPASS,
		COLORCODEDPICKPASS
};

class CCamera : public CViewableBase  
{
public:
	CCamera();
	virtual ~CCamera();

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
	void simulationAboutToStart();
	void simulationEnded();
	void initializeInitialValues(bool simulationIsRunning);
	bool getFullBoundingBox(C3Vector& minV,C3Vector& maxV);
	bool getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV);
	bool getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices);
	void bufferMainDisplayStateVariables();
	void bufferedMainDisplayStateVariablesToDisplay();

	std::string getObjectTypeInfo();
	std::string getObjectTypeInfoExtended();

	void handleTrackingAndHeadAlwaysUp();
	void commonInit();
	int getViewOrientation();
	void setViewOrientation(int ori,bool setPositionAlso=false);
	void lookIn(int windowSize[2],CSView* subView,bool drawText=false,bool passiveSubView=true);
	void setCameraSize(float size);
	float getCameraSize();
	void setAllowPicking(bool a);
	bool getAllowPicking();
	void setTrackedObjectID(int trackedObjID);
	int getTrackedObjectID();
	void setUseParentObjectAsManipulationProxy(bool useParent);
	bool getUseParentObjectAsManipulationProxy();
	void setCameraManipulationModePermissions(int p); // bit coded: own x, own y, own z, full rotation, tilting, never tilting
	int getCameraManipulationModePermissions(); // bit coded: own x, own y, own z, full rotation, tilting, never tilting

	void shiftCameraInCameraManipulationMode(const C3Vector& newLocalPos);
	void rotateCameraInCameraManipulationMode(const C7Vector& newLocalConf);
	void tiltCameraInCameraManipulationMode(float tiltAmount);

	void backwardCompatibility20100713();

	void frameSceneOrSelectedObjects(float windowWidthByHeight,bool forPerspectiveProjection,std::vector<int>* selectedObjects,bool useSystemSelection,bool includeModelObjects,float scalingFactor);


	// Variables which need to be serialized & copied
	CVisualParam colorPart1; // Part1 color
	CVisualParam colorPart2; // Light color or part2 color

//	static bool openGlCameraViewCallbackEnabled;

protected:
	void drawObjects(int renderingMode,bool noSelection,int pass,int navigationMode,int currentWinSize[2],CSView* subView,bool mirrored);
	void drawOverlay(bool passiveView,bool drawText,bool displ_ref,int windowSize[2],CSView* subView);

	int getSingleHit(int hits,unsigned int selectBuff[],bool ignoreDepthBuffer,int& hitThatIgnoresTheSelectableFlag);
	int getSeveralHits(int hits,unsigned int selectBuff[],std::vector<int>& hitList);
	int handleHits(int hits,unsigned int selectBuff[]);
	void handleMouseUpHit(int hitId);

	void performDepthPerception(CSView* subView,bool isPerspective);
	void _handleBannerClick(int bannerID);

	// Variables which need to be serialized & copied (don't forget the vars from the CViewableBase class!)
	float cameraSize;
	bool _useParentObjectAsManipulationProxy;
	bool _allowPicking;
	int _cameraManipulationModePermissions;
	bool _backwardCompatibilityFlag20100713;
	
	// Various
	int trackedObjectIdentifier_NeverDirectlyTouch;
	int hitForMouseUpProcessing_minus2MeansIgnore;
	
	static unsigned int selectBuff[SELECTION_BUFFER_SIZE*4];	// For picking

	bool _initialValuesInitialized;
};
