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
#include "BezierPathPoint.h"

CBezierPathPoint::CBezierPathPoint(const C7Vector& transf)
{
	_transformation=transf;

	cumulativeLength=0.0f;
	segmentLength=0.0f;
	virtualSegmentLength=0.0f;
	virtualCumulativeLength=0.0f;
	_maxRelAbsVelocity=1.0f;
	_onSpotDistance=0.0f;
	cumulativeOnSpotDistance=0.0f;
	segmentOnSpotDistance=0.0f;

}

CBezierPathPoint::~CBezierPathPoint()
{
}

CBezierPathPoint* CBezierPathPoint::copyYourself()
{ // Everything is copied.
	CBezierPathPoint* newPoint=new CBezierPathPoint(_transformation);

	newPoint->cumulativeLength=cumulativeLength;
	newPoint->segmentLength=segmentLength;

	newPoint->cumulativeAngle=cumulativeAngle;
	newPoint->segmentAngleVariation=segmentAngleVariation;

	newPoint->cumulativeOnSpotDistance=cumulativeOnSpotDistance;
	newPoint->segmentOnSpotDistance=segmentOnSpotDistance;

	newPoint->virtualCumulativeLength=virtualCumulativeLength;
	newPoint->virtualSegmentLength=virtualSegmentLength;

	newPoint->_maxRelAbsVelocity=_maxRelAbsVelocity;
	newPoint->_onSpotDistance=_onSpotDistance;

	newPoint->_auxFlags=_auxFlags;
	for (int i=0;i<4;i++)
		newPoint->_auxChannels[i]=_auxChannels[i];

	return(newPoint);
}
