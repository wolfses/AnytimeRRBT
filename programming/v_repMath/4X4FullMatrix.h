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
#include "4X4Matrix.h"
#include "3Vector.h"

class C4X4FullMatrix  
{
public:
	C4X4FullMatrix();	// Needed for serialization
	C4X4FullMatrix(const C4X4Matrix& m);
	C4X4FullMatrix(const C4X4FullMatrix& m);
	~C4X4FullMatrix();

	void invert();
	void clear();
	void setIdentity();
	void buildZRotation(float angle);
	void buildTranslation(float x, float y, float z);
	C3Vector getEulerAngles() const;

	C4X4FullMatrix operator* (const C4X4FullMatrix& m) const;
	C4X4FullMatrix operator* (float d) const;
	C4X4FullMatrix operator/ (float d) const;
	C4X4FullMatrix operator+ (const C4X4FullMatrix& m) const;
	C4X4FullMatrix operator- (const C4X4FullMatrix& m) const;
	
	void operator*= (const C4X4FullMatrix& m);
	void operator+= (const C4X4FullMatrix& m);
	void operator-= (const C4X4FullMatrix& m);
	void operator*= (float d);
	void operator/= (float d);

	C4X4FullMatrix& operator= (const C4X4Matrix& m);
	C4X4FullMatrix& operator= (const C4X4FullMatrix& m);

	inline float& operator() (int row,int col)
	{
		return(data[row][col]);
	}
	inline const float& operator() (int row,int col) const
	{
		return(data[row][col]);
	}
		
private:
	float data[4][4];
};

