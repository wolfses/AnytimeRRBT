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
#include "Ser.h"
#include "huffman.h"
#include "IloIlo.h"
#include "v_repStrings.h"
#include "App.h"

int CSer::SER_SERIALIZATION_VERSION=17; // 9 since 2008/09/01,
										// 10 since 2009/02/14,
										// 11 since 2009/05/15,
										// 12 since 2009/07/03,
										// 13 since 2009/07/21,
										// 14 since 2010/02/26
										// 15 since 2010/04/17 (after removal of CGeomCont and cloning option for geom resources)
										// 16 since 2012/11/10 (after rewritten shape and geometric classes. We do not support serialization version 14 and earlier anymore!)
										// 17 since 2013/08/29 (after correcting for light intensity and object colors)

int CSer::SER_MIN_SERIALIZATION_VERSION_THAT_CAN_READ_THIS=16; // means: files written with this can be read by older v-rep with serialization THE_NUMBER
int CSer::SER_MIN_SERIALIZATION_VERSION_THAT_THIS_CAN_READ=15; // means: this executable can read versions >=THE_NUMBER
																// Above raised from 10 to 14 on 2010/03/02 (to be able to clean up all serialization routines soon)

CSer::CSer(VArchive& ar)
{ // When reading: if serializationVersion==-1 --> not recognized
	_commonInit();
	theArchive=&ar;
}

CSer::CSer(std::vector<char>& bufferArchive)
{ // if bufferArchive is empty, we are storing, otherwise we are restoring
	_commonInit();
	_bufferArchive=&bufferArchive;
}

CSer::~CSer()
{
}

void CSer::_commonInit()
{
	theArchive=NULL;
	_bufferArchive=NULL;
	countingMode=0;
	counter=0;
	_coutingModeDisabledExceptForExceptions=false;
	buffer.reserve(100000);
	_fileBuffer.reserve(1000000);
	_fileBuffer.clear();
	_fileBufferReadPointer=0;
	_foundUnknownCommands=false;
}

void CSer::writeOpen()
{
}

