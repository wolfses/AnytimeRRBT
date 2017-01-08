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
#include "ModelBrowser.h"
#include "OGL.h"
#include "imgLoader.h"
#include "IloIlo.h"
#include "ObjectEdition.h"

#include "AddMenu.h"
#include "PathEdition.h"
#include "TriangleVertexEdition.h"
#include "Tt.h"
#include "v_repStrings.h"
#include "App.h"
#include "FileMenu.h"

const int SAFETY_BORDER_SIZE=20;

CModelBrowser::CModelBrowser()
{
	setUpDefaultValues();
}

CModelBrowser::~CModelBrowser()
{
}

void CModelBrowser::newSceneProcedure()
{
	setUpDefaultValues();
}

void CModelBrowser::setUpDefaultValues()
{
	viewPositionY=0;
	_caughtElements=0;
	verticalScrollbarWidth=0;
	refreshViewFlag=3;
	mouseDownOnSlider=false;
}

int CModelBrowser::getCaughtElements()
{
	return(_caughtElements);
}

void CModelBrowser::clearCaughtElements(int keepMask)
{
	_caughtElements&=keepMask;
}

void CModelBrowser::setRenderingSizeAndPosition(int xSize,int ySize,int xPos,int yPos)
{
	renderingSize[0]=xSize;
	renderingSize[1]=ySize;
	renderingPosition[0]=xPos;
	renderingPosition[1]=yPos;
	refreshViewFlag=3;	
}

void CModelBrowser::looseFocus()
{
	App::ct->oglSurface->setModelThumbnailDraggingInfo(-1,0,0);
}

