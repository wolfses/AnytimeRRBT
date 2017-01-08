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
#include "Tt.h"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include "MyMath.h"
#include <QString>

struct filestruct_A
{
	bool operator<(const filestruct_A& rhs) const
	{
		return valA < rhs.valA;
	}
	int valA;
	int valB;
};

struct filestruct_B
{
	bool operator<(const filestruct_B& rhs) const
	{
		return valA < rhs.valA;
	}
	float valA;
	int valB;
};

struct filestruct_C
{
	bool operator<(const filestruct_C& rhs) const
	{
		return valA < rhs.valA;
	}
	std::string valA;
	int valB;
};

bool tt::stringToFloat(const char* txt,float& f,bool allowNegativeValue,bool infGivesMinusOne)
{
	QString str(txt);
	if (allowNegativeValue)
	{
		QString tmpStr(str);
		while ((tmpStr.length()!=0)&&(tmpStr[0]==' '))
			tmpStr.remove(0,1);
		int l=3;
		if ((tmpStr.length()!=0)&&((tmpStr[0]=='+')||(tmpStr[0]=='-')))
			l=4;
		tmpStr=tmpStr.left(l);
		if (tmpStr.compare("inf",Qt::CaseInsensitive)==0)
		{
			f=FLT_MAX;
			return(true);
		}
		if (tmpStr.compare("+inf",Qt::CaseInsensitive)==0)
		{
			f=FLT_MAX;
			return(true);
		}
		if (tmpStr.compare("-inf",Qt::CaseInsensitive)==0)
		{
			f=FLT_MIN;
			return(true);
		}

		bool ok;
		f=str.toFloat(&ok);
		return(ok);
	}
	else
	{
		QString tmpStr(str);
		while ((tmpStr.length()!=0)&&(tmpStr[0]==' '))
			tmpStr.remove(0,1);
		int l=3;
		if ((tmpStr.length()!=0)&&((tmpStr[0]=='+')||(tmpStr[0]=='-')))
			l=4;
		tmpStr=tmpStr.left(l);
		if (tmpStr.compare("inf",Qt::CaseInsensitive)==0)
		{
			if (infGivesMinusOne)
				f=-1.0f;
			else
				f=FLT_MAX;
			return(true);
		}
		if (tmpStr.compare("+inf",Qt::CaseInsensitive)==0)
		{
			if (infGivesMinusOne)
				f=-1.0f;
			else
				f=FLT_MAX;
			return(true);
		}
		if (tmpStr.compare("-inf",Qt::CaseInsensitive)==0)
		{
			f=0.0f;
			return(true);
		}

		bool ok;
		f=str.toFloat(&ok);
		if (ok&&(f<0.0f))
			f=0.0f;
		return(ok);
	}
}

std::string tt::floatToString(float f,bool canBeNegative)
{
	if ((!canBeNegative)&&(f<0.0f))
		return("Infinity");
	if (f==FLT_MAX)
		return("Infinity");
	if (f==FLT_MIN)
		return("-Infinity");
	QString str;
	if (f==0.0f)
		return("0.00000000");
	if ((fabs(f)>=100000.0)||(fabs(f)<0.0001))
		str=QString("%1").arg(f,0,'e',4);
	else
	{
		int dec=8;
		if (f>=10)
			dec--;
		if (f>=100)
			dec--;
		if (f>=1000)
			dec--;
		if (f>=10000)
			dec--;
		str=QString("%1").arg(f,0,'f',dec);
	}
	return(str.toStdString());
}

bool tt::stringToInt(const char* txt,int& a)
{
	QString str(txt);
	bool ok;
	a=str.toInt(&ok);
	return(ok);
}

std::string tt::intToString(int a)
{
	QString str;
	str=QString("%1").arg(a);
	return(str.toStdString());
}

float tt::floatToUserFloat(float f,float toUserConversion,bool minusValuesGiveInf)
{
	if (f<0.0f)
	{
		if ((!minusValuesGiveInf)&&(f!=FLT_MIN))
			return(f*toUserConversion);
	}
	else
	{
		if (f!=FLT_MAX)
			return(f*toUserConversion);
	}
	return(f);
}

float tt::userFloatToFloat(float userFloat,float fromUserConversion,bool minusValuesGiveInf)
{
	if (userFloat<0.0f)
	{
		if ((!minusValuesGiveInf)&&(userFloat!=FLT_MIN))
			return(userFloat*fromUserConversion);
	}
	else
	{
		if (userFloat!=FLT_MAX)
			return(userFloat*fromUserConversion);
	}
	return(userFloat);
}

