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
#include "DirBrowserElement.h"
#include "DirBrowser.h"
#include "OGL.h"
#include "IloIlo.h"
#include "imgLoader.h"
#include "App.h"
#include "VFileFinder.h"
#include "v_repStrings.h"

int CDirBrowserElement::_nextElementUniqueID=0;
int CDirBrowserElement::_selectedElementUniqueID=0;
CFolderThumbnails CDirBrowserElement::_currentFolderThumbnails;
CDirBrowserElement* CDirBrowserElement::_rootElement=NULL;

CDirBrowserElement::CDirBrowserElement(const char* folderName,const std::string& folderPath)
{
	_folderPath=folderPath;
	_folderName=folderName;
	_elementUniqueID=_nextElementUniqueID++;
	_thumbnailFileIsValid=false;

	// First check if the folder exists:
	if (VFile::doesFolderExist(folderPath))
	{
		_validFolder=true;
		VFileFinder finder;
		finder.searchFolders(folderPath);
		std::vector<std::string> subFolders;
		std::vector<std::string> subNames;
		int index=0;
		SFileOrFolder* foundFolder=finder.getFoundItem(index++);
		while (foundFolder!=NULL)
		{
			if ( (foundFolder->name!=".")&&(foundFolder->name!="..") )
			{
				subNames.push_back(foundFolder->name);
				subFolders.push_back(foundFolder->path);
			}
			foundFolder=finder.getFoundItem(index++);
		}
		for (int i=0;i<int(subFolders.size());i++)
			children.push_back(new CDirBrowserElement(subNames[i].c_str(),subFolders[i]));
	}
	else
	{
		_validFolder=false;
		_folderName=IDSOGL_MODEL_FOLDER_NOT_FOUND;
	}
	computeNumberOfElements();
	_collapsed=true;
}

CDirBrowserElement::~CDirBrowserElement()
{
	for (int i=0;i<int(children.size());i++)
		delete children[i];
	children.clear();
}
void CDirBrowserElement::inflateOrCollapseElementWithUniqueID(int uniqueID)
{
	if (_elementUniqueID==uniqueID)
		_collapsed=!_collapsed;
	else
	{
		for (int i=0;i<int(children.size());i++)
			children[i]->inflateOrCollapseElementWithUniqueID(uniqueID);
	}
}

void CDirBrowserElement::expandCollapseAll(bool expand)
{
	_collapsed=!expand;
	for (int i=0;i<int(children.size());i++)
		children[i]->expandCollapseAll(expand);
}

void CDirBrowserElement::setSelectedElement(int uniqueID)
{
	_selectedElementUniqueID=uniqueID;
	CDirBrowserElement* it=getSelectedElement();
	if (it!=NULL)
	{
		if (it->_validFolder)
			_currentFolderThumbnails.prepareFolderThumbnails(it->_folderPath.c_str());
		else
			_currentFolderThumbnails.prepareFolderThumbnails(NULL);
	}
}

int CDirBrowserElement::selectFolderFromPathAndName(const std::string& pathName)
{
	if ( VREP_LOWCASE_STRING_COMPARE(pathName.c_str(),_folderPath.c_str())==0) // lower case comparison
	{
		return(_elementUniqueID);
	}
	for (int i=0;i<int(children.size());i++)
	{
		int r=children[i]->selectFolderFromPathAndName(pathName);
		if (r!=-1)
		{
			_collapsed=false;
			return(r);
		}
	}
	return(-1);
}

int CDirBrowserElement::openFoldersLeadingToFolderName(const std::string& foldername)
{

	if ( VREP_LOWCASE_STRING_COMPARE(foldername.c_str(),_folderName.c_str())==0) // lower case comparison
		return(_elementUniqueID);
	for (int i=0;i<int(children.size());i++)
	{
		int r=children[i]->openFoldersLeadingToFolderName(foldername);
		if (r!=-1)
		{
			_collapsed=false;
			return(r);
		}
	}
	return(-1);
}


void CDirBrowserElement::setRootElement(CDirBrowserElement* rootElement)
{
	_rootElement=rootElement;
}

