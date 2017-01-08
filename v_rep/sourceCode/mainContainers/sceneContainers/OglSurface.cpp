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
#include "OglSurface.h"
#include "Tt.h"
#include "OGL.h"
#include "App.h"
#include "Simulation.h"
#include "DlgCont.h"

#include "imgLoader.h"
#include "VDateTime.h"
#include "pluginContainer.h"

enum {NO_ONE=0,HIERARCHY_PART,VIEW_SELECTION_PART,PAGE_PART,
		HIERARCHY_RESIZING_PART,HIERARCHY_CLOSING_PART,BROWSER_PART,
		BROWSER_RESIZING_PART,BROWSER_CLOSING_PART};

bool COglSurface::_hierarchyEnabled=false;
bool COglSurface::_browserEnabled=false;
int COglSurface::_hierarchyWidth=300;
int COglSurface::_browserWidth=128+2+4+HIERARCHY_SCROLLBAR_WIDTH*App::sc;

COglSurface::COglSurface()
{
	pageCont=new CPageContainer();
	viewSelector=new CViewSelector();
	pageSelector=new CPageSelector();
	sceneSelector=new CSceneSelector();
	setUpDefaultValues();
}

COglSurface::~COglSurface()
{
	delete pageCont;
	delete sceneSelector;
	delete pageSelector;
	delete viewSelector;
}

void COglSurface::newSceneProcedure()
{
	pageCont->newSceneProcedure();
	App::ct->hierarchy->setRebuildHierarchyFlag();
	viewSelector->newSceneProcedure();
	pageSelector->newSceneProcedure();
	sceneSelector->newSceneProcedure();
}

void COglSurface::initializeInitialValues(bool simulationIsRunning,int initializeOnlyForThisNewObject)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	if (pageCont!=NULL)
		pageCont->initializeInitialValues(simulationIsRunning,initializeOnlyForThisNewObject);
}

void COglSurface::simulationAboutToStart()
{
	initializeInitialValues(true,-1);
}

void COglSurface::simulationEnded()
{
//	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd())
//	{
//	}
	if (pageCont!=NULL)
		pageCont->simulationEnded();
}

void COglSurface::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

void COglSurface::setSurfaceSizeAndPosition(int sizeX,int sizeY,int posX,int posY)
{
	surfaceSize[0]=sizeX;
	surfaceSize[1]=sizeY;
	surfacePosition[0]=posX;
	surfacePosition[1]=posY;
	actualizeAllSurfacesSizeAndPosition();
}

bool COglSurface::getMouseRelPosObjectAndViewSize(int x,int y,int relPos[2],int& objType,int& objID,int vSize[2],bool& viewIsPerspective)
{ // NOT FULLY IMPLEMENTED! objType=-1 --> not supported, 0 --> hierarchy, 1 --> 3DViewable
	int offx=0;
	int offy=0;
	if (sceneSelectionActive)
		return(false);
	if (pageSelectionActive)
		return(false);
	if (viewSelectionActive)
		return(false);
	if (isBrowserEffectivelyVisible())
	{
		if ( (x>=offx)&&(x<offx+_browserWidth)&&(y>=offy)&&(y<offy+surfaceSize[1]) )
		{ // We are in the browser window. Not yet supported
			objType=-1;
			return(true); 
		}
	}
	if (isBrowserEffectivelyVisible())
		offx+=_browserWidth;
	if (_hierarchyEnabled)
	{
		VPoint btl(offx+_hierarchyWidth-BROWSER_HIERARCHY_TITLE_BAR_CLOSING_BUTTON_WIDTH*App::sc-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,surfaceSize[1]-BROWSER_HIERARCHY_TITLE_BAR_HEIGHT*App::sc);
		VPoint btr(offx+_hierarchyWidth-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,surfaceSize[1]);
		if ( (mouseRelativePosition[0]>=btl.x)&&(mouseRelativePosition[0]<=btr.x)&&
			(mouseRelativePosition[1]>=btl.y)&&(mouseRelativePosition[1]<=btr.y) )
		{ // We are on the closing button
			objType=-1;
			return(true); 
		}
		if ( (x>=offx)&&(x<offx+_hierarchyWidth)&&(y>=offy)&&(y<offy+surfaceSize[1]) )
		{
			objType=0;
			relPos[0]=x-offx;
			relPos[1]=y-offy;
			return(true); // We are in the hierarchy window
		}
	}
	if (_hierarchyEnabled)
		offx+=_hierarchyWidth;
	if (pageCont->getMouseRelPosObjectAndViewSize(x-offx,y-offy,relPos,objType,objID,vSize,viewIsPerspective))
		return(true); // We are in the views windows and have an object
	return(false);
}

bool COglSurface::leftMouseButtonDown(int x,int y,int selectionStatus)
{
	int offx=0;
	int offy=0;
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	mousePreviousRelativePosition[0]=mouseRelativePosition[0];
	mousePreviousRelativePosition[1]=mouseRelativePosition[1];
	sceneSelector->clearCaughtElements(0xffff-sim_left_button);
	pageSelector->clearCaughtElements(0xffff-sim_left_button);
	viewSelector->clearCaughtElements(0xffff-sim_left_button);
	App::ct->hierarchy->clearCaughtElements(0xffff-sim_left_button);
	App::ct->browser->clearCaughtElements(0xffff-sim_left_button);
	pageCont->clearCaughtElements(0xffff-sim_left_button);
	_caughtElements&=0xffff-sim_left_button;
	if (sceneSelectionActive)
	{ 
		sceneSelector->leftMouseButtonDown(mouseRelativePosition[0],mouseRelativePosition[1],selectionStatus);
		return(true); // We want the mouse captured!
	}
	if (pageSelectionActive)
	{ 
		pageSelector->leftMouseButtonDown(mouseRelativePosition[0],mouseRelativePosition[1],selectionStatus);
		return(true); // We want the mouse captured!
	}
	if (viewSelectionActive)
	{ 
		viewSelector->leftMouseButtonDown(mouseRelativePosition[0],mouseRelativePosition[1],selectionStatus);
		return(true); // We want the mouse captured!
	}
	if (isBrowserEffectivelyVisible())
	{
		VPoint btl(offx+_browserWidth-BROWSER_HIERARCHY_TITLE_BAR_CLOSING_BUTTON_WIDTH*App::sc-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,surfaceSize[1]-BROWSER_HIERARCHY_TITLE_BAR_HEIGHT*App::sc);
		VPoint btr(offx+_browserWidth-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,surfaceSize[1]);
		if ( (mouseRelativePosition[0]>=btl.x)&&(mouseRelativePosition[0]<=btr.x)&&
			(mouseRelativePosition[1]>=btl.y)&&(mouseRelativePosition[1]<=btr.y) )
		{ // We pressed the browser closing button:
			_caughtElements|=sim_left_button|sim_focus;
			_browserClosingButtonDown=true;
			return(true); // We want the mouse captured!
		}
		if (_browserResizingMousePosition(mouseRelativePosition[0],mouseRelativePosition[1]))
		{ // We start a resizing action of the browser view:
			_caughtElements|=sim_left_button|sim_focus;
			_browserResizingAction=true;
			return(true); // We want the mouse captured!
		}
		if ( App::ct->browser->leftMouseDown(mouseRelativePosition[0]-offx,mouseRelativePosition[1],selectionStatus) )
		{ // Mouse went down on the browser window:
			setFocusObject(FOCUS_ON_BROWSER);
			return(true); // We want the mouse captured!
		}
	}
	if (isBrowserEffectivelyVisible())
		offx+=_browserWidth;
	if (_hierarchyEnabled)
	{
		VPoint btl(offx+_hierarchyWidth-BROWSER_HIERARCHY_TITLE_BAR_CLOSING_BUTTON_WIDTH*App::sc-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,surfaceSize[1]-BROWSER_HIERARCHY_TITLE_BAR_HEIGHT*App::sc);
		VPoint btr(offx+_hierarchyWidth-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,surfaceSize[1]);
		if ( (mouseRelativePosition[0]>=btl.x)&&(mouseRelativePosition[0]<=btr.x)&&
			(mouseRelativePosition[1]>=btl.y)&&(mouseRelativePosition[1]<=btr.y) )
		{ // We pressed the hierarchy closing button:
			_caughtElements|=sim_left_button|sim_focus;
			_hierarchyClosingButtonDown=true;
			return(true); // We want the mouse captured!
		}
		if (_hierarchyResizingMousePosition(mouseRelativePosition[0],mouseRelativePosition[1]))
		{ // We start a resizing action of the hierarchy view:
			_caughtElements|=sim_left_button|sim_focus;
			_hierarchyResizingAction=true;
			return(true); // We want the mouse captured!
		}
		if ( App::ct->hierarchy->leftMouseDown(mouseRelativePosition[0]-offx,mouseRelativePosition[1],selectionStatus) )
		{ // Mouse went down on the hierarchy window:
			setFocusObject(FOCUS_ON_HIERARCHY);
			return(true); // We want the mouse captured!
		}
	}
	if (_hierarchyEnabled)
		offx+=_hierarchyWidth;
	if (pageCont->leftMouseButtonDown(mouseRelativePosition[0]-offx,mouseRelativePosition[1]-offy,selectionStatus))
	{ // Mouse went down on the views window:
		setFocusObject(pageCont->getFocusObject());
		return(true); // We want the mouse captured!
	}
	return(false); // Nothing caught that action
}

