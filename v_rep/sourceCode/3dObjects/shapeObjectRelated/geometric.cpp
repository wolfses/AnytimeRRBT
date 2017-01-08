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
#include "geometric.h"
#include "Global.h"

#include "IloIlo.h"
#include "Tt.h"
#include "MeshRoutines.h"
#include "MeshManip.h"
#include "OGL.h"
#include "collDistInterface.h"
#include "ttUtil.h"
#include "App.h"

int CGeometric::_nextUniqueID=0;

CGeometric::CGeometric()
{
	color.setDefaultValues();
	color.setColor(0.5f+0.18f*(float)(rand()/(float)RAND_MAX),0.5f+0.18f*(float)(rand()/(float)RAND_MAX),0.5f+0.18f*(float)(rand()/(float)RAND_MAX),AMBIENT_MODE);
	edgeColor.setColorsAllBlack();
	insideColor.setDefaultValues();

	_purePrimitive=sim_pure_primitive_none;
	_purePrimitiveXSizeOrDiameter=0.0f;
	_purePrimitiveYSize=0.0f;
	_purePrimitiveZSizeOrHeight=0.0f;
	_purePrimitiveInsideScaling=0.0f; // no inside part

	_heightfieldXCount=0;
	_heightfieldYCount=0;

	_verticeLocalFrame.setIdentity();
	_textureProperty=NULL;

	_edgeWidth=1;
	_visibleEdges=false;
	_culling=false;
	_displayInverted=false;
	_wireframe=false;
	_insideAndOutsideFacesSameColor=true;
	_gouraudShadingAngle=0.5f*degToRad;
	_edgeThresholdAngle=_gouraudShadingAngle;
	_uniqueID=_nextUniqueID++;
}

CGeometric::~CGeometric()
{
	delete _textureProperty;
}

void CGeometric::display(CGeomProxy* geomData,int displayAttrib,CVisualParam* collisionColor,int dynObjFlag_forVisualization,int transparencyHandling)
{ // function has virtual/non-virtual counterpart!
	if (transparencyHandling!=0)
	{// 0=display always, 1=display transparent only, 2=display non-transparent only
		if ((transparencyHandling==1)&&(!getContainsTransparentComponents()))
			return;
		if ((transparencyHandling==2)&&(getContainsTransparentComponents()))
			return;
	}

//	if (transparencyHandling==1)
//		glDepthMask(GL_FALSE);

	glPushMatrix();
	glPushAttrib(GL_POLYGON_BIT);
	glTranslatef(_verticeLocalFrame.X(0),_verticeLocalFrame.X(1),_verticeLocalFrame.X(2));
	C4Vector axis=_verticeLocalFrame.Q.getAngleAndAxisNoChecking();
	glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));
	if (_displayInverted)
		glFrontFace(GL_CW);

	bool wire=(_wireframe||(displayAttrib&sim_displayattribute_forcewireframe))&&((displayAttrib&sim_displayattribute_depthpass)==0);
	bool meantAsWire=wire;
	if (displayAttrib&sim_displayattribute_forbidwireframe)
		wire=false;
	bool edges=_visibleEdges;
	if (displayAttrib&sim_displayattribute_forbidedges)
		edges=false;
	
	if ((!meantAsWire)||((displayAttrib&sim_displayattribute_forvisionsensor)==0)) // added on 2010/08/11 because the wireframe part of the default floor would be rendered solid
	{
		// We now display this object itself:
		bool nonPureDynamicDisplay=false;
		if (displayAttrib&sim_displayattribute_dynamiccontentonly)
		{
			const float colKin[12]={0.6f,0.6f,0.6f,0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f};
			const float colDyn[12]={0.6f,0.075f,0.075f,0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f};
			const float colPass[12]={0.6f,0.075f,0.6f,0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f};
			if (dynObjFlag_forVisualization==1)
				ogl::setColor(colKin,48,0);
			if (dynObjFlag_forVisualization==3)
				ogl::setColor(colDyn,48,0);
			if (dynObjFlag_forVisualization==2)
				ogl::setColor(colPass,48,0);
			nonPureDynamicDisplay=(_purePrimitive==sim_pure_primitive_none);
		}
		else
		{
			if (collisionColor==NULL)
				color.makeCurrentColor(false);
			else
				collisionColor->makeCurrentColor(false);
		}

		if (_culling||(!_insideAndOutsideFacesSameColor))
			glEnable(GL_CULL_FACE);
		// REMOVED ON 2010/07/29 so that we see edges over non-edges shapes (e.g. wall elements)	if (edges||wire)
		{
			glPolygonOffset(0.5f,0.0f); // Second argument set to 0.0 on 2009.01.05 (otherwise strange effects on some graphic cards)
			glEnable(GL_POLYGON_OFFSET_FILL);
		}

		if (!nonPureDynamicDisplay)
		{
			if ((!wire)||(displayAttrib&sim_displayattribute_trianglewireframe))
			{
				for (int pss=0;pss<2;pss++)
				{ // we have only a second pass if the inside color is different and no backface culling!
					if (displayAttrib&sim_displayattribute_trianglewireframe)
						glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);

					if ( (displayAttrib&sim_displayattribute_trianglewireframe)||(collisionColor!=NULL)||(displayAttrib&sim_displayattribute_dynamiccontentonly) )
						_displayTriangles(geomData->getGeomDataModificationCounter(),NULL);
					else
						_displayTriangles(geomData->getGeomDataModificationCounter(),_textureProperty);

					if (displayAttrib&sim_displayattribute_trianglewireframe)
						glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
					if (_insideAndOutsideFacesSameColor||_culling)
						break; // we leave here.. inside and outside colors are same

					if ((displayAttrib&sim_displayattribute_dynamiccontentonly)==0)
					{
						if (pss==0)
						{
							if (collisionColor==NULL)
								insideColor.makeCurrentColor(false);
							glCullFace(GL_FRONT);
						}
						else
						{ // we reset to initial state
							if (collisionColor==NULL)
								color.makeCurrentColor(false);
							glCullFace(GL_BACK);
						}
					}
				}
			}
		}
		else
		{ // we have to show that we have a non-pure shape dynamically simulated: 
			// we show the triangles edges in green (random mesh):
			// we show the triangles edges in grey (convex mesh):
			if ((displayAttrib&sim_displayattribute_trianglewireframe)==0)
			{
				glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
				_displayTriangles(geomData->getGeomDataModificationCounter(),NULL);
			}
			glDisable(GL_POLYGON_OFFSET_FILL);
			glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);

			const float colBl[12]={0.0f,1.0f,0.0f,0.0f,1.0f,0.0f,0.0f,1.0f,0.0f,0.0f,1.0f,0.0f};
			const float colBl2[12]={0.45f,0.45f,0.45f,0.3f,0.3f,0.3f,0.3f,0.3f,0.3f,0.3f,0.3f,0.3f};
			if (_convex)
				ogl::setColor(colBl2,48,0);
			else
				ogl::setColor(colBl,48,0);
			_displayTriangles(geomData->getGeomDataModificationCounter(),NULL);
		}

		glDisable(GL_POLYGON_OFFSET_FILL);

		if ( (edges||wire)&&((displayAttrib&sim_displayattribute_trianglewireframe)==0)&&(!nonPureDynamicDisplay) )
		{

			if (!wire)
			{
				if ((displayAttrib&sim_displayattribute_dynamiccontentonly)==0)
					edgeColor.makeCurrentColor(false);
				else
				{
					const float colEd[12]={0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};
					ogl::setColor(colEd,48,0);
				}
			}
			else
			{
				if ((displayAttrib&sim_displayattribute_dynamiccontentonly)==0)
				{
					ogl::setColor(0.0f,0.0f,0.0f,DIFFUSE_MODE);
					ogl::setColor(0.0f,0.0f,0.0f,SPECULAR_MODE);
				}
				else
				{
					const float colEd[12]={0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};
					ogl::setColor(colEd,48,0);
				}
			}

			if (displayAttrib&sim_displayattribute_thickEdges)
				glLineWidth(float(4*_edgeWidth));
			else
				glLineWidth(float(_edgeWidth));

			bool nothingDisplayed=true;

			if (App::userSettings->antiAliasing&&edges&&(!wire))
			{
				glEnable (GL_LINE_SMOOTH);
				glEnable (GL_BLEND);
				glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
			}

			glBegin(GL_LINES);
			for (int i=0;i<int(_indices.size())/3;i++)
			{ // for each triangle... now, all edges have specular and diffuse color components at 0!!
				int indOr[3]={3*_indices[3*i+0],3*_indices[3*i+1],3*_indices[3*i+2]};
				float* vertices0=&_vertices[indOr[0]];
				float* vertices1=&_vertices[indOr[1]];
				float* vertices2=&_vertices[indOr[2]];
				if ( ( (_edges[(3*i+0)>>3]&(1<<((3*i+0)&7)))!=0) ) // -1 means the edge was disabled
				{
					glVertex3fv(vertices0);
					glVertex3fv(vertices1);
					nothingDisplayed=false;
				}
				if ( ( (_edges[(3*i+1)>>3]&(1<<((3*i+1)&7)))!=0) ) // -1 means the edge was disabled
				{
					glVertex3fv(vertices1);
					glVertex3fv(vertices2);
					nothingDisplayed=false;
				}
				if ( ( (_edges[(3*i+2)>>3]&(1<<((3*i+2)&7)))!=0) ) // -1 means the edge was disabled
				{
					glVertex3fv(vertices2);
					glVertex3fv(vertices0);
					nothingDisplayed=false;
				}
			}
			glEnd();

			// following 2 to reset antialiasing:
			glDisable (GL_LINE_SMOOTH);
			glDisable (GL_BLEND);

			glLineWidth(1.0f);

			// Added following on 25/02/2011 (because above we disable the diffuse and specular components!)
			if (displayAttrib&sim_displayattribute_dynamiccontentonly)
			{
				const float colKin[12]={0.6f,0.6f,0.6f,0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f};
				const float colDyn[12]={0.6f,0.075f,0.075f,0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f};
				const float colPass[12]={0.6f,0.075f,0.6f,0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,0.0f,0.0f,0.0f};
				if (dynObjFlag_forVisualization==1)
					ogl::setColor(colKin,48,0);
				if (dynObjFlag_forVisualization==3)
					ogl::setColor(colDyn,48,0);
				if (dynObjFlag_forVisualization==2)
					ogl::setColor(colPass,48,0);
			}
			else
			{
				if (collisionColor==NULL)
					color.makeCurrentColor(false);
				else
					collisionColor->makeCurrentColor(false);
			}

			if (nothingDisplayed&&wire)
			{ // Special case, when no edge was displayed (e.g. a smooth sphere)
				for (int pss=0;pss<2;pss++)
				{ // we have only a second pass if the inside color is different!
					glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);
					_displayTriangles(geomData->getGeomDataModificationCounter(),NULL);
					glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
					if (_insideAndOutsideFacesSameColor||_culling)
						break; // we leave here.. inside and outside colors are same
					if ((displayAttrib&sim_displayattribute_dynamiccontentonly)==0)
					{
						if (pss==0)
						{
							if (wire&&(collisionColor==NULL))
								insideColor.makeCurrentColor(false);
							glCullFace(GL_FRONT);
						}
						else
						{ // we reset to initial state
							if (wire&&(collisionColor==NULL))
								color.makeCurrentColor(false);
							glCullFace(GL_BACK);
						}
					}
				}
			}
		}
	}
	glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);
	glPopAttrib();
	glPopMatrix();

