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

#include "vrepPrecompiledHeader.h"
#include "uiThread.h"
#include "VThread.h"
#include "IloIlo.h"
#include "App.h"
#include "Global.h"
#include "FileMenu.h"
#include "persistentDataContainer.h"
#include "Tt.h"
#include "ThreadPool.h"
#include "pluginContainer.h"
#include "v_repStrings.h"
#include "VDateTime.h"
#include "VFileDialog.h"
#include "ObjectEdition.h"
#include <QEvent>
#include "qdlgconvexdecomposition.h"
#include "qdlgopenglsettings.h"
#include "qdlgmessageandcheckbox.h"

CUiThread::CUiThread()
{
	qThread=QThread::currentThread();
	_emergencyStopDlg=NULL;

	_progressDlg=NULL;

	_frameId=0;
	_frame_bufferMainDisplayStateVariables=false;
	_lastFrameId=0;

//	timer_100ms=new QTimer(this);
//	connect(timer_100ms, SIGNAL(timeout()), this, SLOT(_timer100ms_fire()));
//	timer_100ms->start(100);


	// Queued:
	connect(this,SIGNAL(_requestSceneRender(bool)),this,SLOT(__requestSceneRender(bool)),Qt::QueuedConnection);
	connect(this,SIGNAL(setFullscreen(bool)),this,SLOT(_setFullscreen(bool)),Qt::QueuedConnection);
	connect(this,SIGNAL(destroyGlTexture(unsigned int)),this,SLOT(_destroyGlTexture(unsigned int)),Qt::QueuedConnection);

	connect(this,SIGNAL(addStatusLine(std::string)),this,SLOT(_addStatusLine(std::string)),Qt::QueuedConnection);
	connect(this,SIGNAL(showEmergencyStopButtonIfNeeded(bool)),this,SLOT(__showEmergencyStopButtonIfNeeded(bool)),Qt::QueuedConnection);

	connect(this,SIGNAL(doVariousUiTasksOnRegularBasis()),this,SLOT(_doVariousUiTasksOnRegularBasis()),Qt::QueuedConnection);


	// Blocking:

	connect(this,SIGNAL(_closeScene_fromApi(bool)),this,SLOT(__closeScene_fromApi(bool)),Qt::BlockingQueuedConnection);
	connect(this,SIGNAL(_loadScene_fromApi(bool&,const char*,bool,bool)),this,SLOT(__loadScene_fromApi(bool&,const char*,bool,bool)),Qt::BlockingQueuedConnection);

	connect(this,SIGNAL(_openOrBringDlgToFront(int)),this,SLOT(__openOrBringDlgToFront(int)),Qt::BlockingQueuedConnection);
	connect(this,SIGNAL(_closeDlg(int)),this,SLOT(__closeDlg(int)),Qt::BlockingQueuedConnection);
	connect(this,SIGNAL(_activateMainWindow(void)),this,SLOT(__activateMainWindow(void)),Qt::BlockingQueuedConnection);

	connect(this,SIGNAL(_createGlContextAndFboAndTextureObjectIfNeeded(void*,bool)),this,SLOT(__createGlContextAndFboAndTextureObjectIfNeeded(void*,bool)),Qt::BlockingQueuedConnection);


	connect(this,SIGNAL(simulationEnded(void)),this,SLOT(_simulationEnded(void)),Qt::BlockingQueuedConnection);
	connect(this,SIGNAL(_requestSceneRender_wait(bool)),this,SLOT(__requestSceneRender(bool)),Qt::QueuedConnection);

	connect(this,SIGNAL(_pluginLoadAndStart(int&,const char*,const char*)),this,SLOT(__pluginLoadAndStart(int&,const char*,const char*)),Qt::BlockingQueuedConnection);
	connect(this,SIGNAL(_pluginStopAndUnload(bool&,int)),this,SLOT(__pluginStopAndUnload(bool&,int)),Qt::BlockingQueuedConnection);

//	connect(this,SIGNAL(prepareOffscreenContextAndFBO(COffscreenGlContext**,CFrameBufferObject**,int,int,int,bool)),this,SLOT(__prepareOffscreenContextAndFBO(COffscreenGlContext**,CFrameBufferObject**,int,int,int,bool)),Qt::BlockingQueuedConnection);
	connect(this,SIGNAL(detectVisionSensorEntity(bool&,CVisionSensor*,int,bool,bool,bool,bool,bool)),this,SLOT(__detectVisionSensorEntity(bool&,CVisionSensor*,int,bool,bool,bool,bool,bool)),Qt::BlockingQueuedConnection);

	connect(this,SIGNAL(_fileDialog(std::string&,bool,const char*,const char*,const char*,const char*,const char*)),this,SLOT(__fileDialog(std::string&,bool,const char*,const char*,const char*,const char*,const char*)),Qt::BlockingQueuedConnection);
	connect(this,SIGNAL(_msgBox(int&,int,int,const char*,const char*)),this,SLOT(__msgBox(int&,int,int,const char*,const char*)),Qt::BlockingQueuedConnection);

	connect(this,SIGNAL(_getOpenFileName(std::string&,QWidget*,WORD,const std::string&,const std::string&,const std::string&,bool,const std::string&,const std::string&,const std::string&,const std::string&,const std::string&,const std::string&,const std::string&,const std::string&,const std::string&,const std::string&,const std::string&)),this,SLOT(__getOpenFileName(std::string&,QWidget*,WORD,const std::string&,const std::string&,const std::string&,bool,const std::string&,const std::string&,const std::string&,const std::string&,const std::string&,const std::string&,const std::string&,const std::string&,const std::string&,const std::string&,const std::string&)),Qt::BlockingQueuedConnection);

	connect(this,SIGNAL(_messageBox_informationSystemModal(WORD&,QWidget*,const std::string&,const std::string&,WORD)),this,SLOT(__messageBox_informationSystemModal(WORD&,QWidget*,const std::string&,const std::string&,WORD)),Qt::BlockingQueuedConnection);
	connect(this,SIGNAL(_messageBox_information(WORD&,QWidget*,const std::string&,const std::string&,WORD)),this,SLOT(__messageBox_information(WORD&,QWidget*,const std::string&,const std::string&,WORD)),Qt::BlockingQueuedConnection);
	connect(this,SIGNAL(_messageBox_question(WORD&,QWidget*,const std::string&,const std::string&,WORD)),this,SLOT(__messageBox_question(WORD&,QWidget*,const std::string&,const std::string&,WORD)),Qt::BlockingQueuedConnection);
	connect(this,SIGNAL(_messageBox_warning(WORD&,QWidget*,const std::string&,const std::string&,WORD)),this,SLOT(__messageBox_warning(WORD&,QWidget*,const std::string&,const std::string&,WORD)),Qt::BlockingQueuedConnection);
	connect(this,SIGNAL(_messageBox_critical(WORD&,QWidget*,const std::string&,const std::string&,WORD)),this,SLOT(__messageBox_critical(WORD&,QWidget*,const std::string&,const std::string&,WORD)),Qt::BlockingQueuedConnection);

	connect(this,SIGNAL(_convexDecompose(int&,int,int,const int*,const float*)),this,SLOT(__convexDecompose(int&,int,int,const int*,const float*)),Qt::BlockingQueuedConnection);
}