bool COglSurface::_browserResizingMousePosition(int x,int y)
{
	return( (x>_browserWidth-4)&&(x<_browserWidth+4)&&
			(y>=0)&&(y<=surfaceSize[1]) );
}

bool COglSurface::_hierarchyResizingMousePosition(int x,int y)
{
	int b=0;
	if (isBrowserEffectivelyVisible())
		b=_browserWidth;
	return( (x>b+_hierarchyWidth-4)&&(x<b+_hierarchyWidth+4)&&
			(y>=0)&&(y<=surfaceSize[1]) );
}

void COglSurface::leftMouseButtonUp(int x,int y)
{
	bool hierarchyCloseCopy=_hierarchyClosingButtonDown;
	bool browserCloseCopy=_browserClosingButtonDown;
	_hierarchyClosingButtonDown=false;
	_hierarchyResizingAction=false;
	_browserClosingButtonDown=false;
	_browserResizingAction=false;
	int offx=0;
	int offy=0;
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	mousePreviousRelativePosition[0]=mouseRelativePosition[0];
	mousePreviousRelativePosition[1]=mouseRelativePosition[1];

	if (viewSelectionActive&&(viewSelector->getCaughtElements()&sim_left_button))
		viewSelector->leftMouseButtonUp(mouseRelativePosition[0],mouseRelativePosition[1]);
	if (pageSelectionActive&&(pageSelector->getCaughtElements()&sim_left_button))
		pageSelector->leftMouseButtonUp(mouseRelativePosition[0],mouseRelativePosition[1]);
	if (sceneSelectionActive&&(sceneSelector->getCaughtElements()&sim_left_button))
		sceneSelector->leftMouseButtonUp(mouseRelativePosition[0],mouseRelativePosition[1]);
	if (isBrowserEffectivelyVisible())
	{
		if (_caughtElements&sim_left_button)
		{
			VPoint btl(offx+_browserWidth-BROWSER_HIERARCHY_TITLE_BAR_CLOSING_BUTTON_WIDTH*App::sc-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,surfaceSize[1]-BROWSER_HIERARCHY_TITLE_BAR_HEIGHT*App::sc);
			VPoint btr(offx+_browserWidth-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,surfaceSize[1]);
			if (browserCloseCopy)
			{
				if ( (mouseRelativePosition[0]>=btl.x)&&(mouseRelativePosition[0]<=btr.x)&&
					(mouseRelativePosition[1]>=btl.y)&&(mouseRelativePosition[1]<=btr.y) )
				{ // We have to close the browser!!
					setBrowserEnabled(false);
				}
			}
		}
		if (App::ct->browser->getCaughtElements()&sim_left_button)
			App::ct->browser->leftMouseUp(mouseRelativePosition[0]-offx,mouseRelativePosition[1]);
	}
	if (isBrowserEffectivelyVisible())
		offx+=_browserWidth;
	if (_hierarchyEnabled)
	{
		if (_caughtElements&sim_left_button)
		{
			VPoint btl(offx+_hierarchyWidth-BROWSER_HIERARCHY_TITLE_BAR_CLOSING_BUTTON_WIDTH*App::sc-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,surfaceSize[1]-BROWSER_HIERARCHY_TITLE_BAR_HEIGHT*App::sc);
			VPoint btr(offx+_hierarchyWidth-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,surfaceSize[1]);
			if (hierarchyCloseCopy)
			{
				if ( (mouseRelativePosition[0]>=btl.x)&&(mouseRelativePosition[0]<=btr.x)&&
					(mouseRelativePosition[1]>=btl.y)&&(mouseRelativePosition[1]<=btr.y) )
				{ // We have to close the hierarchy!!
					setHierarchyEnabled(false);
				}
			}
		}
		if (App::ct->hierarchy->getCaughtElements()&sim_left_button)
			App::ct->hierarchy->leftMouseUp(mouseRelativePosition[0]-offx,mouseRelativePosition[1]);
	}
	if (_hierarchyEnabled)
		offx+=_hierarchyWidth;
	if (pageCont->getCaughtElements()&sim_left_button)
		pageCont->leftMouseButtonUp(mouseRelativePosition[0]-offx,mouseRelativePosition[1]-offy);
}
void COglSurface::mouseWheel(int deltaZ,int x,int y)
{
	int offx=0;
	int offy=0;
	if (sceneSelectionActive)
		return;
	if (pageSelectionActive)
		return;
	if (viewSelectionActive)
		return;
	if (isBrowserEffectivelyVisible())
	{
		App::ct->browser->mouseWheel(deltaZ,x-offx,y-offy);
		offx+=_browserWidth;
	}
	if (_hierarchyEnabled)
	{
		App::ct->hierarchy->mouseWheel(deltaZ,x-offx,y-offy);
		offx+=_hierarchyWidth;
	}
#ifdef MAC_VREP
		deltaZ=-deltaZ; // on Mac the mouse wheel appears inverted for that
#endif
	pageCont->mouseWheel(deltaZ,x-offx,y-offy);
}

