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
#include "DxfFile.h"
#include "Tt.h"
#include "App.h"
#include <algorithm>

CDxfFile::CDxfFile()
{
	addIntoAnotherList=true;
	triangleCountExport=0;
	totalTriangleExport=0;
	fileFormat=FILE_FORMAT_DXF;
}

CDxfFile::~CDxfFile()
{
	for (int i=0;i<int(blockVertices.size());i++)
		delete blockVertices[i];
	for (int i=0;i<int(blockIndices.size());i++)
		delete blockIndices[i];
}

bool CDxfFile::importFunc(std::string fileName)
{
	if (VFile::doesFileExist(fileName))
	{
		try
		{
			VFile file(fileName,VFile::READ|VFile::SHARE_DENY_NONE);
			VArchive archive(&file,VArchive::LOAD);
			archiveLength=DWORD(file.getLength());
			_readFile(archive);
			archive.close();
			file.close();
			return(true);
		}
		catch(VFILE_EXCEPTION_TYPE e)
		{
			VFile::reportAndHandleFileExceptionError(e);
			return(false);
		}
	}
	return(false);
}

void CDxfFile::_readFile(VArchive &archive)
{
	actualPosition=0;
	while (_getCode(archive))
	{
		std::string theBlockName("");
		bool polylineFirstPass=true;
		float bbpx=0;
		float bbpy=0;
		float bbpz=0;
		if ((code=="0")&&(value=="BLOCK"))
		{
			// Inside of a block
			while (_getCode(archive))
			{
				if ((code=="2")||(code=="3")) theBlockName=value;
				if (code=="10") bbpx=float(atof(value.c_str()));
				if (code=="20") bbpy=float(atof(value.c_str()));
				if (code=="30") bbpz=float(atof(value.c_str()));
				while (((code=="0")&&(value=="POLYLINE"))||((code=="0")&&(value=="3DFACE")))
				{	// inside polyline or 3Dface
					tempIndices.clear();
					tempVertices.clear();
					if ((code=="0")&&(value=="POLYLINE")) 
						_polyLineInBlock(archive);
					else
						_threeDFaceInBlock(archive);
					if ((tempIndices.size()!=0)&&(tempVertices.size()!=0))
					{
						if (polylineFirstPass)
						{
							blockNames.push_back(theBlockName);
							blockBasePoints.push_back(bbpx);
							blockBasePoints.push_back(bbpy);
							blockBasePoints.push_back(bbpz);
							std::vector<float>* theFloatAr=new std::vector<float>;
							theFloatAr->reserve(16384);
							theFloatAr->clear();
							blockVertices.push_back(theFloatAr);
							std::vector<int>* theIntAr=new std::vector<int>;
							theIntAr->reserve(4096);
							theIntAr->clear();
							blockIndices.push_back(theIntAr);
						}
						int originalSize=blockVertices[blockVertices.size()-1]->size()/3;
						for (int i=0;i<int(tempVertices.size());i++)
							blockVertices[blockVertices.size()-1]->push_back(tempVertices[i]);
						for (int i=0;i<int(tempIndices.size());i++)
							blockIndices[blockIndices.size()-1]->push_back(tempIndices[i]+originalSize);
						polylineFirstPass=false;
					}
				}
				if ((code=="0")&&(value=="ENDBLK")) break;	// Leave the block
			}
		}
		else if ((code=="2")&&(value=="ENTITIES"))
		{
			// Inside of an entity
			addIntoAnotherList=true;
			while (_getCode(archive))
			{
				while ((code=="0")&&(value=="INSERT"))
				{
					addIntoAnotherList=true;
					_insertCommand(archive,&_importedVertices,&_importedIndices,&_importedNormals,&_importedTextureCoordinates,&_importedNames,&_importedMaterials);
				}
				while ((code=="0")&&(value=="3DFACE"))
				{
					_threeDFaceCommand(archive,&_importedVertices,&_importedIndices,&_importedNormals,&_importedTextureCoordinates,&_importedNames,&_importedMaterials);
					addIntoAnotherList=false;
				}
				while ((code=="0")&&(value=="POLYLINE"))
				{
					_polylineCommand(archive,&_importedVertices,&_importedIndices,&_importedNames);
					if ((tempIndices.size()!=0)&&(tempVertices.size()!=0))
					{
						if (addIntoAnotherList)
						{
							std::vector<float>* ne=new std::vector<float>;	
							std::vector<float>* te=new std::vector<float>;	
							std::vector<float>* ve=new std::vector<float>;	
							std::vector<int>* in=new std::vector<int>;	
							_importedNormals.push_back(ne);
							_importedMaterials.push_back(NULL);
							_importedTextureCoordinates.push_back(te);
							_importedVertices.push_back(ve);
							_importedIndices.push_back(in);
							_importedNames.push_back("");
						}
						int ind=_importedVertices.size()-1;
						int initialSize=_importedVertices[ind]->size()/3;
						for (int i=0;i<int(tempVertices.size());i++)
							_importedVertices[ind]->push_back(tempVertices[i]);
						for (int i=0;i<int(tempIndices.size());i++)
							_importedIndices[ind]->push_back(tempIndices[i]+initialSize);
						addIntoAnotherList=false;
					}
				}
				if ((code=="0")&&(value=="ENDSEC")) break;	// Leave the entity
			}
		}
	}
}

