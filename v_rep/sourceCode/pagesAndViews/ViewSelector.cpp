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
#include "ViewSelector.h"
#include "OGL.h"
#include "OglSurface.h"
#include "App.h"
#include "IloIlo.h"

CViewSelector::CViewSelector()
{
	setUpDefaultValues();
}

CViewSelector::~CViewSelector()
{
	setUpDefaultValues();
}

void CViewSelector::newSceneProcedure()
{
	setUpDefaultValues();
}

void CViewSelector::setUpDefaultValues()
{
	viewSelectionBuffer.clear();
	viewSelectionBufferType.clear();
	viewSelectionSize[0]=1;
	viewSelectionSize[1]=1;
	viewIndex=-1;
	subViewIndex=-1;
	_caughtElements=0;
}

int CViewSelector::getCaughtElements()
{
	return(_caughtElements);
}

void CViewSelector::clearCaughtElements(int keepMask)
{
	_caughtElements&=keepMask;
}

void CViewSelector::setViewSizeAndPosition(int sizeX,int sizeY,int posX,int posY)
{
	viewSize[0]=sizeX;
	viewSize[1]=sizeY;
	viewPosition[0]=posX;
	viewPosition[1]=posY;
}

void CViewSelector::setViewSelectionInfo(int objType,int viewInd,int subViewInd)
{
	objectType=objType;
	viewIndex=viewInd;
	subViewIndex=subViewInd;
}