void COglSurface::mouseMove(int x,int y,bool passiveAndFocused)
{
	int offx=0;
	int offy=0;
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;

	if (sceneSelectionActive)
	{
		if (!passiveAndFocused)
		{
			if (sceneSelector->getCaughtElements()&(sim_right_button|sim_left_button))
				sceneSelector->mouseMove(mouseRelativePosition[0],mouseRelativePosition[1],passiveAndFocused);
		}
		else
			sceneSelector->mouseMove(mouseRelativePosition[0],mouseRelativePosition[1],passiveAndFocused);
	}
	if (pageSelectionActive)
	{
		if (!passiveAndFocused)
		{
			if (pageSelector->getCaughtElements()&(sim_right_button|sim_left_button))
				pageSelector->mouseMove(mouseRelativePosition[0],mouseRelativePosition[1],passiveAndFocused);
		}
		else
			pageSelector->mouseMove(mouseRelativePosition[0],mouseRelativePosition[1],passiveAndFocused);
	}
	if (viewSelectionActive)
	{
		if (!passiveAndFocused)
		{
			if (viewSelector->getCaughtElements()&(sim_right_button|sim_left_button))
				viewSelector->mouseMove(mouseRelativePosition[0],mouseRelativePosition[1],passiveAndFocused);
		}
		else
			viewSelector->mouseMove(mouseRelativePosition[0],mouseRelativePosition[1],passiveAndFocused);
	}
	if (isBrowserEffectivelyVisible())
	{
		VPoint btl(_browserWidth-BROWSER_HIERARCHY_TITLE_BAR_CLOSING_BUTTON_WIDTH*App::sc-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,surfaceSize[1]-BROWSER_HIERARCHY_TITLE_BAR_HEIGHT*App::sc);
		VPoint btr(_browserWidth-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,surfaceSize[1]);
		if (!passiveAndFocused)
		{
			if (_caughtElements&(sim_right_button|sim_left_button))
			{
				if (_browserClosingButtonDown)
				{
					if ( (mouseRelativePosition[0]<btl.x)||(mouseRelativePosition[0]>btr.x)||
						(mouseRelativePosition[1]<btl.y)||(mouseRelativePosition[1]>btr.y) )
						_browserClosingButtonDown=false; // We moved away from the button
				}
				if (_browserResizingAction)
				{
					_browserWidth=mouseRelativePosition[0]-offx;
					actualizeAllSurfacesSizeAndPosition();
				}

			}
			if (App::ct->browser->getCaughtElements()&(sim_right_button|sim_left_button))
				App::ct->browser->mouseMove(mouseRelativePosition[0]-offx,mouseRelativePosition[1],passiveAndFocused);
		}
		else
			App::ct->browser->mouseMove(mouseRelativePosition[0]-offx,mouseRelativePosition[1],passiveAndFocused);
	}
	if (isBrowserEffectivelyVisible())
		offx+=_browserWidth;
	if (_hierarchyEnabled)
	{
		int b=0;
		if (isBrowserEffectivelyVisible())
			b=_browserWidth;
		VPoint btl(b+_hierarchyWidth-BROWSER_HIERARCHY_TITLE_BAR_CLOSING_BUTTON_WIDTH*App::sc-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,surfaceSize[1]-BROWSER_HIERARCHY_TITLE_BAR_HEIGHT*App::sc);
		VPoint btr(b+_hierarchyWidth-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,surfaceSize[1]);
		if (!passiveAndFocused)
		{
			if (_caughtElements&(sim_right_button|sim_left_button))
			{
				if (_hierarchyClosingButtonDown)
				{
					if ( (mouseRelativePosition[0]<btl.x)||(mouseRelativePosition[0]>btr.x)||
						(mouseRelativePosition[1]<btl.y)||(mouseRelativePosition[1]>btr.y) )
						_hierarchyClosingButtonDown=false; // We moved away from the button
				}
				if (_hierarchyResizingAction)
				{
					_hierarchyWidth=mouseRelativePosition[0]-offx;
					actualizeAllSurfacesSizeAndPosition();
				}

			}
			if (App::ct->hierarchy->getCaughtElements()&(sim_right_button|sim_left_button))
				App::ct->hierarchy->mouseMove(mouseRelativePosition[0]-offx,mouseRelativePosition[1],passiveAndFocused);
		}
		else
			App::ct->hierarchy->mouseMove(mouseRelativePosition[0]-offx,mouseRelativePosition[1],passiveAndFocused);
	}
	if (_hierarchyEnabled)
		offx+=_hierarchyWidth;
	if (!passiveAndFocused)
	{
		if (pageCont->getCaughtElements()&(sim_right_button|sim_left_button))
			pageCont->mouseMove(mouseRelativePosition[0]-offx,mouseRelativePosition[1]-offy,passiveAndFocused);
	}
	else
		pageCont->mouseMove(mouseRelativePosition[0]-offx,mouseRelativePosition[1]-offy,passiveAndFocused);

	mousePreviousRelativePosition[0]=mouseRelativePosition[0];
	mousePreviousRelativePosition[1]=mouseRelativePosition[1];
}

int COglSurface::getCursor(int x,int y)
{
	int offx=0;
	int offy=0;
	if (isBrowserEffectivelyVisible())
		offx+=_browserWidth;
	if (_hierarchyEnabled)
		offx+=_hierarchyWidth;
	if (sceneSelectionActive)
		return(sceneSelector->getCursor(x,y));
	if (pageSelectionActive)
		return(pageSelector->getCursor(x,y));
	if (viewSelectionActive)
		return(viewSelector->getCursor(x,y));
	if (isBrowserEffectivelyVisible()&&_browserResizingMousePosition(x,y))
		return(sim_cursor_horizontal_directions);
	if (isBrowserEffectivelyVisible()&&App::ct->browser->getMouseIsOnVerticalResizingBar(x,y))
		return(sim_cursor_vertical_directions);
	if (_hierarchyEnabled&&_hierarchyResizingMousePosition(x,y))
		return(sim_cursor_horizontal_directions);
	return(pageCont->getCursor(x-offx,y-offy));
}

bool COglSurface::rightMouseButtonDown(int x,int y)
{
	int offx=0;
	int offy=0;
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	mousePreviousRelativePosition[0]=mouseRelativePosition[0];
	mousePreviousRelativePosition[1]=mouseRelativePosition[1];
	sceneSelector->clearCaughtElements(0xffff-sim_right_button);
	pageSelector->clearCaughtElements(0xffff-sim_right_button);
	viewSelector->clearCaughtElements(0xffff-sim_right_button);
	App::ct->browser->clearCaughtElements(0xffff-sim_right_button);
	App::ct->hierarchy->clearCaughtElements(0xffff-sim_right_button);
	pageCont->clearCaughtElements(0xffff-sim_right_button);
	if (sceneSelectionActive)
	{ // Mouse went down on scene selector
		setFocusObject(FOCUS_ON_SCENE_SELECTION_WINDOW);
		return(sceneSelector->rightMouseButtonDown(mouseRelativePosition[0],mouseRelativePosition[1]));
	}
	if (pageSelectionActive)
	{ // Mouse went down on page selector
		setFocusObject(FOCUS_ON_PAGE_SELECTION_WINDOW);
		return(pageSelector->rightMouseButtonDown(mouseRelativePosition[0],mouseRelativePosition[1]));
	}
	if (viewSelectionActive)
	{ // Mouse went down on view selector
		setFocusObject(FOCUS_ON_VIEW_SELECTION_WINDOW);
		return(viewSelector->rightMouseButtonDown(mouseRelativePosition[0],mouseRelativePosition[1]));
	}
	if ( isBrowserEffectivelyVisible()&&App::ct->browser->rightMouseDown(mouseRelativePosition[0]-offx,mouseRelativePosition[1]) )
	{ // Mouse went down on browser window
		setFocusObject(FOCUS_ON_BROWSER);
		return(true); // We want the mouse captured!
	}
	if (isBrowserEffectivelyVisible())
		offx+=_browserWidth;
	if ( _hierarchyEnabled&&App::ct->hierarchy->rightMouseDown(mouseRelativePosition[0]-offx,mouseRelativePosition[1]) )
	{ // Mouse went down on hierarchy window
		setFocusObject(FOCUS_ON_HIERARCHY);
		return(true); // We want the mouse captured!
	}
	if (_hierarchyEnabled)
		offx+=_hierarchyWidth;
	if (pageCont->rightMouseButtonDown(mouseRelativePosition[0]-offx,mouseRelativePosition[1]-offy))
	{ // Mouse went down on views window
		setFocusObject(pageCont->getFocusObject());
		return(true); // We want the mouse captured!
	}
	return(false); // Nothing caught that action
}
void COglSurface::rightMouseButtonUp(int x,int y,int absX,int absY,QWidget* mainWindow)
{
	int offx=0;
	int offy=0;
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	mousePreviousRelativePosition[0]=mouseRelativePosition[0];
	mousePreviousRelativePosition[1]=mouseRelativePosition[1];
	if (sceneSelectionActive&&(sceneSelector->getCaughtElements()&sim_right_button))
		sceneSelector->rightMouseButtonUp(mouseRelativePosition[0],mouseRelativePosition[1],absX,absY,mainWindow);
	if (pageSelectionActive&&(pageSelector->getCaughtElements()&sim_right_button))
		pageSelector->rightMouseButtonUp(mouseRelativePosition[0],mouseRelativePosition[1],absX,absY,mainWindow);
	if (viewSelectionActive&&(viewSelector->getCaughtElements()&sim_right_button))
		viewSelector->rightMouseButtonUp(mouseRelativePosition[0],mouseRelativePosition[1],absX,absY,mainWindow);
	if (isBrowserEffectivelyVisible()&&(App::ct->browser->getCaughtElements()&sim_right_button))
		App::ct->browser->rightMouseUp(mouseRelativePosition[0]-offx,mouseRelativePosition[1],absX,absY,mainWindow);
	if (isBrowserEffectivelyVisible())
		offx+=_browserWidth;
	if (_hierarchyEnabled&&(App::ct->hierarchy->getCaughtElements()&sim_right_button))
		App::ct->hierarchy->rightMouseUp(mouseRelativePosition[0]-offx,mouseRelativePosition[1],absX,absY,mainWindow);
	if (_hierarchyEnabled)
		offx+=_hierarchyWidth;
	if (pageCont->getCaughtElements()&sim_right_button)
		pageCont->rightMouseButtonUp(mouseRelativePosition[0]-offx,mouseRelativePosition[1]-offy,absX,absY,mainWindow);
	sceneSelector->clearCaughtElements(0xffff-sim_right_button);
	pageSelector->clearCaughtElements(0xffff-sim_right_button);
	viewSelector->clearCaughtElements(0xffff-sim_right_button);
	App::ct->hierarchy->clearCaughtElements(0xffff-sim_right_button);
	pageCont->clearCaughtElements(0xffff-sim_right_button);
}

