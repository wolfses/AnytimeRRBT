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

// This file is extremely ugly. This needs badly to be rewritten in a "Qt way"!!!

#pragma once

#include <QMutex>
#include "qdlgstopscripts.h"
#include "qdlgprogress.h"
#include "VMessageBox.h"
#include "offscreenGlContext.h"
#include "frameBufferObject.h"
#include "visionSensor.h"

class CUiThread : public QObject
{
	Q_OBJECT
public:
	CUiThread();
	virtual ~CUiThread();

	int convexDecompose(int shapeHandle,int options,const int* intParams,const float* floatParams);
	int getLastFrameId();
	void setLastFrameId(int fid);

	void requestSceneRender(bool bufferMainDisplayStateVariables); // not waiting

	// Following can be called from the UI or the main thread:
	WORD messageBox_informationSystemModal(QWidget* parent,const std::string& title,const std::string& message,WORD flags);
	WORD messageBox_information(QWidget* parent,const std::string& title,const std::string& message,WORD flags);
	WORD messageBox_question(QWidget* parent,const std::string& title,const std::string& message,WORD flags);
	WORD messageBox_warning(QWidget* parent,const std::string& title,const std::string& message,WORD flags);
	WORD messageBox_critical(QWidget* parent,const std::string& title,const std::string& message,WORD flags);

	std::string fileDialog(bool load,const char* title,const char* startPath,const char* initName,const char* extName,const char* ext);
	int msgBox(int boxType,int buttons,const char* title,const char* message);

	void closeScene_fromApi(bool displayMessages);
	bool loadScene_fromApi(const char* pathAndFilename,bool displayMessages,bool setCurrentDir);
	void openOrBringDlgToFront(int dlgId);
	void closeDlg(int dlgId);
	void activateMainWindow();

	int pluginLoadAndStart(const char* fileAndPath,const char* name);
	bool pluginStopAndUnload(int handle);

	std::string getOpenFileName(QWidget* parent,WORD option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2="",const std::string& extension3="",const std::string& extension4="",const std::string& extension5="",const std::string& extension6="",const std::string& extension7="",const std::string& extension8="",const std::string& extension9="",const std::string& extension10="");

	void requestSceneRender_wait();

	void createGlContextAndFboAndTextureObjectIfNeeded(void* obj,bool useStencilBuffer);

	QThread* qThread;
	bool doVariousUiTasksOnRegularBasisPending;

private:

	CQDlgStopScripts* _emergencyStopDlg;
	CQDlgProgress* _progressDlg;

//	QTimer* timer_100ms;

	int _frameId;
	bool _frame_bufferMainDisplayStateVariables;
	int _lastFrameId;


signals:

	void _closeScene_fromApi(bool displayMessages);
	void _loadScene_fromApi(bool& result,const char* pathAndFilename,bool displayMessages,bool setCurrentDir);
	void _openOrBringDlgToFront(int dlgId);
	void _closeDlg(int dlgId);
	void _activateMainWindow();
	void _createGlContextAndFboAndTextureObjectIfNeeded(void* obj,bool useStencilBuffer);

	void destroyGlTexture(unsigned int textureName);
	void setFullscreen(bool f);
	void _pluginLoadAndStart(int& result,const char* fileAndPath,const char* name);
	void _pluginStopAndUnload(bool&result,int handle);

