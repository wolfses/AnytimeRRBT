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

#include "VArchive.h"
#include <vector>

#define SER_VREP_HEADER "VREP" // The file header since 03/07/2009
#define SER_END_OF_OBJECT "EOO"
#define SER_NEXT_STEP "NXT"
#define SER_END_OF_FILE "EOF"

class CSer  
{
public:
	CSer(VArchive& ar);
	CSer(std::vector<char>& bufferArchive);
	virtual ~CSer();

	void writeOpen();
	void writeClose(bool compress);

	int readOpen(int& serializationVersion,WORD& vrepVersionThatWroteThis,int& licenseTypeThatWroteThis);
	void readClose();

	CSer& operator<< (const int& v);
	CSer& operator<< (const float& v);
	CSer& operator<< (const double& v);
	CSer& operator<< (const WORD& v);
	CSer& operator<< (const DWORD& v);
	CSer& operator<< (const long& v);
	CSer& operator<< (const unsigned char& v);
	CSer& operator<< (const char& v);
	CSer& operator<< (const std::string& v);

	CSer& operator>> (int& v);
	CSer& operator>> (float& v);
	CSer& operator>> (double& v);
	CSer& operator>> (WORD& v);
	CSer& operator>> (DWORD& v);
	CSer& operator>> (long& v);
	CSer& operator>> (unsigned char& v);
	CSer& operator>> (char& v);
	CSer& operator>> (std::string& v);

	void flush(bool writeNbOfBytes=true);
	
	bool isStoring();
	void setCountingMode(bool force=false);
	bool setWritingMode(bool force=false);
	void disableCountingModeExceptForExceptions();
	std::vector<unsigned char>* getBufferPointer();
	std::vector<unsigned char>* getFileBuffer();
	int getFileBufferReadPointer();
	void addOffsetToFileBufferReadPointer(int off);
	int getCounter();
	void loadUnknownData();
	bool getFoundUnknownCommands();

	WORD getVrepVersionThatWroteThisFile();
	int getLicenseTypeThatWroteThisFile();
	int getSerializationVersionThatWroteThisFile();
	
	void storeDataName(const char* name);
	std::string readDataName();
	int readBytesButKeepPointerUnchanged(BYTE* buffer,int desiredCount);
	VArchive& getArchive();

	static int SER_SERIALIZATION_VERSION;
	static int SER_MIN_SERIALIZATION_VERSION_THAT_CAN_READ_THIS;
	static int SER_MIN_SERIALIZATION_VERSION_THAT_THIS_CAN_READ;

private:
	void _commonInit();
	VArchive* theArchive;
	std::vector<char>* _bufferArchive;

	int counter;
	int countingMode;
	bool _coutingModeDisabledExceptForExceptions;
	std::vector<unsigned char> buffer;
	std::vector<unsigned char> _fileBuffer;
	int _fileBufferReadPointer;
	bool _foundUnknownCommands;

	WORD _vrepVersionThatWroteThis;
	int _licenseTypeThatWroteThis;
	int _serializationVersionThatWroteThisFile;
};