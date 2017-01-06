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
#include "7Vector.h"
#include "3X3Matrix.h"

class CMatrix;
class C4X4FullMatrix;

class C4X4Matrix  
{
public:

	C4X4Matrix();
	C4X4Matrix(const C4X4Matrix& m);
	C4X4Matrix(const CMatrix& m);
	C4X4Matrix(const float m[4][4]);
	C4X4Matrix(const C3X3Matrix& m,const C3Vector& x);
	C4X4Matrix(const C3Vector& x,const C3Vector& y,const C3Vector& z,const C3Vector& pos);
	C4X4Matrix(const C7Vector& transf);
	~C4X4Matrix();

	void setIdentity();
	void buildInterpolation(const C4X4Matrix& fromThis,const C4X4Matrix& toThat,float t);
	C7Vector getTransformation() const;
	void buildXRotation(float angle);
	void buildYRotation(float angle);
	void buildZRotation(float angle);
	void buildTranslation(float x,float y,float z);
	void rotateAroundX(float angle);
	void rotateAroundY(float angle);
	void rotateAroundZ(float angle);
	void translate(float x,float y,float z);

	C4X4Matrix operator* (const CMatrix& m) const;
	C4X4Matrix& operator= (const CMatrix& m);
	C4X4Matrix& operator= (const C4X4FullMatrix& m);

