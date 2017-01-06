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
#include "Camera.h"
#include "v_rep_internal.h"
#include "Tt.h"
#include "OGL.h"
#include "IloIlo.h"
#include "MeshManip.h"
#include "Global.h"
#include "ObjectEdition.h"
#include "GraphingRoutines.h"
#include "pluginContainer.h"
#include "v_repStrings.h"
#include "VDateTime.h"
#include "ttUtil.h"
#include "easyLock.h"
#include "App.h"
#include "glShader.h"

unsigned int CCamera::selectBuff[SELECTION_BUFFER_SIZE*4];
//bool CCamera::openGlCameraViewCallbackEnabled=false;

CCamera::CCamera()
{
	commonInit();
}

std::string CCamera::getObjectTypeInfo()
{
	return(IDSOGL_CAMERA);
}
std::string CCamera::getObjectTypeInfoExtended()
{
	return(IDSOGL_CAMERA);
}

void CCamera::frameSceneOrSelectedObjects(float windowWidthByHeight,bool forPerspectiveProjection,std::vector<int>* selectedObjects,bool useSystemSelection,bool includeModelObjects,float scalingFactor)
{
	std::vector<float> pts;
	C7Vector camTr(getCumulativeTransformation());
	C7Vector camTrInv(camTr.getInverse());
	int editMode=App::ct->objCont->getEditModeType();
	if ((editMode&SHAPE_EDIT_MODE)!=0)
	{ // just take the vertices
		if (App::ct->objCont->_editionVertices.size()==0)
			editMode=NO_EDIT_MODE;
		else
		{
			C3DObject* parentObj=App::ct->objCont->getObject(App::ct->objCont->getEditModeObjectID());
			if (parentObj!=NULL)
			{
				C7Vector parentTr(parentObj->getCumulativeTransformation());
				if (editMode==VERTEX_EDIT_MODE)
				{
					for (int i=0;i<int(App::ct->objCont->_editionVertices.size())/3;i++)
					{
						C3Vector v(&App::ct->objCont->_editionVertices[3*i+0]);
						v=camTrInv*parentTr*v;
						pts.push_back(v(0));
						pts.push_back(v(1));
						pts.push_back(v(2));
					}
					scalingFactor*=0.98f;
				}
				else
				{ // Triangle or edge edit mode:
					if (App::ct->objCont->_editionIndices.size()!=0)
					{
						for (int i=0;i<int(App::ct->objCont->_editionIndices.size())/3;i++)
						{ // here we will get every vertex many times, but it doesn't really matter
							int ind[3]={App::ct->objCont->_editionIndices[3*i+0],App::ct->objCont->_editionIndices[3*i+1],App::ct->objCont->_editionIndices[3*i+2]};
							for (int j=0;j<3;j++)
							{
								C3Vector v(&App::ct->objCont->_editionVertices[3*ind[j]+0]);
								v=camTrInv*parentTr*v;
								pts.push_back(v(0));
								pts.push_back(v(1));
								pts.push_back(v(2));
							}
						}
						scalingFactor*=0.98f;
					}
					else
						editMode=NO_EDIT_MODE;
				}
			}
			else
				editMode=NO_EDIT_MODE;
		}
	}
	if ((editMode&PATH_EDIT_MODE)!=0)
	{ // just take the path points
		if (App::ct->objCont->_editionPath==NULL)
			editMode=NO_EDIT_MODE;
		else
		{
			int cnt=App::ct->objCont->_editionPath->getSimplePathPointCount();
			CPath* path=App::ct->objCont->getPath(App::ct->objCont->getEditModeObjectID());
			if ((cnt!=0)&&(path!=NULL))
			{
				C7Vector parentTr(path->getCumulativeTransformation());
				for (int i=0;i<cnt;i++)
				{
					CSimplePathPoint* pp=App::ct->objCont->_editionPath->getSimplePathPoint(i);
					C3Vector v(camTrInv*parentTr*pp->getTransformation().X);
					pts.push_back(v(0));
					pts.push_back(v(1));
					pts.push_back(v(2));
				}
				scalingFactor*=0.98f;
			}
			else
				editMode=NO_EDIT_MODE;
		}
	}
	if (editMode==NO_EDIT_MODE)
	{
		C3DObject* skybox=App::ct->objCont->getObject(IDSOGL_SKYBOX_DO_NOT_RENAME);
		// 1. List of all visible objects, excluding this camera, the skybox and objects flaged as "ignoreViewFitting":
		std::vector<C3DObject*> visibleObjs;
		for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
		{
			C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
			if (it!=this)
			{
				bool displayMaybe;
				if (App::ct->simulation->getDynamicContentVisualizationOnly())
					displayMaybe=it->getDynamicObjectFlag_forVisualization()!=0;
				else
					displayMaybe=((!it->isObjectPartOfInvisibleModel())&&(App::ct->mainSettings->activeLayers&it->layer));
				if (displayMaybe)
				{
					if ( ((skybox==NULL)||(!it->isObjectParentedWith(skybox)))&&(!it->getIgnoredByViewFitting()) )
						visibleObjs.push_back(it);
				}
			}
		}
		// 1.b if the list of visible objects is empty, include the objects normally ignored for view fitting!
		if (visibleObjs.size()==0)
		{
			for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
			{
				C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
				if (it!=this)
				{
					bool displayMaybe;
					if (App::ct->simulation->getDynamicContentVisualizationOnly())
						displayMaybe=it->getDynamicObjectFlag_forVisualization()!=0;
					else
						displayMaybe=((!it->isObjectPartOfInvisibleModel())&&(App::ct->mainSettings->activeLayers&it->layer));
					if (displayMaybe)
					{
						if ((skybox==NULL)||(!it->isObjectParentedWith(skybox)))
							visibleObjs.push_back(it);
					}
				}
			}
		}
		// 2. List of current selection, excluding this camera and the skybox objects:
		std::vector<int> tmp;
		if (useSystemSelection)
		{
			for (int i=0;i<App::ct->objCont->getSelSize();i++)
				tmp.push_back(App::ct->objCont->getSelID(i));
		}
		else
		{
			if (selectedObjects!=NULL)
				tmp.assign(selectedObjects->begin(),selectedObjects->end());
		}
		if (includeModelObjects)
			CObjectEdition::addRootObjectChildrenToSelection(tmp);
		std::vector<C3DObject*> selectionVisibleObjs;
		for (int i=0;i<int(tmp.size());i++)
		{ // We only wanna have visible objects, otherwise we get strange behaviour with some models!!
			C3DObject* it=App::ct->objCont->getObject(tmp[i]);
			if ((it!=NULL)&&(it!=this))
			{
				bool displayMaybe;
				if (App::ct->simulation->getDynamicContentVisualizationOnly())
					displayMaybe=it->getDynamicObjectFlag_forVisualization()!=0;
				else
					displayMaybe=((!it->isObjectPartOfInvisibleModel())&&(App::ct->mainSettings->activeLayers&it->layer));
				if (displayMaybe)
				{
					if ((skybox==NULL)||(!it->isObjectParentedWith(skybox)))
						selectionVisibleObjs.push_back(it);
				}
			}
		}
		// 3. set-up a list of all objects that should be framed:
		std::vector<C3DObject*> sel;
		if (selectionVisibleObjs.size()!=0)
			sel.assign(selectionVisibleObjs.begin(),selectionVisibleObjs.end());
		else
			sel.assign(visibleObjs.begin(),visibleObjs.end());
		// 4. Now do the calculations:
		// 4.a Get all relative coordinates of all oriented bounding box corners, or all relative vertices:
		for (int i=0;i<int(sel.size());i++)
		{
			C3DObject* it=sel[i];
			bool done=false;
			if (it->getObjectType()==sim_object_path_type)
			{
				done=true;
				CPath* path=(CPath*)it;
				int cnt=path->pathContainer->getSimplePathPointCount();
				if ((cnt!=0)&&(path!=NULL))
				{
					C7Vector parentTr(path->getCumulativeTransformation());
					for (int i=0;i<cnt;i++)
					{
						CSimplePathPoint* pp=path->pathContainer->getSimplePathPoint(i);
						C3Vector v(camTrInv*parentTr*pp->getTransformation().X);
						pts.push_back(v(0));
						pts.push_back(v(1));
						pts.push_back(v(2));
					}
				}
			}
			if (it->getObjectType()==sim_object_shape_type)
			{
				done=true;
				CShape* shape=(CShape*)it;
				C7Vector trr(camTrInv*shape->getCumulativeTransformation());
				std::vector<float> wvert;
				shape->geomData->geomInfo->getCumulativeMeshes(wvert,NULL,NULL);
				for (int j=0;j<int(wvert.size())/3;j++)
				{
					C3Vector vq(&wvert[3*j+0]);
					vq*=trr;
					pts.push_back(vq(0));
					pts.push_back(vq(1));
					pts.push_back(vq(2));
				}
			}
			if (it->getObjectType()==sim_object_graph_type)
			{ // here we add the 3D curves
				done=true;
				CGraph* gr=(CGraph*)it;

				C7Vector trr(camTrInv*gr->getCumulativeTransformation());

				for (int k=0;k<int(gr->threeDPartners.size());k++)
				{
					CGraphData* part0=gr->getGraphData(gr->threeDPartners[k]->data[0]);
					CGraphData* part1=gr->getGraphData(gr->threeDPartners[k]->data[1]);
					CGraphData* part2=gr->getGraphData(gr->threeDPartners[k]->data[2]);
					int pos=0;
					int absIndex;
					float point[3];
					bool cyclic0,cyclic1,cyclic2;
					float range0,range1,range2;
					if (part0!=NULL)	
						CGraphingRoutines::getCyclicAndRangeValues(part0,cyclic0,range0);
					if (part1!=NULL)	
						CGraphingRoutines::getCyclicAndRangeValues(part1,cyclic1,range1);
					if (part2!=NULL)	
						CGraphingRoutines::getCyclicAndRangeValues(part2,cyclic2,range2);
					while (gr->getAbsIndexOfPosition(pos++,absIndex))
					{
						bool dataIsValid=true;
						if (part0!=NULL)
						{
							if(!gr->getData(part0,absIndex,point[0],cyclic0,range0,true))
								dataIsValid=false;
						}
						else
							dataIsValid=false;
						if (part1!=NULL)
						{
							if(!gr->getData(part1,absIndex,point[1],cyclic1,range1,true))
								dataIsValid=false;
						}
						else
							dataIsValid=false;
						if (part2!=NULL)
						{
							if(!gr->getData(part2,absIndex,point[2],cyclic2,range2,true))
								dataIsValid=false;
						}
						else
							dataIsValid=false;
						if (dataIsValid)
						{
							C3Vector pp(point);
							if (gr->threeDPartners[k]->getCurveRelativeToWorld())
								pp=camTrInv*pp;
							else
								pp=trr*pp;
							pts.push_back(pp(0));
							pts.push_back(pp(1));
							pts.push_back(pp(2));
						}
					}
				}

				// Static 3D curves now:
				for (int k=0;k<int(gr->_staticCurves.size());k++)
				{
					CStaticGraphCurve* itg=gr->_staticCurves[k];
					if (itg->getCurveType()==2)
					{
						for (int j=0;j<int(itg->values.size()/3);j++)
						{
							C3Vector pp(itg->values[3*j+0],itg->values[3*j+1],itg->values[3*j+2]);
							if (itg->getRelativeToWorld())
								pp=camTrInv*pp;
							else
								pp=trr*pp;
							pts.push_back(pp(0));
							pts.push_back(pp(1));
							pts.push_back(pp(2));
						}
					}
				}

				// Now the graph object itself:
				if (gr->xYZPlanesDisplay)
				{
					C3Vector minV,maxV;
					it->getMarkingBoundingBox(minV,maxV);
					minV*=trr;
					maxV*=trr;
					for (int k=0;k<2;k++)
					{
						for (int l=0;l<2;l++)
						{
							for (int m=0;m<2;m++)
							{
								pts.push_back(minV(0)+(maxV(0)-minV(0))*k);
								pts.push_back(minV(1)+(maxV(1)-minV(1))*l);
								pts.push_back(minV(2)+(maxV(2)-minV(2))*m);
							}
						}
					}
				}


			}
			if (!done)
			{
				C3Vector minV,maxV;
				it->getBoundingBoxEncompassingBoundingBox(camTrInv,minV,maxV,false);
				for (int k=0;k<2;k++)
				{
					for (int l=0;l<2;l++)
					{
						for (int m=0;m<2;m++)
						{
							pts.push_back(minV(0)+(maxV(0)-minV(0))*k);
							pts.push_back(minV(1)+(maxV(1)-minV(1))*l);
							pts.push_back(minV(2)+(maxV(2)-minV(2))*m);
						}
					}
				}
			}
		}
	}

	if (pts.size()==0)
		return;

	if (getTrackedObjectID()!=-1)
	{ // When tracking an object, we should stay on the current view axis. To do this, we simply reflect all points left/right/top/bottom relative to the camera!
		std::vector<float> ptsC(pts);
		pts.clear();
		for (int i=0;i<int(ptsC.size())/3;i++)
		{
			C3Vector wv(&ptsC[3*i+0]);
			pts.push_back(wv(0));
			pts.push_back(wv(1));
			pts.push_back(wv(2));
			pts.push_back(-wv(0));
			pts.push_back(wv(1));
			pts.push_back(wv(2));
			pts.push_back(wv(0));
			pts.push_back(-wv(1));
			pts.push_back(wv(2));
			pts.push_back(-wv(0));
			pts.push_back(-wv(1));
			pts.push_back(wv(2));
		}
	}

	C3Vector relativeCameraTranslation;
	relativeCameraTranslation.clear();
	float nearClippingPlaneCorrection=0.0f;
	float farClippingPlaneCorrection=0.0f;
	float viewSizeCorrection=0.0f;
	if (forPerspectiveProjection)
	{
		C3Vector relativeCameraTranslation_verticalAdjustment;
		C3Vector relativeCameraTranslation_horizontalAdjustment;
		relativeCameraTranslation_verticalAdjustment.clear();
		relativeCameraTranslation_horizontalAdjustment.clear();
		std::vector<float> cop(pts);
		//*****************
		float effectiveAngle=getViewAngle()*scalingFactor;
		if (windowWidthByHeight>1.0f)
			effectiveAngle/=windowWidthByHeight;

		C4X4Matrix lowerLimit;
		lowerLimit.buildXRotation(effectiveAngle*0.5f);
		C4X4Matrix lowerLimitInv(lowerLimit.getInverse());
		C3Vector minw;
		C3Vector maxw;
		for (int i=0;i<int(pts.size())/3;i++)
		{
			C3Vector w(pts[3*i+0],pts[3*i+1],pts[3*i+2]);
			w*=lowerLimitInv;
			if (i==0)
			{
				minw=w;
				maxw=w;
			}
			else
			{
				minw.keepMin(w);
				maxw.keepMax(w);
			}
		}
		C4X4Matrix cm(getLocalTransformation().getMatrix()*lowerLimit);
		relativeCameraTranslation_verticalAdjustment=cm.M.axis[1]*minw(1);

		C3Vector camShift(lowerLimit.M.axis[1]*minw(1));
		for (int i=0;i<int(pts.size())/3;i++)
		{
			pts[3*i+0]-=camShift(0);
			pts[3*i+1]-=camShift(1);
			pts[3*i+2]-=camShift(2);
		}

		C4X4Matrix upperLimit;
		upperLimit.buildXRotation(-effectiveAngle*0.5f);
		C4X4Matrix upperLimitInv(upperLimit.getInverse());
		for (int i=0;i<int(pts.size())/3;i++)
		{
			C3Vector w(pts[3*i+0],pts[3*i+1],pts[3*i+2]);
			w*=upperLimitInv;
			if (i==0)
			{
				minw=w;
				maxw=w;
			}
			else
			{
				minw.keepMin(w);
				maxw.keepMax(w);
			}
		}

		cm=getLocalTransformation().getMatrix()*lowerLimit;
		float dist=-maxw(1)/sin(effectiveAngle);
		relativeCameraTranslation_verticalAdjustment+=cm.M.axis[2]*dist;

	// ***************** left-right

		pts.assign(cop.begin(),cop.end());

		effectiveAngle=getViewAngle()*scalingFactor;
		if (windowWidthByHeight<1.0f)
			effectiveAngle*=windowWidthByHeight;

		C4X4Matrix leftLimit;
		leftLimit.buildYRotation(effectiveAngle*0.5f);
		C4X4Matrix leftLimitInv(leftLimit.getInverse());
		for (int i=0;i<int(pts.size())/3;i++)
		{
			C3Vector w(pts[3*i+0],pts[3*i+1],pts[3*i+2]);
			w*=leftLimitInv;
			if (i==0)
			{
				minw=w;
				maxw=w;
			}
			else
			{
				minw.keepMin(w);
				maxw.keepMax(w);
			}
		}
		cm=getLocalTransformation().getMatrix()*leftLimit;
		relativeCameraTranslation_horizontalAdjustment=cm.M.axis[0]*maxw(0);

		camShift=leftLimit.M.axis[0]*maxw(0);
		for (int i=0;i<int(pts.size())/3;i++)
		{
			pts[3*i+0]-=camShift(0);
			pts[3*i+1]-=camShift(1);
			pts[3*i+2]-=camShift(2);
		}

		C4X4Matrix rightLimit;
		rightLimit.buildYRotation(-effectiveAngle*0.5f);
		C4X4Matrix rightLimitInv(rightLimit.getInverse());
		for (int i=0;i<int(pts.size())/3;i++)
		{
			C3Vector w(pts[3*i+0],pts[3*i+1],pts[3*i+2]);
			w*=rightLimitInv;
			if (i==0)
			{
				minw=w;
				maxw=w;
			}
			else
			{
				minw.keepMin(w);
				maxw.keepMax(w);
			}
		}

		cm=getLocalTransformation().getMatrix()*leftLimit;
		dist=minw(0)/sin(effectiveAngle);
		relativeCameraTranslation_horizontalAdjustment+=cm.M.axis[2]*dist;

		// Decide which coords to select
		cm=getLocalTransformation().getMatrix();
		C3Vector vert(cm.getInverse()*relativeCameraTranslation_verticalAdjustment);
		C3Vector horiz(cm.getInverse()*relativeCameraTranslation_horizontalAdjustment);
		C3Vector f;
		f(0)=horiz(0);
		f(1)=vert(1);
		f(2)=vert(2);
		if (horiz(2)<vert(2))
			f(2)=horiz(2);
		relativeCameraTranslation=cm*f;

		// Adjust the near and far clipping planes if needed:
		pts.assign(cop.begin(),cop.end());
		C4X4Matrix ncm(cm);
		ncm.X+=relativeCameraTranslation;
		C4X4Matrix tr(ncm.getInverse()*cm);
		for (int i=0;i<int(pts.size())/3;i++)
		{
			C3Vector w(pts[3*i+0],pts[3*i+1],pts[3*i+2]);
			w*=tr;
			if (i==0)
			{
				minw=w;
				maxw=w;
			}
			else
			{
				minw.keepMin(w);
				maxw.keepMax(w);
			}
		}
		float zCorr=0.0f;
		if (minw(2)<getNearClippingPlane())
			zCorr=minw(2)-getNearClippingPlane();
		relativeCameraTranslation+=cm.M.axis[2]*zCorr; // zCorr is negative or 0
		if (maxw(2)>getFarClippingPlane()+zCorr)
			farClippingPlaneCorrection=maxw(2)-zCorr-getFarClippingPlane();

	}
	else
	{ // orthographic projection:
		C3Vector minw;
		C3Vector maxw;
		for (int i=0;i<int(pts.size())/3;i++)
		{
			C3Vector w(pts[3*i+0],pts[3*i+1],pts[3*i+2]);
			if (i==0)
			{
				minw=w;
				maxw=w;
			}
			else
			{
				minw.keepMin(w);
				maxw.keepMax(w);
			}
		}
		C3Vector center((minw+maxw)/2.0f);
		// Move the camera to look at the center of those points (just 2 translations) + backshift if needed:
		float backShift=0.0f;
		if (minw(2)<getNearClippingPlane())
			backShift=minw(2)-getNearClippingPlane();
		C4X4Matrix cm(getLocalTransformation().getMatrix());
		relativeCameraTranslation=cm.M.axis[0]*center(0)+cm.M.axis[1]*center(1)+cm.M.axis[2]*backShift;
		if (maxw(2)>((getFarClippingPlane()+backShift)/1.2f))
			farClippingPlaneCorrection=(maxw(2)-(getFarClippingPlane()+backShift))*1.2f;
		float vs=getOrthoViewSize();
		float dx=(maxw(0)-minw(0))/scalingFactor;
		float dy=(maxw(1)-minw(1))/scalingFactor;
		float rap=9999999.0f;
		if (dy!=0.0f)
			rap=dx/dy;
		if (windowWidthByHeight>1.0f)
		{
			if (rap>=windowWidthByHeight)
				viewSizeCorrection=dx-vs;
			else
				viewSizeCorrection=(dy-vs/windowWidthByHeight)*windowWidthByHeight;
		}
		else
		{
			if (rap<=windowWidthByHeight)
				viewSizeCorrection=dy-vs;
			else
				viewSizeCorrection=(dx-vs*windowWidthByHeight)/windowWidthByHeight;
		}
	}
	C4X4Matrix cm(getLocalTransformation().getMatrix());
	cm.X+=relativeCameraTranslation;

	setNearClippingPlane(getNearClippingPlane()+nearClippingPlaneCorrection);
	setFarClippingPlane(getFarClippingPlane()+farClippingPlaneCorrection);
	setOrthoViewSize(getOrthoViewSize()+viewSizeCorrection);
	C3DObject* cameraParentProxy=NULL;
	if (getUseParentObjectAsManipulationProxy())
		cameraParentProxy=getParent();

	if (cameraParentProxy!=NULL)
	{ // We report the same camera opening to all cameras attached to cameraParentProxy
		if (!forPerspectiveProjection)
		{
			for (int i=0;i<int(cameraParentProxy->childList.size());i++)
			{
				if (cameraParentProxy->childList[i]->getObjectType()==sim_object_camera_type)
					((CCamera*)cameraParentProxy->childList[i])->setOrthoViewSize(getOrthoViewSize());
			}
		}
		cameraParentProxy->setLocalTransformation(cameraParentProxy->getLocalTransformation()*cm.getTransformation()*getLocalTransformation().getInverse());
	}
	else
		setLocalTransformation(cm.getTransformation());
}


