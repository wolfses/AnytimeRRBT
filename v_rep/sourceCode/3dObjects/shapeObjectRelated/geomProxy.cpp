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
#include "geomProxy.h"
#include "geometric.h"

#include "MeshManip.h"
#include "algos.h"
#include "IloIlo.h"
#include "OGL.h"
#include "collDistInterface.h"
#include "sensingThreadPool.h"
#include "App.h"

CGeomProxy::CGeomProxy()
{
	_commonInit();
}

CGeomProxy::CGeomProxy(const std::vector<float>& allHeights,int xSize,int ySize,float dx,float zSize)
{
	_commonInit();
	std::vector<float> vert;
	std::vector<int> ind;
	float yPos=-float(ySize-1)*dx*0.5f;
	for (int i=0;i<ySize;i++)
	{
		float xPos=-float(xSize-1)*dx*0.5f;
		for (int j=0;j<xSize;j++)
		{
			vert.push_back(xPos);
			vert.push_back(yPos);
			vert.push_back(allHeights[i*xSize+j]);
			xPos+=dx;
		}
		yPos+=dx;
	}

	for (int i=0;i<ySize-1;i++)
	{
		for (int j=0;j<xSize-1;j++)
		{
			ind.push_back((i+0)*xSize+(j+0));
			ind.push_back((i+0)*xSize+(j+1));
			ind.push_back((i+1)*xSize+(j+0));

			ind.push_back((i+0)*xSize+(j+1));
			ind.push_back((i+1)*xSize+(j+1));
			ind.push_back((i+1)*xSize+(j+0));
		}
	}
	acceptNewGeometry(vert,ind,NULL,NULL);

	geomInfo->setPurePrimitiveType(sim_pure_primitive_heightfield,float(xSize-1)*dx,float(ySize-1)*dx,zSize);
	std::vector<float> heightsInCorrectOrder;
	for (int i=0;i<ySize;i++)
	{
		// Following doesn't work correctly somehow... does Bullet use left-hand coord. system?
		//for (int j=xSize-1;j>=0;j--)
		//	heightsInCorrectOrder.push_back(allHeights[i*xSize+j]);
		for (int j=0;j<xSize;j++)
			heightsInCorrectOrder.push_back(allHeights[i*xSize+j]);
	}
	((CGeometric*)geomInfo)->setHeightfieldData(heightsInCorrectOrder,xSize,ySize);
}

CGeomProxy::CGeomProxy(const C7Vector* transformation,const std::vector<float>& vert,const std::vector<int>& ind,const std::vector<float>* normals,const std::vector<float>* textCoord)
{
	_commonInit();
	
	std::vector<float>* norms=NULL;
	std::vector<float> _norms;
	if (normals!=NULL)
	{
		norms=&_norms;
		_norms.assign(normals->begin(),normals->end());
		// Make sure the normals are normalized:
		for (int i=0;i<int(_norms.size())/3;i++)
		{
			C3Vector n(&_norms[3*i]);
			n.normalize();
			_norms[3*i+0]=n(0);
			_norms[3*i+1]=n(1);
			_norms[3*i+2]=n(2);
		}
	}

	if (transformation==NULL)
		acceptNewGeometry(vert,ind,textCoord,norms);
	else
	{
		std::vector<float> wvert(vert);
		for (int i=0;i<int(vert.size())/3;i++)
		{
			C3Vector v(&vert[3*i+0]);
			v*=(*transformation);
			wvert[3*i+0]=v(0);
			wvert[3*i+1]=v(1);
			wvert[3*i+2]=v(2);
		}
		C7Vector tr(*transformation);
		for (int i=0;i<int(_norms.size())/3;i++)
		{
			C3Vector n(&_norms[3*i+0]);
			n*=tr;
			_norms[3*i+0]=n(0);
			_norms[3*i+1]=n(1);
			_norms[3*i+2]=n(2);
		}
		acceptNewGeometry(wvert,ind,textCoord,norms);
	}
}

