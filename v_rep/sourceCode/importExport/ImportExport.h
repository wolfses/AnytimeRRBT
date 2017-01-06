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

#include <vector>

struct SObjMaterial
{
	std::string materialName;
	float ambient[3];
	float diffuse[3];
	float specular[3];
	std::string textureFile;
};

class CImportExport  
{
public:
	CImportExport();
	virtual ~CImportExport();

	virtual bool importFunc(std::string fileName);
	virtual bool exportFunc(std::string fileName,bool poly,std::vector<int>* sel);
	virtual bool exportFunc(std::string fileName,bool poly,const std::vector<std::vector<float>*>& vertices,const std::vector<std::vector<int>*>& indices,const std::vector<std::string>& names);

	int getFileFormat();
	int getGroupCount();
	int getVertexCount(int groupIndex);
	int getTriangleCount(int groupIndex);
	int getTextureCoordinateCount(int groupIndex);
	int getNormalsCount(int groupIndex);
	bool getVertex(int groupIndex,int vertexIndex,float vertex[3]);
	bool getTriangle(int groupIndex,int triangleIndex,int triangle[3]); 
	bool getTextureCoordinate(int groupIndex,int coordIndex,float coord[2]);
	bool getNormal(int groupIndex,int normalIndex,float normal[3]);
	std::string getName(int groupIndex);
	SObjMaterial* getAndClearMaterial(int groupIndex);

protected:
	std::vector<std::vector<float>*> _importedVertices;
	std::vector<std::vector<int>*> _importedIndices;
	std::vector<std::vector<float>*> _importedTextureCoordinates;
	std::vector<std::vector<float>*> _importedNormals;
	std::vector<SObjMaterial*> _importedMaterials;

	std::vector<std::string> _importedNames;

	DWORD archiveLength;
	DWORD actualPosition;
	bool addIntoAnotherList;
	int numberOfTrianglesRead;
	int fileFormat;
};