bool CDxfFile::_getCode(VArchive& archive)
{
	code="";
	value="";
	bool retVal=false;
	if (archive.readLine(actualPosition,code,false))
	{
		if (archive.readLine(actualPosition,value,false))
			retVal=true;
	}
	code.erase(std::remove(code.begin(),code.end(),' '),code.end());
	value.erase(std::remove(value.begin(),value.end(),' '),value.end());
	return(retVal);
}

bool CDxfFile::_writeCode(VArchive& archive,int number,std::string text)
{
	std::string no=tt::FNb(0,number,false);
	archive.writeLine(no);
	archive.writeLine(text);
	return(true);
}

bool CDxfFile::_writeCodeInt(VArchive& archive,int number,int comm)
{
	std::string no=tt::FNb(0,number,false);
	archive.writeLine(no);
	no=tt::FNb(0,comm,false);
	archive.writeLine(no);
	return(true);
}

bool CDxfFile::_writeCodeFloat(VArchive& archive,int number,float comm)
{
	std::string no=tt::FNb(0,number,false);
	archive.writeLine(no);
	no=tt::FNb(comm);
	archive.writeLine(no);
	return(true);
}

void CDxfFile::_polyLineInBlock(VArchive &archive)
{
	bool pass1=false;
	tempVertices.clear();
	tempIndices.clear();
	while (_getCode(archive))
	{
		if ((code=="70")&&(atoi(value.c_str())>=64)) 
			pass1=true;
		while ((code=="0")&&(value=="VERTEX"))
			_vertexInBlock(archive);
		if ((code=="0")&&(value=="SEQEND")) 
			break;
	}
	if (!pass1)	// if it is not a polyface mesh, erase the arrays
	{
		tempVertices.clear();
		tempIndices.clear();
	}
}

void CDxfFile::_threeDFaceInBlock(VArchive &archive)
{
	tempVertices.clear();
	tempIndices.clear();
	float x[4];
	float y[4];
	float z[4];
	while (_getCode(archive))
	{
		if (code=="10") x[0]=float(atof(value.c_str()));
		if (code=="20") y[0]=float(atof(value.c_str()));
		if (code=="30") z[0]=float(atof(value.c_str()));
		if (code=="11") x[1]=float(atof(value.c_str()));
		if (code=="21") y[1]=float(atof(value.c_str()));
		if (code=="31") z[1]=float(atof(value.c_str()));
		if (code=="12") x[2]=float(atof(value.c_str()));
		if (code=="22") y[2]=float(atof(value.c_str()));
		if (code=="32") z[2]=float(atof(value.c_str()));
		if (code=="13") x[3]=float(atof(value.c_str()));
		if (code=="23") y[3]=float(atof(value.c_str()));
		if (code=="33") z[3]=float(atof(value.c_str()));
		if (code=="0") break;
	}
	int valNumb=4;
	if ((x[2]==x[3])&&(y[2]==y[3])&&(z[2]==z[3])) 
		valNumb=3;
	for (int i=0;i<valNumb;i++)
	{
		tempVertices.push_back(x[i]);
		tempVertices.push_back(y[i]);
		tempVertices.push_back(z[i]);
	}
	tempIndices.push_back(0);
	tempIndices.push_back(1);
	tempIndices.push_back(2);
	numberOfTrianglesRead++;
	if (valNumb==4)
	{
		tempIndices.push_back(0);
		tempIndices.push_back(2);
		tempIndices.push_back(3);
		numberOfTrianglesRead++;
	}
	return;
}

