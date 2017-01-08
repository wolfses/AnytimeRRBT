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

#include "commonFunctions.h"
#include <string>
#include "v_repLib.h"

void printToConsole(const char* txt)
{
	static int handle=-1;
	std::string text(txt);
	text+="\n";
	std::cout << text;
	if (simAuxiliaryConsolePrint(handle,text.c_str())<=0)
	{
		handle=simAuxiliaryConsoleOpen("URDF import",5000,2+4,NULL,NULL,NULL,NULL);
		simAuxiliaryConsolePrint(handle,text.c_str());
	}
}

void stringToArray(float array[],const std::string xyz)
{
	
	std::string buf; 
    std::stringstream ss(xyz); 

   	int i = 0;
	while (ss >> buf){ array[i++]=getFloat(buf);}
	
	
}

float getFloat(const std::string& text)
{
	try
	{
		return(boost::lexical_cast<float>(text));
	}
	catch (boost::bad_lexical_cast &)
	{
		return(0.0f);
	}
}

int getInt(const std::string& text)
{
	try
	{
		return(boost::lexical_cast<int>(text));
	}
	catch (boost::bad_lexical_cast &)
	{
		return(0);
	}
}


bool isArrayEmpty(float array[])
{
	if(array[0] == 0.0f && array[1] ==0.0f && array[2] == 0.0f){ return true;}
	else {return false;}
}
float* minus(float arr[])
{
	arr[0] = -arr[0];
	arr[1] = -arr[1];
	arr[2] = -arr[2];

	return arr;
}
std::string printMatrix(const float arr[])
{
	return (" value0 = "+boost::lexical_cast<std::string>(arr[0])+" value1 = "+boost::lexical_cast<std::string>(arr[1])+ " value2 = "+boost::lexical_cast<std::string>(arr[2])).c_str();
}

void setVrepObjectName(int objectHandle,const char* desiredName)
{
	std::string baseName(desiredName);
	for (int i=0;i<int(baseName.size());i++)
	{ // Objects in V-REP can only contain a-z, A-Z, 0-9, '_' or exaclty one '#' optionally followed by a number
		char n=baseName[i];
		if ( ((n<'a')||(n>'z')) && ((n<'A')||(n>'Z')) && ((n<'0')||(n>'9')) )
			baseName[i]='_';
	}
	std::string objName(baseName);
	int suffix=2;
	while (simSetObjectName(objectHandle,objName.c_str())==-1)
		objName=baseName+boost::lexical_cast<std::string>(suffix++);
}

C4Vector getQuaternionFromRpy(float rpy[3])
{
	C4Vector q1,q2,q3;
	q1.setEulerAngles(C3Vector(rpy[0],0.0f,0.0f));
	q2.setEulerAngles(C3Vector(0.0f,rpy[1],0.0f));
	q3.setEulerAngles(C3Vector(0.0f,0.0f,rpy[2]));
	return(q3*q2*q1);
}

