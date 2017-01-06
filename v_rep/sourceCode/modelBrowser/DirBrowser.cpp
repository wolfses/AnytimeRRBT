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

// This file requires some serious refactoring

#include "vrepPrecompiledHeader.h"
#include "v_rep_internal.h"
#include "DirBrowser.h"
#include "OGL.h"
#include "imgLoader.h"
#include "App.h"
#include "ObjectEdition.h"

#include "AddMenu.h"
#include "PathEdition.h"
#include "TriangleVertexEdition.h"
#include "Tt.h"
#include "v_repStrings.h"

const int SAFETY_BORDER_SIZE=20;
const int CONST_VAL_8=8;
const int CONST_VAL_6=6;
const int CONST_VAL_4=4;

CDirBrowser::CDirBrowser()
{
	setUpDefaultValues();
}

CDirBrowser::~CDirBrowser()
{
	removeAll();
	refreshViewFlag=3;
}

void CDirBrowser::newSceneProcedure()
{
	setUpDefaultValues();
}

void CDirBrowser::removeAll()
{
	refreshViewFlag=3;
	resetViewFlag=true;
}

void CDirBrowser::setUpDefaultValues()
{
	viewPosition[0]=0;
	viewPosition[1]=0;
	refreshViewFlag=3;
	resetViewFlag=true;
	shiftingAllowed=false;
	horizontalScrollbarHeight=0;
	verticalScrollbarWidth=0;
	sliderMoveMode=0;
	_caughtElements=0;
}

int CDirBrowser::getCaughtElements()
{
	return(_caughtElements);
}

void CDirBrowser::clearCaughtElements(int keepMask)
{
	_caughtElements&=keepMask;
}

void CDirBrowser::setRenderingSizeAndPosition(int xSize,int ySize,int xPos,int yPos)
{
	renderingSize[0]=xSize+SAFETY_BORDER_SIZE*App::sc;
	renderingSize[1]=ySize+SAFETY_BORDER_SIZE*App::sc;
	renderingPosition[0]=xPos-SAFETY_BORDER_SIZE*App::sc;
	renderingPosition[1]=yPos-SAFETY_BORDER_SIZE*App::sc;
	refreshViewFlag=3; // Important, even if the size and position didn't change!
}

void CDirBrowser::looseFocus()
{
	refreshViewFlag=3;
}

void CDirBrowser::keyPress(int key)
{
	if (App::ct->objCont==NULL)
		return;
	refreshViewFlag=3;	
	if (key==UP_KEY)
		viewPosition[1]=viewPosition[1]+18*App::sc;
	if (key==DOWN_KEY)
		viewPosition[1]=viewPosition[1]-18*App::sc;
	if (key==LEFT_KEY)
		viewPosition[0]=viewPosition[0]+18*App::sc;
	if (key==RIGHT_KEY)
		viewPosition[0]=viewPosition[0]-18*App::sc;
	if (key==ESC_KEY)
	{
		if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
			App::ct->objCont->deselectObjects();
	}
}

