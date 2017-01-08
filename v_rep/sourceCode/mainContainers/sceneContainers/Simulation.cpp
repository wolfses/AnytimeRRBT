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
#include "v_rep_internal.h"
#include "Simulation.h"
#include "Graph.h"
#include "Tt.h"
#include "IloIlo.h"
#include "OGL.h"

#include "GraphingRoutines.h"
#include "GV.h"
#include "ThreadPool.h"
#include "App.h"
#include "v_repStrings.h"
#include "VDateTime.h"
#include "qdlgmessageandcheckbox.h"
#include "persistentDataContainer.h"

const float SIMULATION_DEFAULT_TIME_STEP[5]={0.2f,0.1f,0.05f,0.025f,0.01f};
const int SIMULATION_DEFAULT_PASSES_PER_RENDERING[5]={1,1,1,1,1};

const int SIMULATION_SPEED_MODIFIER_SEQUENCE[10]={1,2,5,10,20,40,80,160,320,640};
const int SIMULATION_SPEED_MODIFIER_START_INDEX[6]={5,4,3,2,1,3};

CSimulation::CSimulation()
{
	_stopRequestCounter=0;
	newSceneProcedure();
}

CSimulation::~CSimulation()
{
	newSceneProcedure();
}

void CSimulation::newSceneProcedure()
{
	_dynamicContentVisualizationOnly=false;
	simulationState=sim_simulation_stopped;
	simulationTime=0.0f;
	simulationTime_real=0.0f;
	simulationTime_real_noCatchUp=0.0f;
	clearSimulationTimeHistory();

	_defaultSimulationParameterIndex=2; // 2 is for default values
	_simulationTimeStep=SIMULATION_DEFAULT_TIME_STEP[_defaultSimulationParameterIndex]; 
	_simulationPassesPerRendering=SIMULATION_DEFAULT_PASSES_PER_RENDERING[_defaultSimulationParameterIndex]; 
	_speedModifierIndexOffset=0;
	_realTimeCoefficient=1.0f;
	_simulationStepCount=0;
	_simulationTimeToPause=10.0f;
	_automaticPauseEnabled=false;
	_pauseAtError=false;
	_pauseOnErrorRequested=false;
	_catchUpIfLate=false;
	_avoidBlocking=false;
	_initialValuesInitialized=false;
	_resetSimulationAtEnd=true;
	_removeNewObjectsAtSimulationEnd=true;
	realTimeSimulation=false;
	_threadedRenderingToggle=false;
	_fullscreenAtSimulationStart=false;
}

float CSimulation::getSpeedModifier_forTimeStep()
{
	if (isSimulationStopped()||(_speedModifierIndexOffset>=0))
		return(1.0f);
	int indStart=SIMULATION_SPEED_MODIFIER_START_INDEX[_defaultSimulationParameterIndex];
	int ind=indStart+_speedModifierIndexOffset;
	float init=float(SIMULATION_SPEED_MODIFIER_SEQUENCE[indStart]);
	float now=float(SIMULATION_SPEED_MODIFIER_SEQUENCE[ind]);
	return(now/init);
}

int CSimulation::getSpeedModifier_forCalcPassPerRendering()
{
	if (isSimulationStopped()||(_speedModifierIndexOffset<=0))
		return(1);
	return(1<<_speedModifierIndexOffset);
}

float CSimulation::getSpeedModifier_forRealTimeCoefficient()
{
	if (isSimulationStopped()||(_speedModifierIndexOffset==0))
		return(1.0f);

	int indStart=SIMULATION_SPEED_MODIFIER_START_INDEX[_defaultSimulationParameterIndex];
	int ind=indStart+_speedModifierIndexOffset;
	float init=float(SIMULATION_SPEED_MODIFIER_SEQUENCE[indStart]);
	float now=float(SIMULATION_SPEED_MODIFIER_SEQUENCE[ind]);
	return(now/init);
}

void CSimulation::setFullscreenAtSimulationStart(bool f)
{
	_fullscreenAtSimulationStart=f;
}

bool CSimulation::getFullscreenAtSimulationStart()
{
	return(_fullscreenAtSimulationStart);
}

void CSimulation::setAvoidBlocking(bool avoidBlocking)
{
	_avoidBlocking=avoidBlocking;
}

bool CSimulation::getAvoidBlocking()
{
	return(_avoidBlocking);
}

void CSimulation::setRemoveNewObjectsAtSimulationEnd(bool r)
{
	_removeNewObjectsAtSimulationEnd=r;
}

bool CSimulation::getRemoveNewObjectsAtSimulationEnd()
{
	return(_removeNewObjectsAtSimulationEnd);
}

void CSimulation::setResetSceneAtSimulationEnd(bool r)
{
	_resetSimulationAtEnd=r;	
}

bool CSimulation::getResetSceneAtSimulationEnd()
{
	return(_resetSimulationAtEnd);
}