void CSer::writeClose(bool compress)
{ // we write the whole file from the fileBuffer:
	// We write the header:
	for (int i=0;i<int(strlen(SER_VREP_HEADER));i++)
	{
		if (theArchive!=NULL)
			(*theArchive) << SER_VREP_HEADER[i];
		else
			(*_bufferArchive).push_back(SER_VREP_HEADER[i]);
	}
	// We write the serialization version:
	if (theArchive!=NULL)
	{
		(*theArchive) << ((char*)&SER_SERIALIZATION_VERSION)[0];
		(*theArchive) << ((char*)&SER_SERIALIZATION_VERSION)[1];
		(*theArchive) << ((char*)&SER_SERIALIZATION_VERSION)[2];
		(*theArchive) << ((char*)&SER_SERIALIZATION_VERSION)[3];
	}
	else
	{
		(*_bufferArchive).push_back(((char*)&SER_SERIALIZATION_VERSION)[0]);
		(*_bufferArchive).push_back(((char*)&SER_SERIALIZATION_VERSION)[1]);
		(*_bufferArchive).push_back(((char*)&SER_SERIALIZATION_VERSION)[2]);
		(*_bufferArchive).push_back(((char*)&SER_SERIALIZATION_VERSION)[3]);
	}

	// We write the minimum sim. version that can read this:
	if (theArchive!=NULL)
	{
		(*theArchive) << ((char*)&SER_MIN_SERIALIZATION_VERSION_THAT_CAN_READ_THIS)[0];
		(*theArchive) << ((char*)&SER_MIN_SERIALIZATION_VERSION_THAT_CAN_READ_THIS)[1];
		(*theArchive) << ((char*)&SER_MIN_SERIALIZATION_VERSION_THAT_CAN_READ_THIS)[2];
		(*theArchive) << ((char*)&SER_MIN_SERIALIZATION_VERSION_THAT_CAN_READ_THIS)[3];
	}
	else
	{
		(*_bufferArchive).push_back(((char*)&SER_MIN_SERIALIZATION_VERSION_THAT_CAN_READ_THIS)[0]);
		(*_bufferArchive).push_back(((char*)&SER_MIN_SERIALIZATION_VERSION_THAT_CAN_READ_THIS)[1]);
		(*_bufferArchive).push_back(((char*)&SER_MIN_SERIALIZATION_VERSION_THAT_CAN_READ_THIS)[2]);
		(*_bufferArchive).push_back(((char*)&SER_MIN_SERIALIZATION_VERSION_THAT_CAN_READ_THIS)[3]);
	}
	// We write the compression method:
	char compressionMethod=0; 
	if (compress)
		compressionMethod=1; // 1 for Huffman
	if (theArchive!=NULL)
		(*theArchive) << char(compressionMethod); 
	else
		(*_bufferArchive).push_back(char(compressionMethod));

	// We write the uncompressed data length:
	int l=_fileBuffer.size();
	if (theArchive!=NULL)
	{
		(*theArchive) << ((char*)&l)[0];
		(*theArchive) << ((char*)&l)[1];
		(*theArchive) << ((char*)&l)[2];
		(*theArchive) << ((char*)&l)[3];
	}
	else
	{
		(*_bufferArchive).push_back(((char*)&l)[0]);
		(*_bufferArchive).push_back(((char*)&l)[1]);
		(*_bufferArchive).push_back(((char*)&l)[2]);
		(*_bufferArchive).push_back(((char*)&l)[3]);
	}

	// We write the compilation version: (since ser ver 13 (2009/07/21))
	int compilVer=6;
	if (theArchive!=NULL)
	{
		(*theArchive) << ((char*)&compilVer)[0];
		(*theArchive) << ((char*)&compilVer)[1];
		(*theArchive) << ((char*)&compilVer)[2];
		(*theArchive) << ((char*)&compilVer)[3];
	}
	else
	{
		(*_bufferArchive).push_back(((char*)&compilVer)[0]);
		(*_bufferArchive).push_back(((char*)&compilVer)[1]);
		(*_bufferArchive).push_back(((char*)&compilVer)[2]);
		(*_bufferArchive).push_back(((char*)&compilVer)[3]);
	}

	// We write the license version that wrote this file and the V-REP version:
	if (theArchive!=NULL)
	{
		WORD v=VREP_PROGRAM_VERSION_NB;
		(*theArchive) << ((char*)&v)[0];
		(*theArchive) << ((char*)&v)[1];

		DWORD licenseType=VREP_LICENSE_TYPE+1; // +1 because zero means: no info about license type yet!

		(*theArchive) << ((char*)&licenseType)[0];
		(*theArchive) << ((char*)&licenseType)[1];
		(*theArchive) << ((char*)&licenseType)[2];
		(*theArchive) << ((char*)&licenseType)[3];
	}
	else
	{
		WORD v=VREP_PROGRAM_VERSION_NB;
		(*_bufferArchive).push_back(((char*)&v)[0]);
		(*_bufferArchive).push_back(((char*)&v)[1]);

		DWORD licenseType=VREP_LICENSE_TYPE+1; // +1 because zero means: no info about license type yet!

		(*_bufferArchive).push_back(((char*)&licenseType)[0]);
		(*_bufferArchive).push_back(((char*)&licenseType)[1]);
		(*_bufferArchive).push_back(((char*)&licenseType)[2]);
		(*_bufferArchive).push_back(((char*)&licenseType)[3]);
	}

	// We write 1000-6 bytes for future use: (since ser ver 13 (2009/07/21)) (-6 since 2009/12/17)
	for (int i=0;i<994;i++)
	{
		if (theArchive!=NULL)
			(*theArchive) << (char)0;
		else
			(*_bufferArchive).push_back((char)0);
	}

	// Now we write all the data:
	if (compress)
	{ // compressed. When changing compression method, then serialization version has to be incremented and older version won't be able to read newer versions anymore!

		// Hufmann:
		unsigned char* writeBuff=new unsigned char[_fileBuffer.size()+400]; // actually 384
		int outSize=Huffman_Compress(&_fileBuffer[0],writeBuff,_fileBuffer.size());
		if (theArchive!=NULL)
		{
			for (int i=0;i<outSize;i++)
				(*theArchive) << writeBuff[i];
		}
		else
		{
			for (int i=0;i<outSize;i++)
				(*_bufferArchive).push_back(writeBuff[i]);
		}
		delete[] writeBuff;
	}
	else
	{ // no compression
		int l=int(_fileBuffer.size());
		if (theArchive!=NULL)
		{
			for (int i=0;i<l;i++)
				(*theArchive) << _fileBuffer[i];
		}
		else
		{
			for (int i=0;i<l;i++)
				(*_bufferArchive).push_back(_fileBuffer[i]);
		}
	}
	_fileBuffer.clear();
}

