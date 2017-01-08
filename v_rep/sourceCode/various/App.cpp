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
#include "App.h"
#include "VThread.h"
#include "IloIlo.h"
#include "ttUtil.h"
#include "pluginContainer.h"
#include "v_repStrings.h"
#include "VDateTime.h"
#include <QThread>
#include "collDistInterface.h"
#include "dynInterface.h"
#include "pathPlanningInterface.h"
#include "VVarious.h"
#include "Tt.h"
#include "auxLibVideo.h"
#include "OGL.h"
#include "oglExt.h"
#include "embeddedFonts.h"
#include "persistentDataContainer.h"
#include "simAndUiThreadSync.h"

void (*_workThreadLoopCallback)();

VTHREAD_ID_TYPE App::_guiThread=0;
CUiThread* App::uiThread=NULL;
CSimThread* App::simThread=NULL;

CVrepQApp* App::qtApp=NULL;
int App::_qApp_argc=1;
char* App::_qApp_argv[1]={" "};
CUserSettings* App::userSettings=NULL;
CDirectoryPaths* App::directories=NULL;
int App::operationalUIParts=0; // sim_gui_menubar,sim_gui_popupmenus,sim_gui_toolbar1,sim_gui_toolbar2, etc.
std::string App::_applicationName="V-REP (Customized)";
CMainWindow* App::mainWindow=NULL;
CMainContainer* App::ct=NULL;
bool App::_exitRequest=false;
bool App::_simulatorIsRunning=false;
std::vector<std::string> App::_applicationArguments;
volatile int App::_quitLevel=0;
volatile int App::_uiMessagesToProcess=2; // 0=none, 1=all except user input, 2=all
volatile bool App::_uiMessagesToProcessApplied=true;

int App::sc=1;


SIMPLE_VTHREAD_RETURN_TYPE _workThread(SIMPLE_VTHREAD_ARGUMENT_TYPE lpData)
{
	FUNCTION_DEBUG;
	VThread::setMainThreadId();
	srand(VDateTime::getTimeInMs());	// Important so that the computer ID has some "true" random component!
										// Remember that each thread starts with a same seed!!!
	App::simThread=new CSimThread();
	CSimAndUiThreadSync::simThread_forbidUiThreadToWrite(true); // lock initially...
	while (!App::getExitRequest())
	{
		// Send the "instancePass" message to all plugins:
		int auxData[4]={App::ct->getModificationFlags(true),0,0,0};
		void* replyBuffer=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_instancepass,auxData,NULL,NULL);
		if (replyBuffer!=NULL)
			simReleaseBuffer_internal((simChar*)replyBuffer); // this should not happen! (or the replying module is not properly doing its job!)

		// Handle the main loop (one pass):
		if (_workThreadLoopCallback!=NULL)
			_workThreadLoopCallback();

		// Handle add-on execution:
		if (!App::ct->simulation->isSimulationRunning()) // when simulation is running, we handle the add-on scripts after the main script was called
			App::ct->luaScriptContainer->handleAddOnScriptExecution();

		App::simThread->executeMessages(); // rendering happens in there too!
	}
	App::setQuitLevel(1);
	App::qtApp->quit();
	while (App::getQuitLevel()==1)
		VThread::sleep(1);

	// Ok, the UI thread has left its exec and is waiting for us
// NOT HERE!!!	App::ct->removeAllObjectsFromAllScenes(); // important to call before we unload the plugins: some plugins are called when objects are destroyed
	delete App::simThread;

	CSimAndUiThreadSync::simThread_allowUiThreadToWrite(); // ...finally unlock


	App::setQuitLevel(3); // tell the UI thread that we are done here

	return(0);
}