void CCamera::commonInit()
{
	_backwardCompatibilityFlag20100713=false;
	_initialValuesInitialized=false;
	setObjectType(sim_object_camera_type);
	_nearClippingPlane=0.05f;
	_farClippingPlane=30.0f;
	cameraSize=0.05f;
	_viewAngle=60.0f*degToRad;
	_orthoViewSize=2.0f;
	_showFogIfAvailable=true;
	_useLocalLights=false;
	_allowPicking=true;
	hitForMouseUpProcessing_minus2MeansIgnore=-2;

	_cameraManipulationModePermissions=0x1f;
	trackedObjectIdentifier_NeverDirectlyTouch=-1;
	layer=CAMERA_LIGHT_LAYER;
	_mainPropertyLocal=0;
	_useParentObjectAsManipulationProxy=false;

	colorPart1.setDefaultValues();
	colorPart1.setColor(0.6f,0.0f,0.0f,AMBIENT_MODE);
	colorPart2.setDefaultValues();
	colorPart2.setColor(0.3f,0.3f,0.3f,AMBIENT_MODE);

	_objectName=IDSOGL_CAMERA;
}

void CCamera::setCameraManipulationModePermissions(int p)
{ // bit coded: own x, own y, own z, full rotation, tilting, never tilting
	// full rotation requires free x, y and z movement
	// never tilting and tilting cannot be activated together
	p&=0x03f;
	if (((_cameraManipulationModePermissions&0x008)==0)&&(p&0x008))
		p|=0x007; // we want full rotation! x,y and z have to be enabled
	if (((_cameraManipulationModePermissions&0x020)==0)&&(p&0x020))
		p&=0x02f; // we want to never tilt! disable manipulation tilting
	if (((_cameraManipulationModePermissions&0x010)==0)&&(p&0x010))
		p&=0x01f; // we want to tilt during manip! disable "never tilting"
	if ((p&0x007)!=0x007)
		p&=0x037; // We restrain x,y or z translation. disable full rotation
	_cameraManipulationModePermissions=p;
}

int CCamera::getCameraManipulationModePermissions()
{ // bit coded: own x, own y, own z, full rotation, tilting, never tilting
	return(_cameraManipulationModePermissions);
}

