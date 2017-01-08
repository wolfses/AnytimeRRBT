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
#include "DrawingObject.h"
#include "OGL.h"
#include "IloIlo.h"
#include "App.h"
#include "Tt.h"
#include "easyLock.h"

const float SPHEREVERTICES[24*3]={
-0.4142f,-1.0000f,-0.4142f,
+0.4142f,-1.0000f,-0.4142f,
+0.4142f,-1.0000f,+0.4142f,
-0.4142f,-1.0000f,+0.4142f,

-0.4142f,-0.4142f,-1.0000f,
-0.4142f,+0.4142f,-1.0000f,
+0.4142f,+0.4142f,-1.0000f,
+0.4142f,-0.4142f,-1.0000f,

+0.4142f,+1.0000f,-0.4142f,
-0.4142f,+1.0000f,-0.4142f,
-0.4142f,+1.0000f,+0.4142f,
+0.4142f,+1.0000f,+0.4142f,

+0.4142f,-0.4142f,+1.0000f,
+0.4142f,+0.4142f,+1.0000f,
-0.4142f,+0.4142f,+1.0000f,
-0.4142f,-0.4142f,+1.0000f,

+1.0000f,-0.4142f,-0.4142f,
+1.0000f,+0.4142f,-0.4142f,
+1.0000f,+0.4142f,+0.4142f,
+1.0000f,-0.4142f,+0.4142f,

-1.0000f,-0.4142f,+0.4142f,
-1.0000f,+0.4142f,+0.4142f,
-1.0000f,+0.4142f,-0.4142f,
-1.0000f,-0.4142f,-0.4142f
};

const int SPHEREQUADINDICES[18*4]={
0,1,2,3,
4,5,6,7,
8,9,10,11,
12,13,14,15,
16,17,18,19,
20,21,22,23,

0,4,7,1,
1,16,19,2,
3,2,12,15,
0,3,20,23,

6,5,9,8,
18,17,8,11,
14,13,11,10,
10,9,22,21,

7,6,17,16,
19,18,13,12,
15,14,21,20,
23,22,5,4
};

const float SPHEREQUADNORMALS[18*3]={
+0.0f,-1.0f,+0.0f,
+0.0f,+0.0f,-1.0f,
+0.0f,+1.0f,+0.0f,
+0.0f,+0.0f,+1.0f,
+1.0f,+0.0f,+0.0f,
-1.0f,+0.0f,+0.0f,

+0.0f,-0.7071f,-0.7071f,
+0.7071f,-0.7071f,+0.0f,
+0.0f,-0.7071f,+0.7071f,
-0.7071f,-0.7071f,+0.0f,

+0.0f,+0.7071f,-0.7071f,
+0.7071f,+0.7071f,+0.0f,
+0.0f,+0.7071f,+0.7071f,
-0.7071f,+0.7071f,+0.0f,

+0.7071f,0.0f,-0.7071f,
+0.7071f,0.0f,+0.7071f,
-0.7071f,0.0f,+0.7071f,
-0.7071f,0.0f,-0.7071f
};

const int SPHERETRIANGLEINDICES[8*3]={
1,7,16,
2,19,12,
3,15,20,
0,23,4,

17,6,8,
18,11,13,
14,10,21,
22,9,5
};

const float SPHERETRIANGLENORMALS[8*3]={
+0.5773f,-0.5773f,-0.5773f,
+0.5773f,-0.5773f,+0.5773f,
-0.5773f,-0.5773f,+0.5773f,
-0.5773f,-0.5773f,-0.5773f,

+0.5773f,+0.5773f,-0.5773f,
+0.5773f,+0.5773f,+0.5773f,
-0.5773f,+0.5773f,+0.5773f,
-0.5773f,+0.5773f,-0.5773f,
};

bool CDrawingObject::getCreatedFromScript()
{
	return(_createdFromScript);
}

void CDrawingObject::setCreatedFromScript(bool c)
{
	_createdFromScript=c;
}

CDrawingObject::CDrawingObject(int theObjectType,float size,float duplicateTolerance,int sceneObjID,int maxItemCount,bool createdFromScript)
{
	_createdFromScript=createdFromScript;
	float tr=0.0f;
	if (theObjectType&sim_drawing_50percenttransparency)
		tr+=0.5f;
	if (theObjectType&sim_drawing_25percenttransparency)
		tr+=0.25f;
	if (theObjectType&sim_drawing_12percenttransparency)
		tr+=0.125f;
	color.setDefaultValues();
	if (tr!=0.0f)
	{
		color.translucid=true;
		color.transparencyFactor=1.0f-tr;
	}

	_objectID=0;
	_sceneObjectID=-1;
	size=tt::getLimitedFloat(0.0001f,100.0f,size);
	_size=size;
	maxItemCount=tt::getLimitedInt(1,10000000,maxItemCount);
	_maxItemCount=maxItemCount;
	_startItem=0;
	int tmp=theObjectType&0x001f;
	if (theObjectType&sim_drawing_vertexcolors)
	{
		if ((tmp!=sim_drawing_lines)&&(tmp!=sim_drawing_triangles))
			theObjectType-=sim_drawing_vertexcolors;
		if ((theObjectType&sim_drawing_itemcolors)&&(theObjectType&sim_drawing_vertexcolors))
			theObjectType-=sim_drawing_vertexcolors;
		if (theObjectType&sim_drawing_itemtransparency)
			theObjectType-=sim_drawing_itemtransparency;

	}
	if (theObjectType&sim_drawing_itemsizes)
	{
		if (tmp==sim_drawing_triangles)
			theObjectType-=sim_drawing_itemsizes;
	}
	if (duplicateTolerance<0.0f)
		duplicateTolerance=0.0f;
	_duplicateTolerance=duplicateTolerance;
	_objectType=theObjectType;
	_setItemSizes();

	if (sceneObjID==-1)
		_sceneObjectID=-1;
	else
	{
		C3DObject* it=App::ct->objCont->getObject(sceneObjID);
		if (it!=NULL)
			_sceneObjectID=sceneObjID;
	}
}

