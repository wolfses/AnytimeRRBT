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
#include "VThread.h"
#include "Ser.h"
#include "VMenubar.h"

class CSimulation : public CMainCont
{
public:
	CSimulation();
	virtual ~CSimulation();
	void simulationAboutToStart();
	void simulationEnded();
	void renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib);
	void newSceneProcedure();
	bool startOrResumeSimulation();
	bool stopSimulation();
	bool pauseSimulation();
	bool isSimulationRunning();
	bool isSimulationPaused();
	bool isSimulationStopped();
	void advanceSimulationByOneStep();
	void setDefaultSimulationParameterIndex(int d);
	int getDefaultSimulationParameterIndex();
	void setRealTimeSimulation(bool realTime);
	bool getRealTimeSimulation();
	void serialize(CSer& ar);

	float getSimulationTime();
	float getSimulationTime_real();
	void clearSimulationTimeHistory();
	void addToSimulationTimeHistory(float simTime,float simTimeReal);
	bool getSimulationTimeHistoryDurations(float& simTime,float& simTimeReal);
	void setResetSceneAtSimulationEnd(bool r);
	bool getResetSceneAtSimulationEnd();
	void setRemoveNewObjectsAtSimulationEnd(bool r);
	bool getRemoveNewObjectsAtSimulationEnd();

	void setAvoidBlocking(bool avoidBlocking);
	bool getAvoidBlocking();

	void incrementStopRequestCounter();
	int getStopRequestCounter();

	bool canGoSlower();
	bool canGoFaster();
	void goFasterOrSlower(int action);
	bool canToggleThreadedRendering();
	void toggleThreadedRendering(bool noWarningMessage);
	bool getThreadedRendering();
	bool getThreadedRenderingIfSimulationWasRunning();
	float getSpeedModifier_forTimeStep();
	int getSpeedModifier_forCalcPassPerRendering();
	float getSpeedModifier_forRealTimeCoefficient();

	void setSimulationTimeStep_raw(float dt);
	float getSimulationTimeStep_raw(int parameterIndex=-1);
	float getSimulationTimeStep_speedModified(int parameterIndex=-1);

	void setSimulationPassesPerRendering_raw(int n);
	int getSimulationPassesPerRendering_raw();
	int getSimulationPassesPerRendering_speedModified();
	float getRealTimeCoefficient_raw();
	void setRealTimeCoefficient_raw(float coeff);
	float getRealTimeCoefficient_speedModified();

	bool getDynamicContentVisualizationOnly();
	void setDynamicContentVisualizationOnly(bool dynOnly);


	void setSimulationTimeToPause(float t);
	float getSimulationTimeToPause();

	void adjustRealTimeTimer(float deltaTime);

	void setFullscreenAtSimulationStart(bool f);
	bool getFullscreenAtSimulationStart();


	bool isRealTimeCalculationStepNeeded();

	void setSimulationStateDirect(int state);

	void emergencyStopPassFromLuaHook();

	void addMenu(VMenu* menu);
	bool evaluateMenuSelection(int commandID);
	bool getInfo(std::string& txtLeft,std::string& txtRight,int& index);

	void setPauseAtError(bool br);
	bool getPauseAtError();
	void pauseOnErrorRequested();
	bool getPauseOnErrorRequested();
	void setPauseTime(float time);
	float getPauseTime();
	bool getPauseEnabled();
	void enablePause(bool e);
	void setCatchUpIfLate(bool c);
	bool getCatchUpIfLate();

	int getSimulationState();

	bool getDisplayWarningAboutNonDefaultParameters();

private:

	bool _resetSimulationAtEnd;
	bool _removeNewObjectsAtSimulationEnd;
	std::vector<int> _initialObjectUniqueIdentifiersForRemovingNewObjects;

	bool _catchUpIfLate;
	bool _avoidBlocking;
	bool _fullscreenAtSimulationStart;

	float simulationTime;
	float simulationTime_real;
	float simulationTime_real_noCatchUp;
	int simulationTime_real_lastInMs;
	float _realTimeCorrection;

	int _simulationStepCount;

	int _savedMouseMode;

	bool _threadedRenderingToggle;
	bool _threadedRenderingMessageShown;

	bool _displayedWarningAboutNonDefaultParameters;

	int _defaultSimulationParameterIndex;
	int _simulationPassesPerRendering;
	float _simulationTimeStep;
	int _speedModifierIndexOffset;


	int timeInMsWhenStopWasPressed;

	bool realTimeSimulation;
	float _realTimeCoefficient;

	float _simulationTimeToPause;
	bool _automaticPauseEnabled;
	bool _pauseAtError;
	bool _pauseOnErrorRequested;


	volatile static int emergencyStopButtonThreadForMainScriptsLaunched;

	int simulationState;
	bool _requestToStop; // Indicates a passage from paused to stop (but through running!). Check the simulation state diagram!!
	bool _requestToPause; // added on 2010/01/13 (simPauseSimulation didn't work in the first simulation pass)

	// Following is needed to determine if we are really in real-time mode
	std::vector<float> simulationTime_history;
	std::vector<float> simulationTime_real_history;

	bool _initialValuesInitialized;
	bool _initialAutomaticPauseEnabled;
	bool _dynamicContentVisualizationOnly;

	int _stopRequestCounter;
};