CGeomProxy::CGeomProxy(const C7Vector& transformation,CGeomWrap* newGeomInfo)
{
	_commonInit();

	newGeomInfo->preMultiplyAllVerticeLocalFrames(transformation);
	geomInfo=newGeomInfo;
	std::vector<float> wvert;
	std::vector<int> wind;
	geomInfo->getCumulativeMeshes(wvert,&wind,NULL);

	// We align the bounding box:
	if (wvert.size()!=0)
	{
		_creationTransf=CAlgos::alignAndCenterGeometryAndGetTransformation(&wvert[0],wvert.size(),&wind[0],wind.size(),NULL,0,true);
		geomInfo->preMultiplyAllVerticeLocalFrames(_creationTransf.getInverse());
	}
	else
		_creationTransf.setIdentity();

	computeBoundingBox();
}


CGeomProxy::~CGeomProxy()
{
	delete geomInfo;
	removeCollisionInformation();
}

void CGeomProxy::_commonInit()
{
	collInfo=NULL;
	geomInfo=NULL;
	_creationTransf.setIdentity();
	_dynamicsFullRefreshFlag=true;
	_geomDataModificationCounter=0;
}

void CGeomProxy::removeCollisionInformation()
{
	FUNCTION_DEBUG;
	CSensingThreadPool::setProtectedAccess(true); // make sure other working threads do not access this structure when being destroyed (should normally never happen)
	if (collInfo!=NULL)
	{
		CCollDistInterface::destroyCollisionInformationStructure(collInfo);
		collInfo=NULL;
	}
	CSensingThreadPool::setProtectedAccess(false);
}

C3Vector CGeomProxy::getBoundingBoxHalfSizes()
{
	return(_boundingBoxHalfSizes);
}

void CGeomProxy::invertFrontBack()
{
	if (geomInfo!=NULL)
	{
		geomInfo->flipFaces();
		removeCollisionInformation();// proximity sensors might check for the side!
	}
}

void CGeomProxy::perform3DObjectLoadingMapping(std::vector<int>* map)
{
	if (geomInfo!=NULL)
		geomInfo->perform3DObjectLoadingMapping(map);
}

void CGeomProxy::performTextureObjectLoadingMapping(std::vector<int>* map)
{
	if (geomInfo!=NULL)
		geomInfo->performTextureObjectLoadingMapping(map);
}

void CGeomProxy::announce3DObjectWillBeErased(int objectID)
{
	if (geomInfo!=NULL)
		geomInfo->announce3DObjectWillBeErased(objectID);
}

void CGeomProxy::initializeCalculationStructureIfNeeded()
{
	C_API_FUNCTION_DEBUG;
	CSensingThreadPool::setProtectedAccess(true); // make sure other working threads do not access this structure when being built
	if ((collInfo==NULL)&&(geomInfo!=NULL))
	{
		std::vector<float> wvert;
		std::vector<int> wind;
		geomInfo->getCumulativeMeshes(wvert,&wind,NULL);
		float maxTriSize=App::ct->environment->getCalculationMaxTriangleSize();
		float minTriSize=(SIM_MAX(SIM_MAX(_boundingBoxHalfSizes(0),_boundingBoxHalfSizes(1)),_boundingBoxHalfSizes(2)))*2.0f*App::ct->environment->getCalculationMinRelTriangleSize();
		if (maxTriSize<minTriSize)
			maxTriSize=minTriSize;
		collInfo=CCollDistInterface::createCollisionInformationStructure(&wvert[0],wvert.size(),&wind[0],wind.size(),maxTriSize,geomInfo->getEdgeThresholdAngle(),App::userSettings->triCountInOBB);
	}
	CSensingThreadPool::setProtectedAccess(false);
}
/*
void CGeomProxy::initializeCollisionInformation()
{
	if ((collInfo==NULL)&&(geomInfo!=NULL))
	{
		std::vector<float> wvert;
		std::vector<int> wind;
		geomInfo->getCumulativeMeshes(wvert,&wind,NULL);
		collInfo=CCollDistInterface::createCollisionInformationStructure(&wvert[0],wvert.size(),&wind[0],wind.size(),App::ct->environment->getCalculationMaxTriangleSize(),geomInfo->getEdgeThresholdAngle(),CIloIlo::userSettings->triCountInOBB);
	}
}
*/
bool CGeomProxy::isCollisionInformationInitialized()
{
	bool retVal;
	CSensingThreadPool::setProtectedAccess(true); // make sure other working threads do not access this structure when being built
	retVal=(collInfo!=NULL);
	CSensingThreadPool::setProtectedAccess(false);
	return(retVal);
}