CDrawingObject::~CDrawingObject()
{
}

int CDrawingObject::getObjectType()
{
	return(_objectType);
}

int CDrawingObject::getSceneObjectID()
{
	return(_sceneObjectID);
}

void CDrawingObject::setObjectID(int newID)
{
	_objectID=newID;
}

int CDrawingObject::getObjectID()
{
	return(_objectID);
}

void CDrawingObject::adjustForFrameChange(const C7Vector& preCorrection)
{
	for (int i=0;i<int(_data.size())/floatsPerItem;i++)
	{
		for (int j=0;j<verticesPerItem;j++)
		{
			C3Vector v(&_data[floatsPerItem*i+j*3+0]);
			v*=preCorrection;
			v.copyTo(&_data[floatsPerItem*i+j*3+0]);
		}
		int off=verticesPerItem*3;
		for (int j=0;j<normalsPerItem;j++)
		{
			C3Vector n(&_data[floatsPerItem*i+off+j*3+0]);
			n=preCorrection.Q*n;
			n.copyTo(&_data[floatsPerItem*i+off+j*3+0]);
		}
	}
}

void CDrawingObject::adjustForScaling(float xScale,float yScale,float zScale)
{
	float avgScaling=(xScale+yScale+zScale)/3.0f;
	int tmp=_objectType&0x001f;
	if ((tmp!=sim_drawing_points)&&(tmp!=sim_drawing_lines))
		_size*=avgScaling;

	for (int i=0;i<int(_data.size())/floatsPerItem;i++)
	{
		for (int j=0;j<verticesPerItem;j++)
		{
			C3Vector v(&_data[floatsPerItem*i+j*3+0]);
			v(0)*=xScale;
			v(1)*=yScale;
			v(2)*=zScale;
			v.copyTo(&_data[floatsPerItem*i+j*3+0]);
		}
		int off=verticesPerItem*3;
		if (_objectType&sim_drawing_itemcolors)
			off+=3;
		if (_objectType&sim_drawing_vertexcolors)
			off+=3*verticesPerItem;
		if (_objectType&sim_drawing_itemsizes)
		{
			_data[floatsPerItem*i+off+0]*=avgScaling;
			off+=1;
		}
		if (_objectType&sim_drawing_itemtransparency)
			off+=1;
	}
}

void CDrawingObject::addItem(const float* itemData)
{
	EASYLOCK(_objectMutex);
	if (itemData==NULL)
	{
		_data.clear();
		_startItem=0;
		return;
	}
	int newPos=_startItem;
	if (int(_data.size())/floatsPerItem>=_maxItemCount)
	{ // the buffer is full
		if (_objectType&sim_drawing_cyclic)
		{
			_startItem++;
			if (_startItem>=_maxItemCount)
				_startItem=0;
		}
		else
			return; // saturated
	}

	C7Vector trInv;
	trInv.setIdentity();
	if (_sceneObjectID>=0)
	{
		C3DObject* it=App::ct->objCont->getObject(_sceneObjectID);
		if (it==NULL)
			_sceneObjectID=-2; // should normally never happen!
		else
			trInv=it->getCumulativeTransformationPart1().getInverse();
	}

	if ( (_duplicateTolerance>0.0f)&&(verticesPerItem==1) )
	{ // Check for duplicates
		C3Vector v(itemData);
		v*=trInv;
		for (int i=0;i<int(_data.size())/floatsPerItem;i++)
		{
			C3Vector w(&_data[floatsPerItem*i+0]);
			if ((w-v).getLength()<=_duplicateTolerance)
				return; // point already there!
		}
	}

	if (int(_data.size())/floatsPerItem<_maxItemCount)
	{ // The buffer is not yet full!
		newPos=_data.size()/floatsPerItem;
		for (int i=0;i<floatsPerItem;i++)
			_data.push_back(0.0f);
	}


	if (_sceneObjectID!=-2)
	{
		int off=0;
		for (int i=0;i<verticesPerItem;i++)
		{
			C3Vector v(itemData+off);
			v*=trInv;
			_data[newPos*floatsPerItem+off+0]=v(0);
			_data[newPos*floatsPerItem+off+1]=v(1);
			_data[newPos*floatsPerItem+off+2]=v(2);
			off+=3;
		}
		for (int i=0;i<normalsPerItem;i++)
		{
			C3Vector v(itemData+off);
			v=trInv.Q*v; // no translational part!
			_data[newPos*floatsPerItem+off+0]=v(0);
			_data[newPos*floatsPerItem+off+1]=v(1);
			_data[newPos*floatsPerItem+off+2]=v(2);
			off+=3;
		}
		for (int i=0;i<otherFloatsPerItem;i++)
			_data[newPos*floatsPerItem+off+i]=itemData[off+i];
	}
}

