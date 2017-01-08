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
#include <stdint.h>

#ifdef COMPILING_EXTERNAL_DYN_DLL
#include "RigidBodyContainerDyn.h"
#else
#include "vrepConfig.h"
#endif

typedef uint64_t VOIDPTR_TO_INT_CAST;

enum { 
	ANNOUNCE_PURE_CONE_NOT_SUPPORTED=0,
	ANNOUNCE_PURE_SPHEROID_NOT_SUPPORTED,
	ANNOUNCE_CONTAINS_NON_PURE_NON_CONVEX_SHAPES,
	ANNOUNCE_CONTAINS_STATIC_SHAPES_ON_DYNAMIC_CONSTRUCTION,
	ANNOUNCE_PURE_HOLLOW_SHAPE_NOT_SUPPORTED,
	ANNOUNCE_VORTEX_PLUGIN_IS_DEMO
};

enum {
	DYNAMIC_SIMULATION_ICON_NONE								=0,
	DYNAMIC_SIMULATION_ICON_OBJECT_IS_DYNAMICALLY_SIMULATED,
	DYNAMIC_SIMULATION_ICON_OBJECT_IS_NOT_DYNAMICALLY_ENABLED,
	DYNAMIC_SIMULATION_ICON_FREE_TO_USE1,
	DYNAMIC_SIMULATION_ICON_FREE_TO_USE2,
	DYNAMIC_SIMULATION_ICON_FREE_TO_USE3,
};

enum {	OBJECT_DYNAMIC_PROPERTY_DYNAMIC		=1,
		OBJECT_DYNAMIC_PROPERTY_RESPONDABLE	=2,
};

#define VORTEX_PARAM_VERSION 2

#define VORTEX_JOINT_FLOAT_PARAM_CNT_0_OLD 47
#define VORTEX_JOINT_INT_PARAM_CNT_0_OLD 4
#define VORTEX_SHAPE_FLOAT_PARAM_CNT_0_OLD 32
#define VORTEX_SHAPE_INT_PARAM_CNT_0_OLD 8
#define VORTEX_FLOAT_PARAM_CNT_0_OLD 10
#define VORTEX_INT_PARAM_CNT_0_OLD 1

#define VORTEX_JOINT_FLOAT_PARAM_CNT_1_OLD 47
#define VORTEX_JOINT_INT_PARAM_CNT_1_OLD 4
#define VORTEX_SHAPE_FLOAT_PARAM_CNT_1_OLD 33
#define VORTEX_SHAPE_INT_PARAM_CNT_1_OLD 8
#define VORTEX_FLOAT_PARAM_CNT_1_OLD 10
#define VORTEX_INT_PARAM_CNT_1_OLD 1

#define VORTEX_JOINT_FLOAT_PARAM_CNT_2 47
#define VORTEX_JOINT_INT_PARAM_CNT_2 4
#define VORTEX_SHAPE_FLOAT_PARAM_CNT_2 36
#define VORTEX_SHAPE_INT_PARAM_CNT_2 8
#define VORTEX_FLOAT_PARAM_CNT_2 10
#define VORTEX_INT_PARAM_CNT_2 1


#ifndef COMPILING_EXTERNAL_DYN_DLL
typedef void (*ptr_startSimulation)(int engine,const float floatParams[50],const int intParams[50],const char boolParams[50]);
typedef void (*ptr_endSimulation)(void);
typedef void (*ptr_step)(float timeStep,float simulationTime);
typedef char (*ptr_isInitialized)(void);
typedef char (*ptr_isDynamicContentAvailable)(void);
typedef void (*ptr_serializeDynamicContent)(const char* filenameAndPath,int bulletSerializationBuffer);
typedef int (*ptr_addParticleObject)(int objectType,float size,float massOverVolume,const void* params,float lifeTime,int maxItemCount,const float* ambient,const float* diffuse,const float* specular,const float* emission);
typedef char (*ptr_removeParticleObject)(int objectHandle);
typedef char (*ptr_addParticleObjectItem)(int objectHandle,const float* itemData,float simulationTime);
typedef int (*ptr_getParticleObjectOtherFloatsPerItem)(int objectHandle);
typedef float* (*ptr_getContactPoints)(int* count);
typedef void** (*ptr_getParticles)(int index,int* particlesCount,int* objectType,float** cols);
typedef char (*ptr_getParticleData)(const void* particle,float* pos,float* size,int* objectType,float** additionalColor);
typedef char (*ptr_getContactForce)(int dynamicPass,int objectHandle,int index,int objectHandles[2],float contactInfo[6]);
typedef void (*ptr_reportDynamicWorldConfiguration)(int totalPassesCount,char doNotApplyJointIntrinsicPositions,float simulationTime);
typedef int (*ptr_getDynamicStepDivider)(void);
typedef int (*ptr_getEngineInfo)(int engine,int* data1,char* data2,char* data3);

#endif