void CGeomProxy::computeBoundingBox()
{ // Only the bounding box is recomputed. 
	std::vector<float> visibleVertices;
	geomInfo->getCumulativeMeshes(visibleVertices,NULL,NULL);
	for (int i=0;i<int(visibleVertices.size())/3;i++)
	{
		if (i==0)
			_boundingBoxHalfSizes.set(&visibleVertices[3*i+0]);
		else
			_boundingBoxHalfSizes.keepMax(C3Vector(&visibleVertices[3*i+0]));
	}
}

void CGeomProxy::scale(float xVal,float yVal,float zVal)
{	// The geometric resource is scaled and the bounding box is recomputed.
	// Normals are not recomputed if xVal==yVal==yVal
	_dynamicsFullRefreshFlag=true; // make sure we refresh part of the dynamic world!
	_geomDataModificationCounter++;
	if (geomInfo->isPure())
	{ // we have some constraints in case we have a pure mesh (or several pure meshes in a group)
		if (geomInfo->isGeometric())
		{ // we have a pure mesh (non-group)
			int purePrim=((CGeometric*)geomInfo)->getPurePrimitiveType();
			if (purePrim==sim_pure_primitive_plane)
				zVal=1.0f;
			if (purePrim==sim_pure_primitive_disc)
			{
				zVal=1.0f;
				yVal=xVal;
			}
			if ( (purePrim==sim_pure_primitive_cylinder)||(purePrim==sim_pure_primitive_cone)||(purePrim==sim_pure_primitive_heightfield) )
				yVal=xVal;
		}
	}
	if (!geomInfo->isGeometric())
	{ // we have a group. We do iso-scaling!
		yVal=xVal;
		zVal=xVal;
	}

	// Scale collision info if we have an isometric scaling:
	if ((xVal==yVal)&&(xVal==zVal)&&(collInfo!=NULL))
		CCollDistInterface::scaleCollisionInformationStructure(collInfo,xVal);
	else
		removeCollisionInformation(); // we have to recompute it!

	// Scale meshes and adjust textures:
	geomInfo->scale(xVal,yVal,zVal);

	// recompute the bounding box:
	computeBoundingBox();
}

void CGeomProxy::setDynamicsFullRefreshFlag(bool refresh)
{
	_dynamicsFullRefreshFlag=refresh;
}

bool CGeomProxy::getDynamicsFullRefreshFlag()
{
	return(_dynamicsFullRefreshFlag);
}

int CGeomProxy::getGeomDataModificationCounter()
{
	return(_geomDataModificationCounter);
}

void CGeomProxy::setTextureDependencies(int shapeID)
{
	if (geomInfo!=NULL)
		geomInfo->setTextureDependencies(shapeID);
}

C7Vector CGeomProxy::recomputeOrientation(C7Vector& m,bool alignWithMainAxis)
{ // This routine will reorient the shape according to its main axis if 
  // alignWithMainAxis is true, and according to the world if false.
  // Don't forget to recompute (not done here) the new local transformation
  // matrices of the objects linked to that geometric resource!!
  // Input m is the cumulative transf. if alignWithMainAxis==false
  // Returned m is the new local transformation of this geometry

	// 1. We remove the old info:
	removeCollisionInformation();

	// 2. We set-up the absolute vertices and normal position/orientation:
	geomInfo->preMultiplyAllVerticeLocalFrames(m);

	// 3. We calculate the new orientation:
	std::vector<float> visibleVertices;
	std::vector<int> visibleIndices;
	geomInfo->getCumulativeMeshes(visibleVertices,&visibleIndices,NULL);
	C7Vector tr;
	if (visibleVertices.size()!=0)
	{
		tr=CAlgos::alignAndCenterGeometryAndGetTransformation(&visibleVertices[0],visibleVertices.size(),&visibleIndices[0],visibleIndices.size(),NULL,0,alignWithMainAxis);
	}
	else
		tr.setIdentity();

	// 4. We apply it:
	geomInfo->preMultiplyAllVerticeLocalFrames(tr.getInverse());

	// 5. We recompute usual things:
	computeBoundingBox();

	_geomDataModificationCounter++;
	return(tr);
}