//	if (transparencyHandling==1)
//		glDepthMask(GL_TRUE);

}

void CGeometric::display_colorCoded(CGeomProxy* geomData,int objectId)
{ // function has virtual/non-virtual counterpart!
	glPushMatrix();
	glPushAttrib(GL_POLYGON_BIT);
	glTranslatef(_verticeLocalFrame.X(0),_verticeLocalFrame.X(1),_verticeLocalFrame.X(2));
	C4Vector axis=_verticeLocalFrame.Q.getAngleAndAxisNoChecking();
	glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));
	if (_displayInverted)
		glFrontFace(GL_CW);

	// set the color-coded object ID:
	ogl::setColorsAllBlack();
	const float black[]={0.0f,0.0f,0.0f,1.0f};
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,black);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,black);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,black);
	glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
	glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,128);
	glColor3ub(objectId&255,(objectId>>8)&255,(objectId>>16)&255);


	if (_culling)
		glEnable(GL_CULL_FACE);

	_displayTriangles(geomData->getGeomDataModificationCounter(),NULL);
	glDisable(GL_CULL_FACE);

	glFrontFace(GL_CCW);
	glPopAttrib();
	glPopMatrix();
}

bool CGeometric::getNonCalculatedTextureCoordinates(std::vector<float>& texCoords)
{
	if (_textureProperty==NULL)
		return(false);
	C7Vector dummyTr;
	dummyTr.setIdentity();
	std::vector<float>* tc=_textureProperty->getTextureCoordinates(-1,dummyTr,_vertices,_indices);
	if (tc==NULL)
		return(false);
	if (!_textureProperty->getFixedCoordinates())
		return(false);
	texCoords.assign(tc->begin(),tc->end());
	return(true);
}

void CGeometric::_displayTriangles(int geomModifCounter,CTextureProperty* tp)
{
	if (!App::ct->environment->getShapeTexturesEnabled())
		tp=NULL;
	std::vector<float>* textureCoords=NULL;
	if (tp!=NULL)
	{
		textureCoords=tp->getTextureCoordinates(geomModifCounter,_verticeLocalFrame,_vertices,_indices);
		if (textureCoords==NULL)
			return; // Should normally never happen (should be caught before!)
		tp->start3DTextureDisplay();
	}
	glBegin(GL_TRIANGLES);
	if (tp==NULL)
	{
		for (int i=0;i<int(_indices.size());i++)
		{
			glNormal3fv(&_normals[3*i+0]);
			glVertex3fv(&_vertices[3*_indices[i]+0]);
		}
	}
	else
	{
		for (int i=0;i<int(_indices.size());i+=3)
		{
			glNormal3fv(&_normals[3*i+0]);
			glTexCoord2fv(&(*textureCoords)[2*i+0]);
			glVertex3fv(&_vertices[3*_indices[i+0]]);

			glNormal3fv(&_normals[3*i+3]);
			glTexCoord2fv(&(*textureCoords)[2*i+2]);
			glVertex3fv(&_vertices[3*_indices[i+1]]);

			glNormal3fv(&_normals[3*i+6]);
			glTexCoord2fv(&(*textureCoords)[2*i+4]);
			glVertex3fv(&_vertices[3*_indices[i+2]]);
		}
	}
	glEnd();
	if (tp!=NULL)
		tp->end3DTextureDisplay();
}

