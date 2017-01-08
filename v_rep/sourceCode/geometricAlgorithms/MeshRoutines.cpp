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
#include "MeshRoutines.h"
#include "MeshManip.h"
#include "Tt.h"
#include <hacdHACD.h>
#include <hacdMicroAllocator.h>

extern "C" {
#include "mem.h"
#include "qHull/qset.h"
#include "libqhull.h"
#include "poly.h"
}

void CMeshRoutines::getEdgeFeatures(float* vertices,int verticesLength,int* indices,int indicesLength,
			std::vector<int>* theVertexIDs,std::vector<int>* theEdgeIDs,std::vector<int>* theFaceIDs,
			float angleTolerance,bool forDisplay)
{ // theVertexIDs, theEdgeIDs or theFaceIDs can be NULL
	// For each vertex, edge and face, an identifier will be associated:
	// Same triangle ID --> triangles belong to the same face (with given tolerance between normals)
	// Same edge ID --> edges belong to the same edge (with given tolerance). -1 --> edge is embedded in a face	
	// Vertex ID is -1 --> vertex is embedded in an edge or face
	// If for Display is true, we calculate edges for nice display, otherwise, we calculate edges that can be used for the dynamic collision rersponse algorithms

	std::vector<int> vertexIDs(verticesLength/3,0);
	std::vector<int> edgeIDs(indicesLength,-2);
	std::vector<int> faceIDs(indicesLength/3,-1);


	// We use a CMeshManip-object for faster access:
	CMeshManip mesh(vertices,verticesLength,indices,indicesLength);
	// First we group similar triangles according to a max. tolerance angle:
	const float cosToleranceAngle=cos(angleTolerance);
	int faceIDCounter=0;
	for (int i=0;i<indicesLength/3;i++)
	{
		if (faceIDs[i]==-1)
		{ // this triangle was not yet processed:
			C3Vector normalVect=mesh.faceNormals[i]; // We get its normal vector
			// We prepare recursion with the triangle itself (beginning):
			std::vector<int> neighbours;
			neighbours.push_back(i);
			// Now we recursively go through all its neighbours:
			while (neighbours.size()!=0)
			{
				int el=neighbours[neighbours.size()-1];
				neighbours.erase(neighbours.end()-1);
				faceIDs[el]=faceIDCounter;
				int indOr[3]={indices[3*el+0],indices[3*el+1],indices[3*el+2]};
				for (int ed=0;ed<3;ed++)
				{ // We check the triangle's 3 edges:
					int ind[3];
					int ked=ed;
					for (int led=0;led<3;led++)
					{
						ind[led]=indOr[ked];
						ked++;
						if (ked==3)
							ked=0;
					}
					CEdgeElement* edgeIt=mesh.edges[ind[0]];
					while (edgeIt!=NULL)
					{ // We have to check all triangles with same edges:
						if (edgeIt->vertex1==ind[1])
						{ // Same edge. We check if already processed:
							if ( (faceIDs[edgeIt->triangle]==-1) ||(forDisplay&&(i!=edgeIt->triangle)) )
							{ // Not yet processed! Is the angle between normals under tolerance?
								C3Vector normalVect2=mesh.faceNormals[edgeIt->triangle];
								if (normalVect.getAngle(normalVect2)<angleTolerance)
								{ // This triangle belongs to the same plane!
									if (!forDisplay)
										neighbours.push_back(edgeIt->triangle);
									// Now we have to mark the edge which is in common as non-existing (-1):
									edgeIDs[3*el+ed]=-1; // Edge from "From" triangle
									edgeIDs[3*edgeIt->triangle+edgeIt->pos]=-1; //Edge from "To" triangle
								}
							}
						}
						edgeIt=edgeIt->next;
					}
				}
			}
			faceIDCounter++;
		}
	}

	// Now we group similar edges according to a max. tolerance angle:
	int edgeIDCounter=0;
	for (int i=0;i<int(mesh.edges.size());i++) // In fact we could go to the half only!
	{
		CEdgeElement* edgeIt=mesh.edges[i];
		while (edgeIt!=NULL)
		{
			if (edgeIDs[3*edgeIt->triangle+edgeIt->pos]==-2)
			{ // This edge exists and wasn't yet processed:
				C3Vector normalVect=edgeIt->n;
				// We prepare recursion with the edge itself (beginning):
				std::vector<int> neighbours;
				neighbours.push_back(3*edgeIt->triangle+edgeIt->pos);
				// Now we recursively go through all its neighbours:
				while (neighbours.size()!=0)
				{
					int el=neighbours[neighbours.size()-1];
					int thePos=el%3;
					neighbours.erase(neighbours.end()-1);
					edgeIDs[el]=edgeIDCounter;
					// Now we check its neighbours on both sides:
					int pointIDs[2];
					pointIDs[0]=indices[el];
					if (thePos==2)
						pointIDs[1]=indices[el-2];
					else
						pointIDs[1]=indices[el+1];
					for (int side=0;side<2;side++)
					{
						int vertexID=pointIDs[side];
						CEdgeElement* edgeIt2=mesh.edges[vertexID];
						while (edgeIt2!=NULL)
						{
							if (edgeIDs[3*edgeIt2->triangle+edgeIt2->pos]==-2)
							{ // Wasn't processed yet
								C3Vector normalVect2=edgeIt2->n;
								float dd=normalVect*normalVect2; // Scalar product
								if ( (dd>cosToleranceAngle)||(dd<-cosToleranceAngle) )
								{ // This segment belongs to the same edge!
									neighbours.push_back(3*edgeIt2->triangle+edgeIt2->pos);
									// Now we have to disable the vertex (-1), but only if the two
									// edges have different coordinates:
									if ( ((pointIDs[0]==edgeIt2->vertex0)&&(pointIDs[1]==edgeIt2->vertex1))||
										((pointIDs[0]==edgeIt2->vertex1)&&(pointIDs[1]==edgeIt2->vertex0)) )
									{
									}
									else
										vertexIDs[vertexID]=-1;
								}
							}
							edgeIt2=edgeIt2->next;
						}
					}
				}
				edgeIDCounter++;
			}
			edgeIt=edgeIt->next;
		}
	}
	// Now we have to do a last thing: disable all vertices which have only disabled edges:
	for (int i=0;i<verticesLength/3;i++)
	{
		CEdgeElement* edgeIt=mesh.edges[i];
		bool hasActiveEdge=false;
		while (edgeIt!=NULL)
		{
			if (edgeIDs[3*edgeIt->triangle+edgeIt->pos]!=-1)
				hasActiveEdge=true;
			edgeIt=edgeIt->next;
		}
		if (!hasActiveEdge)
			vertexIDs[i]=-1; // We disable this point
	}

	if (theVertexIDs!=NULL)
	{
		theVertexIDs->clear();
		theVertexIDs->assign(vertexIDs.begin(),vertexIDs.end());
	}
	if (theEdgeIDs!=NULL)
	{
		theEdgeIDs->clear();
		theEdgeIDs->assign(edgeIDs.begin(),edgeIDs.end());
	}
	if (theFaceIDs!=NULL)
	{
		theFaceIDs->clear();
		theFaceIDs->assign(faceIDs.begin(),faceIDs.end());
	}
}