void CViewSelector::render()
{
	glClearColor(0.5f,0.5f,0.5f,1);	
	glDisable(GL_SCISSOR_TEST);	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	viewSelectionBuffer.clear();
	viewSelectionBufferType.clear();
	viewSelectionSize[0]=1;
	viewSelectionSize[1]=1;
	// Compute grid size
	for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
		if ((it->getObjectType()==sim_object_camera_type)&&((objectType==CAMERA_VIEW_SELECT_MODE)||(objectType==VIEWABLE_VIEW_SELECT_MODE)))
		{
			viewSelectionBuffer.push_back(it->getID());
			viewSelectionBufferType.push_back(0); // This value has no importance for now
		}
		if ((it->getObjectType()==sim_object_graph_type)&&((objectType==GRAPH_VIEW_SELECT_MODE)||(objectType==VIEWABLE_VIEW_SELECT_MODE)))
		{
//			CGraph* graph=(CGraph*)it;
			viewSelectionBuffer.push_back(it->getID());
			viewSelectionBufferType.push_back(0); // Time-graph
			viewSelectionBuffer.push_back(it->getID());
			viewSelectionBufferType.push_back(1); // XY-graph
		}
		if ((it->getObjectType()==sim_object_visionsensor_type)&&((objectType==VISIONSENSOR_VIEW_SELECT_MODE)||(objectType==VIEWABLE_VIEW_SELECT_MODE)))
		{
			viewSelectionBuffer.push_back(it->getID());
			viewSelectionBufferType.push_back(0);
		}
	}
	if (viewSelectionBuffer.size()==0)
	{
		App::ct->oglSurface->setViewSelectionActive(false);
		return; // nothing to see here!!
	}
	float smallWinRatio=1.33f;
	while (viewSelectionSize[0]*viewSelectionSize[1]<int(viewSelectionBuffer.size()))
	{
		int cx=viewSize[0]/(viewSelectionSize[0]+1);
		int cy=viewSize[1]/(viewSelectionSize[1]+1);
		if (cx>smallWinRatio*cy)
			viewSelectionSize[0]++;
		else
			viewSelectionSize[1]++;
	}
	float sp=0.1f;
	int ax=viewSize[0]/viewSelectionSize[0];
	int ay=viewSize[1]/viewSelectionSize[1];
	int tnd[2]={0,0};
	if (ax>smallWinRatio*ay)
	{
		tns[1]=(int)(viewSize[1]/((float)viewSelectionSize[1]+sp*((float)(viewSelectionSize[1]+1))));
		tns[0]=(int)(smallWinRatio*(float)tns[1]);
		tnd[1]=(int)(tns[1]*sp);
		tnd[0]=(viewSize[0]-viewSelectionSize[0]*tns[0])/(viewSelectionSize[0]+1);
	}
	else
	{
		tns[0]=(int)(viewSize[0]/((float)viewSelectionSize[0]+sp*((float)(viewSelectionSize[0]+1))));
		tns[1]=(int)(((float)tns[0])/smallWinRatio);
		tnd[0]=(int)(tns[0]*sp);
		tnd[1]=(viewSize[1]-viewSelectionSize[1]*tns[1])/(viewSelectionSize[1]+1);
	}
	int mouseOn=-1;
	if (_caughtElements&sim_left_button)
	{
		int bufferInd1=getObjectIndexInViewSelection(mouseDownRelativePosition);
		int bufferInd2=getObjectIndexInViewSelection(mouseRelativePosition);
		if (bufferInd1==bufferInd2)
			mouseOn=bufferInd1;
	}
	int mouseOver=getObjectIndexInViewSelection(mouseRelativePosition);
	for (int k=0;k<viewSelectionSize[1];k++)
	{
		for (int l=0;l<viewSelectionSize[0];l++)
		{
			if ((l+viewSelectionSize[0]*k)<int(viewSelectionBuffer.size()))
			{
				glEnable(GL_SCISSOR_TEST);	
				C3DObject* it=App::ct->objCont->getObject(viewSelectionBuffer[l+viewSelectionSize[0]*k]);

				if (mouseOver==l+viewSelectionSize[0]*k)
				{
					glViewport(tnd[0]+l*(tns[0]+tnd[0])-5+viewPosition[0],viewPosition[1]+viewSize[1]-(k+1)*(tns[1]+tnd[1])-5,tns[0]+10,tns[1]+10);
					glScissor(tnd[0]+l*(tns[0]+tnd[0])-5+viewPosition[0],viewPosition[1]+viewSize[1]-(k+1)*(tns[1]+tnd[1])-5,tns[0]+10,tns[1]+10);
					glClearColor(0.7f,0.7f,0.4f,1);	
					glClear (GL_COLOR_BUFFER_BIT);
					glViewport(tnd[0]+l*(tns[0]+tnd[0])-4+viewPosition[0],viewPosition[1]+viewSize[1]-(k+1)*(tns[1]+tnd[1])-4,tns[0]+8,tns[1]+8);
					glScissor(tnd[0]+l*(tns[0]+tnd[0])-4+viewPosition[0],viewPosition[1]+viewSize[1]-(k+1)*(tns[1]+tnd[1])-4,tns[0]+8,tns[1]+8);
					glClearColor(0.9f,0.75f,0.2f,1);	
					glClear (GL_COLOR_BUFFER_BIT);
					glViewport(tnd[0]+l*(tns[0]+tnd[0])-3+viewPosition[0],viewPosition[1]+viewSize[1]-(k+1)*(tns[1]+tnd[1])-3,tns[0]+6,tns[1]+6);
					glScissor(tnd[0]+l*(tns[0]+tnd[0])-3+viewPosition[0],viewPosition[1]+viewSize[1]-(k+1)*(tns[1]+tnd[1])-3,tns[0]+6,tns[1]+6);
					glClearColor(1.0f,0.8f,0.0f,1);	
					glClear (GL_COLOR_BUFFER_BIT);
					glViewport(tnd[0]+l*(tns[0]+tnd[0])-2+viewPosition[0],viewPosition[1]+viewSize[1]-(k+1)*(tns[1]+tnd[1])-2,tns[0]+4,tns[1]+4);
					glScissor(tnd[0]+l*(tns[0]+tnd[0])-2+viewPosition[0],viewPosition[1]+viewSize[1]-(k+1)*(tns[1]+tnd[1])-2,tns[0]+4,tns[1]+4);
					glClearColor(1.0f,0.9f,0.0f,1);	
					glClear (GL_COLOR_BUFFER_BIT);
					glViewport(tnd[0]+l*(tns[0]+tnd[0])-1+viewPosition[0],viewPosition[1]+viewSize[1]-(k+1)*(tns[1]+tnd[1])-1,tns[0]+2,tns[1]+2);
					glScissor(tnd[0]+l*(tns[0]+tnd[0])-1+viewPosition[0],viewPosition[1]+viewSize[1]-(k+1)*(tns[1]+tnd[1])-1,tns[0]+2,tns[1]+2);
					glClearColor(1.0f,1.0f,0.0f,1);	
					glClear (GL_COLOR_BUFFER_BIT);
				}
				else
				{
					glViewport(tnd[0]+l*(tns[0]+tnd[0])-5+viewPosition[0],viewPosition[1]+viewSize[1]-(k+1)*(tns[1]+tnd[1])-5,tns[0]+10,tns[1]+10);
					glScissor(tnd[0]+l*(tns[0]+tnd[0])-5+viewPosition[0],viewPosition[1]+viewSize[1]-(k+1)*(tns[1]+tnd[1])-5,tns[0]+10,tns[1]+10);
					glClearColor(0.5f,0.5f,0.5f,1);	
					glClear (GL_COLOR_BUFFER_BIT);
					glViewport(tnd[0]+l*(tns[0]+tnd[0])-1+viewPosition[0],viewPosition[1]+viewSize[1]-(k+1)*(tns[1]+tnd[1])-1,tns[0]+2,tns[1]+2);
					glScissor(tnd[0]+l*(tns[0]+tnd[0])-1+viewPosition[0],viewPosition[1]+viewSize[1]-(k+1)*(tns[1]+tnd[1])-1,tns[0]+2,tns[1]+2);
					glClearColor(0.6f,0.6f,0.6f,1);	
					glClear (GL_COLOR_BUFFER_BIT);
				}

				glViewport(tnd[0]+l*(tns[0]+tnd[0])+viewPosition[0],viewPosition[1]+viewSize[1]-(k+1)*(tns[1]+tnd[1]),tns[0],tns[1]);
				glScissor(tnd[0]+l*(tns[0]+tnd[0])+viewPosition[0],viewPosition[1]+viewSize[1]-(k+1)*(tns[1]+tnd[1]),tns[0],tns[1]);
				glClearColor(0.6f,0.6f,0.6f,1);	
				glClear (GL_COLOR_BUFFER_BIT);


				bool timeGraph=(viewSelectionBufferType[l+viewSelectionSize[0]*k]==0);
				if (mouseOn!=l+viewSelectionSize[0]*k)
				{
					if (it->getObjectType()==sim_object_camera_type)
						((CCamera*)it)->lookIn(tns,NULL);
					if (it->getObjectType()==sim_object_graph_type)
						((CGraph*)it)->lookAt(tns,NULL,timeGraph,false,true,false);
					if (it->getObjectType()==sim_object_visionsensor_type)
					{
						int xxx[2]={tnd[0]+l*(tns[0]+tnd[0])+viewPosition[0],viewPosition[1]+viewSize[1]-(k+1)*(tns[1]+tnd[1])};
						((CVisionSensor*)it)->lookAt(NULL,xxx,tns);
					}
				}
				glEnable(GL_SCISSOR_TEST);	// I think the vision sensor disables the scissor at some point, trying to correct for that here ;)

				ogl::allLightsOffAmbientFull();
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho(0,tns[0],0,tns[1],-1,1);
				glMatrixMode (GL_MODELVIEW);
				ogl::setColorsAllBlack();
				
				glLoadIdentity ();
				glDisable(GL_DEPTH_TEST);
				if (it->getObjectType()==sim_object_camera_type)
				{
					ogl::setColor(0.1f,0.1f,0.1f,EMISSION_MODE);
					ogl::drawText(2,tns[1]-12,0,it->getName().append(" (Camera)"));
					ogl::setColor(0.9f,0.9f,0.9f,EMISSION_MODE);
					ogl::drawText(1,tns[1]-11,0,it->getName().append(" (Camera)"));
				}
				if (it->getObjectType()==sim_object_graph_type) 
				{
					std::string txt=" (Time Graph)";
					if (!timeGraph)
						txt=" (XY Graph)";
					ogl::setColor(0.1f,0.1f,0.1f,EMISSION_MODE);
					ogl::drawText(2,tns[1]-12,0,it->getName().append(txt));
					ogl::setColor(0.9f,0.9f,0.9f,EMISSION_MODE);
					ogl::drawText(1,tns[1]-11,0,it->getName().append(txt));
				}
				if (it->getObjectType()==sim_object_visionsensor_type) 
				{
					std::string txt=" (Vision Sensor)";
					ogl::setColor(0.1f,0.1f,0.1f,EMISSION_MODE);
					ogl::drawText(2,tns[1]-12,0,it->getName().append(txt));
					ogl::setColor(0.9f,0.9f,0.9f,EMISSION_MODE);
					ogl::drawText(1,tns[1]-11,0,it->getName().append(txt));
				}
				glEnable(GL_DEPTH_TEST);
				glDisable(GL_SCISSOR_TEST);
			}
		}
	}
}

