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
#include "VolumeNode.h"
#include "OGL.h"
#include "collDistInterface.h"

CVolumeNode::CVolumeNode()
{
	_commonInit();
}

CVolumeNode::CVolumeNode(float size,float resolution)
{
	if (size>resolution)
	{
		for (int i=0;i<2;i++)
		{
			for (int j=0;j<2;j++)
			{
				for (int k=0;k<2;k++)
					_childVolumeNodes[i][j][k]=new CVolumeNode(size*0.5f,resolution);
			}
		}
	}
	else
		_commonInit();
}

void CVolumeNode::_commonInit()
{
	for (int i=0;i<2;i++)
	{
		for (int j=0;j<2;j++)
		{
			for (int k=0;k<2;k++)
				_childVolumeNodes[i][j][k]=NULL;
		}
	}
	_normalVector.clear();
}

CVolumeNode::~CVolumeNode()
{
	for (int i=0;i<2;i++)
	{
		for (int j=0;j<2;j++)
		{
			for (int k=0;k<2;k++)
				delete _childVolumeNodes[i][j][k];
		}
	}
}

void CVolumeNode::getVisualBoundingBox(bool& first,float currentSize,const C3Vector& currentPosition,int level,C3Vector& minV,C3Vector& maxV)
{
	float halfSize=currentSize*0.5f;
	float quaterSize=halfSize*0.5f;
	if ((level>0)&&(
		(_childVolumeNodes[0][0][0]!=NULL)||
		(_childVolumeNodes[0][0][1]!=NULL)||
		(_childVolumeNodes[0][1][0]!=NULL)||
		(_childVolumeNodes[0][1][1]!=NULL)||
		(_childVolumeNodes[1][0][0]!=NULL)||
		(_childVolumeNodes[1][0][1]!=NULL)||
		(_childVolumeNodes[1][1][0]!=NULL)||
		(_childVolumeNodes[1][1][1]!=NULL)  ))
	{
		for (int i=-1;i<2;i=i+2)
		{
			C3Vector cp0(currentPosition);
			cp0(0)+=quaterSize*float(i);
			for (int j=-1;j<2;j=j+2)
			{
				C3Vector cp1(cp0);
				cp1(1)+=quaterSize*float(j);
				for (int k=-1;k<2;k=k+2)
				{
					C3Vector cp2(cp1);
					cp2(2)+=quaterSize*float(k);
					if (_childVolumeNodes[(i+1)/2][(j+1)/2][(k+1)/2]!=NULL)
					{
						_childVolumeNodes[(i+1)/2][(j+1)/2][(k+1)/2]->getVisualBoundingBox(first,halfSize,cp2,level-1,minV,maxV);
					}
				}
			}
		}
	}
	else
	{
		if (first)
		{
			minV(0)=currentPosition(0)-halfSize;
			maxV(0)=currentPosition(0)+halfSize;
			minV(1)=currentPosition(1)-halfSize;
			maxV(1)=currentPosition(1)+halfSize;
			minV(2)=currentPosition(2)-halfSize;
			maxV(2)=currentPosition(2)+halfSize;
			first=false;
		}
		else
		{
			if (minV(0)>currentPosition(0)-halfSize)
				minV(0)=currentPosition(0)-halfSize;
			if (maxV(0)<currentPosition(0)+halfSize)
				maxV(0)=currentPosition(0)+halfSize;

			if (minV(1)>currentPosition(1)-halfSize)
				minV(1)=currentPosition(1)-halfSize;
			if (maxV(1)<currentPosition(1)+halfSize)
				maxV(1)=currentPosition(1)+halfSize;

			if (minV(2)>currentPosition(2)-halfSize)
				minV(2)=currentPosition(2)-halfSize;
			if (maxV(2)<currentPosition(2)+halfSize)
				maxV(2)=currentPosition(2)+halfSize;
		}
	}
}