bool CMeshRoutines::getConvexHull(std::vector<float>* verticesInOut,std::vector<int>* indicesInOut)
{
	std::vector<float> outV;
	std::vector<int> outI;
	bool result;
	if (indicesInOut!=NULL)
		result=getConvexHull(verticesInOut,indicesInOut,&outV,&outI);
	else
		result=getConvexHull(verticesInOut,NULL,&outV,NULL);
	verticesInOut->clear();
	verticesInOut->assign(outV.begin(),outV.end());
	if (indicesInOut!=NULL)
	{
		indicesInOut->clear();
		indicesInOut->assign(outI.begin(),outI.end());
	}
	return(result);
}

bool CMeshRoutines::getConvexHull(std::vector<float>* verticesIn,std::vector<int>* indicesIn,
							std::vector<float>* verticesOut,std::vector<int>* indicesOut)
{
	if (indicesIn!=NULL)
		return(getConvexHull(&verticesIn->at(0),verticesIn->size(),&indicesIn->at(0),indicesIn->size(),verticesOut,indicesOut));
	else
		return(getConvexHull(&verticesIn->at(0),verticesIn->size(),NULL,0,verticesOut,indicesOut));
}
bool CMeshRoutines::getConvexHull(float* verticesIn,int verticesInLength,int* indicesIn,int indicesInLength,
						std::vector<float>* verticesOut,std::vector<int>* indicesOut)
{
	C4Vector q,qInv;
	q.setIdentity();
	qInv.setIdentity();
	for (int iteration=0;iteration<10;iteration++)
	{ // the qHull algo sometimes fails somehow (generates hulls with holes)
		coordT* points=new coordT[verticesInLength];
		for (int i=0;i<verticesInLength;i++)
			points[i]=verticesIn[i];
		verticesOut->clear();
		if (indicesOut!=NULL)
			indicesOut->clear();
		char* flags="qhull ";
		int curlong,totlong;     /* memory remaining after qh_memfreeshort */
		FILE *errfile= stderr;    /* error messages from qhull code */
		int exitcode= qh_new_qhull (3,verticesInLength/3,points,0,flags,NULL,errfile);
		C3Vector minV,maxV;
		if (!exitcode)
		{
			vertexT* vertex;
			vertexT* vertex0=NULL;
			std::map<int,int> mapping;
			int index=0;
			FORALLvertices 
			{
				if (vertex0==NULL)
					vertex0=vertex;
				C3Vector v(float(vertex->point[0]),float(vertex->point[1]),float(vertex->point[2]));
				verticesOut->push_back(v(0));
				verticesOut->push_back(v(1));
				verticesOut->push_back(v(2));
				if (index==0)
				{
					minV=v;
					maxV=v;
				}
				else
				{
					minV.keepMin(v);
					maxV.keepMax(v);
				}
				mapping[vertex->id]=index++;
			}

			if (indicesOut!=NULL)
			{
				facetT *facet;
				FORALLfacets 
				{
					setT* vertices=facet->vertices;
					index=0;
					while (true)
					{
						vertexT* v1=(vertexT*)vertices->e[0].p;
						vertexT* v2=(vertexT*)vertices->e[index+1].p;
						vertexT* v3=(vertexT*)vertices->e[index+2].p;
						if (v3==NULL)
							break;
						indicesOut->push_back(mapping[v1->id]);
						indicesOut->push_back(mapping[v2->id]);
						indicesOut->push_back(mapping[v3->id]);
						index++;
					}
				}
				// Make sure the normals are all pointing outwards:
				C3Vector center;
				center.clear();
				for (int i=0;i<int(verticesOut->size()/3);i++)
					center+=C3Vector(&verticesOut->at(3*i));
				center/=float(verticesOut->size()/3);
				for (int i=0;i<int(indicesOut->size()/3);i++)
				{
					int ind[3]={indicesOut->at(3*i+0),indicesOut->at(3*i+1),indicesOut->at(3*i+2)};
					C3Vector v0(&verticesOut->at(3*ind[0]));
					C3Vector v1(&verticesOut->at(3*ind[1]));
					C3Vector v2(&verticesOut->at(3*ind[2]));
					C3Vector w0(v1-v0);
					C3Vector w1(v2-v0);
					C3Vector n(w0^w1);
					C3Vector m(v0-center);
					if (n*m<0.0f)
					{ // flip side
						indicesOut->at(3*i+0)=ind[1];
						indicesOut->at(3*i+1)=ind[0];
					}
				}
			}
		}
		qh_freeqhull(!qh_ALL);                   /* free long memory  */
		qh_memfreeshort (&curlong, &totlong);    /* free short memory and memory allocator */
		delete[] points;
		if (indicesOut==NULL)
			break;
		if (verticesOut->size()==0)
			break; // error

		// We merge close vertices, in order to have less problems with tolerances (1% of the dimension of the hull):
		C3Vector dim(maxV-minV);
		CMeshManip::checkVerticesIndicesNormalsTexCoords(*verticesOut,*indicesOut,NULL,NULL,true,(dim(0)+dim(1)+dim(2))*0.01f/3.0f,false);

		if (checkIfConvex(*verticesOut,*indicesOut,0.001f)) // 0.1%
			break; // ok, the hull is watertight and "very" convex
		// The hull is NOT watertight!
		// We check the output vertices, rotate them by a random amount, and try again (rotation will be reversed before leaving the routine)
		for (int i=0;i<int(verticesOut->size()/3);i++)
		{
			C3Vector v(&verticesOut->at(3*i));
			v=qInv*v;
			verticesOut->at(3*i+0)=v(0);
			verticesOut->at(3*i+1)=v(1);
			verticesOut->at(3*i+2)=v(2);
		}
		q.buildRandomOrientation();
		qInv=q.getInverse();
		for (int i=0;i<int(verticesOut->size()/3);i++)
		{
			C3Vector v(&verticesOut->at(3*i));
			v=q*v;
			verticesOut->at(3*i+0)=v(0);
			verticesOut->at(3*i+1)=v(1);
			verticesOut->at(3*i+2)=v(2);
		}
		verticesIn=&verticesOut->at(0);
		verticesInLength=verticesOut->size();
		// now rerun the algo with the output of previous run
	//	printf("iteration: %i\n",iteration);
	}
	for (int i=0;i<int(verticesOut->size()/3);i++)
	{
		C3Vector v(&verticesOut->at(3*i));
		v=qInv*v;
		verticesOut->at(3*i+0)=v(0);
		verticesOut->at(3*i+1)=v(1);
		verticesOut->at(3*i+2)=v(2);
	}
	return(true);
}


