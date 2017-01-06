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

#include "vrepPrecompiledHeader.h"
#include "PathPoint.h"
#include "v_rep_internal.h"
#include "Tt.h"

CPathPoint::CPathPoint()
{
}

CPathPoint::~CPathPoint()
{
}

void CPathPoint::setTransformation(const C7Vector& tr,int attributes)
{
	_transformation=tr;
	if ((attributes&sim_pathproperty_flat_path)!=0)
		_transformation.X(2)=0.0f;
}

C7Vector CPathPoint::getTransformation()
{
	return(_transformation);
}

void CPathPoint::setMaxRelAbsVelocity(float t)
{
	tt::limitValue(0.0f,1.0f,t);
	_maxRelAbsVelocity=t;
}

float CPathPoint::getMaxRelAbsVelocity()
{
	return(_maxRelAbsVelocity);
}

void CPathPoint::setOnSpotDistance(float d)
{
	tt::limitValue(0.0f,1000.0f,d);
	_onSpotDistance=d;
}

float CPathPoint::getOnSpotDistance()
{
	return(_onSpotDistance);
}

void CPathPoint::setAuxFlags(WORD f)
{
	_auxFlags=f;
}

WORD CPathPoint::getAuxFlags()
{
	return(_auxFlags);
}

void CPathPoint::setAuxChannels(const float c[4])
{
	for (int i=0;i<4;i++)
		_auxChannels[i]=c[i];
}

void CPathPoint::getAuxChannels(float c[4])
{
	for (int i=0;i<4;i++)
		c[i]=_auxChannels[i];
}
