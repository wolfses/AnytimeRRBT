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

class CSimRecorder
{
public:
	CSimRecorder(const char* initialPathAndFilename);
	virtual ~CSimRecorder();

	bool recordFrameIfNeeded(int resX,int resY,int posX,int posY);
	bool getIsRecording();
	bool startRecording(bool manualStart);
	void stopRecording(bool manualStop);
	void setRecordEveryXRenderedFrame(int x);
	int getRecordEveryXRenderedFrame();
	void setFrameRate(int fps);
	int getFrameRate();
	int getFrameCount();
	void setFilenameAndPath(const char* filenameAndPath);
	void setRecorderEnabled(bool e);
	bool getRecorderEnabled();
	std::string getFilenameAndPath();
	void setShowCursor(bool show);
	bool getShowCursor();
	void setShowButtonStates(bool show);
	bool getShowButtonStates();
	bool getManualStart();
	bool getHideInfoTextAndStatusBar();
	void setHideInfoTextAndStatusBar(bool hide);
	void setRecordingSizeChanged(int newXsize,int newYsize);
	void getRecordingSize(int& x, int&y);
	void setAutoFrameRate(bool a);
	bool getAutoFrameRate();

	void setEncoderIndex(int index);
	int getEncoderIndex();
	std::string getEncoderString(int index);

	void setShowSavedMessage(bool s);

protected:

	bool _isRecording;
	int _frameCount;
	int _simulationFrameCount;
	int _recordEveryXRenderedFrame;
	bool _initFailed;
	int _frameRate;
	int _resolution[2];
	int _resolutionInfo[2];
	int _position[2];
	bool _recorderEnabled;
	bool _showCursor;
	bool _showButtonStates;
	bool _hideInfoTextAndStatusBar;
	bool _autoFrameRate;
	int _startedInInstance;
	std::string _filenameAndPath; // no extension
	std::string _filenameAndPathAndExtension;
	int _encoderIndex;
	bool _showSavedMessage;
	float _simulationTimeOfLastFrame;

	bool _aviGenInitialized;

	BYTE* _buffer;
	BYTE* _tempBuffer;

	void _blendVrepMessageIn();
	BYTE* _vrepMessageBitmap;
	int _vrepMsgRes[2];
};
