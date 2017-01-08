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

// This file requires serious refactoring!!

#include "vrepPrecompiledHeader.h"
#include "v_rep_internal.h"
#include "MainWindow.h"
#include "IloIlo.h"
#include "OGL.h"
#include "Simulation.h"
#include "OglSurface.h"
#include "ObjectEdition.h"
#include "FileMenu.h"
#include "ToolBarCommand.h"
#include "TriangleVertexEdition.h"
#include "AddMenu.h"
#include "DlgCont.h"

#include "HelpMenu.h"
#include "PathEdition.h"
#include "GV.h"
#include "Tt.h"
#include "App.h"
#include "pluginContainer.h"
#include "auxLibVideo.h"
#include "VVarious.h"
#include "v_repStrings.h"
#include <boost/lexical_cast.hpp>
#include "VDateTime.h"
#include "VFileFinder.h"
#include "ttUtil.h"
#include "qdlgmessageandcheckbox.h"
#include <QMimeData>
#include <QDragEnterEvent>
#include <QUrl>
#include <QToolButton>
#include <QLabel>
#ifdef MAC_VREP
	#include <OpenGL.h> // in order to undertsand the CGLContextObj, etc.
	#include <curses.h> // required for the beep() function
#endif
#ifdef LIN_VREP
	#include <GL/glx.h>
#endif

bool CMainWindow::ctrlKeyDown=false;
bool CMainWindow::shiftKeyDown=false;
bool CMainWindow::upKeyDown=false;
bool CMainWindow::downKeyDown=false;
bool CMainWindow::leftKeyDown=false;
bool CMainWindow::rightKeyDown=false;
bool CMainWindow::leftMouseDown=false;
bool CMainWindow::middleMouseDown=false;
bool CMainWindow::rightMouseDown=false;

CMainWindow::CMainWindow() : QMainWindow()
{
	focusObject=FOCUS_ON_PAGE;
	clientArea.x=640;
	clientArea.y=480;
	clientPos.x=0;
	clientPos.y=0;
	_toolbar1=NULL;
	_toolbar2=NULL;
	_menubar=NULL;
	fileSystemMenu=NULL;
	editSystemMenu=NULL;
	addSystemMenu=NULL;
	simulationSystemMenu=NULL;
	windowSystemMenu=NULL;
	addOnSystemMenu=NULL;
	helpSystemMenu=NULL;
	secretSystemMenu=NULL;
	_fullscreen=false;
	_hasStereo=false;
	_stereoDistance=0.0f;
	_leftEye=true;

	customMenuBarItemContainer=new CCustomMenuBarItemContainer();
	_savedMouseMode=App::ct->getMouseMode();
	dlgCont=new CDlgCont(this);
	std::string p(App::directories->executableDirectory+VREP_SLASH);

#ifdef MAC_VREP
	// We are inside of the package!!!
	p+="../../../";
#endif

	p+="recording"; // extension is added by V-REP depending on the used container
	simulationRecorder=new CSimRecorder(p.c_str());
	secretText="";
	_mouseButtonsState=0;

	setWindowTitle(App::getApplicationName().c_str()); // somehow it is important for Linux. Even if this title gets later overwritten, Linux keeps this text to display the app name when minimized
	openglWidget=new COpenglWidget();

	if (App::userSettings->stereoDist>0.0f)
	{
		if (openglWidget->format().stereo())
		{
			printf("OpenGL: enabled stereo.\n");
			FUNCTION_INSIDE_DEBUG("OpenGL: enabled stereo.");
			setHasStereo(true);
			setStereoDistance(App::userSettings->stereoDist);
		}
		else
		{
			printf("OpenGL: could not enable stereo.\n");
			FUNCTION_INSIDE_DEBUG("OpenGL: could not enable stereo.");
		}
	}

	openglWidget->makeCurrent();
	App::initGl_ifNeeded();

	if (App::operationalUIParts&sim_gui_statusbar)
	{
		splitter=new QSplitter(Qt::Vertical);
		//	QSplitter* sp2=new QSplitter(Qt::Vertical);
		//	splitter->addWidget(sp2);
		//	sp2->addWidget(openglWidget);
		//	sp2->addWidget(new QPlainTextEdit());
		splitter->addWidget(openglWidget);

		statusBar=new QPlainTextEdit();
		statusBar->setReadOnly(true);
		statusBar->setLineWrapMode(QPlainTextEdit::NoWrap);
		statusBar->setMaximumBlockCount(500);
		statusBar->moveCursor(QTextCursor::End);
		statusBar->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		if (App::userSettings->statusbarInitiallyVisible)
		{
			statusBar->setMinimumHeight(36);
			statusBar->setMaximumHeight(36);
		}
		else
		{
			statusBar->setMinimumHeight(0);
			statusBar->setMaximumHeight(0);
		}

		splitter->addWidget(statusBar);
		splitter->setCollapsible(0,false);
		splitter->setCollapsible(1,true);
		//splitter->setOpaqueResize(false);
		connect(splitter,SIGNAL(splitterMoved(int,int)),this,SLOT(splitterMoved(int,int)));
		setCentralWidget(splitter);
	}
	else
	{
		splitter=NULL;
		statusBar=NULL;
		setCentralWidget(openglWidget);
	}

	resize(800,600);
	signalMapper=new QSignalMapper(this);
	connect(signalMapper,SIGNAL(mapped(int)),this,SLOT(_vrepMessageHandler(int)));
	popupSignalMapper=new QSignalMapper(this);
	connect(popupSignalMapper,SIGNAL(mapped(int)),this,SLOT(_vrepPopupMessageHandler(int)));
	setAcceptDrops(true);
}

CMainWindow::~CMainWindow()
{
	delete simulationRecorder;
	delete customMenuBarItemContainer;
	delete dlgCont;
	App::ct->scintillaConsoleContainer->closeAllConsoles(); // since they are parented with the main window, they automatically get destroyed (i.e. their Window thing!)
	App::ct->closeAllScintillaEditorsInAllScenes(); // since they are parented with the main window, they automatically get destroyed (i.e. their Window thing!)
	ogl::freeOutlineFont();
	ogl::freeBitmapFonts();
	delete _menubar;
	delete _toolbar1;
	delete _toolbar2;
	delete popupSignalMapper;
	delete signalMapper;
	// openglWidget is destroyed automatically by the main window
}

bool CMainWindow::isFullScreen()
{
	return(_fullscreen);
}

void CMainWindow::setFullScreen(bool f)
{
	if (_fullscreen!=f)
	{
		if (f)
		{
			openglWidget->setParent(NULL);
			openglWidget->showFullScreen();
		}
		else
		{
			openglWidget->showNormal();
			splitter->insertWidget(0,openglWidget);
		}
	}
	_fullscreen=f;
}

void CMainWindow::setHasStereo(bool s)
{
	_hasStereo=s;
}

bool CMainWindow::getHasStereo()
{
	return(_hasStereo);
}

void CMainWindow::setStereoDistance(float d)
{
	_stereoDistance=d;
}

float CMainWindow::getStereoDistance()
{
	return(_stereoDistance);
}

void CMainWindow::setLeftEye(bool l)
{
	_leftEye=l;
}

bool CMainWindow::getLeftEye()
{
	return(_leftEye);
}


int CMainWindow::getMouseButtonState()
{
	return(_mouseButtonsState);
}

void CMainWindow::setInitialDimensions(bool maximized)
{
	setWindowDimensions(640,480,true,maximized);
}

void CMainWindow::setWindowDimensions(int x,int y,bool clientSurface,bool maximized)
{
	if (!maximized)
		showNormal();
	int frameWidth=0;
	int frameHeight=0;
	if (!clientSurface)
	{
		frameWidth=frameGeometry().width()-size().width();
		frameHeight=frameGeometry().height()-size().height();
	}
	x-=frameWidth;
	y-=frameHeight;
	resize(x,y);
	if (maximized)
		showMaximized();
}

void CMainWindow::refreshDimensions()
{
	recomputeClientSizeAndPos();	
}

void CMainWindow::simThread_prepareToRenderScene()
{
	FUNCTION_DEBUG;
	// Following is for camera tracking (when simulation is stopped). If sim is running, it is handled in simHandleVarious!
	if (App::ct->simulation->isSimulationStopped())
	{
		for (int i=0;i<int(App::ct->objCont->cameraList.size());i++)
		{
			CCamera*  it=App::ct->objCont->getCamera(App::ct->objCont->cameraList[i]);
			it->handleTrackingAndHeadAlwaysUp();
		}
	}
	// Following is for dummy position assignment to path trajectory when not simulating (and not pausing):
	if (App::ct->simulation->isSimulationStopped())
	{
		for (int i=0;i<int(App::ct->objCont->pathList.size());i++)
		{
			CPath* it=App::ct->objCont->getPath(App::ct->objCont->pathList[i]);
			it->resetPath();
		}
	}

	for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
		it->bufferMainDisplayStateVariables();
	}
	/*
	if (App::ct->simulation->getThreadedRendering())
		App::uiThread->requestSceneRender(false);
	else
		App::uiThread->requestSceneRender_wait();
		*/
}



void CMainWindow::uiThread_renderScene(bool bufferMainDisplayStateVariables)
{
	FUNCTION_DEBUG;
	IF_UI_EVENT_CAN_READ_DATA
	{ // ok, we are allowed to render (i.e. the simulation thread doesn't modify anything serious)
		App::ct->infoDisplay->clearRenderingTime();
		App::ct->infoDisplay->renderingStart();
		if (bufferMainDisplayStateVariables)
		{
			for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
			{
				C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
				it->bufferMainDisplayStateVariables();
			}
		}
		renderOpenGlContent_callFromRenderingThreadOnly();
		App::ct->infoDisplay->renderingEnd();
	}
}


