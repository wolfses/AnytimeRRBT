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
#include "LuaScriptObject.h"
#include "LuaScriptFunctions.h"
#include "IloIlo.h"

#include "LifeControl.h"
#include "ThreadPool.h"
#include "Tt.h"
#include <boost/lexical_cast.hpp>
#include "VDateTime.h"
#include "App.h"

int CLuaScriptObject::_nextIdForExternalScriptEditor=0;

CLuaScriptObject::CLuaScriptObject(bool isMainScriptOrChildScript)
{
	scriptID=SIM_IDSTART_LUASCRIPT;
	objectIDAttachedTo=-1; // used for child scripts
	_objectIDAttachedTo_callback=-1; // used for callback scripts
	_scriptText=NULL;
	_scriptTextLength=0;
	_scriptTextExec=NULL;		// the one getting executed!
	_scriptTextExecLength=0;	// the one getting executed!
	_lifeID=-1;
	numberOfPasses=0;
	threadedExecution=false;
	_explicitHandling=false;
	_executeJustOnce=false;
	_executeInSensingPhase=false;
	_executedOnceAtLeast=false;
	threadedExecutionUnderWay=false;
	destroyAtEndOfSimulation=false;
	_delegateChildScriptExecution=false;
	_mainScriptIsDefaultMainScript=true;
	_executionOrder=SCRIPT_EXECUTION_ORDER_NORMAL;
	_initialExplicitHandling=false;
	_addOn_executionState=0; // stopped
	_addOn_desiredExecutionState=0; // stop

	_previousEditionWindowPosAndSize[0]=100;
	_previousEditionWindowPosAndSize[1]=100;
	_previousEditionWindowPosAndSize[2]=640;
	_previousEditionWindowPosAndSize[3]=680;

	scriptInExecution=0;
	scriptParameters=new CLuaScriptParameters();
	_outsideCommandQueue=new COutsideCommandQueueForScript();

	if (isMainScriptOrChildScript)
		_scriptType=sim_scripttype_childscript;
	else
		_scriptType=sim_scripttype_addonscript;

	L=NULL;

	_filenameForExternalScriptEditor="embScript_"+tt::FNb(_nextIdForExternalScriptEditor++)+".lua";
	if (!App::userSettings->useBuiltInScriptEditor())
	{ 
		if ((_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript))
		{
			// create file
			std::string fname=App::directories->executableDirectory+VREP_SLASH;
			fname.append(_filenameForExternalScriptEditor);

			VFile myFile(fname.c_str(),VFile::CREATE|VFile::WRITE|VFile::SHARE_EXCLUSIVE,true);
			if (myFile.getFile()!=NULL)
			{
				myFile.setLength(0);
				VArchive arW(&myFile,VArchive::STORE);
				arW.close();
				myFile.close();
			}
		}
	}
}

CLuaScriptObject::~CLuaScriptObject()
{
	reset(); // will also call clearAllUserData()!
	CLifeControl::announceDeath(_lifeID);
	delete[] _scriptText;
	delete scriptParameters;
	delete _outsideCommandQueue;
	if (!App::userSettings->useBuiltInScriptEditor())
	{
		if ((_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript))
		{		
			// destroy file
			std::string fname=App::directories->executableDirectory+VREP_SLASH;
			fname.append(_filenameForExternalScriptEditor);
			if (VFile::doesFileExist(fname))
				VFile::eraseFile(fname);
		}
	}
}

void CLuaScriptObject::getPreviousEditionWindowPosAndSize(int posAndSize[4])
{
	for (int i=0;i<4;i++)
		posAndSize[i]=_previousEditionWindowPosAndSize[i];
}

void CLuaScriptObject::setPreviousEditionWindowPosAndSize(const int posAndSize[4])
{
	for (int i=0;i<4;i++)
		_previousEditionWindowPosAndSize[i]=posAndSize[i];
}

std::string CLuaScriptObject::getAddOnName()
{
	return(_addOnName);
}

int CLuaScriptObject::getAddOnExecutionState()
{
	return(_addOn_executionState);
}

void CLuaScriptObject::setAddOnDesiredExecutionState(int state)
{
	_addOn_desiredExecutionState=state;
}


std::string CLuaScriptObject::getFilenameForExternalScriptEditor()
{
	return(_filenameForExternalScriptEditor);
}

void CLuaScriptObject::fromFileToBuffer()
{
	if (!App::userSettings->useBuiltInScriptEditor())
	{ // read file
		if ((_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript))
		{
			std::string fname=App::directories->executableDirectory+VREP_SLASH;
			fname.append(_filenameForExternalScriptEditor);

			VFile myFile(fname.c_str(),VFile::READ|VFile::SHARE_DENY_NONE,true);
			if (myFile.getFile()!=NULL)
			{
				VArchive arW(&myFile,VArchive::LOAD);
				int archiveLength=DWORD(myFile.getLength());
				_scriptTextLength=archiveLength+1;
				delete[] _scriptText;
				_scriptText=new char[_scriptTextLength];
				char ab;
				for (int i=0;i<_scriptTextLength-1;i++)
				{
					arW >> ab;
					_scriptText[i]=ab;
				}
				// there is no closing zero in the file!!
				_scriptText[_scriptTextLength-1]=0;
				arW.close();
				myFile.close();
			}
		}
	}
}

void CLuaScriptObject::fromBufferToFile()
{
	if (!App::userSettings->useBuiltInScriptEditor())
	{ // write file
		if ((_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript))
		{
			std::string fname=App::directories->executableDirectory+VREP_SLASH;
			fname.append(_filenameForExternalScriptEditor);

			VFile myFile(fname.c_str(),VFile::WRITE|VFile::SHARE_EXCLUSIVE,true);
			if (myFile.getFile()!=NULL)
			{
				myFile.setLength(0);
				VArchive arW(&myFile,VArchive::STORE);
				if (_scriptText!=NULL)
				{
					for (int i=0;i<_scriptTextLength-1;i++)
						arW << _scriptText[i];
				}
				// Do not write the closing zero!
				arW.close();
				myFile.close();
			}
		}
	}
}

void CLuaScriptObject::setDelegateChildScriptExecution()
{
	_delegateChildScriptExecution=true;
}

