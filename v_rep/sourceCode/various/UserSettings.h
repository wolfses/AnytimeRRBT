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

#include "MainCont.h"
#include "VisualParam.h"
#include "ViewableBase.h"

class CUserSettings
{
public:
	CUserSettings();
	virtual ~CUserSettings();

	void applyRequestedDefaultPaths();

	void setUndoRedoEnabled(bool isEnabled);
	bool getUndoRedoEnabled();

	void saveUserSettings();
	void loadUserSettings();

	CVisualParam groupSelectionColor;
	bool displayWorldReference;
	bool useGlFinish;
	bool useGlFinish_visionSensors;
	bool oglCompatibilityTweak1;
	float stereoDist;
	int vsync;
	int workThreadCount;
	bool debugOpenGl;
	bool identicalVerticesCheck;
	float identicalVerticesTolerance;
	bool identicalTrianglesCheck;
	bool identicalTrianglesWindingCheck;
	bool compressFiles;
	int triCountInOBB;
	bool saveApproxNormals;
	bool packIndices;
	int undoRedoLevelCount;
	int undoRedoMaxBufferSize;
	bool alwaysShowConsole;
	bool forceBugFix_rel30002;
	bool allowTransparentDialogs;
	bool statusbarInitiallyVisible;
	float dialogTransparencyFactor;
	int autoSaveDelay;
	int bulletSerializationBuffer;
	int abortScriptExecutionButton;
	int timeInMsForUndoRedoTooLongWarning;
	bool forceDeprecatedFileformatLoad;
	int applicationTweak1;
	int applicationTweak2;
	int applicationTweak3;
	bool displayBoundingBoxeWhenObjectSelected;
	bool antiAliasing;
	float dynamicActivityRange;
	int freeServerPortStart;
	int freeServerPortRange;
	void setTranslationStepSize(float s);
	float getTranslationStepSize();
	void setRotationStepSize(float s);
	float getRotationStepSize();
	int renderingSurfaceVShift;
	int renderingSurfaceVResize;
	bool correctColorsAndLights;
	int threadedRenderingDuringSimulation;
	int offscreenContextType;
	int fboType;
	bool forceFboViaExt;
	int desiredOpenGlMajor;
	int desiredOpenGlMinor;
	int visionSensorsUseGuiThread_windowed;
	int visionSensorsUseGuiThread_headless;
	int fileDialogs;
	float mouseWheelZoomFactor;

	float retinaScaling1X;
	float retinaScaling1Y;
	float retinaScaling2X;
	float retinaScaling2Y;
	float retinaScaling3X;
	float retinaScaling3Y;

	bool doNotWritePersistentData;
	bool doNotShowOpenglSettingsMessage;
	bool doNotShowCrashRecoveryMessage;
	bool doNotShowUpdateCheckMessage;

	std::string scriptEditorFont;
	int scriptEditorFontSize;
	std::string auxConsoleFont;
	int auxConsoleFontSize;


	bool useBuiltInScriptEditor();

	bool getUseBuiltInScriptEditorToSave();
	void setUseBuiltInScriptEditorToSave(bool u);

	int getIdleFps();
	void setIdleFps(int fps);

	std::string externalScriptEditor;
	bool _useExternalScriptEditor;

	std::string externalScriptEditorToSave;
	bool _useExternalScriptEditorToSave;

	int guiFontSize_Win;
	int guiFontSize_Mac;
	int guiFontSize_Linux;

	std::string defaultDirectoryForScenes;
	std::string defaultDirectoryForModels;
	std::string defaultDirectoryForCadFiles;
	std::string defaultDirectoryForMiscFiles;

private:
	bool _undoRedoEnabled;
	float _translationStepSize;
	float _rotationStepSize;
	int _idleFps;
};