CUiThread::~CUiThread()
{
	// delete timer_100ms; not needed, timer is automatically deleted when this object is deleted
}

void CUiThread::closeScene_fromApi(bool displayMessages)
{ // can be called by any thread. When closing a scene, textures might be destroyed. That is only ok from the UI thread!
	FUNCTION_DEBUG;
	if (VThread::isCurrentThreadTheUiThread())
		__closeScene_fromApi(displayMessages);
	else
		_closeScene_fromApi(displayMessages);
}

void CUiThread::__closeScene_fromApi(bool displayMessages)
{ // can only be called by the UI thread
	FUNCTION_DEBUG;
	CFileMenu::closeScene(displayMessages);
}

bool CUiThread::loadScene_fromApi(const char* pathAndFilename,bool displayMessages,bool setCurrentDir)
{ // can be called by any thread. When a scene is loaded, a previous scene might be discarded and textures destroyed. That is only ok from the UI thread!
	FUNCTION_DEBUG;
	bool retV;
	if (VThread::isCurrentThreadTheUiThread())
		__loadScene_fromApi(retV,pathAndFilename,displayMessages,setCurrentDir);
	else
		_loadScene_fromApi(retV,pathAndFilename,displayMessages,setCurrentDir);
	return(retV);
}

void CUiThread::__loadScene_fromApi(bool& result,const char* pathAndFilename,bool displayMessages,bool setCurrentDir)
{ // can only be called by the UI thread
	FUNCTION_DEBUG;
	result=CFileMenu::loadScene(pathAndFilename,displayMessages,setCurrentDir);
}

void CUiThread::openOrBringDlgToFront(int dlgId)
{ // can be called by any thread
	FUNCTION_DEBUG;
	if (App::mainWindow!=NULL)
	{ // make sure we are not in headless mode
		if (VThread::isCurrentThreadTheUiThread())
			__openOrBringDlgToFront(dlgId);
		else
			_openOrBringDlgToFront(dlgId);
	}
}

void CUiThread::__openOrBringDlgToFront(int dlgId)
{ // can only be called by the UI thread
	FUNCTION_DEBUG;
	App::mainWindow->dlgCont->openOrBringToFront(dlgId);
}

void CUiThread::closeDlg(int dlgId)
{ // can be called by any thread
	FUNCTION_DEBUG;
	if (App::mainWindow!=NULL)
	{ // make sure we are not in headless mode
		if (VThread::isCurrentThreadTheUiThread())
			__closeDlg(dlgId);
		else
			_closeDlg(dlgId);
	}
}

void CUiThread::__closeDlg(int dlgId)
{ // can only be called by the UI thread
	FUNCTION_DEBUG;
	App::mainWindow->dlgCont->close(dlgId);
}

void CUiThread::activateMainWindow()
{ // can be called by any thread
	FUNCTION_DEBUG;
	if (App::mainWindow!=NULL)
	{ // make sure we are not in headless mode
		if (VThread::isCurrentThreadTheUiThread())
			__activateMainWindow();
		else
			_activateMainWindow();
	}
}

void CUiThread::__activateMainWindow()
{ // can only be called by the UI thread
	FUNCTION_DEBUG;
	App::mainWindow->activateWindow();
}

void CUiThread::createGlContextAndFboAndTextureObjectIfNeeded(void* obj,bool useStencilBuffer)
{ // can be called by any thread
	FUNCTION_DEBUG;
	if (VThread::isCurrentThreadTheUiThread())
	{
		IF_UI_EVENT_CAN_WRITE_DATA
			__createGlContextAndFboAndTextureObjectIfNeeded(obj,useStencilBuffer);
	}
	else
	{
		SIM_THREAD_INDICATE_UI_THREAD_CAN_DO_ANYTHING;
		_createGlContextAndFboAndTextureObjectIfNeeded(obj,useStencilBuffer);
	}
}

void CUiThread::__createGlContextAndFboAndTextureObjectIfNeeded(void* obj,bool useStencilBuffer)
{ // can only be called by the UI thread
	FUNCTION_DEBUG;
	((CVisionSensor*)obj)->createGlContextAndFboAndTextureObjectIfNeeded(useStencilBuffer);
}


std::string CUiThread::getOpenFileName(QWidget* parent,WORD option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2,const std::string& extension3,const std::string& extension4,const std::string& extension5,const std::string& extension6,const std::string& extension7,const std::string& extension8,const std::string& extension9,const std::string& extension10)
{ // can be called by any thread
	FUNCTION_DEBUG;
	std::string retVal;
	if (App::mainWindow!=NULL)
	{ // make sure we are not in headless mode
		if (VThread::isCurrentThreadTheUiThread())
		{	// following condition is important: during simulation, the SIM thread might request the UI thread to do
			// some tasks. This is not possible when a modal dialog is displayed. So we simply
			// try to pause the SIM thread while we are displaying a modal message box:
			IF_UI_EVENT_PAUSE_SIM_THREAD
				retVal=VFileDialog::getOpenFileName_(parent,option,title,startPath,initFilename,allowAnyFile,extensionName,extension1,extension2,extension3,extension4,extension5,extension6,extension7,extension8,extension9,extension10);
		}
		else
			_getOpenFileName(retVal,parent,option,title,startPath,initFilename,allowAnyFile,extensionName,extension1,extension2,extension3,extension4,extension5,extension6,extension7,extension8,extension9,extension10);
	}
	return(retVal);
}