int CMeshRoutines::convexDecompose(const float* vertices,int verticesLength,const int* indices,int indicesLength,std::vector<std::vector<float>*>& verticesList,std::vector<std::vector<int>*>& indicesList,size_t nClusters,double concavity,bool addExtraDistPoints,bool addFacesPoints,double ccConnectDist,size_t targetNTrianglesDecimatedMesh,size_t maxHullVertices,double smallestClusterThreshold)
{ // 2 100 0 1 1 30 2000
	FUNCTION_DEBUG;
   int retVal=0;

	std::vector< HACD::Vec3<HACD::Real> > points;
	std::vector< HACD::Vec3<long> > triangles;

	for (int i=0;i<verticesLength/3;i++)
	{
		HACD::Vec3<HACD::Real> v(vertices[3*i+0],vertices[3*i+1],vertices[3*i+2]);
		points.push_back(v);
	}
	for (int i=0;i<indicesLength/3;i++)
	{
		HACD::Vec3<long> t(indices[3*i+0],indices[3*i+1],indices[3*i+2]);
		triangles.push_back(t);
	}

	HACD::HeapManager * heapManager = HACD::createHeapManager(65536*(1000));

	HACD::HACD * const myHACD = HACD::CreateHACD(heapManager);
	myHACD->SetPoints(&points[0]);
	myHACD->SetNPoints(points.size());
	myHACD->SetTriangles(&triangles[0]);
	myHACD->SetNTriangles(triangles.size());
	myHACD->SetCompacityWeight(0.0001);
    myHACD->SetVolumeWeight(0.0);
    myHACD->SetConnectDist(ccConnectDist);               // if two connected components are seperated by distance < ccConnectDist
                                                        // then create a virtual edge between them so the can be merged during 
                                                        // the simplification process

    myHACD->SetNClusters(nClusters);                     // minimum number of clusters
    myHACD->SetNVerticesPerCH(maxHullVertices);                      // max of 100 vertices per convex-hull
	myHACD->SetConcavity(concavity);                     // maximum concavity
	myHACD->SetSmallClusterThreshold(smallestClusterThreshold);				 // threshold to detect small clusters
	myHACD->SetNTargetTrianglesDecimatedMesh(targetNTrianglesDecimatedMesh); // # triangles in the decimated mesh
//	myHACD->SetCallBack(&CallBack);
    myHACD->SetAddExtraDistPoints(addExtraDistPoints);   
    myHACD->SetAddFacesPoints(addFacesPoints); 

	{
	    myHACD->Compute();
    }

	nClusters = myHACD->GetNClusters();
    retVal=nClusters;

	for(size_t c = 0; c < nClusters; ++c)
	{
		size_t nPoints = myHACD->GetNPointsCH(c);
		size_t nTriangles = myHACD->GetNTrianglesCH(c);
		HACD::Vec3<HACD::Real> * pointsCH = new HACD::Vec3<HACD::Real>[nPoints];
		HACD::Vec3<long> * trianglesCH = new HACD::Vec3<long>[nTriangles];
		myHACD->GetCH(c, pointsCH, trianglesCH);
		std::vector<float>* _vert=new std::vector<float>;
		std::vector<int>* _ind=new std::vector<int>;
		for (int i=0;i<int(nPoints);i++)
		{
			_vert->push_back(float(pointsCH[i].X()));
			_vert->push_back(float(pointsCH[i].Y()));
			_vert->push_back(float(pointsCH[i].Z()));
		}
		for (int i=0;i<int(nTriangles);i++)
		{
			_ind->push_back(trianglesCH[i].X());
			_ind->push_back(trianglesCH[i].Y());
			_ind->push_back(trianglesCH[i].Z());
		}

		getConvexHull(_vert,_ind); // better results with that! (convex decomp. routine has large tolerance regarding convexivity)

		// We do some checkings on our own here, just in case:
		C3Vector mmin,mmax;
		for (int i=0;i<int(_vert->size()/3);i++)
		{
			C3Vector v(&_vert->at(3*i+0));
			if (i==0)
			{
				mmin=v;
				mmax=v;
			}
			else
			{
				mmin.keepMin(v);
				mmax.keepMax(v);
			}
		}
		C3Vector extent(mmax-mmin);
		if ((_ind->size()>=12)&&(SIM_MIN(SIM_MIN(extent(0),extent(1)),extent(2))>0.0001f))
		{
			verticesList.push_back(_vert);
			indicesList.push_back(_ind);
		}
		else
		{
			delete _vert;
			delete _ind;
		}
		delete [] pointsCH;
		delete [] trianglesCH;
	}


    HACD::DestroyHACD(myHACD);
    HACD::releaseHeapManager(heapManager);

	return(retVal);
}

void CMeshRoutines::_insertEdge(std::vector<std::vector<int>* >& allEdges,int vertexIndex1,int vertexIndex2,int triangleIndex)
{
	int minI=SIM_MIN(vertexIndex1,vertexIndex2);	
	int maxI=SIM_MAX(vertexIndex1,vertexIndex2);
	if (allEdges[minI]==NULL)
		allEdges[minI]=new std::vector<int>;
	allEdges[minI]->push_back(maxI);
	allEdges[minI]->push_back(triangleIndex);
}

int CMeshRoutines::_getTriangleIndexFromEdge(std::vector<std::vector<int>* >& allEdges,int vertexIndex1,int vertexIndex2,int triangleIndexToExclude)
{ // returns -1 if there is no such triangle, or more than 1
	int minI=SIM_MIN(vertexIndex1,vertexIndex2);	
	int maxI=SIM_MAX(vertexIndex1,vertexIndex2);
	if (allEdges[minI]==NULL)
		return(-1);
	int cnt=0;
	int retVal=-1;
	for (int i=0;i<int(allEdges[minI]->size())/2;i++)
	{
		if ( (maxI==allEdges[minI]->at(2*i+0))&&(triangleIndexToExclude!=allEdges[minI]->at(2*i+1)) )
		{
			cnt++;
			retVal=allEdges[minI]->at(2*i+1);
		}
	}
	if (cnt==1)
		return(retVal);
	return(-1);
}

