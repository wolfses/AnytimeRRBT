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
#include <sstream>
#include <boost/lexical_cast.hpp>

#include "4X4Matrix.h"
#include "3Vector.h"
#include "7Vector.h"
#include "3X3Matrix.h"
#include "MyMath.h"

#include "commonFunctions.h"

class urdfLink
{
public:
    //Variables Inertial
	float inertial_xyz[3];
	float inertial_rpy[3];
	bool inertiaPresent;

	float visual_xyz[3];
	float visual_rpy[3];
	float visual_box_size[3];				//If all the coordinates are zero do not create the object
	float visual_sphere_size[3];			//If all the coordinates are zero do not create the object
	float visual_cylinder_size[3];			//If all the coordinates are zero do not create the object
	float visual_rgba[4]; // a is ignored
	float visual_mesh_scaling[3];

	//Variables Collision
	float collision_xyz[3];
	float collision_rpy[3];
	float collision_box_size[3];				//If all the coordinates are zero do not create the object
	float collision_sphere_size[3];			//If all the coordinates are zero do not create the object
	float collision_cylinder_size[3];			//If all the coordinates are zero do not create the object
	float collision_mesh_scaling[3];
	

	//Common variables
	float inertia[9];
	float mass;

	std::string name;

	//This variables does not came from the urdf.
	std::string parent;
	simInt nParent;

	std::string child;
	simInt nChild;

	std::string visual_meshFilename;
	std::string visual_meshFilename_alt;
	simInt visual_meshExtension;
	std::string collision_meshFilename;
	std::string collision_meshFilename_alt;
	simInt collision_meshExtension;

	simInt nLinkVisual;
	simInt nLinkCollision;
	
	//Functions
	urdfLink();
	~urdfLink();
	
	void setPosition(std::string gazebo_xyz,std::string choose);
	void setRotation(std::string gazebo_rpy,std::string choose);
	void setBox(std::string gazebo_size,std::string choose);
	void setSphere(std::string gazebo_radius,std::string choose);
	void setCylinder(std::string gazebo_radius,std::string gazebo_length,std::string choose);
	void setMass(std::string gazebo_mass);
	void setInertia(int position, std::string gazebo_inertia_number);
	void setMeshFilename(std::string packagePath,std::string meshFilename,std::string choose);
	void setColor(std::string color);
	void verifyInertia();
	int scaleShapeIfRequired(int shapeHandle,float scalingFactors[3]);
	void createLink(bool hideCollisionLinks,bool convexDecomposeNonConvexCollidables,bool createVisualIfNone,bool& showConvexDecompositionDlg);
};