C7Vector CGeomProxy::recomputeTubeOrCuboidOrientation(C7Vector& m,bool tube,bool& error)
{ // This routine will reorient the tube shape according to its main axis
  // Don't forget to recompute (not done here) the new local transformation
  // matrices of the objects linked to that geometric resource!!
  // Input m is the cumulative transf.
  // Returned m is the new cumulative transformation of this geometry

	error=false; // no error yet
	C7Vector tr;

	// 0. We set-up the absolute vertices and retrieve them:
	geomInfo->preMultiplyAllVerticeLocalFrames(m);
	std::vector<float> visibleVertices;
	std::vector<int> visibleIndices;
	geomInfo->getCumulativeMeshes(visibleVertices,&visibleIndices,NULL);

	// 1. We calculate the new orientation, based on the copy:
	bool success;
	if (tube)
		success=_getTubeReferenceFrame(visibleVertices,tr);
	else
		success=_getCuboidReferenceFrame(visibleVertices,visibleIndices,tr);
	if (!success)
	{
		error=true;
		geomInfo->preMultiplyAllVerticeLocalFrames(m.getInverse()); // don't forget to make operation backward before leaving!
		return(tr);
	}

	// 2. We remove the old info:
	removeCollisionInformation();

	// 4. We have the desired orientation (tr.Q), we now calculate the position (should be the same or very very close to what we have in tr.X)
	C7Vector trInv(tr.getInverse());
	C3Vector maxV,minV;
	for (int i=0;i<int(visibleVertices.size())/3;i++)
	{
		C3Vector v(&visibleVertices[3*i+0]);
		v=trInv*v;
		if (i==0)
		{
			maxV=v;
			minV=v;
		}
		else
		{
			maxV.keepMax(v);
			minV.keepMin(v);
		}
	}
	C3Vector newCenter((maxV(0)+minV(0))*0.5f,(maxV(1)+minV(1))*0.5f,(maxV(2)+minV(2))*0.5f); // relative pos
	newCenter=tr*newCenter; // now abs pos
	tr.X=newCenter;

	// 5. We have the new center. We set all vertices relative to tr!!
	geomInfo->preMultiplyAllVerticeLocalFrames(tr.getInverse());

	// 6. We recompute usual things:
	computeBoundingBox();

	_geomDataModificationCounter++;
	return(tr);
}

