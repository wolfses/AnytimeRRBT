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

// This file requires some serious refactoring!!

#include "vrepPrecompiledHeader.h"
#include "v_rep_internal.h"
#include "Hierarchy.h"
#include "OGL.h"
#include "imgLoader.h"
#include "IloIlo.h"
#include "ObjectEdition.h"

#include "AddMenu.h"
#include "FileMenu.h"
#include "PathEdition.h"
#include "TriangleVertexEdition.h"
#include "Tt.h"
#include "pluginContainer.h"
#include "v_repStrings.h"
#include "App.h"
#include "VVarious.h"
#include "VDateTime.h"
#include "qdlgmodelproperties.h"
#include "qdlgscriptparameters.h"

const int SAFETY_BORDER_SIZE=20;
const int CONST_VAL_40=40;
const int CONST_VAL_8=8;
const int CONST_VAL_6=6;
const int CONST_VAL_4=4;
//const int SAFETY_BORDER_SIZE=40;

CHierarchy::CHierarchy()
{
	setUpDefaultValues();
}

CHierarchy::~CHierarchy()
{
	removeAll();
	refreshViewFlag=3;
	rebuildHierarchyFlag=true;
}

void CHierarchy::newSceneProcedure()
{
	setUpDefaultValues();
}

void CHierarchy::removeAll()
{
	for (int i=0;i<int(rootElements.size());i++)
		delete rootElements[i];
	rootElements.clear();
	refreshViewFlag=3;
	rebuildHierarchyFlag=true;
	resetViewFlag=true;
}

void CHierarchy::setUpDefaultValues()
{
	viewPosition[0]=0;
	viewPosition[1]=0;
	refreshViewFlag=3;
	rebuildHierarchyFlag=true;
	resetViewFlag=true;
	shiftingAllowed=false;
	shiftSelectionStarted=false;
	labelEditObjectID=-1;
	editionTextEditPos=-1;
	horizontalScrollbarHeight=0;
	verticalScrollbarWidth=0;
	sliderMoveMode=0;
	_caughtElements=0;
	_mouseDownDragObjectID=-1;
	_worldSelectID_down=-9999;
	_worldSelectID_moving=-9999;
}

int CHierarchy::getCaughtElements()
{
	return(_caughtElements);
}

void CHierarchy::clearCaughtElements(int keepMask)
{
	_caughtElements&=keepMask;
}

void CHierarchy::setRenderingSizeAndPosition(int xSize,int ySize,int xPos,int yPos)
{
	renderingSize[0]=xSize+SAFETY_BORDER_SIZE*App::sc;
	renderingSize[1]=ySize+SAFETY_BORDER_SIZE*App::sc;
	renderingPosition[0]=xPos-SAFETY_BORDER_SIZE*App::sc;
	renderingPosition[1]=yPos-SAFETY_BORDER_SIZE*App::sc;
	refreshViewFlag=3; // Important, even if the size and position didn't change!
}
void CHierarchy::rebuildHierarchy()
{
	for (int i=0;i<int(rootElements.size());i++)
		delete rootElements[i];
	rootElements.clear();

	if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
	{
		for (int i=0;i<App::ct->getInstanceNumber();i++)
		{
			if (i!=App::ct->getCurrentInstanceIndex())
			{
				CHierarchyElement* el=new CHierarchyElement(-i-1);
				std::string sceneName=VVarious::splitPath_fileBase(App::ct->mainSettingsList[i]->scenePathAndName);
				el->setSceneName(sceneName);
				rootElements.push_back(el);
			}
		}
		CHierarchyElement* newEl=new CHierarchyElement(-App::ct->getCurrentInstanceIndex()-1);
		newEl->addYourChildren();
		std::string sceneName=VVarious::splitPath_fileBase(App::ct->mainSettings->scenePathAndName);
		newEl->setSceneName(sceneName);
		rootElements.push_back(newEl);
	}
	if (App::ct->objCont->getEditModeType()&VERTEX_EDIT_MODE)
	{
		for (int i=0;i<int(App::ct->objCont->_editionVertices.size()/3);i++)
			rootElements.push_back(new CHierarchyElement(i));
	}
	if (App::ct->objCont->getEditModeType()&TRIANGLE_EDIT_MODE)
	{
		for (int i=0;i<int(App::ct->objCont->_editionIndices.size()/3);i++)
			rootElements.push_back(new CHierarchyElement(i));
	}
	if (App::ct->objCont->getEditModeType()&EDGE_EDIT_MODE)
	{
		for (int i=0;i<int(App::ct->objCont->_edgeCont.allEdges.size()/2);i++)
			rootElements.push_back(new CHierarchyElement(i));
	}
	if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
	{
		for (int i=0;i<int(App::ct->objCont->_editionPath->getSimplePathPointCount());i++)
			rootElements.push_back(new CHierarchyElement(i));
	}
	if (App::ct->objCont->getEditModeType()&BUTTON_EDIT_MODE)
	{
		for (int i=0;i<int(App::ct->buttonBlockContainer->allBlocks.size());i++)
		{
			CButtonBlock* it=App::ct->buttonBlockContainer->allBlocks[i];
			if ( ((it->getAttributes()&sim_ui_property_systemblock)==0) )
				rootElements.push_back(new CHierarchyElement(it->getBlockID()));
		}
	}
	for (int i=0;i<int(rootElements.size());i++)
		rootElements[i]->computeNumberOfElements();
	rebuildHierarchyFlag=false;
	refreshViewFlag=3;
}

void CHierarchy::looseFocus()
{
	refreshViewFlag=3;
	labelEditObjectID=-1;
}

