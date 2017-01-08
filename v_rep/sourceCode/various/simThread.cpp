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
#include "simThread.h"
#include "VThread.h"
#include "App.h"
#include "FileMenu.h"
#include "IloIlo.h"
#include "v_repStringTable.h"
#include "Tt.h"
#include "VDateTime.h"
#include "simAndUiThreadSync.h"

CSimThread::CSimThread()
{
	qThread=QThread::currentThread();
}

CSimThread::~CSimThread()
{
}

void CSimThread::executeMessages()
{
	FUNCTION_DEBUG;

	int triggerType=_prepareSceneForRenderIfNeeded();

	bool simulationRunning=!App::ct->simulation->isSimulationStopped();
	CSimAndUiThreadSync::simThread_allowUiThreadToWrite();
	CSimAndUiThreadSync::outputNakedDebugMessage("$$W\n");
	CSimAndUiThreadSync::outputNakedDebugMessage("$$W *******************************************************\n");
	CSimAndUiThreadSync::outputNakedDebugMessage("$$W *******************************************************\n");
	CSimAndUiThreadSync::outputDebugMessage(__func__,"SIM thread waiting (safe)");

	if (triggerType>0)
	{ // we need to render. Send the appropriate signal
		if (triggerType==1)
			App::uiThread->requestSceneRender(false);
		if (triggerType==2)
			App::uiThread->requestSceneRender_wait();
	}

	int pass=0;
	while ((pass==0)||(!CSimAndUiThreadSync::simThread_forbidUiThreadToWrite(false)))
	{
		pass++;
		App::qtApp->processEvents(); // important, otherwise offscreen openGl contexts can be the cause for strange UI locks!

		int currentTime=VDateTime::getTimeInMs();
		static int lastHere=currentTime;
		if (VDateTime::getTimeDiffInMs(lastHere,currentTime)>100)
		{
			lastHere=currentTime;
			// Handle here things to be done from the GUI on a regular basis, but that might be blocking (e.g. dlg display).
			// We handle it here instead from the GUI timer, since we might not be able to pause the
			// SIM thread during simulation (kind of a deadlock that resolves as a write failure for the GUI thread)

			// The doVariousUiTasksOnRegularBasis signal triggers a function that is queued in the UI thread,
			// but the SIM thread doesn't wait for execution. This is because in some situations, modal
			// dialogs won't display and lock everything: they sometimes wait for the SIM thread to
			// process its event queue somehow (similar to the offscreen thing above?)
			App::uiThread->doVariousUiTasksOnRegularBasisPending=true;
			App::uiThread->doVariousUiTasksOnRegularBasis();
			while (App::uiThread->doVariousUiTasksOnRegularBasisPending)
			{
				App::qtApp->processEvents();
				VThread::sleep(5); // when we open modal dialogs, we halt the sim thread. Don't use too much cpu in that case
			}
		}
		if (!simulationRunning)
			VThread::sleep(5); // when we open modal dialogs, we halt the sim thread. Don't use too much cpu in that case
	}
	CSimAndUiThreadSync::outputDebugMessage(__func__,"SIM thread NOT waiting anymore");
	CSimAndUiThreadSync::outputNakedDebugMessage("$$W *******************************************************\n");
	CSimAndUiThreadSync::outputNakedDebugMessage("$$W *******************************************************\n");
	CSimAndUiThreadSync::outputNakedDebugMessage("$$W\n");
}

int CSimThread::_prepareSceneForRenderIfNeeded()
{
	static int lastRenderingTime=0;
	static int frameCount=1000;
	bool render=false;
	if (App::ct->simulation->getSimulationState()&sim_simulation_advancing)
	{
		frameCount++;
		render=(frameCount>=App::ct->simulation->getSimulationPassesPerRendering_speedModified());
		if (render)
			frameCount=0;
	}
	else
	{
		frameCount=1000;
		render=true;
		int sleepTime=(1000/App::userSettings->getIdleFps())-(VDateTime::getTimeDiffInMs(lastRenderingTime));
		const int minSleepTime=4;
		int effectiveSleepTime=sleepTime;
		if (App::mainWindow!=NULL)
		{
			if (App::mainWindow->middleMouseDown||App::mainWindow->leftMouseDown||App::mainWindow->rightMouseDown)
				effectiveSleepTime=minSleepTime;
		}
		if (effectiveSleepTime<minSleepTime)
			effectiveSleepTime=minSleepTime;
		VThread::sleep(effectiveSleepTime);
		//printf("%i\n",effectiveSleepTime);
	}

	if (render)
	{
		lastRenderingTime=VDateTime::getTimeInMs();
		if (App::mainWindow!=NULL)
		{
			App::mainWindow->simThread_prepareToRenderScene();

			if (App::ct->simulation->getThreadedRendering())
				return(1); // means: do not wait
			else
				return(2); // means: wait until rendering finished
		}
		return(0); // we do not want to render
	}
	return(0); // we do not want to render
}