void CDrawingObject::_setItemSizes()
{
	verticesPerItem=0;
	normalsPerItem=0;
	otherFloatsPerItem=0;
	int tmp=_objectType&0x001f;
	if ( (tmp==sim_drawing_points)||(tmp==sim_drawing_trianglepoints)||(tmp==sim_drawing_quadpoints)||(tmp==sim_drawing_discpoints)||(tmp==sim_drawing_cubepoints)||(tmp==sim_drawing_spherepoints) )
		verticesPerItem=1;
	if (tmp==sim_drawing_lines)
		verticesPerItem=2;
	if (tmp==sim_drawing_triangles)
		verticesPerItem=3;

	if ( (tmp==sim_drawing_trianglepoints)||(tmp==sim_drawing_quadpoints)||(tmp==sim_drawing_discpoints)||(tmp==sim_drawing_cubepoints) )
	{
		if ((_objectType&sim_drawing_facingcamera)==0)
			normalsPerItem=1;
	}

	if (_objectType&sim_drawing_itemcolors)
		otherFloatsPerItem+=3;
	if (_objectType&sim_drawing_vertexcolors)
	{ 
		if (tmp==sim_drawing_lines)
			otherFloatsPerItem+=6;
		if (tmp==sim_drawing_triangles)
			otherFloatsPerItem+=9;
	}
	if (_objectType&sim_drawing_itemsizes)
		otherFloatsPerItem+=1;
	if (_objectType&sim_drawing_itemtransparency)
		otherFloatsPerItem+=1;

	floatsPerItem=3*verticesPerItem+3*normalsPerItem+otherFloatsPerItem;
}

void CDrawingObject::draw(bool overlay,bool transparentObject,int displayAttrib,const C4X4Matrix& cameraCTM)
{
	EASYLOCK(_objectMutex);
	if (_objectType&sim_drawing_overlay)
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
		if (_objectType&(sim_drawing_50percenttransparency+sim_drawing_25percenttransparency+sim_drawing_12percenttransparency+sim_drawing_itemtransparency))
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
			tr=it->getCumulativeTransformationPart1();
			if (_objectType&sim_drawing_followparentvisibility)
			{
				if ( ((App::ct->mainSettings->activeLayers&it->layer)==0)&&((displayAttrib&sim_displayattribute_ignorelayer)==0) )
					return; // not visible
				if (it->isObjectPartOfInvisibleModel())
					return; // not visible
				if ( ((_objectType&sim_drawing_painttag)==0)&&(displayAttrib&sim_displayattribute_forvisionsensor) )
					return; // not visible
			}
			else
			{
				if ( ((_objectType&sim_drawing_painttag)==0)&&(displayAttrib&sim_displayattribute_forvisionsensor) )
					return; // not visible
			}
		}
	}
	if (_sceneObjectID==-2)
		return;
	glPushMatrix();
	glPushAttrib(GL_POLYGON_BIT);
	glLoadName(-1);
	color.makeCurrentColor();
	C4X4Matrix cameraRTM(cameraCTM);
	if (_sceneObjectID>=0)
	{
		glTranslatef(tr.X(0),tr.X(1),tr.X(2));
		C4Vector axis=tr.Q.getAngleAndAxis();
		glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));	
		cameraRTM.setMultResult(tr.getInverse().getMatrix(),cameraCTM);
	}

	bool wire=( (_objectType&sim_drawing_wireframe)||(displayAttrib&sim_displayattribute_forcewireframe))&&((displayAttrib&sim_displayattribute_depthpass)==0);
	if (displayAttrib&sim_displayattribute_forbidwireframe)
		wire=false;
	if (wire)
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	if ((_objectType&sim_drawing_backfaceculling)&&(!wire))
		glEnable(GL_CULL_FACE);

	if (overlay)
		glDisable(GL_DEPTH_TEST);


	int tmp=_objectType&0x001f;
	if (tmp==sim_drawing_points)
		_drawPoints(displayAttrib,cameraRTM);
	if (tmp==sim_drawing_lines)
		_drawLines(displayAttrib,cameraRTM);
	if (tmp==sim_drawing_triangles)
		_drawTriangles(displayAttrib);
	if (tmp==sim_drawing_trianglepoints)
		_drawTrianglePoints(displayAttrib,cameraRTM);
	if (tmp==sim_drawing_quadpoints)
		_drawQuadPoints(displayAttrib,cameraRTM);
	if (tmp==sim_drawing_discpoints)
		_drawDiscPoints(displayAttrib,cameraRTM);
	if (tmp==sim_drawing_cubepoints)
		_drawCubePoints(displayAttrib,cameraRTM);
	if (tmp==sim_drawing_spherepoints)
		_drawSpherePoints(displayAttrib);

	if (overlay)
		glEnable(GL_DEPTH_TEST);


	glDisable(GL_CULL_FACE);
	glPopAttrib();
	glPopMatrix();

	if (transparentObject)
		ogl::setBlending(false); 
}