void CVolumeNode::display(float currentSize,float visualResolution,const C3Vector& currentPosition,const std::vector<float>& viewFrustum)
{
	float halfSize=currentSize*0.5f;
	float quaterSize=halfSize*0.5f;
	float cumulDist;
	int res=_getSphereSituationRelativeToViewFrustum(1.7320508f*halfSize,currentPosition,viewFrustum,cumulDist);
	if (res==-1)
		return; // outside of view
	if ( (
		(_childVolumeNodes[0][0][0]!=NULL)||
		(_childVolumeNodes[0][0][1]!=NULL)||
		(_childVolumeNodes[0][1][0]!=NULL)||
		(_childVolumeNodes[0][1][1]!=NULL)||
		(_childVolumeNodes[1][0][0]!=NULL)||
		(_childVolumeNodes[1][0][1]!=NULL)||
		(_childVolumeNodes[1][1][0]!=NULL)||
		(_childVolumeNodes[1][1][1]!=NULL)  ))
	{
		if (cumulDist>600.0f*halfSize)
			return; // too small (visually)
		for (int i=-1;i<2;i=i+2)
		{
			C3Vector cp0(currentPosition);
			cp0(0)+=quaterSize*float(i);
			for (int j=-1;j<2;j=j+2)
			{
				C3Vector cp1(cp0);
				cp1(1)+=quaterSize*float(j);
				for (int k=-1;k<2;k=k+2)
				{
					C3Vector cp2(cp1);
					cp2(2)+=quaterSize*float(k);
					if (_childVolumeNodes[(i+1)/2][(j+1)/2][(k+1)/2]!=NULL)
					{
						_childVolumeNodes[(i+1)/2][(j+1)/2][(k+1)/2]->display(halfSize,visualResolution,cp2,viewFrustum);
					}
				}
			}
		}
	}
	else
	{
		if (cumulDist>600.0f*currentSize)
			return; // too small (visually)
		{
			glBegin(GL_QUADS);
			glNormal3f(-1.0f,0.0f,0.0f);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)-halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)-halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)+halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)+halfSize,currentPosition(2)-halfSize);

			glNormal3f(+1.0f,0.0f,0.0f);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)-halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)+halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)+halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)-halfSize,currentPosition(2)+halfSize);

			glNormal3f(0.0f,-1.0f,0.0f);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)-halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)-halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)-halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)-halfSize,currentPosition(2)+halfSize);

			glNormal3f(0.0f,+1.0f,0.0f);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)+halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)+halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)+halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)+halfSize,currentPosition(2)-halfSize);

			glNormal3f(0.0f,0.0f,-1.0f);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)-halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)+halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)+halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)-halfSize,currentPosition(2)-halfSize);

			glNormal3f(0.0f,0.0f,+1.0f);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)-halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)-halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)+halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)+halfSize,currentPosition(2)+halfSize);
			glEnd();
		}
	}
}