bool CLuaScriptObject::getDelegateChildScriptExecution()
{
	return(_delegateChildScriptExecution);
}

void CLuaScriptObject::setExecuteJustOnce(bool justOnce)
{
	_executeJustOnce=justOnce;
}

bool CLuaScriptObject::getExecuteJustOnce()
{
	return(_executeJustOnce);
}

void CLuaScriptObject::setExecuteInSensingPhase(bool yesInSensingPhase)
{
	_executeInSensingPhase=yesInSensingPhase;
	if (yesInSensingPhase)
	{ // Following items are not compatible:
		threadedExecution=false;
		_explicitHandling=false;
	}
}

bool CLuaScriptObject::getExecuteInSensingPhase()
{
	return(_executeInSensingPhase);
}

void CLuaScriptObject::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	if ((_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript))
	{
		_initialValuesInitialized=simulationIsRunning;
		if (simulationIsRunning)
		{
			_initialExplicitHandling=_explicitHandling;
		}
		_delegateChildScriptExecution=false;
		if (scriptParameters!=NULL)
			scriptParameters->initializeInitialValues(simulationIsRunning);
		if (_outsideCommandQueue!=NULL)
			_outsideCommandQueue->initializeInitialValues(simulationIsRunning);
		_executedOnceAtLeast=false;
	}
}

void CLuaScriptObject::simulationAboutToStart()
{
	if ((_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript)||(_scriptType==sim_scripttype_jointctrlcallback)||(_scriptType==sim_scripttype_contactcallback))
		initializeInitialValues(true);
}

void CLuaScriptObject::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	if ((_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript)||(_scriptType==sim_scripttype_jointctrlcallback)||(_scriptType==sim_scripttype_contactcallback))
	{
		if (scriptParameters!=NULL)
			scriptParameters->simulationEnded();
		if (_outsideCommandQueue!=NULL)
			_outsideCommandQueue->simulationEnded();
		delete[] _scriptTextExec;
		_scriptTextExec=NULL;
		_scriptTextExecLength=0;
		_executedOnceAtLeast=false;
		_delegateChildScriptExecution=false;
		if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
		{
			_explicitHandling=_initialExplicitHandling;
		}
		_initialValuesInitialized=false;
	}
}

void CLuaScriptObject::setExecutionOrder(int order)
{
	_executionOrder=order;
}

int CLuaScriptObject::getExecutionOrder()
{
	return(_executionOrder);
}

void CLuaScriptObject::setCustomizedMainScript(bool customized)
{
	if (_scriptType==sim_scripttype_mainscript)
	{
		_mainScriptIsDefaultMainScript=!customized;
		App::ct->objCont->setFullDialogRefreshFlag();
	}
}

bool CLuaScriptObject::isDefaultMainScript()
{
	return(_mainScriptIsDefaultMainScript);
}

void CLuaScriptObject::setExplicitHandling(bool explicitHandl)
{
	if (explicitHandl)
	{
		if (_scriptType!=sim_scripttype_mainscript)
		{
			_explicitHandling=true;
			_executeInSensingPhase=false;
		}
	}
	else
		_explicitHandling=false;
}

bool CLuaScriptObject::getExplicitHandling()
{
	return(_explicitHandling);
}

void CLuaScriptObject::setScriptType(int typeID)
{ // Don't use unless just after construction!
	_scriptType=typeID;
	if (_scriptType!=sim_scripttype_childscript)
	{
		objectIDAttachedTo=-1;
		_objectIDAttachedTo_callback=-1;
		threadedExecution=false;
		_explicitHandling=false;
	}
}

int CLuaScriptObject::getScriptType()
{
	return(_scriptType);
}

bool CLuaScriptObject::getDestroyAtEndOfSimulation()
{
	return(destroyAtEndOfSimulation);
}

void CLuaScriptObject::setScriptText(const char* scriptTxt)
{
	EASYLOCK(_localMutex);
	if (_scriptText!=NULL)
		delete[] _scriptText;
	_scriptText=NULL;
	_scriptTextLength=0;
	if (scriptTxt!=NULL)
	{
		int l=0;
		while (scriptTxt[l]!=0)
			l++;
		_scriptTextLength=l+1;
		_scriptText=new char[_scriptTextLength];
		for (int i=0;i<_scriptTextLength;i++)
			_scriptText[i]=scriptTxt[i];
	}
	if ((_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript))
		fromBufferToFile();
}

char* CLuaScriptObject::getScriptText()
{
	if ((_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript))
		fromFileToBuffer();
	return(_scriptText);
}

int CLuaScriptObject::getScriptTextLength()
{
	return(_scriptTextLength);
}

int CLuaScriptObject::getLifeID()
{
	if (_lifeID==-1)
		_lifeID=CLifeControl::getLifeID();
	return(_lifeID);
}

int CLuaScriptObject::getScriptID()
{
	return(scriptID);
}

void CLuaScriptObject::setScriptID(int newID)
{ // careful with that function!
	scriptID=newID;
}

std::string CLuaScriptObject::getDescriptiveName()
{ // Cannot put following strings to resources since they are also used in openGL!!!
	std::string pref;
	if ( (_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript) )
	{
		if (!App::userSettings->useBuiltInScriptEditor())
		{
			pref="[";
			pref+=_filenameForExternalScriptEditor;
			pref+="] ";
		}
	}

	if (_scriptType==sim_scripttype_mainscript)
	{
		if (_mainScriptIsDefaultMainScript)
			return(strTranslate(pref+"Main script (default)"));
		return(strTranslate(pref+"Main script (customized)"));
	}
	if (_scriptType==sim_scripttype_childscript)
	{
		if (destroyAtEndOfSimulation)
		{
			//if (threadedExecution)
			//	return(strTranslate("Threaded child script (destroyed)"));
			return(strTranslate(pref+"Child script (destroyed)"));
		}
		C3DObject* it=App::ct->objCont->getObject(objectIDAttachedTo);
		if (it==NULL)
			return(strTranslate(pref+"Child script (unassociated)"));
		std::string retVal;
		retVal=strTranslate(pref+"Child script (associated with ");
		retVal+=it->getName();
		retVal+=")";
		return(retVal);
	}

	if (_scriptType==sim_scripttype_addonscript)
	{
		std::string retVal;
		retVal=strTranslate(pref+"Add-on script '");
		retVal+=_addOnName;
		retVal+="'";
		return(retVal);
	}
	if (_scriptType==sim_scripttype_addonfunction)
	{
		std::string retVal;
		retVal=strTranslate(pref+"Add-on function '");
		retVal+=_addOnName;
		retVal+="'";
		return(retVal);
	}
	if (_scriptType==sim_scripttype_jointctrlcallback)
	{
		std::string retVal;
		retVal=strTranslate(pref+"Joint ctrl callback script ");
		C3DObject* it=App::ct->objCont->getObject(_objectIDAttachedTo_callback);
		if (it==NULL)
			return(strTranslate(pref+"(unassociated)"));

		retVal+="(associated with ";
		retVal+=it->getName();
		retVal+=")";
		return(retVal);
	}
	if (_scriptType==sim_scripttype_contactcallback)
	{
		std::string retVal;
		retVal=strTranslate(pref+"Contact callback script");
		return(retVal);
	}
	return("ERROR");
}