void CModelBrowser::keyPress(int key)
{
	refreshViewFlag=3;	
	App::ct->oglSurface->setModelThumbnailDraggingInfo(-1,0,0);
	if (key==UP_KEY)
		viewPositionY+=16;
	if (key==DOWN_KEY)
		viewPositionY-=16;
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

bool CModelBrowser::render()
{ // return value true means the hierarchy was refreshed
	if (refreshViewFlag<=0)
		return(false);
	refreshViewFlag--;

	// We draw a black background so that the separation between the hierarchy and scene is rendered correctly on ALL graphic cards:
	glEnable(GL_SCISSOR_TEST);	
	glDisable(GL_DEPTH_TEST);
	glScissor(renderingPosition[0],renderingPosition[1],renderingSize[0],renderingSize[1]);
	glClearColor(ogl::SEPARATION_LINE_COLOR[0],ogl::SEPARATION_LINE_COLOR[1],ogl::SEPARATION_LINE_COLOR[2],1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	ogl::setColorsAllBlack();
	glViewport(renderingPosition[0],renderingPosition[1],renderingSize[0],renderingSize[1]);
	ogl::allLightsOffAmbientFull();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f,renderingSize[0],0.0f,renderingSize[1],-1.0f,1.0f);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glScissor(renderingPosition[0],renderingPosition[1],renderingSize[0],renderingSize[1]);

	glClearColor(ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_BRIGHT[0],ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_BRIGHT[1],ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_BRIGHT[2],1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	const int borderBorderDistX=128+2+2;
	const int borderBorderDistY=128+MODEL_BROWSER_TEXT_SPACE_HEIGHT+2+2;
	verticalScrollbarWidth=0; // first we try without!

	for (int passCnt=0;passCnt<3;passCnt++)
	{ // first pass is to check if we need the vertical scroolbar, second to check the virtualVerticalRenderingSize and make sure the viewPositionY is ok. Last pass is for rendering!
		_modelThumbnailIndexAndPos.clear();
		int lines=(renderingSize[0]-verticalScrollbarWidth)/borderBorderDistX;
		if (lines<1)
			lines=1;
		int startX=3;
		int posX=startX;
		int posY=renderingSize[1]-2+viewPositionY;
		int lineCnt=0;
		for (int i=0;i<App::ct->modelDirectoryRoot->_currentFolderThumbnails.getThumbnailCount();i++)
		{
			if (passCnt>1)
			{ // we don't display anything in the first and second pass!
				if ( (posY>=0)&&(posY<renderingSize[1]+borderBorderDistY) )
				{ // we are inside the visible zone!
					CThumbnail* thumbnail=App::ct->modelDirectoryRoot->_currentFolderThumbnails.getThumbnailFromIndex(i);
					thumbnail->display(posX,posY,App::ct->modelDirectoryRoot->_currentFolderThumbnails.getThumbnailNameFromIndex(i));
					_modelThumbnailIndexAndPos.push_back(i);
					_modelThumbnailIndexAndPos.push_back(posX+0);
					_modelThumbnailIndexAndPos.push_back(posY+0);
				}
			}
			lineCnt++;
			if (lineCnt==lines)
			{ // next row
				posX=startX;
				posY-=borderBorderDistY;
				lineCnt=0;
			}
			else
				posX+=borderBorderDistX;
		}
		if (lineCnt!=0)
			posY-=borderBorderDistY;
		// Now posY should be the lowest rendering position
		if (passCnt==0)
		{
			virtualVerticalRenderingSize=(renderingSize[1]-2+viewPositionY)-posY;
			if (virtualVerticalRenderingSize>renderingSize[1])
				verticalScrollbarWidth=HIERARCHY_SCROLLBAR_WIDTH*App::sc;
		}
		if (passCnt==1)
		{
			virtualVerticalRenderingSize=(renderingSize[1]-2+viewPositionY)-posY;
			// Now check if the viewPositionY is ok:
			if (posY>0)
				viewPositionY-=posY;
			if (viewPositionY<0)
				viewPositionY=0;
		}
	}

	// Now the vertical slider:
	if (verticalScrollbarWidth!=0)
	{
		int vtl[2]={0,0};
		int vbr[2]={0,0};
		getSliderPosition(vtl,vbr,NULL);
//		float black[3]={0.0f,0.0f,0.0f};
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
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);
	return(true);
}

void CModelBrowser::getSliderPosition(int vSliderTopLeft[2],int vSliderBottomRight[2],float* prop)
{
	if (verticalScrollbarWidth>0)
	{ // here we compute the vertical slider:
		int effDr=renderingSize[1]-0;
		int sliderS=effDr*effDr/virtualVerticalRenderingSize;
		if (sliderS<HIERARCHY_SCROLLBAR_WIDTH*App::sc)
			sliderS=HIERARCHY_SCROLLBAR_WIDTH*App::sc;
		int sliderP=((effDr-sliderS)*viewPositionY)/(virtualVerticalRenderingSize-effDr);
		vSliderTopLeft[0]=renderingSize[0]-verticalScrollbarWidth;
		vSliderBottomRight[0]=renderingSize[0];
		vSliderTopLeft[1]=renderingSize[1]-sliderP;
		vSliderBottomRight[1]=vSliderTopLeft[1]-sliderS;
		if (prop!=NULL)
			prop[0]=float(virtualVerticalRenderingSize-effDr)/float(effDr-sliderS);
	}
}

bool CModelBrowser::leftMouseDown(int x,int y,int selectionStatus)
{
	if (x<0)
		return(false);
	if (y<0)
		return(false);
	if (x>renderingSize[0])
		return(false);
	if (y>renderingSize[1])
		return(false);
	// The mouse went down on the hierarchy window!
	App::ct->oglSurface->setModelThumbnailDraggingInfo(-1,0,0);
	_caughtElements|=sim_left_button;
	refreshViewFlag=3;
	mouseDownRelativePosition[0]=x;
	mouseDownRelativePosition[1]=y;
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	previousMouseRelativePosition[0]=x;
	previousMouseRelativePosition[1]=y;
	mouseDownOnSlider=false;
	if (verticalScrollbarWidth>0)
	{
		int vtl[2];
		int vbr[2];
		getSliderPosition(vtl,vbr,NULL);
		if ( (mouseDownRelativePosition[0]>vtl[0])&&(mouseDownRelativePosition[0]<vbr[0]) )
		{
			if ( (mouseDownRelativePosition[1]<vtl[1])&&(mouseDownRelativePosition[1]>vbr[1]) )
			{ // Vertical slider action
				mouseDownOnSlider=true;
				viewPositionYWhenMouseOnSliderDown=viewPositionY;
				return(true);
			}
			else
			{ // We clicked the slider, but not the mobile part
				return(true);
			}
		}
	}
	for (int i=0;i<int(_modelThumbnailIndexAndPos.size())/3;i++)
	{
		if ((x>=_modelThumbnailIndexAndPos[3*i+1])&&(x<_modelThumbnailIndexAndPos[3*i+1]+128))
		{
			if ((y<=_modelThumbnailIndexAndPos[3*i+2])&&(y>_modelThumbnailIndexAndPos[3*i+2]-128-MODEL_BROWSER_TEXT_SPACE_HEIGHT))
			{
				if (!App::ct->modelDirectoryRoot->_currentFolderThumbnails.getIsFolderFromIndex(_modelThumbnailIndexAndPos[3*i+0]))
					App::ct->oglSurface->setModelThumbnailDraggingInfo(_modelThumbnailIndexAndPos[3*i+0],_modelThumbnailIndexAndPos[3*i+1]-x,_modelThumbnailIndexAndPos[3*i+2]-y);
				else
				{ // Here we clicked a folder! (in the model browser lower part!!)
					std::string folderPath(App::ct->modelDirectoryRoot->_currentFolderThumbnails.getThumbnailNameAndPathFromIndex(_modelThumbnailIndexAndPos[3*i+0]));
					folderPath.erase(folderPath.end()-4,folderPath.end()-0); // We remove the final ".ttm"
					int elementUniqueID=App::ct->modelDirectoryRoot->selectFolderFromPathAndName(folderPath);
					if (elementUniqueID!=-1)
						App::ct->modelDirectoryRoot->setSelectedElement(elementUniqueID);
					_caughtElements=0;
				}
			}
		}
	}

	return(true);
}

void CModelBrowser::leftMouseUp(int x,int y)
{
	refreshViewFlag=3;
	mouseDownOnSlider=false;
	int thumbnailIndex;
	C3Vector desiredModelPosition;
	bool okToDrop=App::ct->oglSurface->getModelThumbnailDraggingInfo(thumbnailIndex,&desiredModelPosition);
	App::ct->oglSurface->setModelThumbnailDraggingInfo(-1,0,0);
	if (okToDrop)
	{ // we dropped something into the scene!
		std::string pathAndName=App::ct->modelDirectoryRoot->_currentFolderThumbnails.getPathAndModelName(thumbnailIndex);
		if (pathAndName.length()!=0)
		{
			IF_UI_EVENT_CAN_WRITE_DATA
			{
				std::string acknowledgment;
				CFileMenu::loadModel(pathAndName.c_str(),true,false,true,&acknowledgment,false);
				if (App::ct->objCont->getSelSize()==1)
				{ // we could have several model bases (in the old fileformat)
					C3DObject* obj=App::ct->objCont->getLastSelection();
					if (obj!=NULL)
					{
						if ( (obj->getObjectManipulationModePermissions()&0x03)||(obj->getObjectManipulationTranslationRelativeTo()!=0) )
						{ // We can only place the model if the X and/or Y manip are set or if the placement is not relative to world
							C7Vector tr(obj->getLocalTransformation());
							float ss=obj->getNonDefaultTranslationStepSize();
							if (ss==0.0f)
								ss=App::userSettings->getTranslationStepSize();
							desiredModelPosition(0)-=fmod(desiredModelPosition(0),ss);
							desiredModelPosition(1)-=fmod(desiredModelPosition(1),ss);

							tr.X(0)+=desiredModelPosition(0);
							tr.X(1)+=desiredModelPosition(1);
							obj->setLocalTransformation(tr);
							// To avoid flickering:
							obj->bufferMainDisplayStateVariables();
							obj->bufferedMainDisplayStateVariablesToDisplay();
						}
					}
				}
				App::ct->undoBufferContainer->announceChange(); // ************************** UNDO thingy **************************

				App::mainWindow->uiThread_renderScene(true);
				App::mainWindow->uiThread_renderScene(true);

				// keep the acknowledgment modal message in this scope, otherwise we might experience
				// unexpected locks (e.g. UI handles modal message, and SIM thread requests UI to
				// handle a vision sensor!
				if ((acknowledgment!="")&&(!App::isFullScreen()))
					App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDS_MODEL_CONTENT_ACKNOWLEDGMENTS),acknowledgment,VMessageBox::OKELI);
			}
		}
	}
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	_caughtElements&=0xffff-sim_left_button;
}