void CCamera::shiftCameraInCameraManipulationMode(const C3Vector& newLocalPos)
{
	C4X4Matrix oldLocal(_transformation.getMatrix());
	C4X4Matrix newLocal(oldLocal);
	newLocal.X=newLocalPos;
	C4X4Matrix tr(oldLocal.getInverse()*newLocal);
	if ((_cameraManipulationModePermissions&0x001)==0)
		tr.X(0)=0.0f;
	if ((_cameraManipulationModePermissions&0x002)==0)
		tr.X(1)=0.0f;
	if ((_cameraManipulationModePermissions&0x004)==0)
		tr.X(2)=0.0f;
	_transformation=oldLocal*tr;
}
void CCamera::rotateCameraInCameraManipulationMode(const C7Vector& newLocalConf)
{
	if (_cameraManipulationModePermissions&0x008)
		_transformation=newLocalConf;
}
void CCamera::tiltCameraInCameraManipulationMode(float tiltAmount)
{
	if (_cameraManipulationModePermissions&0x010)
	{
		C4X4Matrix oldLocal(_transformation.getMatrix());
		C3X3Matrix rot;
		rot.buildZRotation(tiltAmount);
		oldLocal.M=oldLocal.M*rot;
		_transformation=oldLocal;
	}
}

bool CCamera::getFullBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	minV(0)=-0.5f*cameraSize;
	maxV(0)=0.5f*cameraSize;
	minV(1)=-0.5f*cameraSize;
	maxV(1)=2.2f*cameraSize;
	minV(2)=-2.6f*cameraSize;
	maxV(2)=cameraSize;
	return(true);
}

bool CCamera::getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	return(getFullBoundingBox(minV,maxV));
}

CCamera::~CCamera()
{
}

bool CCamera::getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices)
{
	vertices.clear();
	indices.clear();
	return(false); // for now
}

