// This file is part of the EXTERNAL IK for V-REP
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The EXTERNAL IK is licensed under the terms of EITHER:
//   1. EXTERNAL IK commercial license (contact us for details)
//   2. EXTERNAL IK educational license (see below)
// 
// EXTERNAL IK educational license:
// -------------------------------------------------------------------
// The EXTERNAL IK educational license applies only to EDUCATIONAL
// ENTITIES composed by following people and institutions:
// 
// 1. Hobbyists, students, teachers and professors
// 2. Schools and universities
// 
// EDUCATIONAL ENTITIES do NOT include companies, research institutions,
// non-profit organisations, foundations, etc.
// 
// An EDUCATIONAL ENTITY may use, modify, compile and distribute the
// modified/unmodified EXTERNAL IK under following conditions:
//  
// 1. Distribution should be free of charge.
// 2. Distribution should be to EDUCATIONAL ENTITIES only.
// 3. Usage should be non-commercial.
// 4. Altered source versions must be plainly marked as such and distributed
//    along with any compiled code.
// 5. The origin of the EXTERNAL IK must not be misrepresented. you must
//    not claim that you wrote the original software.
// 
// THE EXTERNAL IK IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#pragma once

#include "mathDefines.h"
#include "extIk.h"
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


