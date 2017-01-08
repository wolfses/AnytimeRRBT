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
#include <vector>

// FULLY STATIC CLASS
class tt  
{
public:

	static bool stringToFloat(const char* txt,float& f,bool allowNegativeValue,bool infGivesMinusOne);
	static std::string floatToString(float f,bool canBeNegative);
	static bool stringToInt(const char* txt,int& a);
	static std::string intToString(int a);
	static float floatToUserFloat(float f,float toUserConversion,bool minusValuesGiveInf);
	static float userFloatToFloat(float userFloat,float fromUserConversion,bool minusValuesGiveInf);

	static std::string decorateString(const char* prefix,const std::string mainText,const char* suffix);

	static void removeComments(std::string& line);
	static bool removeSpacesAtBeginningAndEnd(std::string& line);
	static bool removeSpacesAndEmptyLinesAtBeginningAndEnd(std::string& line);

	static std::string FNb(float number);
	static std::string FNb(int number);
	static std::string FNb(int leadingZeros,float number,int decimals,bool sign=true);
	static std::string FNb(int leadingZeros,int number,bool sign=false);
	static int getDecimalPos(float number,int maxDec);

	static void lightEncodeBuffer(char* buff,int length);
	static void lightDecodeBuffer(char* buff,int length);

	static float getAngleMinusAlpha(float angle,float alpha);
	static float getCyclicAngleDifference(float angleBigger,float angleSmaller,float range);

	static int getSpecialScale(int input);

	static double getLimitedDouble(double minValue,double maxValue,double value);
	static void limitValue(float minValue,float maxValue,float value[2]);
	static void limitValue(int minValue,int maxValue,int* value);
	static bool getValidFloat(const std::string& text,float& value);
	static bool getValidInt(const std::string& text,int& value);

// Name handling and copy handling:
//************************************************************
	// Following is for copied objects:
	static std::string generateNewName_dash(const std::string& name);
	static std::string generateNewName_dash(const std::string& name,int suffixOffset);

	// Following is for copied objects:
	static std::string generateNewName_noDash(const std::string& name);

	// Following is for both:
	static int getNameSuffixNumber(const char* name,bool dash);
	static std::string getNameWithoutSuffixNumber(const char* name,bool dash);
	static bool removeIllegalCharacters(std::string& text,bool allowOneDashFollowedByNumbers);	
//************************************************************


	static bool extractWord(std::string& line,std::string& word);
	static bool extractCommaSeparatedWord(std::string& line,std::string& word);

	static void addToFloatArray(std::vector<float>* ar,float x,float y,float z);
	static void addToIntArray(std::vector<int>* ar,int x,int y,int z);
	
	static void rgbToHsl(float rgb[3],float hsl[3]);
	static void hslToRgb(float hsl[3],float rgb[3]);

	static void orderAscending(std::vector<int>& toBeOrdered,std::vector<int>& index);
	static void orderAscending(std::vector<float>& toBeOrdered,std::vector<int>& index);
	static void orderStrings(std::vector<std::string>& toBeOrdered,std::vector<int>& index);

	static void getColorFromIntensity(float intensity,int colorTable,float col[3]);

private:
	static float _hueToRgb(float m1,float m2,float h);

public:
	static float getNormalizedAngle(float angle);
	static float getNormalizedAngleEx(float angle,float range);

	static int getLimitedInt(int minValue,int maxValue,int value);
	static void limitValue(int minValue,int maxValue,int &value);
	static float getLimitedFloat(float minValue,float maxValue,float value);
	static void limitValue(float minValue,float maxValue,float &value);
};
