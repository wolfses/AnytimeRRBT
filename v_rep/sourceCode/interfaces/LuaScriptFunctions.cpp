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

#include "LuaScriptFunctions.h"
#include "IloIlo.h"
#include "OGL.h"
#include "Tt.h"
#include "LuaTableObject.h"
#include "ThreadPool.h"
#include "LinMotionRoutines.h"
#include "ScintillaConsoleDlg.h"
#include "pluginContainer.h"
#include <boost/lexical_cast.hpp>
#include "VVarious.h"
#include "VDateTime.h"
#include "sensingThreadPool.h"
#include "App.h"

/*
#define LUA_START(funcName) CIloIlo::commonAPIStartProcedure(funcName,true); memorizeLocation(L); std::string functionName(funcName);
#define LUA_END(p) if (true) {forgetLocation();CIloIlo::commonAPIEndProcedure(functionName.c_str(),true);return(p);} else

#define LUA_LOCK CIloIlo::commonAPIStartProcedure("LUA_LOCK",true);
#define LUA_UNLOCK if (true) {CIloIlo::commonAPIEndProcedure("LUA_UNLOCK",true);} else

#define LUA_LOCK_NO_INSTANCE_SWITCH CIloIlo::commonAPIStartProcedure("LUA_LOCK",true);
#define LUA_UNLOCK_NO_INSTANCE_SWITCH if (true) {CIloIlo::commonAPIEndProcedure("LUA_UNLOCK",true);} else

#define LUA_START_NO_LOCK(funcName) CIloIlo::commonAPIStartProcedure(funcName,false); memorizeLocation(L); std::string functionName(funcName);
#define LUA_END_NO_LOCK(p) if (true) {forgetLocation();CIloIlo::commonAPIEndProcedure(functionName.c_str(),false);return(p);} else
*/

#define LUA_START(funcName) memorizeLocation(L); std::string functionName(funcName);
#define LUA_END(p) if (true) {forgetLocation();return(p);} else

#define LUA_LOCK
#define LUA_UNLOCK

#define LUA_LOCK_NO_INSTANCE_SWITCH
#define LUA_UNLOCK_NO_INSTANCE_SWITCH

#define LUA_START_NO_LOCK(funcName) memorizeLocation(L); std::string functionName(funcName);
#define LUA_END_NO_LOCK(p) if (true) {forgetLocation();return(p);} else

#define SIM_SCRIPT_NAME_SUFFIX "sim_script_name_suffix"

std::vector<std::string> locations;
std::vector<VTHREAD_ID_TYPE> threadIDs;
VMutex memorizeLocationMutex;


void memorizeLocation(lua_State* L)
{
	memorizeLocationMutex.lock();
	VTHREAD_ID_TYPE threadID=VThread::getCurrentThreadId();
	int index=getLocationIndex(threadID);
	if (index==-1)
	{
		locations.push_back("");
		threadIDs.push_back(threadID);
		index=getLocationIndex(threadID);
	}

	std::string scriptName("");
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getCurrentScriptID(L));
	if (it!=NULL)
		scriptName=it->getShortDescriptiveName();
	int lineNumber=-1;
	lua_Debug ar;
	if (lua_getstack(L,1,&ar)==1)
	{
		if (lua_getinfo(L,"l",&ar)==1)
			lineNumber=ar.currentline;
	}
	std::string msg("[string \"");
	msg+=scriptName+"\"]:";
	msg+=boost::lexical_cast<std::string>(lineNumber);
	msg+=": ";
	locations[index]=msg;
	memorizeLocationMutex.unlock();
}

std::string getLocationString()
{
	VTHREAD_ID_TYPE threadID=VThread::getCurrentThreadId();
	int index=getLocationIndex(threadID);
	if (index==-1)
		return("[string -unknown location]:?: "); // Should normally not happen!
	return(locations[index]);
}

int getLocationIndex(VTHREAD_ID_TYPE threadID)
{
	for (int i=0;i<int(threadIDs.size());i++)
	{
		if (VThread::areThreadIDsSame(threadID,threadIDs[i]))
			return(i);
	}
	return(-1);
}

void forgetLocation()
{
	memorizeLocationMutex.lock();
	VTHREAD_ID_TYPE threadID=VThread::getCurrentThreadId();
	int index=getLocationIndex(threadID);
	if (index!=-1)
	{ // should always be the case!
		locations.erase(locations.begin()+index);
		threadIDs.erase(threadIDs.begin()+index);
	}
	memorizeLocationMutex.unlock();
}

void pushCorrectTypeOntoLuaStack(lua_State* L,const std::string& txt)
{ // Pushes nil, false, true, number or string (in that order!!) onto the stack depending on the txt content!
	int t=getCorrectType(txt);
	if (t==0)
		lua_pushnil(L);
	if (t==1)
		lua_pushboolean(L,0);
	if (t==2)
		lua_pushboolean(L,1);
	if (t==3)
	{
		float floatVal;
		tt::getValidFloat(txt,floatVal);
		lua_pushnumber(L,floatVal);
	}
	if (t==4)
		lua_pushlstring(L,txt.c_str(),txt.length());
}

int getCorrectType(const std::string& txt)
{ // returns 0=nil, 1=boolean false, 2=boolean true, 3=number or 4=string (in that order!!) depending on the txt content!
	if (txt.length()!=0)
	{
		if (txt.length()!=strlen(txt.c_str()))
			return(4); // We have embedded zeros, this has definitively to be a string:
	}
	if (strcmp(txt.c_str(),"nil")==0)
		return(0);
	if (strcmp(txt.c_str(),"false")==0)
		return(1);
	if (strcmp(txt.c_str(),"true")==0)
		return(2);
	float floatVal;
	if (tt::getValidFloat(txt,floatVal))
		return(3);
	return(4);
}

void getScriptTree(lua_State* L,bool selfIncluded,std::vector<int>& scriptHandles)
{ // Returns all scripts that are built under the current one
	scriptHandles.clear();
	int currentScriptID=getCurrentScriptID(L);
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
	if (selfIncluded)
		scriptHandles.push_back(currentScriptID);
	if (it->getScriptType()==sim_scripttype_mainscript)
	{ // we have a main script here
		for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
		{
			C3DObject* q=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
			CLuaScriptObject* lso=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo(q->getID());
			if (lso!=NULL)
				scriptHandles.push_back(lso->getScriptID());
		}
	}
	else
	{ // we have a script linked to a 3d object
		C3DObject* obj=App::ct->objCont->getObject(it->getObjectIDThatScriptIsAttachedTo());
		if (obj!=NULL)
		{ // the script is attached to an object and is not passive
			std::vector<C3DObject*> objList;
			obj->getAllObjectsRecursive(&objList,false);
			for (int i=0;i<int(objList.size());i++)
			{
				CLuaScriptObject* lso=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo(objList[i]->getID());
				if (lso!=NULL)
					scriptHandles.push_back(lso->getScriptID());
			}
		}
		else
		{ // We have another type of script here! We don't do anything for now
		}
	}
}

void getScriptChain(lua_State* L,bool selfIncluded,bool mainIncluded,std::vector<int>& scriptHandles)
{ // Returns all script IDs that are parents (or grand-parents,grand-grand-parents, etc.) of the current one
	scriptHandles.clear();
	int currentScriptID=getCurrentScriptID(L);
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
	if (it->getScriptType()==sim_scripttype_mainscript)
	{ // we have a main script here
		if (selfIncluded&&mainIncluded)
			scriptHandles.push_back(currentScriptID);
	}
	else
	{ // we have a script linked to a 3d object
		C3DObject* obj=App::ct->objCont->getObject(it->getObjectIDThatScriptIsAttachedTo());
		if (obj!=NULL)
		{ // the script is attached to an object and is not passive
			if (selfIncluded)
				scriptHandles.push_back(currentScriptID);
			while (obj->getParent()!=NULL)
			{
				obj=obj->getParent();
				CLuaScriptObject* lso=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo(obj->getID());
				if (lso!=NULL)
					scriptHandles.push_back(lso->getScriptID());
			}
			if (mainIncluded)
			{
				CLuaScriptObject* lso=App::ct->luaScriptContainer->getMainScript();
				if (lso!=NULL)
					scriptHandles.push_back(lso->getScriptID());
			}
		}
		else
		{ // We have another type of script here. We don't do anything for now
		}
	}
}

lua_State* initializeNewLuaState(const char* scriptSuffixNumberString)
{
    lua_State* L=luaL_newstate();
    luaL_openlibs(L);
	registerNewLuaFunctions(L);
	prepareNewLuaVariables(L,scriptSuffixNumberString);
	lua_sethook(L,luaHookFunction,LUA_MASKCOUNT,100); // This instruction gets also called in luaHookFunction!!!!
	return(L);
}

void registerNewLuaFunctions(lua_State* L)
{
	for (int i=0;simLuaCommands[i].name!="";i++)
		lua_register(L,simLuaCommands[i].name.c_str(),simLuaCommands[i].func);
	registerCustomLuaFunctions(L);
}

void registerCustomLuaFunctions(lua_State* L)
{
	for (int i=0;i<int(CIloIlo::luaCustomFunctionContainer.allCustomFunctions.size());i++)
	{
		CLuaCustomFunction* it=CIloIlo::luaCustomFunctionContainer.allCustomFunctions[i];
		lua_pushnumber(L,it->getFunctionID()+1);
		lua_pushcclosure(L,_simGenericFunctionHandler,1);
#ifdef LIN_VREP
        lua_setglobal(L,it->getFunctionName().c_str());
#else
		lua_setfield(L,LUA_GLOBALSINDEX,it->getFunctionName().c_str());
#endif
	}
}

void luaHookFunction(lua_State* L,lua_Debug* ar)
{
	// Following 2 instructions are important: it can happen that the user locks/unlocks automatic thread switch in a loop,
	// and that the hook function by malchance only gets called when the thread switches are not allowed (due to the loop
	// timing and hook call timing overlap) --> this thread doesn't switch and stays in a lua loop forever.
	// To avoid this we add some random component to the hook timing:
	int randComponent=rand()/(RAND_MAX/10);
	lua_sethook(L,luaHookFunction,LUA_MASKCOUNT,95+randComponent);

	if (VThread::isCurrentThreadTheMainThread())
		App::ct->simulation->emergencyStopPassFromLuaHook();

	CLuaScriptObject* it;
	// Following line is special and specific to threads:
	int instanceAssociatedWithThisThread=CThreadPool::getInstanceCurrentThreadIsAssociatedWith();
	if (instanceAssociatedWithThisThread==-1) // -1 means it doesn't matter
		it=App::ct->luaScriptContainer->getScriptFromID(getCurrentScriptID(L)); // CAREFUL!!! SOMEHOW 'it' can be NULL (first pass of the main script?)
	else
		it=App::ct->luaScriptContainerList[instanceAssociatedWithThisThread]->getScriptFromID(getCurrentScriptID(L)); // CAREFUL!!! SOMEHOW 'it' can be NULL (first pass of the main script?)
	if (CThreadPool::getEmergencyStopForCurrentInstance()||((it!=NULL)&&(it->getDestroyAtEndOfSimulation())) )
	{
		lua_yield(L,0);
		return;
	}
	else
	{
		if (CThreadPool::getStopForCurrentInstanceRequestedAndActivated())
		{ // returns true only after 1-2 seconds after the request arrived
			if (!VThread::isCurrentThreadTheMainThread())
			{
				lua_yield(L,0);
				return;
			}
		}
	}
	CThreadPool::switchBackToPreviousThreadIfNeeded();
}

void prepareNewLuaVariables(lua_State* L,const char* scriptSuffixNumberString)
{
	for (int i=0;simLuaVariables[i].name!="";i++)
	{
		setNewLuaVariable(L,simLuaVariables[i].name.c_str(),simLuaVariables[i].val);
	}
	registerCustomLuaVariables(L);

	// Here we have the name prefix thing:
	std::string tmp(SIM_SCRIPT_NAME_SUFFIX);
	tmp+="='";
	tmp+=scriptSuffixNumberString;
	tmp+="'";
	luaL_dostring(L,tmp.c_str());
}

void registerCustomLuaVariables(lua_State* L)
{
	for (int i=0;i<int(CIloIlo::luaCustomFunctionContainer.allCustomVariableNames.size());i++)
	{
		std::string tmp(CIloIlo::luaCustomFunctionContainer.allCustomVariableNames[i]);
		tmp+="="+CIloIlo::luaCustomFunctionContainer.allCustomVariableValues[i];
		luaL_dostring(L,tmp.c_str());
	}
}

void setNewLuaVariable(lua_State* L,const char* name,int identifier)
{
	std::string tmp(name);
	tmp+="="+boost::lexical_cast<std::string>(identifier);
	luaL_dostring(L,tmp.c_str());
}

int getCurrentScriptID(lua_State* L)
{
	lua_getglobal(L,"sim_current_script_id");
	int retVal=-1;
	if (lua_isnumber(L,-1))
		retVal=lua_tointeger(L,-1);
	lua_pop(L,1);
	return(retVal);
}

bool readCustomFunctionDataFromStack(lua_State* L,int ind,int dataType,
									 std::vector<char>& inBoolVector,
									 std::vector<int>& inIntVector,
									 std::vector<float>& inFloatVector,
									 std::vector<std::string>& inStringVector,
									 std::vector<std::string>& inCharVector,
									 std::vector<int>& inInfoVector)
{ // return value false means there is no more data on the stack
	if (lua_gettop(L)<ind)
		return(false); // not enough data on the stack
	inInfoVector.push_back(sim_lua_arg_invalid);	// Dummy value for type
	inInfoVector.push_back(0);						// dummy value for size
	if (lua_isnil(L,ind))
	{ // Special case: nil should not generate a sim_lua_arg_invalid type!
		inInfoVector[inInfoVector.size()-2]=sim_lua_arg_nil;
		return(true);
	}
	if (dataType&sim_lua_arg_table)
	{ // we have to read a table:
		dataType^=sim_lua_arg_table;
		if (!lua_istable(L,ind))
			return(true); // this is not a table
        int dataSize=int(VREP_LUA_OBJLEN(L,ind));
		std::vector<char> boolV;
		std::vector<int> intV;
		std::vector<float> floatV;
		std::vector<std::string> stringV;
		for (int i=0;i<dataSize;i++)
		{
			lua_rawgeti(L,ind,i+1);
			if (dataType==sim_lua_arg_bool)
			{
				if (!lua_isboolean(L,-1))
				{
					lua_pop(L,1); // we have to pop the value that was pushed with lua_rawgeti
					return(true); // we don't have the correct data type
				}
				boolV.push_back(luaToBool(L,-1));
			}
			else if (dataType==sim_lua_arg_int)
			{
				if (!lua_isnumber(L,-1))
				{
					lua_pop(L,1); // we have to pop the value that was pushed with lua_rawgeti
					return(true); // we don't have the correct data type
				}
				intV.push_back(luaToInt(L,-1));
			}
			else if (dataType==sim_lua_arg_float)
			{
				if (!lua_isnumber(L,-1))
				{
					lua_pop(L,1); // we have to pop the value that was pushed with lua_rawgeti
					return(true); // we don't have the correct data type
				}
				floatV.push_back(luaToFloat(L,-1));
			}
			else if (dataType==sim_lua_arg_string)
			{
				if (!lua_isstring(L,-1))
				{
					lua_pop(L,1); // we have to pop the value that was pushed with lua_rawgeti
					return(true); // we don't have the correct data type
				}
				stringV.push_back(std::string(lua_tostring(L,-1)));
			}
			else
			{
				lua_pop(L,1); // we have to pop the value that was pushed with lua_rawgeti
				return(true); // data type not recognized!
			}
			lua_pop(L,1); // we have to pop the value that was pushed with lua_rawgeti
		}
		// All values in the tables passed the check!
		for (int i=0;i<int(boolV.size());i++)
			inBoolVector.push_back(boolV[i]);
		for (int i=0;i<int(intV.size());i++)
			inIntVector.push_back(intV[i]);
		for (int i=0;i<int(floatV.size());i++)
			inFloatVector.push_back(floatV[i]);
		for (int i=0;i<int(stringV.size());i++)
			inStringVector.push_back(stringV[i]);
		inInfoVector[inInfoVector.size()-1]=dataSize; // Size of the table
		inInfoVector[inInfoVector.size()-2]=dataType|sim_lua_arg_table; // Type
		return(true);
	}
	else
	{ // we have simple data here (not a table)
		if (dataType==sim_lua_arg_bool)
		{
			if (!lua_isboolean(L,ind))
				return(true); // we don't have the correct data type
			inBoolVector.push_back(luaToBool(L,ind));
			inInfoVector[inInfoVector.size()-2]=dataType;
		}
		else if (dataType==sim_lua_arg_int)
		{
			if (!lua_isnumber(L,ind))
				return(true); // we don't have the correct data type
			inIntVector.push_back(luaToInt(L,ind));
			inInfoVector[inInfoVector.size()-2]=dataType;
		}
		else if (dataType==sim_lua_arg_float)
		{
			if (!lua_isnumber(L,ind))
				return(true); // we don't have the correct data type
			inFloatVector.push_back(luaToFloat(L,ind));
			inInfoVector[inInfoVector.size()-2]=dataType;
		}
		else if (dataType==sim_lua_arg_string)
		{
			if (!lua_isstring(L,ind))
				return(true); // we don't have the correct data type
			inStringVector.push_back(std::string(lua_tostring(L,ind)));
			inInfoVector[inInfoVector.size()-2]=dataType;
		}
		else if (dataType==sim_lua_arg_charbuff)
		{
			if (!lua_isstring(L,ind))
				return(true); // we don't have the correct data type
			size_t dataLength;
			char* data=(char*)lua_tolstring(L,ind,&dataLength);
			inCharVector.push_back(std::string(data,dataLength));
			inInfoVector[inInfoVector.size()-2]=dataType;
			inInfoVector[inInfoVector.size()-1]=int(dataLength);
		}
		return(true); // data type not recognized!
	}
	return(true);
}

void writeCustomFunctionDataOntoStack(lua_State* L,int dataType,int dataSize,
									  unsigned char* boolData,int& boolDataPos,
									  int* intData,int& intDataPos,
									  float* floatData,int& floatDataPos,
									  char* stringData,int& stringDataPos,
									  char* charData,int& charDataPos)
{
	if (((dataType|sim_lua_arg_table)-sim_lua_arg_table)==sim_lua_arg_charbuff)
	{ // special handling here
		lua_pushlstring(L,charData+charDataPos,dataSize);
		charDataPos+=dataSize;
	}
	else
	{
		int newTablePos=0;
		bool weHaveATable=false;
		if (dataType&sim_lua_arg_table)
		{ // we have a table
			lua_newtable(L);
			newTablePos=lua_gettop(L);
			dataType^=sim_lua_arg_table;
			weHaveATable=true;
		}
		else
			dataSize=1;
		for (int i=0;i<dataSize;i++)
		{
			if (dataType==sim_lua_arg_bool)
				lua_pushboolean(L,boolData[boolDataPos++]);
			else if (dataType==sim_lua_arg_int)
				lua_pushinteger(L,intData[intDataPos++]);
			else if (dataType==sim_lua_arg_float)
				lua_pushnumber(L,floatData[floatDataPos++]);
			else if (dataType==sim_lua_arg_string)
			{
				lua_pushstring(L,stringData+stringDataPos);
				stringDataPos+=strlen(stringData+stringDataPos)+1; // Thanks to Ulrich Schwesinger for noticing a bug here!
			}
			else
				lua_pushnil(L); // that is an error!

			if (weHaveATable) // that's when we have a table
				lua_rawseti(L,newTablePos,i+1);
		}
	}
}

void getFloatsFromTable(lua_State* L,int tablePos,int floatCount,float* arrayField)
{
	for (int i=0;i<floatCount;i++)
	{
		lua_rawgeti(L,tablePos,i+1);
		arrayField[i]=luaToFloat(L,-1);
		lua_pop(L,1); // we pop one element from the stack;
	}
}

void getDoublesFromTable(lua_State* L,int tablePos,int doubleCount,double* arrayField)
{
	for (int i=0;i<doubleCount;i++)
	{
		lua_rawgeti(L,tablePos,i+1);
		arrayField[i]=lua_tonumber(L,-1);
		lua_pop(L,1); // we pop one element from the stack;
	}
}

bool getIntsFromTable(lua_State* L,int tablePos,int intCount,int* arrayField)
{
	for (int i=0;i<intCount;i++)
	{
		lua_rawgeti(L,tablePos,i+1);
		if (!lua_isnumber(L,-1))
		{
			lua_pop(L,1); // we pop one element from the stack;
			return(false); // Not a number!!
		}
		arrayField[i]=luaToInt(L,-1);
		lua_pop(L,1); // we pop one element from the stack;
	}
	return(true);
}

void getBoolsFromTable(lua_State* L,int tablePos,int boolCount,char* arrayField)
{
	for (int i=0;i<boolCount;i++)
	{
		lua_rawgeti(L,tablePos,i+1);
		arrayField[i]=(char)lua_toboolean(L,-1);
		lua_pop(L,1); // we pop one element from the stack;
	}
}

void pushFloatTableOntoStack(lua_State* L,int floatCount,const float* arrayField)
{
	lua_newtable(L);						
	int newTablePos=lua_gettop(L);
	for (int i=0;i<floatCount;i++)
	{
		lua_pushnumber(L,arrayField[i]);
		lua_rawseti(L,newTablePos,i+1);
	}
}

void pushDoubleTableOntoStack(lua_State* L,int doubleCount,const double* arrayField)
{
	lua_newtable(L);						
	int newTablePos=lua_gettop(L);
	for (int i=0;i<doubleCount;i++)
	{
		lua_pushnumber(L,arrayField[i]);
		lua_rawseti(L,newTablePos,i+1);
	}
}

void pushIntTableOntoStack(lua_State* L,int intCount,const int* arrayField)
{
	lua_newtable(L);						
	int newTablePos=lua_gettop(L);
	for (int i=0;i<intCount;i++)
	{
		lua_pushnumber(L,arrayField[i]);
		lua_rawseti(L,newTablePos,i+1);
	}
}

int luaToInt(lua_State* L,int pos)
{
	return((int)lua_tonumber(L,pos));
}

float luaToFloat(lua_State* L,int pos)
{
	return((float)lua_tonumber(L,pos));
}

bool luaToBool(lua_State* L,int pos)
{
	return(lua_toboolean(L,pos)!=0);
}

bool suffixAdjustStringIfNeeded(const std::string& functionName,bool outputError,lua_State* L,std::string& name)
{
	std::string suffTxt;
	lua_getglobal(L,SIM_SCRIPT_NAME_SUFFIX);
	if (lua_isstring(L,-1))
		suffTxt=lua_tostring(L,-1);
	lua_pop(L,1);

	if (suffTxt=="")
	{ // we don't want adjustment (the script name doesn't have any '#')
		// The special cases like "marci#" are handled on the C-API side!
		return(true); // means no error
	}

	// We want suffix adjustment!
	if (!isDashFree(functionName,name))
	{ // We have a dash in the name of the object we want the handle of
		// We explicitely access the object (no name adjustment)
		// The special cases like "marci#" are handled on the C-API side!
		return(true); // no error
	}

	name+="#";
	name+=suffTxt;
	return(true);// no error
}

bool isDashFree(const std::string& functionName,const std::string& name)
{
	if (name.find('#')==std::string::npos)
		return(true);
	return(false);
}

int checkOneGeneralInputArgument(lua_State* L,int index,std::string& functionName,
						   int type,int cnt_orZeroIfNotTable,bool optional,bool nilInsteadOfTypeAndCountAllowed,bool outputErrors)
{ // return -1 means error, 0 means data is missing, 1 means data is nil, 2 means data is ok
	// 1. We check if there is something on the stack at that position:
	if (lua_gettop(L)<index)
	{ // That data is missing:
		if (optional)
			return(0);
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS,outputErrors);
		return(-1);
	}
	// 2. We check if we have nil on the stack:
	if (lua_isnil(L,index))
	{ // We have nil.
		// Did we expect a boolean? If yes, it is ok
		if ( (type==lua_arg_bool)&&(cnt_orZeroIfNotTable==0) )
			return(2);
		if ( (type==lua_arg_nil)&&(cnt_orZeroIfNotTable==0) )
			return(2);
		if (nilInsteadOfTypeAndCountAllowed)
			return(1);
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG,outputErrors);
		return(-1);
	}
	// 3. we check if we expect a table:
	if (cnt_orZeroIfNotTable>=1)
	{ 
		// We check if we really have a table at that position:
		if (!lua_istable(L,index))
		{
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG,outputErrors);
			return(-1);
		}
		// we check the table size:
        if (int(VREP_LUA_OBJLEN(L,index))<cnt_orZeroIfNotTable)
		{ // the size is not correct
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_ONE_TABLE_SIZE_IS_WRONG,outputErrors);
			return(-1);
		}
		else
		{ // we have the correct size
			// Now we need to check each element individually for the correct type:
			for (int i=0;i<cnt_orZeroIfNotTable;i++)
			{
				lua_rawgeti(L,index,i+1);
				if (!checkOneInputArgument(L,-1,functionName,type,outputErrors))
					return(-1);
				lua_pop(L,1); // we have to pop the value that was pushed with lua_rawgeti
			}
			// Everything went fine:
			return(2);
		}
	}
	else
	{ // we have a normal type
		if (checkOneInputArgument(L,index,functionName,type,outputErrors))
			return(2);
		return(-1);
	}
}

void outputCallErrorMessage(const char* functionName,const char* message,bool outputError)
{
	if (outputError)
	{
		std::string msg(getLocationString());
		msg+=message;
		App::ct->luaScriptContainer->setLuaCallError(functionName,msg.c_str());
		// Here we can decide what happens uppon error:
		App::ct->simulation->pauseOnErrorRequested();
	}
}

void outputCallWarningMessage(const char* functionName,const char* message)
{
	std::string msg(getLocationString());
	msg+=message;
	App::ct->luaScriptContainer->setLuaCallWarning(functionName,msg.c_str());
}

bool checkOneInputArgument(lua_State* L,int index,std::string& functionName,int type,bool outputErrors)
{
	// 1. We check if there is something on the stack at that position:
	if (lua_gettop(L)<index)
	{ // That data is missing:
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS,outputErrors);
		return(false);
	}
	if (type==lua_arg_number)
	{
		if (!lua_isnumber(L,index))
		{
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG,outputErrors);
			return(false); // error
		}
		return(true);
	}
	if (type==lua_arg_bool)
	{ // since anything can be a bool value, we don't generate any error!
		return(true);
	}
	if (type==lua_arg_nil)
	{ // Here we expect a nil value:
		if (!lua_isnil(L,index))
		{
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG,outputErrors);
			return(false);
		}
		return(true);
	}
	if (type==lua_arg_string)
	{
		if (!lua_isstring(L,index))
		{
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG,outputErrors);
			return(false); // error
		}
		return(true);
	}
	if (type==lua_arg_table)
	{
		if (!lua_istable(L,index))
		{
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG,outputErrors);
			return(false); // error
		}
		return(true);
	}
	if (type==lua_arg_function)
	{
		if (!lua_isfunction(L,index))
		{
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG,outputErrors);
			return(false); // error
		}
		return(true);
	}
	if (type==lua_arg_userdata)
	{
		if (!lua_isuserdata(L,index))
		{
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG,outputErrors);
			return(false); // error
		}
		return(true);
	}
	// Here we have a table
	outputCallErrorMessage(functionName.c_str(),SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG,outputErrors);
	return(false);
}

bool checkInputArguments(lua_State* L,std::string& functionName,bool outputErrors,
						 int type1,int type1Cnt_zeroIfNotTable,
						 int type2,int type2Cnt_zeroIfNotTable,
						 int type3,int type3Cnt_zeroIfNotTable,
						 int type4,int type4Cnt_zeroIfNotTable,
						 int type5,int type5Cnt_zeroIfNotTable,
						 int type6,int type6Cnt_zeroIfNotTable,
						 int type7,int type7Cnt_zeroIfNotTable,
						 int type8,int type8Cnt_zeroIfNotTable,
						 int type9,int type9Cnt_zeroIfNotTable,
						 int type10,int type10Cnt_zeroIfNotTable,
						 int type11,int type11Cnt_zeroIfNotTable)
{ // all arguments, except L and the functionName have default values
	// CAREFUL!!! if typeXCnt_zeroIfNotTable is 0, it means it is a simple value (not table)
	// if typeXCnt_zeroIfNotTable is >=1 then we are expecting a table!!
	int inArgs[11]={type1,type2,type3,type4,type5,type6,type7,type8,type9,type10,type11};
	int inArgsCnt_zeroIfNotTable[11]={type1Cnt_zeroIfNotTable,type2Cnt_zeroIfNotTable,type3Cnt_zeroIfNotTable,type4Cnt_zeroIfNotTable,type5Cnt_zeroIfNotTable,type6Cnt_zeroIfNotTable,type7Cnt_zeroIfNotTable,type8Cnt_zeroIfNotTable,type9Cnt_zeroIfNotTable,type10Cnt_zeroIfNotTable,type11Cnt_zeroIfNotTable};
	int totArgs=0;
	for (int i=0;i<11;i++)
	{
		if (inArgs[i]==lua_arg_empty)
			break;
		if (inArgsCnt_zeroIfNotTable[i]<0)
			inArgsCnt_zeroIfNotTable[i]=0;
		totArgs++;
	}
	int numberOfArguments=lua_gettop(L);
	if (numberOfArguments<totArgs)
	{ // we don't have enough arguments!
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS,outputErrors);
		return(false); // error
	}
	for (int i=0;i<totArgs;i++)
	{
		if (checkOneGeneralInputArgument(L,i+1,functionName,inArgs[i],inArgsCnt_zeroIfNotTable[i],false,false,outputErrors)==-1)
			return(false); // error
	}
	return(true);
}

int _simGenericFunctionHandler(lua_State* L)
{	// THIS FUNCTION SHOULD NOT LOCK THE API (AT LEAST NOT WHILE CALLING THE CALLBACK!!) SINCE IT IS NOT DIRECTLY ACCESSING THE API!!!!
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGenericFunctionHandler");
	lua_pushvalue(L,lua_upvalueindex(1));
	int id=lua_tointeger(L,-1)-1;
	lua_pop(L,1); // we have to pop the pushed value to get the original stack state
	for (int j=0;j<int(CIloIlo::luaCustomFunctionContainer.allCustomFunctions.size());j++)
	{ // we now search for the callback to call:
		CLuaCustomFunction* it=CIloIlo::luaCustomFunctionContainer.allCustomFunctions[j];
		if (it->getFunctionID()==id)
		{ // we have the right one! Now we need to prepare the input and output argument arrays:
			// We first read all arguments from the stack
			std::vector<char> inBoolVector;
			std::vector<int> inIntVector;
			std::vector<float> inFloatVector;
			std::vector<std::string> inStringVector;
			std::vector<std::string> inCharVector;
			std::vector<int> inInfoVector;
			for (int i=0;i<int(it->inputArgTypes.size());i++)
			{
				if (!readCustomFunctionDataFromStack(L,i+1,it->inputArgTypes[i],inBoolVector,inIntVector,inFloatVector,inStringVector,inCharVector,inInfoVector))
					break;
			}

			// We retrieve the suffix:
			std::string suffix("");
			lua_getglobal(L,SIM_SCRIPT_NAME_SUFFIX);
			if (lua_isstring(L,-1))
				suffix=lua_tostring(L,-1);
			lua_pop(L,1); // we correct the stack
			// Now we retrieve the object ID this script might be attached to:
			int currentScriptID=getCurrentScriptID(L);
			CLuaScriptObject* itObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
			int linkedObject=-1;
			if (itObj->getScriptType()!=sim_scripttype_mainscript)
			{
				C3DObject* obj=App::ct->objCont->getObject(itObj->getObjectIDThatScriptIsAttachedTo());
				if (obj!=NULL)
					linkedObject=obj->getID();
			}
			// We prepare the callback structure:
			SLuaCallBack* p=new SLuaCallBack;
			p->objectID=linkedObject;
			p->inputBool=NULL;
			p->inputInt=NULL;
			p->inputFloat=NULL;
			p->inputChar=NULL;
			p->inputCharBuff=NULL;
			p->inputArgCount=0;
			p->inputArgTypeAndSize=NULL;
			p->outputBool=NULL;
			p->outputInt=NULL;
			p->outputFloat=NULL;
			p->outputChar=NULL;
			p->outputCharBuff=NULL;
			p->outputArgCount=0;
			p->outputArgTypeAndSize=NULL;
			p->waitUntilZero=0;
			// Now we prepare the input buffers:
			p->inputBool=new unsigned char[inBoolVector.size()];
			p->inputInt=new int[inIntVector.size()];
			p->inputFloat=new float[inFloatVector.size()];
			int charCnt=0;
			for (int k=0;k<int(inStringVector.size());k++)
				charCnt+=inStringVector[k].length()+1; // terminal 0 
			p->inputChar=new char[charCnt];

			int charBuffCnt=0;
			for (int k=0;k<int(inCharVector.size());k++)
				charBuffCnt+=inCharVector[k].length();
			p->inputCharBuff=new char[charBuffCnt];

			p->inputArgCount=inInfoVector.size()/2;
			p->inputArgTypeAndSize=new int[inInfoVector.size()];
			// We fill the input buffers:
			for (int k=0;k<int(inBoolVector.size());k++)
				p->inputBool[k]=inBoolVector[k];
			for (int k=0;k<int(inIntVector.size());k++)
				p->inputInt[k]=inIntVector[k];
			for (int k=0;k<int(inFloatVector.size());k++)
				p->inputFloat[k]=inFloatVector[k];
			charCnt=0;
			for (int k=0;k<int(inStringVector.size());k++)
			{
				for (int l=0;l<int(inStringVector[k].length());l++)
					p->inputChar[charCnt+l]=inStringVector[k][l];
				charCnt+=inStringVector[k].length();
				// terminal 0:
				p->inputChar[charCnt]=0;
				charCnt++;
			}

			charBuffCnt=0;
			for (int k=0;k<int(inCharVector.size());k++)
			{
				for (int l=0;l<int(inCharVector[k].length());l++)
					p->inputCharBuff[charBuffCnt+l]=inCharVector[k][l];
				charBuffCnt+=inCharVector[k].length();
			}

			for (int k=0;k<int(inInfoVector.size());k++)
				p->inputArgTypeAndSize[k]=inInfoVector[k];

			// Now we can call the callback:
			LUA_UNLOCK; // IMPORTANT!!
#ifdef WIN_VREP
//			if (CIloIlo::showApiAccess)
//				printf("_CUSTOM LUA API_ <IN> (func: %s thread: %u)\n",functionName.c_str(),(unsigned long)VThread::getCurrentThreadId());
			it->callBackFunction(p);
//			if (CIloIlo::showApiAccess)
//				printf("_CUSTOM LUA API_ <OUT> (func: %s thread: %u)\n",functionName.c_str(),(unsigned long)VThread::getCurrentThreadId());
#else
//			if (CIloIlo::showApiAccess)
//				printf("_CUSTOM LUA API_ <IN> (func: %s thread: %p)\n",functionName.c_str(),VThread::getCurrentThreadId());
			it->callBackFunction(p);
//			if (CIloIlo::showApiAccess)
//				printf("_CUSTOM LUA API_ <OUT> (func: %s thread: %p)\n",functionName.c_str(),VThread::getCurrentThreadId());
#endif
			bool dontDeleteStructureYet=false;
			while (p->waitUntilZero!=0)
			{ // todo: What happens to a main script caught in here?! check (and fix) later
				if (!CThreadPool::switchBackToPreviousThread())
					break;
				if (CThreadPool::getStopForCurrentInstanceRequestedAndActivated())
				{ // give a chance to the c app to set the waitUntilZero to zero! (above turns true only 1-2 secs after the stop request arrived)
					// Following: the extension module might still write 0 into that position to signal "no more waiting" in
					// case this while loop got interrupted by a stop request.
					dontDeleteStructureYet=true;
					break;
				}
			}
			LUA_LOCK; // IMPORTANT!!

			// We first delete the input buffers:
			delete[] p->inputBool;
			delete[] p->inputInt;
			delete[] p->inputFloat;
			delete[] p->inputChar;
			delete[] p->inputCharBuff;
			delete[] p->inputArgTypeAndSize;
			// Now we have to build the returned data onto the stack:
			if (p->outputArgCount!=0)
			{
				int boolPt=0;
				int intPt=0;
				int floatPt=0;
				int stringPt=0;
				int stringBuffPt=0;
				for (int i=0;i<p->outputArgCount;i++)
				{
					writeCustomFunctionDataOntoStack(L,p->outputArgTypeAndSize[2*i+0],p->outputArgTypeAndSize[2*i+1],
						p->outputBool,boolPt,
						p->outputInt,intPt,
						p->outputFloat,floatPt,
						p->outputChar,stringPt,
						p->outputCharBuff,stringBuffPt);
				}
			}
			// We now delete the output buffers:
			delete[] p->outputBool;
			delete[] p->outputInt;
			delete[] p->outputFloat;
			delete[] p->outputChar;
			delete[] p->outputCharBuff;
			delete[] p->outputArgTypeAndSize;
			// And we return the number of arguments:
			int outputArgCount=p->outputArgCount;
			if (dontDeleteStructureYet)
			{	// We cannot yet delete the structure because an extension module might still write '0' into
				// p->waitUntilZero!! We delete the structure at the end of the simulation.
				App::ct->luaScriptContainer->addCallbackStructureObjectToDestroyAtTheEndOfSimulation(p);
			}
			else
				delete p;
			if (outputArgCount==0)
			{ 
				lua_pushnil(L);
				outputArgCount=1;
			}
			LUA_END(outputArgCount);
		}
	}
	lua_pushnil(L); // indicates an error
	LUA_END(1);	// error, that function doesn't exist!
}

