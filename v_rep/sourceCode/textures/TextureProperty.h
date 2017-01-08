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
#include "TextureObject.h"
#include <vector>
#include "Ser.h"
#include "7Vector.h"

class CTextureProperty
{
public:
	CTextureProperty();
	CTextureProperty(int textureOrVisionSensorObjectID);
	virtual ~CTextureProperty();

	CTextureProperty* copyYourself();
	void serialize(CSer& ar);
	bool announceObjectWillBeErased(int objID);
	void performObjectLoadingMapping(std::vector<int>* map);
	void performTextureObjectLoadingMapping(std::vector<int>* map);
	void addTextureDependencies(int objID,int objSubID);
	void scaleObject(float scalingFactor);
	void setInterpolateColors(bool ic);
	bool getInterpolateColors();
	void setDecalTexture(bool dt);
	bool getDecalTexture();
	void adjustForFrameChange(const C7Vector& mCorrection);

	bool start3DTextureDisplay();
	void end3DTextureDisplay();
	bool start2DTextureDisplay();
	void end2DTextureDisplay();
	int getTextureObjectID();
	std::vector<float>* getTextureCoordinates(int objectStateId,const C7Vector& transf,const std::vector<float>& vertices,const std::vector<int>& triangles);
	C7Vector getTextureRelativeConfig();
	void setTextureRelativeConfig(const C7Vector& c);
	void getTextureScaling(float& x,float& y);
	void setTextureScaling(float x,float y);
	void setRepeatU(bool r);
	bool getRepeatU();
	void setRepeatV(bool r);
	bool getRepeatV();

	void setFixedCoordinates(std::vector<float>* coords); // NULL to remove them and have calculated coords
	bool getFixedCoordinates();

	void setTextureMapMode(int mode);
	int getTextureMapMode();

private:
	void _commonInit();
	CTextureObject* _startedTexture;

	// to copy and serialize:
	bool _interpolateColor;
	bool _decal;
	bool _repeatU;
	bool _repeatV;
	int _textureOrVisionSensorObjectID;
	int _textureCoordinateMode;
	C7Vector _textureRelativeConfig;
	float _textureScalingX;
	float _textureScalingY;
	std::vector<float> _fixedTextureCoordinates;

	// do not copy nor serialize:
	int _objectStateId;
	std::vector<float> _calculatedTextureCoordinates;
};