void CMainWindow::refreshDialogs_uiThread()
{
	IF_UI_EVENT_CAN_READ_DATA
	{
		void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_guipass,NULL,NULL,NULL);
		delete[] (char*)returnVal;

		App::ct->scintillaConsoleContainer->handleConsoles();

		App::ct->undoBufferContainer->memorizeStateIfNeeded();
		// We refresh dialogs and the toolbar here:
		//----------------------------------------------------------------------------------
		static int lastInstance=-1;
		if (App::ct->getCurrentInstanceIndex()!=lastInstance)
		{
			IF_UI_EVENT_CAN_WRITE_DATA
			{
				App::ct->objCont->setFullDialogRefreshFlag();
				App::ct->objCont->toolbarRefreshFlag=true;
				createDefaultMenuBar(false);
				lastInstance=App::ct->getCurrentInstanceIndex();
				refreshDimensions();
				// If the instance was switched, we close all material/color dialogs:
				dlgCont->visibleInstanceAboutToSwitch();
			}
		}

		dlgCont->destroyWhatNeedsDestruction();
		int d=App::ct->objCont->getDialogRefreshDegree();
		if (d!=-1)
		{
			if (d>0)
				App::ct->constraintSolver->removeMultipleDefinedObjects(); // CHANGE THIS!!! TODO
			if (!App::ct->objCont->dialogRefreshDontPublishFlag)
			{
				int data[4]={d,0,0,0};
				void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_refreshdialogs,data,NULL,NULL);
				delete[] (char*)returnVal;
			}
			App::ct->objCont->dialogRefreshDontPublishFlag=false;
			dlgCont->setRefreshDegree(d);
			dlgCont->refresh();
			if (d!=0)
				App::ct->hierarchy->setRebuildHierarchyFlag();
			else
				App::ct->hierarchy->setRefreshViewFlag();
		}
		App::ct->objCont->resetDialogRefreshFlags();
		if (App::ct->objCont->toolbarRefreshFlag)
			actualizeButtonState();
		App::ct->objCont->toolbarRefreshFlag=false;
		//----------------------------------------------------------------------------------

		int ct=VDateTime::getTimeInMs();
		static int timeCounter=ct;
		if ( (VDateTime::getTimeDiffInMs(timeCounter)>1000)||((VDateTime::getTimeDiffInMs(timeCounter)>100)&&(!openglWidget->getOpenGlDisplayEnabled())) )
		{ // Refresh the main window text every 1/4 seconds:
			timeCounter=ct;
			std::string title;
			if (openglWidget->getOpenGlDisplayEnabled())
			{
				title=(IDS____RENDERING__);
				title+=boost::lexical_cast<std::string>(_renderingTimeInMs)+" ms";
			}
			else
			{ // We display the simulation time instead:
				title=IDS____SIMULATION_TIME__;
				title+=gv::getHourMinuteSecondMilisecondStr(App::ct->simulation->getSimulationTime());
			}
			title+=" (";
			title+=tt::FNb(0,_fps,1,false);
			title+=" fps)";
			if (App::ct->objCont->getEditModeType()&TRIANGLE_EDIT_MODE)
				title+=IDS____TRIANGLE_EDIT_MODE;
			else if (App::ct->objCont->getEditModeType()&VERTEX_EDIT_MODE)
				title+=IDS____VERTEX_EDIT_MODE;
			else if (App::ct->objCont->getEditModeType()&EDGE_EDIT_MODE)
				title+=IDS____EDGE_EDIT_MODE;
			else if (App::ct->objCont->getEditModeType()&BUTTON_EDIT_MODE)
				title+=IDS____2D_ELEMENT_EDIT_MODE;
			else if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
				title+=IDS____PATH_EDIT_MODE;

			if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
			{
				if (App::ct->simulation->isSimulationRunning())
					title+=IDS____SIMULATION_RUNNING;
				else if (App::ct->simulation->isSimulationPaused())
					title+=IDS____SIMULATION_PAUSED;
				else
					title+=IDS____SIMULATION_STOPPED;
			}

			if (App::ct->mainSettings->scenePathAndName.compare("")==0)
				title=std::string(IDS_NEW_FILE)+title;
			else
				title=App::directories->getNameFromFull(App::ct->mainSettings->scenePathAndName)+title;
			title=App::getApplicationName()+" - "+title;
			if (App::mainWindow!=NULL)
				App::mainWindow->setWindowTitle(title.c_str());
		}

		if (VDateTime::getTimeDiffInMs(_mouseWheelEventTime)>300)
			_mouseButtonsState&=0xffff-2; // We clear the mouse wheel event

		App::ct->renderingPassAllNonToolDialogs();
	}
}

int CMainWindow::renderOpenGlContent_callFromRenderingThreadOnly()
{ // Called only from the rendering thread!!!
	FUNCTION_DEBUG;
	static int lastTimeRenderingStarted=VDateTime::getTimeInMs();
	int startTime=VDateTime::getTimeInMs();
	_fps=1.0f/(float(VDateTime::getTimeDiffInMs(lastTimeRenderingStarted,startTime))/1000.0f);
	lastTimeRenderingStarted=startTime;

	for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
		it->bufferedMainDisplayStateVariablesToDisplay();
	}

	int d=App::ct->objCont->getDialogRefreshDegree();
	if (d!=-1)
	{
		if (d!=0)
			App::ct->hierarchy->setRebuildHierarchyFlag();
		else
			App::ct->hierarchy->setRefreshViewFlag();
	}

	bool swapTheBuffers=false;
	if (!windowHandle()->isExposed())
		return(0);

	if ( (!openglWidget->getOpenGlDisplayEnabled())&&(App::ct->simulation!=NULL)&&(App::ct->simulation->isSimulationStopped()) )
		openglWidget->setOpenGlDisplayEnabled(true);

	static int previousDisplayWasEnabled=0;
	if (openglWidget->getOpenGlDisplayEnabled())
	{
		swapTheBuffers=true;
		int oglDebugTime=startTime;
		openglWidget->makeCurrent();
		if (App::userSettings->debugOpenGl)
		{
			int oglDebugTimeNow=VDateTime::getTimeInMs();
			printf("openGl debug --> doneCurrent + makeCurrent: %i\n",VDateTime::getTimeDiffInMs(oglDebugTime,oglDebugTimeNow));
			oglDebugTime=oglDebugTimeNow;
		}

		int mp[2]={mouseRenderingPos.x-clientPos.x,mouseRenderingPos.y-clientPos.y};

		CPluginContainer::sendSpecialEventCallbackMessageToSomePlugins(sim_message_eventcallback_renderingpass,NULL,NULL,NULL);

		if (!_hasStereo)
			App::ct->oglSurface->render(_currentCursor,_mouseButtonsState,mp);
		else
		{
			_leftEye=true;
			glDrawBuffer(GL_BACK_LEFT);
			App::ct->oglSurface->render(_currentCursor,_mouseButtonsState,mp);
			_leftEye=false;
			glDrawBuffer(GL_BACK_RIGHT);
			App::ct->oglSurface->render(_currentCursor,_mouseButtonsState,mp);
		}

		if (App::userSettings->debugOpenGl)
		{
			int oglDebugTimeNow=VDateTime::getTimeInMs();
			printf("openGl debug --> sendEventCallbackMessageToAllPlugins + render: %i\n",VDateTime::getTimeDiffInMs(oglDebugTime,oglDebugTimeNow));
			oglDebugTime=oglDebugTimeNow;
		}

		previousDisplayWasEnabled=0;
		if (App::userSettings->useGlFinish) // false by default!
			glFinish();	// Might be important later (synchronization problems)
					// removed on 2009/12/09 upon recomendation of gamedev community
					// re-put on 2010/01/11 because it slows down some graphic cards in a non-proportional way (e.g. 1 object=x ms, 5 objects=20x ms)
					// re-removed again (by default) on 31/01/2013. Thanks a lot to Cedric Pradalier for pointing problems appearing with the NVidia drivers

		if (App::userSettings->debugOpenGl)
		{
			int oglDebugTimeNow=VDateTime::getTimeInMs();
			printf("openGl debug --> glFinish (%i, %i): %i\n",App::userSettings->useGlFinish,App::userSettings->vsync,VDateTime::getTimeDiffInMs(oglDebugTime,oglDebugTimeNow));
			printf("openGl debug --> VENDOR:%s\n",glGetString(GL_VENDOR));
			printf("openGl debug --> RENDERER:%s\n",glGetString(GL_RENDERER));
			printf("openGl debug --> VERSION:%s\n",glGetString(GL_VERSION));
			oglDebugTime=oglDebugTimeNow;
		}
	}
	else
	{
		if (previousDisplayWasEnabled<2)
		{ // clear the screen
			// We draw a dark grey view:

			swapTheBuffers=true;
			openglWidget->makeCurrent();
			glDisable(GL_SCISSOR_TEST);
			glViewport(-2000,-2000,4000,4000);
			glClearColor(0.0f,0.0f,0.0f,1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			if (App::userSettings->useGlFinish) // false by default!
				glFinish();	// Might be important later (synchronization problems)
						// removed on 2009/12/09 upon recomendation of gamedev community
						// re-put on 2010/01/11 because it slows down some graphic cards in a non-proportional way (e.g. 1 object=x ms, 5 objects=20x ms)
						// re-removed again (by default) on 31/01/2013. Thanks a lot to Cedric Pradalier for pointing problems appearing with the NVidia drivers


		}
		if (previousDisplayWasEnabled<2)
			previousDisplayWasEnabled++;
	}
	if (simulationRecorder->getIsRecording())
		simulationRecorder->recordFrameIfNeeded(clientArea.x,clientArea.y,clientPos.x,clientPos.y);

	if (swapTheBuffers) // condition added on 31/1/2012... might help because some VMWare installations crash when disabling the rendering
	{
		int oglDebugTime=VDateTime::getTimeInMs();
		// the only time in the whole application (except for COpenglWidget::paintGL() ) where we can call
		// this command, otherwise we have problems with some graphic cards and VMWare on MAC:
		openglWidget->swapBuffers();

		if (App::userSettings->debugOpenGl)
		{
			int oglDebugTimeNow=VDateTime::getTimeInMs();
			printf("openGl debug --> swapBuffers: %i\n",VDateTime::getTimeDiffInMs(oglDebugTime,oglDebugTimeNow));
			oglDebugTime=oglDebugTimeNow;
		}

		openglWidget->doneCurrent();

		if (App::userSettings->debugOpenGl)
		{
			int oglDebugTimeNow=VDateTime::getTimeInMs();
			printf("openGl debug --> doneCurrent: %i\n",VDateTime::getTimeDiffInMs(oglDebugTime,oglDebugTimeNow));
			oglDebugTime=oglDebugTimeNow;
		}
	}

	_renderingTimeInMs=VDateTime::getTimeDiffInMs(startTime);
	return(_renderingTimeInMs);
}


