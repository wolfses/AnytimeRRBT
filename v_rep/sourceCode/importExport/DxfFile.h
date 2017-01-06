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

class CDxfFile : public CImportExport
{
public:
	CDxfFile();
	virtual ~CDxfFile();
	bool importFunc(std::string fileName);
	bool exportFunc(std::string fileName,bool poly,std::vector<int>* sel);
	bool exportFunc(std::string fileName,bool poly,const std::vector<std::vector<float>*>& vertices,const std::vector<std::vector<int>*>& indices,const std::vector<std::string>& names);

private:
	void _readFile(VArchive& archive);
	bool _getCode(VArchive& archive);
	void _polyLineInBlock(VArchive &archive);
	void _threeDFaceInBlock(VArchive &archive);
	void _vertexInBlock(VArchive &archive);
	void _insertCommand(VArchive &archive,
					std::vector<std::vector<float>*>* aVertices,
					std::vector<std::vector<int>*>* aIndices,
					std::vector<std::vector<float>*>* aNormals,
					std::vector<std::vector<float>*>* aTexCoords,
					std::vector<std::string>* groupNames,
					std::vector<SObjMaterial*>* aMaterials);
	void _threeDFaceCommand(VArchive &archive,
					std::vector<std::vector<float>*>* aVertices,
					std::vector<std::vector<int>*>* aIndices,
					std::vector<std::vector<float>*>* aNormals,
					std::vector<std::vector<float>*>* aTexCoords,
					std::vector<std::string>* groupNames,
					std::vector<SObjMaterial*>* aMaterials);
	void _polylineCommand(VArchive &archive,
					std::vector<std::vector<float>*>* aVertices,
					std::vector<std::vector<int>*>* aIndices,
					std::vector<std::string>* groupNames);
	bool _writeCode(VArchive& archive,int number,std::string text);
	bool _writeCodeInt(VArchive& archive,int number,int comm);
	bool _writeCodeFloat(VArchive& archive,int number,float comm);
	void _writeAShapeWith3DFace(CShape* it,VArchive& archive);
	void _writeAShapeWithPolyline(CShape* it,VArchive& archive,int& initialSize);
	void _writeAMeshWith3DFace(const std::vector<float>& vertices,const std::vector<int>& indices,VArchive& archive);
	void _writeAMeshWithPolyline(const std::vector<float>& vertices,const std::vector<int>& indices,VArchive& archive,int& initialSize);
	bool addIntoAnotherList;
	int triangleCountExport;
	int totalTriangleExport;

	std::string code;
	std::string value;

	std::vector<std::string> blockNames;
	std::vector<std::vector<float>*> blockVertices;
	std::vector<std::vector<int>*> blockIndices;
	std::vector<float> blockBasePoints;
	std::vector<float> tempVertices;
	std::vector<int> tempIndices;
};
