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

#include "simpleFilter.h"
#include "DrawingContainer2D.h"
#include <vector>
#include "Ser.h"

class CComposedFilter
{
public:

	CComposedFilter();
	virtual ~CComposedFilter();

	CComposedFilter* copyYourself();
	void serialize(CSer& ar);
	int getSimpleFilterCount();
	void insertSimpleFilter(CSimpleFilter* it);
	CSimpleFilter* getSimpleFilter(int index);
	CSimpleFilter* getSimpleFilterFromUniqueID(int uniqueID);
	void removeAllSimpleFilters();
	void removeSimpleFilter(int index);
	bool moveSimpleFilter(int index,bool up);
	bool processAndTrigger(CVisionSensor* sensor,int sizeX,int sizeY,const float* inputImage,const float* inputDepth,float* outputImage,std::vector<std::vector<float> >& returnData);
	void displayOverlay(int c0[2],int c1[2]);
	void initializeInitialValues(bool simulationIsRunning);
	void simulationEnded();
	void removeBuffers();

	CDrawingContainer2D drawingContainer;

protected:
	// Variables which need to be serialized & copied 
	std::vector<CSimpleFilter*> _allSimpleFilters;
	float* buffer1;
	float* buffer2;
	float* workImage;
};
