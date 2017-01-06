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

// This file requires some serious refactoring!

#include "vrepPrecompiledHeader.h"
#include "v_rep_internal.h"
#include "PageContainer.h"
#include "Tt.h"
#include "IloIlo.h"
#include "OGL.h"
#include "ObjectEdition.h"
#include "TriangleVertexEdition.h"
#include "Simulation.h"
#include "3DObject.h"

#include "GV.h"
#include "ToolBarCommand.h"
#include "PathEdition.h"
#include "pluginContainer.h"
#include "FileMenu.h"
#include "v_repStrings.h"
#include "App.h"
#include "collDistInterface.h"
#include "dynInterface.h"
#include "pathPlanningInterface.h"

CPageContainer::CPageContainer()
{
	_activePageIndex=0;
	_numberOfPages=0;
	rightMouseCaughtBy=-1;
	leftMouseCaughtBy=-1;
	prepareForPopupMenu=-1;
	rightMouseCaughtSoftDialog=false;
	leftMouseCaughtSoftDialog=false;
	_caughtElements=0;
	setFocusObject(FOCUS_ON_PAGE);
	setUpDefaultPages();
	removeAllPages();
	_initialValuesInitialized=false;
}

CPageContainer::~CPageContainer()
{
	removeAllPages();
}

void CPageContainer::newSceneProcedure()
{
	removeAllPages();
}

int CPageContainer::getPageCount()
{
	return(_numberOfPages);
}

void CPageContainer::initializeInitialValues(bool simulationIsRunning,int initializeOnlyForThisNewObject)
{
	_initialValuesInitialized=simulationIsRunning;
	if (simulationIsRunning)
	{
		_initialActivePageIndex=_activePageIndex;
	}
	for (int i=0;i<_numberOfPages;i++)
	{
		if (_allPages[i]!=NULL)
			_allPages[i]->initializeInitialValues(simulationIsRunning,initializeOnlyForThisNewObject);
	}
}

void CPageContainer::simulationEnded()
{
	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
	{
		_activePageIndex=_initialActivePageIndex;
	}
	for (int i=0;i<_numberOfPages;i++)
	{
		if (_allPages[i]!=NULL)
			_allPages[i]->simulationEnded();
	}
	_initialValuesInitialized=false;
}

int CPageContainer::getCaughtElements()
{
	int retVal=0;
	for (int i=0;i<_numberOfPages;i++)
	{
		if (_allPages[i]!=NULL)
			retVal|=_allPages[i]->getCaughtElements();
	}
	if (App::ct->buttonBlockContainer!=NULL)
		retVal|=App::ct->buttonBlockContainer->getCaughtElements();
	retVal|=_caughtElements;
	return(retVal);
}

void CPageContainer::clearCaughtElements(int keepMask)
{
	for (int i=0;i<_numberOfPages;i++)
	{
		if (_allPages[i]!=NULL)
			_allPages[i]->clearCaughtElements(keepMask);
	}
	if (App::ct->buttonBlockContainer!=NULL)
		App::ct->buttonBlockContainer->clearCaughtElements(keepMask);
	_caughtElements&=keepMask;
}

void CPageContainer::removeAllPages()
{
	for (int i=0;i<_numberOfPages;i++)
	{
		if (_allPages[i]!=NULL)
			delete _allPages[i];
		_allPages[i]=NULL;
	}
}
void CPageContainer::setUpDefaultPages(bool createASingleView)
{ // createASingleView is false by default
	if (_numberOfPages==0)
	{
		_numberOfPages=MAX_NUMBER_OF_PAGES;
		for (int i=0;i<_numberOfPages;i++)
			_allPages[i]=NULL;
	}
	for (int i=0;i<_numberOfPages;i++)
	{
		if (_allPages[i]!=NULL)
		{
			delete _allPages[i];
			_allPages[i]=NULL;
		}
	}
	if (createASingleView)
	{
		CSPage* it=new CSPage(SINGLE_VIEW);
		_allPages[0]=it;
	}
	_activePageIndex=0;
}