bool CDirBrowser::render()
{ // return value true means the hierarchy was refreshed
	if (viewPosition[0]<-2000*App::sc) // somehow there is a bug I can't put the finger on right now (2009/12/16)
		viewPosition[0]=0;
	if (resetViewFlag)
	{
		viewPosition[0]=0;
		viewPosition[1]=0;
		resetViewFlag=false;
	}
	if (refreshViewFlag<=0)
		return(false);
	refreshViewFlag--;

	// We draw a black background so that the separation between the hierarchy and scene is rendered correctly on ALL graphic cards:
	glEnable(GL_SCISSOR_TEST);	
	glDisable(GL_DEPTH_TEST);
	glScissor(renderingPosition[0]+SAFETY_BORDER_SIZE*App::sc,renderingPosition[1]+BROWSER_HIERARCHY_TITLE_BAR_HEIGHT*App::sc,renderingSize[0]-SAFETY_BORDER_SIZE*App::sc,renderingSize[1]);
	glClearColor(ogl::SEPARATION_LINE_COLOR[0],ogl::SEPARATION_LINE_COLOR[1],ogl::SEPARATION_LINE_COLOR[2],1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	int interLineSpace=18;
	int textCenterOffset=interLineSpace/2-4;
	ogl::setColorsAllBlack();
	glViewport(renderingPosition[0],renderingPosition[1],renderingSize[0],renderingSize[1]);
	ogl::allLightsOffAmbientFull();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f,renderingSize[0],0.0f,renderingSize[1],-1.0f,1.0f);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glScissor(renderingPosition[0]+SAFETY_BORDER_SIZE*App::sc,renderingPosition[1],renderingSize[0]-SAFETY_BORDER_SIZE*App::sc,renderingSize[1]);
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// There are 2 passes: one where we don't display anything but retrieve information
	// about min and max positions:
	validateViewPosition();
	int textPos[2]={viewPosition[0],viewPosition[1]+renderingSize[1]};
	minRenderedPosition[0]=100000;
	minRenderedPosition[1]=100000;
	maxRenderedPosition[0]=-100000;
	maxRenderedPosition[1]=-100000;
	bool bright=false;
	inflateIconPosition.clear();
	objectPosition.clear();
		std::vector<int> vertLines;
		App::ct->modelDirectoryRoot->renderElement(this,bright,textCenterOffset,interLineSpace,true,renderingSize,
				textPos,0,&vertLines,minRenderedPosition,maxRenderedPosition);
	maxRenderedPosition[0]=maxRenderedPosition[0]-viewPosition[0];
	maxRenderedPosition[1]=maxRenderedPosition[1]-viewPosition[1];
	minRenderedPosition[0]=minRenderedPosition[0]-viewPosition[0];
	minRenderedPosition[1]=minRenderedPosition[1]-viewPosition[1];

	// Second pass where we display:	
	validateViewPosition();

	textPos[0]=viewPosition[0];
	textPos[1]=viewPosition[1]+renderingSize[1];
	minRenderedPosition[0]=100000;
	minRenderedPosition[1]=100000;
	maxRenderedPosition[0]=-100000;
	maxRenderedPosition[1]=-100000;
	bright=false;
	inflateIconPosition.clear();
	objectPosition.clear();
	vertLines.clear();
	App::ct->modelDirectoryRoot->renderElement(this,bright,textCenterOffset,interLineSpace,false,renderingSize,
				textPos,0,&vertLines,minRenderedPosition,maxRenderedPosition);
	while (CDirBrowserElement::renderDummyElement(bright,textCenterOffset,interLineSpace,renderingSize,textPos));


	maxRenderedPosition[0]=maxRenderedPosition[0]-viewPosition[0];
	maxRenderedPosition[1]=maxRenderedPosition[1]-viewPosition[1];
	minRenderedPosition[0]=minRenderedPosition[0]-viewPosition[0];
	minRenderedPosition[1]=minRenderedPosition[1]-viewPosition[1];
	
	int vtl[2];
	int vbr[2];
	int htl[2];
	int hbr[2];
	int slidersEnable=getSliderPositions(vtl,vbr,htl,hbr,NULL);
//	float black[3]={0.0f,0.0f,0.0f};
//	float white[3]={0.85f,0.85f,0.85f};
	if (slidersEnable&1)
	{ // here we draw the vertical slider:
		ogl::setColor(ogl::HIERARCHY_AND_BROWSER_SCROLLBAR_BACK_COLOR,AMBIENT_MODE);
		int leftTop[2]={renderingSize[0]-verticalScrollbarWidth,renderingSize[1]};
		int rightBottom[2]={renderingSize[0],0};
		glBegin(GL_QUADS);
		glVertex3i(leftTop[0],leftTop[1],0);
		glVertex3i(rightBottom[0],leftTop[1],0);
		glVertex3i(rightBottom[0],rightBottom[1],0);
		glVertex3i(leftTop[0],rightBottom[1],0);
		glEnd();
		ogl::setColor(ogl::HIERARCHY_AND_BROWSER_SCROLLBAR_COLOR,AMBIENT_MODE);
		glBegin(GL_QUADS);
		glVertex3i(vtl[0],vtl[1],0);
		glVertex3i(vbr[0],vtl[1],0);
		glVertex3i(vbr[0],vbr[1],0);
		glVertex3i(vtl[0],vbr[1],0);
		glEnd();

		ogl::setColor(ogl::SEPARATION_LINE_COLOR,AMBIENT_MODE);
		glBegin(GL_LINES);
		glVertex3i(leftTop[0],leftTop[1],0);
		glVertex3i(leftTop[0],rightBottom[1],0);
		glEnd();
	}
	if (slidersEnable&2)
	{ // here we draw the horizontal slider:
		ogl::setColor(ogl::HIERARCHY_AND_BROWSER_SCROLLBAR_BACK_COLOR,AMBIENT_MODE);
		int leftTop[2]={0,horizontalScrollbarHeight+SAFETY_BORDER_SIZE*App::sc};
		int rightBottom[2]={renderingSize[0],0};
		glBegin(GL_QUADS);
		glVertex3i(leftTop[0],leftTop[1],0);
		glVertex3i(rightBottom[0],leftTop[1],0);
		glVertex3i(rightBottom[0],rightBottom[1],0);
		glVertex3i(leftTop[0],rightBottom[1],0);
		glEnd();
		ogl::setColor(ogl::HIERARCHY_AND_BROWSER_SCROLLBAR_COLOR,AMBIENT_MODE);
		glBegin(GL_QUADS);
		glVertex3i(htl[0],htl[1],0);
		glVertex3i(hbr[0],htl[1],0);
		glVertex3i(hbr[0],hbr[1],0);
		glVertex3i(htl[0],hbr[1],0);
		glEnd();

		ogl::setColor(ogl::SEPARATION_LINE_COLOR,AMBIENT_MODE);
		glBegin(GL_LINES);
		glVertex3i(leftTop[0],leftTop[1],0);
		glVertex3i(rightBottom[0]-verticalScrollbarWidth,leftTop[1],0);
		glEnd();

	}
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);
	return(true);
}