std::string CLuaScriptObject::getShortDescriptiveName()
{ // since 2/10/2012 (because if the name is too long, it is truncated when an error message mentions it)
	std::string pref;
	if ( (_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript) )
	{
		if (!App::userSettings->useBuiltInScriptEditor())
		{
			pref="[";
			pref+=_filenameForExternalScriptEditor;
			pref+="] ";
		}
	}

	if (_scriptType==sim_scripttype_mainscript)
		return(strTranslate(pref+"MAIN SCRIPT"));
	if (_scriptType==sim_scripttype_childscript)
	{
		if (destroyAtEndOfSimulation)
			return(strTranslate(pref+"CHILD SCRIPT (DESTROYED)"));
		C3DObject* it=App::ct->objCont->getObject(objectIDAttachedTo);
		if (it==NULL)
			return(strTranslate(pref+"UNASSOCIATED CHILD SCRIPT"));

		std::string retVal;
		retVal=strTranslate(pref+"SCRIPT ");
		retVal+=it->getName();
		return(retVal);
	}
	if (_scriptType==sim_scripttype_addonscript)
	{
		std::string retVal;
		retVal=strTranslate(pref+"ADDON SCRIPT ");
		retVal+=_addOnName;
		return(retVal);
	}
	if (_scriptType==sim_scripttype_addonfunction)
	{
		std::string retVal;
		retVal=strTranslate(pref+"ADDON FUNCTION ");
		retVal+=_addOnName;
		return(retVal);
	}
	if (_scriptType==sim_scripttype_jointctrlcallback)
	{
		std::string retVal;
		retVal=strTranslate(pref+"JOINT CTRL CALLBACK ");
		C3DObject* it=App::ct->objCont->getObject(_objectIDAttachedTo_callback);
		if (it==NULL)
			return(strTranslate(pref+"(UNASSOCIATED)"));
		retVal+=it->getName();
		return(retVal);
	}
	if (_scriptType==sim_scripttype_contactcallback)
	{
		std::string retVal;
		retVal=strTranslate(pref+"CONTACT CALLBACK");
		return(retVal);
	}
	return("ERROR");
}

void CLuaScriptObject::setAddOnName(const char* name)
{
	_addOnName=name;
}

std::string CLuaScriptObject::getScriptSuffixNumberString()
{
	if (_scriptType==sim_scripttype_childscript)
	{
		C3DObject* it=App::ct->objCont->getObject(objectIDAttachedTo);
		if (it==NULL)
			return("");
		int suffNb=tt::getNameSuffixNumber(it->getName().c_str(),true);
		std::string suffix("");
		if (suffNb!=-1)
			suffix=boost::lexical_cast<std::string>(suffNb);
		return(suffix);
	}
	return(""); // only child scripts can have a suffix number
}

std::string CLuaScriptObject::getScriptPseudoName()
{
	if (_scriptType==sim_scripttype_childscript)
	{
		C3DObject* it=App::ct->objCont->getObject(objectIDAttachedTo);
		if (it!=NULL)
			return(it->getName());
	}
	if ( (_scriptType==sim_scripttype_addonscript)||(_scriptType==sim_scripttype_addonfunction) )
		return(_addOnName);
	if (_scriptType==sim_scripttype_jointctrlcallback)
	{
		C3DObject* it=App::ct->objCont->getObject(_objectIDAttachedTo_callback);
		if (it!=NULL)
			return(it->getName());
	}
	return("");
}

void CLuaScriptObject::setThreadedExecution(bool threadedExec)
{
	if (threadedExec)
	{
		if (_scriptType==sim_scripttype_childscript)
		{
			threadedExecution=true;
			_executeInSensingPhase=false;
		}
	}
	else
		threadedExecution=false;
}

bool CLuaScriptObject::getThreadedExecution()
{
	return(threadedExecution);
}

void CLuaScriptObject::perform3DObjectLoadingMapping(std::vector<int>* map)
{
	if (App::ct->objCont!=NULL)
	{
		objectIDAttachedTo=App::ct->objCont->getLoadingMapping(map,objectIDAttachedTo);
		_objectIDAttachedTo_callback=App::ct->objCont->getLoadingMapping(map,_objectIDAttachedTo_callback);
	}
}

bool CLuaScriptObject::announce3DObjectWillBeErased(int objectID,bool copyBuffer)
{ // script will be erased if attached to objectID (if threaded simulation is not running!)
	if (objectIDAttachedTo==objectID)
	{
		if (copyBuffer)
			return(true);
		if (!App::ct->simulation->isSimulationStopped()) // Removed the if(threadedExecution()) thing on 2008/12/08
		{ // Not only scripts marked as threaded, all have to pass here (scripts marked as non-threaded might have been created by a threaded script!)
			App::ct->scintillaEditorContainer->closeEditor(scriptID);
			objectIDAttachedTo=-1; // This is for a potential threaded simulation running
			destroyAtEndOfSimulation=true;
			return(false);
		}
		else
			return(true);
	}
	if (_objectIDAttachedTo_callback==objectID)
		return(true);
	return(false);
}

