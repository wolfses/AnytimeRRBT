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

// This is needed only to be backward compatible with serialization version 15 (Serialization version 16 since Nov. 2012)

#include "vrepPrecompiledHeader.h"
#include "GeomObject.h"
#include "App.h"
#include "geometric.h"

CGeomObject_OLD::CGeomObject_OLD()
{
	_commonInit();
}

CGeomObject_OLD::~CGeomObject_OLD()
{
	delete _geomStruct;
}

CGeomProxy* CGeomObject_OLD::buildEquivalentGeomProxy()
{ // Routine needed for the transition to the new shape-Geom architecture (November 2012). This is only needed when loading files with serialization version 15 or lower
	// Before we had: CShape-CGeomObject_OLD-CShapeComponent_OLD
	// Now we have: CShape-CGeomProxy-CGeomWrap/CGeometric

	CGeomProxy* proxy=new CGeomProxy();
	if (_geomStruct->isGrouping())
	{
		CGeomWrap* geomInfo=new CGeomWrap();
		geomInfo->setDynMaterialId(-1); // this triggers the creation of a material later!

		geomInfo->setMass(_geomStruct->getMass());

		geomInfo->__bulletStickyContact=_geomStruct->getBulletStickyContact();
		geomInfo->__bulletRestitution=_geomStruct->getBulletRestitution();
		geomInfo->__bulletFriction=_geomStruct->getBulletFriction();
		geomInfo->__bulletLinearDamping=_geomStruct->getBulletLinearDamping();
		geomInfo->__bulletAngularDamping=_geomStruct->getBulletAngularDamping();

		geomInfo->__odeMaxContacts=_geomStruct->getOdeMaxContacts();
		geomInfo->__odeFriction=_geomStruct->getOdeFriction();
		geomInfo->__odeSoftCFM=_geomStruct->getOdeSoftCFM();
		geomInfo->__odeSoftERP=_geomStruct->getOdeSoftERP();
		geomInfo->__odeLinearDamping=_geomStruct->getOdeLinearDamping();
		geomInfo->__odeAngularDamping=_geomStruct->getOdeAngularDamping();

		geomInfo->setPrincipalMomentsOfInertia(_geomStruct->getPrincipalMomentsOfInertia());
		geomInfo->setLocalInertiaFrame(_geomStruct->getLocalInertiaFrame());

		_geomStruct->buildEquivalentGeometricsAndInsert(true,this,geomInfo);

		proxy->geomInfo=geomInfo;
	}
	else
	{
		CGeometric* geomInfo=new CGeometric();
		geomInfo->setDynMaterialId(-1); // this triggers the creation of a material later!
		_geomStruct->color.copyYourselfInto(&geomInfo->color);
		_geomStruct->insideColor.copyYourselfInto(&geomInfo->insideColor);
		_geomStruct->edgeColor.copyYourselfInto(&geomInfo->edgeColor);
		geomInfo->setVisibleEdges(_geomStruct->getVisibleEdges());
		geomInfo->setCulling(_geomStruct->getCulling());
		geomInfo->setInsideAndOutsideFacesSameColor(_geomStruct->getInsideAndOutsideFacesSameColor());
		geomInfo->setWireframe(_geomStruct->getWireframe());
		geomInfo->setEdgeWidth(_geomStruct->getEdgeWidth());
		geomInfo->setGouraudShadingAngle(_geomStruct->getGouraudShadingAngle());
		geomInfo->setEdgeThresholdAngle(_geomStruct->getGouraudShadingAngle());
		geomInfo->setName(_geomStruct->getName());
		int hfX,hfY;
		float hfminH,hfmaxH;
		float* hfData=_geomStruct->getHeightfieldData(hfX,hfY,hfminH,hfmaxH);
		if (hfData!=NULL)
		{
			std::vector<float> heights;
			heights.resize(hfX*hfY);
			for (int cntI=0;cntI<hfX*hfY;cntI++)
				heights[cntI]=hfData[cntI];
			geomInfo->setHeightfieldData(heights,hfX,hfY);
		}
		int primit=_geomStruct->getPurePrimitiveType();
		C3Vector p;
		_geomStruct->getPurePrimitiveSizes(p);
		geomInfo->setPurePrimitiveType(primit,p(0),p(1),p(2));
		geomInfo->setTextureProperty(_geomStruct->getTextureProperty());
		_geomStruct->setTextureProperty(NULL);
		geomInfo->setMeshDataDirect(visibleVertices,visibleIndices,visibleNormals,visibleEdges);

		geomInfo->setMass(_geomStruct->getMass());

		geomInfo->__bulletStickyContact=_geomStruct->getBulletStickyContact();
		geomInfo->__bulletRestitution=_geomStruct->getBulletRestitution();
		geomInfo->__bulletFriction=_geomStruct->getBulletFriction();
		geomInfo->__bulletLinearDamping=_geomStruct->getBulletLinearDamping();
		geomInfo->__bulletAngularDamping=_geomStruct->getBulletAngularDamping();
		
		geomInfo->__odeMaxContacts=_geomStruct->getOdeMaxContacts();
		geomInfo->__odeFriction=_geomStruct->getOdeFriction();
		geomInfo->__odeSoftCFM=_geomStruct->getOdeSoftCFM();
		geomInfo->__odeSoftERP=_geomStruct->getOdeSoftERP();
		geomInfo->__odeLinearDamping=_geomStruct->getOdeLinearDamping();
		geomInfo->__odeAngularDamping=_geomStruct->getOdeAngularDamping();

		geomInfo->setPrincipalMomentsOfInertia(_geomStruct->getPrincipalMomentsOfInertia());
		geomInfo->setLocalInertiaFrame(_geomStruct->getLocalInertiaFrame());

		proxy->geomInfo=geomInfo;
	}
	proxy->computeBoundingBox();
	return(proxy);
}