int CDirBrowser::getSliderPositions(int vSliderTopLeft[2],int vSliderBottomRight[2],int hSliderTopLeft[2],int hSliderBottomRight[2],float prop[2])
{
	// we check if we need the sliders:
	verticalScrollbarWidth=0;
	horizontalScrollbarHeight=0;
	int minMaxViewPosition[2];
	int minViewPosition[2];
	for (int i=0;i<3;i++)
	{ // we need to do it 3 times because one added slider might mean we have to add the other one!
		int savedViewPosition[2]={viewPosition[0],viewPosition[1]};
		viewPosition[0]=-99999999;
		viewPosition[1]=-99999999;
		validateViewPosition();
		minViewPosition[0]=viewPosition[0];
		minViewPosition[1]=viewPosition[1];
		viewPosition[0]=+99999999;
		viewPosition[1]=+99999999;
		validateViewPosition();
		int maxViewPosition[2]={viewPosition[0],viewPosition[1]};
		minMaxViewPosition[0]=maxViewPosition[0]-minViewPosition[0];
		minMaxViewPosition[1]=maxViewPosition[1]-minViewPosition[1];
		viewPosition[0]=savedViewPosition[0];
		viewPosition[1]=savedViewPosition[1];
		if (minMaxViewPosition[0]!=0)
			horizontalScrollbarHeight=HIERARCHY_SCROLLBAR_WIDTH*App::sc;
		if (minMaxViewPosition[1]!=0)
			verticalScrollbarWidth=HIERARCHY_SCROLLBAR_WIDTH*App::sc;
	}
	int retVal=0;
	if (verticalScrollbarWidth>0)
	{ // here we compute the vertical slider:
		retVal|=1;
		int effDr=renderingSize[1]-horizontalScrollbarHeight-SAFETY_BORDER_SIZE*App::sc;
		int sliderS=effDr*effDr/(effDr+minMaxViewPosition[1]);
		if (sliderS<HIERARCHY_SCROLLBAR_WIDTH*App::sc)
			sliderS=HIERARCHY_SCROLLBAR_WIDTH*App::sc;
		int sliderP=int(float(effDr-sliderS)*float(viewPosition[1]-minViewPosition[1])/float(minMaxViewPosition[1]));
		vSliderTopLeft[0]=renderingSize[0]-verticalScrollbarWidth;
		vSliderBottomRight[0]=renderingSize[0];
		vSliderTopLeft[1]=renderingSize[1]-sliderP;
		vSliderBottomRight[1]=vSliderTopLeft[1]-sliderS;
		if (prop!=NULL)
			prop[0]=float(minMaxViewPosition[1])/float(effDr-sliderS);
	}
	if (horizontalScrollbarHeight>0)
	{ // here we compute the horizontal slider:
		retVal|=2;
		int effDr=renderingSize[0]-verticalScrollbarWidth+(-SAFETY_BORDER_SIZE-HIERARCHY_ICON_QUARTER_HEIGHT)*App::sc;
		int sliderS=effDr*effDr/(effDr+minMaxViewPosition[0]);
		if (sliderS<HIERARCHY_SCROLLBAR_WIDTH*App::sc)
			sliderS=HIERARCHY_SCROLLBAR_WIDTH*App::sc;
		int sliderP=int(float(effDr-sliderS)*float(viewPosition[0]-minViewPosition[0])/float(minMaxViewPosition[0]));
		hSliderTopLeft[1]=horizontalScrollbarHeight+SAFETY_BORDER_SIZE*App::sc;
		hSliderBottomRight[1]=0;
		hSliderTopLeft[0]=effDr-sliderS-sliderP+SAFETY_BORDER_SIZE*App::sc;
		hSliderBottomRight[0]=hSliderTopLeft[0]+sliderS;
		if (prop!=NULL)
			prop[1]=float(minMaxViewPosition[0])/float(effDr-sliderS);
	}
	return(retVal);
}

