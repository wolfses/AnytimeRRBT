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

#include "vrepPrecompiledHeader.h"
#include "3DObject.h"

#define HIERARCHY_INTER_LINE_SPACE 18
#define HIERARCHY_HALF_INTER_LINE_SPACE 9
#define HIERARCHY_TEXT_CENTER_OFFSET (HIERARCHY_HALF_INTER_LINE_SPACE-4)
#define HIERARCHY_ICON_WIDTH 20
#define HIERARCHY_ICON_HEIGHT 16
#define HIERARCHY_ICON_HALF_WIDTH 10
#define HIERARCHY_ICON_HALF_HEIGHT 8
#define HIERARCHY_ICON_QUARTER_WIDTH 5
#define HIERARCHY_ICON_QUARTER_HEIGHT 4

#define HIERARCHY_INTER_ICON_SPACING 4
/*
#define HIERARCHY_INTER_LINE_SPACE 36
#define HIERARCHY_HALF_INTER_LINE_SPACE 18
#define HIERARCHY_TEXT_CENTER_OFFSET (HIERARCHY_HALF_INTER_LINE_SPACE-8)
#define HIERARCHY_ICON_WIDTH 40
#define HIERARCHY_ICON_HEIGHT 32
#define HIERARCHY_ICON_HALF_WIDTH 20
#define HIERARCHY_ICON_HALF_HEIGHT 16
#define HIERARCHY_ICON_QUARTER_WIDTH 10
#define HIERARCHY_ICON_QUARTER_HEIGHT 8

#define HIERARCHY_INTER_ICON_SPACING 8
*/
class CHierarchy;

class CHierarchyElement  
{
public:
	CHierarchyElement(int theObjectID);
	virtual ~CHierarchyElement();
	int addYourChildren();
	int getLinkedObjectID();
	int getNumberOfElements();
	int computeNumberOfElements();
	CHierarchyElement* getElementLinkedWithObject(int objID);

	void renderElement_3DObject(CHierarchy* hier,int labelEditObjectID,bool& bright,bool dontDisplay,
		int renderingSize[2],int textPos[2],
		int indentNb,std::vector<int>* vertLines,int minRenderedPos[2],int maxRenderedPos[2],bool forDragAndDrop=false,int transparentForTreeObjects=-1,int dropID=-2,int worldClick=-9999);
	void renderElement_editModeList(CHierarchy* hier,int labelEditObjectID,bool& bright,bool dontDisplay,
		int renderingSize[2],int textPos[2],int indentNb,int minRenderedPos[2],int maxRenderedPos[2],
		int selectedState,int editModeType);
	static bool renderDummyElement(bool& bright,int renderingSize[2],int textPos[2]);
	void setSceneName(const std::string& sn);
	bool isLocalWorld();

	std::vector<CHierarchyElement*> children;

private:
	int _drawIcon_3DObject(CHierarchy* hier,int tPosX,int tPosY,C3DObject* it,int pictureID,bool drawIt,float transparencyFactor,bool forDragAndDrop);
	int _drawIcon_editModeList(CHierarchy* hier,int tPosX,int tPosY,int pictureID,bool drawIt);
	void _drawTexturedIcon(int tPosX,int tPosY,int sizeX,int sizeY,float transparencyFactor);
	int objectID;
	std::string _sceneName;
	int numberOfElements;
};