bool CSimulation::getDisplayWarningAboutNonDefaultParameters()
{
	if (isSimulationRunning()&&(_defaultSimulationParameterIndex!=2)&&(!_displayedWarningAboutNonDefaultParameters))
	{
		_displayedWarningAboutNonDefaultParameters=true;
		return(true);
	}
	return(false);
}

void CSimulation::simulationAboutToStart()
{ // careful here: this is called by this through App::ct->simulationAboutToStart!!
	_initialValuesInitialized=true;
	_initialAutomaticPauseEnabled=_automaticPauseEnabled;
	_speedModifierIndexOffset=0;
	_displayedWarningAboutNonDefaultParameters=false;
	_dynamicContentVisualizationOnly=false;
	_threadedRenderingToggle=false;
	_threadedRenderingMessageShown=false;
}

void CSimulation::simulationEnded()
{ // careful here: this is called by this through App::ct->simulationEnded!!
	App::uiThread->showEmergencyStopButtonIfNeeded(false);
	_dynamicContentVisualizationOnly=false;
	if (App::mainWindow!=NULL)
	{
		if (!CIloIlo::simulationShowEnabled)
			App::mainWindow->simulationRecorder->stopRecording(false);
	}
	if (_initialValuesInitialized)
	{
		_automaticPauseEnabled=_initialAutomaticPauseEnabled; // we do not reset this!
		if (_resetSimulationAtEnd)
		{
		}
	}
	_speedModifierIndexOffset=0;
	_initialValuesInitialized=false;
	_threadedRenderingToggle=false;
}

void CSimulation::setCatchUpIfLate(bool c)
{
	_catchUpIfLate=c;
}

bool CSimulation::getCatchUpIfLate()
{
	return(_catchUpIfLate);
}

void CSimulation::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

bool CSimulation::getDynamicContentVisualizationOnly()
{
	return(_dynamicContentVisualizationOnly);
}

void CSimulation::setDynamicContentVisualizationOnly(bool dynOnly)
{
	if ((!isSimulationStopped())||(!dynOnly))
		_dynamicContentVisualizationOnly=dynOnly;
	App::ct->objCont->setFullDialogRefreshFlag(); // so we reflect the effect also to the toolbar button
	App::ct->objCont->toolbarRefreshFlag=true;
}

void CSimulation::setDefaultSimulationParameterIndex(int d)
{
	d=tt::getLimitedInt(0,5,d);
	_defaultSimulationParameterIndex=d;
	App::ct->objCont->setFullDialogRefreshFlag(); // so that the recorder dlg gets correctly refreshed
}

int CSimulation::getDefaultSimulationParameterIndex()
{
	return(_defaultSimulationParameterIndex);
}


bool CSimulation::canGoSlower()
{
	if (!isSimulationRunning())
		return(false);
	if (App::ct->objCont->getEditModeType()!=NO_EDIT_MODE)
		return(false);

	int indStart=SIMULATION_SPEED_MODIFIER_START_INDEX[_defaultSimulationParameterIndex];
	int ind=indStart+_speedModifierIndexOffset;
	return(ind>0);
}

bool CSimulation::canGoFaster()
{
	if (!isSimulationRunning())
		return(false);
	if (App::ct->objCont->getEditModeType()!=NO_EDIT_MODE)
		return(false);

	int indStart=SIMULATION_SPEED_MODIFIER_START_INDEX[_defaultSimulationParameterIndex];
	int ind=indStart+_speedModifierIndexOffset;
	return(ind<9);
}