std::string tt::decorateString(const char* prefix,const std::string mainText,const char* suffix)
{
	std::string retVal(prefix);
	retVal+=mainText;
	retVal+=suffix;
	return(retVal);
}

void tt::lightEncodeBuffer(char* buff,int length)
{ // Just use the first end last bytes for encoding key:
	if (length<3)
		return;
	int start=buff[0]/2;
	int end=buff[length-1]*3;
	for (int i=1;i<length-1;i++)
	{
		if (i&1)
			buff[i]-=i+start+end;
		if (i&2)
			buff[i]+=3*i+start;
		if (i&4)
			buff[i]-=i/2+end;
	}
}

void tt::lightDecodeBuffer(char* buff,int length)
{
	if (length<3)
		return;
	int start=buff[0]/2;
	int end=buff[length-1]*3;
	for (int i=1;i<length-1;i++)
	{
		if (i&1)
			buff[i]+=i+start+end;
		if (i&2)
			buff[i]-=3*i+start;
		if (i&4)
			buff[i]+=i/2+end;
	}
}

void tt::removeComments(std::string& line)
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

bool tt::removeSpacesAtBeginningAndEnd(std::string& line)
{
	while ((line.length()!=0)&&(line[0]==' '))
		line.erase(line.begin());
	while ((line.length()!=0)&&(line[line.length()-1]==' '))
		line.erase(line.begin()+line.length()-1);
	return(line.length()!=0);
}

bool tt::removeSpacesAndEmptyLinesAtBeginningAndEnd(std::string& line)
{
	while ((line.length()!=0)&&((line[0]==' ')||(line[0]==10)))
		line.erase(line.begin());
	while ((line.length()!=0)&&((line[line.length()-1]==' ')||(line[line.length()-1]==10)))
		line.erase(line.begin()+line.length()-1);
	return(line.length()!=0);
}

int tt::getDecimalPos(float number,int maxDec)
{
//	int retVal=0;
	if (number==0.0f)
		return(-1);
	for (int i=0;i<maxDec;i++)
	{
		if (fabs(number)>=1.0f)
		{
			number-=float(int(number));
			number*=10.0f;
			if ((fabs(number)>=1.0f)&&(i<maxDec) )
				return(i+1);
			return(i);
		}
		number*=10.0f;
	}
	return(maxDec);
}

std::string tt::FNb(int leadingZeros,float number,int decimals,bool sign)
{ // sign is true by default

	int dec=getDecimalPos(number,decimals*2);
	if (dec>decimals)
		decimals=dec;

	std::string tmp;
	if (sign)
		tmp=boost::str(boost::format("%%+#0%i.%if") % (leadingZeros+decimals+2) % decimals);
	else
		tmp=boost::str(boost::format("%%#0%i.%if") % (leadingZeros+decimals+2) % decimals);
	std::string tmp2(boost::str(boost::format(tmp) % number));
	return(tmp2);
}

std::string tt::FNb(float number)
{
	return(boost::str(boost::format("%f") % number));
}

std::string tt::FNb(int leadingZeros,int number,bool sign)
{ // sign is false by default
	std::string tmp;
	if (sign)
		tmp=boost::str(boost::format("%%+#0%ii") % leadingZeros);
	else
		tmp=boost::str(boost::format("%%#0%ii") % leadingZeros);
	std::string tmp2(boost::str(boost::format(tmp) % number));
	return(tmp2);
}

std::string tt::FNb(int number)
{
	return(boost::str(boost::format("%i") % number));
}

double tt::getLimitedDouble(double minValue,double maxValue,double value)
{
	if (value>maxValue)
		value=maxValue;
	if (value<minValue)
		value=minValue;
	return(value);
}

void tt::limitValue(float minValue,float maxValue,float value[2])
{
	if (value[0]>maxValue) value[0]=maxValue;
	if (value[1]>maxValue) value[1]=maxValue;
	if (value[0]<minValue) value[0]=minValue;
	if (value[1]<minValue) value[1]=minValue;
}

void tt::limitValue(int minValue,int maxValue,int* value)
{
	if ((*value)>maxValue) 
		(*value)=maxValue;
	if ((*value)<minValue) 
		(*value)=minValue;
}

bool tt::getValidFloat(const std::string& text,float& value)
{	// don't forget those cases: 1.06581410364015e-014
	try
	{
		value=boost::lexical_cast<float>(text);
		return(true);
	}
	catch (boost::bad_lexical_cast &)
	{
		return(false);
	}
}

