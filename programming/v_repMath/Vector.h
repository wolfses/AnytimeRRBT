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
#include "3Vector.h"
#include "4Vector.h"
#include "6Vector.h"
#include "7Vector.h"

class CVector  
{
public:
	CVector();
	CVector(int nElements);
	CVector(const C3Vector& v);
	CVector(const C4Vector& v);
	CVector(const C6Vector& v);
	CVector(const C7Vector& v);
	CVector(const CVector& v);
	~CVector();

	CVector operator* (float d) const;
	CVector operator/ (float d) const;
	CVector operator+ (const CVector& v) const;
	CVector operator- (const CVector& v) const;

	void operator*= (float d);
	void operator/= (float d);
	void operator+= (const CVector& v);
	void operator-= (const CVector& v);
	
	float operator* (const C3Vector& v) const;
	float operator* (const C4Vector& v) const;
	float operator* (const C6Vector& v) const;
	float operator* (const C7Vector& v) const;
	float operator* (const CVector& v) const;

	CVector& operator= (const C3Vector& v);
	CVector& operator= (const C4Vector& v);
	CVector& operator= (const C6Vector& v);
	CVector& operator= (const C7Vector& v);
	CVector& operator= (const CVector& v);

inline float& operator() (int i)
{
	return(data[i]);
}

inline const float& operator() (int i) const
{
	return(data[i]);
}

	int elements;
private:
	float* data;
};