bool CSimulation::startOrResumeSimulation()
{
	if (isSimulationStopped())
	{
		App::uiThread->setFullscreen(_fullscreenAtSimulationStart);
		_initialObjectUniqueIdentifiersForRemovingNewObjects.clear();
		for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
		{
			C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
			_initialObjectUniqueIdentifiersForRemovingNewObjects.push_back(it->getUniqueID());
		}
		App::ct->undoBufferContainer->announceChange();
		_savedMouseMode=App::ct->getMouseMode();

		_speedModifierIndexOffset=0;

		_pauseOnErrorRequested=false;
		CThreadPool::setEmergencyStopForCurrentInstance(false);
		CThreadPool::setRequestStopForCurrentInstance(false);

		if (App::mainWindow!=NULL)
			App::ct->scintillaEditorContainer->applyChanges();

		_realTimeCorrection=0.0f;

		simulationState=sim_simulation_advancing_firstafterstop;

		// COMMENTED FOLLOWING FEW LINES ON 2/12/2013: this was initially (many years ago) done in order
		// to have all calculation structures needed calculated at simulation start, and not "when needed".
		// But this causes many problems. Also, the vision sensors, even if explicitely handled, will create
		// its offscreen buffer here, which will be problematic with threaded scripts that call simHandleVisionSensor
//		App::uiThread->addStatusLine(IDSNS_PREPARING_INITIAL_CALCULATION_STRUCTURES);
//		simHandleCollision_internal(sim_handle_all);
//		simHandleDistance_internal(sim_handle_all,NULL);
//		simHandleProximitySensor_internal(sim_handle_all,NULL,NULL,NULL);
//		simHandleVisionSensor_internal(sim_handle_all,NULL,NULL);
	
//		for (int i=0;i<int(App::ct->objCont->millList.size());i++)
//		{ // we can't call simHandleMill(sim_handle_all,NULL) because we would cut some potential object already here!!
//			float dummySurface,dummyVolume;
//			CMill* it=App::ct->objCont->getMill(App::ct->objCont->millList[i]);
//			it->handleMill(false,dummySurface,dummyVolume,true);
//		}


		// COMMENTED FOLLOWING FEW LINES ON 2/12/2013: this was initially done (many years ago) when
		// we didn't have the "simulationAboutToRun" routines. This is not needed anymore I believe.
//		simResetCollision_internal(sim_handle_all);
//		simResetDistance_internal(sim_handle_all);
//		simResetProximitySensor_internal(sim_handle_all);
//		simResetVisionSensor_internal(sim_handle_all);
//		simResetMill_internal(sim_handle_all);
//		App::uiThread->addStatusLine(IDSNS_DONE);

		simResetScript_internal(sim_handle_all); // in future, put this to an "simulationAboutToStart" routine!!!

		App::ct->infoDisplay->resetInfo();
		App::ct->ikGroups->resetCalculationResults();
		simulationTime=0.0f;
		simulationTime_real=0.0f;
		simulationTime_real_noCatchUp=0.0f;
		clearSimulationTimeHistory();
		_requestToStop=false;
		_requestToPause=false; 

		simulationTime_real_lastInMs=VDateTime::getTimeInMs();
		_simulationStepCount=0;
		App::ct->simulationAboutToStart();

		App::ct->objCont->toolbarRefreshFlag=true;
		App::ct->objCont->setFullDialogRefreshFlag();

		App::uiThread->addStatusLine(IDSNS_SIMULATION_STARTED);
		if (App::mainWindow!=NULL)
			App::mainWindow->simulationRecorder->startRecording(false);
		return(true);
	}
	else if (isSimulationPaused())
	{
		_realTimeCorrection=0.0f;
		if (App::mainWindow!=NULL)
			App::ct->scintillaEditorContainer->applyChanges();
		simulationState=sim_simulation_advancing_firstafterpause;
		simulationTime_real_lastInMs=VDateTime::getTimeInMs();
		App::ct->objCont->toolbarRefreshFlag=true;
		App::ct->objCont->setFullDialogRefreshFlag();
		App::uiThread->addStatusLine(IDSNS_SIMULATION_RESUMED);
		_requestToPause=false;
		return(true);
	}
	// Following not used anymore??
	_requestToPause=false;
	App::ct->objCont->toolbarRefreshFlag=true;
	App::ct->objCont->setFullDialogRefreshFlag();
	return(false);
}

bool CSimulation::stopSimulation()
{
	if (simulationState!=sim_simulation_stopped)
	{
		App::uiThread->addStatusLine(IDSNS_SIMULATION_STOPPING);
		App::uiThread->setFullscreen(false);
	}

	if ((simulationState==sim_simulation_advancing_abouttostop)||
		(simulationState==sim_simulation_advancing_lastbeforestop))
		return(true); // in this situation, we are stopping anyway!!
	if (simulationState==sim_simulation_paused)
	{ // Special case here: we have to change the state directly here (and not automatically in "advanceSimulationByOneStep")
		simulationState=sim_simulation_advancing_firstafterpause;
	}
	if (!_requestToStop)
	{
		timeInMsWhenStopWasPressed=VDateTime::getTimeInMs();
		_requestToStop=true;
	}
	return(true);
}

bool CSimulation::pauseSimulation()
{
	if ((simulationState!=sim_simulation_advancing_firstafterstop)&&
		(simulationState!=sim_simulation_advancing_running)&&
		(simulationState!=sim_simulation_advancing_firstafterpause))
		return(false); // in these situations, we are already about to pause or stopping anyway!!
	if (_requestToStop)
		return(false);
	_requestToPause=true;
	return(true);	
}

bool CSimulation::isSimulationRunning()
{ 
	return((simulationState&sim_simulation_advancing)!=0);
}

bool CSimulation::isSimulationStopped()
{ 
	return(simulationState==sim_simulation_stopped); 
}

bool CSimulation::isSimulationPaused()
{ 
	return(simulationState==sim_simulation_paused); 
}

void CSimulation::adjustRealTimeTimer(float deltaTime)
{ // Keep this routine simple! Is is called in a loop for all instances
	_realTimeCorrection+=deltaTime;
}

