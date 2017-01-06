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

#pragma once

#include "vrepPrecompiledHeader.h"
#include "3DObject.h"
#include "FolderThumbnails.h"

class CDirBrowser;

class CDirBrowserElement  
{
public:
	CDirBrowserElement(const char* folderName,const std::string& folderPath);
	virtual ~CDirBrowserElement();
	int getLinkedObjectID();
	int getNumberOfElements();
	int computeNumberOfElements();

	void renderElement(CDirBrowser* hier,bool& bright,int textCenterOffset,int verticalShift,bool dontDisplay,
		int renderingSize[2],int textPos[2],
		int indentNb,std::vector<int>* vertLines,int minRenderedPos[2],int maxRenderedPos[2]);
	static bool renderDummyElement(bool& bright,int textCenterOffset,int verticalShift,int renderingSize[2],int textPos[2]);

	std::string getFolderPath();
	int getElementUniqueID();
	static void setSelectedElement(int uniqueID);
	int selectFolderFromPathAndName(const std::string& pathName);
	static CDirBrowserElement* getSelectedElement();
	static void setRootElement(CDirBrowserElement* rootElement);
	int openFoldersLeadingToFolderName(const std::string& foldername);

	void inflateOrCollapseElementWithUniqueID(int uniqueID);
	void expandCollapseAll(bool expand);

	std::vector<CDirBrowserElement*> children;
	static CFolderThumbnails _currentFolderThumbnails;

protected:
	CDirBrowserElement* _getElementWithUniqueID(int uniqueID);
	int _drawIcon(CDirBrowser* hier,int tPosX,int tPosY,int pictureID,bool drawIt);
	void _drawTexturedIcon(int tPosX,int tPosY,int sizeX,int sizeY);
	std::string _folderPath;
	std::string _folderName;
	int _elementUniqueID;
	bool _collapsed;
	bool _validFolder;
	bool _thumbnailFileIsValid;
	int numberOfElements;
	static int _nextElementUniqueID;
	static int _selectedElementUniqueID;
	static CDirBrowserElement* _rootElement;
};
