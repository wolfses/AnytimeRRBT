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

#include "VisualParam.h"
#include "MainCont.h"
#include "Thumbnail.h"

class CCamera;

class CEnvironment : public CMainCont
{
public:
	CEnvironment();
	virtual ~CEnvironment();
	void simulationAboutToStart();
	void simulationEnded();
	void renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib);

	void setUpDefaultValues();
	void newSceneProcedure();

	void setBackgroundColor(int viewSize[2]);
	void activateAmbientLight(bool a);
	void serialize(CSer& ar,bool saveAndLoadSceneHistory);
	void setAcknowledgement(const std::string& a);
	std::string getAcknowledgement();
	void setVisualizeWirelessEmitters(bool v);
	bool getVisualizeWirelessEmitters();
	void setVisualizeWirelessReceivers(bool v);
	bool getVisualizeWirelessReceivers();
	void setShapeTexturesEnabled(bool e);
	bool getShapeTexturesEnabled();
	void set2DElementTexturesEnabled(bool e);
	bool get2DElementTexturesEnabled();
	void setShowingInertias(bool e);
	bool getShowingInertias();

	void setRequestFinalSave(bool finalSaveActivated);
	bool getRequestFinalSave();
	void setSceneLocked();
	bool getSceneLocked();
	void setCalculationMaxTriangleSize(float s);
	float getCalculationMaxTriangleSize();
	void setCalculationMinRelTriangleSize(float s);
	float getCalculationMinRelTriangleSize();
	void setSaveExistingCalculationStructures(bool s);
	bool getSaveExistingCalculationStructures();

	void setEnableCustomContactHandlingViaScript(bool c,const char* scriptContent);
	bool getEnableCustomContactHandlingViaScript();

	int getSceneUniqueID();

	void setSaveExistingCalculationStructuresTemp(bool s);
	bool getSaveExistingCalculationStructuresTemp();

	void setSceneCanBeDiscardedWhenNewSceneOpened(bool canBeDiscarded);
	bool getSceneCanBeDiscardedWhenNewSceneOpened();

	quint64 autoSaveLastSaveTimeInSecondsSince1970;
	bool fogEnabled;
	float fogDensity;
	float fogStart;
	float fogEnd;
	int fogType; // 0=linear, 1=exp, 2=exp2
	float fogBackgroundColor[3];
	float backGroundColor[3];
	float backGroundColorDown[3];
	float ambientLightColor[3];
	CVisualParam wirelessEmissionVolumeColor;
	CVisualParam wirelessTransmissionColor;

	std::vector<DWORD> sceneHistory;
	CThumbnail modelThumbnail_notSerializedHere;

protected:
	std::string _acknowledgement;
	bool _visualizeWirelessEmitters;
	bool _visualizeWirelessReceivers;
	bool _requestFinalSave;
	bool _sceneIsLocked;
	bool _shapeTexturesEnabled;
	bool _showingInertias;
	bool _2DElementTexturesEnabled;
	float _calculationMaxTriangleSize;
	float _calculationMinRelTriangleSize;
	bool _saveExistingCalculationStructures;
	bool _saveExistingCalculationStructuresTemp;
	bool _sceneCanBeDiscardedWhenNewSceneOpened;
	bool _useCustomContactHandlingViaScript;

	int _sceneUniqueID;

	static int _nextSceneUniqueID;
};