bool CModelBrowser::rightMouseDown(int x,int y)
{
	if (x<0)
		return(false);
	if (y<0)
		return(false);
	if (x>renderingSize[0])
		return(false);
	if (y>renderingSize[1])
		return(false);
	// The mouse went down on the hierarchy window
	App::ct->oglSurface->setModelThumbnailDraggingInfo(-1,0,0);
	_caughtElements|=sim_right_button;
	refreshViewFlag=3;
	mouseDownRelativePosition[0]=x;
	mouseDownRelativePosition[1]=y;
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	previousMouseRelativePosition[0]=x;
	previousMouseRelativePosition[1]=y;
	return(true); // We catch this event to display a popup-menu when the mouse comes up
}
void CModelBrowser::rightMouseUp(int x,int y,int absX,int absY,QWidget* mainWindow)
{ // Only caught if right button was caught by the hierarchy!
	_caughtElements&=0xffff-sim_right_button;
	if ( (x<0)||(y<0)||(x>renderingSize[0])||(y>renderingSize[1]) )
		return;

	// The mouse went up on the hierarchy window
	App::ct->oglSurface->setModelThumbnailDraggingInfo(-1,0,0);
	refreshViewFlag=3;
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;

}

bool CModelBrowser::mouseWheel(int deltaZ,int x,int y)
{
	if ( (x<0)||(y<0)||(x>renderingSize[0])||(y>renderingSize[1]) )
		return(false);
	refreshViewFlag=3;
	App::ct->oglSurface->setModelThumbnailDraggingInfo(-1,0,0);
	viewPositionY-=(deltaZ/120)*32;//16;
	return(true);
}

