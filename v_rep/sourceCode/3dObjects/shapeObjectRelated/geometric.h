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

#include "geomWrap.h"
#include "TextureProperty.h"

class CGeometric : public CGeomWrap
{
public:
	CGeometric();
	virtual ~CGeometric();

	void display(CGeomProxy* geomData,int displayAttrib,CVisualParam* collisionColor,int dynObjFlag_forVisualization,int transparencyHandling);
	void display_colorCoded(CGeomProxy* geomData,int objectId);
	void displayForCutting(CGeomProxy* geomData,int displayAttrib,CVisualParam* collisionColor);
	void displayGhost(CGeomProxy* geomData,int displayAttrib,bool originalColors,bool backfaceCulling,float transparency,const float* newColors);
	void perform3DObjectLoadingMapping(std::vector<int>* map);
	void performTextureObjectLoadingMapping(std::vector<int>* map);
	void announce3DObjectWillBeErased(int objectID);
	void setTextureDependencies(int shapeID);
	bool getContainsTransparentComponents();
	CGeometric* copyYourself();
	void scale(float xVal,float yVal,float zVal);
	int getPurePrimitiveType();
	void setPurePrimitiveType(int theType,float xOrDiameter,float y,float zOrHeight);
	bool isGeometric();
	bool isPure();
	bool isConvex();
	bool checkIfConvex();
	void setConvex(bool convex);
	void getCumulativeMeshes(std::vector<float>& vertices,std::vector<int>* indices,std::vector<float>* normals);
	void setColor(const char* colorName,int colorComponent,const float* rgbData);
	bool getColor(const char* colorName,int colorComponent,float* rgbData);
	void getAllShapeComponentsCumulative(std::vector<CGeometric*>& shapeComponentList); // needed by the dynamics routine
	CGeometric* getShapeComponentAtIndex(int& index);
	void serialize(CSer& ar);
	void preMultiplyAllVerticeLocalFrames(const C7Vector& preTr);
	void flipFaces();
	float getGouraudShadingAngle();
	void setGouraudShadingAngle(float angle);
	float getEdgeThresholdAngle();
	void setEdgeThresholdAngle(float angle);
	int getTextureCount();
	bool hasTextureThatUsesFixedTextureCoordinates();
	void removeAllTextures();
	void setHeightfieldDiamonds(int d); //0 for Bullet, 1 for ODE

	int getUniqueID();
	void setMesh(const std::vector<float>& vertices,const std::vector<int>& indices,const std::vector<float>* normals,const C7Vector& transformation);
	void setMeshDataDirect(const std::vector<float>& vertices,const std::vector<int>& indices,const std::vector<float>& normals,const std::vector<unsigned char>& edges);



	void setHeightfieldData(const std::vector<float>& heights,int xCount,int yCount);
	float* getHeightfieldData(int& xCount,int& yCount,float& minHeight,float& maxHeight);
	void getPurePrimitiveSizes(C3Vector& s);
	void setPurePrimitiveInsideScaling(float s);
	float getPurePrimitiveInsideScaling();

	C7Vector getVerticeLocalFrame();
	void setVerticeLocalFrame(const C7Vector& tr);

	CTextureProperty* getTextureProperty();
	void setTextureProperty(CTextureProperty* tp);
	bool getNonCalculatedTextureCoordinates(std::vector<float>& texCoords);

	void setVisibleEdges(bool v);
	bool getVisibleEdges();
	void setEdgeWidth(int w);
	int getEdgeWidth();
	void setCulling(bool c);
	bool getCulling();
	bool getDisplayInverted();
	void setDisplayInverted(bool di);

	void actualizeGouraudShadingAndVisibleEdges();

	void setInsideAndOutsideFacesSameColor(bool s);
	bool getInsideAndOutsideFacesSameColor();
	void setWireframe(bool w);
	bool getWireframe();

	void copyVisualAttributesTo(CGeometric* target);

	CVisualParam color;
	CVisualParam insideColor;
	CVisualParam edgeColor;

	// Do not serialize nor copy following 3:
	std::vector<float> textureCoords_notCopiedNorSerialized; // 2 values per vertex

	std::vector<float> _heightfieldHeights;
	int _heightfieldXCount;
	int _heightfieldYCount;

protected:
	void _displayTriangles(int geomModifCounter,CTextureProperty* tp);
	void _recomputeNormals();
	void _computeVisibleEdges();

	static void _savePackedIntegers(CSer& ar,const std::vector<int>& data);
	static void _loadPackedIntegers(CSer& ar,std::vector<int>& data);

	std::vector<float> _vertices;
	std::vector<int> _indices;
	std::vector<float> _normals;
	std::vector<unsigned char> _edges;
	
	bool _visibleEdges;
	bool _culling;
	bool _displayInverted;
	bool _insideAndOutsideFacesSameColor;
	bool _wireframe;
	int _edgeWidth;
	float _gouraudShadingAngle;
	float _edgeThresholdAngle;

	C7Vector _verticeLocalFrame; // frame relative to the shape. All vertices are transformed by it. This frame also represents the configuration of the origin frame of pure shapes!!!!
	CTextureProperty* _textureProperty;
	int _uniqueID;

	int _purePrimitive;
	float _purePrimitiveXSizeOrDiameter;
	float _purePrimitiveYSize;
	float _purePrimitiveZSizeOrHeight;
	float _purePrimitiveInsideScaling;

	static int _nextUniqueID;
};
