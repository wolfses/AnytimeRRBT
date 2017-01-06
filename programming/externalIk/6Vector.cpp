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

#include "6Vector.h"
#include "6X6Matrix.h"


C6Vector::C6Vector()
{
}

C6Vector::C6Vector(float v0,float v1,float v2,float v3,float v4,float v5)
{
	V[0](0)=v0;
	V[0](1)=v1;
	V[0](2)=v2;
	V[1](0)=v3;
	V[1](1)=v4;
	V[1](2)=v5;
}

C6Vector::C6Vector(const float v[6])
{
	V[0](0)=v[0];
	V[0](1)=v[1];
	V[0](2)=v[2];
	V[1](0)=v[3];
	V[1](1)=v[4];
	V[1](2)=v[5];
}

C6Vector::C6Vector(const C3Vector& v0,const C3Vector& v1)
{
	V[0]=v0;
	V[1]=v1;
}

C6Vector::C6Vector(const C6Vector& v)
{
	V[0]=v.V[0];
	V[1]=v.V[1];
}

C6Vector::~C6Vector()
{

}

void C6Vector::clear()
{
	V[0].clear();
	V[1].clear();
}

C6Vector C6Vector::operator* (float d) const
{
	C6Vector retV;
	retV.V[0]=V[0]*d;
	retV.V[1]=V[1]*d;
	return(retV);
}

C6Vector C6Vector::operator/ (float d) const
{
	C6Vector retV;
	retV.V[0]=V[0]/d;
	retV.V[1]=V[1]/d;
	return(retV);
}

C6Vector& C6Vector::operator= (const C6Vector& v)
{
	V[0]=v.V[0];
	V[1]=v.V[1];
	return(*this);
}
C6Vector C6Vector::operator+ (const C6Vector& v) const
{
	C6Vector retV;
	retV.V[0]=V[0]+v.V[0];
	retV.V[1]=V[1]+v.V[1];
	return(retV);
}
C6Vector C6Vector::operator- (const C6Vector& v) const
{
	C6Vector retV;
	retV.V[0]=V[0]-v.V[0];
	retV.V[1]=V[1]-v.V[1];
	return(retV);
}
float C6Vector::operator* (const C6Vector& v) const
{ // Scalar product of spatial vectors:
	return(V[0](0)*v.V[1](0)+V[0](1)*v.V[1](1)+V[0](2)*v.V[1](2)+V[1](0)*v.V[0](0)+V[1](1)*v.V[0](1)+V[1](2)*v.V[0](2));
}

C6X6Matrix C6Vector::getSpatialCross() const
{ // Not very efficiently implemented. Rewrite later!
	C6X6Matrix retM;
	retM.M[0][1].clear();
	retM.M[0][0]=V[0].getProductWithStar();
	retM.M[1][0]=V[1].getProductWithStar();
	retM.M[1][1]=V[0].getProductWithStar();
	return(retM);
}

void C6Vector::operator*= (float d)
{
	V[0]*=d;
	V[1]*=d;
}

void C6Vector::operator/= (float d)
{
	V[0]/=d;
	V[1]/=d;
}

void C6Vector::operator+= (const C6Vector& v)
{
	V[0]+=v.V[0];
	V[1]+=v.V[1];
}

void C6Vector::operator-= (const C6Vector& v)
{
	V[0]-=v.V[0];
	V[1]-=v.V[1];
}
