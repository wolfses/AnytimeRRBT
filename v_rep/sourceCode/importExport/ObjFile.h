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

#include "ImportExport.h"
#include "Shape.h"
#include <vector>

class CObjFile : public CImportExport
{
public:
	CObjFile();
	virtual ~CObjFile();
	bool importFunc(std::string fileName);
	bool exportFunc(std::string fileName,bool poly,std::vector<int>* sel);
	bool exportFunc(std::string fileName,bool poly,const std::vector<std::vector<float>*>& vertices,const std::vector<std::vector<int>*>& indices,const std::vector<std::string>& names);

private:
	bool _readFile(VArchive& archive,const char* fileName);
	bool _getCode(VArchive& archive);
	bool _getMaterialCode(VArchive& archive,DWORD& archiveCurrentPos,std::string& matCode,std::string& matValue);
	bool _getVertexOrTextureCoordOrNormalValue(std::string& text,float& v);
	int _getIndexValue(std::string& text,int& v,int& vt,int& vn);
	bool _writeVertex(VArchive& archive,C3Vector& v);
	bool _writeIndex(VArchive& archive,int a,int b,int c);
	void _appendMaterialFileToMaterialList(const char* matFile,std::vector<SObjMaterial>& materialList);


	std::string code;
	std::string value;
};