bool COglSurface::leftMouseButtonDoubleClick(int x,int y,int selectionStatus)
{
	int offx=0;
	int offy=0;
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	mousePreviousRelativePosition[0]=mouseRelativePosition[0];
	mousePreviousRelativePosition[1]=mouseRelativePosition[1];
	if (sceneSelectionActive)
	{
		setFocusObject(FOCUS_ON_SCENE_SELECTION_WINDOW);
		return(sceneSelector->leftMouseButtonDoubleClick(mouseRelativePosition[0],mouseRelativePosition[1],selectionStatus));
	}
	if (pageSelectionActive)
	{
		setFocusObject(FOCUS_ON_PAGE_SELECTION_WINDOW);
		return(pageSelector->leftMouseButtonDoubleClick(mouseRelativePosition[0],mouseRelativePosition[1],selectionStatus));
	}
	if (viewSelectionActive)
	{
		setFocusObject(FOCUS_ON_VIEW_SELECTION_WINDOW);
		return(viewSelector->leftMouseButtonDoubleClick(mouseRelativePosition[0],mouseRelativePosition[1],selectionStatus));
	}
	if ( isBrowserEffectivelyVisible()&&(App::ct->browser->leftMouseDblClick(mouseRelativePosition[0]-offx,mouseRelativePosition[1],selectionStatus)))
	{
		setFocusObject(FOCUS_ON_BROWSER);
		return(true); // We inform that this action was processed
	}
	if (isBrowserEffectivelyVisible())
		offx+=_browserWidth;
	if ( _hierarchyEnabled&&(App::ct->hierarchy->leftMouseDblClick(mouseRelativePosition[0]-offx,mouseRelativePosition[1],selectionStatus)))
	{
		setFocusObject(FOCUS_ON_HIERARCHY);
		return(true); // We inform that this action was processed
	}
	if (_hierarchyEnabled)
		offx+=_hierarchyWidth;
	if (pageCont->leftMouseButtonDoubleClick(mouseRelativePosition[0]-offx,mouseRelativePosition[1]-offy,selectionStatus))
	{
		setFocusObject(pageCont->getFocusObject());
		return(true); // We inform that this action was processed
	}
	return(false);	// Nothing caught that action
}

bool COglSurface::isSceneSelectionActive()
{
	return(sceneSelectionActive);
}

bool COglSurface::isPageSelectionActive()
{
	return(pageSelectionActive);
}

bool COglSurface::isViewSelectionActive()
{
	return(viewSelectionActive);
}