int _simHandleChildScript(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START_NO_LOCK("simHandleChildScript");
	LUA_LOCK_NO_INSTANCE_SWITCH; // Important to lock for this section (objects might get destroyed in the mean time!)
	if (!checkInputArguments(L,functionName,true,lua_arg_number,0))
	{ // Input argument error
		lua_pushnil(L);
		LUA_UNLOCK_NO_INSTANCE_SWITCH;
		LUA_END_NO_LOCK(1); 
	}
	int uniqueChildScriptHandle=luaToInt(L,1);
	if ( (uniqueChildScriptHandle<0)&&(uniqueChildScriptHandle!=sim_handle_all)&&(uniqueChildScriptHandle!=sim_handle_all_except_explicit) )
	{
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_ARGUMENT,true);
		lua_pushnil(L);
		LUA_UNLOCK_NO_INSTANCE_SWITCH;
		LUA_END_NO_LOCK(1); 
	}
	if (uniqueChildScriptHandle>=0)
	{ // explicit handling
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(uniqueChildScriptHandle);
		if (it==NULL) 
		{
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_SCRIPT_INEXISTANT,true);
			lua_pushnil(L);
			LUA_UNLOCK_NO_INSTANCE_SWITCH;
			LUA_END_NO_LOCK(1); 
		}
		if (!it->getExplicitHandling())
		{
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_SCRIPT_NOT_TAGGED_FOR_EXPLICIT_HANDLING,true);
			lua_pushnil(L);
			LUA_UNLOCK_NO_INSTANCE_SWITCH;
			LUA_END_NO_LOCK(1); 
		}
	}

	int currentScriptID=getCurrentScriptID(L);
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
	if ( (it->getScriptType()!=sim_scripttype_mainscript)&&(it->getScriptType()!=sim_scripttype_childscript) )
	{ // add-ons cannot call this function!
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_NOT_MAIN_NOR_CHILD_SCRIPT,true);
		lua_pushnil(L);
		LUA_UNLOCK_NO_INSTANCE_SWITCH;
		LUA_END_NO_LOCK(1); 
	}

	if ( (it->getDelegateChildScriptExecution())&&(uniqueChildScriptHandle<0) )
	{ // error, scripts marked with the DelegateChildScriptExecution flag can only handle explicitely!
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_NON_EXPLICIT_CHILD_SCRIPT_EXECUTIONS_WERE_DELEGATED,true);
		lua_pushnil(L);
		LUA_UNLOCK_NO_INSTANCE_SWITCH;
		LUA_END_NO_LOCK(1); 
	}
	// *********************************************************************************

	int executionResult=sim_script_no_error;
	// We read the function input arguments:
	CLuaObjectGroup inputArguments;
	inputArguments.buildObjectsFromStack(L,2);
	// We prepare the function output arguments:
	CLuaObjectGroup outputArguments;
	std::vector<int> childScriptIDsToRun;
	if (it->getScriptType()==sim_scripttype_mainscript)
	{ // we have a main script here
		// We first identify all parentless objects and put them into a list:
		for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
		{	// Do we have a parentless object?
			C3DObject* q=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
			if ( (q!=NULL)&&(q->getParent()==NULL) )
				q->getChildScriptsToRun(childScriptIDsToRun);
		}
		// Next we identify all objects that are parented with scripts that have the "delegateChildScriptExecution" flag set:
		for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
		{
			CLuaScriptObject* so=App::ct->luaScriptContainer->allScripts[i];
			if (so->getDelegateChildScriptExecution())
			{
				C3DObject* obj=App::ct->objCont->getObject(so->getObjectIDThatScriptIsAttachedTo());
				if (obj!=NULL)
				{ // the script is attached to an object and is not passive
					for (int j=0;j<int(obj->childList.size());j++)
					{
						std::vector<int> tmp;
						obj->childList[j]->getChildScriptsToRun(tmp);
						for (int k=0;k<int(tmp.size());k++)
						{
							CLuaScriptObject* so2=App::ct->luaScriptContainer->getScriptFromID(tmp[k]);
							if ( (so2!=NULL)&&(!so2->getExplicitHandling()) )
								childScriptIDsToRun.push_back(tmp[k]);
						}
					}
				}
			}
		}
	}
	else
	{ // we have a script linked to a 3d object or a passive script here
		C3DObject* obj=App::ct->objCont->getObject(it->getObjectIDThatScriptIsAttachedTo());
		if (obj!=NULL)
		{ // the script is attached to an object and is not passive
			for (int i=0;i<int(obj->childList.size());i++)
				obj->childList[i]->getChildScriptsToRun(childScriptIDsToRun);
		}
		else
		{ // We have a passive script here. We don't do anything for now
		}
	}
	LUA_UNLOCK_NO_INSTANCE_SWITCH;
	// Now we order all scripts according to their execution order settings:
	std::vector<int> firstToExecute;
	std::vector<int> normalToExecute;
	std::vector<int> lastToExecute;
	for (int i=0;i<int(childScriptIDsToRun.size());i++)
	{
		CLuaScriptObject* as=App::ct->luaScriptContainer->getScriptFromID(childScriptIDsToRun[i]);
		if (as!=NULL)
		{
			if (as->getExecutionOrder()==SCRIPT_EXECUTION_ORDER_FIRST)
				firstToExecute.push_back(childScriptIDsToRun[i]);
			if (as->getExecutionOrder()==SCRIPT_EXECUTION_ORDER_NORMAL)
				normalToExecute.push_back(childScriptIDsToRun[i]);
			if (as->getExecutionOrder()==SCRIPT_EXECUTION_ORDER_LAST)
				lastToExecute.push_back(childScriptIDsToRun[i]);
		}
	}
	childScriptIDsToRun.clear();
	childScriptIDsToRun.insert(childScriptIDsToRun.end(),firstToExecute.begin(),firstToExecute.end());
	childScriptIDsToRun.insert(childScriptIDsToRun.end(),normalToExecute.begin(),normalToExecute.end());
	childScriptIDsToRun.insert(childScriptIDsToRun.end(),lastToExecute.begin(),lastToExecute.end());

	// Now we run all the scripts:
	int scriptExecCount=0;
	bool warningOut=false;
	for (int i=0;i<int(childScriptIDsToRun.size());i++)
	{
		CLuaScriptObject* as=App::ct->luaScriptContainer->getScriptFromID(childScriptIDsToRun[i]);
		if (as!=NULL)
		{
			if ( (uniqueChildScriptHandle==sim_handle_all)||(uniqueChildScriptHandle==as->getScriptID())||
				((uniqueChildScriptHandle==sim_handle_all_except_explicit)&&(!as->getExplicitHandling())) )
			{
				if (!as->getExecuteInSensingPhase())
				{
					if (CLuaScriptObject::sensingScriptsInExecution==false)
					{ // regular case
						scriptExecCount++;
						executionResult|=as->runOneStep(inputArguments,outputArguments);
					}
					else
					{ // we are calling this function from a sensing child script. We cannot execute threaded scripts!
						if (as->getThreadedExecution())
						{
							if (!warningOut)
								outputCallErrorMessage(functionName.c_str(),SIM_ERROR_THREAD_LAUNCHING_SCRIPTS_WILL_NOT_BE_CALLED_FROM_SENSING_SECTION,true);
							warningOut=true;
						}
						else
						{ // ok
							scriptExecCount++;
							executionResult|=as->runOneStep(inputArguments,outputArguments);
						}
					}
				}
			}
		}
	}
	int retValSize=0;
	if ( (uniqueChildScriptHandle==sim_handle_all)||(uniqueChildScriptHandle==sim_handle_all_except_explicit) )
	{ // We wanted to run all child scripts, we return the number of executed scripts:
		lua_pushnumber(L,scriptExecCount);
		retValSize=1;
	}
	else
	{ // We just wanted to run one specific child script, so we just return its return values:
		if (scriptExecCount==0)
		{ // Error, the specified child script was not found:
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_CHILD_SCRIPT_HANDLE_OR_CHILD_SCRIPT_NOT_IN_CURRENT_HIERARCHY,true);
			lua_pushnil(L);
			retValSize=1;
		}
		else
		{
			if (outputArguments.getObjectNumber()>0)
			{ // we have the output objects here
				retValSize=outputArguments.getObjectNumber();
				outputArguments.pushObjectsOnStack(L);
			}
			else
			{ // No output data was returned:
				lua_pushnil(L);
				retValSize=1;
			}
		}
	}
	LUA_END_NO_LOCK(retValSize); 
}

int _simHandleSensingChildScripts(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START_NO_LOCK("simHandleSensingChildScripts");
	LUA_LOCK_NO_INSTANCE_SWITCH; // Important to lock for this section (objects might get destroyed in the mean time!)

	int currentScriptID=getCurrentScriptID(L);
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
	
	if (it->getScriptType()!=sim_scripttype_mainscript)
	{
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_MAIN_SCRIPT,true);
		lua_pushnil(L);
		LUA_UNLOCK_NO_INSTANCE_SWITCH;
		LUA_END_NO_LOCK(1); 
	}

	int executionResult=sim_script_no_error;
	std::vector<CLuaScriptObject*> childScriptsToRun;

	// We first identify all sensing child scripts and put them into a list:
	for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
	{
		CLuaScriptObject* as=App::ct->luaScriptContainer->allScripts[i];
		if ( (as->getExecuteInSensingPhase())&&(as->getObjectIDThatScriptIsAttachedTo()!=-1) )
			childScriptsToRun.push_back(as);
	}

	LUA_UNLOCK_NO_INSTANCE_SWITCH;
	// Now we order all scripts according to their execution order settings:
	std::vector<CLuaScriptObject*> firstToExecute;
	std::vector<CLuaScriptObject*> normalToExecute;
	std::vector<CLuaScriptObject*> lastToExecute;
	for (int i=0;i<int(childScriptsToRun.size());i++)
	{
		CLuaScriptObject* as=childScriptsToRun[i];
		if (as->getExecutionOrder()==SCRIPT_EXECUTION_ORDER_FIRST)
			firstToExecute.push_back(as);
		if (as->getExecutionOrder()==SCRIPT_EXECUTION_ORDER_NORMAL)
			normalToExecute.push_back(as);
		if (as->getExecutionOrder()==SCRIPT_EXECUTION_ORDER_LAST)
			lastToExecute.push_back(as);
	}
	childScriptsToRun.clear();
	childScriptsToRun.insert(childScriptsToRun.end(),firstToExecute.begin(),firstToExecute.end());
	childScriptsToRun.insert(childScriptsToRun.end(),normalToExecute.begin(),normalToExecute.end());
	childScriptsToRun.insert(childScriptsToRun.end(),lastToExecute.begin(),lastToExecute.end());

	// Now we run all the scripts:
	CLuaObjectGroup inputArguments;
	CLuaObjectGroup outputArguments;

	int scriptExecCount=0;
	CLuaScriptObject::sensingScriptsInExecution=true; // we know that we will not be interrupted since this has to run unthreaded!
	for (int i=0;i<int(childScriptsToRun.size());i++)
	{
		CLuaScriptObject* as=childScriptsToRun[i];
		scriptExecCount++;
		executionResult|=as->runOneStep(inputArguments,outputArguments);
	}
	CLuaScriptObject::sensingScriptsInExecution=false; // Important not to forget to reset it!
	int retValSize=0;
	// We return the number of executed scripts:
	lua_pushnumber(L,scriptExecCount);
	retValSize=1;
	LUA_END_NO_LOCK(retValSize); 
}

int _simGetScriptName(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetScriptName");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int a=luaToInt(L,1);
		if (a==sim_handle_self)
			a=getCurrentScriptID(L);
		char* name=simGetScriptName_internal(a);
		if (name!=NULL)
		{
			lua_pushstring(L,name);
			simReleaseBuffer_internal(name);
			LUA_END(1);	
		}
	}
	lua_pushnil(L);
	LUA_END(1);	
}

int _simGetScriptExecutionCount(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetScriptExecutionCount");
	int currentScriptID=getCurrentScriptID(L);
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
	lua_pushnumber(L,it->getNumberOfPasses());
	LUA_END(1);	
}

int _simIsScriptExecutionThreaded(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simIsScriptExecutionThreaded");
	int currentScriptID=getCurrentScriptID(L);
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
	int retVal=0;
	if (it->getThreadedExecution())
		retVal=1;
	lua_pushnumber(L,retVal);
	LUA_END(1);	
}

int _simIsScriptRunningInThread(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simIsScriptRunningInThread");
	int retVal=1;
	if (VThread::isCurrentThreadTheMainThread())
		retVal=0;
	lua_pushnumber(L,retVal);
	LUA_END(1);	
}

int _simGetObjectAssociatedWithScript(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectAssociatedWithScript");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int a=luaToInt(L,1);
		if (a==sim_handle_self)
			a=getCurrentScriptID(L);
		retVal=simGetObjectAssociatedWithScript_internal(a);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);	
}

int _simGetScriptAssociatedWithObject(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetScriptAssociatedWithObject");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simGetScriptAssociatedWithObject_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);	
}

int _simOpenModule(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START_NO_LOCK("simOpenModule");
	moduleCommonPart(L,sim_message_eventcallback_moduleopen);
	LUA_END_NO_LOCK(1);	
}
int _simCloseModule(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START_NO_LOCK("simCloseModule");
	moduleCommonPart(L,sim_message_eventcallback_moduleclose);
	LUA_END_NO_LOCK(1);	
}

int _simHandleModule(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START_NO_LOCK("simHandleModule");
	bool sensingPart=false;
	int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_bool,0,true,false,true);
	if ( (res==0)||(res==2) )
	{
		if (res==2)
			sensingPart=(lua_toboolean(L,2)!=0);
		if (sensingPart)
			moduleCommonPart(L,sim_message_eventcallback_modulehandleinsensingpart);
		else
			moduleCommonPart(L,sim_message_eventcallback_modulehandle);
	}
	else
		lua_pushnumber(L,-1);
	LUA_END_NO_LOCK(1);	
}

void moduleCommonPart(lua_State* L,int action)
{
	LUA_API_FUNCTION_DEBUG;
	std::string functionName;
	if (action==sim_message_eventcallback_moduleopen)
		functionName="simOpenModule";
	if (action==sim_message_eventcallback_moduleclose)
		functionName="simCloseModule";
	if ( (action==sim_message_eventcallback_modulehandle)||(action==sim_message_eventcallback_modulehandleinsensingpart) )
		functionName="simHandleModule";
	int currentScriptID=getCurrentScriptID(L);
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
	if (it->getScriptType()!=sim_scripttype_mainscript)
	{
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_MAIN_SCRIPT,true);
		lua_pushnumber(L,-1);
	}
	else
	{
		bool handleAll=false;
		if (lua_isnumber(L,1))
		{ // We try to check whether we have sim_handle_all as a number:
			if (luaToInt(L,1)==sim_handle_all)
			{
				handleAll=true;
				void* retVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(action,NULL,NULL,NULL);
				delete[] ((char*)retVal);
				lua_pushnumber(L,1);
			}
		}
		if (!handleAll)
		{
			if (checkInputArguments(L,functionName,true,lua_arg_string,0))
			{
				std::string modName(lua_tostring(L,1));
				void* retVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(action,NULL,(char*)modName.c_str(),NULL);
				delete[] ((char*)retVal);
				lua_pushnumber(L,1);
			}
			else
				lua_pushnumber(L,-1);
		}
	}
}

int _simBoolOr16(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simBoolOr16");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		int a=luaToInt(L,1);
		int b=luaToInt(L,2);
		if ( (a>=0)&&(b>=0) )
		{
			a&=0xffff;
			b&=0xffff;
			retVal=a|b;
		}
		else
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_NUMBERS,true);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simBoolAnd16(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simBoolAnd16");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		int a=luaToInt(L,1);
		int b=luaToInt(L,2);
		if ( (a>=0)&&(b>=0) )
		{
			a&=0xffff;
			b&=0xffff;
			retVal=a&b;
		}
		else
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_NUMBERS,true);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simBoolXor16(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simBoolXor16");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		int a=luaToInt(L,1);
		int b=luaToInt(L,2);
		if ( (a>=0)&&(b>=0) )
		{
			a&=0xffff;
			b&=0xffff;
			retVal=a^b;
		}
		else
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_NUMBERS,true);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simHandleDynamics(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simHandleDynamics");
	int retVal=-1; // means error

	int currentScriptID=getCurrentScriptID(L);
	CLuaScriptObject* itScrObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
	if ( (itScrObj->getScriptType()==sim_scripttype_mainscript)||(itScrObj->getScriptType()==sim_scripttype_childscript) )
	{
		if (checkInputArguments(L,functionName,true,lua_arg_number,0))
			retVal=simHandleDynamics_internal(luaToFloat(L,1));
	}
	else
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_MAIN_SCRIPT_OR_CHILD_SCRIPT,true);

	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simHandleIkGroup(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simHandleIkGroup");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simHandleIkGroup_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCheckIkGroup(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCheckIkGroup");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		if (lua_istable(L,2))
		{
			int jointCnt=VREP_LUA_OBJLEN(L,2);
			int* handles=new int[jointCnt];
			getIntsFromTable(L,2,jointCnt,handles);
			float* values=new float[jointCnt];
			int retVal=simCheckIkGroup_internal(luaToInt(L,1),jointCnt,handles,values,NULL);
			lua_pushnumber(L,retVal);
			pushFloatTableOntoStack(L,jointCnt,values);
			delete[] values;
			delete[] handles;
			LUA_END(2);
		}
	}
	lua_pushnumber(L,-1);
	LUA_END(1);
}


