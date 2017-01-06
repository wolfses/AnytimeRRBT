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
#include "Browser.h"
#include "OGL.h"
#include "App.h"

CBrowser::CBrowser()
{
	dirBrowser=new CDirBrowser();
	modelBrowser=new CModelBrowser();
	setUpDefaultValues();
}

CBrowser::~CBrowser()
{
	delete modelBrowser;
	delete dirBrowser;
}

void CBrowser::setRefreshViewFlag()
{
	dirBrowser->setRefreshViewFlag();
	modelBrowser->setRefreshViewFlag();
}

void CBrowser::newSceneProcedure()
{
	setUpDefaultValues();
}

void CBrowser::setUpDefaultValues()
{
	_dirBrowserHeight=240;//160;
	_focusOnModelBrowser=true;
	_resizingAction=false;
	_caughtElements=0;
}

int CBrowser::getCaughtElements()
{
	return(_caughtElements);
}

void CBrowser::clearCaughtElements(int keepMask)
{
	_caughtElements&=keepMask;
}

void CBrowser::setRenderingSizeAndPosition(int xSize,int ySize,int xPos,int yPos)
{
	renderingSize[0]=xSize;
	renderingSize[1]=ySize;
	renderingPosition[0]=xPos;
	renderingPosition[1]=yPos;
	if (_dirBrowserHeight<240*App::sc)//HIERARCHY_SCROLLBAR_WIDTH*3)
		_dirBrowserHeight=240*App::sc;//HIERARCHY_SCROLLBAR_WIDTH*3;
	if (_dirBrowserHeight>renderingSize[1]-160*App::sc)
		_dirBrowserHeight=renderingSize[1]-160*App::sc;
	dirBrowser->setRenderingSizeAndPosition(xSize,_dirBrowserHeight,xPos,yPos+ySize-_dirBrowserHeight);
	modelBrowser->setRenderingSizeAndPosition(xSize,ySize-_dirBrowserHeight,xPos,yPos);
}

void CBrowser::looseFocus()
{
	dirBrowser->looseFocus();
	modelBrowser->looseFocus();
}

void CBrowser::keyPress(int key)
{
	if (App::ct->objCont==NULL)
		return;
	if (_focusOnModelBrowser)
		modelBrowser->keyPress(key);
	else
		dirBrowser->keyPress(key);
	if (key==ESC_KEY)
	{
		if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
			App::ct->objCont->deselectObjects();
		else if (App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)
			App::ct->objCont->deselectEditModeBuffer();
		else if (App::ct->objCont->getEditModeType()&BUTTON_EDIT_MODE)
			App::ct->buttonBlockContainer->looseFocus();
	}
}

bool CBrowser::render()
{ // return value true means the hierarchy was refreshed
	if (dirBrowser->render())
		modelBrowser->setRefreshViewFlag();
	modelBrowser->render();
	// now the horizontal separation line:
	ogl::allLightsOffAmbientFull();
	ogl::setColorsAllBlack();
	glViewport(renderingPosition[0],renderingPosition[1],renderingSize[0],renderingSize[1]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f,renderingSize[0],0.0f,renderingSize[1],-100.0f,100.0f);
	glDisable(GL_DEPTH_TEST);
	ogl::setColor(ogl::SEPARATION_LINE_COLOR,AMBIENT_MODE);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glVertex3i(0,renderingSize[1]-_dirBrowserHeight,0);
	glVertex3i(renderingSize[0],renderingSize[1]-_dirBrowserHeight,0);
	glEnd();
	glLineWidth(1.0f);
	glEnable(GL_DEPTH_TEST);
	return(true);
}