bool CMeshRoutines::checkIfConvex(const std::vector<float>& vertices,const std::vector<int>& indices,float distanceToleranceInPercent)
{ // function has virtual/non-virtual counterpart!
	// We need to check if neighbouring triangle are in a convex config, and if all triangles share an edge with exactly another triangle
	// Finally, we also need to check if the shape is in fact two (or more) convex shapes (i.e. 2 merged convex shapes) (only for test a) )
	bool convex=true;
	// 1. build a fast index:
	std::vector<std::vector<int>* > allEdges(vertices.size()/3,NULL);
	for (int tri=0;tri<int(indices.size()/3);tri++)
	{
		int ind[3]={indices[3*tri+0],indices[3*tri+1],indices[3*tri+2]};
		_insertEdge(allEdges,ind[0],ind[1],tri);
		_insertEdge(allEdges,ind[1],ind[2],tri);
		_insertEdge(allEdges,ind[2],ind[0],tri);
	}


	// 2. Now check each triangle for connectivity (exactly 3 neighbours) and for convexity:
	// *********************************************************************************************
	// The convexity test can be done in 2 different ways:
	// a) check the angle between neighbouring triangle normal vectors
	// b) check for a maximum distance with a vertex outside of the half-spaces defining the convex shape
	// Test a) does not perform well because small neighbouring triangles with a large negative angle could still "look" convex and perform well as convex shapes
	// Test b) is much better for that, but we need a measure of what distance is tolerated. For that, we simple take a percentage of the averag bounding box side.
	// Test b) is however much slower than test a)..

	// We use test b):

	// Following is for test b)
	C3Vector minV,maxV;
	for (int i=0;i<int(vertices.size()/3);i++)
	{
		C3Vector v(&vertices[3*i]);
		if (i==0)
		{
			minV=v;
			maxV=v;
		}
		else
		{
			minV.keepMin(v);
			maxV.keepMax(v);
		}
	}
	C3Vector boxDim(maxV-minV);
	float toleratedDist=distanceToleranceInPercent*(boxDim(0)+boxDim(1)+boxDim(2))/3.0f;
	std::vector<float> planeDefinitions;

	// Following is for test a) and b), plus the connectivity test (exactly 3 neighbours):
	for (int tri=0;tri<int(indices.size()/3);tri++)
	{
		int ind[3]={indices[3*tri+0],indices[3*tri+1],indices[3*tri+2]};
		C3Vector p0(&vertices[3*ind[0]]);
		C3Vector p1(&vertices[3*ind[1]]);
		C3Vector p2(&vertices[3*ind[2]]);
		C3Vector v0(p1-p0);
		C3Vector v1(p2-p0);
		C3Vector n0(v0^v1);
		n0.normalize();

		// Following is for test b)
		float d=p0*n0;
		planeDefinitions.push_back(n0(0));
		planeDefinitions.push_back(n0(1));
		planeDefinitions.push_back(n0(2));
		planeDefinitions.push_back(d);

		// Check all 3 neighbouring triangles:
		for (int i=0;i<3;i++)
		{
			int j=i+1;
			if (j>2)
				j=0;
			int tri2=_getTriangleIndexFromEdge(allEdges,ind[i],ind[j],tri);
			if (tri2==-1)
			{ // the mesh is not closed!
				convex=false;
				break;
			}

			/* Following is for test a)
			int ind2[3]={indices[3*tri2+0],indices[3*tri2+1],indices[3*tri2+2]};
			int ptIndex=ind2[0];
			if ((ind2[1]!=ind[i])&&(ind2[1]!=ind[j]))
				ptIndex=ind2[1];
			if ((ind2[2]!=ind[i])&&(ind2[2]!=ind[j]))
				ptIndex=ind2[2];
			C3Vector q0(&vertices[3*ptIndex]);
			C3Vector w0(q0-p0);
			if (n0*w0>0.0f)
			{ // we have a concavity!! Check if we are under the tolerance:
				q0.set(&vertices[3*ind2[0]]);
				C3Vector q1(&vertices[3*ind2[1]]);
				C3Vector q2(&vertices[3*ind2[2]]);
				w0=q1-q0;
				C3Vector w1(q2-q0);
				C3Vector n1(w0^w1);
				if (n0*n1<=0.0f)
					convex=false; // Oh yes, more than 90 degrees!
				else
				{
					n1.normalize();
					if (n0.getAngle(n1)>0.00349f) // about 0.02 degrees
						convex=false;
				}
				if (!convex)
					break; // the test already failed
			}
			*/

		}
		if (!convex)
			break; // the test already failed
	}

	// Following is for test b):
	if (convex)
	{
		for (int vert=0;vert<int(vertices.size()/3);vert++)
		{
			C3Vector v(&vertices[3*vert+0]);
			for (int i=0;i<int(planeDefinitions.size()/4);i++)
			{
				float d=v(0)*planeDefinitions[4*i+0]+v(1)*planeDefinitions[4*i+1]+v(2)*planeDefinitions[4*i+2]-planeDefinitions[4*i+3];
				if (d>toleratedDist)
				{
					convex=false;
					break;
				}
			}
			if (!convex)
				break; // the test already failed
		}
	}
	// *********************************************************************************************

	// 3. Check if the shape contains 2 (or more) items:
	// This is important only with test a), but we also do it with test b) (e.g. for cases where two identical shapes are coincident)
	if (convex)
	{
		std::vector<bool> allTriangles(int(indices.size()/3),false);
		std::vector<int> trianglesToExplore;
		trianglesToExplore.push_back(0);
		allTriangles[0]=true;
		int triCount=1;
		while (trianglesToExplore.size()!=0)
		{ // iterative exploration
			int tri=trianglesToExplore[trianglesToExplore.size()-1];
			trianglesToExplore.pop_back();
			int ind[3]={indices[3*tri+0],indices[3*tri+1],indices[3*tri+2]};
			// Check all 3 neighbouring triangles:
			for (int i=0;i<3;i++)
			{
				int j=i+1;
				if (j>2)
					j=0;
				int tri2=_getTriangleIndexFromEdge(allEdges,ind[i],ind[j],tri);
				if (!allTriangles[tri2])
				{ // we haven't explored that one yet!
					triCount++;
					allTriangles[tri2]=true;
					trianglesToExplore.push_back(tri2);
				}
			}	
		}
		convex=(triCount==int(indices.size()/3));
	}

	// 4. Clean-up!
	for (int i=0;i<int(allEdges.size());i++)
	{
		if (allEdges[i]!=NULL)
			delete allEdges[i];
	}

	return(convex);
}