bool CDirBrowser::leftMouseDown(int x,int y,int selectionStatus)
{
	if (x<0)
		return(false);
	if (y<0)
		return(false);
	if (x>renderingSize[0]-SAFETY_BORDER_SIZE*App::sc)
		return(false);
	if (y>renderingSize[1]-SAFETY_BORDER_SIZE*App::sc)
		return(false);
	// The mouse went down on the hierarchy window!
	_caughtElements|=sim_left_button;
	refreshViewFlag=3;
	mouseDownRelativePosition[0]=x+SAFETY_BORDER_SIZE*App::sc;
	mouseDownRelativePosition[1]=y+SAFETY_BORDER_SIZE*App::sc;
	mouseRelativePosition[0]=x+SAFETY_BORDER_SIZE*App::sc;
	mouseRelativePosition[1]=y+SAFETY_BORDER_SIZE*App::sc;
	previousMouseRelativePosition[0]=x+SAFETY_BORDER_SIZE*App::sc;
	previousMouseRelativePosition[1]=y+SAFETY_BORDER_SIZE*App::sc;

	int vtl[2];
	int vbr[2];
	int htl[2];
	int hbr[2];
	int slidersEnable=getSliderPositions(vtl,vbr,htl,hbr,NULL);
	sliderMoveMode=0;
	if ( (slidersEnable&1)&&(mouseDownRelativePosition[0]>vtl[0])&&(mouseDownRelativePosition[0]<vbr[0]) )
	{
		if ( (mouseDownRelativePosition[1]<vtl[1])&&(mouseDownRelativePosition[1]>vbr[1]) )
		{ // Vertical slider action
			sliderMoveMode=1;
			viewPosWhenMouseOnSliderDown[0]=viewPosition[0];
			viewPosWhenMouseOnSliderDown[1]=viewPosition[1];
			return(true);
		}
		else
		{ // We clicked the slider, but not the mobile part
			return(true);
		}
	}
	if ( (slidersEnable&2)&&(mouseDownRelativePosition[1]<htl[1])&&(mouseDownRelativePosition[1]>hbr[1]) )
	{
		if ( (mouseDownRelativePosition[0]>htl[0])&&(mouseDownRelativePosition[0]<hbr[0]) )
		{ // Horizontal slider action
			sliderMoveMode=2;
			viewPosWhenMouseOnSliderDown[0]=viewPosition[0];
			viewPosWhenMouseOnSliderDown[1]=viewPosition[1];
			return(true);
		}
		else
		{ // We clicked the slider, but not the mobile part
			return(true);
		}
	}

	shiftingAllowed=true;

	int uniqueID=getInflateActionObjectID(mouseDownRelativePosition[0],mouseDownRelativePosition[1]);
	bool canSelect=true;
	if (uniqueID!=-1)
	{ // Expansion/collapse
		App::ct->modelDirectoryRoot->inflateOrCollapseElementWithUniqueID(uniqueID);
		shiftingAllowed=false;
	}
	if (canSelect)
	{
		int interLineSpace=18;
		int uniqueID=getActionObjectID(mouseDownRelativePosition[1],interLineSpace/2);
		if (uniqueID!=-1)
			App::ct->modelDirectoryRoot->setSelectedElement(uniqueID);
	}
	return(true);
}

