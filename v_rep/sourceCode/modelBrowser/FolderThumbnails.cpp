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
#include "FolderThumbnails.h"

#include "IloIlo.h"
#include "FileMenu.h"
#include "imgLoader.h"
#include "VFileFinder.h"
#include "App.h"

CFolderThumbnails::CFolderThumbnails()
{
}

CFolderThumbnails::~CFolderThumbnails()
{
	clearAll();
}

void CFolderThumbnails::clearAll()
{
	for (int i=0;i<int(_allThumbnails.size());i++)
		delete _allThumbnails[i];
	_allThumbnails.clear();

	_folderPath="";
	_allNamesWithoutExtension.clear();
	_allCreationTimes.clear();
	_allValidFileformats.clear();
	_allModelOrFolder.clear();
}

CThumbnail* CFolderThumbnails::getThumbnailFromIndex(int index)
{
	if ( (index<0)||(index>=int(_allThumbnails.size())) )
		return(NULL);
	return(_allThumbnails[index]);
}

std::string CFolderThumbnails::getThumbnailNameFromIndex(int index)
{
	if ( (index<0)||(index>=int(_allThumbnails.size())) )
		return("");
	return(_allNamesWithoutExtension[index]);
}

std::string CFolderThumbnails::getThumbnailNameAndPathFromIndex(int index)
{
	std::string retVal;
	if ( (index>=0)&&(index<int(_allThumbnails.size())) )
	{
		retVal=_folderPath;
		retVal+=VREP_SLASH;
		retVal+=_allNamesWithoutExtension[index];
		retVal+=".ttm";
	}
	return(retVal);
}

bool CFolderThumbnails::getValidFileformatFromIndex(int index)
{
	if ( (index<0)||(index>=int(_allThumbnails.size())) )
		return(false);
	return(_allValidFileformats[index]!=0);
}

bool CFolderThumbnails::getIsFolderFromIndex(int index)
{
	if ( (index<0)||(index>=int(_allThumbnails.size())) )
		return(false);
	return(_allModelOrFolder[index]==0);
}

int CFolderThumbnails::getThumbnailCount()
{
	return(int(_allThumbnails.size()));
}

void CFolderThumbnails::addThumbnail(CThumbnail* thumbN,const char* nameWithoutExtension,DWORD creationTime,BYTE modelOrFolder,bool validFileformat)
{
	_allThumbnails.push_back(thumbN);
	_allNamesWithoutExtension.push_back(nameWithoutExtension);
	_allCreationTimes.push_back(creationTime);
	_allModelOrFolder.push_back(modelOrFolder);
	_allValidFileformats.push_back(validFileformat);
}

std::string CFolderThumbnails::getPathAndModelName(int thumbnailIndex)
{
	std::string retVal;
	if (_folderPath!="")
		retVal=getThumbnailNameAndPathFromIndex(thumbnailIndex);
	return(retVal);
}