bool CGeomProxy::_getTubeReferenceFrame(const std::vector<float>& v,C7Vector& tr)
{
	tr.setIdentity();
	// 1) Do we have enough points?
	if (v.size()/3<6)
		return(false);
	// 2) Get the longest distance:
	int indexLeft=-1;
	int indexRight=-1;
	float longestDist=0.0f;
	for (int i=0;i<int(v.size())/3;i++)
	{
		C3Vector pt1(&v[3*i+0]);
		for (int j=i+1;j<int(v.size())/3;j++)
		{
			C3Vector pt2(&v[3*j+0]);
			float l=(pt1-pt2).getLength();
			if (l>longestDist)
			{
				longestDist=l;
				indexLeft=i;
				indexRight=j;
			}
		}
	}
	if (indexLeft==-1)
		return(false); // all points are coincident!
	// 3) For each of the 2 found points, find 1 closest neighbour that is not coincident:
	C3Vector leftPt1(&v[3*indexLeft+0]);
	C3Vector rightPt1(&v[3*indexRight+0]);
	int indexLeft2=-1;
	int indexRight2=-1;
	float leftDist=SIM_MAX_FLOAT;
	float rightDist=SIM_MAX_FLOAT;
	for (int i=0;i<int(v.size())/3;i++)
	{
		C3Vector pt(&v[3*i+0]);
		if (i!=indexLeft)
		{
			float l=(pt-leftPt1).getLength();
			if ((l!=0.0f)&&(l<leftDist))
			{
				leftDist=l;
				indexLeft2=i;
			}
		}
		if (i!=indexRight)
		{
			float l=(pt-rightPt1).getLength();
			if ((l!=0.0f)&&(l<rightDist))
			{
				rightDist=l;
				indexRight2=i;
			}
		}
	}
	if ((indexLeft2==-1)||(indexRight2==-1))
		return(false); // error

	// 4) For each of the 2 found segments, find 1 closest neighbour that forms a plane:
	C3Vector leftPt2(&v[3*indexLeft2+0]);
	C3Vector rightPt2(&v[3*indexRight2+0]);
	int indexLeft3=-1;
	int indexRight3=-1;
	leftDist=SIM_MAX_FLOAT;
	rightDist=SIM_MAX_FLOAT;
	for (int i=0;i<int(v.size())/3;i++)
	{
		C3Vector pt(&v[3*i+0]);
		if ((i!=indexLeft)&&(i!=indexLeft2))
		{
			float l1=(pt-leftPt1).getLength();
			float l2=(pt-leftPt2).getLength();
			if ((l1!=0.0f)&&(l2!=0.0f)&&(l1<leftDist))
			{

				float a=(leftPt1-pt).getAngle(leftPt2-pt);
				if ((a>1.0f*degToRad)&&(a<179.0f*degToRad))
				{
					leftDist=l1;
					indexLeft3=i;
				}
			}
		}
		if ((i!=indexRight)&&(i!=indexRight2))
		{
			float l1=(pt-rightPt1).getLength();
			float l2=(pt-rightPt2).getLength();
			if ((l1!=0.0f)&&(l2!=0.0f)&&(l1<rightDist))
			{

				float a=(rightPt1-pt).getAngle(rightPt2-pt);
				if ((a>1.0f*degToRad)&&(a<179.0f*degToRad))
				{
					rightDist=l1;
					indexRight3=i;
				}
			}
		}
	}
	if ((indexLeft3==-1)||(indexRight3==-1))
		return(false); // error
	
	// 5) Prepare the normal vectory of the 2 tube endings (direction doesn't matter):
	C3Vector leftPt3(&v[3*indexLeft3+0]);
	C3Vector rightPt3(&v[3*indexRight3+0]);
	C3Vector nLeft(((leftPt1-leftPt3)^(leftPt2-leftPt3)).getNormalized());
	C3Vector nRight(((rightPt1-rightPt3)^(rightPt2-rightPt3)).getNormalized());
	float a=nLeft.getAngle(nRight);
	if ((a>1.0f*degToRad)&&(a<179.0f*degToRad))
		return(false); // not precise enough

	// 6) Now get all points at each endings that are within 2% of distance to the end planes (relative to the longest distances) and calculate the average positions:
	C3Vector avgLeft,avgRight;
	avgLeft.clear();
	avgRight.clear();
	float cntLeft=0.0f;
	float cntRight=0.0f;
	for (int i=0;i<int(v.size())/3;i++)
	{
		C3Vector pt(&v[3*i+0]);
		C3Vector leftV(pt-leftPt1);
		float d=fabs(leftV*nLeft);
		if (d<longestDist*0.02f)
		{
			cntLeft+=1.0f;
			avgLeft+=pt;
		}

		C3Vector rightV(pt-rightPt1);
		d=fabs(rightV*nRight);
		if (d<longestDist*0.02f)
		{
			cntRight+=1.0f;
			avgRight+=pt;
		}
	}
	if ((cntLeft<3.99f)||(cntRight<3.99f)) // at least 4 points at each ending! (extruded triangle doesn't work anyway because it is not centered in the bounding box)
		return(false); // should not happen
	avgLeft/=cntLeft;
	avgRight/=cntRight;
	C3Vector avgPos((avgLeft+avgRight)*0.5f);

	// 7) now compute a transformation matrix!
	C4X4Matrix m;
	m.setIdentity();
	m.X=avgPos;
	m.M.axis[2]=(avgLeft-avgRight).getNormalized();
	m.M.axis[0]=C3Vector(1.02f,1.33f,1.69f).getNormalized(); // just a random vector;
	m.M.axis[1]=(m.M.axis[2]^m.M.axis[0]).getNormalized();
	m.M.axis[0]=(m.M.axis[1]^m.M.axis[2]).getNormalized();
	tr=m.getTransformation();

	// 8) Last: check if the bounding box is centered (e.g. a triangle-cylinder (with 3 faces)
	C3Vector maxV;
	C3Vector minV;
	C7Vector trInv(tr.getInverse());
	for (int i=0;i<int(v.size())/3;i++)
	{
		C3Vector pt(&v[3*i+0]);
		pt=trInv*pt;
		if (i==0)
		{
			maxV=pt;
			minV=pt;
		}
		else
		{
			maxV.keepMax(pt);
			minV.keepMin(pt);
		}
	}
	C3Vector dims(maxV(0)-minV(0),maxV(1)-minV(1),maxV(2)-minV(2));
	C3Vector vars(fabs(maxV(0)+minV(0)),fabs(maxV(1)+minV(1)),fabs(maxV(2)+minV(2)));
	for (int i=0;i<3;i++)
	{
		if (vars(i)/dims(1)>0.001f) // 0.1% tolerance relative to the box dimension
			return(false); // the bounding box would not be centered!
	}
	return(true);
}