void CDxfFile::_vertexInBlock(VArchive &archive)
{
	float x=0;
	float y=0;
	float z=0;
	int type=0;
	int ind1=0;
	int ind2=0;
	int ind3=0;
	int ind4=0;
	while (_getCode(archive))
	{
		if (code=="10") x=float(atof(value.c_str()));
		if (code=="20") y=float(atof(value.c_str()));
		if (code=="30") z=float(atof(value.c_str()));
		if (code=="70") type=atoi(value.c_str());
		if (code=="71") ind1=atoi(value.c_str());
		if (code=="72") ind2=atoi(value.c_str());
		if (code=="73") ind3=atoi(value.c_str());
		if (code=="74") ind4=atoi(value.c_str());
		// color has to be implemented here (later)
		if (code=="0") break;
	}
	if ((type&192)==192)
	{
		tempVertices.push_back(x);
		tempVertices.push_back(y);
		tempVertices.push_back(z);
	}
	if ((type&192)==128)
	{
		if ((ind1!=0)&&(ind2!=0)&&(ind3!=0))
		{
			tempIndices.push_back(abs(ind1)-1);
			tempIndices.push_back(abs(ind2)-1);
			tempIndices.push_back(abs(ind3)-1);
			numberOfTrianglesRead++;
			if (ind4!=0)
			{
				tempIndices.push_back(abs(ind1)-1);
				tempIndices.push_back(abs(ind3)-1);
				tempIndices.push_back(abs(ind4)-1);
				numberOfTrianglesRead++;
			}
		}
	}
	return;
}

void CDxfFile::_insertCommand(VArchive &archive,
					std::vector<std::vector<float>*>* aVertices,
					std::vector<std::vector<int>*>* aIndices,
					std::vector<std::vector<float>*>* aNormals,
					std::vector<std::vector<float>*>* aTexCoords,
					std::vector<std::string>* groupNames,
					std::vector<SObjMaterial*>* aMaterials)
{
	float x=0;
	float y=0;
	float z=0;
	float dx=0;
	float dy=0;
	float dz=0;
	float r=0;
	std::string theBlockName="";
	while (_getCode(archive))
	{
		if (code=="10") x=float(atof(value.c_str()));
		if (code=="20") y=float(atof(value.c_str()));
		if (code=="30") z=float(atof(value.c_str()));
		if (code=="41") dx=float(atof(value.c_str()));
		if (code=="42") dy=float(atof(value.c_str()));
		if (code=="43") dz=float(atof(value.c_str()));
		if (code=="50") r=float(atof(value.c_str()));
		if (code=="2") theBlockName=value;
		if (code=="0") break;
	}
	int found=-1;
	for (int i=0;i<int(blockNames.size());i++)
		if (blockNames[i]==theBlockName)
		{
			found=i;
			break;
		}
	if (found!=-1)
	{
		std::vector<float>* ve=new std::vector<float>;	
		std::vector<int>* in=new std::vector<int>;
		ve->reserve(blockVertices[found]->size());
		in->reserve(blockIndices[found]->size());
		for (int j=0;j<int(blockVertices[found]->size())/3;j++)
		{
			ve->push_back(x-blockBasePoints[3*found+0]+blockVertices[found]->at(3*j+0)*dx);
			ve->push_back(y-blockBasePoints[3*found+1]+blockVertices[found]->at(3*j+1)*dy);
			ve->push_back(z-blockBasePoints[3*found+2]+blockVertices[found]->at(3*j+2)*dz);
		}
		for (int j=0;j<int(blockIndices[found]->size());j++)
			in->push_back(blockIndices[found]->at(j));
		aVertices->push_back(ve);
		aIndices->push_back(in);
		groupNames->push_back(theBlockName);

		std::vector<float>* vn=new std::vector<float>;	
		std::vector<float>* vt=new std::vector<float>;	
		aTexCoords->push_back(vt);
		aNormals->push_back(vn);
		aMaterials->push_back(NULL);
	}
}