int _simHandleCollision(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simHandleCollision");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simHandleCollision_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simReadCollision(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simReadCollision");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simReadCollision_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simHandleDistance(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simHandleDistance");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float d;
		retVal=simHandleDistance_internal(luaToInt(L,1),&d);
		if (retVal==1)
		{
			lua_pushnumber(L,retVal);
			lua_pushnumber(L,d);
			LUA_END(2);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simReadDistance(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simReadDistance");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float d;
		retVal=simReadDistance_internal(luaToInt(L,1),&d);
		if (retVal==1)
		{
			lua_pushnumber(L,retVal);
			lua_pushnumber(L,d);
			LUA_END(2);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simHandleProximitySensor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simHandleProximitySensor");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float detPt[4];
		int detectedObjectID;
		float surfaceNormal[3];
		retVal=simHandleProximitySensor_internal(luaToInt(L,1),detPt,&detectedObjectID,surfaceNormal);
		if (retVal==1)
		{
			lua_pushnumber(L,retVal);
			lua_pushnumber(L,detPt[3]);
			pushFloatTableOntoStack(L,3,detPt);
			lua_pushnumber(L,detectedObjectID);
			pushFloatTableOntoStack(L,3,surfaceNormal);
			LUA_END(5);
		}
	}
	// we have an error here:
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simReadProximitySensor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simReadProximitySensor");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float detPt[4];
		int detectedObjectID;
		float surfaceNormal[3];
		retVal=simReadProximitySensor_internal(luaToInt(L,1),detPt,&detectedObjectID,surfaceNormal);
		if (retVal==1)
		{
			lua_pushnumber(L,retVal);
			lua_pushnumber(L,detPt[3]);
			pushFloatTableOntoStack(L,3,detPt);
			lua_pushnumber(L,detectedObjectID);
			pushFloatTableOntoStack(L,3,surfaceNormal);
			LUA_END(5);
		}
	}
	// we have an error here (or no detection/handling):
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simHandleMill(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simHandleMill");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float cutSurfaceAndVolume[2];
		retVal=simHandleMill_internal(luaToInt(L,1),cutSurfaceAndVolume);
		if (retVal!=-1)
		{
			lua_pushnumber(L,retVal);
			pushFloatTableOntoStack(L,2,cutSurfaceAndVolume);
			LUA_END(2);
		}
	}
	// we have an error here:
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simHandleVisionSensor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simHandleVisionSensor");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float* auxVals=NULL;
		int* auxValsCount=NULL;
		retVal=simHandleVisionSensor_internal(luaToInt(L,1),&auxVals,&auxValsCount);
		if ((retVal!=-1)&&(auxValsCount!=NULL))
		{
			int off=0;
			lua_pushnumber(L,retVal);
			int tableCount=auxValsCount[0];
			for (int i=0;i<tableCount;i++)
			{
				pushFloatTableOntoStack(L,auxValsCount[i+1],auxVals+off);
				off+=auxValsCount[i+1];
			}
			delete[] auxValsCount;
			delete[] auxVals;
			LUA_END(1+tableCount);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simReadVisionSensor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simReadVisionSensor");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float* auxVals=NULL;
		int* auxValsCount=NULL;
		retVal=simReadVisionSensor_internal(luaToInt(L,1),&auxVals,&auxValsCount);
		if ((retVal!=-1)&&(auxValsCount!=NULL))
		{
			int off=0;
			lua_pushnumber(L,retVal);
			int tableCount=auxValsCount[0];
			for (int i=0;i<tableCount;i++)
			{
				pushFloatTableOntoStack(L,auxValsCount[i+1],auxVals+off);
				off+=auxValsCount[i+1];
			}
			delete[] auxValsCount;
			delete[] auxVals;
			LUA_END(1+tableCount);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simResetCollision(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simResetCollision");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simResetCollision_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simResetDistance(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simResetDistance");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simResetDistance_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simResetProximitySensor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simResetProximitySensor");
	int retVal=-1; //error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simResetProximitySensor_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simResetMill(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simResetMill");
	int retVal=-1; //error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simResetMill_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simResetVisionSensor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simResetVisionSensor");
	int retVal=-1; //error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simResetVisionSensor_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCheckProximitySensor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCheckProximitySensor");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		float detPt[4];
		retVal=simCheckProximitySensor_internal(luaToInt(L,1),luaToInt(L,2),detPt);
		if (retVal==1)
		{
			lua_pushnumber(L,retVal);
			lua_pushnumber(L,detPt[3]);
			pushFloatTableOntoStack(L,3,detPt);
			LUA_END(3);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCheckProximitySensorEx(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCheckProximitySensorEx");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		float detPt[4];
		int detObj;
		float normVect[3];
		retVal=simCheckProximitySensorEx_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3),luaToFloat(L,4),luaToFloat(L,5),detPt,&detObj,normVect);
		if (retVal==1)
		{
			lua_pushnumber(L,retVal);
			lua_pushnumber(L,detPt[3]);
			pushFloatTableOntoStack(L,3,detPt);
			lua_pushnumber(L,detObj);
			pushFloatTableOntoStack(L,3,normVect);
			LUA_END(5);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCheckProximitySensorEx2(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCheckProximitySensorEx2");
	int retVal=-1; // means error
	
	if (checkOneGeneralInputArgument(L,1,functionName,lua_arg_number,0,false,false,true)!=2)
	{ // first argument (sensor handle)
		lua_pushnumber(L,retVal);
		LUA_END(1);
	}
	int sensorID=luaToInt(L,1);
	if (checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,0,false,false,true)!=2)
	{ // third argument (item type)
		lua_pushnumber(L,retVal);
		LUA_END(1);
	}
	int itemType=luaToInt(L,3);
	if (checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,0,false,false,true)!=2)
	{ // forth argument (item count)
		lua_pushnumber(L,retVal);
		LUA_END(1);
	}
	int itemCount=luaToInt(L,4);
	int requiredValues=itemCount*3*(itemType+1);
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,requiredValues,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int mode=luaToInt(L,5);
		float threshold=luaToFloat(L,6);
		float maxAngle=luaToFloat(L,7);
		float* vertices=new float[requiredValues];
		getFloatsFromTable(L,2,requiredValues,vertices);

		float detPt[4];
		float normVect[3];
		retVal=simCheckProximitySensorEx2_internal(sensorID,vertices,itemType,itemCount,mode,threshold,maxAngle,detPt,normVect);
		delete[] vertices;
		if (retVal==1)
		{
			lua_pushnumber(L,retVal);
			lua_pushnumber(L,detPt[3]);
			pushFloatTableOntoStack(L,3,detPt);
			pushFloatTableOntoStack(L,3,normVect);
			LUA_END(4);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetVisionSensorResolution(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetVisionSensorResolution");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int resolution[2];
		if (simGetVisionSensorResolution_internal(luaToInt(L,1),resolution)==1)
		{
			pushIntTableOntoStack(L,2,resolution);
			LUA_END(1);
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simGetVisionSensorImage(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetVisionSensorImage");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int posX=0;
		int posY=0;
		int sizeX=0;
		int sizeY=0;
		int retType=0;
		// Now check the optional arguments:
		int res;
		res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_number,0,true,false,true);
		if ((res==0)||(res==2))
		{
			if (res==2)
				posX=luaToInt(L,2);
			res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,0,true,false,true);
			if ((res==0)||(res==2))
			{
				if (res==2)
					posY=luaToInt(L,3);
				res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,0,true,false,true);
				if ((res==0)||(res==2))
				{
					if (res==2)
						sizeX=luaToInt(L,4);
					res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,0,true,false,true);
					if ((res==0)||(res==2))
					{
						if (res==2)
							sizeY=luaToInt(L,5);
						res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,0,true,false,true);
						if ((res==0)||(res==2))
						{
							if (res==2)
								retType=luaToInt(L,6);
							CVisionSensor* rs=App::ct->objCont->getVisionSensor(luaToInt(L,1));
							if (rs!=NULL)
							{
								if ( (sizeX==0)&&(sizeY==0) )
								{ // we have default values here (the whole picture)
									int reso[2];
									rs->getRealResolution(reso);
									sizeX=reso[0];
									sizeY=reso[1];
								}
								float* buffer=rs->readPortionOfImage(posX,posY,sizeX,sizeY,true);
								if (buffer!=NULL)
								{
									if (retType==0)
										pushFloatTableOntoStack(L,sizeX*sizeY*3,buffer);
									else
									{ // here we return RGB data in a string
										char* str=new char[sizeX*sizeY*3];
										int vvv=sizeX*sizeY*3;
										for (int i=0;i<vvv;i++)
											str[i]=char(buffer[i]*255.0001f);
										lua_pushlstring(L,(const char*)str,vvv);
										delete[] ((char*)str);
									}
									delete[] ((char*)buffer);
									LUA_END(1);
								}
								else
									outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_ARGUMENTS,true);
							}
							else
								outputCallErrorMessage(functionName.c_str(),SIM_ERROR_VISION_SENSOR_INEXISTANT,true);
						}
					}
				}
			}
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simSetVisionSensorImage(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetVisionSensorImage");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int sensHandle=luaToInt(L,1);
		C3DObject* it=App::ct->objCont->getObject(sensHandle);
		if (it==NULL)
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_OBJECT_INEXISTANT,true);
		else
		{ // Ok we have a valid object
			if (it->getObjectType()!=sim_object_visionsensor_type)
				outputCallErrorMessage(functionName.c_str(),SIM_ERROR_OBJECT_NOT_VISION_SENSOR,true);
			else
			{ // ok we have a valid vision sensor
				int res[2];
				CVisionSensor* rendSens=(CVisionSensor*)it;
				rendSens->getRealResolution(res);
				// We check if we have a table or string at position 2:
				bool notTableNorString=true;
				if (lua_istable(L,2))
				{ // Ok we have a table. Now what size is it?
					notTableNorString=false;
					// Now we check if the provided table has correct size:
					if (int(VREP_LUA_OBJLEN(L,2))<res[0]*res[1]*3)
						outputCallErrorMessage(functionName.c_str(),SIM_ERROR_ONE_TABLE_SIZE_IS_WRONG,true);
					else
					{
						float* img=new float[res[0]*res[1]*3];
						getFloatsFromTable(L,2,res[0]*res[1]*3,img); // we do the operation directly without going through the c-api
						if (rendSens->setExternalImage(img))
							retVal=1;
						delete[] img;
					}
				}
				if (lua_isstring(L,2))
				{ // Ok we have a string. Now what size is it?
					notTableNorString=false;
					// Now we check if the provided string has correct size:
					size_t dataLength;
					char* data=(char*)lua_tolstring(L,2,&dataLength);
					if (int(dataLength)<res[0]*res[1]*3)
						outputCallErrorMessage(functionName.c_str(),SIM_ERROR_ONE_STRING_SIZE_IS_WRONG,true);
					else
					{
						float* img=new float[res[0]*res[1]*3];
						for (int i=0;i<res[0]*res[1]*3;i++)
							img[i]=float(data[i])/255.0f;
						if (rendSens->setExternalImage(img))
							retVal=1;
						delete[] img;
					}
				}
				if (notTableNorString)
					outputCallErrorMessage(functionName.c_str(),SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG,true);
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetVisionSensorDepthBuffer(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetVisionSensorDepthBuffer");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int posX=0;
		int posY=0;
		int sizeX=0;
		int sizeY=0;
		// Now check the optional arguments:
		int res;
		res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_number,0,true,false,true);
		if ((res==0)||(res==2))
		{
			if (res==2)
				posX=luaToInt(L,2);
			res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,0,true,false,true);
			if ((res==0)||(res==2))
			{
				if (res==2)
					posY=luaToInt(L,3);
				res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,0,true,false,true);
				if ((res==0)||(res==2))
				{
					if (res==2)
						sizeX=luaToInt(L,4);
					res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,0,true,false,true);
					if ((res==0)||(res==2))
					{
						if (res==2)
							sizeY=luaToInt(L,5);
						CVisionSensor* rs=App::ct->objCont->getVisionSensor(luaToInt(L,1));
						if (rs!=NULL)
						{
							if ( (sizeX==0)&&(sizeY==0) )
							{ // we have default values here (the whole picture)
								int reso[2];
								rs->getRealResolution(reso);
								sizeX=reso[0];
								sizeY=reso[1];
							}
							float* buffer=rs->readPortionOfImage(posX,posY,sizeX,sizeY,false);
							if (buffer!=NULL)
							{
								pushFloatTableOntoStack(L,sizeX*sizeY,buffer);
								delete[] ((char*)buffer);
								LUA_END(1);
							}
							else
								outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_ARGUMENTS,true);
						}
						else
							outputCallErrorMessage(functionName.c_str(),SIM_ERROR_VISION_SENSOR_INEXISTANT,true);
					}
				}
			}
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simCheckVisionSensor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCheckVisionSensor");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		float* auxVals=NULL;
		int* auxValsCount=NULL;
		retVal=simCheckVisionSensor_internal(luaToInt(L,1),luaToInt(L,2),&auxVals,&auxValsCount);
		if ((retVal!=-1)&&(auxValsCount!=NULL))
		{
			int off=0;
			lua_pushnumber(L,retVal);
			int tableCount=auxValsCount[0];
			for (int i=0;i<tableCount;i++)
			{
				pushFloatTableOntoStack(L,auxValsCount[i+1],auxVals+off);
				off+=auxValsCount[i+1];
			}
			delete[] auxValsCount;
			delete[] auxVals;
			LUA_END(1+tableCount);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCheckVisionSensorEx(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCheckVisionSensorEx");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_bool,0))
	{
		simBool returnImage=luaToBool(L,3);
		float* buffer=simCheckVisionSensorEx_internal(luaToInt(L,1),luaToInt(L,2),returnImage);
		if (buffer!=NULL)
		{
			int res[2];
			simGetVisionSensorResolution_internal(luaToInt(L,1),res);
			if (returnImage)

				pushFloatTableOntoStack(L,res[0]*res[1]*3,buffer);
			else
				pushFloatTableOntoStack(L,res[0]*res[1],buffer);
			simReleaseBuffer_internal((char*)buffer);
			LUA_END(1);
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simGetObjects(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjects");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simGetObjects_internal(luaToInt(L,1),luaToInt(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetObjectHandle(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectHandle");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		std::string name(lua_tostring(L,1));
		if (suffixAdjustStringIfNeeded(functionName,true,L,name))
		{
			CIloIlo::quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
			retVal=simGetObjectHandle_internal(name.c_str());
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetScriptHandle(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetScriptHandle");
	int retVal=-1; // means error
	if (lua_gettop(L)==0) // no arguments
		retVal=getCurrentScriptID(L);
	else
	{
		if (checkInputArguments(L,functionName,false,lua_arg_nil,0)) // we don't output errors here!!
			retVal=getCurrentScriptID(L); // nil argument
		else
		{
			if (checkInputArguments(L,functionName,true,lua_arg_string,0))
			{
				std::string name(lua_tostring(L,1));
				if (suffixAdjustStringIfNeeded(functionName,true,L,name))
				{
					CIloIlo::quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
					retVal=simGetScriptHandle_internal(name.c_str());
				}
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetIkGroupHandle(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetIkGroupHandle");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		std::string name(lua_tostring(L,1));
		if (suffixAdjustStringIfNeeded(functionName,true,L,name))
		{
			CIloIlo::quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
			retVal=simGetIkGroupHandle_internal(name.c_str());
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetCollisionHandle(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetCollisionHandle");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		std::string name(lua_tostring(L,1));
		if (suffixAdjustStringIfNeeded(functionName,true,L,name))
		{
			CIloIlo::quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
			retVal=simGetCollisionHandle_internal(name.c_str());
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetDistanceHandle(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetDistanceHandle");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		std::string name(lua_tostring(L,1));
		if (suffixAdjustStringIfNeeded(functionName,true,L,name))
		{
			CIloIlo::quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
			retVal=simGetDistanceHandle_internal(name.c_str());
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetMechanismHandle(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetMechanismHandle");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		std::string name(lua_tostring(L,1));
		if (suffixAdjustStringIfNeeded(functionName,true,L,name))
		{
			CIloIlo::quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
			retVal=simGetMechanismHandle_internal(name.c_str());
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetPathPlanningHandle(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetPathPlanningHandle");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		std::string name(lua_tostring(L,1));
		if (suffixAdjustStringIfNeeded(functionName,true,L,name))
		{
			CIloIlo::quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
			retVal=simGetPathPlanningHandle_internal(name.c_str());
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetMotionPlanningHandle(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetMotionPlanningHandle");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		std::string name(lua_tostring(L,1));
		if (suffixAdjustStringIfNeeded(functionName,true,L,name))
		{
			CIloIlo::quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
			retVal=simGetMotionPlanningHandle_internal(name.c_str());
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSearchPath(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSearchPath");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		int pathPlanningObjectHandle=luaToInt(L,1);
		float maximumSearchTime=tt::getLimitedFloat(0.001f,36000.0f,luaToFloat(L,2));
		float subDt=0.05f; // 50 ms at a time (default)
		bool foundError=false;
		// Now check the optional argument:
		int res;
		if (!foundError) // sub-dt argument:
		{
			res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,0,true,true,true);
			if (res==2)
			{ // get the data
				subDt=tt::getLimitedFloat(0.001f,SIM_MIN(1.0f,maximumSearchTime),luaToFloat(L,3));
			}
			foundError=(res==-1);
		}
		if (!foundError)
		{
			CPathPlanningTask* it=App::ct->pathPlanning->getObject(pathPlanningObjectHandle);
			if (it==NULL)
				CApiErrors::setLastErrorMessage(functionName.c_str(),SIM_ERROR_PATH_PLANNING_OBJECT_INEXISTANT);
			else
			{
				retVal=0;
				if (VThread::isCurrentThreadTheMainThread())
				{ // non-threaded
					if (it->performSearch(false,maximumSearchTime))
						retVal=1;
				}
				else
				{ // threaded call:
					CPathPlanningTask* oldIt=it;
					it=oldIt->copyYourself(); // we copy it because the original might be destroyed at any time
					it->setOriginalTask(oldIt);
					retVal=-1; // for error
					if (it->initiateSteppedSearch(false,maximumSearchTime,subDt))
					{
						retVal=-2; // means search not yet finished
						while (retVal==-2)
						{
							retVal=it->performSteppedSearch();
							if (retVal==-2)
							{ // we are not yet finished with the search!
								LUA_UNLOCK; // Important!!
								CThreadPool::switchBackToPreviousThread();
								if (CThreadPool::getStopForCurrentInstanceRequested())
								{
									LUA_LOCK; // Important!!
									retVal=-1; // generate an error
									break; // will generate an error (retVal is -1)
								}
								LUA_LOCK; // Important!!
							}
						}
						if (retVal==-1)
						{
							CApiErrors::setLastErrorMessage(functionName.c_str(),SIM_ERROR_PATH_PLANNING_OBJECT_NOT_CONSISTENT_ANYMORE);
							retVal=-1; // for error
						}
						// Return values are -1 (error), 0 (no path found) 1 (partial path found) and 2 (full path found)
					}
					else
					{ // the task is not consistent!
						CApiErrors::setLastErrorMessage(functionName.c_str(),SIM_ERROR_PATH_PLANNING_OBJECT_NOT_CONSISTENT);
					}
					CPathPlanningTask* originalIt=it->getOriginalTask();
					int tree1Handle,tree2Handle;
					it->getAndDisconnectSearchTrees(tree1Handle,tree2Handle); // to keep trees visible!
					delete it;
					// Now we connect the trees only if the originalTask still exists:
					bool found=false;
					for (int ot=0;ot<int(App::ct->pathPlanning->allObjects.size());ot++)
					{
						if (App::ct->pathPlanning->allObjects[ot]==originalIt)
						{
							found=true;
							break;
						}
					}
					if (found)
						originalIt->connectExternalSearchTrees(tree1Handle,tree2Handle);
					else
					{
						App::ct->drawingCont->removeObject(tree1Handle);
						App::ct->drawingCont->removeObject(tree2Handle);
					}
				}
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simInitializePathSearch(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simInitializePathSearch");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int pathPlanningObjectHandle=luaToInt(L,1);
		float maximumSearchTime=luaToFloat(L,2);
		float searchTimeStep=luaToFloat(L,3);
		retVal=simInitializePathSearch_internal(pathPlanningObjectHandle,maximumSearchTime,searchTimeStep);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simPerformPathSearchStep(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simPerformPathSearchStep");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_bool,0))
	{
		int temporaryPathSearchObjectHandle=luaToInt(L,1);
		bool abortSearch=luaToBool(L,2);
		retVal=simPerformPathSearchStep_internal(temporaryPathSearchObjectHandle,abortSearch);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetCollectionHandle(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetCollectionHandle");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		std::string name(lua_tostring(L,1));
		if (suffixAdjustStringIfNeeded(functionName,true,L,name))
		{
			CIloIlo::quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
			retVal=simGetCollectionHandle_internal(name.c_str());
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simRemoveCollection(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simRemoveCollection");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simRemoveCollection_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetObjectPosition(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectPosition");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		float coord[3];
		if (simGetObjectPosition_internal(luaToInt(L,1),luaToInt(L,2),coord)==1)
			pushFloatTableOntoStack(L,3,coord);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simGetObjectOrientation(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectOrientation");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		float coord[3];
		if (simGetObjectOrientation_internal(luaToInt(L,1),luaToInt(L,2),coord)==1)
			pushFloatTableOntoStack(L,3,coord);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simSetObjectPosition(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetObjectPosition");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,3))
	{
		float coord[3];
		getFloatsFromTable(L,3,3,coord);
		retVal=simSetObjectPosition_internal(luaToInt(L,1),luaToInt(L,2),coord);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetObjectOrientation(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetObjectOrientation");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,3))
	{
		float coord[3];
		getFloatsFromTable(L,3,3,coord);
		retVal=simSetObjectOrientation_internal(luaToInt(L,1),luaToInt(L,2),coord);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetJointPosition(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetJointPosition");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float jointVal[1];
		if (simGetJointPosition_internal(luaToInt(L,1),jointVal)!=-1)
			lua_pushnumber(L,jointVal[0]);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simSetJointPosition(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetJointPosition");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetJointPosition_internal(luaToInt(L,1),luaToFloat(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetJointTargetPosition(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetJointTargetPosition");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetJointTargetPosition_internal(luaToInt(L,1),luaToFloat(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetJointTargetPosition(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetJointTargetPosition");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float targetPos;
		if (simGetJointTargetPosition_internal(luaToInt(L,1),&targetPos)!=-1)
		{
			lua_pushnumber(L,targetPos);
			LUA_END(1);
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simSetJointForce(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetJointForce");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetJointForce_internal(luaToInt(L,1),luaToFloat(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetPathPosition(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetPathPosition");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float pathVal[1];
		if (simGetPathPosition_internal(luaToInt(L,1),pathVal)!=-1)
			lua_pushnumber(L,pathVal[0]);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simSetPathPosition(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetPathPosition");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetPathPosition_internal(luaToInt(L,1),luaToFloat(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetPathLength(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetPathLength");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float pathLen[1];
		if (simGetPathLength_internal(luaToInt(L,1),pathLen)!=-1)
			lua_pushnumber(L,pathLen[0]);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simSetJointTargetVelocity(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetJointTargetVelocity");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetJointTargetVelocity_internal(luaToInt(L,1),luaToFloat(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetJointTargetVelocity(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetJointTargetVelocity");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float targetVel;
		if (simGetJointTargetVelocity_internal(luaToInt(L,1),&targetVel)!=-1)
		{
			lua_pushnumber(L,targetVel);
			LUA_END(1);
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simSetPathTargetNominalVelocity(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetPathTargetNominalVelocity");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetPathTargetNominalVelocity_internal(luaToInt(L,1),luaToFloat(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simRefreshDialogs(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simRefreshDialogs");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simRefreshDialogs_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetUIHandle(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetUIHandle");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		std::string name(lua_tostring(L,1));
		if (suffixAdjustStringIfNeeded(functionName,true,L,name))
		{
			CIloIlo::quicklyDisableAndAutomaticallyReenableCNameSuffixAdjustment();
			retVal=simGetUIHandle_internal(name.c_str());
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetUIProperty(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetUIProperty");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simGetUIProperty_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetUIEventButton(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetUIEventButton");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int auxVals[2];
		retVal=simGetUIEventButton_internal(luaToInt(L,1),auxVals);
		lua_pushnumber(L,retVal);
		if (retVal==-1)
		{
			LUA_END(1);
		}
		pushIntTableOntoStack(L,2,auxVals);
		LUA_END(2);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetUIProperty(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetUIProperty");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetUIProperty_internal(luaToInt(L,1),luaToInt(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetUIButtonSize(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetUIButtonSize");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		int size[2];
		if (simGetUIButtonSize_internal(luaToInt(L,1),luaToInt(L,2),size)==1)
			pushIntTableOntoStack(L,2,size);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simGetUIButtonProperty(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetUIButtonProperty");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simGetUIButtonProperty_internal(luaToInt(L,1),luaToInt(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetUIButtonProperty(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetUIButtonProperty");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetUIButtonProperty_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetUISlider(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetUISlider");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simGetUISlider_internal(luaToInt(L,1),luaToInt(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetUISlider(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetUISlider");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetUISlider_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetUIButtonLabel(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetUIButtonLabel");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		std::string stra;
		std::string strb;
		char* str1=NULL;
		char* str2=NULL;
		int res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_string,0,false,true,true);
		if (res!=-1)
		{
			if (res==2)
			{
				stra=lua_tostring(L,3);
				str1=(char*)stra.c_str();
			}
			res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_string,0,true,true,true);
			if (res!=-1)
			{
				if (res==2)
				{
					strb=lua_tostring(L,4);
					str2=(char*)strb.c_str();
				}
				retVal=simSetUIButtonLabel_internal(luaToInt(L,1),luaToInt(L,2),str1,str2);
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetUIButtonLabel(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetUIButtonLabel");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		char* label=simGetUIButtonLabel_internal(luaToInt(L,1),luaToInt(L,2));
		if (label==NULL)
			lua_pushnil(L);
		else
		{
			lua_pushstring(L,label);
			simReleaseBuffer_internal(label);
		}
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simGetObjectName(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectName");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		char* name=simGetObjectName_internal(luaToInt(L,1));
		if (name==NULL)
			lua_pushnil(L);
		else
		{
			lua_pushstring(L,name);
			simReleaseBuffer_internal(name);
		}
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simGetCollectionName(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetCollectionName");
	//	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		char* name=simGetCollectionName_internal(luaToInt(L,1));
		if (name==NULL)
			lua_pushnil(L);
		else
		{
			lua_pushstring(L,name);
			simReleaseBuffer_internal(name);
		}
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simGetModuleName(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetModuleName");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		unsigned char version;
		char* name=simGetModuleName_internal(luaToInt(L,1),&version);
		if (name==NULL)
			lua_pushnil(L);
		else
		{
			lua_pushstring(L,name);
			simReleaseBuffer_internal(name);
			lua_pushnumber(L,version);
			LUA_END(2);
		}
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simSetUIButtonColor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetUIButtonColor");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		float col[9];
		float* acp[3]={col,col+3,col+6};
//		bool failed=true;
		int res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,3,false,true,true);
		if (res!=-1)
		{
			if (res!=2)
				acp[0]=NULL;
			res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,3,true,true,true);
			if (res!=-1)
			{
				if (res!=2)
					acp[1]=NULL;
				res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,3,true,true,true);
				if (res!=-1)
				{
					if (res!=2)
						acp[2]=NULL;
					for (int i=0;i<3;i++)
					{
						if (acp[i]!=NULL)
							getFloatsFromTable(L,3+i,3,acp[i]);
					}
					retVal=simSetUIButtonColor_internal(luaToInt(L,1),luaToInt(L,2),acp[0],acp[1],acp[2]);
				}
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetSimulationTime(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetSimulationTime");
	float theTime=simGetSimulationTime_internal();
	if (theTime>=0.0f)
	{
		lua_pushnumber(L,theTime);
		LUA_END(1);
	}
	lua_pushnumber(L,-1);
	LUA_END(1);
}

int _simGetSimulationState(lua_State* L)
{
	// In case we copy-paste a script during execution, the new script doesn't get the sim_simulation_starting message,
	// but that is ok!!! sim_simulation_starting is only for a simulation start. For a first run in a script, use some
	// random variable and check whether it is != from nil!! or use simGetScriptExecutionCount
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetSimulationState");
	int retVal=simGetSimulationState_internal();
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetSystemTime(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetSystemTime");
	lua_pushnumber(L,simGetSystemTime_internal());
	LUA_END(1);
}

int _simGetSystemTimeInMs(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetSystemTimeInMs");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int lastTime=luaToInt(L,1);
		lua_pushnumber(L,simGetSystemTimeInMs_internal(lastTime));
		LUA_END(1);
	}
	LUA_END(0);
}

int _simCheckCollision(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCheckCollision");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simCheckCollision_internal(luaToInt(L,1),luaToInt(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCheckCollisionEx(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCheckCollisionEx");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		float* intersections[1];
		retVal=simCheckCollisionEx_internal(luaToInt(L,1),luaToInt(L,2),intersections);
		if (retVal>0)
		{
			lua_pushnumber(L,retVal);
			pushFloatTableOntoStack(L,retVal*6,(*intersections));
			simReleaseBuffer_internal((char*)(*intersections));
			LUA_END(2);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCheckDistance(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCheckDistance");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		float distanceData[7];
		retVal=simCheckDistance_internal(luaToInt(L,1),luaToInt(L,2),luaToFloat(L,3),distanceData);
		if (retVal==1)
		{
			lua_pushnumber(L,1);
			pushFloatTableOntoStack(L,7,distanceData);
			LUA_END(2);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetObjectConfiguration(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectConfiguration");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		char* data=simGetObjectConfiguration_internal(luaToInt(L,1));
		if (data==NULL)
			lua_pushnumber(L,-1);
		else
		{
			CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getCurrentScriptID(L));
			lua_pushnumber(L,it->setUserData(data));
		}
	}
	else
		lua_pushnumber(L,-1);
	LUA_END(1);
}

int _simSetObjectConfiguration(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetObjectConfiguration");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getCurrentScriptID(L));
		char* data=(char*)it->getUserData(lua_tointeger(L,1));
		if (data==NULL)
		{
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_HANDLE,true);
			lua_pushnumber(L,-1);
		}
		else
			lua_pushnumber(L,simSetObjectConfiguration_internal(data));
	}
	else
		lua_pushnumber(L,-1);
	LUA_END(1);
}

int _simGetConfigurationTree(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetConfigurationTree");
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getCurrentScriptID(L));
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int id=luaToInt(L,1);
		if (id==sim_handle_self)
		{
			int objID=it->getObjectIDThatScriptIsAttachedTo();
			id=objID;
			if (id==-1)
				outputCallErrorMessage(functionName.c_str(),SIM_ERROR_ARGUMENT_VALID_ONLY_WITH_CHILD_SCRIPTS,true);
		}
		if (id!=-1)
		{
			char* data=simGetConfigurationTree_internal(id);
			if (data==NULL)
				lua_pushnumber(L,-1);
			else
				lua_pushnumber(L,it->setUserData(data));
		}
		else
			lua_pushnumber(L,-1);
	}
	else
		lua_pushnumber(L,-1);
	LUA_END(1);
}

int _simSetConfigurationTree(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetConfigurationTree");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getCurrentScriptID(L));
		char* data=(char*)it->getUserData(lua_tointeger(L,1));
		if (data==NULL)
		{
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_HANDLE,true);
			lua_pushnumber(L,-1);
		}
		else
			lua_pushnumber(L,simSetConfigurationTree_internal(data));
	}
	else
		lua_pushnumber(L,-1);
	LUA_END(1);
}

int _simHandleMechanism(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simHandleMechanism");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simHandleMechanism_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetLastError(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetLastError");
	char* it=simGetLastError_internal();
	if (it==NULL)
		lua_pushnil(L);
	else
	{
		lua_pushstring(L,it);
		simReleaseBuffer_internal(it);
	}
	LUA_END(1);
}

int _simGetSimulationTimeStep(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetSimulationTimeStep");
	lua_pushnumber(L,simGetSimulationTimeStep_internal());
	LUA_END(1);
}

int _simGetSimulatorMessage(lua_State* L)
{ // Careful!! This command does not map its corresponding C-API command!! (different message pipeline)
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetSimulatorMessage");
	int auxVals[4];
	float aux2Vals[8];
	int aux2Cnt;
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getCurrentScriptID(L));
	int commandID=it->extractCommandFromOutsideCommandQueue(auxVals,aux2Vals,aux2Cnt);
	if (commandID==-1)
	{
		lua_pushnumber(L,-1);
		LUA_END(1);
	}
	else
	{
		lua_pushnumber(L,commandID);
		pushIntTableOntoStack(L,4,auxVals);
		if (aux2Cnt!=0)
		{
			pushFloatTableOntoStack(L,aux2Cnt,aux2Vals);
			LUA_END(3);
		}
		LUA_END(2);
	}
}

int _simResetPath(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simResetPath");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simResetPath_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simHandlePath(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simHandlePath");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simHandlePath_internal(luaToInt(L,1),luaToFloat(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simResetJoint(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simResetJoint");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simResetJoint_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simHandleJoint(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simHandleJoint");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simHandleJoint_internal(luaToInt(L,1),luaToFloat(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simResetGraph(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simResetGraph");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simResetGraph_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simHandleGraph(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simHandleGraph");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simHandleGraph_internal(luaToInt(L,1),luaToFloat(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simAddStatusbarMessage(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAddStatusbarMessage");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
		retVal=simAddStatusbarMessage_internal(lua_tostring(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetScriptSimulationParameter(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetScriptSimulationParameter");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0))
	{
		bool returnString=false;
		int ret=checkOneGeneralInputArgument(L,3,functionName,lua_arg_bool,0,true,false,true);
		if ((ret==0)||(ret==2))
		{
			if (ret==2)
				returnString=luaToBool(L,3);
			int handle=lua_tointeger(L,1);
			if (handle==sim_handle_self)
				handle=getCurrentScriptID(L);
			std::string parameterName(lua_tostring(L,2));
			if ( (handle!=sim_handle_tree)&&(handle!=sim_handle_chain)&&(handle!=sim_handle_all) )
			{
				int l;
				char* p=simGetScriptSimulationParameter_internal(handle,parameterName.c_str(),&l);
				if (p==NULL)
					lua_pushnil(L);
				else
				{
					std::string a;
					a.assign(p,l);
					if (returnString)
						lua_pushlstring(L,a.c_str(),a.length());
					else
						pushCorrectTypeOntoLuaStack(L,a);
				}
				simReleaseBuffer_internal(p);
			}
			else
			{
				std::vector<int> scriptHandles;
				if (handle==sim_handle_tree)
					getScriptTree(L,false,scriptHandles);
				if (handle==sim_handle_chain)
					getScriptChain(L,false,false,scriptHandles);
				if (handle==sim_handle_all)
				{
					for (int i=0;i<int(App::ct->luaScriptContainer->allScripts.size());i++)
					{
						CLuaScriptObject* it=App::ct->luaScriptContainer->allScripts[i];
						int scrType=it->getScriptType();
						if ((scrType==sim_scripttype_mainscript)||(scrType==sim_scripttype_childscript)) // make sure plugin script etc. are not included!
							scriptHandles.push_back(it->getScriptID());
					}
				}
				std::vector<std::string> retParams;
				std::vector<int> retHandles;
				for (int i=0;i<int(scriptHandles.size());i++)
				{
					int l;
					char* p=simGetScriptSimulationParameter_internal(scriptHandles[i],parameterName.c_str(),&l);
					if (p!=NULL)
					{
						std::string a;
						a.assign(p,l);
						simReleaseBuffer_internal(p);
						retParams.push_back(a);
						retHandles.push_back(scriptHandles[i]);
					}
				}
				if (retParams.size()==0)
					lua_pushnil(L);
				else
				{ // now we push two tables onto the stack:
					CLuaTableObject params;
					for (int i=0;i<int(retParams.size());i++)
					{
						int t=getCorrectType(retParams[i]);
						if (returnString)
							t=4; // we force for strings!
						CLuaObject* anObj=NULL;
						if (t==0)
							anObj=new CLuaObject();
						if (t==1)
							anObj=new CLuaObject(false);
						if (t==2)
							anObj=new CLuaObject(true);
						if (t==3)
						{
							float v;
							tt::getValidFloat(retParams[i],v);
							anObj=new CLuaObject(v);
						}
						if (t==4)
							anObj=new CLuaObject(retParams[i]);
						params.addElement(anObj);
					}
					params.pushObjectOnStack(L);
					pushIntTableOntoStack(L,retHandles.size(),&retHandles[0]);
					LUA_END(2);
				}
			}
		}
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simSetScriptSimulationParameter(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetScriptSimulationParameter");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0,lua_arg_string,0))
	{
		int handle=lua_tointeger(L,1);
		if (handle==sim_handle_self)
			handle=getCurrentScriptID(L);
		std::string parameterName(lua_tostring(L,2));
		size_t parameterValueLength;
		char* parameterValue=(char*)lua_tolstring(L,3,&parameterValueLength);
		if ( (handle!=sim_handle_tree)&&(handle!=sim_handle_chain) )
		{
			retVal=simSetScriptSimulationParameter_internal(handle,parameterName.c_str(),parameterValue,parameterValueLength);
		}
		else
		{
			std::vector<int> scriptHandles;
			if (handle==sim_handle_tree)
				getScriptTree(L,false,scriptHandles);
			else
				getScriptChain(L,false,false,scriptHandles);
			retVal=0;
			for (int i=0;i<int(scriptHandles.size());i++)
			{
				if (simSetScriptSimulationParameter_internal(scriptHandles[i],parameterName.c_str(),parameterValue,parameterValueLength)==1)
					retVal++;
			}
		}
	}

	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simDisplayDialog(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simDisplayDialog");
	int retVal=-1;// error
	int elementHandle;
	if (checkInputArguments(L,functionName,true,lua_arg_string,0,lua_arg_string,0,lua_arg_number,0,lua_arg_bool,0))
	{
		char* initialText=NULL;
		float* titleColor=NULL;
		float* dialogColor=NULL;
		bool modal=luaToBool(L,4);
		int dialogType=luaToInt(L,3);
		bool errorOccured=false;
		if (modal)
		{
			int currentScriptID=getCurrentScriptID(L);
			CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
			if (it!=NULL)
			{
				if (VThread::isCurrentThreadTheMainThread())
				{
					outputCallErrorMessage(functionName.c_str(),SIM_ERROR_SCRIPT_MUST_RUN_IN_THREAD_FOR_MODAL_OPERATION,true);
					errorOccured=true;
				}
				else
				{
					if (dialogType==sim_dlgstyle_message)
					{
						outputCallErrorMessage(functionName.c_str(),SIM_ERROR_CANNOT_USE_THAT_STYLE_IN_MODAL_OPERATION,true);
						errorOccured=true;
					}
				}
			}
			else
				modal=false; // Should anyway never happen!!!
		}
		if (!errorOccured)
		{
			int res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_string,0,true,true,true);
			if (res!=-1)
			{
				if (res==2)
				{
					std::string tmp(lua_tostring(L,5));
					initialText=new char[tmp.length()+1];
					initialText[tmp.length()]=0;
					for (int i=0;i<int(tmp.length());i++)
						initialText[i]=tmp[i];
				}
				int res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,6,true,true,true);
				if (res!=-1)
				{
					if (res==2)
					{
						titleColor=new float[6];
						getFloatsFromTable(L,6,6,titleColor);
					}
					int res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,6,true,true,true);
					if (res!=-1)
					{
						if (res==2)
						{
							dialogColor=new float[6];
							getFloatsFromTable(L,7,6,dialogColor);
						}
						retVal=simDisplayDialog_internal(lua_tostring(L,1),lua_tostring(L,2),dialogType,initialText,titleColor,dialogColor,&elementHandle);
						if (retVal!=-1)
						{
							CGenericDialog* it=App::ct->genericDialogContainer->getDialogFromID(retVal);
							if (it!=NULL)
							{
								int currentScriptID=getCurrentScriptID(L);
								CLuaScriptObject* itScrObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
								if ( (itScrObj->getScriptType()==sim_scripttype_mainscript)||(itScrObj->getScriptType()==sim_scripttype_childscript) )
								{
									it->setCreatedInMainOrChildScript(true); // this will trigger automatic destruction at simulation end
									it->setPauseActive(false); // that dlg should be inactive during pause
								}
								it->setModal(modal);
								if (modal)
								{
									LUA_UNLOCK; // Important!!
									// Now wait here until a button was pressed! (or the simulation is aborted)
									while (it->getDialogResult()==sim_dlgret_still_open)
									{
										CThreadPool::switchBackToPreviousThread();
										if (CThreadPool::getStopForCurrentInstanceRequested())
											break;
									}
									LUA_LOCK; // Important!!
								}
							}
						}
					}
				}
			}			
		}
		delete[] initialText;
		delete[] titleColor;
		delete[] dialogColor;
	}
	if (retVal!=-1)
	{
		lua_pushnumber(L,retVal);
		lua_pushnumber(L,elementHandle);
		LUA_END(2);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetDialogResult(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetDialogResult");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simGetDialogResult_internal(lua_tointeger(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetDialogInput(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetDialogInput");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		char* v=simGetDialogInput_internal(lua_tointeger(L,1));
		if (v!=NULL)
		{
			lua_pushstring(L,v);
			simReleaseBuffer_internal(v);
			LUA_END(1);
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simEndDialog(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simEndDialog");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simEndDialog_internal(lua_tointeger(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simStopSimulation(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simStopSimulation");
	int retVal=-1;// error
	retVal=simStopSimulation_internal();
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simPauseSimulation(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simPauseSimulation");
	int retVal=-1;// error
	retVal=simPauseSimulation_internal();
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simStartSimulation(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simStartSimulation");
	int retVal=-1;// error
	retVal=simStartSimulation_internal();
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetObjectMatrix(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectMatrix");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		float arr[12];
		if (simGetObjectMatrix_internal(lua_tointeger(L,1),lua_tointeger(L,2),arr)==1)
			pushFloatTableOntoStack(L,12,arr); // Success
		else
			lua_pushnil(L); // error
	}
	else
		lua_pushnil(L); // error
	LUA_END(1);
}

int _simSetObjectMatrix(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetObjectMatrix");
	int retVal=-1; // error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,12))
	{
		float arr[12];
		getFloatsFromTable(L,3,12,arr);
		retVal=simSetObjectMatrix_internal(lua_tointeger(L,1),lua_tointeger(L,2),arr);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetJointMatrix(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetJointMatrix");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float arr[12];
		if (simGetJointMatrix_internal(lua_tointeger(L,1),arr)==1)
			pushFloatTableOntoStack(L,12,arr); // Success
		else
			lua_pushnil(L); // error
	}
	else
		lua_pushnil(L); // error
	LUA_END(1);
}

int _simSetSphericalJointMatrix(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetSphericalJointMatrix");
	int retVal=-1; // error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,12))
	{
		float arr[12];
		getFloatsFromTable(L,2,12,arr);
		retVal=simSetSphericalJointMatrix_internal(lua_tointeger(L,1),arr);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simBuildIdentityMatrix(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simBuildIdentityMatrix");
	float arr[12];
	simBuildIdentityMatrix_internal(arr);
	pushFloatTableOntoStack(L,12,arr);
	LUA_END(1);
}

int _simCopyMatrix(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCopyMatrix");
	if (checkInputArguments(L,functionName,true,lua_arg_number,12))
	{
		float arr[12];
		getFloatsFromTable(L,1,12,arr);
		pushFloatTableOntoStack(L,12,arr);
	}
	else
		lua_pushnil(L); // error
	LUA_END(1);
}

int _simBuildMatrix(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simBuildMatrix");
	if (checkInputArguments(L,functionName,true,lua_arg_number,3,lua_arg_number,3))
	{
		float arr[12];
		float pos[3];
		float euler[3];
		getFloatsFromTable(L,1,3,pos);
		getFloatsFromTable(L,2,3,euler);
		if (simBuildMatrix_internal(pos,euler,arr)==1)
			pushFloatTableOntoStack(L,12,arr);
		else
			lua_pushnil(L); // error
	}
	else
		lua_pushnil(L); // error
	LUA_END(1);
}

int _simGetEulerAnglesFromMatrix(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetEulerAnglesFromMatrix");
	if (checkInputArguments(L,functionName,true,lua_arg_number,12))
	{
		float arr[12];
		float euler[3];
		getFloatsFromTable(L,1,12,arr);
		if (simGetEulerAnglesFromMatrix_internal(arr,euler)==1)
			pushFloatTableOntoStack(L,3,euler);
		else
			lua_pushnil(L); // error
	}
	else
		lua_pushnil(L); // error
	LUA_END(1);
}

int _simGetInvertedMatrix(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetInvertedMatrix");
	if (checkInputArguments(L,functionName,true,lua_arg_number,12))
	{
		float arr[12];
		getFloatsFromTable(L,1,12,arr);
		simInvertMatrix_internal(arr);
		pushFloatTableOntoStack(L,12,arr);
	}
	else
		lua_pushnil(L); // error
	LUA_END(1);
}

int _simMultiplyMatrices(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simMultiplyMatrices");
	if (checkInputArguments(L,functionName,true,lua_arg_number,12,lua_arg_number,12))
	{
		float inM0[12];
		float inM1[12];
		float outM[12];
		getFloatsFromTable(L,1,12,inM0);
		getFloatsFromTable(L,2,12,inM1);
		if (simMultiplyMatrices_internal(inM0,inM1,outM)!=-1)
			pushFloatTableOntoStack(L,12,outM);
		else
			lua_pushnil(L); // error
	}
	else
		lua_pushnil(L); // error
	LUA_END(1);
}

int _simInterpolateMatrices(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simInterpolateMatrices");
	if (checkInputArguments(L,functionName,true,lua_arg_number,12,lua_arg_number,12,lua_arg_number,0))
	{
		float inM0[12];
		float inM1[12];
		float outM[12];
		getFloatsFromTable(L,1,12,inM0);
		getFloatsFromTable(L,2,12,inM1);
		if (simInterpolateMatrices_internal(inM0,inM1,luaToFloat(L,3),outM)!=-1)
			pushFloatTableOntoStack(L,12,outM);
		else
			lua_pushnil(L); // error
	}
	else
		lua_pushnil(L); // error
	LUA_END(1);
}

int _simMultiplyVector(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simMultiplyVector");
	if (checkInputArguments(L,functionName,true,lua_arg_number,12,lua_arg_number,3))
	{
		float m[12];
		float vect[3];
		getFloatsFromTable(L,1,12,m);
		getFloatsFromTable(L,2,3,vect);
		if (simTransformVector_internal(m,vect)!=-1)
			pushFloatTableOntoStack(L,3,vect);
		else
			lua_pushnil(L); // error
	}
	else
		lua_pushnil(L); // error
	LUA_END(1);
}

int _simGetObjectParent(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectParent");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simGetObjectParent_internal(lua_tointeger(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetObjectChild(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectChild");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simGetObjectChild_internal(lua_tointeger(L,1),lua_tointeger(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetObjectParent(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetObjectParent");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_bool,0))
		retVal=simSetObjectParent_internal(lua_tointeger(L,1),lua_tointeger(L,2),lua_toboolean(L,3));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetObjectType(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectType");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simGetObjectType_internal(lua_tointeger(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetJointType(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetJointType");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simGetJointType_internal(lua_tointeger(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetBooleanParameter(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetBooleanParameter");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_bool,0))
		retVal=simSetBooleanParameter_internal(lua_tointeger(L,1),lua_toboolean(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetBooleanParameter(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetBooleanParameter");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int retVal=simGetBooleanParameter_internal(lua_tointeger(L,1));
		if (retVal!=-1)
			lua_pushboolean(L,retVal!=0);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simSetIntegerParameter(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetIntegerParameter");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetIntegerParameter_internal(lua_tointeger(L,1),lua_tointeger(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetIntegerParameter(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetIntegerParameter");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int v;
		int retVal=simGetIntegerParameter_internal(lua_tointeger(L,1),&v);
		if (retVal!=-1)
			lua_pushnumber(L,v);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simSetFloatingParameter(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetFloatingParameter");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetFloatingParameter_internal(lua_tointeger(L,1),luaToFloat(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetFloatingParameter(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetFloatingParameter");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float v;
		int retVal=simGetFloatingParameter_internal(lua_tointeger(L,1),&v);
		if (retVal!=-1)
			lua_pushnumber(L,v);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simSetStringParameter(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetStringParameter");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0))
		retVal=simSetStringParameter_internal(luaToInt(L,1),lua_tostring(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetStringParameter(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetStringParameter");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		char* s=simGetStringParameter_internal(lua_tointeger(L,1));
		if (s!=NULL)
		{
			lua_pushstring(L,s);
			delete[] s;
		}
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simSetArrayParameter(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetArrayParameter");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_table,0))
	{
		int parameter=lua_tointeger(L,1);
		if (true)
		{ // for now all array parameters are tables of 3 floats
			float theArray[3];
			getFloatsFromTable(L,2,3,theArray);
			retVal=simSetArrayParameter_internal(parameter,theArray);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetArrayParameter(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetArrayParameter");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int parameter=lua_tointeger(L,1);
		if (true)
		{ // for now all parameters are tables of 3 floats
			float theArray[3];
			int retVal=simGetArrayParameter_internal(parameter,theArray);
			if (retVal!=-1)
			{
				pushFloatTableOntoStack(L,3,theArray);
			}
			else
				lua_pushnil(L);
		}
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simCreateUIButtonArray(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCreateUIButtonArray");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simCreateUIButtonArray_internal(lua_tointeger(L,1),lua_tointeger(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetUIButtonArrayColor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetUIButtonArrayColor");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,2,lua_arg_number,3))
	{
		int pos[2];
		float col[3];
		getIntsFromTable(L,3,2,pos);
		getFloatsFromTable(L,4,3,col);
		retVal=simSetUIButtonArrayColor_internal(lua_tointeger(L,1),lua_tointeger(L,2),pos,col);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simDeleteUIButtonArray(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simDeleteUIButtonArray");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simDeleteUIButtonArray_internal(lua_tointeger(L,1),lua_tointeger(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simRemoveObject(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simRemoveObject");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simRemoveObject_internal(lua_tointeger(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetObjectName(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetObjectName");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0))
		retVal=simSetObjectName_internal(lua_tointeger(L,1),lua_tostring(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetCollectionName(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetCollectionName");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0))
		retVal=simSetCollectionName_internal(lua_tointeger(L,1),lua_tostring(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetJointInterval(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetJointInterval");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		simBool cyclic;
		simFloat interval[2];
		if (simGetJointInterval_internal(lua_tointeger(L,1),&cyclic,interval)==1)
		{
			lua_pushboolean(L,cyclic!=0);
			pushFloatTableOntoStack(L,2,interval);
			LUA_END(2);
		}
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simSetJointInterval(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetJointInterval");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_bool,0,lua_arg_number,2))
	{
		float interval[2];
		getFloatsFromTable(L,3,2,interval);
		retVal=simSetJointInterval_internal(lua_tointeger(L,1),lua_toboolean(L,2),interval);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simLoadScene(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simLoadScene");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
		retVal=simLoadScene_internal(lua_tostring(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSaveScene(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSaveScene");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
		retVal=simSaveScene_internal(lua_tostring(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simLoadModel(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simLoadModel");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
		retVal=simLoadModel_internal(lua_tostring(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSaveModel(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSaveModel");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0))
		retVal=simSaveModel_internal(lua_tointeger(L,1),lua_tostring(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSaveUI(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSaveUI");
	int retVal=-1;// error

	int res=checkOneGeneralInputArgument(L,1,functionName,lua_arg_table,1,false,true,true);
	if (res>0)
	{
		int res2=checkOneGeneralInputArgument(L,2,functionName,lua_arg_string,0,false,false,true);
		if (res2==2)
		{
			if (res==1)
				retVal=simSaveUI_internal(0,NULL,lua_tostring(L,2));
			else
			{
                int tl=int(VREP_LUA_OBJLEN(L,1));
				int* tble=new int[tl];
				getIntsFromTable(L,1,tl,tble);
				retVal=simSaveUI_internal(tl,tble,lua_tostring(L,2));
				delete[] tble;
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simLoadUI(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simLoadUI");
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		int tble[1000];
		int res=simLoadUI_internal(lua_tostring(L,1),1000,tble);
		if (res>0)
		{
			pushIntTableOntoStack(L,res,tble);
		}
		else
			lua_pushnil(L);
		LUA_END(1);
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simRemoveUI(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simRemoveUI");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simRemoveUI_internal(lua_tointeger(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simIsObjectInSelection(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simIsObjectInSelection");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simIsObjectInSelection_internal(lua_tointeger(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simAddObjectToSelection(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAddObjectToSelection");
	int retVal=-1;// error
	// We check if argument 1 is nil (special case):
	if (checkOneGeneralInputArgument(L,1,functionName,lua_arg_nil,0,false,true,false)==1) // we do not generate an error message!
	{
		retVal=1; // nothing happens
	}
	else
	{
		// We check if we have a table at position 1:
		if (!lua_istable(L,1))
		{ // It is not a table!
			if (checkInputArguments(L,functionName,false,lua_arg_number,0,lua_arg_number,0)) // we don't generate an error
				retVal=simAddObjectToSelection_internal(lua_tointeger(L,1),lua_tointeger(L,2));
			else
			{ // Maybe we have a special case with one argument only?
				// nil is a valid argument!
				if (checkInputArguments(L,functionName,false,lua_arg_nil,0)) // we don't generate an error
					retVal=1;
				else
				{
					if (checkInputArguments(L,functionName,true,lua_arg_number,0))
					{
						if (lua_tointeger(L,1)==sim_handle_all)
							retVal=simAddObjectToSelection_internal(lua_tointeger(L,1),-1);
						else
							checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0); // we just generate an error
					}
				}
			}
		}
		else
		{ // Ok we have a table. Now what size is it?
            int tableLen=int(VREP_LUA_OBJLEN(L,1));
			int* buffer=new int[tableLen];
			if (getIntsFromTable(L,1,tableLen,buffer))
			{
				for (int i=0;i<tableLen;i++)
				{
					if (App::ct->objCont->getObject(buffer[i])!=NULL)
						App::ct->objCont->addObjectToSelection(buffer[i]);
				}
				retVal=1;
			}
			else
				outputCallErrorMessage(functionName.c_str(),SIM_ERROR_TABLE_CONTAINS_INVALID_TYPES,true);
			delete[] buffer;
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simRemoveObjectFromSelection(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simRemoveObjectFromSelection");
	int retVal=-1;// error
	// We check if argument 1 is nil (special case):
	if (checkOneGeneralInputArgument(L,1,functionName,lua_arg_nil,0,false,true,false)==1) // we do not generate an error message!
	{
		retVal=1; // nothing happens
	}
	else
	{
		// We check if we have a table at position 1:
		if (!lua_istable(L,1))
		{ // It is not a table!
			if (checkInputArguments(L,functionName,false,lua_arg_number,0,lua_arg_number,0)) // we don't generate an error
				retVal=simRemoveObjectFromSelection_internal(lua_tointeger(L,1),lua_tointeger(L,2));
			else
			{
				if (checkInputArguments(L,functionName,true,lua_arg_number,0))
				{
					if (lua_tointeger(L,1)==sim_handle_all)
						retVal=simRemoveObjectFromSelection_internal(lua_tointeger(L,1),-1);
					else
						checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0); // we just generate an error
				}
			}
		}
		else
		{ // Ok we have a table. Now what size is it?
            int tableLen=int(VREP_LUA_OBJLEN(L,1));
			int* buffer=new int[tableLen];
			if (getIntsFromTable(L,1,tableLen,buffer))
			{
				for (int i=0;i<tableLen;i++)
					retVal=simRemoveObjectFromSelection_internal(sim_handle_single,buffer[i]);
			}
			else
				outputCallErrorMessage(functionName.c_str(),SIM_ERROR_TABLE_CONTAINS_INVALID_TYPES,true);
			delete[] buffer;
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetObjectSelectionSize(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectSelectionSize");
	int	retVal=simGetObjectSelectionSize_internal();
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetObjectLastSelection(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectLastSelection");
	int	retVal=simGetObjectLastSelection_internal();
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetObjectSelection(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectSelection");
	int selSize=App::ct->objCont->getSelSize();
	if (selSize==0)
		lua_pushnil(L);
	else
	{
		int* sel=new int[selSize];
		int	res=simGetObjectSelection_internal(sel);
		if (res<=0)
			lua_pushnil(L);
		else
			pushIntTableOntoStack(L,selSize,sel);
		delete[] sel;
	}
	LUA_END(1);
}

int _simGetRealTimeSimulation(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetRealTimeSimulation");
	int	retVal=simGetRealTimeSimulation_internal();
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCreateUI(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCreateUI");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0,lua_arg_number,0,lua_arg_number,2,lua_arg_number,2))
	{
		int clientSize[2];
		int cellSize[2];
		getIntsFromTable(L,3,2,clientSize);
		getIntsFromTable(L,4,2,cellSize);
		int menuAttributes=lua_tointeger(L,2);
		int b=0;
		for (int i=0;i<8;i++)
		{
			if (menuAttributes&(1<<i))
				b++;
		}
		int* buttonHandles=new int[b];
		menuAttributes=(menuAttributes|sim_ui_menu_systemblock)-sim_ui_menu_systemblock;
		retVal=simCreateUI_internal(lua_tostring(L,1),menuAttributes,clientSize,cellSize,buttonHandles);
		if (retVal!=-1)
		{
			lua_pushnumber(L,retVal);
			pushIntTableOntoStack(L,b,buttonHandles);
			delete[] buttonHandles;
			LUA_END(2);
		}
		delete[] buttonHandles;
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCreateUIButton(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCreateUIButton");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,2,lua_arg_number,2,lua_arg_number,0))
	{
		int pos[2];
		int size[2];
		getIntsFromTable(L,2,2,pos);
		getIntsFromTable(L,3,2,size);
		retVal=simCreateUIButton_internal(lua_tointeger(L,1),pos,size,lua_tointeger(L,4));
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simLaunchExecutable(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simLaunchExecutable");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		std::string file(lua_tostring(L,1));
		int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_string,0,true,false,true);
		if ((res==0)||(res==2))
		{
			std::string args;
			if (res==2)
				args=lua_tostring(L,2);
			res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,0,true,false,true);
			if ((res==0)||(res==2))
			{
				int showStatus=1;
				if (res==2)
					showStatus=luaToInt(L,3);
				int sh=VVARIOUS_SHOWNORMAL;
				if (showStatus==0)
					sh=VVARIOUS_HIDE;
				if (VVarious::executeExternalApplication(file,args,App::directories->executableDirectory,sh)) // executable directory needed because otherwise the shellExecute command might switch directories!
					retVal=1;
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simTest(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simTest");

	int startHandle=lua_tointeger(L,1);
	int goalHandle=lua_tointeger(L,2);
	int dummyHandle=lua_tointeger(L,3);
	float t=luaToFloat(L,4);

	C3DObject* start=App::ct->objCont->getObject(startHandle);
	C3DObject* goal=App::ct->objCont->getObject(goalHandle);
	C3DObject* dummy=App::ct->objCont->getObject(dummyHandle);

	C7Vector startTr(start->getCumulativeTransformationPart1());
	C7Vector goalTr(goal->getCumulativeTransformationPart1());

	C7Vector dummyTr;

	dummyTr.buildInterpolation(startTr,goalTr,fabs(t));
	if (t<0.0f)
		dummyTr.Q.buildInterpolation_otherWayRound(startTr.Q,goalTr.Q,fabs(t));

	dummy->setLocalTransformation(dummyTr);


	LUA_END(0);
}


int _simSetNavigationMode(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetNavigationMode");
	int retVal=-1; //error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simSetNavigationMode_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetNavigationMode(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetNavigationMode");
	int retVal=-1; //error
	retVal=simGetNavigationMode_internal();
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetPage(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetPage");
	int retVal=-1; //error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simSetPage_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetPage(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetPage");
	int retVal=-1; //error
	retVal=simGetPage_internal();
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simReleaseScriptRawBuffer(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simReleaseScriptRawBuffer");
	int retVal=-1; //error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		int handle=lua_tointeger(L,1);
		if (handle==sim_handle_self)
			handle=getCurrentScriptID(L);
		if ( (handle!=sim_handle_tree)&&(handle!=sim_handle_chain) )
			retVal=simReleaseScriptRawBuffer_internal(handle,luaToInt(L,2));
		else
		{
			std::vector<int> scriptHandles;
			if (handle==sim_handle_tree)
				getScriptTree(L,false,scriptHandles);
			else
				getScriptChain(L,false,false,scriptHandles);
			for (int i=0;i<int(scriptHandles.size());i++)
				retVal=simReleaseScriptRawBuffer_internal(scriptHandles[i],sim_handle_all);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCopyPasteSelectedObjects(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCopyPasteSelectedObjects");
	int retVal=simCopyPasteSelectedObjects_internal();
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simScaleSelectedObjects(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simScaleSelectedObjects");
	int retVal=-1; //error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_bool,0))
		retVal=simScaleSelectedObjects_internal(luaToFloat(L,1),luaToBool(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simDeleteSelectedObjects(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simDeleteSelectedObjects");
	int retVal=simDeleteSelectedObjects_internal();
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetObjectUniqueIdentifier(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectUniqueIdentifier");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int handle=luaToInt(L,1);
		if (handle==sim_handle_all)
		{
			int cnt=App::ct->objCont->objectList.size();
			int* buffer=new int[cnt];
			if (simGetObjectUniqueIdentifier_internal(handle,buffer)!=-1)
			{
				pushIntTableOntoStack(L,cnt,buffer);
				delete[] buffer;
				LUA_END(1);
			}
			delete[] buffer;
		}
		else
		{
			int retVal;
			if (simGetObjectUniqueIdentifier_internal(handle,&retVal)!=-1)
			{
				lua_pushnumber(L,retVal);
				LUA_END(1);
			}
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simGetNameSuffix(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetNameSuffix");
	if (checkInputArguments(L,functionName,false,lua_arg_nil,0))
	{ // we want the suffix of current script
		std::string suffTxt;
		int suffixNumber=-1;
		lua_getglobal(L,SIM_SCRIPT_NAME_SUFFIX);
		if (lua_isstring(L,-1))
			suffTxt=lua_tostring(L,-1);
		lua_pop(L,1);
		if (suffTxt!="")
			tt::getValidInt(suffTxt,suffixNumber);
		lua_pushnumber(L,suffixNumber);
		LUA_END(1);
	}
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{ // we want the suffix of the provided name
		std::string nameWithSuffix(lua_tostring(L,1));
		std::string name(tt::getNameWithoutSuffixNumber(nameWithSuffix.c_str(),true));
		int suffixNumber=tt::getNameSuffixNumber(nameWithSuffix.c_str(),true);
		lua_pushnumber(L,suffixNumber);
		lua_pushstring(L,name.c_str());
		LUA_END(2);
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simSetNameSuffix(lua_State* L)
{ 
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetNameSuffix");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int nb=lua_tointeger(L,1);
		std::string suffTxt("");
		if (nb>=0)
			suffTxt=tt::FNb(nb);
		std::string tmp(SIM_SCRIPT_NAME_SUFFIX);
		tmp+="='";
		tmp+=suffTxt;
		tmp+="'";
		luaL_dostring(L,tmp.c_str());
		retVal=1;
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetThreadSwitchTiming(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetThreadSwitchTiming");
	int retVal=-1;
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		if (CThreadPool::setThreadSwitchTiming(lua_tointeger(L,1)))
			retVal=1;
		else
			retVal=0;
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetThreadAutomaticSwitch(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetThreadAutomaticSwitch");
	int retVal=-1;
	if (checkInputArguments(L,functionName,true,lua_arg_bool,0))
	{
		if (CThreadPool::setThreadAutomaticSwitch(luaToBool(L,1)))
			retVal=1;
		else
			retVal=0;
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetThreadResumeLocation(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetThreadResumeLocation");
	int retVal=-1;
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		if (CThreadPool::setThreadResumeLocation(lua_tointeger(L,1),lua_tointeger(L,2)))
			retVal=1;
		else
			retVal=0;
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simResumeThreads(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START_NO_LOCK("simResumeThreads");
	LUA_LOCK_NO_INSTANCE_SWITCH; // Important to lock for this section (objects might get destroyed in the mean time!)
	int retVal=-1;
	int currentScriptID=getCurrentScriptID(L);
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
	if (it->getScriptType()==sim_scripttype_mainscript)
	{
		if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		{
			int loc=lua_tointeger(L,1);
			App::ct->infoDisplay->runningThreadedScriptStart(0);
			int startTime=VDateTime::getTimeInMs();
			LUA_UNLOCK_NO_INSTANCE_SWITCH;
			retVal=CThreadPool::handleAllThreads_withResumeLocation(loc);
			LUA_LOCK_NO_INSTANCE_SWITCH;
			App::ct->infoDisplay->mainScriptPaused(VDateTime::getTimeDiffInMs(startTime));
			App::ct->infoDisplay->runningThreadedScriptEnd();
		}
	}
	else
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_MAIN_SCRIPT,true);
	LUA_UNLOCK_NO_INSTANCE_SWITCH;
	LUA_END_NO_LOCK(retVal); 
}



int _simSwitchThread(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START_NO_LOCK("simSwitchThread");
	int retVal=-1;
	if (CThreadPool::switchBackToPreviousThread())
		retVal=1;
	else
		retVal=0;
	lua_pushnumber(L,retVal);
	LUA_END_NO_LOCK(1);
}

int _simPackInts(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simPackInts");
	if (lua_gettop(L)>0)
	{
		if (lua_istable(L,1))
		{
            int tableSize=int(VREP_LUA_OBJLEN(L,1));
			char* data=new char[sizeof(int)*tableSize];
			for (int i=0;i<tableSize;i++)
			{
				lua_rawgeti(L,1,i+1);
				int v=lua_tointeger(L,-1);
				data[4*i+0]=((char*)&v)[0];
				data[4*i+1]=((char*)&v)[1];
				data[4*i+2]=((char*)&v)[2];
				data[4*i+3]=((char*)&v)[3];
				lua_pop(L,1); // we have to pop the value that was pushed with lua_rawgeti
			}
			lua_pushlstring(L,(const char*)data,tableSize*sizeof(int));
			delete[] data;
			LUA_END(1);
		}
		else
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG,true);
	}
	else
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS,true);
	lua_pushnil(L);
	LUA_END(1);
}

int _simPackFloats(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simPackFloats");
	if (lua_gettop(L)>0)
	{
		if (lua_istable(L,1))
		{
            int tableSize=int(VREP_LUA_OBJLEN(L,1));
			char* data=new char[sizeof(float)*tableSize];
			for (int i=0;i<tableSize;i++)
			{
				lua_rawgeti(L,1,i+1);
				float v=(float)lua_tonumber(L,-1);
				data[4*i+0]=((char*)&v)[0];
				data[4*i+1]=((char*)&v)[1];
				data[4*i+2]=((char*)&v)[2];
				data[4*i+3]=((char*)&v)[3];
				lua_pop(L,1); // we have to pop the value that was pushed with lua_rawgeti
			}
			lua_pushlstring(L,(const char*)data,tableSize*sizeof(float));
			delete[] data;
			LUA_END(1);
		}
		else
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG,true);
	}
	else
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS,true);
	lua_pushnil(L);
	LUA_END(1);
}

int _simUnpackInts(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simUnpackInts");
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		int startIndex=0;
		int count=0;
		int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_number,0,true,false,true);
		if ((res==0)||(res==2))
		{
			if (res==2)
				startIndex=luaToInt(L,2);

			res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,0,true,false,true);
			if ((res==0)||(res==2))
			{
				if (res==2)
					count=luaToInt(L,3);
				
				size_t dataLength;
				char* data=(char*)lua_tolstring(L,1,&dataLength);
				dataLength=4*(dataLength/4);
				int packetCount=dataLength/4;
				if (count==0)
					count=int(1999999999);

				if ((startIndex>=0)&&(startIndex<packetCount))
				{
					if (startIndex+count>packetCount)
						count=packetCount-startIndex;

					int* theInts=new int[count];
					for (int i=0;i<int(count);i++)
					{
						int v;
						((char*)&v)[0]=data[4*(i+startIndex)+0];
						((char*)&v)[1]=data[4*(i+startIndex)+1];
						((char*)&v)[2]=data[4*(i+startIndex)+2];
						((char*)&v)[3]=data[4*(i+startIndex)+3];
						theInts[i]=v;
					}
					pushIntTableOntoStack(L,count,theInts);
					delete[] theInts;
					LUA_END(1);
				}
			}
		}
	}
	lua_pushnil(L);
	LUA_END(1);

}
int _simUnpackFloats(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simUnpackFloats");
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		int startIndex=0;
		int count=0;
		int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_number,0,true,false,true);
		if ((res==0)||(res==2))
		{
			if (res==2)
				startIndex=luaToInt(L,2);

			res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,0,true,false,true);
			if ((res==0)||(res==2))
			{
				if (res==2)
					count=luaToInt(L,3);
				
				size_t dataLength;
				char* data=(char*)lua_tolstring(L,1,&dataLength);
				dataLength=4*(dataLength/4);
				int packetCount=dataLength/4;
				if (count==0)
					count=int(1999999999);

				if ((startIndex>=0)&&(startIndex<packetCount))
				{
					if (startIndex+count>packetCount)
						count=packetCount-startIndex;

					float* theFloats=new float[count];
					for (int i=0;i<int(count);i++)
					{
						float v;
						((char*)&v)[0]=data[4*(i+startIndex)+0];
						((char*)&v)[1]=data[4*(i+startIndex)+1];
						((char*)&v)[2]=data[4*(i+startIndex)+2];
						((char*)&v)[3]=data[4*(i+startIndex)+3];
						theFloats[i]=v;
					}
					pushFloatTableOntoStack(L,count,theFloats);
					delete[] theFloats;
					LUA_END(1);
				}
			}
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simSendData(lua_State* L)
{ // can be called from the main script or from child scripts
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSendData");
	int retVal=-1;

	int currentScriptID=getCurrentScriptID(L);
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
	if ( (it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript) )
	{
		if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_string,0,lua_arg_string,0))
		{
			int targetID=lua_tointeger(L,1);
			if ( (targetID<=0)&&(targetID!=sim_handle_all)&&(targetID!=sim_handle_tree)&&(targetID!=sim_handle_chain) )
				outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_TARGET_HANDLE,true);
			{
				int dataHeader=lua_tointeger(L,2);
				if (dataHeader<0)
					outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_DATA_HEADER,true);
				{
					std::string dataName(lua_tostring(L,3));
					if ( (dataName.length()==0)||(dataName.find(char(0))!=std::string::npos) )
						outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_DATA_NAME,true);
					{
						size_t dataLength;
						char* data=(char*)lua_tolstring(L,4,&dataLength);
						if (dataLength<1)
							outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_DATA,true);
						{
							// Following are default values:
							int antennaHandle=sim_handle_self;
							float actionRadius=100.0f;
							float emissionAngle1=piValue;
							float emissionAngle2=piValTimes2;
							float persistence=0.0f;
							bool err=false;
							int res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,0,true,false,true);
							if (res==2)
								antennaHandle=lua_tointeger(L,5);
							err|=((res!=0)&&(res!=2));
							if (!err)
							{
								if ( (antennaHandle<0)&&(antennaHandle!=sim_handle_default)&&((antennaHandle!=sim_handle_self)||(it->getScriptType()!=sim_scripttype_childscript)) )
								{
									outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_ANTENNA_HANDLE,true);
									err=true;
								}
								else
								{
									if (antennaHandle==sim_handle_self)
										antennaHandle=it->getObjectIDThatScriptIsAttachedTo();
									if (antennaHandle!=sim_handle_default)
									{
										C3DObject* ant=App::ct->objCont->getObject(antennaHandle);
										if (ant==NULL)
										{
											outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_ANTENNA_HANDLE,true);
											err=true;
										}
									}
								}
							}
							if (!err)
							{
								int res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,0,true,false,true);
								if (res==2)
									actionRadius=luaToFloat(L,6);
								err|=((res!=0)&&(res!=2));
							}
							if (!err)
							{
								int res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,0,true,false,true);
								if (res==2)
									emissionAngle1=luaToFloat(L,7);
								err|=((res!=0)&&(res!=2));
							}
							if (!err)
							{
								int res=checkOneGeneralInputArgument(L,8,functionName,lua_arg_number,0,true,false,true);
								if (res==2)
									emissionAngle2=luaToFloat(L,8);
								err|=((res!=0)&&(res!=2));
							}
							if (!err)
							{
								int res=checkOneGeneralInputArgument(L,9,functionName,lua_arg_number,0,true,false,true);
								if (res==2)
									persistence=luaToFloat(L,9);
								err|=((res!=0)&&(res!=2));
							}
							if (!err)
							{
								actionRadius=tt::getLimitedFloat(0.0f,SIM_MAX_FLOAT,actionRadius);
								emissionAngle1=tt::getLimitedFloat(0.0f,piValue,emissionAngle1);
								emissionAngle2=tt::getLimitedFloat(0.0f,piValTimes2,emissionAngle2);
								persistence=tt::getLimitedFloat(0.0f,99999999999999.9f,persistence);
								if (persistence==0.0f)
									persistence=App::ct->simulation->getSimulationTimeStep_speedModified()*1.5f;

								App::ct->luaScriptContainer->broadcastDataContainer.broadcastData(currentScriptID,targetID,dataHeader,dataName,
									App::ct->simulation->getSimulationTime()+persistence,actionRadius,antennaHandle,
									emissionAngle1,emissionAngle2,data,dataLength);
								retVal=1;
							}
						}
					}
				}
			}
		}
	}
	else
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_MAIN_SCRIPT_OR_CHILD_SCRIPT,true);

	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simReceiveData(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simReceiveData");
	int currentScriptID=getCurrentScriptID(L);
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
	if ( (it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript) )
	{
		int dataHeader=-1;
		std::string dataName;
		int antennaHandle=sim_handle_self;
		int index=-1;
		bool err=false;
		int res=checkOneGeneralInputArgument(L,1,functionName,lua_arg_number,0,true,false,true);
		if (res==2)
		{
			dataHeader=lua_tointeger(L,1);
			if (dataHeader<0)
				dataHeader=-1;
		}
		err|=((res!=0)&&(res!=2));
		if (!err)
		{
			int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_string,0,true,true,true);
			if (res==2)
			{
				dataName=lua_tostring(L,2);
				if (dataName.length()<1)
				{
					outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_DATA_NAME,true);
					err=true;
				}
			}
			err|=(res<0);
		}
		if (!err)
		{
			int res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,0,true,false,true);
			if (res==2)
			{
				antennaHandle=lua_tointeger(L,3);
				if ( (antennaHandle<0)&&(antennaHandle!=sim_handle_default)&&((antennaHandle!=sim_handle_self)||(it->getScriptType()!=sim_scripttype_childscript)) )
				{
					outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_ANTENNA_HANDLE,true);
					err=true;
				}
			}
			err|=((res!=0)&&(res!=2));
			if (!err)
			{
				if (antennaHandle==sim_handle_self)
					antennaHandle=it->getObjectIDThatScriptIsAttachedTo();
				if (antennaHandle!=sim_handle_default)
				{
					C3DObject* ant=App::ct->objCont->getObject(antennaHandle);
					if (ant==NULL)
					{
						outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_ANTENNA_HANDLE,true);
						err=true;
					}
				}
			}
		}
		if (!err)
		{
			int res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,0,true,false,true);
			if (res==2)
			{
				index=lua_tointeger(L,4);
				if (index<0)
					index=-1;
			}
			err|=((res!=0)&&(res!=2));
		}
		if (!err)
		{
			int theDataHeader;
			int theDataLength;
			int theSenderID;
			std::string theDataName;
			char* data0=App::ct->luaScriptContainer->broadcastDataContainer.receiveData(currentScriptID,App::ct->simulation->getSimulationTime(),
					dataHeader,dataName,antennaHandle,theDataLength,index,theSenderID,theDataHeader,theDataName);
			if (data0!=NULL)
			{
				lua_pushlstring(L,data0,theDataLength);
				lua_pushnumber(L,theSenderID);
				lua_pushnumber(L,theDataHeader);
				lua_pushstring(L,theDataName.c_str());
				LUA_END(4);
			}
			else
			{
				lua_pushnil(L);
				LUA_END(1);
			}
		}
		else
		{
			lua_pushnil(L);
			LUA_END(1);
		}
	}
	outputCallErrorMessage(functionName.c_str(),SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_MAIN_SCRIPT_OR_CHILD_SCRIPT,true);
	lua_pushnil(L);
	LUA_END(1);
}

int _simSetGraphUserData(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetGraphUserData");
	int retVal=-1; // for error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0,lua_arg_number,0))
	{
		int graphHandle=lua_tointeger(L,1);
		std::string dataName(lua_tostring(L,2));
		float data=luaToFloat(L,3);
		retVal=simSetGraphUserData_internal(graphHandle,dataName.c_str(),data);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetUIPosition(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetUIPosition");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int position[2];
		if (simGetUIPosition_internal(luaToInt(L,1),position)!=-1)
			pushIntTableOntoStack(L,2,position);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simSetUIPosition(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetUIPosition");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,2))
	{
		int position[2];
		getIntsFromTable(L,2,2,position);
		retVal=simSetUIPosition_internal(luaToInt(L,1),position);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simAddDrawingObject(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAddDrawingObject");
	int retVal=-1; // means error

	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int objType=luaToInt(L,1);
		float size=luaToFloat(L,2);
		float duplicateTolerance=luaToFloat(L,3);
		int parentID=luaToInt(L,4);
		int maxItemCount=luaToInt(L,5);
		float* ambient=NULL;
		float* diffuse=NULL;
		float* specular=NULL;
		float* emission=NULL;
		int res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,3,true,true,true);
		int okToGo=(res!=-1);
		if (okToGo)
		{
			float ambientC[3];
			float diffuseC[3];
			float specularC[3];
			float emissionC[3];
			if (res>0)
			{
				if (res==2)
				{
					getFloatsFromTable(L,6,3,ambientC);
					ambient=ambientC;
				}
				res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,3,true,true,true);
				okToGo=(res!=-1);
				if (okToGo)
				{
					if (res>0)
					{
						if (res==2)
						{
							getFloatsFromTable(L,7,3,diffuseC);
							diffuse=diffuseC;
						}
						res=checkOneGeneralInputArgument(L,8,functionName,lua_arg_number,3,true,true,true);
						okToGo=(res!=-1);
						if (okToGo)
						{
							if (res>0)
							{
								if (res==2)
								{
									getFloatsFromTable(L,8,3,specularC);
									specular=specularC;
								}
								res=checkOneGeneralInputArgument(L,9,functionName,lua_arg_number,3,true,true,true);
								okToGo=(res!=-1);
								if (okToGo)
								{
									if (res>0)
									{
										if (res==2)
										{
											getFloatsFromTable(L,9,3,emissionC);
											emission=emissionC;
										}

									}
								}
							}
						}
					}
				}
			}
			if (okToGo)
			{
				retVal=simAddDrawingObject_internal(objType,size,duplicateTolerance,parentID,maxItemCount,ambient,diffuse,specular,emission);
				if (retVal!=-1)
				{ // following condition added on 2011/01/06 so as to not remove objects created from the c/c++ interface or from an add-on:
					int currentScriptID=getCurrentScriptID(L);
					CLuaScriptObject* itScrObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
					if ( (itScrObj->getScriptType()==sim_scripttype_mainscript)||(itScrObj->getScriptType()==sim_scripttype_childscript) )
					{
						CDrawingObject* anObj=App::ct->drawingCont->getObject(retVal);
						if (anObj!=NULL)
							anObj->setCreatedFromScript(true);
					}
				}
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simRemoveDrawingObject(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simRemoveDrawingObject");
	int retVal=-1; // means error

	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int objectHandle=luaToInt(L,1);
		if (objectHandle==sim_handle_all)
		{ // following condition added here on 2011/01/06 so as not to remove objects created from a C/c++ call or from add-on:
			int currentScriptID=getCurrentScriptID(L);
			CLuaScriptObject* itScrObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
			App::ct->drawingCont->removeAllObjects((itScrObj->getScriptType()==sim_scripttype_mainscript)||(itScrObj->getScriptType()==sim_scripttype_childscript));
			retVal=1;
		}
		else
			retVal=simRemoveDrawingObject_internal(objectHandle);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simAddDrawingObjectItem(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAddDrawingObjectItem");
	int retVal=-1; // means error

	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int drawingObjHandle=luaToInt(L,1);
		CDrawingObject* it=App::ct->drawingCont->getObject(drawingObjHandle);
		int d=3;
		if (it!=NULL)
			d=it->verticesPerItem*3+it->normalsPerItem*3+it->otherFloatsPerItem;
		int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_number,d,true,true,true);
		if (res==2)
		{
			float vertex[20]; // we should have enough here!
			getFloatsFromTable(L,2,d,vertex);
			retVal=simAddDrawingObjectItem_internal(drawingObjHandle,vertex);
		}
		else
		{
			if (res>=0)
				retVal=simAddDrawingObjectItem_internal(drawingObjHandle,NULL);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simAddParticleObject(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAddParticleObject");
	int retVal=-1; // means error

	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		// The 4th argument can be nil or a table. Check for that:
		if (lua_gettop(L)<4)
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_FUNCTION_REQUIRES_MORE_ARGUMENTS,true);
		else
		{
            if ( (!lua_isnil(L,4))&&((!lua_istable(L,4))||(int(VREP_LUA_OBJLEN(L,4))<3)) )
				outputCallErrorMessage(functionName.c_str(),SIM_ERROR_ONE_ARGUMENT_TYPE_IS_WRONG,true);
			else
			{
				if ( (checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,0,false,false,true)==2)&&
					(checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,0,false,false,true)==2) )
				{
					int objType=luaToInt(L,1);
					float size=luaToFloat(L,2);
					float massOverVolume=luaToFloat(L,3);
                    int paramLen=int(VREP_LUA_OBJLEN(L,4));
					paramLen=(paramLen-1)/2;
					paramLen=paramLen*2+1;
					void* params=NULL;
					if (!lua_isnil(L,4))
					{
						int intParams[30];
						float floatParams[30];
						getIntsFromTable(L,4,paramLen,intParams);
						getFloatsFromTable(L,4,paramLen,floatParams);
						params=new char[paramLen*sizeof(int)];
						((int*)params)[0]=SIM_MIN(intParams[0],(paramLen-1)/2);
						for (int i=0;i<(paramLen-1)/2;i++)
						{
							((int*)params)[1+2*i]=intParams[1+2*i+0];
							((float*)params)[1+2*i+1]=floatParams[1+2*i+1];
						}
					}
					float lifeTime=luaToFloat(L,5);
					int maxItemCount=luaToInt(L,6);
					float* ambient=NULL;
					float* diffuse=NULL;
					float* specular=NULL;
					float* emission=NULL;
					int res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,3,true,true,true);
					int okToGo=(res!=-1);
					if (okToGo)
					{
						float ambientC[3];
						float diffuseC[3];
						float specularC[3];
						float emissionC[3];
						if (res>0)
						{
							if (res==2)
							{
								getFloatsFromTable(L,7,3,ambientC);
								ambient=ambientC;
							}
							res=checkOneGeneralInputArgument(L,8,functionName,lua_arg_number,3,true,true,true);
							okToGo=(res!=-1);
							if (okToGo)
							{
								if (res>0)
								{
									if (res==2)
									{
										getFloatsFromTable(L,8,3,diffuseC);
										diffuse=diffuseC;
									}
									res=checkOneGeneralInputArgument(L,9,functionName,lua_arg_number,3,true,true,true);
									okToGo=(res!=-1);
									if (okToGo)
									{
										if (res>0)
										{
											if (res==2)
											{
												getFloatsFromTable(L,9,3,specularC);
												specular=specularC;
											}
											res=checkOneGeneralInputArgument(L,10,functionName,lua_arg_number,3,true,true,true);
											okToGo=(res!=-1);
											if (okToGo)
											{
												if (res>0)
												{
													if (res==2)
													{
														getFloatsFromTable(L,10,3,emissionC);
														emission=emissionC;
													}

												}
											}
										}
									}
								}
							}
						}
						if (okToGo)
						{
							retVal=simAddParticleObject_internal(objType,size,massOverVolume,params,lifeTime,maxItemCount,ambient,diffuse,specular,emission);
						}
					}
					delete[] ((char*)params);
				}
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simRemoveParticleObject(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simRemoveParticleObject");
	int retVal=-1; // means error

	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simRemoveParticleObject_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simAddParticleObjectItem(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAddParticleObjectItem");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int particleObjHandle=luaToInt(L,1);
		int d=6+CDynInterface::getParticleObjectOtherFloatsPerItem(particleObjHandle);
		int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_number,d,true,true,true);
		if (res==2)
		{
			float vertex[20]; // we should have enough here!
			getFloatsFromTable(L,2,d,vertex);
			retVal=simAddParticleObjectItem_internal(particleObjHandle,vertex);
		}
		else
		{
			if (res>=0)
				retVal=simAddParticleObjectItem_internal(particleObjHandle,NULL);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetObjectSizeFactor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectSizeFactor");
	float retVal=-1.0f; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simGetObjectSizeFactor_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simResetMilling(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simResetMilling");
	int retVal=-1; //error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simResetMilling_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simApplyMilling(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simApplyMilling");
	int retVal=-1; //error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simApplyMilling_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetIntegerSignal(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetIntegerSignal");
	int retVal=-1; //error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0,lua_arg_number,0))
	{
		int currentScriptID=getCurrentScriptID(L);
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
		App::ct->signalContainer->setIntegerSignal(std::string(lua_tostring(L,1)).c_str(),luaToInt(L,2),(it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript));
		retVal=1;
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetIntegerSignal(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetIntegerSignal");
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		int intVal;
		if (simGetIntegerSignal_internal(std::string(lua_tostring(L,1)).c_str(),&intVal)==1)
			lua_pushnumber(L,intVal);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simClearIntegerSignal(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simClearIntegerSignal");
	int retVal=-1; //error
	int res=checkOneGeneralInputArgument(L,1,functionName,lua_arg_string,0,true,true,true);
	if (res>=0)
	{
		if (res!=2)
		{
			int currentScriptID=getCurrentScriptID(L);
			CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
			retVal=App::ct->signalContainer->clearAllIntegerSignals((it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript));
		}
		else
			retVal=simClearIntegerSignal_internal(std::string(lua_tostring(L,1)).c_str());
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetFloatSignal(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetFloatSignal");
	int retVal=-1; //error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0,lua_arg_number,0))
	{
		int currentScriptID=getCurrentScriptID(L);
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
		App::ct->signalContainer->setFloatSignal(std::string(lua_tostring(L,1)).c_str(),luaToFloat(L,2),(it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript));
		retVal=1;
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetFloatSignal(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetFloatSignal");
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		float floatVal;
		if (simGetFloatSignal_internal(std::string(lua_tostring(L,1)).c_str(),&floatVal)==1)
			lua_pushnumber(L,floatVal);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simClearFloatSignal(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simClearFloatSignal");
	int retVal=-1; //error
	int res=checkOneGeneralInputArgument(L,1,functionName,lua_arg_string,0,true,true,true);
	if (res>=0)
	{
		if (res!=2)
		{
			int currentScriptID=getCurrentScriptID(L);
			CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
			retVal=App::ct->signalContainer->clearAllFloatSignals((it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript));
		}
		else
			retVal=simClearFloatSignal_internal(std::string(lua_tostring(L,1)).c_str());
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetStringSignal(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetStringSignal");
	int retVal=-1; //error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0,lua_arg_string,0))
	{
		size_t dataLength;
		char* data=(char*)lua_tolstring(L,2,&dataLength);
		int currentScriptID=getCurrentScriptID(L);
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
		App::ct->signalContainer->setStringSignal(std::string(lua_tostring(L,1)).c_str(),std::string(data,dataLength),(it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript));
		retVal=1;
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetStringSignal(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetStringSignal");
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		int stringLength;
		char* str=simGetStringSignal_internal(std::string(lua_tostring(L,1)).c_str(),&stringLength);
		if (str!=NULL)
		{
			lua_pushlstring(L,str,stringLength);
			simReleaseBuffer_internal(str);
		}
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simClearStringSignal(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simClearStringSignal");
	int retVal=-1; //error
	int res=checkOneGeneralInputArgument(L,1,functionName,lua_arg_string,0,true,true,true);
	if (res>=0)
	{
		if (res!=2)
		{
			int currentScriptID=getCurrentScriptID(L);
			CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
			retVal=App::ct->signalContainer->clearAllStringSignals((it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript));
		}
		else
			retVal=simClearStringSignal_internal(std::string(lua_tostring(L,1)).c_str());
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetSignalName(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetSignalName");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		char* str=simGetSignalName_internal(luaToInt(L,1),luaToInt(L,2));
		if (str!=NULL)
		{
			lua_pushstring(L,str);
			simReleaseBuffer_internal(str);
		}
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simWaitForSignal(lua_State* L)
{ // can only be called from a script running in a thread!!
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simWaitForSignal");
	if (VThread::isCurrentThreadTheMainThread())
	{
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD,true);
		lua_pushnil(L);
	}
	else
	{
		if (checkInputArguments(L,functionName,true,lua_arg_string,0))
		{
			std::string signalName(lua_tostring(L,1));
			int intVal;
			bool signalPresent=false;

			CSignalContainer* sigCont=NULL;
			sigCont=App::ct->signalContainer;

			LUA_UNLOCK; // Important!!
			while (!signalPresent)
			{
				if (sigCont->getIntegerSignal(signalName.c_str(),intVal))
				{
					lua_pushnumber(L,intVal);
					signalPresent=true;
				}
				float floatVal;
				if ( (!signalPresent)&&(sigCont->getFloatSignal(signalName.c_str(),floatVal)) )
				{
					lua_pushnumber(L,floatVal);
					signalPresent=true;
				}
				std::string strVal;
				if ( (!signalPresent)&&(sigCont->getStringSignal(signalName.c_str(),strVal)) )
				{
					lua_pushstring(L,strVal.c_str());
					signalPresent=true;
				}
				if (!signalPresent)
				{
					CThreadPool::switchBackToPreviousThread();
					if (CThreadPool::getStopForCurrentInstanceRequested())
					{
						lua_pushnil(L);
						break;
					}
				}
			}
			LUA_LOCK; // Important!!
		}
		else
			lua_pushnil(L);
	}
	LUA_END(1);
}

int _simGetObjectProperty(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectProperty");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simGetObjectProperty_internal(lua_tointeger(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetObjectProperty(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetObjectProperty");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetObjectProperty_internal(lua_tointeger(L,1),lua_tointeger(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetObjectSpecialProperty(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectSpecialProperty");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simGetObjectSpecialProperty_internal(lua_tointeger(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetObjectSpecialProperty(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetObjectSpecialProperty");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetObjectSpecialProperty_internal(lua_tointeger(L,1),lua_tointeger(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetModelProperty(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetModelProperty");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simGetModelProperty_internal(lua_tointeger(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetModelProperty(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetModelProperty");
	int retVal=-1;// error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetModelProperty_internal(lua_tointeger(L,1),lua_tointeger(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}


int _simMoveToObject(lua_State* L)
{ // can only be called from a script running in a thread!!
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simMoveToObject");
	if (VThread::isCurrentThreadTheMainThread())
	{
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD,true);
		lua_pushnil(L); // error
	}
	else
	{
		if (CThreadPool::getStopForCurrentInstanceRequested())
		{ // Important to first check if we are supposed to leave the thread!!
			lua_pushnil(L); // special error
			LUA_END(1);
		}
		if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		{ // Those are the arguments that are always required! (the rest can be ignored or set to nil!
			int objID=lua_tointeger(L,1);
			int targetObjID=lua_tointeger(L,2);
			float maxVelocity=0.1f;
			float relativeDistanceOnPath=-1.0f;
			int positionAndOrOrientation=3; // position and orientation (default value)
			C3DObject* object=App::ct->objCont->getObject(objID);
			C3DObject* targetObject=App::ct->objCont->getObject(targetObjID);
			float accel=0.0f; // means infinite accel!! (default value)
			bool foundError=false;
			if ((!foundError)&&((object==NULL)||(targetObject==NULL)))
			{
				outputCallErrorMessage(functionName.c_str(),SIM_ERROR_OBJECT_OR_TARGET_OBJECT_DOES_NOT_EXIST,true);
				foundError=true;
			}
			if ((!foundError)&&(targetObject==object))
			{
				outputCallErrorMessage(functionName.c_str(),SIM_ERROR_OBJECT_IS_SAME_AS_TARGET_OBJECT,true);
				foundError=true;
			}
			// Now check the optional arguments:
			int res;
			if (!foundError) // position and/or orientation argument:
			{
				res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,0,true,true,true);
				if (res==2)
				{ // get the data
					positionAndOrOrientation=abs(luaToInt(L,3));
					if ((positionAndOrOrientation&3)==0)
						positionAndOrOrientation=1; // position only
				}
				foundError=(res==-1);
			}
			if (!foundError) // positionOnPath argument:
			{
				res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,0,true,true,true);
				if (res==2)
				{ // get the data
					relativeDistanceOnPath=tt::getLimitedFloat(0.0f,1.0f,luaToFloat(L,4));
					if (targetObject->getObjectType()!=sim_object_path_type)
					{
						outputCallErrorMessage(functionName.c_str(),SIM_ERROR_TARGET_OBJECT_IS_NOT_A_PATH,true);
						foundError=true;
					}
				}
				foundError=(res==-1);
			}
			if (!foundError) // Velocity argument:
			{
				res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,0,false,false,true);
				if (res==2)
				{ // get the data
					maxVelocity=luaToFloat(L,5);
				}
				else
					foundError=true; // this argument is not optional!
			}
			if (!foundError) // Accel argument:
			{
				res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,0,true,true,true);
				if (res==2)
				{ // get the data
					accel=fabs(luaToFloat(L,6));
				}
				foundError=(res==-1);
			}
			if (foundError)
				lua_pushnil(L); // error
			else
			{ // do the job here!
				C7Vector startTr(object->getCumulativeTransformationPart1());
				float currentVel=0.0f;
				CVThreadData* threadData=CThreadPool::getCurrentThreadData();
				float lastTime=App::ct->simulation->getSimulationTime()+threadData->usedMovementTime;
				float vdl=1.0f;
				// vld is the totalvirtual distance
				float currentPos=0.0f;

				bool movementFinished=false;
				float dt=App::ct->simulation->getSimulationTimeStep_speedModified(); // this is the time left if we leave here
				float previousLL=0.0f;
				while ( (!movementFinished)&&(vdl!=0.0f) )
				{
					float currentTime=App::ct->simulation->getSimulationTime()+App::ct->simulation->getSimulationTimeStep_speedModified();
					dt=currentTime-lastTime;
					lastTime=currentTime;

					if (accel==0.0f)
					{ // Means infinite acceleration
						float timeNeeded=(vdl-currentPos)/maxVelocity;
						currentVel=maxVelocity;
						if (timeNeeded>dt)
						{
							currentPos+=dt*maxVelocity;
							dt=0.0f; // this is what is left
						}
						else
						{
							currentPos=vdl;
							if (timeNeeded>=0.0f)
								dt-=timeNeeded;
						}
					}
					else
					{
						double p=currentPos;
						double v=currentVel;
						double t=dt;
						CLinMotionRoutines::getNextValues(p,v,maxVelocity,accel,0.0f,vdl,0.0f,0.0f,t);
						currentPos=float(p);
						currentVel=float(v);
						dt=float(t);
					}

					// Now check if we are within tolerances:
					if (fabs(currentPos-vdl)<=0.00001f)
						movementFinished=true;

					// Set the new configuration of the object:
					float ll=currentPos/vdl;
					if (ll>1.0f)
						ll=1.0f;
					if ((App::ct->objCont->getObject(objID)==object)&&(App::ct->objCont->getObject(targetObjID)==targetObject)) // make sure the objects are still valid (running in a thread)
					{
						C7Vector targetTr(targetObject->getCumulativeTransformationPart1());
						bool goOn=true;
						if (relativeDistanceOnPath>=0.0f)
						{ // we should have a path here
							if (targetObject->getObjectType()==sim_object_path_type)
							{
								C7Vector pathLoc;
								if ( ((CPath*)targetObject)->pathContainer->getTransformationOnBezierCurveAtNormalizedVirtualDistance(relativeDistanceOnPath,pathLoc))
									targetTr*=pathLoc;
								else
									relativeDistanceOnPath=-1.0f; // the path is empty!
							}
							else
								goOn=false;
						}
						if (goOn)
						{
							C7Vector newAbs;
							newAbs.buildInterpolation(startTr,targetTr,(ll-previousLL)/(1.0f-previousLL));
							startTr=newAbs;
							C7Vector parentInv(object->getParentCumulativeTransformation().getInverse());
							C7Vector currentTr(object->getCumulativeTransformationPart1());
							if ((positionAndOrOrientation&1)==0)
								newAbs.X=currentTr.X;
							if ((positionAndOrOrientation&2)==0)
								newAbs.Q=currentTr.Q;
							object->setLocalTransformation(parentInv*newAbs);
						}
						else
							movementFinished=true; // the target object is not a path anymore!!
					}
					else
						movementFinished=true; // the object was destroyed during execution of the command!
					previousLL=ll;
					if (!movementFinished)
					{
						LUA_UNLOCK; // Important!!
						CThreadPool::switchBackToPreviousThread();
						if (CThreadPool::getStopForCurrentInstanceRequested())
						{
							LUA_LOCK; // Important!!
							lua_pushnil(L); // error
							break;
						}
						LUA_LOCK; // Important!!
					}
				}
				// The movement finished. Now add the time used:
				threadData->usedMovementTime=App::ct->simulation->getSimulationTimeStep_speedModified()-dt;
				lua_pushnumber(L,dt); // success (deltaTime left)
			}
		}
		else
			lua_pushnil(L); // error
	}
	LUA_END(1);
}

int _simFollowPath(lua_State* L)
{ // can only be called from a script running in a thread!!
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simFollowPath");
	if (VThread::isCurrentThreadTheMainThread())
	{
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD,true);
		lua_pushnil(L); // error
	}
	else
	{
		if (CThreadPool::getStopForCurrentInstanceRequested())
		{ // Important to first check if we are supposed to leave the thread!!
			lua_pushnil(L); // special error
			LUA_END(1);
		}
		if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
		{ // Those are the arguments that are always required! (the rest can be ignored or set to nil!
			int objID=lua_tointeger(L,1);
			int pathID=lua_tointeger(L,2);
			float posOnPath=luaToFloat(L,4);
			int positionAndOrOrientation=abs(luaToInt(L,3));
			if (positionAndOrOrientation==0)
				positionAndOrOrientation=1;
			float maxVelocity=luaToFloat(L,5);
			C3DObject* object=App::ct->objCont->getObject(objID);
			CPath* path=App::ct->objCont->getPath(pathID);
			float accel=0.0f; // means infinite accel!! (default value)
			bool foundError=false;
			if ((!foundError)&&(object==NULL))
			{
				outputCallErrorMessage(functionName.c_str(),SIM_ERROR_OBJECT_INEXISTANT,true);
				foundError=true;
			}
			if ((!foundError)&&(path==NULL))
			{
				outputCallErrorMessage(functionName.c_str(),SIM_ERROR_PATH_INEXISTANT,true);
				foundError=true;
			}
			if (!foundError)
			{
				if (path->pathContainer->getAttributes()&sim_pathproperty_closed_path)
				{
					if (posOnPath<0.0f)
						posOnPath=0.0f;
				}
				else
					posOnPath=tt::getLimitedFloat(0.0f,1.0f,posOnPath);
			}

			// Now check the optional arguments:
			int res;
			if (!foundError) // Accel argument:
			{
				res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,0,true,true,true);
				if (res==2)
				{ // get the data
					accel=fabs(luaToFloat(L,6));
				}
				foundError=(res==-1);
			}
			if (foundError)
				lua_pushnil(L); // error
			else
			{ // do the job here!
				float bezierPathLength=path->pathContainer->getBezierVirtualPathLength();
				double pos=posOnPath*bezierPathLength;
				float vel=0.0f;
				CVThreadData* threadData=CThreadPool::getCurrentThreadData();
				float lastTime=App::ct->simulation->getSimulationTime()+threadData->usedMovementTime;
				bool movementFinished=(bezierPathLength==0.0f);
				float dt=App::ct->simulation->getSimulationTimeStep_speedModified(); // this is the time left if we leave here
				while (!movementFinished)
				{
					if ((App::ct->objCont->getObject(objID)!=object)||(App::ct->objCont->getPath(pathID)!=path) ) // make sure the objects are still valid (running in a thread)
					{
						dt=App::ct->simulation->getSimulationTimeStep_speedModified();
						break;
					}


					float currentTime=App::ct->simulation->getSimulationTime()+App::ct->simulation->getSimulationTimeStep_speedModified();
					dt=currentTime-lastTime;
					lastTime=currentTime;
					if (accel==0.0f)
					{ // Means infinite acceleration
						path->pathContainer->handlePath_keepObjectUnchanged(dt,pos,vel,maxVelocity,1.0f,false,true);
					}
					else
					{
						path->pathContainer->handlePath_keepObjectUnchanged(dt,pos,vel,maxVelocity,accel,false,false);
					}

					// Now check if we are within tolerances:
					if ( ((fabs(pos-bezierPathLength)<=0.00001f)&&(maxVelocity>=0.0f)) || ((fabs(pos-0.0f)<=0.00001f)&&(maxVelocity<=0.0f)) )
						movementFinished=true;

					// Set the new configuration of the object:
					float ll=float(pos/bezierPathLength);
					C7Vector newAbs;
					if (path->pathContainer->getTransformationOnBezierCurveAtNormalizedVirtualDistance(ll,newAbs))
					{
						newAbs=path->getCumulativeTransformationPart1()*newAbs;
						C7Vector parentInv(object->getParentCumulativeTransformation().getInverse());
						C7Vector currAbs(object->getCumulativeTransformationPart1());
						if ((positionAndOrOrientation&1)==0)
							newAbs.X=currAbs.X;
						if ((positionAndOrOrientation&2)==0)
							newAbs.Q=currAbs.Q;
						object->setLocalTransformation(parentInv*newAbs);
					}
					else
						movementFinished=true;

					if (!movementFinished)
					{
						LUA_UNLOCK; // Important!!
						CThreadPool::switchBackToPreviousThread();
						if (CThreadPool::getStopForCurrentInstanceRequested())
						{
							LUA_LOCK; // Important!!
							lua_pushnil(L); // error
							break;
						}
						LUA_LOCK; // Important!!
					}
				}
				// The movement finished. Now add the time used:
				threadData->usedMovementTime=App::ct->simulation->getSimulationTimeStep_speedModified()-dt;
				lua_pushnumber(L,dt); // success
			}
		}
		else
			lua_pushnil(L); // error
	}
	LUA_END(1);
}

int _simGetDataOnPath(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetDataOnPath");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		int auxFlags;
		float auxChannels[4];
		if (simGetDataOnPath_internal(luaToInt(L,1),luaToFloat(L,2),0,&auxFlags,auxChannels)==1)
		{
			lua_pushnumber(L,auxFlags);
			pushFloatTableOntoStack(L,4,auxChannels);
			LUA_END(2);
		}
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simGetPositionOnPath(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetPositionOnPath");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		float coord[3];
		if (simGetPositionOnPath_internal(luaToInt(L,1),luaToFloat(L,2),coord)==1)
			pushFloatTableOntoStack(L,3,coord);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simGetOrientationOnPath(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetOrientationOnPath");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		float coord[3];
		if (simGetOrientationOnPath_internal(luaToInt(L,1),luaToFloat(L,2),coord)==1)
			pushFloatTableOntoStack(L,3,coord);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simGetClosestPositionOnPath(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetClosestPositionOnPath");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,3))
	{
		float coord[3];
		getFloatsFromTable(L,2,3,coord);
		float dist=0.0f;
		if (simGetClosestPositionOnPath_internal(luaToInt(L,1),coord,&dist)!=-1)
			lua_pushnumber(L,dist);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simDelegateChildScriptExecution(lua_State* L)
{ // can only be called from a script running in a thread!!
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simDelegateChildScriptExecution");
	int currentScriptID=getCurrentScriptID(L);
	CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
	int retVal=-1;
	if (!it->getThreadedExecution())
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_CANNOT_BE_CALLED_IF_DIDN_T_LAUNCH_THREAD,true);
	else
	{
		if (!it->getDelegateChildScriptExecution())
		{ // first time we set the flag!
			it->setDelegateChildScriptExecution();
			// Now handle all non-explicit child scripts one time here:
			CLuaObjectGroup inputArguments; // no input arguments
			CLuaObjectGroup outputArguments; // no output arguments
			std::vector<int> childScriptIDsToRun;

			C3DObject* obj=App::ct->objCont->getObject(it->getObjectIDThatScriptIsAttachedTo());
			if (obj!=NULL)
			{ // the script is attached to an object and is not passive
				for (int i=0;i<int(obj->childList.size());i++)
				{
					std::vector<int> tmp;
					obj->childList[i]->getChildScriptsToRun(tmp);
					for (int j=0;j<int(tmp.size());j++)
					{
						CLuaScriptObject* so=App::ct->luaScriptContainer->getScriptFromID(tmp[j]);
						if (!so->getExplicitHandling())
							childScriptIDsToRun.push_back(tmp[j]); // only non-explicit scripts!
					}
				}
			}
			LUA_UNLOCK;
			// Now we order all scripts according to their execution order settings:
			std::vector<int> firstToExecute;
			std::vector<int> normalToExecute;
			std::vector<int> lastToExecute;
			for (int i=0;i<int(childScriptIDsToRun.size());i++)
			{
				CLuaScriptObject* as=App::ct->luaScriptContainer->getScriptFromID(childScriptIDsToRun[i]);
				if (as!=NULL)
				{
					if (as->getExecutionOrder()==SCRIPT_EXECUTION_ORDER_FIRST)
						firstToExecute.push_back(childScriptIDsToRun[i]);
					if (as->getExecutionOrder()==SCRIPT_EXECUTION_ORDER_NORMAL)
						normalToExecute.push_back(childScriptIDsToRun[i]);
					if (as->getExecutionOrder()==SCRIPT_EXECUTION_ORDER_LAST)
						lastToExecute.push_back(childScriptIDsToRun[i]);
				}
			}
			childScriptIDsToRun.clear();
			childScriptIDsToRun.insert(childScriptIDsToRun.end(),firstToExecute.begin(),firstToExecute.end());
			childScriptIDsToRun.insert(childScriptIDsToRun.end(),normalToExecute.begin(),normalToExecute.end());
			childScriptIDsToRun.insert(childScriptIDsToRun.end(),lastToExecute.begin(),lastToExecute.end());

			// Before we run the scripts, we elevate the thread switch timing to 1 second (important!)
			int originalTiming;
			CThreadPool::getThreadSwitchTiming(originalTiming);
			CThreadPool::setThreadSwitchTiming(1000);

			// Now we run all the scripts:
			for (int i=0;i<int(childScriptIDsToRun.size());i++)
			{
				CLuaScriptObject* as=App::ct->luaScriptContainer->getScriptFromID(childScriptIDsToRun[i]);
				if (as!=NULL)
					as->runOneStep(inputArguments,outputArguments);
			}
			// Now restore the original switch timing:
			CThreadPool::setThreadSwitchTiming(originalTiming);
			LUA_LOCK; 
		}
		retVal=1;
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simWait(lua_State* L)
{ // can only be called from a script running in a thread!!
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simWait");
	if (VThread::isCurrentThreadTheMainThread())
	{
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD,true);
		lua_pushnil(L); // error
	}
	else
	{
		if (CThreadPool::getStopForCurrentInstanceRequested())
		{ // Important to first check if we are supposed to leave the thread!!
			lua_pushnil(L); // special error
			LUA_END(1);
		}
		if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		{
			float deltaTime=luaToFloat(L,1);
			int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_bool,0,true,false,true);
			if (res!=-1)
			{
				bool simulationTime=true;
				if (res==2)
					simulationTime=luaToBool(L,2);
				if (!simulationTime)
				{ // real-time wait
					CVThreadData* threadData=CThreadPool::getCurrentThreadData();
					int startTime=VDateTime::getTimeInMs();
					bool err=false;
					while (true)
					{
						float diff=float(VDateTime::getTimeDiffInMs(startTime))/1000.0f;
						if (diff>=deltaTime)
							break;
						LUA_UNLOCK; // Important!!
						CThreadPool::switchBackToPreviousThread();
						if (CThreadPool::getStopForCurrentInstanceRequested())
						{
							LUA_LOCK; // Important!!
							err=true;
							break;
						}
						LUA_LOCK; // Important!!
					}
					threadData->usedMovementTime=0.0f; // important!
					if (err)
						lua_pushnil(L); // error
					else
						lua_pushnumber(L,0.0f); // success (deltaTime left)
				}
				else
				{ // simulation time wait
					CVThreadData* threadData=CThreadPool::getCurrentThreadData();
					float startTime=App::ct->simulation->getSimulationTime()+threadData->usedMovementTime;
					float overshootTime=0.0f;
					bool err=false;
					while (true)
					{
						float currentTime=App::ct->simulation->getSimulationTime();
						float diff=currentTime-startTime;
						if (diff>=deltaTime)
						{
							overshootTime=diff-deltaTime; // this is the "overshoot" time!
							break;
						}
						LUA_UNLOCK; // Important!!
						CThreadPool::switchBackToPreviousThread();
						if (CThreadPool::getStopForCurrentInstanceRequested())
						{
							LUA_LOCK; // Important!!
							err=true;
							break;
						}
						LUA_LOCK; // Important!!
					}
					threadData->usedMovementTime=overshootTime; // important!
					if (err)
						lua_pushnil(L); // error
					else
						lua_pushnumber(L,-overshootTime); // success (deltaTime left)
				}
			}
			else
				lua_pushnil(L); // second argument error
		}
		else
			lua_pushnil(L); // error
	}
	LUA_END(1);
}

int _simReadForceSensor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simReadForceSensor");
	int retVal=-1;
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float force[3];
		float torque[3];
		int res=simReadForceSensor_internal(luaToInt(L,1),force,torque);
		lua_pushnumber(L,res);
		if ( (res==-1)||((res&1)==0) )
		{
			LUA_END(1);
		}
		pushFloatTableOntoStack(L,3,force);
		pushFloatTableOntoStack(L,3,torque);
		LUA_END(3);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simBreakForceSensor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simBreakForceSensor");
	int retVal=-1;
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simBreakForceSensor_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetShapeVertex(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetShapeVertex");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		float relPos[3];
		int retVal=simGetShapeVertex_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3),relPos);
		if (retVal!=1)
		{
			lua_pushnumber(L,retVal);
			LUA_END(1);
		}
		else
		{
			lua_pushnumber(L,retVal);
			pushFloatTableOntoStack(L,3,relPos);
			LUA_END(2);
		}
	}
	lua_pushnumber(L,-1);
	LUA_END(1);
}

int _simGetShapeTriangle(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetShapeTriangle");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int indices[3];
		float normals[9];
		int retVal=simGetShapeTriangle_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3),indices,normals);
		if (retVal!=1)
		{
			lua_pushnumber(L,retVal);
			LUA_END(1);
		}
		else
		{
			lua_pushnumber(L,retVal);
			pushIntTableOntoStack(L,3,indices);
			pushFloatTableOntoStack(L,9,normals);
			LUA_END(3);
		}
	}
	lua_pushnumber(L,-1);
	LUA_END(1);
}

int _simGetLightParameters(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetLightParameters");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float ambient[3];
		float diffuse[3];
		float specular[3];
		int retVal=simGetLightParameters_internal(luaToInt(L,1),ambient,diffuse,specular);
		lua_pushnumber(L,retVal);
		if (retVal>=0)
		{
			pushFloatTableOntoStack(L,3,ambient);
			pushFloatTableOntoStack(L,3,diffuse);
			pushFloatTableOntoStack(L,3,specular);
			LUA_END(4);
		}
		LUA_END(1);
	}
	lua_pushnumber(L,-1);
	LUA_END(1);
}

int _simSetLightParameters(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetLightParameters");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		int objHandle=luaToInt(L,1);
		int state=luaToInt(L,2);
		float* ambientP=NULL;
		float* diffuseP=NULL;
		float* specularP=NULL;
		float ambient_[3]={0.0f,0.0f,0.0f};
		float diffuse_[3]={0.0f,0.0f,0.0f};
		float specular_[3]={0.0f,0.0f,0.0f};
		int res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,3,true,true,true);
		if (res!=-1)
		{
			if (res==2)
			{ // get the data
				getFloatsFromTable(L,3,3,ambient_);
				ambientP=ambient_;
			}
			int res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,3,true,true,true);
			if (res!=-1)
			{
				if (res==2)
				{ // get the data
					getFloatsFromTable(L,4,3,diffuse_);
					diffuseP=diffuse_;
				}
				int res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,3,true,true,true);
				if (res!=-1)
				{
					if (res==2)
					{ // get the data
						getFloatsFromTable(L,5,3,specular_);
						specularP=specular_;
					}
					retVal=simSetLightParameters_internal(objHandle,state,ambientP,diffuseP,specularP);
				}
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simHandleVarious(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simHandleVarious");
	int retVal=simHandleVarious_internal();
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetVelocity(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetVelocity");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float linVel[3];
		float angVel[3];
		int retVal=simGetVelocity_internal(luaToInt(L,1),linVel,angVel);
		if (retVal!=-1)
		{
			pushFloatTableOntoStack(L,3,linVel);
			pushFloatTableOntoStack(L,3,angVel);
			LUA_END(2);
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simGetObjectVelocity(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectVelocity");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float linVel[3];
		float angVel[3];
		int retVal=simGetObjectVelocity_internal(luaToInt(L,1),linVel,angVel);
		if (retVal!=-1)
		{
			pushFloatTableOntoStack(L,3,linVel);
			pushFloatTableOntoStack(L,3,angVel);
			LUA_END(2);
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simAddForceAndTorque(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAddForceAndTorque");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float f[3]={0.0f,0.0f,0.0f};
		float t[3]={0.0f,0.0f,0.0f};
		bool err=false;
		int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_number,3,true,true,true);
		if (res==2)
			getFloatsFromTable(L,2,3,f);
		err|=(res<0);
		if (!err)
		{
			int res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,3,true,true,true);
			if (res==2)
				getFloatsFromTable(L,3,3,t);
			err|=(res<0);
			if (!err)
				retVal=simAddForceAndTorque_internal(luaToInt(L,1),f,t);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simAddForce(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAddForce");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,3,lua_arg_number,3))
	{
		float r[3];
		float f[3];
		getFloatsFromTable(L,2,3,r);
		getFloatsFromTable(L,3,3,f);
		retVal=simAddForce_internal(luaToInt(L,1),r,f);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetExplicitHandling(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetExplicitHandling");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetExplicitHandling_internal(luaToInt(L,1),luaToInt(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetExplicitHandling(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetExplicitHandling");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simGetExplicitHandling_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetLinkDummy(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetLinkDummy");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simGetLinkDummy_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetLinkDummy(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetLinkDummy");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetLinkDummy_internal(luaToInt(L,1),luaToInt(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetShapeColor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetShapeColor");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{ // First arg ok
		int shapeHandle=luaToInt(L,1);
		std::string strTmp;
		char* str=NULL;
		int colorComponent=0;
		bool err=false;
		int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_string,0,false,true,true);
		if (res==2)
		{
			strTmp=lua_tostring(L,2);
			str=(char*)strTmp.c_str();
		}
		err|=(res<1);
		if (!err)
		{
			res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,0,false,false,true);
			if (res==2)
				colorComponent=luaToInt(L,3);
			err|=(res<2);
			if (!err)
			{
				int floatsInTableExpected=3;
				if (colorComponent==4)
					floatsInTableExpected=1;
				res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,floatsInTableExpected,false,false,true);
				if (res==2)
				{
					float rgbData[3];
					getFloatsFromTable(L,4,floatsInTableExpected,rgbData);
					retVal=simSetShapeColor_internal(shapeHandle,str,colorComponent,rgbData);
				}
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetShapeColor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetShapeColor");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{ // First arg ok
		int shapeHandle=luaToInt(L,1);
		std::string strTmp;
		char* str=NULL;
		int colorComponent=0;
		bool err=false;
		int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_string,0,false,true,true);
		if (res==2)
		{
			strTmp=lua_tostring(L,2);
			str=(char*)strTmp.c_str();
		}
		err|=(res<1);
		if (!err)
		{
			res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,0,false,false,true);
			if (res==2)
				colorComponent=luaToInt(L,3);
			err|=(res<2);
			if (!err)
			{
				float rgbData[3];
				retVal=simGetShapeColor_internal(shapeHandle,str,colorComponent,rgbData);
				if (retVal>0)
				{
					lua_pushnumber(L,retVal);
					pushFloatTableOntoStack(L,3,rgbData);
					LUA_END(2);
				}
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simResetDynamicObject(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simResetDynamicObject");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simResetDynamicObject_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetJointMode(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetJointMode");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetJointMode_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetJointMode(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetJointMode");
	int retVal=-1; // means error
	int options=0;
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simGetJointMode_internal(luaToInt(L,1),&options);
	lua_pushnumber(L,retVal);
	lua_pushnumber(L,options);
	LUA_END(2);
}

int _simSerialOpen(lua_State* L)
{ 
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSerialOpen");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0,lua_arg_number,0))
	{
		CSerialPort* port=new CSerialPort(true);
		size_t dataLength;
		const char* data=lua_tolstring(L,1,&dataLength);
		if (port->open(data,luaToInt(L,2)))
		{
			retVal=App::ct->serialPortContainer->addOpenedPort(port);
		}
		else
			delete port;
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSerialClose(lua_State* L)
{ 
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSerialClose");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simSerialClose_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSerialSend(lua_State* L)
{ 
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSerialSend");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0))
	{
		size_t dataLength;
		char* data=(char*)lua_tolstring(L,2,&dataLength);
		retVal=simSerialSend_internal(luaToInt(L,1),data,dataLength);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSerialRead(lua_State* L)
{ 
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSerialRead");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_bool,0))
	{
		bool blocking=luaToBool(L,3);
		std::string closingString="";
		float timeOut=0.0f;
		if (blocking)
		{
			if (VThread::isCurrentThreadTheMainThread())
			{
				outputCallErrorMessage(functionName.c_str(),SIM_ERROR_BLOCKING_OPERATION_ONLY_FROM_THREAD,true);
				lua_pushnil(L);
				LUA_END(1);
			}
			
			int res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_string,0,true,true,true);
			if (res==2)
			{
				size_t dataLength;
				char* data=(char*)lua_tolstring(L,4,&dataLength);
				closingString.assign(data,dataLength);
			}
			if (res==-1)
			{
				lua_pushnil(L);
				LUA_END(1);
			}
	
			res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,0,true,false,true);
			if (res==2)
				timeOut=luaToFloat(L,5);
			if (res==-1)
			{
				lua_pushnil(L);
				LUA_END(1);
			}
		}
		int maxLength=luaToInt(L,2);
		char* data=new char[maxLength];
		int dataRead=0;
		if (blocking)
		{
			int startTime=VDateTime::getTimeInMs();
			LUA_UNLOCK; // Important!!
			while (maxLength>dataRead)
			{
				int nb=simSerialRead_internal(luaToInt(L,1),data+dataRead,maxLength-dataRead);
				if (nb<0)
					break; // error
				dataRead+=nb;
				int csl=int(closingString.length());
				if ( (csl!=0)&&(dataRead>=csl) )
				{ // Break if we find the closing signature:
					bool same=true;
					for (int i=0;i<csl;i++)
					{
						if (data[dataRead-csl+i]!=closingString[i])
						{
							same=false;
							break;
						}
					}
					if (same)
					{
						dataRead-=csl;
						break;
					}
				}
				if (maxLength>dataRead)
				{
					CThreadPool::switchBackToPreviousThread();
					if (CThreadPool::getStopForCurrentInstanceRequested())
						break;
				}
				if ( (timeOut>0.0000001f)&&((float(VDateTime::getTimeDiffInMs(startTime))/1000.0f)>timeOut) )
					break;
			}
			LUA_LOCK; // Important!!
		}
		else
			dataRead=simSerialRead_internal(luaToInt(L,1),data,maxLength);
		if (dataRead>0)
			lua_pushlstring(L,(const char*)data,dataRead);
		else
			lua_pushnil(L);
		delete[] data;
		LUA_END(1);
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simSerialCheck(lua_State* L)
{ 
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSerialCheck");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simSerialCheck_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetContactInfo(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetContactInfo");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int collidingObjects[2];
		float contactInfo[6];
		int res=simGetContactInfo_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3),collidingObjects,contactInfo);
		if (res==1)
		{
			pushIntTableOntoStack(L,2,collidingObjects);
			pushFloatTableOntoStack(L,3,contactInfo);
			pushFloatTableOntoStack(L,3,contactInfo+3);
			LUA_END(3);
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simSetThreadIsFree(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	// VERY SPECIAL HANDLING FOR THIS FUNCTION!!!!!!!!
	if (VThread::isCurrentThreadTheMainThread())
	{
		LUA_START("simSetThreadIsFree");
		CApiErrors::setLastErrorMessage(functionName.c_str(),SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD);
		lua_pushnumber(L,-1);
		LUA_END(1);
	}

	memorizeLocation(L);
	std::string functionName("simSetThreadIsFree");
	bool result=false;
	if (checkInputArguments(L,functionName,true,lua_arg_bool,0))
	{
		result=CThreadPool::setThreadFreeMode(luaToBool(L,1));
	}

	forgetLocation();
	if (result)
		lua_pushnumber(L,1);
	else
		lua_pushnumber(L,0);
	return(1);
}

int _simTubeOpen(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simTubeOpen");
	int retVal=-1; // Error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0,lua_arg_number,0))
	{
		std::string strTmp=lua_tostring(L,2);
		int currentScriptID=getCurrentScriptID(L);
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
		retVal=App::ct->commTubeContainer->openTube(luaToInt(L,1),strTmp.c_str(),(it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript),luaToInt(L,3));
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simTubeClose(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simTubeClose");
	int retVal=-1; // Error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simTubeClose_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simTubeWrite(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simTubeWrite");
	int retVal=-1; // Error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0))
	{
		size_t dataLength;
		char* data=(char*)lua_tolstring(L,2,&dataLength);
		retVal=simTubeWrite_internal(luaToInt(L,1),data,dataLength);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simTubeRead(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simTubeRead");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int dataLength;
		char* data=NULL;
		bool blocking=false;
		int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_bool,0,true,false,true);
		if (res==2)
			blocking=luaToBool(L,2);
		if (blocking)
		{
			CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(getCurrentScriptID(L));
			if (it!=NULL)
			{
				if (VThread::isCurrentThreadTheMainThread())
				{
					outputCallErrorMessage(functionName.c_str(),SIM_ERROR_BLOCKING_OPERATION_ONLY_FROM_THREAD,true);
					lua_pushnil(L);
				}
				else
				{
					data=simTubeRead_internal(luaToInt(L,1),&dataLength);
					while (data==NULL)
					{
						LUA_UNLOCK; // Important!!
						// Now wait here until a button was pressed! (or the simulation is aborted)
						CThreadPool::switchBackToPreviousThread();
						if (CThreadPool::getStopForCurrentInstanceRequested())
						{
							LUA_LOCK; // Important!!
							break;
						}
						LUA_LOCK; // Important!!
						data=simTubeRead_internal(luaToInt(L,1),&dataLength);
					}
					if (data==NULL)
						lua_pushnil(L);
					else
						lua_pushlstring(L,(const char*)data,dataLength);
				}
			}
			else
				lua_pushnil(L);
		}
		else
		{
			data=simTubeRead_internal(luaToInt(L,1),&dataLength);
			if (data)
				lua_pushlstring(L,(const char*)data,dataLength);
			else
				lua_pushnil(L);
		}
		delete[] data;
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simTubeStatus(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simTubeStatus");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int readSize;
		int writeSize;
		int status=simTubeStatus_internal(luaToInt(L,1),&readSize,&writeSize);
		if (status<0)
		{
			lua_pushnil(L);
			LUA_END(1);
		}
		lua_pushnumber(L,status);
		lua_pushnumber(L,readSize);
		lua_pushnumber(L,writeSize);
		LUA_END(3);
	}
	else
	{
		lua_pushnil(L);
		LUA_END(1);
	}
}

int _simAuxiliaryConsoleOpen(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAuxiliaryConsoleOpen");
	int retVal=-1; // Error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0,lua_arg_number,0,lua_arg_number,0))
	{
		int mode=luaToInt(L,3);
		int currentScriptID=getCurrentScriptID(L);
		CLuaScriptObject* itScrObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
		if ( (itScrObj->getScriptType()==sim_scripttype_mainscript)||(itScrObj->getScriptType()==sim_scripttype_childscript) )
		{ // Add-ons do not have this restriction
			if ((mode&1)==0)
				mode++;
		}
		int* p=NULL;
		int* s=NULL;
		float* tc=NULL;
		float* bc=NULL;
//		bool err=false;
		int res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,2,true,true,true);
		if (res>=0)
		{
			int pp[2]={0,0};
			if (res==2)
			{
				getIntsFromTable(L,4,2,pp);
				p=pp;
			}
			res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,2,true,true,true);
			if (res>=0)
			{
				int ss[2]={0,0};
				if (res==2)
				{
					getIntsFromTable(L,5,2,ss);
					s=ss;
				}
				res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,3,true,true,true);
				if (res>=0)
				{
					float tc_[3]={0.0f,0.0f,0.0f};
					if (res==2)
					{
						getFloatsFromTable(L,6,3,tc_);
						tc=tc_;
					}
					res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,3,true,true,true);
					if (res>=0)
					{
						float bc_[3]={0.0f,0.0f,0.0f};
						if (res==2)
						{
							getFloatsFromTable(L,7,3,bc_);
							bc=bc_;
						}
						retVal=simAuxiliaryConsoleOpen_internal(lua_tostring(L,1),luaToInt(L,2),mode,p,s,tc,bc);
					}
				}
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simAuxiliaryConsoleClose(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAuxiliaryConsoleClose");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simAuxiliaryConsoleClose_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simAuxiliaryConsoleShow(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAuxiliaryConsoleShow");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_bool,0))
		retVal=simAuxiliaryConsoleShow_internal(luaToInt(L,1),luaToBool(L,2));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simAuxiliaryConsolePrint(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAuxiliaryConsolePrint");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
//		bool err=false;
		int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_string,0,false,true,true);
		if (res==1)
			retVal=simAuxiliaryConsolePrint_internal(luaToInt(L,1),NULL);
		if (res==2)
			retVal=simAuxiliaryConsolePrint_internal(luaToInt(L,1),lua_tostring(L,2));
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simImportShape(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simImportShape");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int fileType=luaToInt(L,1);
		std::string pathAndFilename(lua_tostring(L,2));
		int options=luaToInt(L,3);
		float identicalVerticeTolerance=luaToFloat(L,4);
		float scalingFactor=luaToFloat(L,5);
		retVal=simImportShape_internal(fileType,pathAndFilename.c_str(),options,identicalVerticeTolerance,scalingFactor);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simImportMesh(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simImportMesh");
	int retValCnt=1;
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int fileType=luaToInt(L,1);
		std::string pathAndFilename(lua_tostring(L,2));
		int options=luaToInt(L,3);
		float identicalVerticeTolerance=luaToFloat(L,4);
		float scalingFactor=luaToFloat(L,5);
		float** vertices;
		int* verticesSizes;
		int** indices;
		int* indicesSizes;
		char** names;
		int elementCount=simImportMesh_internal(fileType,pathAndFilename.c_str(),options,identicalVerticeTolerance,scalingFactor,&vertices,&verticesSizes,&indices,&indicesSizes,NULL,&names);

		if (elementCount>0)
		{
			// Vertices:
			lua_newtable(L);						
			int tablePos=lua_gettop(L);
			for (int i=0;i<elementCount;i++)
			{
				lua_newtable(L);						
				int smallTablePos=lua_gettop(L);
				for (int j=0;j<verticesSizes[i];j++)
				{
					lua_pushnumber(L,vertices[i][j]);
					lua_rawseti(L,smallTablePos,j+1);
				}
				lua_rawseti(L,tablePos,i+1);
			}
			// Indices:
			lua_newtable(L);						
			tablePos=lua_gettop(L);
			for (int i=0;i<elementCount;i++)
			{
				lua_newtable(L);						
				int smallTablePos=lua_gettop(L);
				for (int j=0;j<indicesSizes[i];j++)
				{
					lua_pushnumber(L,indices[i][j]);
					lua_rawseti(L,smallTablePos,j+1);
				}
				lua_rawseti(L,tablePos,i+1);
			}
			// For future:
			lua_pushnil(L);

			// Names:
			lua_newtable(L);						
			tablePos=lua_gettop(L);
			for (int i=0;i<elementCount;i++)
			{
				lua_pushstring(L,names[i]);
				lua_rawseti(L,tablePos,i+1);
			}
			retValCnt=4;

			for (int i=0;i<elementCount;i++)
			{
				delete[] names[i];
				delete[] indices[i];
				delete[] vertices[i];
			}
			delete[] names;
			delete[] indicesSizes;
			delete[] indices;
			delete[] verticesSizes;
			delete[] vertices;

		}
		else
			lua_pushnil(L);

	}
	else
		lua_pushnil(L);
	LUA_END(retValCnt);
}

int _simExportMesh(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simExportMesh");
	int retVal=-1; // indicates an error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0,lua_arg_number,0,lua_arg_number,0))
	{
		int fileType=luaToInt(L,1);
		std::string pathAndFilename(lua_tostring(L,2));
		int options=luaToInt(L,3);
		float scalingFactor=luaToFloat(L,4);
		int elementCount=15487;
		if ( (lua_gettop(L)>=8)&&lua_istable(L,5)&&lua_istable(L,6)&&lua_isnil(L,7)&&lua_istable(L,8) )
		{
            int ve=VREP_LUA_OBJLEN(L,5);
            int ie=VREP_LUA_OBJLEN(L,6);
            int nameE=VREP_LUA_OBJLEN(L,8);
			elementCount=SIM_MIN(SIM_MIN(ve,ie),nameE);
		}
		if ( (checkOneGeneralInputArgument(L,5,functionName,lua_arg_table,elementCount,false,false,true)==2)&&
				(checkOneGeneralInputArgument(L,6,functionName,lua_arg_table,elementCount,false,false,true)==2)&&
				(checkOneGeneralInputArgument(L,7,functionName,lua_arg_nil,0,false,false,true)==2)&&
				(checkOneGeneralInputArgument(L,8,functionName,lua_arg_string,elementCount,false,false,true)==2) )
		{
			float** vertices;
			int* verticesSizes;	
			int** indices;
			int* indicesSizes;
			char** names;
			vertices=new float*[elementCount];
			verticesSizes=new int[elementCount];
			indices=new int*[elementCount];
			indicesSizes=new int[elementCount];
			names=new char*[elementCount];
			// Following needed if we break before (because one table in a table is not correct (or not a table)):
			for (int i=0;i<elementCount;i++)
			{
				vertices[i]=NULL;
				verticesSizes[i]=0;
				indices[i]=NULL;
				indicesSizes[i]=0;
				names[i]=NULL;
			}

			bool error=false;
			for (int i=0;i<elementCount;i++)
			{
				if (!error)
				{
					lua_rawgeti(L,5,i+1);
					if (lua_istable(L,-1))
					{
                        int vl=VREP_LUA_OBJLEN(L,-1);
						if (checkOneGeneralInputArgument(L,lua_gettop(L),functionName,lua_arg_number,vl,false,false,true)==2)
						{
							verticesSizes[i]=vl;
							vertices[i]=new float[vl];
							getFloatsFromTable(L,lua_gettop(L),vl,vertices[i]);
						}
						else
							error=true;
					}
					else
						error=true;
					lua_pop(L,1); // we have to pop the value that was pushed with lua_rawgeti
				}
				if (!error)
				{
					lua_rawgeti(L,6,i+1);
					if (lua_istable(L,-1))
					{
                        int vl=VREP_LUA_OBJLEN(L,-1);
						if (checkOneGeneralInputArgument(L,lua_gettop(L),functionName,lua_arg_number,vl,false,false,true)==2)
						{
							indicesSizes[i]=vl;
							indices[i]=new int[vl];
							getIntsFromTable(L,lua_gettop(L),vl,indices[i]);
						}
						else
							error=true;
					}
					else
						error=true;
					lua_pop(L,1); // we have to pop the value that was pushed with lua_rawgeti
				}
				if (!error)
				{
					lua_rawgeti(L,8,i+1);
					std::string name(lua_tostring(L,-1));
					names[i]=new char[name.length()+1];
					for (int j=0;j<int(name.length());j++)
						names[i][j]=name[j];
					names[i][name.length()]=0; // terminal 0
					lua_pop(L,1); // we have to pop the value that was pushed with lua_rawgeti
				}
			}

			if (!error)
				retVal=simExportMesh_internal(fileType,pathAndFilename.c_str(),options,scalingFactor,elementCount,vertices,verticesSizes,indices,indicesSizes,NULL,names);

			for (int i=0;i<elementCount;i++)
			{
				delete[] vertices[i];
				delete[] indices[i];
				delete[] names[i];
			}
			delete[] names;
			delete[] indicesSizes;
			delete[] indices;
			delete[] verticesSizes;
			delete[] vertices;
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}



int _simCreateMeshShape(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCreateMeshShape");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		int options=luaToInt(L,1);
		float shadingAngle=luaToFloat(L,2);

		int vl=2;
		int il=2;
		if ( (lua_gettop(L)>=4)&&lua_istable(L,3)&&lua_istable(L,4) )
		{
            vl=VREP_LUA_OBJLEN(L,3);
            il=VREP_LUA_OBJLEN(L,4);
		}
//		bool err=false;
		int res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,vl,false,false,true);
		if (res==2)
		{
			res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,il,false,false,true);
			if (res==2)
			{
				float* vertices=new float[vl];
				int* indices=new int[il];
				getFloatsFromTable(L,3,vl,vertices);
				getIntsFromTable(L,4,il,indices);
				retVal=simCreateMeshShape_internal(options,shadingAngle,vertices,vl,indices,il,NULL);
				delete[] indices;
				delete[] vertices;
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetShapeMesh(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetShapeMesh");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float* vertices;
		int verticesSize;
		int* indices;
		int indicesSize;
		float* normals;
		int result=simGetShapeMesh_internal(luaToInt(L,1),&vertices,&verticesSize,&indices,&indicesSize,&normals);
		if (result>0)
		{
			pushFloatTableOntoStack(L,verticesSize,vertices);
			pushIntTableOntoStack(L,indicesSize,indices);
			pushFloatTableOntoStack(L,indicesSize*3,normals);
			delete[] vertices;
			delete[] indices;
			delete[] normals;
			LUA_END(3);
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simCreatePureShape(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCreatePureShape");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,3,lua_arg_number,0))
	{
		int primitiveType=luaToInt(L,1);
		int options=luaToInt(L,2);
		float sizes[3];
		getFloatsFromTable(L,3,3,sizes);
		float mass=luaToFloat(L,4);
		int* precision=NULL;
		int prec[2];
		int res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,2,true,true,true);
		if (res>=0)
		{
			if (res==2)
			{
				getIntsFromTable(L,5,2,prec);
				precision=prec;
			}
			retVal=simCreatePureShape_internal(primitiveType,options,sizes,mass,precision);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCreateHeightfieldShape(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCreateHeightfieldShape");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int options=luaToInt(L,1);
		float shadingAngle=luaToFloat(L,2);
		int xPointCount=luaToInt(L,3);
		int yPointCount=luaToInt(L,4);
		float xSize=luaToFloat(L,5);
		int res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,xPointCount*yPointCount,false,false,true);
		if (res==2)
		{
			float* heights=new float[xPointCount*yPointCount];
			getFloatsFromTable(L,6,xPointCount*yPointCount,heights);
			retVal=simCreateHeightfieldShape_internal(options,shadingAngle,xPointCount,yPointCount,xSize,heights);
			delete[] heights;
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}


int _simAddBanner(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAddBanner");
	int retVal=-1; // means error

	if (checkInputArguments(L,functionName,true,lua_arg_string,0,lua_arg_number,0,lua_arg_number))
	{
		std::string label(lua_tostring(L,1));
		float size=luaToFloat(L,2);
		int options=luaToInt(L,3);
		
		float* positionAndEulerAngles=NULL;
		int parentObjectHandle=-1;
		float* labelColors=NULL;
		float* backgroundColors=NULL;

		int res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,6,true,true,true);
		int okToGo=(res!=-1);
		if (okToGo)
		{
			float positionAndEulerAnglesC[6];
			if (res>0)
			{
				if (res==2)
				{
					getFloatsFromTable(L,4,6,positionAndEulerAnglesC);
					positionAndEulerAngles=positionAndEulerAnglesC;
				}
				res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,0,true,true,true);
				okToGo=(res!=-1);
				if (okToGo)
				{
					if (res>0)
					{
						if (res==2)
							parentObjectHandle=luaToInt(L,5);
						res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,12,true,true,true);
						okToGo=(res!=-1);
						if (okToGo)
						{
							float labelColorsC[12];
							if (res>0)
							{
								if (res==2)
								{
									getFloatsFromTable(L,6,12,labelColorsC);
									labelColors=labelColorsC;
								}
								res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,12,true,true,true);
								okToGo=(res!=-1);
								if (okToGo)
								{
									float backgroundColorsC[12];
									if (res>0)
									{
										if (res==2)
										{
											getFloatsFromTable(L,7,12,backgroundColorsC);
											backgroundColors=backgroundColorsC;
										}
									}
								}
							}
						}
					}
				}
			}
			if (okToGo)
			{
				retVal=simAddBanner_internal(label.c_str(),size,options,positionAndEulerAngles,parentObjectHandle,labelColors,backgroundColors);
				if (retVal!=-1)
				{ // following condition added on 2011/01/06 so as to not remove objects created from the c/c++ interface or an add-on:
					int currentScriptID=getCurrentScriptID(L);
					CLuaScriptObject* itScrObj=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
					CBannerObject* anObj=App::ct->bannerCont->getObject(retVal);
					if (anObj!=NULL)
						anObj->setCreatedFromScript((itScrObj->getScriptType()==sim_scripttype_mainscript)||(itScrObj->getScriptType()==sim_scripttype_childscript));
				}
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simRemoveBanner(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simRemoveBanner");
	int retVal=-1; // means error

	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int objectHandle=luaToInt(L,1);
		if (objectHandle==sim_handle_all)
		{ // following condition added here on 2011/01/06 so as not to remove objects created from a C/c++ call
			App::ct->bannerCont->removeAllObjects(true);
			retVal=1;
		}
		else
			retVal=simRemoveBanner_internal(objectHandle);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCreateJoint(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCreateJoint");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int jointType=luaToInt(L,1);
		int jointMode=luaToInt(L,2);
		int options=luaToInt(L,3);
		float* sizes=NULL;
		float* colorA=NULL;
		float* colorB=NULL;
		float s[2];
		float cA[12];
		float cB[12];
		int res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,2,true,true,true);
		if (res>=0)
		{
			if (res==2)
			{
				getFloatsFromTable(L,4,2,s);
				sizes=s;
			}
			res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,12,true,true,true);
			if (res>=0)
			{
				if (res==2)
				{
					getFloatsFromTable(L,5,12,cA);
					colorA=cA;
				}
				res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,12,true,true,true);
				if (res>=0)
				{
					if (res==2)
					{
						getFloatsFromTable(L,6,12,cB);
						colorB=cB;
					}
					retVal=simCreateJoint_internal(jointType,jointMode,options,sizes,colorA,colorB);
				}
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCreateDummy(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCreateDummy");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float size=luaToFloat(L,1);
		float* color=NULL;
		float c[12];
		int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_number,12,true,true,true);
		if (res>=0)
		{
			if (res==2)
			{
				getFloatsFromTable(L,2,12,c);
				color=c;
			}
			retVal=simCreateDummy_internal(size,color);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCreatePath(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCreatePath");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int attribs=luaToInt(L,1);
		int* intP=NULL;
		float* floatP=NULL;
		int intParams[3];
		float floatParams[3];
		float* color=NULL;
		float c[12];
		int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_number,3,true,true,true);
		if (res>=0)
		{
			if (res==2)
			{
				getIntsFromTable(L,2,3,intParams);
				intP=intParams;
			}
			res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,3,true,true,true);
			if (res>=0)
			{
				if (res==2)
				{
					getFloatsFromTable(L,3,3,floatParams);
					floatP=floatParams;
				}
				res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,12,true,true,true);
				if (res>=0)
				{
					if (res==2)
					{
						getFloatsFromTable(L,4,12,c);
						color=c;
					}
					retVal=simCreatePath_internal(attribs,intP,floatP,color);
				}
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCreateProximitySensor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCreateProximitySensor");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,8,lua_arg_number,15))
	{
		int sensorType=luaToInt(L,1);
		int subType=luaToInt(L,2);
		int options=luaToInt(L,3);
		int intParams[8];
		float floatParams[15];
		getIntsFromTable(L,4,8,intParams);
		getFloatsFromTable(L,5,15,floatParams);

		float* color=NULL;
		float c[48];
		int res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,48,true,true,true);
		if (res>=0)
		{
			if (res==2)
			{
				getFloatsFromTable(L,6,48,c);
				color=c;
			}
			retVal=simCreateProximitySensor_internal(sensorType,subType,options,intParams,floatParams,color);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCreateForceSensor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCreateForceSensor");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,5,lua_arg_number,5))
	{
		int options=luaToInt(L,1);
		int intParams[5];
		float floatParams[5];
		getIntsFromTable(L,2,5,intParams);
		getFloatsFromTable(L,3,5,floatParams);

		float* color=NULL;
		float c[24];
		int res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,24,true,true,true);
		if (res>=0)
		{
			if (res==2)
			{
				getFloatsFromTable(L,4,24,c);
				color=c;
			}
			retVal=simCreateForceSensor_internal(options,intParams,floatParams,color);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCreateVisionSensor(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCreateVisionSensor");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,4,lua_arg_number,11))
	{
		int options=luaToInt(L,1);
		int intParams[4];
		float floatParams[11];
		getIntsFromTable(L,2,4,intParams);
		getFloatsFromTable(L,3,11,floatParams);

		float* color=NULL;
		float c[48];
		int res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,48,true,true,true);
		if (res>=0)
		{
			if (res==2)
			{
				getFloatsFromTable(L,4,48,c);
				color=c;
			}
			retVal=simCreateVisionSensor_internal(options,intParams,floatParams,color);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simFloatingViewAdd(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simFloatingViewAdd");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
		retVal=simFloatingViewAdd_internal(luaToFloat(L,1),luaToFloat(L,2),luaToFloat(L,3),luaToFloat(L,4),luaToInt(L,5));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simFloatingViewRemove(lua_State* L)
{ 
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simFloatingViewRemove");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simFloatingViewRemove_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simAdjustView(lua_State* L)
{ 
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAdjustView");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_string,0,true,true,true);
		if (res>=0)
		{
			char* txt=NULL;
			if (res==2)
				txt=(char*)lua_tostring(L,4);
			if ( (txt!=NULL)&&(strlen(txt)>0) )
				retVal=simAdjustView_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3),txt);
			else
				retVal=simAdjustView_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3),NULL);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCameraFitToView(lua_State* L)
{ 
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCameraFitToView");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
//		int objCnt=0;
		int* objPtr=NULL;
		int options=0;
		float scaling=1.0f;
		int tableLen=2;
		if (lua_istable(L,2))
		{
            tableLen=int(VREP_LUA_OBJLEN(L,2));
			int* buffer=new int[tableLen];
			objPtr=buffer;
			getIntsFromTable(L,2,tableLen,buffer);
		}
		int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_number,tableLen,true,true,true);
		if (res>=0)
		{
			res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,0,true,false,true);
			if (res>=0)
			{
				if (res==2)
					options=luaToInt(L,3);
				res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,0,true,false,true);
				if (res>=0)
				{
					if (res==2)
						scaling=luaToFloat(L,4);
					retVal=simCameraFitToView_internal(luaToInt(L,1),tableLen,objPtr,options,scaling);
				}
			}
		}
		delete[] objPtr;
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetObjectIntParameter(lua_State* L)
{ 
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectIntParameter");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		int param;
		retVal=simGetObjectIntParameter_internal(luaToInt(L,1),luaToInt(L,2),&param);
		if (retVal>0)
		{
			lua_pushnumber(L,retVal);
			lua_pushnumber(L,param);
			LUA_END(2);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetObjectIntParameter(lua_State* L)
{ 
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetObjectIntParameter");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetObjectIntParameter_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}


int _simGetObjectFloatParameter(lua_State* L)
{ 
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectFloatParameter");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		float param;
		retVal=simGetObjectFloatParameter_internal(luaToInt(L,1),luaToInt(L,2),&param);
		if (retVal>0)
		{
			lua_pushnumber(L,retVal);
			lua_pushnumber(L,param);
			LUA_END(2);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetObjectFloatParameter(lua_State* L)
{ 
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetObjectFloatParameter");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
		retVal=simSetObjectFloatParameter_internal(luaToInt(L,1),luaToInt(L,2),luaToFloat(L,3));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetObjectStringParameter(lua_State* L)
{ 
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectStringParameter");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		int paramLength;
		char* strBuff=simGetObjectStringParameter_internal(luaToInt(L,1),luaToInt(L,2),&paramLength);
		if (strBuff!=NULL)
		{
			lua_pushlstring(L,strBuff,paramLength);
			delete[] strBuff;
			LUA_END(1);
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simSetObjectStringParameter(lua_State* L)
{ 
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetObjectStringParameter");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_string,0))
	{
		size_t dataLength;
		char* data=(char*)lua_tolstring(L,3,&dataLength);
		retVal=simSetObjectStringParameter_internal(luaToInt(L,1),luaToInt(L,2),data,dataLength);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetRotationAxis(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetRotationAxis");
	if (checkInputArguments(L,functionName,true,lua_arg_number,12,lua_arg_number,12))
	{
		float inM0[12];
		float inM1[12];
		float outAxis[3];
		float angle;
		getFloatsFromTable(L,1,12,inM0);
		getFloatsFromTable(L,2,12,inM1);
		if (simGetRotationAxis_internal(inM0,inM1,outAxis,&angle)!=-1)
		{
			pushFloatTableOntoStack(L,3,outAxis);
			lua_pushnumber(L,angle);
			LUA_END(2);
		}
		else
			lua_pushnil(L); // error
	}
	else
		lua_pushnil(L); // error
	LUA_END(1);
}

int _simRotateAroundAxis(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simRotateAroundAxis");
	if (checkInputArguments(L,functionName,true,lua_arg_number,12,lua_arg_number,3,lua_arg_number,3,lua_arg_number,0))
	{
		float inM[12];
		float axis[3];
		float pos[3];
		float outM[12];
		getFloatsFromTable(L,1,12,inM);
		getFloatsFromTable(L,2,3,axis);
		getFloatsFromTable(L,3,3,pos);
		if (simRotateAroundAxis_internal(inM,axis,pos,luaToFloat(L,4),outM)!=-1)
			pushFloatTableOntoStack(L,12,outM);
		else
			lua_pushnil(L); // error
	}
	else
		lua_pushnil(L); // error
	LUA_END(1);
}

int _simGetJointForce(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetJointForce");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float jointF[1];
		if (simGetJointForce_internal(luaToInt(L,1),jointF)!=-1)
			lua_pushnumber(L,jointF[0]);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simSetIkGroupProperties(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetIkGroupProperties");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		retVal=simSetIkGroupProperties_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3),luaToFloat(L,4),NULL);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetIkElementProperties(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetIkElementProperties");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,2,true,true,true);
		if (res>=0)
		{
			float* precision=NULL;
			float prec[2];
			if (res==2)
			{
				getFloatsFromTable(L,4,2,prec);
				precision=prec;
			}
			res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,2,true,true,true);
			if (res>=0)
			{
				float* weight=NULL;
				float w[2];
				if (res==2)
				{
					getFloatsFromTable(L,5,2,w);
					weight=w;
				}
				retVal=simSetIkElementProperties_internal(luaToInt(L,1),luaToInt(L,2),luaToInt(L,3),precision,weight,NULL);
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simAddSceneCustomData(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAddSceneCustomData");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0))
	{
		int headerNumber=luaToInt(L,1);
		size_t dataLength;
		char* data=(char*)lua_tolstring(L,2,&dataLength);
		retVal=simAddSceneCustomData_internal(headerNumber,data,dataLength);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetSceneCustomData(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetSceneCustomData");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int headerNumber=luaToInt(L,1);
		int dataLength=simGetSceneCustomDataLength_internal(headerNumber);
		if (dataLength>=0)
		{
			char* data=new char[dataLength];
			int retVal=simGetSceneCustomData_internal(headerNumber,data);
			if (retVal==-1)
			{
				delete[] data;
				lua_pushnil(L);
				LUA_END(1);
			}
			else
			{
				lua_pushlstring(L,data,dataLength);
				delete[] data;
				LUA_END(1);
			}
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}


int _simAddObjectCustomData(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAddObjectCustomData");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_string,0))
	{
		int objectHandle=luaToInt(L,1);
		int headerNumber=luaToInt(L,2);
		size_t dataLength;
		char* data=(char*)lua_tolstring(L,3,&dataLength);
		retVal=simAddObjectCustomData_internal(objectHandle,headerNumber,data,dataLength);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}


int _simGetObjectCustomData(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectCustomData");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		int objectHandle=luaToInt(L,1);
		int headerNumber=luaToInt(L,2);
		int dataLength=simGetObjectCustomDataLength_internal(objectHandle,headerNumber);
		if (dataLength>=0)
		{
			char* data=new char[dataLength];
			int retVal=simGetObjectCustomData_internal(objectHandle,headerNumber,data);
			if (retVal==-1)
			{
				delete[] data;
				lua_pushnil(L);
				LUA_END(1);
			}
			else
			{
				lua_pushlstring(L,data,dataLength);
				delete[] data;
				LUA_END(1);
			}
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}


int _simPersistentDataWrite(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simPersistentDataWrite");
	int retVal=-1; //error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0,lua_arg_string,0))
	{
		int options=0;
		int res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,0,true,false,true);
		if ((res==0)||(res==2))
		{
			if (res==2)
				options=luaToInt(L,3);
			size_t dataLength;
			char* data=(char*)lua_tolstring(L,2,&dataLength);
			retVal=simPersistentDataWrite_internal(std::string(lua_tostring(L,1)).c_str(),data,dataLength,options);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simPersistentDataRead(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simPersistentDataRead");
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		int stringLength;
		char* str=simPersistentDataRead_internal(std::string(lua_tostring(L,1)).c_str(),&stringLength);

		if (str!=NULL)
		{
			lua_pushlstring(L,str,stringLength);
			simReleaseBuffer_internal(str);
		}
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simIsHandleValid(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simIsHandleValid");
	int retVal=-1; //error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int objType=-1;
		int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_number,0,true,false,true);
		if ((res==0)||(res==2))
		{
			if (res==2)
				objType=luaToInt(L,2);
			retVal=simIsHandleValid_internal(luaToInt(L,1),objType);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simRMLPos(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simRMLPos");
	int retVal=-43; //error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int dofs=luaToInt(L,1);
		double timeStep=lua_tonumber(L,2);
		int flags=luaToInt(L,3);
		int res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,dofs*3,false,false,true);
		if (res==2)
		{
			double* currentPosVelAccel=new double[dofs*3];
			getDoublesFromTable(L,4,dofs*3,currentPosVelAccel);
			res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,dofs*3,false,false,true);
			if (res==2)
			{
				double* maxVelAccelJerk=new double[dofs*3];
				getDoublesFromTable(L,5,dofs*3,maxVelAccelJerk);
				res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_bool,dofs,false,false,true);
				if (res==2)
				{
					char* selection=new char[dofs];
					getBoolsFromTable(L,6,dofs,selection);
					res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,dofs*2,false,false,true);
					if (res==2)
					{
						double* targetPosVel=new double[dofs*2];
						getDoublesFromTable(L,7,dofs*2,targetPosVel);

						BYTE auxData[1+4];
						auxData[0]=1;
						((int*)(auxData+1))[0]=0;

						int currentScriptID=getCurrentScriptID(L);
						CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
						if ((it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript))
							((int*)(auxData+1))[0]=1; // destroy at simulation end!
						retVal=simRMLPos_internal(dofs,timeStep,flags,currentPosVelAccel,maxVelAccelJerk,(unsigned char*)selection,targetPosVel,auxData);
						delete[] targetPosVel;
					}
					delete[] selection;
				}
				delete[] maxVelAccelJerk;
			}
			delete[] currentPosVelAccel;
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simRMLStep(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simRMLStep");
	int retVal=-1; //error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		int handle=luaToInt(L,1);
		double timeStep=lua_tonumber(L,2);

		// Get the Dofs of this object:
		int dofs=-1;
		{
			int auxVals[4]={0,handle,0,0};
			int replyData[4]={-1,-1,-1,-1};
			CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_rmlinfo,auxVals,NULL,replyData);
			dofs=replyData[1];
		}

		if (dofs>0)
		{
			double* newPosVelAccel=new double[dofs*3];
			BYTE auxData[1+8+8];
			auxData[0]=1;
			retVal=simRMLStep_internal(handle,timeStep,newPosVelAccel,auxData,NULL);
			if (retVal>=0)
			{
				lua_pushnumber(L,retVal);
				pushDoubleTableOntoStack(L,dofs*3,newPosVelAccel);
				lua_pushnumber(L,((double*)(auxData+1))[0]);
				delete[] newPosVelAccel;
				LUA_END(3);
			}
			delete[] newPosVelAccel;
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simRMLRemove(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simRMLRemove");
	int retVal=-1;
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int handle=luaToInt(L,1);
		retVal=simRMLRemove_internal(handle);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}


int _simRMLVel(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simRMLVel");
	int retVal=-43; //error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int dofs=luaToInt(L,1);
		double timeStep=lua_tonumber(L,2);
		int flags=luaToInt(L,3);
		int res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,dofs*3,false,false,true);
		if (res==2)
		{
			double* currentPosVelAccel=new double[dofs*3];
			getDoublesFromTable(L,4,dofs*3,currentPosVelAccel);
			res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,dofs*2,false,false,true);
			if (res==2)
			{
				double* maxAccelJerk=new double[dofs*2];
				getDoublesFromTable(L,5,dofs*2,maxAccelJerk);
				res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_bool,dofs,false,false,true);
				if (res==2)
				{
					char* selection=new char[dofs];
					getBoolsFromTable(L,6,dofs,selection);
					res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,dofs,false,false,true);
					if (res==2)
					{
						double* targetVel=new double[dofs];
						getDoublesFromTable(L,7,dofs,targetVel);

						BYTE auxData[1+4];
						auxData[0]=1;
						((int*)(auxData+1))[0]=0;

						int currentScriptID=getCurrentScriptID(L);
						CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
						if ((it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript))
							((int*)(auxData+1))[0]=1; // destroy at simulation end!

						retVal=simRMLVel_internal(dofs,timeStep,flags,currentPosVelAccel,maxAccelJerk,(unsigned char*)selection,targetVel,auxData);
						delete[] targetVel;
					}
					delete[] selection;
				}
				delete[] maxAccelJerk;
			}
			delete[] currentPosVelAccel;
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}


int _simGetObjectQuaternion(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetObjectQuaternion");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		float coord[4];
		if (simGetObjectQuaternion_internal(luaToInt(L,1),luaToInt(L,2),coord)==1)
			pushFloatTableOntoStack(L,4,coord);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simSetObjectQuaternion(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetObjectQuaternion");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,4))
	{
		float coord[4];
		getFloatsFromTable(L,3,4,coord);
		retVal=simSetObjectQuaternion_internal(luaToInt(L,1),luaToInt(L,2),coord);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simBuildMatrixQ(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simBuildMatrixQ");
	if (checkInputArguments(L,functionName,true,lua_arg_number,3,lua_arg_number,4))
	{
		float arr[12];
		float pos[3];
		float quaternion[4];
		getFloatsFromTable(L,1,3,pos);
		getFloatsFromTable(L,2,4,quaternion);
		if (simBuildMatrixQ_internal(pos,quaternion,arr)==1)
			pushFloatTableOntoStack(L,12,arr);
		else
			lua_pushnil(L); // error
	}
	else
		lua_pushnil(L); // error
	LUA_END(1);
}

int _simGetQuaternionFromMatrix(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetQuaternionFromMatrix");
	if (checkInputArguments(L,functionName,true,lua_arg_number,12))
	{
		float arr[12];
		float quaternion[4];
		getFloatsFromTable(L,1,12,arr);
		if (simGetQuaternionFromMatrix_internal(arr,quaternion)==1)
			pushFloatTableOntoStack(L,4,quaternion);
		else
			lua_pushnil(L); // error
	}
	else
		lua_pushnil(L); // error
	LUA_END(1);
}

int _simRMLMoveToPosition(lua_State* L)
{ // can only be called from a script running in a thread!!
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simRMLMoveToPosition");
//	int retVal=-1; //error
	if (VThread::isCurrentThreadTheMainThread())
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD,true);
	else
	{
		if (!CThreadPool::getStopForCurrentInstanceRequested())
		{
			if (checkInputArguments(L,functionName,false,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
			{ // the 3 first types are ok!
				int objectHandle=luaToInt(L,1);
				int relativeToObjectHandle=luaToInt(L,2);
				int flags=luaToInt(L,3);
				C3DObject* it=App::ct->objCont->getObject(objectHandle);
				C3DObject* relativeIt=NULL;
				bool relParentError=false;
				if (relativeToObjectHandle!=-1)
				{
					if (relativeToObjectHandle==sim_handle_parent)
						relativeIt=it->getParent();
					else
					{
						relativeIt=App::ct->objCont->getObject(relativeToObjectHandle);
						relParentError=(relativeIt==NULL);
					}
				}
				if ( (it==NULL)||((relativeIt==NULL)&&relParentError) )
					outputCallErrorMessage(functionName.c_str(),SIM_ERROR_OBJECT_INEXISTANT,true);
				else
				{
					C7Vector startPose(it->getCumulativeTransformationPart1());
					C7Vector tr;
					tr.setIdentity();
					if (relativeIt!=NULL)
						tr=relativeIt->getCumulativeTransformationPart1();
					C7Vector trInv(tr.getInverse());
					startPose=trInv*startPose;
					C7Vector goalPose(startPose); // if we specify nil for the goal pos/quat we use the same as start

					double currentVel[4]={0.0,0.0,0.0,0.0};
					double currentAccel[4]={0.0,0.0,0.0,0.0};
					double maxVel[4];
					double maxAccel[4];
					double maxJerk[4];
					double targetVel[4]={0.0,0.0,0.0,0.0};
					bool argError=false;

					// currentVel
					int res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,4,false,true,true);
					if ((!argError)&&(res<1))
						argError=true;
					else
					{
						if (res==2)
							getDoublesFromTable(L,4,4,currentVel);
					}

					// currentAccel
					res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,4,false,true,true);
					if ((!argError)&&(res<1))
						argError=true;
					else
					{
						if (res==2)
							getDoublesFromTable(L,5,4,currentAccel);
					}

					// maxVel
					res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,4,false,false,true);
					if ((!argError)&&(res<2))
						argError=true;
					else
					{
						if (res==2)
							getDoublesFromTable(L,6,4,maxVel);
					}

					// maxAccel
					res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,4,false,false,true);
					if ((!argError)&&(res<2))
						argError=true;
					else
					{
						if (res==2)
							getDoublesFromTable(L,7,4,maxAccel);
					}

					// maxJerk
					res=checkOneGeneralInputArgument(L,8,functionName,lua_arg_number,4,false,false,true);
					if ((!argError)&&(res<2))
						argError=true;
					else
					{
						if (res==2)
							getDoublesFromTable(L,8,4,maxJerk);
					}

					// targetPos
					res=checkOneGeneralInputArgument(L,9,functionName,lua_arg_number,3,false,true,true);
					if ((!argError)&&(res<1))
						argError=true;
					else
					{
						if (res==2)
						{
							float dummy[3];
							getFloatsFromTable(L,9,3,dummy);
							goalPose.X.set(dummy);
						}
					}

					// targetQuat
					res=checkOneGeneralInputArgument(L,10,functionName,lua_arg_number,4,false,true,true);
					if ((!argError)&&(res<1))
						argError=true;
					else
					{
						if (res==2)
						{
							float dummy[4];
							getFloatsFromTable(L,10,4,dummy);
							goalPose.Q(0)=dummy[3];
							goalPose.Q(1)=dummy[0];
							goalPose.Q(2)=dummy[1];
							goalPose.Q(3)=dummy[2];
						}
					}

					// targetVel
					res=checkOneGeneralInputArgument(L,11,functionName,lua_arg_number,4,false,true,true);
					if ((!argError)&&(res<1))
						argError=true;
					else
					{
						if (res==2)
							getDoublesFromTable(L,11,4,targetVel);
					}

					if (!argError)
					{
						float matrStart[12];
						float matrGoal[12];
						float axis[3];
						float angle;
						float quat[4];
						quat[0]=startPose.Q(1);
						quat[1]=startPose.Q(2);
						quat[2]=startPose.Q(3);
						quat[3]=startPose.Q(0);
						simBuildMatrixQ_internal(startPose.X.data,quat,matrStart);
						quat[0]=goalPose.Q(1);
						quat[1]=goalPose.Q(2);
						quat[2]=goalPose.Q(3);
						quat[3]=goalPose.Q(0);
						simBuildMatrixQ_internal(goalPose.X.data,quat,matrGoal);
						simGetRotationAxis_internal(matrStart,matrGoal,axis,&angle);
						BYTE auxData[9];
						auxData[0]=1;
						double currentPosVelAccel[3*4];
						currentPosVelAccel[0+0]=(double)startPose.X(0);
						currentPosVelAccel[0+1]=(double)startPose.X(1);
						currentPosVelAccel[0+2]=(double)startPose.X(2);
						currentPosVelAccel[0+3]=0.0;
						for (int i=0;i<4;i++)
							currentPosVelAccel[4+i]=currentVel[i];
						for (int i=0;i<4;i++)
							currentPosVelAccel[8+i]=currentAccel[i];

						double maxVelAccelJerk[3*4];
						for (int i=0;i<4;i++)
							maxVelAccelJerk[0+i]=maxVel[i];
						for (int i=0;i<4;i++)
							maxVelAccelJerk[4+i]=maxAccel[i];
						for (int i=0;i<4;i++)
							maxVelAccelJerk[8+i]=maxJerk[i];

						BYTE selection[4]={1,1,1,1};

						double targetPosVel[2*4];
						targetPosVel[0+0]=(double)goalPose.X(0);
						targetPosVel[0+1]=(double)goalPose.X(1);
						targetPosVel[0+2]=(double)goalPose.X(2);
						targetPosVel[0+3]=(double)angle;
						for (int i=0;i<4;i++)
							targetPosVel[4+i]=targetVel[i];

						double newPosVelAccel[3*4];
						bool movementFinished=false;


						int rmlHandle=simRMLPos_internal(4,0.0001,flags,currentPosVelAccel,maxVelAccelJerk,selection,targetPosVel,NULL);
						while (!movementFinished)
						{
							double dt=(double)App::ct->simulation->getSimulationTimeStep_speedModified();
							int rmlRes=simRMLStep_internal(rmlHandle,dt,newPosVelAccel,auxData,NULL);
							it=App::ct->objCont->getObject(objectHandle);
							if ((rmlRes<0)||(it==NULL))
								movementFinished=true; // error
							else
							{
								movementFinished=(rmlRes==1);
								// Set the current position/orientation:
								for (int i=0;i<3*4;i++)
									currentPosVelAccel[i]=newPosVelAccel[i];
								C7Vector currentPose;
								currentPose.X(0)=(float)currentPosVelAccel[0];
								currentPose.X(1)=(float)currentPosVelAccel[1];
								currentPose.X(2)=(float)currentPosVelAccel[2];
								float matrOut[12];
								float axisPos[3]={0.0f,0.0f,0.0f};
								simRotateAroundAxis_internal(matrStart,axis,axisPos,(float)currentPosVelAccel[3],matrOut);
								simGetQuaternionFromMatrix_internal(matrOut,quat);
								currentPose.Q(0)=quat[3];
								currentPose.Q(1)=quat[0];
								currentPose.Q(2)=quat[1];
								currentPose.Q(3)=quat[2];
								currentPose=tr*currentPose;
								it->setAbsoluteTransformation(currentPose);
									
								if (movementFinished)
								{
									CVThreadData* threadData=CThreadPool::getCurrentThreadData();
									threadData->usedMovementTime=(float)(((double*)(auxData+1))[0]);
									lua_pushnumber(L,1);
									pushDoubleTableOntoStack(L,3,newPosVelAccel);
									pushFloatTableOntoStack(L,4,quat);
									pushDoubleTableOntoStack(L,4,newPosVelAccel+4);
									pushDoubleTableOntoStack(L,4,newPosVelAccel+8);
									lua_pushnumber(L,dt-((double*)(auxData+1))[0]);
									LUA_END(6);
								}

								if (!movementFinished)
								{
									LUA_UNLOCK; // Important!!
									CThreadPool::switchBackToPreviousThread();
									if (CThreadPool::getStopForCurrentInstanceRequested())
									{
										LUA_LOCK; // Important!!
										break;
									}
									LUA_LOCK; // Important!!
								}
							}
						}
						simRMLRemove_internal(rmlHandle);
					}
				}
			}
		}
	}
	lua_pushnumber(L,-1); // error
	LUA_END(1);
}

int _simRMLMoveToJointPositions(lua_State* L)
{ // can only be called from a script running in a thread!!
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simRMLMoveToJointPositions");
//	int retVal=-1; //error
	if (VThread::isCurrentThreadTheMainThread())
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD,true);
	else
	{
		if (!CThreadPool::getStopForCurrentInstanceRequested())
		{

			if ( (!lua_istable(L,1))||(VREP_LUA_OBJLEN(L,1)<1) )
				outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_FIRST_ARGUMENT,true);
			else
			{ 
				int dofs=VREP_LUA_OBJLEN(L,1);
				int flags=-1;
				int* jointHandles=new int[dofs];
				double* currentVel=new double[dofs];
				double* currentAccel=new double[dofs];
				double* maxVel=new double[dofs];
				double* maxAccel=new double[dofs];
				double* maxJerk=new double[dofs];
				double* targetPos=new double[dofs];
				double* targetVel=new double[dofs];
				int* direction=new int[dofs];
				for (int i=0;i<dofs;i++)
				{
					currentVel[i]=0.0;
					currentAccel[i]=0.0;
					maxVel[i]=0.0;
					maxAccel[i]=0.0;
					maxJerk[i]=0.0;
					targetPos[i]=0.0;
					targetVel[i]=0.0;
					direction[i]=0;
				}
				bool argError=false;

				// jointHandles
				int res=checkOneGeneralInputArgument(L,1,functionName,lua_arg_number,dofs,false,false,true);
				if ((!argError)&&(res<2))
					argError=true;
				else
				{
					if (res==2)
						getIntsFromTable(L,1,dofs,jointHandles);
				}
				if (!argError)
				{ // check if all joint handles are ok:
					for (int i=0;i<dofs;i++)
					{
						CJoint* act=App::ct->objCont->getJoint(jointHandles[i]);
						if (act==NULL)
							argError=true;
					}
					if (argError)
						outputCallErrorMessage(functionName.c_str(),SIM_ERROR_FOUND_INVALID_HANDLES,true);
				}

				// flags
				res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_number,0,false,false,true);
				if ((!argError)&&(res<2))
					argError=true;
				else
				{
					if (res==2)
						flags=luaToInt(L,2);
				}

				// currentVel
				res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,dofs,false,true,true);
				if ((!argError)&&(res<1))
					argError=true;
				else
				{
					if (res==2)
						getDoublesFromTable(L,3,dofs,currentVel);
				}

				// currentAccel
				res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,dofs,false,true,true);
				if ((!argError)&&(res<1))
					argError=true;
				else
				{
					if (res==2)
						getDoublesFromTable(L,4,dofs,currentAccel);
				}

				// maxVel
				res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,dofs,false,false,true);
				if ((!argError)&&(res<2))
					argError=true;
				else
				{
					if (res==2)
						getDoublesFromTable(L,5,dofs,maxVel);
				}

				// maxAccel
				res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,dofs,false,false,true);
				if ((!argError)&&(res<2))
					argError=true;
				else
				{
					if (res==2)
						getDoublesFromTable(L,6,dofs,maxAccel);
				}

				// maxJerk
				res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,dofs,false,false,true);
				if ((!argError)&&(res<2))
					argError=true;
				else
				{
					if (res==2)
						getDoublesFromTable(L,7,dofs,maxJerk);
				}

				// targetPos
				res=checkOneGeneralInputArgument(L,8,functionName,lua_arg_number,dofs,false,false,true);
				if ((!argError)&&(res<2))
					argError=true;
				else
				{
					if (res==2)
						getDoublesFromTable(L,8,dofs,targetPos);
				}

				// targetVel
				res=checkOneGeneralInputArgument(L,9,functionName,lua_arg_number,dofs,false,true,true);
				if ((!argError)&&(res<1))
					argError=true;
				else
				{
					if (res==2)
						getDoublesFromTable(L,9,dofs,targetVel);
				}

				res=checkOneGeneralInputArgument(L,10,functionName,lua_arg_number,dofs,true,true,true);
				if ((!argError)&&(res<0))
					argError=true;
				else
				{
					if (res==2)
						getIntsFromTable(L,10,dofs,direction);
				}

				if (!argError)
				{
					BYTE auxData[9];
					auxData[0]=1;
					double* currentPosVelAccel=new double[3*dofs];
					double* newPosVelAccel=new double[3*dofs];
					double* maxVelAccelJerk=new double[3*dofs];
					double* targetPosVel=new double[2*dofs];
					BYTE* selection=new BYTE[dofs];;
					for (int i=0;i<dofs;i++)
					{
						CJoint* act=App::ct->objCont->getJoint(jointHandles[i]);
						if (act!=NULL) // should always pass!
						{
							selection[i]=1;
							float ps=act->getPosition();
							if ( (act->getJointType()==sim_joint_revolute_subtype)&&act->getPositionIsCyclic() )
							{
								if (direction[i]==0)
								{ // smallest movement:
									float dx=targetPos[i]-ps;
									while (dx>=piValTimes2)
									{
										ps+=piValTimes2;
										dx=targetPos[i]-ps;
									}
									while (dx<0.0f)
									{
										ps-=piValTimes2;
										dx=targetPos[i]-ps;
									}
									float b=ps+piValTimes2;
									if (fabs(targetPos[i]-b)<fabs(targetPos[i]-ps))
										ps=b;
								}
								if (direction[i]>0)
								{ // positive direction:
									float dx=targetPos[i]-ps;
									while (dx<piValTimes2*float(direction[i]-1))
									{
										ps-=piValTimes2;
										dx=targetPos[i]-ps;
									}
									while (dx>=piValTimes2*float(direction[i]))
									{
										ps+=piValTimes2;
										dx=targetPos[i]-ps;
									}
								}
								if (direction[i]<0)
								{ // negative direction:
									float dx=targetPos[i]-ps;
									while (dx>-piValTimes2*float(-direction[i]-1))
									{
										ps+=piValTimes2;
										dx=targetPos[i]-ps;
									}
									while (dx<=-piValTimes2*float(-direction[i]))
									{
										ps-=piValTimes2;
										dx=targetPos[i]-ps;
									}
								}
								currentPosVelAccel[0*dofs+i]=ps;
							}
							else
								currentPosVelAccel[0*dofs+i]=ps;
						}
						else
							selection[i]=0;

						currentPosVelAccel[1*dofs+i]=currentVel[i];
						currentPosVelAccel[2*dofs+i]=currentAccel[i];

						maxVelAccelJerk[0*dofs+i]=maxVel[i];
						maxVelAccelJerk[1*dofs+i]=maxAccel[i];
						maxVelAccelJerk[2*dofs+i]=maxJerk[i];

						targetPosVel[0*dofs+i]=targetPos[i];
						targetPosVel[1*dofs+i]=targetVel[i];

						selection[i]=1;
					}

					bool movementFinished=false;


					int rmlHandle=simRMLPos_internal(dofs,0.0001,flags,currentPosVelAccel,maxVelAccelJerk,selection,targetPosVel,NULL);
					while (!movementFinished)
					{
						double dt=(double)App::ct->simulation->getSimulationTimeStep_speedModified();
						int rmlRes=simRMLStep_internal(rmlHandle,dt,newPosVelAccel,auxData,NULL);
						if (rmlRes<0)
							movementFinished=true; // error
						else
						{
							movementFinished=(rmlRes==1);
							// Set the current positions:
							for (int i=0;i<3*dofs;i++)
								currentPosVelAccel[i]=newPosVelAccel[i];
							for (int i=0;i<dofs;i++)
							{
								CJoint* act=App::ct->objCont->getJoint(jointHandles[i]);
								if ( (act!=NULL)&&(act->getJointType()!=sim_joint_spherical_subtype) )
								{ // might have been destroyed in the mean time
									if (act->getJointMode()==sim_jointmode_force)
										act->setDynamicMotorPositionControlTargetPosition((float)currentPosVelAccel[0*dofs+i]);
									else
										act->setPosition((float)currentPosVelAccel[0*dofs+i]);
								}
							}

							if (movementFinished)
							{
								CVThreadData* threadData=CThreadPool::getCurrentThreadData();
								threadData->usedMovementTime=(float)(((double*)(auxData+1))[0]);
								lua_pushnumber(L,1);
								pushDoubleTableOntoStack(L,dofs,newPosVelAccel+0*dofs);
								pushDoubleTableOntoStack(L,dofs,newPosVelAccel+1*dofs);
								pushDoubleTableOntoStack(L,dofs,newPosVelAccel+2*dofs);
								lua_pushnumber(L,dt-((double*)(auxData+1))[0]);

								delete[] currentPosVelAccel;
								delete[] newPosVelAccel;
								delete[] maxVelAccelJerk;
								delete[] targetPosVel;
								delete[] selection;

								delete[] jointHandles;
								delete[] currentVel;
								delete[] currentAccel;
								delete[] maxVel;
								delete[] maxAccel;
								delete[] maxJerk;
								delete[] targetPos;
								delete[] targetVel;
								LUA_END(5);
							}

							if (!movementFinished)
							{
								LUA_UNLOCK; // Important!!
								CThreadPool::switchBackToPreviousThread();
								if (CThreadPool::getStopForCurrentInstanceRequested())
								{
									LUA_LOCK; // Important!!
									break;
								}
								LUA_LOCK; // Important!!
							}
						}
					}
					simRMLRemove_internal(rmlHandle);


//					while (!movementFinished)
//					{
//						double dt=(double)App::ct->simulation->getSimulationTimeStep_speedModified();
//						int rmlRes=simRMLPosition_internal(dofs,dt,flags,currentPosVelAccel,maxVelAccelJerk,selection,targetPosVel,newPosVelAccel,auxData);
//						if (rmlRes<0)
//							movementFinished=true; // error
//						else
//						{
//							movementFinished=(rmlRes==1);
//							// Set the current positions:
//							for (int i=0;i<3*dofs;i++)
//								currentPosVelAccel[i]=newPosVelAccel[i];
//							for (int i=0;i<dofs;i++)
//							{
//								CJoint* act=App::ct->objCont->getJoint(jointHandles[i]);
//								if ( (act!=NULL)&&(act->getJointType()!=sim_joint_spherical_subtype) )
//								{ // might have been destroyed in the mean time
//									if (act->getJointMode()==sim_jointmode_force)
//										act->setDynamicMotorPositionControlTargetPosition((float)currentPosVelAccel[0*dofs+i]);
//									else
//										act->setPosition((float)currentPosVelAccel[0*dofs+i]);
//								}
//							}
								
//							if (movementFinished)
//							{
//								CVThreadData* threadData=CThreadPool::getCurrentThreadData();
//								threadData->usedMovementTime=(float)(((double*)(auxData+1))[0]);
//								lua_pushnumber(L,1);
//								pushDoubleTableOntoStack(L,dofs,newPosVelAccel+0*dofs);
//								pushDoubleTableOntoStack(L,dofs,newPosVelAccel+1*dofs);
//								pushDoubleTableOntoStack(L,dofs,newPosVelAccel+2*dofs);
//								lua_pushnumber(L,dt-((double*)(auxData+1))[0]);

//								delete[] currentPosVelAccel;
//								delete[] newPosVelAccel;
//								delete[] maxVelAccelJerk;
//								delete[] targetPosVel;
//								delete[] selection;

//								delete[] jointHandles;
//								delete[] currentVel;
//								delete[] currentAccel;
//								delete[] maxVel;
//								delete[] maxAccel;
//								delete[] maxJerk;
//								delete[] targetPos;
//								delete[] targetVel;
//								LUA_END(5);
//							}

//							if (!movementFinished)
//							{
//								LUA_UNLOCK; // Important!!
//								CThreadPool::switchBackToPreviousThread();
//								if (CThreadPool::getStopForCurrentInstanceRequested())
//								{
//									LUA_LOCK; // Important!!
//									break;
//								}
//								LUA_LOCK; // Important!!
//							}
//						}
//					}

					delete[] currentPosVelAccel;
					delete[] newPosVelAccel;
					delete[] maxVelAccelJerk;
					delete[] targetPosVel;
					delete[] selection;
				}

				delete[] jointHandles;
				delete[] currentVel;
				delete[] currentAccel;
				delete[] maxVel;
				delete[] maxAccel;
				delete[] maxJerk;
				delete[] targetPos;
				delete[] targetVel;
			}
		}
	}
	lua_pushnumber(L,-1); // error
	LUA_END(1);
}

int _simFileDialog(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simFileDialog");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0,lua_arg_string,0,lua_arg_string,0,lua_arg_string,0,lua_arg_string,0))
	{
		int mode=luaToInt(L,1);
		std::string title(lua_tostring(L,2));
		std::string startPath(lua_tostring(L,3));
		std::string initName(lua_tostring(L,4));
		std::string extName(lua_tostring(L,5));
		std::string ext(lua_tostring(L,6));
		char* pathAndName=simFileDialog_internal(mode,title.c_str(),startPath.c_str(),initName.c_str(),extName.c_str(),ext.c_str());
		if (pathAndName!=NULL)
		{
			lua_pushstring(L,pathAndName);
			simReleaseBuffer_internal(pathAndName);
		}
		else
			lua_pushnil(L); // error
	}
	else
		lua_pushnil(L); // error
	LUA_END(1);
}

int _simMsgBox(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simMsgBox");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_string,0,lua_arg_string,0))
	{
		int dlgType=luaToInt(L,1);
		int dlgButtons=luaToInt(L,2);
		std::string title(lua_tostring(L,3));
		std::string message(lua_tostring(L,4));
		int retVal=simMsgBox_internal(dlgType,dlgButtons,title.c_str(),message.c_str());
		lua_pushnumber(L,retVal);
	}
	else
		lua_pushnumber(L,-1); // error
	LUA_END(1);
}

int _simEnableWorkThreads(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simEnableWorkThreads");
	int retVal=-1;
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int threadCnt=luaToInt(L,1);
		int currentThreadCnt=CSensingThreadPool::getThreadCount();
		if (threadCnt!=currentThreadCnt)
		{
			CSensingThreadPool::killThreads();
			if (threadCnt>0)
				CSensingThreadPool::launchThreads(threadCnt);
		}
		retVal=CSensingThreadPool::getThreadCount();
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simWaitForWorkThreads(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simWaitForWorkThreads");
	if (CSensingThreadPool::getThreadCount()>0)
	{
		CSensingThreadPool::waitUntilAllThreadsIdleAndTasksDone();
		CSensingThreadPool::clearAllTasks();
	}
	LUA_END(0);
}


int _simSetShapeMassAndInertia(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetShapeMassAndInertia");
	int result=-1;
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,9,lua_arg_number,3))
	{
		int handle=lua_tointeger(L,1);
		float mass=luaToFloat(L,2);
		float inertiaMatrix[9];
		getFloatsFromTable(L,3,9,inertiaMatrix);
		float centerOfMass[3];
		getFloatsFromTable(L,4,3,centerOfMass);
		float* transf=NULL;
		float transformation[12];
		int res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,12,true,true,true);
		if (res>=0)
		{
			if (res==2)
			{
				getFloatsFromTable(L,5,12,transformation);
				transf=transformation;
			}
			result=simSetShapeMassAndInertia_internal(handle,mass,inertiaMatrix,centerOfMass,transf);
		}
	}
	lua_pushnumber(L,result);
	LUA_END(1);
}

int _simGetShapeMassAndInertia(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetShapeMassAndInertia");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int handle=lua_tointeger(L,1);
		float* transf=NULL;
		float transformation[12];
		int res=checkOneGeneralInputArgument(L,2,functionName,lua_arg_number,12,true,true,true);
		if (res>=0)
		{
			if (res==2)
			{
				getFloatsFromTable(L,2,12,transformation);
				transf=transformation;
			}
			float mass;
			float inertiaMatrix[9];
			float centerOfMass[3];
			int result=simGetShapeMassAndInertia_internal(handle,&mass,inertiaMatrix,centerOfMass,transf);
			if (result==-1)
				lua_pushnil(L);
			lua_pushnumber(L,mass);
			pushFloatTableOntoStack(L,9,inertiaMatrix);
			pushFloatTableOntoStack(L,3,centerOfMass);
			LUA_END(3);
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simGroupShapes(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGroupShapes");
	int retVal=-1; // error
	if (checkInputArguments(L,functionName,true,lua_arg_number,1))
	{
		int tableSize=int(VREP_LUA_OBJLEN(L,1));
		int* theTable=new int[tableSize];
		getIntsFromTable(L,1,tableSize,theTable);
		retVal=simGroupShapes_internal(theTable,tableSize);
		delete[] theTable;
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simUngroupShape(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simUngroupShape");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int shapeHandle=lua_tointeger(L,1);
		int count;
		int* handles=simUngroupShape_internal(shapeHandle,&count);
		if (handles!=NULL)
		{
			pushIntTableOntoStack(L,count,handles);
			delete[] handles;
		}
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}


int _simConvexDecompose(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simConvexDecompose");
	int retVal=-1;
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,4,lua_arg_number,3))
	{
		int shapeHandle=lua_tointeger(L,1);
		int options=lua_tointeger(L,2);
		int intParams[5];
		float floatParams[5];
		getIntsFromTable(L,3,4,intParams);
		getFloatsFromTable(L,4,3,floatParams);
		intParams[4]=0;
		floatParams[3]=0.0f;
		floatParams[4]=0.0f;
		retVal=simConvexDecompose_internal(shapeHandle,options,intParams,floatParams);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simInsertPathCtrlPoints(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simInsertPathCtrlPoints");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int pathHandle=luaToInt(L,1);
		int options=luaToInt(L,2);
		int startIndex=luaToInt(L,3);
		int ptCnt=luaToInt(L,4);
//		int tableSize=int(VREP_LUA_OBJLEN(L,5));

		int res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,ptCnt*11,false,false,true);
		if (res==2)
		{
			float* data=new float[ptCnt*11];
			getFloatsFromTable(L,5,ptCnt*11,data);
			for (int i=0;i<ptCnt;i++)
				((int*)(data+11*i+8))[0]=int(data[11*i+8]+0.5f);
			retVal=simInsertPathCtrlPoints_internal(pathHandle,options,startIndex,ptCnt,data);
			delete[] data;
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCutPathCtrlPoints(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCutPathCtrlPoints");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int pathHandle=luaToInt(L,1);
		int startIndex=luaToInt(L,2);
		int ptCnt=luaToInt(L,3);
		retVal=simCutPathCtrlPoints_internal(pathHandle,startIndex,ptCnt);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}


int _simGetIkGroupMatrix(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetIkGroupMatrix");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		int ikGroupHandle=luaToInt(L,1);
		int options=luaToInt(L,2);
		int matrixSize[2];
		float* data=simGetIkGroupMatrix_internal(ikGroupHandle,options,matrixSize);
		if (data!=NULL)
		{
			pushFloatTableOntoStack(L,matrixSize[0]*matrixSize[1],data);
			pushIntTableOntoStack(L,2,matrixSize);
			LUA_END(2);
		}
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simGetMpConfigForTipPose(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetMpConfigForTipPose");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,7))
	{
		int mpHandle=luaToInt(L,1);
		int options=luaToInt(L,2);
		float closeNodesDistance=luaToFloat(L,3);
		int trialCount=luaToInt(L,4);
		float tipPose[7];
		int maxTimeInMs=0;
		getFloatsFromTable(L,5,7,tipPose);
		CMotionPlanningTask* it=App::ct->motionPlanning->getObject(mpHandle);
		if (it!=NULL)
		{
			int res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,0,true,true,true);
			if (res>=0)
			{
				if (res==2)
					maxTimeInMs=luaToInt(L,6);
				int res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,it->getJointCount(),true,true,true);
				if (res>=0)
				{
					std::vector<float> jointPositions(it->getJointCount(),0.0f);
					if (res==2)
					{ // we have some ref. configs we want to take into account
						int tableSize=int(VREP_LUA_OBJLEN(L,7));
						int refCount=tableSize/it->getJointCount();
						if (refCount>0)
						{
							std::vector<float> referenceConfigs(it->getJointCount()*refCount,0.0f);
							getFloatsFromTable(L,7,it->getJointCount()*refCount,&referenceConfigs[0]);
							res=checkOneGeneralInputArgument(L,8,functionName,lua_arg_number,it->getJointCount(),true,true,true);
							if (res>=0)
							{
								std::vector<float> jointWeights(it->getJointCount(),1.0f); // default weights
								if (res==2)
								{ // we have some weights we want to take into account
									getFloatsFromTable(L,8,it->getJointCount(),&jointWeights[0]);
								}

								// Check the behaviour and correction pass arguments:
								res=checkOneGeneralInputArgument(L,9,functionName,lua_arg_number,it->getJointCount(),true,true,true);
								if (res>=0)
								{
									std::vector<int> behaviour(it->getJointCount(),0);
									int correctionPasses=0;
									if (res==2)
									{ // we have some "behaviour" values we want to take into account:
										getIntsFromTable(L,9,it->getJointCount(),&behaviour[0]);
									}
									res=checkOneGeneralInputArgument(L,10,functionName,lua_arg_number,0,true,true,true);
									if (res>=0)
									{
										if (res==2)
											correctionPasses=luaToInt(L,10);
										retVal=simGetMpConfigForTipPose_internal(mpHandle,options,closeNodesDistance,trialCount,tipPose,maxTimeInMs,&jointPositions[0],&referenceConfigs[0],refCount,&jointWeights[0],&behaviour[0],correctionPasses);
									}
								}
							}
						}
						else
							retVal=simGetMpConfigForTipPose_internal(mpHandle,options,closeNodesDistance,trialCount,tipPose,maxTimeInMs,&jointPositions[0],NULL,0,NULL,NULL,0);
					}
					else
						retVal=simGetMpConfigForTipPose_internal(mpHandle,options,closeNodesDistance,trialCount,tipPose,maxTimeInMs,&jointPositions[0],NULL,0,NULL,NULL,0);
					if (retVal>0)
					{
						lua_pushnumber(L,retVal);
						pushFloatTableOntoStack(L,it->getJointCount(),&jointPositions[0]);
						LUA_END(2);
					}
				}
			}
		}
		else
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_MOTION_PLANNING_OBJECT_INEXISTANT,true);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simFindMpPath(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simFindMpPath");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,1,lua_arg_number,1,lua_arg_number,0,lua_arg_number,0))
	{
		int mpHandle=luaToInt(L,1);
		int options=luaToInt(L,4);
		float stepSize=luaToFloat(L,5);
		int maxTimeInMs=0;
		CMotionPlanningTask* it=App::ct->motionPlanning->getObject(mpHandle);
		if (it!=NULL)
		{
			int dof=it->getJointCount();
			// now check the args again, this time taking the table sizes into account
			if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,dof,lua_arg_number,dof,lua_arg_number,0,lua_arg_number,0))
			{
				std::vector<float> startConfig(dof,0.0f);
				std::vector<float> goalConfig(dof,0.0f);
				getFloatsFromTable(L,2,dof,&startConfig[0]);
				getFloatsFromTable(L,3,dof,&goalConfig[0]);

				int res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,0,true,true,true);
				if (res>=0)
				{
					if (res==2)
						maxTimeInMs=luaToInt(L,6);
					int configCount=0;
					float* configs=simFindMpPath_internal(mpHandle,&startConfig[0],&goalConfig[0],options,stepSize,&configCount,maxTimeInMs,NULL,NULL,NULL);
					if (configs!=NULL)
					{
						pushFloatTableOntoStack(L,dof*configCount,configs);
						pushFloatTableOntoStack(L,configCount,configs+dof*configCount);
						pushFloatTableOntoStack(L,configCount*3,configs+dof*configCount+configCount);
						pushFloatTableOntoStack(L,configCount*4,configs+dof*configCount+configCount+configCount*3);
						pushFloatTableOntoStack(L,configCount,configs+dof*configCount+configCount+configCount*3+configCount*4);
						delete[] configs;
						LUA_END(5);
					}
				}
			}
		}
		else
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_MOTION_PLANNING_OBJECT_INEXISTANT,true);
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simSimplifyMpPath(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSimplifyMpPath");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int mpHandle=luaToInt(L,1);
		CMotionPlanningTask* it=App::ct->motionPlanning->getObject(mpHandle);
		if (it!=NULL)
		{
			int dof=it->getJointCount();
			if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,dof))
			{
				int configCnt=VREP_LUA_OBJLEN(L,2)/dof;
				if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,dof*configCnt,lua_arg_number,0,lua_arg_number,0))
				{
					std::vector<float> configs(dof*configCnt,0.0f);
					getFloatsFromTable(L,2,dof*configCnt,&configs[0]);
					int options=luaToInt(L,3);
					float stepSize=luaToFloat(L,4);
					int increment=1;
					int maxTimeInMs=0;
					int res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,0,true,true,true);
					if (res>=0)
					{
						if (res==2)
							increment=luaToInt(L,5);
						res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,0,true,true,true);
						if (res>=0)
						{
							if (res==2)
								maxTimeInMs=luaToInt(L,6);
							int outConfigCnt=0;

							float* outConfigs=simSimplifyMpPath_internal(mpHandle,&configs[0],configCnt,options,stepSize,increment,&outConfigCnt,maxTimeInMs,NULL,NULL,NULL);
							if (outConfigs!=NULL)
							{
								pushFloatTableOntoStack(L,dof*outConfigCnt,outConfigs);
								pushFloatTableOntoStack(L,outConfigCnt,outConfigs+dof*outConfigCnt);
								pushFloatTableOntoStack(L,outConfigCnt*3,outConfigs+dof*outConfigCnt+outConfigCnt);
								pushFloatTableOntoStack(L,outConfigCnt*4,outConfigs+dof*outConfigCnt+outConfigCnt+outConfigCnt*3);
								pushFloatTableOntoStack(L,outConfigCnt,outConfigs+dof*outConfigCnt+outConfigCnt+outConfigCnt*3+outConfigCnt*4);
								delete[] outConfigs;
								LUA_END(5);
							}
						}
					}
				}
			}
		}
		else
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_MOTION_PLANNING_OBJECT_INEXISTANT,true);
	}
	lua_pushnil(L);
	LUA_END(1);
}


int _simFindIkPath(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simFindIkPath");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,1,lua_arg_number,1,lua_arg_number,0,lua_arg_number,0))
	{
		int mpHandle=luaToInt(L,1);
		int options=luaToInt(L,4);
		float stepSize=luaToFloat(L,5);
		CMotionPlanningTask* it=App::ct->motionPlanning->getObject(mpHandle);
		if (it!=NULL)
		{
			int dof=it->getJointCount();
			// now check the args again, this time taking the table sizes into account
			if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,dof,lua_arg_number,7,lua_arg_number,0,lua_arg_number,0))
			{
				std::vector<float> startConfig(dof,0.0f);
				std::vector<float> goalPose(7,0.0f);
				getFloatsFromTable(L,2,dof,&startConfig[0]);
				getFloatsFromTable(L,3,7,&goalPose[0]);

				int configCount=0;
				float* configs=simFindIkPath_internal(mpHandle,&startConfig[0],&goalPose[0],options,stepSize,&configCount,NULL,NULL,NULL);
				if (configs!=NULL)
				{
					pushFloatTableOntoStack(L,dof*configCount,configs);
					pushFloatTableOntoStack(L,configCount,configs+dof*configCount);
					pushFloatTableOntoStack(L,configCount*3,configs+dof*configCount+configCount);
					pushFloatTableOntoStack(L,configCount*4,configs+dof*configCount+configCount+configCount*3);
					pushFloatTableOntoStack(L,configCount,configs+dof*configCount+configCount+configCount*3+configCount*4);
					delete[] configs;
					LUA_END(5);
				}
			}
		}
		else
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_MOTION_PLANNING_OBJECT_INEXISTANT,true);
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simGetMpConfigTransition(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetMpConfigTransition");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,1,lua_arg_number,1,lua_arg_number,0))
	{
		int mpHandle=luaToInt(L,1);
		int options=luaToInt(L,4);
		CMotionPlanningTask* it=App::ct->motionPlanning->getObject(mpHandle);
		if (it!=NULL)
		{
			int dof=it->getJointCount();
			// now check the args again, this time taking the table sizes into account
			if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,dof,lua_arg_number,dof,lua_arg_number,0))
			{
				std::vector<float> startConfig(dof,0.0f);
				std::vector<float> goalConfig(dof,0.0f);
				getFloatsFromTable(L,2,dof,&startConfig[0]);
				getFloatsFromTable(L,3,dof,&goalConfig[0]);

				int res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,1,false,true,true);
				if (res>=1)
				{
					int* selectPtr=NULL;
					std::vector<int> selectDat;
					if (res==2)
					{
						int selectTableSize=int(VREP_LUA_OBJLEN(L,5));
						selectDat.resize(selectTableSize+1,0);
						selectDat[0]=selectTableSize/2;
						getIntsFromTable(L,5,selectTableSize,&selectDat[1]);
						selectPtr=&selectDat[0];
					}

					res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,0,false,false,true);
					if (res==2)
					{
						res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,0,false,false,true);
						if (res==2)
						{
							float stepSize1=luaToFloat(L,6);
							float stepSize2=luaToFloat(L,7);
							float* wayPoints=NULL;
							std::vector<float> way;
							res=checkOneGeneralInputArgument(L,8,functionName,lua_arg_number,14,true,true,true);
							if (res>=0)
							{
								int wayPointCnt=0;
								if (res==2)
								{
									int wpc=VREP_LUA_OBJLEN(L,8);
									wayPointCnt=wpc/7;
									way.resize(wayPointCnt*7,0.0f);
									getFloatsFromTable(L,8,wayPointCnt*7,&way[0]);
									wayPoints=&way[0];
								}

								int configCount=0;
								float* configs=simGetMpConfigTransition_internal(mpHandle,&startConfig[0],&goalConfig[0],options,selectPtr,stepSize1,stepSize2,wayPointCnt,wayPoints,&configCount,NULL,NULL);
								if (configs!=NULL)
								{
									pushFloatTableOntoStack(L,dof*configCount,configs);
									pushFloatTableOntoStack(L,configCount,configs+dof*configCount);
									pushFloatTableOntoStack(L,configCount*3,configs+dof*configCount+configCount);
									pushFloatTableOntoStack(L,configCount*4,configs+dof*configCount+configCount+configCount*3);
									pushFloatTableOntoStack(L,configCount,configs+dof*configCount+configCount+configCount*3+configCount*4);
									delete[] configs;
									LUA_END(5);
								}
							}
						}
					}
				}
			}
		}
		else
			outputCallErrorMessage(functionName.c_str(),SIM_ERROR_MOTION_PLANNING_OBJECT_INEXISTANT,true);
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simAddGhost(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAddGhost");
	int retVal=-1; // error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int ghostGroup=luaToInt(L,1);
		int objectHandle=luaToInt(L,2);
		int options=luaToInt(L,3);
		float startTime=luaToFloat(L,4);
		float endTime=luaToFloat(L,5);
		int res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,12,true,true,true);
		if (res>=0)
		{
			bool defaultColors=true;
			float color[12];
			if (res==2)
			{
				defaultColors=false;
				getFloatsFromTable(L,6,12,color);
			}
			if (defaultColors)
				retVal=simAddGhost_internal(ghostGroup,objectHandle,options,startTime,endTime,NULL);
			else
				retVal=simAddGhost_internal(ghostGroup,objectHandle,options,startTime,endTime,color);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simModifyGhost(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simModifyGhost");
	int retVal=-1; // error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int ghostGroup=luaToInt(L,1);
		int ghostId=luaToInt(L,2);
		int operation=luaToInt(L,3);
		float floatValue=luaToFloat(L,4);

		int options=0;
		int optionsMask=0;
		int res=0;
		if (operation==10)
			res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,0,false,false,true);
		if (res>=0)
		{
			if ((res==2)&&(operation==10))
			{
				options=luaToInt(L,5);
				res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,0,false,false,true);
				if (res==2)
					optionsMask=luaToInt(L,6);
				else
					res=-1;
			}
			if (res>=0)
			{
				int floatCnt=7;
				if (operation==13)
					floatCnt=12;
				res=0;
				if ((operation>=11)&&(operation<=13))
					res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,floatCnt,false,false,true);
				if (res>=0)
				{
					float colorOrTransfData[12];
					if ((res==2)&&(operation>=11)&&(operation<=13))
						getFloatsFromTable(L,7,floatCnt,colorOrTransfData);
					retVal=simModifyGhost_internal(ghostGroup,ghostId,operation,floatValue,options,optionsMask,colorOrTransfData);
				}
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simQuitSimulator(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simQuitSimulator");
	if (checkInputArguments(L,functionName,true,lua_arg_bool,0))
	{
		bool doNotDisplayMessages=luaToBool(L,1);
		simQuitSimulator_internal(doNotDisplayMessages);
	}
	LUA_END(0);
}

int _simGetThreadId(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetThreadId");
	int retVal=simGetThreadId_internal();
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetMaterialId(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetMaterialId");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		std::string matName(lua_tostring(L,1));
		retVal=simGetMaterialId_internal(matName.c_str());
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSetShapeMaterial(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSetShapeMaterial");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		int shapeHandle=luaToInt(L,1);
		int materialId=luaToInt(L,2);
		retVal=simSetShapeMaterial_internal(shapeHandle,materialId);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetShapeMaterial(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetShapeMaterial");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int shapeHandle=luaToInt(L,1);
		retVal=simGetShapeMaterial_internal(shapeHandle);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetTextureId(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetTextureId");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_string,0))
	{
		std::string matName(lua_tostring(L,1));
		int resolution[2];
		retVal=simGetTextureId_internal(matName.c_str(),resolution);
		if (retVal>=0)
		{
			lua_pushnumber(L,retVal);
			pushIntTableOntoStack(L,2,resolution);
			LUA_END(2);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simReadTexture(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simReadTexture");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		int textureId=luaToInt(L,1);
		int options=luaToInt(L,2);
		int posX=0;
		int posY=0;
		int sizeX=0;
		int sizeY=0;
		// Now check the optional arguments:
		int res;
		res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,0,true,false,true);
		if ((res==0)||(res==2))
		{
			if (res==2)
				posX=luaToInt(L,3);
			res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,0,true,false,true);
			if ((res==0)||(res==2))
			{
				if (res==2)
					posY=luaToInt(L,4);
				res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,0,true,false,true);
				if ((res==0)||(res==2))
				{
					if (res==2)
						sizeX=luaToInt(L,5);
					res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,0,true,false,true);
					if ((res==0)||(res==2))
					{
						if (res==2)
							sizeY=luaToInt(L,6);

						CTextureObject* to=App::ct->textureCont->getObject(textureId);
						if (to!=NULL)
						{
							int tSizeX,tSizeY;
							to->getTextureSize(tSizeX,tSizeY);
							if ( (posX>=0)&&(posY>=0)&&(sizeX>=0)&&(sizeY>=0)&&(posX+sizeX<=tSizeX)&&(posY+sizeY<=tSizeY) )
							{
								if (sizeX==0)
								{
									posX=0;
									sizeX=tSizeX;
								}
								if (sizeY==0)
								{
									posY=0;
									sizeY=tSizeY;
								}
								char* textureData=simReadTexture_internal(textureId,options,posX,posY,sizeX,sizeY);
								if (textureData!=NULL)
								{ // here we return RGB data in a string
									lua_pushlstring(L,(const char*)textureData,sizeX*sizeY*3);
									simReleaseBuffer_internal(textureData);
									LUA_END(1);
								}
							}
							else
								outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_ARGUMENTS,true);
						}
						else
							outputCallErrorMessage(functionName.c_str(),SIM_ERROR_TEXTURE_INEXISTANT,true);
					}
				}
			}
		}
	}
	LUA_END(0);
}

int _simWriteTexture(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simWriteTexture");
	int retVal=-1; // error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_string,0))
	{
		int textureId=luaToInt(L,1);
		int options=luaToInt(L,2);
		size_t dataLength;
		char* data=(char*)lua_tolstring(L,3,&dataLength);
		int posX=0;
		int posY=0;
		int sizeX=0;
		int sizeY=0;
		float interpol=0.0f;
		// Now check the optional arguments:
		int res;
		res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,0,true,false,true);
		if ((res==0)||(res==2))
		{
			if (res==2)
				posX=luaToInt(L,4);
			res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,0,true,false,true);
			if ((res==0)||(res==2))
			{
				if (res==2)
					posY=luaToInt(L,5);
				res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,0,true,false,true);
				if ((res==0)||(res==2))
				{
					if (res==2)
						sizeX=luaToInt(L,6);
					res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,0,true,false,true);
					if ((res==0)||(res==2))
					{
						if (res==2)
							sizeY=luaToInt(L,7);
						res=checkOneGeneralInputArgument(L,8,functionName,lua_arg_number,0,true,false,true);
						if ((res==0)||(res==2))
						{
							if (res==2)
								interpol=luaToFloat(L,8);
							CTextureObject* to=App::ct->textureCont->getObject(textureId);
							if (to!=NULL)
							{
								int tSizeX,tSizeY;
								to->getTextureSize(tSizeX,tSizeY);
								if ( (posX>=0)&&(posY>=0)&&(sizeX>=0)&&(sizeY>=0)&&(posX+sizeX<=tSizeX)&&(posY+sizeY<=tSizeY)&&(dataLength>=sizeX*sizeY*3) )
									retVal=simWriteTexture_internal(textureId,options,data,posX,posY,sizeX,sizeY,interpol);
								else
									outputCallErrorMessage(functionName.c_str(),SIM_ERROR_INVALID_ARGUMENTS,true);
							}
							else
								outputCallErrorMessage(functionName.c_str(),SIM_ERROR_TEXTURE_INEXISTANT,true);
						}
					}
				}
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simCreateTexture(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simCreateTexture");
	if (checkInputArguments(L,functionName,true,lua_arg_string,0,lua_arg_number,0))
	{
		std::string fileName(lua_tostring(L,1));
		int options=luaToInt(L,2);
		float* planeSizesP=NULL;
		float planeSizes[2];
		float* scalingUVP=NULL;
		float scalingUV[2];
		float* xy_gP=NULL;
		float xy_g[3];
		int resolution[2]={0,0}; // means: just any!

		// Now check the optional arguments:
		int res;
		res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,2,true,true,true);
		if (res>=0)
		{
			if (res==2)
			{
				getFloatsFromTable(L,3,2,planeSizes);
				planeSizesP=planeSizes;
			}

			res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,2,true,true,true);
			if (res>=0)
			{
				if (res==2)
				{
					getFloatsFromTable(L,4,2,scalingUV);
					scalingUVP=scalingUV;
				}

				res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,3,true,true,true);
				if (res>=0)
				{
					if (res==2)
					{
						getFloatsFromTable(L,5,3,xy_g);
						xy_gP=xy_g;
					}

					res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,0,true,true,true);
					if (res>=0)
					{
						int maxTextureSize=0; // just the original
						if (res==2)
							maxTextureSize=luaToInt(L,6);

						res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,2,fileName.length()!=0,fileName.length()!=0,true);
						if (res>=0)
						{
							if (res==2)
								getIntsFromTable(L,7,2,resolution);

							if (fileName.length()==0)
							{ // we are not loading a texture, but creating it!
								resolution[0]=tt::getLimitedInt(1,4096,resolution[0]);
								resolution[1]=tt::getLimitedInt(1,4096,resolution[1]);
							}

							int textureId;
							int shapeHandle=simCreateTexture_internal(fileName.c_str(),options,planeSizesP,scalingUVP,xy_gP,maxTextureSize,&textureId,resolution,NULL);
							if (shapeHandle>=0)
							{
								lua_pushnumber(L,shapeHandle);
								lua_pushnumber(L,textureId);
								pushIntTableOntoStack(L,2,resolution);
								LUA_END(3);

							}
						}
					}
				}
			}
		}
	}
	lua_pushnumber(L,-1); // error
	LUA_END(1);
}


int _simWriteCustomDataBlock(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simWriteCustomDataBlock");
	int retVal=-1; // error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0))
	{
		int objectHandle=luaToInt(L,1);
		std::string dataName(lua_tostring(L,2));
		int res;
		res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_string,0,false,true,true);
		if (res>=1)
		{
			size_t dataLength=0;
			char* data=NULL;
			if (res==2)
				data=(char*)lua_tolstring(L,3,&dataLength);
			retVal=simWriteCustomDataBlock_internal(objectHandle,dataName.c_str(),data,dataLength);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}


int _simReadCustomDataBlock(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simReadCustomDataBlock");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0))
	{
		int objectHandle=luaToInt(L,1);
		std::string dataName(lua_tostring(L,2));
		int dataLength;
		char* data=simReadCustomDataBlock_internal(objectHandle,dataName.c_str(),&dataLength);
		if (data!=NULL)
		{
			lua_pushlstring(L,(const char*)data,dataLength);
			simReleaseBuffer_internal(data);
			LUA_END(1);
		}
	}
	LUA_END(0);
}

int _simAddPointCloud(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simAddPointCloud");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0,lua_arg_number,3))
	{
		int pageMask=luaToInt(L,1);
		int layerMask=luaToInt(L,2);
		int objectHandle=luaToInt(L,3);
		int options=luaToInt(L,4);
		int currentScriptID=getCurrentScriptID(L);
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(currentScriptID);
		if ( (it->getScriptType()==sim_scripttype_mainscript)||(it->getScriptType()==sim_scripttype_childscript) )
			options=(options|1)-1; // cloud is automatically removed at the end of the simulation (i.e. is not persistent)
		float pointSize=luaToFloat(L,5);
		int pointCnt=VREP_LUA_OBJLEN(L,6)/3;
		std::vector<float> pointCoordinates(pointCnt*3,0.0f);
		getFloatsFromTable(L,6,pointCnt*3,&pointCoordinates[0]);
		int res;
		res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,12,true,true,true);
		if (res>=0)
		{
			unsigned char* defaultColors=NULL;
			std::vector<unsigned char> _defCols(12,0);
			if (res==2)
			{
				std::vector<int> _defCols_(12,0);
				getIntsFromTable(L,7,12,&_defCols_[0]);
				for (int i=0;i<12;i++)
					_defCols[i]=(unsigned char)_defCols_[i];
				defaultColors=&_defCols[0];
			}
			res=checkOneGeneralInputArgument(L,8,functionName,lua_arg_number,pointCnt*3,true,true,true);
			if (res>=0)
			{
				unsigned char* pointColors=NULL;
				std::vector<unsigned char> _pointCols;
				if (res==2)
				{
					_pointCols.resize(pointCnt*3,0);
					std::vector<int> _pointCols_(pointCnt*3,0);
					getIntsFromTable(L,8,pointCnt*3,&_pointCols_[0]);
					for (int i=0;i<pointCnt*3;i++)
						_pointCols[i]=(unsigned char)_pointCols_[i];
					pointColors=&_pointCols[0];
				}
				res=checkOneGeneralInputArgument(L,9,functionName,lua_arg_number,pointCnt*3,true,true,true);
				if (res>=0)
				{
					float* pointNormals=NULL;
					std::vector<float> _pointNormals;
					if (res==2)
					{
						_pointNormals.resize(pointCnt*3,0);
						getFloatsFromTable(L,9,pointCnt*3,&_pointNormals[0]);
						pointNormals=&_pointNormals[0];
					}
					retVal=simAddPointCloud_internal(pageMask,layerMask,objectHandle,options,pointSize,pointCoordinates.size()/3,&pointCoordinates[0],(char*)defaultColors,(char*)pointColors,pointNormals);
				}
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simModifyPointCloud(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simModifyPointCloud");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		int pointCloudHandle=luaToInt(L,1);
		int operation=luaToInt(L,2);
		retVal=simModifyPointCloud_internal(pointCloudHandle,operation,NULL,NULL);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simGetShapeGeomInfo(lua_State* L)
{
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetShapeGeomInfo");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		int handle=luaToInt(L,1);
		int intData[5];
		float floatData[5];
		retVal=simGetShapeGeomInfo_internal(handle,intData,floatData,NULL);
		if (retVal>=0)
		{
			lua_pushnumber(L,retVal);
			lua_pushnumber(L,intData[0]);
			pushFloatTableOntoStack(L,4,floatData);
			LUA_END(3);
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}





int _simResetTracing(lua_State* L)
{ // deprecated
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simResetTracing");
	outputCallWarningMessage(functionName.c_str(),SIM_ERROR_FUNCTION_DEPRECATED_AND_HAS_NO_EFFECT);
	lua_pushnil(L);
	LUA_END(1);
}

int _simHandleTracing(lua_State* L)
{ // deprecated
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simHandleTracing");
	outputCallWarningMessage(functionName.c_str(),SIM_ERROR_FUNCTION_DEPRECATED_AND_HAS_NO_EFFECT);
	lua_pushnil(L);
	LUA_END(1);
}

int _simMoveToPosition(lua_State* L)
{ // can only be called from a script running in a thread!! DEPRECATED
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simMoveToPosition");
	if (VThread::isCurrentThreadTheMainThread())
	{
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD,true);
		lua_pushnil(L); // error
	}
	else
	{
		if (CThreadPool::getStopForCurrentInstanceRequested())
		{ // Important to first check if we are supposed to leave the thread!!
			lua_pushnil(L); // special error
			LUA_END(1);
		}
		if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
		{ // Those are the arguments that are always required! (the rest can be ignored or set to nil!
			int objID=lua_tointeger(L,1);
			int relativeToObjID=lua_tointeger(L,2);
			float posTarget[3];
			float eulerTarget[3];
			float maxVelocity;
			C3DObject* object=App::ct->objCont->getObject(objID);
			C3DObject* relToObject=NULL;
			float accel=0.0f; // means infinite accel!! (default value)
			float angleToLinearCoeff=0.1f/(90.0f*degToRad); // (default value)
			int distCalcMethod=sim_distcalcmethod_dl_if_nonzero; // (default value)
			bool foundError=false;
			if ((!foundError)&&(object==NULL))
			{
				outputCallErrorMessage(functionName.c_str(),SIM_ERROR_OBJECT_INEXISTANT,true);
				foundError=true;
			}
			if ((!foundError)&&(relativeToObjID==sim_handle_parent))
			{
				relativeToObjID=-1;
				C3DObject* parent=object->getParent();
				if (parent!=NULL)
					relativeToObjID=parent->getID();
			}
			if ((!foundError)&&(relativeToObjID!=-1))
			{
				relToObject=App::ct->objCont->getObject(relativeToObjID);
				if (relToObject==NULL)
				{ // error, object doesn't exist!
					outputCallErrorMessage(functionName.c_str(),SIM_ERROR_OBJECT_INEXISTANT,true);
					foundError=true;
				}
			}

			// Now check the optional arguments:
			int res;
			BYTE posAndOrient=0;
			if (!foundError) // position argument:
			{
				res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,3,true,true,true);
				if (res==2)
				{ // get the data
					getFloatsFromTable(L,3,3,posTarget);
					posAndOrient|=1;
				}
				foundError=(res==-1);
			}
			if (!foundError) // orientation argument:
			{
				res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,3,true,true,true);
				if (res==2)
				{ // get the data
					getFloatsFromTable(L,4,3,eulerTarget);
					posAndOrient|=2;
				}
				foundError=(res==-1);
			}
			if ((!foundError)&&(posAndOrient==0))
			{
				foundError=true;
				outputCallErrorMessage(functionName.c_str(),"Target position and/or target orientation has to be specified.",true);
			}
			if (!foundError) // target velocity argument:
			{
				res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,0,false,false,true);
				if (res==2)
				{ // get the data
					maxVelocity=luaToFloat(L,5);
				}
				else
					foundError=true;
			}
			if (!foundError) // Accel argument:
			{
				res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_number,0,true,true,true);
				if (res==2)
				{ // get the data
					accel=fabs(luaToFloat(L,6));
				}
				foundError=(res==-1);
			}
			if (!foundError) // distance method:
			{
				res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,2,true,true,true);
				if (res==2)
				{ // get the data
					float tmpF[2];
					int tmpI[2];
					getFloatsFromTable(L,7,2,tmpF);
					getIntsFromTable(L,7,2,tmpI);
					distCalcMethod=tmpI[0];
					angleToLinearCoeff=tmpF[1];
				}
				foundError=(res==-1);
			}
			if (foundError)
				lua_pushnil(L); // error
			else
			{ // do the job here!
				C7Vector startTr(object->getCumulativeTransformationPart1());
				C7Vector relTr;
				relTr.setIdentity();
				if (relToObject!=NULL)
					relTr=relToObject->getCumulativeTransformation();
				startTr=relTr.getInverse()*startTr;

				C7Vector targetTr(startTr);
				if (posAndOrient&1)
					targetTr.X.set(posTarget);
				if (posAndOrient&2)
					targetTr.Q.setEulerAngles(eulerTarget[0],eulerTarget[1],eulerTarget[2]);
				float currentVel=0.0f;
				CVThreadData* threadData=CThreadPool::getCurrentThreadData();
				float lastTime=App::ct->simulation->getSimulationTime()+threadData->usedMovementTime;

				float dl=(targetTr.X-startTr.X).getLength();
				float da=targetTr.Q.getAngleBetweenQuaternions(startTr.Q)*angleToLinearCoeff;
				float vdl=dl;
				if (distCalcMethod==sim_distcalcmethod_dl)
					vdl=dl;
				if (distCalcMethod==sim_distcalcmethod_dac)
					vdl=da;
				if (distCalcMethod==sim_distcalcmethod_max_dl_dac)
					vdl=SIM_MAX(dl,da);
				if (distCalcMethod==sim_distcalcmethod_dl_and_dac)
					vdl=dl+da;
				if (distCalcMethod==sim_distcalcmethod_sqrt_dl2_and_dac2)
					vdl=sqrtf(dl*dl+da*da);
				if (distCalcMethod==sim_distcalcmethod_dl_if_nonzero)
				{
					vdl=dl;
					if (dl<0.00005f) // Was dl==0.0f before (tolerance problem). Changed on 1/4/2011
						vdl=da;
				}
				if (distCalcMethod==sim_distcalcmethod_dac_if_nonzero)
				{
					vdl=da;
					if (da<0.01f*degToRad) // Was da==0.0f before (tolerance problem). Changed on 1/4/2011
						vdl=dl;
				}
				// vld is the totalvirtual distance
				float currentPos=0.0f;
				bool movementFinished=false;
				float dt=App::ct->simulation->getSimulationTimeStep_speedModified(); // this is the time left if we leave here

				if (vdl==0.0f)
				{ // if the path length is 0 (the two positions might still be not-coincident, depending on the calculation method!)
					if (App::ct->objCont->getObject(objID)==object) // make sure the object is still valid (running in a thread)
					{
						if (relToObject==NULL)
						{ // absolute
							C7Vector parentInv(object->getParentCumulativeTransformation().getInverse());
							object->setLocalTransformation(parentInv*targetTr);
						}
						else
						{ // relative to a specific object (2009/11/17)
							if (App::ct->objCont->getObject(relativeToObjID)==relToObject) // make sure the object is still valid (running in a thread)
							{ // ok
								C7Vector relToTr(relToObject->getCumulativeTransformation());
								targetTr=relToTr*targetTr;
								C7Vector parentInv(object->getParentCumulativeTransformation().getInverse());
								object->setLocalTransformation(parentInv*targetTr);
							}
						}
					}
					movementFinished=true;
				}

				while (!movementFinished)
				{
					float currentTime=App::ct->simulation->getSimulationTime()+App::ct->simulation->getSimulationTimeStep_speedModified();
					dt=currentTime-lastTime;
					lastTime=currentTime;

					if (accel==0.0f)
					{ // Means infinite acceleration
						float timeNeeded=(vdl-currentPos)/maxVelocity;
						currentVel=maxVelocity;
						if (timeNeeded>dt)
						{
							currentPos+=dt*maxVelocity;
							dt=0.0f; // this is what is left
						}
						else
						{
							currentPos=vdl;
							if (timeNeeded>=0.0f)
								dt-=timeNeeded;
						}
					}
					else
					{
						double p=currentPos;
						double v=currentVel;
						double t=dt;
						CLinMotionRoutines::getNextValues(p,v,maxVelocity,accel,0.0f,vdl,0.0f,0.0f,t);
						currentPos=float(p);
						currentVel=float(v);
						dt=float(t);
					}

					// Now check if we are within tolerances:
					if (fabs(currentPos-vdl)<=0.00001f)//tol[0])
						movementFinished=true;

					// Set the new configuration of the object:
					float ll=currentPos/vdl;
					if (ll>1.0f)
						ll=1.0f;
					C7Vector newAbs;
					newAbs.buildInterpolation(startTr,targetTr,ll);
					if (App::ct->objCont->getObject(objID)==object) // make sure the object is still valid (running in a thread)
					{
						if ( (relToObject!=NULL)&&(App::ct->objCont->getObject(relativeToObjID)!=relToObject) )
							movementFinished=true; // the object was destroyed during execution of the command!
						else
						{
							C7Vector parentInv(object->getParentCumulativeTransformation().getInverse());
							C7Vector currAbs(object->getCumulativeTransformationPart1());
							C7Vector relToTr;
							relToTr.setIdentity();
							if (relToObject!=NULL)
								relToTr=relToObject->getCumulativeTransformation();
							currAbs=relToTr.getInverse()*currAbs;
							if ((posAndOrient&1)==0)
								newAbs.X=currAbs.X;
							if ((posAndOrient&2)==0)
								newAbs.Q=currAbs.Q;
							newAbs=relToTr*newAbs;
							object->setLocalTransformation(parentInv*newAbs);
						}
					}
					else
						movementFinished=true; // the object was destroyed during execution of the command!

					if (!movementFinished)
					{
						LUA_UNLOCK; // Important!!
						CThreadPool::switchBackToPreviousThread();
						if (CThreadPool::getStopForCurrentInstanceRequested())
						{
							LUA_LOCK; // Important!!
							lua_pushnil(L); // error
							break;
						}
						LUA_LOCK; // Important!!
					}
				}
				// The movement finished. Now add the time used:
				threadData->usedMovementTime=App::ct->simulation->getSimulationTimeStep_speedModified()-dt;
				lua_pushnumber(L,dt); // success (deltaTime left)
			}
		}
		else
			lua_pushnil(L); // error
	}
	LUA_END(1);
}

int _simMoveToJointPositions(lua_State* L)
{ // can only be called from a script running in a thread!! DEPRECATED
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simMoveToJointPositions");
	if (VThread::isCurrentThreadTheMainThread())
	{
		outputCallErrorMessage(functionName.c_str(),SIM_ERROR_CAN_ONLY_BE_CALLED_FROM_A_THREAD,true);
		lua_pushnil(L); // error
	}
	else
	{
		if (CThreadPool::getStopForCurrentInstanceRequested())
		{ // Important to first check if we are supposed to leave the thread!!
			lua_pushnil(L); // special error
			LUA_END(1);
		}
        if ( (!lua_istable(L,1))||(!lua_istable(L,2))||(VREP_LUA_OBJLEN(L,1)>VREP_LUA_OBJLEN(L,2))||(VREP_LUA_OBJLEN(L,1)==0) )
		{
			outputCallErrorMessage(functionName.c_str(),"One of the function's argument type is not correct or table sizes are invalid or do not match",true);
			lua_pushnil(L); // error
		}
		else
		{ // Ok we have 2 tables with same sizes.
            int tableLen=VREP_LUA_OBJLEN(L,1);
			bool sameTimeFinish=true;
			float maxVelocity=0.0f;
			float accel=0.0f; // means infinite accel!! (default value)
			bool accelTablePresent=false;
			float angleToLinearCoeff=1.0f; 
			bool foundError=false;
			// Now check the other arguments:
			int res;
			if (lua_istable(L,3))
				sameTimeFinish=false; // we do not finish at the same time!
			if (!foundError) // velocity or velocities argument (not optional!):
			{
				if (sameTimeFinish)
				{
					res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,0,false,false,true);
					if (res==2)
						maxVelocity=luaToFloat(L,3);
					else
						foundError=true;
				}
				else
				{
					res=checkOneGeneralInputArgument(L,3,functionName,lua_arg_number,tableLen,false,false,true);
					if (res!=2)
						foundError=true;
				}
			}
			if (!foundError) // Accel argument:
			{
				if (sameTimeFinish)
				{
					res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,0,true,true,true);
					if (res==2)
					{ // get the data
						accel=fabs(luaToFloat(L,4));
					}
					foundError=(res==-1);
				}
				else
				{
					res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,tableLen,true,true,true);
					if (res==2)
						accelTablePresent=true;
					foundError=(res==-1);
				}
			}
			if (!foundError) // angleToLinearCoeff argument:
			{
				if (sameTimeFinish)
				{
					res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,0,true,true,true);
					if (res==2)
					{ // get the data
						angleToLinearCoeff=fabs(luaToFloat(L,5));
					}
					foundError=(res==-1);
				}
				else
					angleToLinearCoeff=1.0f; // no conversion!
			}
			if (foundError)
				lua_pushnil(L); // error
			else
			{ // do the job here!
				int* jointHandles=new int[tableLen];
				float* jointCurrentVirtualPositions=new float[tableLen];
				float* jointCurrentVirtualVelocities=new float[tableLen];
				float* jointStartPositions=new float[tableLen];
				float* jointTargetPositions=new float[tableLen];
				float* jointVirtualDistances=new float[tableLen];
				float* jointMaxVelocities=new float[tableLen];
				float* jointAccels=new float[tableLen];

				getIntsFromTable(L,1,tableLen,jointHandles);
				getFloatsFromTable(L,2,tableLen,jointTargetPositions);
				if (!sameTimeFinish)
				{
					getFloatsFromTable(L,3,tableLen,jointMaxVelocities);
					for (int i=0;i<tableLen;i++)
						jointMaxVelocities[i]=fabs(jointMaxVelocities[i]);
					if (accelTablePresent)
					{
						getFloatsFromTable(L,4,tableLen,jointAccels);
						for (int i=0;i<tableLen;i++)
							jointAccels[i]=fabs(jointAccels[i]);
					}
				}
				float maxVirtualDist=0.0f;
				int maxVirtualDistIndex=0;
				for (int i=0;i<tableLen;i++)
				{
					jointCurrentVirtualPositions[i]=0.0f;
					jointCurrentVirtualVelocities[i]=0.0f;
					if (sameTimeFinish)
						jointMaxVelocities[i]=maxVelocity;
					if (!accelTablePresent)
						jointAccels[i]=accel;

					CJoint* it=App::ct->objCont->getJoint(jointHandles[i]);
					if ((it!=NULL)&&(it->getJointType()!=sim_joint_spherical_subtype))
					{ // make sure target is within allowed range, and check the maximum virtual distance:
						jointStartPositions[i]=it->getPosition();
						float minP=it->getPositionIntervalMin();
						float maxP=minP+it->getPositionIntervalRange();
						if (it->getPositionIsCyclic())
						{
							float da=tt::getAngleMinusAlpha(jointTargetPositions[i],jointStartPositions[i]);
							jointTargetPositions[i]=jointStartPositions[i]+da;
						}
						else
						{
							if (minP>jointTargetPositions[i])
								jointTargetPositions[i]=minP;
							if (maxP<jointTargetPositions[i])
								jointTargetPositions[i]=maxP;
						}
						float d=fabs(jointTargetPositions[i]-jointStartPositions[i]);
						if (it->getJointType()==sim_joint_revolute_subtype)
							d*=angleToLinearCoeff;
						jointVirtualDistances[i]=d;
						if (d>maxVirtualDist)
						{
							maxVirtualDist=d;
							maxVirtualDistIndex=i;
						}
					}
					else
					{
						// Following are default values in case the joint doesn't exist or is spherical:
						jointStartPositions[i]=0.0f;
						jointTargetPositions[i]=0.0f;
						jointVirtualDistances[i]=0.0f;
					}
				}
				CVThreadData* threadData=CThreadPool::getCurrentThreadData();
				float lastTime=App::ct->simulation->getSimulationTime()+threadData->usedMovementTime;
				bool movementFinished=false;
				float dt=App::ct->simulation->getSimulationTimeStep_speedModified(); // this is the time left if we leave here

				if (maxVirtualDist==0.0f)
					movementFinished=true;


				while (!movementFinished)
				{
					float currentTime=App::ct->simulation->getSimulationTime()+App::ct->simulation->getSimulationTimeStep_speedModified();
					dt=currentTime-lastTime;
					float minTimeLeft=dt;
					lastTime=currentTime;
					if (sameTimeFinish)
					{
						float timeLeftLocal=dt;
						// 1. handle the joint with longest distance first:
						// Does the main joint still exist?
						if (App::ct->objCont->getJoint(jointHandles[maxVirtualDistIndex])!=NULL)
						{
							if (accel==0.0f)
							{ // means infinite accel
								float timeNeeded=(jointVirtualDistances[maxVirtualDistIndex]-jointCurrentVirtualPositions[maxVirtualDistIndex])/maxVelocity;
								jointCurrentVirtualVelocities[maxVirtualDistIndex]=maxVelocity;
								if (timeNeeded>timeLeftLocal)
								{
									jointCurrentVirtualPositions[maxVirtualDistIndex]+=timeLeftLocal*maxVelocity;
									timeLeftLocal=0.0f; // this is what is left
								}
								else
								{
									jointCurrentVirtualPositions[maxVirtualDistIndex]=jointVirtualDistances[maxVirtualDistIndex];
									if (timeNeeded>=0.0f)
										timeLeftLocal-=timeNeeded;
								}
							}
							else
							{
								double p=jointCurrentVirtualPositions[maxVirtualDistIndex];
								double v=jointCurrentVirtualVelocities[maxVirtualDistIndex];
								double t=timeLeftLocal;
								CLinMotionRoutines::getNextValues(p,v,maxVelocity,accel,0.0f,jointVirtualDistances[maxVirtualDistIndex],0.0f,0.0f,t);
								jointCurrentVirtualPositions[maxVirtualDistIndex]=float(p);
								jointCurrentVirtualVelocities[maxVirtualDistIndex]=float(v);
								timeLeftLocal=float(t);
							}
							minTimeLeft=timeLeftLocal;
							// 2. We adjust the other joints accordingly:
							float f=1;
							if (jointVirtualDistances[maxVirtualDistIndex]!=0.0f)
								f=jointCurrentVirtualPositions[maxVirtualDistIndex]/jointVirtualDistances[maxVirtualDistIndex];
							for (int i=0;i<tableLen;i++)
							{
								if (i!=maxVirtualDistIndex)
									jointCurrentVirtualPositions[i]=jointVirtualDistances[i]*f;
							}
							// 3. Check if within tolerance:
							if (fabs(jointCurrentVirtualPositions[maxVirtualDistIndex]-jointVirtualDistances[maxVirtualDistIndex])<=0.00001f)
								movementFinished=true;
						}
						else
						{ // the main joint was removed. End here!
							movementFinished=true;
						}
					}
					else
					{
						bool withinTolerance=true;
						for (int i=0;i<tableLen;i++)
						{
							if (App::ct->objCont->getJoint(jointHandles[i])!=NULL)
							{
								// Check if within tolerance (before):
								if (fabs(jointCurrentVirtualPositions[i]-jointVirtualDistances[i])>0.00001f)
								{
									float timeLeftLocal=dt;
									if (jointAccels[i]==0.0f)
									{ // means infinite accel
										float timeNeeded=(jointVirtualDistances[i]-jointCurrentVirtualPositions[i])/jointMaxVelocities[i];
										jointCurrentVirtualVelocities[i]=jointMaxVelocities[i];
										if (timeNeeded>timeLeftLocal)
										{
											jointCurrentVirtualPositions[i]+=timeLeftLocal*jointMaxVelocities[i];
											timeLeftLocal=0.0f; // this is what is left
										}
										else
										{
											jointCurrentVirtualPositions[i]=jointVirtualDistances[i];
											if (timeNeeded>=0.0f)
												timeLeftLocal-=timeNeeded;
										}
									}
									else
									{
										double p=jointCurrentVirtualPositions[i];
										double v=jointCurrentVirtualVelocities[i];
										double t=timeLeftLocal;
										CLinMotionRoutines::getNextValues(p,v,jointMaxVelocities[i],jointAccels[i],0.0f,jointVirtualDistances[i],0.0f,0.0f,t);
										jointCurrentVirtualPositions[i]=float(p);
										jointCurrentVirtualVelocities[i]=float(v);
										timeLeftLocal=float(t);
									}
									if (timeLeftLocal<minTimeLeft)
										minTimeLeft=timeLeftLocal;
									// Check if within tolerance (after):
									if (fabs(jointCurrentVirtualPositions[i]-jointVirtualDistances[i])>0.00001f)
										withinTolerance=false;
								}
							}
						}
						if (withinTolerance)
							movementFinished=true;
					}
					dt=minTimeLeft;

					// We set all joint positions:
					for (int i=0;i<tableLen;i++)
					{
						CJoint* joint=App::ct->objCont->getJoint(jointHandles[i]);
						if ( (joint!=NULL)&&(joint->getJointType()!=sim_joint_spherical_subtype)&&(jointVirtualDistances[i]!=0.0f) )
						{
							if (joint->getJointMode()==sim_jointmode_force)
								joint->setDynamicMotorPositionControlTargetPosition(jointStartPositions[i]+(jointTargetPositions[i]-jointStartPositions[i])*jointCurrentVirtualPositions[i]/jointVirtualDistances[i]);
							else
								joint->setPosition(jointStartPositions[i]+(jointTargetPositions[i]-jointStartPositions[i])*jointCurrentVirtualPositions[i]/jointVirtualDistances[i]);
						}
					}

					if (!movementFinished)
					{
						LUA_UNLOCK; // Important!!
						CThreadPool::switchBackToPreviousThread();
						if (CThreadPool::getStopForCurrentInstanceRequested())
						{
							LUA_LOCK; // Important!!
							lua_pushnil(L); // error
							break;
						}
						LUA_LOCK; // Important!!
					}
				}
				// The movement finished. Now add the time used:
				threadData->usedMovementTime=App::ct->simulation->getSimulationTimeStep_speedModified()-dt;
				lua_pushnumber(L,dt); // success (deltaTime left)

				delete[] jointAccels;
				delete[] jointMaxVelocities;
				delete[] jointVirtualDistances;
				delete[] jointTargetPositions;
				delete[] jointStartPositions;
				delete[] jointCurrentVirtualVelocities;
				delete[] jointCurrentVirtualPositions;
				delete[] jointHandles;
			}
		}
	}
	LUA_END(1);
}

int _simSerialPortOpen(lua_State* L)
{ // For backward compatibility (10/04/2012) DEPRECATED
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSerialPortOpen");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0))
	{
		CSerialPort* port=new CSerialPort(true);
		std::string theString("\\\\.\\COM");
		int prtNb=luaToInt(L,1);
		theString+=tt::FNb(prtNb);
		if (port->open(theString.c_str(),luaToInt(L,2)))
		{
			port->setPortNumber_backwardCompatibility(prtNb);
			App::ct->serialPortContainer->addOpenedPort(port);
			retVal=1;
		}
		else
			delete port;
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSerialPortClose(lua_State* L)
{ // For backward compatibility (10/04/2012) DEPRECATED
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSerialPortClose");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
		retVal=simSerialPortClose_internal(luaToInt(L,1));
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSerialPortSend(lua_State* L)
{ // For backward compatibility (10/04/2012) DEPRECATED
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSerialPortSend");
	int retVal=-1; // means error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_string,0))
	{
		size_t dataLength;
		char* data=(char*)lua_tolstring(L,2,&dataLength);
		retVal=simSerialPortSend_internal(luaToInt(L,1),data,dataLength);
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simSerialPortRead(lua_State* L)
{ // For backward compatibility (10/04/2012) DEPRECATED
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simSerialPortRead");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_bool,0))
	{
		bool blocking=luaToBool(L,3);
		std::string closingString="";
		float timeOut=0.0f;
		if (blocking)
		{
			if (VThread::isCurrentThreadTheMainThread())
			{
				outputCallErrorMessage(functionName.c_str(),SIM_ERROR_BLOCKING_OPERATION_ONLY_FROM_THREAD,true);
				lua_pushnil(L);
				LUA_END(1);
			}
			
			int res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_string,0,true,true,true);
			if (res==2)
			{
				size_t dataLength;
				char* data=(char*)lua_tolstring(L,4,&dataLength);
				closingString.assign(data,dataLength);
			}
			if (res==-1)
			{
				lua_pushnil(L);
				LUA_END(1);
			}
	
			res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,0,true,false,true);
			if (res==2)
				timeOut=luaToFloat(L,5);
			if (res==-1)
			{
				lua_pushnil(L);
				LUA_END(1);
			}
		}
		int maxLength=luaToInt(L,2);
		char* data=new char[maxLength];
		int dataRead=0;
		if (blocking)
		{
			int startTime=VDateTime::getTimeInMs();
			LUA_UNLOCK; // Important!!
			while (maxLength>dataRead)
			{
				int nb=simSerialPortRead_internal(luaToInt(L,1),data+dataRead,maxLength-dataRead);
				if (nb<0)
					break; // error
				dataRead+=nb;
				int csl=int(closingString.length());
				if ( (csl!=0)&&(dataRead>=csl) )
				{ // Break if we find the closing signature:
					bool same=true;
					for (int i=0;i<csl;i++)
					{
						if (data[dataRead-csl+i]!=closingString[i])
						{
							same=false;
							break;
						}
					}
					if (same)
					{
						dataRead-=csl;
						break;
					}
				}
				if (maxLength>dataRead)
				{
					CThreadPool::switchBackToPreviousThread();
					if (CThreadPool::getStopForCurrentInstanceRequested())
						break;
				}
				if ( (timeOut>0.0000001f)&&((float(VDateTime::getTimeDiffInMs(startTime))/1000.0f)>timeOut) )
					break;
			}
			LUA_LOCK; // Important!!
		}
		else
			dataRead=simSerialPortRead_internal(luaToInt(L,1),data,maxLength);
		if (dataRead>0)
			lua_pushlstring(L,(const char*)data,dataRead);
		else
			lua_pushnil(L);
		delete[] data;
		LUA_END(1);
	}
	lua_pushnil(L);
	LUA_END(1);
}

int _simGetInstanceIndex(lua_State* L)
{ // DEPRECATED
	LUA_API_FUNCTION_DEBUG;
	LUA_START("");
	lua_pushnumber(L,0);
	LUA_END(1);
}

int _simGetVisibleInstanceIndex(lua_State* L)
{ // DEPRECATED
	LUA_API_FUNCTION_DEBUG;
	LUA_START("");
	lua_pushnumber(L,0);
	LUA_END(1);
}

int _simGetSystemTimeInMilliseconds(lua_State* L)
{ // DEPRECATED
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simGetSystemTimeInMilliseconds");
	int res=checkOneGeneralInputArgument(L,1,functionName,lua_arg_number,0,true,false,true);
	if (res==0)
	{
		lua_pushnumber(L,VDateTime::getTimeInMs());
		LUA_END(1);
	}
	if (res==2)
	{
		int lastTime=luaToInt(L,1);
		lua_pushnumber(L,VDateTime::getTimeDiffInMs(lastTime));
		LUA_END(1);
	}
	LUA_END(0);
}

int _simLockInterface(lua_State* L)
{ // DEPRECATED
	LUA_API_FUNCTION_DEBUG;
	LUA_START_NO_LOCK("simLockInterface");
	lua_pushnumber(L,-1);
	LUA_END_NO_LOCK(1);
}

int _simJointGetForce(lua_State* L)
{ // DEPRECATED since release 3.1.2
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simJointGetForce");
	if (checkInputArguments(L,functionName,true,lua_arg_number,0))
	{
		float jointF[1];
		if (simGetJointForce_internal(luaToInt(L,1),jointF)!=-1)
			lua_pushnumber(L,jointF[0]);
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	LUA_END(1);
}

int _simRMLPosition(lua_State* L)
{ // DEPRECATED since release 3.1.2
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simRMLPosition");
	outputCallWarningMessage(functionName.c_str(),"Function is deprecated. Use simRMLPos instead.");
	int retVal=-43; //error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int dofs=luaToInt(L,1);
		double timeStep=lua_tonumber(L,2);
		int flags=luaToInt(L,3);
		int res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,dofs*3,false,false,true);
		if (res==2)
		{
			double* currentPosVelAccel=new double[dofs*3];
			getDoublesFromTable(L,4,dofs*3,currentPosVelAccel);
			res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,dofs*3,false,false,true);
			if (res==2)
			{
				double* maxVelAccelJerk=new double[dofs*3];
				getDoublesFromTable(L,5,dofs*3,maxVelAccelJerk);
				res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_bool,dofs,false,false,true);
				if (res==2)
				{
					char* selection=new char[dofs];
					getBoolsFromTable(L,6,dofs,selection);
					res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,dofs*2,false,false,true);
					if (res==2)
					{
						double* targetPosVel=new double[dofs*2];
						getDoublesFromTable(L,7,dofs*2,targetPosVel);

						double* newPosVelAccel=new double[dofs*3];
						BYTE auxData[1+8+8];
						auxData[0]=1;
						retVal=simRMLPosition_internal(dofs,timeStep,flags,currentPosVelAccel,maxVelAccelJerk,(unsigned char*)selection,targetPosVel,newPosVelAccel,auxData);
						if (retVal>=0)
						{
							lua_pushnumber(L,retVal);
							pushDoubleTableOntoStack(L,dofs*3,newPosVelAccel);
							lua_pushnumber(L,((double*)(auxData+1))[0]);
						}
						delete[] newPosVelAccel;
						delete[] targetPosVel;
					}
					delete[] selection;
				}
				delete[] maxVelAccelJerk;
			}
			delete[] currentPosVelAccel;
			if (retVal>=0)
			{
				LUA_END(3);
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}

int _simRMLVelocity(lua_State* L)
{ // DEPRECATED since release 3.1.2
	LUA_API_FUNCTION_DEBUG;
	LUA_START("simRMLVelocity");
	outputCallWarningMessage(functionName.c_str(),"Function is deprecated. Use simRMLVel instead.");
	int retVal=-43; //error
	if (checkInputArguments(L,functionName,true,lua_arg_number,0,lua_arg_number,0,lua_arg_number,0))
	{
		int dofs=luaToInt(L,1);
		double timeStep=lua_tonumber(L,2);
		int flags=luaToInt(L,3);
		int res=checkOneGeneralInputArgument(L,4,functionName,lua_arg_number,dofs*3,false,false,true);
		if (res==2)
		{
			double* currentPosVelAccel=new double[dofs*3];
			getDoublesFromTable(L,4,dofs*3,currentPosVelAccel);
			res=checkOneGeneralInputArgument(L,5,functionName,lua_arg_number,dofs*2,false,false,true);
			if (res==2)
			{
				double* maxAccelJerk=new double[dofs*2];
				getDoublesFromTable(L,5,dofs*2,maxAccelJerk);
				res=checkOneGeneralInputArgument(L,6,functionName,lua_arg_bool,dofs,false,false,true);
				if (res==2)
				{
					char* selection=new char[dofs];
					getBoolsFromTable(L,6,dofs,selection);
					res=checkOneGeneralInputArgument(L,7,functionName,lua_arg_number,dofs,false,false,true);
					if (res==2)
					{
						double* targetVel=new double[dofs];
						getDoublesFromTable(L,7,dofs,targetVel);

						double* newPosVelAccel=new double[dofs*3];
						BYTE auxData[1+8+8];
						auxData[0]=1;
						retVal=simRMLVelocity_internal(dofs,timeStep,flags,currentPosVelAccel,maxAccelJerk,(unsigned char*)selection,targetVel,newPosVelAccel,auxData);
						if (retVal>=0)
						{
							lua_pushnumber(L,retVal);
							pushDoubleTableOntoStack(L,dofs*3,newPosVelAccel);
							lua_pushnumber(L,((double*)(auxData+1))[0]);
						}
						delete[] newPosVelAccel;
						delete[] targetVel;
					}
					delete[] selection;
				}
				delete[] maxAccelJerk;
			}
			delete[] currentPosVelAccel;
			if (retVal>=0)
			{
				LUA_END(3);
			}
		}
	}
	lua_pushnumber(L,retVal);
	LUA_END(1);
}