void CCamera::display(CViewableBase* renderingObject,int displayAttrib)
{
	FUNCTION_INSIDE_DEBUG("CCamera::display");
	EASYLOCK(_objectMutex);
	// At the beginning of every 3DObject display routine:
//	CGlShader a;
//	if (_objectName.compare("Camera")==0)
//		a.bind();
	commonStart(displayAttrib);

	// Bounding box display:
	if (displayAttrib&sim_displayattribute_renderpass) 
		displayBoundingBox(displayAttrib);

	// Object display:
	if (getShouldObjectBeDisplayed(displayAttrib))
	{
		if ((App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
		{
			if (_objectProperty&sim_objectproperty_selectmodelbaseinstead)
				glLoadName(getModelSelectionID());
			else
				glLoadName(getID());
		}
		else
			glLoadName(-1);
		if ( (displayAttrib&sim_displayattribute_forcewireframe)&&(displayAttrib&sim_displayattribute_renderpass) )
			glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);

		App::ct->objCont->enableAuxClippingPlanes(_objectID);
		colorPart1.makeCurrentColor();
		glPushMatrix();
			glTranslatef(0.0f,0.0f,-cameraSize);
			ogl::displayBox(0.4f*cameraSize,cameraSize,2.0f*cameraSize,true);
		glPopMatrix();  
		glPushMatrix();
			colorPart2.makeCurrentColor();
			glTranslatef(0.0f,1.3f*cameraSize,-0.25f*cameraSize);
			glRotatef(90.0f,0,1,0);
			ogl::displayCylinder(2.0f*cameraSize,cameraSize/2.0f,20);
			glTranslatef(1.5f*cameraSize,0.0f,0.0f);
			ogl::displayCylinder(2.0f*cameraSize,cameraSize/2.0f,20);
		glPopMatrix();
		glTranslatef(0.0f,0.0f,cameraSize/6.0f);
		ogl::displayCone(cameraSize,5.0f*cameraSize/3.0f,20,true);
		App::ct->objCont->disableAuxClippingPlanes();
	}

	// At the end of every 3DObject display routine:
	commonFinish();
}

void CCamera::lookIn(int windowSize[2],CSView* subView,bool drawText,bool passiveSubView)
{ // drawText is false and passiveSubView is true by default
	FUNCTION_DEBUG;
	// Default values (used for instance in view selection mode)

	// This routine is quite messy and needs refactoring!!!
	bool isPerspective=true;
	int renderingMode=RENDERING_MODE_SOLID;
	bool displ_ref=false;
	int currentWinSize[2];
	int mouseRelativePosition[2];
	int mouseDownRelativePosition[2];
	int selectionStatus=NOSELECTION;
	bool mouseIsDown=false;
	bool mouseJustWentDown=false;
	bool mouseJustWentUp=false;
	bool mouseMovedWhileDown=false;
	int navigationMode=sim_navigation_passive;
	if (windowSize!=NULL)
	{
		currentWinSize[0]=windowSize[0];
		currentWinSize[1]=windowSize[1];
	}
	if (subView!=NULL)
	{
		isPerspective=subView->getPerspectiveDisplay();
		renderingMode=subView->getRenderingMode();
		displ_ref=true;
		subView->getViewSize(currentWinSize);
		subView->getMouseRelativePosition(mouseRelativePosition);
		subView->getMouseDownRelativePosition(mouseDownRelativePosition);
		if (!passiveSubView)
		{
			selectionStatus=subView->getSelectionStatus();
			mouseIsDown=subView->isMouseDown();
			mouseJustWentDown=subView->didMouseJustGoDown();
			mouseJustWentUp=subView->didMouseJustGoUp();
			mouseMovedWhileDown=subView->didMouseMoveWhileDown();
			navigationMode=App::ct->getMouseMode()&0x00ff;
		}
	}

	// Set data for view frustum culling
	_planesCalculated=false;
	_currentViewSize[0]=currentWinSize[0];
	_currentViewSize[1]=currentWinSize[1];
	_currentPerspective=isPerspective;

	// There is a maximum of 3 passes:
	// Pass 0 : DEPTHPASS : Pass used when doing depth perception
	//			Everything should be drawn as
	//			glPolygonMode (GL_FRONT_AND_BACK,GL_FILL)
	//			This pass is never displayed and can be skipped
	// Pass 1 : PICKPASS : Pass used for picking
	//			This pass might be displayed and can be skipped
	// Pass 2 : RENDERPASS : Pass which is always displayed and cannot be skipped

	int passes[3]={RENDERPASS,-1,-1};

	bool specialSelectionAndNavigationPass=false;
	bool regularObjectsCannotBeSelected=false;
	bool processHitForMouseUpProcessing=false;
	if (subView!=NULL)
	{
		if (hitForMouseUpProcessing_minus2MeansIgnore!=-2)
		{
			int mousePos[2];
			int mouseDownPos[2];
			subView->getMouseRelativePosition(mousePos);
			subView->getMouseDownRelativePosition(mouseDownPos);
			int dx[2]={mouseDownPos[0]-mousePos[0],mouseDownPos[1]-mousePos[1]};
			if (dx[0]*dx[0]+dx[1]*dx[1]>1)
				hitForMouseUpProcessing_minus2MeansIgnore=-2;
		}
		if (mouseJustWentDown)
		{
			hitForMouseUpProcessing_minus2MeansIgnore=-2;
			if (mouseJustWentUp)
			{
				processHitForMouseUpProcessing=true;
				if (selectionStatus==NOSELECTION)
				{
					passes[0]=DEPTHPASS;
					passes[1]=RENDERPASS;
				}
				else
				{
					passes[0]=PICKPASS;
					passes[1]=RENDERPASS;
				}
			}
			else
			{
				if (selectionStatus==NOSELECTION)
				{ // no ctrl or shift key
					passes[0]=DEPTHPASS;
					passes[1]=PICKPASS;
					passes[2]=RENDERPASS;
					if ((App::ct->getMouseMode()&sim_navigation_clickselection)==0)
						regularObjectsCannotBeSelected=true;
					else
						specialSelectionAndNavigationPass=true;
				}
				else if (selectionStatus==SHIFTSELECTION)
				{ // shift key
//					pass=RENDERPASS;
//					numberOfPasses=1;
				}
				else
				{ // ctrl key
					passes[0]=PICKPASS;
					passes[1]=RENDERPASS;
				}
			}
		}
		else if (mouseJustWentUp)
		{
			processHitForMouseUpProcessing=true;
			if (selectionStatus==SHIFTSELECTION)
			{
				if (((App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)!=0)&&App::mainWindow->ctrlKeyDown)
					passes[0]=COLORCODEDPICKPASS;
				else
					passes[0]=PICKPASS;
				passes[1]=RENDERPASS;
			}
			else
			{
//				pass=RENDERPASS;
//				numberOfPasses=1;
			}
		}
	}
	App::ct->objCont->setUniqueSelectedPathID(-1);



	for (int passIndex=0;passIndex<3;passIndex++)
	{
		int pass=passes[passIndex];
		if (pass==-1)
			break;

		if (pass==RENDERPASS)
			App::ct->environment->setBackgroundColor(currentWinSize);
		else
		{
			glClearColor(0.0,0.0,0.0,0.0);
			glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
		}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		if (pass==PICKPASS)
		{
			if (!_allowPicking)
				break;
			GLint viewport[4];
			glSelectBuffer(SELECTION_BUFFER_SIZE*4-4,selectBuff); //+4 is for...? Sometime
			// the application badly crashes during picking, and I guess it might be because
			// openGL is accessing forbidden memory... But I don't know!!

			glGetIntegerv(GL_VIEWPORT,viewport);
			glRenderMode(GL_SELECT);
			glInitNames();
			glPushName(0);			
			int mouseX=mouseRelativePosition[0];
			int mouseY=mouseRelativePosition[1];
			tt::limitValue(1,currentWinSize[0],mouseX);
			tt::limitValue(1,currentWinSize[1],mouseY);
			int pickSizeX=abs(mouseX-mouseDownRelativePosition[0]);
			int pickSizeY=abs(mouseY-mouseDownRelativePosition[1]);
			tt::limitValue(1,10000,pickSizeX);
			tt::limitValue(1,10000,pickSizeY);
			int centerX=(mouseX+mouseDownRelativePosition[0])/2;
			int centerY=(mouseY+mouseDownRelativePosition[1])/2;
			if (selectionStatus==SHIFTSELECTION)
				ogl::pickMatrixSpecial(centerX+viewport[0],centerY+viewport[1],pickSizeX,pickSizeY,viewport);	
			else
				ogl::pickMatrixSpecial(mouseX+viewport[0],mouseY+viewport[1],3,3,viewport);	
		}

		if (pass!=PICKPASS)
			glRenderMode(GL_RENDER);

		float ratio=(float)(currentWinSize[0]/(float)currentWinSize[1]);
		if (isPerspective)
		{
			if (ratio>1.0f)
			{
				float a=2.0f*(float)atan(tan(_viewAngle/2.0f)/ratio)*radToDeg;
				ogl::perspectiveSpecial(a,ratio,_nearClippingPlane,_farClippingPlane);
			}
			else
				ogl::perspectiveSpecial(_viewAngle*radToDeg,ratio,_nearClippingPlane,_farClippingPlane);
		}
		else
		{
			if (ratio>1.0f)
				glOrtho(-_orthoViewSize*0.5f,_orthoViewSize*0.5f,-_orthoViewSize*0.5f/ratio,_orthoViewSize*0.5f/ratio,ORTHO_CAMERA_NEAR_CLIPPING_PLANE,ORTHO_CAMERA_FAR_CLIPPING_PLANE);
			else
				glOrtho(-_orthoViewSize*0.5f*ratio,_orthoViewSize*0.5f*ratio,-_orthoViewSize*0.5f,_orthoViewSize*0.5f,ORTHO_CAMERA_NEAR_CLIPPING_PLANE,ORTHO_CAMERA_FAR_CLIPPING_PLANE);
		}
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	
		C4X4Matrix m4(getCumulativeTransformation_forDisplay(true).getMatrix());

		if (App::mainWindow->getHasStereo())
		{ // handle stereo cameras correctly
			C4X4Matrix displ;
			displ.setIdentity();
			if (App::mainWindow->getLeftEye())
				displ.X(0)=App::mainWindow->getStereoDistance()/2;
			else
				displ.X(0)=-App::mainWindow->getStereoDistance()/2;
			m4=m4*displ;
		}

		// The following 6 instructions have the same effect as gluLookAt()
		m4.inverse();
		m4.rotateAroundY(piValue);
		float m4_[4][4];
		m4.copyTo(m4_);
		CMeshManip::transposeMatrix_4x4Array(m4_);
		glLoadMatrixf((float*)m4_);

		if (pass==RENDERPASS)
		{
			glEnable(GL_LIGHTING);
			App::ct->objCont->actualizeLights(this,App::ct->simulation->getDynamicContentVisualizationOnly());
		}
		else
		{
			glDisable(GL_LIGHTING);
			App::ct->environment->activateAmbientLight(false);
			for (int i=0;i<CIloIlo::maximumOpenGlLights;i++)
				glDisable(GL_LIGHT0+i);
		}

		if ((pass==DEPTHPASS)||(pass==RENDERPASS))
		{
			C7Vector camTr(getCumulativeTransformation());
			C7Vector camTri(camTr.getInverse());

			// Handle mirrors here (not elegant at all, rewrite this later!!)
			if (App::ct->objCont->mirrorList.size()!=0)
			{
				setFrustumCullingTemporarilyDisabled(true);
				// Prep stencil buffer:
				glEnable(GL_STENCIL_TEST);
				glClearStencil(0);
				glClear (GL_STENCIL_BUFFER_BIT);
				int drawOk=1;

				std::vector<int> allMirrors;
				std::vector<float> allMirrorDist;
				for (int mir=0;mir<int(App::ct->objCont->mirrorList.size());mir++)
				{
					CMirror* myMirror=App::ct->objCont->getMirror(App::ct->objCont->mirrorList[mir]);
					C7Vector mmtr(myMirror->getCumulativeTransformation());
					mmtr=camTri*mmtr;

					if ( (!myMirror->isObjectPartOfInvisibleModel())&&(App::ct->mainSettings->activeLayers&myMirror->layer)&&myMirror->getIsMirror() )
					{
						allMirrors.push_back(App::ct->objCont->mirrorList[mir]);
						allMirrorDist.push_back(mmtr.X(2));
					}
				}
				tt::orderAscending(allMirrorDist,allMirrors);

				for (int mir=0;mir<int(allMirrors.size());mir++)
				{
					CMirror* myMirror=App::ct->objCont->getMirror(allMirrors[mir]);

					C7Vector mtr(myMirror->getCumulativeTransformation());
					C7Vector mtri(mtr.getInverse());
					C3Vector mtrN(mtr.Q.getMatrix().axis[2]);
					C4Vector mtrAxis=mtr.Q.getAngleAndAxisNoChecking();
					C4Vector mtriAxis=mtri.Q.getAngleAndAxisNoChecking();
					float d=(mtrN*mtr.X);
					C3Vector v0(+myMirror->getMirrorWidth()*0.5f,-myMirror->getMirrorHeight()*0.5f,0.0f);
					C3Vector v1(+myMirror->getMirrorWidth()*0.5f,+myMirror->getMirrorHeight()*0.5f,0.0f);
					C3Vector v2(-myMirror->getMirrorWidth()*0.5f,+myMirror->getMirrorHeight()*0.5f,0.0f);
					C3Vector v3(-myMirror->getMirrorWidth()*0.5f,-myMirror->getMirrorHeight()*0.5f,0.0f);
					v0*=mtr;
					v1*=mtr;
					v2*=mtr;
					v3*=mtr;

					C3Vector MirrCam(camTr.X-mtr.X);
					bool inFrontOfMirror=(((MirrCam*mtrN)>0.0f)&&myMirror->getActive()&&(!App::ct->mainSettings->mirrorsDisabled) );

					glStencilFunc(GL_ALWAYS, drawOk, drawOk); // we can draw everywhere
					glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // we draw drawOk where depth test passes
					glDepthMask(GL_FALSE);
					glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
					glBegin (GL_QUADS);
					glVertex3fv(v0.data);
					glVertex3fv(v1.data);
					glVertex3fv(v2.data);
					glVertex3fv(v3.data);
					glEnd ();
					glDepthMask(GL_TRUE);
					glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

					// Enable stencil masking:
					glStencilFunc(GL_EQUAL, drawOk, drawOk); // we draw only where stencil is drawOk
					glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

					// Draw the mirror view:
					if (inFrontOfMirror)
					{
						glEnable(GL_CLIP_PLANE0);
						double cpv[4]={-mtrN(0),-mtrN(1),-mtrN(2),d};
						glClipPlane(GL_CLIP_PLANE0,cpv);
						glPushMatrix();
						glTranslatef(mtr.X(0),mtr.X(1),mtr.X(2));
						glRotatef(mtrAxis(0)*radToDeg,mtrAxis(1),mtrAxis(2),mtrAxis(3));
						glScalef (1., 1., -1.);
						glTranslatef(mtri.X(0),mtri.X(1),mtri.X(2));
						glRotatef(mtriAxis(0)*radToDeg,mtriAxis(1),mtriAxis(2),mtriAxis(3));
						glFrontFace (GL_CW);
						CMirror::currentMirrorContentBeingRendered=myMirror->getID();
						drawObjects(renderingMode,selectionStatus==NOSELECTION,pass,navigationMode,currentWinSize,subView,true);
						CMirror::currentMirrorContentBeingRendered=-1;
						glFrontFace (GL_CCW);
						glPopMatrix();
						glDisable(GL_CLIP_PLANE0);
					}

					// Now draw the mirror overlay:
					glPushAttrib (0xffffffff);
					glDepthFunc(GL_ALWAYS);
					glDisable(GL_LIGHTING);
					if (inFrontOfMirror)
					{
						glEnable (GL_BLEND);
						glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glColor4f (myMirror->mirrorColor[0],myMirror->mirrorColor[1],myMirror->mirrorColor[2],1.0f-myMirror->getReflectance());
					}
					else
					{
						glColor4f (myMirror->mirrorColor[0],myMirror->mirrorColor[1],myMirror->mirrorColor[2],1.0f);
					}
					glBegin (GL_QUADS);
					glVertex3fv(v0.data);
					glVertex3fv(v1.data);
					glVertex3fv(v2.data);
					glVertex3fv(v3.data);
					glEnd ();
					glPopAttrib();
					glDepthFunc(GL_LEQUAL);
					drawOk++;
				}
				glDisable(GL_STENCIL_TEST);

				setFrustumCullingTemporarilyDisabled(false);

			}
		}



		bool secondPartOfManipOverlayNeeded=false;

		// Display the sphere for rotation and shift-operations:
		//-----------------------------------------------------------------------------------------------------
		if (mouseIsDown&&(!passiveSubView)&&(pass==RENDERPASS)&&(selectionStatus==NOSELECTION)&&( (navigationMode==sim_navigation_camerarotate)||
			(navigationMode==sim_navigation_camerashift)||(navigationMode==sim_navigation_cameratilt)||
			(navigationMode==sim_navigation_objectshift)||
			(navigationMode==sim_navigation_objectrotate) ) )
		{
			float centerPos[3];
			subView->getCenterPosition(centerPos);
			float mousePosDepth=subView->getMousePositionDepth();
			glLoadName(-1);
			ogl::setColorsAllBlack();
			if ((navigationMode==sim_navigation_camerarotate)||
				(navigationMode==sim_navigation_camerashift)||
				(navigationMode==sim_navigation_cameratilt))
			{
				ogl::setColor(1.0f,0.0f,0.0f,EMISSION_MODE);
				glPushMatrix();
				glTranslatef(centerPos[0],centerPos[1],centerPos[2]);
				float sphereRadius=10.0f;
				if (isPerspective)
				{
					if (ratio>1.0f)
						sphereRadius=mousePosDepth*tan(_viewAngle*0.5f)*sphereRadius/currentWinSize[0];
					else
						sphereRadius=mousePosDepth*tan(_viewAngle*0.5f)*sphereRadius/currentWinSize[1];
				}
				else
				{
					if (ratio>1.0f)
						sphereRadius=_orthoViewSize*0.5f*sphereRadius/currentWinSize[0];
					else
						sphereRadius=_orthoViewSize*0.5f*sphereRadius/currentWinSize[1];
				}
				if (CViewableBase::fogWasActivated)
					glDisable(GL_FOG);

				ogl::displaySphere(sphereRadius);
				glPopMatrix();

				// Following is a bit dirty, but quick ;)
				float clippNear=_nearClippingPlane;
				if (!isPerspective)
					clippNear=ORTHO_CAMERA_NEAR_CLIPPING_PLANE;
				if ( (mousePosDepth==clippNear)&&(navigationMode!=sim_navigation_cameratilt) )
				{ // We should display a differentiated thing here (kind of half-error!)
					C7Vector cct(getCumulativeTransformationPart1());
					C7Vector icct(cct.getInverse());
					C3Vector c(centerPos);
					C3Vector rc(icct*c);
					float di=2.0f;
					for (int jk=0;jk<2;jk++)
					{
						for (float i0=-di;i0<2.2f*di;i0+=2.0f*di)
						{
							for (float j0=-di;j0<2.2f*di;j0+=2.0f*di)
							{
								c=rc+C3Vector(i0*sphereRadius,j0*sphereRadius,0.0f);
								c=cct*c;
								glPushMatrix();
								glTranslatef(c(0),c(1),c(2));
								ogl::displaySphere(sphereRadius);
								glPopMatrix();
							}
						}
						di*=2.0f;
					}
				}
				else
				{ // we display some neat wireframe!
				}
				if (CViewableBase::fogWasActivated)
					glEnable(GL_FOG);
			}
			else
			{ // We only display the manip mode overlay grids (no more green ball):
				secondPartOfManipOverlayNeeded=true;
				ogl::setColorsAllBlack();
				for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
				{
					C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
					it->displayManipulationModeOverlayGrid(false);
				}
			}
		}
		//-----------------------------------------------------------------------------------------------------

		// very normal rendering:
		drawObjects(renderingMode,selectionStatus==NOSELECTION,pass,navigationMode,currentWinSize,subView,false);

		if (secondPartOfManipOverlayNeeded)
		{
			ogl::setColorsAllBlack();
			const float black[]={0.0f,0.0f,0.0f,0.25f};
			glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,black);

			for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
			{
				C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
				it->displayManipulationModeOverlayGrid(true);
			}
		}

		if (pass==DEPTHPASS)
		{
			performDepthPerception(subView,isPerspective);
			// The following instruct. is important depending on the depth-testing type
			// we perform. It can be omitted with glDepthFunc(GL_LEQUAL)
			App::ct->environment->setBackgroundColor(currentWinSize);
		}
		if (pass==PICKPASS)
			hitForMouseUpProcessing_minus2MeansIgnore=handleHits(glRenderMode(GL_RENDER),selectBuff);

		if (pass==COLORCODEDPICKPASS)
		{
			GLint viewport[4];
			glGetIntegerv(GL_VIEWPORT,viewport);

			int mouseX=mouseRelativePosition[0];
			int mouseY=mouseRelativePosition[1];
			tt::limitValue(1,currentWinSize[0],mouseX);
			tt::limitValue(1,currentWinSize[1],mouseY);
			int pickSizeX=abs(mouseX-mouseDownRelativePosition[0]);
			int pickSizeY=abs(mouseY-mouseDownRelativePosition[1]);
			tt::limitValue(1,10000,pickSizeX);
			tt::limitValue(1,10000,pickSizeY);
			unsigned char* bf=new unsigned char[pickSizeX*pickSizeY*3];
			glReadPixels(SIM_MIN(mouseX,mouseDownRelativePosition[0])+viewport[0],SIM_MIN(mouseY,mouseDownRelativePosition[1])+viewport[1],pickSizeX,pickSizeY,GL_RGB,GL_UNSIGNED_BYTE,bf);
			std::vector<bool> sel;
			sel.resize(1000000,false);
			for (int i=0;i<pickSizeX*pickSizeY;i++)
			{
				unsigned int v=bf[3*i+0]+(bf[3*i+1]<<8)+(bf[3*i+2]<<16);
				if (v<1000000)
					sel[v]=true;
			}

			for (int i=1;i<1000000;i++)
			{
				if (sel[i])
					App::ct->objCont->addItemToEditModeBuffer(i-1);
			}

			delete[] bf;
		}

		if (processHitForMouseUpProcessing)
		{
			if (((App::ct->getMouseMode()&0x0300)&sim_navigation_clickselection)&&(App::mainWindow!=NULL)&&(!App::mainWindow->ctrlKeyDown)&&(!App::mainWindow->shiftKeyDown)&&(hitForMouseUpProcessing_minus2MeansIgnore!=-2))
				handleMouseUpHit(hitForMouseUpProcessing_minus2MeansIgnore);
			hitForMouseUpProcessing_minus2MeansIgnore=-2;
			processHitForMouseUpProcessing=false;
		}

		if (pass==RENDERPASS)
		{
			glDisable(GL_FOG);

//*************************************************
// Following allows to hand the rendered camera image to a plugin, that might modify it and return it!
//*************************************************
			static int prevResX=-1;
			static int prevResY=-1;
			static char* buff=NULL;
			if (CPluginContainer::shouldSend_openglcameraview_msg())
//			if (openGlCameraViewCallbackEnabled)
			{
				if ((prevResX!=_currentViewSize[0])||(prevResY!=_currentViewSize[1]))
				{
					delete[] buff;
					buff=new char[_currentViewSize[0]*_currentViewSize[1]*3];
				}

				GLint viewport[4];
				glGetIntegerv(GL_VIEWPORT,viewport);

				glReadPixels(viewport[0],viewport[1], _currentViewSize[0], _currentViewSize[1], GL_RGB, GL_UNSIGNED_BYTE, buff);

				int auxVals[4];
				int retVals[4];
				auxVals[0]=_currentViewSize[0];
				auxVals[1]=_currentViewSize[1];
				auxVals[2]=-1;
				if (subView!=NULL)
					auxVals[2]=subView->getViewIndex();
				auxVals[3]=0;
				CPluginContainer::sendSpecialEventCallbackMessageToSomePlugins(sim_message_eventcallback_openglcameraview,auxVals,buff,retVals);
				if (auxVals[3]!=0)
				{ // we want to apply a new image!
					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					glOrtho(0,_currentViewSize[0],0,_currentViewSize[1],-100,100);
					glMatrixMode(GL_MODELVIEW);
					glLoadIdentity ();
					glDisable(GL_DEPTH_TEST);
					ogl::setColor(1.0f,1.0f,1.0f,AMBIENT_MODE);

					GLuint _oglTextureName;
					glGenTextures(1,&_oglTextureName);
					glBindTexture(GL_TEXTURE_2D,_oglTextureName);
					glPixelStorei(GL_UNPACK_ALIGNMENT,1);
					glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,_currentViewSize[0],_currentViewSize[1],0,GL_RGB,GL_UNSIGNED_BYTE,buff);
					glPixelStorei(GL_UNPACK_ALIGNMENT,4); // important to restore!

					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // keep to GL_LINEAR here!!
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
					glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
					glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
					glTexEnvi (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D,_oglTextureName);

					glColor3f(1.0f,1.0f,1.0f);

					glBegin(GL_QUADS);
					glTexCoord2f(0.0f,0.0f);
					glVertex3i(0,0,0);
					glTexCoord2f(0.0f,1.0f);
					glVertex3i(0,_currentViewSize[1],0);
					glTexCoord2f(1.0f,1.0f);
					glVertex3i(_currentViewSize[0],_currentViewSize[1],0);
					glTexCoord2f(1.0f,0.0f);
					glVertex3i(_currentViewSize[0],0,0);
					glEnd();

					glDisable(GL_TEXTURE_2D);
					glDeleteTextures(1,&_oglTextureName);
					glEnable(GL_DEPTH_TEST);
				}
			}
			else
			{
				delete[] buff;
				buff=NULL;
				prevResX=-1;
			}
//*************************************************

			drawOverlay(passiveSubView,drawText,displ_ref,currentWinSize,subView);

		}
		glRenderMode(GL_RENDER);	
		pass++;
	}


}

