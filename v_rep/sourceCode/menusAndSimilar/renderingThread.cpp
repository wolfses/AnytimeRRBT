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

#include "renderingThread.h"
#include "VThread.h"
#include "VDateTime.h"
#include "IloIlo.h"
#include "renderingThreadLock.h"


CRenderingThread::CRenderingThread(QThread* mainThread) : QThread()
{
	_phase=0;
	_freeRunning=false;
	_frameId=0;
	_frameIdDone=0;
	_timeToRender=0;
	_mainThread=mainThread;
}

CRenderingThread::~CRenderingThread()
{
}

void CRenderingThread::run()
{
	while (_phase==0)
		VThread::sleep(1); // FOLLOWING IS DANGEROUS HERE : VThread::switchThread();

	_initGl();

	while (true)
	{
		static int lastRenderTime=VDateTime::getTimeInMs();
		if (_freeRunning)
		{
			while ((_frameIdDone==_frameId)&&(VDateTime::getTimeDiffInMs(lastRenderTime)<300)&&(_phase==1))
				VThread::sleep(1); // FOLLOWING IS DANGEROUS HERE : VThread::switchThread();
		}
		else
		{
			while ((_frameIdDone==_frameId)&&(_phase==1))
				VThread::sleep(1); // FOLLOWING IS DANGEROUS HERE : VThread::switchThread();
		}
		if (_phase!=1)
			break;
		int renderStartTime=VDateTime::getTimeInMs();
		_renderScene();
		lastRenderTime=VDateTime::getTimeInMs();
		_timeToRender=VDateTime::getTimeDiffInMs(renderStartTime,lastRenderTime);
		_frameIdDone=_frameId;
	}
	CIloIlo::mainWindow->openglWidget->doneCurrent();
	CIloIlo::mainWindow->openglWidget->context()->moveToThread(_mainThread);
}

void CRenderingThread::initialize()
{
	_phase=1;
}

void CRenderingThread::requestTermination()
{
	_phase=2;
}

int CRenderingThread::requestRendering()
{
	_frameId++;
	if (!_freeRunning)
	{
		while (_frameIdDone!=_frameId)
			VThread::sleep(1); // FOLLOWING IS DANGEROUS HERE : VThread::switchThread();
	}
	return(_timeToRender);
}

void CRenderingThread::setFreeRunning(bool free)
{
	_freeRunning=free;
}

void CRenderingThread::_initGl()
{
	CRenderingThreadLock lock("CRenderingThread::_initGl");
	CIloIlo::mainWindow->initGL();
}

void CRenderingThread::_renderScene()
{
	CRenderingThreadLock lock("CRenderingThread::_renderScene");
	CIloIlo::mainWindow->renderOpenGlContent_callFromRenderingThreadOnly();
}
