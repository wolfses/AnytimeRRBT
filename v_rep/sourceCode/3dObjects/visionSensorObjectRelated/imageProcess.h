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

class CImageProcess  
{
public:
	CImageProcess();
	virtual ~CImageProcess();
	static float* createRGBImage(int resX,int resY);
	static void copyRGBImage(int resX,int resY,float* imageSource,float* imageDest);
	static void clearRGBImage(int resX,int resY,float* image,float clearRed,float clearGreen,float clearBlue);

	static float* createIntensityImage(int resX,int resY);
	static void copyIntensityImage(int resX,int resY,float* intensImageSource,float* intensImageDest);
	static void clearIntensityImage(int resX,int resY,float* intensImage,float clearIntens);

	static void filter3x3RgbImage(int resX,int resY,float* rgbIn,float* rgbOut,float m[9]);
	static void filter5x5RgbImage(int resX,int resY,float* rgbIn,float* rgbOut,float m[25]);
	static void clampRgbImage(int resX,int resY,float* rgbImage,float lowClamp,float highClamp);

	static void deleteImage(float* image);

	static void rgbImageToIntensityImage(int resX,int resY,float* rgbImage,float* intensImage);
	static void intensityImageToRGBImage(int resX,int resY,float* rgbImage,float* intensImage);

	static void getEdges(int resX,int resY,float* imageSource,float* imageDest);
	static void scaleIntensity(int resX,int resY,float* image,float scaleFactor,bool clampToMax);
	static void scaleRGB(int resX,int resY,float* image,float redFactor,float greenFactor,float blueFactor,bool clampToMax);
	static void boxBlurIntensity(int resX,int resY,float* imageSource,float* imageDest);
	static void boxBlurRGB(int resX,int resY,float* imageSource,float* imageDest);
	static void blurIntensity(int resX,int resY,float* image);
	static void blurRGB(int resX,int resY,float* image);
	static void horizontalFlipRGB(int resX,int resY,float* image);
	static void verticalFlipRGB(int resX,int resY,float* image);

	static void scaleRgbImageWithIntensityImage(int resX,int resY,float* rgbImage,float* intensImage);

	static void keepThresholdIntensity(int resX,int resY,float* image,float thresholdVal,bool keepAbove);
	static void keepThresholdRGB(int resX,int resY,float* image,float thresholdVal,bool keepAbove);
	static void nonZeroToOneIntensity(int resX,int resY,float* image);
	static void nonZeroToOneRGB(int resX,int resY,float* image);
	static void addImagesRGB(int resX,int resY,float* imageInOut,float* imageOverlay);
	static void clampToOneRGB(int resX,int resY,float* image);

	static void predef_lightBlurRGB(int resX,int resY,float* image);
	static void predef_heavyBlurRGB(int resX,int resY,float* image);
	static void predef_getThinEdgesRGB(int resX,int resY,float* image);
	static void predef_getThickEdgesRGB(int resX,int resY,float* image);
	static void predef_getThinEdgeOverlayRGB(int resX,int resY,float* image);
	static void predef_invertRGB(int resX,int resY,float* image);
};
