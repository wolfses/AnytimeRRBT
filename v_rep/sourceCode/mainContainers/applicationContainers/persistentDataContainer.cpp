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
#include "persistentDataContainer.h"
#include "App.h"
#include "VVarious.h"

CPersistentDataContainer::CPersistentDataContainer()
{
	_filename="persistentData.dat";
	initializeWithDataFromFile();
}

CPersistentDataContainer::CPersistentDataContainer(const std::string& filename)
{
	_filename=filename;
	initializeWithDataFromFile();
}

CPersistentDataContainer::~CPersistentDataContainer()
{
	removeAllData();
}

int CPersistentDataContainer::removeAllData()
{
	int retVal=int(_dataNames.size());
	_dataNames.clear();
	_dataValues.clear();
	return(retVal);
}

void CPersistentDataContainer::writeData(const char* dataName,const std::string& value,bool toFile)
{
	_writeData(dataName,value);
	if (toFile)
	{
		std::vector<std::string> _dataNamesAux;
		std::vector<std::string> _dataValuesAux;
		_readFromFile(_dataNamesAux,_dataValuesAux);
		_dataNames.swap(_dataNamesAux);
		_dataValues.swap(_dataValuesAux);
		_writeData(dataName,value);
		_dataNames.swap(_dataNamesAux);
		_dataValues.swap(_dataValuesAux);
		_writeToFile(_dataNamesAux,_dataValuesAux);
	}
}

void CPersistentDataContainer::_writeData(const char* dataName,const std::string& value)
{
	if (dataName!=NULL)
	{
		int index=_getDataIndex(dataName);
		if (index==-1)
		{
			if (strlen(dataName)!=0)
			{ // we have to add this data:
				_dataNames.push_back(dataName);
				_dataValues.push_back(value);
			}
		}
		else
		{ 
			if (value.length()!=0)
				_dataValues[index]=value; // we have to update this data:
			else
			{ // we have to remove this data:
				_dataNames.erase(_dataNames.begin()+index);
				_dataValues.erase(_dataValues.begin()+index);
			}
		}
	}
}

bool CPersistentDataContainer::readData(const char* dataName,std::string& value)
{
	if ((dataName==NULL)||(strlen(dataName)==0))
		return(false);
	int index=_getDataIndex(dataName);
	if (index==-1)
		return(false);
	value=_dataValues[index];
	return(true);
}

int CPersistentDataContainer::_getDataIndex(const char* dataName)
{
	for (int i=0;i<int(_dataNames.size());i++)
	{
		if (_dataNames[i].compare(dataName)==0)
			return(i);
	}
	return(-1);
}

void CPersistentDataContainer::initializeWithDataFromFile()
{
	_readFromFile(_dataNames,_dataValues);
}


void CPersistentDataContainer::_readFromFile(std::vector<std::string>& dataNames,std::vector<std::string>& dataValues)
{
	dataNames.clear();
	dataValues.clear();
	std::string filenameAndPath(VVarious::splitPath_path(VVarious::getModulePathAndFile())+VREP_SLASH+V_REP_SYSTEM_DIRECTORY_NAME+VREP_SLASH+_filename.c_str());
	if (VFile::doesFileExist(filenameAndPath))
	{
		try
		{
			VFile file(filenameAndPath,VFile::READ|VFile::SHARE_DENY_NONE);
			VArchive archive(&file,VArchive::LOAD);
			_serialize(archive,dataNames,dataValues);
			archive.close();
			file.close();
		}
		catch(VFILE_EXCEPTION_TYPE e)
		{
			// silent error since 3/2/2012: when the system folder dowesn't exist, we don't want an error!!    VFile::reportAndHandleFileExceptionError(e);
		}
	}
}

void CPersistentDataContainer::_writeToFile(std::vector<std::string>& dataNames,std::vector<std::string>& dataValues)
{
	std::string filenameAndPath(VVarious::splitPath_path(VVarious::getModulePathAndFile())+VREP_SLASH+V_REP_SYSTEM_DIRECTORY_NAME+VREP_SLASH+_filename.c_str());
	try
	{
		VFile myFile(filenameAndPath,VFile::CREATE|VFile::WRITE|VFile::SHARE_EXCLUSIVE);
		myFile.setLength(0);
		VArchive archive(&myFile,VArchive::STORE);
		_serialize(archive,dataNames,dataValues);
		archive.close();
		myFile.close();
	}
	catch(VFILE_EXCEPTION_TYPE e)
	{
		// silent error since 3/2/2012: when the system folder dowesn't exist, we don't want an error!!    VFile::reportAndHandleFileExceptionError(e);
	}
}

void CPersistentDataContainer::_serialize(VArchive& ar,std::vector<std::string>& dataNames,std::vector<std::string>& dataValues)
{
	if (ar.isStoring())
	{ // Storing
		ar << int(dataNames.size());
		for (int i=0;i<int(dataNames.size());i++)
		{
			ar << (unsigned char) (1);

			ar << int(dataNames[i].size());
			for (int j=0;j<int(dataNames[i].size());j++)
				ar << (unsigned char)(dataNames[i][j]);

			ar << int(dataValues[i].size());
			for (int j=0;j<int(dataValues[i].size());j++)
				ar << (unsigned char)(dataValues[i][j]);
		}
		ar << (unsigned char) (0);
	}
	else
	{		// Loading
		int count;
		ar >> count;
		if (count>0)
		{
			unsigned char d=1;
			while (d!=0)
			{
				ar >> d;
				if (d!=0)
				{
					std::string dat;
					std::string val;

					int l;
					unsigned char v;
					ar >> l;
					for (int i=0;i<l;i++)
					{
						ar >> v;
						dat+=v;
					}

					ar >> l;
					for (int i=0;i<l;i++)
					{
						ar >> v;
						val+=v;
					}

					dataNames.push_back(dat);
					dataValues.push_back(val);
				}
			}
		}
	}
}

