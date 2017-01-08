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

#include <string>
#include <vector>
#include <QLibrary>

typedef  unsigned char (__cdecl *ptrStart)(void*,int);
typedef  void (__cdecl *ptrEnd)(void);
typedef  void* (__cdecl *ptrMessage)(int,int*,void*,int*);

class CPlugin
{
public:
	CPlugin(const char* filename,const char* pluginName);
	virtual ~CPlugin();
	int load();
	void* sendEventCallbackMessage(int msg,int* auxVals,void* data,int retVals[4]);

	ptrStart startAddress;
	ptrEnd endAddress;
	ptrMessage messageAddress;

	std::string name;
	std::string _filename;
	unsigned char pluginVersion;
	int handle;
	int _loadCount;
	QLibrary* instance;
};


class CPluginContainer  
{
public:
	CPluginContainer();
	virtual ~CPluginContainer();

	static int addPlugin(const char* filename,const char* pluginName);
	static void* sendEventCallbackMessageToAllPlugins(int msg,int* auxVals,void* data,int retVals[4]);
	static CPlugin* getPluginFromName(const char* pluginName);
	static CPlugin* getPluginFromIndex(int index);
	static bool killPlugin(int handle);
	static void killAllPlugins();



	static bool enableOrDisableSpecificEventCallback(int eventCallbackType,const char* pluginName);
	static void sendSpecialEventCallbackMessageToSomePlugins(int msg,int* auxVals,void* data,int retVals[4]);

	static bool shouldSend_openglframe_msg();
	static bool shouldSend_openglcameraview_msg();
private:
	static int _nextHandle;
	static std::vector<CPlugin*> _allPlugins;

	static std::vector<std::string> _renderingpass_eventEnabledPluginNames;
	static std::vector<std::string> _opengl_eventEnabledPluginNames;
	static std::vector<std::string> _openglframe_eventEnabledPluginNames;
	static std::vector<std::string> _openglcameraview_eventEnabledPluginNames;
};