void CUiThread::__getOpenFileName(std::string& fileAndPath,QWidget* parent,WORD option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2,const std::string& extension3,const std::string& extension4,const std::string& extension5,const std::string& extension6,const std::string& extension7,const std::string& extension8,const std::string& extension9,const std::string& extension10)
{ // is called by the UI thread only
	FUNCTION_DEBUG;
	if (App::mainWindow!=NULL)
	{ // make sure we are not in headless mode
		fileAndPath=VFileDialog::getOpenFileName_(parent,option,title,startPath,initFilename,allowAnyFile,extensionName,extension1,extension2,extension3,extension4,extension5,extension6,extension7,extension8,extension9,extension10);
	}
	else
		fileAndPath="";
}

/*
Following is now triggered by the SIM thread, in the PAUSE location
void CUiThread::_timer100ms_fire()
{ // is called by the UI thread only
	FUNCTION_DEBUG;
	IF_UI_EVENT_CAN_READ_DATA
	{ // ok, we are allowed to refresh (i.e. the simulation thread doesn't modify anything serious)
		if (App::mainWindow!=NULL)
			App::mainWindow->refreshDialogs_uiThread();
		if (App::ct!=NULL)
			_doVariousUiTasksOnRegularBasis();
	}
}
*/


void CUiThread::requestSceneRender(bool bufferMainDisplayStateVariables)
{ // is called by the non-UI thread
	FUNCTION_DEBUG;
	if (App::mainWindow!=NULL)
	{ // make sure we are not in headless mode
		static int lastHere=VDateTime::getTimeInMs();
		if (VDateTime::getTimeDiffInMs(lastHere)>33) // max 30 requ./sec
		{ // avoid flooding the UI thread with potentially thousands of render requests/seconds
//			printf(".");
			_frame_bufferMainDisplayStateVariables=bufferMainDisplayStateVariables;
			// bufferMainDisplayStateVariables is true when the UI thread holds the SIM thread (so the UI thread can modify the scene). The SIM thread still fires rendering signals with this set to true
			_frameId++;
			_requestSceneRender(bufferMainDisplayStateVariables);
			lastHere=VDateTime::getTimeInMs();
		}
	}
}

void CUiThread::requestSceneRender_wait()
{ // is called by the non-UI thread
	if (App::mainWindow!=NULL)
	{ // make sure we are not in headless mode
		_frameId++;
		_frame_bufferMainDisplayStateVariables=false;
		_requestSceneRender_wait(false);
		while (_frameId!=_lastFrameId)
		{
			VThread::sleep(1);
			// App::simThread->executeMessages();
		}
		_lastFrameId=_frameId;
	}
}

void CUiThread::__requestSceneRender(bool bufferMainDisplayStateVariables)
{ // is called by the UI thread
	if ((_frameId!=_lastFrameId)&&(App::mainWindow!=NULL))
	{
		static int lastHere=VDateTime::getTimeInMs();
		if (VDateTime::getTimeDiffInMs(lastHere)>10)
		{ // if many requests are comming in, or if the rendering takes time, leave some time to other tasks!
			App::mainWindow->uiThread_renderScene(_frame_bufferMainDisplayStateVariables);
			lastHere=VDateTime::getTimeInMs();
		}
		_lastFrameId=_frameId;
	}
}

int CUiThread::getLastFrameId()
{ // can be called by any thread
	return(_lastFrameId);
}

void CUiThread::setLastFrameId(int fid)
{ // can be called by any thread
	_lastFrameId=fid;
}

/*
void CUiThread::__prepareOffscreenContextAndFBO(COffscreenGlContext** offContext,CFrameBufferObject** fbo,int offscreenType,int resX,int resY,bool useStencilBuffer)
{ // called by the UI thread. The SIM thread waits in the mean time.
	QGLWidget* otherWidgetToShareResourcesWith=NULL;
	if (App::mainWindow!=NULL)
		otherWidgetToShareResourcesWith=App::mainWindow->openglWidget;
	offContext[0]=new COffscreenGlContext(offscreenType,resX,resY,otherWidgetToShareResourcesWith); // if not native, we cannot call the context from different threads!!!
	fbo[0]=new CFrameBufferObject(true,resX,resY,useStencilBuffer);
	offContext[0]->makeCurrent();
	offContext[0]->moveGlContextToThread(App::simThread->qThread);
}
*/
void CUiThread::__detectVisionSensorEntity(bool& returnValue,CVisionSensor* sensor,int entityID,bool detectAll,bool dontSwapImageBuffers,bool entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,bool hideEdgesIfModel,bool overrideRenderableFlagsForNonCollections)
{ // called by the UI thread. The SIM thread waits in the mean time.
	FUNCTION_DEBUG;
	returnValue=sensor->detectEntity2(entityID,detectAll,dontSwapImageBuffers,entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,hideEdgesIfModel,overrideRenderableFlagsForNonCollections);
}

WORD CUiThread::messageBox_informationSystemModal(QWidget* parent,const std::string& title,const std::string& message,WORD flags)
{ // called by any thread.
	FUNCTION_DEBUG;
	if (App::mainWindow!=NULL)
	{ // make sure we are not in headless mode
		if (VThread::isCurrentThreadTheUiThread())
		{	// following condition is important: during simulation, the SIM thread might request the UI thread to do
			// some tasks. This is not possible when a modal message box is displayed. So we simply
			// try to pause the SIM thread while we are displaying a modal message box:
			IF_UI_EVENT_PAUSE_SIM_THREAD
				return(VMessageBox::informationSystemModal_(parent,title,message,flags));
			return(VMessageBox::REPLY_ERROR);
		}
		WORD retVal;
		_messageBox_informationSystemModal(retVal,parent,title,message,flags);
		return(retVal);
	}
	else
		return(VMessageBox::REPLY_ERROR);
}
void CUiThread::__messageBox_informationSystemModal(WORD& retVal,QWidget* parent,const std::string& title,const std::string& message,WORD flags)
{ // called by the UI thread.
	FUNCTION_DEBUG;
	retVal=VMessageBox::informationSystemModal_(parent,title,message,flags);
}

WORD CUiThread::messageBox_information(QWidget* parent,const std::string& title,const std::string& message,WORD flags)
{ // called by any thread.
	FUNCTION_DEBUG;
	if (App::mainWindow!=NULL)
	{ // make sure we are not in headless mode
		if (VThread::isCurrentThreadTheUiThread())
		{	// following condition is important: during simulation, the SIM thread might request the UI thread to do
			// some tasks. This is not possible when a modal message box is displayed. So we simply
			// try to pause the SIM thread while we are displaying a modal message box:
			IF_UI_EVENT_PAUSE_SIM_THREAD
				return(VMessageBox::information_(parent,title,message,flags));
			return(VMessageBox::REPLY_ERROR);
		}
		WORD retVal;
		_messageBox_information(retVal,parent,title,message,flags);
		return(retVal);
	}
	else
		return(VMessageBox::REPLY_ERROR);
}

