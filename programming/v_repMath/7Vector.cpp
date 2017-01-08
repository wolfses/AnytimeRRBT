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

#include "7Vector.h"
#include "4X4Matrix.h"

C7Vector::C7Vector()
{
}

C7Vector::C7Vector(const C7Vector& v)
{
	(*this)=v;
}

C7Vector::C7Vector(const C4Vector& q)
{
	Q=q;
	X.clear();
}

C7Vector::C7Vector(const C3Vector& x)
{
	X=x;
	Q.setIdentity();
}

C7Vector::C7Vector(const C4Vector& q,const C3Vector& x)
{
	Q=q;
	X=x;
}

C7Vector::C7Vector(const float m[4][4])
{
	set(m);
}

C7Vector::C7Vector(const C4X4Matrix& m)
{
	set(m);
}

C7Vector::C7Vector(float angle,const C3Vector& pos,const C3Vector& dir)
{ // Builds a rotation around dir at position pos of angle angle (in radians)
	C7Vector shift1;
	shift1.setIdentity();
	shift1.X(0)=-pos(0);
	shift1.X(1)=-pos(1);
	shift1.X(2)=-pos(2);
	C7Vector shift2;
	shift2.setIdentity();
	shift2.X=pos;
	C7Vector rot;
	rot.setIdentity();
	rot.Q.setAngleAndAxis(angle,dir);
	(*this)=shift2*rot*shift1;
}


C7Vector::~C7Vector()
{

}

void C7Vector::set(float m[4][4])
{
	C4X4Matrix tr(m);
	(*this)=tr.getTransformation();
}

void C7Vector::set(const C4X4Matrix& m)
{
	(*this)=m.getTransformation();
}

C3Vector C7Vector::getAxis(int index) const
{
	return(Q.getAxis(index));
}

void C7Vector::setIdentity()
{
	Q.setIdentity();
	X.clear();
}

C4X4Matrix C7Vector::getMatrix() const
{
	return(C4X4Matrix(Q.getMatrix(),X));
}
void C7Vector::copyTo(float m[4][4]) const
{ // Temporary routine. Remove later!
	C4X4Matrix tmp(getMatrix());
	for (int i=0;i<3;i++)
	{
		for (int j=0;j<3;j++)
			m[i][j]=tmp.M(i,j);
		m[i][3]=tmp.X(i);
	}
	m[3][0]=0.0f;
	m[3][1]=0.0f;
	m[3][2]=0.0f;
	m[3][3]=1.0f;
}

C7Vector& C7Vector::operator= (const C7Vector& v)
{
	Q=v.Q;
	X=v.X;
	return(*this);
}

void C7Vector::setMultResult(const C7Vector& v1,const C7Vector& v2)
{
	X=v1.X+(v1.Q*v2.X);
	Q=v1.Q*v2.Q;
}

C7Vector C7Vector::operator* (const C7Vector& v) const
{ // Transformation multiplication
	C7Vector retV;
	retV.X=X+(Q*v.X);
	retV.Q=Q*v.Q;
	retV.Q.normalize();
	return(retV);
}

void C7Vector::operator*= (const C7Vector& v)
{ 
	X+=(Q*v.X);
	Q*=v.Q;
}

C3Vector C7Vector::operator* (const C3Vector& v) const
{ // Vector transformation
	return(X+(Q*v)); 
}

void C7Vector::inverse()
{
	(*this)=getInverse();
}

C7Vector C7Vector::getInverse() const
{
	C7Vector retV;
	retV.Q=Q.getInverse();
	retV.X=(retV.Q*X)*-1.0f;
	return(retV);
}

void C7Vector::buildInterpolation(const C7Vector& fromThis,const C7Vector& toThat,float t)
{	// Builds the interpolation (based on t) from 'fromThis' to 'toThat'
	Q.buildInterpolation(fromThis.Q,toThat.Q,t);
	X.buildInterpolation(fromThis.X,toThat.X,t);
}

const C7Vector C7Vector::identityTransformation(C4Vector(1.0f,0.0f,0.0f,0.0f),C3Vector(0.0f,0.0f,0.0f));
