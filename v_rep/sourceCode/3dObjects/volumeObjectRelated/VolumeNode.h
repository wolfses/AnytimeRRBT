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
#include <vector>
#include "Ser.h"

class CVolumeNode
{
public:
	CVolumeNode();
	CVolumeNode(float size,float resolution);
	virtual ~CVolumeNode();

	// Following functions are inherited from 3DObject
	void display(float currentSize,float visualResolution,const C3Vector& currentPosition,const std::vector<float>& viewFrustum);
	void displayWireframe(bool first,float currentSize,float visualResolution,const C3Vector& currentPosition);
	CVolumeNode* copyYourself();
	void serialize(CSer& ar,int version);
	void getVisualBoundingBox(bool& first,float currentSize,const C3Vector& currentPosition,int level,C3Vector& minV,C3Vector& maxV);
	bool cutVolume(float currentSize,float resolution,const C3Vector& currentPosition,const std::vector<float>& planes,char& somethingCut);

protected:
	void _commonInit();
	int _getBoxSituationRelativeToVolume(float boxHalfSize,const C3Vector& boxPosition,const std::vector<float>& planes);
	inline int _getSphereSituationRelativeToVolume(float sphereRadius,const C3Vector& spherePosition,const std::vector<float>& planes,C3Vector& normalVect)
	{
		// 'planes' contain a collection of plane definitions:
		// Each plane is defined by 4 values a, b, c & d (consecutive in the array):
		// ax+by+cz+d=0
		// The normal vector for each plane (a,b,c) should point outside of the volume
		// The volume has to be convex (but not obligatory closed)
		// The routine returns -1 if the sphere is definitively outside, 0 if we don't know and 1 if definitively inside
		int insideCount=0;
		for (int i=0;i<int(planes.size())/4;i++)
		{
			C3Vector abc(&planes[4*i+0]);
			float d=planes[4*i+3];
			float tmp=(abc*spherePosition+d);
			if (tmp>=sphereRadius)
			{
				normalVect(0)=-abc(0);
				normalVect(1)=-abc(1);
				normalVect(2)=-abc(2);
				return(-1);// definitively outside
			}
			else
			{
				if (tmp<=-sphereRadius)
					insideCount++;
			}
		}
		if (insideCount==int(planes.size())/4)
			return(1); // definitively inside!
		return(0); // We don't know!
	}

	inline int _getSphereSituationRelativeToViewFrustum(float sphereRadius,const C3Vector& spherePosition,const std::vector<float>& planes,float& cumulativeDist)
	{
		// 'planes' contain a collection of 6 plane definitions. 4 are side planes, 2 are near and far clipping
		// Each plane is defined by 4 values a, b, c & d (consecutive in the array):
		// ax+by+cz+d=0
		// The normal vector for each plane (a,b,c) should point outside of the volume
		// The volume has to be convex (but not obligatory closed)
		// The routine returns -1 if the sphere is definitively outside, 0 if we don't know and 1 if definitively inside
		int insideCount=0;
		cumulativeDist=0.0f;
		for (int i=0;i<int(planes.size())/4;i++)
		{
			C3Vector abc(&planes[4*i+0]);
			float d=planes[4*i+3];
			float tmp=(abc*spherePosition+d);
			if (tmp>=sphereRadius)
				return(-1);// definitively outside
			else
			{
				if (tmp<=-sphereRadius)
				{
					insideCount++;
					if (i<4)
						cumulativeDist-=tmp;
				}
			}
		}
		if (insideCount==int(planes.size())/4)
			return(1); // definitively inside!
		return(0); // We don't know!
	}

	int _cutVolumeTest(int level,float currentSize,float resolution,const C3Vector& currentPosition,const std::vector<float>& planes);
	inline bool _isPointTouchingVolume(const C3Vector& p,const std::vector<float>* planes,C3Vector& normalVect)
	{ // normalVect can be null. 
		// planes contains a collection of plane definitions:
		// Each plane is defined by 4 values a, b, c & d (consecutive in the array):
		// ax+by+cz+d=0
		// The normal vector for each plane (a,b,c) should point outside of the volume
		// The volume has to be closed and convex
		if (planes->size()==0) 
			return(false);	
		for (int i=0;i<int(planes->size())/4;i++)
		{
			C3Vector abc(&(*planes)[4*i+0]);
			if ((abc*p+(*planes)[4*i+3])>=0.0f)
			{
				normalVect(0)=-abc(0);
				normalVect(1)=-abc(1);
				normalVect(2)=-abc(2);
				return(false);
			}
		}
		return(true);
	}

	CVolumeNode* _childVolumeNodes[2][2][2];
	C3Vector _normalVector;
};