App::App(bool headless)
{
	FUNCTION_DEBUG;

	_initSuccessful=false;
	uiThread=NULL;
	directories=NULL;
	userSettings=NULL;

	for (int i=0;i<9;i++)
		_applicationArguments.push_back("");

	qtApp=new CVrepQApp(_qApp_argc,_qApp_argv);

	qRegisterMetaType<std::string>("std::string");

	Q_INIT_RESOURCE(targaFiles);
	Q_INIT_RESOURCE(toolbarFiles);
	Q_INIT_RESOURCE(imageFiles_vrep);
	Q_INIT_RESOURCE(variousImageFiles);


#ifdef WIN_VREP
	CVrepQApp::setStyle("plastique"); //best
#endif

// Load the auxiliary Windows functions:
#ifdef WIN_VREP
	std::string libPathAndName(VVarious::splitPath_path(VVarious::getModulePathAndFile()));
	libPathAndName+=VREP_SLASH;
	libPathAndName+="v_repWin.dll";
	if (CAuxLib::loadLibrary(libPathAndName.c_str()))
	{
		if (!CAuxLib::getVrepAuxLibProcAddresses())
		{ // Error
			CAuxLib::unloadLibrary();
			if (!headless)
				VMessageBox::critical_(NULL,"V-REP auxiliary library","Could not find all required functions in the v_repWin library. V-REP might now crash!",VMessageBox::OKELI);
			printf("Could not find all required functions in the v_repWin library. V-REP might now crash!\n");
			return;
		}
	}
	else
	{ // Error
		if (!headless)
			VMessageBox::critical_(NULL,"V-REP auxiliary library","Could not find or correctly load v_repWin library. V-REP might now crash!",VMessageBox::OKELI);
		printf("Could not find or correctly load v_repWin library. V-REP might now crash!\n");
		return;
	}
#endif

//******************************* VIDEO LIBRARY *********************************
	std::string vidLibPathAndName(VVarious::splitPath_path(VVarious::getModulePathAndFile()));
	vidLibPathAndName+=VREP_SLASH;
#ifdef WIN_VREP
	vidLibPathAndName+="vvcl.dll";
#endif
#ifdef MAC_VREP
	vidLibPathAndName+="libvvcl.dylib";
#endif
#ifdef LIN_VREP
	vidLibPathAndName+="libvvcl.so";
#endif
	if (CAuxLibVideo::loadLibrary(vidLibPathAndName.c_str()))
	{
		if (!CAuxLibVideo::getAuxLibProcAddresses())
		{ // Error
			CAuxLibVideo::unloadLibrary();
			if (!headless)
				VMessageBox::critical_(NULL,"Video compression library","Could not find all required functions in the video compression library.",VMessageBox::OKELI);
			printf("Could not find all required functions in the video compression library.\n");
		}
	}
	else
	{
		if (!headless)
			VMessageBox::critical_(NULL,"Video compression library","Could not find or correctly load the video compression library.",VMessageBox::OKELI);
		printf("Could not find or correctly load the video compression library.\n");
	}
//**************************************************************************************

	directories=new CDirectoryPaths();
	userSettings=new CUserSettings();
	userSettings->applyRequestedDefaultPaths();

#ifdef WIN_VREP
	int ps=userSettings->guiFontSize_Win;
#endif
#ifdef MAC_VREP
	int ps=userSettings->guiFontSize_Mac;
#endif
#ifdef LIN_VREP
	int ps=userSettings->guiFontSize_Linux;
#endif
	const char tx1[]=" { font: ";
	const char tx2[]="px} ";
	std::string ss="QToolButton";
	ss+=tx1+tt::FNb(0,ps)+tx2;
	ss+="QLabel";
	ss+=tx1+tt::FNb(0,ps)+tx2;
	ss+="QLineEdit";
	ss+=tx1+tt::FNb(0,ps)+tx2;
	ss+="QCheckBox";
	ss+=tx1+tt::FNb(0,ps)+tx2;
	ss+="QGroupBox";
	ss+=tx1+tt::FNb(0,ps)+tx2;
	ss+="QComboBox";
	ss+=tx1+tt::FNb(0,ps)+tx2;
	ss+="QListWidget";
	ss+=tx1+tt::FNb(0,ps)+tx2;
	ss+="QRadioButton";
	ss+=tx1+tt::FNb(0,ps)+tx2;
	ss+="QPlainTextEdit";
	ss+=tx1+tt::FNb(0,ps)+tx2;
	qtApp->setStyleSheet(ss.c_str());

	uiThread=new CUiThread();
	_guiThread=VThread::getCurrentThreadId();
	VThread::setUiThreadId();
	srand(VDateTime::getTimeInMs());	// Important so that the computer ID has some "true" random component!
										// Remember that each thread starts with a same seed!!!

	_initSuccessful=true;
}

