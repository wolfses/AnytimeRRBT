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

#include "Rotate.h"

Rotate::Rotate() : Transformation(Transformation::Rotate)
{

}

Rotate::Rotate(const vec3& rotAxis,double angle) : Transformation(Transformation::Rotate)
{
	m_RotAxis = rotAxis;
	m_RotAngle = angle;
}

Rotate::Rotate(double X,double Y, double Z, double angle) : Transformation(Transformation::Rotate)
{
	m_RotAxis.set(X,Y,Z);
	m_RotAngle = angle;
}

const vec3& Rotate::getRotationAxis() const
{
	return m_RotAxis;
}

double Rotate::getRotationAngle() const
{
	return m_RotAngle;
}

void Rotate::setRoationAxis(const vec3& axis)
{
	m_RotAxis = axis;
}
void Rotate::setRotationAngle(double rotAngle)
{
	m_RotAngle = rotAngle;
}

Transformation* Rotate::clone() const
{
	return new Rotate(*this);
}
