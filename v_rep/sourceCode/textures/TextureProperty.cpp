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
#include "v_rep_internal.h"
#include "TextureProperty.h"
#include "IloIlo.h"

#include "OGL.h"
#include "Tt.h"
#include "v_repStrings.h"
#include "App.h"

CTextureProperty::CTextureProperty()
{
	_commonInit();
}

CTextureProperty::CTextureProperty(int textureOrVisionSensorObjectID)
{
	_commonInit();
	_textureOrVisionSensorObjectID=textureOrVisionSensorObjectID;
}

CTextureProperty::~CTextureProperty()
{
}

void CTextureProperty::setTextureMapMode(int mode)
{
	if ((_textureCoordinateMode!=mode)||getFixedCoordinates())
	{
		setFixedCoordinates(NULL);
		_calculatedTextureCoordinates.clear();
		_textureCoordinateMode=mode;
		if (mode==TEXTURE_COORD_CYLINDER)
		{
			_textureScalingX=1.0f;
			_repeatU=true;
		}
		if (mode==TEXTURE_COORD_SPHERE)
		{
			_textureScalingX=1.0f;
			_textureScalingY=1.0f;
			_repeatU=true;
			_repeatV=true;
		}
	}
}

int CTextureProperty::getTextureMapMode()
{
	return(_textureCoordinateMode);
}

void CTextureProperty::_commonInit()
{
	_startedTexture=NULL;
	_interpolateColor=false;
	_decal=false; // from true to false on 12/6/2011
	_repeatU=false;
	_repeatV=false;
	_textureOrVisionSensorObjectID=-1;
	_objectStateId=-1; // uninitialized
	_textureCoordinateMode=TEXTURE_COORD_PLANE;

	_textureRelativeConfig.setIdentity();
	_textureScalingX=1.0f;
	_textureScalingY=1.0f;
}

void CTextureProperty::setRepeatU(bool r)
{
	_repeatU=r;
	_objectStateId=-1; // Force a new calculation of texture coordinates
}

bool CTextureProperty::getRepeatU()
{
	return(_repeatU);
}

void CTextureProperty::setRepeatV(bool r)
{
	_repeatV=r;
	_objectStateId=-1; // Force a new calculation of texture coordinates
}

bool CTextureProperty::getRepeatV()
{
	return(_repeatV);
}

void CTextureProperty::addTextureDependencies(int objID,int objSubID)
{
	CTextureObject* it=App::ct->textureCont->getObject(_textureOrVisionSensorObjectID);
	if (it!=NULL)
		it->addDependentObject(objID,objSubID);
}

void CTextureProperty::scaleObject(float scalingFactor)
{
	_objectStateId=-1; // Force a new calculation of texture coordinates
	_textureRelativeConfig.X*=scalingFactor;
	if ((_textureCoordinateMode==TEXTURE_COORD_PLANE)||(_textureCoordinateMode==TEXTURE_COORD_CUBE))
	{
		_textureScalingX*=scalingFactor;
		_textureScalingY*=scalingFactor;
	}
	if (_textureCoordinateMode==TEXTURE_COORD_CYLINDER)
	{
		_textureScalingY*=scalingFactor;
	}
}

void CTextureProperty::adjustForFrameChange(const C7Vector& mCorrection)
{
	_textureRelativeConfig=mCorrection*_textureRelativeConfig;
}