void CMeshRoutines::createCube(std::vector<float>& vertices,std::vector<int>& indices,const C3Vector& sizes,const int subdivisions[3])
{
	vertices.clear();
	indices.clear();
	int divX=subdivisions[0];
	int divY=subdivisions[1];
	int divZ=subdivisions[2];
	float xhSize=sizes(0)/2.0f;
	float yhSize=sizes(1)/2.0f;
	float zhSize=sizes(2)/2.0f;
	float xs=sizes(0)/((float)divX);
	float ys=sizes(1)/((float)divY);
	float zs=sizes(2)/((float)divZ);

	// We first create the vertices:
	//******************************
	// Plane1:
	for (int i=0;i<(divY+1);i++)
	{ // along y
		for (int j=0;j<(divX+1);j++)
		{ // along x
			tt::addToFloatArray(&vertices,-xhSize+j*xs,-yhSize+i*ys,-zhSize);	
		}
	}
	// Plane2:
	for (int i=1;i<(divZ+1);i++)
	{ // along z
		for (int j=0;j<(divX+1);j++)
		{ // along x
			tt::addToFloatArray(&vertices,-xhSize+j*xs,-yhSize,-zhSize+i*zs);	
		}
	}
	// Plane3:
	for (int i=1;i<(divZ+1);i++)
	{ // along z
		for (int j=0;j<(divX+1);j++)
		{ // along x
			tt::addToFloatArray(&vertices,-xhSize+j*xs,+yhSize,-zhSize+i*zs);	
		}
	}
	// Plane4:
	for (int i=1;i<divY;i++)
	{ // along y
		for (int j=0;j<(divX+1);j++)
		{ // along x
			tt::addToFloatArray(&vertices,-xhSize+j*xs,-yhSize+i*ys,+zhSize);	
		}
	}
	// Plane5:
	for (int i=1;i<divY;i++)
	{ // along y
		for (int j=1;j<divZ;j++)
		{ // along z
			tt::addToFloatArray(&vertices,-xhSize,-yhSize+i*ys,-zhSize+j*zs);	
		}
	}
	// Plane6:
	for (int i=1;i<divY;i++)
	{ // along y
		for (int j=1;j<divZ;j++)
		{ // along z
			tt::addToFloatArray(&vertices,+xhSize,-yhSize+i*ys,-zhSize+j*zs);	
		}
	}
	
	// Now we create the indices:
	//***************************
	// Plane1:
	for (int i=0;i<divY;i++)
	{ // along y
		for (int j=0;j<divX;j++)
		{ // along x
			tt::addToIntArray(&indices,i*(divX+1)+j+1,i*(divX+1)+j,(i+1)*(divX+1)+j+1);
			tt::addToIntArray(&indices,(i+1)*(divX+1)+j+1,i*(divX+1)+j,(i+1)*(divX+1)+j);
		}
	}
	// Plane2:
	int off=divY*(divX+1)+divX+1;
	for (int i=0;i<divX;i++)
	{ // along x
		tt::addToIntArray(&indices,i,i+1,off+i+1);
		tt::addToIntArray(&indices,i,off+i+1,off+i);
	}
	off=divY*(divX+1);
	for (int i=1;i<divZ;i++)
	{ // along y
		for (int j=0;j<divX;j++)
		{ // along x
			tt::addToIntArray(&indices,off+i*(divX+1)+j,off+i*(divX+1)+j+1,off+(i+1)*(divX+1)+j+1);
			tt::addToIntArray(&indices,off+i*(divX+1)+j,off+(i+1)*(divX+1)+j+1,off+(i+1)*(divX+1)+j);
		}
	}
	// Plane3:
	int oldOff=divY*(divX+1);
	off=(divY+1)*(divX+1)+divZ*(divX+1);
	for (int i=0;i<divX;i++)
	{ // along x
		tt::addToIntArray(&indices,oldOff+i+1,oldOff+i,off+i+1);
		tt::addToIntArray(&indices,off+i+1,oldOff+i,off+i);
	}
	off=(divY+1)*(divX+1)+(divZ-1)*(divX+1);
	for (int i=1;i<divZ;i++)
	{ // along y
		for (int j=0;j<divX;j++)
		{ // along x
			tt::addToIntArray(&indices,off+i*(divX+1)+j+1,off+i*(divX+1)+j,off+(i+1)*(divX+1)+j+1);
			tt::addToIntArray(&indices,off+(i+1)*(divX+1)+j+1,off+i*(divX+1)+j,off+(i+1)*(divX+1)+j);
		}
	}
	// Plane4:
	oldOff=(divY+1)*(divX+1)+divZ*(divX+1)+(divZ-1)*(divX+1);
	if (divY>=2)
		off=(divY+1)*(divX+1)+2*divZ*(divX+1)+(divY-2)*(divX+1);
	else
		off=(divY+1)*(divX+1)+(divZ-1)*(divX+1);
	for (int i=0;i<divX;i++)
	{ // along x
		tt::addToIntArray(&indices,oldOff+i+1,oldOff+i,off+i+1);
		tt::addToIntArray(&indices,off+i+1,oldOff+i,off+i);
	}
	if (divY>=2)
	{
		oldOff=(divY+1)*(divX+1)+2*divZ*(divX+1);
		off=(divY+1)*(divX+1)+(divZ-1)*(divX+1);
		for (int i=0;i<divX;i++)
		{ // along x
			tt::addToIntArray(&indices,oldOff+i+1,oldOff+i,off+i+1);
			tt::addToIntArray(&indices,off+i+1,oldOff+i,off+i);
		}
	}
	off=(divY+1)*(divX+1)+2*divZ*(divX+1)-(divX+1);
	for (int i=1;i<divY-1;i++)
	{ // along y
		for (int j=0;j<divX;j++)
		{ // along x
			tt::addToIntArray(&indices,off+i*(divX+1)+j,off+i*(divX+1)+j+1,off+(i+1)*(divX+1)+j+1);
			tt::addToIntArray(&indices,off+i*(divX+1)+j,off+(i+1)*(divX+1)+j+1,off+(i+1)*(divX+1)+j);
		}
	}
	// Plane5:
	if (divZ==1)
	{ // No vertical division
		if (divY==1)
		{ // No division in Y -> here we have only one face
			tt::addToIntArray(&indices,(divX+1)*divY,0,(divY+1)*(divX+1)+2*divZ*(divX+1)-divX-1);	
			tt::addToIntArray(&indices,(divY+1)*(divX+1)+2*divZ*(divX+1)-divX-1,0,(divY+1)*(divX+1)+(divZ-1)*(divX+1));	
		}
		else
		{
			tt::addToIntArray(&indices,(divX+1)*divY,(divX+1)*(divY-1),(divY+1)*(divX+1)+2*divZ*(divX+1)-divX-1);	
			tt::addToIntArray(&indices,(divX+1)*(divY+1)+2*divZ*(divX+1)+(divY-2)*(divX+1),(divY+1)*(divX+1)+2*divZ*(divX+1)-divX-1,(divX+1)*(divY-1));	
			tt::addToIntArray(&indices,(divY+1)*(divX+1)+2*divZ*(divX+1),0,(divY+1)*(divX+1)+(divZ-1)*(divX+1));	
			tt::addToIntArray(&indices,divX+1,0,(divY+1)*(divX+1)+2*divZ*(divX+1));	
		}
		int up=(divY+1)*(divX+1)+2*divZ*(divX+1);
		int down=divX+1;
		for (int i=0;i<divY-2;i++)
		{
			tt::addToIntArray(&indices,down+i*(divX+1),up+i*(divX+1),up+(i+1)*(divX+1));	
			tt::addToIntArray(&indices,down+(i+1)*(divX+1),down+i*(divX+1),up+(i+1)*(divX+1));	
		}
	}
	else
	{ // Vertical division
		if (divY==1)
		{ // No division in Y
			tt::addToIntArray(&indices,divY*(divX+1),0,(divY+1)*(divX+1));	
			tt::addToIntArray(&indices,divY*(divX+1),(divY+1)*(divX+1),(divY+1)*(divX+1)+divZ*(divX+1));	
			tt::addToIntArray(&indices,(divY+1)*(divX+1)+2*divZ*(divX+1)-2*(divX+1),(divY+1)*(divX+1)+(divZ-1)*(divX+1),(divY+1)*(divX+1)+2*divZ*(divX+1)-(divX+1));	
			tt::addToIntArray(&indices,(divY+1)*(divX+1)+(divZ-1)*(divX+1),(divY+1)*(divX+1)+2*divZ*(divX+1)-2*(divX+1),(divY+1)*(divX+1)+(divZ-2)*(divX+1));	
			int left=(divY+1)*(divX+1);
			int right=(divY+1)*(divX+1)+divZ*(divX+1);
			for (int i=0;i<divZ-2;i++)
			{
				tt::addToIntArray(&indices,right+i*(divX+1),left+i*(divX+1),left+(i+1)*(divX+1));	
				tt::addToIntArray(&indices,right+(i+1)*(divX+1),right+i*(divX+1),left+(i+1)*(divX+1));	
			}
		}
		else
		{ // Here we have the very general case
			// The corners first:
			int pos=(divY+1)*(divX+1)+2*divZ*(divX+1)+(divY-1)*(divX+1);
			int posInt=(divY+1)*(divX+1)+2*divZ*(divX+1);
			tt::addToIntArray(&indices,divX+1,0,(divY+1)*(divX+1));	
			tt::addToIntArray(&indices,divX+1,(divY+1)*(divX+1),pos);	
			tt::addToIntArray(&indices,divY*(divX+1),(divY-1)*(divX+1),pos+(divY-2)*(divZ-1));	
			tt::addToIntArray(&indices,divY*(divX+1),pos+(divY-2)*(divZ-1),(divY+1)*(divX+1)+divZ*(divX+1));	
			tt::addToIntArray(&indices,pos+divZ-2,(divY+1)*(divX+1)+(divZ-2)*(divX+1),(divY+1)*(divX+1)+(divZ-1)*(divX+1));
			tt::addToIntArray(&indices,pos+divZ-2,(divY+1)*(divX+1)+(divZ-1)*(divX+1),posInt);
			tt::addToIntArray(&indices,posInt-2*(divX+1),pos+(divZ-1)*(divY-1)-1,posInt+(divY-2)*(divX+1));
			tt::addToIntArray(&indices,posInt-2*(divX+1),posInt+(divY-2)*(divX+1),posInt-(divX+1));
			// The down and up part:
			int down1=divX+1;
			int down2=pos;
			int up1=pos+divZ-2;
			int up2=posInt;
			for (int i=0;i<divY-2;i++)
			{
				tt::addToIntArray(&indices,down1+(i+1)*(divX+1),down1+i*(divX+1),down2+i*(divZ-1));	
				tt::addToIntArray(&indices,down1+(i+1)*(divX+1),down2+i*(divZ-1),down2+(i+1)*(divZ-1));	
				tt::addToIntArray(&indices,up1+(i+1)*(divZ-1),up1+i*(divZ-1),up2+i*(divX+1));	
				tt::addToIntArray(&indices,up1+(i+1)*(divZ-1),up2+i*(divX+1),up2+(i+1)*(divX+1));	
			}
			// The left and right part:
			int left1=(divY+1)*(divX+1);
			int left2=pos;
			int right1=pos+(divZ-1)*(divY-2);
			int right2=(divY+1)*(divX+1)+divZ*(divX+1);
			for (int i=0;i<divZ-2;i++)
			{
				tt::addToIntArray(&indices,left1+i*(divX+1),left1+(i+1)*(divX+1),left2+i);	
				tt::addToIntArray(&indices,left2+i,left1+(i+1)*(divX+1),left2+i+1);	
				tt::addToIntArray(&indices,right1+i,right1+i+1,right2+i*(divX+1));	
				tt::addToIntArray(&indices,right2+i*(divX+1),right1+i+1,right2+(i+1)*(divX+1));	
			}
		}
	}
	off=(divY+1)*(divX+1)+2*divZ*(divX+1)+(divY-1)*(divX+1);
	for (int i=0;i<divY-2;i++)
	{ // along y
		for (int j=0;j<divZ-2;j++)
		{ // along z
			tt::addToIntArray(&indices,off+i*(divZ-1)+j,off+i*(divZ-1)+j+1,off+(i+1)*(divZ-1)+j);
			tt::addToIntArray(&indices,off+i*(divZ-1)+j+1,off+(i+1)*(divZ-1)+j+1,off+(i+1)*(divZ-1)+j);
		}
	}

	// Plane6:
	if (divZ==1)
	{ // No vertical division
		if (divY==1)
		{ // No division in Y -> here we have only one face
			tt::addToIntArray(&indices,divX,(divX+1)*divY+divX,(divY+1)*(divX+1)+2*divZ*(divX+1)-1);	
			tt::addToIntArray(&indices,divX,(divY+1)*(divX+1)+2*divZ*(divX+1)-1,(divY+1)*(divX+1)+(divZ-1)*(divX+1)+divX);	
		}
		else
		{
			tt::addToIntArray(&indices,(divX+1)*(divY-1)+divX,(divX+1)*divY+divX,(divY+1)*(divX+1)+2*divZ*(divX+1)-1);	
			tt::addToIntArray(&indices,(divY+1)*(divX+1)+2*divZ*(divX+1)-1,(divX+1)*(divY+1)+2*divZ*(divX+1)+(divY-2)*(divX+1)+divX,(divX+1)*(divY-1)+divX);	
			tt::addToIntArray(&indices,divX,(divY+1)*(divX+1)+2*divZ*(divX+1)+divX,(divY+1)*(divX+1)+(divZ-1)*(divX+1)+divX);	
			tt::addToIntArray(&indices,divX,2*divX+1,(divY+1)*(divX+1)+2*divZ*(divX+1)+divX);	
		}
		int up=(divY+1)*(divX+1)+2*divZ*(divX+1)+divX;
		int down=2*divX+1;
		for (int i=0;i<divY-2;i++)
		{
			tt::addToIntArray(&indices,up+i*(divX+1),down+i*(divX+1),up+(i+1)*(divX+1));	
			tt::addToIntArray(&indices,down+i*(divX+1),down+(i+1)*(divX+1),up+(i+1)*(divX+1));	
		}
	}
	else
	{ // Vertical division
		if (divY==1)
		{ // No division in Y
			tt::addToIntArray(&indices,divX,divY*(divX+1)+divX,(divY+1)*(divX+1)+divX);	
			tt::addToIntArray(&indices,(divY+1)*(divX+1)+divX,divY*(divX+1)+divX,(divY+1)*(divX+1)+divZ*(divX+1)+divX);	
			tt::addToIntArray(&indices,(divY+1)*(divX+1)+(divZ-1)*(divX+1)+divX,(divY+1)*(divX+1)+2*divZ*(divX+1)-2*(divX+1)+divX,(divY+1)*(divX+1)+2*divZ*(divX+1)-1);	
			tt::addToIntArray(&indices,(divY+1)*(divX+1)+2*divZ*(divX+1)-2*(divX+1)+divX,(divY+1)*(divX+1)+(divZ-1)*(divX+1)+divX,(divY+1)*(divX+1)+(divZ-2)*(divX+1)+divX);	
			int left=(divY+1)*(divX+1)+divX;
			int right=(divY+1)*(divX+1)+divZ*(divX+1)+divX;
			for (int i=0;i<divZ-2;i++)
			{
				tt::addToIntArray(&indices,left+i*(divX+1),right+i*(divX+1),left+(i+1)*(divX+1));	
				tt::addToIntArray(&indices,right+i*(divX+1),right+(i+1)*(divX+1),left+(i+1)*(divX+1));	
			}
		}
		else
		{ // Here we have the very general case
			// The corners first:
			int pos=(divY+1)*(divX+1)+2*divZ*(divX+1)+(divY-1)*(divX+1)+(divY-1)*(divZ-1);
			int posInt=(divY+1)*(divX+1)+2*divZ*(divX+1)+divX;
			tt::addToIntArray(&indices,divX,2*divX+1,(divY+1)*(divX+1)+divX);	
			tt::addToIntArray(&indices,(divY+1)*(divX+1)+divX,2*divX+1,pos);	
			tt::addToIntArray(&indices,(divY-1)*(divX+1)+divX,divY*(divX+1)+divX,pos+(divY-2)*(divZ-1));	
			tt::addToIntArray(&indices,pos+(divY-2)*(divZ-1),divY*(divX+1)+divX,(divY+1)*(divX+1)+divZ*(divX+1)+divX);	
			tt::addToIntArray(&indices,(divY+1)*(divX+1)+(divZ-2)*(divX+1)+divX,pos+divZ-2,(divY+1)*(divX+1)+(divZ-1)*(divX+1)+divX);
			tt::addToIntArray(&indices,(divY+1)*(divX+1)+(divZ-1)*(divX+1)+divX,pos+divZ-2,posInt);
			tt::addToIntArray(&indices,pos+(divZ-1)*(divY-1)-1,posInt-2*(divX+1),posInt+(divY-2)*(divX+1));
			tt::addToIntArray(&indices,posInt+(divY-2)*(divX+1),posInt-2*(divX+1),posInt-(divX+1));
			// The down and up part:
			int down1=2*divX+1;
			int down2=pos;
			int up1=pos+divZ-2;
			int up2=posInt;
			for (int i=0;i<divY-2;i++)
			{
				tt::addToIntArray(&indices,down1+i*(divX+1),down1+(i+1)*(divX+1),down2+i*(divZ-1));	
				tt::addToIntArray(&indices,down2+i*(divZ-1),down1+(i+1)*(divX+1),down2+(i+1)*(divZ-1));	
				tt::addToIntArray(&indices,up1+i*(divZ-1),up1+(i+1)*(divZ-1),up2+i*(divX+1));	
				tt::addToIntArray(&indices,up2+i*(divX+1),up1+(i+1)*(divZ-1),up2+(i+1)*(divX+1));	
			}
			// The left and right part:
			int left1=(divY+1)*(divX+1)+divX;
			int left2=pos;
			int right1=pos+(divZ-1)*(divY-2);
			int right2=(divY+1)*(divX+1)+divZ*(divX+1)+divX;
			for (int i=0;i<divZ-2;i++)
			{
				tt::addToIntArray(&indices,left1+(i+1)*(divX+1),left1+i*(divX+1),left2+i);	
				tt::addToIntArray(&indices,left1+(i+1)*(divX+1),left2+i,left2+i+1);	
				tt::addToIntArray(&indices,right1+i+1,right1+i,right2+i*(divX+1));	
				tt::addToIntArray(&indices,right1+i+1,right2+i*(divX+1),right2+(i+1)*(divX+1));	
			}
		}
	}
	off=(divY+1)*(divX+1)+2*divZ*(divX+1)+(divY-1)*(divX+1)+(divZ-1)*(divY-1);
	for (int i=0;i<divY-2;i++)
	{ // along y
		for (int j=0;j<divZ-2;j++)
		{ // along z
			tt::addToIntArray(&indices,off+i*(divZ-1)+j+1,off+i*(divZ-1)+j,off+(i+1)*(divZ-1)+j);
			tt::addToIntArray(&indices,off+(i+1)*(divZ-1)+j+1,off+i*(divZ-1)+j+1,off+(i+1)*(divZ-1)+j);
		}
	}
}

