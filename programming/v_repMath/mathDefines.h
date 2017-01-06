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