bool CBrowser::leftMouseDown(int x,int y,int selectionStatus)
{
	if (x<0)
		return(false);
	if (y<0)
		return(false);
	if (x>renderingSize[0])
		return(false);
	if (y>renderingSize[1])
		return(false);
	// The mouse went down on the browser window!
	if ( (y>renderingSize[1]-_dirBrowserHeight-4)&&(y<renderingSize[1]-_dirBrowserHeight+4) )
	{ // We start a resizing action of the browser/dir view:
		_caughtElements|=sim_left_button|sim_focus;
		_resizingAction=true;
		return(true); // We want the mouse captured!
	}
	else
	{
		_focusOnModelBrowser=!dirBrowser->leftMouseDown(x,y-renderingSize[1]+_dirBrowserHeight,selectionStatus);
		modelBrowser->leftMouseDown(x,y,selectionStatus);
		_caughtElements|=sim_left_button;
	}
	return(true);
}

void CBrowser::leftMouseUp(int x,int y)
{
	_resizingAction=false;
	dirBrowser->leftMouseUp(x,y-renderingSize[1]+_dirBrowserHeight);
	modelBrowser->leftMouseUp(x,y);
	_caughtElements&=0xffff-sim_left_button;
	_resizingAction=false;
}

bool CBrowser::rightMouseDown(int x,int y)
{
	if (x<0)
		return(false);
	if (y<0)
		return(false);
	if (x>renderingSize[0])
		return(false);
	if (y>renderingSize[1])
		return(false);
	// The mouse went down on the browser window
	_focusOnModelBrowser=!dirBrowser->rightMouseDown(x,y-renderingSize[1]+_dirBrowserHeight);
	modelBrowser->rightMouseDown(x,y);
	_caughtElements|=sim_right_button;
	return(true); // We catch this event to display a popup-menu when the mouse comes up
}

void CBrowser::rightMouseUp(int x,int y,int absX,int absY,QWidget* mainWindow)
{ // Only caught if right button was caught by the hierarchy!
	_caughtElements&=0xffff-sim_right_button;
	if ( (x<0)||(y<0)||(x>renderingSize[0])||(y>renderingSize[1]) )
		return;
	dirBrowser->rightMouseUp(x,y-renderingSize[1]+_dirBrowserHeight,absX,absY,mainWindow);
	modelBrowser->rightMouseUp(x,y,absX,absY,mainWindow);
}

bool CBrowser::mouseWheel(int deltaZ,int x,int y)
{
	if ( (x<0)||(y<0)||(x>renderingSize[0])||(y>renderingSize[1]) )
		return(false);
	dirBrowser->mouseWheel(deltaZ,x,y-renderingSize[1]+_dirBrowserHeight);
	modelBrowser->mouseWheel(deltaZ,x,y);
	return(true);
}

bool CBrowser::getMouseIsOnVerticalResizingBar(int x,int y)
{ // needed to display the correct cursor during a resizing operation
	if ( (y>renderingSize[1]-_dirBrowserHeight-4)&&(y<renderingSize[1]-_dirBrowserHeight+4) )
		return(x<renderingSize[0]);
	return(false);
}

void CBrowser::mouseMove(int x,int y,bool passiveAndFocused)
{
	if (_resizingAction)
	{
		_dirBrowserHeight=renderingSize[1]-y;
		setRenderingSizeAndPosition(renderingSize[0],renderingSize[1],renderingPosition[0],renderingPosition[1]);
	}
	else
	{
		dirBrowser->mouseMove(x,y-renderingSize[1]+_dirBrowserHeight,passiveAndFocused);
		modelBrowser->mouseMove(x,y,passiveAndFocused);
	}
}

bool CBrowser::leftMouseDblClick(int x,int y,int selectionStatus)
{
	if ( (x<0)||(y<0)||(x>renderingSize[0])||(y>renderingSize[1]) )
		return(false);
	// The mouse went down on the browser window
	dirBrowser->leftMouseDblClick(x,y-renderingSize[1]+_dirBrowserHeight,selectionStatus);
	modelBrowser->leftMouseDblClick(x,y,selectionStatus);
	return(false);
}
