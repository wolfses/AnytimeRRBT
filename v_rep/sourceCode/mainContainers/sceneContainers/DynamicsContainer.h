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

#include "MainCont.h"
#include "3Vector.h"
#include "Ser.h"
#include "VisualParam.h"

class CDynamicsContainer : public CMainCont 
{
public:
	CDynamicsContainer();
	virtual ~CDynamicsContainer();
	void serialize(CSer& ar);
	void simulationAboutToStart();
	void simulationEnded();
	void renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib);
	void renderYour3DStuff_overlay(CViewableBase* renderingObject,int displayAttrib);

	void handleDynamics(float dt);
	bool getContactForce(int dynamicPass,int objectHandle,int index,int objectHandles[2],float contactInfo[6]);

	void reportDynamicWorldConfiguration();

	void addWorldIfNotThere();
	void removeWorld();
	bool isWorldThere();

	void markForWarningDisplay_pureSpheroidNotSupported();
	void markForWarningDisplay_pureConeNotSupported();
	void markForWarningDisplay_containsNonPureNonConvexShapes();
	void markForWarningDisplay_containsStaticShapesOnDynamicConstruction();
	void markForWarningDisplay_pureHollowShapeNotSupported();
	void markForWarningDisplay_physicsEngineNotSupported();
	void markForWarningDisplay_vortexPluginIsDemo();

	void displayWarningsIfNeeded();
	bool displayNonDefaultParameterWarningRequired();
	bool displayNonPureNonConvexShapeWarningRequired();
	bool displayStaticShapeOnDynamicConstructionWarningRequired();
	bool displayVortexPluginIsDemoRequired();

	void setDynamicEngineType(int t);
	int getDynamicEngineType();
	void setDynamicsStepSize(float f,bool setDirect);
	float getDynamicsStepSize();
	void setDynamicsEnabled(bool e);
	bool getDynamicsEnabled();
	void setGravity(const C3Vector& gr);
	C3Vector getGravity();

	void setUseDynamicDefaultCalculationParameters(int defType); // 0=default very precise, 1=default precise, 2=default fast, 3=default very fast, 4=custom
	int getUseDynamicDefaultCalculationParameters();

	void setDynamicInternalScalingFactor(float f,bool setDirect);
	float getDynamicInternalScalingFactor();
	void setBulletDynamicFullInternalScaling(bool b,bool setDirect);
	bool getBulletDynamicFullInternalScaling();

	void setBulletDynamicCollisionMarginFactor(float f,bool setDirect);
	float getBulletDynamicCollisionMarginFactor();

	void setODEDynamicInternalScalingFactor(float f,bool setDirect);
	float getODEDynamicInternalScalingFactor();
	void setODEDynamicConstraintSolvingIterations(int iterations,bool setDirect);
	int getODEDynamicConstraintSolvingIterations();
	void setODEDynamicsStepSize(float f,bool setDirect);
	float getODEDynamicsStepSize();
	void setODEUseQuickStep(bool u,bool setDirect);
	bool getODEUseQuickStep();
	void setODEDynamicFullInternalScaling(bool b,bool setDirect);
	bool getODEDynamicFullInternalScaling();

	void setODEDynamicGlobalCFM(float p,bool setDirect);
	float getODEDynamicGlobalCFM();
	void setODEDynamicGlobalERP(float p,bool setDirect);
	float getODEDynamicGlobalERP();


	void getVortexFloatParams(std::vector<float>& p);
	void setVortexFloatParams(const std::vector<float>& p,bool setDirect);
	void getVortexIntParams(std::vector<int>& p);
	void setVortexIntParams(const std::vector<int>& p,bool setDirect);

	void getVortexDefaultFloatParams(std::vector<float>& p,int defType);
	void getVortexDefaultIntParams(std::vector<int>& p,int defType);


	float getPositionScalingFactorDyn();
	float getLinearVelocityScalingFactorDyn();
	float getMassScalingFactorDyn();
	float getMasslessInertiaScalingFactorDyn();
	float getForceScalingFactorDyn();
	float getTorqueScalingFactorDyn();
	float getGravityScalingFactorDyn();

	void setDynamicConstraintSolvingIterations(int iterations,bool setDirect);
	int getDynamicConstraintSolvingIterations();
	void setDisplayContactPoints(bool d);
	bool getDisplayContactPoints();



	CVisualParam contactPointColor;

protected:
	void _renderParticle(int displayAttrib,const C4X4Matrix& cameraCTM,const C3Vector& _currentPosition,float _size,int _objectType,const float* _additionalColor);
	void _drawPoints(int displayAttrib,const C4X4Matrix& cameraRTM,const C3Vector& _currentPosition,float _size,int _objectType,const float* _additionalColor);
	void _drawRoughSphere(int displayAttrib,const C3Vector& _currentPosition,float _size,int _objectType,const float* _additionalColor);
	void _drawSphere(int displayAttrib,const C3Vector& _currentPosition,float _size,int _objectType,const float* _additionalColor);

	void _resetWarningFlags();

	unsigned char _pureSpheroidNotSupportedMark;
	unsigned char _pureConeNotSupportedMark;
	unsigned char _containsNonPureNonConvexShapes;
	unsigned char _containsStaticShapesOnDynamicConstruction;
	unsigned char _pureHollowShapeNotSupportedMark;
	unsigned char _physicsEngineNotSupportedWarning;
	unsigned char _nonDefaultEngineSettingsWarning;
	unsigned char _vortexPluginIsDemoWarning;


	// To serialize:
	bool _dynamicsEnabled;
	int _dynamicEngineToUse;
	C3Vector _gravity;
	int _dynamicDefaultTypeCalculationParameters;
	bool _displayContactPoints;

	float _dynamicsBULLETStepSize;
	bool _dynamicBULLETFullInternalScaling;
	float _dynamicBULLETInternalScalingFactor;
	int _dynamicBULLETConstraintSolvingIterations;
	float _dynamicBULLETCollisionMarginFactor;

	float _dynamicsODEStepSize;
	float _dynamicODEInternalScalingFactor;
	int _dynamicODEConstraintSolvingIterations;
	bool _dynamicODEFullInternalScaling;
	bool _dynamicODEUseQuickStep;
	float _dynamicODEGlobalCFM;
	float _dynamicODEGlobalERP;

	std::vector<float> _vortexFloatParams;
	std::vector<int> _vortexIntParams;
};
