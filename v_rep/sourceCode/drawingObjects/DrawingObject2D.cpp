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
#include "DrawingObject2D.h"
#include "OGL.h"
#include "Tt.h"

CDrawingObject2D::CDrawingObject2D(int theObjectType,float* vertices,float col[3],float size,float dir,char* txt)
{
	size=tt::getLimitedFloat(0.001f,10000.0f,size);
	_size=size;
	_dir=dir;
	_objectType=theObjectType;
	_col[0]=col[0];
	_col[1]=col[1];
	_col[2]=col[2];
	int t=_objectType&255;
	if ((t==sim_drawing2d_plus)||(t==sim_drawing2d_cross)||
		(t==sim_drawing2d_frame)||(t==sim_drawing2d_square)||
		(t==sim_drawing2d_arrow)||(t==sim_drawing2d_text)||
		(t==sim_drawing2d_circle)||(t==sim_drawing2d_disc))
	{
		for (int i=0;i<2;i++)
			_vertices[i]=vertices[i];
	}
	if ((t==sim_drawing2d_line)||(t==sim_drawing2d_rectangle))
	{
		for (int i=0;i<4;i++)
			_vertices[i]=vertices[i];
	}
	if (t==sim_drawing2d_triangle)
	{
		for (int i=0;i<6;i++)
			_vertices[i]=vertices[i];
	}
	if (t==sim_drawing2d_text)
	{
		if (txt!=NULL)
			_txt=txt;
	}
}

CDrawingObject2D::~CDrawingObject2D()
{
}