bool tt::getValidInt(const std::string& text,int& value)
{
	try
	{
		value=boost::lexical_cast<int>(text);
		return(true);
	}
	catch (boost::bad_lexical_cast &)
	{ // "1.0" and such are caught here too!
		return(false);
	}
}

bool tt::extractWord(std::string& line,std::string& word)
{ // Returns true if a word could be extracted
	// 1. We remove leading spaces/tabs
	while ( (line.length()!=0)&&((line[0]==' ')||(line[0]==(char)9)) )
		line.erase(line.begin());
	// 2. We extract a word:
	word="";
	while ( (line.length()!=0)&&(line[0]!=' ')&&(line[0]!=(char)9) )
	{
		word.append(line.begin(),line.begin()+1);
		line.erase(line.begin());
	}
	return(word.length()!=0);
}

bool tt::extractCommaSeparatedWord(std::string& line,std::string& word)
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

int tt::getNameSuffixNumber(const char* name,bool dash)
{ // -1 means there is no suffix!!
	std::string n(name);
	// In case the string is empty:
	if (n.length()==0)
		return(-1);
	// We count how many digits at the end:
	char aChar=n[n.length()-1];
	std::string oldNumber("");
	while ( (aChar>='0')&&(aChar<='9') )
	{
		oldNumber.insert(oldNumber.begin(),aChar);
		n.erase(n.end()-1);
		if (n.length()!=0)
			aChar=name[n.length()-1];
		else
			aChar='a';
	}
	if ((aChar!='#')&&dash) // 2009/02/04 (NEW COPY INDICATOR!)
		return(-1);
	if (oldNumber.length()==0)
		return(-1);
	return(atoi(oldNumber.c_str()));
}

std::string tt::getNameWithoutSuffixNumber(const char* name,bool dash)
{
	std::string n(name);
	if (n.length()==0)
		return("");
	// We count how many digits at the end:
	char aChar=n[n.length()-1];
	while ( (aChar>='0')&&(aChar<='9') )
	{
		n.erase(n.end()-1);
		if (n.length()!=0)
			aChar=name[n.length()-1];
		else
			aChar='a';
	}
	if (dash) // 2009/02/04 (NEW COPY INDICATOR!)
	{
		if (aChar!='#')
			return(name);
		n.erase(n.end()-1);
	}
	return(n);
}

std::string tt::generateNewName_dash(const std::string& name)
{
	return(generateNewName_dash(name,1)); // increment by 1
}

std::string tt::generateNewName_dash(const std::string& name,int suffixOffset)
{
	int newNumber=getNameSuffixNumber(name.c_str(),true)+suffixOffset;
	std::string nameWithoutSuffix(getNameWithoutSuffixNumber(name.c_str(),true));
	if (newNumber>=0)
	{ // should anyway always pass!!
		std::string newNumberStr=FNb(0,newNumber,false);
		nameWithoutSuffix+="#"; // 2009/02/04 (NEW COPY INDICATOR!)
		nameWithoutSuffix.append(newNumberStr);
	}
	return(nameWithoutSuffix);
}

std::string tt::generateNewName_noDash(const std::string& name)
{
	int newNumber=getNameSuffixNumber(name.c_str(),false)+1;
	std::string nameWithoutSuffix(getNameWithoutSuffixNumber(name.c_str(),false));
	std::string newNumberStr=FNb(0,newNumber,false);
	nameWithoutSuffix.append(newNumberStr);
	return(nameWithoutSuffix);
}

