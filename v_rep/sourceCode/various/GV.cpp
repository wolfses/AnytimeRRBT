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

#include "vrepPrecompiledHeader.h"
#include "GV.h"
#include "Global.h"
#include "Tt.h"
#include "MyMath.h"
#include <boost/lexical_cast.hpp>

std::string gv::getAngleUnitStr()
{
	if (degrees)
		return("Degrees");
	else
		return("Radians");
}

std::string gv::getSizeUnitStr()
{
	if (meters)
		return("Meters");
	else
		return("Millimeters");
}

std::string gv::getSizeUnitStrShort()
{
	if (meters)
		return("m");
	else
		return("mm");
}

std::string gv::getMassUnitStr()
{
	if (kilograms)
		return("Kilograms");
	else
		return("Grams");
}

std::string gv::getMassUnitStrShort()
{
	if (kilograms)
		return("kg");
	else
		return("g");
}

std::string gv::getSurfaceUnitStr()
{
	if (meters)
		return("Square meters");
	else
		return("Square millimeters");
}

std::string gv::getVolumeUnitStr()
{
	if (meters)
		return("Cubic meters");
	else
		return("Cubic millimeters");
}

std::string gv::getTimeUnitStr()
{
	if (!seconds)
		return("Milliseconds");
	else
		return("Seconds");
}

std::string gv::getTimeUnitStrShort()
{
	if (!seconds)
		return("ms");
	else
		return("s");
}

std::string gv::getIntUnitStr()
{
	return("Integer");
}

std::string gv::getFloatUnitStr()
{
	return("Floating point");
}

std::string gv::getBoolUnitStr()
{
	return("Boolean");
}

std::string gv::getCountUnitStr()
{
	return("Count");
}

std::string gv::getBoolStateUnitStr()
{
	return("Boolean state");
}

std::string gv::getUserUnitStr()
{
	return("User unit");
}

std::string gv::getNullUnitStr()
{
	return("Null");
}

std::string gv::formatUnitStr(const std::string& txt,const std::string& s,bool inParenthesis,bool startWithSpace)
{
	std::string str(txt);
	if (startWithSpace)
		str+=" ";
	if (inParenthesis)
		str+="[";
	for (int i=0;i<int(s.length());i++)
	{
		if (s[i]=='m')
		{
			if (meters)
				str+="m";
			else
				str+="mm";
		}
		else if (s[i]=='s')
		{
			if (seconds)
				str+="s";
			else
				str+="ms";
		}
		else if (s[i]=='k')
		{
			if (kilograms)
				str+="kg";
			else
				str+="g";
		}
		else if (s[i]=='r')
		{
			if (degrees)
				str+="deg";
			else
				str+="rad";
		}
		else
			str+=s[i];
	}
	if (inParenthesis)
		str+="]";
	return(str);
}

void gv::setToSeconds()
{
	seconds=true;
	userToSeconds=1.0f;
	secondsToUser=1.0f;

	leadingZeroTime=0;
	decimalsTime=3;

	angularVelToUser=radToUser/secondsToUser;
	userToAngularVel=userToRad/userToSeconds;
	angularAccelToUser=radToUser/(secondsToUser*secondsToUser);
	userToAngularAccel=userToRad/(userToSeconds*userToSeconds);

	linearVelToUser=meterToUser/secondsToUser;
	userToLinearVel=userToMeter/userToSeconds;
	linearAccelToUser=meterToUser/(secondsToUser*secondsToUser);
	userToLinearAccel=userToMeter/(userToSeconds*userToSeconds);
	userToForce=userToKilograms*userToMeter/(userToSeconds*userToSeconds);
	userToTorque=userToKilograms*userToMeter*userToMeter/(userToSeconds*userToSeconds);
	forceToUser=kilogramsToUser*meterToUser/(secondsToUser*secondsToUser);
	torqueToUser=kilogramsToUser*meterToUser*meterToUser/(secondsToUser*secondsToUser);

	if (degrees)
	{
		decimalsOrDot=2;
		decimalsOrDotDot=2;
	}
	else
	{
		decimalsOrDot=4;
		decimalsOrDotDot=4;
	}
	if (meters)
	{
		decimalsPosDot=4;
		decimalsPosDotDot=4;
	}
	else
	{
		decimalsPosDot=1;
		decimalsPosDotDot=1;
	}
}