void COglSurface::render(int currentCursor,int mouseButtonState,int mousePos[2])
{
	FUNCTION_DEBUG;
	if (pageSelectionActive)
		pageSelector->render();
	else if (sceneSelectionActive)
		sceneSelector->render();
	else if (viewSelectionActive)
		viewSelector->render();
	else
	{
		// We render from right to left for the hierarchy and object browser (since both will render larger on their left)
		bool hierarchyWasRendered=false;
		if (_hierarchyEnabled)
		{
			int b=0;
			if (isBrowserEffectivelyVisible())
				b=_browserWidth;
			hierarchyWasRendered=App::ct->hierarchy->render();

			// We now display the hierarchy title:
			ogl::allLightsOffAmbientFull();
			ogl::setColorsAllBlack();
			glViewport(surfacePosition[0],surfacePosition[1],surfaceSize[0],surfaceSize[1]);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0.0f,surfaceSize[0],0.0f,surfaceSize[1],-100.0f,100.0f);
			glMatrixMode (GL_MODELVIEW);
			glLoadIdentity();
			glDisable(GL_DEPTH_TEST);

			std::string hierarchyTitle("  Scene hierarchy");
			int t=App::ct->objCont->getEditModeType();
			if (t&SHAPE_EDIT_MODE)
			{
				std::string objName("ERROR");
				CShape* theShape=App::ct->objCont->getShape(App::ct->objCont->getEditModeObjectID());
				if (theShape!=NULL)
					objName=theShape->getName();
				if (t&VERTEX_EDIT_MODE)
					hierarchyTitle="  Vertices (";
				if (t&TRIANGLE_EDIT_MODE)
					hierarchyTitle="  Triangles (";
				if (t&EDGE_EDIT_MODE)
					hierarchyTitle="  Edges (";
				hierarchyTitle+=objName+")";
			}
			if (t&PATH_EDIT_MODE)
			{
				std::string objName("  ERROR");
				CPath* thePath=App::ct->objCont->getPath(App::ct->objCont->getEditModeObjectID());
				if (thePath!=NULL)
					objName=thePath->getName();
				hierarchyTitle="  Control points (";
				hierarchyTitle+=objName+")";
			}
			if (t&BUTTON_EDIT_MODE)
				hierarchyTitle="  Custom user interfaces";

			float txtCol[3]={0.2f,0.2f,0.2f};
			float* bkgrndCol=ogl::TITLE_BAR_COLOR;
			VPoint size(_hierarchyWidth-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,BROWSER_HIERARCHY_TITLE_BAR_HEIGHT*App::sc);
			VPoint pos(b+size.x/2,surfaceSize[1]-size.y/2);
			int buttonAttrib=sim_buttonproperty_label|sim_buttonproperty_enabled|sim_buttonproperty_verticallycentered;
			ogl::drawButton(pos,size,txtCol,bkgrndCol,bkgrndCol,hierarchyTitle,buttonAttrib,false,0,0.0f,false,0,NULL,NULL,NULL,NULL,NULL);
			pos.x=b+_hierarchyWidth+(-BROWSER_HIERARCHY_TITLE_BAR_CLOSING_BUTTON_WIDTH/2)*App::sc-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH;
			size.x=BROWSER_HIERARCHY_TITLE_BAR_CLOSING_BUTTON_WIDTH*App::sc;
			float* bkgrndCol2=ogl::TITLE_BAR_BUTTON_COLOR;
			buttonAttrib=sim_buttonproperty_button|sim_buttonproperty_enabled|sim_buttonproperty_horizontallycentered|sim_buttonproperty_verticallycentered;
			if (_hierarchyClosingButtonDown)
				buttonAttrib|=sim_buttonproperty_isdown;
			ogl::drawButton(pos,size,txtCol,bkgrndCol2,bkgrndCol2,"&&Check",buttonAttrib,false,0,0.0f,false,0,NULL,NULL,NULL,NULL,NULL);

			ogl::setColor(ogl::SEPARATION_LINE_COLOR,AMBIENT_MODE);
			glLineWidth(1.0f);
			glBegin(GL_LINES);
			glVertex3i(b,surfaceSize[1]+(-BROWSER_HIERARCHY_TITLE_BAR_HEIGHT-1)*App::sc,0);
			glVertex3i(b+_hierarchyWidth-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,surfaceSize[1]+(-BROWSER_HIERARCHY_TITLE_BAR_HEIGHT-1)*App::sc,0);
			glEnd();
			glLineWidth(1.0f);

			glEnable(GL_DEPTH_TEST);
		}
		if (isBrowserEffectivelyVisible())
		{
			if (hierarchyWasRendered)
				App::ct->browser->setRefreshViewFlag();
			App::ct->browser->render();
			// We now display the browser title:
			ogl::allLightsOffAmbientFull();
			ogl::setColorsAllBlack();
			glViewport(surfacePosition[0],surfacePosition[1],surfaceSize[0],surfaceSize[1]);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0.0f,surfaceSize[0],0.0f,surfaceSize[1],-100.0f,100.0f);
			glMatrixMode (GL_MODELVIEW);
			glLoadIdentity();
			glDisable(GL_DEPTH_TEST);

			std::string browserTitle("  Model browser");
			float txtCol[3]={0.2f,0.2f,0.2f};
			float* bkgrndCol=ogl::TITLE_BAR_COLOR;
			VPoint size(_browserWidth-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,BROWSER_HIERARCHY_TITLE_BAR_HEIGHT*App::sc);
			VPoint pos(size.x/2,surfaceSize[1]-size.y/2);
			int buttonAttrib=sim_buttonproperty_label|sim_buttonproperty_enabled|sim_buttonproperty_verticallycentered;
			ogl::drawButton(pos,size,txtCol,bkgrndCol,bkgrndCol,browserTitle,buttonAttrib,false,0,0.0f,false,0,NULL,NULL,NULL,NULL,NULL);
			pos.x=_browserWidth+(-BROWSER_HIERARCHY_TITLE_BAR_CLOSING_BUTTON_WIDTH/2)*App::sc-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH;
			size.x=BROWSER_HIERARCHY_TITLE_BAR_CLOSING_BUTTON_WIDTH*App::sc;
			float* bkgrndCol2=ogl::TITLE_BAR_BUTTON_COLOR;
			buttonAttrib=sim_buttonproperty_button|sim_buttonproperty_enabled|sim_buttonproperty_horizontallycentered|sim_buttonproperty_verticallycentered;
			if (_browserClosingButtonDown)
				buttonAttrib|=sim_buttonproperty_isdown;
			ogl::drawButton(pos,size,txtCol,bkgrndCol2,bkgrndCol2,"&&Check",buttonAttrib,false,0,0.0f,false,0,NULL,NULL,NULL,NULL,NULL);

			ogl::setColor(ogl::SEPARATION_LINE_COLOR,AMBIENT_MODE);
			glLineWidth(1.0f);
			glBegin(GL_LINES);
			glVertex3i(0+0,surfaceSize[1]+(-BROWSER_HIERARCHY_TITLE_BAR_HEIGHT-1)*App::sc,0);
			glVertex3i(0+_browserWidth-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH,surfaceSize[1]+(-BROWSER_HIERARCHY_TITLE_BAR_HEIGHT-1)*App::sc,0);
			glEnd();
			glLineWidth(1.0f);

			glEnable(GL_DEPTH_TEST);
		}
		pageCont->renderCurrentPage();
		// We now have to draw separations between the different parts:
		if (isBrowserEffectivelyVisible())
		{
			ogl::allLightsOffAmbientFull();
			ogl::setColorsAllBlack();
			glViewport(surfacePosition[0],surfacePosition[1],surfaceSize[0],surfaceSize[1]);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0.0f,surfaceSize[0],0.0f,surfaceSize[1],-100.0f,100.0f);
			glDisable(GL_DEPTH_TEST);
			ogl::setColor(ogl::SEPARATION_LINE_COLOR,AMBIENT_MODE);
			glLineWidth(2.0f);
			glBegin(GL_LINES);
			glVertex3i(0+_browserWidth-1,0,0);
			glVertex3i(0+_browserWidth-1,surfaceSize[1],0);
			glEnd();
			glLineWidth(1.0f);
			glEnable(GL_DEPTH_TEST);
		}
		if (_hierarchyEnabled)
		{
			int b=0;
			if (isBrowserEffectivelyVisible())
				b=_browserWidth;
			ogl::allLightsOffAmbientFull();
			ogl::setColorsAllBlack();
			glViewport(surfacePosition[0],surfacePosition[1],surfaceSize[0],surfaceSize[1]);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0.0f,surfaceSize[0],0.0f,surfaceSize[1],-100.0f,100.0f);
			glDisable(GL_DEPTH_TEST);
			ogl::setColor(ogl::SEPARATION_LINE_COLOR,AMBIENT_MODE);
			glLineWidth(2.0f);
			glBegin(GL_LINES);
			glVertex3i(b+_hierarchyWidth-1,0,0);
			glVertex3i(b+_hierarchyWidth-1,surfaceSize[1],0);
			glEnd();
			glLineWidth(1.0f);
			glEnable(GL_DEPTH_TEST);
		}
	}

	if (_modelThumbnailIndexForDragging!=-1)
	{
		if (App::ct->modelDirectoryRoot->_currentFolderThumbnails.getValidFileformatFromIndex(_modelThumbnailIndexForDragging))
		{
			CThumbnail* thumb=App::ct->modelDirectoryRoot->_currentFolderThumbnails.getThumbnailFromIndex(_modelThumbnailIndexForDragging);
			char* img=thumb->getPointerToUncompressedImage();
			int posX=mousePos[0]+_modelThumbnailDraggingOffset[0];
			int posY=mousePos[1]+_modelThumbnailDraggingOffset[1];
			_modelThumbnailDraggingLastMousePos[0]=mousePos[0];
			_modelThumbnailDraggingLastMousePos[1]=mousePos[1];

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0,surfaceSize[0],0,surfaceSize[1],-1,1);
			glDisable(GL_SCISSOR_TEST);	
			glViewport(surfacePosition[0],surfacePosition[1],surfaceSize[0],surfaceSize[1]);
			glDisable(GL_DEPTH_TEST);
			
			ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // We turn blending on!

			glDisable(GL_LIGHTING);

			if (img==NULL)
			{
				int dummyInt;
				if (getModelThumbnailDraggingInfo(dummyInt,NULL))
					glColor3f(1.0f,1.0f,1.0f);
				else
					glColor3f(0.0f,0.0f,0.0f);
				glBegin(GL_QUADS);
				glVertex3i(posX,posY,0);
				glVertex3i(posX+128,posY,0);
				glVertex3i(posX+128,posY-128,0);
				glVertex3i(posX,posY-128,0);
				glEnd();

				glColor3f(0.5f,0.0f,0.0f);
				int textWidth=ogl::getTextLengthInPixels("Thumbnail");
				ogl::drawText(posX+64-textWidth/2,posY-64+ogl::getFontHeight()/2,0,"Thumbnail",true);
				textWidth=ogl::getTextLengthInPixels("not available");
				ogl::drawText(posX+64-textWidth/2,posY-64-ogl::getFontHeight()/2,0,"not available",true);
			}
			else
			{
				unsigned int textureName;
				glGenTextures(1,&textureName);
				glBindTexture(GL_TEXTURE_2D,textureName);
				glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,128,128,0,GL_RGBA,GL_UNSIGNED_BYTE,img);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

				glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP); // GL_REPEAT
				glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP); // GL_REPEAT
				glTexEnvf (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
				glEnable(GL_TEXTURE_2D);

				int dummyInt;
				if (getModelThumbnailDraggingInfo(dummyInt,NULL))
					glColor4f(1.0f, 1.0f, 1.0f, 1.0f);//0.125f); // fading
				else
					glColor4f(0.0f, 0.0f, 0.0f, 1.0f);//0.125f); // fading

				glBegin(GL_QUADS);
				glTexCoord2f(0.0f,1.0f);
				glVertex3i(posX,posY,0);
				glTexCoord2f(1.0f,1.0f);
				glVertex3i(posX+128,posY,0);
				glTexCoord2f(1.0f,0.0f);
				glVertex3i(posX+128,posY-128,0);
				glTexCoord2f(0.0f,0.0f);
				glVertex3i(posX,posY-128,0);
				glEnd();

				glDisable(GL_TEXTURE_2D);
				ogl::setBlending(false);
				glDisable(GL_ALPHA_TEST);
				glDisable(GL_TEXTURE_2D);
				glDeleteTextures(1,&textureName);
			}

			glEnable(GL_LIGHTING);

			glEnable(GL_DEPTH_TEST);
			App::ct->browser->setRefreshViewFlag();
			App::ct->hierarchy->setRefreshViewFlag();
		}
	}

	if ((App::mainWindow!=NULL)&&App::mainWindow->simulationRecorder->getIsRecording()&&App::mainWindow->simulationRecorder->getShowCursor())
	{ // OpenGL cursor:
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,surfaceSize[0],0,surfaceSize[1],-1,1);
		glDisable(GL_SCISSOR_TEST);	
		glViewport(surfacePosition[0],surfacePosition[1],surfaceSize[0],surfaceSize[1]);
		glDisable(GL_DEPTH_TEST);
		int pc[2]={1,-1};

		if (currentCursor==sim_cursor_arrow)
		{
			App::ct->globalTextureCont->startTextureDisplay(CURSOR_ARROW);
			pc[0]=0;
			pc[1]=-16;
		}
		if (currentCursor==sim_cursor_finger)
		{
			App::ct->globalTextureCont->startTextureDisplay(CURSOR_FINGER);
			pc[0]=1;
			pc[1]=-15;
		}
		if (currentCursor==sim_cursor_all_directions)
			App::ct->globalTextureCont->startTextureDisplay(CURSOR_ALL_DIR);
		if (currentCursor==sim_cursor_horizontal_directions)
			App::ct->globalTextureCont->startTextureDisplay(CURSOR_HORIZONTAL_DIR);
		if (currentCursor==sim_cursor_vertical_directions)
			App::ct->globalTextureCont->startTextureDisplay(CURSOR_VERTICAL_DIR);
		if (currentCursor==sim_cursor_slash_directions)
			App::ct->globalTextureCont->startTextureDisplay(CURSOR_SLASH_DIR);
		if (currentCursor==sim_cursor_backslash_directions)
			App::ct->globalTextureCont->startTextureDisplay(CURSOR_BACKSLASH_DIR);

		// Added following on 2011/01/26 to remove the annoying green borders that appear on some graphic cards:
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

		glColor3f(1.0f, 1.0f, 1.0f);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER,0.0f);
		glDisable(GL_LIGHTING);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f);
		glVertex3i(mousePos[0]+pc[0]-16,mousePos[1]+pc[1]-16,0);
		glTexCoord2f(1.0f,0.0f);
		glVertex3i(mousePos[0]+pc[0]+16,mousePos[1]+pc[1]-16,0);
		glTexCoord2f(1.0f,1.0f);
		glVertex3i(mousePos[0]+pc[0]+16,mousePos[1]+pc[1]+16,0);
		glTexCoord2f(0.0f,1.0f);
		glVertex3i(mousePos[0]+pc[0]-16,mousePos[1]+pc[1]+16,0);
		glEnd();
		App::ct->globalTextureCont->endTextureDisplay();

		if ((App::mainWindow!=NULL)&&App::mainWindow->simulationRecorder->getShowButtonStates())
		{		
			if (mouseButtonState&0x0f)
			{
				if (mouseButtonState&8)
					App::ct->globalTextureCont->startTextureDisplay(CURSOR_MIDDLE_BUTTON);
				else
				{
					if (mouseButtonState&4)
						App::ct->globalTextureCont->startTextureDisplay(CURSOR_RIGHT_BUTTON);
					else
					{
						if (mouseButtonState&1)
							App::ct->globalTextureCont->startTextureDisplay(CURSOR_LEFT_BUTTON);
						else
						{
							if (mouseButtonState&2)
								App::ct->globalTextureCont->startTextureDisplay(CURSOR_WHEEL_BUTTON);
						}
					}
				}

				// Added following on 2011/01/26 to remove the annoying green borders that appear on some graphic cards:
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

				int off[2]={+24,-16};
				glBegin(GL_QUADS);
				glTexCoord2f(0.0f,0.0f);
				glVertex3i(mousePos[0]-16+off[0],mousePos[1]-16+off[1],0);
				glTexCoord2f(1.0f,0.0f);
				glVertex3i(mousePos[0]+16+off[0],mousePos[1]-16+off[1],0);
				glTexCoord2f(1.0f,1.0f);
				glVertex3i(mousePos[0]+16+off[0],mousePos[1]+16+off[1],0);
				glTexCoord2f(0.0f,1.0f);
				glVertex3i(mousePos[0]-16+off[0],mousePos[1]+16+off[1],0);
				glEnd();
				App::ct->globalTextureCont->endTextureDisplay();

				if ((App::mainWindow!=NULL)&&App::mainWindow->shiftKeyDown)
				{
					App::ct->globalTextureCont->startTextureDisplay(CURSOR_SHIFT_BUTTON);

					// Added following on 2011/01/26 to remove the annoying green borders that appear on some graphic cards:
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

					int off[2]={+24+24,-16};
					glBegin(GL_QUADS);
					glTexCoord2f(0.0f,0.0f);
					glVertex3i(mousePos[0]-16+off[0],mousePos[1]-16+off[1],0);
					glTexCoord2f(1.0f,0.0f);
					glVertex3i(mousePos[0]+16+off[0],mousePos[1]-16+off[1],0);
					glTexCoord2f(1.0f,1.0f);
					glVertex3i(mousePos[0]+16+off[0],mousePos[1]+16+off[1],0);
					glTexCoord2f(0.0f,1.0f);
					glVertex3i(mousePos[0]-16+off[0],mousePos[1]+16+off[1],0);
					glEnd();
					App::ct->globalTextureCont->endTextureDisplay();
				}
				if ((App::mainWindow!=NULL)&&App::mainWindow->ctrlKeyDown)
				{
					App::ct->globalTextureCont->startTextureDisplay(CURSOR_CTRL_BUTTON);

					// Added following on 2011/01/26 to remove the annoying green borders that appear on some graphic cards:
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

					int off[2]={+24+24,-16};
					glBegin(GL_QUADS);
					glTexCoord2f(0.0f,0.0f);
					glVertex3i(mousePos[0]-16+off[0],mousePos[1]-16+off[1],0);
					glTexCoord2f(1.0f,0.0f);
					glVertex3i(mousePos[0]+16+off[0],mousePos[1]-16+off[1],0);
					glTexCoord2f(1.0f,1.0f);
					glVertex3i(mousePos[0]+16+off[0],mousePos[1]+16+off[1],0);
					glTexCoord2f(0.0f,1.0f);
					glVertex3i(mousePos[0]-16+off[0],mousePos[1]+16+off[1],0);
					glEnd();
					App::ct->globalTextureCont->endTextureDisplay();
				}
			}
		}
		glEnable(GL_LIGHTING);
		ogl::setBlending(false);
		glDisable(GL_ALPHA_TEST);

		glEnable(GL_DEPTH_TEST);
		if ( (isBrowserEffectivelyVisible())&&(mousePos[0]<=_browserWidth+6) )
			App::ct->browser->setRefreshViewFlag();
		int b=0;
		if (isBrowserEffectivelyVisible())
			b=_browserWidth;
		if ( (_hierarchyEnabled)&&(mousePos[0]<=b+_hierarchyWidth+6)&&(mousePos[0]>=b-36) ) // 36 because the cursor can be very wide (when the mouse button states are displayed)
			App::ct->hierarchy->setRefreshViewFlag();
	}

