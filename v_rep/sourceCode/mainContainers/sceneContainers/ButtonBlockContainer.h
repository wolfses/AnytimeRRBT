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

#include "ButtonBlock.h" 
#include <vector>
#include "MainCont.h"
#include "VMenubar.h"
#include "VPoint.h"


const int INFO_BOX_ROW_COUNT=20;

class CButtonBlockContainer : public CMainCont
{
public:

	CButtonBlockContainer(bool mainContainer);
	virtual ~CButtonBlockContainer();

	void newSceneProcedure(bool mainContainer);
	void updateInfoWindowColorStyle(int colorStyle);
	void simulationAboutToStart();
	void simulationEnded();
	void renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib);

	CButtonBlock* getButtonBlockWithID(int id);
	bool mouseDown(int xCoord,int yCoord,int currentView,int selectionStatus);
	bool mouseDownTest(int xCoord,int yCoord,int currentView);
	bool leftMouseButtonDoubleClick(int xCoord,int yCoord,int currentView);
	bool mouseUp(int xCoord,int yCoord,int currentView);
	bool mouseMove(int xCoord,int yCoord);
	void looseFocus();
	void removeAllBlocks(bool alsoSystemBlocks);
	void displayAllBlocks(int currentView,bool dialogsHaveFocus);
	void announceObjectWillBeErased(int objID);
	void setTextureDependencies();

	void insertBlock(CButtonBlock* theNewBlock,bool objectIsACopy);
	void insertBlockWithSuffixOffset(CButtonBlock* theNewBlock,bool objectIsACopy,int suffixOffset);
	void getMinAndMaxNameSuffixes(int& minSuffix,int& maxSuffix);
	bool canSuffix1BeSetToSuffix2(int suffix1,int suffix2);
	void setSuffix1ToSuffix2(int suffix1,int suffix2);

	CButtonBlock* getBlockWithID(int id);
	CButtonBlock* getBlockWithName(std::string name);
	bool removeBlockFromID(int id);
	void setEditBoxEdition(int block,int button,bool applyChangesIfLeavingEditMode);
	bool getEditBoxEdition();
	void onKeyDown(DWORD key);
	void setButtonEditMode(bool isOn);
	bool getButtonEditMode();
	bool doesBlockIDExist(int id);
	void deselectButtons();
	void addToSelection(int pos);
	void removeFromSelection(int pos);
	void xorAddToSelection(int pos);
	bool isInSelection(int pos);
	void setViewSizeAndPosition(int sX,int sY,int pX,int pY);
	CSoftButton* getInfoBoxButton(int index,int subIndex);
	CButtonBlockContainer* loadSystemButtonBlocks(std::string fullPathAndFilename);

	void a3DObjectWasSelected(int objID);
	void sendBlockToFront(int blockID);
	int getNonSystemBlockCount();
	int getUnassociatedNonSystemBlockCount();

	void _copyBlockInEdition();
	void _pasteCopiedBlock();
	void _deleteBlockInEdition();
	void addMenu(VMenu* menu);
	bool evaluateMenuSelection(int commandID);

	void setBlockInEdition(int blockID);
	int getBlockInEdition();

	int getCaughtElements();
	void clearCaughtElements(int keepMask);

	int winSize[2];
	int winPos[2];
	int caughtBlock;
	int caughtButton;

	int caughtBlockForDownUpEvent;
	int caughtButtonForDownUpEvent;
	bool caughtButtonDownForDownUpEvent;

	int editBoxInEditionBlock;
	int editBoxInEditionButton;
	int editBoxEditionPosition;
	std::string editBoxEditionText;
	bool caughtButtonDown;
	bool mouseCaptured;
	VPoint mousePos;
	bool editMode;
	bool buttonsLocked;
	std::vector<int> selectedButtons;
	std::vector<CButtonBlock*> allBlocks;
	int shiftSelectionStart;


private:
	int _blockInEditMode;
	CButtonBlock* copyOfBlock_forEditMode;
	CButtonBlock* infoBox;
	int infoBoxButtonHandlers[INFO_BOX_ROW_COUNT*2];

	int newSceneProcedurePasses;
	int _lastBlockIdInFront;
};