bool CGeomProxy::_getCuboidReferenceFrame(const std::vector<float>& v,const std::vector<int>& ind,C7Vector& tr)
{
	tr.setIdentity();
	// 1) Do we have enough points?
	if (v.size()/3<8)
		return(false);
	// 2) Get the biggest triangle (in surface)
	int biggestTriIndex=-1;
	float biggestTriSurface=0.0f;
	C3Vector triangleN1;
	for (int i=0;i<int(ind.size())/3;i++)
	{
		C3Vector pt1(&v[3*ind[3*i+0]+0]);
		C3Vector pt2(&v[3*ind[3*i+1]+0]);
		C3Vector pt3(&v[3*ind[3*i+2]+0]);
		C3Vector v1(pt1-pt2);
		C3Vector v2(pt1-pt3);
		float s=(v1^v2).getLength();
		if (s>biggestTriSurface)
		{
			biggestTriSurface=s;
			biggestTriIndex=i;
			triangleN1=(v1^v2).getNormalized();
		}
	}
	if (biggestTriIndex==-1)
		return(false);
	
	// 3) Get the biggest triangle where the surface normal is perpendicular to the first triangle
	int biggestTriIndex2=-1;
	float biggestTriSurface2=0.0f;
	C3Vector triangleN2;
	for (int i=0;i<int(ind.size())/3;i++)
	{
		if (i!=biggestTriIndex)
		{
			C3Vector pt1(&v[3*ind[3*i+0]+0]);
			C3Vector pt2(&v[3*ind[3*i+1]+0]);
			C3Vector pt3(&v[3*ind[3*i+2]+0]);
			C3Vector v1(pt1-pt2);
			C3Vector v2(pt1-pt3);
			float s=(v1^v2).getLength();
			C3Vector n((v1^v2).getNormalized());
			if ((s>biggestTriSurface2)&&(fabs(triangleN1*n)<0.0001f))
			{
				biggestTriSurface2=s;
				biggestTriIndex2=i;
				triangleN2=n;
			}
		}
	}
	if (biggestTriIndex2==-1)
		return(false);
	

	// 4) now compute a transformation matrix!
	C4X4Matrix m;
	m.setIdentity();
	m.X.clear();
	m.M.axis[0]=triangleN1;
	m.M.axis[1]=triangleN2;
	m.M.axis[2]=(m.M.axis[0]^m.M.axis[1]).getNormalized();

	// 4) Get the center!
	C3Vector maxV;
	C3Vector minV;
	C4X4Matrix mInv(m.getInverse());
	for (int i=0;i<int(v.size())/3;i++)
	{
		C3Vector pt(&v[3*i+0]);
		pt=mInv*pt;
		if (i==0)
		{
			maxV=pt;
			minV=pt;
		}
		else
		{
			maxV.keepMax(pt);
			minV.keepMin(pt);
		}
	}
	C3Vector avgPos((maxV(0)+minV(0))*0.5f,(maxV(1)+minV(1))*0.5f,(maxV(2)+minV(2))*0.5f);
	avgPos=m*avgPos;
	m.X=avgPos;

	// 5) get the dimensions and reorient the frame to have z the longest dim, y the second longest
	C3Vector dim(maxV(0)-minV(0),maxV(1)-minV(1),maxV(2)-minV(2));
	C3X3Matrix rot;
	rot.setIdentity();
	float xDim=dim(0);
	float yDim=dim(1);
	if ((dim(0)>dim(1))&&(dim(0)>dim(2)))
	{
		rot.buildYRotation(piValD2);
		xDim=dim(2);
	}
	if ((dim(1)>dim(0))&&(dim(1)>dim(2)))
	{
		rot.buildXRotation(piValD2);
		yDim=dim(2);
	}
	m.M*=rot;
	// z has the biggest dimension now
	if (yDim<xDim)
	{
		rot.buildZRotation(piValD2);
		m.M*=rot;
	}
	// ok, now we have z,y,x ordered from largest to smallest
	tr=m.getTransformation();
	return(true);
}

