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
#include "Thumbnail.h"
#include "OGL.h"
#include "Global.h"

CThumbnail::CThumbnail()
{
	_thumbnailRGBAImage=NULL;
	_compressData=false;
}

CThumbnail::~CThumbnail()
{
	delete[] _thumbnailRGBAImage;
}

void CThumbnail::display(int posX,int posY,const std::string& name)
{
	int borderWidth=1;
	int textSpaceHeight=MODEL_BROWSER_TEXT_SPACE_HEIGHT;
	glDisable(GL_LIGHTING);
	glColor3f(ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_BRIGHT[0],ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_BRIGHT[1],ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_BRIGHT[2]);
	glBegin(GL_QUADS);
	glVertex3i(posX+borderWidth,posY-borderWidth,0);
	glVertex3i(posX+borderWidth+128,posY-borderWidth,0);
	glVertex3i(posX+borderWidth+128,posY-borderWidth-128-textSpaceHeight,0);
	glVertex3i(posX+borderWidth,posY-borderWidth-128-textSpaceHeight,0);
	glEnd();
	if (_thumbnailRGBAImage==NULL)
	{
		glColor3f(0.5f,0.0f,0.0f);
		int textWidth=ogl::getTextLengthInPixels("Thumbnail");
		ogl::drawText(posX+borderWidth+64-textWidth/2,posY-borderWidth-64+ogl::getFontHeight()/2,0,"Thumbnail",true);
		textWidth=ogl::getTextLengthInPixels("not available");
		ogl::drawText(posX+borderWidth+64-textWidth/2,posY-borderWidth-64-ogl::getFontHeight()/2,0,"not available",true);
	}
	else
	{
		ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // We turn blending on!
		unsigned int textureName;
		glGenTextures(1,&textureName);
		glBindTexture(GL_TEXTURE_2D,textureName);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,128,128,0,GL_RGBA,GL_UNSIGNED_BYTE,_thumbnailRGBAImage);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP); // GL_REPEAT
		glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP); // GL_REPEAT
		glTexEnvf (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
		glColor4f(ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_BRIGHT[0],ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_BRIGHT[1],ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_BRIGHT[2], 1.0f);//0.125f); // fading

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f,1.0f);
		glVertex3i(posX+borderWidth,posY-borderWidth,0);
		glTexCoord2f(1.0f,1.0f);
		glVertex3i(posX+borderWidth+128,posY-borderWidth,0);
		glTexCoord2f(1.0f,0.0f);
		glVertex3i(posX+borderWidth+128,posY-borderWidth-128,0);
		glTexCoord2f(0.0f,0.0f);
		glVertex3i(posX+borderWidth,posY-borderWidth-128,0);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		ogl::setBlending(false);
		glDisable(GL_ALPHA_TEST);
		glDeleteTextures(1,&textureName);
	}
	glColor3f(0.5f,0.5f,0.5f);

	std::vector<std::string> separateWords;
	std::vector<int> wordWidths;
	ogl::getSeparateWords(name,separateWords);
	ogl::getSeparateWordWidths(separateWords,wordWidths);

	int spaceWidth=ogl::getSeparateWordWidths(separateWords,wordWidths);
	int width;
	std::string line=ogl::getTextThatFitIntoPixelWidth(separateWords,wordWidths,spaceWidth,128,width,false);
	if (separateWords.size()==0)
	{ // simple, the whole text fits onto one line!
		ogl::drawText(posX+borderWidth+64-width/2,posY-borderWidth-128-textSpaceHeight/2,0,line,true);
	}
	else
	{ // trickier, the text doesn't fit onto one line!
		ogl::drawText(posX+borderWidth+64-width/2,posY-borderWidth-128-textSpaceHeight/4,0,line,true);
		int dotDotWidth=ogl::getTextLengthInPixels("..");
		line=ogl::getTextThatFitIntoPixelWidth(separateWords,wordWidths,spaceWidth,128-dotDotWidth,width,true);
		if (separateWords.size()==0)
		{ // All the text fits
			ogl::drawText(posX+borderWidth+64-width/2,posY-borderWidth-128-3*textSpaceHeight/4,0,line,true);
		}
		else
		{ // Not all the text fits
			width+=dotDotWidth;
			line+="..";
			ogl::drawText(posX+borderWidth+64-width/2,posY-borderWidth-128-3*textSpaceHeight/4,0,line,true);
		}
	}
	glEnable(GL_LIGHTING);
}

