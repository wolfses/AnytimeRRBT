// This file is part of the COLLADA PLUGIN for V-REP
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The COLLADA PLUGIN is licensed under the terms of GNU GPL:
// 
// -------------------------------------------------------------------
// The COLLADA PLUGIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// THE COLLADA PLUGIN IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
//  
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with the COLLADA PLUGIN.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

// Written by Alex Doumanoglou on behalf of Dr. Marc Freese

#include "mat4.h"
#include <math.h>

mat4::mat4()
{
	// load identity matrix
	for(int i=0;i<4;i++) {
		for(int j=0;j<4;j++) {
			if(i!=j)
				m_Mat[i][j] = 0;
			else
				m_Mat[i][j] = 1;
		}
	}
}

double& mat4::operator()(int row,int col)
{
	return m_Mat[row][col];
}

const double& mat4::operator()(int row,int col) const
{
	return m_Mat[row][col];
}

mat4 mat4::operator*(const mat4& other) const
{
	mat4 mat;
	for(int i=0;i<4;i++) {
		for(int j=0;j<4;j++) {
			mat.m_Mat[i][j] = 0;
			for(int k=0;k<4;k++) {
				mat.m_Mat[i][j] += m_Mat[i][k]*other.m_Mat[k][j];
			}
		}
	}
	return mat;
}

mat4& mat4::operator*=(const mat4& other)
{	
	*this = (*this)*other;
	return *this;
}

mat4 mat4::IdentityMatrix()
{
	return mat4();
}

mat4 mat4::TranslationMatrix(const vec3& t)
{
	mat4 T;
	T.m_Mat[0][3] = t.X;
	T.m_Mat[1][3] = t.Y;
	T.m_Mat[2][3] = t.Z;
	return T;
}

mat4 mat4::ScaleMatrix(const vec3& s)
{
	mat4 T;
	T.m_Mat[0][0] = s.X;
	T.m_Mat[1][1] = s.Y;
	T.m_Mat[2][2] = s.Z;
	return T;
}

mat4 mat4::RotationMatrix(vec3 axis,double angle)
{
	const float MATH_PI = 3.141592653f;
	float rad = angle*MATH_PI/180.0f;
	mat4 R;	
	axis.normalize();
	double x=axis.X;

	double y = axis.Y;
	double z = axis.Z;
	double c = cos(rad);
	double s = sin(rad);
	R.m_Mat[0][0] = x*x*(1-c)+c;
	R.m_Mat[0][1] = x*y*(1-c)-z*s;
	R.m_Mat[0][2] = x*z*(1-c)+y*s;
	R.m_Mat[1][0] = y*x*(1-c)+z*s;
	R.m_Mat[1][1] = y*y*(1-c)+c;
	R.m_Mat[1][2] = y*z*(1-c)-x*s;
	R.m_Mat[2][0] = z*x*(1-c)-y*s;
	R.m_Mat[2][1] = z*y*(1-c)+x*s;
	R.m_Mat[2][2] = z*z*(1-c)+c;

	return R;
}
