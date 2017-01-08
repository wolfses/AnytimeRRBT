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

#include "uiThread.h"
#include "simThread.h"
#include "vrepQApp.h"
#include "DirectoryPaths.h"
#include "UserSettings.h"
#include "VMutex.h"
#include "MainWindow.h"
#include "mainContainer.h"
#include "simAndUiThreadSync.h"

class App
{
public:
	App(bool headless);
	virtual ~App();
	bool wasInitSuccessful();
	static void showSplashScreen();
	static void setIcon();
	static void setApplicationName(const char* name);
	static std::string getApplicationName();
	static void createMainContainer();
	static void deleteMainContainer();
	static void createMainWindow();
	static void deleteMainWindow();

	static void run(void(*initCallBack)(),void(*loopCallBack)(),void(*deinitCallBack)());
	static void setExitRequest(bool r);
	static bool getExitRequest();
	static bool isSimulatorRunning();
	static int getCoreCount();

	static bool isFullScreen();
	static void addStatusLine_(const char* line);

	static void setShowConsole(bool s);
	static void initGl_ifNeeded();

	static void setQuitLevel(int l);
	static int getQuitLevel();

	static void setUiMessagesToProcess(int mt);
	static bool getUiMessagesToProcessAreApplied();

	static std::string getApplicationArgument(int index);
	static void setApplicationArgument(int index,std::string arg);

	static CUiThread* uiThread;
	static CSimThread* simThread;
	static CVrepQApp* qtApp;
	static CMainWindow* mainWindow;
	static CMainContainer* ct;

	static CDirectoryPaths* directories;
	static CUserSettings* userSettings;

	static int operationalUIParts;
	static int sc;

private:
	static void _runInitializationCallback(void(*initCallBack)());
	static void _runDeinitializationCallback(void(*deinitCallBack)());

	static void _processGuiEventsUntilQuit();

	bool _initSuccessful;
	static int _qApp_argc;
	static char* _qApp_argv[1];
	static VTHREAD_ID_TYPE _guiThread;
	static bool _exitRequest;
	static bool _simulatorIsRunning;
	static std::string _applicationName;
	static std::vector<std::string> _applicationArguments;
	static volatile int _quitLevel;

	static volatile int _uiMessagesToProcess; // 0=none, 1=all except user input, 2=all
	static volatile bool _uiMessagesToProcessApplied;
};