int CLuaScriptObject::getObjectIDThatScriptIsAttachedTo()
{
	return(objectIDAttachedTo);
}

int CLuaScriptObject::getObjectIDThatScriptIsAttachedTo_callback()
{
	return(_objectIDAttachedTo_callback);
}

void CLuaScriptObject::setObjectIDThatScriptIsAttachedTo(int newObjectID)
{
	objectIDAttachedTo=newObjectID;
	if (newObjectID!=-1)
		_scriptType=sim_scripttype_childscript;
}

void CLuaScriptObject::setObjectIDThatScriptIsAttachedTo_callback(int newObjectID)
{
	_objectIDAttachedTo_callback=newObjectID;
}

int CLuaScriptObject::getNumberOfPasses()
{
	return(numberOfPasses);
}

bool CLuaScriptObject::getGlobalMainScriptInAndOutCount()
{
	return(globalMainScriptInAndOutCount);
}

int CLuaScriptObject::getGlobalMainScriptExitTime()
{
	return(globalMainScriptExitTime);
}

// Following only for main or child scripts!!!
int CLuaScriptObject::runOneStep(CLuaObjectGroup& inputArguments,CLuaObjectGroup& outputArguments)
{ // return value 0 indicates nothing to execute, 1 no error, 2 a lua compilation or runtime error, 3 a lua simulator call error (from this script or its child scripts), 4 if a script is being called several times (reentrance problem)
	int retVal=sim_script_no_error;
	scriptInExecution++;
	if (scriptInExecution!=1)
	{ // This cannot happen with main scripts, but can happen with non-threaded child scripts called by threaded childscript whose associated object was destroyed! 
		scriptInExecution--;
		return(retVal);
	}
	if (getScriptType()==sim_scripttype_mainscript)
	{
		// PUT THESE THINGS SOMEWHERE ELSE (E.G. INTO AN OWN simPrepareSimulationStep)!!!!!!!
		//***********************************************************************
		App::ct->luaScriptContainer->broadcastDataContainer.removeTimedOutObjects(App::ct->simulation->getSimulationTime()); // remove invalid elements
		globalMainScriptExitTime=VDateTime::getTimeInMs();
		App::ct->infoDisplay->runningThreadedScriptStart(CThreadPool::getThreadCountForCurrentInstance());

		// Following 3 lines changed/added on 24/02/2011:
		// CThreadPool::handleAllFibers(true);
		CThreadPool::prepareAllThreadsForResume_calledBeforeMainScript();
		CThreadPool::handleAllThreads_withResumeLocation(0);

		App::ct->infoDisplay->runningThreadedScriptEnd();

// FOLLOWING INSTRUCTION REMOVED ON 8/1/2014:
// It seems this is not needed anymore, after separating the GUI thread
// (at the same time, this instruction is causing crashes in specific situations,
// because internally "reportDynamicWorldConfiguration" is not updating the world beforehand,
// so if a linked dummy was destroyed, the linked object is not valid anymore, but will be touched anyway
// in "reportDynamicWorldConfiguration", because the next world update will only happen when simHandleDynamics is called)
//		App::ct->dynamicsContainer->reportDynamicWorldConfiguration(); // 2010/02/08 unshiftable shapes might have been shifted with the green toolbar buttons!


		//***********************************************************************
		if (CThreadPool::getEmergencyStopForCurrentInstance())
		{
			CThreadPool::handleAllThreads_withResumeLocation(-1); // Added on 24/02/2011 (important, otherwise threads might never resume!!!)
			scriptInExecution--;
			return(retVal); // this is important in the case where we pressed the emergency stop (the stop button in the middle of the screen)
		}
		globalMainScriptInAndOutCount=!globalMainScriptInAndOutCount;
		App::ct->luaScriptContainer->setMainScriptLaunched(true);
	}
	else
	{
		if (threadedExecutionUnderWay)
		{
			scriptInExecution--;
			return(retVal); // this script is being executed by another thread!
		}
	}
	
	if (_scriptTextExec==NULL)
	{
		fromFileToBuffer();
		if ( (_scriptTextLength!=0)&&(_scriptText!=NULL) )
		{ // we copy the script buffer (so that if we modify the script during a simulation, the script behaviour stays same!)
			_scriptTextExec=new char[_scriptTextLength];
			_scriptTextExecLength=_scriptTextLength;
			for (int i=0;i<_scriptTextLength;i++)
				_scriptTextExec[i]=_scriptText[i];
		}
	}
	if ( (_scriptTextExecLength==0)||(_scriptTextExec==NULL) )
	{
		if (getScriptType()==sim_scripttype_mainscript)
		{
			globalMainScriptInAndOutCount=!globalMainScriptInAndOutCount;
			globalMainScriptExitTime=VDateTime::getTimeInMs();
			App::ct->luaScriptContainer->setMainScriptLaunched(false);
		}
		scriptInExecution--;
		return(retVal);
	}

	if (threadedExecution)
	{
		if (_executeJustOnce&&_executedOnceAtLeast)
		{
			scriptInExecution--;
			return(0);
		}
		_executedOnceAtLeast=true;

		if (CThreadPool::getStopForCurrentInstanceRequested())// will also return true in case of emergency stop request
		{
			scriptInExecution--;
			return(retVal);
		}
		threadedExecutionUnderWay=true;
		_globalMutex.lock();
		toBeCalledByThread.push_back(this);
		inputToTransmitToThreadedScript.push_back(inputArguments.copyYourself());
		_globalMutex.unlock();
		VTHREAD_ID_TYPE associatedFiberOrThreadID=CThreadPool::createNewThread(startAddressForThreadedScripts);
		CThreadPool::switchToThread(associatedFiberOrThreadID);

		scriptInExecution--;
		return(retVal);
	}
	else
	{
		if (_executeJustOnce&&_executedOnceAtLeast)
		{
			scriptInExecution--;
			return(0);
		}
		_executedOnceAtLeast=true;

		int retVal=runOneStepNow(inputArguments,outputArguments);
		scriptInExecution--;

		if (getScriptType()==sim_scripttype_mainscript)
		{
			App::ct->infoDisplay->runningThreadedScriptStart(0);
			CThreadPool::handleAllThreads_withResumeLocation(-1); // Will handle all unhandled threads
			App::ct->infoDisplay->runningThreadedScriptEnd();
			float dt=App::ct->simulation->getSimulationTimeStep_speedModified();
			for (int i=0;i<int(App::ct->objCont->jointList.size());i++)
				App::ct->objCont->getJoint(App::ct->objCont->jointList[i])->measureJointVelocity(dt);
			for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
				App::ct->objCont->getObject(App::ct->objCont->objectList[i])->measureVelocity(dt);
		}

		if ((getScriptType()==sim_scripttype_mainscript)&&App::ct->simulation->getPauseOnErrorRequested())
			App::ct->simulation->pauseSimulation();
		return(retVal);
	}
}