void CGeometric::displayForCutting(CGeomProxy* geomData,int displayAttrib,CVisualParam* collisionColor)
{ // function has virtual/non-virtual counterpart!
	C_API_FUNCTION_DEBUG;
	CCollDistInterface::lockUnlock(true); // to avoid accessing invalid data! Do not forget to unlock!!
	glPushAttrib(GL_POLYGON_BIT);

	bool wire=(_wireframe||(displayAttrib&sim_displayattribute_forcewireframe))&&((displayAttrib&sim_displayattribute_depthpass)==0);
	if (displayAttrib&sim_displayattribute_forbidwireframe)
		wire=false;
	bool edges=_visibleEdges;
	if (displayAttrib&sim_displayattribute_forbidedges)
		edges=false;
	if (collisionColor==NULL)
		color.makeCurrentColor();
	else
		collisionColor->makeCurrentColor();
	if (_culling||(!_insideAndOutsideFacesSameColor))
		glEnable(GL_CULL_FACE);
// REMOVED ON 2010/07/29 so that we see edges over non-edges shapes (e.g. wall elements)	if (edges||wire)
	{
		glPolygonOffset(0.5f,0.0f); // Second argument set to 0.0 on 2009.01.05 (otherwise strange effects on some graphic cards)
		glEnable(GL_POLYGON_OFFSET_FILL);
	}
	C3Vector v0,v1,v2;
	int ind[3];
	CVisualParam col;
	if ( (!wire)||(displayAttrib&sim_displayattribute_trianglewireframe) )
	{
		if (displayAttrib&sim_displayattribute_trianglewireframe)
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		for (int pss=0;pss<2;pss++)
		{ // we have only a second pass if the inside color is different!
			// FIRST TRIANGLES:
			glBegin(GL_TRIANGLES);
			int calcTriangleCount=CCollDistInterface::getCalculatedTriangleCount(geomData->collInfo);
			int* calcIndices=CCollDistInterface::getCalculatedTrianglesPointer(geomData->collInfo);
			float* calcVertices=CCollDistInterface::getCalculatedVerticesPointer(geomData->collInfo);
			for (int i=0;i<calcTriangleCount;i++)
			{
				ind[0]=calcIndices[3*i+0];
				if (ind[0]!=-1)
				{
					ind[1]=calcIndices[3*i+1];
					ind[2]=calcIndices[3*i+2];
					v0.set(calcVertices+3*ind[0]);
					v1.set(calcVertices+3*ind[1]);
					v2.set(calcVertices+3*ind[2]);
					v1-=v0;
					v2-=v0;
					v0=(v1^v2).getNormalized();
					glNormal3f(v0(0),v0(1),v0(2));
					glVertex3fv(calcVertices+3*ind[0]);
					glVertex3fv(calcVertices+3*ind[1]);
					glVertex3fv(calcVertices+3*ind[2]);
				}
			}
			glEnd();

			// THEN POLYGONS:
			int calcPolygonCount=CCollDistInterface::getCalculatedPolygonCount(geomData->collInfo);
			for (int i=0;i<calcPolygonCount;i++)
			{
				int polSize=CCollDistInterface::getCalculatedPolygonSize(geomData->collInfo,i);
				int* pol=CCollDistInterface::getCalculatedPolygonArrayPointer(geomData->collInfo,i);
				if (polSize!=0)
				{
					glBegin(GL_TRIANGLE_FAN); // GL_POLYGON is problematic on certain graphic cards!
					v0.set(calcVertices+3*pol[0]);
					v1.set(calcVertices+3*pol[1]);
					v2.set(calcVertices+3*pol[2]);
					v1-=v0;
					v2-=v0;
					v0=(v1^v2).getNormalized();
					glNormal3f(v0(0),v0(1),v0(2));
					for (int j=0;j<polSize-2;j++)
					{
						glVertex3fv(calcVertices+3*pol[0]);
						glVertex3fv(calcVertices+3*pol[j+1]); 
						glVertex3fv(calcVertices+3*pol[j+2]);
					}
					glEnd();
				}
			}
			if (_insideAndOutsideFacesSameColor||_culling)
				break; // we leave here.. inside and outside colors are same
			if (pss==0)
			{
				if (collisionColor==NULL)
					insideColor.makeCurrentColor();
				glCullFace(GL_FRONT);
			}
			else
			{ // we reset to initial state
				if (collisionColor==NULL)
					color.makeCurrentColor();
				glCullFace(GL_BACK);
			}
		}
		if (displayAttrib&sim_displayattribute_trianglewireframe)
			glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
	}

	glDisable(GL_POLYGON_OFFSET_FILL);

	if ( edges||(wire&&((displayAttrib&sim_displayattribute_trianglewireframe)==0)) )
	{
		if (!wire)
			edgeColor.makeCurrentColor();

		if (displayAttrib&sim_displayattribute_thickEdges)
			glLineWidth(float(4*_edgeWidth));
		else
			glLineWidth(float(_edgeWidth));
		
		if (App::userSettings->antiAliasing&&edges&&(!wire))
		{
			glEnable (GL_LINE_SMOOTH);
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
		}

		glBegin(GL_LINES);
		int calcSegmentsCount=CCollDistInterface::getCalculatedSegmentCount(geomData->collInfo);
		int* calcSegments=CCollDistInterface::getCalculatedSegmentsPointer(geomData->collInfo);
		float* calcVertices=CCollDistInterface::getCalculatedVerticesPointer(geomData->collInfo);
		for (int i=0;i<calcSegmentsCount;i++)
		{
			ind[0]=calcSegments[2*i+0];
			if (ind[0]!=-1)
			{
				ind[1]=calcSegments[2*i+1];
				v0.set(calcVertices+3*ind[0]);
				v1.set(calcVertices+3*ind[1]);
				glVertex3fv(calcVertices+3*ind[0]);
				glVertex3fv(calcVertices+3*ind[1]);
			}

		}
		glEnd();

		// following 2 to reset antialiasing:
		glDisable (GL_LINE_SMOOTH);
		glDisable (GL_BLEND);		
		
		glLineWidth(1.0f);
	}
	glDisable(GL_CULL_FACE);
	glPopAttrib();
	CCollDistInterface::lockUnlock(false);
}


void CGeometric::displayGhost(CGeomProxy* geomData,int displayAttrib,bool originalColors,bool backfaceCulling,float transparency,const float* newColors)
{ // function has virtual/non-virtual counterpart!
	if (originalColors)
	{
		display(geomData,displayAttrib,NULL,0,0);
		return;
	}

	glPushMatrix();
	glPushAttrib(GL_POLYGON_BIT);
	glTranslatef(_verticeLocalFrame.X(0),_verticeLocalFrame.X(1),_verticeLocalFrame.X(2));
	C4Vector axis=_verticeLocalFrame.Q.getAngleAndAxisNoChecking();
	glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));
	if (_displayInverted)
		glFrontFace(GL_CW);

	bool wire=(_wireframe||(displayAttrib&sim_displayattribute_forcewireframe))&&((displayAttrib&sim_displayattribute_depthpass)==0);
	bool meantAsWire=wire;
	if (displayAttrib&sim_displayattribute_forbidwireframe)
		wire=false;
	bool edges=_visibleEdges;
	if (displayAttrib&sim_displayattribute_forbidedges)
		edges=false;
	if ((!meantAsWire)||((displayAttrib&sim_displayattribute_forvisionsensor)==0)) // added on 2010/08/11 because the wireframe part of the default floor would be rendered solid
	{
		// We now display this object itself:
		ogl::setColor(newColors,color.shininess,transparency);
		if (transparency>0.01f)
			ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


		if (_culling||(!_insideAndOutsideFacesSameColor)||backfaceCulling)
			glEnable(GL_CULL_FACE);
		// REMOVED ON 2010/07/29 so that we see edges over non-edges shapes (e.g. wall elements)	if (edges||wire)
		{
			glPolygonOffset(0.5f,0.0f); // Second argument set to 0.0 on 2009.01.05 (otherwise strange effects on some graphic cards)
			glEnable(GL_POLYGON_OFFSET_FILL);
		}

		if ((!wire)||(displayAttrib&sim_displayattribute_trianglewireframe))
		{
			for (int pss=0;pss<2;pss++)
			{ // we have only a second pass if the inside color is different and no backface culling!
				if (displayAttrib&sim_displayattribute_trianglewireframe)
					glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);

				if ( (displayAttrib&sim_displayattribute_trianglewireframe)||(displayAttrib&sim_displayattribute_dynamiccontentonly) )
					_displayTriangles(geomData->getGeomDataModificationCounter(),NULL);
				else
					_displayTriangles(geomData->getGeomDataModificationCounter(),_textureProperty);

				if (displayAttrib&sim_displayattribute_trianglewireframe)
					glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
				if (_insideAndOutsideFacesSameColor||_culling)
					break; // we leave here.. inside and outside colors are same

				if ((displayAttrib&sim_displayattribute_dynamiccontentonly)==0)
				{
					ogl::setColor(newColors,color.shininess,transparency);
					if (transparency!=0.0f)
						ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
					if (pss==0)
						glCullFace(GL_FRONT);
					else
					{ // we reset to initial state
						glCullFace(GL_BACK);
					}
				}
			}
		}
		glDisable(GL_POLYGON_OFFSET_FILL);

		if ( (edges||wire)&&((displayAttrib&sim_displayattribute_trianglewireframe)==0) )
		{

			if (!wire)
			{
				edgeColor.makeCurrentColor(false);
			}
			else
			{
				ogl::setColor(0.0f,0.0f,0.0f,DIFFUSE_MODE);
				ogl::setColor(0.0f,0.0f,0.0f,SPECULAR_MODE);
			}

			if (displayAttrib&sim_displayattribute_thickEdges)
				glLineWidth(float(4*_edgeWidth));
			else
				glLineWidth(float(_edgeWidth));

			bool nothingDisplayed=true;

			if (App::userSettings->antiAliasing&&edges&&(!wire))
			{
				glEnable (GL_LINE_SMOOTH);
				glEnable (GL_BLEND);
				glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
			}

			glBegin(GL_LINES);
			for (int i=0;i<int(_indices.size())/3;i++)
			{ // for each triangle... now, all edges have specular and diffuse color components at 0!!
				int indOr[3]={3*_indices[3*i+0],3*_indices[3*i+1],3*_indices[3*i+2]};
				float* vertices0=&_vertices[indOr[0]];
				float* vertices1=&_vertices[indOr[1]];
				float* vertices2=&_vertices[indOr[2]];
				if ( ( (_edges[(3*i+0)>>3]&(1<<((3*i+0)&7)))!=0) ) // -1 means the edge was disabled
				{
					glVertex3fv(vertices0);
					glVertex3fv(vertices1);
					nothingDisplayed=false;
				}
				if ( ( (_edges[(3*i+1)>>3]&(1<<((3*i+1)&7)))!=0) ) // -1 means the edge was disabled
				{
					glVertex3fv(vertices1);
					glVertex3fv(vertices2);
					nothingDisplayed=false;
				}
				if ( ( (_edges[(3*i+2)>>3]&(1<<((3*i+2)&7)))!=0) ) // -1 means the edge was disabled
				{
					glVertex3fv(vertices2);
					glVertex3fv(vertices0);
					nothingDisplayed=false;
				}
			}
			glEnd();

			// following 2 to reset antialiasing:
			glDisable (GL_LINE_SMOOTH);
			glDisable (GL_BLEND);

			glLineWidth(1.0f);

			ogl::setColor(newColors,color.shininess,transparency);
			if (transparency>0.01f)
				ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

			if (nothingDisplayed&&wire)
			{ // Special case, when no edge was displayed (e.g. a smooth sphere)
				for (int pss=0;pss<2;pss++)
				{ // we have only a second pass if the inside color is different!
					glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);
					_displayTriangles(geomData->getGeomDataModificationCounter(),NULL);
					glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
					if (_insideAndOutsideFacesSameColor||_culling)
						break; // we leave here.. inside and outside colors are same
					if ((displayAttrib&sim_displayattribute_dynamiccontentonly)==0)
					{
						if (pss==0)
							glCullFace(GL_FRONT);
						else
						{ // we reset to initial state
							glCullFace(GL_BACK);
						}
					}
				}
			}
		}
	}
	glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);
	glPopAttrib();
	glPopMatrix();
}



