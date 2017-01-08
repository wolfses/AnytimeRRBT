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

#include "ViewableBase.h"
#include "SView.h"
#include "composedFilter.h"
#include "visionSensorGlStuff.h"

struct SHandlingResult
{
	bool sensorWasTriggered;
	bool sensorResultIsValid;
	float sensorDataRed[3]; // min, max, average
	float sensorDataGreen[3]; // min, max, average
	float sensorDataBlue[3]; // min, max, average
	float sensorDataIntensity[3]; // min, max, average
	float sensorDataDepth[3]; // min, max, average
	int calcTimeInMs;
};

class CVisionSensor : public CViewableBase  
{
public:
	CVisionSensor();
	virtual ~CVisionSensor();

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

	void commonInit();

	void getRealResolution(int r[2]);
	void setDesiredResolution(int r[2]);
	void getDesiredResolution(int r[2]);

	void setSize(const C3Vector& s);
	C3Vector getSize();
	void setPerspectiveOperation(bool p);
	bool getPerspectiveOperation();
	void setExplicitHandling(bool explicitHandl);
	bool getExplicitHandling();
	void setShowVolumeWhenNotDetecting(bool s);
	bool getShowVolumeWhenNotDetecting();
	void setShowVolumeWhenDetecting(bool s);
	bool getShowVolumeWhenDetecting();
	void resetSensor();
	bool handleSensor();
	bool checkSensor(int entityID,bool overrideRenderableFlagsForNonCollections);
	float* checkSensorEx(int entityID,bool imageBuffer,bool entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,bool hideEdgesIfModel,bool overrideRenderableFlagsForNonCollections);
	bool setExternalImage(const float* img);

	void setIgnoreRGBInfo(bool ignore);
	bool getIgnoreRGBInfo();
	void setIgnoreDepthInfo(bool ignore);
	bool getIgnoreDepthInfo();
	void setColorCodeId(bool codeId);
	bool getColorCodeId();


	void getSensingVolumeCorners(C3Vector& sizeAndPosClose,C3Vector& sizeAndPosFar);
	void setDetectableEntityID(int entityID);
	int getDetectableEntityID();
	void lookAt(CSView* viewObject,int viewPos[2]=NULL,int viewSize[2]=NULL);

	bool detectEntity(int entityID,bool detectAll,bool dontSwapImageBuffers,bool entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,bool hideEdgesIfModel,bool overrideRenderableFlagsForNonCollections);
	bool detectEntity2(int entityID,bool detectAll,bool dontSwapImageBuffers,bool entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,bool hideEdgesIfModel,bool overrideRenderableFlagsForNonCollections);
	void renderForDetection(int entityID,bool detectAll,bool entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,bool hideEdgesIfModel,bool overrideRenderableFlagsForNonCollections,const std::vector<int>& activeMirrors);
	void setDefaultBufferValues(const float v[3]);
	void getDefaultBufferValues(float v[3]);

	void setUseExternalImage(bool u);
	bool getUseExternalImage();

	float* readPortionOfImage(int posX,int posY,int sizeX,int sizeY,bool rgbOtherwiseDepth);
	void setUseEnvironmentBackgroundColor(bool s);
	bool getUseEnvironmentBackgroundColor();
	float getCalculationTime();
	CTextureObject* getTextureObject();

	bool applyFilter();
	void swapImageBuffers();
	CComposedFilter* getComposedFilter();
	void setComposedFilter(CComposedFilter* newFilter);
	void createGlContextAndFboAndTextureObjectIfNeeded(bool useStencilBuffer);

	// Variables which need to be serialized & copied (don't forget the vars from the CViewableBase class!)
	CVisualParam bodyColor;
	CVisualParam bodyActiveColor;
	CVisualParam detectionVolumeColor;
	CVisualParam detectionVolumeActiveColor;

	float* _previousImageBuffer;
	float* _imageBuffer;
	float* _depthBuffer;

	SHandlingResult sensorResult;
	std::vector<std::vector<float> > sensorAuxiliaryResult; // e.g. vectors, etc. set by a filter or an extension module's filter

protected:
	void _drawObjects(int entityID,bool detectAll,bool entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,bool hideEdgesIfModel,bool overrideRenderableFlagsForNonCollections);
	int _getActiveMirrors(int entityID,bool detectAll,bool entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,bool overrideRenderableFlagsForNonCollections,std::vector<int>& activeMirrors);
	void _reserveBuffers();
	void _clearBuffers();
	void _getDefaultReturnValues();
	void _removeGlContextAndFboAndTextureObjectIfNeeded();

	unsigned char* _rgbBuffer;

	CVisionSensorGlStuff* _contextFboAndTexture;

	// Variables which need to be serialized & copied (don't forget the vars from the CViewableBase class!)
	int _resolutionX;
	int _resolutionY;
	int _desiredResolution[2];
	int _detectableEntityID;
	C3Vector _size;
	bool _perspectiveOperation;
	bool _explicitHandling;
	bool _showVolumeWhenNotDetecting;
	bool _showVolumeWhenDetecting;
	bool _useExternalImage;
	bool _useSameBackgroundAsEnvironment;
	float _defaultBufferValues[3];
	bool _ignoreRGBInfo;
	bool _ignoreDepthInfo;
	bool _colorCodeId;

	CComposedFilter* _composedFilter;

	// Other variables:
	bool _initialValuesInitialized;
	bool _initialExplicitHandling;
};