int CViewSelector::getObjectIndexInViewSelection(int mousePos[2])
{
	int pos[2]={-1,-1};
	int space[2]={0,0};
	space[0]=(viewSize[0]-tns[0]*viewSelectionSize[0])/(viewSelectionSize[0]+1);
	space[1]=(viewSize[1]-tns[1]*viewSelectionSize[1])/(viewSelectionSize[1]+1);
	int mPos[2]={space[0],space[1]};
	int grid[2]={0,0};
	while (mousePos[0]>mPos[0])
	{
		if (mousePos[0]<mPos[0]+tns[0])
		{
			pos[0]=grid[0];
			break;
		}
		else
		{
			grid[0]++;
			mPos[0]=mPos[0]+tns[0]+space[0];
		}
	}
	while (viewSize[1]-mousePos[1]>mPos[1])
	{
		if (viewSize[1]-mousePos[1]<mPos[1]+tns[1])
		{
			pos[1]=grid[1];
			break;
		}
		else
		{
			grid[1]++;
			mPos[1]=mPos[1]+tns[1]+space[1];
		}
	}
	if ((pos[0]==-1)||(pos[1]==-1))
		return(-1);
	if (pos[0]+viewSelectionSize[0]*pos[1]>=int(viewSelectionBuffer.size()))
		return(-1);
	return(pos[0]+viewSelectionSize[0]*pos[1]);
}