bool tt::removeIllegalCharacters(std::string& text,bool allowOneDashFollowedByNumbers)
{	// Illegal characters are replaced with underscore.
	// Permitted characters are: a-z, A-Z, 0-9, parenthesis and underscore
	// If allowOneDashFollowedByNumbers is true, then one dash (not in first position) followed by a number are ok 
	// Return value is true if something was modified
	bool retVal=false;

	size_t dashPos=std::string::npos;
	if (allowOneDashFollowedByNumbers)
	{
		dashPos=text.find('#');
		if (dashPos!=std::string::npos)
		{
			if ( (dashPos==0)||(dashPos==(text.length()-1)) )
				dashPos=std::string::npos;
			else
			{
				// Now check if there is a valid number after the dash:
				if ((text[dashPos+1]=='0')&&(text.length()>dashPos+2)) // no leading zero allowed after the dash, unless last char
					dashPos=-1;
				else
				{
					bool ok=true;
					for (int i=int(dashPos)+1;i<int(text.length());i++)
					{
						if ((text[i]<'0')||(text[i]>'9'))
						{
							ok=false;
							break;
						}
					}
					if (ok)
						text[dashPos]='_'; // so that we don't generate falsly a 'true' return value
					else
						dashPos=-1;
				}
			}
		}
	}

	for (int i=0;i<int(text.length());i++)
	{
		bool ok=false;
		if ((text[i]>='a')&&(text[i]<='z'))
			ok=true;
		if ((text[i]>='A')&&(text[i]<='Z'))
			ok=true;
		if ((text[i]>='0')&&(text[i]<='9'))
			ok=true;
		if ((text[i]=='(')||(text[i]==')')||(text[i]=='_'))
			ok=true;
		if (!ok)
		{
			text[i]='_';
			retVal=true;
		}
	}
	if (dashPos!=std::string::npos)
		text[dashPos]='#';
	return(retVal);
}

void tt::addToFloatArray(std::vector<float>* ar,float x,float y,float z)
{
	ar->push_back(x);
	ar->push_back(y);
	ar->push_back(z);
}
void tt::addToIntArray(std::vector<int>* ar,int x,int y,int z)
{
	ar->push_back(x);
	ar->push_back(y);
	ar->push_back(z);
}

float tt::getAngleMinusAlpha(float angle,float alpha)
{	// Angle and Alpha are specified in radian!!
	// Returns angle-alpha. Angle and alpha are angles (cyclic)!!
	
	// First we shift angle to zero (alpha follows):
	alpha=alpha-angle;
	// We limit alpha to values between 0 and 2Pi
	alpha=CMath::robustFmod(alpha,piValTimes2);
	if (alpha<0.0f)
		alpha=alpha+piValTimes2;
	if (alpha>piValue)
		return(piValTimes2-alpha); // angle is bigger
	else
		return(-alpha); // angle is smaller
}

float tt::getCyclicAngleDifference(float angleBigger,float angleSmaller,float range)
{
	angleBigger=getNormalizedAngleEx(angleBigger,range);
	angleSmaller=getNormalizedAngleEx(angleSmaller,range);

	float v[3];
	v[0]=angleBigger-angleSmaller;
	v[1]=angleBigger+range-angleSmaller;
	v[2]=angleBigger-range-angleSmaller;

	float smallest=SIM_MAX_FLOAT;
	int smallestIndex=-1;
	for (int i=0;i<3;i++)
	{
		if (fabs(v[i])<smallest)
		{
			smallest=fabs(v[i]);
			smallestIndex=i;
		}
	}

	return(v[smallestIndex]);
}

int tt::getSpecialScale(int input)
{ // Returns a special "Pseudo logarithmic" scale
	bool neg=input<0;
	if (input<0)
		input=-input;

	int tens=(input-2)/9;
	int unit=(input+tens)%10;
	if ( (unit==0)&&(input!=0) )
		unit=10;
	int output=1;
	while (tens!=0)
	{
		output=output*10;
		tens--;
	}
	output=output*unit;
	if (neg)
		output=-output;
	return(output);
}


void tt::rgbToHsl(float rgb[3],float hsl[3])
{
	float r=rgb[0];
	float g=rgb[1];
	float b=rgb[2];
	float h,s,l,delta;
	float cmax=SIM_MAX(r,SIM_MAX(g,b));
	float cmin=SIM_MIN(r,SIM_MIN(g,b));
	l=(cmax+cmin)/2.0f;
	if (cmax==cmin) 
	{
		s=0.0f;
		h=0.0f;
	} 
	else 
	{
		if(l<0.5f) 
			s=(cmax-cmin)/(cmax+cmin);
		else
			s=(cmax-cmin)/(2.0f-cmax-cmin);
		delta=cmax-cmin;
		if (r==cmax)
			h=(g-b)/delta;
		else 
			if (g==cmax)
				h=2.0f+(b-r)/delta;
			else
				h=4.0f+(r-g)/delta;
		h=h/6.0f;
		if (h<0.0f)
			h=h+1.0f;
	}
	hsl[0]=h;
	hsl[1]=s;
	hsl[2]=l;
}

float tt::_hueToRgb(float m1,float m2,float h)
{
	if (h<0.0f) 
		h=h+1.0f;
	if (h>1.0f) 
		h=h-1.0f;
	if (6.0f*h<1.0f)
		return(m1+(m2-m1)*h*6.0f);
	if (2.0f*h<1.0f)
		return(m2);
	if (3.0f*h<2.0f)
		return(m1+(m2-m1)*((2.0f/3.0f)-h)*6.0f);
	return(m1);
}