void CSimulation::advanceSimulationByOneStep()
{
	if (!isSimulationRunning())
		return;

	if (_automaticPauseEnabled&&(simulationTime>=_simulationTimeToPause))
	{
		pauseSimulation();
		_automaticPauseEnabled=false;
	}

	_simulationStepCount++;
	if (_simulationStepCount==1)
		_realTimeCorrection=0.0f;
	App::ct->infoDisplay->prepareInfo();
	App::ct->infoDisplay->resetInfo();
	App::ct->ikGroups->resetCalculationResults();
	simulationTime+=getSimulationTimeStep_speedModified();

	int ct=VDateTime::getTimeInMs();
	float drt=float(VDateTime::getTimeDiffInMs(simulationTime_real_lastInMs)+(_realTimeCorrection*1000.0f))*getRealTimeCoefficient_speedModified()/1000.0f;
	simulationTime_real+=drt;
	simulationTime_real_noCatchUp+=drt;	
	if ( (!_catchUpIfLate)&&(simulationTime_real_noCatchUp>simulationTime+getSimulationTimeStep_speedModified()) )
		simulationTime_real_noCatchUp=simulationTime+getSimulationTimeStep_speedModified();
	_realTimeCorrection=0.0f;
	simulationTime_real_lastInMs=ct;
	addToSimulationTimeHistory(simulationTime,simulationTime_real);

	if (simulationState==sim_simulation_advancing_firstafterstop)
		simulationState=sim_simulation_advancing_running;
	else if (simulationState==sim_simulation_advancing_running)
	{
		if (_requestToStop)
		{
			CThreadPool::setRequestStopForCurrentInstance(true);
			simulationState=sim_simulation_advancing_abouttostop;
			_requestToStop=false;
		}
		else
		{
			if (_requestToPause)
			{
				simulationState=sim_simulation_advancing_lastbeforepause;
				_requestToPause=false;
			}
		}
	}
	else if (simulationState==sim_simulation_advancing_lastbeforepause)
	{
		simulationState=sim_simulation_paused;
		App::ct->objCont->toolbarRefreshFlag=true;
		App::ct->objCont->setFullDialogRefreshFlag();
		App::uiThread->addStatusLine(IDSNS_SIMULATION_PAUSED);
	}
	else if (simulationState==sim_simulation_advancing_firstafterpause)
	{
		simulationState=sim_simulation_advancing_running;
	}
	else if (simulationState==sim_simulation_advancing_abouttostop)
	{
		// Check if all threads have stopped
		if (CThreadPool::getThreadCountForCurrentInstance()==0)
			simulationState=sim_simulation_advancing_lastbeforestop;
	}
	else if (simulationState==sim_simulation_advancing_lastbeforestop)
	{
		CThreadPool::setEmergencyStopForCurrentInstance(false);
		CThreadPool::setRequestStopForCurrentInstance(false);

		simulationState=sim_simulation_stopped;
		App::ct->undoBufferContainer->announceChange(); // keeps this (this has the objects in their last position, including additional objects)
		simResetMilling_internal(sim_handle_all); // important to reset all shapes and volumes!!! (the instruction in the main script might have been removed!)
		App::ct->simulationEnded();
		App::ct->setMouseMode(_savedMouseMode);

		App::ct->objCont->toolbarRefreshFlag=true;
		App::ct->objCont->setFullDialogRefreshFlag();
		App::uiThread->addStatusLine(IDSNS_SIMULATION_STOPPED);
		if (_removeNewObjectsAtSimulationEnd)
		{
			std::vector<C3DObject*> toRemove;
			for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
			{
				C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
				bool found=false;
				for (int j=0;j<int(_initialObjectUniqueIdentifiersForRemovingNewObjects.size());j++)
				{
					if (it->getUniqueID()==_initialObjectUniqueIdentifiersForRemovingNewObjects[j])
					{
						found=true;
						break;
					}
				}
				if (!found)
					toRemove.push_back(it);
			}
			for (int i=0;i<int(toRemove.size());i++)
				App::ct->objCont->eraseObject(toRemove[i]);
		}
		_initialObjectUniqueIdentifiersForRemovingNewObjects.clear();
		App::ct->undoBufferContainer->announceChange(); // keeps this (additional objects were removed, and object positions were reset)
	}
}

int CSimulation::getSimulationState()
{ 
	return(simulationState);
}

void CSimulation::setSimulationTimeStep_raw(float dt)
{
	if (isSimulationStopped()&&(_defaultSimulationParameterIndex==5))
	{
		tt::limitValue(0.0001f,10.0f,dt);
		_simulationTimeStep=dt;
		App::ct->objCont->setFullDialogRefreshFlag(); // so that the recorder dlg gets correctly refreshed
	}
}

float CSimulation::getSimulationTimeStep_raw(int parameterIndex)
{ // parameterIndex is -1 by default
	if (parameterIndex==-1)
		parameterIndex=_defaultSimulationParameterIndex;
	if (parameterIndex==5)
		return(_simulationTimeStep);
	return(SIMULATION_DEFAULT_TIME_STEP[parameterIndex]);
}

float CSimulation::getSimulationTimeStep_speedModified(int parameterIndex)
{ // parameterIndex is -1 by default
	return(getSimulationTimeStep_raw(parameterIndex)*getSpeedModifier_forTimeStep());
}

int CSimulation::getSimulationPassesPerRendering_speedModified()
{
	return(getSimulationPassesPerRendering_raw()*getSpeedModifier_forCalcPassPerRendering());
}