	void _requestSceneRender_wait(bool bufferMainDisplayStateVariables);
	void _requestSceneRender(bool bufferMainDisplayStateVariables);
	void _fileDialog(std::string& nameAndPath,bool load,const char* title,const char* startPath,const char* initName,const char* extName,const char* ext);
	void _msgBox(int& retVal,int boxType,int buttons,const char* title,const char* message);
	void simulationEnded();
	void _messageBox_informationSystemModal(WORD& retVal,QWidget* parent,const std::string& title,const std::string& message,WORD flags);
	void _messageBox_information(WORD& retVal,QWidget* parent,const std::string& title,const std::string& message,WORD flags);
	void _messageBox_question(WORD& retVal,QWidget* parent,const std::string& title,const std::string& message,WORD flags);
	void _messageBox_warning(WORD& retVal,QWidget* parent,const std::string& title,const std::string& message,WORD flags);
	void _messageBox_critical(WORD& retVal,QWidget* parent,const std::string& title,const std::string& message,WORD flags);
	void addStatusLine(std::string line);
	void showEmergencyStopButtonIfNeeded(bool showState);
	void _getOpenFileName(std::string& fileAndPath,QWidget* parent,WORD option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2,const std::string& extension3,const std::string& extension4,const std::string& extension5,const std::string& extension6,const std::string& extension7,const std::string& extension8,const std::string& extension9,const std::string& extension10);

//	void prepareOffscreenContextAndFBO(COffscreenGlContext** offContext,CFrameBufferObject** fbo,int offscreenType,int resX,int resY,bool useStencilBuffer);
	void detectVisionSensorEntity(bool& returnValue,CVisionSensor* sensor,int entityID,bool detectAll,bool dontSwapImageBuffers,bool entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,bool hideEdgesIfModel,bool overrideRenderableFlagsForNonCollections);
	void _convexDecompose(int& retVal,int shapeHandle,int options,const int* intParams,const float* floatParams);
	void doVariousUiTasksOnRegularBasis();

private slots:
	void __closeScene_fromApi(bool displayMessages);
	void __loadScene_fromApi(bool& result,const char* pathAndFilename,bool displayMessages,bool setCurrentDir);
	void __openOrBringDlgToFront(int dlgId);
	void __closeDlg(int dlgId);
	void __activateMainWindow();

	void __createGlContextAndFboAndTextureObjectIfNeeded(void* obj,bool useStencilBuffer);

	void _setFullscreen(bool f);
	void _destroyGlTexture(unsigned int textureName);
//	void _timer100ms_fire();
	void __pluginLoadAndStart(int& result,const char* fileAndPath,const char* name);
	void __pluginStopAndUnload(bool&result,int handle);
	void __fileDialog(std::string& nameAndPath,bool load,const char* title,const char* startPath,const char* initName,const char* extName,const char* ext);
	void __msgBox(int& retVal,int boxType,int buttons,const char* title,const char* message);

	void __requestSceneRender(bool bufferMainDisplayStateVariables);
	void _simulationEnded();
	void __messageBox_informationSystemModal(WORD& retVal,QWidget* parent,const std::string& title,const std::string& message,WORD flags);
	void __messageBox_information(WORD& retVal,QWidget* parent,const std::string& title,const std::string& message,WORD flags);
	void __messageBox_question(WORD& retVal,QWidget* parent,const std::string& title,const std::string& message,WORD flags);
	void __messageBox_warning(WORD& retVal,QWidget* parent,const std::string& title,const std::string& message,WORD flags);
	void __messageBox_critical(WORD& retVal,QWidget* parent,const std::string& title,const std::string& message,WORD flags);
	void _addStatusLine(std::string line);
	void __showEmergencyStopButtonIfNeeded(bool showState);
	void __getOpenFileName(std::string& fileAndPath,QWidget* parent,WORD option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2,const std::string& extension3,const std::string& extension4,const std::string& extension5,const std::string& extension6,const std::string& extension7,const std::string& extension8,const std::string& extension9,const std::string& extension10);
//	void __prepareOffscreenContextAndFBO(COffscreenGlContext** offContext,CFrameBufferObject** fbo,int offscreenType,int resX,int resY,bool useStencilBuffer);
	void __detectVisionSensorEntity(bool& returnValue,CVisionSensor* sensor,int entityID,bool detectAll,bool dontSwapImageBuffers,bool entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,bool hideEdgesIfModel,bool overrideRenderableFlagsForNonCollections);
	void __convexDecompose(int& retVal,int shapeHandle,int options,const int* intParams,const float* floatParams);
	void _doVariousUiTasksOnRegularBasis();
};