void CVolumeNode::displayWireframe(bool first,float currentSize,float visualResolution,const C3Vector& currentPosition)
{
	float halfSize=currentSize*0.5f;
	float quaterSize=halfSize*0.5f;
	bool childNodesPresent=(
		(_childVolumeNodes[0][0][0]!=NULL)||
		(_childVolumeNodes[0][0][1]!=NULL)||
		(_childVolumeNodes[0][1][0]!=NULL)||
		(_childVolumeNodes[0][1][1]!=NULL)||
		(_childVolumeNodes[1][0][0]!=NULL)||
		(_childVolumeNodes[1][0][1]!=NULL)||
		(_childVolumeNodes[1][1][0]!=NULL)||
		(_childVolumeNodes[1][1][1]!=NULL)  );
	if (childNodesPresent)
	{
		for (int i=-1;i<2;i=i+2)
		{
			C3Vector cp0(currentPosition);
			cp0(0)+=quaterSize*float(i);
			for (int j=-1;j<2;j=j+2)
			{
				C3Vector cp1(cp0);
				cp1(1)+=quaterSize*float(j);
				for (int k=-1;k<2;k=k+2)
				{
					C3Vector cp2(cp1);
					cp2(2)+=quaterSize*float(k);
					if (_childVolumeNodes[(i+1)/2][(j+1)/2][(k+1)/2]!=NULL)
					{
						_childVolumeNodes[(i+1)/2][(j+1)/2][(k+1)/2]->displayWireframe(false,halfSize,visualResolution,cp2);
					}
				}
			}
		}
	}
	if (first||(visualResolution>=currentSize))
	{
		if (first)//level!=0)
		{
			glBegin(GL_QUADS);
			glNormal3f(-1.0f,0.0f,0.0f);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)-halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)-halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)+halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)+halfSize,currentPosition(2)-halfSize);

			glNormal3f(+1.0f,0.0f,0.0f);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)-halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)+halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)+halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)-halfSize,currentPosition(2)+halfSize);

			glNormal3f(0.0f,-1.0f,0.0f);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)-halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)-halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)-halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)-halfSize,currentPosition(2)+halfSize);

			glNormal3f(0.0f,+1.0f,0.0f);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)+halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)+halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)+halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)+halfSize,currentPosition(2)-halfSize);

			glNormal3f(0.0f,0.0f,-1.0f);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)-halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)+halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)+halfSize,currentPosition(2)-halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)-halfSize,currentPosition(2)-halfSize);

			glNormal3f(0.0f,0.0f,+1.0f);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)-halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)-halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)+halfSize,currentPosition(1)+halfSize,currentPosition(2)+halfSize);
			glVertex3f(currentPosition(0)-halfSize,currentPosition(1)+halfSize,currentPosition(2)+halfSize);
			glEnd();
		}
		else
		{
			glBegin(GL_POINTS);
			glNormal3fv(_normalVector.data);
			glVertex3f(currentPosition(0),currentPosition(1),currentPosition(2));
			glEnd();
		}
	}
	first=false;
}


CVolumeNode* CVolumeNode::copyYourself()
{
	CVolumeNode* newVolumeNode=new CVolumeNode();
	for (int i=0;i<2;i++)
	{
		for (int j=0;j<2;j++)
		{
			for (int k=0;k<2;k++)
			{
				if (_childVolumeNodes[i][j][k]!=NULL)
					newVolumeNode->_childVolumeNodes[i][j][k]=_childVolumeNodes[i][j][k]->copyYourself();
			}
		}
	}
	newVolumeNode->_normalVector=_normalVector;
	return(newVolumeNode);
}

void CVolumeNode::serialize(CSer& ar,int version)
{
	if (ar.isStoring())
	{	// Storing
		char state=0;
		for (int i=0;i<2;i++)
		{
			for (int j=0;j<2;j++)
			{
				for (int k=0;k<2;k++)
				{
					if (_childVolumeNodes[i][j][k]!=NULL)
						SIM_SET_BIT(state,i+j*2+k*4);
				}
			}
		}
		ar << state;

		// We save the approximated normal vector:
		//******************
		C3Vector n(_normalVector);
		n*=15.0f;
		n(0)+=0.5f*n(0)/fabsf(n(0));
		n(1)+=0.5f*n(1)/fabsf(n(1));
		n(2)+=0.5f*n(2)/fabsf(n(2));
		char x=char(n(0));
		char y=char(n(1));
		char z=char(n(2));
		WORD w=(x+15)+((y+15)<<5)+((z+15)<<10);
		ar << w;
		//******************

		for (int i=0;i<2;i++)
		{
			for (int j=0;j<2;j++)
			{
				for (int k=0;k<2;k++)
				{
					if (_childVolumeNodes[i][j][k]!=NULL)
						_childVolumeNodes[i][j][k]->serialize(ar,version);
				}
			}
		}
	}
	else
	{ // Loading
		char state=0;
		ar >> state;

		// We load the approximated normal vector:
		//******************
		WORD w;
		ar >> w;
		char x=(w&0x001f)-15;
		char y=((w>>5)&0x001f)-15;
		char z=((w>>10)&0x001f)-15;
		_normalVector.set((float)x,(float)y,(float)z);
		_normalVector.normalize();
		//******************

		for (int i=0;i<2;i++)
		{
			for (int j=0;j<2;j++)
			{
				for (int k=0;k<2;k++)
				{
					if (SIM_IS_BIT_SET(state,i+j*2+k*4))
					{
						_childVolumeNodes[i][j][k]=new CVolumeNode();
						_childVolumeNodes[i][j][k]->serialize(ar,version);
					}
				}
			}
		}
	}
}