//*************************************************
// Following allows to hand the full rendered frame to a plugin, that might modify it and return it!
//*************************************************
	static int prevResX=-1;
	static int prevResY=-1;
	static char* buff=NULL;
	if (CPluginContainer::shouldSend_openglframe_msg())
	{
		if ((prevResX!=surfaceSize[0])||(prevResY!=surfaceSize[1]))
		{
			delete[] buff;
			buff=new char[surfaceSize[0]*surfaceSize[1]*3];
		}
		glReadPixels(0, 0, surfaceSize[0], surfaceSize[1], GL_RGB, GL_UNSIGNED_BYTE, buff);

		int auxVals[4];
		int retVals[4];
		auxVals[0]=surfaceSize[0];
		auxVals[1]=surfaceSize[1];
		auxVals[2]=0;
		auxVals[3]=0;
		CPluginContainer::sendSpecialEventCallbackMessageToSomePlugins(sim_message_eventcallback_openglframe,auxVals,buff,retVals);
		if (auxVals[3]!=0)
		{ // we want to apply a new image!
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0,surfaceSize[0],0,surfaceSize[1],-100,100);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity ();
			glDisable(GL_DEPTH_TEST);
			ogl::setColor(1.0f,1.0f,1.0f,AMBIENT_MODE);

			GLuint _oglTextureName;
			glGenTextures(1,&_oglTextureName);
			glBindTexture(GL_TEXTURE_2D,_oglTextureName);
			glPixelStorei(GL_UNPACK_ALIGNMENT,1);
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,surfaceSize[0],surfaceSize[1],0,GL_RGB,GL_UNSIGNED_BYTE,buff);
			glPixelStorei(GL_UNPACK_ALIGNMENT,4); // important to restore!
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // keep to GL_LINEAR here!!
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
			glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
			glTexEnvi (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,_oglTextureName);

			glColor3f(1.0f,1.0f,1.0f);

			glBegin(GL_QUADS);
			glTexCoord2f(0.0f,0.0f);
			glVertex3i(0,0,0);
			glTexCoord2f(0.0f,1.0f);
			glVertex3i(0,surfaceSize[1],0);
			glTexCoord2f(1.0f,1.0f);
			glVertex3i(surfaceSize[0],surfaceSize[1],0);
			glTexCoord2f(1.0f,0.0f);
			glVertex3i(surfaceSize[0],0,0);
			glEnd();

			glDisable(GL_TEXTURE_2D);
			glDeleteTextures(1,&_oglTextureName);
			glEnable(GL_DEPTH_TEST);
		}
	}
	else
	{
		delete[] buff;
		buff=NULL;
		prevResX=-1;
	}
//*************************************************

}