void CDrawingObject2D::draw(int minViewPos[2],int maxViewPos[2])
{
	float dx=float(maxViewPos[0]-minViewPos[0]);
	float dy=float(maxViewPos[1]-minViewPos[1]);
	ogl::setColorsAllBlack();
	int t=_objectType&255;
	float hs=(dx+dy)*0.25f*_size;
	if (_objectType&sim_drawing2d_pixelsize)
		hs=_size/2.0f;
	if ((t==sim_drawing2d_plus)||(t==sim_drawing2d_cross)||(t==sim_drawing2d_frame)||
		(t==sim_drawing2d_square)||(t==sim_drawing2d_arrow)||
		(t==sim_drawing2d_circle)||(t==sim_drawing2d_disc))
	{
		int startI=1;
		if (_objectType&sim_drawing2d_blackbkground)
			startI=0;
		glLineWidth(3.0f);
		int ad=0;
		for (int n=startI;n<2;n++)
		{
			if (n==1)
			{
				glLineWidth(1.0f);
				ogl::setColor(_col,AMBIENT_MODE);
				ad=0;
			}
			else
				ad=1;
			if (t==sim_drawing2d_plus)
			{
				glBegin(GL_LINES);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]-hs)-ad,minViewPos[1]+int(dy*_vertices[1]-0.0f),0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]+hs)+ad,minViewPos[1]+int(dy*_vertices[1]+0.0f),0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]-0.0f),minViewPos[1]+int(dy*_vertices[1]-hs)-ad,0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]+0.0f),minViewPos[1]+int(dy*_vertices[1]+hs)+ad,0);
				glEnd();
			}
			if (t==sim_drawing2d_cross)
			{
				glBegin(GL_LINES);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]-hs)-ad,minViewPos[1]+int(dy*_vertices[1]-hs)-ad,0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]+hs)+ad,minViewPos[1]+int(dy*_vertices[1]+hs)+ad,0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]-hs)-ad,minViewPos[1]+int(dy*_vertices[1]+hs)+ad,0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]+hs)+ad,minViewPos[1]+int(dy*_vertices[1]-hs)-ad,0);
				glEnd();
			}
			if (t==sim_drawing2d_frame)
			{
				glBegin(GL_LINES);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]-hs),minViewPos[1]+int(dy*_vertices[1]-hs),0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]+hs),minViewPos[1]+int(dy*_vertices[1]-hs),0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]+hs),minViewPos[1]+int(dy*_vertices[1]-hs),0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]+hs),minViewPos[1]+int(dy*_vertices[1]+hs),0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]+hs),minViewPos[1]+int(dy*_vertices[1]+hs),0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]-hs),minViewPos[1]+int(dy*_vertices[1]+hs),0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]-hs),minViewPos[1]+int(dy*_vertices[1]+hs),0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]-hs),minViewPos[1]+int(dy*_vertices[1]-hs),0);
				glEnd();
			}
			if (t==sim_drawing2d_square)
			{
				glBegin(GL_QUADS);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]-hs)-ad,minViewPos[1]+int(dy*_vertices[1]-hs)-ad,0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]+hs)+ad,minViewPos[1]+int(dy*_vertices[1]-hs)-ad,0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]+hs)+ad,minViewPos[1]+int(dy*_vertices[1]+hs)+ad,0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]-hs)-ad,minViewPos[1]+int(dy*_vertices[1]+hs)+ad,0);
				glEnd();
			}
			if (t==sim_drawing2d_arrow)
			{
				glBegin(GL_LINES);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]),minViewPos[1]+int(dy*_vertices[1]),0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]+hs*2.0f*cos(_dir)),minViewPos[1]+int(dy*_vertices[1]+hs*2.0f*sin(_dir)),0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]+hs*2.0f*cos(_dir)),minViewPos[1]+int(dy*_vertices[1]+hs*2.0f*sin(_dir)),0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]+hs*1.5f*cos(_dir+0.3f)),minViewPos[1]+int(dy*_vertices[1]+hs*1.5f*sin(_dir+0.3f)),0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]+hs*2.0f*cos(_dir)),minViewPos[1]+int(dy*_vertices[1]+hs*2.0f*sin(_dir)),0);
				glVertex3i(minViewPos[0]+int(dx*_vertices[0]+hs*1.5f*cos(_dir-0.3f)),minViewPos[1]+int(dy*_vertices[1]+hs*1.5f*sin(_dir-0.3f)),0);
				glEnd();
			}

			if (t==sim_drawing2d_circle)
			{
				glBegin(GL_LINE_STRIP);
				const float da=piValTimes2/16.0f;
				for (int i=0;i<17;i++)
					glVertex3i(minViewPos[0]+int(dx*_vertices[0]+hs*cos(da*float(i))+0.5f),minViewPos[1]+int(dy*_vertices[1]+hs*sin(da*float(i))+0.5f),0);
				glEnd();
			}
			if (t==sim_drawing2d_disc)
			{
				glBegin(GL_TRIANGLE_FAN);
				const float da=piValTimes2/16.0f;
				for (int i=0;i<17;i++)
					glVertex3i(minViewPos[0]+int(dx*_vertices[0]+(hs+float(ad))*cos(da*float(i))+0.5f),minViewPos[1]+int(dy*_vertices[1]+(hs+float(ad))*sin(da*float(i))+0.5f),0);
				glEnd();
			}

		}
		glLineWidth(1.0f);
	}
	if (t==sim_drawing2d_text)
	{
		int startI=1;
		if (_objectType&sim_drawing2d_blackbkground)
			startI=0;
		int ad=0;
		for (int n=startI;n<2;n++)
		{
			if (n==1)
			{
				ogl::setColor(_col,AMBIENT_MODE);
				ad=0;
			}
			else
				ad=1;
			ogl::drawTexti(minViewPos[0]+int(dx*_vertices[0])-ad,minViewPos[1]+int(dy*_vertices[1])-ad,0,_txt);
		}
	}
	if (t==sim_drawing2d_line)
	{
		int startI=1;
		if (_objectType&sim_drawing2d_blackbkground)
			startI=0;
		glLineWidth(3.0f);
		for (int n=startI;n<2;n++)
		{
			if (n==1)
			{
				glLineWidth(1.0f);
				ogl::setColor(_col,AMBIENT_MODE);
			}
			glBegin(GL_LINES);
			glVertex3i(minViewPos[0]+int(dx*_vertices[0]),minViewPos[1]+int(dy*_vertices[1]),0);
			glVertex3i(minViewPos[0]+int(dx*_vertices[2]),minViewPos[1]+int(dy*_vertices[3]),0);
			glEnd();
		}
		glLineWidth(1.0f);
	}
	if (t==sim_drawing2d_triangle)
	{
		ogl::setColor(_col,AMBIENT_MODE);
		glBegin(GL_TRIANGLES);
		glVertex3i(minViewPos[0]+int(dx*_vertices[0]),minViewPos[1]+int(dy*_vertices[1]),0);
		glVertex3i(minViewPos[0]+int(dx*_vertices[2]),minViewPos[1]+int(dy*_vertices[3]),0);
		glVertex3i(minViewPos[0]+int(dx*_vertices[4]),minViewPos[1]+int(dy*_vertices[5]),0);
		glEnd();
	}
	if (t==sim_drawing2d_rectangle)
	{
		ogl::setColor(_col,AMBIENT_MODE);
		glBegin(GL_QUADS);
		glVertex3i(minViewPos[0]+int(dx*_vertices[0]),minViewPos[1]+int(dy*_vertices[1]),0);
		glVertex3i(minViewPos[0]+int(dx*_vertices[0]),minViewPos[1]+int(dy*_vertices[3]),0);
		glVertex3i(minViewPos[0]+int(dx*_vertices[2]),minViewPos[1]+int(dy*_vertices[3]),0);
		glVertex3i(minViewPos[0]+int(dx*_vertices[2]),minViewPos[1]+int(dy*_vertices[1]),0);
		glEnd();
	}

}