float CSimulation::getRealTimeCoefficient_speedModified()
{
	return(getRealTimeCoefficient_raw()*getSpeedModifier_forRealTimeCoefficient());
}


void CSimulation::setRealTimeSimulation(bool realTime)
{
	if (isSimulationStopped())
		realTimeSimulation=realTime;
}

bool CSimulation::isRealTimeCalculationStepNeeded()
{
	if (!realTimeSimulation)
		return(false);
	if (!isSimulationRunning())
		return(false);
	float crt=simulationTime_real_noCatchUp+float(VDateTime::getTimeDiffInMs(simulationTime_real_lastInMs))*getRealTimeCoefficient_speedModified()/1000.0f;
	return (simulationTime+getSimulationTimeStep_speedModified()<crt);
}

bool CSimulation::getRealTimeSimulation()
{
	return(realTimeSimulation);
}

float CSimulation::getRealTimeCoefficient_raw()
{
	return(_realTimeCoefficient);
}
void CSimulation::setRealTimeCoefficient_raw(float coeff)
{
	tt::limitValue(0.01f,100.0f,coeff);
	_realTimeCoefficient=coeff;
}

void CSimulation::setSimulationPassesPerRendering_raw(int n)
{
	if (_defaultSimulationParameterIndex==5)
	{
		tt::limitValue(1,200,n);
		_simulationPassesPerRendering=n;
	}
}

int CSimulation::getSimulationPassesPerRendering_raw()
{
	if (_defaultSimulationParameterIndex==5)
		return(_simulationPassesPerRendering);
	return(SIMULATION_DEFAULT_PASSES_PER_RENDERING[_defaultSimulationParameterIndex]);
}

void CSimulation::emergencyStopPassFromLuaHook()
{
	if (App::userSettings->abortScriptExecutionButton!=0)
	{
		if ( (VDateTime::getTimeDiffInMs(CLuaScriptObject::getGlobalMainScriptExitTime())>(App::userSettings->abortScriptExecutionButton*1000))&&CLuaScriptObject::getGlobalMainScriptInAndOutCount() )
			App::uiThread->showEmergencyStopButtonIfNeeded(true);
		else
			App::uiThread->showEmergencyStopButtonIfNeeded(false);
	}
}

void CSimulation::setSimulationStateDirect(int state)
{ // Careful with that function!
	simulationState=state;
}

void CSimulation::setSimulationTimeToPause(float t)
{
	t=tt::getLimitedFloat(0.0f,604800.0f,t);
	_simulationTimeToPause=t;
}

float CSimulation::getSimulationTimeToPause()
{
	return(_simulationTimeToPause);
}

void CSimulation::clearSimulationTimeHistory()
{
	simulationTime_history.clear();
	simulationTime_real_history.clear();
}

void CSimulation::addToSimulationTimeHistory(float simTime,float simTimeReal)
{
	simulationTime_history.push_back(simTime);
	simulationTime_real_history.push_back(simTimeReal);
	if (simulationTime_history.size()>10)
	{
		simulationTime_history.erase(simulationTime_history.begin());
		simulationTime_real_history.erase(simulationTime_real_history.begin());
	}
}

bool CSimulation::getSimulationTimeHistoryDurations(float& simTime,float& simTimeReal)
{
	if (simulationTime_history.size()<2)
	{
		simTime=0.0f;
		simTimeReal=0.0f;
		return(false);
	}
	simTime=simulationTime_history[simulationTime_history.size()-1]-simulationTime_history[0];
	simTimeReal=simulationTime_real_history[simulationTime_real_history.size()-1]-simulationTime_real_history[0];
	return(true);
}

void CSimulation::setPauseAtError(bool br)
{
	_pauseAtError=br;
}

bool CSimulation::getPauseAtError()
{
	return(_pauseAtError);
}

void CSimulation::pauseOnErrorRequested()
{
	if (_pauseAtError&&(!_requestToStop))
		_pauseOnErrorRequested=true;
}

bool CSimulation::getPauseOnErrorRequested()
{
	if (_pauseOnErrorRequested)
	{
		_pauseOnErrorRequested=false;
		return(true);
	}
	return(false);
}

void CSimulation::setPauseTime(float time)
{
	time=tt::getLimitedFloat(0.001f,604800.0f,time); // max one week!
	_simulationTimeToPause=time;
}

float CSimulation::getPauseTime()
{
	return (_simulationTimeToPause);
}

bool CSimulation::getPauseEnabled()
{
	return(_automaticPauseEnabled);
}

void CSimulation::enablePause(bool e)
{
	_automaticPauseEnabled=e;
}


float CSimulation::getSimulationTime()
{
	return(simulationTime);
}

float CSimulation::getSimulationTime_real()
{
	return(simulationTime_real);
}