void CHierarchy::keyPress(int key)
{
	if (App::ct->objCont==NULL)
		return;
	refreshViewFlag=3;	
	if (labelEditObjectID==-1)
	{ // Normal mode (no label edition)
		if (key==UP_KEY)
			viewPosition[1]=viewPosition[1]+HIERARCHY_INTER_LINE_SPACE*App::sc;
		if (key==DOWN_KEY)
			viewPosition[1]=viewPosition[1]-HIERARCHY_INTER_LINE_SPACE*App::sc;
		if (key==LEFT_KEY)
			viewPosition[0]=viewPosition[0]+HIERARCHY_INTER_LINE_SPACE*App::sc;
		if (key==RIGHT_KEY)
			viewPosition[0]=viewPosition[0]-HIERARCHY_INTER_LINE_SPACE*App::sc;

		if (key==CTRL_V_KEY)
		{
			if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
				CObjectEdition::evaluateMenuSelection(PASTE_OBJECTS_CMD);
			if (App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)
				CTriangleVertexEdition::evaluateMenuSelection(PASTE_TRIANGLE_VERTEX_CMD);
			if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
				CPathEdition::evaluateMenuSelection(PASTE_PATH_POINT_CMD,NULL);
			if (App::ct->objCont->getEditModeType()&BUTTON_EDIT_MODE)
				App::ct->buttonBlockContainer->evaluateMenuSelection(PASTE_2DELEMENT_CMD);
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
			if (App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)
			{
				if ((key==DELETE_KEY)||(key==BACKSPACE_KEY))
					CTriangleVertexEdition::evaluateMenuSelection(DELETE_TRIANGLE_VERTEX_EDGE_CMD);
				if (key==CTRL_X_KEY)
					CTriangleVertexEdition::evaluateMenuSelection(TRIANGLE_VERTEX_CUT_CMD);
				if (key==CTRL_C_KEY)
					CTriangleVertexEdition::evaluateMenuSelection(TRIANGLE_VERTEX_COPY_CMD);
			}
			if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
			{
				if ((key==DELETE_KEY)||(key==BACKSPACE_KEY))
					CPathEdition::evaluateMenuSelection(DELETE_PATH_POINT_CMD,NULL);
				if (key==CTRL_X_KEY)
					CPathEdition::evaluateMenuSelection(PATH_POINT_CUT_CMD,NULL);
				if (key==CTRL_C_KEY)
					CPathEdition::evaluateMenuSelection(PATH_POINT_COPY_CMD,NULL);
			}
			if (App::ct->objCont->getEditModeType()&BUTTON_EDIT_MODE)
			{
				if ((key==DELETE_KEY)||(key==BACKSPACE_KEY))
					App::ct->buttonBlockContainer->evaluateMenuSelection(DELETE_2DELEMENT_CMD);
				if (key==CTRL_X_KEY)
					App::ct->buttonBlockContainer->evaluateMenuSelection(CUT_2DELEMENT_CMD);
				if (key==CTRL_C_KEY)
					App::ct->buttonBlockContainer->evaluateMenuSelection(COPY_2DELEMENT_CMD);
			}
		}

		App::mainWindow->handleCommonKeyPressEvents(key);

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
	else
	{ // Label edition mode
		int em=App::ct->objCont->getEditModeType();
		if ( (em==NO_EDIT_MODE)||(em==BUTTON_EDIT_MODE) )
		{
			C3DObject* it=App::ct->objCont->getObject(labelEditObjectID);
			CButtonBlock* blk=App::ct->buttonBlockContainer->getBlockWithID(labelEditObjectID);
			if ( (key==ENTER_KEY)||(key==TAB_KEY) )
			{
				tt::removeIllegalCharacters(editionText,true);
				if (editionText!="")
				{
					if ( (em==NO_EDIT_MODE)&&(it!=NULL) )
					{
						if (App::ct->objCont->getObject(editionText)==NULL)
						{
							if ( (VREP_LOWCASE_STRING_COMPARE("world",editionText.c_str())!=0)&&(VREP_LOWCASE_STRING_COMPARE("none",editionText.c_str())!=0) )
							{
								it->setName(editionText);
								App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
							}
						}
					}
					if ( (em==BUTTON_EDIT_MODE)&&(blk!=NULL) )
					{
						if (App::ct->buttonBlockContainer->getBlockWithName(editionText)==NULL)
						{
							blk->setBlockName(editionText);
							App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
						}
					}
					App::ct->objCont->setFullDialogRefreshFlag();
				}
				labelEditObjectID=-1;
			}
			if ( (key==LEFT_KEY)||(key==RIGHT_KEY) )
			{
				if (editionTextEditPos==-1)
					editionTextEditPos=editionText.length();
				if ( (key==LEFT_KEY)&&(editionTextEditPos>0) )
					editionTextEditPos--;
				if ( (key==RIGHT_KEY)&&(editionTextEditPos<int(editionText.length())) )
					editionTextEditPos++;
			}
			if (key==BACKSPACE_KEY)
			{
				if (editionTextEditPos==-1)
				{
					editionText="";
					editionTextEditPos=0;
				}
				else
				{
					if (editionText.length()!=0)
					{
						if (editionTextEditPos>0)
						{
							editionText.erase(editionText.begin()+editionTextEditPos-1);
							editionTextEditPos--;
						}
					}
				}
			}
			if (key==DELETE_KEY)
			{
				if (editionTextEditPos==-1)
				{
					editionText="";
					editionTextEditPos=0;
				}
				else
				{
					if (editionText.length()!=0)
					{
						if (editionTextEditPos<int(editionText.length()))
							editionText.erase(editionText.begin()+editionTextEditPos);
					}
				}
			}
			if ( (key>=32)&&(key<123) )
			{
				if (editionTextEditPos==-1)
				{
					editionTextEditPos=0;
					editionText="";
				}
				editionText.insert(editionText.begin()+editionTextEditPos,(char)key);
				editionTextEditPos++;
			}
		}
	}
}

bool CHierarchy::render()
{ // return value true means the hierarchy was refreshed
	FUNCTION_DEBUG;
	if (viewPosition[0]<-20000) // From -2000 to -20000 on 3/4/2011 // somehow there is a bug I can't put the finger on right now (2009/12/16)
		viewPosition[0]=0;
	if (rebuildHierarchyFlag)
		rebuildHierarchy();
	if (resetViewFlag)
	{
		viewPosition[0]=0;
		viewPosition[1]=0;
		resetViewFlag=false;
	}
	if (refreshViewFlag<=0)
		return(false);
	refreshViewFlag--;


	int editModeType=App::ct->objCont->getEditModeType();
	bool hierarchDragUnderway=false;
	int dx=0;
	int dy=0;
	int dropID=-9999;
	if (editModeType==NO_EDIT_MODE)
	{
		if (_worldSelectID_down!=-9999)
			refreshViewFlag=3;

		if (_mouseDownDragObjectID!=-1)
		{
			if (App::ct->objCont->getLastSelectionID()!=_mouseDownDragObjectID)
				_mouseDownDragObjectID=-1; // we probably pressed esc
			if (_mouseDownDragObjectID!=-1)
			{
				refreshViewFlag=3;
				dx=mouseRelativePosition[0]-mouseDownRelativePosition[0];
				dy=mouseRelativePosition[1]-mouseDownRelativePosition[1];
				hierarchDragUnderway=((abs(dx)>8)||(abs(dy)>8));
				if (hierarchDragUnderway)
				{
					dropID=objectIDWhereTheMouseCurrentlyIs_minus9999ForNone;
					if (dropID==-9999)
						dropID=-App::ct->getCurrentInstanceIndex()-1; // world
					else
					{
						if (dropID<0)
						{
							if (dropID!=-App::ct->getCurrentInstanceIndex()-1)
								dropID=-9999;
						}
					}
				}
			}
		}

		static bool lastHierarchDragUnderway=false;
		static int lastDragUnderwayTime=0;
		static int dragStartTime=0;
		if (hierarchDragUnderway)
		{
			int ct=VDateTime::getTimeInMs();
			if (!lastHierarchDragUnderway)
				dragStartTime=ct;
			if (ct-dragStartTime>1000)
			{
				if (ct-lastDragUnderwayTime>50)
				{
					if (mouseRelativePosition[1]<renderingPosition[1]+(BROWSER_HIERARCHY_TITLE_BAR_HEIGHT+CONST_VAL_40+HIERARCHY_INTER_LINE_SPACE*4)*App::sc) // Bottom border
						viewPosition[1]=viewPosition[1]+HIERARCHY_INTER_LINE_SPACE*App::sc;
					if (mouseRelativePosition[1]<renderingPosition[1]+(BROWSER_HIERARCHY_TITLE_BAR_HEIGHT+CONST_VAL_40+HIERARCHY_INTER_LINE_SPACE*2)*App::sc) // Bottom border
						viewPosition[1]=viewPosition[1]+HIERARCHY_INTER_LINE_SPACE*App::sc;
					if (mouseRelativePosition[1]>renderingPosition[1]+BROWSER_HIERARCHY_TITLE_BAR_HEIGHT*App::sc+renderingSize[1]-HIERARCHY_INTER_LINE_SPACE*4*App::sc) // top border
						viewPosition[1]=viewPosition[1]-HIERARCHY_INTER_LINE_SPACE*App::sc;
					if (mouseRelativePosition[1]>renderingPosition[1]+BROWSER_HIERARCHY_TITLE_BAR_HEIGHT*App::sc+renderingSize[1]-HIERARCHY_INTER_LINE_SPACE*2*App::sc) // top border
						viewPosition[1]=viewPosition[1]-HIERARCHY_INTER_LINE_SPACE*App::sc;

					if (mouseRelativePosition[0]<HIERARCHY_INTER_LINE_SPACE*2*App::sc) // left border
						viewPosition[0]=viewPosition[0]+HIERARCHY_INTER_LINE_SPACE*App::sc;
					if (mouseRelativePosition[0]>renderingSize[0]-HIERARCHY_INTER_LINE_SPACE*2*App::sc) // right border
						viewPosition[0]=viewPosition[0]-HIERARCHY_INTER_LINE_SPACE*App::sc;
					objectIDWhereTheMouseCurrentlyIs_minus9999ForNone=getActionObjectID_icon(0,mouseRelativePosition[1],true);
					lastDragUnderwayTime=ct;
				}
			}
		}
		lastHierarchDragUnderway=hierarchDragUnderway;

	}

	// We draw a black background so that the separation between the hierarchy and scene is rendered correctly on ALL graphic cards:
	glEnable(GL_SCISSOR_TEST);	
	glDisable(GL_DEPTH_TEST);
	glScissor(renderingPosition[0]+SAFETY_BORDER_SIZE*App::sc,renderingPosition[1]+BROWSER_HIERARCHY_TITLE_BAR_HEIGHT*App::sc,renderingSize[0]-SAFETY_BORDER_SIZE*App::sc,renderingSize[1]);
	
	glClearColor(ogl::SEPARATION_LINE_COLOR[0],ogl::SEPARATION_LINE_COLOR[1],ogl::SEPARATION_LINE_COLOR[2],1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	ogl::setColorsAllBlack();
	glEnable(GL_SCISSOR_TEST);	
	glViewport(renderingPosition[0],renderingPosition[1],renderingSize[0],renderingSize[1]);
	ogl::allLightsOffAmbientFull();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f,renderingSize[0],0.0f,renderingSize[1],-1.0f,1.0f);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glDisable(GL_DEPTH_TEST);
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
	scriptIconPosition.clear();
	scriptParametersIconPosition.clear();
	objectPosition.clear();
	objectIconPosition.clear();
	modelIconPosition.clear();
	textPosition.clear();
	lineLastPosition.clear();
	simulationIconPosition.clear();
	if (editModeType==NO_EDIT_MODE)
	{
		for (int i=0;i<int(rootElements.size());i++)
		{
			std::vector<int> vertLines;
			rootElements[i]->renderElement_3DObject(this,labelEditObjectID,bright,true,renderingSize,
				textPos,0,&vertLines,minRenderedPosition,maxRenderedPosition);
		}
		_drawLinesLinkingDummies(maxRenderedPosition);
	}
	std::vector<char> editModeSelectionStateList;
	if (editModeType&SHAPE_OR_PATH_EDIT_MODE)
	{
		if (editModeType&VERTEX_EDIT_MODE)
			editModeSelectionStateList.resize(App::ct->objCont->_editionVertices.size()/3,0);
		if (editModeType&TRIANGLE_EDIT_MODE)
			editModeSelectionStateList.resize(App::ct->objCont->_editionIndices.size()/3,0);
		if (editModeType&EDGE_EDIT_MODE)
			editModeSelectionStateList.resize(App::ct->objCont->_edgeCont.allEdges.size()/2,0);
		if (editModeType&PATH_EDIT_MODE)
			editModeSelectionStateList.resize(App::ct->objCont->_editionPath->getSimplePathPointCount(),0);
		for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
		{
			int ind=App::ct->objCont->editModeBuffer[i];
			if (i==int(App::ct->objCont->editModeBuffer.size())-1)
				editModeSelectionStateList[ind]=2; // last selection
			else
				editModeSelectionStateList[ind]=1;
		}
		for (int i=0;i<int(rootElements.size());i++)
		{
			rootElements[i]->renderElement_editModeList(this,labelEditObjectID,bright,true,renderingSize,
				textPos,0,minRenderedPosition,maxRenderedPosition,editModeSelectionStateList[i],editModeType);
		}
	}
	if (editModeType&BUTTON_EDIT_MODE)
	{
		for (int i=0;i<int(rootElements.size());i++)
		{
			rootElements[i]->renderElement_editModeList(this,labelEditObjectID,bright,true,renderingSize,
				textPos,0,minRenderedPosition,maxRenderedPosition,0,editModeType);
		}
	}

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
	scriptIconPosition.clear();
	scriptParametersIconPosition.clear();
	objectPosition.clear();
	objectIconPosition.clear();
	modelIconPosition.clear();
	textPosition.clear();
	lineLastPosition.clear();
	simulationIconPosition.clear();
	if (editModeType==NO_EDIT_MODE)
	{

		std::vector<CHierarchyElement*> el;
		std::vector<int> elNb;
		for (int i=0;i<int(rootElements.size());i++)
		{ // This is needed to order the elements from most sub-elements to least sub-elements
			int els=rootElements[i]->getNumberOfElements();
			int j;
			for (j=0;j<int(elNb.size());j++)
			{
				if (els>elNb[j])
					break;
			}
			elNb.insert(elNb.begin()+j,els);
			el.insert(el.begin()+j,rootElements[i]);
		}	

		int objFromHalf=-1;
		if (hierarchDragUnderway)
			objFromHalf=_mouseDownDragObjectID;

		int worldClickThing=-9999;
		if (_worldSelectID_down==_worldSelectID_moving)
			worldClickThing=_worldSelectID_down;

		for (int i=0;i<int(el.size());i++)
		{
			std::vector<int> vertLines;
			el[i]->renderElement_3DObject(this,labelEditObjectID,bright,false,renderingSize,
				textPos,0,&vertLines,minRenderedPosition,maxRenderedPosition,false,objFromHalf,dropID,worldClickThing);
		}
		while (CHierarchyElement::renderDummyElement(bright,renderingSize,textPos));
		_drawLinesLinkingDummies(maxRenderedPosition);

		if (hierarchDragUnderway)
		{
			textPos[0]=dx+_mouseDownDragOffset[0];
			textPos[1]=dy+_mouseDownDragOffset[1];

			CHierarchyElement* it=NULL;
			for (int i=0;i<int(el.size());i++)
			{
				it=el[i]->getElementLinkedWithObject(_mouseDownDragObjectID);
				if (it!=NULL)
					break;
			}
			if (it!=NULL)
			{ // should normally never be NULL
				std::vector<int> vertLines;
				int renderingSizeCopy[2]={renderingSize[0],renderingSize[1]};
				int minRenderedPositionCopy[2]={minRenderedPosition[0],minRenderedPosition[1]};
				int maxRenderedPositionCopy[2]={maxRenderedPosition[0],maxRenderedPosition[1]};
				it->renderElement_3DObject(this,labelEditObjectID,bright,false,renderingSizeCopy,
					textPos,0,&vertLines,minRenderedPositionCopy,maxRenderedPositionCopy,true);
			}
		}

	}
	if (editModeType&SHAPE_OR_PATH_EDIT_MODE)
	{
		for (int i=0;i<int(rootElements.size());i++)
		{
			rootElements[i]->renderElement_editModeList(this,labelEditObjectID,bright,false,renderingSize,
				textPos,0,minRenderedPosition,maxRenderedPosition,editModeSelectionStateList[i],editModeType);
		}
		while (CHierarchyElement::renderDummyElement(bright,renderingSize,textPos));
	}
	if (editModeType&BUTTON_EDIT_MODE)
	{
		for (int i=0;i<int(rootElements.size());i++)
		{
			rootElements[i]->renderElement_editModeList(this,labelEditObjectID,bright,false,renderingSize,
				textPos,0,minRenderedPosition,maxRenderedPosition,0,editModeType);
		}
		while (CHierarchyElement::renderDummyElement(bright,renderingSize,textPos));
	}
	maxRenderedPosition[0]=maxRenderedPosition[0]-viewPosition[0];
	maxRenderedPosition[1]=maxRenderedPosition[1]-viewPosition[1];
	minRenderedPosition[0]=minRenderedPosition[0]-viewPosition[0];
	minRenderedPosition[1]=minRenderedPosition[1]-viewPosition[1];
	
	// We now draw the selection square:
	if (shiftSelectionStarted)
	{
		int limitedPos[2]={mouseRelativePosition[0],mouseRelativePosition[1]};
		if (limitedPos[0]>renderingSize[0])
			limitedPos[0]=renderingSize[0];
		if (limitedPos[1]>renderingSize[1])
			limitedPos[1]=renderingSize[1];
		if (limitedPos[0]<SAFETY_BORDER_SIZE*App::sc)
			limitedPos[0]=SAFETY_BORDER_SIZE*App::sc;
		if (limitedPos[1]<SAFETY_BORDER_SIZE*App::sc)
			limitedPos[1]=SAFETY_BORDER_SIZE*App::sc;
		float black[3]={0.0f,0.0f,0.0f};
		float yellow[3]={1.0f,1.0f,0.0f};
		ogl::setColor(yellow,black,black,black,12,0.2f);
		ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_QUADS);
		glVertex3i(mouseDownRelativePosition[0],mouseDownRelativePosition[1],0);
		glVertex3i(mouseDownRelativePosition[0],limitedPos[1],0);
		glVertex3i(limitedPos[0],limitedPos[1],0);
		glVertex3i(limitedPos[0],mouseDownRelativePosition[1],0);
		glEnd();
		ogl::setBlending(false);			
		ogl::setColor(0.0f,0.0f,0.0f,AMBIENT_MODE);
		glBegin(GL_LINE_STRIP);
		glVertex3i(mouseDownRelativePosition[0],mouseDownRelativePosition[1],0);
		glVertex3i(mouseDownRelativePosition[0],limitedPos[1],0);
		glVertex3i(limitedPos[0],limitedPos[1],0);
		glVertex3i(limitedPos[0],mouseDownRelativePosition[1],0);
		glVertex3i(mouseDownRelativePosition[0],mouseDownRelativePosition[1],0);
		glEnd();
	}
	int vtl[2];
	int vbr[2];
	int htl[2];
	int hbr[2];
	int slidersEnable=getSliderPositions(vtl,vbr,htl,hbr,NULL);
//	float black[3]={0.0f,0.0f,0.0f};

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

int CHierarchy::getSliderPositions(int vSliderTopLeft[2],int vSliderBottomRight[2],int hSliderTopLeft[2],int hSliderBottomRight[2],float prop[2])
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
		int effDr=renderingSize[1]-horizontalScrollbarHeight-SAFETY_BORDER_SIZE*App::sc;//-4;
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
		int effDr=renderingSize[0]-verticalScrollbarWidth+(-SAFETY_BORDER_SIZE-CONST_VAL_4)*App::sc;
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

bool CHierarchy::leftMouseDown(int x,int y,int selectionStatus)
{
	_mouseDownDragObjectID=-1;
	_worldSelectID_down=-9999;
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
	shiftSelectionStarted=false;
	labelEditObjectID=-1;

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
	bool clickSelection=false;
	if ( (selectionStatus!=CTRLSELECTION)&&(selectionStatus!=SHIFTSELECTION) )
	{
			clickSelection=true;
	}

	shiftingAllowed=( (selectionStatus!=CTRLSELECTION)&&(selectionStatus!=SHIFTSELECTION) );
	int objID=getInflateActionObjectID(mouseDownRelativePosition[0],mouseDownRelativePosition[1]);
	bool canSelect=true;
	if ((objID>=0)&&(selectionStatus!=CTRLSELECTION)&&(selectionStatus!=SHIFTSELECTION))
	{ // Expansion/collapse
		C3DObject* it=App::ct->objCont->getObject(objID);
		if (it!=NULL)
		{
			it->setObjectProperty(it->getObjectProperty()^sim_objectproperty_collapsed);
			shiftingAllowed=false;
			canSelect=false;
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
	}
	if ( (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)&&canSelect )
	{ // NO EDIT MODE
		if ( (selectionStatus==CTRLSELECTION)||clickSelection )
		{
			int objID=getActionObjectID(mouseDownRelativePosition[1],HIERARCHY_HALF_INTER_LINE_SPACE*App::sc);
			if (objID>=0)
			{
				C3DObject* obj=App::ct->objCont->getObject(objID);
				if (obj!=NULL) // just in case
				{
					if (selectionStatus==CTRLSELECTION)
					{
						App::ct->objCont->xorAddObjectToSelection(objID); // Normal selection/deselection
					}
					else
					{
						App::ct->objCont->deselectObjects();
						App::ct->objCont->addObjectToSelection(objID); // Normal selection
						int dxv[2];
						if (getLineObjectID(mouseDownRelativePosition[1],dxv)==objID)
						{
							_mouseDownDragObjectID=objID;
							_mouseDownDragOffset[0]=dxv[0];
							_mouseDownDragOffset[1]=dxv[1];
						}
					}
				}
			}
			else
			{
				if (objID==-9999)
					App::ct->objCont->deselectObjects();
				else
				{
					_worldSelectID_down=objID;
					_worldSelectID_moving=objID;
				}
			}
		}
		if (selectionStatus==SHIFTSELECTION)
			shiftSelectionStarted=true;
	}
	if ( (App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)&&canSelect )
	{ // SHAPE OR PATH EDIT MODE
		if ( (selectionStatus==CTRLSELECTION)||clickSelection )
		{
			int objID=getActionObjectID(mouseDownRelativePosition[1],HIERARCHY_HALF_INTER_LINE_SPACE*App::sc);
			if (objID>=0)
			{
				if (selectionStatus==CTRLSELECTION)
					App::ct->objCont->xorAddItemToEditModeBuffer(objID);
				else
				{
					App::ct->objCont->deselectEditModeBuffer();
					App::ct->objCont->addItemToEditModeBuffer(objID);
				}
			}
			else
				App::ct->objCont->deselectEditModeBuffer();
		}
		if (selectionStatus==SHIFTSELECTION)
			shiftSelectionStarted=true;
	}
	if ( (App::ct->objCont->getEditModeType()&BUTTON_EDIT_MODE)&&canSelect )
	{ // BUTTON EDIT MODE
		if ( (selectionStatus==CTRLSELECTION)||clickSelection )
		{
			int objID=getActionObjectID(mouseDownRelativePosition[1],HIERARCHY_HALF_INTER_LINE_SPACE*App::sc);
			App::ct->buttonBlockContainer->setBlockInEdition(objID);
			//App::ct->buttonBlockContainer->sendBlockToFront(objID);
			App::ct->objCont->setLightDialogRefreshFlag();
		}
	}
	return(true);
}

void CHierarchy::leftMouseUp(int x,int y)
{
	int dx=(x+SAFETY_BORDER_SIZE*App::sc)-mouseDownRelativePosition[0];
	int dy=(y+SAFETY_BORDER_SIZE*App::sc)-mouseDownRelativePosition[1];
	bool hierarchDragUnderway=((abs(dx)>8)||(abs(dy)>8));
	if ((_mouseDownDragObjectID!=-1)&&hierarchDragUnderway)
	{
		if (App::ct->objCont->getLastSelectionID()==_mouseDownDragObjectID)
		{

			if (objectIDWhereTheMouseCurrentlyIs_minus9999ForNone==-9999)
				objectIDWhereTheMouseCurrentlyIs_minus9999ForNone=-App::ct->getCurrentInstanceIndex()-1; // world
			else
			{
				if (objectIDWhereTheMouseCurrentlyIs_minus9999ForNone<0)
				{
					if (objectIDWhereTheMouseCurrentlyIs_minus9999ForNone!=-App::ct->getCurrentInstanceIndex()-1)
						objectIDWhereTheMouseCurrentlyIs_minus9999ForNone=-9999;
				}
			}

			if ((objectIDWhereTheMouseCurrentlyIs_minus9999ForNone!=_mouseDownDragObjectID)&&(objectIDWhereTheMouseCurrentlyIs_minus9999ForNone!=-9999))
			{
				if (objectIDWhereTheMouseCurrentlyIs_minus9999ForNone<0)
					App::ct->objCont->makeObjectChildOf(App::ct->objCont->getObject(_mouseDownDragObjectID),NULL);
				else
					App::ct->objCont->makeObjectChildOf(App::ct->objCont->getObject(_mouseDownDragObjectID),App::ct->objCont->getObject(objectIDWhereTheMouseCurrentlyIs_minus9999ForNone));
				App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
			}
		}
	}

	_mouseDownDragObjectID=-1;
	sliderMoveMode=0;
	refreshViewFlag=3;
	mouseRelativePosition[0]=x+SAFETY_BORDER_SIZE*App::sc;
	mouseRelativePosition[1]=y+SAFETY_BORDER_SIZE*App::sc;


	if (shiftSelectionStarted&&(_caughtElements&sim_left_button))
	{ // SHIFT SELECTION
		int limitedPos=mouseRelativePosition[1];
		if (limitedPos>renderingSize[1])
			limitedPos=renderingSize[1];
		if (limitedPos<SAFETY_BORDER_SIZE*App::sc)
			limitedPos=SAFETY_BORDER_SIZE*App::sc;
		std::vector<int> objToBeSelected;
		getActionObjectIDs(mouseDownRelativePosition[1],limitedPos,HIERARCHY_HALF_INTER_LINE_SPACE*App::sc,&objToBeSelected);
		if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
		{
			for (int i=0;i<int(objToBeSelected.size());i++)
			{
				C3DObject* obj=App::ct->objCont->getObject(objToBeSelected[i]);
				if (obj!=NULL) // Just in case
				{
					// Following is problematic if we want to build a parent-child relationship with a
					// collapsed object --> all its children will become direct children if we are not careful!
					//if (obj->getCollapsed())
					//{ // selection of the tree
					//	std::vector<C3DObject*> objList;
					//	obj->getAllObjectsRecursive(&objList,true,true);
					//	for (int ll=int(objList.size())-1;ll>=0;ll--)
					//		App::ct->objCont->addObjectToSelection(objList[ll]->getID());
					//}
					//else
						App::ct->objCont->addObjectToSelection(objToBeSelected[i]); // Normal selection
				}
			}
		}
		if (App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)
		{
			for (int i=0;i<int(objToBeSelected.size());i++)
				App::ct->objCont->addItemToEditModeBuffer(objToBeSelected[i]);
		}
	}
	_caughtElements&=0xffff-sim_left_button;
	shiftingAllowed=false;
	shiftSelectionStarted=false;

	// We have to do this at the very end of the routine since we are switching instances:
	if ((_worldSelectID_down!=-9999)&&(_worldSelectID_down==getActionObjectID(mouseRelativePosition[1],HIERARCHY_HALF_INTER_LINE_SPACE*App::sc)))
	{
		int nii=(-_worldSelectID_down)-1;
		_worldSelectID_down=-9999;
		App::ct->evaluateMenuSelection(SWITCH_TO_INSTANCE_INDEX0_CMD+nii);
	}

}
bool CHierarchy::rightMouseDown(int x,int y)
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
	objectIDWhereTheMouseCurrentlyIs_minus9999ForNone=getActionObjectID_icon(0,mouseRelativePosition[1],true);
	labelEditObjectID=-1;
	return(true); // We catch this event to display a popup-menu when the mouse comes up
}
void CHierarchy::rightMouseUp(int x,int y,int absX,int absY,QWidget* mainWindow)
{ // Only caught if right button was caught by the hierarchy!
	_caughtElements&=0xffff-sim_right_button;
	if ( (x<0)||(y<0)||(x>renderingSize[0]-SAFETY_BORDER_SIZE*App::sc)||(y>renderingSize[1]-SAFETY_BORDER_SIZE*App::sc) )
		return;

	// The mouse went up on the hierarchy window
	refreshViewFlag=3;
	mouseRelativePosition[0]=x+SAFETY_BORDER_SIZE*App::sc;
	mouseRelativePosition[1]=y+SAFETY_BORDER_SIZE*App::sc;

	if (App::operationalUIParts&sim_gui_popups)
	{ // Default popups
		if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
		{
			VMenu* mainMenu=VMenubar::createPopupMenu();
			addMenu(mainMenu);

			VMenu* objectEditionMenu=VMenubar::createPopupMenu();
			CObjectEdition::addMenu(objectEditionMenu);
			VMenubar::appendMenuAndDetach(mainMenu,objectEditionMenu,true,IDS_EDIT_MENU_ITEM);

			VMenu* addMenu=VMenubar::createPopupMenu();
			CAddMenu::addMenu(addMenu,NULL,false);
			VMenubar::appendMenuAndDetach(mainMenu,addMenu,true,IDS_ADD_MENU_ITEM);

			int selSize=App::ct->objCont->getSelSize();
			VMenu* hierarchyColoringMenu=VMenubar::createPopupMenu();
			bool cols[4]={false,false,false,false};
			if (selSize>0)
			{
				for (int i=0;i<selSize;i++)
				{
					C3DObject* anO=App::ct->objCont->getObject(App::ct->objCont->getSelID(i));
					int colInd=anO->getHierarchyColorIndex();
					if (colInd==-1)
						cols[0]=true;
					if (colInd==0)
						cols[1]=true;
					if (colInd==1)
						cols[2]=true;
					if (colInd==2)
						cols[3]=true;
				}
				VMenubar::appendMenuItem(hierarchyColoringMenu,true,cols[0],HIERARCHY_COLORING_NONE_CMD,IDSN_HIERARCHY_COLORING_NONE_MENU_ITEM,true);
				VMenubar::appendMenuItem(hierarchyColoringMenu,true,cols[1],HIERARCHY_COLORING_RED_CMD,IDSN_HIERARCHY_COLORING_RED_MENU_ITEM,true);
				VMenubar::appendMenuItem(hierarchyColoringMenu,true,cols[2],HIERARCHY_COLORING_GREEN_CMD,IDSN_HIERARCHY_COLORING_GREEN_MENU_ITEM,true);
				VMenubar::appendMenuItem(hierarchyColoringMenu,true,cols[3],HIERARCHY_COLORING_BLUE_CMD,IDSN_HIERARCHY_COLORING_BLUE_MENU_ITEM,true);
			}
			VMenubar::appendMenuAndDetach(mainMenu,hierarchyColoringMenu,selSize>0,IDSN_HIERARCHY_COLORING_MENU_ITEM);


			int command=VMenubar::trackPopupMenu(mainMenu,absX,absY,mainWindow);
			bool processed=false;
			if (!processed)
				processed=evaluateMenuSelection(command);
			if (!processed)
				processed=CObjectEdition::evaluateMenuSelection(command);
			if (!processed)
				processed=CAddMenu::evaluateMenuSelection(command,NULL);
			delete mainMenu;
		} 
		else
		{
			VMenu* mainMenu=VMenubar::createPopupMenu();

			if (App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)
				CTriangleVertexEdition::addMenu(mainMenu);
			if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
				CPathEdition::addMenu(mainMenu,NULL);
			if (App::ct->objCont->getEditModeType()&BUTTON_EDIT_MODE)
				App::ct->buttonBlockContainer->addMenu(mainMenu);

			int command=VMenubar::trackPopupMenu(mainMenu,absX,absY,mainWindow);

			if (App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)
				CTriangleVertexEdition::evaluateMenuSelection(command);
			if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
				CPathEdition::evaluateMenuSelection(command,NULL);
			if (App::ct->objCont->getEditModeType()&BUTTON_EDIT_MODE)
				App::ct->buttonBlockContainer->evaluateMenuSelection(command);
			delete mainMenu;
		}
	}
}

bool CHierarchy::mouseWheel(int deltaZ,int x,int y)
{
	if ( (x<0)||(y<0)||(x>renderingSize[0]-SAFETY_BORDER_SIZE*App::sc)||(y>renderingSize[1]-SAFETY_BORDER_SIZE*App::sc) )
		return(false);
	refreshViewFlag=3;
	viewPosition[1]=viewPosition[1]-(deltaZ/120)*HIERARCHY_INTER_LINE_SPACE*App::sc;
	return(true);
}

void CHierarchy::mouseMove(int x,int y,bool passiveAndFocused)
{
	_worldSelectID_moving=-9999;
	mouseRelativePosition[0]=x+SAFETY_BORDER_SIZE*App::sc;
	mouseRelativePosition[1]=y+SAFETY_BORDER_SIZE*App::sc;
	objectIDWhereTheMouseCurrentlyIs_minus9999ForNone=getActionObjectID_icon(0,mouseRelativePosition[1],true);
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
			int objID=getActionObjectID(mouseRelativePosition[1],HIERARCHY_HALF_INTER_LINE_SPACE*App::sc);
			if (objID<0)
				_worldSelectID_moving=objID;
		}
		else if (shiftSelectionStarted)
			refreshViewFlag=3;
	}
	previousMouseRelativePosition[0]=mouseRelativePosition[0];
	previousMouseRelativePosition[1]=mouseRelativePosition[1];
}