void CDxfFile::_threeDFaceCommand(VArchive &archive,
					std::vector<std::vector<float>*>* aVertices,
					std::vector<std::vector<int>*>* aIndices,
					std::vector<std::vector<float>*>* aNormals,
					std::vector<std::vector<float>*>* aTexCoords,
					std::vector<std::string>* groupNames,
					std::vector<SObjMaterial*>* aMaterials)
{
	float x[4];
	float y[4];
	float z[4];
	while (_getCode(archive))
	{
		if (code=="10") x[0]=float(atof(value.c_str()));
		if (code=="20") y[0]=float(atof(value.c_str()));
		if (code=="30") z[0]=float(atof(value.c_str()));
		if (code=="11") x[1]=float(atof(value.c_str()));
		if (code=="21") y[1]=float(atof(value.c_str()));
		if (code=="31") z[1]=float(atof(value.c_str()));
		if (code=="12") x[2]=float(atof(value.c_str()));
		if (code=="22") y[2]=float(atof(value.c_str()));
		if (code=="32") z[2]=float(atof(value.c_str()));
		if (code=="13") x[3]=float(atof(value.c_str()));
		if (code=="23") y[3]=float(atof(value.c_str()));
		if (code=="33") z[3]=float(atof(value.c_str()));
		if (code=="0") break;
	}
	if (addIntoAnotherList)
	{
		std::vector<float>* ve=new std::vector<float>;	
		std::vector<int>* in=new std::vector<int>;	
		ve->reserve(16384);
		in->reserve(4096);
		aVertices->push_back(ve);
		aIndices->push_back(in);
		groupNames->push_back("");

		std::vector<float>* vn=new std::vector<float>;	
		std::vector<float>* vt=new std::vector<float>;	
		aTexCoords->push_back(vt);
		aNormals->push_back(vn);
		aMaterials->push_back(NULL);
	}
	int ind=aVertices->size()-1;
	int valNumb=4;
	if ((x[2]==x[3])&&(y[2]==y[3])&&(z[2]==z[3])) 
		valNumb=3;
	int initialSize=aVertices->at(ind)->size()/3;
	for (int i=0;i<valNumb;i++)
	{
		aVertices->at(ind)->push_back(x[i]);
		aVertices->at(ind)->push_back(y[i]);
		aVertices->at(ind)->push_back(z[i]);
	}
	aIndices->at(ind)->push_back(initialSize+0);
	aIndices->at(ind)->push_back(initialSize+1);
	aIndices->at(ind)->push_back(initialSize+2);
	numberOfTrianglesRead++;
	if (valNumb==4)
	{
		aIndices->at(ind)->push_back(initialSize+0);
		aIndices->at(ind)->push_back(initialSize+2);
		aIndices->at(ind)->push_back(initialSize+3);
		numberOfTrianglesRead++;
	}
}

void CDxfFile::_polylineCommand(VArchive &archive,
					std::vector<std::vector<float>*>* aVertices,
					std::vector<std::vector<int>*>* aIndices,
					std::vector<std::string>* groupNames)
{
	tempVertices.clear();
	tempIndices.clear();
	bool pass1=false;
	while (_getCode(archive))
	{
		if ((code=="70")&&(atoi(value.c_str())>=64))
			pass1=true;
		while ((code=="0")&&(value=="VERTEX"))
			_vertexInBlock(archive);	// Even not in a block, we can call it!
		if ((code=="0")&&(value=="SEQEND")) 
			break;
	}
	if (!pass1)	// if it is not a polyface mesh, erase the arrays
	{
		tempVertices.clear();
		tempIndices.clear();
	}
}