bool CVolumeNode::cutVolume(float currentSize,float resolution,const C3Vector& currentPosition,const std::vector<float>& planes,char& somethingCut)
{ // return value true mean: this node has to be destroyed (empty)
	float halfSize=currentSize*0.5f;
	float quaterSize=halfSize*0.5f;

	// First we don't care whether this node has children. We check this node against the planes:
	int resTest=_getSphereSituationRelativeToVolume(1.7320508f*halfSize,currentPosition,planes,_normalVector);
	if (resTest==-1)
		return(false); // the whole box lies outside. We don't do anything
	if (resTest==1)
	{
		somethingCut=1;
		return(true); // the whole box lies inside. We want this node destroyed by the calling function
	}

	// Now we explore the node:
	if (
		(_childVolumeNodes[0][0][0]!=NULL)||
		(_childVolumeNodes[0][0][1]!=NULL)||
		(_childVolumeNodes[0][1][0]!=NULL)||
		(_childVolumeNodes[0][1][1]!=NULL)||
		(_childVolumeNodes[1][0][0]!=NULL)||
		(_childVolumeNodes[1][0][1]!=NULL)||
		(_childVolumeNodes[1][1][0]!=NULL)||
		(_childVolumeNodes[1][1][1]!=NULL)  )
	{
		int emptyCount=0;
		for (int i=-1;i<2;i=i+2)
		{
			C3Vector cp0(currentPosition);
			cp0(0)+=quaterSize*float(i);
			for (int j=-1;j<2;j=j+2)
			{
				C3Vector cp1(cp0);
				cp1(1)+=quaterSize*float(j);
				for (int k=-1;k<2;k=k+2)
				{
					C3Vector cp2(cp1);
					cp2(2)+=quaterSize*float(k);
					if (_childVolumeNodes[(i+1)/2][(j+1)/2][(k+1)/2]!=NULL)
					{
						if (_childVolumeNodes[(i+1)/2][(j+1)/2][(k+1)/2]->cutVolume(halfSize,resolution,cp2,planes,somethingCut))
						{ // we have to remove that node:
							somethingCut=1;
							delete _childVolumeNodes[(i+1)/2][(j+1)/2][(k+1)/2];
							_childVolumeNodes[(i+1)/2][(j+1)/2][(k+1)/2]=NULL;
							emptyCount++;
						}
					}
					else
						emptyCount++;
				}
			}
		}
		return(emptyCount>=8);
	}
	else
	{ // This node is intact. We might have to divide or remove it completely
		if (currentSize>resolution)
		{
			// Now we might have to divide that volume. We first test if this is really needed:
			if (_cutVolumeTest(2,currentSize,resolution,currentPosition,planes)!=-1)
			{ // we now have to create 8 sub-volumes:
				for (int i=0;i<2;i++)
				{
					for (int j=0;j<2;j++)
					{
						for (int k=0;k<2;k++)
							_childVolumeNodes[i][j][k]=new CVolumeNode(halfSize,halfSize);
					}
				}
				if (cutVolume(currentSize,resolution,currentPosition,planes,somethingCut))
				{
					somethingCut=1;
					return(true); // will be cut (This can happen unfortunately (we built the nodes then destroy them all))
				}
				return(false);
			}
			else
				return(false); // will not be cut!
		}
		else
		{
			if (_isPointTouchingVolume(currentPosition,&planes,_normalVector)) // we reached the desired resolution level. We now just test the box center:
			{
				somethingCut=1;
				return(true);
			}
			return(false);
		}
	}
}