App::~App()
{
	FUNCTION_DEBUG;
	delete uiThread;

	// Clear the TAG that V-REP crashed! (because if we arrived here, we didn't crash!)
	CPersistentDataContainer cont(FILENAME_OF_USER_SETTINGS_IN_BINARY_FILE);
	cont.writeData("SIMSETTINGS_VREP_CRASHED","No",!App::userSettings->doNotWritePersistentData);

	// Remove any remaining auto-saved file:
	for (int i=1;i<30;i++)
	{
		std::string testScene=App::directories->executableDirectory+VREP_SLASH;
		testScene.append("AUTO_SAVED_INSTANCE_");
		testScene+=tt::FNb(i);
		testScene+=".ttt";
		if (VFile::doesFileExist(testScene))
			VFile::eraseFile(testScene);
	}

	delete directories;
	directories=NULL;
	delete userSettings;
	userSettings=NULL;
	CAuxLibVideo::unloadLibrary();
#ifdef WIN_VREP
	CAuxLib::unloadLibrary();
#endif

	if (qtApp!=NULL)
	{
		Q_CLEANUP_RESOURCE(variousImageFiles);
		Q_CLEANUP_RESOURCE(imageFiles_vrep);
		Q_CLEANUP_RESOURCE(toolbarFiles);
		Q_CLEANUP_RESOURCE(targaFiles);
		delete qtApp;
		qtApp=NULL;
	}
	_applicationArguments.clear();
}

bool App::wasInitSuccessful()
{
	return(_initSuccessful);
}

void App::setExitRequest(bool r)
{
	_exitRequest=r;
}

bool App::getExitRequest()
{
	return(_exitRequest);
}

bool App::isSimulatorRunning()
{
	return(_simulatorIsRunning);
}

int App::getCoreCount()
{
	int retVal=QThread::idealThreadCount();
	if (retVal<1)
		retVal=1;
	return(retVal);
}

void App::showSplashScreen()
{
	QPixmap pixmap;
	// At this stage the plugins are not yet loaded. So we verify if previous run used one of the major plugins that have a different license:
	CPersistentDataContainer temp; // very unelegant, change this later!
	std::string dummyVal;
	if (temp.readData("usesMajorPlugin__",dummyVal))
		pixmap.load(":/imageFiles_vrep/splashEdu.png"); // you may change this splash screen only if you use the commercial version of the plugins!
	else
		pixmap.load(":/imageFiles_vrep/splashBasic.png");

	QSplashScreen splash(pixmap,Qt::WindowStaysOnTopHint);
	splash.setMask(pixmap.mask());
	splash.show();
	int ct=VDateTime::getTimeInMs();
	while (VDateTime::getTimeDiffInMs(ct)<2000)
	{
		splash.raise();
		App::qtApp->processEvents();
		VThread::sleep(1);
	}
	splash.hide();
}

void App::setIcon()
{
#ifdef WIN_VREP
	App::qtApp->setWindowIcon(QIcon(":/variousImageFiles/v_repIcon16.png"));
#else



	App::qtApp->setWindowIcon(QIcon(":/imageFiles_vrep/v_repIcon128.png"));
#endif
}

void App::setApplicationName(const char* name)
{
	_applicationName=name;
}

std::string App::getApplicationName()
{
	return(_applicationName);
}

void App::createMainContainer()
{
	FUNCTION_DEBUG;
	ct=new CMainContainer();
	ct->initialize(true);
}

void App::deleteMainContainer()
{
	FUNCTION_DEBUG;
	ct->deinitialize();
	delete ct;
	ct=NULL;
}

