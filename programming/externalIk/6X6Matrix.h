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
#include "6Vector.h"
#include "3X3Matrix.h"

class C6X6Matrix  
{
public:
	C6X6Matrix();
	C6X6Matrix(const C6X6Matrix& m);
	C6X6Matrix(const C3X3Matrix& m00,const C3X3Matrix& m01,const C3X3Matrix& m10,const C3X3Matrix& m11);
	~C6X6Matrix();

	void clear();
	void setMultResult(const C6X6Matrix& m1,const C6X6Matrix& m2);
	void setIdentity();

	C6X6Matrix operator* (const C6X6Matrix& m) const;
	C6X6Matrix operator+ (const C6X6Matrix& m) const;
	C6X6Matrix operator- (const C6X6Matrix& m) const;

	void operator*= (const C6X6Matrix& m);
	void operator+= (const C6X6Matrix& m);
	void operator-= (const C6X6Matrix& m);

	C6Vector operator* (const C6Vector& v) const;
	C6X6Matrix& operator= (const C6X6Matrix& m);

	inline void getInternalData(float d[36]) const
	{
		M[0][0].getInternalData(d+0);
		M[0][1].getInternalData(d+9);
		M[1][0].getInternalData(d+18);
		M[1][1].getInternalData(d+27);
	}
	inline void setInternalData(const float d[36])
	{
		M[0][0].setInternalData(d+0);
		M[0][1].setInternalData(d+9);
		M[1][0].setInternalData(d+18);
		M[1][1].setInternalData(d+27);
	}
	inline float& operator() (unsigned i,unsigned j)
	{
		return(M[i/3][j/3](i%3,j%3));
	}
	inline const float& operator() (unsigned i,unsigned j) const
	{
		return(M[i/3][j/3](i%3,j%3));
	}

	C3X3Matrix M[2][2];
};