void CSimulation::addMenu(VMenu* menu)
{
	bool noEditMode=(App::ct->objCont->getEditModeType()==NO_EDIT_MODE);

	if (isSimulationPaused())
		VMenubar::appendMenuItem(menu,noEditMode,false,START_RESUME_SIMULATION_REQUEST_CMD,IDS_RESUME_SIMULATION_MENU_ITEM);
	else
		VMenubar::appendMenuItem(menu,noEditMode&&(!isSimulationRunning()),false,START_RESUME_SIMULATION_REQUEST_CMD,IDS_START_SIMULATION_MENU_ITEM);
	VMenubar::appendMenuItem(menu,noEditMode&&isSimulationRunning(),false,PAUSE_SIMULATION_REQUEST_CMD,IDS_PAUSE_SIMULATION_MENU_ITEM);
	VMenubar::appendMenuItem(menu,noEditMode&&(!isSimulationStopped()),false,STOP_SIMULATION_REQUEST_CMD,IDS_STOP_SIMULATION_MENU_ITEM);
	VMenubar::appendMenuSeparator(menu);
	VMenubar::appendMenuItem(menu,noEditMode&&isSimulationStopped(),App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_bullet,TOGGLE_TO_BULLET_ENGINE_CMD,IDS_SWITCH_TO_BULLET_ENGINE_MENU_ITEM,true);
	VMenubar::appendMenuItem(menu,noEditMode&&isSimulationStopped(),App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_ode,TOGGLE_TO_ODE_ENGINE_CMD,IDS_SWITCH_TO_ODE_ENGINE_MENU_ITEM,true);
	VMenubar::appendMenuItem(menu,noEditMode&&isSimulationStopped(),App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_vortex,TOGGLE_TO_VORTEX_ENGINE_CMD,IDS_SWITCH_TO_VORTEX_ENGINE_MENU_ITEM,true);
	VMenubar::appendMenuSeparator(menu);
	VMenubar::appendMenuItem(menu,noEditMode&&isSimulationStopped(),App::ct->simulation->getRealTimeSimulation(),TOGGLE_REAL_TIME_SIMULATION_CMD,IDSN_REAL_TIME_SIMULATION,true);
	VMenubar::appendMenuItem(menu,canGoSlower(),false,SLOWER_SIMULATION_CMD,IDSN_SLOW_DOWN_SIMULATION);
	VMenubar::appendMenuItem(menu,canGoFaster(),false,FASTER_SIMULATION_CMD,IDSN_SPEED_UP_SIMULATION);
	VMenubar::appendMenuItem(menu,canToggleThreadedRendering(),getThreadedRenderingIfSimulationWasRunning(),THREADED_RENDERING_CMD,IDSN_THREADED_RENDERING,true);

	VMenubar::appendMenuSeparator(menu);
	if (App::mainWindow!=NULL)
	{
		VMenubar::appendMenuItem(menu,true,App::mainWindow->dlgCont->isVisible(SIMULATION_DLG),TOGGLE_SIMULATION_DLG_CMD,IDSN_SIMULATION_SETTINGS,true);
	}
}

void CSimulation::goFasterOrSlower(int action)
{
	if (action<0)
	{ // We wanna go slower
		if (canGoSlower())
		{
			// testing App::uiThread->setFullscreen(false);
			_speedModifierIndexOffset--;
			App::ct->objCont->setLightDialogRefreshFlag();
			App::ct->objCont->toolbarRefreshFlag=true;
		}
	}
	if (action>0)
	{ // We wanna go faster
		if (canGoFaster())
		{
			// testing App::uiThread->setFullscreen(true);
			_speedModifierIndexOffset++;
			App::ct->objCont->setLightDialogRefreshFlag();
			App::ct->objCont->toolbarRefreshFlag=true;
		}
	}
}

bool CSimulation::canToggleThreadedRendering()
{
	return(isSimulationRunning()&&(App::userSettings->threadedRenderingDuringSimulation>=0));
}

void CSimulation::toggleThreadedRendering(bool noWarningMessage)
{
	if (App::mainWindow==NULL)
		noWarningMessage=true;
	_threadedRenderingToggle=!_threadedRenderingToggle;
	if (getThreadedRendering()&&(!_threadedRenderingMessageShown))
	{ // warning message
		_threadedRenderingMessageShown=true;
		if (!noWarningMessage)
		{
			CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
			std::string val;
			cont.readData("THREADEDRENDERING_WARNING_NO_SHOW",val);
			int intVal=0;
			tt::getValidInt(val,intVal);
			if (intVal<3)
			{
				CQDlgMessageAndCheckbox dlg(App::mainWindow);
				dlg.title=strTranslate(IDSN_THREADED_RENDERING);
				dlg.text=strTranslate(IDSN_THREADED_RENDERING_WARNING);
				dlg.checkbox=strTranslate(IDSN_DO_NOT_SHOW_THIS_MESSAGE_AGAIN_3X);
				dlg.refresh();
				dlg.makeDialogModal();
				if (dlg.checkboxState)
				{
					intVal++;
					val=tt::FNb(intVal);
					cont.writeData("THREADEDRENDERING_WARNING_NO_SHOW",val,!App::userSettings->doNotWritePersistentData);
				}
			}
		}
	}
	App::ct->objCont->toolbarRefreshFlag=true; // will trigger a refresh
}