void CThumbnail::compressThumbnail(const char* uncompressedRGBAImage,char* compressedRGBImage)
{
	for (int i=0;i<128*64;i++)
	{
		unsigned char r0=uncompressedRGBAImage[8*i+0]/16;
		unsigned char g0=uncompressedRGBAImage[8*i+1]/16;
		unsigned char b0=uncompressedRGBAImage[8*i+2]/16;
		unsigned char a0=uncompressedRGBAImage[8*i+3];
		if (a0==0)
		{
			r0=0;
			g0=15;
			b0=0;
		}
		unsigned char r1=uncompressedRGBAImage[8*i+4]/16;
		unsigned char g1=uncompressedRGBAImage[8*i+5]/16;
		unsigned char b1=uncompressedRGBAImage[8*i+6]/16;
		unsigned char a1=uncompressedRGBAImage[8*i+7];
		if (a1==0)
		{
			r1=0;
			g1=15;
			b1=0;
		}
		unsigned char byte1=(r0&0x0f)|(16*(g0&0x0f));
		unsigned char byte2=(b0&0x0f)|(16*(r1&0x0f));
		unsigned char byte3=(g1&0x0f)|(16*(b1&0x0f));
		compressedRGBImage[3*i+0]=byte1;
		compressedRGBImage[3*i+1]=byte2;
		compressedRGBImage[3*i+2]=byte3;
	}
}

void CThumbnail::uncompressThumbnail(const char* compressedRGBImage,char* uncompressedRGBAImage)
{
	for (int i=0;i<128*64;i++)
	{
		unsigned char byte1=compressedRGBImage[3*i+0];
		unsigned char byte2=compressedRGBImage[3*i+1];
		unsigned char byte3=compressedRGBImage[3*i+2];
		unsigned char r0=((byte1&0x0f)*16);
		unsigned char g0=(byte1&0xf0);
		unsigned char b0=((byte2&0x0f)*16);
		unsigned char r1=(byte2&0xf0);
		unsigned char g1=((byte3&0x0f)*16);
		unsigned char b1=(byte3&0xf0);
		unsigned char a0=255;
		unsigned char a1=255;
		if ((r0==0)&&(g0==240)&&(b0==0))
			a0=0;
		if ((r1==0)&&(g1==240)&&(b1==0))
			a1=0;
		uncompressedRGBAImage[8*i+0]=r0;
		uncompressedRGBAImage[8*i+1]=g0;
		uncompressedRGBAImage[8*i+2]=b0;
		uncompressedRGBAImage[8*i+3]=a0;
		uncompressedRGBAImage[8*i+4]=r1;
		uncompressedRGBAImage[8*i+5]=g1;
		uncompressedRGBAImage[8*i+6]=b1;
		uncompressedRGBAImage[8*i+7]=a1;
	}
}

bool CThumbnail::getCompressedImage(char* compressedRGBImage)
{
	if (_thumbnailRGBAImage==NULL)
		return(false);
	compressThumbnail(_thumbnailRGBAImage,compressedRGBImage);
	return(true);
}

char* CThumbnail::getPointerToUncompressedImage()
{
	return(_thumbnailRGBAImage);
}

void CThumbnail::setUncompressedThumbnailImage(const char* uncompressedRGBAImage,bool rgba,bool verticalFlip)
{
	setRandomImage();
	if (verticalFlip)
	{
		for (int i=0;i<128;i++)
		{
			for (int j=0;j<128;j++)
			{
				if (rgba)
				{
					for (int k=0;k<4;k++)
						_thumbnailRGBAImage[4*(i*128+j)+k]=uncompressedRGBAImage[4*((127-i)*128+j)+k];
				}
				else
				{
					for (int k=0;k<3;k++)
						_thumbnailRGBAImage[4*(i*128+j)+k]=uncompressedRGBAImage[3*((127-i)*128+j)+k];
					_thumbnailRGBAImage[4*(i*128+j)+3]=255;
				}
			}
		}
	}
	else
	{
		if (rgba)
		{
			for (int i=0;i<128*128*4;i++)
				_thumbnailRGBAImage[i]=uncompressedRGBAImage[i];
		}
		else
		{
			for (int i=0;i<128*128;i++)
			{
				_thumbnailRGBAImage[4*i+0]=uncompressedRGBAImage[3*i+0];
				_thumbnailRGBAImage[4*i+1]=uncompressedRGBAImage[3*i+1];
				_thumbnailRGBAImage[4*i+2]=uncompressedRGBAImage[3*i+2];
				_thumbnailRGBAImage[4*i+3]=255;
			}
		}
	}
	_compressData=false;
}

void CThumbnail::setUncompressedThumbnailImageFromFloat(const float* uncompressedRGBAImage)
{
	setRandomImage();
	for (int i=0;i<128*128*4;i++)
		_thumbnailRGBAImage[i]=BYTE(uncompressedRGBAImage[i]*255.1f);
	_compressData=false;
}