void gv::setToKilograms()
{
	kilograms=true;
	userToKilograms=1.0f;
	kilogramsToUser=1.0f;
	densityToUser=kilogramsToUser/(meterToUser*meterToUser*meterToUser);
	userToDensity=meterToUser*meterToUser*meterToUser/kilogramsToUser;

	userToForce=userToKilograms*userToMeter/(userToSeconds*userToSeconds);
	userToTorque=userToKilograms*userToMeter*userToMeter/(userToSeconds*userToSeconds);
	forceToUser=kilogramsToUser*meterToUser/(secondsToUser*secondsToUser);
	torqueToUser=kilogramsToUser*meterToUser*meterToUser/(secondsToUser*secondsToUser);

	decimalsMass=4;
	if (meters)
		decimalsDensity=1;
	else
		decimalsDensity=8;
}

void gv::setToGrams()
{
	kilograms=false;
	userToKilograms=0.001f;
	kilogramsToUser=1000.0f;
	densityToUser=kilogramsToUser/(meterToUser*meterToUser*meterToUser);
	userToDensity=meterToUser*meterToUser*meterToUser/kilogramsToUser;

	userToForce=userToKilograms*userToMeter/(userToSeconds*userToSeconds);
	userToTorque=userToKilograms*userToMeter*userToMeter/(userToSeconds*userToSeconds);
	forceToUser=kilogramsToUser*meterToUser/(secondsToUser*secondsToUser);
	torqueToUser=kilogramsToUser*meterToUser*meterToUser/(secondsToUser*secondsToUser);

	decimalsMass=2;
	if (meters)
		decimalsDensity=0;
	else
		decimalsDensity=4;
}

void gv::setToMiliseconds()
{
	seconds=false;
	userToSeconds=0.001f;
	secondsToUser=1000.0f;
	leadingZeroTime=0;
	decimalsTime=1;

	angularVelToUser=radToUser/secondsToUser;
	userToAngularVel=userToRad/userToSeconds;
	angularAccelToUser=radToUser/(secondsToUser*secondsToUser);
	userToAngularAccel=userToRad/(userToSeconds*userToSeconds);

	linearVelToUser=meterToUser/secondsToUser;
	userToLinearVel=userToMeter/userToSeconds;
	linearAccelToUser=meterToUser/(secondsToUser*secondsToUser);
	userToLinearAccel=userToMeter/(userToSeconds*userToSeconds);

	userToForce=userToKilograms*userToMeter/(userToSeconds*userToSeconds);
	userToTorque=userToKilograms*userToMeter*userToMeter/(userToSeconds*userToSeconds);
	forceToUser=kilogramsToUser*meterToUser/(secondsToUser*secondsToUser);
	torqueToUser=kilogramsToUser*meterToUser*meterToUser/(secondsToUser*secondsToUser);

	if (degrees)
	{
		decimalsOrDot=5;
		decimalsOrDotDot=5;
	}
	else
	{
		decimalsOrDot=6;
		decimalsOrDotDot=6;
	}
	if (meters)
	{
		decimalsPosDot=6;
		decimalsPosDotDot=6;
	}
	else
	{
		decimalsPosDot=1;
		decimalsPosDotDot=1;
	}
}

void gv::setToDegrees()
{
	radToUser=radToDeg;
	userToRad=degToRad;
	meterOverRadToUser=meterToUser/radToUser;
	userToMeterOverRad=userToMeter/userToRad;
	leadingZeroOr=3;
	decimalsOr=2;
	leadingZeroPitch=0;
	if (meters)
		decimalsPitch=6;
	else
		decimalsPitch=3;
	degrees=true;

	angularVelToUser=radToUser/secondsToUser;
	userToAngularVel=userToRad/userToSeconds;
	angularAccelToUser=radToUser/(secondsToUser*secondsToUser);
	userToAngularAccel=userToRad/(userToSeconds*userToSeconds);

	if (seconds)
	{
		decimalsOrDot=2;
		decimalsOrDotDot=2;
	}
	else
	{
		decimalsOrDot=5;
		decimalsOrDotDot=5;
	}
}
void gv::setToRadians()
{
	radToUser=1.0f;
	userToRad=1.0f;
	meterOverRadToUser=meterToUser/radToUser;
	userToMeterOverRad=userToMeter/userToRad;
	leadingZeroOr=1;
	decimalsOr=4;
	leadingZeroPitch=0;
	if (meters)
		decimalsPitch=4;
	else
		decimalsPitch=1;
	degrees=false;

	angularVelToUser=radToUser/secondsToUser;
	userToAngularVel=userToRad/userToSeconds;
	angularAccelToUser=radToUser/(secondsToUser*secondsToUser);
	userToAngularAccel=userToRad/(userToSeconds*userToSeconds);

	if (seconds)
	{
		decimalsOrDot=4;
		decimalsOrDotDot=4;
	}
	else
	{
		decimalsOrDot=6;
		decimalsOrDotDot=6;
	}
}
void gv::setToMeters()
{
	meterToUser=1.0f;
	userToMeter=1.0f;
	meterOverRadToUser=meterToUser/radToUser;
	userToMeterOverRad=userToMeter/userToRad;
	leadingZeroPos=1;
	decimalsPos=4;
	leadingZeroPitch=0;
	if (degrees)
		decimalsPitch=6;
	else
		decimalsPitch=4;
	meters=true;

	linearVelToUser=meterToUser/secondsToUser;
	userToLinearVel=userToMeter/userToSeconds;
	linearAccelToUser=meterToUser/(secondsToUser*secondsToUser);
	userToLinearAccel=userToMeter/(userToSeconds*userToSeconds);

	userToForce=userToKilograms*userToMeter/(userToSeconds*userToSeconds);
	userToTorque=userToKilograms*userToMeter*userToMeter/(userToSeconds*userToSeconds);
	forceToUser=kilogramsToUser*meterToUser/(secondsToUser*secondsToUser);
	torqueToUser=kilogramsToUser*meterToUser*meterToUser/(secondsToUser*secondsToUser);

	if (seconds)
	{
		decimalsPosDot=4;
		decimalsPosDotDot=4;
	}
	else
	{
		decimalsPosDot=6;
		decimalsPosDotDot=6;
	}
	densityToUser=kilogramsToUser/(meterToUser*meterToUser*meterToUser);
	userToDensity=meterToUser*meterToUser*meterToUser/kilogramsToUser;
	if (kilograms)
		decimalsDensity=1;
	else
		decimalsDensity=0;
}

