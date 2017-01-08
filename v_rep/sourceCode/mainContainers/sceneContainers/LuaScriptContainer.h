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
#include "LuaScriptObject.h"
#include "LuaObjectGroup.h"
#include "BroadcastDataContainer.h"
#include "v_rep_internal.h"
#include "VMenubar.h"


#define ADDON_SCRIPT_PREFIX_AUTOSTART "vrepAddOnScript_"
#define ADDON_SCRIPT_PREFIX_NOAUTOSTART "vrepAddOnScript-"
#define ADDON_FUNCTION_PREFIX "vrepAddOnFunc_"
#define ADDON_EXTENTION "lua"

class CLuaScriptContainer : public CMainCont 
{
public:
	CLuaScriptContainer();
	virtual ~CLuaScriptContainer();
	void simulationAboutToStart();
	void simulationEnded();
	void renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib);
	void removeAllScripts(bool keepAddOnScripts);
	void announceObjectWillBeErased(int objectID);
	void addMenu(VMenu* menu);
	bool evaluateMenuSelection(int commandID);
	bool removeScript(int scriptID);
	int insertScript(CLuaScriptObject* script);
	CLuaScriptObject* getScriptFromID(int scriptID);
	CLuaScriptObject* getScriptFromPseudoName(std::string scriptPseudoName);
	CLuaScriptObject* getMainScript();
	CLuaScriptObject* getScriptFromObjectAttachedTo(int threeDObjectID);
	CLuaScriptObject* getScriptFromObjectAttachedTo_callback(int threeDObjectID);
	CLuaScriptObject* getCustomContactHandlingScript_callback();
	void setLuaCallError(const char* functionName,const char* errMsg);
	void setLuaCallWarning(const char* functionName,const char* errMsg);
	void setLuaError(const char* functionName,const char* errMsg,int errorType);
	void setLuaWarning(const char* functionName,const char* errMsg);
	void resetAllSimulationScripts();
	void clearErrorFlag();
	bool isErrorFlagSet();
	int insertDefaultScript_mainAndChildScriptsOnly(int scriptType,bool threaded);
	int insertAddOnScripts();
	int prepareAddOnFunctionNames();
	void setMainScriptLaunched(bool launched);
	bool getMainScriptLaunched();
	void setInAddOnNow(bool inAddOn);
	bool getInAddOnNow();
	void setInLuaCallbackNow(bool inCallback);
	bool getInLuaCallbackNow();
	void removeLastExecutedMainScriptReturnValues();
	void removeMainScriptInputValues();
	void _removeDestroyedScripts();
	void addCallbackStructureObjectToDestroyAtTheEndOfSimulation(SLuaCallBack* object);
	bool addCommandToOutsideCommandQueues(int commandID,int auxVal1,int auxVal2,int auxVal3,int auxVal4,const float aux2Vals[8],int aux2Count);

	void handleAddOnScriptExecution();

	static void setAdditionalAddOnScript(std::string scriptNameWithExtension,bool firstSceneOnly);

	std::vector<CLuaScriptObject*> allScripts;

	std::vector<std::string> allAddOnFunctionNames;

	CLuaObjectGroup* lastExecutedMainScriptReturnValues;
	CLuaObjectGroup* mainScriptInputValues;

	int getScriptSimulationParameter_mainAndChildScriptsOnly(int scriptHandle,const char* parameterName,std::string& parameterValue);
	int setScriptSimulationParameter_mainAndChildScriptsOnly(int scriptHandle,const char* parameterName,const char* parameterValue,int parameterValueLength);

	CBroadcastDataContainer broadcastDataContainer;

protected:
	bool mainScriptLaunched;
	bool _inAddOnNow;
	bool _inCallbackNow;

	bool errorFlag;
	std::vector<SLuaCallBack*> _callbackStructureToDestroyAtEndOfSimulation;
	static std::string _additionalAddOnScriptFirstScene;
	static std::string _additionalAddOnScriptAllScenes;
};