int CSer::readOpen(int& serializationVersion,WORD& vrepVersionThatWroteThis,int& licenseTypeThatWroteThis)
{ // return values: -3=wrong fileformat, -2=format too old, -1=format too new, 0=compressor unknown, 1=alright!
	vrepVersionThatWroteThis=0; // means: not yet supported
	licenseTypeThatWroteThis=-1; // means: not yet supported
	serializationVersion=-1; // error
	_serializationVersionThatWroteThisFile=serializationVersion;
	int minSerializationVersionThatCanReadThis=-1; // error

	int compilationVersion=1; // default for serialization versions before 13

	// We try to read the new header:
	if (theArchive!=NULL)
	{
		VFile* theFile=theArchive->getFile();
		if (theFile->getLength()<strlen(SER_VREP_HEADER))
			return(-3); // wrong fileformat
	}
	else
	{
		if ((*_bufferArchive).size()<strlen(SER_VREP_HEADER))
			return(-3); // wrong fileformat
	}
	char compressMethod=0;
	int originalDataSize=0;
	std::string head;
	int bufferArchivePointer=0;
	for (int i=0;i<int(strlen(SER_VREP_HEADER));i++)
	{
		char tmp;
		if (theArchive!=NULL)
			(*theArchive) >> tmp;
		else
			tmp=(*_bufferArchive)[bufferArchivePointer++];
		head+=tmp;
	}
	int alreadyReadDataCount;
	if (head!=SER_VREP_HEADER)
		return(-3); // we don't have the appropriate header! (wrong fileformat)
	else
	{ // We have the correct header!
		// We read the serialization version:
		if (theArchive!=NULL)
		{
			(*theArchive) >> ((char*)&serializationVersion)[0];
			(*theArchive) >> ((char*)&serializationVersion)[1];
			(*theArchive) >> ((char*)&serializationVersion)[2];
			(*theArchive) >> ((char*)&serializationVersion)[3];
			(*theArchive) >> ((char*)&minSerializationVersionThatCanReadThis)[0];
			(*theArchive) >> ((char*)&minSerializationVersionThatCanReadThis)[1];
			(*theArchive) >> ((char*)&minSerializationVersionThatCanReadThis)[2];
			(*theArchive) >> ((char*)&minSerializationVersionThatCanReadThis)[3];
		}
		else
		{
			((char*)&serializationVersion)[0]=(*_bufferArchive)[bufferArchivePointer++];
			((char*)&serializationVersion)[1]=(*_bufferArchive)[bufferArchivePointer++];
			((char*)&serializationVersion)[2]=(*_bufferArchive)[bufferArchivePointer++];
			((char*)&serializationVersion)[3]=(*_bufferArchive)[bufferArchivePointer++];
			((char*)&minSerializationVersionThatCanReadThis)[0]=(*_bufferArchive)[bufferArchivePointer++];
			((char*)&minSerializationVersionThatCanReadThis)[1]=(*_bufferArchive)[bufferArchivePointer++];
			((char*)&minSerializationVersionThatCanReadThis)[2]=(*_bufferArchive)[bufferArchivePointer++];
			((char*)&minSerializationVersionThatCanReadThis)[3]=(*_bufferArchive)[bufferArchivePointer++];
		}
		_serializationVersionThatWroteThisFile=serializationVersion;
		// We read the compression method:
		if (theArchive!=NULL)
			(*theArchive) >> compressMethod;
		else
			compressMethod=(*_bufferArchive)[bufferArchivePointer++];
		// We read the uncompressed data size:
		if (theArchive!=NULL)
		{
			(*theArchive) >> ((char*)&originalDataSize)[0];
			(*theArchive) >> ((char*)&originalDataSize)[1];
			(*theArchive) >> ((char*)&originalDataSize)[2];
			(*theArchive) >> ((char*)&originalDataSize)[3];
		}
		else
		{
			((char*)&originalDataSize)[0]=(*_bufferArchive)[bufferArchivePointer++];
			((char*)&originalDataSize)[1]=(*_bufferArchive)[bufferArchivePointer++];
			((char*)&originalDataSize)[2]=(*_bufferArchive)[bufferArchivePointer++];
			((char*)&originalDataSize)[3]=(*_bufferArchive)[bufferArchivePointer++];
		}

		alreadyReadDataCount=17; // this is for ser version 12, ser version 13 has additional 1004!! (added a bit further down)
		if (serializationVersion>12)
		{ // for serialization version 13 and above! (2009/07/21)
			if (theArchive!=NULL)
			{
				(*theArchive) >> ((char*)&compilationVersion)[0];
				(*theArchive) >> ((char*)&compilationVersion)[1];
				(*theArchive) >> ((char*)&compilationVersion)[2];
				(*theArchive) >> ((char*)&compilationVersion)[3];
			}
			else
			{
				((char*)&compilationVersion)[0]=(*_bufferArchive)[bufferArchivePointer++];
				((char*)&compilationVersion)[1]=(*_bufferArchive)[bufferArchivePointer++];
				((char*)&compilationVersion)[2]=(*_bufferArchive)[bufferArchivePointer++];
				((char*)&compilationVersion)[3]=(*_bufferArchive)[bufferArchivePointer++];
			}

			// We read the license version that wrote this file and the V-REP version:
			if (theArchive!=NULL)
			{
				(*theArchive) >> ((char*)&vrepVersionThatWroteThis)[0];
				(*theArchive) >> ((char*)&vrepVersionThatWroteThis)[1];
				DWORD licenseTypeThatWroteThisTmp;
				(*theArchive) >> ((char*)&licenseTypeThatWroteThisTmp)[0];
				(*theArchive) >> ((char*)&licenseTypeThatWroteThisTmp)[1];
				(*theArchive) >> ((char*)&licenseTypeThatWroteThisTmp)[2];
				(*theArchive) >> ((char*)&licenseTypeThatWroteThisTmp)[3];
				licenseTypeThatWroteThis=licenseTypeThatWroteThisTmp-1; // -1 because -1 means: no info about license type yet!
			}
			else
			{
				((char*)&vrepVersionThatWroteThis)[0]=(*_bufferArchive)[bufferArchivePointer++];
				((char*)&vrepVersionThatWroteThis)[1]=(*_bufferArchive)[bufferArchivePointer++];
				DWORD licenseTypeThatWroteThisTmp;
				((char*)&licenseTypeThatWroteThisTmp)[0]=(*_bufferArchive)[bufferArchivePointer++];
				((char*)&licenseTypeThatWroteThisTmp)[1]=(*_bufferArchive)[bufferArchivePointer++];
				((char*)&licenseTypeThatWroteThisTmp)[2]=(*_bufferArchive)[bufferArchivePointer++];
				((char*)&licenseTypeThatWroteThisTmp)[3]=(*_bufferArchive)[bufferArchivePointer++];
				licenseTypeThatWroteThis=licenseTypeThatWroteThisTmp-1; // -1 because -1 means: no info about license type yet!
			}
			char dummy;
			for (int i=0;i<994;i++) // from 1000 to 994 on 2009/12/17
			{
				if (theArchive!=NULL)
					(*theArchive) >> dummy; // not used for now
				else
					dummy=(*_bufferArchive)[bufferArchivePointer++];
			}
			alreadyReadDataCount+=1004;
		}
	}


	_vrepVersionThatWroteThis=vrepVersionThatWroteThis;
	_licenseTypeThatWroteThis=licenseTypeThatWroteThis;

	if (serializationVersion<SER_MIN_SERIALIZATION_VERSION_THAT_THIS_CAN_READ)
		return(-2); // This file is too old
	if (minSerializationVersionThatCanReadThis>SER_SERIALIZATION_VERSION)
		return(-1); // This file is too new
	if (serializationVersion>SER_SERIALIZATION_VERSION)
	{ // we might have problems reading this (even if it should be supported). Some functions might not be available.
		App::uiThread->messageBox_warning(App::mainWindow,strTranslate("Serialization"),strTranslate(IDS_READING_NEWER_SERIALIZATION_FILE_WARNING),VMessageBox::OKELI);
	}

	// We read the whole file:
	if (theArchive!=NULL)
	{
		unsigned long l=(unsigned long)theArchive->getFile()->getLength()-alreadyReadDataCount;
		char dummy;
		for (unsigned long i=0;i<l;i++)
		{
			(*theArchive) >> dummy;
			_fileBuffer.push_back(dummy);
		}
	}
	else
	{
		for (unsigned long i=bufferArchivePointer;i<(*_bufferArchive).size();i++)
			_fileBuffer.push_back((*_bufferArchive)[i]);
	}

	if (compressMethod!=0)
	{ // compressed
		if (compressMethod==1) // for now, only Huffman is supported
		{ // Huffman uncompression:
			unsigned char* uncompressedBuffer=new unsigned char[originalDataSize];
			Huffman_Uncompress(&_fileBuffer[0],uncompressedBuffer,_fileBuffer.size(),originalDataSize);
			_fileBuffer.clear();
			for (int i=0;i<originalDataSize;i++)
				_fileBuffer.push_back(uncompressedBuffer[i]);
			delete[] uncompressedBuffer;
			return(1); // everything went ok!
		}
	}
	else
		return(1); // everything went ok!
	return(0); // error, unknown compressor
}