VTHREAD_RETURN_TYPE CLuaScriptObject::startAddressForThreadedScripts(VTHREAD_ARGUMENT_TYPE lpData)
{
	_globalMutex.lock();
	CLuaScriptObject* it=toBeCalledByThread[0];
	toBeCalledByThread.erase(toBeCalledByThread.begin());
	CLuaObjectGroup* ic=inputToTransmitToThreadedScript[0]; 
	inputToTransmitToThreadedScript.erase(inputToTransmitToThreadedScript.begin());
	_globalMutex.unlock();
	CLuaObjectGroup b;

	it->runOneStepNow(*ic,b);
	delete ic; // strange : another thread than the one that created it destroys it, no?
	it->reset(true); // added on 2010/03/05 because of some crashes with threaded scripts (Lua said: resource destroyed by other thread than the one that created it!)
	it->threadedExecutionUnderWay=false;
	return(0);
}

// Following only for main or child scripts!!
int CLuaScriptObject::runOneStepNow(CLuaObjectGroup& inputArguments,CLuaObjectGroup& outputArguments)
{
	int retVal=sim_script_no_error;
	App::ct->infoDisplay->scriptStart(!VThread::isCurrentThreadTheMainThread(),getScriptType()==sim_scripttype_mainscript);
	if (L==NULL)
	{
// Keep following, useful for debugging! (the same thread/fibre that creates L has to destroy it later!!! (says Lua, otherwise crash!)
//		printf("Lua open, thread ID: %i, objectID: %i\n",VThread::getCurrentThreadId(),scriptID);
        L=initializeNewLuaState(getScriptSuffixNumberString().c_str());
    }
	int oldTop=lua_gettop(L);	// We store lua's stack

	std::string tmp("sim_current_script_id=");
	tmp+=boost::lexical_cast<std::string>(getScriptID());
	luaL_dostring(L,tmp.c_str());

	App::ct->luaScriptContainer->setLuaCallError("",SIM_LUA_CALL_NO_ERROR);
	App::ct->luaScriptContainer->clearErrorFlag();

	int loadBufferRes=luaL_loadbuffer(L,_scriptTextExec,_scriptTextExecLength-1,getShortDescriptiveName().c_str());
	if (loadBufferRes==0)
	{
		inputArguments.pushObjectsOnStack(L);
		if (lua_pcall(L,inputArguments.getObjectNumber(),LUA_MULTRET,0)!=0)
		{ // a runtime error occurred!
			// We have to exit the thread free mode if we are still in there (the instance should automatically be restored when this thread resumes):
			if (CThreadPool::isThreadInFreeMode())
				CThreadPool::setThreadFreeMode(false);

			std::string errMsg;
			if (lua_isstring(L,-1))
				errMsg=std::string(lua_tostring(L,-1));
			else
				errMsg="(error unknown)";
			lua_pop(L,-1); // pop error from stack

			App::ct->luaScriptContainer->setLuaError("",errMsg.c_str(),1);
			retVal|=sim_script_lua_error;
			if (App::ct->luaScriptContainer->isErrorFlagSet())
				retVal|=sim_script_call_error;
			// Here we can decide what happens uppon error:
			App::ct->simulation->pauseOnErrorRequested();
		}
		else
		{
			if (CThreadPool::isThreadInFreeMode())
				CThreadPool::setThreadFreeMode(false);
			if (App::ct->luaScriptContainer->isErrorFlagSet())
				retVal|=sim_script_call_error;
			outputArguments.buildObjectsFromStack(L,1);
		}
	}
	else
	{ // A compilation error occurred!
		std::string errMsg;
		if (lua_isstring(L,-1))
			errMsg=std::string(lua_tostring(L,-1));
		else
			errMsg="(error unknown)";
		App::ct->luaScriptContainer->setLuaError("",errMsg.c_str(),0);
		lua_pop(L,-1); // pop error from stack
		retVal|=sim_script_lua_error;
		if (App::ct->luaScriptContainer->isErrorFlagSet())
			retVal|=sim_script_call_error;
		// Here we can decide what happens uppon error:
		App::ct->simulation->pauseOnErrorRequested();
	}
	numberOfPasses++;

	lua_settop(L,oldTop);		// We restore lua's stack
	App::ct->infoDisplay->scriptEnd(!VThread::isCurrentThreadTheMainThread(),getScriptType()==sim_scripttype_mainscript);
	if (getScriptType()==sim_scripttype_mainscript)
	{
		// Here we have to prepare the error message for the info box:
		std::string tmpTxt;
		if (retVal&(sim_script_lua_error|sim_script_call_error))
			tmpTxt="&&fg930(script error)";
		App::ct->infoDisplay->setMainScriptMessage(tmpTxt.c_str());

		globalMainScriptInAndOutCount=!globalMainScriptInAndOutCount;
		globalMainScriptExitTime=VDateTime::getTimeInMs();
		App::ct->luaScriptContainer->setMainScriptLaunched(false); // important for the threaded execution! REALLY?!!
	}
	return(retVal);
}