void CUiThread::__messageBox_information(WORD& retVal,QWidget* parent,const std::string& title,const std::string& message,WORD flags)
{ // called by the UI thread.
	FUNCTION_DEBUG;
	retVal=VMessageBox::information_(parent,title,message,flags);
}

WORD CUiThread::messageBox_question(QWidget* parent,const std::string& title,const std::string& message,WORD flags)
{  // called by any thread.
	FUNCTION_DEBUG;
	if (App::mainWindow!=NULL)
	{ // make sure we are not in headless mode
		if (VThread::isCurrentThreadTheUiThread())
		{	// following condition is important: during simulation, the SIM thread might request the UI thread to do
			// some tasks. This is not possible when a modal message box is displayed. So we simply
			// try to pause the SIM thread while we are displaying a modal message box:
			IF_UI_EVENT_PAUSE_SIM_THREAD
				return(VMessageBox::question_(parent,title,message,flags));
			return(VMessageBox::REPLY_ERROR);
		}
		WORD retVal;
		_messageBox_question(retVal,parent,title,message,flags);
		return(retVal);
	}
	else
		return(VMessageBox::REPLY_ERROR);
}

void CUiThread::__messageBox_question(WORD& retVal,QWidget* parent,const std::string& title,const std::string& message,WORD flags)
{  // called by the UI thread.
	FUNCTION_DEBUG;
	retVal=VMessageBox::question_(parent,title,message,flags);
}

WORD CUiThread::messageBox_warning(QWidget* parent,const std::string& title,const std::string& message,WORD flags)
{  // called by any thread.
	FUNCTION_DEBUG;
	if (App::mainWindow!=NULL)
	{ // make sure we are not in headless mode
		if (VThread::isCurrentThreadTheUiThread())
		{	// following condition is important: during simulation, the SIM thread might request the UI thread to do
			// some tasks. This is not possible when a modal message box is displayed. So we simply
			// try to pause the SIM thread while we are displaying a modal message box:
			IF_UI_EVENT_PAUSE_SIM_THREAD
				return(VMessageBox::warning_(parent,title,message,flags));
			return(VMessageBox::REPLY_ERROR);
		}
		WORD retVal;
		_messageBox_warning(retVal,parent,title,message,flags);
		return(retVal);
	}
	else
		return(VMessageBox::REPLY_ERROR);
}

void CUiThread::__messageBox_warning(WORD& retVal,QWidget* parent,const std::string& title,const std::string& message,WORD flags)
{ // called by the UI thread.
	FUNCTION_DEBUG;
	retVal=VMessageBox::warning_(parent,title,message,flags);
}

WORD CUiThread::messageBox_critical(QWidget* parent,const std::string& title,const std::string& message,WORD flags)
{ // called by any thread.
	FUNCTION_DEBUG;
	if (App::mainWindow!=NULL)
	{ // make sure we are not in headless mode
		if (VThread::isCurrentThreadTheUiThread())
		{	// following condition is important: during simulation, the SIM thread might request the UI thread to do
			// some tasks. This is not possible when a modal message box is displayed. So we simply
			// try to pause the SIM thread while we are displaying a modal message box:
			IF_UI_EVENT_PAUSE_SIM_THREAD
				return(VMessageBox::critical_(parent,title,message,flags));
			return(VMessageBox::REPLY_ERROR);
		}
		WORD retVal;
		_messageBox_critical(retVal,parent,title,message,flags);
		return(retVal);
	}
	else
		return(VMessageBox::REPLY_ERROR);
}

void CUiThread::__messageBox_critical(WORD& retVal,QWidget* parent,const std::string& title,const std::string& message,WORD flags)
{ // called by the UI thread.
	FUNCTION_DEBUG;
	retVal=VMessageBox::critical_(parent,title,message,flags);
}

std::string CUiThread::fileDialog(bool load,const char* title,const char* startPath,const char* initName,const char* extName,const char* ext)
{ // called by any thread.
	FUNCTION_DEBUG;
	std::string retVal;
	if (App::mainWindow!=NULL)
	{ // make sure we are not in headless mode
		if (VThread::isCurrentThreadTheUiThread())
		{	// following condition is important: during simulation, the SIM thread might request the UI thread to do
			// some tasks. This is not possible when a modal dialog is displayed. So we simply
			// try to pause the SIM thread while we are displaying a modal message box:
			IF_UI_EVENT_PAUSE_SIM_THREAD
				__fileDialog(retVal,load,title,startPath,initName,extName,ext);
		}
		else
			_fileDialog(retVal,load,title,startPath,initName,extName,ext);
	}
	return(retVal);
}

int CUiThread::msgBox(int boxType,int buttons,const char* title,const char* message)
{ // called by any thread.
	FUNCTION_DEBUG;
	int retVal=sim_msgbox_return_error;
	if (App::mainWindow!=NULL)
	{ // make sure we are not in headless mode
		if (VThread::isCurrentThreadTheUiThread())
		{	// following condition is important: during simulation, the SIM thread might request the UI thread to do
			// some tasks. This is not possible when a modal message box is displayed. So we simply
			// try to pause the SIM thread while we are displaying a modal message box:
			IF_UI_EVENT_PAUSE_SIM_THREAD
				__msgBox(retVal,boxType,buttons,title,message);
		}
		else
			_msgBox(retVal,boxType,buttons,title,message);
	}
	return(retVal);
}

void CUiThread::__fileDialog(std::string& nameAndPath,bool load,const char* title,const char* startPath,const char* initName,const char* extName,const char* ext)
{ // called only from the UI thread
	FUNCTION_DEBUG;
	if (load)
		nameAndPath=VFileDialog::getOpenFileName_(App::mainWindow,0,title,startPath,initName,false,extName,ext);
	else
		nameAndPath=VFileDialog::getSaveFileName(App::mainWindow,0,title,startPath,initName,false,extName,ext);
}