bool CViewSelector::leftMouseButtonDown(int x,int y,int selectionStatus)
{
	if ( (x<0)||(y<0)||(x>viewSize[0])||(y>viewSize[1]) )
		return(false);
	_caughtElements&=0xffff-sim_left_button;
	// The mouse went down in this window zone
	mouseDownRelativePosition[0]=x;
	mouseDownRelativePosition[1]=y;
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	_caughtElements|=sim_left_button;
	return(true);
}
void CViewSelector::leftMouseButtonUp(int x,int y)
{
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	if (_caughtElements&sim_left_button)
	{
		int bufferInd1=getObjectIndexInViewSelection(mouseDownRelativePosition);
		int bufferInd2=getObjectIndexInViewSelection(mouseRelativePosition);
		if ( (bufferInd1==bufferInd2)&&(bufferInd1!=-1) )
		{ // Mouse went down and up on the same item:
			C3DObject* it=App::ct->objCont->getObject(viewSelectionBuffer[bufferInd1]);
			if (it!=NULL)
			{
				CSPage* view=App::ct->oglSurface->pageCont->getPage(viewIndex);
				if (view!=NULL)
				{
					CSView* subView=view->getView(subViewIndex);
					if (subView!=NULL)
					{
						subView->setLinkedObjectID(it->getID(),false);
						subView->setTimeGraph(viewSelectionBufferType[bufferInd1]==0);
						App::ct->oglSurface->setViewSelectionActive(false);
						App::ct->undoBufferContainer->announceChange(); // **************** UNDO THINGY ****************
					}
				}
			}
		}
	}
}

C3DObject* CViewSelector::getViewableObject(int x,int y)
{
	int pos[2]={x,y};
	int ind=getObjectIndexInViewSelection(pos);
	if (ind==-1)
		return(NULL);
	return(App::ct->objCont->getObject(viewSelectionBuffer[ind]));
}

int CViewSelector::getCursor(int x,int y)
{
	if (getViewableObject(x,y)!=NULL)
		return(sim_cursor_finger);
	return(-1);
}

void CViewSelector::mouseMove(int x,int y,bool passiveAndFocused)
{
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
}
bool CViewSelector::rightMouseButtonDown(int x,int y)
{
	if ( (x<0)||(y<0)||(x>viewSize[0])||(y>viewSize[1]) )
		return(false);
	_caughtElements&=0xffff-sim_right_button;
	return(false); // Not processed yet!
}
void CViewSelector::rightMouseButtonUp(int x,int y,int absX,int absY,QWidget* mainWindow)
{
}
bool CViewSelector::leftMouseButtonDoubleClick(int x,int y,int selectionStatus)
{
	return(false); // Not processed yet!
}

void CViewSelector::keyPress(int key)
{
}

bool CViewSelector::evaluateMenuSelection(int commandID,int subViewIndex)
{ // Return value is true if the command belonged to hierarchy menu and was executed
	if (commandID==VIEW_SELECTOR_ANY_CMD)
	{
		IF_UI_EVENT_CAN_WRITE_DATA_CMD("VIEW_SELECTOR_ANY_CMD")
		{
			CSPage* view=App::ct->oglSurface->pageCont->getPage(App::ct->oglSurface->pageCont->getActivePageIndex());
			if (view==NULL)
				return(true);
			CSView* subView=view->getView(subViewIndex);
			if (subView==NULL)
				return(true);
			int cameraNb=App::ct->objCont->getCameraNumberInSelection(&App::ct->objCont->objectList);
			int graphNb=App::ct->objCont->getGraphNumberInSelection(&App::ct->objCont->objectList);
			int rendSensNb=App::ct->objCont->getVisionSensorNumberInSelection(&App::ct->objCont->objectList);

			if (cameraNb+graphNb+rendSensNb>0)
				App::ct->oglSurface->startViewSelection(VIEWABLE_VIEW_SELECT_MODE,subViewIndex);
		}
		return(true);
	}
	return(false);
}
