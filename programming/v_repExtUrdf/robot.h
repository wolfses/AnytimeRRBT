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
#include "tinyxml2.h"
#include "link.h"
#include "joint.h"
#include "sensor.h"
#include "4X4Matrix.h"



class robot
{
private:
	std::vector<joint*> vJoints;
	std::vector<urdfLink*> vLinks;
	std::vector<sensor*> vSensors;

public:
	//Data
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement *robotElement;

	std::string filenameAndPath;
	std::string packagePath;
	std::string name;
	
	//Functions
	robot(std::string filename,bool hideCollisionLinks,bool hideJoints,bool convexDecomposeNonConvexCollidables,bool createVisualIfNone,bool showConvexDecompositionDlg,bool centerAboveGround,bool makeModel,bool noSelfCollision,bool positionCtrl);
	~robot();

	void openFile(); //opens the filenamAndPath which is given by a file dialog
	
	void readJoints();
	void readLinks();
	void readSensors();

	void createJoints(bool hideJoints,bool positionCtrl);
	void createLinks(bool hideCollisionLinks,bool convexDecomposeNonConvexCollidables,bool createVisualIfNone,bool showConvexDecompositionDlg);
	void createSensors();

	int getJointPosition(std::string jointName);
	int getLinkPosition(std::string linkName);

	void setLocalRespondableMaskCummulative_alternate(int objHandle,bool bitSet);
};