void CGeometric::perform3DObjectLoadingMapping(std::vector<int>* map)
{ // function has virtual/non-virtual counterpart!
	if (_textureProperty!=NULL)
		_textureProperty->performObjectLoadingMapping(map);
}

void CGeometric::performTextureObjectLoadingMapping(std::vector<int>* map)
{ // function has virtual/non-virtual counterpart!
	if (_textureProperty!=NULL)
		_textureProperty->performTextureObjectLoadingMapping(map);
}

void CGeometric::announce3DObjectWillBeErased(int objectID)
{ // function has virtual/non-virtual counterpart!
	if (_textureProperty!=NULL)
	{
		if (_textureProperty->announceObjectWillBeErased(objectID))
		{
			delete _textureProperty;
			_textureProperty=NULL;
		}
	}
}

void CGeometric::setTextureDependencies(int shapeID)
{ // function has virtual/non-virtual counterpart!
	if (_textureProperty!=NULL)
		_textureProperty->addTextureDependencies(shapeID,_uniqueID);
}

int CGeometric::getTextureCount()
{ // function has virtual/non-virtual counterpart!
	if (_textureProperty!=NULL)
		return(1);
	return(0);
}

bool CGeometric::hasTextureThatUsesFixedTextureCoordinates()
{ // function has virtual/non-virtual counterpart!
	if (_textureProperty!=NULL)
		return(_textureProperty->getFixedCoordinates());
	return(false);
}

bool CGeometric::getContainsTransparentComponents()
{ // function has virtual/non-virtual counterpart!
	return(color.translucid||insideColor.translucid);
}

CGeometric* CGeometric::copyYourself()
{ // function has virtual/non-virtual counterpart!
	CGeometric* newIt=new CGeometric();
	copyWrapperInfos(newIt);

	color.copyYourselfInto(&newIt->color);
	insideColor.copyYourselfInto(&newIt->insideColor);
	edgeColor.copyYourselfInto(&newIt->edgeColor);

	newIt->_purePrimitive=_purePrimitive;
	newIt->_purePrimitiveXSizeOrDiameter=_purePrimitiveXSizeOrDiameter;
	newIt->_purePrimitiveYSize=_purePrimitiveYSize;
	newIt->_purePrimitiveZSizeOrHeight=_purePrimitiveZSizeOrHeight;
	newIt->_purePrimitiveInsideScaling=_purePrimitiveInsideScaling;
	newIt->_verticeLocalFrame=_verticeLocalFrame;

	newIt->_heightfieldXCount=_heightfieldXCount;
	newIt->_heightfieldYCount=_heightfieldYCount;
	newIt->_heightfieldHeights.insert(newIt->_heightfieldHeights.end(),_heightfieldHeights.begin(),_heightfieldHeights.end());

	newIt->_visibleEdges=_visibleEdges;
	newIt->_culling=_culling;
	newIt->_displayInverted=_displayInverted;
	newIt->_insideAndOutsideFacesSameColor=_insideAndOutsideFacesSameColor;
	newIt->_wireframe=_wireframe;
	newIt->_gouraudShadingAngle=_gouraudShadingAngle;
	newIt->_edgeThresholdAngle=_edgeThresholdAngle;
	newIt->_edgeWidth=_edgeWidth;

	newIt->_vertices.assign(_vertices.begin(),_vertices.end());
	newIt->_indices.assign(_indices.begin(),_indices.end());
	newIt->_normals.assign(_normals.begin(),_normals.end());
	newIt->_edges.assign(_edges.begin(),_edges.end());

	if (_textureProperty!=NULL)
		newIt->_textureProperty=_textureProperty->copyYourself();

	return(newIt);
}

void CGeometric::scale(float xVal,float yVal,float zVal)
{ // function has virtual/non-virtual counterpart!
	// Following should not really be needed (normally already done by the calling function)
	//--------------------
	if (_purePrimitive==sim_pure_primitive_plane)
		zVal=1.0f;
	if (_purePrimitive==sim_pure_primitive_disc)
	{
		zVal=1.0f;
		yVal=xVal;
	}
	if ( (_purePrimitive==sim_pure_primitive_cylinder)||(_purePrimitive==sim_pure_primitive_cone)||(_purePrimitive==sim_pure_primitive_heightfield) )
		yVal=xVal;

	scaleWrapperInfos(xVal,yVal,zVal);

	_purePrimitiveXSizeOrDiameter*=xVal;
	_purePrimitiveYSize*=yVal;
	_purePrimitiveZSizeOrHeight*=zVal;

	_verticeLocalFrame.X(0)*=xVal;
	_verticeLocalFrame.X(1)*=yVal;
	_verticeLocalFrame.X(2)*=zVal;

	C7Vector inverse(_verticeLocalFrame.getInverse());
	for (int i=0;i<int(_vertices.size())/3;i++)
	{
		C3Vector v(&_vertices[3*i+0]);
		v=_verticeLocalFrame.Q*v;
		v(0)*=xVal;
		v(1)*=yVal;
		v(2)*=zVal;
		v=inverse.Q*v;
		_vertices[3*i+0]=v(0);
		_vertices[3*i+1]=v(1);
		_vertices[3*i+2]=v(2);
	}
	
	if (_purePrimitive==sim_pure_primitive_heightfield)
	{
		for (int i=0;i<_heightfieldXCount*_heightfieldYCount;i++)
			_heightfieldHeights[i]*=zVal;
	}

	if ((_textureProperty!=NULL)&&(xVal==yVal)&&(xVal==zVal)) // we only scale texture prop if scaling is isometric, otherwise difficult to adjust geom size for texture!
		_textureProperty->scaleObject(xVal);

	if ((xVal!=yVal)||(xVal!=zVal))
		actualizeGouraudShadingAndVisibleEdges(); // we need to recompute the normals and edges
	if ((xVal<0.0f)||(yVal<0.0f)||(zVal<0.0f)) // that effectively flips faces!
		checkIfConvex();
}