void CUiThread::__msgBox(int& retVal,int boxType,int buttons,const char* title,const char* message)
{ // called only from the UI thread
	FUNCTION_DEBUG;
	int buts=VMessageBox::OKELI;
	if (buttons==sim_msgbox_buttons_ok)
		buts=VMessageBox::OKELI;
	if (buttons==sim_msgbox_buttons_yesno)
		buts=VMessageBox::YES_NO;
	if (buttons==sim_msgbox_buttons_yesnocancel)
		buts=VMessageBox::YES_NO_CANCEL;
	if (buttons==sim_msgbox_buttons_okcancel)
		buts=VMessageBox::OK_CANCEL;

	WORD returned=999;
	if (boxType==sim_msgbox_type_info)
		returned=VMessageBox::information_(App::mainWindow,title,message,buts);
	if (boxType==sim_msgbox_type_question)
		returned=VMessageBox::question_(App::mainWindow,title,message,buts);
	if (boxType==sim_msgbox_type_warning)
		returned=VMessageBox::warning_(App::mainWindow,title,message,buts);
	if (boxType==sim_msgbox_type_critical)
		returned=VMessageBox::critical_(App::mainWindow,title,message,buts);

	retVal=-1;
	if (returned==VMessageBox::REPLY_CANCEL)
		retVal=sim_msgbox_return_cancel;
	if (returned==VMessageBox::REPLY_NO)
		retVal=sim_msgbox_return_no;
	if (returned==VMessageBox::REPLY_YES)
		retVal=sim_msgbox_return_yes;
	if (returned==VMessageBox::REPLY_OK)
		retVal=sim_msgbox_return_ok;
}

void CUiThread::_simulationEnded()
{ // called by the UI thread (but indirectly by the SIM thread)
	FUNCTION_DEBUG;
//	IF_UI_EVENT_CAN_READ_DATA // not needed since the SIM thread is waiting
	App::ct->scintillaConsoleContainer->simulationEnded();
}

void CUiThread::_setFullscreen(bool f)
{ // called by the UI thread
	FUNCTION_DEBUG;
	if (App::mainWindow!=NULL)
		App::mainWindow->setFullScreen(f);
}

void CUiThread::_destroyGlTexture(unsigned int textureName)
{ // called by the UI thread
	FUNCTION_DEBUG;
	glDeleteTextures(1,&textureName);
}

void CUiThread::_addStatusLine(std::string line)
{ // called by the UI thread.
	App::addStatusLine_(line.c_str());
}

int CUiThread::pluginLoadAndStart(const char* fileAndPath,const char* name)
{ // can be called from any thread
	FUNCTION_DEBUG;
	int retVal;
	if (VThread::isCurrentThreadTheUiThread())
		__pluginLoadAndStart(retVal,fileAndPath,name);
	else
	{ // the SIM thread waits here until done
		SIM_THREAD_INDICATE_UI_THREAD_CAN_DO_ANYTHING;
		_pluginLoadAndStart(retVal,fileAndPath,name);
	}
	return(retVal);
}

bool CUiThread::pluginStopAndUnload(int handle)
{ // can be called from any thread
	FUNCTION_DEBUG;
	bool retVal;
	if (VThread::isCurrentThreadTheUiThread())
		__pluginStopAndUnload(retVal,handle);
	else
	{ // the SIM thread waits here until done
		SIM_THREAD_INDICATE_UI_THREAD_CAN_DO_ANYTHING;
		_pluginStopAndUnload(retVal,handle);
	}
	return(retVal);
}

void CUiThread::__pluginLoadAndStart(int& result,const char* fileAndPath,const char* name)
{ // called only from the UI thread. The SIM thread waits in the mean time.
	FUNCTION_DEBUG;
	result=-3; // could not load
	IF_UI_EVENT_CAN_WRITE_DATA // should always pass
	{
		result=CPluginContainer::addPlugin(fileAndPath,name);
	}
}

void CUiThread::__pluginStopAndUnload(bool& result,int handle)
{ // called only from the UI thread. The SIM thread waits in the mean time.
	FUNCTION_DEBUG;
	result=false; // could not unload
	IF_UI_EVENT_CAN_WRITE_DATA // should always pass
	{
		result=CPluginContainer::killPlugin(handle);
	}
}

void CUiThread::__showEmergencyStopButtonIfNeeded(bool showState)
{ // called only from the UI thread
	if (App::mainWindow!=NULL)
	{ // make sure we are not in headless mode
		if (showState)
		{
			if (_emergencyStopDlg==NULL)
			{
				_emergencyStopDlg=new CQDlgStopScripts(App::mainWindow);
				_emergencyStopDlg->show();
			}
			if (CQDlgStopScripts::stopScriptNow)
			{
				IF_UI_EVENT_CAN_READ_DATA
				{
					CQDlgStopScripts::stopScriptNow=false;
					CThreadPool::setEmergencyStopForCurrentInstance(true);
					if (App::ct->simulation->getSimulationState()!=sim_simulation_advancing_lastbeforestop)
						App::ct->simulation->setSimulationStateDirect(sim_simulation_advancing_abouttostop);
				}
			}
		}
		else
		{
			if (_emergencyStopDlg!=NULL)
			{
				delete _emergencyStopDlg;
				_emergencyStopDlg=NULL;
			}
		}
	}
}

int CUiThread::convexDecompose(int shapeHandle,int options,const int* intParams,const float* floatParams)
{ // can be called from any thread
	FUNCTION_DEBUG;
	int retVal=-1;
	if (App::mainWindow==NULL)
	{ // we are in headless mode: do not display the dialog!
		options=(options|2)-2;
	}

	if (VThread::isCurrentThreadTheUiThread())
	{	// following condition is important: during simulation, the SIM thread might request the UI thread to do
		// some tasks. This is not possible when a modal dialog is displayed. So we simply
		// try to pause the SIM thread while we are displaying a modal dialog:
		IF_UI_EVENT_PAUSE_SIM_THREAD
			__convexDecompose(retVal,shapeHandle,options,intParams,floatParams); // we can call it directly
	}
	else
	{ // the SIM thread waits here until done
		SIM_THREAD_INDICATE_UI_THREAD_CAN_DO_ANYTHING;
		_convexDecompose(retVal,shapeHandle,options,intParams,floatParams);
	}
	return(retVal);
}