int CVolumeNode::_getBoxSituationRelativeToVolume(float boxHalfSize,const C3Vector& boxPosition,const std::vector<float>& planes)
{
	// 'planes' contain a collection of plane definitions:
	// Each plane is defined by 4 values a, b, c & d (consecutive in the array):
	// ax+by+cz+d=0
	// The normal vector for each plane (a,b,c) should point outside of the volume
	// The volume has to be convex (but not obligatory closed)
	// The routine returns -1 if the box is definitively outside, 0 if we don't know and 1 if definitively inside

	C3Vector edges[8];
	C3Vector v0(boxHalfSize,0.0f,0.0f);
	C3Vector v1(0.0f,boxHalfSize,0.0f);
	C3Vector v2(0.0f,0.0f,boxHalfSize);
	edges[0]=v0+v1+v2+boxPosition;
	edges[1]=v0+v1-v2+boxPosition;
	edges[2]=v0-v1+v2+boxPosition;
	edges[3]=v0-v1-v2+boxPosition;
	edges[4]=v1+v2+boxPosition-v0;
	edges[5]=v1-v2+boxPosition-v0;
	edges[6]=v2+boxPosition-v0-v1;
	edges[7]=boxPosition-v0-v1-v2;
	bool cutByAnyPlane=false;
	for (int i=0;i<int(planes.size())/4;i++)
	{
		C3Vector abc(&planes[4*i+0]);
		float d=planes[4*i+3];
		float tmp;
		unsigned char outsideCount=0;
		unsigned char insideCount=0;
		for (int j=0;j<8;j++)
		{
			tmp=(abc*edges[j]+d);
			if (tmp>=0.0f)
				outsideCount++;
			else
				insideCount++;
		}
		if (insideCount==0)
			return(-1); // definitively outside (within tolerance)
		cutByAnyPlane|=(outsideCount!=0);
	}
	if (cutByAnyPlane)
		return(0); // We don't know!
	return(1); // definitively inside!
}

int CVolumeNode::_cutVolumeTest(int level,float currentSize,float resolution,const C3Vector& currentPosition,const std::vector<float>& planes)
{ // this checks if the volume will be cut, without building new nodes. Ret vals: -1 not cut, 0=maybe cut, 1 cut
	float halfSize=currentSize*0.5f;
	float quaterSize=halfSize*0.5f;
	level--;
	for (int i=-1;i<2;i=i+2)
	{
		C3Vector cp0(currentPosition);
		cp0(0)+=quaterSize*float(i);
		for (int j=-1;j<2;j=j+2)
		{
			C3Vector cp1(cp0);
			cp1(1)+=quaterSize*float(j);
			for (int k=-1;k<2;k=k+2)
			{
				C3Vector cp2(cp1);
				cp2(2)+=quaterSize*float(k);
				if (currentSize>resolution)
				{
					C3Vector dummy;
					int res=_getSphereSituationRelativeToVolume(1.7320508f*halfSize,cp2,planes,dummy);
					if (res==1)
						return(1); // we can directly return, will be cut
					if (res==0)
					{ // we don't know. we have to further explore if we can 
						if (level>0)
						{ // yes, we continue exploration
							int res=_cutVolumeTest(level,halfSize,resolution,cp2,planes);
							if (res>=0)
								return(res); // we can directly return (will be cut, or maybe)
						}
						else
							return(0); // we don't know, but we can directly return
					}
				}
				else
				{
					if (CCollDistInterface::isPointTouchingVolume(cp2,&planes)) // we reached the desired resolution level. We now just test the box center
						return(1); // will be cut
					return(-1); // not cut
				}
			}
		}
	}
	return(-1); // not cut
}