void CGeometric::setMeshDataDirect(const std::vector<float>& vertices,const std::vector<int>& indices,const std::vector<float>& normals,const std::vector<unsigned char>& edges)
{
	_vertices.assign(vertices.begin(),vertices.end());
	_indices.assign(indices.begin(),indices.end());
	_normals.assign(normals.begin(),normals.end());
	_edges.assign(edges.begin(),edges.end());
	checkIfConvex();
}

void CGeometric::setMesh(const std::vector<float>& vertices,const std::vector<int>& indices,const std::vector<float>* normals,const C7Vector& transformation)
{
	_vertices.assign(vertices.begin(),vertices.end());
	_indices.assign(indices.begin(),indices.end());
	if (normals==NULL)
	{
		CMeshManip::getNormals(&_vertices,&_indices,&_normals);
		_recomputeNormals();
	}
	else
		_normals.assign(normals->begin(),normals->end());
	_verticeLocalFrame=transformation;
	_computeVisibleEdges();
	checkIfConvex();
}

void CGeometric::setPurePrimitiveType(int theType,float xOrDiameter,float y,float zOrHeight)
{ // function has virtual/non-virtual counterpart!
	_purePrimitive=theType;
	_purePrimitiveXSizeOrDiameter=xOrDiameter;
	_purePrimitiveYSize=y;
	_purePrimitiveZSizeOrHeight=zOrHeight;

	// Set the convex flag only to true!! never to false, at least not here
//	_convex=(theType!=sim_pure_primitive_heightfield)&&(theType!=sim_pure_primitive_none);
	if ((theType!=sim_pure_primitive_heightfield)&&(theType!=sim_pure_primitive_none))
		_convex=true;
	if (theType!=sim_pure_primitive_heightfield)
	{
		_heightfieldHeights.clear();
		_heightfieldXCount=0;
		_heightfieldYCount=0;
	}

	if ((theType!=sim_pure_primitive_cuboid)&&(theType!=sim_pure_primitive_spheroid)&&(theType!=sim_pure_primitive_cylinder))
		_purePrimitiveInsideScaling=0.0f; // no inside part!
}

int CGeometric::getPurePrimitiveType()
{ // function has virtual/non-virtual counterpart!
	return(_purePrimitive);
}

bool CGeometric::isGeometric()
{ // function has virtual/non-virtual counterpart!
	return(true);
}

bool CGeometric::isPure()
{ // function has virtual/non-virtual counterpart!
	return(_purePrimitive!=sim_pure_primitive_none);
}

bool CGeometric::isConvex()
{ // function has virtual/non-virtual counterpart!
	return(_convex);
}

void CGeometric::setConvex(bool convex)
{ // function has virtual/non-virtual counterpart!
	_convex=convex;

/* removed on 24/3/2013
	if (_convex)
	{ // convex shape handling in Bullet includes a very large margin. We can:
		// 1. shrink the dynamic model using _bulletAutoShrinkConvexMesh. This adds some initial preprocessing time, can lead to crashes, and edges and points appear shifted inwards. Faces react correctly.
		// 2. reduce the margin (what we do here). Erwin from Bullet doesn't recommend it (doesn't say why), but I got the best (still not good!!) results with it
		_bulletNonDefaultCollisionMarginFactor=0.002f;
		_bulletNonDefaultCollisionMargin=true;
	}
	else
		_bulletNonDefaultCollisionMargin=false;
		*/
}

void CGeometric::getCumulativeMeshes(std::vector<float>& vertices,std::vector<int>* indices,std::vector<float>* normals)
{ // function has virtual/non-virtual counterpart!
	int offset=vertices.size()/3;
	for (int i=0;i<int(_vertices.size())/3;i++)
	{
		C3Vector v(&_vertices[3*i]);
		v*=_verticeLocalFrame;
		vertices.push_back(v(0));
		vertices.push_back(v(1));
		vertices.push_back(v(2));
	}
	if (indices!=NULL)
	{
		for (int i=0;i<int(_indices.size());i++)
			indices->push_back(_indices[i]+offset);
	}
	if (normals!=NULL)
	{
		C4Vector rot(_verticeLocalFrame.Q);
		for (int i=0;i<int(_normals.size())/3;i++)
		{
			C3Vector v(&_normals[3*i]);
			v=rot*v;
			normals->push_back(v(0));
			normals->push_back(v(1));
			normals->push_back(v(2));
		}
	}
}

void CGeometric::setColor(const char* colorName,int colorComponent,const float* rgbData)
{ // function has virtual/non-virtual counterpart!
	if ( (colorName==NULL)||(color.colorName.compare(colorName)==0) )
	{
		if (colorComponent<4)
		{ // regular components
			for (int i=0;i<3;i++)
				color.colors[colorComponent*3+i]=rgbData[i];
		}
		if (colorComponent==4)
		{
			color.transparencyFactor=rgbData[0];
			color.translucid=(rgbData[0]<1.0f);
		}
	}
	if ((colorName!=NULL)&&(insideColor.colorName.compare(colorName)==0))
	{
		if (colorComponent<4)
		{ // regular components
			for (int i=0;i<3;i++)
				insideColor.colors[colorComponent*3+i]=rgbData[i];
		}
		if (colorComponent==4)
		{
			insideColor.transparencyFactor=rgbData[0];
			insideColor.translucid=(rgbData[0]<1.0f);
		}
	}
	if ((colorName!=NULL)&&(edgeColor.colorName.compare(colorName)==0))
	{
		if (colorComponent<4)
		{ // regular components
			for (int i=0;i<3;i++)
				edgeColor.colors[colorComponent*3+i]=rgbData[i];
		}
	}
}

bool CGeometric::getColor(const char* colorName,int colorComponent,float* rgbData)
{ // function has virtual/non-virtual counterpart!
	if ( (colorName==NULL)||(color.colorName.compare(colorName)==0) )
	{
		if (colorComponent<4)
		{ // regular components
			for (int i=0;i<3;i++)
				rgbData[i]=color.colors[colorComponent*3+i];
			return(true);
		}
		if (colorComponent==4)
		{
			rgbData[0]=color.transparencyFactor;
			return(true);
		}
		return(false);
	}
	if ((colorName!=NULL)&&(insideColor.colorName.compare(colorName)==0))
	{
		if (colorComponent<4)
		{ // regular components
			for (int i=0;i<3;i++)
				rgbData[i]=insideColor.colors[colorComponent*3+i];
			return(true);
		}
		if (colorComponent==4)
		{
			rgbData[0]=insideColor.transparencyFactor;
			return(true);
		}
		return(false);
	}
	if ((colorName!=NULL)&&(edgeColor.colorName.compare(colorName)==0))
	{
		if (colorComponent<4)
		{ // regular components
			for (int i=0;i<3;i++)
				rgbData[i]=edgeColor.colors[colorComponent*3+i];
		}
		return(false);
	}
	return(false);
}

void CGeometric::getAllShapeComponentsCumulative(std::vector<CGeometric*>& shapeComponentList)
{	// function has virtual/non-virtual counterpart!
	// needed by the dynamics routine. We return ALL shape components!
	shapeComponentList.push_back(this);
}

CGeometric* CGeometric::getShapeComponentAtIndex(int& index)
{ // function has virtual/non-virtual counterpart!
	if (index<0)
		return(NULL);
	if (index==0)
		return(this);
	index--;
	return(NULL);
}

int CGeometric::getUniqueID()
{
	return(_uniqueID);
}

void CGeometric::setHeightfieldData(const std::vector<float>& heights,int xCount,int yCount)
{
	_heightfieldHeights.clear();
	_heightfieldHeights.insert(_heightfieldHeights.end(),heights.begin(),heights.end());
	_heightfieldXCount=xCount;
	_heightfieldYCount=yCount;
}

float* CGeometric::getHeightfieldData(int& xCount,int& yCount,float& minHeight,float& maxHeight)
{
	if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_bullet)
		setHeightfieldDiamonds(0);
	if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_ode)
		setHeightfieldDiamonds(1);
	if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_vortex)
		setHeightfieldDiamonds(0);
	if ( (_purePrimitive!=sim_pure_primitive_heightfield)||(_heightfieldHeights.size()==0) )
		return(NULL);
	xCount=_heightfieldXCount;
	yCount=_heightfieldYCount;
	minHeight=+99999999.0f;
	maxHeight=-99999999.0f;
	for (int i=0;i<int(_heightfieldHeights.size());i++)
	{
		if (_heightfieldHeights[i]<minHeight)
			minHeight=_heightfieldHeights[i];
		if (_heightfieldHeights[i]>maxHeight)
			maxHeight=_heightfieldHeights[i];
	}
	return(&_heightfieldHeights[0]);
}

