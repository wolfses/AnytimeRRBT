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

#include "mathDefines.h"
#include <vector>

#include "3Vector.h"
#include "4Vector.h"
#include "6Vector.h"
#include "7Vector.h"
#include "Vector.h"
#include "3X3Matrix.h"
#include "4X4Matrix.h"
#include "6X6Matrix.h"
#include "4X4FullMatrix.h"
#include "MMatrix.h"

class CMath  
{
public:
	CMath();
	virtual ~CMath();

	static void limitValue(float minValue,float maxValue,float &value);
	static void limitValue(int minValue,int maxValue,int &value);


	static float robustAsin(float v);
	static float robustAcos(float v);
	static float robustFmod(float v,float w);
	static double robustmod(double v,double w);
	static bool isFloatNumberOk(float v);
	static bool isDoubleNumberOk(double v);
};
