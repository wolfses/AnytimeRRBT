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
#include <string>

class CInfoDisplay : public CMainCont
{
public:
	CInfoDisplay();
	virtual ~CInfoDisplay();
	void newSceneProcedure();
	void printInformation();
	void simulationAboutToStart();
	void simulationEnded();
	void renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib);

	void prepareInfo();
	void resetInfo();
	void scriptStart(bool threaded,bool mainScript);
	void mainScriptPaused(int pauseTimeInMs);
	void scriptEnd(bool threaded,bool mainScript);
	void runningThreadedScriptStart(int threadedScriptCount);
	void runningThreadedScriptEnd();
	void setMainScriptMessage(const char* msg);
	void collisionDetectionStart();
	void collisionDetectionEnd(bool detected);
	void distanceCalculationStart();
	void distanceCalculationEnd();
	void proximitySensorSimulationStart();
	void proximitySensorSimulationEnd(bool detected);
	void visionSensorSimulationStart();
	void visionSensorSimulationEnd(bool detected);
	void inverseKinematicsStart();
	void inverseKinematicsEnd();
	void renderingStart();
	void renderingEnd();
	void clearRenderingTime();
	void setWorkThreadExecutionDuration_inMs(int d);


	void gcsStart();
	void gcsEnd();
	void dynamicsStart();
	void dynamicsEnd(int calcPasses,bool dynamicContent);
	void millSimulationStart();
	void millSimulationEnd(float surfaceRemoved,float volumeRemoved);

	void simulationPassStart();
	void simulationPassEnd();

	float getCollisionCalculationTime();
	float getDistanceCalculationTime();
	float getProximitySensorCalculationTime();
	float getVisionSensorCalculationTime();
	float getIkCalculationTime();
	float getGcsCalculationTime();
	float getMainScriptExecutionTime();
	float getMillingCalculationTime();
	float getDynamicsCalculationTime();
	float getSimulationPassExecutionTime();
	float getRenderingDuration();
	float getWorkThreadExecutionDuration();
	int getWorkThreadExecutionDuration_inMs();

private:
	int _regularScriptCount;
	int _threadedScriptCount;
	std::string _mainScriptMessage;
	int _mainScriptStartTime;
	int _mainScriptDuration;
	int _threadedScriptStartTime;
	int _threadedScriptDuration;

	int _simulationPassStartTime;
	int _simulationPassDuration;

	int _renderingStartTime;
	int _renderingDuration;

	int _workThreadExecutionDuration;

	int _collCalcCount;
	int _collDetectCount;
	int _collStartTime;
	int _collCalcDuration;

	int _distCalcCount;
	int _distStartTime;
	int _distCalcDuration;

	int _sensCalcCount;
	int _sensDetectCount;
	int _sensStartTime;
	int _sensCalcDuration;

	int _rendSensCalcCount;
	int _rendSensDetectCount;
	int _rendSensStartTime;
	int _rendSensCalcDuration;

	int _ikCalcCount;
	int _ikStartTime;
	int _ikCalcDuration;

	int _gcsCalcCount;
	int _gcsStartTime;
	int _gcsCalcDuration;

	int _millCalcCount;
	float _millRemovedSurface;
	float _millRemovedVolume;
	int _millStartTime;
	int _millCalcDuration;

	int _dynamicsStartTime;
	int _dynamicsCalcDuration;
	int _dynamicsCalcPasses;
	bool _dynamicsContentAvailable;

	std::string scriptTxt[2];
	std::string collTxt[2];
	std::string distTxt[2];
	std::string sensTxt[2];
	std::string workThreadsTxt[2];
	std::string rendSensTxt[2];
	std::string ikTxt[2];
	std::string gcsTxt[2];
	std::string dynamicsTxt[2];
	std::string millTxt[2];
};
