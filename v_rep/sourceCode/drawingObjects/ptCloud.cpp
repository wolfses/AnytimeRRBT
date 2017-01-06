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

#include "ptCloud.h"
#include "OGL.h"
#include "App.h"

CPtCloud::CPtCloud(int pageMask,int layerMask,int objectHandle,int options,float pointSize,int ptCnt,const float* vertices,const unsigned char* colors,const float* normals,const unsigned char* defaultColors)
{
	_pageMask=pageMask;
	_layerMask=layerMask;
	_objectHandle=objectHandle;
	_options=options;
	_pointSize=pointSize;
	_vertices.assign(vertices,vertices+ptCnt*3);
	if (defaultColors!=NULL)
	{
		for (int i=0;i<4;i++)
		{
			_defaultColors[4*i+0]=float(defaultColors[3*i+0])/255.0f;
			_defaultColors[4*i+1]=float(defaultColors[3*i+1])/255.0f;
			_defaultColors[4*i+2]=float(defaultColors[3*i+2])/255.0f;
			_defaultColors[4*i+3]=1.0f;
		}
	}
	else
	{
		_defaultColors[0]=0.5f;
		_defaultColors[1]=0.5f;
		_defaultColors[2]=0.5f;
		_defaultColors[3]=1.0f;
		_defaultColors[4]=0.25f;
		_defaultColors[5]=0.25f;
		_defaultColors[6]=0.25f;
		_defaultColors[7]=1.0f;
		_defaultColors[8]=0.25f;
		_defaultColors[9]=0.25f;
		_defaultColors[10]=0.25f;
		_defaultColors[11]=1.0f;
		_defaultColors[12]=0.0f;
		_defaultColors[13]=0.0f;
		_defaultColors[14]=0.0f;
		_defaultColors[15]=1.0f;
	}

	if (colors!=NULL)
	{
		_colors.resize(ptCnt*4);
		for (int i=0;i<ptCnt;i++)
		{
			_colors[4*i+0]=float(colors[3*i+0])/255.0f;
			_colors[4*i+1]=float(colors[3*i+1])/255.0f;
			_colors[4*i+2]=float(colors[3*i+2])/255.0f;
			_colors[4*i+3]=1.0f;
		}
	}
	if (normals!=NULL)
		_normals.assign(normals,normals+ptCnt*3);

	C3DObject* it=App::ct->objCont->getObject(_objectHandle);
	if (it!=NULL)
	{
		C7Vector tr(it->getCumulativeTransformationPart1_forDisplay(true));
		C7Vector trInv(tr.getInverse());
		for (int i=0;i<ptCnt;i++)
		{
			C3Vector v(&_vertices[3*i]);
			v=trInv*v;
			_vertices[3*i+0]=v(0);
			_vertices[3*i+1]=v(1);
			_vertices[3*i+2]=v(2);
			if (_normals.size()!=0)
			{
				C3Vector n(&_normals[3*i]);
				v=trInv.Q*v;
				_normals[3*i+0]=n(0);
				_normals[3*i+1]=n(1);
				_normals[3*i+2]=n(2);
			}
		}
	}
}

CPtCloud::~CPtCloud()
{
}

bool CPtCloud::isPersistent()
{
	return((_options&1)!=0);
}

void CPtCloud::setObjectID(int id)
{
	_id=id;
}

int CPtCloud::getObjectID()
{
	return(_id);
}

bool CPtCloud::announceObjectWillBeErased(int objectHandleAttachedTo)
{ // return value true means: destroy me!
	return(_objectHandle==objectHandleAttachedTo);
}

void CPtCloud::draw()
{
	int currentPage=App::ct->oglSurface->pageCont->getActivePageIndex();
	int p=1<<currentPage;
	if ( (_pageMask==0) || ((_pageMask&p)!=0) )
	{
		int currentLayers=App::ct->mainSettings->activeLayers;
		if ((currentLayers&_layerMask)!=0)
		{
			glPushMatrix();
			C3DObject* it=App::ct->objCont->getObject(_objectHandle);
			if (it!=NULL)
			{
				C7Vector tr=it->getCumulativeTransformationPart1_forDisplay(true);
				glTranslatef(tr.X(0),tr.X(1),tr.X(2));
				C4Vector axis=tr.Q.getAngleAndAxisNoChecking();
				glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));
			}

			// in future, use VBOs instead!
			glPointSize(_pointSize);
			glBegin(GL_POINTS);

			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,_defaultColors);
			glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,_defaultColors+12);
			glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,64);
			if (_normals.size()==0)
			{
				const float black[4]={0.0f,0.0f,0.0f,1.0f};
				glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,black);
				glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,black);
			}
			else
			{
				glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,_defaultColors+4);
				glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,_defaultColors+8);
			}

			if (_colors.size()==0)
			{
				if (_normals.size()==0)
				{
					for (int i=0;i<int(_vertices.size());i+=3)
						glVertex3fv(&_vertices[i]);
				}
				else
				{
					for (int i=0;i<int(_vertices.size());i+=3)
					{
						glNormal3fv(&_normals[i]);
						glVertex3fv(&_vertices[i]);
					}
				}
			}
			else
			{
				if (_normals.size()==0)
				{
					for (int i=0;i<int(_vertices.size()/3);i++)
					{
						glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,&_colors[4*i]);
						glVertex3fv(&_vertices[3*i]);
					}
				}
				else
				{
					for (int i=0;i<int(_vertices.size()/3);i++)
					{
						glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,&_colors[4*i]);
						glNormal3fv(&_normals[3*i]);
						glVertex3fv(&_vertices[3*i]);
					}
				}
			}

			glEnd();

			glPointSize(1.0f);

			glPopMatrix();
		}
	}
}