CDirBrowserElement* CDirBrowserElement::getSelectedElement()
{
	return(_rootElement->_getElementWithUniqueID(_selectedElementUniqueID));
}

CDirBrowserElement* CDirBrowserElement::_getElementWithUniqueID(int uniqueID)
{
	if (_elementUniqueID==uniqueID)
		return(this);
	else
	{
		for (int i=0;i<int(children.size());i++)
		{
			CDirBrowserElement* it=children[i]->_getElementWithUniqueID(uniqueID);
			if (it!=NULL)
				return(it);
		}
	}
	return(NULL);	
}

std::string CDirBrowserElement::getFolderPath()
{
	return(_folderPath);
}

int CDirBrowserElement::getElementUniqueID()
{
	return(_elementUniqueID);
}

void CDirBrowserElement::renderElement(CDirBrowser* hier,bool& bright,int textCenterOffset,int verticalShift,bool dontDisplay,
		int renderingSize[2],int textPos[2],
		int indentNb,std::vector<int>* vertLines,int minRenderedPos[2],int maxRenderedPos[2])
{
	bright=!bright;
	const BYTE horizontalShift=13*App::sc;
	if (textPos[0]<minRenderedPos[0])
		minRenderedPos[0]=textPos[0];
	if (textPos[1]<minRenderedPos[1])
		minRenderedPos[1]=textPos[1];
	if (textPos[0]>maxRenderedPos[0])
		maxRenderedPos[0]=textPos[0];
	if (textPos[1]>maxRenderedPos[1])
		maxRenderedPos[1]=textPos[1];

	if (textPos[1]<-verticalShift)
		dontDisplay=true; // We don't display what is outside of the view!
	bool textInside=(textPos[1]<renderingSize[1]+verticalShift);
	bool inSelection=false;
	if ((_selectedElementUniqueID==_elementUniqueID)&&textInside&&(!dontDisplay))
	{
		ogl::setColor(ogl::HIERARCHY_AND_BROWSER_LAST_SELECTION_COLOR,AMBIENT_MODE);
		glBegin(GL_QUADS);
		glVertex3i(0,textPos[1]+textCenterOffset-verticalShift/2,0);
		glVertex3i(renderingSize[0],textPos[1]+textCenterOffset-verticalShift/2,0);
		glVertex3i(renderingSize[0],textPos[1]+textCenterOffset+verticalShift/2,0);
		glVertex3i(0,textPos[1]+textCenterOffset+verticalShift/2,0);
		glEnd();
		inSelection=true;
	}
	else if (textInside&&(!dontDisplay))
	{
		ogl::setColor(ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_BRIGHT,AMBIENT_MODE);
		glBegin(GL_QUADS);
		glVertex3i(0,textPos[1]+textCenterOffset-verticalShift/2,0);
		glVertex3i(renderingSize[0],textPos[1]+textCenterOffset-verticalShift/2,0);
		glVertex3i(renderingSize[0],textPos[1]+textCenterOffset+verticalShift/2,0);
		glVertex3i(0,textPos[1]+textCenterOffset+verticalShift/2,0);
		glEnd();
	}

	if (textInside&&(!dontDisplay))
	{
		hier->objectPosition.push_back(textPos[1]+textCenterOffset);
		hier->objectPosition.push_back(_elementUniqueID);
	}
	int off=2;
	{
		if (children.size()!=0)
		{
			if (_collapsed)
			{
				int picture=PLUS_SIGN_TREE_PICTURE;
				if (dontDisplay||(!textInside))
					picture=NO_TREE_PICTURE;
				off=off+_drawIcon(hier,textPos[0]+off+8,textPos[1]+textCenterOffset,picture,true);
			}
			else
			{
				int picture=MINUS_SIGN_TREE_PICTURE;
				if (dontDisplay||(!textInside))
					picture=NO_TREE_PICTURE;
				off=off+_drawIcon(hier,textPos[0]+off+8,textPos[1]+textCenterOffset,picture,true);
			}
		}
		else
			off=off+_drawIcon(hier,textPos[0]+off+8,textPos[1]+textCenterOffset,NO_TREE_PICTURE,true);
		off=off+_drawIcon(hier,textPos[0]+off+8,textPos[1]+textCenterOffset,-1,!dontDisplay);
	}
	int textEndPosition=textPos[0]+off+ogl::getTextLengthInPixels(_folderName);

	if (textEndPosition>maxRenderedPos[0])
		maxRenderedPos[0]=textEndPosition;

	if (textInside&&(!dontDisplay))
	{
		ogl::setColor(0.4f,0.4f,0.4f,AMBIENT_MODE);
		ogl::drawText(textPos[0]+off,textPos[1],0,_folderName,false);
		ogl::setColor(ogl::HIERARCHY_AND_BROWSER_LINE_COLOR,AMBIENT_MODE);
		for (int i=0;i<int(vertLines->size());i++)
		{
			if (vertLines->at(i)<indentNb-1)
			{
				glBegin(GL_LINES);
				glVertex3i(textPos[0]+3+horizontalShift/2-horizontalShift*(indentNb-vertLines->at(i)),textPos[1]+textCenterOffset+verticalShift/2,0);
				glVertex3i(textPos[0]+3+horizontalShift/2-horizontalShift*(indentNb-vertLines->at(i)),textPos[1]+textCenterOffset-verticalShift/2,0);
				glEnd();
			}
		}
	}

	textPos[1]=textPos[1]-verticalShift;
	if ( !_collapsed )
	{
		int xPosCopy=textPos[0];
		textPos[0]=textPos[0]+horizontalShift;
		int indentCopy=indentNb+1;
		ogl::setColor(ogl::HIERARCHY_AND_BROWSER_LINE_COLOR,AMBIENT_MODE);
		std::vector<CDirBrowserElement*> el;
		std::vector<int> elNb;
		for (int i=0;i<int(children.size());i++)
		{ // This is needed to order the elements from least sub-elements to most sub-elements
			int els=children[i]->getNumberOfElements();
			int j;
			for (j=0;j<int(elNb.size());j++)
			{
				if (els<elNb[j])
					break;
			}
			elNb.insert(elNb.begin()+j,els);
			el.insert(el.begin()+j,children[i]);
		}	
		for (int i=0;i<int(el.size());i++)
		{
			if (i!=int(el.size())-1)
			{ // Vertical line going through
				vertLines->push_back(indentNb);
				int txtYTmp=textPos[1];
				el[i]->renderElement(hier,bright,textCenterOffset,verticalShift,dontDisplay,
											renderingSize,textPos,indentCopy,
											vertLines,minRenderedPos,maxRenderedPos);
				vertLines->erase(vertLines->end()-1);
				if (!dontDisplay)
				{
					glBegin(GL_LINES);
					glVertex3i(xPosCopy+3+horizontalShift/2,txtYTmp+textCenterOffset+verticalShift/2,0);
					glVertex3i(xPosCopy+3+horizontalShift/2,txtYTmp+textCenterOffset-verticalShift/2,0);
					glVertex3i(xPosCopy+4+horizontalShift/2,txtYTmp+textCenterOffset,0);
					glVertex3i(xPosCopy+4+horizontalShift,txtYTmp+textCenterOffset,0);
					glEnd();
				}
			}
			else
			{ // Vertical line stopping in the middle (L)
				int txtYTmp=textPos[1];
				el[i]->renderElement(hier,bright,textCenterOffset,verticalShift,dontDisplay,
											renderingSize,textPos,indentCopy,
											vertLines,minRenderedPos,maxRenderedPos);
				if (!dontDisplay)
				{
					glBegin(GL_LINES);
					glVertex3i(xPosCopy+3+horizontalShift/2,txtYTmp+textCenterOffset+verticalShift/2,0);
					glVertex3i(xPosCopy+3+horizontalShift/2,txtYTmp+textCenterOffset,0);
					glVertex3i(xPosCopy+4+horizontalShift/2,txtYTmp+textCenterOffset,0);
					glVertex3i(xPosCopy+4+horizontalShift,txtYTmp+textCenterOffset,0);
					glEnd();
				}
			}
		}
		textPos[0]=xPosCopy;
	}
}

