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
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#include <QtCore/QCoreApplication>
#include "v_repLib.h"
#include <vector>
#include <QLibrary>
#include <QFileInfo>
#include <QDir>
#include <iostream>

#ifdef WIN_VREP
#include <direct.h>
#endif

// Following required to have Lua extension libraries work under Linux. Very strange indeed.
//****************************************************************************
#ifdef LINUX_VREP
extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}
void dummyFunction()
{
    lua_State *L;
    L=luaL_newstate();
}
#endif
//****************************************************************************

std::vector<int> pluginHandles;
std::string sceneOrModelOrUiToLoad;
bool autoStart=false;
int simStopDelay=0;
bool autoQuit=false;

int loadPlugin(const char* theName,const char* theDirAndName)
{
	std::cout << "Plugin '" << theName << "': loading...\n";
	int pluginHandle=simLoadModule(theDirAndName,theName);
	if (pluginHandle==-3)
#ifdef WIN_VREP
		std::cout << "Plugin '" << theName << "': load failed (could not load). The plugin probably couldn't load dependency libraries. Try rebuilding the plugin.\n";
#endif
#ifdef MAC_VREP
		std::cout << "Plugin '" << theName << "': load failed (could not load). The plugin probably couldn't load dependency libraries. Try 'otool -L pluginName.dylib' for more infos, or simply rebuild the plugin.\n";
#endif
#ifdef LINUX_VREP
		std::cout << "Plugin '" << theName << "': load failed (could not load). The plugin probably couldn't load dependency libraries. Try 'ldd pluginName.so' for more infos, or simply rebuild the plugin.\n";
#endif

	if (pluginHandle==-2)
		std::cout << "Plugin '" << theName << "': load failed (missing entry points).\n";
	if (pluginHandle==-1)
		std::cout << "Plugin '" << theName << "': load failed (failed initialization).\n";
	if (pluginHandle>=0)
		std::cout << "Plugin '" << theName << "': load succeeded.\n";
	return(pluginHandle);
}

void simulatorInit()
{
	std::cout << "Simulator launched.\n";

	QFileInfo pathInfo(QCoreApplication::applicationFilePath());
	std::string pa=pathInfo.path().toStdString();
	QDir dir(pa.c_str());
	dir.setFilter(QDir::Files|QDir::Hidden); // |QDir::NoSymLinks); // removed on 11/4/2013 thanks to Karl Robillard
	dir.setSorting(QDir::Name);
	QStringList filters;
	int bnl=8;
#ifdef WIN_VREP
	std::string tmp("v_repExt*.dll");
#endif
#ifdef MAC_VREP
	std::string tmp("libv_repExt*.dylib");
	bnl=11;
#endif
#ifdef LINUX_VREP
	std::string tmp("libv_repExt*.so");
	bnl=11;
#endif
	filters << tmp.c_str();
	dir.setNameFilters(filters);
	QFileInfoList list=dir.entryInfoList();
	std::vector<std::string> theNames;
	std::vector<std::string> theDirAndNames;
	for (int i=0;i<list.size();++i)
	{
		QFileInfo fileInfo=list.at(i);
		std::string bla(fileInfo.baseName().toLocal8Bit());
		std::string tmp;
		if (bnl==int(bla.size()))
			tmp="VrepExt"; // This is the extension module of v_rep (exception in naming)!
		else
			tmp.assign(bla.begin()+bnl,bla.end());

		bool underscoreFound=false;
		for (int i=0;i<int(tmp.length());i++)
		{
			if (tmp[i]=='_')
				underscoreFound=true;
		}
		if (!underscoreFound)
		{
			theNames.push_back(tmp);
			theDirAndNames.push_back(fileInfo.absoluteFilePath().toLocal8Bit().data());
		}
	}

	// Load the system plugins first:
	for (int i=0;i<int(theNames.size());i++)
	{
		if ((theNames[i].compare("MeshCalc")==0)||(theNames[i].compare("Dynamics")==0)||(theNames[i].compare("PathPlanning")==0))
		{
			int pluginHandle=loadPlugin(theNames[i].c_str(),theDirAndNames[i].c_str());
			if (pluginHandle>=0)
				pluginHandles.push_back(pluginHandle);
			theDirAndNames[i]=""; // mark as 'already loaded'
		}
	}
	simLoadModule("",""); // indicate that we are done with the system plugins

	// Now load the other plugins too:
	for (int i=0;i<int(theNames.size());i++)
	{
		if (theDirAndNames[i].compare("")!=0)
		{ // not yet loaded
			int pluginHandle=loadPlugin(theNames[i].c_str(),theDirAndNames[i].c_str());
			if (pluginHandle>=0)
				pluginHandles.push_back(pluginHandle);
		}
	}

	if (sceneOrModelOrUiToLoad.length()!=0)
	{ // Here we double-clicked a V-REP file or dragged-and-dropped it onto this application
		int l=int(sceneOrModelOrUiToLoad.length());
		if ((l>4)&&(sceneOrModelOrUiToLoad[l-4]=='.')&&(sceneOrModelOrUiToLoad[l-3]=='t')&&(sceneOrModelOrUiToLoad[l-2]=='t'))
		{
			simSetBooleanParameter(sim_boolparam_scene_and_model_load_messages,1);
			if (sceneOrModelOrUiToLoad[l-1]=='t') // trying to load a scene?
			{
				if (simLoadScene(sceneOrModelOrUiToLoad.c_str())==-1)
					simAddStatusbarMessage("Scene could not be opened.");
			}
			if (sceneOrModelOrUiToLoad[l-1]=='m') // trying to load a model?
			{
				if (simLoadModel(sceneOrModelOrUiToLoad.c_str())==-1)
					simAddStatusbarMessage("Model could not be loaded.");
			}
			if (sceneOrModelOrUiToLoad[l-1]=='b') // trying to load a UI?
			{
				if (simLoadUI(sceneOrModelOrUiToLoad.c_str(),0,NULL)==-1)
					simAddStatusbarMessage("UI could not be loaded.");
			}
			simSetBooleanParameter(sim_boolparam_scene_and_model_load_messages,0);
		}
	}
}

