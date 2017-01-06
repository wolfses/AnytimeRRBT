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

// This file includes many many very ugly things and requires some serious refactoring!!!!

#pragma once
#include <vector>
#include "TextureProperty.h"
#include "VPoint.h"

enum {	AMBIENT_MODE=0,
		DIFFUSE_MODE,
		SPECULAR_MODE,
		EMISSION_MODE
};

struct SOglFont
{
	std::string fontName;
	int fontHeight;
	unsigned int fontBase;
	int fontWidths[256];
};

//FULLY STATIC CLASS
class ogl  
{
public:
	static void drawReference(float size,float brightness=0.8f,bool line=true,bool setColors=true);

	static void setColorsAllBlack();
	static void setColor(float r,float g,float b,short colorMode);
	static void setBlendedColor(float r,float g,float b,int colorMode,float weight,float r2=0.5f,float g2=0.5f,float b2=0.5f);
	static void setColor(const float rgb[3],short colorMode);
	static void setBlendedColor(float rgb[3],int colorMode,float weight,float rgb2[3]);
	static void setRichTextColor(float col[3]);
	static void getRichTextColor(float col[3]);
	static void setRichTextDefaultColor(float col[3]);
	static void getRichTextDefaultColor(float col[3]);
	static void setColor(const float ambientRgb[3],const float diffuseRgb[3],const float specularRgb[3],const float emissiveRgb[3],int shininess,float alpha=1.0f);
	static void setColor(const float allColors[12],int shininess,float alpha);
	static void lockColors(bool lock);
	static void setShininess(int shin);
	static void setBlending(bool on,int param1=0,int param2=0);
	static void allLightsOffAmbientFull();

	static void displayBox(float x,float y,float z,bool solid);
	static void displayCylinder(float d,float l,BYTE div,BYTE openEnds=0,bool smooth=true);
	static void displayDisk(float d,BYTE div);
	static void displayCone(float d,float l,BYTE div,bool openEnds=false,bool smooth=true);
	static void displaySphere(float r,BYTE sides=10,BYTE faces=5,bool smooth=true);


	static void perspectiveSpecial(float fovy,float aspect,float zNear,float zFar);
	static void frustumSpecial(float left,float right,float bottom,float top, 
								float nearval,float farval);
	static void pickMatrixSpecial(int x,int y,int width,int height,
								int viewport[4]);

	static void drawQuad(int posX,int posY,int sizeX,int sizeY,float col[3]);
	static void drawBorder(int posX,int posY,int sizeX,int sizeY,float col[3],int type,int borderSize=3);

	static void drawOutlineText(const char* txt,const C7Vector& trOrig,float textHeight,const C4X4Matrix* cameraAbsConfig,bool overlay,bool left,bool right,bool culling,float* textColor,float* backColor=NULL,bool sizeInPixels=false,int windowHeight=0,float verticalViewSizeOrAngle=0.0f,bool perspective=true,bool fullyFacingCamera=true);
	static void drawText(int posX,int posY,int posZ,std::string txt,bool verticallyCentered=false);
	static void drawBitmapTextBackgroundIntoScene(float posX,float posY,float posZ,std::string txt);
	static void drawBitmapTextIntoScene(float posX,float posY,float posZ,const std::string& txt);
	static void drawTexti(int posX,int posY,int posZ,std::string txt);
	static void drawCenteredText(int posX,int posY,int posZ,std::string txt,bool verticallyCentered=false);
	static int getTextLengthInPixels(const std::string& txt);
	static std::string getTextThatFitIntoPixelWidth(std::vector<std::string>& separateWords,std::vector<int>& widths,int spaceWidth,int pixelWidth,int& textCharCount_pixelWidth,bool allowWordCut);
	static int getSeparateWords(const std::string& txt,std::vector<std::string>& separateWords);
	static int getSeparateWordWidths(const std::vector<std::string>& separateWords,std::vector<int>& widths);
	static int getFontHeight();
	static int getFontCharWidth(unsigned char c);
	static int getInterline();

