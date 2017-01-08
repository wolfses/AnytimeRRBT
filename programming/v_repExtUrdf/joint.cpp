// This file is part of the URDF PLUGIN for V-REP
//  
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// A big thanks to Ignacio Tartavull for his precious help!
// 
// The URDF PLUGIN is licensed under the terms of GNU GPL:
// 
// -------------------------------------------------------------------
// The URDF PLUGIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// THE URDF PLUGIN IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with the URDF PLUGIN.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------

//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

// The URDF plugin is courtesy of Ignacio Tartavull. A few modifications by Marc.

#include "joint.h"
#include "commonFunctions.h"
#include "4X4Matrix.h"

joint::joint() 
{
	//Initialize arrays
	 origin_xyz[0] = 0;   origin_xyz[1] = 0;   origin_xyz[2] = 0;
	 origin_rpy[0] = 0;	  origin_rpy[1] = 0;   origin_rpy[2] = 0;
	 axis[0] = 1; 		  axis[1] = 0;		   axis[2] = 0;

	 jointBaseFrame.setIdentity();

	 nJoint = -1;
	 jointType=-1;
	 lowerLimit=0.0f;
	 upperLimit=0.0f;

	 effortLimitAngular=2.0f;
	 effortLimitLinear=200.0f;
	 velocityLimitAngular=20.0f;
	 velocityLimitLinear=20.0f;
}


joint::~joint()
{
}

void joint::setJointType(std::string gazeboJointType)
{
		if (gazeboJointType == "revolute")
			jointType=0;
		if (gazeboJointType == "prismatic")
			jointType=1;
		if (gazeboJointType == "spherical")
			jointType=2;
		if (gazeboJointType == "continuous")
			jointType=3;
		if (gazeboJointType == "fixed")
			jointType=4;
}

void joint::setOrigin(std::string gazebo_origin_xyz,std::string gazebo_origin_rpy)
{
	stringToArray(origin_xyz,gazebo_origin_xyz);
	stringToArray(origin_rpy,gazebo_origin_rpy);
}

void joint::setParent(std::string gazebo_parentName)
{
	parentLink = gazebo_parentName;
}

void joint::setChild(std::string gazebo_childName)
{
	childLink = gazebo_childName;
}

void joint::setAxis(std::string gazebo_axis_xyz)
{
	stringToArray(axis,gazebo_axis_xyz.c_str());
}