void simulatorLoop()
{	// The main application loop (excluding the GUI part)
	static bool wasRunning=false;
	int auxValues[4];
	int messageID=0;
	int dataSize;
	if (autoStart)
	{
		simStartSimulation();
		autoStart=false;
	}
	while (messageID!=-1)
	{
		simChar* data=simGetSimulatorMessage(&messageID,auxValues,&dataSize);
		if (messageID!=-1)
		{
			if (messageID==sim_message_simulation_start_resume_request)
				simStartSimulation();
			if (messageID==sim_message_simulation_pause_request)
				simPauseSimulation();
			if (messageID==sim_message_simulation_stop_request)
				simStopSimulation();
			if (data!=NULL)
				simReleaseBuffer(data);
		}
	}

	// Handle a running simulation:
	if ( (simGetSimulationState()&sim_simulation_advancing)!=0 )
	{
		wasRunning=true;
		if ( (simGetRealTimeSimulation()!=1)||(simIsRealTimeSimulationStepNeeded()==1) )
		{
			if ((simHandleMainScript()&sim_script_main_script_not_called)==0)
				simAdvanceSimulationByOneStep();
			if ((simStopDelay>0)&&(simGetSimulationTime()>=float(simStopDelay)/1000.0f))
			{
				simStopDelay=0;
				simStopSimulation();
			}
		}
	}
	else
	{
		if (wasRunning&&autoQuit)
		{
			wasRunning=false;
			simQuitSimulator(true); // will post the quit command
		}
	}
}

void simulatorDeinit()
{
	// Unload all plugins:
	for (int i=0;i<int(pluginHandles.size());i++)
		simUnloadModule(pluginHandles[i]);
	pluginHandles.clear();
	std::cout << "Simulator ended.\n";
}

int main(int argc, char* argv[])
{
    std::string libLoc(argv[0]);
    while ((libLoc.length()>0)&&(libLoc[libLoc.length()-1]))
    {
        int l=libLoc.length();
        if (libLoc[l-1]!='/')
            libLoc.erase(libLoc.end()-1);
        else
        { // we might have a 2 byte char:
            if (l>1)
            {
                if (libLoc[l-2]>0x7F)
                    libLoc.erase(libLoc.end()-1);
                else
                    break;

            }
            else
                break;
        }
    }
    chdir(libLoc.c_str());

#ifdef WIN_VREP
	LIBRARY lib=loadVrepLibrary("v_rep");
#endif
#ifdef MAC_VREP
	LIBRARY lib=loadVrepLibrary("libv_rep.dylib");
#endif
#ifdef LINUX_VREP
	LIBRARY lib=loadVrepLibrary("libv_rep.so");
#endif

	bool wasRunning=false;
	if (lib!=NULL)
	{
		if (getVrepProcAddresses(lib)!=0)
		{
			int guiItems=sim_gui_all;
			for (int i=1;i<argc;i++)
			{
				std::string arg(argv[i]);
				if (arg[0]=='-')
				{
					if (arg.length()>=2)
					{
						if (arg[1]=='h')
							guiItems=sim_gui_headless;
						if (arg[1]=='s')
						{
							autoStart=true;
							simStopDelay=0;
							unsigned int p=2;
							while (arg.length()>p)
							{
								simStopDelay*=10;
								if ((arg[p]>='0')&&(arg[p]<='9'))
									simStopDelay+=arg[p]-'0';
								else
								{
									simStopDelay=0;
									break;
								}
								p++;
							}
						}
						if (arg[1]=='q')
							autoQuit=true;
						if ((arg[1]=='a')&&(arg.length()>2))
						{
							std::string tmp;
							tmp.assign(arg.begin()+2,arg.end());
							simSetStringParameter(sim_stringparam_additional_addonscript_firstscene,tmp.c_str()); // normally, never call API functions before simRunSimulator!!
						}
						if ((arg[1]=='b')&&(arg.length()>2))
						{
							std::string tmp;
							tmp.assign(arg.begin()+2,arg.end());
							simSetStringParameter(sim_stringparam_additional_addonscript,tmp.c_str()); // normally, never call API functions before simRunSimulator!!
						}
						if ((arg[1]=='g')&&(arg.length()>2))
						{
							static int cnt=0;
							std::string tmp;
							tmp.assign(arg.begin()+2,arg.end());
							if (cnt<9)
								simSetStringParameter(sim_stringparam_app_arg1+cnt,tmp.c_str()); // normally, never call API functions before simRunSimulator!!
							cnt++;
						}
					}
				}
				else
				{
					if (sceneOrModelOrUiToLoad.length()==0)
						sceneOrModelOrUiToLoad=arg;
				}
			}

			if (simRunSimulator("V-REP",guiItems,simulatorInit,simulatorLoop,simulatorDeinit)!=1)
				std::cout << "Failed initializing and running V-REP\n";
			else
				wasRunning=true;
		}
		else
			std::cout << "Error: could not find all required functions in the V-REP library\n";
		unloadVrepLibrary(lib);
	}
	else
		printf("Error: could not find or correctly load the V-REP library\n");
	if (!wasRunning)
		getchar();
	return(0);
}
