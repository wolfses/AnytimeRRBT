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