void CDrawingObject::_drawPoints(int displayAttrib,const C4X4Matrix& cameraRTM)
{
	C3Vector n(cameraRTM.M.axis[2]*-1.0f);

	// Following 2 new since introduction of sim_drawing_itemtransparency:
	if (_objectType&sim_drawing_itemtransparency)
		ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // We turn blending on!

	if (_objectType&sim_drawing_itemsizes)
	{ // This is a special routine in case we want various point sizes (change not possible between glBegin and glEnd)
		C3Vector v;
		float rgbaAmb[4]={0.0f,0.0f,0.0f,1.0f};
		int off=0;
		for (int i=0;i<int(_data.size())/floatsPerItem;i++)
		{
			int p=_startItem+i;
			if (p>=_maxItemCount)
				p-=_maxItemCount;
			if (_objectType&sim_drawing_itemcolors)
			{
				rgbaAmb[0]=_data[floatsPerItem*p+3];
				rgbaAmb[1]=_data[floatsPerItem*p+4];
				rgbaAmb[2]=_data[floatsPerItem*p+5];
				if (_objectType&sim_drawing_emissioncolor)
					glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rgbaAmb);
				else
					glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,rgbaAmb);
				off=3;
			}
			glPointSize(_data[floatsPerItem*p+3+off]);

			if (_objectType&sim_drawing_itemtransparency)
			{ // Following part new since introduction of sim_drawing_itemtransparency:
				float rgbaDiff[4]={color.colors[3],color.colors[4],color.colors[5],1.0f-_data[floatsPerItem*p+3+off+1]};
				glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,rgbaDiff);
			}

			glBegin(GL_POINTS);
			v.set(&_data[floatsPerItem*p+0]);
			glNormal3fv(n.data);
			glVertex3fv(v.data);
			glEnd();
		}
		glPointSize(1.0f);
	}
	else
	{ // fixed point size
		glPointSize(_size);
		glBegin(GL_POINTS);
		C3Vector v;
		float rgbaAmb[4]={0.0f,0.0f,0.0f,1.0f};
		int off=0;
		for (int i=0;i<int(_data.size())/floatsPerItem;i++)
		{
			int p=_startItem+i;
			if (p>=_maxItemCount)
				p-=_maxItemCount;
			v.set(&_data[floatsPerItem*p+0]);
			if (_objectType&sim_drawing_itemcolors)
			{
				rgbaAmb[0]=_data[floatsPerItem*p+3];
				rgbaAmb[1]=_data[floatsPerItem*p+4];
				rgbaAmb[2]=_data[floatsPerItem*p+5];
				if (_objectType&sim_drawing_emissioncolor)
					glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rgbaAmb);
				else
					glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,rgbaAmb);
				off=3;
			}

			if (_objectType&sim_drawing_itemtransparency)
			{ // Following part new since introduction of sim_drawing_itemtransparency:
				float rgbaDiff[4]={color.colors[3],color.colors[4],color.colors[5],1.0f-_data[floatsPerItem*p+3+off]};
				glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,rgbaDiff);
			}

			glNormal3fv(n.data);
			glVertex3fv(v.data);
		}
		glEnd();
		glPointSize(1.0f);
	}

	// Following 2 new since introduction of sim_drawing_itemtransparency:
	if (_objectType&sim_drawing_itemtransparency)
		ogl::setBlending(false); // make sure we turn blending off!
}

void CDrawingObject::_drawTrianglePoints(int displayAttrib,const C4X4Matrix& cameraRTM)
{
	// Following 2 new since introduction of sim_drawing_itemtransparency:
	if (_objectType&sim_drawing_itemtransparency)
		ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // We turn blending on!

	glBegin(GL_TRIANGLES);
	C3Vector v,n,x,y;
	float rgbaAmb[4]={0.0f,0.0f,0.0f,1.0f};
	float s=_size;
	float sxc=0.866f;
	float syc=0.5f;
	for (int i=0;i<int(_data.size())/floatsPerItem;i++)
	{
		int off=0;
		int p=_startItem+i;
		if (p>=_maxItemCount)
			p-=_maxItemCount;
		v.set(&_data[floatsPerItem*p+off]);
		off+=3;
		if (_objectType&sim_drawing_facingcamera)
			n=cameraRTM.M.axis[2]*-1.0f;
		else
		{
			n.set(&_data[floatsPerItem*p+off]);
			off+=3;
		}
		if (n(2)<0.8f)
			x=(n^C3Vector::unitZVector).getNormalized();
		else
			x=(n^C3Vector::unitXVector).getNormalized();
		y=n^x;
		if (_objectType&sim_drawing_itemcolors)
		{
			rgbaAmb[0]=_data[floatsPerItem*p+off+0];
			rgbaAmb[1]=_data[floatsPerItem*p+off+1];
			rgbaAmb[2]=_data[floatsPerItem*p+off+2];
			if (_objectType&sim_drawing_emissioncolor)
				glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rgbaAmb);
			else
				glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,rgbaAmb);
			off+=3;
		}
		if (_objectType&sim_drawing_itemsizes)
		{
			s=_data[floatsPerItem*p+off];
			off++;
		}

		if (_objectType&sim_drawing_itemtransparency)
		{ // Following part new since introduction of sim_drawing_itemtransparency:
			float rgbaDiff[4]={color.colors[3],color.colors[4],color.colors[5],1.0f-_data[floatsPerItem*p+off]};
			off++;
			glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,rgbaDiff);
		}

		glNormal3fv(n.data);
		glVertex3fv((v+(y*s)).data);
		glVertex3fv((v-(x*(s*sxc))-(y*(s*syc))).data);
		glVertex3fv((v+(x*(s*sxc))-(y*(s*syc))).data);
	}
	glEnd();

	// Following 2 new since introduction of sim_drawing_itemtransparency:
	if (_objectType&sim_drawing_itemtransparency)
		ogl::setBlending(false); // make sure we turn blending off!
}

