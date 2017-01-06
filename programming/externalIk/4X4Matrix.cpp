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

#include "4X4Matrix.h"
#include "4X4FullMatrix.h"
#include "MMatrix.h"


C4X4Matrix::C4X4Matrix()
{
}

C4X4Matrix::C4X4Matrix(const C3X3Matrix& m,const C3Vector& x)
{
	M=m;
	X=x;
}

C4X4Matrix::C4X4Matrix(const C7Vector& transf)
{
	M=transf.Q.getMatrix();
	X=transf.X;
}

C4X4Matrix::C4X4Matrix(const C4X4Matrix& m)
{
	(*this)=m;
}

C4X4Matrix::C4X4Matrix(const CMatrix& m)
{
	(*this)=m;
}

C4X4Matrix::C4X4Matrix(const float m[4][4])
{
	for (int i=0;i<3;i++)
	{
		for (int j=0;j<3;j++)
			M(i,j)=m[i][j];
		X(i)=m[i][3];
	}
}

C4X4Matrix::C4X4Matrix(const C3Vector& x,const C3Vector& y,const C3Vector& z,const C3Vector& pos)
{
	M.set(x,y,z);
	X=pos;
}

C4X4Matrix::~C4X4Matrix()
{

}
void C4X4Matrix::setIdentity()
{
	M.setIdentity();
	X.clear();
}

C4X4Matrix C4X4Matrix::operator* (const CMatrix& m) const
{
	C4X4Matrix retM((*this)*C4X4Matrix(m));
	return(retM);
}

C4X4Matrix& C4X4Matrix::operator= (const CMatrix& m)
{
	M.axis[0](0)=m(0,0);
	M.axis[0](1)=m(1,0);
	M.axis[0](2)=m(2,0);
	M.axis[1](0)=m(0,1);
	M.axis[1](1)=m(1,1);
	M.axis[1](2)=m(2,1);
	M.axis[2](0)=m(0,2);
	M.axis[2](1)=m(1,2);
	M.axis[2](2)=m(2,2);
	X(0)=m(0,3);
	X(1)=m(1,3);
	X(2)=m(2,3);
	return(*this);
}

C4X4Matrix& C4X4Matrix::operator= (const C4X4FullMatrix& m)
{
	M.axis[0](0)=m(0,0);
	M.axis[0](1)=m(1,0);
	M.axis[0](2)=m(2,0);
	M.axis[1](0)=m(0,1);
	M.axis[1](1)=m(1,1);
	M.axis[1](2)=m(2,1);
	M.axis[2](0)=m(0,2);
	M.axis[2](1)=m(1,2);
	M.axis[2](2)=m(2,2);
	X(0)=m(0,3);
	X(1)=m(1,3);
	X(2)=m(2,3);
	return(*this);
}

C7Vector C4X4Matrix::getTransformation() const
{
	return(C7Vector(M.getQuaternion(),X));
}


void C4X4Matrix::buildInterpolation(const C4X4Matrix& fromThis,const C4X4Matrix& toThat,float t)
{	// Builds the interpolation (based on t) from 'fromThis' to 'toThat'
	C7Vector out;
	out.buildInterpolation(fromThis.getTransformation(),toThat.getTransformation(),t);
	(*this)=out;
}

void C4X4Matrix::rotateAroundX(float angle)
{
	C4X4Matrix rot;
	rot.setIdentity();
	rot.M.buildXRotation(angle);
	(*this)=rot*(*this);
}

void C4X4Matrix::rotateAroundY(float angle)
{
	C4X4Matrix rot;
	rot.setIdentity();
	rot.M.buildYRotation(angle);
	(*this)=rot*(*this);
}

void C4X4Matrix::rotateAroundZ(float angle)
{
	C4X4Matrix rot;
	rot.setIdentity();
	rot.M.buildZRotation(angle);
	(*this)=rot*(*this);
}

void C4X4Matrix::buildXRotation(float angle)
{
	setIdentity();
	M.buildXRotation(angle);
}

void C4X4Matrix::buildYRotation(float angle)
{
	setIdentity();
	M.buildYRotation(angle);
}

void C4X4Matrix::buildZRotation(float angle)
{
	setIdentity();
	M.buildZRotation(angle);
}

void C4X4Matrix::buildTranslation(float x,float y,float z)
{
	setIdentity();
	X(0)=x;
	X(1)=y;
	X(2)=z;
}

void C4X4Matrix::translate(float x,float y,float z)
{
	X(0)+=x;
	X(1)+=y;
	X(2)+=z;
}