void CGeometric::setHeightfieldDiamonds(int d)
{ 
	if (_purePrimitive==sim_pure_primitive_heightfield)
	{
		for (int i=0;i<int(_indices.size())/6;i++)
		{
			if (d==0)
			{
				_indices[6*i+1]=_indices[6*i+3];
				_indices[6*i+5]=_indices[6*i+2];
			}
			if (d==1)
			{
				_indices[6*i+1]=_indices[6*i+4];
				_indices[6*i+5]=_indices[6*i+0];
			}
		}
	}
}

void CGeometric::getPurePrimitiveSizes(C3Vector& s)
{
	s(0)=_purePrimitiveXSizeOrDiameter;
	s(1)=_purePrimitiveYSize;
	s(2)=_purePrimitiveZSizeOrHeight;
}

void CGeometric::setPurePrimitiveInsideScaling(float s)
{
	_purePrimitiveInsideScaling=tt::getLimitedFloat(0.0f,0.99999f,s);
}

float CGeometric::getPurePrimitiveInsideScaling()
{
	return(_purePrimitiveInsideScaling);
}

CTextureProperty* CGeometric::getTextureProperty()
{
	return(_textureProperty);
}

void CGeometric::setTextureProperty(CTextureProperty* tp)
{ // careful, this doesn't check if a _textureProperty already exists! Has to be done and destroyed outside!
	_textureProperty=tp;
}

void CGeometric::setInsideAndOutsideFacesSameColor(bool s)
{
	_insideAndOutsideFacesSameColor=s;
}
bool CGeometric::getInsideAndOutsideFacesSameColor()
{
	return(_insideAndOutsideFacesSameColor);
}

bool CGeometric::getVisibleEdges()
{
	return(_visibleEdges);
}

void CGeometric::setVisibleEdges(bool v)
{
	_visibleEdges=v;
}

int CGeometric::getEdgeWidth()
{
	return(_edgeWidth);
}

void CGeometric::setEdgeWidth(int w)
{
	w=tt::getLimitedInt(1,4,w);
	_edgeWidth=w;
}

bool CGeometric::getCulling()
{
	return(_culling);
}

void CGeometric::setCulling(bool c)
{
	_culling=c;
}

bool CGeometric::getDisplayInverted()
{
	return(_displayInverted);
}

void CGeometric::setDisplayInverted(bool di)
{
	_displayInverted=di;
}

void CGeometric::copyVisualAttributesTo(CGeometric* target)
{
	color.copyYourselfInto(&target->color);
	insideColor.copyYourselfInto(&target->insideColor);
	edgeColor.copyYourselfInto(&target->edgeColor);

	target->_visibleEdges=_visibleEdges;
	target->_culling=_culling;
	target->_displayInverted=_displayInverted;
	target->_insideAndOutsideFacesSameColor=_insideAndOutsideFacesSameColor;
	target->_wireframe=_wireframe;
	target->_edgeWidth=_edgeWidth;
	target->_gouraudShadingAngle=_gouraudShadingAngle;
	target->_edgeThresholdAngle=_edgeThresholdAngle;
}

float CGeometric::getGouraudShadingAngle()
{ // function has virtual/non-virtual counterpart!
	return(_gouraudShadingAngle);
}

void CGeometric::setGouraudShadingAngle(float angle)
{ // function has virtual/non-virtual counterpart!
	tt::limitValue(0.0f,89.0f*degToRad,angle);
	if (_gouraudShadingAngle!=angle)
	{
		_gouraudShadingAngle=angle;
		_recomputeNormals();
	}
}

float CGeometric::getEdgeThresholdAngle()
{ // function has virtual/non-virtual counterpart!
	return(_edgeThresholdAngle);
}

void CGeometric::setEdgeThresholdAngle(float angle)
{ // function has virtual/non-virtual counterpart!
	tt::limitValue(0.0f,89.0f*degToRad,angle);
	if (_edgeThresholdAngle!=angle)
	{
		_edgeThresholdAngle=angle;
		_computeVisibleEdges();
	}
}

void CGeometric::setWireframe(bool w)
{
	_wireframe=w;
}

bool CGeometric::getWireframe()
{
	return(_wireframe);
}

C7Vector CGeometric::getVerticeLocalFrame()
{
	return(_verticeLocalFrame);
}

void CGeometric::setVerticeLocalFrame(const C7Vector& tr)
{
	_verticeLocalFrame=tr;
}

void CGeometric::preMultiplyAllVerticeLocalFrames(const C7Vector& preTr)
{ // function has virtual/non-virtual counterpart!

	_transformationsSinceGrouping=preTr*_transformationsSinceGrouping;
	_localInertiaFrame=preTr*_localInertiaFrame;

	_verticeLocalFrame=preTr*_verticeLocalFrame;

	if (_textureProperty!=NULL)
		_textureProperty->adjustForFrameChange(preTr);
}

void CGeometric::removeAllTextures()
{ // function has virtual/non-virtual counterpart!
	delete _textureProperty;
	_textureProperty=NULL;
}

void CGeometric::flipFaces()
{ // function has virtual/non-virtual counterpart!
	int save;
	float normSave;
	for (int i=0;i<int(_indices.size())/3;i++)
	{
		save=_indices[3*i+0];
		_indices[3*i+0]=_indices[3*i+2];
		_indices[3*i+2]=save;

		normSave=-_normals[3*(3*i+0)+0];
		_normals[3*(3*i+0)+0]=-_normals[3*(3*i+2)+0];
		_normals[3*(3*i+1)+0]*=-1.0f;
		_normals[3*(3*i+2)+0]=normSave;

		normSave=-_normals[3*(3*i+0)+1];
		_normals[3*(3*i+0)+1]=-_normals[3*(3*i+2)+1];
		_normals[3*(3*i+1)+1]*=-1.0f;
		_normals[3*(3*i+2)+1]=normSave;

		normSave=-_normals[3*(3*i+0)+2];
		_normals[3*(3*i+0)+2]=-_normals[3*(3*i+2)+2];
		_normals[3*(3*i+1)+2]*=-1.0f;
		_normals[3*(3*i+2)+2]=normSave;  
	}
	_computeVisibleEdges();
	checkIfConvex();
}

void CGeometric::actualizeGouraudShadingAndVisibleEdges()
{
	_recomputeNormals();
	_computeVisibleEdges();
}

void CGeometric::_recomputeNormals()
{
	float maxAngle=_gouraudShadingAngle;
	C3Vector v[3];
	for (int i=0;i<int(_indices.size())/3;i++)
	{	// Here we restore first all the normal vectors
		v[0]=C3Vector(&_vertices[3*(_indices[3*i+0])]);
		v[1]=C3Vector(&_vertices[3*(_indices[3*i+1])]);
		v[2]=C3Vector(&_vertices[3*(_indices[3*i+2])]);

		C3Vector v1(v[1]-v[0]);
		C3Vector v2(v[2]-v[0]);
		C3Vector n((v1^v2).getNormalized());

		_normals[9*i+0]=n(0);
		_normals[9*i+1]=n(1);
		_normals[9*i+2]=n(2);
		_normals[9*i+3]=n(0);
		_normals[9*i+4]=n(1);
		_normals[9*i+5]=n(2);
		_normals[9*i+6]=n(0);
		_normals[9*i+7]=n(1);
		_normals[9*i+8]=n(2);
	}

	std::vector<std::vector<int>*> indexToNormals;
	for (int i=0;i<int(_vertices.size())/3;i++)
	{
		std::vector<int>* sharingNormals=new std::vector<int>;
		indexToNormals.push_back(sharingNormals);
	}
	for (int i=0;i<int(_indices.size())/3;i++)
	{
		indexToNormals[_indices[3*i+0]]->push_back(3*i+0);
		indexToNormals[_indices[3*i+1]]->push_back(3*i+1);
		indexToNormals[_indices[3*i+2]]->push_back(3*i+2);
	}
	std::vector<float> changedNorm(_normals.size());

	for (int i=0;i<int(indexToNormals.size());i++)
	{
		for (int j=0;j<int(indexToNormals[i]->size());j++)
		{
			C3Vector totN;
			float nb=1.0f;
			C3Vector nActual;
			nActual.set(&_normals[3*(indexToNormals[i]->at(j))]);
			totN=nActual;
			for (int k=0;k<int(indexToNormals[i]->size());k++)
			{
				if (j!=k)
				{
					C3Vector nToCompare(&_normals[3*(indexToNormals[i]->at(k))]);
					if (nActual.getAngle(nToCompare)<maxAngle)
					{
						totN+=nToCompare;
						nb=nb+1.0f;
					}
				}
			}
			changedNorm[3*indexToNormals[i]->at(j)+0]=totN(0)/nb;
			changedNorm[3*indexToNormals[i]->at(j)+1]=totN(1)/nb;
			changedNorm[3*indexToNormals[i]->at(j)+2]=totN(2)/nb;
		}
		indexToNormals[i]->clear();
		delete indexToNormals[i];
	}
	// Now we have to replace the modified normals:
	for (int i=0;i<int(_indices.size())/3;i++)
	{
		for (int j=0;j<9;j++)
			_normals[9*i+j]=changedNorm[9*i+j];
	}
}