void CCamera::drawObjects(int renderingMode,bool noSelection,int pass,int navigationMode,int currentWinSize[2],CSView* subView,bool mirrored)
{ // subView can be NULL!!! (e.g. view selector)
	FUNCTION_DEBUG;

	// This routine is also quite dirty and requires refactoring!!

	int displayAttrib=sim_displayattribute_renderpass;
	if (pass==DEPTHPASS)
		displayAttrib=sim_displayattribute_depthpass;
	if (mirrored)
		displayAttrib=sim_displayattribute_mirror;
	if (pass==PICKPASS)
		displayAttrib=sim_displayattribute_pickpass;
	if (pass==COLORCODEDPICKPASS)
		displayAttrib=sim_displayattribute_colorcodedpickpass;
	if (((renderingMode==RENDERING_MODE_WIREFRAME_EDGES)||(renderingMode==RENDERING_MODE_WIREFRAME_TRIANGLES)||(renderingMode==RENDERING_MODE_BOUNDINGBOX_WIREFRAME))&&(pass==RENDERPASS))
	{
		displayAttrib|=sim_displayattribute_forcewireframe;
		if (renderingMode==RENDERING_MODE_WIREFRAME_TRIANGLES)
			displayAttrib|=sim_displayattribute_trianglewireframe;
	}
	if ( (renderingMode==RENDERING_MODE_BOUNDINGBOX_SOLID)||(renderingMode==RENDERING_MODE_BOUNDINGBOX_WIREFRAME) )
		displayAttrib|=sim_displayattribute_simplifyasboundingbox;

	if (App::ct->simulation->getDynamicContentVisualizationOnly())
		displayAttrib|=sim_displayattribute_dynamiccontentonly;

	int viewIndex=-1;
	if (subView!=NULL)
	{
		if (!subView->getShowEdges())
			displayAttrib|=sim_displayattribute_forbidedges;
		if (subView->getThickEdges())
			displayAttrib|=sim_displayattribute_thickEdges;
		viewIndex=subView->getViewIndex();
	}

	bool shapeEditModeAndPicking=( ((App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)!=0)&&((pass==PICKPASS)||(pass==COLORCODEDPICKPASS)) );


	// Prepare picking identifiers
	glInitNames();
	glPushName(-1);

	glShadeModel(GL_SMOOTH);

	if ((displayAttrib&(sim_displayattribute_colorcoded|sim_displayattribute_colorcodedpickpass))==0)
		glEnable(GL_DITHER);
	else
		glDisable(GL_DITHER);

	ogl::lockColors(false);

	// If selection size is bigger than 10, we set up a fast index:
	std::vector<BYTE>* selMap=NULL; // An arry which will show which object is selected
	if (App::ct->objCont->getSelSize()>10)
	{
		selMap=new std::vector<BYTE>(App::ct->objCont->getHighestObjectID()+1,0);
		for (int i=0;i<App::ct->objCont->getSelSize();i++)
			selMap->at(App::ct->objCont->getSelID(i))=1;
	}
	int lastSel=-1;
	if (App::ct->objCont->getSelSize()>0)
		lastSel=App::ct->objCont->getSelID(App::ct->objCont->getSelSize()-1);
	std::vector<C3DObject*> toRender;
	std::vector<int> transparentObjects;
	std::vector<float> transparentObjectDist;
	C7Vector camTrInv(getCumulativeTransformationPart1().getInverse());
	C3DObject* viewBoxObject=NULL;
	for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
	{
		C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
		if (it->getObjectType()==sim_object_shape_type)
		{
			CShape* sh=(CShape*)it;
			if (sh->getContainsTransparentComponent())
			{
				C7Vector obj(it->getCumulativeTransformationPart1());
				transparentObjectDist.push_back(-(camTrInv*obj).X(2));
				transparentObjects.push_back(it->getID());
			}
			else
				toRender.push_back(it);
		}
		else
		{
			if (it->getObjectType()==sim_object_mirror_type)
			{
				CMirror* mir=(CMirror*)it;
				if (mir->getContainsTransparentComponent())
				{
					C7Vector obj(it->getCumulativeTransformationPart1());
					transparentObjectDist.push_back(-(camTrInv*obj).X(2));
					transparentObjects.push_back(it->getID());
				}
				else
					toRender.push_back(it);
			}
			else
				toRender.push_back(it);
		}
		if (it->getName()==IDSOGL_SKYBOX_DO_NOT_RENAME)
			viewBoxObject=it;
	}
	tt::orderAscending(transparentObjectDist,transparentObjects);
	for (int i=0;i<int(transparentObjects.size());i++)
		toRender.push_back(App::ct->objCont->getObject(transparentObjects[i]));

	//************ Viewbox thing ***************
	if (viewBoxObject!=NULL)
	{ // we set the same position as the camera, but we keep the initial orientation
		// If the camera is in ortho view mode, we additionally shift it along the viewing axis
		// to be sure we don't cover anything visible with the far side of the box (the near side is clipped by model settings)
		C4Vector rel(viewBoxObject->getLocalTransformationPart1().Q);
		C7Vector cam(getCumulativeTransformation());
		if (!_currentPerspective)
		{
			C3Vector minV,maxV;
			bool first=true;
			viewBoxObject->getGlobalMarkingBoundingBox(getCumulativeTransformation().getInverse(),minV,maxV,first,true);
			float shift=ORTHO_CAMERA_FAR_CLIPPING_PLANE-0.505f*(maxV(2)-minV(2)); // just a bit more than half!
			cam.X+=cam.Q.getMatrix().axis[2]*shift;
		}
		C7Vector newLocal(viewBoxObject->getParentCumulativeTransformation().getInverse()*cam);
		newLocal.Q=rel;
		viewBoxObject->setLocalTransformation(newLocal);
		viewBoxObject->bufferMainDisplayStateVariables();
		viewBoxObject->bufferedMainDisplayStateVariablesToDisplay();
	}
	//***************************************

	// For those special drawing routines that require the window size and other info:
	float verticalViewSizeOrAngle;
	float ratio=(float)(currentWinSize[0]/(float)currentWinSize[1]);
	if (_currentPerspective)
	{
		if (ratio>1.0f)
			verticalViewSizeOrAngle=2.0f*(float)atan(tan(_viewAngle/2.0f)/ratio);
		else
			verticalViewSizeOrAngle=_viewAngle;
	}
	else
	{
		if (ratio>1.0f)
			verticalViewSizeOrAngle=_orthoViewSize/ratio;
		else
			verticalViewSizeOrAngle=_orthoViewSize;
	}

	App::ct->objCont->prepareAuxClippingPlanes();
	App::ct->objCont->enableAuxClippingPlanes(-1);

	int auxVals[4];
	int retVals[4];
	auxVals[0]=0;
	auxVals[1]=displayAttrib;
	auxVals[2]=_objectID;
	auxVals[3]=viewIndex;

	if (!shapeEditModeAndPicking)
	{
		CPluginContainer::sendSpecialEventCallbackMessageToSomePlugins(sim_message_eventcallback_opengl,auxVals,NULL,retVals);

		App::ct->renderYourGeneralObject3DStuff_beforeRegularObjects(this,displayAttrib,currentWinSize,verticalViewSizeOrAngle,_currentPerspective); // those objects are not supposed to be translucid!

		auxVals[0]=1;
		auxVals[1]=displayAttrib;
		auxVals[2]=_objectID;
		auxVals[3]=viewIndex;
		CPluginContainer::sendSpecialEventCallbackMessageToSomePlugins(sim_message_eventcallback_opengl,auxVals,NULL,retVals);
	}

	App::ct->objCont->disableAuxClippingPlanes();

	if (!shapeEditModeAndPicking)
	{
		for (int rp=0;rp<int(toRender.size());rp++)
		{
			C3DObject* it=toRender[rp];
			int atr=displayAttrib;
			if (((it->getObjectProperty()&sim_objectproperty_depthinvisible)==0)||(pass!=DEPTHPASS))
			{
				if (selMap!=NULL)
				{
					if (selMap->at(it->getID())!=0)
						atr|=sim_displayattribute_selected;
					if (it->getID()==lastSel)
						atr|=sim_displayattribute_mainselection;
				}
				else
				{
					for (int i=0;i<App::ct->objCont->getSelSize();i++)
					{
						if (it->getID()==App::ct->objCont->getSelID(i))
						{
							atr|=sim_displayattribute_selected;
							if (it->getID()==lastSel)
								atr|=sim_displayattribute_mainselection;
							break;
						}
					}
				}
				// Draw everything except for the camera you look through (unless we look through the mirror) and the object which is being edited!
				if  ( (it->getID()!=getID())||mirrored )
				{
					if (App::ct->collections->isObjectInMarkedGroup(it->getID()))
						atr|=sim_displayattribute_groupselection;
					if ((App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
						it->display(this,atr);
					else if (it->getID()!=App::ct->objCont->getEditModeObjectID())
						it->display(this,atr);
				}
			}
		}
	}

	if (selMap!=NULL)
	{
		delete selMap;
		selMap=NULL;
	}

	App::ct->objCont->enableAuxClippingPlanes(-1);

	if (!shapeEditModeAndPicking)
	{
		auxVals[0]=2;
		auxVals[1]=displayAttrib;
		auxVals[2]=_objectID;
		auxVals[3]=viewIndex;
		CPluginContainer::sendSpecialEventCallbackMessageToSomePlugins(sim_message_eventcallback_opengl,auxVals,NULL,retVals);

		App::ct->renderYourGeneralObject3DStuff_afterRegularObjects(this,displayAttrib,currentWinSize,verticalViewSizeOrAngle,_currentPerspective); // those objects can be translucid

		auxVals[0]=3;
		auxVals[1]=displayAttrib;
		auxVals[2]=_objectID;
		auxVals[3]=viewIndex;
		CPluginContainer::sendSpecialEventCallbackMessageToSomePlugins(sim_message_eventcallback_opengl,auxVals,NULL,retVals);

		App::ct->renderYourGeneralObject3DStuff_onTopOfRegularObjects(this,displayAttrib,currentWinSize,verticalViewSizeOrAngle,_currentPerspective); // those objects are overlay

		auxVals[0]=4;
		auxVals[1]=displayAttrib;
		auxVals[2]=_objectID;
		auxVals[3]=viewIndex;
		CPluginContainer::sendSpecialEventCallbackMessageToSomePlugins(sim_message_eventcallback_opengl,auxVals,NULL,retVals);

		//Now we display all graphs' 3D curves that should appear on top of everything:
		for (int i=0;i<int(App::ct->objCont->graphList.size());i++)
		{
			CGraph* it=App::ct->objCont->getGraph(App::ct->objCont->graphList[i]);
			if (it!=NULL)
			{
				it->setJustDrawCurves(true);
				it->display(this,displayAttrib);
				it->setJustDrawCurves(false);
			}
		}
	}

	App::ct->objCont->disableAuxClippingPlanes();

	// We render the object being edited last (the vertices appear above everything)
	if (App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)
	{
		C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->getEditModeObjectID());
		if (it!=NULL)
			it->display(this,displayAttrib);
	}
	glLoadName(-1);

	if (pass==RENDERPASS)
	{
		// Wireless communication activities:
		if ((displayAttrib&sim_displayattribute_dynamiccontentonly)==0)
			App::ct->luaScriptContainer->broadcastDataContainer.visualizeCommunications(VDateTime::getTimeInMs());
	}

	if (!shapeEditModeAndPicking)
	{
		auxVals[0]=5;
		auxVals[1]=displayAttrib;
		auxVals[2]=_objectID;
		auxVals[3]=viewIndex;
		CPluginContainer::sendSpecialEventCallbackMessageToSomePlugins(sim_message_eventcallback_opengl,auxVals,NULL,retVals);
	}
}

void CCamera::performDepthPerception(CSView* subView,bool isPerspective)
{
	FUNCTION_DEBUG;
	if (subView==NULL)
		return;
	int mouseMode=App::ct->getMouseMode();
	int windowSize[2];
	subView->getViewSize(windowSize);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	GLfloat pixel[1];
	float farDivFarMinusNear=_farClippingPlane/(_farClippingPlane-_nearClippingPlane);
	if (!isPerspective)
		farDivFarMinusNear=ORTHO_CAMERA_FAR_CLIPPING_PLANE/(ORTHO_CAMERA_FAR_CLIPPING_PLANE-ORTHO_CAMERA_NEAR_CLIPPING_PLANE);
	float m[4][4];
	getCumulativeTransformationMatrixPart1(m);

	int mouseRelativePosition[2];
	subView->getMouseRelativePosition(mouseRelativePosition);

	glReadPixels(mouseRelativePosition[0]+viewport[0],mouseRelativePosition[1]+viewport[1],1,1,GL_DEPTH_COMPONENT,GL_FLOAT,pixel);
	if (isPerspective)
		subView->setMousePositionDepth(_nearClippingPlane*_farClippingPlane/((_farClippingPlane-_nearClippingPlane)*(farDivFarMinusNear-pixel[0])));
	else 
		subView->setMousePositionDepth(ORTHO_CAMERA_NEAR_CLIPPING_PLANE+pixel[0]*(ORTHO_CAMERA_FAR_CLIPPING_PLANE-ORTHO_CAMERA_NEAR_CLIPPING_PLANE));

	float clippFar=_farClippingPlane;
	float clippNear=_nearClippingPlane;
	if (!isPerspective)
	{
		clippFar=ORTHO_CAMERA_FAR_CLIPPING_PLANE;
		clippNear=ORTHO_CAMERA_NEAR_CLIPPING_PLANE;
	}

	if ( (subView->getMousePositionDepth()>clippFar*0.9999f)||
		((mouseMode&0x00ff)==sim_navigation_cameratilt) )
	{ // The cursor hit the far clipping plane or we are in a tilting mode:
		subView->setMousePositionDepth(clippNear);
		float p[3];
		p[0]=m[0][3]+m[0][2]*clippNear;
		p[1]=m[1][3]+m[1][2]*clippNear;
		p[2]=m[2][3]+m[2][2]*clippNear;
		subView->setCenterPosition(p);
	}
	else
	{
		float yShift;
		float xShift;
		float ratio=(float)(windowSize[0]/(float)windowSize[1]);
		int mouseDownRelPos[2];
		subView->getMouseDownRelativePosition(mouseDownRelPos);
		if (isPerspective)
		{
			if (ratio>1.0f)
			{
				float tmp=(windowSize[0]/2)/tan(_viewAngle*0.5f);
				xShift=(-mouseDownRelPos[0]+(windowSize[0]/2))*subView->getMousePositionDepth()/tmp;
				float angle2=2.0f*(float)atan(tan(_viewAngle/2.0f)/ratio)*radToDeg;
				tmp=(windowSize[1]/2)/tan(angle2*degToRad*0.5f);
				yShift=(mouseDownRelPos[1]-(windowSize[1]/2))*subView->getMousePositionDepth()/tmp;
			}
			else
			{
				float tmp=(windowSize[1]/2)/tan(_viewAngle*0.5f);
				yShift=(mouseDownRelPos[1]-(windowSize[1]/2))*subView->getMousePositionDepth()/tmp;
				float angle2=2.0f*(float)atan(tan(_viewAngle/2.0f)*ratio)*radToDeg;
				tmp=(windowSize[0]/2)/tan(angle2*degToRad*0.5f);
				xShift=(-mouseDownRelPos[0]+(windowSize[0]/2))*subView->getMousePositionDepth()/tmp;
			}
		}
		else
		{
			if (ratio>1.0f)
			{
				float tmp=windowSize[0]/_orthoViewSize;
				xShift=(-mouseDownRelPos[0]+(windowSize[0]/2))/tmp;
				tmp=windowSize[1]/(_orthoViewSize/ratio);
				yShift=(+mouseDownRelPos[1]-(windowSize[1]/2))/tmp;
			}
			else
			{
				float tmp=windowSize[1]/_orthoViewSize;
				yShift=(mouseDownRelPos[1]-(windowSize[1]/2))/tmp;
				tmp=windowSize[0]/(_orthoViewSize*ratio);
				xShift=(-mouseDownRelPos[0]+(windowSize[0]/2))/tmp;
			}
		}
		float p[3];
		p[0]=m[0][3]+m[0][2]*subView->getMousePositionDepth()+m[0][1]*yShift+m[0][0]*xShift;
		p[1]=m[1][3]+m[1][2]*subView->getMousePositionDepth()+m[1][1]*yShift+m[1][0]*xShift;
		p[2]=m[2][3]+m[2][2]*subView->getMousePositionDepth()+m[2][1]*yShift+m[2][0]*xShift;
		subView->setCenterPosition(p);
	}
}

void CCamera::drawOverlay(bool passiveView,bool drawText,bool displ_ref,int windowSize[2],CSView* subView)
{
	FUNCTION_DEBUG;
	int navigationMode=sim_navigation_passive;
	int selectionMode=NOSELECTION;
	if (subView!=NULL)
	{
		navigationMode=App::ct->getMouseMode()&0x00ff;
		selectionMode=subView->getSelectionStatus();
	}
	ogl::allLightsOffAmbientFull();
	ogl::setColorsAllBlack();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,windowSize[0],0,windowSize[1],-100,100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();

	glDisable(GL_DEPTH_TEST);

	// Draw the selection square
	if ( (selectionMode==SHIFTSELECTION)&&(subView!=NULL)&&subView->isMouseDown() )
	{
		int mouseDownRelPos[2];
		int mouseRelPos[2];
		subView->getMouseDownRelativePosition(mouseDownRelPos);
		subView->getMouseRelativePosition(mouseRelPos);
		int x1=mouseDownRelPos[0];
		int y1=mouseDownRelPos[1];
		int x2=mouseRelPos[0];
		int y2=mouseRelPos[1];
		tt::limitValue(0,windowSize[0],x1);
		tt::limitValue(0,windowSize[1],y1);
		tt::limitValue(0,windowSize[0],x2);
		tt::limitValue(0,windowSize[1],y2);

		float black[3]={0.0f,0.0f,0.0f};
		float yellow[3]={1.0f,1.0f,0.0f};
		ogl::setColor(yellow,black,black,black,12,0.2f);
		ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_QUADS);
			glVertex3i(x1,y1,0); 
			glVertex3i(x1,y2,0);
			glVertex3i(x2,y2,0);
			glVertex3i(x2,y1,0);
			glVertex3i(x1,y1,0);
		glEnd();
		ogl::setBlending(false);		
		ogl::setColor(1.0f,1.0f,1.0f,AMBIENT_MODE);
		glBegin(GL_LINE_STRIP);
			glVertex3i(x1,y1,0); 
			glVertex3i(x1,y2,0);
			glVertex3i(x2,y2,0);
			glVertex3i(x2,y1,0);
			glVertex3i(x1,y1,0);
		glEnd();
		glDisable(GL_LINE_STIPPLE);
	}

	if (App::userSettings->displayWorldReference&&displ_ref)
	{
		glTranslatef(float(windowSize[0]-60),40.0f,0.0f);
		C7Vector tr2(getCumulativeTransformation());
		tr2.inverse();
		C4X4Matrix m1;
		m1.buildYRotation(piValue);
		C7Vector tr0(m1.getTransformation()*tr2);
		glDisable(GL_DEPTH_TEST);
		float refSize=30;

		C3Vector euler(tr0.Q.getEulerAngles());
		glPushMatrix();
		glRotatef(euler(0)*radToDeg,1.0,0.0,0.0);	
		glRotatef(euler(1)*radToDeg,0.0,1.0,0.0);
		glRotatef(euler(2)*radToDeg,0.0,0.0,1.0);

		ogl::drawReference(refSize,1.0f,true);
		glPopMatrix();
		ogl::setColor(1.0f,1.0f,1.0f,AMBIENT_MODE);
		glTranslatef(0.2f*refSize,0.2f*refSize,0.0f);
		C3X3Matrix m0(tr0.Q.getMatrix());
		ogl::drawBitmapTextIntoScene(m0(0,0)*refSize,m0(1,0)*refSize,0.0f,IDSOGL_X);
		ogl::drawBitmapTextIntoScene(m0(0,1)*refSize,m0(1,1)*refSize,0.0f,IDSOGL_Y);
		ogl::drawBitmapTextIntoScene(m0(0,2)*refSize,m0(1,2)*refSize,0.0f,IDSOGL_Z);
		glEnable(GL_DEPTH_TEST);
	}
}