void COglSurface::setUpDefaultValues()
{
	_browserClosingButtonDown=false;
	_browserResizingAction=false;
	_hierarchyClosingButtonDown=false;
	_hierarchyResizingAction=false;
	surfaceSize[0]=2000;
	surfaceSize[1]=2000;
	surfacePosition[0]=0;
	surfacePosition[1]=0;
	viewSelectionActive=false;
	pageSelectionActive=false;
	sceneSelectionActive=false;
	focusObject=FOCUS_ON_PAGE;
	App::ct->browser->setUpDefaultValues();
	App::ct->hierarchy->setUpDefaultValues();
	pageCont->setUpDefaultPages();
	_caughtElements=0;
	_modelThumbnailIndexForDragging=-1;

	actualizeAllSurfacesSizeAndPosition();	// Important because the newly created views
											// don't have their position/size set!
	if (App::ct->objCont!=NULL)
		App::ct->objCont->toolbarRefreshFlag=true;
}

void COglSurface::setModelThumbnailDraggingInfo(int thumbnailIndex,int offsetX,int offsetY)
{
	_modelThumbnailIndexForDragging=thumbnailIndex;
	_modelThumbnailDraggingOffset[0]=offsetX;
	_modelThumbnailDraggingOffset[1]=offsetY;
}

bool COglSurface::getModelThumbnailDraggingInfo(int& thumbnailIndex,C3Vector* desiredModelPosition)
{ // return value true means we are in a droppable zone! desiredModelPosition can be NULL
	thumbnailIndex=_modelThumbnailIndexForDragging;
	if (_modelThumbnailIndexForDragging<0)
		return(false);

	int relPos[2];
	int objType,objID;
	int viewSize[2];
	bool viewIsPerspective;
	if (getMouseRelPosObjectAndViewSize(_modelThumbnailDraggingLastMousePos[0],_modelThumbnailDraggingLastMousePos[1],relPos,objType,objID,viewSize,viewIsPerspective))
	{
		if (objType==0)
		{
			if (desiredModelPosition!=NULL)
				desiredModelPosition->clear(); // at the origin
			return(true);
		}
		if (objType==1)
		{
			C3DObject* obj=App::ct->objCont->getObject(objID);
			if (obj!=NULL)
			{
				if (obj->getObjectType()==sim_object_camera_type)
				{
					CCamera* thecam=(CCamera*)obj;
					float pos[2]={float(relPos[0]),float(relPos[1])};
					float screenHalfSizes[2]={float(viewSize[0])/2.0f,float(viewSize[1])/2.0f};
					float halfSizes[2];
					float ratio=screenHalfSizes[0]/screenHalfSizes[1];
					if (viewIsPerspective)
					{
						if (ratio>1.0f)
						{
							float a=2.0f*(float)atan(tan(thecam->getViewAngle()/2.0f)/ratio);
							halfSizes[0]=thecam->getViewAngle()/2.0f;
							halfSizes[1]=a/2.0f;
						}
						else
						{
							float a=2.0f*(float)atan(tan(thecam->getViewAngle()/2.0f)*ratio);
							halfSizes[0]=a/2.0f;
							halfSizes[1]=thecam->getViewAngle()/2.0f;
						}
						pos[0]-=screenHalfSizes[0];
						pos[1]-=screenHalfSizes[1];
						pos[0]=atan((pos[0]/screenHalfSizes[0])*tan(halfSizes[0]));
						pos[1]=atan((pos[1]/screenHalfSizes[1])*tan(halfSizes[1]));
						screenHalfSizes[0]=halfSizes[0];
						screenHalfSizes[1]=halfSizes[1];
					}
					else
					{
						if (ratio>1.0f)
						{
							halfSizes[0]=thecam->getOrthoViewSize()*0.5f;
							halfSizes[1]=thecam->getOrthoViewSize()*0.5f/ratio;
						}
						else
						{
							halfSizes[1]=thecam->getOrthoViewSize()*0.5f;
							halfSizes[0]=thecam->getOrthoViewSize()*0.5f*ratio;
						}
						pos[0]-=screenHalfSizes[0];
						pos[1]-=screenHalfSizes[1];
					}

					C4X4Matrix objAbs;
					objAbs.setIdentity();

					C4X4Matrix originalPlane(objAbs); // x-y plane
					C4X4Matrix plane(originalPlane);
					C3Vector p; // point on the plane
					float d=-(plane.X*plane.M.axis[2]);
					float screenP[2]={pos[0],pos[1]};
					C4X4Matrix cam(thecam->getCumulativeTransformationPart1().getMatrix());
					bool singularityProblem=false;

					C3Vector pp(cam.X);
					if (!viewIsPerspective)
					{
						if (fabs(cam.M.axis[2]*plane.M.axis[2])<0.005f)
							singularityProblem=true;
						pp-=cam.M.axis[0]*halfSizes[0]*(screenP[0]/screenHalfSizes[0]);
						pp+=cam.M.axis[1]*halfSizes[1]*(screenP[1]/screenHalfSizes[1]);
						float t=(-d-(plane.M.axis[2]*pp))/(cam.M.axis[2]*plane.M.axis[2]);
						p=pp+cam.M.axis[2]*t;
					}
					else
					{
						C3Vector v(cam.M.axis[2]+cam.M.axis[0]*tan(-screenP[0])+cam.M.axis[1]*tan(screenP[1]));
						v.normalize();
						pp+=v;
						if (fabs(v*plane.M.axis[2])<0.005f)
							singularityProblem=true;
						float t=(-d-(plane.M.axis[2]*pp))/(v*plane.M.axis[2]);
						p=pp+v*t;
					}
					if (!singularityProblem)
					{
						C3Vector pRel(cam.getInverse()*p);
						singularityProblem|=(pRel(2)<thecam->getNearClippingPlane());
						singularityProblem|=(pRel(2)>thecam->getFarClippingPlane());
					}
					if (singularityProblem)
						return(false);
					if (desiredModelPosition!=NULL)
						desiredModelPosition[0]=p;
					return(true); // Ok we can drop the model here!
				}
			}
		}
	}
	return(false);
}

int COglSurface::getCaughtElements()
{
	int retVal=0;
	if (sceneSelectionActive)
		retVal|=sceneSelector->getCaughtElements();
	if (pageSelectionActive)
		retVal|=pageSelector->getCaughtElements();
	if (viewSelectionActive)
		retVal|=viewSelector->getCaughtElements();
	if (isBrowserEffectivelyVisible())
		retVal|=App::ct->browser->getCaughtElements();
	if (_hierarchyEnabled)
		retVal|=App::ct->hierarchy->getCaughtElements();
	retVal|=pageCont->getCaughtElements();
	retVal|=_caughtElements;
	return(retVal);
}

void COglSurface::clearCaughtElements(int keepMask)
{
	sceneSelector->clearCaughtElements(keepMask);
	pageSelector->clearCaughtElements(keepMask);
	viewSelector->clearCaughtElements(keepMask);
	App::ct->browser->clearCaughtElements(keepMask);
	App::ct->hierarchy->clearCaughtElements(keepMask);
	pageCont->clearCaughtElements(keepMask);
	_caughtElements&=keepMask;
}

void COglSurface::setFocusObject(int obj)
{
	focusObject=obj;
	if (obj==FOCUS_ON_PAGE)
	{
		setSceneSelectionActive(false);
		setPageSelectionActive(false);
		setViewSelectionActive(false);
		App::ct->browser->looseFocus();
		App::ct->hierarchy->looseFocus();
		pageCont->setFocusObject(obj);
	}
	if (obj==FOCUS_ON_BROWSER)
	{
		setSceneSelectionActive(false);
		setPageSelectionActive(false);
		setViewSelectionActive(false);
		App::ct->hierarchy->looseFocus();
		pageCont->looseFocus();
	}
	if (obj==FOCUS_ON_HIERARCHY)
	{
		setSceneSelectionActive(false);
		setPageSelectionActive(false);
		setViewSelectionActive(false);
		App::ct->browser->looseFocus();
		pageCont->looseFocus();
	}
	if (obj==FOCUS_ON_SCENE_SELECTION_WINDOW)
	{
		setPageSelectionActive(false);
		setViewSelectionActive(false);
		App::ct->hierarchy->looseFocus();
		App::ct->browser->looseFocus();
		pageCont->looseFocus();
	}
	if (obj==FOCUS_ON_PAGE_SELECTION_WINDOW)
	{
		setSceneSelectionActive(false);
		setViewSelectionActive(false);
		App::ct->hierarchy->looseFocus();
		App::ct->browser->looseFocus();
		pageCont->looseFocus();
	}
	if (obj==FOCUS_ON_VIEW_SELECTION_WINDOW)
	{
		setSceneSelectionActive(false);
		setPageSelectionActive(false);
		App::ct->hierarchy->looseFocus();
		App::ct->browser->looseFocus();
		pageCont->looseFocus();
	}
	if (obj==FOCUS_ON_SOFT_DIALOG)
	{
		setSceneSelectionActive(false);
		setPageSelectionActive(false);
		setViewSelectionActive(false);
		App::ct->hierarchy->looseFocus();
		App::ct->browser->looseFocus();
		pageCont->setFocusObject(obj);
	}
}
int COglSurface::getFocusObject()
{
	return(focusObject);
}

