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
#include "DirectoryPaths.h"
#include "Global.h"
#include "VVarious.h"

CDirectoryPaths::CDirectoryPaths()
{
	static std::string currentDir(getPathFromFull(VVarious::getModulePathAndFile()));
	executableDirectory=currentDir;
	systemDirectory=executableDirectory+VREP_SLASH+V_REP_SYSTEM_DIRECTORY_NAME;
#ifdef MAC_VREP
	// We are inside of the package!!!
	sceneDirectory=executableDirectory+"/../../../"+V_REP_SCENES_DIRECTORY_NAME;// if scenes can't be found, it will use the last used directory somehow!
	modelDirectory=executableDirectory+"/../../../"+V_REP_MODELS_DIRECTORY_NAME;
	uiDirectory=executableDirectory+"/../../../";
	cadFormatDirectory=executableDirectory+"/../../../"+V_REP_CADFILES_DIRECTORY_NAME;
	textureDirectory=executableDirectory+"/../../../";
	otherFilesDirectory=executableDirectory+"/../../../";
#else
	sceneDirectory=executableDirectory+VREP_SLASH+V_REP_SCENES_DIRECTORY_NAME;// if scenes can't be found, it will use the last used directory somehow!
	modelDirectory=executableDirectory+VREP_SLASH+V_REP_MODELS_DIRECTORY_NAME;
	uiDirectory=executableDirectory;
	cadFormatDirectory=executableDirectory+VREP_SLASH+V_REP_CADFILES_DIRECTORY_NAME;
	textureDirectory=executableDirectory;
	otherFilesDirectory=executableDirectory;
#endif
}

CDirectoryPaths::~CDirectoryPaths()
{

}

std::string CDirectoryPaths::getPathFromFull(const std::string& full)
{ // no final backslash or slash is returned

	return(VVarious::splitPath_path(full));

}

std::string CDirectoryPaths::getNameFromFull(const std::string& full)
{
	return(VVarious::splitPath_fileBase(full));
}
