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
#include "LuaScriptContainer.h"
#include "Tt.h"
#include "VVarious.h"
#include "IloIlo.h"
#include "ThreadPool.h"
#include "VFileFinder.h"
#include "v_repStrings.h"
#include "App.h"

std::string CLuaScriptContainer::_additionalAddOnScriptFirstScene;
std::string CLuaScriptContainer::_additionalAddOnScriptAllScenes;

CLuaScriptContainer::CLuaScriptContainer()
{
	lastExecutedMainScriptReturnValues=NULL;
	mainScriptInputValues=NULL;
	removeLastExecutedMainScriptReturnValues();
	removeMainScriptInputValues();
	mainScriptLaunched=false;
	_inAddOnNow=false;
	_inCallbackNow=false;
	insertDefaultScript_mainAndChildScriptsOnly(sim_scripttype_mainscript,false);
	insertAddOnScripts();
	prepareAddOnFunctionNames();
}

CLuaScriptContainer::~CLuaScriptContainer()
{
	removeAllScripts(false);
	removeLastExecutedMainScriptReturnValues();
	removeMainScriptInputValues();
	for (int i=0;i<int(_callbackStructureToDestroyAtEndOfSimulation.size());i++)
		delete _callbackStructureToDestroyAtEndOfSimulation[i];
	_callbackStructureToDestroyAtEndOfSimulation.clear();
	broadcastDataContainer.removeAllObjects();
}

void CLuaScriptContainer::simulationAboutToStart()
{
	broadcastDataContainer.simulationAboutToStart();
	for (int i=0;i<int(allScripts.size());i++)
//	{
//		if ((allScripts[i]->getScriptType()==sim_scripttype_mainscript)||(allScripts[i]->getScriptType()==sim_scripttype_childscript)||(allScripts[i]->getScriptType()==sim_scripttype_jointctrlcallback))
			allScripts[i]->simulationAboutToStart();
//	}
}

void CLuaScriptContainer::simulationEnded()
{
	for (int i=0;i<int(allScripts.size());i++)
//	{
//		if ((allScripts[i]->getScriptType()==sim_scripttype_mainscript)||(allScripts[i]->getScriptType()==sim_scripttype_childscript)||(allScripts[i]->getScriptType()==sim_scripttype_jointctrlcallback))
			allScripts[i]->simulationEnded();
//	}
	broadcastDataContainer.simulationEnded();
	_removeDestroyedScripts();
	for (int i=0;i<int(_callbackStructureToDestroyAtEndOfSimulation.size());i++)
		delete _callbackStructureToDestroyAtEndOfSimulation[i];
	_callbackStructureToDestroyAtEndOfSimulation.clear();
//	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
//	{
//	}
}

void CLuaScriptContainer::setAdditionalAddOnScript(std::string scriptNameWithExtension,bool firstSceneOnly)
{
	if (firstSceneOnly)
		_additionalAddOnScriptFirstScene=scriptNameWithExtension;
	else
		_additionalAddOnScriptAllScenes=scriptNameWithExtension;
}

void CLuaScriptContainer::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

void CLuaScriptContainer::addCallbackStructureObjectToDestroyAtTheEndOfSimulation(SLuaCallBack* object)
{
	_callbackStructureToDestroyAtEndOfSimulation.push_back(object);
}

void CLuaScriptContainer::removeLastExecutedMainScriptReturnValues()
{
	delete lastExecutedMainScriptReturnValues;
	lastExecutedMainScriptReturnValues=NULL;
}

void CLuaScriptContainer::_removeDestroyedScripts()
{
	for (int i=0;i<int(allScripts.size());i++)
	{
		if (allScripts[i]->getDestroyAtEndOfSimulation())
		{
			delete allScripts[i];
			allScripts.erase(allScripts.begin()+i);
			i--;
		}
	}
}

void CLuaScriptContainer::removeMainScriptInputValues()
{
	delete mainScriptInputValues;
	mainScriptInputValues=NULL;
}

void CLuaScriptContainer::setLuaCallError(const char* functionName,const char* errMsg)
{
	setLuaError(functionName,errMsg,2);
}

void CLuaScriptContainer::setLuaCallWarning(const char* functionName,const char* errMsg)
{
	setLuaWarning(functionName,errMsg);
}