void CUiThread::__convexDecompose(int& retVal,int shapeHandle,int options,const int* intParams,const float* floatParams)
{ // can only be called by the UI thread
	FUNCTION_DEBUG;
	retVal=-1;
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		static bool addExtraDistPoints=true;
		static bool addFacesPoints=true;
		static int nClusters=1;
		static int maxHullVertices=200; // from 100 to 200 on 5/2/2014
		static float maxConcavity=100.0f;
		static float smallClusterThreshold=0.25f;
		static int maxTrianglesInDecimatedMesh=500;
		static float maxConnectDist=30.0f;
		static bool individuallyConsiderMultishapeComponents=false;
		static bool randomColors=false;
		static int maxIterations=4;
		if ((options&4)==0)
		{
			addExtraDistPoints=(options&8)!=0;
			addFacesPoints=(options&16)!=0;
			nClusters=intParams[0];
			maxHullVertices=intParams[2];
			maxConcavity=floatParams[0];
			smallClusterThreshold=floatParams[2];
			maxTrianglesInDecimatedMesh=intParams[1];
			maxConnectDist=floatParams[1];
			individuallyConsiderMultishapeComponents=(options&32)!=0;
			randomColors=(options&64)!=0;
			maxIterations=intParams[3];
			if (maxIterations<=0)
				maxIterations=4; // zero asks for default value
		}
		bool abortp=false;
		if ((options&2)!=0)
		{
			// memorize current dlg settings:
			bool _addExtraDistPoints=CQDlgConvexDecomposition::addExtraDistPoints;
			bool _addFacesPoints=CQDlgConvexDecomposition::addFacesPoints;
			int _nClusters=CQDlgConvexDecomposition::nClusters;
			int _maxHullVertices=CQDlgConvexDecomposition::maxHullVertices;
			float _maxConcavity=CQDlgConvexDecomposition::maxConcavity;
			float _smallClusterThreshold=CQDlgConvexDecomposition::smallClusterThreshold;
			int _maxTrianglesInDecimatedMesh=CQDlgConvexDecomposition::maxTrianglesInDecimatedMesh;
			float _maxConnectDist=CQDlgConvexDecomposition::maxConnectDist;
			bool _individuallyConsiderMultishapeComponents=CQDlgConvexDecomposition::individuallyConsiderMultishapeComponents;
			bool _randomColors=CQDlgConvexDecomposition::randomColors;
			int _maxIterations=CQDlgConvexDecomposition::maxIterations;

			// apply desired dlg settings:
			CQDlgConvexDecomposition::addExtraDistPoints=addExtraDistPoints;
			CQDlgConvexDecomposition::addFacesPoints=addFacesPoints;
			CQDlgConvexDecomposition::nClusters=nClusters;
			CQDlgConvexDecomposition::maxHullVertices=maxHullVertices;
			CQDlgConvexDecomposition::maxConcavity=maxConcavity;
			CQDlgConvexDecomposition::smallClusterThreshold=smallClusterThreshold;
			CQDlgConvexDecomposition::maxTrianglesInDecimatedMesh=maxTrianglesInDecimatedMesh;
			CQDlgConvexDecomposition::maxConnectDist=maxConnectDist;
			CQDlgConvexDecomposition::individuallyConsiderMultishapeComponents=individuallyConsiderMultishapeComponents;
			CQDlgConvexDecomposition::randomColors=randomColors;
			CQDlgConvexDecomposition::maxIterations=maxIterations;

			// Display the dialog:
			CQDlgConvexDecomposition theDialog(App::mainWindow);
			theDialog.refresh();
			if (theDialog.makeDialogModal()==VDIALOG_MODAL_RETURN_CANCEL)
				abortp=true;

			// Retrieve the dlg settings:
			addExtraDistPoints=theDialog.addExtraDistPoints;
			addFacesPoints=theDialog.addFacesPoints;
			nClusters=theDialog.nClusters;
			maxHullVertices=theDialog.maxHullVertices;
			maxConcavity=theDialog.maxConcavity;
			smallClusterThreshold=theDialog.smallClusterThreshold;
			maxTrianglesInDecimatedMesh=theDialog.maxTrianglesInDecimatedMesh;
			maxConnectDist=theDialog.maxConnectDist;
			individuallyConsiderMultishapeComponents=theDialog.individuallyConsiderMultishapeComponents;
			randomColors=theDialog.randomColors;
			maxIterations=theDialog.maxIterations;

			// restore previous dlg settings:
			CQDlgConvexDecomposition::addExtraDistPoints=_addExtraDistPoints;
			CQDlgConvexDecomposition::addFacesPoints=_addFacesPoints;
			CQDlgConvexDecomposition::nClusters=_nClusters;
			CQDlgConvexDecomposition::maxHullVertices=_maxHullVertices;
			CQDlgConvexDecomposition::maxConcavity=_maxConcavity;
			CQDlgConvexDecomposition::smallClusterThreshold=_smallClusterThreshold;
			CQDlgConvexDecomposition::maxTrianglesInDecimatedMesh=_maxTrianglesInDecimatedMesh;
			CQDlgConvexDecomposition::maxConnectDist=_maxConnectDist;
			CQDlgConvexDecomposition::individuallyConsiderMultishapeComponents=_individuallyConsiderMultishapeComponents;
			CQDlgConvexDecomposition::randomColors=_randomColors;
			CQDlgConvexDecomposition::maxIterations=_maxIterations;
		}
		if (!abortp)
			retVal=CObjectEdition::generateConvexDecomposed(shapeHandle,nClusters,maxConcavity,addExtraDistPoints,addFacesPoints,maxConnectDist,maxTrianglesInDecimatedMesh,maxHullVertices,smallClusterThreshold,individuallyConsiderMultishapeComponents,randomColors,maxIterations);
		else
			retVal=-1;

		if ( (retVal!=-1)&&((options&1)!=0) )
		{ // we wanted a morph!!
			CShape* newShape=App::ct->objCont->getShape(retVal);
			CShape* it=App::ct->objCont->getShape(shapeHandle);
			C7Vector newLocal(it->getParentCumulativeTransformation().getInverse()*newShape->getCumulativeTransformation());
			C7Vector oldLocal(it->getLocalTransformation());
			delete it->geomData;
			it->geomData=newShape->geomData; // we exchange the geomData object
			it->setLocalTransformation(newLocal); // The shape's frame was changed!
			it->incrementMemorizedConfigurationValidCounter(); // so if we are running in a simulation, the shape doesn't get reset at its initial config
			newShape->geomData=NULL;
			App::ct->objCont->eraseObject(newShape);
			// We need to correct all its children for this change of frame:
			for (int i=0;i<int(it->childList.size());i++)
			{
				it->childList[i]->setLocalTransformation(newLocal.getInverse()*oldLocal*it->childList[i]->getLocalTransformationPart1());
				it->childList[i]->incrementMemorizedConfigurationValidCounter(); // so if we are running in a simulation, the shape doesn't get reset at its initial config
			}
			retVal=shapeHandle;
		}
	}
}