bool CHierarchy::leftMouseDblClick(int x,int y,int selectionStatus)
{
	// Following few on 23/9/2013, because somehow with Qt5 there is no mouse up event following:
	_mouseDownDragObjectID=-1;
	_worldSelectID_down=-9999;
	sliderMoveMode=0;
	_caughtElements&=0xffff-sim_left_button;


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
	shiftSelectionStarted=false;
	shiftingAllowed=false;
	// We check if we have to launch a script editor window:
	int scriptID=getScriptActionObjectID(mouseDownRelativePosition[0],mouseDownRelativePosition[1]);
	if (scriptID!=-1)
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptID);
		if (it!=NULL)
		{
			bool openScriptEditor=true;
			int auxData[4]={it->getObjectIDThatScriptIsAttachedTo(),-1,-1,-1};
			int retVals[4]={-1,-1,-1,-1};
			void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_scripticondblclick,auxData,NULL,retVals);
			delete[] (char*)returnVal;
			openScriptEditor=(retVals[0]!=1);
			if (openScriptEditor)
				App::ct->scintillaEditorContainer->openEditorForScript(it->getScriptID());
		}
		return(true);
	}
	scriptID=getScriptParameterActionObjectID(mouseDownRelativePosition[0],mouseDownRelativePosition[1]);
	if (scriptID!=-1)
	{
		CLuaScriptObject* it=App::ct->luaScriptContainer->getScriptFromID(scriptID);
		if ( (it!=NULL)&&((App::operationalUIParts&sim_gui_scriptsimulationparameters)!=0) )
		{
			IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
			{
				CQDlgScriptParameters theDialog(App::mainWindow);
				theDialog.script=it;
				theDialog.refresh();
				theDialog.makeDialogModal();
			}
		}
		return(true);
	}

	if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
	{
		// Do we need to open an object property dialog?
		int objID=getActionObjectID_icon(mouseDownRelativePosition[0],mouseDownRelativePosition[1]);
		if (objID!=-9999) // minus numbers are for the world(s)
		{ // yes!
			if (objID>=0)
			{ // Regular object
				App::ct->objCont->deselectObjects();
				App::ct->objCont->addObjectToSelection(objID);
				App::ct->objCont->setFullDialogRefreshFlag();
				App::mainWindow->dlgCont->evaluateMenuSelection(OPEN_OBJECT_DLG_OBJECT_SPECIFIC_PART_CMD);
			}
			else
			{ // World object!
				App::mainWindow->dlgCont->evaluateMenuSelection(OPEN_ENVIRONMENT_DLG_CMD);
			}
			return(true);
		}

		// Do we need to open a model property dialog?
		objID=getActionModelID_icon(mouseDownRelativePosition[0],mouseDownRelativePosition[1]);
		if (objID>=0)
		{ // yes!
			C3DObject* it=App::ct->objCont->getObject(objID);
			if (it!=NULL)
			{
				IF_UI_EVENT_PAUSE_SIM_THREAD_FOR_MODAL_DLG
				{
					CQDlgModelProperties theDialog(App::mainWindow);
					theDialog.modelBaseObject=it;
					theDialog.acknowledgment=it->getModelAcknowledgement();
					int originalMainPropertyOverride=it->getMainPropertyOverride();
					CThumbnail* originalThmb=App::ct->environment->modelThumbnail_notSerializedHere.copyYourself();
					theDialog.refresh();
					if (theDialog.makeDialogModal()!=VDIALOG_MODAL_RETURN_CANCEL)
					{
						it->setModelAcknowledgement(theDialog.acknowledgment);
						// The rest was directly modified!
					}
					else
					{ // we have to restore a few values:
						it->setMainPropertyOverride(originalMainPropertyOverride);
						App::ct->environment->modelThumbnail_notSerializedHere.copyFrom(originalThmb);
					}
					delete originalThmb;
					App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
					App::ct->objCont->setFullDialogRefreshFlag();
				}
			}
			return(true);
		}

		// Do we need to open a warning message?
		objID=getSimulationActionObjectID(mouseDownRelativePosition[0],mouseDownRelativePosition[1]);
		if (objID!=-1)
		{ // yes!
			C3DObject* it=App::ct->objCont->getObject(objID);
			if (it!=NULL)
			{
				std::string txt;
				if (it->getDynamicSimulationIconCode()==DYNAMIC_SIMULATION_ICON_OBJECT_IS_NOT_DYNAMICALLY_ENABLED)
				{
					txt=IDS_OBJECT;
					txt+=" '"+it->getName()+"' ";
					txt+=IDS_IS_NOT_DYNAMICALLY_ENABLED_WARNING;
				}
				if (it->getDynamicSimulationIconCode()==DYNAMIC_SIMULATION_ICON_OBJECT_IS_DYNAMICALLY_SIMULATED)
				{
					txt=IDS_OBJECT;
					txt+=" '"+it->getName()+"' ";
					txt+=std::string(IDS_IS_DYNAMICALLY_SIMULATED)+"\n";
					if (it->getObjectType()==sim_object_shape_type)
					{
						CShape* so=(CShape*)it;
						if (so->getShapeIsDynamicallyStatic())
						{ // static
							if (so->getRespondable())
								txt+=IDS_SHAPE_IS_STATIC_AND_RESPONDABLE;
							else
								txt+=IDS_SHAPE_IS_STATIC_AND_NON_RESPONDABLE;
						}
						else
						{ // non-static
							if (so->getRespondable())
								txt+=IDS_SHAPE_IS_NON_STATIC_AND_RESPONDABLE;
							else
								txt+=IDS_SHAPE_IS_NON_STATIC_AND_NON_RESPONDABLE;
						}
					}
					if (it->getObjectType()==sim_object_joint_type)
					{
						CJoint* so=(CJoint*)it;
						if (so->getJointMode()==sim_jointmode_force)
							txt+=IDS_JOINT_OPERATES_IN_FORCE_TORQUE_MODE;
						else
							txt+=IDS_JOINT_OPERATES_IN_HYBRID_FASHION;
						txt+=' ';
						if (so->getEnableDynamicMotor())
						{
							if (so->getEnableDynamicMotorControlLoop())
								txt+=IDS__CONTROL_LOOP_ENABLED_;
							else
								txt+=IDS__CONTROL_LOOP_DISABLED_;
						}
						else
							txt+=IDS__MOTOR_DISABLED_;
					}
					if (it->getObjectType()==sim_object_forcesensor_type)
					{
						CForceSensor* so=(CForceSensor*)it;
						if (so->getEnableForceThreshold()||so->getEnableTorqueThreshold())
							txt+=IDS_FORCE_SENSOR_IS_BREAKABLE;
						else
							txt+=IDS_FORCE_SENSOR_IS_NOT_BREAKABLE;
					}
				}
				if (txt!="")
					App::uiThread->messageBox_information(App::mainWindow,strTranslate("Dynamic property"),txt,VMessageBox::OKELI);
			}
			return(true);
		}

		// Do we need to do some label editing?
		objID=getTextActionObjectID(mouseDownRelativePosition[0],mouseDownRelativePosition[1]);
		if (objID==-9999)
			objID=-1;
		labelEditObjectID=objID;
		C3DObject* it=App::ct->objCont->getObject(objID);
		if (it!=NULL)
		{
			editionText=it->getName();
			editionTextEditPos=editionText.length();
		}
		return(true);
	}
	if (App::ct->objCont->getEditModeType()&BUTTON_EDIT_MODE)
	{
		// Did we double-click the icon?
		int objID=getActionObjectID_icon(mouseDownRelativePosition[0],mouseDownRelativePosition[1]);
		if (objID!=-9999)
		{ // yes!
			App::ct->buttonBlockContainer->setBlockInEdition(objID);
			App::ct->objCont->setFullDialogRefreshFlag();
			App::mainWindow->dlgCont->evaluateMenuSelection(OPEN_BUTTON_DLG_CMD);
			return(true);
		}
		objID=getTextActionObjectID(mouseDownRelativePosition[0],mouseDownRelativePosition[1]);
		if (objID==-9999)
			objID=-1;
		labelEditObjectID=objID;
		CButtonBlock* it=App::ct->buttonBlockContainer->getBlockWithID(objID);
		if (it!=NULL)
		{
			editionText=it->getBlockName();
			editionTextEditPos=editionText.length();
		}
		return(true);
	}
	if (App::ct->objCont->getEditModeType()&(VERTEX_EDIT_MODE|PATH_EDIT_MODE))
	{
		// Did we double-click the icon?
		int objID=getActionObjectID_icon(mouseDownRelativePosition[0],mouseDownRelativePosition[1]);
		if (objID!=-9999)
		{ // yes!
			App::ct->objCont->deselectEditModeBuffer();
			App::ct->objCont->addItemToEditModeBuffer(objID);
			App::ct->objCont->setFullDialogRefreshFlag();
			App::mainWindow->dlgCont->evaluateMenuSelection(OPEN_TRANSFORMATIONS_DLG_CMD);
			return(true);
		}
	}
	return(false);
}
void CHierarchy::validateViewPosition()
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
void CHierarchy::setRebuildHierarchyFlag()
{
	rebuildHierarchyFlag=true;
	refreshViewFlag=3;
}
void CHierarchy::setResetViewFlag()
{
	resetViewFlag=true;
}
void CHierarchy::setRefreshViewFlag()
{
	refreshViewFlag=3;
}