void CMainWindow::createDefaultMenuBar(bool reserved)
{
	if (App::operationalUIParts&sim_gui_menubar)
	{ // Default menu bar
		delete _menubar;
		_menubar=new VMenubar(this);

		VMenubarType* mainMenu=VMenubar::createMenu();

		bool menuBarEnabled=true;
		if (App::ct->oglSurface!=NULL)
		{
			if (App::ct->oglSurface->isViewSelectionActive()||App::ct->oglSurface->isPageSelectionActive()||App::ct->oglSurface->isSceneSelectionActive())
				menuBarEnabled=false;
		}

// Since Qt5, Mac MenuBars don't have separators anymore... this is a quick and dirty workaround:
#define DUMMY_SPACE_QMENUBAR_QT5 ""
#ifdef MAC_VREP
#if QT_VERSION >= 0x050000
#undef DUMMY_SPACE_QMENUBAR_QT5
#define DUMMY_SPACE_QMENUBAR_QT5 "    "
#endif
#endif


		if ((App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
		{
			VMenu* fileMenu=VMenubar::createPopupMenu();
			fileSystemMenu=VMenubar::appendMenuAndDetach(mainMenu,fileMenu,menuBarEnabled,std::string(IDS_FILE_MENU_ITEM)+DUMMY_SPACE_QMENUBAR_QT5);
			connect(fileSystemMenu,SIGNAL(aboutToShow()),this,SLOT(_aboutToShowFileSystemMenu()));
		}

		VMenu* editMenu=VMenubar::createPopupMenu();
		editSystemMenu=VMenubar::appendMenuAndDetach(mainMenu,editMenu,menuBarEnabled,std::string(IDS_EDIT_MENU_ITEM)+DUMMY_SPACE_QMENUBAR_QT5);
		connect(editSystemMenu,SIGNAL(aboutToShow()),this,SLOT(_aboutToShowEditSystemMenu()));

		if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
		{
			VMenu* addMenu=VMenubar::createPopupMenu();
			addSystemMenu=VMenubar::appendMenuAndDetach(mainMenu,addMenu,menuBarEnabled,std::string(IDS_ADD_MENU_ITEM)+DUMMY_SPACE_QMENUBAR_QT5);
			connect(addSystemMenu,SIGNAL(aboutToShow()),this,SLOT(_aboutToShowAddSystemMenu()));

			VMenu* simulationMenu=VMenubar::createPopupMenu();
			simulationSystemMenu=VMenubar::appendMenuAndDetach(mainMenu,simulationMenu,menuBarEnabled,std::string(IDS_SIMULATION_MENU_ITEM)+DUMMY_SPACE_QMENUBAR_QT5);
			connect(simulationSystemMenu,SIGNAL(aboutToShow()),this,SLOT(_aboutToShowSimulationSystemMenu()));
		}

		VMenu* windowMenu=VMenubar::createPopupMenu();
		windowSystemMenu=VMenubar::appendMenuAndDetach(mainMenu,windowMenu,menuBarEnabled,std::string(IDS_TOOLS_MENU_ITEM)+DUMMY_SPACE_QMENUBAR_QT5);
		connect(windowSystemMenu,SIGNAL(aboutToShow()),this,SLOT(_aboutToShowWindowSystemMenu()));

		if (customMenuBarItemContainer->allItems.size()!=0)
		{ // Plugins
			VMenu* aMenu=VMenubar::createPopupMenu();
			customMenuBarItemContainer->_menuHandle=VMenubar::appendMenuAndDetach(mainMenu,aMenu,menuBarEnabled,std::string(IDS_MODULES_MENU_ITEM)+DUMMY_SPACE_QMENUBAR_QT5);
			connect(customMenuBarItemContainer->_menuHandle,SIGNAL(aboutToShow()),this,SLOT(_aboutToShowCustomMenu()));
		}

		VMenu* addOnMenu=VMenubar::createPopupMenu();
		addOnSystemMenu=VMenubar::appendMenuAndDetach(mainMenu,addOnMenu,menuBarEnabled,std::string(IDS_ADDON_MENU_ITEM)+DUMMY_SPACE_QMENUBAR_QT5);
		connect(addOnSystemMenu,SIGNAL(aboutToShow()),this,SLOT(_aboutToShowAddOnSystemMenu()));

		{
			VMenu* instancesMenu=VMenubar::createPopupMenu();
			instancesSystemMenu=VMenubar::appendMenuAndDetach(mainMenu,instancesMenu,menuBarEnabled,std::string(IDS_INSTANCES_MENU_ITEM)+DUMMY_SPACE_QMENUBAR_QT5);
			connect(instancesSystemMenu,SIGNAL(aboutToShow()),this,SLOT(_aboutToShowInstancesSystemMenu()));
		}


		VMenu* helpMenu=VMenubar::createPopupMenu();
		helpSystemMenu=VMenubar::appendMenuAndDetach(mainMenu,helpMenu,menuBarEnabled,std::string(IDS_HELP_MENU_ITEM)+DUMMY_SPACE_QMENUBAR_QT5);
		connect(helpSystemMenu,SIGNAL(aboutToShow()),this,SLOT(_aboutToShowHelpSystemMenu()));

		_menubar->setMenuAndDetach(mainMenu);
	}
}

void CMainWindow::removeDefaultMenuBar()
{
	if (App::operationalUIParts&sim_gui_menubar)
	{ // default menu bar
		delete _menubar;
		_menubar=NULL;
		refreshDimensions();
	}
}

void CMainWindow::createDefaultToolBars()
{
	if ((_toolbar1==NULL)&&(App::operationalUIParts&sim_gui_toolbar1))
	{
		_toolbar1=new QToolBar(tr("Navigation"));
		_toolbar1->setIconSize(QSize(28,28));
		_toolbar1->setAllowedAreas(Qt::TopToolBarArea|Qt::BottomToolBarArea);
		addToolBar(Qt::TopToolBarArea,_toolbar1);
#ifdef MAC_VREP
		_toolbar1->setMovable(false); // 14/7/2013: since Qt5.1.0 the toolbar looks just plain white when undocked under MacOS
#endif

		_toolbarActionCameraShift=_toolbar1->addAction(QIcon(":/toolbarFiles/cameraShift.png"),tr(IDS_TOOLBAR_TOOLTIP_CAMERA_SHIFT));
		_toolbarActionCameraShift->setCheckable(true);
		connect(_toolbarActionCameraShift,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionCameraShift,CAMERA_SHIFT_NAVIGATION_CMD);

		_toolbarActionCameraRotate=_toolbar1->addAction(QIcon(":/toolbarFiles/cameraRotate.png"),tr(IDS_TOOLBAR_TOOLTIP_CAMERA_ROTATE));
		_toolbarActionCameraRotate->setCheckable(true);
		connect(_toolbarActionCameraRotate,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionCameraRotate,CAMERA_ROTATE_NAVIGATION_CMD);

		_toolbarActionCameraZoom=_toolbar1->addAction(QIcon(":/toolbarFiles/cameraZoom.png"),tr(IDS_TOOLBAR_TOOLTIP_CAMERA_ZOOM));
		_toolbarActionCameraZoom->setCheckable(true);
		connect(_toolbarActionCameraZoom,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionCameraZoom,CAMERA_ZOOM_NAVIGATION_CMD);

		_toolbarActionCameraAngle=_toolbar1->addAction(QIcon(":/toolbarFiles/cameraAngle.png"),tr(IDS_TOOLBAR_TOOLTIP_CAMERA_OPENING_ANGLE));
		_toolbarActionCameraAngle->setCheckable(true);
		connect(_toolbarActionCameraAngle,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionCameraAngle,CAMERA_ANGLE_NAVIGATION_CMD);

		_toolbarActionCameraSizeToScreen=_toolbar1->addAction(QIcon(":/toolbarFiles/cameraResize.png"),tr(IDS_TOOLBAR_TOOLTIP_CAMERA_FIT_TO_SCREEN));
		_toolbarActionCameraSizeToScreen->setCheckable(false);
		connect(_toolbarActionCameraSizeToScreen,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionCameraSizeToScreen,CAMERA_SHIFT_TO_FRAME_SELECTION_CMD);

		_toolbarActionCameraFly=_toolbar1->addAction(QIcon(":/toolbarFiles/cameraFly.png"),tr(IDS_TOOLBAR_TOOLTIP_CAMERA_FLY));
		_toolbarActionCameraFly->setCheckable(true);
		connect(_toolbarActionCameraFly,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionCameraFly,CAMERA_FLY_NAVIGATION_CMD);

		_toolbar1->addSeparator();


		_toolbarActionClickSelection=_toolbar1->addAction(QIcon(":/toolbarFiles/clickSelection.png"),tr(IDS_TOOLBAR_TOOLTIP_CLICK_SELECTION));
		_toolbarActionClickSelection->setCheckable(true);
		connect(_toolbarActionClickSelection,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionClickSelection,OBJECT_SELECTION_SELECTION_CMD);

/*		
		_toolbarActionPathPointCreation=_toolbar1->addAction(QIcon(":/toolbarFiles/pathPointCreation.png"),tr(IDS_TOOLBAR_TOOLTIP_PATH_POINT_CREATION));
		_toolbarActionPathPointCreation->setCheckable(true);
		connect(_toolbarActionPathPointCreation,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionPathPointCreation,PATH_POINT_CREATION_MODE_CMD);
//*/

		_toolbarActionObjectShift=_toolbar1->addAction(QIcon(":/toolbarFiles/objectShift.png"),tr(IDS_TOOLBAR_TOOLTIP_OBJECT_SHIFT));
		_toolbarActionObjectShift->setCheckable(true);
		connect(_toolbarActionObjectShift,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionObjectShift,OBJECT_SHIFT_NAVIGATION_CMD);


		_toolbarActionObjectRotate=_toolbar1->addAction(QIcon(":/toolbarFiles/objectRotate.png"),tr(IDS_TOOLBAR_TOOLTIP_OBJECT_ROTATE));
		_toolbarActionObjectRotate->setCheckable(true);
		connect(_toolbarActionObjectRotate,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionObjectRotate,OBJECT_ROTATE_NAVIGATION_CMD);

		_toolbar1->addSeparator();


		_toolbarActionAssemble=_toolbar1->addAction(QIcon(":/toolbarFiles/assemble.png"),tr(IDSN_ASSEMBLE));
		_toolbarActionAssemble->setCheckable(false);
		connect(_toolbarActionAssemble,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionAssemble,ASSEMBLE_CMD);

		_toolbarActionTransferDna=_toolbar1->addAction(QIcon(":/toolbarFiles/transferDna.png"),tr(IDSN_TRANSFER_DNA));
		_toolbarActionTransferDna->setCheckable(false);
		connect(_toolbarActionTransferDna,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionTransferDna,TRANSFER_DNA_CMD);


		_toolbar1->addSeparator();


		_toolbarActionUndo=_toolbar1->addAction(QIcon(":/toolbarFiles/undo.png"),tr(IDSN_UNDO));
		_toolbarActionUndo->setCheckable(false);
		connect(_toolbarActionUndo,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionUndo,UNDO_CMD);

		_toolbarActionRedo=_toolbar1->addAction(QIcon(":/toolbarFiles/redo.png"),tr(IDSN_REDO));
		_toolbarActionRedo->setCheckable(false);
		connect(_toolbarActionRedo,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionRedo,REDO_CMD);
		_toolbar1->addSeparator();

		_toolbarActionDynamicContentVisualization=_toolbar1->addAction(QIcon(":/toolbarFiles/dynamics.png"),tr(IDS_TOOLBAR_TOOLTIP_VISUALIZE_DYNAMIC_CONTENT));
		_toolbarActionDynamicContentVisualization->setCheckable(true);
		connect(_toolbarActionDynamicContentVisualization,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionDynamicContentVisualization,TOGGLE_DYNAMIC_CONTENT_VISUALIZATION_CMD);


		_engineSelectCombo=new QComboBox();
#ifdef WIN_VREP
		_engineSelectCombo->setMinimumWidth(60);
		_engineSelectCombo->setMaximumWidth(60);
		_engineSelectCombo->setMinimumHeight(24);
		_engineSelectCombo->setMaximumHeight(24);
#endif
#ifdef MAC_VREP
		_engineSelectCombo->setMinimumWidth(65);
		_engineSelectCombo->setMaximumWidth(65);
		_engineSelectCombo->setMinimumHeight(24);
		_engineSelectCombo->setMaximumHeight(24);
#endif
#ifdef LIN_VREP
		_engineSelectCombo->setMinimumWidth(60);
		_engineSelectCombo->setMaximumWidth(60);
		_engineSelectCombo->setMinimumHeight(24);
		_engineSelectCombo->setMaximumHeight(24);
#endif
		_engineSelectCombo->addItem(tr(IDS_BULLET));
		_engineSelectCombo->addItem(tr(IDS_ODE));
		_engineSelectCombo->addItem(tr(IDS_VORTEX));
		_engineSelectCombo->setToolTip(strTranslate(IDS_TOOLBAR_TOOLTIP_DYNAMICS_ENGINE));
		_toolbar1->addWidget(_engineSelectCombo);
		connect(_engineSelectCombo,SIGNAL(activated(int)),this,SLOT(_engineSelectedViaToolbar(int)));

		_enginePrecisionCombo=new QComboBox();
#ifdef WIN_VREP
		_enginePrecisionCombo->setMinimumWidth(120);
		_enginePrecisionCombo->setMaximumWidth(120);
		_enginePrecisionCombo->setMinimumHeight(24);
		_enginePrecisionCombo->setMaximumHeight(24);
#endif
#ifdef MAC_VREP
		_enginePrecisionCombo->setMinimumWidth(125);
		_enginePrecisionCombo->setMaximumWidth(125);
		_enginePrecisionCombo->setMinimumHeight(24);
		_enginePrecisionCombo->setMaximumHeight(24);
#endif
#ifdef LIN_VREP
		_enginePrecisionCombo->setMinimumWidth(120);
		_enginePrecisionCombo->setMaximumWidth(120);
		_enginePrecisionCombo->setMinimumHeight(24);
		_enginePrecisionCombo->setMaximumHeight(24);
#endif
		_enginePrecisionCombo->addItem(tr(IDSN_ENGINE_VERY_PRECISE));
		_enginePrecisionCombo->addItem(tr(IDSN_ENGINE_PRECISE));
		_enginePrecisionCombo->addItem(tr(IDSN_ENGINE_FAST));
		_enginePrecisionCombo->addItem(tr(IDSN_ENGINE_VERY_FAST));
		_enginePrecisionCombo->addItem(tr(IDSN_ENGINE_CUSTOM));
		_enginePrecisionCombo->setToolTip(strTranslate(IDS_TOOLBAR_TOOLTIP_DYNAMICS_SETTINGS));
		_toolbar1->addWidget(_enginePrecisionCombo);
		connect(_enginePrecisionCombo,SIGNAL(activated(int)),this,SLOT(_enginePrecisionViaToolbar(int)));

		_timeStepConfigCombo=new QComboBox();
#ifdef WIN_VREP
		_timeStepConfigCombo->setMinimumWidth(135);
		_timeStepConfigCombo->setMaximumWidth(135);
		_timeStepConfigCombo->setMinimumHeight(24);
		_timeStepConfigCombo->setMaximumHeight(24);
#endif
#ifdef MAC_VREP
		_timeStepConfigCombo->setMinimumWidth(140);
		_timeStepConfigCombo->setMaximumWidth(140);
		_timeStepConfigCombo->setMinimumHeight(24);
		_timeStepConfigCombo->setMaximumHeight(24);
#endif
#ifdef LIN_VREP
		_timeStepConfigCombo->setMinimumWidth(135);
		_timeStepConfigCombo->setMaximumWidth(135);
		_timeStepConfigCombo->setMinimumHeight(24);
		_timeStepConfigCombo->setMaximumHeight(24);
#endif
		_timeStepConfigCombo->addItem(tr(IDSN_TIME_STEP_CONFIG_200));
		_timeStepConfigCombo->addItem(tr(IDSN_TIME_STEP_CONFIG_100));
		_timeStepConfigCombo->addItem(tr(IDSN_TIME_STEP_CONFIG_50));
		_timeStepConfigCombo->addItem(tr(IDSN_TIME_STEP_CONFIG_25));
		_timeStepConfigCombo->addItem(tr(IDSN_TIME_STEP_CONFIG_10));

		float dt=(App::ct->simulation->getSimulationTimeStep_speedModified(5)*1000.0f);
		std::string txt("dt=");
		txt+=tt::FNb(0,dt,1,false);
		txt+=IDSN_TIME_STEP_CONFIG_CUSTOM;
		_timeStepConfigCombo->addItem(txt.c_str());
//		_timeStepConfigCombo->addItem(tr(IDSN_TIME_STEP_CONFIG_CUSTOM));

		_timeStepConfigCombo->setToolTip(strTranslate(IDS_TOOLBAR_TOOLTIP_SIMULATION_TIME_STEP));
		_toolbar1->addWidget(_timeStepConfigCombo);
		connect(_timeStepConfigCombo,SIGNAL(activated(int)),this,SLOT(_timeStepConfigViaToolbar(int)));

		_toolbarActionStart=_toolbar1->addAction(QIcon(":/toolbarFiles/start.png"),tr(IDS_TOOLBAR_TOOLTIP_SIMULATION_START));
		_toolbarActionStart->setCheckable(true);
		connect(_toolbarActionStart,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionStart,START_RESUME_SIMULATION_REQUEST_CMD);

		_toolbarActionPause=_toolbar1->addAction(QIcon(":/toolbarFiles/pause.png"),tr(IDS_PAUSE_SIMULATION_MENU_ITEM));
		_toolbarActionPause->setCheckable(true);
		connect(_toolbarActionPause,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionPause,PAUSE_SIMULATION_REQUEST_CMD);

		_toolbarActionStop=_toolbar1->addAction(QIcon(":/toolbarFiles/stop.png"),tr(IDS_STOP_SIMULATION_MENU_ITEM));
		_toolbarActionStop->setCheckable(false);
		connect(_toolbarActionStop,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionStop,STOP_SIMULATION_REQUEST_CMD);

		_toolbarActionRealTime=_toolbar1->addAction(QIcon(":/toolbarFiles/realTime.png"),tr(IDS_TOOLBAR_TOOLTIP_REALTIMESIMULATION));
		_toolbarActionRealTime->setCheckable(true);
		connect(_toolbarActionRealTime,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionRealTime,TOGGLE_REAL_TIME_SIMULATION_CMD);

		_toolbarActionReduceSpeed=_toolbar1->addAction(QIcon(":/toolbarFiles/reduceSpeed.png"),tr(IDSN_SLOW_DOWN_SIMULATION));
		_toolbarActionReduceSpeed->setCheckable(false);
		connect(_toolbarActionReduceSpeed,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionReduceSpeed,SLOWER_SIMULATION_CMD);

		_toolbarActionIncreaseSpeed=_toolbar1->addAction(QIcon(":/toolbarFiles/increaseSpeed.png"),tr(IDSN_SPEED_UP_SIMULATION));
		_toolbarActionIncreaseSpeed->setCheckable(false);
		connect(_toolbarActionIncreaseSpeed,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionIncreaseSpeed,FASTER_SIMULATION_CMD);

		_toolbarActionThreadedRendering=_toolbar1->addAction(QIcon(":/toolbarFiles/threadedRendering.png"),tr(IDSN_THREADED_RENDERING));
		_toolbarActionThreadedRendering->setCheckable(true);
		connect(_toolbarActionThreadedRendering,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionThreadedRendering,THREADED_RENDERING_CMD);
		_toolbar1->addSeparator();


		_toolbarActionPageSelector=_toolbar1->addAction(QIcon(":/toolbarFiles/pageSelector.png"),tr(IDSN_PAGE_SELECTOR));
		_toolbarActionPageSelector->setCheckable(true);
		connect(_toolbarActionPageSelector,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionPageSelector,PAGE_SELECTOR_CMD);

		_toolbarActionSceneSelector=_toolbar1->addAction(QIcon(":/toolbarFiles/sceneSelector.png"),tr(IDSN_SCENE_SELECTOR));
		_toolbarActionSceneSelector->setCheckable(true);
		connect(_toolbarActionSceneSelector,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionSceneSelector,SCENE_SELECTOR_CMD);

	}
	if ((_toolbar2==NULL)&&(App::operationalUIParts&sim_gui_toolbar2))
	{
		_toolbar2=new QToolBar(tr("Tools"));
		_toolbar2->setIconSize(QSize(28,28));
		_toolbar2->setAllowedAreas(Qt::LeftToolBarArea|Qt::RightToolBarArea);
		addToolBar(Qt::LeftToolBarArea,_toolbar2);
#ifdef MAC_VREP
		_toolbar2->setMovable(false); // 14/7/2013: since Qt5.1.0 the toolbar looks just plain white when undocked under MacOS
#endif

		_toolbarActionSimulationSettings=_toolbar2->addAction(QIcon(":/toolbarFiles/simulationSettings.png"),tr(IDSN_SIMULATION_SETTINGS));
		_toolbarActionSimulationSettings->setCheckable(true);
		connect(_toolbarActionSimulationSettings,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionSimulationSettings,TOGGLE_SIMULATION_DLG_CMD);

		_toolbar2->addSeparator();

		_toolbarActionObjectProperties=_toolbar2->addAction(QIcon(":/toolbarFiles/commonProperties.png"),tr(IDSN_OBJECT_PROPERTIES_MENU_ITEM));
		_toolbarActionObjectProperties->setCheckable(true);
		connect(_toolbarActionObjectProperties,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionObjectProperties,TOGGLE_OBJECT_DLG_CMD);

		_toolbarActionCalculationModules=_toolbar2->addAction(QIcon(":/toolbarFiles/calcmods.png"),tr(IDSN_CALCULATION_MODULE_PROPERTIES_MENU_ITEM));
		_toolbarActionCalculationModules->setCheckable(true);
		connect(_toolbarActionCalculationModules,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionCalculationModules,TOGGLE_CALCULATION_DLG_CMD);


		_toolbar2->addSeparator();

		_toolbarActionCollections=_toolbar2->addAction(QIcon(":/toolbarFiles/collections.png"),tr(IDSN_COLLECTIONS));
		_toolbarActionCollections->setCheckable(true);
		connect(_toolbarActionCollections,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionCollections,TOGGLE_COLLECTION_DLG_CMD);

		_toolbarActionScripts=_toolbar2->addAction(QIcon(":/toolbarFiles/scripts.png"),tr(IDSN_SCRIPTS));
		_toolbarActionScripts->setCheckable(true);
		connect(_toolbarActionScripts,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionScripts,TOGGLE_LUA_SCRIPT_DLG_CMD);

		_toolbarActionShapeEdition=_toolbar2->addAction(QIcon(":/toolbarFiles/shapeEdition.png"),tr(IDS_SHAPE_EDITION_TOOLBAR_TIP));
		_toolbarActionShapeEdition->setCheckable(true);
		connect(_toolbarActionShapeEdition,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionShapeEdition,TOGGLE_SHAPE_EDITION_DLG_CMD);

		_toolbarActionPathEdition=_toolbar2->addAction(QIcon(":/toolbarFiles/pathEdition.png"),tr(IDS_PATH_EDITION_TOOLBAR_TIP));
		_toolbarActionPathEdition->setCheckable(true);
		connect(_toolbarActionPathEdition,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionPathEdition,TOGGLE_PATH_EDITION_DLG_CMD);

		_toolbarAction2dElements=_toolbar2->addAction(QIcon(":/toolbarFiles/2dElements.png"),tr(IDS_2D_ELEMENTS_TIP));
		_toolbarAction2dElements->setCheckable(true);
		connect(_toolbarAction2dElements,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarAction2dElements,TOGGLE_CUSTOM_UI_DLG_CMD);

		_toolbar2->addSeparator();

		_toolbarActionSelection=_toolbar2->addAction(QIcon(":/toolbarFiles/selection.png"),tr(IDSN_SELECTION_DIALOG));
		_toolbarActionSelection->setCheckable(true);
		connect(_toolbarActionSelection,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionSelection,TOGGLE_SELECTION_DLG_CMD);

		_toolbarActionModelBrowser=_toolbar2->addAction(QIcon(":/toolbarFiles/modelBrowser.png"),tr(IDSN_MODEL_BROWSER));
		_toolbarActionModelBrowser->setCheckable(true);
		connect(_toolbarActionModelBrowser,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionModelBrowser,TOGGLE_BROWSER_DLG_CMD);

		_toolbarActionSceneHierarchy=_toolbar2->addAction(QIcon(":/toolbarFiles/sceneHierarchy.png"),tr(IDSN_SCENE_HIERARCHY));
		_toolbarActionSceneHierarchy->setCheckable(true);
		connect(_toolbarActionSceneHierarchy,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionSceneHierarchy,TOGGLE_HIERARCHY_DLG_CMD);

		_toolbarActionLayers=_toolbar2->addAction(QIcon(":/toolbarFiles/layers.png"),tr(IDS_LAYERS));
		_toolbarActionLayers->setCheckable(true);
		connect(_toolbarActionLayers,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionLayers,TOGGLE_LAYERS_DLG_CMD);

		_toolbarActionAviRecorder=_toolbar2->addAction(QIcon(":/toolbarFiles/aviRecorder.png"),tr(IDSN_AVI_RECORDER));
		_toolbarActionAviRecorder->setCheckable(true);
		connect(_toolbarActionAviRecorder,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionAviRecorder,TOGGLE_AVI_RECORDER_DLG_CMD);

		_toolbarActionUserSettings=_toolbar2->addAction(QIcon(":/toolbarFiles/userSettings.png"),tr(IDSN_USER_SETTINGS));
		_toolbarActionUserSettings->setCheckable(true);
		connect(_toolbarActionUserSettings,SIGNAL(triggered()),signalMapper,SLOT(map()));
		signalMapper->setMapping(_toolbarActionUserSettings,TOGGLE_SETTINGS_DLG_CMD);
 
	}
	refreshDimensions();
	App::ct->objCont->toolbarRefreshFlag=true;

}

void CMainWindow::hideWindow()
{
	hide();
}

void CMainWindow::showWindow()
{
	show();
}

void CMainWindow::hideWindowAndAllDialogs()
{
	hideWindow();
	if (dlgCont!=NULL)
		dlgCont->hideDialogsButDontCloseThem();
	App::ct->scintillaEditorContainer->showOrHideAllEditors(false);
}

void CMainWindow::showWindowAndAllDialogs()
{
	showWindow();
	if (dlgCont!=NULL)
		dlgCont->showDialogsButDontOpenThem();
	App::ct->scintillaEditorContainer->showOrHideAllEditors(true);
}


bool CMainWindow::event(QEvent* event)
{
	if (event->type()==QEvent::Close)
	{
		openglWidget->setOpenGlDisplayEnabled(true); // We might be in fast simulation mode...
		CIloIlo::simulationShowEnabled=false;
		CFileMenu::evaluateMenuSelection(EXIT_SIMULATOR_CMD);
		event->ignore();
		return(true);
	}
	return(QMainWindow::event(event));
}

void CMainWindow::dragEnterEvent(QDragEnterEvent* dEvent)
{
	if (dEvent->mimeData()->hasUrls())
	{
		QStringList pathList;
        QList<QUrl> urlList=dEvent->mimeData()->urls();
		if (urlList.size()>0)
		{
			int sceneCnt=0;
			int modelCnt=0;
			int uiCnt=0;
			int fileCnt=0;
			for (int i=0;i<urlList.size()&&(i<100);++i)
			{
				std::string pathFile=urlList.at(i).toLocalFile().toStdString();
				std::string extension(VVarious::splitPath_fileExtension(pathFile));
				if ( (extension.length()==3)&&(extension[0]=='t')&&(extension[1]=='t') )
				{
					if (extension[2]=='t')
						sceneCnt++;
					if (extension[2]=='m')
						modelCnt++;
					if (extension[2]=='b')
						uiCnt++;
				}
				fileCnt++;
			}
			if ( (fileCnt==sceneCnt)&&(sceneCnt>0) )
				dEvent->acceptProposedAction();
			if ( (fileCnt==modelCnt)&&(modelCnt>0) )
				dEvent->acceptProposedAction();
			if ( (fileCnt==uiCnt)&&(uiCnt>0) )
				dEvent->acceptProposedAction();
		}
	}
}

void CMainWindow::dropEvent(QDropEvent* dEvent)
{
	if (dEvent->mimeData()->hasUrls())
	{
		QStringList pathList;
        QList<QUrl> urlList=dEvent->mimeData()->urls();
		if (urlList.size()>0)
		{
			std::vector<std::string> scenes;
			std::vector<std::string> models;
			std::vector<std::string> uis;
			int fileCnt=0;
			for (int i=0;i<urlList.size()&&(i<100);++i)
			{
				std::string pathFile=urlList.at(i).toLocalFile().toLocal8Bit().data();
				std::string extension(VVarious::splitPath_fileExtension(pathFile));
				if ( (extension.length()==3)&&(extension[0]=='t')&&(extension[1]=='t') )
				{
					if (extension[2]=='t')
						scenes.push_back(pathFile);
					if (extension[2]=='m')
						models.push_back(pathFile);
					if (extension[2]=='b')
						uis.push_back(pathFile);
				}
				fileCnt++;
			}
			if ( ( (fileCnt==int(scenes.size()))&&(scenes.size()>0) ) ||  ( (fileCnt==int(models.size()))&&(models.size()>0) ) ||  ( (fileCnt==int(uis.size()))&&(uis.size()>0) ) )
				_dropFilesIntoScene(scenes,models,uis);
		}
	}
}


void CMainWindow::onLeftMouseButtonDoubleClickTT(int xPos,int yPos)
{
	recomputeMousePos(xPos,yPos);
	int selectionStatus=NOSELECTION;
	int mm=App::ct->getMouseMode();
	if ( shiftKeyDown&&(mm&sim_navigation_shiftselection) )
		selectionStatus=SHIFTSELECTION;	
	else if ( ctrlKeyDown&&(mm&sim_navigation_ctrlselection) ) 
		selectionStatus=CTRLSELECTION;
	if (openglWidget->getOpenGlDisplayEnabled())
		App::ct->oglSurface->leftMouseButtonDoubleClick(mouseRenderingPos.x-clientPos.x,mouseRenderingPos.y-clientPos.y,selectionStatus);
	openglWidget->setOpenGlDisplayEnabled(true); // enable the display again
}


void CMainWindow::onLeftMouseButtonDownTT(int xPos,int yPos)
{ // keys: bit0: ctrl, bit1: shift
	recomputeMousePos(xPos,yPos);
	int mm=App::ct->getMouseMode();
	int selectionStatus=NOSELECTION;
	if ( shiftKeyDown&&ctrlKeyDown&&(mm&sim_navigation_ctrlselection) )
		selectionStatus=CTRLSELECTION;														
	else if ( shiftKeyDown&&(mm&sim_navigation_shiftselection) ) 
		selectionStatus=SHIFTSELECTION;	
	else if ( ctrlKeyDown&&(mm&sim_navigation_ctrlselection) ) 
		selectionStatus=CTRLSELECTION;
	App::ct->oglSurface->clearCaughtElements(0xffff-sim_left_button);
	if (openglWidget->getOpenGlDisplayEnabled())
		App::ct->oglSurface->leftMouseButtonDown(mouseRenderingPos.x-clientPos.x,mouseRenderingPos.y-clientPos.y,selectionStatus);
	openglWidget->setOpenGlDisplayEnabled(true); // Enable the display again
}

void CMainWindow::onRightMouseButtonDownTT(int xPos,int yPos)
{
	if (App::ct->objCont!=NULL)
		App::ct->objCont->setLightDialogRefreshFlag();
	recomputeMousePos(xPos,yPos);
	App::ct->oglSurface->clearCaughtElements(0xffff-sim_right_button);
	if (openglWidget->getOpenGlDisplayEnabled())
		App::ct->oglSurface->rightMouseButtonDown(mouseRenderingPos.x-clientPos.x,mouseRenderingPos.y-clientPos.y);
	openglWidget->setOpenGlDisplayEnabled(true); // Enable the display again
}

void CMainWindow::onLeftMouseButtonUpTT(int xPos,int yPos)
{
	CIloIlo::flyModeCameraHandle=-1;
	CIloIlo::flyModeCameraHandleInstance=-1;
	if (App::ct->objCont!=NULL)
		App::ct->objCont->setLightDialogRefreshFlag(); // to refresh dlgs when an object has been dragged for instance
	App::ct->objCont->_ikManipulationObjectID=-1;
	recomputeMousePos(xPos,yPos);
	if (App::ct->oglSurface->getCaughtElements()&sim_left_button)

		App::ct->oglSurface->leftMouseButtonUp(mouseRenderingPos.x-clientPos.x,mouseRenderingPos.y-clientPos.y);
	App::ct->oglSurface->clearCaughtElements(0xffff-sim_left_button);
}

void CMainWindow::onRightMouseButtonUpTT(int xPos,int yPos)
{ // Right mouse button up is exclusively reserved for pop-up menu
	recomputeMousePos(xPos,yPos);
	if (App::ct->oglSurface->getCaughtElements()&sim_right_button)
	{
		QPoint glob(openglWidget->mapToGlobal(QPoint(xPos,yPos)));
		App::ct->oglSurface->rightMouseButtonUp(mouseRenderingPos.x-clientPos.x,mouseRenderingPos.y-clientPos.y,glob.x(),glob.y(),this);
	}

	App::ct->oglSurface->clearCaughtElements(0xffff-sim_right_button);
}

void CMainWindow::getOglMousePos(int pos[2])
{
	pos[0]=_oglMousePos[0];
	pos[1]=_oglMousePos[1];
}

void CMainWindow::onMouseMoveTT(int xPos,int yPos)
{
	recomputeMousePos(xPos,yPos);
	if (openglWidget->getOpenGlDisplayEnabled())
	{
		if (App::ct->oglSurface->getCaughtElements()&(sim_right_button|sim_left_button))
			App::ct->oglSurface->mouseMove(mouseRenderingPos.x-clientPos.x,mouseRenderingPos.y-clientPos.y,false);
		else 
			App::ct->oglSurface->mouseMove(mouseRenderingPos.x-clientPos.x,mouseRenderingPos.y-clientPos.y,true);
	}
	_oglMousePos[0]=mouseRenderingPos.x-clientPos.x;
	_oglMousePos[1]=mouseRenderingPos.y-clientPos.y;
}

void CMainWindow::actualizeButtonState()
{ // This is only for the default toolbars
	bool allowFitToView=false;
	if (App::ct->oglSurface!=NULL)
	{
		int pageIndex=App::ct->oglSurface->pageCont->getActivePageIndex();
		CSPage* page=App::ct->oglSurface->pageCont->getPage(pageIndex);
		if (page!=NULL)
		{
			int ind=page->getLastMouseDownViewIndex();
			if (ind==-1)
				ind=0;
			CSView* view=page->getView(ind);
			if (view!=NULL)
			{
				CCamera* cam=App::ct->objCont->getCamera(view->getLinkedObjectID());
				allowFitToView=((cam!=NULL)&&(App::ct->objCont->getEditModeType()!=BUTTON_EDIT_MODE));
			}
		}
	}

	bool noShapePathEditModeNoSelector=true;
	bool noSelector=true;
	if ((App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)||(App::ct->objCont->getEditModeType()==PATH_EDIT_MODE))
		noShapePathEditModeNoSelector=false;
	if (App::ct->oglSurface->isSceneSelectionActive()||App::ct->oglSurface->isPageSelectionActive()||App::ct->oglSurface->isViewSelectionActive())
		noSelector=false;
	if (!noSelector)
		noShapePathEditModeNoSelector=false;

	// There is another such routine!! XXBFVGA
	bool assembleEnabled=false;
	bool disassembleEnabled=false;
	int selS=App::ct->objCont->getSelSize();
	if (selS==1)
	{ // here we can only have disassembly
		C3DObject* it=App::ct->objCont->getLastSelection();
		disassembleEnabled=(it->getParent()!=NULL);
	}
	else if (selS==2)
	{ // here we can have assembly or disassembly
		C3DObject* it1=App::ct->objCont->getLastSelection();
		C3DObject* it2=App::ct->objCont->getObject(App::ct->objCont->getSelID(0));
		if ((it1->getParent()==it2)||(it2->getParent()==it1))
			disassembleEnabled=true; // disassembly
		else
		{ // assembly
			if ( it1->getAssemblyRole(false)&&it2->getAssemblyRole(true)&&(it1->getAssemblyMatchValue(false)==it2->getAssemblyMatchValue(true)) )
				assembleEnabled=true;
			else
			{ // here we might have the opposite of what we usually do to assemble (i.e. last selection should always be parent, but not here)
				// we assemble anyways if the roles are unequivoque:
				if ( it2->getAssemblyRole(false)&&it1->getAssemblyRole(true)&&(it2->getAssemblyMatchValue(false)==it1->getAssemblyMatchValue(true)) )
					assembleEnabled=true;
			}
		}
	}

	bool transferDnaAllowed=false;
	if ( (selS==1)&&noSelector&&(App::ct->objCont->getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped() )
	{
		C3DObject* it=App::ct->objCont->getLastSelection();
		if (it->getObjectProperty()&sim_objectproperty_canupdatedna)
		{ 
			bool model=it->getModelBase();
			// Check if we have a sibling in the scene:
			for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
			{
				C3DObject* it2=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
				if ( (it2!=it)&&(it2->getObjectProperty()&sim_objectproperty_canupdatedna)&&(it2->getUniqueUpdatableString().compare(it->getUniqueUpdatableString())==0) )
				{
					if (!model)
					{
						transferDnaAllowed=true; // yes, and it is not a model
						break;
					}
					else
					{ // Here we also have to check that the sibling model is not located in the same hierarchy as this one:
						bool sameHierarchy=false;
						while (true)
						{
							if (it2==NULL)
								break;
							if (it2==it)
							{
								sameHierarchy=true;
								break;
							}
							it2=it2->getParent();
						}
						if (!sameHierarchy)
						{
							transferDnaAllowed=true;
							break;
						}
					}
				}
			}
		}
	}


	if (_toolbar1!=NULL)
	{ // We enable/disable some buttons:
		_toolbarActionCameraShift->setEnabled(noSelector);
		_toolbarActionCameraRotate->setEnabled(noSelector);
		_toolbarActionCameraZoom->setEnabled(noSelector);
		_toolbarActionCameraAngle->setEnabled(noSelector);
		_toolbarActionCameraSizeToScreen->setEnabled(allowFitToView&&noSelector);
		_toolbarActionCameraFly->setEnabled(noSelector);

		if (disassembleEnabled)
			_toolbarActionAssemble->setIcon(QIcon(":/toolbarFiles/disassemble.png"));
		else
			_toolbarActionAssemble->setIcon(QIcon(":/toolbarFiles/assemble.png"));
		_toolbarActionAssemble->setEnabled(assembleEnabled||disassembleEnabled);

		_toolbarActionTransferDna->setEnabled(transferDnaAllowed);

		_toolbarActionObjectShift->setEnabled(noSelector);
		bool rot=true;
		if (App::ct->objCont!=NULL)
			rot=App::ct->objCont->getPointerToSelectedPathPointIndices_nonEditMode()->size()==0;
		_toolbarActionObjectRotate->setEnabled(rot&&noSelector);
		_toolbarActionClickSelection->setEnabled(noSelector);

		_toolbarActionUndo->setEnabled(App::ct->undoBufferContainer->canUndo()&&noSelector);
		_toolbarActionRedo->setEnabled(App::ct->undoBufferContainer->canRedo()&&noSelector);

		_toolbarActionDynamicContentVisualization->setEnabled((!App::ct->simulation->isSimulationStopped())&&noSelector);
		_engineSelectCombo->setEnabled((App::ct->objCont->getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped()&&App::ct->dynamicsContainer->getDynamicsEnabled()&&noSelector);
		_enginePrecisionCombo->setEnabled((App::ct->objCont->getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped()&&App::ct->dynamicsContainer->getDynamicsEnabled()&&noSelector);
		_timeStepConfigCombo->setEnabled((App::ct->objCont->getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped()&&noSelector);
		_toolbarActionStart->setEnabled((App::ct->objCont->getEditModeType()==NO_EDIT_MODE)&&(!App::ct->simulation->isSimulationRunning())&&noSelector);
		_toolbarActionPause->setEnabled((App::ct->objCont->getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationRunning()&&noSelector);
		_toolbarActionStop->setEnabled((App::ct->objCont->getEditModeType()==NO_EDIT_MODE)&&(!App::ct->simulation->isSimulationStopped())&&noSelector);
		_toolbarActionRealTime->setEnabled((App::ct->objCont->getEditModeType()==NO_EDIT_MODE)&&App::ct->simulation->isSimulationStopped()&&noSelector);
		_toolbarActionReduceSpeed->setEnabled(App::ct->simulation->canGoSlower()&&noSelector);
		_toolbarActionIncreaseSpeed->setEnabled(App::ct->simulation->canGoFaster()&&noSelector);
		_toolbarActionThreadedRendering->setEnabled(App::ct->simulation->canToggleThreadedRendering()&&noSelector);

		_toolbarActionPageSelector->setEnabled((!App::ct->oglSurface->isViewSelectionActive())&&(!App::ct->oglSurface->isSceneSelectionActive())&&(App::ct->objCont->getEditModeType()!=BUTTON_EDIT_MODE));

		_toolbarActionSceneSelector->setEnabled((App::ct->getInstanceNumber()>1)&&App::ct->simulation->isSimulationStopped()&&(!App::ct->oglSurface->isPageSelectionActive())&&(!App::ct->oglSurface->isViewSelectionActive()));

		// Now we check/uncheck some buttons:
		_toolbarActionCameraShift->setChecked((App::ct->getMouseMode()&0x00ff)==sim_navigation_camerashift);
		_toolbarActionCameraRotate->setChecked((App::ct->getMouseMode()&0x00ff)==sim_navigation_camerarotate);
		_toolbarActionCameraZoom->setChecked((App::ct->getMouseMode()&0x00ff)==sim_navigation_camerazoom);
		_toolbarActionCameraAngle->setChecked((App::ct->getMouseMode()&0x00ff)==sim_navigation_cameraangle);
		_toolbarActionCameraFly->setChecked((App::ct->getMouseMode()&0x00ff)==sim_navigation_camerafly);
		_toolbarActionObjectShift->setChecked((App::ct->getMouseMode()&0x00ff)==sim_navigation_objectshift);
		_toolbarActionObjectRotate->setChecked((App::ct->getMouseMode()&0x00ff)==sim_navigation_objectrotate);
		_toolbarActionClickSelection->setChecked((App::ct->getMouseMode()&0x0300)==sim_navigation_clickselection);
//		_toolbarActionPathPointCreation->setChecked((App::ct->getMouseMode()&0x0300)==sim_navigation_createpathpoint);

		_toolbarActionDynamicContentVisualization->setChecked(App::ct->simulation->getDynamicContentVisualizationOnly());

		if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_bullet)
			_engineSelectCombo->setCurrentIndex(0);
		if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_ode)
			_engineSelectCombo->setCurrentIndex(1);
		if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_vortex)
			_engineSelectCombo->setCurrentIndex(2);

		_enginePrecisionCombo->setCurrentIndex(App::ct->dynamicsContainer->getUseDynamicDefaultCalculationParameters());
		if (App::ct->simulation->isSimulationStopped())
		{
			_timeStepConfigCombo->setToolTip(strTranslate(IDS_TOOLBAR_TOOLTIP_SIMULATION_TIME_STEP));
			_timeStepConfigCombo->setItemText(0,tr(IDSN_TIME_STEP_CONFIG_200));
			_timeStepConfigCombo->setItemText(1,tr(IDSN_TIME_STEP_CONFIG_100));
			_timeStepConfigCombo->setItemText(2,tr(IDSN_TIME_STEP_CONFIG_50));
			_timeStepConfigCombo->setItemText(3,tr(IDSN_TIME_STEP_CONFIG_25));
			_timeStepConfigCombo->setItemText(4,tr(IDSN_TIME_STEP_CONFIG_10));

//			_timeStepConfigCombo->setItemText(5,tr(IDSN_TIME_STEP_CONFIG_CUSTOM));
			float dt=(App::ct->simulation->getSimulationTimeStep_speedModified(5)*1000.0f);
			std::string txt("dt=");
			txt+=tt::FNb(0,dt,1,false);
			txt+=IDSN_TIME_STEP_CONFIG_CUSTOM;
			_timeStepConfigCombo->setItemText(5,txt.c_str());

			_timeStepConfigCombo->setCurrentIndex(App::ct->simulation->getDefaultSimulationParameterIndex());
		}
		else
		{
			_timeStepConfigCombo->setToolTip(strTranslate(IDS_TOOLBAR_TOOLTIP_DT_SIMULATION_TIME_STEP_AND_PPF));
			float dt=(App::ct->simulation->getSimulationTimeStep_speedModified()*1000.0f);
			std::string txt("dt=");
			txt+=tt::FNb(0,dt,1,false);
			txt+=" ms, ppf=";
			txt+=tt::FNb(App::ct->simulation->getSimulationPassesPerRendering_speedModified());
			_timeStepConfigCombo->setItemText(App::ct->simulation->getDefaultSimulationParameterIndex(),txt.c_str());
		}

		_toolbarActionStart->setChecked(App::ct->simulation->isSimulationRunning());
		_toolbarActionPause->setChecked(App::ct->simulation->isSimulationPaused());
		_toolbarActionRealTime->setChecked(App::ct->simulation->getRealTimeSimulation());

		_toolbarActionThreadedRendering->setChecked(App::ct->simulation->getThreadedRenderingIfSimulationWasRunning());

		_toolbarActionPageSelector->setChecked(App::ct->oglSurface->isPageSelectionActive());

		_toolbarActionSceneSelector->setChecked(App::ct->oglSurface->isSceneSelectionActive());

	}
	if (_toolbar2!=NULL)
	{ // We enable/disable some buttons:
		_toolbarActionSimulationSettings->setEnabled(noShapePathEditModeNoSelector);
		_toolbarActionObjectProperties->setEnabled(noShapePathEditModeNoSelector);
		_toolbarActionCalculationModules->setEnabled(noShapePathEditModeNoSelector);
		_toolbarActionCollections->setEnabled(noShapePathEditModeNoSelector);

		_toolbarActionScripts->setEnabled(noShapePathEditModeNoSelector);

		_toolbarActionShapeEdition->setEnabled((App::ct->objCont->isLastSelectionAShape()&&App::ct->simulation->isSimulationStopped()&&(App::ct->objCont->getEditModeType()==NO_EDIT_MODE))||(App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE));
		_toolbarActionPathEdition->setEnabled((App::ct->objCont->isLastSelectionAPath()&&App::ct->simulation->isSimulationStopped()&&(App::ct->objCont->getEditModeType()==NO_EDIT_MODE))||(App::ct->objCont->getEditModeType()&PATH_EDIT_MODE));
		_toolbarAction2dElements->setEnabled(App::ct->simulation->isSimulationStopped()&&((App::ct->objCont->getEditModeType()==NO_EDIT_MODE)||(App::ct->objCont->getEditModeType()==BUTTON_EDIT_MODE)));

		_toolbarActionSelection->setEnabled(noShapePathEditModeNoSelector);
		_toolbarActionModelBrowser->setEnabled(App::ct->objCont->getEditModeType()==NO_EDIT_MODE);
		_toolbarActionSceneHierarchy->setEnabled(true);
		_toolbarActionLayers->setEnabled(true);
		_toolbarActionAviRecorder->setEnabled(CAuxLibVideo::video_recorderGetEncoderString!=NULL);
		_toolbarActionUserSettings->setEnabled(noShapePathEditModeNoSelector);

		// Now we check/uncheck some buttons:
		_toolbarActionSimulationSettings->setChecked(dlgCont->isVisible(SIMULATION_DLG));
		_toolbarActionObjectProperties->setChecked(dlgCont->isVisible(OBJECT_DLG));
		_toolbarActionCalculationModules->setChecked(dlgCont->isVisible(CALCULATION_DLG));
		_toolbarActionCollections->setChecked(dlgCont->isVisible(COLLECTION_DLG));

		_toolbarActionScripts->setChecked(dlgCont->isVisible(LUA_SCRIPT_DLG));

		_toolbarActionShapeEdition->setChecked(App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE);
		_toolbarActionPathEdition->setChecked(App::ct->objCont->getEditModeType()==PATH_EDIT_MODE);

		_toolbarAction2dElements->setChecked(App::ct->objCont->getEditModeType()==BUTTON_EDIT_MODE);
		_toolbarActionSelection->setChecked(dlgCont->isVisible(SELECTION_DLG));
		_toolbarActionModelBrowser->setChecked(dlgCont->isVisible(BROWSER_DLG));
		_toolbarActionSceneHierarchy->setChecked(dlgCont->isVisible(HIERARCHY_DLG));
		_toolbarActionLayers->setChecked(dlgCont->isVisible(LAYERS_DLG));
		_toolbarActionAviRecorder->setChecked(dlgCont->isVisible(AVI_RECORDER_DLG));
		_toolbarActionUserSettings->setChecked(dlgCont->isVisible(SETTINGS_DLG));
	}
}

void CMainWindow::recomputeMousePos(int xPos,int yPos)
{
	mousePos.x=xPos;
	mousePos.y=clientArea.y-yPos;
	mouseRenderingPos.x=mousePos.x;
	mouseRenderingPos.y=mousePos.y;
}

void CMainWindow::recomputeClientSizeAndPos()
{
	if ( (App::ct->oglSurface!=NULL)&&windowHandle()&&windowHandle()->isExposed()&&(clientArea.x!=0)&&(clientArea.y!=0) ) // Added the two last args to avoid a collaps of the hierarchy when switching to another app (2011/01/26)
		App::ct->oglSurface->setSurfaceSizeAndPosition(clientArea.x,clientArea.y+App::userSettings->renderingSurfaceVResize,clientPos.x,clientPos.y+App::userSettings->renderingSurfaceVShift);
}

void CMainWindow::_dropFilesIntoScene(const std::vector<std::string>& tttFiles,const std::vector<std::string>& ttmFiles,const std::vector<std::string>& ttbFiles)
{
	IF_UI_EVENT_CAN_WRITE_DATA
	{
		if (((tttFiles.size()>0)&&(ttmFiles.size()==0)&&(ttbFiles.size()==0))||((tttFiles.size()==0)&&(ttmFiles.size()>0)&&(ttbFiles.size()==0))||((tttFiles.size()==0)&&(ttmFiles.size()==0)&&(ttbFiles.size()>0)))
		{
			if (App::ct->objCont->getEditModeType()!=NO_EDIT_MODE)
				App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Drag and drop"),strTranslate(IDS_END_EDIT_MODE_BEFORE_PROCEEDING),VMessageBox::OKELI);
			else
			{
				if (tttFiles.size()>0)
				{ // loading (a) scene(s):
					if (!App::ct->simulation->isSimulationStopped())
						App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Drag and drop"),strTranslate(IDS_STOP_SIMULATION_BEFORE_PROCEEDING),VMessageBox::OKELI);
					else
					{
						for (int i=0;i<int(tttFiles.size());i++)
						{
							CFileMenu::createNewScene(true,false);
							CFileMenu::loadScene(tttFiles[i].c_str(),true,false);
						}
					}
				}
				else
				{
					if (ttmFiles.size()>0)
					{ // loading (a) model(s):
						for (int i=0;i<int(ttmFiles.size());i++)
							CFileMenu::loadModel(ttmFiles[i].c_str(),true,false,false,NULL,false); // will temporarily halt the simulation thread.
						App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
					}
					if (ttbFiles.size()>0)
					{ // loading (a) ui(s):
						for (int i=0;i<int(ttbFiles.size());i++)
							CFileMenu::loadUserInterfaces(ttbFiles[i].c_str(),true,false,NULL,false); // will temporarily halt the simulation thread.
						App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
					}
				}
			}
		}
	}
}


void CMainWindow::_engineSelectedViaToolbar(int index)
{
	if (index==0)
		App::ct->simulation->evaluateMenuSelection(TOGGLE_TO_BULLET_ENGINE_CMD);
	if (index==1)
		App::ct->simulation->evaluateMenuSelection(TOGGLE_TO_ODE_ENGINE_CMD);
	if (index==2)
		App::ct->simulation->evaluateMenuSelection(TOGGLE_TO_VORTEX_ENGINE_CMD);
}

void CMainWindow::_enginePrecisionViaToolbar(int index)
{
	App::ct->dynamicsContainer->setUseDynamicDefaultCalculationParameters(index);
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
}

void CMainWindow::_timeStepConfigViaToolbar(int index)
{
	App::ct->simulation->setDefaultSimulationParameterIndex(index);
	App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
}

void CMainWindow::_vrepPopupMessageHandler(int id)
{
	if (VMenubar::doNotExecuteCommandButMemorizeIt)
	{
		VMenubar::memorizedCommand=id;
		VMenubar::doNotExecuteCommandButMemorizeIt=false;
		return;
	}
	_vrepMessageHandler(id);
}

void CMainWindow::_vrepMessageHandler(int id)
{
	// NOT HERE!!  IF_UI_EVENT_CAN_READ_DATA
	{
		bool processed=false;
		processed=CToolBarCommand::evaluateCommand(id);
		if (!processed)
			processed=CFileMenu::evaluateMenuSelection(id);
		if (!processed)
			processed=CObjectEdition::evaluateMenuSelection(id);
		if ((App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)&&(!processed))
			processed=CTriangleVertexEdition::evaluateMenuSelection(id);
		if ((App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)&&(!processed))
			processed=CPathEdition::evaluateMenuSelection(id,NULL);
		if ((App::ct->objCont->getEditModeType()&BUTTON_EDIT_MODE)&&(!processed))
			processed=App::ct->buttonBlockContainer->evaluateMenuSelection(id);
		if (!processed)
			processed=CAddMenu::evaluateMenuSelection(id,NULL);
		if (!processed)
			processed=App::ct->simulation->evaluateMenuSelection(id);
		if (!processed)
			processed=App::ct->luaScriptContainer->evaluateMenuSelection(id);
		if (!processed)
			processed=dlgCont->evaluateMenuSelection(id);
		if (!processed)
			processed=CHelpMenu::evaluateMenuSelection(id);
		if (!processed)
			processed=App::ct->evaluateMenuSelection(id);
		if (!processed)
			processed=customMenuBarItemContainer->evaluateMenuSelection(id);
		App::ct->objCont->toolbarRefreshFlag=true;
	}
}

void CMainWindow::_aboutToShowFileSystemMenu()
{
	fileSystemMenu->clear();
	CFileMenu::addMenu(fileSystemMenu);
}

void CMainWindow::_aboutToShowEditSystemMenu()
{
	editSystemMenu->clear();
	if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
		CObjectEdition::addMenu(editSystemMenu);
	else if (App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)
		CTriangleVertexEdition::addMenu(editSystemMenu);
	else if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
		CPathEdition::addMenu(editSystemMenu,NULL);
	else if (App::ct->objCont->getEditModeType()&BUTTON_EDIT_MODE)
		App::ct->buttonBlockContainer->addMenu(editSystemMenu);
}

void CMainWindow::_aboutToShowAddSystemMenu()
{
	addSystemMenu->clear();
	CAddMenu::addMenu(addSystemMenu,NULL,false);
}

void CMainWindow::_aboutToShowSimulationSystemMenu()
{
	simulationSystemMenu->clear();
	App::ct->simulation->addMenu(simulationSystemMenu);
}

void CMainWindow::_aboutToShowAddOnSystemMenu()
{
	addOnSystemMenu->clear();
	App::ct->luaScriptContainer->addMenu(addOnSystemMenu);
}

void CMainWindow::_aboutToShowWindowSystemMenu()
{
	windowSystemMenu->clear();
	dlgCont->addMenu(windowSystemMenu);
}

void CMainWindow::_aboutToShowHelpSystemMenu()
{
	helpSystemMenu->clear();
	CHelpMenu::addMenu(helpSystemMenu);
}

void CMainWindow::_aboutToShowInstancesSystemMenu()
{
	instancesSystemMenu->clear();
	App::ct->addMenu(instancesSystemMenu);
}

void CMainWindow::_aboutToShowCustomMenu()
{
	customMenuBarItemContainer->_menuHandle->clear();
	customMenuBarItemContainer->addMenus(NULL,customMenuBarItemContainer->_menuHandle);
}

void CMainWindow::splitterMoved(int pos,int index)
{
	if (splitter->sizes()[1]<=60)
		statusBar->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	else
		statusBar->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	bool static first=true;
	if (first)
	{
		statusBar->setMinimumHeight(36);
		statusBar->setMaximumHeight(600);
	}
	first=false;
}


void CMainWindow::handleCommonKeyPressEvents(int key)
{ // is called from the page container and from the hierarchy to handle all shortcuts that operate in a same way
	if (key==CTRL_Y_KEY)
		CObjectEdition::evaluateMenuSelection(REDO_CMD);
	if (key==CTRL_Z_KEY)
		CObjectEdition::evaluateMenuSelection(UNDO_CMD);
	if (key==CTRL_S_KEY)
		CFileMenu::evaluateMenuSelection(SAVE_SCENE_CMD);
	if (key==CTRL_O_KEY)
		CFileMenu::evaluateMenuSelection(OPEN_SCENE_CMD);
	if (key==CTRL_W_KEY)
		CFileMenu::evaluateMenuSelection(CLOSE_SCENE_CMD);
	if (key==CTRL_Q_KEY)
		CFileMenu::evaluateMenuSelection(EXIT_SIMULATOR_CMD);
	if (key==CTRL_N_KEY)
		CFileMenu::evaluateMenuSelection(NEW_SCENE_CMD);
	if (key==CTRL_A_KEY)
	{
		if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
			CObjectEdition::evaluateMenuSelection(SELECT_ALL_OBJECTS_CMD);
		else if (App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)
			CTriangleVertexEdition::evaluateMenuSelection(SELECT_ALL_SHAPE_EDIT_MODE_ITEMS_CMD);
		else if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
			CPathEdition::evaluateMenuSelection(SELECT_ALL_PATH_POINTS_CMD,NULL);
		return;
	}
	if (key==CTRL_SPACE_KEY)
	{
		if (App::ct->simulation->isSimulationRunning())
			App::ct->simulation->evaluateMenuSelection(STOP_SIMULATION_REQUEST_CMD);
		else
			App::ct->simulation->evaluateMenuSelection(START_RESUME_SIMULATION_REQUEST_CMD);
	}
	if (key==CTRL_D_KEY)
	{
		if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
			App::mainWindow->dlgCont->evaluateMenuSelection(OPEN_OBJECT_DLG_OBJECT_SPECIFIC_PART_CMD);
	}
	if (key==CTRL_G_KEY)
	{
		if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
			App::mainWindow->dlgCont->evaluateMenuSelection(OPEN_CALCULATION_DLG_CMD);
	}
	if (key==CTRL_E_KEY)
	{
		if ((App::ct->getMouseMode()&0x00ff)==sim_navigation_camerashift)
			App::ct->setMouseMode((App::ct->getMouseMode()&0xff00)|sim_navigation_objectshift);
		else
		{
			if ((App::ct->getMouseMode()&0x00ff)==sim_navigation_objectshift)
				App::ct->setMouseMode((App::ct->getMouseMode()&0xff00)|sim_navigation_objectrotate);
			else
				App::ct->setMouseMode((App::ct->getMouseMode()&0xff00)|sim_navigation_camerashift);
		}
	}

}


