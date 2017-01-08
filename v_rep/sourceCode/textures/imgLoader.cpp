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
#include "imgLoader.h"
#include "TGAFormat.h"
#include "stb_image.h"
#include "ttUtil.h"
#include "VVarious.h"
#include <QImage>

unsigned char* CImageLoader::load(const char* filename,int* resX,int* resY,int* colorComponents,int desiredColorComponents,int scaleTo)
{ // scaleTo is 0 by default (no scaling). ScaleTo should be a power of 2!
	std::string ext(CTTUtil::getLowerCaseString(VVarious::splitPath_fileExtension(filename).c_str()));
	unsigned char* data=NULL;
	if ((ext.compare("tga")==0)||(ext.compare("gif")==0))
	{
		data=stbi_load(filename,resX,resY,colorComponents,desiredColorComponents);
	}
	else
	{
		QImage image;
		if (image.load(filename))
		{
			if (image.hasAlphaChannel())
				colorComponents[0]=4;
			else
				colorComponents[0]=3;
			resX[0]=image.width();
			resY[0]=image.height();
			data=new unsigned char[resX[0]*resY[0]*colorComponents[0]];
			for (int j=0;j<resY[0];j++)
			{
				for (int i=0;i<resX[0];i++)
				{
					QRgb pixel=image.pixel(i,j);
					if (colorComponents[0]==3)
					{
						data[3*(j*resX[0]+i)+0]=(unsigned char)qRed(pixel);
						data[3*(j*resX[0]+i)+1]=(unsigned char)qGreen(pixel);
						data[3*(j*resX[0]+i)+2]=(unsigned char)qBlue(pixel);
					}
					else
					{
						data[4*(j*resX[0]+i)+0]=(unsigned char)qRed(pixel);
						data[4*(j*resX[0]+i)+1]=(unsigned char)qGreen(pixel);
						data[4*(j*resX[0]+i)+2]=(unsigned char)qBlue(pixel);
						data[4*(j*resX[0]+i)+3]=(unsigned char)qAlpha(pixel);
					}
				}
			}
		}
	}
	if ((scaleTo!=0)&&(data!=NULL))
	{
		unsigned char* img=data;
		int s[2];
		s[0]=resX[0];
		s[1]=resY[0];
		int ns[2];
		ns[0]=resX[0];
		ns[1]=resY[0];
		for (int i=0;i<2;i++)
		{ // set the side size to a power of 2 and smaller or equal to 'scaleTo':
			int v=s[i];
			v&=(32768-1);
			WORD tmp=32768;
			while (tmp!=1)
			{
				if (v&tmp)
				{
					v=tmp;
					break;
				}
				tmp/=2;
			}
			if (v!=s[i])
				v=v*2;
			while (v>scaleTo)
				v/=2;
			ns[i]=v;
		}
		if ((ns[0]!=s[0])||(ns[1]!=s[1]))
		{
			data=getScaledImage(img,colorComponents[0],s[0],s[1],ns[0],ns[1]);
			delete[] img;
			resX[0]=ns[0];
			resY[0]=ns[1];
		}
	}
	return(data);
}

unsigned char* CImageLoader::loadQTgaImageData(const char* fileAndPath,int& resX,int& resY,bool& rgba,BYTE invisibleColor[3],int bitsPerPixel[1])
{
	unsigned char* data=CTGAFormat::getQ_ImageData(fileAndPath,resX,resY,rgba,invisibleColor,bitsPerPixel);
	return(data);
}

unsigned char* CImageLoader::getScaledImage(const unsigned char* originalImg,int colorComponents,int originalX,int originalY,int newX,int newY)
{
	QImage::Format f=QImage::Format_RGB888;
	unsigned char* im=new unsigned char[originalX*originalY*colorComponents];
	if (colorComponents==4)
	{
		f=QImage::Format_ARGB32;
		for (int i=0;i<originalX*originalY;i++)
		{ // from rgba to argb
			im[4*i+0]=originalImg[4*i+3];
			im[4*i+1]=originalImg[4*i+0];
			im[4*i+2]=originalImg[4*i+1];
			im[4*i+3]=originalImg[4*i+2];
		}
	}
	else
	{
		for (int i=0;i<originalX*originalY;i++)
		{
			im[3*i+0]=originalImg[3*i+0];
			im[3*i+1]=originalImg[3*i+1];
			im[3*i+2]=originalImg[3*i+2];
		}
	}
	QImage image(im,originalX,originalY,originalX*colorComponents,f);
	unsigned char* nim=new unsigned char[newX*newY*colorComponents];
	QImage nimage(image.scaled(newX,newY,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

	for (int j=0;j<newY;j++)
	{
		for (int i=0;i<newX;i++)
		{
			QRgb pixel=nimage.pixel(i,j);
			if (colorComponents==3)
			{
				nim[3*(j*newX+i)+0]=(unsigned char)qRed(pixel);
				nim[3*(j*newX+i)+1]=(unsigned char)qGreen(pixel);
				nim[3*(j*newX+i)+2]=(unsigned char)qBlue(pixel);
			}
			else
			{
				nim[4*(j*newX+i)+0]=(unsigned char)qRed(pixel);
				nim[4*(j*newX+i)+1]=(unsigned char)qGreen(pixel);
				nim[4*(j*newX+i)+2]=(unsigned char)qBlue(pixel);
				nim[4*(j*newX+i)+3]=(unsigned char)qAlpha(pixel);
			}
		}
	}
	delete[] im;
	return(nim);
}