void CDirBrowser::leftMouseUp(int x,int y)
{
	sliderMoveMode=0;
	refreshViewFlag=3;
	mouseRelativePosition[0]=x+SAFETY_BORDER_SIZE*App::sc;
	mouseRelativePosition[1]=y+SAFETY_BORDER_SIZE*App::sc;
	_caughtElements&=0xffff-sim_left_button;
	shiftingAllowed=false;
}
bool CDirBrowser::rightMouseDown(int x,int y)
{
	if (x<0)
		return(false);
	if (y<0)
		return(false);
	if (x>renderingSize[0]-SAFETY_BORDER_SIZE*App::sc)
		return(false);
	if (y>renderingSize[1]-SAFETY_BORDER_SIZE*App::sc)
		return(false);
	// The mouse went down on the hierarchy window
	_caughtElements|=sim_right_button;
	refreshViewFlag=3;
	mouseDownRelativePosition[0]=x+SAFETY_BORDER_SIZE*App::sc;
	mouseDownRelativePosition[1]=y+SAFETY_BORDER_SIZE*App::sc;
	mouseRelativePosition[0]=x+SAFETY_BORDER_SIZE*App::sc;
	mouseRelativePosition[1]=y+SAFETY_BORDER_SIZE*App::sc;
	previousMouseRelativePosition[0]=x+SAFETY_BORDER_SIZE*App::sc;
	previousMouseRelativePosition[1]=y+SAFETY_BORDER_SIZE*App::sc;
	return(true); // We catch this event to display a popup-menu when the mouse comes up
}
void CDirBrowser::rightMouseUp(int x,int y,int absX,int absY,QWidget* mainWindow)
{ // Only caught if right button was caught by the hierarchy!
	_caughtElements&=0xffff-sim_right_button;
	if ( (x<0)||(y<0)||(x>renderingSize[0]-SAFETY_BORDER_SIZE*App::sc)||(y>renderingSize[1]-SAFETY_BORDER_SIZE*App::sc) )
		return;
	// The mouse went up on this window
	refreshViewFlag=3;
	mouseRelativePosition[0]=x+SAFETY_BORDER_SIZE*App::sc;
	mouseRelativePosition[1]=y+SAFETY_BORDER_SIZE*App::sc;

	if (App::operationalUIParts&sim_gui_popups)
	{ // Default popups
		VMenu* mainMenu=VMenubar::createPopupMenu();
		addMenu(mainMenu);
		int command=VMenubar::trackPopupMenu(mainMenu,absX,absY,mainWindow);
		bool processed=false;
		if (!processed)
			processed=evaluateMenuSelection(command);
		delete mainMenu;
	}
}

bool CDirBrowser::mouseWheel(int deltaZ,int x,int y)
{
	if ( (x<0)||(y<0)||(x>renderingSize[0]-SAFETY_BORDER_SIZE*App::sc)||(y>renderingSize[1]-SAFETY_BORDER_SIZE*App::sc) )
		return(false);
	refreshViewFlag=3;
	viewPosition[1]=viewPosition[1]-(deltaZ/120)*18;
	return(true);
}