std::vector<float>* CTextureProperty::getTextureCoordinates(int objectStateId,const C7Vector& transf,const std::vector<float>& vertices,const std::vector<int>& triangles)
{ // can return NULL if texture needs to be destroyed!
	if (_fixedTextureCoordinates.size()!=0)
	{ // We have fixed coordinates!
		if ( (triangles.size()==_fixedTextureCoordinates.size()/2) )
			return(&_fixedTextureCoordinates);
		return(NULL); // texture needs to be destroyed
	}

	if ( (objectStateId==_objectStateId)&&(triangles.size()==_calculatedTextureCoordinates.size()/2) )
		return(&_calculatedTextureCoordinates);

	if (vertices.size()==0)
	{
		_calculatedTextureCoordinates.clear();
		return(&_calculatedTextureCoordinates);
	}
	// we need to compute the texture coordinates!
	_calculatedTextureCoordinates.clear();
	C7Vector tr(_textureRelativeConfig.getInverse()*transf);
	CTextureObject* it=NULL;
	if ( (_textureOrVisionSensorObjectID>=SIM_IDSTART_TEXTURE)&&(_textureOrVisionSensorObjectID<=SIM_IDEND_TEXTURE) )
		it=App::ct->textureCont->getObject(_textureOrVisionSensorObjectID);
	else
	{
		CVisionSensor* rend=App::ct->objCont->getVisionSensor(_textureOrVisionSensorObjectID);
		if (rend!=NULL)
			it=rend->getTextureObject();
	}
	float xs=1.0f;
	float ys=1.0f;
	if (it!=NULL)
	{
		_objectStateId=objectStateId;
		int sizeX,sizeY;
		it->getTextureSize(sizeX,sizeY);
		if (sizeX>=sizeY)
			ys=float(sizeY)/float(sizeX);
		else
			xs=float(sizeX)/float(sizeY);
	}

	for (int i=0;i<int(triangles.size())/3;i++)
	{
		int ind3[3]={triangles[3*i+0],triangles[3*i+1],triangles[3*i+2]};
		float tc[3][2];

		C3Vector v[3]={&vertices[3*ind3[0]],&vertices[3*ind3[1]],&vertices[3*ind3[2]]};
		v[0]*=tr;
		v[1]*=tr;
		v[2]*=tr;
		float x=0.0f;
		float y=0.0f;

		if ((_textureCoordinateMode==TEXTURE_COORD_CYLINDER)||(_textureCoordinateMode==TEXTURE_COORD_SPHERE))
		{
			int verticesInCenterCnt=0;
			int verticesInCenterIndex[3];
			int verticesNotInCenterCnt=0;
			int verticesNotInCenterIndex[3];
			for (int ot3=0;ot3<3;ot3++)
			{
				if ( (fabs(v[ot3](1))<0.00001f)&&(fabs(v[ot3](0))<0.00001f) )
				{ // this vertex is in the center!
					verticesInCenterIndex[verticesInCenterCnt]=ot3;
					verticesInCenterCnt++;
				}
				else
				{ // this vertex is not in the center!
					verticesNotInCenterIndex[verticesNotInCenterCnt]=ot3;
					verticesNotInCenterCnt++;
				}
				if (_textureCoordinateMode==TEXTURE_COORD_CYLINDER)
				{
					x=((atan2(v[ot3](1),v[ot3](0))/piValue)+1.0f)/2.0f;// _textureScalingX doesn't make sense here!  (2.0f*_textureScalingX);
					y=(v[ot3](2)/_textureScalingY*ys)+0.5f;
				}
				if (_textureCoordinateMode==TEXTURE_COORD_SPHERE)
				{
					x=((atan2(v[ot3](1),v[ot3](0))/piValue)+1.0f)/2.0f; // _textureScalingX doesn't make sense here!   (2.0f*_textureScalingX);
					float a2=C3Vector::unitZVector.getAngle(v[ot3]);
					y=(1.0f-(a2/piValue)); // _textureScalingX doesn't make sense here!  /_textureScalingY;
				}
				tc[ot3][0]=x;
				tc[ot3][1]=y;
			}

			if (verticesInCenterCnt==2)
			{
				tc[verticesInCenterIndex[0]][0]=tc[verticesNotInCenterIndex[0]][0];
				tc[verticesInCenterIndex[1]][0]=tc[verticesNotInCenterIndex[0]][0];
			}
			else
			{
				if (verticesInCenterCnt==1)
				{
					float am=tc[verticesNotInCenterIndex[0]][0];
					float bm=tc[verticesNotInCenterIndex[1]][0];
					if (am>=bm)
					{
						if (am-bm>0.5f)
							tc[verticesInCenterIndex[0]][0]=(am+bm+1.0f)*0.5f;
						else
							tc[verticesInCenterIndex[0]][0]=(am+bm)*0.5f;
					}
					else
					{
						if (bm-am>0.5f)
							tc[verticesInCenterIndex[0]][0]=(am+bm+1.0f)*0.5f;
						else
							tc[verticesInCenterIndex[0]][0]=(am+bm)*0.5f;
					}
				}
			}

			int sorted[3];
			if (tc[0][0]>=tc[1][0])
			{
				if (tc[0][0]>=tc[2][0])
				{
					if (tc[1][0]>=tc[2][0])
					{
						sorted[0]=0;
						sorted[1]=1;
						sorted[2]=2;
					}
					else
					{
						sorted[0]=0;
						sorted[1]=2;
						sorted[2]=1;
					}

				}
				else
				{ // 1>2, 1<3
					sorted[0]=2;
					sorted[1]=0;
					sorted[2]=1;
				}
			}
			else
			{ // 1<2
				if (tc[0][0]>=tc[2][0])
				{ // 1<2, 1>3
					sorted[0]=1;
					sorted[1]=0;
					sorted[2]=2;
				}
				else
				{
					if (tc[1][0]>=tc[2][0])
					{
						sorted[0]=1;
						sorted[1]=2;
						sorted[2]=0;
					}
					else
					{
						sorted[0]=2;
						sorted[1]=1;
						sorted[2]=0;
					}
				}
			}
			
			if (tc[sorted[0]][0]-tc[sorted[2]][0]>0.5f)
			{
				tc[sorted[2]][0]+=1.0f;
				if (tc[sorted[0]][0]-tc[sorted[1]][0]>0.5f)
					tc[sorted[1]][0]+=1.0f;
			}

		}
		if (_textureCoordinateMode==TEXTURE_COORD_CUBE)
		{ // TEXTURE_COORD_CUBE
			C3Vector n((v[1]-v[0])^(v[2]-v[0]));
			n.normalize();
			int xIndex;
			int yIndex;
			if ( (fabs(n(0)))>=(fabs(n(1))) )
			{ // x>y
				if ( (fabs(n(0)))>=(fabs(n(2))) )
				{ // x>z
					xIndex=1;
					yIndex=2;
				}
				else
				{ // z>x
					xIndex=0;
					yIndex=1;
				}
			}
			else
			{ // y>x
				if ( (fabs(n(1)))>=(fabs(n(2))) )
				{ // y>z
					xIndex=2;
					yIndex=0;
				}
				else
				{ // z>y
					xIndex=0;
					yIndex=1;
				}
			}
			tc[0][0]=(v[0](xIndex)/(_textureScalingX*xs))+0.5f;
			tc[0][1]=(v[0](yIndex)/(_textureScalingY*ys))+0.5f;
			tc[1][0]=(v[1](xIndex)/(_textureScalingX*xs))+0.5f;
			tc[1][1]=(v[1](yIndex)/(_textureScalingY*ys))+0.5f;
			tc[2][0]=(v[2](xIndex)/(_textureScalingX*xs))+0.5f;
			tc[2][1]=(v[2](yIndex)/(_textureScalingY*ys))+0.5f;
		}
		if (_textureCoordinateMode==TEXTURE_COORD_PLANE)
		{ // TEXTURE_COORD_PLANE
			tc[0][0]=(v[0](0)/(_textureScalingX*xs))+0.5f;
			tc[0][1]=(v[0](1)/(_textureScalingY*ys))+0.5f;
			tc[1][0]=(v[1](0)/(_textureScalingX*xs))+0.5f;
			tc[1][1]=(v[1](1)/(_textureScalingY*ys))+0.5f;
			tc[2][0]=(v[2](0)/(_textureScalingX*xs))+0.5f;
			tc[2][1]=(v[2](1)/(_textureScalingY*ys))+0.5f;
		}

		_calculatedTextureCoordinates.push_back(tc[0][0]);
		_calculatedTextureCoordinates.push_back(tc[0][1]);
		_calculatedTextureCoordinates.push_back(tc[1][0]);
		_calculatedTextureCoordinates.push_back(tc[1][1]);
		_calculatedTextureCoordinates.push_back(tc[2][0]);
		_calculatedTextureCoordinates.push_back(tc[2][1]);
	}

	return(&_calculatedTextureCoordinates);
}