void CPageContainer::setActivePage(int pageIndex)
{
	if ( (pageIndex<_numberOfPages)&&(pageIndex>=0) )
	{
		_activePageIndex=pageIndex;
		clearAllLastMouseDownViewIndex(); // foc
		App::ct->objCont->toolbarRefreshFlag=true;
	}
}

void CPageContainer::renderCurrentPage()
{
	FUNCTION_DEBUG;
	if (App::ct->objCont->getEditModeType()!=BUTTON_EDIT_MODE)
	{
		bool validView=true;
		if ( (_activePageIndex<0)||(_activePageIndex>=_numberOfPages) )
			validView=false;
		CSPage* it=getPage(_activePageIndex);
		if (it==NULL)
			validView=false;
		
		if (validView)
			it->render();
		else
		{ // We draw a dark grey view:
			glEnable(GL_SCISSOR_TEST);	
			glViewport(_pagePosition[0],_pagePosition[1],_pageSize[0],_pageSize[1]);
			glScissor(_pagePosition[0],_pagePosition[1],_pageSize[0],_pageSize[1]);
			glClearColor(0.3f,0.3f,0.3f,1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
		}
	}


	// Now we have to clear all mouseJustWentDown and mouseJustWentUp flag in case
	// it was not processed
	for (int  i=0;i<_numberOfPages;i++)
	{
		if (_allPages[i]!=NULL)
			_allPages[i]->clearAllMouseJustWentDownAndUpFlags();
	}

	// We prepare a new modelview to draw on top:
	ogl::setColorsAllBlack();
	glDisable(GL_SCISSOR_TEST);	
	glViewport(_pagePosition[0],_pagePosition[1],_pageSize[0],_pageSize[1]);
	ogl::allLightsOffAmbientFull();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f,_pageSize[0],0.0f,_pageSize[1],-1.0f,1.0f);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glDisable(GL_DEPTH_TEST);

	if (App::ct->objCont->getEditModeType()==BUTTON_EDIT_MODE)
	{
		glEnable(GL_SCISSOR_TEST);	
		glViewport(_pagePosition[0],_pagePosition[1],_pageSize[0],_pageSize[1]);
		glScissor(_pagePosition[0],_pagePosition[1],_pageSize[0],_pageSize[1]);
		glClearColor(0.3f,0.3f,0.3f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		App::ct->globalTextureCont->startTextureDisplay(CHECKED_BACKGROUND_PICTURE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

		glTexEnvf (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f);
		glVertex3i(0,0,0);
		glTexCoord2f(_pageSize[0]/16.0f,0.0f);
		glVertex3i(_pageSize[0],0,0);
		glTexCoord2f(_pageSize[0]/16.0f,_pageSize[1]/16.0f);
		glVertex3i(_pageSize[0],_pageSize[1],0);
		glTexCoord2f(0.0f,_pageSize[1]/16.0f);
		glVertex3i(0,_pageSize[1],0);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		App::ct->globalTextureCont->endTextureDisplay();
	}
	else
	{ // Now we draw the banners:
		if (App::ct->bannerCont!=NULL)
			App::ct->bannerCont->renderAll2DBanners();
	}

	// Now we draw soft dialogs:
	if (App::ct->buttonBlockContainer!=NULL)
		App::ct->buttonBlockContainer->displayAllBlocks(_activePageIndex,focusObject==FOCUS_ON_SOFT_DIALOG);
	App::ct->infoDisplay->printInformation();


	// If you are using an EDU license for the DYNAMICS plugin, MESH CALCULATION plugin or PATH PLANNING plugin, you have to keep the V-REP tag in place!!
	if ((App::mainWindow!=NULL)&&(!App::mainWindow->simulationRecorder->getIsRecording()))
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,_pageSize[0],0,_pageSize[1],-1,1);
		glDisable(GL_SCISSOR_TEST);	
		glViewport(_pagePosition[0],_pagePosition[1],_pageSize[0],_pageSize[1]);
		glDisable(GL_DEPTH_TEST);
		int xSize=256;
		int ySize=128;
		float blend=1.0f;
		float col[12]={1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f};
		ogl::setColor(col,1,blend);
		ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		bool displayTag=true;
		// If you are using V-REP in conjunction with the DYNAMICS PLUGIN, the MESH CALCULATION PLUGIN or the PATH PLANNING PLUGIN, you have to keep the EDU tag!! See the license agreements for details.
		if (CCollDistInterface::initializeFunctionsIfNeeded()||CDynInterface::initializeFunctionsIfNeeded()||CPathPlanningInterface::initializeFunctionsIfNeeded())
			App::ct->globalTextureCont->startTextureDisplay(EDU_TAG);
		else
			App::ct->globalTextureCont->startTextureDisplay(VREP_TAG);
		if (displayTag)
		{
			glColor3f(1.0f, 1.0f, 1.0f);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f,0.0f);
			glVertex3i(0,0,0);
			glTexCoord2f(1.0f,0.0f);
			glVertex3i(xSize,0,0);
			glTexCoord2f(1.0f,1.0f);
			glVertex3i(xSize,ySize,0);
			glTexCoord2f(0.0f,1.0f);
			glVertex3i(0,ySize,0);
			glEnd();
		}
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		ogl::setBlending(false);
		glDisable(GL_ALPHA_TEST);
		glEnable(GL_DEPTH_TEST);
	}
}