void CDirBrowser::mouseMove(int x,int y,bool passiveAndFocused)
{
	mouseRelativePosition[0]=x+SAFETY_BORDER_SIZE*App::sc;
	mouseRelativePosition[1]=y+SAFETY_BORDER_SIZE*App::sc;
	if (!passiveAndFocused)
	{
		int vtl[2];
		int vbr[2];
		int htl[2];
		int hbr[2];
		float prop[2];
		if (sliderMoveMode&1)
		{ // we are moving the vertical slider
			getSliderPositions(vtl,vbr,htl,hbr,prop);
			viewPosition[1]=viewPosWhenMouseOnSliderDown[1]-int(prop[0]*float(mouseRelativePosition[1]-mouseDownRelativePosition[1]));
			refreshViewFlag=3;
		} 
		else if (sliderMoveMode&2)
		{ // we are moving the horizontal slider
			getSliderPositions(vtl,vbr,htl,hbr,prop);
			viewPosition[0]=viewPosWhenMouseOnSliderDown[0]-int(prop[1]*float(mouseRelativePosition[0]-mouseDownRelativePosition[0]));
			refreshViewFlag=3;
		} 
		else if (shiftingAllowed)
		{
			refreshViewFlag=3;
			viewPosition[0]=viewPosition[0]+mouseRelativePosition[0]-previousMouseRelativePosition[0];
			viewPosition[1]=viewPosition[1]+mouseRelativePosition[1]-previousMouseRelativePosition[1];
		}
	}
	previousMouseRelativePosition[0]=mouseRelativePosition[0];
	previousMouseRelativePosition[1]=mouseRelativePosition[1];
}
bool CDirBrowser::leftMouseDblClick(int x,int y,int selectionStatus)
{
	if ( (x<0)||(y<0)||(x>renderingSize[0]-SAFETY_BORDER_SIZE*App::sc)||(y>renderingSize[1]-SAFETY_BORDER_SIZE*App::sc) )
		return(false);
	// The mouse went down on the hierarchy window
	refreshViewFlag=3;
	mouseDownRelativePosition[0]=x+SAFETY_BORDER_SIZE*App::sc;
	mouseDownRelativePosition[1]=y+SAFETY_BORDER_SIZE*App::sc;
	mouseRelativePosition[0]=x+SAFETY_BORDER_SIZE*App::sc;
	mouseRelativePosition[1]=y+SAFETY_BORDER_SIZE*App::sc;
	previousMouseRelativePosition[0]=x+SAFETY_BORDER_SIZE*App::sc;
	previousMouseRelativePosition[1]=y+SAFETY_BORDER_SIZE*App::sc;
	shiftingAllowed=false;

	return(false);
}
void CDirBrowser::validateViewPosition()
{
	int tmp=renderingSize[0]-maxRenderedPosition[0]-viewPosition[0]-SAFETY_BORDER_SIZE*App::sc-verticalScrollbarWidth;
	if (tmp>0)
		viewPosition[0]=renderingSize[0]-(maxRenderedPosition[0]-minRenderedPosition[0])-SAFETY_BORDER_SIZE*App::sc-verticalScrollbarWidth;
	if (viewPosition[0]>SAFETY_BORDER_SIZE*App::sc)
		viewPosition[0]=SAFETY_BORDER_SIZE*App::sc;
	tmp=renderingSize[1]-(maxRenderedPosition[1]-minRenderedPosition[1])+viewPosition[1]+(-SAFETY_BORDER_SIZE-CONST_VAL_4)*App::sc-horizontalScrollbarHeight;
	if (tmp>0)
		viewPosition[1]=maxRenderedPosition[1]-minRenderedPosition[1]-renderingSize[1]+(SAFETY_BORDER_SIZE+CONST_VAL_6)*App::sc+horizontalScrollbarHeight;
	if (viewPosition[1]<(-SAFETY_BORDER_SIZE+CONST_VAL_8)*App::sc)
		viewPosition[1]=(-SAFETY_BORDER_SIZE+CONST_VAL_8)*App::sc;
}

void CDirBrowser::setRefreshViewFlag()
{
	refreshViewFlag=3;
}

int CDirBrowser::getInflateActionObjectID(int mousePositionX,int mousePositionY)
{
	for (int i=0;i<int(inflateIconPosition.size())/3;i++)
	{
		if ((mousePositionX>inflateIconPosition[3*i+0]-8)&&(mousePositionX<inflateIconPosition[3*i+0]+8) )
		{
			if ((mousePositionY>inflateIconPosition[3*i+1]-8)&&(mousePositionY<inflateIconPosition[3*i+1]+8) )
				return(inflateIconPosition[3*i+2]);
		}
	}
	return(-1);
}

int CDirBrowser::getActionObjectID(int mousePositionY,int tolerance)
{
	for (int i=0;i<int(objectPosition.size())/2;i++)
	{
		if ((mousePositionY<=objectPosition[2*i+0]+tolerance)&&(mousePositionY>=objectPosition[2*i+0]-tolerance) )
			return(objectPosition[2*i+1]);
	}
	return(-1);
}

void CDirBrowser::addMenu(VMenu* menu)
{ 
	VMenubar::appendMenuItem(menu,true,false,EXPAND_HIERARCHY_CMD,IDS_EXPAND_ALL_MENU_ITEM);
	VMenubar::appendMenuItem(menu,true,false,COLLAPSE_HIERARCHY_CMD,IDS_COLLAPSE_ALL_MENU_ITEM);
}

bool CDirBrowser::evaluateMenuSelection(int commandID)
{ // Return value is true if the command belonged to Add menu and was executed
	if ( (commandID==EXPAND_HIERARCHY_CMD)||(commandID==COLLAPSE_HIERARCHY_CMD) )
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("EXPAND...COLLAPSE_HIERARCHY_CMD")
		{
			App::ct->modelDirectoryRoot->expandCollapseAll(commandID==EXPAND_HIERARCHY_CMD);
		}
		return(true);
	}
	return(false);
}
