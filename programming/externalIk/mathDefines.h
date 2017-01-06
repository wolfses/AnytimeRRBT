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
#include <limits>
#include <float.h>
#include <cstdlib>

#ifdef MAC_VREP
#include <boost/math/special_functions/fpclassify.hpp>
#endif

#define piValue 3.14159265359f
#define piValTimes2 6.28318530718f
#define radToDeg 57.2957795130785499f
#define degToRad 0.017453292519944444f
#define SIM_MAX_FLOAT (0.01f*FLT_MAX)
#define SIM_MAX_DOUBLE (0.01f*DBL_MAX)
#define SIM_MAX_INT INT_MAX
#define SIM_RAND_FLOAT (rand()/(float)RAND_MAX)

#ifdef WIN_VREP
	#define VREP_IS_NAN(x) (_isnan(x))
	#define VREP_IS_FINITE(x) (_finite(x))
#endif

#ifdef MAC_VREP
	#define VREP_IS_NAN(x) ((boost::math::isnan)(x))
	#define VREP_IS_FINITE(x) (finite(x))
#endif

#ifdef LIN_VREP
	#define VREP_IS_NAN(x) (isnan(x))
	#define VREP_IS_FINITE(x)                         \
			(sizeof (x) == sizeof (float)             \
			? __finitef (x)                           \
			: sizeof (x) == sizeof (double)           \
			? __finite (x) : __finitel (x))
#endif