void COglSurface::setBrowserEnabled(bool isEnabled)
{
	_browserEnabled=isEnabled;
	if (!isEnabled)
		setFocusObject(FOCUS_ON_PAGE);
	actualizeAllSurfacesSizeAndPosition();
	App::ct->objCont->toolbarRefreshFlag=true;
}

void COglSurface::setHierarchyEnabled(bool isEnabled)
{
	_hierarchyEnabled=isEnabled;
	if (!isEnabled)
		setFocusObject(FOCUS_ON_PAGE);
	actualizeAllSurfacesSizeAndPosition();
	App::ct->objCont->toolbarRefreshFlag=true;
}

void COglSurface::setSceneSelectionActive(bool isActive)
{
	bool diff=(sceneSelectionActive!=isActive);
	sceneSelectionActive=isActive;
	if (isActive)
	{
		App::ct->closeTemporarilyDialogsForSceneSelector();
		setFocusObject(FOCUS_ON_SCENE_SELECTION_WINDOW);
	}
	else
		App::ct->reopenTemporarilyClosedDialogsForSceneSelector();
	actualizeAllSurfacesSizeAndPosition();
	if (App::ct->objCont!=NULL)
		App::ct->objCont->toolbarRefreshFlag=true;
	if (diff&&(App::mainWindow!=NULL))
		App::mainWindow->createDefaultMenuBar(false);
}

void COglSurface::setPageSelectionActive(bool isActive)
{
	bool diff=(pageSelectionActive!=isActive);
	pageSelectionActive=isActive;
	if (isActive)
	{
		App::ct->closeTemporarilyDialogsForPageSelector();
		setFocusObject(FOCUS_ON_PAGE_SELECTION_WINDOW);
	}
	else
		App::ct->reopenTemporarilyClosedDialogsForPageSelector();
	actualizeAllSurfacesSizeAndPosition();
	if (App::ct->objCont!=NULL)
		App::ct->objCont->toolbarRefreshFlag=true;
	if (diff&&(App::mainWindow!=NULL))
		App::mainWindow->createDefaultMenuBar(false);
}

void COglSurface::setViewSelectionActive(bool isActive)
{
	bool diff=(viewSelectionActive!=isActive);
	viewSelectionActive=isActive;
	if (isActive)
	{
		App::ct->closeTemporarilyDialogsForViewSelector();
		setFocusObject(FOCUS_ON_VIEW_SELECTION_WINDOW);
	}
	else
		App::ct->reopenTemporarilyClosedDialogsForViewSelector();
	actualizeAllSurfacesSizeAndPosition();
	if (App::ct->objCont!=NULL)
		App::ct->objCont->toolbarRefreshFlag=true;
	if (diff&&(App::mainWindow!=NULL))
		App::mainWindow->createDefaultMenuBar(false);
}

void COglSurface::setHierarchySurfaceSizeAndPosition()
{
	int b=0;
	if (isBrowserEffectivelyVisible())
		b=_browserWidth;
	if (_hierarchyEnabled)
	{
		if (_hierarchyWidth>surfaceSize[0]-b)
			_hierarchyWidth=surfaceSize[0]-b;
		if (_hierarchyWidth<128+2+4+HIERARCHY_SCROLLBAR_WIDTH*App::sc)
			_hierarchyWidth=128+2+4+HIERARCHY_SCROLLBAR_WIDTH*App::sc;
		App::ct->hierarchy->setRenderingSizeAndPosition(_hierarchyWidth-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH+1,surfaceSize[1]+(-BROWSER_HIERARCHY_TITLE_BAR_HEIGHT-1-1)*App::sc,surfacePosition[0]+b,surfacePosition[1]);
	}
}

void COglSurface::setBrowserSurfaceSizeAndPosition()
{
	int h=0;
	if (_hierarchyEnabled)
		h=_hierarchyWidth;
	if (_browserEnabled)
	{
		if (_browserWidth>surfaceSize[0]-h)
			_browserWidth=surfaceSize[0]-h;
		if (_browserWidth<128+2+4+HIERARCHY_SCROLLBAR_WIDTH*App::sc)
			_browserWidth=128+2+4+HIERARCHY_SCROLLBAR_WIDTH*App::sc;
		App::ct->browser->setRenderingSizeAndPosition(_browserWidth-BROWSER_HIERARCHY_MAIN_RENDERING_WINDOW_SEPARATION_WIDTH+1,surfaceSize[1]+(-BROWSER_HIERARCHY_TITLE_BAR_HEIGHT-1-1)*App::sc,surfacePosition[0]+0,surfacePosition[1]);
	}
}

void COglSurface::setViewSurfaceSizeAndPosition()
{
	int h=0;
	int b=0;
	if (_hierarchyEnabled)
		h=_hierarchyWidth;
	if (isBrowserEffectivelyVisible())
		b=_browserWidth;
	pageCont->setPageSizeAndPosition(surfaceSize[0]-h-b,surfaceSize[1],surfacePosition[0]+h+b,surfacePosition[1]);
}

void COglSurface::actualizeAllSurfacesSizeAndPosition()
{
	setBrowserSurfaceSizeAndPosition();
	setHierarchySurfaceSizeAndPosition();
	setViewSurfaceSizeAndPosition();
	sceneSelector->setViewSizeAndPosition(surfaceSize[0],surfaceSize[1],surfacePosition[0],surfacePosition[1]);
	pageSelector->setViewSizeAndPosition(surfaceSize[0],surfaceSize[1],surfacePosition[0],surfacePosition[1]);
	viewSelector->setViewSizeAndPosition(surfaceSize[0],surfaceSize[1],surfacePosition[0],surfacePosition[1]);
}

bool COglSurface::isHierarchyEnabled()
{
	return(_hierarchyEnabled);
}

bool COglSurface::isBrowserEnabled()
{
	return(_browserEnabled);
}

bool COglSurface::isBrowserEffectivelyVisible()
{
	if (App::ct->objCont==NULL)
		return(false);
	return(_browserEnabled&&(App::ct->objCont->getEditModeType()==NO_EDIT_MODE));
}

void COglSurface::keyPress(int key,QWidget* mainWindow)
{
	if (key==ESC_KEY)
		pageCont->clearAllLastMouseDownViewIndex();

	if (focusObject==FOCUS_ON_BROWSER)
	{
		App::ct->browser->keyPress(key);
		return;
	}
	if (focusObject==FOCUS_ON_HIERARCHY)
	{
		App::ct->hierarchy->keyPress(key);
		return;
	}
	if ((focusObject==FOCUS_ON_PAGE)||(focusObject==FOCUS_ON_SOFT_DIALOG))
	{
		pageCont->keyPress(key,mainWindow);
		return;
	}
	if (focusObject==FOCUS_ON_SCENE_SELECTION_WINDOW)
	{
		sceneSelector->keyPress(key);
		return;
	}
	if (focusObject==FOCUS_ON_PAGE_SELECTION_WINDOW)
	{
		pageSelector->keyPress(key);
		return;
	}
	if (focusObject==FOCUS_ON_VIEW_SELECTION_WINDOW)
	{
		viewSelector->keyPress(key);
		return;
	}
}

void COglSurface::startViewSelection(int objectType,int subViewIndex)
{
	viewSelector->setViewSelectionInfo(objectType,pageCont->getActivePageIndex(),subViewIndex);
	setViewSelectionActive(true);
}

int COglSurface::getHierarchyWidth()
{
	return(_hierarchyWidth);
}
void COglSurface::setHierarchyWidth(int w)
{
	_hierarchyWidth=w;
	actualizeAllSurfacesSizeAndPosition();
}

int COglSurface::getBrowserWidth()
{
	return(_browserWidth);
}
void COglSurface::setBrowserWidth(int w)
{
	_browserWidth=w;
	actualizeAllSurfacesSizeAndPosition();
}
