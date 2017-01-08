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
#include "BannerObject.h"
#include "OGL.h"
#include "IloIlo.h"
#include "App.h"
#include "Tt.h"

bool CBannerObject::getCreatedFromScript()
{
	return(_createdFromScript);
}

void CBannerObject::setCreatedFromScript(bool c)
{
	_createdFromScript=c;
}

CBannerObject::CBannerObject(const char* label,int options,int sceneObjID,const float relConfig[6],const float labelCol[12],const float backCol[12],float height)
{
	_label=label;
	_height=height;
	_createdFromScript=false;
	color.setColorsAllBlack();
	backColor.setColorsAllBlack();
	backColor.colors[0]=1.0f;
	backColor.colors[1]=1.0f;
	backColor.colors[2]=1.0f;
	if (labelCol!=NULL)
	{
		color.setColor(labelCol+0,AMBIENT_MODE);
		color.setColor(labelCol+3,DIFFUSE_MODE);
		color.setColor(labelCol+6,SPECULAR_MODE);
		color.setColor(labelCol+9,EMISSION_MODE);
	}
	if (backCol!=NULL)
	{
		backColor.setColor(backCol+0,AMBIENT_MODE);
		backColor.setColor(backCol+3,DIFFUSE_MODE);
		backColor.setColor(backCol+6,SPECULAR_MODE);
		backColor.setColor(backCol+9,EMISSION_MODE);
	}
	_objectID=0;
	_sceneObjectID=sceneObjID;
	_options=options;
	_relativeConfig.setIdentity();
	if (relConfig!=NULL)
	{
		_relativeConfig.X.set(relConfig);
		_relativeConfig.Q.setEulerAngles(relConfig[3],relConfig[4],relConfig[5]);
	}
}

CBannerObject::~CBannerObject()
{
}

int CBannerObject::getSceneObjectID()
{
	return(_sceneObjectID);
}

void CBannerObject::setObjectID(int newID)
{
	_objectID=newID;
}

int CBannerObject::getObjectID()
{
	return(_objectID);
}

void CBannerObject::adjustForFrameChange(const C7Vector& preCorrection)
{
	_relativeConfig*=preCorrection;
}

void CBannerObject::adjustForScaling(float xScale,float yScale,float zScale)
{
	float avgScaling=(xScale+yScale+zScale)/3.0f;
	_relativeConfig.X*=avgScaling;
}

int CBannerObject::getOptions()
{
	return(_options);
}

int CBannerObject::getParentObjectHandle()
{
	return(_sceneObjectID);
}

void CBannerObject::draw2DStuff()
{

}

bool CBannerObject::mouseDownOn2DWindow(int mouseX,int mouseY)
{

	return(false);
}

void CBannerObject::draw3DStuff(bool overlay,bool transparentObject,int displayAttrib,const C4X4Matrix& cameraCTM,int windowSize[2],float verticalViewSizeOrAngle,bool perspective)
{
	if (_options&sim_banner_overlay)
	{
		if (!overlay)
			return;
	}
	else
	{
		if (overlay)
			return;
	}

	if (!overlay)
	{
		if (false)//_options&(sim_drawing_50percenttransparency+sim_drawing_25percenttransparency+sim_drawing_12percenttransparency))
		{
			if (!transparentObject)
				return;
		}
		else
		{
			if (transparentObject)
				return;
		}
	}


	C7Vector tr;
	tr.setIdentity();
	if (_sceneObjectID>=0)
	{
		C3DObject* it=App::ct->objCont->getObject(_sceneObjectID);
		if (it==NULL)
			_sceneObjectID=-2; // should normally never happen
		else
		{
			tr=it->getCumulativeTransformationPart1_forDisplay(true);
			if (_options&sim_banner_followparentvisibility)
			{
				if ( ((App::ct->mainSettings->activeLayers&it->layer)==0)&&((displayAttrib&sim_displayattribute_ignorelayer)==0) )
					return; // not visible
				if (it->isObjectPartOfInvisibleModel())
					return; // not visible
			}
		}
	}
	if (_sceneObjectID==-2)
		return;

	tr*=_relativeConfig;
	float* bckColor=NULL;
	if ((_options&sim_banner_nobackground)==0)
		bckColor=backColor.colors;


	if (_options&(sim_banner_clickselectsparent+sim_banner_clicktriggersevent))
		glLoadName(_objectID+NON_OBJECT_PICKING_ID_BANNER_START); // But bitmap fonts cannot be picked!
	else
		glLoadName(-1);

	if (_options&sim_banner_bitmapfont)
	{
		if (_options&sim_banner_overlay)
		{
			glDisable(GL_DEPTH_TEST);
			glDepthRange(0.0f,0.0f);
		}
		ogl::setColorsAllBlack();

		if (bckColor!=NULL)
		{
			ogl::setColor(SIM_MAX(bckColor[0],bckColor[9]),SIM_MAX(bckColor[1],bckColor[10]),SIM_MAX(bckColor[2],bckColor[11]),EMISSION_MODE);
			ogl::drawBitmapTextBackgroundIntoScene(tr.X(0),tr.X(1),tr.X(2),_label.c_str());
		}

		ogl::setColor(SIM_MAX(color.colors[0],color.colors[9]),SIM_MAX(color.colors[1],color.colors[10]),SIM_MAX(color.colors[2],color.colors[11]),EMISSION_MODE);
		ogl::drawBitmapTextIntoScene(tr.X(0),tr.X(1),tr.X(2),_label.c_str());

		glDepthRange(0.0f,1.0f);
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		if (_options&(sim_banner_fullyfacingcamera+sim_banner_facingcamera))
			ogl::drawOutlineText(_label.c_str(),tr,_height,&cameraCTM,(_options&sim_banner_overlay)!=0,(_options&sim_banner_left)!=0,(_options&sim_banner_right)!=0,(_options&sim_banner_backfaceculling)!=0,color.colors,bckColor,(_options&sim_banner_keepsamesize)!=0,windowSize[1],verticalViewSizeOrAngle,perspective,(_options&sim_banner_fullyfacingcamera)!=0);
		else
			ogl::drawOutlineText(_label.c_str(),tr,_height,NULL,(_options&sim_banner_overlay)!=0,(_options&sim_banner_left)!=0,(_options&sim_banner_right)!=0,(_options&sim_banner_backfaceculling)!=0,color.colors,bckColor,(_options&sim_banner_keepsamesize)!=0,windowSize[1],verticalViewSizeOrAngle,perspective,false);
	}

	glLoadName(-1);
}

bool CBannerObject::announceObjectWillBeErased(int objID)
{
	return(_sceneObjectID==objID);
}