void CDrawingObject::_drawQuadPoints(int displayAttrib,const C4X4Matrix& cameraRTM)
{
	// Following 2 new since introduction of sim_drawing_itemtransparency:
	if (_objectType&sim_drawing_itemtransparency)
		ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // We turn blending on!

	glBegin(GL_QUADS);
	C3Vector v,n,x,y;
	float rgbaAmb[4]={0.0f,0.0f,0.0f,1.0f};
	float s=_size;
	for (int i=0;i<int(_data.size())/floatsPerItem;i++)
	{
		int off=0;
		int p=_startItem+i;
		if (p>=_maxItemCount)
			p-=_maxItemCount;
		v.set(&_data[floatsPerItem*p+off]);
		off+=3;
		if (_objectType&sim_drawing_facingcamera)
			n=cameraRTM.M.axis[2]*-1.0f;
		else
		{
			n.set(&_data[floatsPerItem*p+off]);
			off+=3;
		}
		if (n(2)<0.8f)
			x=(n^C3Vector::unitZVector).getNormalized();
		else
			x=(n^C3Vector::unitXVector).getNormalized();
		y=n^x;
		if (_objectType&sim_drawing_itemcolors)
		{
			rgbaAmb[0]=_data[floatsPerItem*p+off+0];
			rgbaAmb[1]=_data[floatsPerItem*p+off+1];
			rgbaAmb[2]=_data[floatsPerItem*p+off+2];
			if (_objectType&sim_drawing_emissioncolor)
				glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rgbaAmb);
			else
				glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,rgbaAmb);
			off+=3;
		}
		if (_objectType&sim_drawing_itemsizes)
		{
			s=_data[floatsPerItem*p+off];
			off++;
		}

		if (_objectType&sim_drawing_itemtransparency)
		{ // Following part new since introduction of sim_drawing_itemtransparency:
			float rgbaDiff[4]={color.colors[3],color.colors[4],color.colors[5],1.0f-_data[floatsPerItem*p+off]};
			off++;
			glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,rgbaDiff);
		}

		glNormal3fv(n.data);
		glVertex3fv((v+(y*s)+(x*s)).data);
		glVertex3fv((v+(y*s)-(x*s)).data);
		glVertex3fv((v-(y*s)-(x*s)).data);
		glVertex3fv((v-(y*s)+(x*s)).data);
	}
	glEnd();

	// Following 2 new since introduction of sim_drawing_itemtransparency:
	if (_objectType&sim_drawing_itemtransparency)
		ogl::setBlending(false); // make sure we turn blending off!
}

void CDrawingObject::_drawDiscPoints(int displayAttrib,const C4X4Matrix& cameraRTM)
{
	// Following 2 new since introduction of sim_drawing_itemtransparency:
	if (_objectType&sim_drawing_itemtransparency)
		ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // We turn blending on!

	C3Vector v,n,x,y;
	float rgbaAmb[4]={0.0f,0.0f,0.0f,1.0f};
	float s=_size;
	float ssc=0.4142f;
	for (int i=0;i<int(_data.size())/floatsPerItem;i++)
	{
		int off=0;
		int p=_startItem+i;
		if (p>=_maxItemCount)
			p-=_maxItemCount;
		v.set(&_data[floatsPerItem*p+off]);
		off+=3;
		if (_objectType&sim_drawing_facingcamera)
			n=cameraRTM.M.axis[2]*-1.0f;
		else
		{
			n.set(&_data[floatsPerItem*p+off]);
			off+=3;
		}
		if (n(2)<0.8f)
			x=(n^C3Vector::unitZVector).getNormalized();
		else
			x=(n^C3Vector::unitXVector).getNormalized();
		y=n^x;
		if (_objectType&sim_drawing_itemcolors)
		{
			rgbaAmb[0]=_data[floatsPerItem*p+off+0];
			rgbaAmb[1]=_data[floatsPerItem*p+off+1];
			rgbaAmb[2]=_data[floatsPerItem*p+off+2];
			if (_objectType&sim_drawing_emissioncolor)
				glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rgbaAmb);
			else
				glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,rgbaAmb);
			off+=3;
		}
		if (_objectType&sim_drawing_itemsizes)
		{
			s=_data[floatsPerItem*p+off];
			off++;
		}

		if (_objectType&sim_drawing_itemtransparency)
		{ // Following part new since introduction of sim_drawing_itemtransparency:
			float rgbaDiff[4]={color.colors[3],color.colors[4],color.colors[5],1.0f-_data[floatsPerItem*p+off]};
			off++;
			glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,rgbaDiff);
		}

		glBegin(GL_TRIANGLE_FAN); // GL_POLYGON is problematic on certain graphic cards!
		glNormal3fv(n.data);
		glVertex3fv((v+(x*s)+(y*(s*ssc))).data);
		glVertex3fv((v+(x*(s*ssc))+(y*s)).data);
		glVertex3fv((v-(x*(s*ssc))+(y*s)).data);
		glVertex3fv((v-(x*s)+(y*(s*ssc))).data);
		glVertex3fv((v-(x*s)-(y*(s*ssc))).data);
		glVertex3fv((v-(x*(s*ssc))-(y*s)).data);
		glVertex3fv((v+(x*(s*ssc))-(y*s)).data);
		glVertex3fv((v+(x*s)-(y*(s*ssc))).data);
		glEnd();
	}

	// Following 2 new since introduction of sim_drawing_itemtransparency:
	if (_objectType&sim_drawing_itemtransparency)
		ogl::setBlending(false); // make sure we turn blending off!
}