void CLuaScriptContainer::setLuaError(const char* functionName,const char* errMsg,int errorType)
{
	if (std::string(errMsg).compare(SIM_LUA_CALL_NO_ERROR)!=0)
	{
		errorFlag=true;
		std::string message("Lua unknown error: ");
		if (errorType==0)
			message="Lua compilation error: ";
		if (errorType==1)
			message="Lua runtime error: ";
		if (errorType==2)
			message="Lua call error: ";
		if (strlen(functionName)!=0)
		{
			message+=errMsg;
			message+=tt::decorateString(" (",functionName,")");
			App::uiThread->addStatusLine(message.c_str());
		}
		else
		{
			message+=errMsg;
			App::uiThread->addStatusLine(message.c_str());
		}
	}
}

void CLuaScriptContainer::setLuaWarning(const char* functionName,const char* errMsg)
{
	std::string message("Lua call warning: ");
	if (strlen(functionName)!=0)
	{
		message+=errMsg;
		message+=tt::decorateString(" (",functionName,")");
		App::uiThread->addStatusLine(message.c_str());
	}
	else
	{
		message+=errMsg;
		App::uiThread->addStatusLine(message.c_str());
	}
}

void CLuaScriptContainer::clearErrorFlag()
{
	errorFlag=false;
}
bool CLuaScriptContainer::isErrorFlagSet()
{
	return(errorFlag);
}

void CLuaScriptContainer::setMainScriptLaunched(bool launched)
{
	mainScriptLaunched=launched;
}

bool CLuaScriptContainer::getMainScriptLaunched()
{
	return(mainScriptLaunched);
}

void CLuaScriptContainer::setInAddOnNow(bool inAddOn)
{
	_inAddOnNow=inAddOn;
}

void CLuaScriptContainer::setInLuaCallbackNow(bool inCallback)
{
	_inCallbackNow=inCallback;
}

bool CLuaScriptContainer::getInAddOnNow()
{
	return(_inAddOnNow);
}

bool CLuaScriptContainer::getInLuaCallbackNow()
{
	return(_inCallbackNow);
}

void CLuaScriptContainer::removeAllScripts(bool keepAddOnScripts)
{
	if (!keepAddOnScripts)
	{
		for (int i=0;i<int(allScripts.size());i++)
			delete allScripts[i];
		allScripts.clear();
	}
	else
	{
		for (int i=0;i<int(allScripts.size());i++)
		{
			CLuaScriptObject* it=allScripts[i];
			if ( (it->getScriptType()!=sim_scripttype_addonscript)&&(it->getScriptType()!=sim_scripttype_addonfunction) )
			{
				removeScript(it->getScriptID());
				i=-1; // ordering might have changed
			}
		}
	}
}

void CLuaScriptContainer::resetAllSimulationScripts()
{
	for (int i=0;i<int(allScripts.size());i++)
	{
		if ((allScripts[i]->getScriptType()==sim_scripttype_mainscript)||(allScripts[i]->getScriptType()==sim_scripttype_childscript)||(allScripts[i]->getScriptType()==sim_scripttype_jointctrlcallback)||(allScripts[i]->getScriptType()==sim_scripttype_contactcallback))
			allScripts[i]->reset();
	}
}

void CLuaScriptContainer::announceObjectWillBeErased(int objectID)
{ // Never called from copy buffer!
	int i=0;
	while (i<int(allScripts.size()))
	{
		if (allScripts[i]->announce3DObjectWillBeErased(objectID,false))
		{
			if (removeScript(allScripts[i]->getScriptID()))
				i=0; // ordering may have changed
			else
				i++;
		}
		else
			i++;
	}
}

bool CLuaScriptContainer::removeScript(int scriptID)
{
	for (int i=0;i<int(allScripts.size());i++)
	{
		if (allScripts[i]->getScriptID()==scriptID)
		{
			delete allScripts[i];
			allScripts.erase(allScripts.begin()+i);
			break;
		}
	}
	App::ct->objCont->setFullDialogRefreshFlag();
	return(true);
}

CLuaScriptObject* CLuaScriptContainer::getScriptFromID(int scriptID)
{
	for (int i=0;i<int(allScripts.size());i++)
	{
		if (allScripts[i]->getScriptID()==scriptID)
			return(allScripts[i]);
	}
	return(NULL);
}

CLuaScriptObject* CLuaScriptContainer::getScriptFromObjectAttachedTo(int threeDObjectID)
{ // used for child scripts
	for (int i=0;i<int(allScripts.size());i++)
	{
		if (allScripts[i]->getObjectIDThatScriptIsAttachedTo()==threeDObjectID)
			return(allScripts[i]);
	}
	return(NULL);
}