void gv::setToMilimeters()
{
	meterToUser=1000.0f;
	userToMeter=0.001f;
	meterOverRadToUser=meterToUser/radToUser;
	userToMeterOverRad=userToMeter/userToRad;
	leadingZeroPos=3;
	decimalsPos=2;
	leadingZeroPitch=0;
	if (degrees)
		decimalsPitch=3;
	else
		decimalsPitch=1;
	meters=false;

	linearVelToUser=meterToUser/secondsToUser;
	userToLinearVel=userToMeter/userToSeconds;
	linearAccelToUser=meterToUser/(secondsToUser*secondsToUser);
	userToLinearAccel=userToMeter/(userToSeconds*userToSeconds);

	userToForce=userToKilograms*userToMeter/(userToSeconds*userToSeconds);
	userToTorque=userToKilograms*userToMeter*userToMeter/(userToSeconds*userToSeconds);
	forceToUser=kilogramsToUser*meterToUser/(secondsToUser*secondsToUser);
	torqueToUser=kilogramsToUser*meterToUser*meterToUser/(secondsToUser*secondsToUser);

	if (seconds)
	{
		decimalsPosDot=1;
		decimalsPosDotDot=1;
	}
	else
	{
		decimalsPosDot=1;
		decimalsPosDotDot=1;
	}
	densityToUser=kilogramsToUser/(meterToUser*meterToUser*meterToUser);
	userToDensity=meterToUser*meterToUser*meterToUser/kilogramsToUser;
	if (kilograms)
		decimalsDensity=8;
	else
		decimalsDensity=4;
}

std::string gv::getTimeStr(float timeInSeconds,int additionalDecimals)
{
	return(tt::FNb(leadingZeroTime,timeInSeconds*secondsToUser,decimalsTime+additionalDecimals,false));
}

std::string gv::getFrequencyStr(float frequInHerz)
{
	int decimals=3;
	if (!seconds)
		decimals=6;
	return(tt::FNb(leadingZeroTime,frequInHerz/secondsToUser,decimals,false));
}

std::string gv::getHourMinuteSecondMilisecondStr(float timeInSeconds)
{
	std::string tmp(tt::FNb(2,int(timeInSeconds/3600.0f),false));
	tmp+=":";
	tmp+=tt::FNb(2,int(CMath::robustFmod(timeInSeconds/60.0f,60.0f)),false)+":";
	tmp+=tt::FNb(2,int(CMath::robustFmod(timeInSeconds,60.0f)),false)+".";
	tmp+=tt::FNb(2,int(CMath::robustFmod(timeInSeconds*100.0f,100.0f)),false);
	return(tmp);
}

std::string gv::getAngleStr(bool sign,float angleInRad,int leadingZeros)
{ // if leadingZeros==-1, the default leading zeros are used
	if (leadingZeros==-1)
		leadingZeros=leadingZeroOr;
	return(tt::FNb(leadingZeros,angleInRad*radToUser,decimalsOr,sign));
}