void CDrawingObject::_drawCubePoints(int displayAttrib,const C4X4Matrix& cameraRTM)
{
	// Following 2 new since introduction of sim_drawing_itemtransparency:
	if (_objectType&sim_drawing_itemtransparency)
		ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // We turn blending on!

	C3Vector v,n,x,y,x0,y0,z0;
	float rgbaAmb[4]={0.0f,0.0f,0.0f,1.0f};
	float s=_size;
	for (int i=0;i<int(_data.size())/floatsPerItem;i++)
	{
		int off=0;
		int p=_startItem+i;
		if (p>=_maxItemCount)
			p-=_maxItemCount;
		v.set(&_data[floatsPerItem*p+off]);
		off+=3;
		if (_objectType&sim_drawing_facingcamera)
			n=cameraRTM.M.axis[2]*-1.0f;
		else
		{
			n.set(&_data[floatsPerItem*p+off]);
			off+=3;
		}
		if (n(2)<0.8f)
			x=(n^C3Vector::unitZVector).getNormalized();
		else
			x=(n^C3Vector::unitXVector).getNormalized();
		y=n^x;
		if (_objectType&sim_drawing_itemcolors)
		{
			rgbaAmb[0]=_data[floatsPerItem*p+off+0];
			rgbaAmb[1]=_data[floatsPerItem*p+off+1];
			rgbaAmb[2]=_data[floatsPerItem*p+off+2];
			if (_objectType&sim_drawing_emissioncolor)
				glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rgbaAmb);
			else
				glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,rgbaAmb);
			off+=3;
		}
		if (_objectType&sim_drawing_itemsizes)
		{
			s=_data[floatsPerItem*p+off];
			off++;
		}

		if (_objectType&sim_drawing_itemtransparency)
		{ // Following part new since introduction of sim_drawing_itemtransparency:
			float rgbaDiff[4]={color.colors[3],color.colors[4],color.colors[5],1.0f-_data[floatsPerItem*p+off]};
			off++;
			glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,rgbaDiff);
		}

		x0=x*s;
		y0=y*s;
		z0=n*s;
		glBegin(GL_QUADS);
		glNormal3fv(x.data);
		glVertex3fv((v+x0+y0+z0).data);
		glVertex3fv((v+x0-y0+z0).data);
		glVertex3fv((v+x0-y0-z0).data);
		glVertex3fv((v+x0+y0-z0).data);

		glNormal3fv(y.data);
		glVertex3fv((v+x0+y0+z0).data);
		glVertex3fv((v+x0+y0-z0).data);
		glVertex3fv((v-x0+y0-z0).data);
		glVertex3fv((v-x0+y0+z0).data);

		glNormal3fv((x*-1.0f).data);
		glVertex3fv((v-x0-y0+z0).data);
		glVertex3fv((v-x0+y0+z0).data);
		glVertex3fv((v-x0+y0-z0).data);
		glVertex3fv((v-x0-y0-z0).data);

		glNormal3fv((y*-1.0f).data);
		glVertex3fv((v-x0-y0+z0).data);
		glVertex3fv((v-x0-y0-z0).data);
		glVertex3fv((v+x0-y0-z0).data);
		glVertex3fv((v+x0-y0+z0).data);

		glNormal3fv(n.data);
		glVertex3fv((v+x0+y0+z0).data);
		glVertex3fv((v-x0+y0+z0).data);
		glVertex3fv((v-x0-y0+z0).data);
		glVertex3fv((v+x0-y0+z0).data);

		glNormal3fv((n*-1.0f).data);
		glVertex3fv((v-x0-y0-z0).data);
		glVertex3fv((v-x0+y0-z0).data);
		glVertex3fv((v+x0+y0-z0).data);
		glVertex3fv((v+x0-y0-z0).data);

		glEnd();
	}

	// Following 2 new since introduction of sim_drawing_itemtransparency:
	if (_objectType&sim_drawing_itemtransparency)
		ogl::setBlending(false); // make sure we turn blending off!
}

void CDrawingObject::_drawSpherePoints(int displayAttrib)
{
	// Following 2 new since introduction of sim_drawing_itemtransparency:
	if (_objectType&sim_drawing_itemtransparency)
		ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // We turn blending on!

	C3Vector v;
	float rgbaAmb[4]={0.0f,0.0f,0.0f,1.0f};
	float s=_size;
	for (int i=0;i<int(_data.size())/floatsPerItem;i++)
	{
		int off=0;
		int p=_startItem+i;
		if (p>=_maxItemCount)
			p-=_maxItemCount;
		v.set(&_data[floatsPerItem*p+0]);
		if (_objectType&sim_drawing_itemcolors)
		{
			rgbaAmb[0]=_data[floatsPerItem*p+3];
			rgbaAmb[1]=_data[floatsPerItem*p+4];
			rgbaAmb[2]=_data[floatsPerItem*p+5];
			if (_objectType&sim_drawing_emissioncolor)
				glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rgbaAmb);
			else
				glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,rgbaAmb);
			off=3;
		}
		if (_objectType&sim_drawing_itemsizes)
		{
			s=_data[floatsPerItem*p+3+off];
			off++;
		}

		if (_objectType&sim_drawing_itemtransparency)
		{ // Following part new since introduction of sim_drawing_itemtransparency:
			float rgbaDiff[4]={color.colors[3],color.colors[4],color.colors[5],1.0f-_data[floatsPerItem*p+3+off]};
			off++;
			glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,rgbaDiff);
		}

		glBegin(GL_QUADS);
		for (int j=0;j<18;j++)
		{
			C3Vector x0(SPHEREVERTICES+3*SPHEREQUADINDICES[4*j+0]);
			C3Vector x1(SPHEREVERTICES+3*SPHEREQUADINDICES[4*j+1]);
			C3Vector x2(SPHEREVERTICES+3*SPHEREQUADINDICES[4*j+2]);
			C3Vector x3(SPHEREVERTICES+3*SPHEREQUADINDICES[4*j+3]);
			x0=v+x0*s;
			x1=v+x1*s;
			x2=v+x2*s;
			x3=v+x3*s;
			glNormal3fv(SPHEREQUADNORMALS+3*j);
			glVertex3fv(x0.data);
			glVertex3fv(x1.data);
			glVertex3fv(x2.data);
			glVertex3fv(x3.data);
		}
		glEnd();
		glBegin(GL_TRIANGLES);
		for (int j=0;j<8;j++)
		{
			C3Vector x0(SPHEREVERTICES+3*SPHERETRIANGLEINDICES[3*j+0]);
			C3Vector x1(SPHEREVERTICES+3*SPHERETRIANGLEINDICES[3*j+1]);
			C3Vector x2(SPHEREVERTICES+3*SPHERETRIANGLEINDICES[3*j+2]);
			x0=v+x0*s;
			x1=v+x1*s;
			x2=v+x2*s;
			glNormal3fv(SPHERETRIANGLENORMALS+3*j);
			glVertex3fv(x0.data);
			glVertex3fv(x1.data);
			glVertex3fv(x2.data);
		}
		glEnd();
	}

	// Following 2 new since introduction of sim_drawing_itemtransparency:
	if (_objectType&sim_drawing_itemtransparency)
		ogl::setBlending(false); // make sure we turn blending off!
}