void tt::hslToRgb(float hsl[3],float rgb[3])
{
	float h=hsl[0];
	float s=hsl[1];
	float l=hsl[2];
	float m1,m2;

	if (s==0.0f)
	{
		rgb[0]=l;
		rgb[1]=l;
		rgb[2]=l;
	}
	else 
	{
		if (l<=0.5f)
			m2=l*(1.0f+s);
		else
			m2=l+s-l*s;
		m1=2.0f*l-m2;
		rgb[0]=_hueToRgb(m1,m2,h+1.0f/3.0f);
		rgb[1]=_hueToRgb(m1,m2,h);
		rgb[2]=_hueToRgb(m1,m2,h-1.0f/3.0f);
	}
}

void tt::orderAscending(std::vector<int>& toBeOrdered,std::vector<int>& index)
{
	std::vector<filestruct_A> lst;
	for (int i=0;i<int(toBeOrdered.size());i++)
	{
		filestruct_A s;
		s.valA=toBeOrdered[i];
		s.valB=index[i];
		lst.push_back(s);
	}
	std::sort(lst.begin(),lst.end());
	for (int i=0;i<int(lst.size());i++)
	{
		toBeOrdered[i]=lst[i].valA;
		index[i]=lst[i].valB;
	}
}

void tt::orderAscending(std::vector<float>& toBeOrdered,std::vector<int>& index)
{
	std::vector<filestruct_B> lst;
	for (int i=0;i<int(toBeOrdered.size());i++)
	{
		filestruct_B s;
		s.valA=toBeOrdered[i];
		s.valB=index[i];
		lst.push_back(s);
	}
	std::sort(lst.begin(),lst.end());
	for (int i=0;i<int(lst.size());i++)
	{
		toBeOrdered[i]=lst[i].valA;
		index[i]=lst[i].valB;
	}
}

void tt::orderStrings(std::vector<std::string>& toBeOrdered,std::vector<int>& index)
{
	std::vector<filestruct_C> lst;
	for (int i=0;i<int(toBeOrdered.size());i++)
	{
		filestruct_C s;
		s.valA=toBeOrdered[i];
		s.valB=index[i];
		lst.push_back(s);
	}
	std::sort(lst.begin(),lst.end());
	for (int i=0;i<int(lst.size());i++)
	{
		toBeOrdered[i]=lst[i].valA;
		index[i]=lst[i].valB;
	}
}


void tt::getColorFromIntensity(float intensity,int colorTable,float col[3])
{
	if (intensity>1.0f)
		intensity=1.0f;
	if (intensity<0.0f)
		intensity=0.0f;
	const float c[12]={0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,0.0f,0.0f,1.0f,1.0f,0.0f};
	int d=int(intensity*3);
	if (d>2)
		d=2;
	float r=(intensity-float(d)/3.0f)*3.0f;
	col[0]=c[3*d+0]*(1.0f-r)+c[3*(d+1)+0]*r;
	col[1]=c[3*d+1]*(1.0f-r)+c[3*(d+1)+1]*r;
	col[2]=c[3*d+2]*(1.0f-r)+c[3*(d+1)+2]*r;
}


float tt::getNormalizedAngle(float angle)
{ // Returns an angle between -PI and +PI
	return(getNormalizedAngleEx(angle,piValTimes2));
}

float tt::getNormalizedAngleEx(float angle,float range)
{ // returns a value between -range/2 and + range/2
	angle=CMath::robustFmod(angle,range);
	if (angle<-range/2)
		angle+=range;
	else if (angle>+range/2)
		angle-=range;
	return(angle);
}

int tt::getLimitedInt(int minValue,int maxValue,int value)
{
	if (value>maxValue)
		value=maxValue;
	if (value<minValue)
		value=minValue;
	return(value);
}

void tt::limitValue(int minValue,int maxValue,int &value)
{
	if (value>maxValue) value=maxValue;
	if (value<minValue) value=minValue;
}

float tt::getLimitedFloat(float minValue,float maxValue,float value)
{
	if (value>maxValue)
		value=maxValue;
	if (value<minValue)
		value=minValue;
	return(value);
}

void tt::limitValue(float minValue,float maxValue,float &value)
{
	if (value>maxValue) value=maxValue;
	if (value<minValue) value=minValue;
}
