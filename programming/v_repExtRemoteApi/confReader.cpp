// This file is part of the REMOTE API
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The REMOTE API is licensed under the terms of GNU GPL:
// 
// -------------------------------------------------------------------
// The REMOTE API is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// THE REMOTE API IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with the REMOTE API.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#include "confReader.h"
#include <fstream>
#include <stdlib.h>

CConfReader::CConfReader()
{
}

CConfReader::~CConfReader()
{
}

bool CConfReader::readConfiguration(const char* filename)
{
	bool retVal=false;
	std::ifstream file;
	file.open(filename);
	if (file.is_open())
	{
		while (!file.eof())
		{
			char l[400];
			file.getline(l,400);
			std::string line(l);
			if ((line.length()!=0)&&(line[line.length()-1]==13)) // Needed on Linux. Thanks to Patrick Bouffard
				line.erase(line.begin()+line.length()-1);
			_removeComments(line);
			size_t ePos=line.find('=');
			if ( (ePos!=std::string::npos)&&(ePos>0)&&(ePos+1<line.length()) )
			{
				std::string var,val;
				var.assign(line.begin(),line.begin()+ePos);
				val.assign(line.begin()+ePos+1,line.end());
				if ( _removeFrontAndBackSpacesAndTabs(var)&&_removeFrontAndBackSpacesAndTabs(val)&&(var.find(' ')==std::string::npos) )
				{
					int ind=_getVariableIndex((const char*)var.c_str());
					if (ind!=-1)
					{ // Variable multiple defined. We keep the last one!
						_variables.erase(_variables.begin()+ind);
						_values.erase(_values.begin()+ind);
					}
					_variables.push_back(var);
					_values.push_back(val);
				}
			}
		}
		file.close();
		retVal=true;
	}
	return(retVal);
}

bool CConfReader::getString(const char* variableName,std::string& variable)
{
	int ind=_getVariableIndex(variableName);
	if (ind==-1)
		return(false);
	variable=_values[ind];
	return(true);
}

bool CConfReader::getBoolean(const char* variableName,bool& variable)
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


bool CConfReader::getInteger(const char* variableName,int& variable)
{
	int ind=_getVariableIndex(variableName);
	if (ind==-1)
		return(false);
	variable=atoi(_values[ind].c_str());
	return(true);
}

bool CConfReader::getFloat(const char* variableName,float& variable)
{
	int ind=_getVariableIndex(variableName);
	if (ind==-1)
		return(false);
	variable=float(atof(_values[ind].c_str()));
	return(true);
}

int CConfReader::_getVariableIndex(const char* variableName)
{
	for (int i=0;i<int(_variables.size());i++)
	{
		if (_variables[i].compare(variableName)==0)
			return(i);
	}
	return(-1);
}

void CConfReader::_removeComments(std::string& line)
{
	for (int i=0;i<int(line.length())-1;i++)
	{
		if ( (line[i]=='/')&&(line[i+1]=='/') )
		{
			line.erase(line.begin()+i,line.end());
			return;
		}
	}
}

bool CConfReader::_removeFrontAndBackSpacesAndTabs(std::string& line)
{
	while ((line.length()!=0)&&( (line[0]==' ')||(line[0]==9) ) )
		line.erase(line.begin());
	while ((line.length()!=0)&&( (line[line.length()-1]==' ')||(line[line.length()-1]==9)  ) )
		line.erase(line.begin()+line.length()-1);
	return(line.length()!=0);
}