bool CTextureProperty::start3DTextureDisplay()
{
	if (_textureOrVisionSensorObjectID==-1)
		return(false);
	CTextureObject* it=NULL;
	if ((_textureOrVisionSensorObjectID>=SIM_IDSTART_TEXTURE)&&(_textureOrVisionSensorObjectID<=SIM_IDEND_TEXTURE))
		it=App::ct->textureCont->getObject(_textureOrVisionSensorObjectID);
	CVisionSensor* rs=NULL;
	if ((_textureOrVisionSensorObjectID>=SIM_IDSTART_3DOBJECT)&&(_textureOrVisionSensorObjectID<=SIM_IDEND_3DOBJECT))
	{
		rs=App::ct->objCont->getVisionSensor(_textureOrVisionSensorObjectID);
		if (rs!=NULL)
			it=rs->getTextureObject();
	}
	if (it!=NULL)
	{
		_startedTexture=it;
		it->startTextureDisplay(_interpolateColor,_decal,_repeatU,_repeatV);
		return(true);
	}
	if (rs!=NULL)
		return(false);
	static bool first=true;
	if (first)
		App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSNOTR_APPLICATION_ERROR),strTranslate(IDSNOTR_STRANGE_ERROR8),VMessageBox::OKELI);
	first=false;
	return(false);
}

