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

#include <string>
#include <vector>

// FULLY STATIC CLASS
class VFileDialog  
{
public:
	static std::string getOpenFileName_(QWidget* parent,WORD option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2="",const std::string& extension3="",const std::string& extension4="",const std::string& extension5="",const std::string& extension6="",const std::string& extension7="",const std::string& extension8="",const std::string& extension9="",const std::string& extension10="");
	static std::string getSaveFileName(QWidget* parent,WORD option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2="",const std::string& extension3="",const std::string& extension4="",const std::string& extension5="",const std::string& extension6="",const std::string& extension7="",const std::string& extension8="",const std::string& extension9="",const std::string& extension10="");
	static void getOpenFileNames(std::vector<std::string>& files,QWidget* parent,WORD option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2="",const std::string& extension3="",const std::string& extension4="",const std::string& extension5="",const std::string& extension6="",const std::string& extension7="",const std::string& extension8="",const std::string& extension9="",const std::string& extension10="");

private:
	static std::string _getOpenOrSaveFileName(bool open,QWidget* parent,WORD option,const std::string& title,const std::string& startPath,const std::string& initFilename,bool allowAnyFile,const std::string& extensionName,const std::string& extension1,const std::string& extension2,const std::string& extension3,const std::string& extension4,const std::string& extension5,const std::string& extension6,const std::string& extension7,const std::string& extension8,const std::string& extension9,const std::string& extension10);
};