void CFolderThumbnails::prepareFolderThumbnails(const char* folderPath)
{
	clearAll();
	std::vector<int> initialSelection;
	App::ct->objCont->getSelectedObjects(initialSelection);
	if (folderPath!=NULL)
	{
		_folderPath=folderPath;
		// 1. Get all files and their info from the folder:
		std::vector<std::string> allModelNames;
		std::vector<DWORD> allModelCreationTimes;
		std::vector<BYTE> allModelOrFolder; // 1=model, 0=folder

		VFileFinder finder;
		finder.searchFilesOrFolders(_folderPath);
		int index=0;
		SFileOrFolder* foundItem=finder.getFoundItem(index++);
		while (foundItem!=NULL)
		{
			if (foundItem->isFile)
			{ // Files
				std::string filename(foundItem->name);
				int l=filename.length();
				if ((l>=5)&&(filename[l-4]=='.')&&(filename[l-3]=='t')&&(filename[l-2]=='t')&&(filename[l-1]=='m'))
				{ // above condition is somehow needed, otherwise file "*.ttmt" or similar are also retrieved!
					filename.erase(filename.end()-4,filename.end()-0);
					allModelNames.push_back(filename);
					allModelCreationTimes.push_back(DWORD(foundItem->lastWriteTime));
					allModelOrFolder.push_back(1); // this is a model
				}
			}
			else
			{ // Added on 28/05/2011 to accomodate for folder thumbnail display
				std::string filename(foundItem->name);
				if ( (filename!=".")&&(filename!="..") )
				{ // We don't wanna the . and .. folders
					allModelNames.push_back(filename);
					allModelCreationTimes.push_back(DWORD(foundItem->lastWriteTime));
					allModelOrFolder.push_back(0); // this is a folder
				}
			}
			foundItem=finder.getFoundItem(index++);
		}
		// 2. Check if a thumbnail file exists:
		clearAll();
		_folderPath=folderPath;
		std::string thmbFile(_folderPath);
		thmbFile+=VREP_SLASH;
		thmbFile+="v_repModelThumbnails.ttmt";
		bool thumbnailFileExistsAndWasLoaded=false;
		if (VFile::doesFileExist(thmbFile))
		{
			VFile file(thmbFile.c_str(),VFile::READ|VFile::SHARE_DENY_NONE);
			VArchive archive(&file,VArchive::LOAD);
			CSer serObj(archive);
			int serializationVersion;
			WORD vrepVersionThatWroteThis;
			int licenseTypeThatWroteThis;
			int result=serObj.readOpen(serializationVersion,vrepVersionThatWroteThis,licenseTypeThatWroteThis);
			if (result==1)
			{
				thumbnailFileExistsAndWasLoaded=true;
				serializePart1(serObj);
				// a. do we have the same directory as written in the file?
				if (_folderPath.compare(folderPath)!=0)
					thumbnailFileExistsAndWasLoaded=false;
				// b. do we have the same number of files?
				if (_allNamesWithoutExtension.size()!=allModelNames.size())
					thumbnailFileExistsAndWasLoaded=false;
				else
				{ // we have the same number of files. Check if the names and last write times are same:
					bool same=true;
					for (int i=0;i<int(allModelNames.size());i++)
					{
						if (allModelNames[i].compare(_allNamesWithoutExtension[i])!=0)
						{
							same=false;
							break;
						}
						if (allModelCreationTimes[i]!=_allCreationTimes[i])
						{
							same=false;
							break;
						}
						if (allModelOrFolder[i]!=_allModelOrFolder[i])
						{ // Check also if both are models or folders!
							same=false;
							break;
						}
					}
					if (same)
						serializePart2(serObj);
					else
						thumbnailFileExistsAndWasLoaded=false;
				}
				serObj.readClose();
				archive.close();
				file.close();
			}
		}
		// 3. Now load all thumbnails freshly (if needed):
		if ( (!thumbnailFileExistsAndWasLoaded)&&(allModelNames.size()!=0) )
		{
			clearAll();
			_folderPath=folderPath;
			for (int i=0;i<int(allModelNames.size());i++)
			{
				if (allModelOrFolder[i]==1)
				{ // we have a model here
					std::string nameAndPath(_folderPath);
					nameAndPath+=VREP_SLASH;
					nameAndPath+=allModelNames[i];
					nameAndPath+=".ttm";
					int result;
					CThumbnail* thumbnail=loadModelThumbnail(nameAndPath.c_str(),result);
					addThumbnail(thumbnail,allModelNames[i].c_str(),allModelCreationTimes[i],1,result>=0);
				}
				else
				{ // we have a folder here!
					int xres,yres;
					bool rgba;
					unsigned char* thumbnail=CImageLoader::loadQTgaImageData(":/targaFiles/128x128folder.tga",xres,yres,rgba,NULL);
					CThumbnail* foldThumb=new CThumbnail();
					foldThumb->setUncompressedThumbnailImage((char*)thumbnail,true,false);
					delete[] thumbnail;
					addThumbnail(foldThumb,allModelNames[i].c_str(),allModelCreationTimes[i],0,true);
				}
			}
			// 4. Serialize the thumbnail file for fast access in future:
			std::string thmbFile(_folderPath);
			thmbFile+=VREP_SLASH;
			thmbFile+="v_repModelThumbnails.ttmt";
			VFile myFile(thmbFile.c_str(),VFile::CREATE|VFile::WRITE|VFile::SHARE_EXCLUSIVE);
			myFile.setLength(0);
			VArchive archive(&myFile,VArchive::STORE);
			CSer serObj(archive);
			serObj.writeOpen();
			serializePart1(serObj);
			serializePart2(serObj);
			serObj.writeClose(App::userSettings->compressFiles);
			archive.close();
			myFile.close();
		}
	}
	// Now restore previous object selection state:
	App::ct->objCont->deselectObjects();
	for (int i=0;i<int(initialSelection.size());i++)
		App::ct->objCont->addObjectToSelection(initialSelection[i]);
}