void CTextureProperty::end3DTextureDisplay()
{
	if (_startedTexture!=NULL)
		_startedTexture->endTextureDisplay();
	_startedTexture=NULL;
}

bool CTextureProperty::start2DTextureDisplay()
{
	if (_textureOrVisionSensorObjectID==-1)
		return(false);
	CTextureObject* it=NULL;
	if ((_textureOrVisionSensorObjectID>=SIM_IDSTART_TEXTURE)&&(_textureOrVisionSensorObjectID<=SIM_IDEND_TEXTURE))
		it=App::ct->textureCont->getObject(_textureOrVisionSensorObjectID);
	CVisionSensor* rs=NULL;
	if ((_textureOrVisionSensorObjectID>=SIM_IDSTART_3DOBJECT)&&(_textureOrVisionSensorObjectID<=SIM_IDEND_3DOBJECT))
	{
		rs=App::ct->objCont->getVisionSensor(_textureOrVisionSensorObjectID);
		if (rs!=NULL)
			it=rs->getTextureObject();
	}
	if (it!=NULL)
	{
		_startedTexture=it;

		// Following 3 to have "transparency"
		glColor3f(1.0f, 1.0f, 1.0f);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER,0.0f);

		it->startTextureDisplay(_interpolateColor,_decal,_repeatU,_repeatV);
		return(true);
	}
	if (rs!=NULL)
		return(false);
	static bool first=true;
	if (first)
		App::uiThread->messageBox_critical(App::mainWindow,strTranslate(IDSNOTR_APPLICATION_ERROR),strTranslate(IDSNOTR_STRANGE_ERROR9),VMessageBox::OKELI);
	first=false;
	return(false);
}

void CTextureProperty::end2DTextureDisplay()
{
	if (_startedTexture!=NULL)
		_startedTexture->endTextureDisplay();
	_startedTexture=NULL;
	// Following 2 to remove "transparency"
	ogl::setBlending(false);
	glDisable(GL_ALPHA_TEST);
}

void CTextureProperty::setInterpolateColors(bool ic)
{
	_interpolateColor=ic;
}

bool CTextureProperty::getInterpolateColors()
{
	return(_interpolateColor);
}

void CTextureProperty::setDecalTexture(bool dt)
{
	_decal=dt;
}

bool CTextureProperty::getDecalTexture()
{
	return(_decal);
}


CTextureProperty* CTextureProperty::copyYourself()
{
	CTextureProperty* newObj=new CTextureProperty();
	newObj->_interpolateColor=_interpolateColor;
	newObj->_decal=_decal;
	newObj->_repeatU=_repeatU;
	newObj->_repeatV=_repeatV;
	newObj->_textureOrVisionSensorObjectID=_textureOrVisionSensorObjectID;
	newObj->_textureCoordinateMode=_textureCoordinateMode;
	newObj->_textureRelativeConfig=_textureRelativeConfig;
	newObj->_textureScalingX=_textureScalingX;
	newObj->_textureScalingY=_textureScalingY;
	newObj->_fixedTextureCoordinates.assign(_fixedTextureCoordinates.begin(),_fixedTextureCoordinates.end());

	return(newObj);
}

int CTextureProperty::getTextureObjectID()
{
	return(_textureOrVisionSensorObjectID);
}
bool CTextureProperty::announceObjectWillBeErased(int objID)
{
	if ((_textureOrVisionSensorObjectID>=SIM_IDSTART_3DOBJECT)&&(_textureOrVisionSensorObjectID<=SIM_IDEND_3DOBJECT))
		return(_textureOrVisionSensorObjectID==objID);
	return(false);
}