void CModelBrowser::mouseMove(int x,int y,bool passiveAndFocused)
{
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	if (!passiveAndFocused)
	{
		int vtl[2];
		int vbr[2];
		float prop=0.0f;
		if (mouseDownOnSlider)
		{ // we are moving the vertical slider
			getSliderPosition(vtl,vbr,&prop);
			viewPositionY=viewPositionYWhenMouseOnSliderDown-int(prop*float(mouseRelativePosition[1]-mouseDownRelativePosition[1]));
			refreshViewFlag=3;
		} 
	}
	previousMouseRelativePosition[0]=mouseRelativePosition[0];
	previousMouseRelativePosition[1]=mouseRelativePosition[1];
}
bool CModelBrowser::leftMouseDblClick(int x,int y,int selectionStatus)
{
	if ( (x<0)||(y<0)||(x>renderingSize[0])||(y>renderingSize[1]) )
		return(false);
	// The mouse went down on the hierarchy window
	App::ct->oglSurface->setModelThumbnailDraggingInfo(-1,0,0);
	refreshViewFlag=3;
	mouseDownRelativePosition[0]=x;
	mouseDownRelativePosition[1]=y;
	mouseRelativePosition[0]=x;
	mouseRelativePosition[1]=y;
	previousMouseRelativePosition[0]=x;
	previousMouseRelativePosition[1]=y;
	return(false);
}

void CModelBrowser::setRefreshViewFlag()
{
	refreshViewFlag=3;
}

void CModelBrowser::validateViewPosition()
{
}

void CModelBrowser::addMenu(VMenu* menu)
{ 
}

bool CModelBrowser::evaluateMenuSelection(int commandID)
{ // Return value is true if the command belonged to Add menu and was executed
	return(false);
}