CLuaScriptObject* CLuaScriptContainer::getScriptFromObjectAttachedTo_callback(int threeDObjectID)
{ // used for callback scripts
	for (int i=0;i<int(allScripts.size());i++)
	{
		if (allScripts[i]->getObjectIDThatScriptIsAttachedTo_callback()==threeDObjectID)
			return(allScripts[i]);
	}
	return(NULL);
}

CLuaScriptObject* CLuaScriptContainer::getCustomContactHandlingScript_callback()
{
	for (int i=0;i<int(allScripts.size());i++)
	{
		if (allScripts[i]->getScriptType()==sim_scripttype_contactcallback)
			return(allScripts[i]);
	}
	return(NULL);
}

CLuaScriptObject* CLuaScriptContainer::getScriptFromPseudoName(std::string scriptPseudoName)
{
	if (scriptPseudoName=="")
		return(getMainScript());
	for (int i=0;i<int(allScripts.size());i++)
	{
		if (allScripts[i]->getScriptPseudoName()==scriptPseudoName)
			return(allScripts[i]);
	}
	return(NULL);
}

CLuaScriptObject* CLuaScriptContainer::getMainScript()
{
	for (int i=0;i<int(allScripts.size());i++)
	{
		if (allScripts[i]->getScriptType()==sim_scripttype_mainscript)
			return(allScripts[i]);
	}
	return(NULL);
}

int CLuaScriptContainer::insertScript(CLuaScriptObject* script)
{
	// We make sure the id is unique:
	int newID=SIM_IDSTART_LUASCRIPT;
	while (getScriptFromID(newID)!=NULL)
		newID++;
	script->setScriptID(newID);
	allScripts.push_back(script);

	return(newID);
}

int CLuaScriptContainer::insertDefaultScript_mainAndChildScriptsOnly(int scriptType,bool threaded)
{ 
	if (scriptType!=sim_scripttype_childscript)
		threaded=false; // just to make sure
	int retVal=-1;
	std::string filenameAndPath(App::directories->systemDirectory+VREP_SLASH);

	if (scriptType==sim_scripttype_mainscript)
		filenameAndPath+=DEFAULT_MAINSCRIPT_NAME;
	if (scriptType==sim_scripttype_childscript)
	{
		if (threaded)
			filenameAndPath+=DEFAULT_THREADEDCHILDSCRIPT_NAME;
		else
			filenameAndPath+=DEFAULT_NONTHREADEDCHILDSCRIPT_NAME;
	}

	if (VFile::doesFileExist(filenameAndPath))
	{
		try
		{
			VFile file(filenameAndPath,VFile::READ|VFile::SHARE_DENY_NONE);
			VArchive archive(&file,VArchive::LOAD);
			DWORD archiveLength=DWORD(file.getLength());
			char* defaultScript=new char[archiveLength+1];
			for (int i=0;i<int(archiveLength);i++)
				archive >> defaultScript[i];
			defaultScript[archiveLength]=0;
			CLuaScriptObject* defScript=new CLuaScriptObject(true);
			retVal=insertScript(defScript);
			defScript->setScriptType(scriptType);
			defScript->setScriptText(defaultScript);
			defScript->setThreadedExecution(threaded);
			if (threaded)
				defScript->setExecuteJustOnce(true);
			delete[] defaultScript;		
			archive.close();
			file.close();
		}
		catch(VFILE_EXCEPTION_TYPE e)
		{
			VFile::reportAndHandleFileExceptionError(e);
			char defaultMessage[]="Default script file could not be found!"; // do not use comments ("--"), we want to cause an execution error!
			CLuaScriptObject* defScript=new CLuaScriptObject(true);
			retVal=insertScript(defScript);
			defScript->setScriptType(scriptType);
			defScript->setScriptText(defaultMessage);
			defScript->setThreadedExecution(threaded);
		}
	}
	else
	{
		char defaultMessage[]="Default script file could not be found!"; // do not use comments ("--"), we want to cause an execution error!
		CLuaScriptObject* defScript=new CLuaScriptObject(true);
		retVal=insertScript(defScript);
		defScript->setScriptType(scriptType);
		defScript->setScriptText(defaultMessage);
		defScript->setThreadedExecution(threaded);
	}
	if (App::ct->objCont!=NULL)
		App::ct->objCont->setLightDialogRefreshFlag();
	return(retVal);
}