void App::createMainWindow()
{
	FUNCTION_DEBUG;
	mainWindow=new CMainWindow();
	mainWindow->createDefaultMenuBar(false);
	mainWindow->createDefaultToolBars();
	mainWindow->setInitialDimensions(true);
	setShowConsole(userSettings->alwaysShowConsole);
}

void App::deleteMainWindow()
{
	FUNCTION_DEBUG;
	delete mainWindow;
	mainWindow=NULL;
}

void App::setShowConsole(bool s)
{
#ifdef WIN_VREP
	CAuxLib::vrepWin_showConsole(char(s));
#endif
}


void App::_runInitializationCallback(void(*initCallBack)())
{
	FUNCTION_DEBUG;
	if (initCallBack!=NULL)
		initCallBack(); // this should load all plugins

	if (CCollDistInterface::initializeFunctionsIfNeeded())
		printf("Using the 'MeshCalc' plugin.\n");
	else
		printf("The 'MeshCalc' plugin could not be initialized.\n");

	if (CDynInterface::initializeFunctionsIfNeeded())
		printf("Using the 'Dynamics' plugin.\n");
	else
		printf("The 'Dynamics' plugin could not be initialized.\n");

	if (CPathPlanningInterface::initializeFunctionsIfNeeded())
		printf("Using the 'PathPlanning' plugin.\n");
	else
		printf("The 'PathPlanning' plugin could not be initialized.\n");

	CSimpleFilter::readAllExternalFilters();

	// Following is so that we can display the correct splash screen at start-up (at that time we haven't yet loaded the plugins!):
	CPersistentDataContainer a;
	if (CCollDistInterface::initializeFunctionsIfNeeded()||CDynInterface::initializeFunctionsIfNeeded()||CPathPlanningInterface::initializeFunctionsIfNeeded())
		a.writeData("usesMajorPlugin__","yes",!App::userSettings->doNotWritePersistentData);
	else
		a.writeData("usesMajorPlugin__","",!App::userSettings->doNotWritePersistentData);
}

void App::_runDeinitializationCallback(void(*deinitCallBack)())
{
	FUNCTION_DEBUG;
	if (deinitCallBack!=NULL)
		deinitCallBack(); // this will unload all plugins!!
}

void App::run(void(*initCallBack)(),void(*loopCallBack)(),void(*deinitCallBack)())
{
	FUNCTION_DEBUG;
	setExitRequest(false);
	if (mainWindow!=NULL)
		mainWindow->setFocus(Qt::MouseFocusReason); // needed because at first Qt behaves strangely (really??)
	uiThread->addStatusLine(IDSNS_SIMULATOR_LAUNCHED);
	_simulatorIsRunning=true;

	_runInitializationCallback(initCallBack); // load the plugins via callback mechanism

	_workThreadLoopCallback=loopCallBack;
	VThread::launchSimpleThread(_workThread,VThread::VTHREAD_NORMAL_PRIORITY);

	_processGuiEventsUntilQuit(); // the UI thread sits here during the whole application!

	// App::qtApp->quit() was called from the SIM thread.
	_quitLevel=2; // indicate to the SIM thread that the UI thread has left its exec

	// Wait until the SIM thread has finished its clean-up:
	while (_quitLevel==2)
		VThread::sleep(1);

	App::ct->removeAllObjectsFromAllScenes(); // important to call before we unload the plugins: some plugins are called when objects are destroyed

	// Ok, we unload the plugins and leave
	_runDeinitializationCallback(deinitCallBack); // unload the plugins via callback mechanism

	_simulatorIsRunning=false;
}

void App::_processGuiEventsUntilQuit()
{
	// 1. Best if possible to use:
	qtApp->exec();

	// 2. Second best if possible to use:
	// QEventLoop eventLoop;
	// eventLoop->exec();

	// 3. Not ideal, but most flexibility:
	/*
	while (!App::getExitRequest())
	{
		if (!qtApp->hasPendingEvents())
		{
			_uiMessagesToProcessApplied=true;
			qtApp->processEvents(QEventLoop::WaitForMoreEvents);
		}
		else
		{
			if (_uiMessagesToProcess==0)
				VThread::sleep(1);
			if (_uiMessagesToProcess==1)
				qtApp->processEvents(QEventLoop::WaitForMoreEvents|QEventLoop::ExcludeUserInputEvents|QEventLoop::ExcludeSocketNotifiers);
			if (_uiMessagesToProcess==2)
				qtApp->processEvents();
		}
	}
	while (_quitLevel==0)
		VThread::sleep(1);
		*/
}