void CTextureProperty::performObjectLoadingMapping(std::vector<int>* map)
{ // New_Object_ID=map[Old_Object_ID]
	if ((_textureOrVisionSensorObjectID>=SIM_IDSTART_3DOBJECT)&&(_textureOrVisionSensorObjectID<=SIM_IDEND_3DOBJECT))
		_textureOrVisionSensorObjectID=App::ct->objCont->getLoadingMapping(map,_textureOrVisionSensorObjectID); // texture is a vision sensor texture object
}

void CTextureProperty::performTextureObjectLoadingMapping(std::vector<int>* map)
{
	if ((_textureOrVisionSensorObjectID>=SIM_IDSTART_TEXTURE)&&(_textureOrVisionSensorObjectID<=SIM_IDEND_TEXTURE))
		_textureOrVisionSensorObjectID=App::ct->objCont->getLoadingMapping(map,_textureOrVisionSensorObjectID); // texture is a regular texture object
}

C7Vector CTextureProperty::getTextureRelativeConfig()
{
	return(_textureRelativeConfig);
}

void CTextureProperty::setTextureRelativeConfig(const C7Vector& c)
{
	_textureRelativeConfig=c;
	_objectStateId=-1; // Force a new calculation of texture coordinates
}

void CTextureProperty::setFixedCoordinates(std::vector<float>* coords)
{ // NULL to remove them and have calculated coords
	_fixedTextureCoordinates.clear();
	if ( (coords!=NULL)&&(coords->size()!=0) )
	{
		_fixedTextureCoordinates.assign(coords->begin(),coords->end());
		_repeatU=true;
		_repeatV=true;
	}
}

bool CTextureProperty::getFixedCoordinates()
{
	return(_fixedTextureCoordinates.size()!=0);
}

void CTextureProperty::getTextureScaling(float& x,float& y)
{
	x=_textureScalingX;
	y=_textureScalingY;
}

void CTextureProperty::setTextureScaling(float x,float y)
{
	_textureScalingX=x;
	_textureScalingY=y;
	_objectStateId=-1; // Force a new calculation of texture coordinates
}

void CTextureProperty::serialize(CSer& ar)
{
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Bst");
		BYTE nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,_interpolateColor);
		SIM_SET_CLEAR_BIT(nothing,1,_decal);
		SIM_SET_CLEAR_BIT(nothing,2,_repeatU);
		SIM_SET_CLEAR_BIT(nothing,3,_repeatV);
		ar << nothing;
		ar.flush();

		ar.storeDataName("Tob");
		ar << _textureOrVisionSensorObjectID << _textureCoordinateMode << _textureScalingX << _textureScalingY;
		ar.flush();

		ar.storeDataName("Trc");
		ar << _textureRelativeConfig.Q(0) << _textureRelativeConfig.Q(1) << _textureRelativeConfig.Q(2) << _textureRelativeConfig.Q(3);
		ar << _textureRelativeConfig.X(0) << _textureRelativeConfig.X(1) << _textureRelativeConfig.X(2);
		ar.flush();

		ar.storeDataName("Ftc");
		for (int i=0;i<int(_fixedTextureCoordinates.size());i++)
			ar << _fixedTextureCoordinates[i];
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
				if (theName=="Bst")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_interpolateColor=SIM_IS_BIT_SET(nothing,0);
					_decal=SIM_IS_BIT_SET(nothing,1);
					_repeatU=SIM_IS_BIT_SET(nothing,2);
					_repeatV=SIM_IS_BIT_SET(nothing,3);
				}
				if (theName.compare("Tob")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _textureOrVisionSensorObjectID >> _textureCoordinateMode >> _textureScalingX >> _textureScalingY;
				}
				if (theName.compare("Trc")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _textureRelativeConfig.Q(0) >> _textureRelativeConfig.Q(1) >> _textureRelativeConfig.Q(2) >> _textureRelativeConfig.Q(3);
					ar >> _textureRelativeConfig.X(0) >> _textureRelativeConfig.X(1) >> _textureRelativeConfig.X(2);
				}

				if (theName.compare("Ftc")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					_fixedTextureCoordinates.resize(byteQuantity/sizeof(float),0.0f);
					for (int i=0;i<int(_fixedTextureCoordinates.size());i++)
						ar >> _fixedTextureCoordinates[i];
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
	}
}