// FULLY STATIC CLASS
class CDynInterface  
{
public:
#ifndef COMPILING_EXTERNAL_DYN_DLL
	static bool initializeFunctionsIfNeeded();
	static void setSystemPluginsLoadPhaseOver();
#endif

	static void startSimulation(int engine,const float floatParams[50],const int intParams[50],const char boolParams[50]);
	static void endSimulation();
	static void step(float timeStep,float simulationTime);
	static char isInitialized();
	static char isDynamicContentAvailable();
	static void serializeDynamicContent(const char* filenameAndPath,int bulletSerializationBuffer);
	static int addParticleObject(int objectType,float size,float massOverVolume,const void* params,float lifeTime,int maxItemCount,const float* ambient,const float* diffuse,const float* specular,const float* emission);
	static char removeParticleObject(int objectHandle);
	static char addParticleObjectItem(int objectHandle,const float* itemData,float simulationTime);
	static int getParticleObjectOtherFloatsPerItem(int objectHandle);
	static float* getContactPoints(int* count);
	static void** getParticles(int index,int* particlesCount,int* objectType,float** cols);
	static char getParticleData(const void* particle,float* pos,float* size,int* objectType,float** additionalColor);
	static char getContactForce(int dynamicPass,int objectHandle,int index,int objectHandles[2],float contactInfo[6]);
	static void reportDynamicWorldConfiguration(int totalPassesCount,char doNotApplyJointIntrinsicPositions,float simulationTime);
	static int getDynamicStepDivider();
	static int getEngineInfo(int engine,int* data1,char* data2,char* data3);

#ifndef COMPILING_EXTERNAL_DYN_DLL
	static ptr_startSimulation _startSimulation;
	static ptr_endSimulation _endSimulation;
	static ptr_step _step;
	static ptr_isInitialized _isInitialized;
	static ptr_isDynamicContentAvailable _isDynamicContentAvailable;
	static ptr_serializeDynamicContent _serializeDynamicContent;
	static ptr_addParticleObject _addParticleObject;
	static ptr_removeParticleObject _removeParticleObject;
	static ptr_addParticleObjectItem _addParticleObjectItem;
	static ptr_getParticleObjectOtherFloatsPerItem _getParticleObjectOtherFloatsPerItem;
	static ptr_getContactPoints _getContactPoints;
	static ptr_getParticles _getParticles;
	static ptr_getParticleData _getParticleData;
	static ptr_getContactForce _getContactForce;
	static ptr_reportDynamicWorldConfiguration _reportDynamicWorldConfiguration;
	static ptr_getDynamicStepDivider _getDynamicStepDivider;
	static ptr_getEngineInfo _getEngineInfo;

	static bool _dynInterfaceInitialized;
	static bool _dynSystemPluginsLoadPhaseOver;
#endif

#ifdef COMPILING_EXTERNAL_DYN_DLL

	static int getEngine();

	static float getBulletDynamicsStepSize();
	static float getBulletDynamicCollisionMarginFactor();
	static int getBulletDynamicConstraintSolvingIterations();

	static int getODEDynamicConstraintSolvingIterations();
	static float getODEDynamicsStepSize();
	static bool getODEUseQuickStep();
	static float getODEDynamicGlobalCFM();
	static float getODEDynamicGlobalERP();

	static float getVortexDynamicsStepSize();

	static float getPositionScalingFactorDyn();
	static float getLinearVelocityScalingFactorDyn();
	static float getMassScalingFactorDyn();
	static float getMasslessInertiaScalingFactorDyn();
	static float getForceScalingFactorDyn();
	static float getTorqueScalingFactorDyn();
	static float getGravityScalingFactorDyn();

	static float getDynamicsInternalTimeStep();
	static void setDynamicsInternalTimeStep(float dt);

	static int getDynamicParticlesIdStart();
	static int get3dObjectIdStart();
	static int get3dObjectIdEnd();

	static float getDynamicActivityRange();

private:
	static int _dynamicEngineToUse;

	static float _dynamicsBulletStepSize;
	static int _dynamicBulletConstraintSolvingIterations;
	static float _dynamicBulletCollisionMarginFactor;

	static float _dynamicsODEStepSize;
	static int _dynamicODEConstraintSolvingIterations;
	static bool _dynamicODEUseQuickStep;
	static float _dynamicODEGlobalCFM;
	static float _dynamicODEGlobalERP;

	static float _dynamicsVortexStepSize;

	static float _positionScalingFactorDyn;
	static float _linearVelocityScalingFactorDyn;
	static float _massScalingFactorDyn;
	static float _masslessInertiaScalingFactorDyn;
	static float _forceScalingFactorDyn;
	static float _torqueScalingFactorDyn;
	static float _gravityScalingFactorDyn;

	static float _dynamicsInternalTimeStep;

	static int _dynamicParticlesIdStart;
	static int _3dobjectIdStart;
	static int _3dObjectIdEnd;

	static float _dynamicActivityRange;

	static CRigidBodyContainerDyn* _dynamicsContainer;
#endif 
};