void CGeometric::_computeVisibleEdges()
{
	if (_indices.size()==0)
		return;
	float softAngle=_edgeThresholdAngle;
	_edges.clear();
	std::vector<int> eIDs;
	CMeshRoutines::getEdgeFeatures(&_vertices[0],_vertices.size(),&_indices[0],_indices.size(),NULL,&eIDs,NULL,softAngle,true);
	_edges.assign((_indices.size()/8)+1,0);
	std::vector<bool> usedEdges(_indices.size(),false);
	for (int i=0;i<int(eIDs.size());i++)
	{
		if (eIDs[i]!=-1)
		{
			_edges[i>>3]|=(1<<(i&7));
			usedEdges[eIDs[i]]=true;
		}
	}
}


bool CGeometric::checkIfConvex()
{ // function has virtual/non-virtual counterpart!
	_convex=CMeshRoutines::checkIfConvex(_vertices,_indices,0.015f); // 1.5% tolerance of the average bounding box side length
	setConvex(_convex);
	return(_convex);
}


void CGeometric::_savePackedIntegers(CSer& ar,const std::vector<int>& data)
{
	ar << int(data.size());
	int prevInd=0;
	for (int i=0;i<int(data.size());i++)
	{
		int currInd=data[i];
		int diff=currInd-prevInd;
		if (abs(diff)<0x00000020) // 32
		{ // this index takes only 1 byte storage!
			ar << (unsigned char)(diff+31); // we use the two msb to indicate the byte count (1 --> 0)
		}
		else
		{ // this index takes more than 1 byte storage!
			if (abs(diff)<0x00002000) // 8192
			{ // this index takes 2 byte storage!
				diff+=8191;
				ar << (unsigned char)((diff&0x0000003f)|0x00000040); // we use the two msb to indicate the byte count (2 --> 1)
				ar << (unsigned char)(diff>>6);
			}
			else
			{ // this index takes more than 2 byte storage!
				if (abs(diff)<0x00200000) // 2097152
				{ // this index takes 3 byte storage!
					diff+=2097151;
					ar << (unsigned char)((diff&0x0000003f)|0x00000080); // we use the two msb to indicate the byte count (3 --> 2)
					ar << (unsigned char)(diff>>6);
					ar << (unsigned char)(diff>>14);
				}
				else
				{ // this index takes 4 byte storage!
					diff+=536870911;
					ar << (unsigned char)((diff&0x0000003f)|0x000000c0); // we use the two msb to indicate the byte count (4 --> 3)
					ar << (unsigned char)(diff>>6);
					ar << (unsigned char)(diff>>14);
					ar << (unsigned char)(diff>>22);
				}
			}
		}
		prevInd=currInd;
	}
}

void CGeometric::_loadPackedIntegers(CSer& ar,std::vector<int>& data)
{
	int dataLength;
	ar >> dataLength;
	data.clear();
	unsigned char b0,b1,b2,b3,storageB;
	int prevInd=0;
	for (int i=0;i<dataLength;i++)
	{
		b1=0;
		b2=0;
		b3=0;
		ar >> b0;
		storageB=((b0&0xc0)>>6); 
		b0&=0x3f; // we remove the storage byte info
		if (storageB>=1)
			ar >> b1; // this index takes 2 or more storage bytes
		if (storageB>=2)
			ar >> b2; // this index takes 3 or more storage bytes
		if (storageB>=3)
			ar >> b3; // this index takes 4 storage bytes
		int diff=b0+(b1<<6)+(b2<<14)+(b3<<22);
		if (storageB==0)
			diff-=31;
		if (storageB==1)
			diff-=8191;
		if (storageB==2)
			diff-=2097151;
		if (storageB==3)
			diff-=536870911;
		int currInd=prevInd+diff;
		data.push_back(currInd);
		prevInd=currInd;
	}
}