void CMeshRoutines::createSphere(std::vector<float>& vertices,std::vector<int>& indices,const C3Vector& sizes,int sides,int faces)
{
	vertices.clear();
	indices.clear();
	float xhSize=sizes(0)/2.0f;
	float yhSize=sizes(1)/2.0f;
	float zhSize=sizes(2)/2.0f;

	float sa=2.0f*piValue/((float)sides);
	float fa=piValue/((float)faces);
	// We set up the vertices:
	tt::addToFloatArray(&vertices,0.0f,0.0f,1.0f);
	tt::addToFloatArray(&vertices,0.0f,0.0f,-1.0f);
	for (int i=0;i<sides;i++)
	{
		for (int j=1;j<faces;j++)
		{
			tt::addToFloatArray(&vertices,(float)(sin(fa*j)*cos(sa*i)),(float)(sin(fa*j)*sin(sa*i)),(float)cos(fa*j));	
		}
	}
	
	// We set up the indices:
	for (int i=0;i<sides-1;i++)
	{
		// First top and bottom part:
		tt::addToIntArray(&indices,0,i*(faces-1)+2,(i+1)*(faces-1)+2);
		tt::addToIntArray(&indices,(i+1)*(faces-1)+1,1,(i+2)*(faces-1)+1);
		for (int j=0;j<faces-2;j++)
		{ // Here the rest:
			tt::addToIntArray(&indices,i*(faces-1)+2+j,i*(faces-1)+2+j+1,(i+1)*(faces-1)+2+j);	
			tt::addToIntArray(&indices,i*(faces-1)+2+j+1,(i+1)*(faces-1)+2+j+1,(i+1)*(faces-1)+2+j);	
		}
	}
	// We have to close the sphere here:
	// First top and bottom part:
	tt::addToIntArray(&indices,0,(sides-1)*(faces-1)+2,2);
	tt::addToIntArray(&indices,sides*(faces-1)+1,1,(faces-1)+1);
	for (int j=0;j<faces-2;j++)
	{ // Here the rest:
		tt::addToIntArray(&indices,(sides-1)*(faces-1)+2+j,(sides-1)*(faces-1)+2+j+1,2+j);	
		tt::addToIntArray(&indices,(sides-1)*(faces-1)+2+j+1,2+j+1,2+j);	
	}
	// Now we scale the sphere:
	for (int i=0;i<int(vertices.size())/3;i++)
	{
		C3Vector p(vertices[3*i+0],vertices[3*i+1],vertices[3*i+2]);
		p(0)=p(0)*xhSize;
		p(1)=p(1)*yhSize;
		p(2)=p(2)*zhSize;
		vertices[3*i+0]=p(0);
		vertices[3*i+1]=p(1);
		vertices[3*i+2]=p(2);
	}
}