int CLuaScriptContainer::insertAddOnScripts()
{
	int addOnsCount=0;
	VFileFinder finder;
	finder.searchFilesWithExtension(App::directories->executableDirectory,ADDON_EXTENTION);
	int cnt=0;
	SFileOrFolder* foundItem=finder.getFoundItem(cnt);
	while (foundItem!=NULL)
	{
		if ( (foundItem->name.find(ADDON_SCRIPT_PREFIX_AUTOSTART)==0)||(foundItem->name.find(ADDON_SCRIPT_PREFIX_NOAUTOSTART)==0) )
		{
			std::string fp(App::directories->executableDirectory+VREP_SLASH);
			fp+=foundItem->name;

			if (VFile::doesFileExist(fp))
			{
				try
				{
					VFile file(fp,VFile::READ|VFile::SHARE_DENY_NONE);
					VArchive archive(&file,VArchive::LOAD);
					DWORD archiveLength=DWORD(file.getLength());
					char* script=new char[archiveLength+1];
					for (int i=0;i<int(archiveLength);i++)
						archive >> script[i];
					script[archiveLength]=0;
					CLuaScriptObject* defScript=new CLuaScriptObject(false);
					insertScript(defScript);
					defScript->setScriptType(sim_scripttype_addonscript);
					defScript->setScriptText(script);

					std::string nm(foundItem->name);
					nm.erase(nm.begin(),nm.begin()+strlen(ADDON_SCRIPT_PREFIX_AUTOSTART));
					nm.erase(nm.end()-strlen(ADDON_EXTENTION)-1,nm.end());

					defScript->setAddOnName(nm.c_str());
					defScript->setThreadedExecution(false);
					if (foundItem->name.find(ADDON_SCRIPT_PREFIX_AUTOSTART)==0)
						defScript->setAddOnDesiredExecutionState(1); // running

					delete[] script;		
					archive.close();
					file.close();
					addOnsCount++;
					printf("Add-on script '%s' was loaded.\n",foundItem->name.c_str());
				}
				catch(VFILE_EXCEPTION_TYPE e)
				{
					printf("Failed loading add-on script '%s'.\n",foundItem->name.c_str());
				}
			}

		}
		cnt++;
		foundItem=finder.getFoundItem(cnt);
	}

	// Now the additional add-on scripts (this is enabled via command line options -a and -b:
	std::vector<std::string> additionalScripts;
	if (_additionalAddOnScriptFirstScene.length()!=0)
	{
		additionalScripts.push_back(_additionalAddOnScriptFirstScene);
		_additionalAddOnScriptFirstScene="";
	}
	if (_additionalAddOnScriptAllScenes.length()!=0)
		additionalScripts.push_back(_additionalAddOnScriptAllScenes);
	for (int addScr=0;addScr<int(additionalScripts.size());addScr++)
	{
		std::string fp(additionalScripts[addScr]);
		if (!VVarious::isAbsolutePath(fp))
		{
			fp=App::directories->executableDirectory+VREP_SLASH;
			fp+=additionalScripts[addScr];
		}
		std::string fileName_noExtension(VVarious::splitPath_fileBase(fp));
		std::string fileName_withExtension(VVarious::splitPath_fileBaseAndExtension(fp));
		if (VFile::doesFileExist(fp))
		{
			try
			{
				VFile file(fp,VFile::READ|VFile::SHARE_DENY_NONE);
				VArchive archive(&file,VArchive::LOAD);
				DWORD archiveLength=DWORD(file.getLength());
				char* script=new char[archiveLength+1];
				for (int i=0;i<int(archiveLength);i++)
					archive >> script[i];
				script[archiveLength]=0;
				CLuaScriptObject* defScript=new CLuaScriptObject(false);
				insertScript(defScript);
				defScript->setScriptType(sim_scripttype_addonscript);
				defScript->setScriptText(script);

				defScript->setAddOnName(fileName_noExtension.c_str());
				defScript->setThreadedExecution(false);
				defScript->setAddOnDesiredExecutionState(1); // running

				delete[] script;
				archive.close();
				file.close();
				addOnsCount++;
				printf("Add-on script '%s' was loaded.\n",fileName_withExtension.c_str());
			}
			catch(VFILE_EXCEPTION_TYPE e)
			{
				printf("Failed loading add-on script '%s'.\n",fileName_withExtension.c_str());
			}
		}
	}

	return(addOnsCount);
}

