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
#include "pluginContainer.h"
#include "v_repConst.h"
#include "collDistInterface.h"
#include "dynInterface.h"
#include "pathPlanningInterface.h"

CPlugin::CPlugin(const char* filename,const char* pluginName)
{
	_filename=filename;
	name=pluginName;
	instance=NULL;
	_loadCount=1;
}

CPlugin::~CPlugin()
{
	if (instance!=NULL)
	{
		instance->unload();
		delete instance;
	}
}

int CPlugin::load()
{
	QLibrary* lib=new QLibrary(_filename.c_str()); // here we have the extension in the filename (.dll, .so or .dylib)
	if (!lib->load())
	{
		delete lib;
		lib=NULL;
	}
	if (lib!=NULL)
	{
		instance=lib;
		startAddress=(ptrStart)(lib->resolve("v_repStart"));
		endAddress=(ptrEnd)(lib->resolve("v_repEnd"));
		messageAddress=(ptrMessage)(lib->resolve("v_repMessage"));
		if ( (startAddress!=NULL)&&(endAddress!=NULL)&&(messageAddress!=NULL) )
		{
			pluginVersion=startAddress(NULL,0);
			if (pluginVersion!=0)
				return(int(pluginVersion)); // success!
			instance->unload();
			delete instance;
			instance=NULL;
			return(0); // could not properly initialize
		}
		else
		{

			instance->unload();
			delete instance;
			instance=NULL;
			return(-1); // missing entry points
		}
	}
	else
		return(-2); // could not load
}

void* CPlugin::sendEventCallbackMessage(int msg,int* auxVals,void* data,int retVals[4])
{
	return(messageAddress(msg,auxVals,data,retVals));
}


int CPluginContainer::_nextHandle=0;
std::vector<CPlugin*> CPluginContainer::_allPlugins;

std::vector<std::string> CPluginContainer::_renderingpass_eventEnabledPluginNames;
std::vector<std::string> CPluginContainer::_opengl_eventEnabledPluginNames;
std::vector<std::string> CPluginContainer::_openglframe_eventEnabledPluginNames;
std::vector<std::string> CPluginContainer::_openglcameraview_eventEnabledPluginNames;

CPluginContainer::CPluginContainer()
{
}

CPluginContainer::~CPluginContainer()
{
}

int CPluginContainer::addPlugin(const char* filename,const char* pluginName)
{
	FUNCTION_DEBUG;
	if (strlen(filename)==0)
	{		
		CCollDistInterface::initializeFunctionsIfNeeded();
		CDynInterface::initializeFunctionsIfNeeded();
		CPathPlanningInterface::initializeFunctionsIfNeeded();

		CCollDistInterface::setSystemPluginsLoadPhaseOver();
		CDynInterface::setSystemPluginsLoadPhaseOver();
		CPathPlanningInterface::setSystemPluginsLoadPhaseOver();
		return(-1);
	}
	FUNCTION_INSIDE_DEBUG(pluginName);

	CPlugin* plug=getPluginFromName(pluginName);
	if (plug!=NULL)
	{
		plug->_loadCount++;
		return(plug->handle);
	}
	plug=new CPlugin(filename,pluginName);
	int loadRes=plug->load();
	if (loadRes<=0)
	{ // failed
		delete plug;
		return(loadRes-1);
	}
	plug->handle=_nextHandle;
	_nextHandle++;
	_allPlugins.push_back(plug);
	return(plug->handle);
}

CPlugin* CPluginContainer::getPluginFromName(const char* pluginName)
{
	for (int i=0;i<int(_allPlugins.size());i++)
	{
		if (_allPlugins[i]->name==std::string(pluginName))
			return(_allPlugins[i]);
	}
	return(NULL);
}

CPlugin* CPluginContainer::getPluginFromIndex(int index)
{
	if ((index<0)||(index>=int(_allPlugins.size())))
		return(NULL);
	return(_allPlugins[index]);
}

bool CPluginContainer::killPlugin(int handle)
{
	FUNCTION_DEBUG;
	for (int i=0;i<int(_allPlugins.size());i++)
	{
		if (_allPlugins[i]->handle==handle)
		{
			FUNCTION_INSIDE_DEBUG(_allPlugins[i]->name.c_str());
			if (_allPlugins[i]->_loadCount==1)
			{ // will unload it
				_allPlugins[i]->endAddress();
				delete _allPlugins[i];
				_allPlugins.erase(_allPlugins.begin()+i);
				return(true);
			}
			else
			{ // cannot yet unload it... others might still depend on it!
				_allPlugins[i]->_loadCount--;
				return(false);
			}
		}
	}
	return(false);
}

