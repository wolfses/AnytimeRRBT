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

#pragma once

#include "VisualParam.h"
#include "4X4Matrix.h"
#include "VMutex.h"

class CDrawingObject  
{
public:
	CDrawingObject(int theObjectType,float size,float duplicateTolerance,int sceneObjID,int maxItemCount,bool createdFromScript);
	virtual ~CDrawingObject();

	void setObjectID(int newID);
	int getObjectID();
	void draw(bool overlay,bool transparentObject,int displayAttrib,const C4X4Matrix& cameraCTM);
	void addItem(const float* itemData);
	int getObjectType();
	bool announceObjectWillBeErased(int objID);
	void adjustForFrameChange(const C7Vector& preCorrection);
	void adjustForScaling(float xScale,float yScale,float zScale);

	bool canMeshBeExported();
	void getExportableMesh(std::vector<float>& vertices,std::vector<int>& indices);

	int getSceneObjectID();
	bool getCreatedFromScript();
	void setCreatedFromScript(bool c);

	CVisualParam color;

	int verticesPerItem;
	int normalsPerItem;
	int otherFloatsPerItem;
	int floatsPerItem;

protected:
	void _drawPoints(int displayAttrib,const C4X4Matrix& cameraRTM);
	void _drawTrianglePoints(int displayAttrib,const C4X4Matrix& cameraRTM);
	void _drawQuadPoints(int displayAttrib,const C4X4Matrix& cameraRTM);
	void _drawDiscPoints(int displayAttrib,const C4X4Matrix& cameraRTM);
	void _drawCubePoints(int displayAttrib,const C4X4Matrix& cameraRTM);
	void _drawSpherePoints(int displayAttrib);
	void _drawLines(int displayAttrib,const C4X4Matrix& cameraRTM);
	void _drawTriangles(int displayAttrib);

	void _exportTrianglePoints(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices);
	void _exportQuadPoints(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices);
	void _exportDiscPoints(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices);
	void _exportCubePoints(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices);
	void _exportSpherePoints(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices);
	void _exportTriangles(C7Vector& tr,std::vector<float>& vertices,std::vector<int>& indices);
	void _exportTriOrQuad(C7Vector& tr,C3Vector* v0,C3Vector* v1,C3Vector* v2,C3Vector* v3,std::vector<float>& vertices,std::vector<int>& indices,int& nextIndex);

	void _setItemSizes();

	int _objectID;
	int _sceneObjectID;
	int _objectType;
	float _size;
	int _maxItemCount;
	int _startItem;
	float _duplicateTolerance;
	bool _createdFromScript;

	VMutex _objectMutex;

	std::vector<float> _data;
};
