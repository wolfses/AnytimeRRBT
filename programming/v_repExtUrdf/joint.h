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

#pragma once

#include "v_repLib.h"
#include <iostream>
#include <vector>
#include <bitset>
#include "4X4Matrix.h"


class joint
{
public:
	//Variables
	float origin_xyz[3];
	float origin_rpy[3];
	float axis[3];
	float lowerLimit;
	float upperLimit;
	float effortLimitAngular;
	float effortLimitLinear;
	float velocityLimitAngular;
	float velocityLimitLinear;

	C7Vector jointBaseFrame;

	simInt nJoint;
	std::string name;
	simInt jointType;

	std::string parentJoint;
	std::string parentLink;
	std::string childLink;

	//Functions
	joint(void);
	~joint(void);
	
	//Read
	void setJointType(std::string gazeboJointType);
	void setOrigin(std::string gazebo_origin_xyz,std::string gazebo_origin_rpy);
	void setAxis(std::string gazebo_axis_xyz);
	void setParent(std::string gazebo_parentName);
	void setChild(std::string gazebo_childName);
};