CSPage* CPageContainer::getPage(int pageIndex)
{
	if ( (pageIndex<0)||(pageIndex>=_numberOfPages) )
		return(NULL); // Invalid view number
	return(_allPages[pageIndex]);	
}

int CPageContainer::getActivePageIndex()
{
	return(_activePageIndex);
}
void CPageContainer::announceObjectWillBeErased(int objectID)
{ // Never called from copy buffer!
	for (int i=0;i<_numberOfPages;i++)
	{
		if (_allPages[i]!=NULL)
			_allPages[i]->announceObjectWillBeErased(objectID);
	}
}

void CPageContainer::setPageSizeAndPosition(int sizeX,int sizeY,int posX,int posY)
{
	_pageSize[0]=sizeX;
	_pageSize[1]=sizeY;
	_pagePosition[0]=posX;
	_pagePosition[1]=posY;
	// We set the view position and size for all soft dialogs:
	if (App::ct->buttonBlockContainer!=NULL)
		App::ct->buttonBlockContainer->setViewSizeAndPosition(sizeX,sizeY,posX,posY);
	// We set the view position and size for all views:
	for (int i=0;i<_numberOfPages;i++)
	{
		if (_allPages[i]!=NULL)
			_allPages[i]->setPageSizeAndPosition(sizeX,sizeY,posX,posY);
	}
}

bool CPageContainer::getMouseRelPosObjectAndViewSize(int x,int y,int relPos[2],int& objType,int& objID,int vSize[2],bool& viewIsPerspective)
{ // NOT FULLY IMPLEMENTED! objType=-1 --> not supported, 0 --> hierarchy, 1 --> 3DViewable
	if ( (x<0)||(x>_pageSize[0])||(y<0)||(y>_pageSize[1]) )
		return(false);
	// The position is in this window zone
	if ((App::ct->buttonBlockContainer!=NULL)&&App::ct->buttonBlockContainer->mouseDownTest(x,y,_activePageIndex))
	{ // not yet supported
		objType=-1;
		return(true); 
	}
	CSPage* it=getPage(_activePageIndex);
	if (it!=NULL)
	{
		if (it->getMouseRelPosObjectAndViewSize(x,y,relPos,objType,objID,vSize,viewIsPerspective))
			return(true); // we are in this view
	}
	return(false);
}