void CSer::readClose()
{
	_fileBuffer.clear();
}

bool CSer::isStoring()
{
	if (theArchive!=NULL)
		return(theArchive->isStoring()!=0);
	else
		return(_bufferArchive->size()==0);
}

void CSer::disableCountingModeExceptForExceptions()
{
	_coutingModeDisabledExceptForExceptions=true;
}

void CSer::setCountingMode(bool force)
{ // force is false by default
	if ((!_coutingModeDisabledExceptForExceptions)||force)
	{
		countingMode++;
		if (countingMode==1)
			counter=0;
	}
	else
	{ // here we write a dummy value!!
		for (int i=0;i<int(sizeof(counter));i++)
			_fileBuffer.push_back(((char*)&counter)[i]);
	}
}

bool CSer::setWritingMode(bool force)
{ // force is false by default
	if ((!_coutingModeDisabledExceptForExceptions)||force)
	{
		countingMode--;
		if (countingMode==0)
		{
			for (int i=0;i<int(sizeof(counter));i++)
				_fileBuffer.push_back(((char*)&counter)[i]);
			counter=0;
			return(true);
		}
		else
		{
			counter+=sizeof(counter);
			return(false);
		}
	}
	else
		return(false);
}

int CSer::getCounter()
{
	return(counter);
}