void CLuaScriptObject::runAddOn()
{
	if (_addOn_executionState==3)
		return;
	int runMode=0; //0=starting, 1=running, 2=pausing, 3=restarting
	if (_addOn_desiredExecutionState==1)
	{ // we wanna run it
		if (_addOn_executionState==0)
			runMode=0;
		if (_addOn_executionState==1)
			runMode=1;
		if (_addOn_executionState==2)
			runMode=3;
		_addOn_executionState=1;
	}
	else
	{ // we wanna pause it
		if (_addOn_executionState==0)
			return;
		if (_addOn_executionState==1)
			runMode=2;
		if (_addOn_executionState==2)
			return;
		_addOn_executionState=2;
	}

	if (_scriptTextExec==NULL)
	{
		if ( (_scriptTextLength!=0)&&(_scriptText!=NULL) )
		{ // we copy the script buffer
			_scriptTextExec=new char[_scriptTextLength];
			_scriptTextExecLength=_scriptTextLength;
			for (int i=0;i<_scriptTextLength;i++)
				_scriptTextExec[i]=_scriptText[i];
		}
	}
	if ( (_scriptTextExecLength==0)||(_scriptTextExec==NULL) )
	{
		_addOn_executionState=3; // error
		return;
	}

	if (L==NULL)
        L=initializeNewLuaState(""); // only child scripts can have a suffix number
	int oldTop=lua_gettop(L);	// We store lua's stack

	std::string tmp("sim_current_script_id=");
	tmp+=boost::lexical_cast<std::string>(getScriptID());
	luaL_dostring(L,tmp.c_str());

	App::ct->luaScriptContainer->setLuaCallError("",SIM_LUA_CALL_NO_ERROR);
	App::ct->luaScriptContainer->clearErrorFlag();


	App::ct->luaScriptContainer->setInAddOnNow(true);
	int loadBufferRes=luaL_loadbuffer(L,_scriptTextExec,_scriptTextExecLength-1,getShortDescriptiveName().c_str());
	if (loadBufferRes==0)
	{
		lua_pushnumber(L,runMode);
		if (lua_pcall(L,1,LUA_MULTRET,0)!=0)
		{ // a runtime error occurred!
			std::string errMsg;
			if (lua_isstring(L,-1))
				errMsg=std::string(lua_tostring(L,-1));
			else
				errMsg="(error unknown)";
			lua_pop(L,-1); // pop error from stack

			App::ct->luaScriptContainer->setLuaError("",errMsg.c_str(),1);
			_addOn_executionState=3; // error
		}
	}
	else
	{ // A compilation error occurred!
		std::string errMsg;
		if (lua_isstring(L,-1))
			errMsg=std::string(lua_tostring(L,-1));
		else
			errMsg="(error unknown)";
		App::ct->luaScriptContainer->setLuaError("",errMsg.c_str(),0);
		lua_pop(L,-1); // pop error from stack
		_addOn_executionState=3; // error
	}
	App::ct->luaScriptContainer->setInAddOnNow(false);
	numberOfPasses++;
	lua_settop(L,oldTop);		// We restore lua's stack
}

void CLuaScriptObject::runJointCtrlCallback(const std::vector<bool>& inDataBool,const std::vector<int>& inDataInt,const std::vector<float>& inDataFloat,std::vector<float>& outDataFloat)
{ // args are not very generic, but for now that works fine (this routine is only used by joints with custom control loop)
	outDataFloat.clear();
	if (_scriptTextExec==NULL)
	{
		if ( (_scriptTextLength!=0)&&(_scriptText!=NULL) )
		{ // we copy the script buffer
			_scriptTextExec=new char[_scriptTextLength];
			_scriptTextExecLength=_scriptTextLength;
			for (int i=0;i<_scriptTextLength;i++)
				_scriptTextExec[i]=_scriptText[i];
		}
	}
	if ( (_scriptTextExecLength==0)||(_scriptTextExec==NULL) )
	{
		return; // error
	}

	if (L==NULL)
        L=initializeNewLuaState("");

	int oldTop=lua_gettop(L);	// We store lua's stack

	std::string tmp("sim_current_script_id=");
	tmp+=boost::lexical_cast<std::string>(getScriptID());
	luaL_dostring(L,tmp.c_str());

	App::ct->luaScriptContainer->setLuaCallError("",SIM_LUA_CALL_NO_ERROR);
	App::ct->luaScriptContainer->clearErrorFlag();

	App::ct->luaScriptContainer->setInLuaCallbackNow(true);
	int loadBufferRes=luaL_loadbuffer(L,_scriptTextExec,_scriptTextExecLength-1,getShortDescriptiveName().c_str());
	if (loadBufferRes==0)
	{
		for (int i=0;i<int(inDataBool.size());i++)
			lua_pushboolean(L,inDataBool[i]);
		for (int i=0;i<int(inDataInt.size());i++)
			lua_pushnumber(L,inDataInt[i]);
		for (int i=0;i<int(inDataFloat.size());i++)
			lua_pushnumber(L,inDataFloat[i]);
		if (lua_pcall(L,int(inDataBool.size())+int(inDataInt.size())+int(inDataFloat.size()),LUA_MULTRET,0)!=0)
		{ // a runtime error occurred!
			std::string errMsg;
			if (lua_isstring(L,-1))
				errMsg=std::string(lua_tostring(L,-1));
			else
				errMsg="(error unknown)";
			lua_pop(L,-1); // pop error from stack

			App::ct->luaScriptContainer->setLuaError("",errMsg.c_str(),1);
		}
		else
		{ // return values:
			int numberOfArgs=lua_gettop(L);
			for (int i=0;i<numberOfArgs;i++)
			{
				if (lua_isnumber(L,i+1))
					outDataFloat.push_back(float(lua_tonumber(L,i+1)));
			}
		}
	}
	else
	{ // A compilation error occurred!
		std::string errMsg;
		if (lua_isstring(L,-1))
			errMsg=std::string(lua_tostring(L,-1));
		else
			errMsg="(error unknown)";
		App::ct->luaScriptContainer->setLuaError("",errMsg.c_str(),0);
		lua_pop(L,-1); // pop error from stack
	}
	App::ct->luaScriptContainer->setInLuaCallbackNow(false);
	numberOfPasses++;
	lua_settop(L,oldTop);		// We restore lua's stack
}