bool CPageContainer::leftMouseButtonDown(int x,int y,int selectionStatus)
{
	if ( (x<0)||(x>_pageSize[0])||(y<0)||(y>_pageSize[1]) )
		return(false);
	// The mouse went down in this window zone
	for (int i=0;i<_numberOfPages;i++)
	{
		if (_allPages[i]!=NULL)
			_allPages[i]->clearCaughtElements(0xffff-sim_left_button);
	}
	if (App::ct->buttonBlockContainer!=NULL)
		App::ct->buttonBlockContainer->clearCaughtElements(0xffff-sim_left_button);

	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	leftMouseCaughtBy=-1;
	leftMouseCaughtSoftDialog=false;
	if ((App::ct->buttonBlockContainer!=NULL)&&App::ct->buttonBlockContainer->mouseDown(mouseRelativePosition[0],mouseRelativePosition[1],_activePageIndex,selectionStatus))
	{
		setFocusObject(FOCUS_ON_SOFT_DIALOG);
		return(true);
	}
	setFocusObject(FOCUS_ON_PAGE);
	if ((App::ct->bannerCont!=NULL)&&App::ct->bannerCont->mouseDownOn2DWindow(mouseRelativePosition[0],mouseRelativePosition[1]))
		return(true);
	CSPage* it=getPage(_activePageIndex);
	if (it!=NULL)
	{
		if (it->leftMouseButtonDown(x,y,selectionStatus))
		{ // The mouse was caught by this view:
			return(true);
		}
	}
	// Here we could handle mouse catch for the view container (not handled now)
	return(false);
}

void CPageContainer::clearAllLastMouseDownViewIndex()
{
	for (int i=0;i<_numberOfPages;i++)
	{
		CSPage* p=_allPages[i];
		if (p!=NULL)
			p->clearLastMouseDownViewIndex();
	}
}

int CPageContainer::getCursor(int x,int y)
{
	if ( (x<0)||(x>_pageSize[0])||(y<0)||(y>_pageSize[1]) )
		return(-1);
	if ((App::ct->buttonBlockContainer!=NULL)&&App::ct->buttonBlockContainer->mouseDownTest(x,y,_activePageIndex))
		return(-1);
	CSPage* it=getPage(_activePageIndex);
	if (it==NULL)
		return(-1); // The active view doesn't exist!
	return(it->getCursor(x,y));
}

void CPageContainer::leftMouseButtonUp(int x,int y)
{
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	if (App::ct->buttonBlockContainer!=NULL)
	{
		if (App::ct->buttonBlockContainer->getCaughtElements()&sim_left_button)
			App::ct->buttonBlockContainer->mouseUp(mouseRelativePosition[0],mouseRelativePosition[1],_activePageIndex);
	}

	CSPage* it=getPage(_activePageIndex);
	if ( (it!=NULL)&&(it->getCaughtElements()&sim_left_button) )
		it->leftMouseButtonUp(mouseRelativePosition[0],mouseRelativePosition[1]);
}

void CPageContainer::mouseMove(int x,int y,bool passiveAndFocused)
{
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;

	if (App::ct->buttonBlockContainer!=NULL)
	{
		if (!passiveAndFocused)
		{
			if (App::ct->buttonBlockContainer->getCaughtElements()&(sim_right_button|sim_left_button))
				App::ct->buttonBlockContainer->mouseMove(mouseRelativePosition[0],mouseRelativePosition[1]);
		}
		else
			App::ct->buttonBlockContainer->mouseMove(mouseRelativePosition[0],mouseRelativePosition[1]);
	}
	for (int i=0;i<_numberOfPages;i++)
	{
		if (_allPages[i]!=NULL)
		{
			if (!passiveAndFocused)
			{
				if (_allPages[i]->getCaughtElements()&(sim_right_button|sim_left_button))
					_allPages[i]->mouseMove(mouseRelativePosition[0],mouseRelativePosition[1],passiveAndFocused);
			}
			else
			{
				if (i==_activePageIndex)
					_allPages[i]->mouseMove(mouseRelativePosition[0],mouseRelativePosition[1],passiveAndFocused);
			}
		}
	}
}
void CPageContainer::mouseWheel(int deltaZ,int x,int y)
{
	// We sent this event only to the active view if no view was caught:
	CSPage* it=getPage(_activePageIndex);
	if (it!=NULL)
		it->mouseWheel(deltaZ,x,y);
}