int CCamera::getSingleHit(int hits,unsigned int selectBuff[],bool ignoreDepthBuffer,int& hitThatIgnoresTheSelectableFlag)
{ // this routine is called in non-edit mode, but also while in an edit mode!!
	FUNCTION_DEBUG;
	hitThatIgnoresTheSelectableFlag=-1;
	if (hits!=0)
	{
		unsigned int nearestObj=0;
		unsigned int distance=selectBuff[1];
		for (int i=0;i<hits;i++)
		{
			if (i==0)
			{ // the first hit. We always take this one into account
				distance=selectBuff[4*i+1];
				nearestObj=selectBuff[4*i+3];
				hitThatIgnoresTheSelectableFlag=nearestObj;
				if ((nearestObj<=SIM_IDEND_3DOBJECT)&&(App::ct->objCont->getEditModeType()==NO_EDIT_MODE))
				{ // since 3/6/2013 we check for the selectable flag here instead of in the object display routine
					C3DObject* theObj=App::ct->objCont->getObject(nearestObj);
					if ((theObj!=NULL)&&((theObj->getObjectProperty()&sim_objectproperty_selectable)==0))
						nearestObj=(unsigned int)-1;
				}
			}
			else
			{
				if (!ignoreDepthBuffer)
				{
					bool shapeEditAndNoHitYet=((App::ct->objCont->getEditModeType()&SHAPE_EDIT_MODE)&&(nearestObj==-1));
					if ((selectBuff[4*i+1]<=distance)||shapeEditAndNoHitYet)
					{ // this hit is closer
						distance=selectBuff[4*i+1];
						nearestObj=selectBuff[4*i+3];
						hitThatIgnoresTheSelectableFlag=nearestObj;
						if ((nearestObj<=SIM_IDEND_3DOBJECT)&&(App::ct->objCont->getEditModeType()==NO_EDIT_MODE))
						{ // since 3/6/2013 we check for the selectable flag here instead of in the object display routine
							C3DObject* theObj=App::ct->objCont->getObject(nearestObj);
							if ((theObj!=NULL)&&((theObj->getObjectProperty()&sim_objectproperty_selectable)==0))
								nearestObj=(unsigned int)-1;
						}
					}
				}
				else
				{
					if (nearestObj==(unsigned int)-1)
					{ // we take this hit since we don't yet have a hit
						distance=selectBuff[4*i+1];
						nearestObj=selectBuff[4*i+3];
						hitThatIgnoresTheSelectableFlag=nearestObj;
						if ((nearestObj<=SIM_IDEND_3DOBJECT)&&(App::ct->objCont->getEditModeType()==NO_EDIT_MODE))
						{ // since 3/6/2013 we check for the selectable flag here instead of in the object display routine
							C3DObject* theObj=App::ct->objCont->getObject(nearestObj);
							if ((theObj!=NULL)&&((theObj->getObjectProperty()&sim_objectproperty_selectable)==0))
								nearestObj=(unsigned int)-1;
						}
					}
				}
			}
		}

		if (nearestObj==(unsigned int)-1)
			return(-1); // we didn't hit anything
		else
			return((int)nearestObj); // we hit this ID
	}
	else
		return(-1); // we didn't hit anything
}

