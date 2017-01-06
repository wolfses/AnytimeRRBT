// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// -------------------------------------------------------------------
// THIS FILE IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// You are free to use/modify/distribute this file for whatever purpose!
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#pragma once

#define piValue 3.1415926535f

// Never change following data, data is serialized together with a scene!
// Keep following data same across all of your plugins to V-REP
#define DEVELOPER_DATA_HEADER 0		// Keep always same for all plugins created by you or your company!!!
									// This number identifies data you might save together with scene objects
									// If several developers or companies are using the same number, then
									// data attached to scene objects might not be interpreted correctly.
									// Best is to use the serial number of your first copy of V-REP and
									// always stick to it.

// Following section defines data tags. You can attach custom data to any object in a scene.
// That custom data is saved and loaded together with the scene object.
// Refer to the plugin tutorial in the V-REP manual for more details.

#define	MTB_BASE					210 // identifies the base object of an "mtb"-robot
#define MTB_PROGRAM					211 // TAG under which the robot language program is saved. MTB_PROGRAM is attached to the base of the MTB robot model
#define MTB_OPTIONS					214 // indicates options (bit1=automatic execution. MTB_OPTIONS is attached to the base of the MTB robot model

// You can add more tags here. But make sure that all tags under the same DEVELOPER_DATA_HEADER
// do not collide (also across other plugins)