void CPageContainer::looseFocus()
{
	if (App::ct->buttonBlockContainer!=NULL)
		App::ct->buttonBlockContainer->looseFocus();
	setFocusObject(FOCUS_ON_UNKNOWN_OBJECT);
}

int CPageContainer::getFocusObject()
{
	return(focusObject);
}

void CPageContainer::setFocusObject(int obj)
{
	focusObject=obj;
	if (focusObject==FOCUS_ON_PAGE)
	{
		if (App::ct->buttonBlockContainer!=NULL)
		{
			App::ct->buttonBlockContainer->setEditBoxEdition(-1,-1,false);
			App::ct->buttonBlockContainer->looseFocus();
		}
	}
}

void CPageContainer::keyPress(int key,QWidget* mainWindow)
{
	if ((focusObject==FOCUS_ON_SOFT_DIALOG)&&(App::ct->buttonBlockContainer!=NULL))
	{
		App::ct->buttonBlockContainer->onKeyDown(key);
		return;
	}

	if (key==CTRL_V_KEY)
	{
		if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
			CObjectEdition::evaluateMenuSelection(PASTE_OBJECTS_CMD);
		else if (App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)
			CTriangleVertexEdition::evaluateMenuSelection(PASTE_TRIANGLE_VERTEX_CMD);
		else if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
			CPathEdition::evaluateMenuSelection(PASTE_PATH_POINT_CMD,NULL);
		return;
	}
	if ( (key==DELETE_KEY)||(key==BACKSPACE_KEY)||(key==CTRL_X_KEY)||(key==CTRL_C_KEY) )
	{
		if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
		{
			if ((key==DELETE_KEY)||(key==BACKSPACE_KEY))
				CObjectEdition::evaluateMenuSelection(DELETE_OBJECTS_CMD);
			if (key==CTRL_X_KEY)
				CObjectEdition::evaluateMenuSelection(OBJECT_FULL_CUT_CMD);
			if (key==CTRL_C_KEY)
				CObjectEdition::evaluateMenuSelection(OBJECT_FULL_COPY_CMD);
		}
		else if (App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)
		{
			if ((key==DELETE_KEY)||(key==BACKSPACE_KEY))
				CTriangleVertexEdition::evaluateMenuSelection(DELETE_TRIANGLE_VERTEX_EDGE_CMD);
			if (key==CTRL_X_KEY)
				CTriangleVertexEdition::evaluateMenuSelection(TRIANGLE_VERTEX_CUT_CMD);
			if (key==CTRL_C_KEY)
				CTriangleVertexEdition::evaluateMenuSelection(TRIANGLE_VERTEX_COPY_CMD);
		}
		else if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
		{
			if ((key==DELETE_KEY)||(key==BACKSPACE_KEY))
				CPathEdition::evaluateMenuSelection(DELETE_PATH_POINT_CMD,NULL);
			if (key==CTRL_X_KEY)
				CPathEdition::evaluateMenuSelection(PATH_POINT_CUT_CMD,NULL);
			if (key==CTRL_C_KEY)
				CPathEdition::evaluateMenuSelection(PATH_POINT_COPY_CMD,NULL);
		}
		return;
	}

	if (App::mainWindow!=NULL)
		App::mainWindow->handleCommonKeyPressEvents(key);

	if (key==ESC_KEY)
	{
		if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
			App::ct->objCont->deselectObjects();
		else if (App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)
			App::ct->objCont->deselectEditModeBuffer();
		else if (App::ct->objCont->getEditModeType()&BUTTON_EDIT_MODE)
		{
			App::ct->objCont->deselectObjects();
			App::ct->buttonBlockContainer->deselectButtons();
		}
		return;
	}

	int flags=0;
	if (App::mainWindow!=NULL)
	{
		if (App::mainWindow->ctrlKeyDown)
			flags|=1;
		if (App::mainWindow->shiftKeyDown)
			flags|=2;
	}
	int data[4]={key,flags,0,0};
	void* retVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_keypress,data,NULL,NULL);
	delete[] (char*)retVal;
	data[0]=key;
	data[1]=flags;
	retVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_keypress,data,NULL,NULL); // for backward compatibility
	delete[] (char*)retVal;
	App::ct->outsideCommandQueue->addCommand(sim_message_keypress,key,flags,0,0,NULL,0);

	App::ct->setModificationFlag(1024); // key was pressed
}

