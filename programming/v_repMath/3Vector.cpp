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

#include "MyMath.h"
#include "3Vector.h"
#include "3X3Matrix.h"
#include "4X4Matrix.h"
#include "7Vector.h"

C3Vector::C3Vector()
{
}

C3Vector::C3Vector(float v0,float v1,float v2)
{
	data[0]=v0;
	data[1]=v1;
	data[2]=v2;
}

C3Vector::C3Vector(const float v[3])
{
	data[0]=v[0];
	data[1]=v[1];
	data[2]=v[2];
}

C3Vector::C3Vector(const C3Vector& v)
{
	(*this)=v;
}

C3Vector::~C3Vector()
{
}

float C3Vector::getAngle(const C3Vector& v) const
{ // Return value is in radian!!
	C3Vector a(getNormalized());
	C3Vector b(v.getNormalized());
	return(CMath::robustAcos(a*b));
}


C3X3Matrix C3Vector::getProductWithStar() const
{
	C3X3Matrix retM;
	retM(0,0)=0.0f;
	retM(0,1)=-data[2];
	retM(0,2)=data[1];
	retM(1,0)=data[2];
	retM(1,1)=0.0f;
	retM(1,2)=-data[0];
	retM(2,0)=-data[1];
	retM(2,1)=data[0];
	retM(2,2)=0.0f;
	return(retM);
}

void C3Vector::operator*= (const C4X4Matrix& m)
{
//	(*this)=m*(*this);
	float x=data[0];
	float y=data[1];
	float z=data[2];
	data[0]=m.M.axis[0].data[0]*x+m.M.axis[1].data[0]*y+m.M.axis[2].data[0]*z+m.X.data[0];
	data[1]=m.M.axis[0].data[1]*x+m.M.axis[1].data[1]*y+m.M.axis[2].data[1]*z+m.X.data[1];
	data[2]=m.M.axis[0].data[2]*x+m.M.axis[1].data[2]*y+m.M.axis[2].data[2]*z+m.X.data[2];
}
void C3Vector::operator*= (const C3X3Matrix& m)
{
//	(*this)=m*(*this);
	float x=data[0];
	float y=data[1];
	float z=data[2];
	data[0]=m.axis[0].data[0]*x+m.axis[1].data[0]*y+m.axis[2].data[0]*z;
	data[1]=m.axis[0].data[1]*x+m.axis[1].data[1]*y+m.axis[2].data[1]*z;
	data[2]=m.axis[0].data[2]*x+m.axis[1].data[2]*y+m.axis[2].data[2]*z;
}


void C3Vector::operator*= (const C7Vector& transf)
{
	(*this)=transf*(*this);
}

void C3Vector::buildInterpolation(const C3Vector& fromThis,const C3Vector& toThat,float t)
{
	(*this)=fromThis+((toThat-fromThis)*t);
}

const C3Vector C3Vector::oneOneOneVector(1.0f,1.0f,1.0f);
const C3Vector C3Vector::unitXVector(1.0f,0.0f,0.0f);
const C3Vector C3Vector::unitYVector(0.0f,1.0f,0.0f);
const C3Vector C3Vector::unitZVector(0.0f,0.0f,1.0f);
const C3Vector C3Vector::zeroVector(0.0f,0.0f,0.0f);