int CCamera::getSeveralHits(int hits,unsigned int selectBuff[],std::vector<int>& hitList)
{ // this routine is called in non-edit mode, but also while in an edit mode!!
	FUNCTION_DEBUG;
	hitList.clear();
	if (hits!=0)
	{
		int cnt=0;
		for (int i=0;i<hits;i++)
		{
			unsigned int theHit=selectBuff[4*i+3];
			if (theHit!=((unsigned int)-1))
			{
				if ((theHit<=SIM_IDEND_3DOBJECT)&&(App::ct->objCont->getEditModeType()==NO_EDIT_MODE))
				{ // since 3/6/2013 we check for the selectable flag here instead of in the object display routine
					C3DObject* theObj=App::ct->objCont->getObject(theHit);
					if ((theObj!=NULL)&&((theObj->getObjectProperty()&sim_objectproperty_selectable)==0))
						theHit=(unsigned int)-1;
				}
				if (int(theHit)!=-1)
				{
					cnt++;
					hitList.push_back((int)theHit);
				}
			}
		}
		return(cnt);
	}
	else
		return(0); // we didn't hit anything
}

void CCamera::handleMouseUpHit(int hitId)
{
	FUNCTION_DEBUG;
	if (hitId==-1)
	{
		if ((App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
			App::ct->objCont->deselectObjects();
		else
			App::ct->objCont->deselectEditModeBuffer();
	}
	else
	{
		if ( (hitId<SIM_IDEND_3DOBJECT)||(hitId>=NON_OBJECT_PICKING_ID_PATH_PTS_START) ) // We need the NON_OBJECT_PICKING_ID_PATH_PTS_START start here to select individual path points when not in path edit mode!!!!!!!
		{
			if ((App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
			{
				App::ct->objCont->deselectObjects();
				App::ct->objCont->addObjectToSelection(hitId);
			}
			else
			{
				App::ct->objCont->deselectEditModeBuffer();
				App::ct->objCont->addItemToEditModeBuffer(hitId,false);
			}
		}
		if ( (hitId>=NON_OBJECT_PICKING_ID_BANNER_START)&&(hitId<NON_OBJECT_PICKING_ID_BANNER_END) )
			_handleBannerClick(hitId-NON_OBJECT_PICKING_ID_BANNER_START);
	}
}

int CCamera::handleHits(int hits,unsigned int selectBuff[])
{ // -2 means: handle no mouse up hits. Otherwise, handle mouse up hits!
	FUNCTION_DEBUG;
	if (App::mainWindow==NULL)
		return(-2);
	if (App::mainWindow->getMouseButtonState()&4) // added on 2011/01/12 because this routine is now also called when not in click-select mode, etc. We need to make sure we don't have a "virtual" left mouse button clicked triggered by the right mouse button
		return(-2);
	if (App::mainWindow->shiftKeyDown)
	{
		std::vector<int> hitList;
		if (getSeveralHits(hits,selectBuff,hitList)>0)
		{
			for (int i=0;i<int(hitList.size());i++)
			{
				if ( (hitList[i]<SIM_IDEND_3DOBJECT)||(hitList[i]>=NON_OBJECT_PICKING_ID_PATH_PTS_START) ) // We need the NON_OBJECT_PICKING_ID_PATH_PTS_START start here to select individual path points when not in path edit mode!!!!!!!
				{
					if ((App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
						App::ct->objCont->addObjectToSelection(hitList[i]);
					else
						App::ct->objCont->addItemToEditModeBuffer(hitList[i]);
				}
				if ( (hitList[i]>=NON_OBJECT_PICKING_ID_BANNER_START)&&(hitList[i]<NON_OBJECT_PICKING_ID_BANNER_END) )
					_handleBannerClick(hitList[i]-NON_OBJECT_PICKING_ID_BANNER_START);
			}
		}
		return(-2);
	}
	else
	{
		if (App::mainWindow->ctrlKeyDown)
		{
			int dummy;
			bool ignoreDepth=App::ct->objCont->showHiddenVerticeAndEdges&&((App::ct->objCont->getEditModeType()&VERTEX_EDIT_MODE)||(App::ct->objCont->getEditModeType()&EDGE_EDIT_MODE));
			int hitId=getSingleHit(hits,selectBuff,ignoreDepth,dummy);
			if (hitId==-1)
			{
				if ((App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
					App::ct->objCont->deselectObjects();
				else
					App::ct->objCont->deselectEditModeBuffer();
			}
			else
			{
				if ( (hitId<SIM_IDEND_3DOBJECT)||(hitId>=NON_OBJECT_PICKING_ID_PATH_PTS_START) ) // We need the NON_OBJECT_PICKING_ID_PATH_PTS_START start here to select individual path points when not in path edit mode!!!!!!!
				{
					if ((App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
						App::ct->objCont->xorAddObjectToSelection(hitId);
					else
						App::ct->objCont->xorAddItemToEditModeBuffer(hitId,false);
				}
				if ( (hitId>=NON_OBJECT_PICKING_ID_BANNER_START)&&(hitId<NON_OBJECT_PICKING_ID_BANNER_END) )
					_handleBannerClick(hitId-NON_OBJECT_PICKING_ID_BANNER_START);
			}
			return(-2);
		}
		else
		{ // no ctrl or shift key down here. We simply return the hit for processing at a later stage
			int hitThatIgnoresTheSelectableFlag;
			bool ignoreDepth=App::ct->objCont->showHiddenVerticeAndEdges&&((App::ct->objCont->getEditModeType()&VERTEX_EDIT_MODE)||(App::ct->objCont->getEditModeType()&EDGE_EDIT_MODE));
			int hitId=getSingleHit(hits,selectBuff,ignoreDepth,hitThatIgnoresTheSelectableFlag);

			// Now generate a plugin callback:
			int data[4]={hitThatIgnoresTheSelectableFlag,0,0,0};
			void* retVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_pickselectdown,data,NULL,NULL);
			delete[] (char*)retVal;
			// Now generate a script message:
			App::ct->outsideCommandQueue->addCommand(sim_message_pick_select_down,hitThatIgnoresTheSelectableFlag,0,0,0,NULL,0);

			return(hitId);
		}
	}
	return(-2);
}



void CCamera::_handleBannerClick(int bannerID)
{
	FUNCTION_DEBUG;
	if (App::ct->objCont->getEditModeType()!=NO_EDIT_MODE)
		return;
	CBannerObject* it=App::ct->bannerCont->getObject(bannerID);
	if (it!=NULL)
	{
		if (it->getOptions()&sim_banner_clickselectsparent)
		{
			if (it->getParentObjectHandle()>=0)
				App::ct->objCont->addObjectToSelection(it->getParentObjectHandle());
		}
		if (it->getOptions()&sim_banner_clicktriggersevent)
		{
			int auxVals[4]={bannerID,0,0,0};
			void* returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_eventcallback_bannerclicked,auxVals,NULL,NULL);
			delete[] (char*)returnVal;
			auxVals[0]=bannerID;
			returnVal=CPluginContainer::sendEventCallbackMessageToAllPlugins(sim_message_bannerclicked,auxVals,NULL,NULL); // for backward compatibility
			delete[] (char*)returnVal;
			App::ct->outsideCommandQueue->addCommand(sim_message_bannerclicked,bannerID,0,0,0,NULL,0);
		}
	}
}

void CCamera::handleTrackingAndHeadAlwaysUp()
{
	FUNCTION_DEBUG;
	// 1. First tracking:
	// Check if the tracked object is not parented with that camera
	// (camera would follow the object which would follow the camera which...)
	C3DObject* tr=App::ct->objCont->getObject(trackedObjectIdentifier_NeverDirectlyTouch);
	if ((tr==NULL)||(tr==this)||tr->isObjectParentedWith(this))
	{
		trackedObjectIdentifier_NeverDirectlyTouch=-1;
		tr=NULL;
	}
	if (tr!=NULL)
	{
		C7Vector tracked(tr->getCumulativeTransformation());
		C7Vector self(getCumulativeTransformation());
		C4Vector rot1(self.Q.getAxis(2),tracked.X-self.X);
		self.Q=rot1*self.Q;
		// We check if the camera looks to +Z or -Z:
		C3Vector zAxis(self.Q.getAxis(2));
		if ( (fabs(zAxis(0))>0.00001f)||(fabs(zAxis(1))>0.00001f) )
		{ // Camera does not look to +Z or -Z:
			C3Vector rotAxis(zAxis^C3Vector(0.0f,0.0f,1.0f));
			C4Vector rot(piValue/2.0f,rotAxis);
			zAxis=rot*zAxis;
			C4Vector rot2(self.Q.getAxis(1),zAxis);
			self.Q=rot2*self.Q;
			C7Vector parentInv(getParentCumulativeTransformation().getInverse());
			setLocalTransformation(parentInv*self);
		}
	}

	// 2. Now permanent head up:
	if (_cameraManipulationModePermissions&0x020)
	{ // We have to keep head up
		C7Vector cameraCTM(getCumulativeTransformationPart1());
		C3X3Matrix trM2(cameraCTM.Q);
		if ( (fabs(trM2.axis[2](0))>0.00001f)||(fabs(trM2.axis[2](1))>0.00001f) )
		{ // We have to do it:
			float val=1.0f;
			if (trM2.axis[1](2)<0.0f)
				val=-1.0f;
			C3Vector rotAx(trM2.axis[2]^C3Vector(0.0f,0.0f,val));
			C3Vector target(C4Vector(piValue/2.0f,rotAx)*trM2.axis[2]);
			C4Vector rot(trM2.axis[1],target);
			cameraCTM.Q=rot*cameraCTM.Q;
			setLocalTransformation(getParentCumulativeTransformation().getInverse()*cameraCTM);
		}
	}
}


void CCamera::scaleObject(float scalingFactor)
{
	cameraSize*=scalingFactor;
	_nearClippingPlane*=scalingFactor;
	_farClippingPlane*=scalingFactor;
	_orthoViewSize*=scalingFactor;

	scaleObjectMain(scalingFactor);
}

void CCamera::setCameraSize(float size)
{
	tt::limitValue(0.001f,100.0f,size);
	cameraSize=size;
}

float CCamera::getCameraSize()
{
	return(cameraSize);
}

int CCamera::getTrackedObjectID()
{
	return(trackedObjectIdentifier_NeverDirectlyTouch);
}

void CCamera::setUseParentObjectAsManipulationProxy(bool useParent)
{
	_useParentObjectAsManipulationProxy=useParent;
}

bool CCamera::getUseParentObjectAsManipulationProxy()
{
	return(_useParentObjectAsManipulationProxy);
}

void CCamera::setTrackedObjectID(int trackedObjID)
{
	if (trackedObjID==_objectID)
		return;
	if (trackedObjID==-1)
	{
		trackedObjectIdentifier_NeverDirectlyTouch=-1;
		return;
	}
	if (App::ct->objCont->getObject(trackedObjID)==NULL)
		return;
	trackedObjectIdentifier_NeverDirectlyTouch=trackedObjID;
	App::ct->objCont->setLightDialogRefreshFlag();
}

C3DObject* CCamera::copyYourself()
{	
	CCamera* newCamera=(CCamera*)copyYourselfMain();

	// Various
	newCamera->cameraSize=cameraSize;
	newCamera->_viewAngle=_viewAngle;
	newCamera->_orthoViewSize=_orthoViewSize;
	newCamera->_nearClippingPlane=_nearClippingPlane;
	newCamera->_farClippingPlane=_farClippingPlane;
	newCamera->_showFogIfAvailable=_showFogIfAvailable;
	newCamera->trackedObjectIdentifier_NeverDirectlyTouch=trackedObjectIdentifier_NeverDirectlyTouch;
	newCamera->_useParentObjectAsManipulationProxy=_useParentObjectAsManipulationProxy;
	newCamera->_cameraManipulationModePermissions=_cameraManipulationModePermissions;
	newCamera->_useLocalLights=_useLocalLights;
	newCamera->_allowPicking=_allowPicking;

	// Colors:
	colorPart1.copyYourselfInto(&newCamera->colorPart1);
	colorPart2.copyYourselfInto(&newCamera->colorPart2);

	return(newCamera);
}

bool CCamera::announceObjectWillBeErased(int objID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	// This routine can be called for objCont-objects, but also for objects
	// in the copy-buffer!! So never make use of any 
	// 'ct::objCont->getObject(id)'-call or similar
	// Return value true means 'this' has to be erased too!
	announceObjectWillBeErasedMain(objID,copyBuffer);
	if (trackedObjectIdentifier_NeverDirectlyTouch==objID)
		trackedObjectIdentifier_NeverDirectlyTouch=-1;
	return(false);
}
void CCamera::announceGroupWillBeErased(int groupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGroupWillBeErasedMain(groupID,copyBuffer);
}
void CCamera::announceCollisionWillBeErased(int collisionID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceCollisionWillBeErasedMain(collisionID,copyBuffer);
}
void CCamera::announceDistanceWillBeErased(int distanceID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceDistanceWillBeErasedMain(distanceID,copyBuffer);
}
void CCamera::announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceIkObjectWillBeErasedMain(ikGroupID,copyBuffer);
}
void CCamera::announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGcsObjectWillBeErasedMain(gcsObjectID,copyBuffer);
}
void CCamera::performObjectLoadingMapping(std::vector<int>* map)
{ // New_Object_ID=map[Old_Object_ID]
	performObjectLoadingMappingMain(map);
	trackedObjectIdentifier_NeverDirectlyTouch=App::ct->objCont->getLoadingMapping(map,trackedObjectIdentifier_NeverDirectlyTouch);
}
void CCamera::performGroupLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performGroupLoadingMappingMain(map);
}
void CCamera::performCollisionLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performCollisionLoadingMappingMain(map);
}
void CCamera::performDistanceLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performDistanceLoadingMappingMain(map);
}
void CCamera::performIkLoadingMapping(std::vector<int>* map)
{
	performIkLoadingMappingMain(map);
}
void CCamera::performGcsLoadingMapping(std::vector<int>* map)
{
	performGcsLoadingMappingMain(map);
}

void CCamera::performTextureObjectLoadingMapping(std::vector<int>* map)
{
	performTextureObjectLoadingMappingMain(map);
}

void CCamera::performDynMaterialObjectLoadingMapping(std::vector<int>* map)
{
	performDynMaterialObjectLoadingMappingMain(map);
}

void CCamera::bufferMainDisplayStateVariables()
{
	bufferMainDisplayStateVariablesMain();
}

void CCamera::bufferedMainDisplayStateVariablesToDisplay()
{
	bufferedMainDisplayStateVariablesToDisplayMain();
}

int CCamera::getViewOrientation()
{
	C3X3Matrix m(getCumulativeTransformation().Q);
	if (fabs(m.axis[2].getAngle(C3Vector(-1.0f,0.0f,0.0f))*radToDeg)<0.1f)
		return(POSITIVE_X_VIEW);
	if (fabs(m.axis[2].getAngle(C3Vector(+1.0f,0.0f,0.0f))*radToDeg)<0.1f)
		return(NEGATIVE_X_VIEW);
	if (fabs(m.axis[2].getAngle(C3Vector(0.0f,-1.0f,0.0f))*radToDeg)<0.1f)
		return(POSITIVE_Y_VIEW);
	if (fabs(m.axis[2].getAngle(C3Vector(0.0f,+1.0f,0.0f))*radToDeg)<0.1f)
		return(NEGATIVE_Y_VIEW);
	if (fabs(m.axis[2].getAngle(C3Vector(0.0f,0.0f,-1.0f))*radToDeg)<0.1f)
		return(POSITIVE_Z_VIEW);
	if (fabs(m.axis[2].getAngle(C3Vector(0.0f,0.0f,+1.0f))*radToDeg)<0.1f)
		return(NEGATIVE_Z_VIEW);
	return(NO_SPECIFIC_VIEW);
}
void CCamera::setViewOrientation(int ori,bool setPositionAlso)
{ // setPositionAlso is false by default
	float alpha,beta,gamma,x,y,z;
	bool done=false;
	if (ori==POSITIVE_X_VIEW)
	{
		alpha=0.0f;
		beta=-90.0f;
		gamma=-90.0f;
		x=+3.0f;
		y=0.0f;
		z=0.0f;
		done=true;
	}
	if (ori==NEGATIVE_X_VIEW)
	{
		alpha=0.0f;
		beta=+90.0f;
		gamma=+90.0f;
		x=-3.0f;
		y=0.0f;
		z=0.0f;
		done=true;
	}
	if (ori==POSITIVE_Y_VIEW)
	{
		alpha=+90.0f;
		beta=0.0f;
		gamma=0.0f;
		x=0.0f;
		y=+3.0f;
		z=0.0f;
		done=true;
	}
	if (ori==NEGATIVE_Y_VIEW)
	{
		alpha=-90.0f;
		beta=0.0f;
		gamma=+180.0f;
		x=0.0f;
		y=-3.0f;
		z=0.0f;
		done=true;
	}
	if (ori==POSITIVE_Z_VIEW)
	{
		alpha=+180.0f;
		beta=0.0f;
		gamma=0.0f;
		x=0.0f;
		y=0.0f;
		z=+3.0f;
		done=true;
	}
	if (ori==NEGATIVE_Z_VIEW)
	{
		alpha=0.0f;
		beta=0.0f;
		gamma=+180.0f;
		x=0.0f;
		y=0.0f;
		z=-3.0f;
		done=true;
	}
	if (done)
	{
		C7Vector tot(getCumulativeTransformation());
		tot.Q.setEulerAngles(C3Vector(alpha*degToRad,beta*degToRad,gamma*degToRad));
		if (setPositionAlso)
			tot.X.set(x,y,z);
		C7Vector parentInv(getParentCumulativeTransformation().getInverse());
		setLocalTransformation(parentInv*tot);
	}
}

void CCamera::setAllowPicking(bool a)
{
	_allowPicking=a;
}

bool CCamera::getAllowPicking()
{
	return(_allowPicking);
}

void CCamera::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	initializeInitialValuesMain(simulationIsRunning);
	_initialValuesInitialized=simulationIsRunning;
	if (simulationIsRunning)
	{
		_initialViewAngle=_viewAngle;
		_initialOrthoViewSize=_orthoViewSize;
	}
}

void CCamera::simulationAboutToStart()
{
	initializeInitialValues(true);
}

void CCamera::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd()&&((getCumulativeMainPropertyOverride()&sim_modelproperty_not_reset)==0))
	{
		_viewAngle=_initialViewAngle;
		_orthoViewSize=_initialOrthoViewSize;
	}
	_initialValuesInitialized=false;
	simulationEndedMain();
}