bool CPageContainer::rightMouseButtonDown(int x,int y)
{
	if ( (x<0)||(y<0)||(x>_pageSize[0])||(y>_pageSize[1]) )
		return(false);
	// The mouse went down in this window zone
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;

	if (App::ct->buttonBlockContainer!=NULL)
		App::ct->buttonBlockContainer->clearCaughtElements(0xffff-sim_right_button);
	for (int i=0;i<_numberOfPages;i++)
	{
		if (_allPages[i]!=NULL)
			_allPages[i]->clearCaughtElements(0xffff-sim_right_button);
	}
	prepareForPopupMenu=-1;
	// First we check the soft dialogs:

	if (App::ct->buttonBlockContainer!=NULL)
	{
		// Following 2 lines should be replaced with the right button down handling routine (when it exists)! (and then return true!)
		if (App::ct->buttonBlockContainer->mouseDownTest(mouseRelativePosition[0],mouseRelativePosition[1],_activePageIndex))
			return(false);
	}

	setFocusObject(FOCUS_ON_PAGE);
	CSPage* it=getPage(_activePageIndex);
	if (it!=NULL)
	{
		if (it->rightMouseButtonDown(mouseRelativePosition[0],mouseRelativePosition[1]))
			return(true); // The active view caught that event!
	}
	else
	{ // Here we prepare info for the popup menu in the case no view exists for that index:
		_caughtElements|=sim_right_button; // we catch the right button
		prepareForPopupMenu=_activePageIndex;
		return(true); // We might display a popup menu to allow the user to create a view for that _activePageIndex
	}
	return(false); // Nothing caught that action
}

void CPageContainer::rightMouseButtonUp(int x,int y,int absX,int absY,QWidget* mainWindow)
{
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;


	if (App::ct->buttonBlockContainer!=NULL)
	{
		// ROUTINE DOES NOT YET EXIST!
	}
	CSPage* it=getPage(_activePageIndex);
	if (it!=NULL)
	{
		if (it->getCaughtElements()&sim_right_button)
		{
			if (it->rightMouseButtonUp(mouseRelativePosition[0],mouseRelativePosition[1],absX,absY,mainWindow))
				removePage(_activePageIndex);
		}
	}
	else
	{
		if (_caughtElements&sim_right_button)
		{
			if ( (prepareForPopupMenu>=0)&&(prepareForPopupMenu<_numberOfPages)&&(prepareForPopupMenu==_activePageIndex) )
			{
				// Did the mouse go up in this zone?
				if ( (x>=0)&&(y>=0)&&(x<=_pageSize[0])&&(y<=_pageSize[1]) )
				{ // Yes! We display a popup menu to allow the user to create a new view at that index:
					if (App::operationalUIParts&sim_gui_popups)
					{ // Default popups
						VMenu* mainMenu=VMenubar::createPopupMenu();
						addEmptyPageMenu(mainMenu);
						int command=VMenubar::trackPopupMenu(mainMenu,absX,absY,mainWindow);
						evaluateEmptyPageMenuSelection(command,_activePageIndex);
						delete mainMenu;
					}
				}
			}
		}
	}
	prepareForPopupMenu=-1;
}
bool CPageContainer::leftMouseButtonDoubleClick(int x,int y,int selectionStatus)
{
	if ( (x<0)||(y<0)||(x>_pageSize[0])||(y>_pageSize[1]) )
		return(false);
	// The mouse went down in this window zone
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	// Soft dialogs:
	if (App::ct->buttonBlockContainer!=NULL)
	{
		if (App::ct->buttonBlockContainer->leftMouseButtonDoubleClick(x,y,_activePageIndex))
		{
			setFocusObject(FOCUS_ON_SOFT_DIALOG);
			return(true);
		}
	}
	setFocusObject(FOCUS_ON_PAGE);
	{ // If dialog focus could be lost:
		CSPage* it=getPage(_activePageIndex);
		if (it!=NULL)
			return(it->leftMouseButtonDoubleClick(x,y,selectionStatus));
	}
	return(false); // Not processed
}

