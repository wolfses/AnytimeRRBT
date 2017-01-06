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
#include "3X3Matrix.h"
#include "4X4Matrix.h"
#include "6X6Matrix.h"
#include "Vector.h"

class CMatrix  
{
public:
	CMatrix();
	CMatrix(int nRows,int nCols);
	CMatrix(const C3X3Matrix& m);
	CMatrix(const C4X4Matrix& m);
	CMatrix(const C6X6Matrix& m);
	CMatrix(const CMatrix& m);
	~CMatrix();

	bool inverse();
	void transpose();
	void clear();
	void setIdentity();

	CMatrix operator* (const C3X3Matrix& m) const;
	CMatrix operator* (const C4X4Matrix& m) const;
	CMatrix operator* (const C6X6Matrix& m) const;
	CMatrix operator* (const CMatrix& m) const;
	CMatrix operator* (float d) const;
	CMatrix operator/ (float d) const;
	CVector operator* (const CVector& v) const;
	CMatrix operator+ (const CMatrix& m) const;
	CMatrix operator- (const CMatrix& m) const;
	
	void operator*= (const CMatrix& m);
	void operator+= (const CMatrix& m);
	void operator-= (const CMatrix& m);
	void operator*= (float d);
	void operator/= (float d);

	CMatrix& operator= (const C3X3Matrix& m);
	CMatrix& operator= (const C4X4Matrix& m);
	CMatrix& operator= (const C6X6Matrix& m);
	CMatrix& operator= (const CMatrix& m);

inline float& operator() (int row,int col)
{
	return(data[row*cols+col]);
}

inline float getAt(int row,int col) const
{
	return(data[row*cols+col]);
}

inline void setAt(int row,int col,float value)
{
	data[row*cols+col]=value;
}
 
inline const float& operator() (int row,int col) const
{
	return(data[row*cols+col]);
}


	int rows,cols;
	float* data;
};