int CLuaScriptObject::runContactCallback(const int inDataInt[3],int outDataInt[3],float outDataFloat[14])
{ // args are not very generic, but for now that works fine (this routine is only used by joints with custom control loop)
	int retVal=0;
	if (_scriptTextExec==NULL)
	{
		if ( (_scriptTextLength!=0)&&(_scriptText!=NULL) )
		{ // we copy the script buffer
			_scriptTextExec=new char[_scriptTextLength];
			_scriptTextExecLength=_scriptTextLength;
			for (int i=0;i<_scriptTextLength;i++)
				_scriptTextExec[i]=_scriptText[i];
		}
	}
	if ( (_scriptTextExecLength==0)||(_scriptTextExec==NULL) )
		return(retVal); // error

	if (L==NULL)
        L=initializeNewLuaState("");

	int oldTop=lua_gettop(L);	// We store lua's stack

	std::string tmp("sim_current_script_id=");
	tmp+=boost::lexical_cast<std::string>(getScriptID());
	luaL_dostring(L,tmp.c_str());

	App::ct->luaScriptContainer->setLuaCallError("",SIM_LUA_CALL_NO_ERROR);
	App::ct->luaScriptContainer->clearErrorFlag();

	App::ct->luaScriptContainer->setInLuaCallbackNow(true);
	int loadBufferRes=luaL_loadbuffer(L,_scriptTextExec,_scriptTextExecLength-1,getShortDescriptiveName().c_str());
	if (loadBufferRes==0)
	{
		for (int i=0;i<3;i++)
			lua_pushnumber(L,inDataInt[i]);
		if (lua_pcall(L,3,LUA_MULTRET,0)!=0)
		{ // a runtime error occurred!
			std::string errMsg;
			if (lua_isstring(L,-1))
				errMsg=std::string(lua_tostring(L,-1));
			else
				errMsg="(error unknown)";
			lua_pop(L,-1); // pop error from stack

			App::ct->luaScriptContainer->setLuaError("",errMsg.c_str(),1);
		}
		else
		{ // return values:
			int numberOfArgs=lua_gettop(L);
			if (numberOfArgs>=1)
			{
				retVal=luaToInt(L,1);
				if (retVal>0)
				{
					if ((numberOfArgs>=3)&&lua_istable(L,2)&&lua_istable(L,3))
					{ // we expect 2 tables!
						getIntsFromTable(L,2,3,outDataInt);
						getFloatsFromTable(L,3,14,outDataFloat);
					}
					else
						retVal=0; // error
				}
			}
		}
	}
	else
	{ // A compilation error occurred!
		std::string errMsg;
		if (lua_isstring(L,-1))
			errMsg=std::string(lua_tostring(L,-1));
		else
			errMsg="(error unknown)";
		App::ct->luaScriptContainer->setLuaError("",errMsg.c_str(),0);
		lua_pop(L,-1); // pop error from stack
	}
	App::ct->luaScriptContainer->setInLuaCallbackNow(false);
	numberOfPasses++;
	lua_settop(L,oldTop);		// We restore lua's stack
	return(retVal);
}

void CLuaScriptObject::reset(bool dontResetInitialParams)
{ // dontResetInitialParams is false by default
	if (L!=NULL)
	{
// Keep following, useful for debugging! (the same thread/fibre that created L has to destroy it!!!) (says Lua, otherwise crash!)
//		printf("Lua close, thread ID: %i, objectID: %i\n",VThread::getCurrentThreadId(),scriptID);
		lua_close(L);
	}
	L=NULL;
	numberOfPasses=0;
	delete[] _scriptTextExec;
	_scriptTextExec=NULL;
	_scriptTextExecLength=0;
	clearAllUserData();
	if (!dontResetInitialParams)
		scriptParameters->clearInitialParameters();
}

int CLuaScriptObject::setUserData(char* data)
{
	_userData.push_back(data);
	int id=0;
	for (int i=0;i<int(_userDataIds.size());i++)
	{
		if (_userDataIds[i]==id)
		{
			id++;
			i=-1; // we need to restart the search from the beginning!
		}
	}
	_userDataIds.push_back(id);
	return(id);
}

char* CLuaScriptObject::getUserData(int id)
{
	for (int i=0;i<int(_userDataIds.size());i++)
	{
		if (_userDataIds[i]==id)
			return(_userData[i]);
	}
	return(NULL);
}

void CLuaScriptObject::releaseUserData(int id)
{
	for (int i=0;i<int(_userDataIds.size());i++)
	{
		if (_userDataIds[i]==id)
		{
			delete[] _userData[i];
			_userData.erase(_userData.begin()+i);
			_userDataIds.erase(_userDataIds.begin()+i);
			break;
		}
	}
}

void CLuaScriptObject::clearAllUserData()
{
	for (int i=0;i<int(_userData.size());i++)
		delete[] _userData[i];
	_userData.clear();
	_userDataIds.clear();
}

CLuaScriptObject* CLuaScriptObject::copyYourself()
{
	CLuaScriptObject* it=new CLuaScriptObject((_scriptType==sim_scripttype_mainscript)||(_scriptType==sim_scripttype_childscript));
	it->_scriptType=_scriptType;
	it->scriptID=scriptID;
	it->objectIDAttachedTo=objectIDAttachedTo;
	it->_objectIDAttachedTo_callback=_objectIDAttachedTo_callback;
	it->threadedExecution=threadedExecution;
	it->_explicitHandling=_explicitHandling;
	it->_executionOrder=_executionOrder;
	it->setScriptText(getScriptText());
//	// DONT COPY THE _adjustNameSuffix-THING!!!!
	delete it->scriptParameters;
	it->scriptParameters=scriptParameters->copyYourself();
	it->_executeJustOnce=_executeJustOnce;
	it->_executeInSensingPhase=_executeInSensingPhase;

	it->_initialValuesInitialized=_initialValuesInitialized;
	it->_initialExplicitHandling=_initialExplicitHandling;
	return(it);
}

CLuaScriptParameters* CLuaScriptObject::getScriptParametersObject()
{
	return(scriptParameters);
}

bool CLuaScriptObject::addCommandToOutsideCommandQueue(int commandID,int auxVal1,int auxVal2,int auxVal3,int auxVal4,const float aux2Vals[8],int aux2Count)
{
	if (_outsideCommandQueue!=NULL)
		return(_outsideCommandQueue->addCommand(commandID,auxVal1,auxVal2,auxVal3,auxVal4,aux2Vals,aux2Count));
	return(true);
}