int CHierarchy::getInflateActionObjectID(int mousePositionX,int mousePositionY)
{
	for (int i=0;i<int(inflateIconPosition.size())/3;i++)
	{
		if ((mousePositionX>=inflateIconPosition[3*i+0]-HIERARCHY_ICON_QUARTER_WIDTH*App::sc)&&(mousePositionX<=inflateIconPosition[3*i+0]+HIERARCHY_ICON_QUARTER_WIDTH*App::sc) )
		{
			if ((mousePositionY>=inflateIconPosition[3*i+1]-HIERARCHY_ICON_QUARTER_HEIGHT*App::sc)&&(mousePositionY<=inflateIconPosition[3*i+1]+HIERARCHY_ICON_QUARTER_HEIGHT*App::sc) )
				return(inflateIconPosition[3*i+2]);
		}
	}
	return(-1);
}

int CHierarchy::getScriptActionObjectID(int mousePositionX,int mousePositionY)
{
	for (int i=0;i<int(scriptIconPosition.size())/3;i++)
	{
		if ((mousePositionX>=scriptIconPosition[3*i+0]-HIERARCHY_ICON_HALF_WIDTH*App::sc)&&(mousePositionX<=scriptIconPosition[3*i+0]+HIERARCHY_ICON_HALF_WIDTH*App::sc) )
		{
			if ((mousePositionY>=scriptIconPosition[3*i+1]-HIERARCHY_ICON_HALF_HEIGHT*App::sc)&&(mousePositionY<=scriptIconPosition[3*i+1]+HIERARCHY_ICON_HALF_HEIGHT*App::sc) )
				return(scriptIconPosition[3*i+2]);
		}
	}
	return(-1);
}
int CHierarchy::getScriptParameterActionObjectID(int mousePositionX,int mousePositionY)
{
	for (int i=0;i<int(scriptParametersIconPosition.size())/3;i++)
	{
		if ((mousePositionX>=scriptParametersIconPosition[3*i+0]-HIERARCHY_ICON_HALF_WIDTH*App::sc)&&(mousePositionX<=scriptParametersIconPosition[3*i+0]+HIERARCHY_ICON_HALF_WIDTH*App::sc) )
		{
			if ((mousePositionY>=scriptParametersIconPosition[3*i+1]-HIERARCHY_ICON_HALF_HEIGHT*App::sc)&&(mousePositionY<=scriptParametersIconPosition[3*i+1]+HIERARCHY_ICON_HALF_HEIGHT*App::sc) )
				return(scriptParametersIconPosition[3*i+2]);
		}
	}
	return(-1);
}