bool CDxfFile::exportFunc(std::string fileName,bool poly,const std::vector<std::vector<float>*>& vertices,const std::vector<std::vector<int>*>& indices,const std::vector<std::string>& names)
{
	VFile myFile(fileName.c_str(),VFile::CREATE|VFile::WRITE|VFile::SHARE_EXCLUSIVE);
	myFile.setLength(0);
	VArchive archive(&myFile,VArchive::STORE);
	_writeCode(archive,0,"SECTION");
	_writeCode(archive,2,"HEADER");
	_writeCode(archive,0,"ENDSEC");
	_writeCode(archive,0,"SECTION");
	_writeCode(archive,2,"BLOCKS");
	for (int i=0;i<int(vertices.size());i++)
	{
		_writeCode(archive,0,"BLOCK");
		_writeCode(archive,2,names[i]);
		_writeCodeInt(archive,70,64);
		_writeCode(archive,10,"0.0");
		_writeCode(archive,20,"0.0");
		_writeCode(archive,30,"0.0");
		_writeCode(archive,3,names[i]);
		if (poly)
		{
			_writeCode(archive,0,"POLYLINE");
			_writeCode(archive,66,"1");
			_writeCode(archive,70,"64");
		}
		int initialSize=0;	// only used by polyline
		if (poly)
			_writeAMeshWithPolyline(*vertices[i],*indices[i],archive,initialSize);
		else
			_writeAMeshWith3DFace(*vertices[i],*indices[i],archive);
		if (poly)
			_writeCode(archive,0,"SEQEND");
		_writeCode(archive,0,"ENDBLK");
	}

	_writeCode(archive,0,"ENDSEC");
	_writeCode(archive,0,"SECTION");
	_writeCode(archive,2,"ENTITIES");
	for (int i=0;i<int(vertices.size());i++)
	{
		_writeCode(archive,0,"INSERT");
		_writeCode(archive,2,names[i]);
		_writeCode(archive,10,"0.0");
		_writeCode(archive,20,"0.0");
		_writeCode(archive,30,"0.0");
		_writeCode(archive,41,"1.0");
		_writeCode(archive,42,"1.0");
		_writeCode(archive,43,"1.0");
		_writeCode(archive,50,"0.0");
	}

	_writeCode(archive,0,"ENDSEC");
	_writeCode(archive,0,"EOF");
	archive.close();
	myFile.close();
	return(true);
}

