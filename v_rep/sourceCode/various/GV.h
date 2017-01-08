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

#include <string>

enum {
	sim_miliseconds=0,
	sim_seconds
};

//FULLY STATIC CLASS
class gv  
{
public:
	static void setToDegrees();
	static void setToRadians();
	static void setToMeters();
	static void setToMilimeters();
	static void setToSeconds();
	static void setToMiliseconds();
	static void setToKilograms();
	static void setToGrams();
	static std::string getAngleStr(bool sign,float angleInRad,int leadingZeros=-1);
	static std::string getSizeStr(bool sign,float sizeInMeter,int leadingZeros=-1);
	static std::string getMassStr(float massInKilograms);
	static std::string getDensityStr(float densityInKilogramsByVolume);
	static std::string getInertiaOverMassStr(float inertia);
	static std::string getSizeOverAngleStr(bool sign,float sizeOverAngleInMeterOverRad,int leadingZeros=-1);


	static std::string getAngularVelocityStr(bool sign,float vInRadpSec);
	static std::string getLinearVelocityStr(bool sign,float vInMpSec);

	static std::string getAngularAccelStr(bool sign,float vInRadpSecSec);
	static std::string getLinearAccelStr(bool sign,float vInMpSecSec);



	static std::string getPitchStr(bool sign,float sizeInMeterOverRad,int leadingZeros=-1);
	static std::string getFloatStr(float aFloat);
	static std::string getIntStr(int anInt);
	static std::string getTimeStr(float timeInSeconds,int additionalDecimals=0);
	static std::string getFrequencyStr(float frequInHerz);
	static std::string getHourMinuteSecondMilisecondStr(float timeInSeconds);


	static std::string getAngleUnitStr();
	static std::string getSizeUnitStr();
	static std::string getSizeUnitStrShort();
	static std::string getMassUnitStr();
	static std::string getMassUnitStrShort();
	static std::string getSurfaceUnitStr();
	static std::string getVolumeUnitStr();
	static std::string getTimeUnitStr();
	static std::string getTimeUnitStrShort();
	static std::string getIntUnitStr();
	static std::string getFloatUnitStr();
	static std::string getBoolUnitStr();
	static std::string getCountUnitStr();
	static std::string getBoolStateUnitStr();
	static std::string getUserUnitStr();
	static std::string getNullUnitStr();
	static std::string formatUnitStr(const std::string& txt,const std::string& s,bool inParenthesis=true,bool startWithSpace=true);

	static int leadingZeroPos;
	static int leadingZeroOr;
	static int leadingZeroPitch;
	static int leadingZeroTime;
	static int decimalsPos;
	static int decimalsOr;
	static int decimalsPitch;
	static int decimalsTime;
	static int decimalsMass;
	static int decimalsDensity;
	static float radToUser;
	static float userToRad;
	static float meterToUser;
	static float userToMeter;
	static float meterOverRadToUser;
	static float userToMeterOverRad;


	static float angularVelToUser;
	static float userToAngularVel;
	static float angularAccelToUser;
	static float userToAngularAccel;
	static float linearVelToUser;
	static float userToLinearVel;
	static float linearAccelToUser;
	static float userToLinearAccel;
	static float userToForce;
	static float forceToUser;
	static float userToTorque;
	static float torqueToUser;

	static bool degrees;
	static bool meters;
	static bool seconds;
	static bool kilograms;
	static float userToSeconds;
	static float secondsToUser;
	static float userToKilograms;
	static float kilogramsToUser;
	static float userToDensity;
	static float densityToUser;

	static int decimalsPosDot;
	static int decimalsOrDot;
	static int decimalsPosDotDot;
	static int decimalsOrDotDot;
};
