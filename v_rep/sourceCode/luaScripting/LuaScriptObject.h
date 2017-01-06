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

extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
}

#include "LuaObjectGroup.h"
#include "LuaScriptParameters.h"
#include "OutsideCommandQueueForScript.h"
#include "VMutex.h"
#include "VThread.h"

#define DEFAULT_MAINSCRIPT_NAME "dltmscpt.txt"
#define DEFAULT_NONTHREADEDCHILDSCRIPT_NAME "dltcscpt.txt"
#define DEFAULT_THREADEDCHILDSCRIPT_NAME "dlttscpt.txt"
#define DEFAULT_JOINTCALLBACKSCRIPT_NAME "defaultJointCtrlCallbackScript.txt"
#define DEFAULT_CONTACTCALLBACKSCRIPT_NAME "defaultContactCallbackScript.txt"

class CLuaScriptObject
{
public:
	CLuaScriptObject(bool isMainScriptOrChildScript);
	virtual ~CLuaScriptObject();

	void initializeInitialValues(bool simulationIsRunning);
	void simulationAboutToStart();
	void simulationEnded();

	int getScriptID();
	void setScriptID(int newID);
	std::string getDescriptiveName();
	std::string getShortDescriptiveName();
	std::string getScriptSuffixNumberString();
	std::string getScriptPseudoName();
	void setAddOnName(const char* name);
	std::string getAddOnName();

	CLuaScriptObject* copyYourself();
	void serialize(CSer& ar);
	void perform3DObjectLoadingMapping(std::vector<int>* map);
	bool announce3DObjectWillBeErased(int objectID,bool copyBuffer);
	int getObjectIDThatScriptIsAttachedTo(); // for child scripts
	int getObjectIDThatScriptIsAttachedTo_callback(); // for callback scripts
	void setObjectIDThatScriptIsAttachedTo(int newObjectID); // for child scripts
	void setObjectIDThatScriptIsAttachedTo_callback(int newObjectID); // for callback scripts
	int getLifeID();
	void setScriptText(const char* scriptTxt);
	char* getScriptText();
	int getScriptTextLength();
	int runOneStep(CLuaObjectGroup& inputArguments,CLuaObjectGroup& outputArguments);
	int runOneStepNow(CLuaObjectGroup& inputArguments,CLuaObjectGroup& outputArguments);
	void runAddOn();
	void runJointCtrlCallback(const std::vector<bool>& inDataBool,const std::vector<int>& inDataInt,const std::vector<float>& inDataFloat,std::vector<float>& outDataFloat);
	int runContactCallback(const int inDataInt[3],int outDataInt[3],float outDataFloat[14]);
	void reset(bool dontResetInitialParams=false);
	int getNumberOfPasses();
	int setUserData(char* data);
	char* getUserData(int id);
	void releaseUserData(int id);
	void clearAllUserData();
	void setThreadedExecution(bool threadedExec);
	bool getThreadedExecution();
	void setExecutionOrder(int order);
	int getExecutionOrder();
	bool getDestroyAtEndOfSimulation();
	void setScriptType(int typeID);
	int getScriptType();
	void setExplicitHandling(bool explicitHandl);
	bool getExplicitHandling();
	void setExecuteJustOnce(bool justOnce);
	bool getExecuteJustOnce();
	void setDelegateChildScriptExecution();
	bool getDelegateChildScriptExecution();
	void setExecuteInSensingPhase(bool yesInSensingPhase);
	bool getExecuteInSensingPhase();

	void getPreviousEditionWindowPosAndSize(int posAndSize[4]);
	void setPreviousEditionWindowPosAndSize(const int posAndSize[4]);


	CLuaScriptParameters* getScriptParametersObject();

	static bool getGlobalMainScriptInAndOutCount();
	static int getGlobalMainScriptExitTime();

	void setCustomizedMainScript(bool customized);
	bool isDefaultMainScript();

	int getAddOnExecutionState();
	void setAddOnDesiredExecutionState(int state);

	bool addCommandToOutsideCommandQueue(int commandID,int auxVal1,int auxVal2,int auxVal3,int auxVal4,const float aux2Vals[8],int aux2Count);
	int extractCommandFromOutsideCommandQueue(int auxVals[4],float aux2Vals[8],int& aux2Count);

	static bool sensingScriptsInExecution;

	void fromFileToBuffer(); // when using an external editor
	void fromBufferToFile();

	std::string getFilenameForExternalScriptEditor();

protected:
	// Variables that need to be copied and serialized:
	int scriptID;
	int _scriptType; // sim_scriptproperty_mainscript, etc.
	bool threadedExecution;
	bool _explicitHandling;
	bool _executeJustOnce;
	bool _executeInSensingPhase;
	bool _mainScriptIsDefaultMainScript;
	int _executionOrder;
	int objectIDAttachedTo;
	int _objectIDAttachedTo_callback;
	char* _scriptText;
	int _scriptTextLength;
	char* _scriptTextExec;		// the one getting executed!
	int _scriptTextExecLength;	// the one getting executed!
	CLuaScriptParameters* scriptParameters;
	COutsideCommandQueueForScript* _outsideCommandQueue;

	// Other variables that don't need serialization:
	int _lifeID;
	lua_State* L;
	int numberOfPasses;
	bool threadedExecutionUnderWay;
	bool destroyAtEndOfSimulation;
	bool _executedOnceAtLeast;
	bool _delegateChildScriptExecution;
	int scriptInExecution;
	VMutex _localMutex;
	static VMutex _globalMutex;
	std::string _addOnName;
	int _addOn_executionState; // 0=stopped, 1=running, 2=paused, 3=error
	int _addOn_desiredExecutionState; // 1=run, 2=pause

	std::string _filenameForExternalScriptEditor;
	static int _nextIdForExternalScriptEditor;

	static std::vector<CLuaScriptObject*> toBeCalledByThread;
	static std::vector<CLuaObjectGroup*> inputToTransmitToThreadedScript;
	static VTHREAD_RETURN_TYPE startAddressForThreadedScripts(VTHREAD_ARGUMENT_TYPE lpData);
	static bool globalMainScriptInAndOutCount;
	static int globalMainScriptExitTime;

	std::vector<char*> _userData;
	std::vector<int> _userDataIds;

	bool _initialValuesInitialized;
	bool _initialExplicitHandling;

	int _previousEditionWindowPosAndSize[4];
};