bool CDxfFile::exportFunc(std::string fileName,bool poly,std::vector<int>* sel)
{
	VFile myFile(fileName.c_str(),VFile::CREATE|VFile::WRITE|VFile::SHARE_EXCLUSIVE);
	myFile.setLength(0);
	VArchive archive(&myFile,VArchive::STORE);
	_writeCode(archive,0,"SECTION");
	_writeCode(archive,2,"HEADER");
	_writeCode(archive,0,"ENDSEC");
	_writeCode(archive,0,"SECTION");
	_writeCode(archive,2,"BLOCKS");
	std::vector<int> objects(*sel);
	// Check number of triangles for the progress dialog
	triangleCountExport=0;
	totalTriangleExport=0;
	for (int i=0;i<int(objects.size());i++)
	{
		CShape* it=App::ct->objCont->getShape(objects[i]);
		if (it!=NULL)
		{
			std::vector<float> wvert;
			std::vector<int> wind;
			it->geomData->geomInfo->getCumulativeMeshes(wvert,&wind,NULL);
			totalTriangleExport+=wind.size()/3;
		}
	}
	for (int i=0;i<int(objects.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(objects[i]);
		if (it->getObjectType()==sim_object_shape_type)
		{
			_writeCode(archive,0,"BLOCK");
			_writeCode(archive,2,it->getName());
			_writeCodeInt(archive,70,64);
			_writeCode(archive,10,"0.0");
			_writeCode(archive,20,"0.0");
			_writeCode(archive,30,"0.0");
			_writeCode(archive,3,it->getName());
			if ( poly&&(it->getObjectType()==sim_object_shape_type) )
			{ // Only shapes can be exported as POLYLINE
				_writeCode(archive,0,"POLYLINE");
				_writeCode(archive,66,"1");
				_writeCode(archive,70,"64");
			}
			int initialSize=0;	// only used by polyline
			if (it->getObjectType()==sim_object_shape_type)
			{
				if (poly)
					_writeAShapeWithPolyline((CShape*)it,archive,initialSize);
				else
					_writeAShapeWith3DFace((CShape*)it,archive);
			}
			if ( poly&&(it->getObjectType()==sim_object_shape_type) ) 
				_writeCode(archive,0,"SEQEND");
			_writeCode(archive,0,"ENDBLK");
		}
	}

	_writeCode(archive,0,"ENDSEC");
	_writeCode(archive,0,"SECTION");
	_writeCode(archive,2,"ENTITIES");
	for (int i=0;i<int(objects.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(objects[i]);
		if (it->getObjectType()==sim_object_shape_type)
		{
			_writeCode(archive,0,"INSERT");
			_writeCode(archive,2,it->getName());
			_writeCode(archive,10,"0.0");
			_writeCode(archive,20,"0.0");
			_writeCode(archive,30,"0.0");
			_writeCode(archive,41,"1.0");
			_writeCode(archive,42,"1.0");
			_writeCode(archive,43,"1.0");
			_writeCode(archive,50,"0.0");
		}
	}

	_writeCode(archive,0,"ENDSEC");
	_writeCode(archive,0,"EOF");
	archive.close();
	myFile.close();
	return(true);
}

void CDxfFile::_writeAShapeWith3DFace(CShape* it,VArchive& archive)
{
	C7Vector m(it->getCumulativeTransformationPart1());
	std::vector<float> wvert;
	std::vector<int> wind;
	it->geomData->geomInfo->getCumulativeMeshes(wvert,&wind,NULL);
	for (int j=0;j<int(wind.size())/3;j++)
	{	
		_writeCode(archive,0,"3DFACE");
		C3Vector c1;
		C3Vector c2;
		C3Vector c3;
		c1(0)=wvert[3*wind[3*j+0]+0];
		c1(1)=wvert[3*wind[3*j+0]+1];
		c1(2)=wvert[3*wind[3*j+0]+2];
		c2(0)=wvert[3*wind[3*j+1]+0];
		c2(1)=wvert[3*wind[3*j+1]+1];
		c2(2)=wvert[3*wind[3*j+1]+2];
		c3(0)=wvert[3*wind[3*j+2]+0];
		c3(1)=wvert[3*wind[3*j+2]+1];
		c3(2)=wvert[3*wind[3*j+2]+2];
		c1=m*c1;
		c2=m*c2;
		c3=m*c3;
		_writeCodeFloat(archive,10,c1(0));
		_writeCodeFloat(archive,20,c1(1));
		_writeCodeFloat(archive,30,c1(2));
		_writeCodeFloat(archive,11,c2(0));
		_writeCodeFloat(archive,21,c2(1));
		_writeCodeFloat(archive,31,c2(2));
		_writeCodeFloat(archive,12,c3(0));
		_writeCodeFloat(archive,22,c3(1));
		_writeCodeFloat(archive,32,c3(2));
		_writeCodeFloat(archive,13,c3(0));
		_writeCodeFloat(archive,23,c3(1));
		_writeCodeFloat(archive,33,c3(2));
		triangleCountExport++;
		_writeCode(archive,0,"SEQEND");
	}
}

void CDxfFile::_writeAShapeWithPolyline(CShape* it,VArchive& archive,int& initialSize)
{
	C7Vector m(it->getCumulativeTransformationPart1());
	std::vector<float> wvert;
	std::vector<int> wind;
	it->geomData->geomInfo->getCumulativeMeshes(wvert,&wind,NULL);
	for (int i=0;i<int(wvert.size())/3;i++)
	{
		_writeCode(archive,0,"VERTEX");
		C3Vector c1(wvert[3*i+0],wvert[3*i+1],wvert[3*i+2]);
		c1=m*c1;
		_writeCodeFloat(archive,10,c1(0));
		_writeCodeFloat(archive,20,c1(1));
		_writeCodeFloat(archive,30,c1(2));
		_writeCode(archive,70,"192");
	}
	for (int i=0;i<int(wind.size())/3;i++)
	{
		_writeCode(archive,0,"VERTEX");
		_writeCode(archive,10,"0.0");
		_writeCode(archive,20,"0.0");
		_writeCode(archive,30,"0.0");
		_writeCode(archive,70,"128");
		_writeCodeInt(archive,71,wind[3*i+0]+1+initialSize);
		_writeCodeInt(archive,72,wind[3*i+1]+1+initialSize);
		_writeCodeInt(archive,73,wind[3*i+2]+1+initialSize);
		triangleCountExport++;
	}
	initialSize=initialSize+wvert.size()/3;
}


void CDxfFile::_writeAMeshWith3DFace(const std::vector<float>& vertices,const std::vector<int>& indices,VArchive& archive)
{
	for (int j=0;j<int(indices.size())/3;j++)
	{	
		_writeCode(archive,0,"3DFACE");
		C3Vector c1;
		C3Vector c2;
		C3Vector c3;
		c1(0)=vertices[3*indices[3*j+0]+0];
		c1(1)=vertices[3*indices[3*j+0]+1];
		c1(2)=vertices[3*indices[3*j+0]+2];
		c2(0)=vertices[3*indices[3*j+1]+0];
		c2(1)=vertices[3*indices[3*j+1]+1];
		c2(2)=vertices[3*indices[3*j+1]+2];
		c3(0)=vertices[3*indices[3*j+2]+0];
		c3(1)=vertices[3*indices[3*j+2]+1];
		c3(2)=vertices[3*indices[3*j+2]+2];
		_writeCodeFloat(archive,10,c1(0));
		_writeCodeFloat(archive,20,c1(1));
		_writeCodeFloat(archive,30,c1(2));
		_writeCodeFloat(archive,11,c2(0));
		_writeCodeFloat(archive,21,c2(1));
		_writeCodeFloat(archive,31,c2(2));
		_writeCodeFloat(archive,12,c3(0));
		_writeCodeFloat(archive,22,c3(1));
		_writeCodeFloat(archive,32,c3(2));
		_writeCodeFloat(archive,13,c3(0));
		_writeCodeFloat(archive,23,c3(1));
		_writeCodeFloat(archive,33,c3(2));
		triangleCountExport++;
		_writeCode(archive,0,"SEQEND");
	}
}

void CDxfFile::_writeAMeshWithPolyline(const std::vector<float>& vertices,const std::vector<int>& indices,VArchive& archive,int& initialSize)
{
	for (int i=0;i<int(vertices.size())/3;i++)
	{
		_writeCode(archive,0,"VERTEX");
		C3Vector c1(vertices[3*i+0],vertices[3*i+1],vertices[3*i+2]);
		_writeCodeFloat(archive,10,c1(0));
		_writeCodeFloat(archive,20,c1(1));
		_writeCodeFloat(archive,30,c1(2));
		_writeCode(archive,70,"192");
	}
	for (int i=0;i<int(indices.size())/3;i++)
	{
		_writeCode(archive,0,"VERTEX");
		_writeCode(archive,10,"0.0");
		_writeCode(archive,20,"0.0");
		_writeCode(archive,30,"0.0");
		_writeCode(archive,70,"128");
		_writeCodeInt(archive,71,indices[3*i+0]+1+initialSize);
		_writeCodeInt(archive,72,indices[3*i+1]+1+initialSize);
		_writeCodeInt(archive,73,indices[3*i+2]+1+initialSize);
		triangleCountExport++;
	}
	initialSize=initialSize+vertices.size()/3;
}