void CMeshRoutines::createCylinder(std::vector<float>& vertices,std::vector<int>& indices,const C3Vector& sizes,int sides,int faces,int discDiv,bool open,bool cone)
{
	vertices.clear();
	indices.clear();
	float xhSize=sizes(0)/2.0f;
	float yhSize=sizes(1)/2.0f;
	float zhSize=sizes(2)/2.0f;

	float zzz=1.0f/faces;
	float dd=1.0f/((float)discDiv);
	float sa=2.0f*piValue/((float)sides);
	int sideStart=0;
	// We set up the vertices:
	if (!open)
	{ // The two middle vertices:
		sideStart=2;
		tt::addToFloatArray(&vertices,0.0f,0.0f,0.5f);	
		tt::addToFloatArray(&vertices,0.0f,0.0f,-0.5f);
	}

	if (cone)
	{
		for (int i=0;i<sides;i++)
		{ // The side vertices:
			for (int j=0;j<faces+1;j++)
				tt::addToFloatArray(&vertices,(float)cos(sa*i)*j/faces,(float)sin(sa*i)*j/faces,0.5f-j*zzz);	
		}
	}
	else
	{
		for (int i=0;i<sides;i++)
		{ // The side vertices:
			for (int j=0;j<faces+1;j++)
				tt::addToFloatArray(&vertices,(float)cos(sa*i),(float)sin(sa*i),0.5f-j*zzz);	
		}
	}

	int dstStart=vertices.size()/3;
	int dsbStart=0;
	if (!open)
	{ // The disc subdivision vertices:
		for (int i=1;i<discDiv;i++)
			for (int j=0;j<sides;j++)
				tt::addToFloatArray(&vertices,(1.0f-dd*i)*(float)cos(sa*j),(1.0f-dd*i)*(float)sin(sa*j),0.5f);	
		dsbStart=vertices.size()/3;
		for (int i=1;i<discDiv;i++)
			for (int j=0;j<sides;j++)
				tt::addToFloatArray(&vertices,(1.0f-dd*i)*(float)cos(sa*j),(1.0f-dd*i)*(float)sin(sa*j),-0.5f);	
	}


	// We set up the indices:
	for (int i=0;i<sides-1;i++)
	{
		// First top and bottom part:
		if (!open)
		{
			if (discDiv==1)
			{
				tt::addToIntArray(&indices,0,i*(faces+1)+sideStart,(i+1)*(faces+1)+sideStart);
				tt::addToIntArray(&indices,(i+1)*(faces+1)+sideStart-1,1,(i+2)*(faces+1)+sideStart-1);
			}
			else
			{
				tt::addToIntArray(&indices,0,dstStart+i+sides*(discDiv-2),dstStart+i+sides*(discDiv-2)+1);
				tt::addToIntArray(&indices,dsbStart+i+sides*(discDiv-2),1,dsbStart+i+sides*(discDiv-2)+1);
				for (int j=0;j<discDiv-2;j++)
				{
					tt::addToIntArray(&indices,dstStart+j*sides+i,dstStart+j*sides+i+1,dstStart+(j+1)*sides+i);	
					tt::addToIntArray(&indices,dstStart+j*sides+i+1,dstStart+(j+1)*sides+i+1,dstStart+(j+1)*sides+i);	
					tt::addToIntArray(&indices,dsbStart+j*sides+i+1,dsbStart+j*sides+i,dsbStart+(j+1)*sides+i);	
					tt::addToIntArray(&indices,dsbStart+(j+1)*sides+i+1,dsbStart+j*sides+i+1,dsbStart+(j+1)*sides+i);	
				}
				tt::addToIntArray(&indices,sideStart+i*(faces+1),sideStart+(i+1)*(faces+1),dstStart+i);
				tt::addToIntArray(&indices,sideStart+(i+1)*(faces+1),dstStart+i+1,dstStart+i);
				tt::addToIntArray(&indices,sideStart+(i+2)*(faces+1)-1,sideStart+(i+1)*(faces+1)-1,dsbStart+i);
				tt::addToIntArray(&indices,dsbStart+i+1,sideStart+(i+2)*(faces+1)-1,dsbStart+i);
			}
		}
		for (int j=0;j<faces;j++)
		{ // Here the rest:
			tt::addToIntArray(&indices,i*(faces+1)+sideStart+j,i*(faces+1)+sideStart+j+1,(i+1)*(faces+1)+sideStart+j);	
			tt::addToIntArray(&indices,i*(faces+1)+sideStart+j+1,(i+1)*(faces+1)+sideStart+j+1,(i+1)*(faces+1)+sideStart+j);	
		}
	}

	// We have to close the cylinder here:
	// First top and bottom part:
	if (!open)
	{
		if (discDiv==1)
		{
			tt::addToIntArray(&indices,0,(sides-1)*(faces+1)+sideStart,sideStart);
			tt::addToIntArray(&indices,sides*(faces+1)+sideStart-1,1,(faces+1)+sideStart-1);
		}
		else
		{
			tt::addToIntArray(&indices,0,dstStart+sides*(discDiv-1)-1,dstStart+sides*(discDiv-2));
			tt::addToIntArray(&indices,dsbStart+sides*(discDiv-1)-1,1,dsbStart+sides*(discDiv-2));

			for (int j=0;j<discDiv-2;j++)
			{
				tt::addToIntArray(&indices,dstStart+j*sides+sides-1,dstStart+j*sides,dstStart+(j+2)*sides-1);	
				tt::addToIntArray(&indices,dstStart+j*sides,dstStart+(j+1)*sides,dstStart+(j+2)*sides-1);	
				tt::addToIntArray(&indices,dsbStart+j*sides,dsbStart+(j+1)*sides-1,dsbStart+(j+2)*sides-1);	
				tt::addToIntArray(&indices,dsbStart+(j+1)*sides,dsbStart+j*sides,dsbStart+(j+2)*sides-1);	
			}
			tt::addToIntArray(&indices,sideStart+(sides-1)*(faces+1),sideStart,dstStart+sides-1);
			tt::addToIntArray(&indices,sideStart,dstStart,dstStart+sides-1);
			tt::addToIntArray(&indices,sideStart+faces,sideStart+faces+(sides-1)*(faces+1),dsbStart+sides-1);
			tt::addToIntArray(&indices,dsbStart,sideStart+faces,dsbStart+sides-1);
		}
	}
	for (int j=0;j<faces;j++)
	{ // Here the rest:
		tt::addToIntArray(&indices,(sides-1)*(faces+1)+sideStart+j,(sides-1)*(faces+1)+sideStart+j+1,sideStart+j);	
		tt::addToIntArray(&indices,(sides-1)*(faces+1)+sideStart+j+1,sideStart+j+1,sideStart+j);	
	}

	if (cone)
	{ // We have a degenerate cylinder, we need to remove degenerate triangles and double vertices:
		CMeshManip::checkVerticesIndicesNormalsTexCoords(vertices,indices,NULL,NULL,true,0.0000001f,false);
	}

	// Now we scale the cylinder:
	for (int i=0;i<int(vertices.size())/3;i++)
	{
		C3Vector p(vertices[3*i+0],vertices[3*i+1],vertices[3*i+2]);
		p(0)=p(0)*xhSize;
		p(1)=p(1)*yhSize;
		p(2)=p(2)*2.0f*zhSize;
		vertices[3*i+0]=p(0);
		vertices[3*i+1]=p(1);
		vertices[3*i+2]=p(2);
	}
}