void CUiThread::_doVariousUiTasksOnRegularBasis()
{ // this is called by the UI thread, and the UI thread is already authorized to read data
	FUNCTION_DEBUG;

	if (App::mainWindow!=NULL)
		App::mainWindow->refreshDialogs_uiThread();



	static bool crashedPreviously=false;
	// Here we maybe need to load auto-saved scenes:
	// *************************************************************************************************************
	static int forAutoSave_renderPass=0;
	forAutoSave_renderPass++;
	if (forAutoSave_renderPass==5)
	{
		CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
		std::string val;
		cont.readData("SIMSETTINGS_VREP_CRASHED",val);
		if ((val=="Yes")&&(App::mainWindow!=NULL))
		{ // ask what to do:
			if (!App::isFullScreen())
			{
				crashedPreviously=true;
				if (!App::userSettings->doNotShowCrashRecoveryMessage)
				{
					IF_UI_EVENT_CAN_WRITE_DATA
					{
#ifdef MAC_VREP
						if (VMessageBox::REPLY_YES==App::uiThread->messageBox_question(App::mainWindow,IDSN_VREP_CRASH,IDSN_VREP_CRASH_OR_NEW_INSTANCE_INFO_MAC,VMessageBox::YES_NO))
#else
						if (VMessageBox::REPLY_YES==App::uiThread->messageBox_question(App::mainWindow,IDSN_VREP_CRASH,IDSN_VREP_CRASH_OR_NEW_INSTANCE_INFO,VMessageBox::YES_NO))
#endif
						{
							std::string testScene=App::directories->executableDirectory+VREP_SLASH;
							testScene.append("AUTO_SAVED_INSTANCE_1.ttt");
							if (CFileMenu::loadScene(testScene.c_str(),false,false))
							{
								App::ct->mainSettings->scenePathAndName="";
								App::uiThread->addStatusLine(IDSNS_SCENE_WAS_RESTORED_FROM_AUTO_SAVED_SCENE);
							}
							int instanceNb=2;
							while (true)
							{
								testScene=App::directories->executableDirectory+VREP_SLASH;
								testScene.append("AUTO_SAVED_INSTANCE_");
								testScene+=tt::FNb(instanceNb);
								testScene+=".ttt";
								if (VFile::doesFileExist(testScene))
								{
									App::ct->createNewInstance(true);
									if (CFileMenu::loadScene(testScene.c_str(),false,false))
									{
										App::ct->mainSettings->scenePathAndName="";
										App::uiThread->addStatusLine(IDSNS_SCENE_WAS_RESTORED_FROM_AUTO_SAVED_SCENE);
									}
									else
										break;
									instanceNb++;
								}
								else
									break;
							}
							App::ct->makeInstanceCurrentFromIndex(0,false);
						}
					}
				}
			}
		}
	}
	else
	{
		if (forAutoSave_renderPass==10)
		{ // Set the TAG: V-REP started normally
			CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
			cont.writeData("SIMSETTINGS_VREP_CRASHED","Yes",!App::userSettings->doNotWritePersistentData);
		}
			if ((forAutoSave_renderPass>10)&&(App::userSettings->autoSaveDelay>0)&&App::ct->simulation->isSimulationStopped())
			{
				if (VDateTime::getSecondsSince1970()>(App::ct->environment->autoSaveLastSaveTimeInSecondsSince1970+App::userSettings->autoSaveDelay*60))
				{
					IF_UI_EVENT_CAN_WRITE_DATA
					{
						std::string savedLoc=App::ct->mainSettings->scenePathAndName;
						std::string testScene=App::directories->executableDirectory+VREP_SLASH;
						testScene+="AUTO_SAVED_INSTANCE_";
						testScene+=tt::FNb(App::ct->getCurrentInstanceIndex()+1);
						testScene+=".ttt";
						CFileMenu::saveScene(testScene.c_str(),false,false);
						std::string info=IDSNS_AUTO_SAVED_SCENE;
						info+=" ("+testScene+")";
						App::uiThread->addStatusLine(info.c_str());
						App::ct->mainSettings->scenePathAndName=savedLoc;
						App::ct->environment->autoSaveLastSaveTimeInSecondsSince1970=VDateTime::getSecondsSince1970();
					}
				}
			}
		}
	// *************************************************************************************************************


	// Here we maybe need to display the openGL settings dialog:
	// *************************************************************************************************************
	static int forTestScene_renderPass=0;
	forTestScene_renderPass++;
	if ((forTestScene_renderPass==10)&&(!crashedPreviously)&&(App::mainWindow!=NULL))
	{
		CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
		std::string val;
		cont.readData("SIMSETTINGS_OPENGLSETTINGS_NO_SHOW",val);
		int intVal=0;
		tt::getValidInt(val,intVal);
		if (intVal<5)
		{ // show the message 5 times
			if (!App::isFullScreen())
			{
				if (!App::userSettings->doNotShowOpenglSettingsMessage)
				{
					IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG // required with modal dialogs
					{
						CQDlgOpenGlSettings theDialog(App::mainWindow);

						theDialog.offscreenContextType=App::userSettings->offscreenContextType;
						theDialog.fboType=App::userSettings->fboType;
						theDialog.exclusiveGuiRendering=(App::userSettings->visionSensorsUseGuiThread_windowed!=0);
						theDialog.compatibilityTweak1=App::userSettings->oglCompatibilityTweak1;
						theDialog.glFinish_normal=App::userSettings->useGlFinish;
						theDialog.glFinish_visionSensors=App::userSettings->useGlFinish_visionSensors;
						theDialog.idleFps=App::userSettings->getIdleFps();
						theDialog.forceExt=App::userSettings->forceFboViaExt;
						theDialog.glVersionMajor=App::userSettings->desiredOpenGlMajor;
						theDialog.glVersionMinor=App::userSettings->desiredOpenGlMinor;

						theDialog.refresh();
						if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
						{
							App::userSettings->offscreenContextType=theDialog.offscreenContextType;
							App::userSettings->fboType=theDialog.fboType;
							App::userSettings->visionSensorsUseGuiThread_windowed=-1;
							if (!theDialog.exclusiveGuiRendering)
								App::userSettings->visionSensorsUseGuiThread_windowed=0;
							App::userSettings->oglCompatibilityTweak1=theDialog.compatibilityTweak1;
							App::userSettings->useGlFinish=theDialog.glFinish_normal;
							App::userSettings->useGlFinish_visionSensors=theDialog.glFinish_visionSensors;
							App::userSettings->setIdleFps(theDialog.idleFps);
							App::userSettings->forceFboViaExt=theDialog.forceExt;
							App::userSettings->desiredOpenGlMajor=theDialog.glVersionMajor;
							App::userSettings->desiredOpenGlMinor=theDialog.glVersionMinor;
							App::userSettings->saveUserSettings();
						}
						intVal++;
						val=tt::FNb(intVal);
						cont.writeData("SIMSETTINGS_OPENGLSETTINGS_NO_SHOW",val,!App::userSettings->doNotWritePersistentData);
					}
				}
			}
		}
	}
	// *************************************************************************************************************



	bool displayNonStandardParams=App::ct->simulation->getDisplayWarningAboutNonDefaultParameters();
	bool displayNonPureNonConvexShapeUseWarning=false;
	bool displayStaticShapeOnDynamicConstructionWarning=false;

	if ((!App::isFullScreen())&&(App::mainWindow!=NULL))
	{
		App::ct->dynamicsContainer->displayWarningsIfNeeded(); // Warnings when something not supported by the dynamics engine
		displayNonPureNonConvexShapeUseWarning|=App::ct->dynamicsContainer->displayNonPureNonConvexShapeWarningRequired();
		if (App::ct->dynamicsContainer->isWorldThere())
		{
			displayNonStandardParams|=App::ct->dynamicsContainer->displayNonDefaultParameterWarningRequired();
			displayStaticShapeOnDynamicConstructionWarning|=App::ct->dynamicsContainer->displayStaticShapeOnDynamicConstructionWarningRequired();
		}
	}

	if (displayNonStandardParams&&(App::mainWindow!=NULL))
	{
		CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
		std::string val;
		cont.readData("SIMSETTINGS_WARNING_NO_SHOW",val);
		int intVal=0;
		tt::getValidInt(val,intVal);
		if (intVal<3)
		{
			if (!App::isFullScreen())
			{
				// following condition is important: during simulation, the SIM thread might request the UI thread to do
				// some tasks. This is not possible when a modal dialog is displayed. So we simply
				// try to pause the SIM thread while we are displaying a modal dialog:
				IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
				{
					CQDlgMessageAndCheckbox dlg(App::mainWindow);
					dlg.title=strTranslate(IDSN_SIMULATION_PARAMETERS);
					dlg.text=strTranslate(IDSN_NON_STANDARD_SIM_PARAMS_WARNING);
					dlg.checkbox=strTranslate(IDSN_DO_NOT_SHOW_THIS_MESSAGE_AGAIN_3X);

					dlg.refresh();
					dlg.makeDialogModal();
					if (dlg.checkboxState)
					{
						intVal++;
						val=tt::FNb(intVal);
						cont.writeData("SIMSETTINGS_WARNING_NO_SHOW",val,!App::userSettings->doNotWritePersistentData);
					}
				}
			}
		}
	}

	if (App::ct->dynamicsContainer->displayVortexPluginIsDemoRequired()&&(App::mainWindow!=NULL))
	{
		if (!App::isFullScreen())
			App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_PHYSICS_ENGINE),strTranslate(IDS_WARNING_WITH_VORTEX_DEMO_PLUGIN),VMessageBox::OKELI);
	}

	if (displayNonPureNonConvexShapeUseWarning&&(App::mainWindow!=NULL))
	{
		CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
		std::string val;
		cont.readData("NONPURESHAPEFORDYNAMICS_WARNING_NO_SHOW",val);
		int intVal=0;
		tt::getValidInt(val,intVal);
		if (intVal<3)
		{
			if (!App::isFullScreen())
			{
				// following condition is important: during simulation, the SIM thread might request the UI thread to do
				// some tasks. This is not possible when a modal dialog is displayed. So we simply
				// try to pause the SIM thread while we are displaying a modal dialog:
				IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
				{
					CQDlgMessageAndCheckbox dlg(App::mainWindow);
					dlg.title=strTranslate(IDSN_DYNAMIC_CONTENT);
					dlg.text=strTranslate(IDSN_USING_NON_PURE_NON_CONVEX_SHAPES_FOR_DYNAMICS_WARNING);
					dlg.checkbox=strTranslate(IDSN_DO_NOT_SHOW_THIS_MESSAGE_AGAIN_3X);

					dlg.refresh();
					dlg.makeDialogModal();
					if (dlg.checkboxState)
					{
						intVal++;
						val=tt::FNb(intVal);
						cont.writeData("NONPURESHAPEFORDYNAMICS_WARNING_NO_SHOW",val,!App::userSettings->doNotWritePersistentData);
					}
				}
			}
		}
	}

	if (displayStaticShapeOnDynamicConstructionWarning&&(App::mainWindow!=NULL))
	{
		CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
		std::string val;
		cont.readData("STATICSHAPEONTOPOFDYNAMICCONSTRUCTION_WARNING_NO_SHOW",val);
		int intVal=0;
		tt::getValidInt(val,intVal);
		if (intVal<3)
		{
			if (!App::isFullScreen())
			{
				// following condition is important: during simulation, the SIM thread might request the UI thread to do
				// some tasks. This is not possible when a modal dialog is displayed. So we simply
				// try to pause the SIM thread while we are displaying a modal dialog:
				IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
				{
					CQDlgMessageAndCheckbox dlg(App::mainWindow);
					dlg.title=strTranslate(IDSN_DYNAMIC_CONTENT);
					dlg.text=strTranslate(IDSN_USING_STATIC_SHAPE_ON_TOP_OF_DYNAMIC_CONSTRUCTION_WARNING);
					dlg.checkbox=strTranslate(IDSN_DO_NOT_SHOW_THIS_MESSAGE_AGAIN_3X);

					dlg.refresh();
					dlg.makeDialogModal();
					if (dlg.checkboxState)
					{
						intVal++;
						val=tt::FNb(intVal);
						cont.writeData("STATICSHAPEONTOPOFDYNAMICCONSTRUCTION_WARNING_NO_SHOW",val,!App::userSettings->doNotWritePersistentData);
					}
				}
			}
		}
	}
	doVariousUiTasksOnRegularBasisPending=false;
}

