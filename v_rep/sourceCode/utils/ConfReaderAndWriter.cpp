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
#include "ConfReaderAndWriter.h"
#include "Tt.h"
#include "VFile.h"
#include "VArchive.h"
#include <boost/lexical_cast.hpp>

CConfReaderAndWriter::CConfReaderAndWriter()
{
}

CConfReaderAndWriter::~CConfReaderAndWriter()
{
}

bool CConfReaderAndWriter::readConfiguration(const char* filename)
{
	bool retVal=false;
	if (VFile::doesFileExist(filename))
	{
		try
		{
			VFile file(filename,VFile::READ|VFile::SHARE_DENY_NONE);
			VArchive archive(&file,VArchive::LOAD);
			DWORD actualPosition=0;
			std::string line;
			while (archive.readLine(actualPosition,line,false))
			{
				tt::removeComments(line);
				size_t ePos=line.find('=');
				if ( (ePos!=std::string::npos)&&(ePos>0)&&(ePos+1<line.length()) )
				{
					std::string var,val;
					var.assign(line.begin(),line.begin()+ePos);
					val.assign(line.begin()+ePos+1,line.end());
					if ( tt::removeSpacesAtBeginningAndEnd(var)&&tt::removeSpacesAtBeginningAndEnd(val)&&(var.find(' ')==std::string::npos) )
					{
						int ind=_getVariableIndex((const char*)var.c_str());
						if (ind!=-1)
						{ // Variable multiple defined. We keep the last one!
							_variables.erase(_variables.begin()+ind);
							_values.erase(_values.begin()+ind);
							_comments.erase(_comments.begin()+ind);
						}
						_variables.push_back(var);
						_values.push_back(val);
						_comments.push_back("");
					}
				}
			}
			archive.close();
			file.close();
			retVal=true;
		}
		catch(VFILE_EXCEPTION_TYPE e)
		{
			VFile::reportAndHandleFileExceptionError(e);
			retVal=false;
		}
	}
	return(retVal);
}

bool CConfReaderAndWriter::writeConfiguration(const char* filename)
{
	bool retVal=false;
	try
	{
		VFile myFile(filename,VFile::CREATE|VFile::WRITE|VFile::SHARE_EXCLUSIVE);
		myFile.setLength(0);
		VArchive archive(&myFile,VArchive::STORE);

		std::string line;
		for (int i=0;i<int(_variables.size());i++)
		{
			if (_variables[i].length()>0)
			{
				line=_variables[i]+" = ";
				line+=_values[i];
				if (_comments[i]!="")
				{
					line+=" // ";
					line+=_comments[i];
				}
			}
			else
				line=_comments[i];
			archive.writeLine(line);
		}
		archive.close();
		myFile.close();
		retVal=true;
	}
	catch(VFILE_EXCEPTION_TYPE e)
	{
		VFile::reportAndHandleFileExceptionError(e);
		retVal=false;
	}
	return(retVal);
}

bool CConfReaderAndWriter::getString(const char* variableName,std::string& variable)
{
	int ind=_getVariableIndex(variableName);
	if (ind==-1)
		return(false);
	variable=_values[ind];
	return(true);
}

bool CConfReaderAndWriter::getBoolean(const char* variableName,bool& variable)
{
	int ind=_getVariableIndex(variableName);
	if (ind==-1)
		return(false);
	std::string val=_values[ind];
	for (int i=0;i<int(val.length());i++)
	{
		if (val[i]>='a')
			val[i]-=('a'-'A');
	}
	if (val.compare("TRUE")==0)
	{
		variable=true;
		return(true);
	}
	if (val.compare("FALSE")==0)
	{
		variable=false;
		return(true);
	}
	return(false);
}


bool CConfReaderAndWriter::getInteger(const char* variableName,int& variable)
{
	int ind=_getVariableIndex(variableName);
	if (ind==-1)
		return(false);
	return(tt::getValidInt(_values[ind],variable));
}

bool CConfReaderAndWriter::getFloat(const char* variableName,float& variable)
{
	int ind=_getVariableIndex(variableName);
	if (ind==-1)
		return(false);
	return(tt::getValidFloat(_values[ind],variable));
}

bool CConfReaderAndWriter::getVector(const char* variableName,float variable[3])
{
	int ind=_getVariableIndex(variableName);
	if (ind==-1)
		return(false);
	std::string v=_values[ind];
	size_t cPos=v.find(',');
	std::string tmp;
	if ( (cPos!=std::string::npos)&&(cPos>0)&&(cPos+1<v.length()) )
	{
		tmp.assign(v.begin(),v.begin()+cPos);
		if (!tt::getValidFloat(tmp,variable[0]))
			return(false);
		tmp.assign(v.begin()+cPos+1,v.end());
		v=tmp;
		cPos=v.find(',');
		if ( (cPos!=std::string::npos)&&(cPos>0)&&(cPos+1<v.length()) )
		{
			tmp.assign(v.begin(),v.begin()+cPos);
			if (!tt::getValidFloat(tmp,variable[1]))
				return(false);
			tmp.assign(v.begin()+cPos+1,v.end());
			return(tt::getValidFloat(tmp,variable[2]));
		}
	}
	return(false);
}

bool CConfReaderAndWriter::addString(const char* variableName,std::string variable,const char* comment)
{
	if (_getVariableIndex(variableName)!=-1)
		return(false); // variable already present
	_variables.push_back(variableName);
	_comments.push_back(comment);
	_values.push_back(variable);
	return(true);
}

bool CConfReaderAndWriter::addInteger(const char* variableName,int variable,const char* comment)
{
	if (_getVariableIndex(variableName)!=-1)
		return(false); // variable already present
	_variables.push_back(variableName);
	_comments.push_back(comment);
	_values.push_back(boost::lexical_cast<std::string>(variable));
	return(true);
}

bool CConfReaderAndWriter::addFloat(const char* variableName,float variable,const char* comment)
{
	if (_getVariableIndex(variableName)!=-1)
		return(false); // variable already present
	_variables.push_back(variableName);
	_comments.push_back(comment);
	_values.push_back(boost::lexical_cast<std::string>(variable));
	return(true);
}

bool CConfReaderAndWriter::addRandomLine(const char* lineText)
{
	_variables.push_back("");
	_comments.push_back(lineText);
	_values.push_back("");
	return(true);
}

bool CConfReaderAndWriter::addVector(const char* variableName,float variable[3],const char* comment)
{
	if (_getVariableIndex(variableName)!=-1)
		return(false); // variable already present
	_variables.push_back(variableName);
	_comments.push_back(comment);
	std::string tmp=boost::lexical_cast<std::string>(variable[0])+",";
	tmp+=boost::lexical_cast<std::string>(variable[1])+",";
	tmp+=boost::lexical_cast<std::string>(variable[2]);
	_values.push_back(tmp);
	return(true);
}

bool CConfReaderAndWriter::addBoolean(const char* variableName,bool variable,const char* comment)
{
	if (_getVariableIndex(variableName)!=-1)
		return(false); // variable already present
	_variables.push_back(variableName);
	_comments.push_back(comment);
	if (variable)
		_values.push_back("true");
	else
		_values.push_back("false");
	return(true);
}

int CConfReaderAndWriter::_getVariableIndex(const char* variableName)
{
	for (int i=0;i<int(_variables.size());i++)
	{
		if (_variables[i].compare(variableName)==0)
			return(i);
	}
	return(-1);
}