void CDrawingObject::_drawLines(int displayAttrib,const C4X4Matrix& cameraRTM)
{
	// Following 2 new since introduction of sim_drawing_itemtransparency:
	if (_objectType&sim_drawing_itemtransparency)
		ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // We turn blending on!

	C3Vector n(cameraRTM.M.axis[2]*-1.0f);

	if (_objectType&sim_drawing_itemsizes)
	{ // This is a special routine in case we want various line sizes (change not possible between glBegin and glEnd)
		C3Vector v,w;
		float rgbaAmb[4]={0.0f,0.0f,0.0f,1.0f};
		float rgbaAmb2[4]={0.0f,0.0f,0.0f,1.0f};
		int off=0;
		for (int i=0;i<int(_data.size())/floatsPerItem;i++)
		{
			int p=_startItem+i;
			if (p>=_maxItemCount)
				p-=_maxItemCount;
			if ( (_objectType&sim_drawing_itemcolors)||(_objectType&sim_drawing_vertexcolors) )
			{
				rgbaAmb[0]=_data[floatsPerItem*p+6];
				rgbaAmb[1]=_data[floatsPerItem*p+7];
				rgbaAmb[2]=_data[floatsPerItem*p+8];
				if (_objectType&sim_drawing_emissioncolor)
					glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rgbaAmb);
				else
					glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,rgbaAmb);
				off=3;
			}
			if (_objectType&sim_drawing_vertexcolors)
			{
				rgbaAmb2[0]=_data[floatsPerItem*p+6+off];
				rgbaAmb2[1]=_data[floatsPerItem*p+7+off];
				rgbaAmb2[2]=_data[floatsPerItem*p+8+off];
				off+=3;
			}
			glLineWidth(_data[floatsPerItem*p+6+off]);

			if (_objectType&sim_drawing_itemtransparency)
			{ // Following part new since introduction of sim_drawing_itemtransparency:
				float rgbaDiff[4]={color.colors[3],color.colors[4],color.colors[5],1.0f-_data[floatsPerItem*p+6+off+1]};
				glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,rgbaDiff);
			}

			glBegin(GL_LINES);
			v.set(&_data[floatsPerItem*p+0]);
			w.set(&_data[floatsPerItem*p+3]);
			glNormal3fv(n.data);
			glVertex3fv(v.data);
			if (_objectType&sim_drawing_vertexcolors)
			{
				if (_objectType&sim_drawing_emissioncolor)
					glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rgbaAmb2);
				else
					glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,rgbaAmb2);
			}
			glVertex3fv(w.data);
			glEnd();
		}
		glLineWidth(1.0f);
	}
	else
	{ // fixed point size
		glLineWidth(_size);
		glBegin(GL_LINES);
		C3Vector v,w;
		float rgbaAmb[4]={0.0f,0.0f,0.0f,1.0f};
		float rgbaAmb2[4]={0.0f,0.0f,0.0f,1.0f};
		int off=0;
		for (int i=0;i<int(_data.size())/floatsPerItem;i++)
		{
			int p=_startItem+i;
			if (p>=_maxItemCount)
				p-=_maxItemCount;
			v.set(&_data[floatsPerItem*p+0]);
			w.set(&_data[floatsPerItem*p+3]);
			if ( (_objectType&sim_drawing_itemcolors)||(_objectType&sim_drawing_vertexcolors) )
			{
				rgbaAmb[0]=_data[floatsPerItem*p+6];
				rgbaAmb[1]=_data[floatsPerItem*p+7];
				rgbaAmb[2]=_data[floatsPerItem*p+8];
				if (_objectType&sim_drawing_emissioncolor)
					glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rgbaAmb);
				else
					glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,rgbaAmb);
				off=3;
			}
			if (_objectType&sim_drawing_vertexcolors)
			{
				rgbaAmb2[0]=_data[floatsPerItem*p+6+off];
				rgbaAmb2[1]=_data[floatsPerItem*p+7+off];
				rgbaAmb2[2]=_data[floatsPerItem*p+8+off];
				off+=3;
			}

			if (_objectType&sim_drawing_itemtransparency)
			{ // Following part new since introduction of sim_drawing_itemtransparency:
				float rgbaDiff[4]={color.colors[3],color.colors[4],color.colors[5],1.0f-_data[floatsPerItem*p+6+off]};
				glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,rgbaDiff);
			}

			glNormal3fv(n.data);
			glVertex3fv(v.data);
			if (_objectType&sim_drawing_vertexcolors)
			{
				if (_objectType&sim_drawing_emissioncolor)
					glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rgbaAmb2);
				else
					glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,rgbaAmb2);
			}
			glVertex3fv(w.data);
		}
		glEnd();
		glLineWidth(1.0f);
	}

	// Following 2 new since introduction of sim_drawing_itemtransparency:
	if (_objectType&sim_drawing_itemtransparency)
		ogl::setBlending(false); // make sure we turn blending off!
}

