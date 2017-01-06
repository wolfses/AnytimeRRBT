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

#include "vrepPrecompiledHeader.h"
#include <vector>
#include "Ser.h"

class CStaticGraphCurve  
{
public:
	
	CStaticGraphCurve();
	CStaticGraphCurve(int type,std::vector<float>* xVals,std::vector<float>* yVals,std::vector<float>* zVals);
	virtual ~CStaticGraphCurve();

	// Various
	int getCurveType();
	void setLinkPoints(bool l);
	bool getLinkPoints();
	std::string getName();
	void setName(std::string theName);
	void setLabel(bool l);
	bool getLabel();
	void setCurveWidth(float w);
	float getCurveWidth();
	void setRelativeToWorld(bool r);
	bool getRelativeToWorld();

	void serialize(CSer& ar);
	CStaticGraphCurve* copyYourself();
	
	// Variables which need to be serialized & copied 
	std::vector <float> values;
	float color[3];

protected:
	// Variables which need to be serialized & copied 
	int _curveType;
	float _curveWidth; // for 3D curves only!
	bool _linkPoints;
	bool _label;
	bool _relativeToWorld;
	std::string _name;
};
