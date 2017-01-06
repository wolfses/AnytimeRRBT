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

// This file requires some serious refactoring!!

#pragma once

#include "TextureProperty.h"
#include "VPoint.h"

class CSoftButton  
{
public:
	CSoftButton(std::string theLabel,int w,int h,int theLength,int theHeight);
	virtual ~CSoftButton();

	void announce3DObjectWillBeErased(int objID);
	void performTextureObjectLoadingMapping(std::vector<int>* map);
	void perform3DObjectLoadingMapping(std::vector<int>* map);
	void setTextureDependencies(int buttonBlockID);

	float backgroundColor[3];
	float downBackgroundColor[3];
	float textColor[3];
	std::string label;
	std::string downLabel;
	int buttonID;
	int xPos;
	int yPos;

	void serialize(CSer& ar);
	int getLifeID();

	void setSliderPos(float pos);
	float getSliderPos();
	void setVertical(bool isVertical);
	bool getVertical();
	void initializeInitialValues(bool simulationIsRunning);
	void simulationEnded();

	int getButtonType();
	void enableArray(bool enable);
	bool isArrayEnabled();
	bool setArrayColor(int x,int y,const float col[3]);
	void drawArray(VPoint p,VPoint s);
	int getLength();
	void adjustLength(int newLength);
	int getHeight();
	void adjustHeight(int newHeight);
	void setAttributes(int attr);
	int getAttributes();
	int getUniqueID();
	void setTextureProperty(CTextureProperty* tp);
	void removeVisionSensorTexture();
	CTextureProperty* getTextureProperty();
	float* arrayColors; // not serialized, but copied!

	CSoftButton* copyYourself();

private:
	CTextureProperty* _textureProperty;
	int _buttonAttributes;
	int length;
	int height;

	float sliderPos;

	bool vertical; // don't serialize, it is calculated
	int _lifeID;

	bool _initialValuesInitialized;
	std::string _initialSimulation_label;
	int _initialSimulation_buttonAttributes;
	float _initialSimulation_sliderPos;

	int _buttonUniqueID;

	static int _nextButtonUniqueID;
};