void CGeomObject_OLD::_commonInit()
{
	visibleTextureCoords_notCopiedNorSerialized.clear();
	visibleVertices.clear();
	visibleIndices.clear();
	visibleNormals.clear();
	visibleEdges.clear();

	collNode=NULL;
	calcVertices.clear();
	calcIndices.clear();
	calcSegments.clear();
	calcPolygons.clear();
	calcEdgeIDs.clear();
	calcFaceIDs.clear();
	calcPointIDs.clear();
	_geomStruct=NULL;
	_creationTransf.setIdentity();
}

void CGeomObject_OLD::computeBoundingBox()
{
	for (int i=0;i<int(visibleVertices.size())/3;i++)
	{
		if (i==0)
			_boundingBoxHalfSizes.set(&visibleVertices[3*i+0]);
		else
			_boundingBoxHalfSizes.keepMax(C3Vector(&visibleVertices[3*i+0]));
	}
}

void CGeomObject_OLD::serialize(CSer& ar)
{
	if (!ar.isStoring())
	{		// Loading
		int byteQuantity;
		std::string theName="";
		while (theName.compare(SER_END_OF_OBJECT)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_END_OF_OBJECT)!=0)
			{
				bool noHit=true;
				if (theName.compare("Var")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE dummy;
					ar >> dummy;
				}
				if (App::ct->undoBufferContainer->isUndoSavingOrRestoringUnderWay())
				{ // undo/redo serialization
					if (theName.compare("Ver")==0)
					{
						noHit=false;
						ar >> byteQuantity;
						int id;
						ar >> id;
						App::ct->undoBufferContainer->undoBufferArrays.getVertexBuffer(id,visibleVertices);
					}
					if (theName.compare("Ind")==0)
					{
						noHit=false;
						ar >> byteQuantity;
						int id;
						ar >> id;
						App::ct->undoBufferContainer->undoBufferArrays.getIndexBuffer(id,visibleIndices);
					}
					if (theName.compare("Nor")==0)
					{
						noHit=false;
						ar >> byteQuantity;
						int id;
						ar >> id;
						App::ct->undoBufferContainer->undoBufferArrays.getNormalsBuffer(id,visibleNormals);
					}
				}
				else
				{ // normal serialization
					if (theName.compare("Ver")==0)
					{
						noHit=false;
						ar >> byteQuantity;
						visibleVertices.resize(byteQuantity/sizeof(float),0.0f);
						for (int i=0;i<int(visibleVertices.size());i++)
							ar >> visibleVertices[i];
					}
					if (theName.compare("Ind")==0)
					{
						noHit=false;
						ar >> byteQuantity;
						visibleIndices.resize(byteQuantity/sizeof(int),0);
						for (int i=0;i<int(visibleIndices.size());i++)
							ar >> visibleIndices[i];
					}
					if (theName.compare("Nor")==0)
					{
						noHit=false;
						ar >> byteQuantity;
						visibleNormals.resize(byteQuantity/sizeof(float),0.0f);
						for (int i=0;i<int(visibleNormals.size());i++)
							ar >> visibleNormals[i];
					}
				}

				if (theName.compare("In2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					loadPackedIntegers(ar,visibleIndices);
				}
				if (theName.compare("No2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					visibleNormals.resize(byteQuantity*6/sizeof(float),0.0f);
					for (int i=0;i<byteQuantity/2;i++)
					{
						WORD w;
						ar >> w;
						char x=(w&0x001f)-15;
						char y=((w>>5)&0x001f)-15;
						char z=((w>>10)&0x001f)-15;
						C3Vector n((float)x,(float)y,(float)z);
						n.normalize();
						visibleNormals[3*i+0]=n(0);
						visibleNormals[3*i+1]=n(1);
						visibleNormals[3*i+2]=n(2);
					}
				}
				if (theName.compare("Ved")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					visibleEdges.resize(byteQuantity,0);
					for (int i=0;i<byteQuantity;i++)
						ar >> visibleEdges[i];
				}


				if (theName.compare("Gst")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					delete _geomStruct;
					_geomStruct=new CShapeComponent_OLD();
					_geomStruct->serialize(ar);
				}
				if (theName.compare("Bn0")==0)
				{
 					noHit=false;
					ar >> byteQuantity;
					calcEdgeIDs.resize(byteQuantity/sizeof(int),0);
					for (int i=0;i<int(calcEdgeIDs.size());i++)
						ar >> calcEdgeIDs[i];
				}
				if (theName.compare("Pn0")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					loadPackedIntegers(ar,calcEdgeIDs);
				}

				if (theName.compare("Bn1")==0)
				{
 					noHit=false;
					ar >> byteQuantity;
					calcFaceIDs.resize(byteQuantity/sizeof(int),0);
					for (int i=0;i<int(calcFaceIDs.size());i++)
						ar >> calcFaceIDs[i];
				}
				if (theName.compare("Pn1")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					loadPackedIntegers(ar,calcFaceIDs);
				}

				if (theName.compare("Bn2")==0)
				{
 					noHit=false;
					ar >> byteQuantity;
					calcPointIDs.resize(byteQuantity/sizeof(int),0);
					for (int i=0;i<int(calcPointIDs.size());i++)
						ar >> calcPointIDs[i];
				}
				if (theName.compare("Pn2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					loadPackedIntegers(ar,calcPointIDs);
				}

				if (theName.compare("Bn3")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					calcVertices.resize(byteQuantity/sizeof(float),0.0f);
					for (int i=0;i<int(calcVertices.size());i++)
						ar >> calcVertices[i];
				}
				if (theName.compare("Bn4")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					calcIndices.resize(byteQuantity/sizeof(int),0);
					for (int i=0;i<int(calcIndices.size());i++)
						ar >> calcIndices[i];
				}
				if (theName.compare("Pn4")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					loadPackedIntegers(ar,calcIndices);
				}
				if (theName.compare("Bn5")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					calcSegments.resize(byteQuantity/sizeof(int),0);
					for (int i=0;i<int(calcSegments.size());i++)
						ar >> calcSegments[i];
				}
				if (theName.compare("Pn5")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					loadPackedIntegers(ar,calcSegments);
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}

		// FOLLOWING CODE TO TRANSFORM (BRUTALLY) ALL GROUPED SHAPES INTO SIMPLE SHAPES
		//if (_geomStruct->isGrouping())
		//{
		//	_geomStruct->vertStart=0;
		//	_geomStruct->triStart=0;
		//	_geomStruct->vertRange=visibleVerticesLength/3;
		//	_geomStruct->triRange=visibleIndicesLength/3;
		//	_geomStruct->childrenList.clear();
		//}

		computeBoundingBox();
	}
}

void CGeomObject_OLD::loadPackedIntegers(CSer& ar,std::vector<int>& data)
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