void CMeshRoutines::createAnnulus(std::vector<float>& vertices,std::vector<int>& indices,float Dlarge,float Dsmall,float zShift,int sides,bool faceUp)
{
	vertices.clear();
	indices.clear();
	float R=Dlarge*0.5f;
	float r=Dsmall*0.5f;

	float sa=2.0f*piValue/((float)sides);

	// We set up the vertices:
	for (int i=0;i<sides;i++)
		tt::addToFloatArray(&vertices,R*(float)cos(sa*i),R*(float)sin(sa*i),zShift);	
	for (int i=0;i<sides;i++)
		tt::addToFloatArray(&vertices,r*(float)cos(sa*i),r*(float)sin(sa*i),zShift);	

	// We set up the indices:
	for (int i=0;i<sides-1;i++)
	{
		if (faceUp)
		{
			tt::addToIntArray(&indices,i,i+sides,i+1);
			tt::addToIntArray(&indices,i+sides,i+sides+1,i+1);
		}
		else
		{
			tt::addToIntArray(&indices,i+sides,i,i+1);
			tt::addToIntArray(&indices,i+sides+1,i+sides,i+1);
		}
	}
	// the closing part:
	if (faceUp)
	{
		tt::addToIntArray(&indices,sides-1,2*sides-1,0);
		tt::addToIntArray(&indices,2*sides-1,sides,0);
	}
	else
	{
		tt::addToIntArray(&indices,2*sides-1,sides-1,0);
		tt::addToIntArray(&indices,sides,2*sides-1,0);
	}
}