void CSer::loadUnknownData()
{ // If return value is true, a warning message should be displayed
	int quantity;
	for (int i=0;i<int(sizeof(quantity));i++)
		((char*)&quantity)[i]=_fileBuffer[_fileBufferReadPointer++];
	_foundUnknownCommands=true;
	_fileBufferReadPointer+=quantity;
}

bool CSer::getFoundUnknownCommands()
{
	return(_foundUnknownCommands);
}

std::vector<unsigned char>* CSer::getBufferPointer()
{
	return(&buffer);
}

CSer& CSer::operator<< (const int& v)
{
	buffer.push_back(((unsigned char*)&v)[0]);
	buffer.push_back(((unsigned char*)&v)[1]);
	buffer.push_back(((unsigned char*)&v)[2]);
	buffer.push_back(((unsigned char*)&v)[3]);
	return(*this);
}

CSer& CSer::operator<< (const float& v)
{
	buffer.push_back(((unsigned char*)&v)[0]);
	buffer.push_back(((unsigned char*)&v)[1]);
	buffer.push_back(((unsigned char*)&v)[2]);
	buffer.push_back(((unsigned char*)&v)[3]);
	return(*this);
}

CSer& CSer::operator<< (const double& v)
{
	unsigned char* tmp=(unsigned char*)(&v);
	for (int i=0;i<int(sizeof(v));i++)
		buffer.push_back(tmp[i]);
	return(*this);
}

CSer& CSer::operator<< (const WORD& v)
{
	unsigned char* tmp=(unsigned char*)(&v);
	for (int i=0;i<int(sizeof(v));i++)
		buffer.push_back(tmp[i]);
	return(*this);
}

CSer& CSer::operator<< (const DWORD& v)
{
	unsigned char* tmp=(unsigned char*)(&v);
	for (int i=0;i<int(sizeof(v));i++)
		buffer.push_back(tmp[i]);
	return(*this);
}

CSer& CSer::operator<< (const long& v)
{
	unsigned char* tmp=(unsigned char*)(&v);
	for (int i=0;i<int(sizeof(v));i++)
		buffer.push_back(tmp[i]);
	return(*this);
}

CSer& CSer::operator<< (const unsigned char& v)
{
	buffer.push_back(v);
	return(*this);
}

CSer& CSer::operator<< (const char& v)
{
	buffer.push_back(v);
	return(*this);
}

CSer& CSer::operator<< (const std::string& v)
{
	(*this) << ((int)v.length());
	for (int i=0;i<int(v.length());i++)
		(*this) << ((unsigned char)v[i]);
	return(*this);
}

std::vector<unsigned char>* CSer::getFileBuffer()
{
	return(&_fileBuffer);
}