void CGeometric::serialize(CSer& ar)
{ // function has virtual/non-virtual counterpart!
	serializeWrapperInfos(ar);
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Cl0");
		ar.setCountingMode();
		color.serialize(ar);
		if (ar.setWritingMode())
			color.serialize(ar);

		ar.storeDataName("Cl2");
		ar.setCountingMode();
		insideColor.serialize(ar);
		if (ar.setWritingMode())
			insideColor.serialize(ar);

		ar.storeDataName("Ecl");
		ar.setCountingMode();
		edgeColor.serialize(ar);
		if (ar.setWritingMode())
			edgeColor.serialize(ar);

		if (App::ct->undoBufferContainer->isUndoSavingOrRestoringUnderWay())
		{ // undo/redo serialization:
			ar.storeDataName("Ver");
			ar << App::ct->undoBufferContainer->undoBufferArrays.addVertexBuffer(_vertices,App::ct->undoBufferContainer->getNextBufferId());
			ar.flush();

			ar.storeDataName("Ind");
			ar << App::ct->undoBufferContainer->undoBufferArrays.addIndexBuffer(_indices,App::ct->undoBufferContainer->getNextBufferId());
			ar.flush();

			ar.storeDataName("Nor");
			ar << App::ct->undoBufferContainer->undoBufferArrays.addNormalsBuffer(_normals,App::ct->undoBufferContainer->getNextBufferId());
			ar.flush();
		}
		else
		{ // normal serialization:
			ar.storeDataName("Ver");
			if (_vertices.size()!=0)
			{
				std::vector<unsigned char>* serBuffer=ar.getBufferPointer();
				unsigned char* ptr = reinterpret_cast<unsigned char*>(&_vertices[0]);
				serBuffer->insert(serBuffer->end(),ptr,ptr+_vertices.size()* sizeof(float));
			}
			ar.flush();

			if (App::userSettings->packIndices)
			{ // to save storage space
				ar.storeDataName("In2");
				_savePackedIntegers(ar,_indices);
				ar.flush();
			}
			else
			{
				ar.storeDataName("Ind");
				for (int i=0;i<int(_indices.size());i++)
					ar << _indices[i];
				ar.flush();
			}

			if (App::userSettings->saveApproxNormals)
			{
				ar.storeDataName("No2");
				for (int i=0;i<int(_normals.size())/3;i++)
				{
					C3Vector n(&_normals[3*i]);
					n*=15.0f;
					n(0)+=0.5f*n(0)/fabsf(n(0));
					n(1)+=0.5f*n(1)/fabsf(n(1));
					n(2)+=0.5f*n(2)/fabsf(n(2));
					char x=char(n(0));
					char y=char(n(1));
					char z=char(n(2));
					WORD w=(x+15)+((y+15)<<5)+((z+15)<<10);
					ar << w;
				}
				ar.flush();
			}
			else
			{
				ar.storeDataName("Nor");
				for (int i=0;i<int(_normals.size());i++)
					ar << _normals[i];
				ar.flush();
			}
		}

		ar.storeDataName("Ved"); // Recomputing edges at load takes too much time
		if (_edges.size()!=0)
		{
			std::vector<unsigned char>* serBuffer=ar.getBufferPointer();
			unsigned char* ptr = &_edges[0];
			serBuffer->insert(serBuffer->end(),ptr,ptr+_edges.size());
		}
		ar.flush();

		ar.storeDataName("Ppr");
		ar << _purePrimitive << _purePrimitiveXSizeOrDiameter << _purePrimitiveYSize << _purePrimitiveZSizeOrHeight;
		ar.flush();

		ar.storeDataName("Pp2");
		ar << _purePrimitiveInsideScaling;
		ar.flush();

		ar.storeDataName("Ppf");
		ar << _verticeLocalFrame(0) << _verticeLocalFrame(1) << _verticeLocalFrame(2) << _verticeLocalFrame(3);
		ar << _verticeLocalFrame(4) << _verticeLocalFrame(5) << _verticeLocalFrame(6);
		ar.flush();

		ar.storeDataName("Gsa"); // write this always before Gs2
		ar << _gouraudShadingAngle << _edgeWidth;
		ar.flush();

		ar.storeDataName("Gs2");
		ar << _edgeThresholdAngle;
		ar.flush();

		ar.storeDataName("Var");
		BYTE nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,_visibleEdges);
		SIM_SET_CLEAR_BIT(nothing,1,_culling);
		SIM_SET_CLEAR_BIT(nothing,2,_wireframe);
		SIM_SET_CLEAR_BIT(nothing,3,_insideAndOutsideFacesSameColor);
		// RESERVED... DO NOT USE  // SIM_SET_CLEAR_BIT(nothing,4,true); // means: we do not have to make the convectivity test for this shape (was already done). Added this on 16/1/2013
		SIM_SET_CLEAR_BIT(nothing,5,true); // means: we do not have to make the convectivity test for this shape (was already done). Added this on 28/1/2013
		SIM_SET_CLEAR_BIT(nothing,6,_displayInverted);
		ar << nothing;
		ar.flush();

		if (_textureProperty!=NULL)
		{
			ar.storeDataName("Toj");
			ar.setCountingMode();
			_textureProperty->serialize(ar);
			if (ar.setWritingMode())
				_textureProperty->serialize(ar);
		}

		ar.storeDataName("Hfd"); // Has to come after PURE TYPE!
		ar << _heightfieldXCount << _heightfieldYCount;
		for (int i=0;i<int(_heightfieldHeights.size());i++)
			ar << _heightfieldHeights[i];
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
				if (theName.compare("Cl0")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					color.serialize(ar);
				}
				if (theName.compare("Cl2")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					insideColor.serialize(ar);
				}
				if (theName.compare("Ecl")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					edgeColor.serialize(ar);
				}
				if (App::ct->undoBufferContainer->isUndoSavingOrRestoringUnderWay())
				{ // undo/redo serialization
					if (theName.compare("Ver")==0)
					{
						noHit=false;
						ar >> byteQuantity;
						int id;
						ar >> id;
						App::ct->undoBufferContainer->undoBufferArrays.getVertexBuffer(id,_vertices);
					}
					if (theName.compare("Ind")==0)
					{
						noHit=false;
						ar >> byteQuantity;
						int id;
						ar >> id;
						App::ct->undoBufferContainer->undoBufferArrays.getIndexBuffer(id,_indices);
					}
					if (theName.compare("Nor")==0)
					{
						noHit=false;
						ar >> byteQuantity;
						int id;
						ar >> id;
						App::ct->undoBufferContainer->undoBufferArrays.getNormalsBuffer(id,_normals);
					}
				}
				else
				{ // normal serialization
					if (theName.compare("Ver")==0)
					{
						noHit=false;
						ar >> byteQuantity;
						_vertices.resize(byteQuantity/sizeof(float),0.0f);
						for (int i=0;i<int(_vertices.size());i++)
							ar >> _vertices[i];
					}
					if (theName.compare("Ind")==0)
					{
						noHit=false;
						ar >> byteQuantity;
						_indices.resize(byteQuantity/sizeof(int),0);
						for (int i=0;i<int(_indices.size());i++)
							ar >> _indices[i];
					}
					if (theName.compare("Nor")==0)
					{
						noHit=false;
						ar >> byteQuantity;
						_normals.resize(byteQuantity/sizeof(float),0.0f);
						for (int i=0;i<int(_normals.size());i++)
							ar >> _normals[i];
					}
				}

				if (theName.compare("In2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					_loadPackedIntegers(ar,_indices);
				}
				if (theName.compare("No2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					_normals.resize(byteQuantity*6/sizeof(float),0.0f);
					for (int i=0;i<byteQuantity/2;i++)
					{
						WORD w;
						ar >> w;
						char x=(w&0x001f)-15;
						char y=((w>>5)&0x001f)-15;
						char z=((w>>10)&0x001f)-15;
						C3Vector n((float)x,(float)y,(float)z);
						n.normalize();
						_normals[3*i+0]=n(0);
						_normals[3*i+1]=n(1);
						_normals[3*i+2]=n(2);
					}
				}
				if (theName.compare("Ved")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					_edges.resize(byteQuantity,0);
					for (int i=0;i<byteQuantity;i++)
						ar >> _edges[i];
				}
				if (theName.compare("Ppr")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _purePrimitive >> _purePrimitiveXSizeOrDiameter >> _purePrimitiveYSize >> _purePrimitiveZSizeOrHeight;
				}
				if (theName.compare("Pp2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _purePrimitiveInsideScaling;
				}
				if (theName.compare("Ppf")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _verticeLocalFrame(0) >> _verticeLocalFrame(1) >> _verticeLocalFrame(2) >> _verticeLocalFrame(3);
					ar >> _verticeLocalFrame(4) >> _verticeLocalFrame(5) >> _verticeLocalFrame(6);
				}
				if (theName.compare("Gsa")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _gouraudShadingAngle >> _edgeWidth;
					_edgeThresholdAngle=_gouraudShadingAngle;
				}
				if (theName.compare("Gs2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _edgeThresholdAngle;
				}
				if (theName=="Var")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_visibleEdges=SIM_IS_BIT_SET(nothing,0);
					_culling=SIM_IS_BIT_SET(nothing,1);
					_wireframe=SIM_IS_BIT_SET(nothing,2);
					_insideAndOutsideFacesSameColor=SIM_IS_BIT_SET(nothing,3);
					// reserved   doTheConvectivityTest=!SIM_IS_BIT_SET(nothing,4); // version 3.0.1 was buggy
					// reserved doTheConvectivityTest=!SIM_IS_BIT_SET(nothing,5); // since version 3.0.2 (version 3.0.1 was buggy)
					_displayInverted=SIM_IS_BIT_SET(nothing,6);
				}
				if (theName.compare("Toj")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					_textureProperty=new CTextureProperty();
					_textureProperty->serialize(ar);
				}
				if (theName.compare("Hfd")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _heightfieldXCount >> _heightfieldYCount;
					for (int i=0;i<_heightfieldXCount*_heightfieldYCount;i++)
					{
						float dummy;
						ar >> dummy;
						_heightfieldHeights.push_back(dummy);
					}
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
//***********************
		if ( (!App::ct->undoBufferContainer->isUndoSavingOrRestoringUnderWay()) && (ar.getVrepVersionThatWroteThisFile()<30003) )
		{
			checkIfConvex();
			if ( (fabs(__bulletNonDefaultCollisionMarginFactor-0.002f)<0.0001f) && ( (ar.getVrepVersionThatWroteThisFile()==30002)||(ar.getVrepVersionThatWroteThisFile()==30001) ) )
			{ // Here we very probably correct a bug that affected version 3.0.2! (some people might have reverted to version 3.0.1, but the bug then was saved in 3.0.1, so we also check that version)
				__bulletNonDefaultCollisionMarginFactor=0.1f;
				__bulletNonDefaultCollisionMargin=false; // !!!!
				__bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape=0.002f;
				__bulletNonDefaultCollisionMargin_forConvexAndNonPureShape=true;
			}
		}

		if (App::userSettings->forceBugFix_rel30002)
		{
			__bulletNonDefaultCollisionMarginFactor=0.1f;
			__bulletNonDefaultCollisionMargin=false;
			__bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape=0.002f;
			__bulletNonDefaultCollisionMargin_forConvexAndNonPureShape=true;
		}
//***********************
		if ((ar.getSerializationVersionThatWroteThisFile()<17)&&App::userSettings->correctColorsAndLights)
		{ // on 29/08/2013 we corrected all default lights. So we need to correct for that change:
			CTTUtil::scaleColorUp_(color.colors);
			CTTUtil::scaleColorUp_(insideColor.colors);
			CTTUtil::scaleColorUp_(edgeColor.colors);
		}
	}
}