void CPageContainer::removePage(int pageIndex)
{
	if ( (pageIndex<0)||(pageIndex>=_numberOfPages) )
		return;
	if (_allPages[pageIndex]!=NULL)
	{
		delete _allPages[pageIndex];
		_allPages[pageIndex]=NULL;
	}
}

void CPageContainer::serialize(CSer& ar)
{
	if (ar.isStoring())
	{ // Storing
		for (int i=0;i<_numberOfPages;i++)
		{
			if (_allPages[i]!=NULL)
			{
				ar.storeDataName("Vwo");
				ar.setCountingMode();
				_allPages[i]->serialize(ar);
				if (ar.setWritingMode())
					_allPages[i]->serialize(ar);
			}
			else
			{
				ar.storeDataName("Nul");
				ar << (int)0;
				ar.flush();
			}
		}

		ar.storeDataName("Avi");
		ar << _activePageIndex;
		ar.flush();

		ar.storeDataName(SER_END_OF_OBJECT);
	}
	else
	{		// Loading
		removeAllPages();
		int viewCounter=0;
		int byteQuantity;
		std::string theName="";
		while (theName.compare(SER_END_OF_OBJECT)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_END_OF_OBJECT)!=0)
			{
				bool noHit=true;
				if (theName.compare("Avi")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _activePageIndex;
					App::ct->objCont->toolbarRefreshFlag=true;
				}
				if (theName.compare("Vwo")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					CSPage* theView=new CSPage(0);
					theView->serialize(ar);
					_allPages[viewCounter]=theView;
					viewCounter++;
				}
				if (theName.compare("Nul")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					int tmp;
					ar >> tmp;
					viewCounter++;
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}

void CPageContainer::performObjectLoadingMapping(std::vector<int>* map)
{
	for (int i=0;i<_numberOfPages;i++)
	{
		if (_allPages[i]!=NULL)
			_allPages[i]->performObjectLoadingMapping(map);
	}
}

void CPageContainer::addEmptyPageMenu(VMenu* menu)
{
	VMenubar::appendMenuItem(menu,true,false,CREATE_SINGLE_VIEW_TYPE_CMD,IDS_VIEW_TYPE_SINGLE_MENU_ITEM);
	VMenubar::appendMenuItem(menu,true,false,CREATE_HORIZONTAL_VIEW_TYPE_CMD,IDS_VIEW_TYPE_TWO_HORIZ_MENU_ITEM);
	VMenubar::appendMenuItem(menu,true,false,CREATE_VERTICAL_VIEW_TYPE_CMD,IDS_VIEW_TYPE_TWO_VERT_MENU_ITEM);
	VMenubar::appendMenuItem(menu,true,false,CREATE_HORIZONTAL_3_VIEW_TYPE_CMD,IDS_VIEW_TYPE_THREE_HORIZ_MENU_ITEM);
	VMenubar::appendMenuItem(menu,true,false,CREATE_VERTICAL_3_VIEW_TYPE_CMD,IDS_VIEW_TYPE_THREE_VERT_MENU_ITEM);
	VMenubar::appendMenuItem(menu,true,false,CREATE_HORIZONTAL_1_PLUS_3_VIEW_TYPE_CMD,IDS_VIEW_TYPE_HORIZONTAL_ONE_PLUS_THREE_MENU_ITEM);
	VMenubar::appendMenuItem(menu,true,false,CREATE_VERTICAL_1_PLUS_3_VIEW_TYPE_CMD,IDS_VIEW_TYPE_VERTICAL_ONE_PLUS_THREE_MENU_ITEM);
	VMenubar::appendMenuItem(menu,true,false,CREATE_HORIZONTAL_1_PLUS_4_VIEW_TYPE_CMD,IDS_VIEW_TYPE_HORIZONTAL_ONE_PLUS_FOUR_MENU_ITEM);
	VMenubar::appendMenuItem(menu,true,false,CREATE_VERTICAL_1_PLUS_4_VIEW_TYPE_CMD,IDS_VIEW_TYPE_VERTICAL_ONE_PLUS_FOUR_MENU_ITEM);
	VMenubar::appendMenuItem(menu,true,false,CREATE_FOUR_VIEW_TYPE_CMD,IDS_VIEW_TYPE_FOUR_MENU_ITEM);
	VMenubar::appendMenuItem(menu,true,false,CREATE_SIX_VIEW_TYPE_CMD,IDS_VIEW_TYPE_SIX_MENU_ITEM);
	VMenubar::appendMenuItem(menu,true,false,CREATE_EIGHT_VIEW_TYPE_CMD,IDS_VIEW_TYPE_EIGHT_MENU_ITEM);
}

bool CPageContainer::evaluateEmptyPageMenuSelection(int commandID,int viewIndex)
{ // Return value is true if the command belonged to hierarchy menu and was executed
	if ( (viewIndex<0)||(viewIndex>=_numberOfPages) )
		return(false);
	if (_allPages[viewIndex]!=NULL)
		return(false);
	CSPage* it=NULL;
	if (commandID==CREATE_SINGLE_VIEW_TYPE_CMD)
		it=new CSPage(SINGLE_VIEW);
	if (commandID==CREATE_FOUR_VIEW_TYPE_CMD)
		it=new CSPage(FOUR_VIEWS);
	if (commandID==CREATE_SIX_VIEW_TYPE_CMD)
		it=new CSPage(SIX_VIEWS);
	if (commandID==CREATE_EIGHT_VIEW_TYPE_CMD)
		it=new CSPage(EIGHT_VIEWS);
	if (commandID==CREATE_HORIZONTAL_VIEW_TYPE_CMD)
		it=new CSPage(HORIZONTALLY_DIVIDED);
	if (commandID==CREATE_VERTICAL_VIEW_TYPE_CMD)
		it=new CSPage(VERTICALLY_DIVIDED);
	if (commandID==CREATE_HORIZONTAL_3_VIEW_TYPE_CMD)
		it=new CSPage(HORIZONTALLY_DIVIDED_3);
	if (commandID==CREATE_VERTICAL_3_VIEW_TYPE_CMD)
		it=new CSPage(VERTICALLY_DIVIDED_3);
	if (commandID==CREATE_HORIZONTAL_1_PLUS_3_VIEW_TYPE_CMD)
		it=new CSPage(HORIZONTAL_1_PLUS_3_VIEWS);
	if (commandID==CREATE_VERTICAL_1_PLUS_3_VIEW_TYPE_CMD)
		it=new CSPage(VERTICAL_1_PLUS_3_VIEWS);
	if (commandID==CREATE_HORIZONTAL_1_PLUS_4_VIEW_TYPE_CMD)
		it=new CSPage(HORIZONTAL_1_PLUS_4_VIEWS);
	if (commandID==CREATE_VERTICAL_1_PLUS_4_VIEW_TYPE_CMD)
		it=new CSPage(VERTICAL_1_PLUS_4_VIEWS);
	App::uiThread->addStatusLine(IDSNS_CREATED_VIEWS);
	if (it!=NULL)
	{
		it->setPageSizeAndPosition(_pageSize[0],_pageSize[1],_pagePosition[0],_pagePosition[1]);
		_allPages[viewIndex]=it;
		App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		return(true);
	}
	return(false);
}