bool CSimulation::getThreadedRendering()
{
	return((!isSimulationStopped())&&getThreadedRenderingIfSimulationWasRunning());
}

bool CSimulation::getThreadedRenderingIfSimulationWasRunning()
{
	bool threaded=(App::userSettings->threadedRenderingDuringSimulation==1);
	if (_threadedRenderingToggle)
		threaded=!threaded;
	return(threaded);
}

void CSimulation::incrementStopRequestCounter()
{
	_stopRequestCounter++;
}

int CSimulation::getStopRequestCounter()
{
	return(_stopRequestCounter);
}

bool CSimulation::evaluateMenuSelection(int commandID)
{ // Return value is true if the command belonged to hierarchy menu and was executed
	if (commandID==TOGGLE_REAL_TIME_SIMULATION_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_REAL_TIME_SIMULATION_CMD")
		{
			if (App::ct->simulation->isSimulationStopped()&&(App::ct->objCont->getEditModeType()==NO_EDIT_MODE) )
			{
				App::ct->simulation->setRealTimeSimulation(!App::ct->simulation->getRealTimeSimulation());
				if (App::ct->simulation->getRealTimeSimulation())
					App::uiThread->addStatusLine(IDSNS_TOGGLED_TO_REAL_TIME_MODE);
				else
					App::uiThread->addStatusLine(IDSNS_TOGGLED_TO_NON_REAL_TIME_MODE);
				App::ct->objCont->setLightDialogRefreshFlag();
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
			}
		}
		return(true);
	}

	if (commandID==SLOWER_SIMULATION_CMD)
	{ 
		IF_UI_EVENT_CAN_READ_DATA_CMD("SLOWER_SIMULATION_CMD")
		{
			goFasterOrSlower(-1);
		}
		return(true);
	}
	if (commandID==FASTER_SIMULATION_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("FASTER_SIMULATION_CMD")
		{
			goFasterOrSlower(1);
		}
		return(true);
	}
	if (commandID==THREADED_RENDERING_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("THREADED_RENDERING_CMD")
		{
			toggleThreadedRendering(true);
		}
		return(true);
	}
	if (commandID==TOGGLE_DYNAMIC_CONTENT_VISUALIZATION_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_DYNAMIC_CONTENT_VISUALIZATION_CMD")
		{
			if (!isSimulationStopped())
				setDynamicContentVisualizationOnly(!getDynamicContentVisualizationOnly());
		}
	}
	if (commandID==START_RESUME_SIMULATION_REQUEST_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("START_RESUME_SIMULATION_REQUEST_CMD")
		{
			App::ct->simulatorMessageQueue->addCommand(sim_message_simulation_start_resume_request,0,0,0,0,NULL,0);
		}
		return(true);
	}
	if (commandID==PAUSE_SIMULATION_REQUEST_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("PAUSE_SIMULATION_REQUEST_CMD")
		{
			App::ct->simulatorMessageQueue->addCommand(sim_message_simulation_pause_request,0,0,0,0,NULL,0);
		}
		return(true);
	}
	if (commandID==STOP_SIMULATION_REQUEST_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("STOP_SIMULATION_REQUEST_CMD")
		{
			App::ct->simulatorMessageQueue->addCommand(sim_message_simulation_stop_request,0,0,0,0,NULL,0);
			incrementStopRequestCounter();
		}
		return(true);
	}
	if (commandID==TOGGLE_TO_BULLET_ENGINE_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_TO_BULLET_ENGINE_CMD")
		{
			App::ct->dynamicsContainer->setDynamicEngineType(sim_physics_bullet);
		}
		return(true);
	}
	if (commandID==TOGGLE_TO_ODE_ENGINE_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_TO_ODE_ENGINE_CMD")
		{
			App::ct->dynamicsContainer->setDynamicEngineType(sim_physics_ode);
		}
		return(true);
	}
	if (commandID==TOGGLE_TO_VORTEX_ENGINE_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_TO_VORTEX_ENGINE_CMD")
		{
			App::ct->dynamicsContainer->setDynamicEngineType(sim_physics_vortex);
		}
		return(true);
	}
	if (commandID==TOGGLE_SIMULATION_DLG_CMD)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("TOGGLE_SIMULATION_DLG_CMD")
		{
			if (App::mainWindow!=NULL)
				App::mainWindow->dlgCont->toggle(SIMULATION_DLG);
		}
		return(true);
	}
	return(false);
}

