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
// This file was automatically created for V-REP release V3.1.2 on June 16th 2014

#pragma once

#include "vrepPrecompiledHeader.h"

// FULLY STATIC CLASS
class CCuttingRoutine  
{
public:
	// The one and only general routine:
	static int cutEntity(int millID,int entityID,int& cutObject,float& cutSurface,float& cutVolume,bool justForInitialization,bool overrideCuttableFlagIfNonCollection);
private:
	static bool _cutShape(int millID,int shapeID,float& cutSurface,bool justForInitialization,bool overrideShapeCuttableFlag);
	static bool _cutVolumeObject(int millID,int volumeObjectID,float& cutVolume,bool justForInitialization,bool overrideVolumeCuttableFlag);
	static int _cutGroup(int millID,int groupID,int& cutObject,float& cutSurface,float& cutVolume,bool justForInitialization);
};