CGeomProxy* CGeomProxy::copyYourself()
{
	CGeomProxy* newGeom=new CGeomProxy();

	newGeom->_creationTransf=_creationTransf;

	delete newGeom->geomInfo;
	newGeom->geomInfo=NULL;
	if (geomInfo!=NULL)
		newGeom->geomInfo=geomInfo->copyYourself();

	newGeom->_boundingBoxHalfSizes=_boundingBoxHalfSizes;

	if (collInfo!=NULL)
		newGeom->collInfo=CCollDistInterface::copyCollisionInformationStructure(collInfo);
	return(newGeom);
}

C7Vector CGeomProxy::getCreationTransformation()
{ // only valid just after shape creation!
	return(_creationTransf);
}

void CGeomProxy::setCreationTransformation(const C7Vector& tr)
{
	_creationTransf=tr;
}

void CGeomProxy::acceptNewGeometry(const std::vector<float>& vert,const std::vector<int>& ind,const std::vector<float>* textCoord,const std::vector<float>* norm)//,const std::vector<unsigned char>* ed)
{ 
	FUNCTION_DEBUG;
	std::vector<float> wwert(vert);
	std::vector<int> wwind(ind);

	// 1. We remove the old info:
	removeCollisionInformation();
	
	// 2. We set-up the new geometry:
	CGeometric* newGeomInfo=new CGeometric();

	if (textCoord!=NULL)
		newGeomInfo->textureCoords_notCopiedNorSerialized.assign(textCoord->begin(),textCoord->end());
	CMeshManip::removeNonReferencedVertices(wwert,wwind);
	newGeomInfo->setMesh(wwert,wwind,norm,C7Vector::identityTransformation); // will do the convectivity test


	newGeomInfo->color.setDefaultValues();
	newGeomInfo->color.setColor(
		0.1f+0.4f*(float)(rand()/(float)RAND_MAX),
		0.1f+0.4f*(float)(rand()/(float)RAND_MAX),
		0.1f+0.4f*(float)(rand()/(float)RAND_MAX),
		AMBIENT_MODE);

	if (geomInfo!=NULL)
	{ // copy a few properties (not all)
		newGeomInfo->setDynMaterialId(geomInfo->getDynMaterialId());
		//geomInfo->copyEnginePropertiesTo(newGeomInfo);

		if (geomInfo->isGeometric())
			((CGeometric*)geomInfo)->copyVisualAttributesTo(newGeomInfo);
	}

	delete geomInfo;
	geomInfo=newGeomInfo;

	// We align the bounding box:
	if (wwert.size()!=0)
	{
		std::vector<float> dummyVert(wwert);
		std::vector<int> dummyInd(wwind);
		_creationTransf=CAlgos::alignAndCenterGeometryAndGetTransformation(&dummyVert[0],dummyVert.size(),&dummyInd[0],dummyInd.size(),NULL,0,true);
		geomInfo->preMultiplyAllVerticeLocalFrames(_creationTransf.getInverse());
	}
	else
		_creationTransf.setIdentity();

	computeBoundingBox();

	_geomDataModificationCounter++;
}

