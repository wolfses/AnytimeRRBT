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

class CMirror : public C3DObject
{
public:
	CMirror();
	virtual ~CMirror();

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

	void setMirrorWidth(float w);
	float getMirrorWidth();
	void setMirrorHeight(float h);
	float getMirrorHeight();
	void setReflectance(float r);
	float getReflectance();

	void setActive(bool a);
	bool getActive();
	void setIsMirror(bool m);
	bool getIsMirror();

	void setClippingObjectOrCollection(int co);
	int getClippingObjectOrCollection();

	bool getContainsTransparentComponent();

	// Variables which need to be serialized & copied
	float mirrorColor[3];
	CVisualParam clipPlaneColor;
	static int currentMirrorContentBeingRendered;

protected:
	void _commonInit();

	// Variables which need to be serialized & copied
	float _mirrorWidth;
	float _mirrorHeight;
	float _mirrorReflectance;
	bool _active;
	bool _isMirror;
	int _clippingObjectOrCollection;

	// Variables which do not need to be serialized
	bool _initialValuesInitialized;
	bool _initialMirrorActive;
};
