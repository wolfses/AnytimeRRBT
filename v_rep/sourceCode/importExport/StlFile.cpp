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
#include "StlFile.h"
#include "Tt.h"
#include "App.h"

CStlFile::CStlFile(int format)
{
	fileFormat=format;
}

CStlFile::~CStlFile()
{
}

bool CStlFile::exportFunc(std::string fileName,bool poly,const std::vector<std::vector<float>*>& vertices,const std::vector<std::vector<int>*>& indices,const std::vector<std::string>& names)
{
	VFile myFile(fileName.c_str(),VFile::CREATE|VFile::WRITE|VFile::SHARE_EXCLUSIVE);
	myFile.setLength(0);
	VArchive archive(&myFile,VArchive::STORE);
	unsigned int totalTriangles=0;
	for (int i=0;i<int(vertices.size());i++)
		totalTriangles+=indices[i]->size()/3;
	std::string headerText("Exported with V-REP");
	for (int i=0;i<80;i++)
	{
		char c=' ';
		if (int(headerText.length())>i)
			c=headerText[i];
		archive << c;
	}
	archive << totalTriangles;
	for (int i=0;i<int(vertices.size());i++)
	{
		for (int j=0;j<int(indices[i]->size())/3;j++)
		{
			int ind[3]={(*indices[i])[3*j+0],(*indices[i])[3*j+1],(*indices[i])[3*j+2]};
			C3Vector v0((*vertices[i])[3*ind[0]+0],(*vertices[i])[3*ind[0]+1],(*vertices[i])[3*ind[0]+2]);
			C3Vector v1((*vertices[i])[3*ind[1]+0],(*vertices[i])[3*ind[1]+1],(*vertices[i])[3*ind[1]+2]);
			C3Vector v2((*vertices[i])[3*ind[2]+0],(*vertices[i])[3*ind[2]+1],(*vertices[i])[3*ind[2]+2]);
			_writeTriangle(archive,v0,v1,v2);
		}
	}
	archive.close();
	myFile.close();
	return(true);
}

