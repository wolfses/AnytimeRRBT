// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// -------------------------------------------------------------------
// THIS FILE IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// You are free to use/modify/distribute this file for whatever purpose!
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

//**************************************************
// This static class basically contains all objects
// of the plugin. In particular, it holds
// the dialog of the plugin, and also all
// mtbRobot objects related to MTB's models
// in V-REP. Each MTB model in V-REP should
// have its corresponding object (CMtbRobot) in
// this plugin. 
//**************************************************

#include "Access.h"
#include "v_repLib.h"

CMtbRobotContainer* CAccess::mtbRobotContainer=NULL;
CMtbRobotDialog* CAccess::mtbRobotDialog=NULL; // MTB dialog
std::string CAccess::currentDirAndPath;

CAccess::CAccess()
{ // Fully static class
}

CAccess::~CAccess()
{ // Fully static class
}

void CAccess::createNonGui()
{ // Set-up of data structures:
	mtbRobotContainer=new CMtbRobotContainer();
}

void CAccess::destroyNonGui()
{ // Clean-up all non-GUI stuff
	delete mtbRobotContainer;
	mtbRobotContainer=NULL;
}

void CAccess::createGui()
{
	QWidget* pMainWindow = (QWidget*)simGetMainWindow(1);
	mtbRobotDialog=new CMtbRobotDialog(pMainWindow); // The plugin dialog
	simAddModuleMenuEntry("",1,&mtbRobotDialog->dialogMenuItemHandle);
	simSetModuleMenuItemState(mtbRobotDialog->dialogMenuItemHandle,1,"MTB Robot Properties");
}

void CAccess::destroyGui()
{ // Clean-up GUI
	delete mtbRobotDialog;
	mtbRobotDialog=NULL;
}

int CAccess::getDataLocationAndSize(const std::vector<unsigned char>& buffer,int dataName,int& theSize)
{
	int p=0;
	while (p<=int(buffer.size()-8))
	{
		int n,l;
		for (int i=0;i<4;i++)
			((unsigned char*)&n)[i]=buffer[p+i];
		for (int i=0;i<4;i++)
			((unsigned char*)&l)[i]=buffer[p+4+i];
		if (n==dataName)
		{
			if (l!=0)
			{
				theSize=l;
				return(p+8);
			}
			return(-1); // shouldn't happen if data saved correctly
		}
		p+=8+l;
	}
	return(-1); // data not present
}

bool CAccess::extractSerializationData(std::vector<unsigned char>& buffer,int dataName,std::vector<unsigned char>& data)
{ // Extracts the data saved under the dataName item (see header file on data organization)
	data.clear();
	int l;
	int p=getDataLocationAndSize(buffer,dataName,l);
	if (p!=-1)
	{
		for (int i=0;i<l;i++)
			data.push_back(buffer[p+i]);
		buffer.erase(buffer.begin()+p-8,buffer.begin()+p+l);
		return(true);
	}
	return(false); // that data was not present!
}

void CAccess::insertSerializationData(std::vector<unsigned char>& buffer,int dataName,const std::vector<unsigned char>& data)
{ // Inserts data under the dataName item (see header file on data organization)
	// 1. We remove the old data:
	std::vector<unsigned char> dummyBuffer;
	extractSerializationData(buffer,dataName,dummyBuffer);
	// 2. and insert the new data:
	// 2.1 data name:
	for (int i=0;i<4;i++)
		buffer.push_back(((unsigned char*)&dataName)[i]);
	// 2.2 Data length:
	int l=int(data.size());
	for (int i=0;i<4;i++)
		buffer.push_back(((unsigned char*)&l)[i]);
	// 2.3 The data itself:
	for (int i=0;i<l;i++)
		buffer.push_back(data[i]);
}

void CAccess::push_int(std::vector<unsigned char>& buffer,int data)
{
	for (int i=0;i<int(sizeof(data));i++)
		buffer.push_back(((unsigned char*)&data)[i]);
}

void CAccess::push_float(std::vector<unsigned char>& buffer,float data)
{
	for (int i=0;i<int(sizeof(data));i++)
		buffer.push_back(((unsigned char*)&data)[i]);
}

int CAccess::pop_int(std::vector<unsigned char>& buffer)
{
	int retVal=0;
	if (buffer.size()<sizeof(retVal))
		return(retVal);
	for (int i=sizeof(retVal)-1;i>=0;i--)
	{
		((unsigned char*)&retVal)[i]=buffer[buffer.size()-1];
		buffer.pop_back();
	}
	return(retVal);
}

float CAccess::pop_float(std::vector<unsigned char>& buffer)
{
	float retVal=0.0f;
	if (buffer.size()<sizeof(retVal))
		return(retVal);
	for (int i=sizeof(retVal)-1;i>=0;i--)
	{
		((unsigned char*)&retVal)[i]=buffer[buffer.size()-1];
		buffer.pop_back();
	}
	return(retVal);
}