int CHierarchy::getTextActionObjectID(int mousePositionX,int mousePositionY)
{
	for (int i=0;i<int(textPosition.size())/6;i++)
	{
		if ((mousePositionX>=textPosition[6*i+2])&&(mousePositionX<=textPosition[6*i+3]) )
		{
			if ((mousePositionY>=textPosition[6*i+4]-HIERARCHY_HALF_INTER_LINE_SPACE*App::sc)&&(mousePositionY<=textPosition[6*i+4]+HIERARCHY_HALF_INTER_LINE_SPACE*App::sc) )
				return(textPosition[6*i+5]);
		}
	}
	return(-9999); // minus number are for worlds (also in different instances)
}

int CHierarchy::getLineObjectID(int mousePositionY,int textPosStart[2])
{
	for (int i=0;i<int(textPosition.size())/6;i++)
	{
		if ((mousePositionY>=textPosition[6*i+4]-HIERARCHY_HALF_INTER_LINE_SPACE*App::sc)&&(mousePositionY<=textPosition[6*i+4]+HIERARCHY_HALF_INTER_LINE_SPACE*App::sc) )
		{
			if (textPosStart!=NULL)
			{
				textPosStart[0]=textPosition[6*i+0];
				textPosStart[1]=textPosition[6*i+1];
			}
			return(textPosition[6*i+5]);
		}
	}
	return(-9999); // minus number are for worlds (also in different instances)
}