void CCamera::serialize(CSer& ar)
{
	serializeMain(ar);
	if (ar.isStoring())
	{ // Storing
		int trck=-1;
		if (trackedObjectIdentifier_NeverDirectlyTouch!=-1)
			trck=trackedObjectIdentifier_NeverDirectlyTouch;
		ar.storeDataName("Cp4");
		ar << trck << cameraSize;
		ar.flush();

		ar.storeDataName("Cp3");
		ar << _orthoViewSize << _viewAngle;
		ar.flush();

		ar.storeDataName("Ccp"); 
		ar << _nearClippingPlane << _farClippingPlane;
		ar.flush();

		ar.storeDataName("Cmp"); 
		ar << _cameraManipulationModePermissions;
		ar.flush();

		ar.storeDataName("Ca2");
		BYTE nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,_useParentObjectAsManipulationProxy);
		SIM_SET_CLEAR_BIT(nothing,1,!_showFogIfAvailable);
		SIM_SET_CLEAR_BIT(nothing,2,_useLocalLights);
		SIM_SET_CLEAR_BIT(nothing,3,!_allowPicking);	
		ar << nothing;
		ar.flush();

		ar.storeDataName("Cl1");
		ar.setCountingMode();
		colorPart1.serialize(ar);
		if (ar.setWritingMode())
			colorPart1.serialize(ar);

		ar.storeDataName("Cl2");
		ar.setCountingMode();
		colorPart2.serialize(ar);
		if (ar.setWritingMode())
			colorPart2.serialize(ar);

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
				if (theName.compare("Cp4")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> trackedObjectIdentifier_NeverDirectlyTouch >> cameraSize;
				}
				if (theName.compare("Cp3")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _orthoViewSize >> _viewAngle;
				}
				if (theName.compare("Ccp")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _nearClippingPlane >> _farClippingPlane;
				}
				if (theName=="Caz")
				{ // keep for backward compatibility (2010/07/13)
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_useParentObjectAsManipulationProxy=SIM_IS_BIT_SET(nothing,0);
					if (!_backwardCompatibilityFlag20100713)
					{
						bool headUp=SIM_IS_BIT_SET(nothing,1);
						bool keepHeadAlwaysUp=SIM_IS_BIT_SET(nothing,2);
						if (headUp)
							_cameraManipulationModePermissions&=0x02f;
						else
							_cameraManipulationModePermissions|=0x010;
						if (keepHeadAlwaysUp)
							_cameraManipulationModePermissions|=0x020;
						else
							_cameraManipulationModePermissions&=0x01f;
					}
				}
				if (theName=="Ca2")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_useParentObjectAsManipulationProxy=SIM_IS_BIT_SET(nothing,0);
					_showFogIfAvailable=!SIM_IS_BIT_SET(nothing,1);
					_useLocalLights=SIM_IS_BIT_SET(nothing,2);
					_allowPicking=!SIM_IS_BIT_SET(nothing,3);
				}
				if (theName.compare("Cl1")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					colorPart1.serialize(ar);
				}
				if (theName.compare("Cl2")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					colorPart2.serialize(ar);
				}
				if (theName.compare("Cmp")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _cameraManipulationModePermissions;
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
		// For backward compatibility (27/7/2011):
		// ****************************************
		if (ar.getVrepVersionThatWroteThisFile()<20509)
		{
			if ( (_objectName.compare("DefaultXViewCamera")==0)||(_objectName.compare("DefaultYViewCamera")==0)||(_objectName.compare("DefaultZViewCamera")==0) )
				_showFogIfAvailable=false;
		}
		// ****************************************

		if ((ar.getSerializationVersionThatWroteThisFile()<17)&&App::userSettings->correctColorsAndLights)
		{ // on 29/08/2013 we corrected all default lights. So we need to correct for that change:
			CTTUtil::scaleColorUp_(colorPart1.colors);
			CTTUtil::scaleColorUp_(colorPart2.colors);
		}
	}
}

void CCamera::serializeWExtIk(CExtIkSer& ar)
{
	serializeWExtIkMain(ar);
	CDummy::serializeWExtIkStatic(ar);
}


void CCamera::backwardCompatibility20100713()
{
	_backwardCompatibilityFlag20100713=true;
}