std::string gv::getSizeStr(bool sign,float sizeInMeter,int leadingZeros)
{ // if leadingZeros==-1, the default leading zeros are used
	if (leadingZeros==-1)
		leadingZeros=leadingZeroPos;
	return(tt::FNb(leadingZeros,sizeInMeter*meterToUser,decimalsPos,sign));
}

std::string gv::getMassStr(float massInKilograms)
{
	return(tt::FNb(0,massInKilograms*kilogramsToUser,decimalsMass,false));
}

std::string gv::getDensityStr(float densityInKilogramsByVolume)
{
	return(tt::FNb(0,densityInKilogramsByVolume*densityToUser,decimalsDensity,false));
}

std::string gv::getInertiaOverMassStr(float inertia)
{
	int decimals=6;
	if (!meters)
		decimals=3;
	return(tt::FNb(1,inertia*meterToUser*meterToUser,decimals,false));
}

std::string gv::getSizeOverAngleStr(bool sign,float sizeOverAngleInMeterOverRad,int leadingZeros)
{ // if leadingZeros==-1, the default leading zeros are used
	if (leadingZeros==-1)
	{
		leadingZeros=leadingZeroPos-leadingZeroOr;
		tt::limitValue(0,6,leadingZeros);
	}
	int d=decimalsPos-decimalsOr+4; // was +2 but the angular coeff in path dlg needed more
	tt::limitValue(0,6,d);
	return(tt::FNb(leadingZeros,sizeOverAngleInMeterOverRad*meterToUser/radToUser,d,sign));
}



std::string gv::getAngularVelocityStr(bool sign,float vInRadpSec)
{
	return(tt::FNb(0,vInRadpSec*angularVelToUser,decimalsOrDot,sign));
}

std::string gv::getLinearVelocityStr(bool sign,float vInMpSec)
{
	return(tt::FNb(0,vInMpSec*linearVelToUser,decimalsPosDot,sign));
}

std::string gv::getAngularAccelStr(bool sign,float vInRadpSecSec)
{
	return(tt::FNb(0,vInRadpSecSec*angularAccelToUser,decimalsOrDotDot,sign));
}

std::string gv::getLinearAccelStr(bool sign,float vInMpSecSec)
{
	return(tt::FNb(0,vInMpSecSec*linearAccelToUser,decimalsPosDotDot,sign));
}

std::string gv::getPitchStr(bool sign,float sizeInMeterOverRad,int leadingZeros)
{ // if leadingZeros==-1, the default leading zeros are used
	if (leadingZeros==-1)
		leadingZeros=leadingZeroPitch;
	return(tt::FNb(leadingZeros,sizeInMeterOverRad*meterOverRadToUser,decimalsPitch,sign));
}

std::string gv::getFloatStr(float aFloat)
{
	std::string retVal(boost::lexical_cast<std::string>(aFloat));
	return(retVal);
}

std::string gv::getIntStr(int anInt)
{
	std::string retVal(boost::lexical_cast<std::string>(anInt));
	return(retVal);
}


int gv::leadingZeroPos=1;
int gv::leadingZeroOr=3;
int gv::decimalsPos=4;
int gv::decimalsOr=2;
int gv::decimalsMass=3;
float gv::radToUser=radToDeg;
float gv::userToRad=degToRad;
float gv::meterToUser=1.0f;
float gv::userToMeter=1.0f;

int gv::leadingZeroPitch=0;
int gv::decimalsPitch=4;
float gv::meterOverRadToUser=degToRad;
float gv::userToMeterOverRad=radToDeg;

bool gv::degrees=true;
bool gv::meters=true;
bool gv::seconds=true;
bool gv::kilograms=true;

float gv::userToSeconds=1.0f;
float gv::secondsToUser=1.0f;

float gv::userToKilograms=1.0f;
float gv::kilogramsToUser=1.0f;

float gv::userToDensity=1.0f;
float gv::densityToUser=1.0f;
int gv::decimalsDensity=1;


float gv::angularVelToUser=radToDeg;
float gv::userToAngularVel=degToRad;
float gv::angularAccelToUser=radToDeg;
float gv::userToAngularAccel=degToRad;
float gv::linearVelToUser=1.0f;
float gv::userToLinearVel=1.0f;
float gv::linearAccelToUser=1.0f;
float gv::userToLinearAccel=1.0f;

float gv::userToForce=1.0f;
float gv::forceToUser=1.0f;
float gv::userToTorque=1.0f;
float gv::torqueToUser=1.0f;



int gv::leadingZeroTime=0;
int gv::decimalsTime=3;

int gv::decimalsPosDot=4;
int gv::decimalsOrDot=2;
int gv::decimalsPosDotDot=4;
int gv::decimalsOrDotDot=2;