int CHierarchy::getActionObjectID_icon(int mousePositionX,int mousePositionY,bool ignoreXPositionAndCheckLineInstead/*=false*/)
{
	if (!ignoreXPositionAndCheckLineInstead)
	{
		for (int i=0;i<int(objectIconPosition.size())/3;i++)
		{
			if ( (mousePositionX>=objectIconPosition[3*i+0]-HIERARCHY_ICON_HALF_WIDTH*App::sc)&&(mousePositionX<=objectIconPosition[3*i+0]+HIERARCHY_ICON_HALF_WIDTH*App::sc) )
			{
				if ((mousePositionY>=objectIconPosition[3*i+1]-HIERARCHY_ICON_HALF_HEIGHT*App::sc)&&(mousePositionY<=objectIconPosition[3*i+1]+HIERARCHY_ICON_HALF_HEIGHT*App::sc) )
					return(objectIconPosition[3*i+2]);
			}
		}
	}
	else
	{
		for (int i=0;i<int(objectIconPosition.size())/3;i++)
		{
			if ((mousePositionY>=objectIconPosition[3*i+1]-HIERARCHY_HALF_INTER_LINE_SPACE*App::sc)&&(mousePositionY<=objectIconPosition[3*i+1]+HIERARCHY_HALF_INTER_LINE_SPACE*App::sc) )
				return(objectIconPosition[3*i+2]);
		}
	}
	return(-9999); // minus number are for worlds (also in different instances)
}

