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
#include "Ser.h"

class CViewableBase : public C3DObject
{
public:
	CViewableBase();
	virtual ~CViewableBase();

	// Following functions need to be implemented in each class derived from CViewableBase
	virtual C3DObject* copyYourself();
	virtual void display(CViewableBase* renderingObject,int displayAttrib);
	virtual void scaleObject(float scalingFactor);
	virtual void serialize(CSer& ar);
	virtual bool announceObjectWillBeErased(int objID,bool copyBuffer);
	virtual void announceGroupWillBeErased(int groupID,bool copyBuffer);
	virtual void announceCollisionWillBeErased(int collisionID,bool copyBuffer);
	virtual void announceDistanceWillBeErased(int distanceID,bool copyBuffer);
	virtual void performObjectLoadingMapping(std::vector<int>* map);
	virtual void performGroupLoadingMapping(std::vector<int>* map);
	virtual void performCollisionLoadingMapping(std::vector<int>* map);
	virtual void performDistanceLoadingMapping(std::vector<int>* map);
	virtual void simulationAboutToStart();
	virtual void simulationEnded();

	virtual std::string getObjectTypeInfo();
	virtual std::string getObjectTypeInfoExtended();


	void setNearClippingPlane(float nearPlane);
	float getNearClippingPlane();
	void setFarClippingPlane(float farPlane);
	float getFarClippingPlane();
	void setViewAngle(float angle);
	float getViewAngle();
	void setOrthoViewSize(float theSize);
	float getOrthoViewSize();

	void setShowFogIfAvailable(bool showFog);
	bool getShowFogIfAvailable();

	bool isObjectInsideView(const C7Vector& objectM,const C3Vector& maxBB);
	void computeViewFrustumIfNeeded();

	void setFrustumCullingTemporarilyDisabled(bool d);

	void setFogTimer(float seconds);
	float getFogStrength();

	void setUseLocalLights(bool l);
	bool getuseLocalLights();

	int getDisabledColorComponents();
	void setDisabledColorComponents(int c);

	std::vector<float> viewFrustum;

	static bool fogWasActivated;

protected:
	// View frustum culling:
	bool _isBoxOutsideVolumeApprox(const C4X4Matrix& tr,const C3Vector& s,std::vector<float>* planes);
	
	float _nearClippingPlane;
	float _farClippingPlane;
	float _viewAngle;
	float _orthoViewSize;
	bool _showFogIfAvailable;
	bool _useLocalLights;

	bool _planesCalculated;
	int _currentViewSize[2];
	bool _currentPerspective;
	bool _frustumCullingTemporarilyDisabled;

	float _initialViewAngle;
	float _initialOrthoViewSize;

	int _fogTimer;
	float _fogTimerDuration;

	int _disabledColorComponents; // not serialized
};