bool CSimulation::getInfo(std::string& txtLeft,std::string& txtRight,int& index)
{
	if (isSimulationStopped())
		return(false);
	if (index==0)
	{
		txtLeft="Simulation time:";
		if (realTimeSimulation)
		{
			txtRight="";//"&&fg060";
			float st,str;
			if (getSimulationTimeHistoryDurations(st,str))
			{
				if (fabs((st-str)/str)>0.1f)
					txtRight="&&fg930"; // When current simulation speed is too slow
				else
				{
					if (fabs(simulationTime-simulationTime_real)>10.0f*getSimulationTimeStep_speedModified())
						txtRight="&&fg930"; // When simulation is behind
				}
			}
			txtRight+=gv::getHourMinuteSecondMilisecondStr(simulationTime)+" &&fg@@@(real time: ";
			if (fabs(getRealTimeCoefficient_speedModified()-1.0f)<0.01f)
				txtRight+=gv::getHourMinuteSecondMilisecondStr(simulationTime_real)+")";
			else
			{
				txtRight+=gv::getHourMinuteSecondMilisecondStr(simulationTime_real)+" (x";
				txtRight+=tt::FNb(0,getRealTimeCoefficient_speedModified(),3,false)+"))";
			}
			txtRight+=" (dt="+tt::FNb(0,getSimulationTimeStep_speedModified()*1000.0f,1,false)+" ms)";
		}
		else
		{
			txtRight="&&fg@@@"+gv::getHourMinuteSecondMilisecondStr(simulationTime);
			txtRight+=" (dt="+tt::FNb(0,getSimulationTimeStep_speedModified()*1000.0f,1,false)+" ms)";
		}
	}
	else
	{
		index=0;
		return(false);
	}
	index++;
	return(true);
}

void CSimulation::serialize(CSer& ar)
{
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Sts");
		ar << _simulationTimeStep;
		ar.flush();

		ar.storeDataName("Spr");
		ar << _simulationPassesPerRendering;
		ar.flush();

		ar.storeDataName("Spi");
		ar << _defaultSimulationParameterIndex;
		ar.flush();

		ar.storeDataName("Ss2");
		BYTE nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,realTimeSimulation);
		SIM_SET_CLEAR_BIT(nothing,1,_avoidBlocking);
		SIM_SET_CLEAR_BIT(nothing,2,_automaticPauseEnabled);
		SIM_SET_CLEAR_BIT(nothing,3,_pauseAtError);
		SIM_SET_CLEAR_BIT(nothing,4,_catchUpIfLate);
		SIM_SET_CLEAR_BIT(nothing,5,_fullscreenAtSimulationStart);
		SIM_SET_CLEAR_BIT(nothing,6,!_resetSimulationAtEnd);
		SIM_SET_CLEAR_BIT(nothing,7,!_removeNewObjectsAtSimulationEnd);

		ar << nothing;
		ar.flush();

		ar.storeDataName("Rtc");
		ar << _realTimeCoefficient;
		ar.flush();

		ar.storeDataName("Pat");
		ar << _simulationTimeToPause;
		ar.flush();

		ar.storeDataName(SER_END_OF_OBJECT);
	}
	else
	{		// Loading
		int byteQuantity;
		std::string theName="";
		while (theName.compare(SER_END_OF_OBJECT)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_END_OF_OBJECT)!=0)
			{
				bool noHit=true;
				if (theName.compare("Sts")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _simulationTimeStep;
				}
				if (theName.compare("Spr")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _simulationPassesPerRendering;
				}
				if (theName.compare("Spi")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _defaultSimulationParameterIndex;
				}
				if (theName=="Sst")
				{ // for backward compatibility (still in serialization version 15 or before)
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					realTimeSimulation=SIM_IS_BIT_SET(nothing,0);
					_avoidBlocking=SIM_IS_BIT_SET(nothing,1);
					_automaticPauseEnabled=SIM_IS_BIT_SET(nothing,2);
					_pauseAtError=SIM_IS_BIT_SET(nothing,3);
					_catchUpIfLate=SIM_IS_BIT_SET(nothing,4);
					bool defaultSimulationTimeStep=SIM_IS_BIT_SET(nothing,5);
					_resetSimulationAtEnd=!SIM_IS_BIT_SET(nothing,6);
					_removeNewObjectsAtSimulationEnd=!SIM_IS_BIT_SET(nothing,7);
					if (defaultSimulationTimeStep)
						_defaultSimulationParameterIndex=2; // for default parameters
					else
						_defaultSimulationParameterIndex=5; // for custom parameters
				}
				if (theName=="Ss2")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					realTimeSimulation=SIM_IS_BIT_SET(nothing,0);
					_avoidBlocking=SIM_IS_BIT_SET(nothing,1);
					_automaticPauseEnabled=SIM_IS_BIT_SET(nothing,2);
					_pauseAtError=SIM_IS_BIT_SET(nothing,3);
					_catchUpIfLate=SIM_IS_BIT_SET(nothing,4);
					_fullscreenAtSimulationStart=SIM_IS_BIT_SET(nothing,5);
					_resetSimulationAtEnd=!SIM_IS_BIT_SET(nothing,6);
					_removeNewObjectsAtSimulationEnd=!SIM_IS_BIT_SET(nothing,7);
				}

				if (theName.compare("Rtc")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _realTimeCoefficient;
				}
				if (theName.compare("Pat")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _simulationTimeToPause;
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}

volatile int CSimulation::emergencyStopButtonThreadForMainScriptsLaunched=0;