void CDrawingObject::_drawTriangles(int displayAttrib)
{
	// Following 2 new since introduction of sim_drawing_itemtransparency:
	if (_objectType&sim_drawing_itemtransparency)
		ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // We turn blending on!

	glBegin(GL_TRIANGLES);
	C3Vector v,w,x,n;
	float rgbaAmb[4]={0.0f,0.0f,0.0f,1.0f};
	float rgbaAmb2[4]={0.0f,0.0f,0.0f,1.0f};
	float rgbaAmb3[4]={0.0f,0.0f,0.0f,1.0f};
	int off=0;
	for (int i=0;i<int(_data.size())/floatsPerItem;i++)
	{
		int p=_startItem+i;
		if (p>=_maxItemCount)
			p-=_maxItemCount;
		v.set(&_data[floatsPerItem*p+0]);
		w.set(&_data[floatsPerItem*p+3]);
		x.set(&_data[floatsPerItem*p+6]);
		if ( (_objectType&sim_drawing_itemcolors)||(_objectType&sim_drawing_vertexcolors) )
		{
			rgbaAmb[0]=_data[floatsPerItem*p+9];
			rgbaAmb[1]=_data[floatsPerItem*p+10];
			rgbaAmb[2]=_data[floatsPerItem*p+11];
			if (_objectType&sim_drawing_emissioncolor)
				glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rgbaAmb);
			else
				glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,rgbaAmb);
			off=3;
		}

		if (_objectType&sim_drawing_vertexcolors)
		{
			rgbaAmb2[0]=_data[floatsPerItem*p+9+off];
			rgbaAmb2[1]=_data[floatsPerItem*p+10+off];
			rgbaAmb2[2]=_data[floatsPerItem*p+11+off];
			off+=3;
			rgbaAmb3[0]=_data[floatsPerItem*p+9+off];
			rgbaAmb3[1]=_data[floatsPerItem*p+10+off];
			rgbaAmb3[2]=_data[floatsPerItem*p+11+off];
			off+=3;
		}

		if (_objectType&sim_drawing_itemtransparency)
		{ // Following part new since introduction of sim_drawing_itemtransparency:
			float rgbaDiff[4]={color.colors[3],color.colors[4],color.colors[5],1.0f-_data[floatsPerItem*p+9+off]};
			off++;
			glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,rgbaDiff);
		}

		n=(w-v)^(x-v);
		float l=n.getLength();
		if (l!=0.0f)
			n/=l;
		glNormal3fv(n.data);
		glVertex3fv(v.data);
		if (_objectType&sim_drawing_vertexcolors)
		{
			if (_objectType&sim_drawing_emissioncolor)
				glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rgbaAmb2);
			else
				glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,rgbaAmb2);
		}
		glVertex3fv(w.data);
		if (_objectType&sim_drawing_vertexcolors)
		{
			if (_objectType&sim_drawing_emissioncolor)
				glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rgbaAmb3);
			else
				glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,rgbaAmb3);
		}
		glVertex3fv(x.data);
	}
	glEnd();

	// Following 2 new since introduction of sim_drawing_itemtransparency:
	if (_objectType&sim_drawing_itemtransparency)
		ogl::setBlending(false); // make sure we turn blending off!
}

bool CDrawingObject::announceObjectWillBeErased(int objID)
{
	return(_sceneObjectID==objID);
}

bool CDrawingObject::canMeshBeExported()
{
	int tmp=_objectType&0x001f;
	return((tmp==sim_drawing_triangles)||(tmp==sim_drawing_trianglepoints)||(tmp==sim_drawing_quadpoints)||
		(tmp==sim_drawing_discpoints)||(tmp==sim_drawing_cubepoints)||(tmp==sim_drawing_spherepoints));
}

void CDrawingObject::getExportableMesh(std::vector<float>& vertices,std::vector<int>& indices)
{
	vertices.clear();
	indices.clear();
	C7Vector tr;
	tr.setIdentity();
	if (_sceneObjectID>=0)
	{
		C3DObject* it=App::ct->objCont->getObject(_sceneObjectID);
		if (it!=NULL)
			tr=it->getCumulativeTransformationPart1();
	}
	int tmp=_objectType&0x001f;
	if (tmp==sim_drawing_triangles)
		_exportTriangles(tr,vertices,indices);
	if (tmp==sim_drawing_trianglepoints)
		_exportTrianglePoints(tr,vertices,indices);
	if (tmp==sim_drawing_quadpoints)
		_exportQuadPoints(tr,vertices,indices);
	if (tmp==sim_drawing_discpoints)
		_exportDiscPoints(tr,vertices,indices);
	if (tmp==sim_drawing_cubepoints)
		_exportCubePoints(tr,vertices,indices);
	if (tmp==sim_drawing_spherepoints)
		_exportSpherePoints(tr,vertices,indices);
}

void CDrawingObject::_exportTrianglePoints(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices)
{
	// finish this routine (and others linked to export functionality)

}

void CDrawingObject::_exportQuadPoints(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices)
{
	// finish this routine (and others linked to export functionality)

}

void CDrawingObject::_exportDiscPoints(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices)
{
	// finish this routine (and others linked to export functionality)

}

void CDrawingObject::_exportCubePoints(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices)
{
	// finish this routine (and others linked to export functionality)

}

void CDrawingObject::_exportSpherePoints(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices)
{
	// finish this routine (and others linked to export functionality)

}

void CDrawingObject::_exportTriangles(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices)
{
	// finish this routine (and others linked to export functionality)
	C3Vector v;
	for (int i=0;i<int(_data.size())/floatsPerItem;i++)
	{
		int p=_startItem+i;
		if (p>=_maxItemCount)
			p-=_maxItemCount;
		v.set(&_data[floatsPerItem*p+0]);
		v*=tr;
		v.set(&_data[floatsPerItem*p+3]);
		v*=tr;
		v.set(&_data[floatsPerItem*p+6]);
		v*=tr;
	}
	glEnd();
}

void CDrawingObject::_exportTriOrQuad(C7Vector& tr,C3Vector* v0,C3Vector* v1,C3Vector* v2,C3Vector* v3,std::vector<float>& vertices,std::vector<int>& indices,int& nextIndex)
{
	// finish this routine (and others linked to export functionality)
	v0[0]*=tr;
	v1[0]*=tr;
	v2[0]*=tr;
	if (v3!=NULL)
		v3[0]*=tr;
}
