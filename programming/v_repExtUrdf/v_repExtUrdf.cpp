// This file is part of the URDF PLUGIN for V-REP
//  
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// A big thanks to Ignacio Tartavull for his precious help!
// 
// The URDF PLUGIN is licensed under the terms of GNU GPL:
// 
// -------------------------------------------------------------------
// The URDF PLUGIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// THE URDF PLUGIN IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with the URDF PLUGIN.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------

//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

// The URDF plugin is courtesy of Ignacio Tartavull. A few modifications by Marc.

#include "v_repExtUrdf.h"
#include "v_repLib.h"
#include <iostream>
#include <vector>
#include <bitset>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <math.h>
#include "urdfdialog.h"

#ifdef _WIN32
	#include <direct.h>
#endif /* _WIN32 */

#if defined (__linux) || defined (__APPLE__)
	#include <string.h>
	#define _stricmp(x,y) strcasecmp(x,y)
#endif

#define PLUGIN_VERSION 5	// 1 until 20/1/2013 (1 was a very early beta)
							// 2 until 10/1/2014 (V-REP3.0.5)
							// 3: new lock
							// 4: since V-REP 3.1.2
							// 5: since after V-REP 3.1.3

LIBRARY vrepLib;
CUrdfDialog* urdfDialog=NULL;

// This is the plugin start routine (called just once, just after the plugin was loaded):
VREP_DLLEXPORT unsigned char v_repStart(void* reservedPointer,int reservedInt)
{
	// Dynamically load and bind V-REP functions:
	 // ******************************************
	 // 1. Figure out this plugin's directory:
	 char curDirAndFile[1024];
 #ifdef _WIN32
	 _getcwd(curDirAndFile, sizeof(curDirAndFile));
 #elif defined (__linux) || defined (__APPLE__)
	 getcwd(curDirAndFile, sizeof(curDirAndFile));
 #endif
	 std::string currentDirAndPath(curDirAndFile);
	 // 2. Append the V-REP library's name:
	 std::string temp(currentDirAndPath);
 #ifdef _WIN32
	 temp+="/v_rep.dll";
 #elif defined (__linux)
	 temp+="/libv_rep.so";
 #elif defined (__APPLE__)
	 temp+="/libv_rep.dylib";
 #endif /* __linux || __APPLE__ */
	// 3. Load the V-REP library:
	vrepLib=loadVrepLibrary(temp.c_str());
	if (vrepLib==NULL)
	{
		std::cout << "Error, could not find or correctly load the V-REP library. Cannot start 'Urdf' plugin.\n";
		return(0); // Means error, V-REP will unload this plugin
	}
	if (getVrepProcAddresses(vrepLib)==0)
	{
		std::cout << "Error, could not find all required functions in the V-REP library. Cannot start 'Urdf' plugin.\n";
		unloadVrepLibrary(vrepLib);
		return(0); // Means error, V-REP will unload this plugin
	}
	// ******************************************

	// Check the version of V-REP:
	// ******************************************
	int vrepVer;
	simGetIntegerParameter(sim_intparam_program_version,&vrepVer);
	if (vrepVer<30001) // if V-REP version is smaller than 3.00.01
	{
		std::cout << "Sorry, your V-REP copy is somewhat old. Cannot start 'Urdf' plugin.\n";
		unloadVrepLibrary(vrepLib);
		return(0); // Means error, V-REP will unload this plugin
	}
	// ******************************************


	QWidget* pMainWindow = (QWidget*)simGetMainWindow(1);
	urdfDialog=new CUrdfDialog(pMainWindow); // The plugin dialog
	simAddModuleMenuEntry("",1,&urdfDialog->dialogMenuItemHandle);
	simSetModuleMenuItemState(urdfDialog->dialogMenuItemHandle,1,"URDF import...");


	return(PLUGIN_VERSION); // initialization went fine, we return the version number of this plugin (can be queried with simGetModuleName)
}

// This is the plugin end routine (called just once, when V-REP is ending, i.e. releasing this plugin):
VREP_DLLEXPORT void v_repEnd()
{
	// Here you could handle various clean-up tasks
	delete urdfDialog;
	unloadVrepLibrary(vrepLib); // release the library
}

// This is the plugin messaging routine (i.e. V-REP calls this function very often, with various messages):
VREP_DLLEXPORT void* v_repMessage(int message,int* auxiliaryData,void* customData,int* replyData)
{ // This is called quite often. Just watch out for messages/events you want to handle
	// Keep following 6 lines at the beginning and unchanged:
	static bool refreshDlgFlag=true;
	int errorModeSaved;
	simGetIntegerParameter(sim_intparam_error_report_mode,&errorModeSaved);
	simSetIntegerParameter(sim_intparam_error_report_mode,sim_api_errormessage_ignore);
	void* retVal=NULL;

	// Here we can intercept many messages from V-REP (actually callbacks).
	// For a complete list of messages that you can intercept/react with, search for "sim_message_eventcallback"-type constants
	// in the V-REP user manual.

	if (message==sim_message_eventcallback_refreshdialogs)
		refreshDlgFlag=true; // V-REP dialogs were refreshed. Maybe a good idea to refresh this plugin's dialog too

	if (message==sim_message_eventcallback_menuitemselected)
	{ // A custom menu bar entry was selected..
		if (auxiliaryData[0]==urdfDialog->dialogMenuItemHandle)
			urdfDialog->makeVisible(!urdfDialog->getVisible()); // Toggle visibility of the dialog
	}

	if (message==sim_message_eventcallback_instancepass)
	{ // It is important to always correctly react to events in V-REP. This message is the most convenient way to do so:
		urdfDialog->handleCommands();
		urdfDialog->setSimulationStopped(simGetSimulationState()==sim_simulation_stopped);

		int flags=auxiliaryData[0];
		bool sceneContentChanged=((flags&(1+2+4+8+16+32+64+256))!=0); // object erased, created, model or scene loaded, und/redo called, instance switched, or object scaled since last sim_message_eventcallback_instancepass message
		bool instanceSwitched=((flags&64)!=0);

		if (instanceSwitched)
		{
			// React to an instance switch here!!
		}

		if (sceneContentChanged)
		{
			refreshDlgFlag=true;
		}
	}
	if ((message==sim_message_eventcallback_guipass)&&refreshDlgFlag)
	{ // handle refresh of the plugin's dialog:
		urdfDialog->refresh(); // Refresh the dialog
		refreshDlgFlag=false;
	}

	// Keep following unchanged:
	simSetIntegerParameter(sim_intparam_error_report_mode,errorModeSaved); // restore previous settings
	return(retVal);
}