CThumbnail* CFolderThumbnails::loadModelThumbnail(const char* pathAndFilename,int& result)
{ // result: -1=model not recognized, 0=model has no thumbnail, 1=model has thumbnail
	result=-1;
	CThumbnail* retThumbnail=NULL;
	if (VFile::doesFileExist(pathAndFilename))
	{
		VFile file(pathAndFilename,VFile::READ|VFile::SHARE_DENY_NONE);
		VArchive archive(&file,VArchive::LOAD);
		CSer serObj(archive);

		int serializationVersion;
		WORD vrepVersionThatWroteThis;
		int licenseTypeThatWroteThis;
		result=serObj.readOpen(serializationVersion,vrepVersionThatWroteThis,licenseTypeThatWroteThis);
		if (result==1)
		{
			result=0;

			CThumbnail* thumbO=App::ct->environment->modelThumbnail_notSerializedHere.copyYourself();
			App::ct->objCont->loadModel(serObj,true);
			retThumbnail=App::ct->environment->modelThumbnail_notSerializedHere.copyYourself();
			App::ct->environment->modelThumbnail_notSerializedHere.copyFrom(thumbO);
			delete thumbO;
			if (retThumbnail->getPointerToUncompressedImage()!=NULL)
				result=1;
			serObj.readClose();
		}
		archive.close();
		file.close();
	}
	return(retThumbnail);
}


void CFolderThumbnails::serializePart1(CSer& ar)
{
	if (ar.isStoring())
	{ // Storing
		ar.storeDataName("Tfp");
		ar << _folderPath;
		ar.flush();

		ar.storeDataName("Tn2");
		ar << int(_allNamesWithoutExtension.size());
		for (int i=0;i<int(_allNamesWithoutExtension.size());i++)
		{
			ar << _allNamesWithoutExtension[i];
			ar << _allCreationTimes[i];
			ar << _allModelOrFolder[i];
			ar << _allValidFileformats[i];
		}
		ar.flush();

		ar.storeDataName(SER_END_OF_OBJECT);
	}
	else
	{		// Loading
		int byteQuantity;
		std::string theName="";
		while (theName.compare(SER_END_OF_OBJECT)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_END_OF_OBJECT)!=0)
			{
				bool noHit=true;
				if (theName.compare("Tfp")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _folderPath;
				}
				if (theName.compare("Tnd")==0)
				{ // For backward compatibility (28/05/2011)
					noHit=false;
					ar >> byteQuantity;
					int thmbCnt;
					ar >> thmbCnt;
					for (int i=0;i<thmbCnt;i++)
					{
						std::string dum;
						ar >> dum;
						_allNamesWithoutExtension.push_back(dum);
						DWORD dum2;
						ar >> dum2;
						_allCreationTimes.push_back(dum2);
						_allModelOrFolder.push_back(1); // 1=model
						BYTE dum3;
						ar >> dum3;
						_allValidFileformats.push_back(dum3);
					}
				}
				if (theName.compare("Tn2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					int thmbCnt;
					ar >> thmbCnt;
					for (int i=0;i<thmbCnt;i++)
					{
						std::string dum;
						ar >> dum;
						_allNamesWithoutExtension.push_back(dum);
						DWORD dum2;
						ar >> dum2;
						_allCreationTimes.push_back(dum2);
						BYTE dum3;
						ar >> dum3;
						_allModelOrFolder.push_back(dum3);
						ar >> dum3;
						_allValidFileformats.push_back(dum3);
					}
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}

void CFolderThumbnails::serializePart2(CSer& ar)
{
	if (ar.isStoring())
	{ // Storing
		for (int i=0;i<int(_allThumbnails.size());i++)
		{
			ar.storeDataName("Tc2");
			ar.setCountingMode();
			_allThumbnails[i]->serialize(ar);
			if (ar.setWritingMode())
				_allThumbnails[i]->serialize(ar);
		}

		ar.storeDataName(SER_END_OF_OBJECT);
	}
	else
	{		// Loading
		int byteQuantity;
		std::string theName="";
		char* compressedImage=new char[128*64*3];
		while (theName.compare(SER_END_OF_OBJECT)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_END_OF_OBJECT)!=0)
			{
				bool noHit=true;
				if (theName.compare("Tci")==0)
				{ // for backward compatibility! (serialization version 15 sometime!)
					noHit=false;
					ar >> byteQuantity;
					BYTE imagePresent;
					ar >> imagePresent;
					char dum;
					CThumbnail* it=new CThumbnail();
					if (imagePresent!=0)
					{
						for (int j=0;j<128*64*3;j++)
						{
							ar >> dum;
							compressedImage[j]=dum;
						}
						it->setCompressedThumbnailImage(compressedImage);
						_allThumbnails.push_back(it);
					}
					else
						_allThumbnails.push_back(it);
				}
				if (theName.compare("Tc2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					CThumbnail* it=new CThumbnail();
					it->serialize(ar);
					_allThumbnails.push_back(it);
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
		delete[] compressedImage;
	}
}