bool CGeomProxy::applyCuttingChanges(const C7Vector& shapeCTM)
{ // return value true means: this shape has to be destroyed!
	FUNCTION_DEBUG;
	if (collInfo==NULL)
		return(true);
	int options=0;
	if (App::userSettings->identicalVerticesCheck)
		options|=1;
	if (App::userSettings->identicalTrianglesCheck)
		options|=4;
	if (App::userSettings->identicalTrianglesWindingCheck)
		options|=8;
	float* vertices;
	int verticesSize;
	int* indices;
	int indicesSize;
	if (CCollDistInterface::getCutMesh(collInfo,&shapeCTM,&vertices,&verticesSize,&indices,&indicesSize,options))
	{
		std::vector<float> vert(vertices,vertices+verticesSize);
		std::vector<int> ind(indices,indices+indicesSize);
		CCollDistInterface::releaseBuffer(vertices);
		CCollDistInterface::releaseBuffer(indices);
		removeCollisionInformation();
		acceptNewGeometry(vert,ind,NULL,NULL);
		return(false);
	}
	return(true);
}

void CGeomProxy::serialize(CSer& ar)
{
	if (ar.isStoring())
	{		// Storing
		// geomInfo hast to be stored before collInfo!!!
		if (geomInfo->isGeometric())
			ar.storeDataName("Gst");
		else
			ar.storeDataName("Gsg");
		ar.setCountingMode();
		geomInfo->serialize(ar);
		if (ar.setWritingMode())
			geomInfo->serialize(ar);

		// (if undo/redo under way, getSaveExistingCalculationStructuresTemp is false)
		if (App::ct->environment->getSaveExistingCalculationStructuresTemp()&&isCollisionInformationInitialized())
		{
			if (CCollDistInterface::getCalculatedPolygonCount(collInfo)!=0)
			{
				removeCollisionInformation(); // Make sure the shape is not in the cut-state:
				initializeCalculationStructureIfNeeded();
			}

			int collInfoDataSize;
			unsigned char* collInfoData=CCollDistInterface::getCollisionInformationStructureSerializationData(collInfo,collInfoDataSize);

			ar.storeDataName("Coi");
			ar.setCountingMode(true);
			for (int i=0;i<collInfoDataSize;i++)
				ar << collInfoData[i];
			ar.flush(false);
			if (ar.setWritingMode(true))
			{
				for (int i=0;i<collInfoDataSize;i++)
					ar << collInfoData[i];
				ar.flush(false);
			}
			CCollDistInterface::releaseBuffer(collInfoData);
		}
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

				if (theName.compare("Gst")==0)
				{ // geometric
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					delete geomInfo;
					geomInfo=new CGeometric();
					((CGeometric*)geomInfo)->serialize(ar);
				}
				if (theName.compare("Gsg")==0)
				{ // geomWrap
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					delete geomInfo;
					geomInfo=new CGeomWrap();
					geomInfo->serialize(ar);
				}

				if (theName.compare("Coi")==0)
				{
 					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo never stores calc structures)

					std::vector<unsigned char> data;
					data.reserve(byteQuantity);
					unsigned char dummy;
					for (int i=0;i<byteQuantity;i++)
					{
						ar >> dummy;
						data.push_back(dummy);
					}
					std::vector<float> wvert;
					std::vector<int> wind;
					geomInfo->getCumulativeMeshes(wvert,&wind,NULL);
					collInfo=CCollDistInterface::getCollisionInformationStructureFromSerializationData(&data[0],&wvert[0],wvert.size(),&wind[0],wind.size());
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
		computeBoundingBox();
	}
}