int CLuaScriptContainer::prepareAddOnFunctionNames()
{
	int addOnsCount=0;
	VFileFinder finder;
	finder.searchFilesWithExtension(App::directories->executableDirectory,ADDON_EXTENTION);
	int cnt=0;
	SFileOrFolder* foundItem=finder.getFoundItem(cnt);
	while (foundItem!=NULL)
	{
		if (foundItem->name.find(ADDON_FUNCTION_PREFIX)==0)
		{
			std::string nm(foundItem->name);
			nm.erase(nm.begin(),nm.begin()+strlen(ADDON_FUNCTION_PREFIX));
			nm.erase(nm.end()-strlen(ADDON_EXTENTION)-1,nm.end());
			allAddOnFunctionNames.push_back(nm);
			addOnsCount++;
		}
		cnt++;
		foundItem=finder.getFoundItem(cnt);
	}
	return(addOnsCount);
}

void CLuaScriptContainer::handleAddOnScriptExecution()
{
	for (int i=0;i<int(allScripts.size());i++)
	{
		CLuaScriptObject* it=allScripts[i];
		if (it->getScriptType()==sim_scripttype_addonscript)
			it->runAddOn();
	}
}

int CLuaScriptContainer::getScriptSimulationParameter_mainAndChildScriptsOnly(int scriptHandle,const char* parameterName,std::string& parameterValue)
{
	int retVal=0;
	for (int i=0;i<int(allScripts.size());i++)
	{
		if (!allScripts[i]->getDestroyAtEndOfSimulation())
		{
			if ( (scriptHandle==allScripts[i]->getScriptID())||
				(scriptHandle==sim_handle_all)||
				( (scriptHandle==sim_handle_main_script)&&(allScripts[i]->getScriptType()==sim_scripttype_mainscript) ) )
			{
				if ((allScripts[i]->getScriptType()==sim_scripttype_mainscript)||(allScripts[i]->getScriptType()==sim_scripttype_childscript))
				{ // We only access main script and child script1
					bool res=allScripts[i]->getScriptParametersObject()->getParameterValue(parameterName,parameterValue);
					if (res)
						retVal++;
				}
			}
		}
	}
	return(retVal);
}

int CLuaScriptContainer::setScriptSimulationParameter_mainAndChildScriptsOnly(int scriptHandle,const char* parameterName,const char* parameterValue,int parameterValueLength)
{
	int retVal=0;
	for (int i=0;i<int(allScripts.size());i++)
	{
		if (!allScripts[i]->getDestroyAtEndOfSimulation())
		{
			if ( (scriptHandle==allScripts[i]->getScriptID())||
				(scriptHandle==sim_handle_all)||
				( (scriptHandle==sim_handle_main_script)&&(allScripts[i]->getScriptType()==sim_scripttype_mainscript) ) )
			{
				if ((allScripts[i]->getScriptType()==sim_scripttype_mainscript)||(allScripts[i]->getScriptType()==sim_scripttype_childscript))
				{ // We only access main script and child script1
					bool res=allScripts[i]->getScriptParametersObject()->setParameterValue(parameterName,parameterValue,parameterValueLength);
					if (res)
						retVal++;
				}
			}
		}
	}
	return(retVal);
}

bool CLuaScriptContainer::addCommandToOutsideCommandQueues(int commandID,int auxVal1,int auxVal2,int auxVal3,int auxVal4,const float aux2Vals[8],int aux2Count)
{
	for (int i=0;i<int(allScripts.size());i++)
	{
		if (!allScripts[i]->getDestroyAtEndOfSimulation())
			allScripts[i]->addCommandToOutsideCommandQueue(commandID,auxVal1,auxVal2,auxVal3,auxVal4,aux2Vals,aux2Count);
	}
	return(true);
}

void CLuaScriptContainer::addMenu(VMenu* menu)
{
	// First add-on functions:
	int id=ADDON_FUNCTION_MENU_ITEM_START_CMD;
	for (int i=0;i<int(App::ct->luaScriptContainer->allAddOnFunctionNames.size());i++)
	{
		VMenubar::appendMenuItem(menu,true,false,id,App::ct->luaScriptContainer->allAddOnFunctionNames[i].c_str());
		id++;
	}

	VMenubar::appendMenuSeparator(menu);

	// Now add-on scripts:
	id=ADDON_SCRIPT_MENU_ITEM_START_CMD;
	for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->allScripts[i];
		if (it->getScriptType()==sim_scripttype_addonscript)
		{
			int st=it->getAddOnExecutionState();
			std::string txt;
			txt=it->getAddOnName();
			if (st==3)
				txt+=" (stopped, error)";
			if (st==2)
				txt+=" (paused)";
			if (st==1)
				txt+=" (running)";
			if (st==0)
				txt+=" (stopped)";
			VMenubar::appendMenuItem(menu,st!=3,st==1,id,txt.c_str(),true);
			id++;
		}
	}
}

