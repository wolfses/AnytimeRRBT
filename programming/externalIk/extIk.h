// This file is part of the EXTERNAL IK for V-REP
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The EXTERNAL IK is licensed under the terms of EITHER:
//   1. EXTERNAL IK commercial license (contact us for details)
//   2. EXTERNAL IK educational license (see below)
// 
// EXTERNAL IK educational license:
// -------------------------------------------------------------------
// The EXTERNAL IK educational license applies only to EDUCATIONAL
// ENTITIES composed by following people and institutions:
// 
// 1. Hobbyists, students, teachers and professors
// 2. Schools and universities
// 
// EDUCATIONAL ENTITIES do NOT include companies, research institutions,
// non-profit organisations, foundations, etc.
// 
// An EDUCATIONAL ENTITY may use, modify, compile and distribute the
// modified/unmodified EXTERNAL IK under following conditions:
//  
// 1. Distribution should be free of charge.
// 2. Distribution should be to EDUCATIONAL ENTITIES only.
// 3. Usage should be non-commercial.
// 4. Altered source versions must be plainly marked as such and distributed
//    along with any compiled code.
// 5. The origin of the EXTERNAL IK must not be misrepresented. you must
//    not claim that you wrote the original software.
// 
// THE EXTERNAL IK IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#pragma once

#include <math.h>
#include <vector>
#include <string>
#include <float.h>
#include <limits>

#define VREP_INLINE inline
#define SIM_ASSERT(x)
#define SIM_IS_BIT_SET(var,bit) (((var) & (1<<(bit)))!=0)

#ifdef _WIN32
	#define VREP_IS_FINITE(x) (_finite(x))
	#define VREP_IS_NAN(x) (_isnan(x))
#endif // _WIN32

#ifdef __APPLE__
	#define VREP_IS_FINITE(x) (finite(x))
	#define VREP_IS_NAN(x) ((boost::math::isnan)(x))
#endif // __APPLE__

#ifdef __linux
	#define VREP_IS_NAN(x) (isnan(x))
	#define VREP_IS_FINITE(x)                           \
            (sizeof (x) == sizeof (float)                   \
            ? __finitef (x)                                 \
            : sizeof (x) == sizeof (double)                 \
            ? __finite (x) : __finitel (x))
#endif // __linux

bool simEmbLaunch();
bool simEmbShutDown();
int simEmbStart(unsigned char* data,int dataLength);

int simEmbGetObjectHandle(const char* objectName);
int simEmbGetObjectParent(int objectHandle);
int simEmbSetObjectParent(int objectHandle,int parentObjectHandle,bool keepInPlace);

int simEmbGetJointPosition(int jointHandle,float* position);
int simEmbSetJointPosition(int jointHandle,float position);
int simEmbGetJointTransformation(int jointHandle,float* position,float* quaternion);
int simEmbSetSphericalJointQuaternion(int jointHandle,const float* quaternion);
int simEmbSetJointMode(int jointHandle,int jointMode);

int simEmbGetIkGroupHandle(const char* ikGroupName);
int simEmbHandleIkGroup(int ikGroupHandle);
int simEmbSetIkGroupExplicitHandling(int ikGroupHandle,bool explicitHandling);
int simEmbSetIkGroupProperties(int ikGroupHandle,int resolutionMethod,int maxIterations,float damping);
int simEmbSetIkElementProperties(int ikGroupHandle,int tipDummyHandle,int constraints,const float* precision,const float* weight);

int simEmbGetObjectTransformation(int objectHandle,int relativeToObjectHandle,float* position,float* quaternion);
int simEmbSetObjectTransformation(int objectHandle,int relativeToObjectHandle,const float* position,const float* quaternion);
int simEmbQuaternionToEulerAngles(const float* quaternion,float* euler);
int simEmbEulerAnglesToQuaternion(const float* euler,float* quaternion);
int simEmbInvertTransformation(float* position,float* quaternion);
int simEmbMultiplyTransformations(const float* position1,const float* quaternion1,const float* position2,const float* quaternion2,float* positionOut,float* quaternionOut);
int simEmbInterpolateTransformations(const float* position1,const float* quaternion1,const float* position2,const float* quaternion2,float interpolFactor,float* positionOut,float* quaternionOut);
int simEmbMultTransformationWithVector(const float* position,const float* quaternion,float* vect);
int simEmbGetRotationAxis(const float* positionStart,const float* quaternionStart,const float* positionGoal,const float* quaternionGoal,float* axis,float* angle);
int simEmbRotateAroundAxis(const float* positionIn,const float* quaternionIn,const float* axisVector,const float* axisPosition,float angle,float* positionOut,float* quaternionOut);