void App::setUiMessagesToProcess(int mt)
{
	_uiMessagesToProcess=mt;
	_uiMessagesToProcessApplied=false;
}

bool App::getUiMessagesToProcessAreApplied()
{
	return(_uiMessagesToProcessApplied);
}

void App::addStatusLine_(const char* line)
{ // do not call directly!
	if ((operationalUIParts&sim_gui_statusbar)&&(mainWindow!=NULL))
	{
		if (mainWindow->statusBar!=NULL)
		{
			mainWindow->statusBar->appendPlainText(line);
			mainWindow->statusBar->moveCursor(QTextCursor::End);
			mainWindow->statusBar->moveCursor(QTextCursor::PreviousBlock);
			mainWindow->statusBar->ensureCursorVisible();
		}
	}
}

bool App::isFullScreen()
{
	if (mainWindow==NULL)
		return(false);
	return(mainWindow->isFullScreen());
}

void App::setQuitLevel(int l)
{
	_quitLevel=l;
}

int App::getQuitLevel()
{
	return(_quitLevel);
}

std::string App::getApplicationArgument(int index)
{
	if (_applicationArguments.size()==0)
	{
		for (int i=0;i<9;i++)
			_applicationArguments.push_back("");
	}
	if (index<9)
		return(_applicationArguments[index]);
	return("");
}

void App::setApplicationArgument(int index,std::string arg)
{
	if (_applicationArguments.size()==0)
	{
		for (int i=0;i<9;i++)
			_applicationArguments.push_back("");
	}
	if (index<9)
		_applicationArguments[index]=arg;
}

void App::initGl_ifNeeded()
{ // when calling this we need to have a valid openGl context!!
	static bool done=false;
	if (done)
		return;
	FUNCTION_DEBUG;
	done=true;

	oglExt::prepareExtensionFunctions(userSettings->forceFboViaExt);

	// Turn off vertical screen synchronization. Very important, otherwise the simulation gets drastically slowed down!
	// Check these threads:
	// http://www.opengl.org/discussion_boards/ubbthreads.php?ubb=showflat&Number=271567
	oglExt::turnOffVSync(userSettings->vsync);

	//CIloIlo::nonPowerOfTwoTexturesAvailable=oglExt::areNonPowerOfTwoTexturesAvailable();

	oglExt::initDefaultGlValues();

	glGetIntegerv(GL_MAX_LIGHTS,&CIloIlo::maximumOpenGlLights);


	ogl::loadBitmapFont(VREPFONT_MS_SANS_SERIF,16,0);
	ogl::loadBitmapFont(VREPFONT_COURIER_NEW,16,1);

	// In future, use following fonts for Retina displays:
//		ogl::loadBitmapFont(VREPFONT_MS_SANS_SERIF_30,32,0);
//		ogl::loadBitmapFont(VREPFONT_LUCIDA_CONSOLE_26,32,1);
	ogl::loadOutlineFont(VREPOUTLINEFONT_ARIAL_INT,VREPOUTLINEFONT_ARIAL_FLOAT);

	std::string glVer("OpenGL: ");
	glVer+=(char*)glGetString(GL_VENDOR);
	FUNCTION_INSIDE_DEBUG(glVer.c_str());
	glVer="OpenGL: ";
	glVer+=(char*)glGetString(GL_RENDERER);
	FUNCTION_INSIDE_DEBUG(glVer.c_str());
	glVer="OpenGL: ";
	glVer+=(char*)glGetString(GL_VERSION);
	FUNCTION_INSIDE_DEBUG(glVer.c_str());

}