int CHierarchy::getActionModelID_icon(int mousePositionX,int mousePositionY)
{
	for (int i=0;i<int(modelIconPosition.size())/3;i++)
	{
		if ((mousePositionX>=modelIconPosition[3*i+0]-HIERARCHY_ICON_QUARTER_WIDTH*App::sc)&&(mousePositionX<=modelIconPosition[3*i+0]+HIERARCHY_ICON_QUARTER_WIDTH*App::sc) )
		{
			if ((mousePositionY>=modelIconPosition[3*i+1]-HIERARCHY_ICON_QUARTER_HEIGHT*App::sc)&&(mousePositionY<=modelIconPosition[3*i+1]+HIERARCHY_ICON_QUARTER_HEIGHT*App::sc) )
				return(modelIconPosition[3*i+2]);
		}
	}
	return(-9999); // minus number are for worlds (also in different instances)
}

int CHierarchy::getSimulationActionObjectID(int mousePositionX,int mousePositionY)
{
	for (int i=0;i<int(simulationIconPosition.size())/3;i++)
	{
		if ((mousePositionX>=simulationIconPosition[3*i+0]-HIERARCHY_ICON_HALF_WIDTH*App::sc)&&(mousePositionX<=simulationIconPosition[3*i+0]+HIERARCHY_ICON_HALF_WIDTH*App::sc) )
		{
			if ((mousePositionY>=simulationIconPosition[3*i+1]-HIERARCHY_ICON_HALF_HEIGHT*App::sc)&&(mousePositionY<=simulationIconPosition[3*i+1]+HIERARCHY_ICON_HALF_HEIGHT*App::sc) )
				return(simulationIconPosition[3*i+2]);
		}
	}
	return(-1);
}

int CHierarchy::getActionObjectID(int mousePositionY,int tolerance)
{
	for (int i=0;i<int(objectPosition.size())/2;i++)
	{
		if ((mousePositionY<=objectPosition[2*i+0]+tolerance)&&(mousePositionY>=objectPosition[2*i+0]-tolerance) )
			return(objectPosition[2*i+1]);
	}
	return(-9999);
}

void CHierarchy::getActionObjectIDs(int mouseDownPositionY,int mouseUpPositionY,int tolerance,std::vector<int>* toBeSelected)
{
	int minV=mouseUpPositionY-tolerance;
	int maxV=mouseDownPositionY+tolerance;
	if (mouseDownPositionY<mouseUpPositionY)
	{
		minV=mouseDownPositionY-tolerance;
		maxV=mouseUpPositionY+tolerance;
	}
	for (int i=0;i<int(objectPosition.size())/2;i++)
	{
		if ((maxV>=objectPosition[2*i+0])&&(minV<=objectPosition[2*i+0]) )
			toBeSelected->push_back(objectPosition[2*i+1]);
	}
}

std::string CHierarchy::getEditedLabel()
{
	return(editionText);
}

void CHierarchy::setEditionLabel(std::string txt)
{
	editionText=txt;
}

void CHierarchy::drawEditionLabel(int textPosX,int textPosY)
{
	int buttonWidth=20+ogl::getTextLengthInPixels(editionText);
	VPoint p(textPosX-2+buttonWidth/2,textPosY+HIERARCHY_TEXT_CENTER_OFFSET*App::sc);
	VPoint s(buttonWidth,HIERARCHY_INTER_LINE_SPACE*App::sc);
	float txtCol[3]={0.0f,0.0f,0.0f};
	float backCol[3]={1.0f,1.0f,0.0f};
	int buttonAttrib=sim_buttonproperty_editbox|sim_buttonproperty_enabled|sim_buttonproperty_verticallycentered;
	ogl::drawButton(p,s,txtCol,backCol,backCol,editionText,buttonAttrib,true,editionTextEditPos,0.0f,false,VDateTime::getTimeInMs(),NULL,NULL,NULL,NULL,NULL);

	refreshViewFlag=1;
}

void CHierarchy::addMenu(VMenu* menu)
{ 
	bool selection=App::ct->objCont->getSelSize()>0;
	VMenubar::appendMenuItem(menu,true,false,EXPAND_HIERARCHY_CMD,IDS_EXPAND_ALL_MENU_ITEM);
	VMenubar::appendMenuItem(menu,true,false,COLLAPSE_HIERARCHY_CMD,IDS_COLLAPSE_ALL_MENU_ITEM);
	VMenubar::appendMenuItem(menu,selection,false,EXPAND_SELECTED_HIERARCHY_CMD,IDS_EXPAND_SELECTED_TREE_MENU_ITEM);
	VMenubar::appendMenuItem(menu,selection,false,COLLAPSE_SELECTED_HIERARCHY_CMD,IDS_COLLAPSE_SELECTED_TREE_MENU_ITEM);
}