int CDirBrowserElement::_drawIcon(CDirBrowser* hier,int tPosX,int tPosY,int pictureID,bool drawIt)
{ // pictureID is -1 by default. It is then ignored. The size of the icon is 20x16
	const int sizeX=20*App::sc;
	const int sizeY=16*App::sc;
	const int interIconSpacing=4*App::sc;
	int retVal=0;
	if (pictureID!=NO_TREE_PICTURE)
	{
		if (pictureID!=-1)
		{
			if (drawIt)
			{
				App::ct->globalTextureCont->startTextureDisplay(pictureID);
				if (((pictureID==PLUS_SIGN_TREE_PICTURE)||(pictureID==MINUS_SIGN_TREE_PICTURE)))
				{
					hier->inflateIconPosition.push_back(tPosX);
					hier->inflateIconPosition.push_back(tPosY);
					hier->inflateIconPosition.push_back(_elementUniqueID);
				}
				_drawTexturedIcon(tPosX,tPosY,sizeX,sizeY);
			}
			retVal+=sizeX+interIconSpacing;
		}
		else
		{
			if (drawIt)
			{
				App::ct->globalTextureCont->startTextureDisplay(FOLDER_PICTURE);
				_drawTexturedIcon(tPosX,tPosY,sizeX,sizeY);
			}
			retVal+=sizeX+interIconSpacing;
		}
	}
	else
		retVal+=sizeX+interIconSpacing;
	return(retVal);
}