	static void loadBitmapFont(const unsigned char* fontData,int fontHeight,int theFontIndex);
	static void loadOutlineFont(const int* fontData,const float* fontVertices);

	static void freeBitmapFonts();
	static void freeOutlineFont();
	static void selectBitmapFont(int index);

	static void drawButton(VPoint p,VPoint s,float txtColor[3],float _bckgrndColor1[3],
		float _bckgrndColor2[3],std::string text,int buttonTypeAndAttributes,
		bool editing,int editionPos,float sliderPos,bool vertical,int timeInMs,float secondTextColor[3],CTextureProperty* backgroundTexture,VPoint* backgroundPos,VPoint* backgroundSize,CTextureProperty* foregroundTexture);

	static int getMultilineTextInfo(const std::string& text,std::vector<std::string>& lines,int* textMaxWidth=NULL,int* textHeight=NULL,int* charHeight=NULL);
	static int getRichTextInfo(std::string& text,std::vector<int>& iconsAndPos);
	static void drawRichText(int posX,int posY,int size,std::string& text,std::vector<int>& iconsAndPos,int alignement,float backgroundColor[3],float secondTextColor[3]);
	static void drawRichTextIcon(int centerPosX,int centerPosY,int size,int icon);

	static void drawButtonEdit(VPoint p,VPoint s,bool selected,bool mainSel);
private:
	static void drawText(std::string t);
	static void setTextPosition(float x,float y,float z);

	static bool colorsLocked;
	static int fontIndex;
	static char opacBitmap[10000];

	static unsigned int outlineFontBase;
	static float outlineFontSizes[256];
	static float outlineFontHeight;
	static float outlineFontCenter;


	static float currentRichTextColor[3];
	static float defaultRichTextColor[3];


	static SOglFont oglFonts[];
public:
	static float HIERARCHY_AND_BROWSER_LAST_SELECTION_COLOR[3];
	static float HIERARCHY_NOT_LAST_SELECTION_COLOR_BRIGHT[3];
	static float HIERARCHY_NOT_LAST_SELECTION_COLOR_DARK[3];
	static float HIERARCHY_COLLECTION_SELECTION_COLOR_BRIGHT[3];
	static float HIERARCHY_COLLECTION_SELECTION_COLOR_DARK[3];
	static float HIERARCHY_DROP_LOCATION_COLOR[3];
	static float HIERARCHY_WORLD_CLICK_COLOR[3];

	static float HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_BRIGHT[3];
	static float HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_DARK[3];
	static float HIERARCHY_AND_BROWSER_TEXT_COLOR_VISIBLE[3];
	static float HIERARCHY_AND_BROWSER_TEXT_COLOR_INVISIBLE[3];
	static float HIERARCHY_UNACTIVE_WORLD_COLOR[3];

	static float HIERARCHY_AND_BROWSER_LINE_COLOR[3];
	static float HIERARCHY_AND_BROWSER_SCROLLBAR_BACK_COLOR[3];
	static float HIERARCHY_AND_BROWSER_SCROLLBAR_COLOR[3];

	static float TITLE_BAR_COLOR[3];
	static float TITLE_BAR_BUTTON_COLOR[3];
	static float SEPARATION_LINE_COLOR[3];

	static float MANIPULATION_MODE_OVERLAY_COLOR[4];
	static float MANIPULATION_MODE_OVERLAY_GRID_COLOR[4];

	static float HIERARCHY_NO_SELECTION_RED_BRIGHT[3];
	static float HIERARCHY_NO_SELECTION_RED_DARK[3];

	static float HIERARCHY_NO_SELECTION_GREEN_BRIGHT[3];
	static float HIERARCHY_NO_SELECTION_GREEN_DARK[3];

	static float HIERARCHY_NO_SELECTION_BLUE_BRIGHT[3];
	static float HIERARCHY_NO_SELECTION_BLUE_DARK[3];
};
