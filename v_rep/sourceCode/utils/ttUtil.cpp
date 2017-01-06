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
#include "ttUtil.h"
#include <boost/lexical_cast.hpp>

void CTTUtil::lightBinaryEncode(char* data,int length)
{ // Very simple!
	for (int i=0;i<length;i++)
		data[i]=(((data[i]&0xf0)>>4)|((data[i]&0x0f)<<4))^(i&0xff);
}

void CTTUtil::lightBinaryDecode(char* data,int length)
{ // very simple!
	for (int i=0;i<length;i++)
	{
		char tmp=data[i]^(i&0xff);
		data[i]=(((tmp&0xf0)>>4)|((tmp&0x0f)<<4));
	}
}

WORD CTTUtil::getCRC(char* data,int length)
{
	WORD crc=0;
	int p=0;
	for (int i=0;i<length;i++)
	{
		crc=crc^(((WORD)data[p])<<8);
		for (int j=0;j<8;j++)
		{
			if (crc&WORD(0x8000))
				crc=(crc<<1)^WORD(0x1021);
			else
				crc<<=1;
		}
		p++;
	}
	return(crc);
}

WORD CTTUtil::getCRC(const std::string& data)
{
	if (data.length()==0)
		return(0);
	return(getCRC((char*)(&data[0]),data.length()));
}

bool CTTUtil::extractCommaSeparatedWord(std::string& line,std::string& word)
{ // Returns true if a word could be extracted
	word="";
	while ( (line.length()!=0)&&(line[0]!=',') )
	{
		word.append(line.begin(),line.begin()+1);
		line.erase(line.begin());
	}
	if ( (line.length()!=0)&&(line[0]==',') )
		line.erase(line.begin());
	return(word.length()!=0);
}

std::string CTTUtil::intToString(int intVal)
{
	return(boost::lexical_cast<std::string>(intVal));
}

std::string CTTUtil::dwordToString(DWORD dwordVal)
{
	return(boost::lexical_cast<std::string>(dwordVal));
}

std::string CTTUtil::dataToString(char* data,int startPos,int length)
{
	std::string retVal("");
	for (int i=0;i<length;i++)
		retVal+=data[startPos+i];
	return(retVal);
}

void CTTUtil::stringToData(const std::string& str,char* data,int startPos)
{
	for (int i=0;i<int(str.length());i++)
		data[startPos+i]=str[i];
}

std::string CTTUtil::getAdjustedString(const std::string& str,int length)
{
	std::string retString(str);
	while (int(retString.length())<length)
		retString+=' ';
	while (int(retString.length())>length)
		retString.erase(retString.end()-1);
	return(retString);
}

std::string CTTUtil::getLightEncodedString(const std::string& ss)
{ // ss can contain any char, also 0!
	std::string txt(ss);
	std::string s;
	for (int i=0;i<int(txt.length());i++)
	{
		int n[2];
		n[0]=(unsigned char)(txt[i])+i;
		n[1]=n[0]/16;
		n[0]-=n[1]*16;
		for (int j=0;j<2;j++)
		{
			int t=n[1-j];
			if (t>9)
				s+='A'+(t-10);
			else
				s+='0'+t;
		}
	}
	return(s);
}

std::string CTTUtil::getLightDecodedString(const std::string& ss)
{ // return string can contain any char, also 0!
	std::string txt(ss);
	std::string s;
	for (int i=0;i<int(txt.length())/2;i++)
	{
		int v=0;
		for (int j=0;j<2;j++)
		{
			int w;
			char a=txt[2*i+j];
			if (a>='A')
				w=10+(a-'A');
			else
				w=a-'0';
			if (j==0)
				v+=w*16;
			else
				v+=w;
		}		
		v-=i;
		s+=char(v);
	}
	return(s);
}

bool CTTUtil::removeSpacesAtBeginningAndEnd(std::string& line)
{
	while ((line.length()!=0)&&(line[0]==' '))
		line.erase(line.begin());
	while ((line.length()!=0)&&(line[line.length()-1]==' '))
		line.erase(line.begin()+line.length()-1);
	return(line.length()!=0);
}

std::string CTTUtil::getFormattedString(const char* a,const char* b,const char* c,const char* d,const char* e,const char* f,const char* g,const char* h)
{
	std::string retVal("");
	if (a==NULL)
		return(retVal);
	retVal+=a;
	if (b==NULL)
		return(retVal);
	retVal+=b;
	if (c==NULL)
		return(retVal);
	retVal+=c;
	if (d==NULL)
		return(retVal);
	retVal+=d;
	if (e==NULL)
		return(retVal);
	retVal+=e;
	if (f==NULL)
		return(retVal);
	retVal+=f;
	if (g==NULL)
		return(retVal);
	retVal+=g;
	if (h==NULL)
		return(retVal);
	retVal+=h;
	return(retVal);
}

int CTTUtil::replaceWordInLine(std::string& line,const std::string& oldText,const std::string& newText)
{
	int retVal=0;
	int p=0;
	p=line.find(oldText,p);
	while (p!=-1)
	{
		line.erase(line.begin()+p,line.begin()+p+oldText.length());
		line.insert(line.begin()+p,newText.begin(),newText.end());
		p=line.find(oldText,p+1);
		retVal++;
	}
	return(retVal);
}

std::string CTTUtil::formatString(const std::string fmt, ...)
{ // taken from http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
	// Courtesy of Erik Aronesty
    int size = 512;
    std::string str;
    va_list ap;
    while (1)
	{
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf((char *)str.c_str(), size, fmt.c_str(), ap);
        va_end(ap);
        if (n > -1 && n < size) {
            str.resize(n);
            return str;
        }
        if (n > -1)
            size = n + 1;
        else
            size *= 2;
    }
    return str;
}

std::string CTTUtil::getLowerCaseString(const char* str)
{
	std::string retValue(str);
	for (int i=0;i<int(retValue.length());i++)
	{
		if ((retValue[i]>=65)&&(retValue[i]<=90))
			retValue[i]+=32;
	}
	return(retValue);
}

void CTTUtil::scaleLightDown_(float& r,float& g,float& b)
{ // this is for backward compatibility (29/8/2013). Default lights are now less intense, and more similar to other applications
	r*=0.675f;
	g*=0.675f;
	b*=0.675f;
}

void CTTUtil::scaleColorUp_(float& r,float& g,float& b)
{ // this is for backward compatibility (29/8/2013). Default lights are now less intense, and more similar to other applications
	float f=1.5f;
	float m=SIM_MAX(SIM_MAX(r,g),b);
	float d=1.0f/m;
	if (d<f)
		f=d;
	r*=f;
	g*=f;
	b*=f;
}

void CTTUtil::scaleLightDown_(float* rgb)
{
	scaleLightDown_(rgb[0],rgb[1],rgb[2]);
}

void CTTUtil::scaleColorUp_(float* rgb)
{
	scaleColorUp_(rgb[0],rgb[1],rgb[2]);
}