int CLuaScriptObject::extractCommandFromOutsideCommandQueue(int auxVals[4],float aux2Vals[8],int& aux2Count)
{
	if (_outsideCommandQueue!=NULL)
		return(_outsideCommandQueue->extractOneCommand(auxVals,aux2Vals,aux2Count));
	return(-1);
}

void CLuaScriptObject::serialize(CSer& ar)
{
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Si2");
		ar << scriptID << objectIDAttachedTo << _scriptType;
		ar.flush();

		// Keep following close to the beginning!
		ar.storeDataName("Va2");
		BYTE nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,threadedExecution);
		SIM_SET_CLEAR_BIT(nothing,1,_explicitHandling);
//		FREE
		SIM_SET_CLEAR_BIT(nothing,3,!_mainScriptIsDefaultMainScript);
		SIM_SET_CLEAR_BIT(nothing,4,_executeJustOnce);
		SIM_SET_CLEAR_BIT(nothing,5,_executeInSensingPhase);
		ar << nothing;
		ar.flush();

		ar.storeDataName("Seo");
		ar << _executionOrder;
		ar.flush();

		// We store scripts in a light encoded way:
		ar.storeDataName("Ste");
		tt::lightEncodeBuffer(_scriptText,_scriptTextLength);
		for (int i=0;i<_scriptTextLength;i++)
			ar << _scriptText[i];
		tt::lightDecodeBuffer(_scriptText,_scriptTextLength); // Important to decode it afterward serialization!
		ar.flush();


		ar.storeDataName("Prm");
		ar.setCountingMode();
		scriptParameters->serialize(ar);
		if (ar.setWritingMode())
			scriptParameters->serialize(ar);

		ar.storeDataName("Coi");
		ar << _objectIDAttachedTo_callback;
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
				bool justLoadedCustomScriptBuffer=false;
				if (theName.compare("Si2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> scriptID >> objectIDAttachedTo >> _scriptType;
				}
				if (theName.compare("Seo")==0)
				{ 
					noHit=false;
					ar >> byteQuantity;
					ar >> _executionOrder;
				}
				if (theName.compare("Ste")==0)
				{ // The script is stored encoded!
					noHit=false;
					ar >> byteQuantity;
					_scriptTextLength=byteQuantity;
					if (_scriptText!=NULL)
						delete[] _scriptText;
					_scriptText=NULL;
					if (_scriptTextLength!=0)
					{
						_scriptText=new char[_scriptTextLength];
						char dummy;
						for (int i=0;i<_scriptTextLength;i++)
						{
							ar >> dummy;
							_scriptText[i]=dummy;
						}
						tt::lightDecodeBuffer(_scriptText,_scriptTextLength);
					}
					justLoadedCustomScriptBuffer=true;
				}
				if (justLoadedCustomScriptBuffer)
				{ // We just loaded the script text.
					if (_scriptType==sim_scripttype_mainscript)
					{ // We just loaded a main script text. Do we want to load the default main script instead?
						if (_mainScriptIsDefaultMainScript)
						{ // Yes!
							std::string filenameAndPath(App::directories->systemDirectory+VREP_SLASH);
							filenameAndPath+=DEFAULT_MAINSCRIPT_NAME;
							if (VFile::doesFileExist(filenameAndPath))
							{
								try
								{
									VFile file(filenameAndPath.c_str(),VFile::READ|VFile::SHARE_DENY_NONE);
									VArchive archive2(&file,VArchive::LOAD);
									DWORD archiveLength=DWORD(file.getLength());
									// We replace current script with a default main script
									_scriptTextLength=archiveLength+1;
									if (_scriptText!=NULL)
										delete[] _scriptText;
									_scriptText=NULL;
									if (_scriptTextLength!=0)
									{
										_scriptText=new char[_scriptTextLength];
										char dummy;
										for (int i=0;i<_scriptTextLength-1;i++)
										{
											archive2 >> dummy;
											_scriptText[i]=dummy;
										}
									}
									_scriptText[_scriptTextLength-1]=0;
									archive2.close();
									file.close();
								}
								catch(VFILE_EXCEPTION_TYPE e)
								{
									VFile::reportAndHandleFileExceptionError(e);
// Removed following line on 2010/03/03: even if the default main script is not there, we might still want the default main script next time (if there maybe).
// Following line also causes problems when converting to a new fileformat!
//								_mainScriptIsDefaultMainScript=false; // We couldn't find the default main script, we turn this one into a customized main script!
								}
							}
						}
					}
					else
					{ // Here we can handle Lua compatibility issues.
					}
					fromBufferToFile();
				}
				if (theName=="Va2")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					threadedExecution=SIM_IS_BIT_SET(nothing,0);
					_explicitHandling=SIM_IS_BIT_SET(nothing,1);
//					FREE
					_mainScriptIsDefaultMainScript=!SIM_IS_BIT_SET(nothing,3);
					_executeJustOnce=SIM_IS_BIT_SET(nothing,4);
					_executeInSensingPhase=SIM_IS_BIT_SET(nothing,5);
				}
				if (theName.compare("Prm")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					delete scriptParameters;
					scriptParameters=new CLuaScriptParameters();
					scriptParameters->serialize(ar);
					if (_scriptType==sim_scripttype_mainscript)
					{ // We just loaded a main script. Do we want to load the default main script parameters instead?
						if (_mainScriptIsDefaultMainScript)
						{ // Yes!
							// For now we just clear all parameters! (in future we might load default  parameters)
							delete scriptParameters;
							scriptParameters=new CLuaScriptParameters();
						}
					}
				}

				if (theName.compare("Coi")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _objectIDAttachedTo_callback;
				}

				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}


bool CLuaScriptObject::globalMainScriptInAndOutCount=false;
int CLuaScriptObject::globalMainScriptExitTime=0;
bool CLuaScriptObject::sensingScriptsInExecution=false;

VMutex CLuaScriptObject::_globalMutex;

std::vector<CLuaScriptObject*> CLuaScriptObject::toBeCalledByThread;
std::vector<CLuaObjectGroup*> CLuaScriptObject::inputToTransmitToThreadedScript;