bool CHierarchy::evaluateMenuSelection(int commandID)
{ // Return value is true if the command belonged to Add menu and was executed
	if ( (commandID==EXPAND_HIERARCHY_CMD)||(commandID==COLLAPSE_HIERARCHY_CMD) )
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("EXPAND...COLLAPSE_HIERARCHY_CMD")
		{
			for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
			{
				C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
				it->setObjectProperty(it->getObjectProperty()|sim_objectproperty_collapsed);
				if (commandID==EXPAND_HIERARCHY_CMD)
					it->setObjectProperty(it->getObjectProperty()-sim_objectproperty_collapsed);
			}
			if (commandID==EXPAND_HIERARCHY_CMD)
				App::uiThread->addStatusLine(IDSNS_HIERARCHY_EXPANDED);
			else
				App::uiThread->addStatusLine(IDSNS_HIERARCHY_COLLAPSED);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		return(true);
	}
	if ( (commandID==EXPAND_SELECTED_HIERARCHY_CMD)||(commandID==COLLAPSE_SELECTED_HIERARCHY_CMD) )
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("EXPAND...COLLAPSE_SELECTED_HIERARCHY_CMD")
		{
			for (int i=0;i<int(App::ct->objCont->getSelSize());i++)
			{
				C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->getSelID(i));
				std::vector<C3DObject*> toExplore;
				toExplore.push_back(it);
				while (toExplore.size()!=0)
				{
					it=toExplore[0];
					it->setObjectProperty(it->getObjectProperty()|sim_objectproperty_collapsed);
					if (commandID==EXPAND_SELECTED_HIERARCHY_CMD)
						it->setObjectProperty(it->getObjectProperty()-sim_objectproperty_collapsed);
					toExplore.erase(toExplore.begin());
					for (int j=0;j<int(it->childList.size());j++)
						toExplore.push_back(it->childList[j]);
				}
			}
			if (commandID==EXPAND_HIERARCHY_CMD)
				App::uiThread->addStatusLine(IDSNS_HIERARCHY_TREES_EXPANDED);
			else
				App::uiThread->addStatusLine(IDSNS_HIERARCHY_TREES_COLLAPSED);
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		return(true);
	}
	if ( (commandID>=HIERARCHY_COLORING_NONE_CMD)&&(commandID<=HIERARCHY_COLORING_BLUE_CMD) )
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("HIERARCHY_COLORING...CMD")
		{
			for (int i=0;i<int(App::ct->objCont->getSelSize());i++)
			{
				C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->getSelID(i));
				it->setHierarchyColorIndex(commandID-HIERARCHY_COLORING_NONE_CMD-1);
			}
			App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************
		}
		return(true);
	}
	
	return(false);
}

void CHierarchy::_drawLinesLinkingDummies(int maxRenderedPos[2])
{
	std::vector<int> positions; // contains only objects that have a dummy linking to another, as child (or the dummy itself)
	for (int i=0;i<int(App::ct->objCont->dummyList.size());i++)
	{
		CDummy* dummy=App::ct->objCont->getDummy(App::ct->objCont->dummyList[i]);
		if (dummy->getLinkedDummyID()!=-1)
		{
			int dummyID=dummy->getID();
			int linkedDummyID=dummy->getLinkedDummyID();
			C3DObject* obj=dummy;
			bool found=false;
			while ( (!found)&&(obj!=NULL) )
			{
				int idToSearch=obj->getID();
				for (int j=0;j<int(lineLastPosition.size()/3);j++)
				{
					if (idToSearch==lineLastPosition[3*j+2])
					{ // we found a pos
						positions.push_back(lineLastPosition[3*j+0]);
						positions.push_back(lineLastPosition[3*j+1]);
						positions.push_back(dummyID);
						positions.push_back(linkedDummyID);
						positions.push_back(j); // index in the lineLastPosition list
						if (dummyID==idToSearch)
							positions.push_back(1); // the dummy is visible
						else
							positions.push_back(0); // the dummy is not visible (built on a collapsed item)
						positions.push_back(dummy->getLinkType());
						found=true;
						break;
					}
				}
				if (!found)
					obj=obj->getParent();
			}
		}
	}
	glLineStipple(1,0x5555);
	glEnable(GL_LINE_STIPPLE);
	std::vector<int> linesPresent; // minY, maxY, X
	const int segmentOffset=5;
	const int segmentWidth=10;
	for (int i=0;i<int(positions.size())/7;i++)
	{
		if (positions[7*i+5]!=-1)
		{ // not yet processed
			int firstIndex=positions[7*i+4];
			for (int j=i+1;j<int(positions.size())/7;j++)
			{
				int secondDummyID=positions[7*j+2];
				if (positions[7*j+5]!=-1)
				{ // not yet processed
					if (secondDummyID==positions[7*i+3])
					{ // this is the one!
						int secondIndex=positions[7*j+4];
						if (secondIndex!=firstIndex)
						{ // make sure the two dummies are not linking to the same line!
							int maxX=SIM_MAX(positions[7*i+0],positions[7*j+0]);
							int overallOffset=0;
							// a. we don't want to intersect any text/icon in-between. We search for the overallOffset needed:
							int low=SIM_MIN(firstIndex,secondIndex);
							int high=SIM_MAX(firstIndex,secondIndex);
							int maxOtherX=0;
							for (int k=low+1;k<high;k++)
							{
								if (lineLastPosition[3*k+0]>maxOtherX)
									maxOtherX=lineLastPosition[3*k+0];
							}
							if (maxOtherX>maxX)
								overallOffset+=maxOtherX-maxX;
							// b. we don't want to overlap other dummy-dummy link lines
							// i.e. maxX+overallOffset should no lie within other lines maxX+overallOffset
							int minY=SIM_MIN(positions[7*i+1],positions[7*j+1]);
							int maxY=SIM_MAX(positions[7*i+1],positions[7*j+1]);
							for (int k=0;k<int(linesPresent.size())/3;k++)
							{
								if ((maxY>=linesPresent[3*k+0])&&(minY<=linesPresent[3*k+1]))
								{
									bool restart=false;
									while (abs(maxX+overallOffset-linesPresent[3*k+2])<segmentWidth+4)
									{
										overallOffset+=2;
										restart=true;
									}
									if (restart)
										k=-1;
								}
							}

							if ( (positions[7*i+6]==sim_dummy_linktype_dynamics_loop_closure)||(positions[7*i+6]==sim_dummy_linktype_dynamics_force_constraint) )
								ogl::setColor(0.0f,0.0f,1.0f,AMBIENT_MODE);
							if ( (positions[7*i+6]==sim_dummy_linktype_gcs_loop_closure)||(positions[7*i+6]==sim_dummy_linktype_gcs_tip)||(positions[7*i+6]==sim_dummy_linktype_gcs_target) )
								ogl::setColor(0.0f,0.6f,0.0f,AMBIENT_MODE);
							if (positions[7*i+6]==sim_dummy_linktype_ik_tip_target)
								ogl::setColor(1.0f,0.0f,0.0f,AMBIENT_MODE);
							glBegin(GL_LINES);
							glVertex3i(maxX+segmentOffset+overallOffset,positions[7*i+1],0);
							glVertex3i(maxX+segmentOffset+segmentWidth+overallOffset,positions[7*i+1],0);
							glEnd();
							if (positions[7*i+5]==1)
							{
								glDisable(GL_LINE_STIPPLE);
								glBegin(GL_LINES);
								glVertex3i(maxX+segmentOffset+overallOffset,positions[7*i+1],0);
								glVertex3i(maxX+segmentOffset+4+overallOffset,positions[7*i+1]+4,0);
								glVertex3i(maxX+segmentOffset+overallOffset,positions[7*i+1],0);
								glVertex3i(maxX+segmentOffset+4+overallOffset,positions[7*i+1]-4,0);
								glEnd();
								glEnable(GL_LINE_STIPPLE);
							}
							glBegin(GL_LINES);
							glVertex3i(maxX+segmentOffset+overallOffset,positions[7*j+1],0);
							glVertex3i(maxX+segmentOffset+segmentWidth+overallOffset,positions[7*j+1],0);
							glEnd();
							if (positions[7*j+5]==1)
							{
								glDisable(GL_LINE_STIPPLE);
								glBegin(GL_LINES);
								glVertex3i(maxX+segmentOffset+overallOffset,positions[7*j+1],0);
								glVertex3i(maxX+segmentOffset+4+overallOffset,positions[7*j+1]+4,0);
								glVertex3i(maxX+segmentOffset+overallOffset,positions[7*j+1],0);
								glVertex3i(maxX+segmentOffset+4+overallOffset,positions[7*j+1]-4,0);
								glEnd();
								glEnable(GL_LINE_STIPPLE);
							}
							glBegin(GL_LINES);
							glVertex3i(maxX+segmentOffset+segmentWidth+overallOffset,positions[7*i+1],0);
							glVertex3i(maxX+segmentOffset+segmentWidth+overallOffset,positions[7*j+1],0);
							glEnd();
							linesPresent.push_back(minY);
							linesPresent.push_back(maxY);
							linesPresent.push_back(maxX+overallOffset);
							if (maxRenderedPos[0]<maxX+overallOffset+segmentOffset+segmentWidth)
								maxRenderedPos[0]=maxX+overallOffset+segmentOffset+segmentWidth;
						}
						positions[7*j+5]=-1; // do not process this position anymore
						break;
					}
				}	
			}
		}
	}
	glDisable(GL_LINE_STIPPLE);
}