bool CStlFile::exportFunc(std::string fileName,bool poly,std::vector<int>* sel)
{	
	VFile myFile(fileName.c_str(),VFile::CREATE|VFile::WRITE|VFile::SHARE_EXCLUSIVE);
	myFile.setLength(0);
	VArchive archive(&myFile,VArchive::STORE);
	std::vector<int> objects(*sel);
	unsigned int totalTriangles=0;
	for (int i=0;i<int(objects.size());i++)
	{
		CShape* it=App::ct->objCont->getShape(objects[i]);
		if (it!=NULL)
		{
			std::vector<float> wvert;
			std::vector<int> wind;
			it->geomData->geomInfo->getCumulativeMeshes(wvert,&wind,NULL);
			totalTriangles+=wind.size()/3;
		}
	}
	std::string headerText("Exported with V-REP");
	for (int i=0;i<80;i++)
	{
		char c=' ';
		if (int(headerText.length())>i)
			c=headerText[i];
		archive << c;
	}
	archive << totalTriangles;
	for (int i=0;i<int(objects.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(objects[i]);
		if (it->getObjectType()==sim_object_shape_type)
		{
			C7Vector m(it->getCumulativeTransformationPart1());
			CShape* sh=(CShape*)it;
			std::vector<float> wvert;
			std::vector<int> wind;
			sh->geomData->geomInfo->getCumulativeMeshes(wvert,&wind,NULL);
			for (int j=0;j<int(wind.size())/3;j++)
			{
				int ind[3]={wind[3*j+0],wind[3*j+1],wind[3*j+2]};
				C3Vector v0(wvert[3*ind[0]+0],wvert[3*ind[0]+1],wvert[3*ind[0]+2]);
				C3Vector v1(wvert[3*ind[1]+0],wvert[3*ind[1]+1],wvert[3*ind[1]+2]);
				C3Vector v2(wvert[3*ind[2]+0],wvert[3*ind[2]+1],wvert[3*ind[2]+2]);
				v0=m*v0;
				v1=m*v1;
				v2=m*v2;
				_writeTriangle(archive,v0,v1,v2);
			}
		}
	}
	archive.close();
	myFile.close();
	return(true);
}

bool CStlFile::importFunc(std::string fileName)
{
	if (VFile::doesFileExist(fileName))
	{
		int passCnt=1;
		int format=fileFormat;
		if (fileFormat==FILE_FORMAT_ANY_STL)
		{
			passCnt=2;
			format=FILE_FORMAT_ASCII_STL;
		}

		for (int i=0;i<passCnt;i++)
		{
			try
			{
				VFile file(fileName.c_str(),VFile::READ|VFile::SHARE_DENY_NONE);
				VArchive archive(&file,VArchive::LOAD);
				archiveLength=DWORD(file.getLength());
				bool result=false;
				if (archiveLength>=84)
					result=_readFile(archive,format);
				archive.close();
				file.close();
				if ((fileFormat!=FILE_FORMAT_ANY_STL)||(i!=0))
					return(result);
				if ((getGroupCount()!=0)&&(getVertexCount(0)>=3)&&(getTriangleCount(0)>=1))
					return(result);
				format=FILE_FORMAT_BINARY_STL;
			}
			catch(VFILE_EXCEPTION_TYPE e)
			{
				//VFile::reportAndHandleFileExceptionError(e);
				//return(false);
			}
		}
	}
	return(false);
}

bool CStlFile::_readFile(VArchive& archive,int format)
{
	if (archiveLength<5)
		return(false);
	std::vector<float>* norms=new std::vector<float>;
	_importedNormals.push_back(norms);
	_importedMaterials.push_back(NULL);
	std::vector<float>* textureCoords=new std::vector<float>;
	_importedTextureCoordinates.push_back(textureCoords);
	std::vector<float>* vertices=new std::vector<float>;
	_importedVertices.push_back(vertices);
	std::vector<int>* indices=new std::vector<int>;
	_importedIndices.push_back(indices);
	_importedNames.push_back("STL_Imported");

	bool error=false;
	char dummyChar;
	float dummyFloat;
	std::string first5Chars;
	for (int i=0;i<5;i++)
	{
		archive >> dummyChar;
		first5Chars+=tolower(dummyChar);
	}

	if (format==FILE_FORMAT_ASCII_STL)
	{ // ASCII STL
		std::string line,word;
		DWORD actualPosition=5;
		archive.readLine(actualPosition,line,true);
		bool insideFacet=false;
		bool insideLoop=false;
		int vertexNb=0;
		C3Vector v[3];
		while (archive.readLine(actualPosition,line,true))
		{
			if (tt::extractWord(line,word))
			{
				for (int i=0;i<int(word.size());i++)
					word[i]=tolower(word[i]);
				if (word=="facet")
					insideFacet=true;
				if (word=="endfacet")
					insideFacet=false;
				if ((word=="outer")&&insideFacet)
					insideLoop=true;
				if ((word=="endloop")&&insideLoop)
				{
					insideLoop=false;
					vertexNb=0;
				}
				if ((word=="vertex")&&insideLoop)
				{
					if (vertexNb<3)
					{
						std::string v0,v1,v2;
						if (tt::extractWord(line,v0)&&tt::extractWord(line,v1)&&tt::extractWord(line,v2))
						{
							float w0,w1,w2;
							if (tt::getValidFloat(v0,w0)&&tt::getValidFloat(v1,w1)&&tt::getValidFloat(v2,w2))
								v[vertexNb++].set(w0,w1,w2);
							else
								vertexNb=10; // That triangle will then be ignored!
						}
					}
					else
						vertexNb=10; // That triangle will then be ignored!
					if (vertexNb==3)
					{
						indices->push_back(0+vertices->size()/3);
						indices->push_back(1+vertices->size()/3);
						indices->push_back(2+vertices->size()/3);
						for (int i=0;i<3;i++)
						{
							for (int j=0;j<3;j++)
								vertices->push_back(v[i](j));
						}
						vertexNb=10;
					}
				}
			}
		}
	}
	else
	{ // Binary STL
		for (int i=0;i<75;i++)
			archive >> dummyChar;
		unsigned int facetNumber;
		archive >> facetNumber;
		int bytesLeft=archiveLength-84;
		if ((bytesLeft+2)/50<int(facetNumber)) // the +2 are the last 2 bytes that are sometimes missing
			facetNumber=(bytesLeft+2)/50;
		for (int i=0;i<int(facetNumber);i++)
		{
			archive >> dummyFloat >> dummyFloat >> dummyFloat; // we ignore normals for now
			for (int j=0;j<9;j++)
			{ // the vertices
				archive >> dummyFloat;
				vertices->push_back(dummyFloat);
			}
			indices->push_back(3*i+0);
			indices->push_back(3*i+1);
			indices->push_back(3*i+2);
			if (i!=int(facetNumber-1))
				archive >> dummyChar >> dummyChar;
		}
	}
	return(!error);
}

void CStlFile::_writeTriangle(VArchive& archive,C3Vector& v0,C3Vector& v1,C3Vector& v2)
{
	C3Vector n((v1-v0)^(v2-v0));
	if (n.getLength()==0)
		n=C3Vector::unitXVector;
	else
		n.normalize();
	archive << n(0) << n(1) << n(2);
	archive << v0(0) << v0(1) << v0(2);
	archive << v1(0) << v1(1) << v1(2);
	archive << v2(0) << v2(1) << v2(2);
	archive << char(' ') << char(' ');
}