void CDirBrowserElement::_drawTexturedIcon(int tPosX,int tPosY,int sizeX,int sizeY)
{
	ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // We turn blending on!
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	glTexEnvf (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f);
	glVertex3i(tPosX-sizeX/2,tPosY-sizeY/2,0);
	glTexCoord2f(0.625f,0.0f); // icons themselves are 20x16, but since some gfx cards don't support that non-power of 2 res, we save them as 32x16!
	glVertex3i(tPosX+sizeX/2,tPosY-sizeY/2,0);
	glTexCoord2f(0.625f,1.0f); // icons themselves are 20x16, but since some gfx cards don't support that non-power of 2 res, we save them as 32x16!
	glVertex3i(tPosX+sizeX/2,tPosY+sizeY/2,0);
	glTexCoord2f(0.0f,1.0f);
	glVertex3i(tPosX-sizeX/2,tPosY+sizeY/2,0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	ogl::setBlending(false);
	glDisable(GL_ALPHA_TEST);
	App::ct->globalTextureCont->endTextureDisplay();
}

bool CDirBrowserElement::renderDummyElement(bool& bright,int textCenterOffset,int verticalShift,int renderingSize[2],int textPos[2])
{ // Just renders the background and returns true when inside the visible zone
	bright=!bright;
	if (textPos[1]<-verticalShift)
		return(false);
	if (textPos[1]<renderingSize[1]+verticalShift)
	{
		ogl::setColor(ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_BRIGHT,AMBIENT_MODE);
		glBegin(GL_QUADS);
		glVertex3i(0,textPos[1]+textCenterOffset-verticalShift/2,0);
		glVertex3i(renderingSize[0],textPos[1]+textCenterOffset-verticalShift/2,0);
		glVertex3i(renderingSize[0],textPos[1]+textCenterOffset+verticalShift/2,0);
		glVertex3i(0,textPos[1]+textCenterOffset+verticalShift/2,0);
		glEnd();
		textPos[1]=textPos[1]-verticalShift;
		return(true);
	}
	return(false);
}


int CDirBrowserElement::getNumberOfElements()
{
	return(numberOfElements);
}

int CDirBrowserElement::computeNumberOfElements()
{
	numberOfElements=1;
	for (int i=0;i<int(children.size());i++)
	{
		children[i]->computeNumberOfElements();
		numberOfElements+=children[i]->getNumberOfElements();
	}
	return(numberOfElements);
}