void CThumbnail::setRandomImage()
{
	if (_thumbnailRGBAImage==NULL)
		_thumbnailRGBAImage=new char[128*128*4];
}

void CThumbnail::setCompressedThumbnailImage(const char* compressedRGBImage)
{
	setRandomImage();
	uncompressThumbnail(compressedRGBImage,_thumbnailRGBAImage);
	_compressData=true;
}

void CThumbnail::clearThumbnailImage()
{
	delete[] _thumbnailRGBAImage;
	_thumbnailRGBAImage=NULL;
	_compressData=false;
}

bool CThumbnail::copyUncompressedImageToBuffer(char* buffer)
{
	if (_thumbnailRGBAImage==NULL)
		return(false);
	for (int i=0;i<128*128*4;i++)
		buffer[i]=_thumbnailRGBAImage[i];
	return(true);
}

bool CThumbnail::hasImage()
{
	return(_thumbnailRGBAImage!=NULL);
}

CThumbnail* CThumbnail::copyYourself()
{
	CThumbnail* newT=new CThumbnail();
	if (_thumbnailRGBAImage!=NULL)
		newT->setUncompressedThumbnailImage(_thumbnailRGBAImage,true,false);
	newT->_compressData=_compressData;
	return(newT);
}

void CThumbnail::copyFrom(CThumbnail* it)
{
	if (it->_thumbnailRGBAImage==NULL)
	{
		delete[] _thumbnailRGBAImage;
		_thumbnailRGBAImage=NULL;
	}
	else
	{
		if (_thumbnailRGBAImage==NULL)
			_thumbnailRGBAImage=new char[128*128*4];
		setUncompressedThumbnailImage(it->_thumbnailRGBAImage,true,false);
	}
	_compressData=it->_compressData;
}

void CThumbnail::serialize(CSer& ar,bool forceCompressedSaving/*=false*/)
{
	if (ar.isStoring())
	{ // Storing
		ar.storeDataName("Tm2");
		if (_thumbnailRGBAImage==NULL)
			ar << BYTE(0);
		else
		{
			ar << BYTE(1);
			if (forceCompressedSaving||_compressData)
			{
				ar << BYTE(1);
				char* compressedImage=new char[128*64*3];
				getCompressedImage(compressedImage);
				for (int j=0;j<128*64*3;j++)
					ar << compressedImage[j];
				delete[] compressedImage;
			}
			else
			{
				ar << BYTE(0);
				for (int j=0;j<128*128*4;j++)
					ar << _thumbnailRGBAImage[j];
			}
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
//***************************************************************
				if (theName.compare("Tmb")==0)
				{ // for backward compatibility (7/3/2012)
					noHit=false;
					ar >> byteQuantity;
					BYTE imagePresent;
					ar >> imagePresent;
					char dum;
					if (imagePresent!=0)
					{
						_thumbnailRGBAImage=new char[128*128*4];
						BYTE compressed;
						ar >> compressed;
						_compressData=(compressed!=0);
						if (_compressData)
						{
							char* compressedImage=new char[128*64*3];
							for (int j=0;j<128*64*3;j++)
							{
								ar >> dum;
								compressedImage[j]=dum;
							}
							uncompressThumbnail(compressedImage,_thumbnailRGBAImage);
							delete[] compressedImage;
						}
						else
						{
							for (int j=0;j<128*128*4;j++)
							{
								ar >> dum;
								_thumbnailRGBAImage[j]=dum;
							}
						}
						for (int j=0;j<128*128;j++)
							_thumbnailRGBAImage[4*j+3]=char(float(BYTE(_thumbnailRGBAImage[4*j+3]))/0.22508f);
					}
				}
//***************************************************************
				if (theName.compare("Tm2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE imagePresent;
					ar >> imagePresent;
					char dum;
					if (imagePresent!=0)
					{
						if (_thumbnailRGBAImage==NULL)
							_thumbnailRGBAImage=new char[128*128*4];
						BYTE compressed;
						ar >> compressed;
						_compressData=(compressed!=0);
						if (_compressData)
						{
							char* compressedImage=new char[128*64*3];
							for (int j=0;j<128*64*3;j++)
							{
								ar >> dum;
								compressedImage[j]=dum;
							}
							uncompressThumbnail(compressedImage,_thumbnailRGBAImage);
							delete[] compressedImage;
						}
						else
						{
							for (int j=0;j<128*128*4;j++)
							{
								ar >> dum;
								_thumbnailRGBAImage[j]=dum;
							}
						}
					}
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}
