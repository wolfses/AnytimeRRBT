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
#include "3Vector.h"
#include "4Vector.h"

class C4X4Matrix; // Forward declaration

class C7Vector  
{
public:
	C7Vector();
	C7Vector(const C7Vector& v);
	C7Vector(const C4Vector& q);
	C7Vector(const C3Vector& x);
	C7Vector(const C4Vector& q,const C3Vector& x);
	C7Vector(const float m[4][4]);
	C7Vector(const C4X4Matrix& m);
	C7Vector(float angle,const C3Vector& pos,const C3Vector& dir);
	~C7Vector();

	void setIdentity();
	void set(float m[4][4]);
	void set(const C4X4Matrix& m);
	C4X4Matrix getMatrix() const;
	C7Vector getInverse() const;
	void setMultResult(const C7Vector& v1,const C7Vector& v2);
	void buildInterpolation(const C7Vector& fromThis,const C7Vector& toThat,float t);
	void inverse();
	void copyTo(float m[4][4]) const;
	C3Vector getAxis(int index) const;

	C7Vector operator* (const C7Vector& v) const;

	void operator*= (const C7Vector& v);

	C3Vector operator* (const C3Vector& v) const;
	C7Vector& operator= (const C7Vector& v);

	inline void getInternalData(float d[7]) const
	{
		X.getInternalData(d+0);
		Q.getInternalData(d+3);
	}
	inline void setInternalData(const float d[7])
	{
		X.setInternalData(d+0);
		Q.setInternalData(d+3);
	}
	inline bool operator!= (const C7Vector& v)
	{
		return( (Q!=v.Q)||(X!=v.X) );
	}
	inline float& operator() (unsigned i)
	{
		if (i<3)
			return(X(i));
		else
			return(Q(i-3));
	}
	inline const float& operator() (unsigned i) const
	{
		if (i<3)
			return(X(i));
		else
			return(Q(i-3));
	}

	static const C7Vector identityTransformation;
	
	C4Vector Q;
	C3Vector X;
};
