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

#include "VPoint.h"
#include "SoftButton.h" 

class CButtonBlockContainer;

class CButtonBlock  
{
public:
	
	CButtonBlock(int theBlockWidth,int theBlockHeight,
						   int theButtonWidth,int theButtonHeight,
						   int theView);
	virtual ~CButtonBlock();
	void commonInit();
	void displayBlock(int winSize[2],bool justCameToFront);
	CSoftButton* getButtonWithID(int id);
	CSoftButton* getButtonWithUniqueID(int id);
	CSoftButton* getButtonAtPos(int x,int y);
	CSoftButton* getButtonAtPosDontUseFastIndex(int x,int y);
	void getAllAttachedTextureProperties(std::vector<CTextureProperty*>& textPropVect);
	bool removeButtonFromPos(int x,int y,bool updateFastIndex=true);
	bool insertButton(CSoftButton* theNewButton);
	bool insertButtonWithoutChecking(CSoftButton* theNewButton);
	void setDesiredBlockPosition(int x,int y);
	int mouseDownCatch(int xCoord,int yCoord,bool& cursorCatch,bool test);
	int mouseDownCatchInexistant(int xCoord,int yCoord,bool test);
	void mouseMoveCatch(int xCoord,int yCoord);
	void setBlockSize(VPoint size);
	void getBlockSize(VPoint& size);
	void getDesiredBlockPosition(VPoint& pos);
	void getBlockPositionRelative(VPoint& pos);
	void getBlockPositionAbsolute(VPoint& pos);
	bool doesButtonIDExist(int id);
	void serialize(CSer &ar);
	void setButtonSize(VPoint theSize);
	void getButtonSize(VPoint& theSize);
	CButtonBlock* copyYourself();
	void recomputePositionFastIndex();
	void setRollupMin(VPoint rollupMinVal);
	void getRollupMin(VPoint& rollupMinVal);
	void setRollupMax(VPoint rollupMaxVal);
	void getRollupMax(VPoint& rollupMaxVal);
	void initializeInitialValues(bool simulationIsRunning);
	void simulationAboutToStart();
	void simulationEnded();
	bool isButtonInVisibleZone(CSoftButton* it);
	void setBlockID(int newBlockID);
	int getBlockID();
	void setViewToAppearOn(int theView);
	int getViewToAppearOn();
	int getFreeButtonHandle();
	void perform3DObjectLoadingMapping(std::vector<int>* map);
	void performTextureObjectLoadingMapping(std::vector<int>* map);
	bool announce3DObjectWillBeErased(int objectID,bool copyBuffer);
	void setTextureDependencies();
	void removeAllVisionSensorTextures();
	void removeAllObjectAttachements();

	void setBlockName(std::string name);
	void setGenericDialog(bool genericDialog);
	bool isGenericDialog();
	std::string getBlockName();
	bool isDisplayedNow();
	int getObjectIDAttachedTo();
	void setObjectIDAttachedTo(int objID);
	int getCaughtElements();
	void clearCaughtElements(int keepMask);

	void setLastEventButtonID(int id,int auxVals[2]);
	int getLastEventButtonID(int auxVals[2]);
	void setAttributes(int attr);
	int getAttributes();
	int getLifeID();
	CTextureProperty* getTextureProperty();
	void setTextureProperty(CTextureProperty* tp);

	// Variables which need to be serialized & copied
	int blockID;
	int viewToAppearOn;
	int blockWidth;
	int blockHeight;

	int buttonWidth;
	int buttonHeight;

	std::vector<CSoftButton*> allButtons;

protected:
	VPoint _getBlockSizeAndOtherButtonSizeAndPos(VPoint& blockSize,VPoint& blockPos,VPoint& buttonSize,VPoint& buttonPos,VPoint& otherButtonSize,CSoftButton* button);

	// Variables which need to be serialized & copied
	int _attributes;
	VPoint blockPosition;
	VPoint desiredBlockPosition;
	VPoint rollupMin;
	VPoint rollupMax;
	std::string blockName;
	int objectIDAttachedTo;
	CTextureProperty* _textureProperty;


	// Other:
	std::vector<int> positionFastIndex;
	bool _genericDialog;

	// Don't serialize:
	int _copyPositionOffset;
	int _caughtElements;
	int _lastEventButtonID;
	int _lastEventButtonAuxVals[2];

	bool _initialValuesInitialized;
	int _initialAttributes;
	int _initialPos[2];
	int _lifeID;
};