bool CLuaScriptContainer::evaluateMenuSelection(int commandID)
{ // Return value is true if the command belonged to hierarchy menu and was executed
	if ( (commandID>=ADDON_FUNCTION_MENU_ITEM_START_CMD)&&(commandID<=ADDON_FUNCTION_MENU_ITEM_END_CMD) )
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("ADDON_FUNCTION_MENU_ITEM_START_CMD")
		{
			int index=commandID-ADDON_FUNCTION_MENU_ITEM_START_CMD;
			if (index<int(allAddOnFunctionNames.size()))
			{
				std::string txt(IDSNS_STARTING_ADDON_FUNCTION);
				txt+=" ";
				txt+=allAddOnFunctionNames[index];
				App::uiThread->addStatusLine(txt.c_str());

				// execute the add-on function here!!
				std::string fp(App::directories->executableDirectory+VREP_SLASH);
				fp+=ADDON_FUNCTION_PREFIX;
				fp+=allAddOnFunctionNames[index];
				fp+=".";
				fp+=ADDON_EXTENTION;
				if (VFile::doesFileExist(fp))
				{
					try
					{
						VFile file(fp,VFile::READ|VFile::SHARE_DENY_NONE);
						VArchive archive(&file,VArchive::LOAD);
						DWORD archiveLength=DWORD(file.getLength());
						char* script=new char[archiveLength+1];
						for (int i=0;i<int(archiveLength);i++)
							archive >> script[i];
						script[archiveLength]=0;
						CLuaScriptObject* defScript=new CLuaScriptObject(false);
						int scriptID=insertScript(defScript);
						defScript->setScriptType(sim_scripttype_addonfunction);
						defScript->setScriptText(script);
						defScript->setAddOnName(allAddOnFunctionNames[index].c_str());
						defScript->setThreadedExecution(false);
						defScript->setAddOnDesiredExecutionState(1); // make sure it will run
						delete[] script;
						archive.close();
						file.close();

						defScript->runAddOn();
						removeScript(scriptID);
					}
					catch(VFILE_EXCEPTION_TYPE e)
					{
	//					printf("Failed loading add-on script '%s'.\n",foundItem->name.c_str());
	//					VFile::reportAndHandleFileExceptionError(e);
					}
				}
				App::uiThread->addStatusLine(IDSNS_ENDED_ADDON_FUNCTION);
			}
		}
		return(true);
	}

	if ( (commandID>=ADDON_SCRIPT_MENU_ITEM_START_CMD)&&(commandID<=ADDON_SCRIPT_MENU_ITEM_END_CMD) )
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("ADDON_SCRIPT_MENU_ITEM_START_CMD")
		{
			int index=commandID-ADDON_SCRIPT_MENU_ITEM_START_CMD;
			int cnt=0;
			CLuaScriptObject* it=NULL;
			for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
			{
				it=App::ct->luaScriptContainer->allScripts[i];
				if (it->getScriptType()==sim_scripttype_addonscript)
				{
					if (index==cnt)
						break;
					it=NULL;
					cnt++;
				}
			}
			if ( (it!=NULL)&&(it->getAddOnExecutionState()!=3) )
			{
				std::string txt;
				int st=it->getAddOnExecutionState();
				if (st==2)
				{
					txt=IDSNS_RESUMED_ADDON_SCRIPT;
					it->setAddOnDesiredExecutionState(1);
				}
				if (st==1)
				{
					txt=IDSNS_PAUSED_ADDON_SCRIPT;
					it->setAddOnDesiredExecutionState(2);
				}
				if (st==0)
				{
					txt=IDSNS_STARTED_ADDON_SCRIPT;
					it->setAddOnDesiredExecutionState(1);
				}
				txt+=" ";
				txt+=it->getAddOnName();
				App::uiThread->addStatusLine(txt.c_str());
			}
		}
		return(true);
	}

	return(false);
}