int CSer::getFileBufferReadPointer()
{
	return(_fileBufferReadPointer);
}

void CSer::addOffsetToFileBufferReadPointer(int off)
{
	_fileBufferReadPointer+=off;
}

CSer& CSer::operator>> (int& v)
{
	unsigned char* tmp=(unsigned char*)(&v);
	for (int i=0;i<int(sizeof(v));i++)
		tmp[i]=_fileBuffer[_fileBufferReadPointer++];
	return(*this);
}

CSer& CSer::operator>> (float& v)
{
	unsigned char* tmp=(unsigned char*)(&v);
	for (int i=0;i<int(sizeof(v));i++)
		tmp[i]=_fileBuffer[_fileBufferReadPointer++];
	return(*this);
}

CSer& CSer::operator>> (double& v)
{
	unsigned char* tmp=(unsigned char*)(&v);
	for (int i=0;i<int(sizeof(v));i++)
		tmp[i]=_fileBuffer[_fileBufferReadPointer++];
	return(*this);
}

CSer& CSer::operator>> (WORD& v)
{
	unsigned char* tmp=(unsigned char*)(&v);
	for (int i=0;i<int(sizeof(v));i++)
		tmp[i]=_fileBuffer[_fileBufferReadPointer++];
	return(*this);
}

CSer& CSer::operator>> (DWORD& v)
{
	unsigned char* tmp=(unsigned char*)(&v);
	for (int i=0;i<int(sizeof(v));i++)
		tmp[i]=_fileBuffer[_fileBufferReadPointer++];
	return(*this);
}

CSer& CSer::operator>> (long& v)
{
	unsigned char* tmp=(unsigned char*)(&v);
	for (int i=0;i<int(sizeof(v));i++)
		tmp[i]=_fileBuffer[_fileBufferReadPointer++];
	return(*this);
}

CSer& CSer::operator>> (unsigned char& v)
{
	v=_fileBuffer[_fileBufferReadPointer++];
	return(*this);
}

CSer& CSer::operator>> (char& v)
{
	v=_fileBuffer[_fileBufferReadPointer++];
	return(*this);
}

CSer& CSer::operator>> (std::string& v)
{
	v="";
	int l;
	for (int i=0;i<int(sizeof(l));i++)
		((char*)&l)[i]=_fileBuffer[_fileBufferReadPointer++];
	for (int i=0;i<l;i++)
		v+=_fileBuffer[_fileBufferReadPointer++];
	return(*this);
}

void CSer::flush(bool writeNbOfBytes)
{ // writeNbOfBytes is true by default
	if (countingMode==0)
	{
		if (writeNbOfBytes)
		{
			int s=(int)buffer.size();
			for (int i=0;i<int(sizeof(s));i++)
				_fileBuffer.push_back(((char*)&s)[i]);
		}
		for (int i=0;i<int(buffer.size());i++)
			_fileBuffer.push_back(buffer[i]);
		buffer.clear();
	}
	else
	{
		if (writeNbOfBytes)
			counter+=sizeof(int)+buffer.size();
		else
			counter+=buffer.size();
		buffer.clear();
	}
}

void CSer::storeDataName(const char* name)
{
	if (countingMode!=0)
		counter+=3;
	else
	{
		for (int i=0;i<3;i++)
			_fileBuffer.push_back(name[i]);
	}
}

std::string CSer::readDataName()
{
	std::string retS="";
	for (int i=0;i<3;i++)
		retS+=_fileBuffer[_fileBufferReadPointer++];
	return(retS);
}

int CSer::readBytesButKeepPointerUnchanged(BYTE* buffer,int desiredCount)
{
	int left=int(_fileBuffer.size())-_fileBufferReadPointer;
	if (left<desiredCount)
		desiredCount=left; // we can't read as much as we want!
	for (int i=0;i<desiredCount;i++)
		buffer[i]=_fileBuffer[_fileBufferReadPointer+i];
	return(desiredCount); // we return the nb of bytes we could read
}

VArchive& CSer::getArchive()
{
	return(*theArchive);
}

WORD CSer::getVrepVersionThatWroteThisFile()
{
	return(_vrepVersionThatWroteThis);
}

int CSer::getLicenseTypeThatWroteThisFile()
{
	return(_licenseTypeThatWroteThis);
}

int CSer::getSerializationVersionThatWroteThisFile()
{
	return(_serializationVersionThatWroteThisFile);
}
