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