	inline void getInternalData(float d[12]) const
	{ // temporary routine, remove later
		X.getInternalData(d+0);
		M.getInternalData(d+3);
	}
	inline void setInternalData(const float d[12])
	{
		X.setInternalData(d+0);
		M.setInternalData(d+3);
	}
	inline void inverse()
	{
		// Speed optimized version:
		float tmp=M.axis[0](1);
		M.axis[0](1)=M.axis[1](0);
		M.axis[1](0)=tmp;;
		tmp=M.axis[0](2);
		M.axis[0](2)=M.axis[2](0);
		M.axis[2](0)=tmp;;
		tmp=M.axis[1](2);
		M.axis[1](2)=M.axis[2](1);
		M.axis[2](1)=tmp;;
		float v[3]={-X(0),-X(1),-X(2)};
		X(0)=M.axis[0](0)*v[0]+M.axis[1](0)*v[1]+M.axis[2](0)*v[2];
		X(1)=M.axis[0](1)*v[0]+M.axis[1](1)*v[1]+M.axis[2](1)*v[2];
		X(2)=M.axis[0](2)*v[0]+M.axis[1](2)*v[1]+M.axis[2](2)*v[2];
		// Normal version:
		//	(*this)=getInverse();
	}
	inline void copyTo(float m[4][4]) const
	{
		for (int i=0;i<3;i++)
		{
			for (int j=0;j<3;j++)
				m[i][j]=M(i,j);
			m[i][3]=X(i);
		}
		m[3][0]=0.0f;
		m[3][1]=0.0f;
		m[3][2]=0.0f;
		m[3][3]=1.0f;
	}
	inline void set(float m[4][4])
	{
		for (int i=0;i<3;i++)
		{
			for (int j=0;j<3;j++)
				M.axis[j](i)=m[i][j];
			X(i)=m[i][3];
		}
	}
	inline void copyToInterface(float* m) const
	{ // Temporary routine. Remove later!
		for (int i=0;i<3;i++)
		{
			m[4*i+0]=M.axis[0](i);
			m[4*i+1]=M.axis[1](i);
			m[4*i+2]=M.axis[2](i);
			m[4*i+3]=X(i);
		}
	}
	inline void copyFromInterface(const float* m)
	{ // Temporary routine. Remove later!
		for (int i=0;i<3;i++)
		{
			M.axis[0](i)=m[4*i+0];
			M.axis[1](i)=m[4*i+1];
			M.axis[2](i)=m[4*i+2];
			X(i)=m[4*i+3];
		}
	}
	inline bool isValid() const
	{
		if (!M.isValid())
			return(false);
		if (!X.isValid())
			return(false);
		return(true);
	}
	inline C4X4Matrix getInverse() const
	{
		C4X4Matrix retM(*this);
		retM.inverse();
		return(retM);
	}
	inline void setMultResult(const C4X4Matrix& m1,const C4X4Matrix& m2)
	{ // Use this routine to avoid temporary variables
		M.axis[0](0)=m1.M.axis[0](0)*m2.M.axis[0](0)+m1.M.axis[1](0)*m2.M.axis[0](1)+m1.M.axis[2](0)*m2.M.axis[0](2);
		M.axis[0](1)=m1.M.axis[0](1)*m2.M.axis[0](0)+m1.M.axis[1](1)*m2.M.axis[0](1)+m1.M.axis[2](1)*m2.M.axis[0](2);
		M.axis[0](2)=m1.M.axis[0](2)*m2.M.axis[0](0)+m1.M.axis[1](2)*m2.M.axis[0](1)+m1.M.axis[2](2)*m2.M.axis[0](2);
		M.axis[1](0)=m1.M.axis[0](0)*m2.M.axis[1](0)+m1.M.axis[1](0)*m2.M.axis[1](1)+m1.M.axis[2](0)*m2.M.axis[1](2);
		M.axis[1](1)=m1.M.axis[0](1)*m2.M.axis[1](0)+m1.M.axis[1](1)*m2.M.axis[1](1)+m1.M.axis[2](1)*m2.M.axis[1](2);
		M.axis[1](2)=m1.M.axis[0](2)*m2.M.axis[1](0)+m1.M.axis[1](2)*m2.M.axis[1](1)+m1.M.axis[2](2)*m2.M.axis[1](2);
		M.axis[2](0)=m1.M.axis[0](0)*m2.M.axis[2](0)+m1.M.axis[1](0)*m2.M.axis[2](1)+m1.M.axis[2](0)*m2.M.axis[2](2);
		M.axis[2](1)=m1.M.axis[0](1)*m2.M.axis[2](0)+m1.M.axis[1](1)*m2.M.axis[2](1)+m1.M.axis[2](1)*m2.M.axis[2](2);
		M.axis[2](2)=m1.M.axis[0](2)*m2.M.axis[2](0)+m1.M.axis[1](2)*m2.M.axis[2](1)+m1.M.axis[2](2)*m2.M.axis[2](2);
		X(0)=m1.M.axis[0](0)*m2.X(0)+m1.M.axis[1](0)*m2.X(1)+m1.M.axis[2](0)*m2.X(2)+m1.X(0);
		X(1)=m1.M.axis[0](1)*m2.X(0)+m1.M.axis[1](1)*m2.X(1)+m1.M.axis[2](1)*m2.X(2)+m1.X(1);
		X(2)=m1.M.axis[0](2)*m2.X(0)+m1.M.axis[1](2)*m2.X(1)+m1.M.axis[2](2)*m2.X(2)+m1.X(2);
	}
	inline C4X4Matrix operator* (const C4X4Matrix& m) const
	{	// Speed optimized version:
		C4X4Matrix retM;
		retM.M.axis[0](0)=M.axis[0](0)*m.M.axis[0](0)+M.axis[1](0)*m.M.axis[0](1)+M.axis[2](0)*m.M.axis[0](2);
		retM.M.axis[0](1)=M.axis[0](1)*m.M.axis[0](0)+M.axis[1](1)*m.M.axis[0](1)+M.axis[2](1)*m.M.axis[0](2);
		retM.M.axis[0](2)=M.axis[0](2)*m.M.axis[0](0)+M.axis[1](2)*m.M.axis[0](1)+M.axis[2](2)*m.M.axis[0](2);
		retM.M.axis[1](0)=M.axis[0](0)*m.M.axis[1](0)+M.axis[1](0)*m.M.axis[1](1)+M.axis[2](0)*m.M.axis[1](2);
		retM.M.axis[1](1)=M.axis[0](1)*m.M.axis[1](0)+M.axis[1](1)*m.M.axis[1](1)+M.axis[2](1)*m.M.axis[1](2);
		retM.M.axis[1](2)=M.axis[0](2)*m.M.axis[1](0)+M.axis[1](2)*m.M.axis[1](1)+M.axis[2](2)*m.M.axis[1](2);
		retM.M.axis[2](0)=M.axis[0](0)*m.M.axis[2](0)+M.axis[1](0)*m.M.axis[2](1)+M.axis[2](0)*m.M.axis[2](2);
		retM.M.axis[2](1)=M.axis[0](1)*m.M.axis[2](0)+M.axis[1](1)*m.M.axis[2](1)+M.axis[2](1)*m.M.axis[2](2);
		retM.M.axis[2](2)=M.axis[0](2)*m.M.axis[2](0)+M.axis[1](2)*m.M.axis[2](1)+M.axis[2](2)*m.M.axis[2](2);
		retM.X(0)=M.axis[0](0)*m.X(0)+M.axis[1](0)*m.X(1)+M.axis[2](0)*m.X(2)+X(0);
		retM.X(1)=M.axis[0](1)*m.X(0)+M.axis[1](1)*m.X(1)+M.axis[2](1)*m.X(2)+X(1);
		retM.X(2)=M.axis[0](2)*m.X(0)+M.axis[1](2)*m.X(1)+M.axis[2](2)*m.X(2)+X(2);
		return(retM);
		/*	// Normal version:
		C4X4Matrix retM;
		retM.M=M*m.M;
		retM.X=(M*m.X)+X;
		return(retM);
		//	*/
	}
	inline C3Vector operator* (const C3Vector& v) const
	{
		// Speed optimized version:
		return(C3Vector(M.axis[0](0)*v(0)+M.axis[1](0)*v(1)+M.axis[2](0)*v(2)+X(0),
			M.axis[0](1)*v(0)+M.axis[1](1)*v(1)+M.axis[2](1)*v(2)+X(1),
			M.axis[0](2)*v(0)+M.axis[1](2)*v(1)+M.axis[2](2)*v(2)+X(2)));
		// Normal version:
		//	return(C3Vector((M*v)+X));
	}
	inline C4X4Matrix& operator= (const C4X4Matrix& m)
	{
		// Speed optimized version:
		M.axis[0](0)=m.M.axis[0](0);
		M.axis[0](1)=m.M.axis[0](1);
		M.axis[0](2)=m.M.axis[0](2);
		M.axis[1](0)=m.M.axis[1](0);
		M.axis[1](1)=m.M.axis[1](1);
		M.axis[1](2)=m.M.axis[1](2);
		M.axis[2](0)=m.M.axis[2](0);
		M.axis[2](1)=m.M.axis[2](1);
		M.axis[2](2)=m.M.axis[2](2);
		X(0)=m.X(0);
		X(1)=m.X(1);
		X(2)=m.X(2);
		return(*this);
		/*	// Normal version:	
		M=m.M;
		X=m.X;
		return(*this);
		//	*/
	}
	inline void operator*= (const C4X4Matrix& m)
	{
		C4X4Matrix retM((*this)*m);
		(*this)=retM;
	}

	C3X3Matrix M;
	C3Vector X;
};