void CPluginContainer::killAllPlugins()
{
	for (int i=0;i<int(_allPlugins.size());i++)
	{
		_allPlugins[i]->endAddress();
		delete _allPlugins[i];
	}
	_allPlugins.clear();
}

void* CPluginContainer::sendEventCallbackMessageToAllPlugins(int msg,int* auxVals,void* data,int retVals[4])
{
	bool special=false;
	int memorized[4]={0,0,0,0};
	for (int i=0;i<int(_allPlugins.size());i++)
	{
		if (retVals!=NULL)
		{
			retVals[0]=-1;
			retVals[1]=-1;
			retVals[2]=-1;
			retVals[3]=-1;
		}
		void* returnData=_allPlugins[i]->messageAddress(msg,auxVals,data,retVals);
		if ( (returnData!=NULL)||((retVals!=NULL)&&((retVals[0]!=-1)||(retVals[1]!=-1)||(retVals[2]!=-1)||(retVals[3]!=-1))) )
		{
			if (msg!=sim_message_eventcallback_mainscriptabouttobecalled) // this message is handled in a special fashion, because the remoteApi and ROS might interfere otherwise!
				return(returnData); // We interrupt the callback forwarding!
			memorized[0]=retVals[0];
			memorized[1]=retVals[1];
			memorized[2]=retVals[2];
			memorized[3]=retVals[3];
			special=true;
		}
	}
	if (special)
	{
		retVals[0]=memorized[0];
		retVals[1]=memorized[1];
		retVals[2]=memorized[2];
		retVals[3]=memorized[3];
	}
	return(NULL);
}

bool CPluginContainer::shouldSend_openglframe_msg()
{
	return(_openglframe_eventEnabledPluginNames.size()>0);
}

bool CPluginContainer::shouldSend_openglcameraview_msg()
{
	return(_openglcameraview_eventEnabledPluginNames.size()>0);
}

bool CPluginContainer::enableOrDisableSpecificEventCallback(int eventCallbackType,const char* pluginName)
{ // resources should normally be locked at this stage
	std::vector<std::string>* vect=NULL;
	if (eventCallbackType==sim_message_eventcallback_renderingpass)
		vect=&_renderingpass_eventEnabledPluginNames;
	if (eventCallbackType==sim_message_eventcallback_opengl)
		vect=&_opengl_eventEnabledPluginNames;
	if (eventCallbackType==sim_message_eventcallback_openglframe)
		vect=&_openglframe_eventEnabledPluginNames;
	if (eventCallbackType==sim_message_eventcallback_openglcameraview)
		vect=&_openglcameraview_eventEnabledPluginNames;

	bool present=false;
	for (int i=0;i<int(vect->size());i++)
	{
		if (vect->at(i).compare(pluginName)==0)
		{ // already there. We remove it
			present=true;
			vect->erase(vect->begin()+i);
			break;
		}
	}
	if (!present)
		vect->push_back(pluginName); // not yet there, we add it
	return(present);
}

void CPluginContainer::sendSpecialEventCallbackMessageToSomePlugins(int msg,int* auxVals,void* data,int retVals[4])
{
	std::vector<std::string>* vect=NULL;
	if (msg==sim_message_eventcallback_renderingpass)
		vect=&_renderingpass_eventEnabledPluginNames;
	if (msg==sim_message_eventcallback_opengl)
		vect=&_opengl_eventEnabledPluginNames;
	if (msg==sim_message_eventcallback_openglframe)
		vect=&_openglframe_eventEnabledPluginNames;
	if (msg==sim_message_eventcallback_openglcameraview)
		vect=&_openglcameraview_eventEnabledPluginNames;

	if (vect!=NULL)
	{
		for (int i=0;i<int(vect->size());i++)
		{
			CPlugin* plug=getPluginFromName(vect->at(i).c_str());
			if (plug!=NULL)
			{
				void* returnData=plug->messageAddress(msg,auxVals,data,retVals);
				if (returnData!=NULL)
					delete[] (char*)returnData;
			}
		}
	}
}
