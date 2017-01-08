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

#include "sensor.h"
#include "commonFunctions.h"
#include "4X4Matrix.h"

sensor::sensor()
{
	//Initialize arrays
	 origin_xyz[0] = 0;   origin_xyz[1] = 0;   origin_xyz[2] = 0;
	 origin_rpy[0] = 0;	  origin_rpy[1] = 0;   origin_rpy[2] = 0;

	 resolution[0]=256;
	 resolution[1]=256;

	 clippingPlanes[0]=0.01f;
	 clippingPlanes[1]=2.0f;

	 cameraSensorPresent=false;
	 proximitySensorPresent=false;
	 gazeboSpec=false;

	 nSensor = -1;
	 nSensorAux = -1;
}


sensor::~sensor()
{
}

