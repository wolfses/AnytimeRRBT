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
#include "visionSensor.h"
#include "v_rep_internal.h"
#include "Global.h"
#include "Tt.h"
#include "MeshManip.h"
#include "OGL.h"
#include "IloIlo.h"
#include "imageProcess.h"
#include "ObjectEdition.h"
#include "v_repStrings.h"
#include <boost/lexical_cast.hpp>
#include "VDateTime.h"
#include "VVarious.h"
#include "ttUtil.h"
#include "ThreadPool.h"
#include <QtOpenGL>
#include "easyLock.h"
#include "App.h"

const int attributesForVisionSensorRendering=sim_displayattribute_renderpass|
												sim_displayattribute_forbidwireframe|
												sim_displayattribute_forbidedges|
												sim_displayattribute_originalcolors|
												sim_displayattribute_ignorelayer|
												sim_displayattribute_forvisionsensor;

CVisionSensor::CVisionSensor()
{
	commonInit();
}

std::string CVisionSensor::getObjectTypeInfo()
{
	return(IDSOGL_VISION_SENSOR);
}
std::string CVisionSensor::getObjectTypeInfoExtended()
{
	std::string retVal;
	retVal=IDSOGL_VISION_SENSOR_SAMPLING_RESOLUTION;
	retVal+=boost::lexical_cast<std::string>(_resolutionX)+"x";
	retVal+=boost::lexical_cast<std::string>(_resolutionY)+")";
	return(retVal);
}

float* CVisionSensor::readPortionOfImage(int posX,int posY,int sizeX,int sizeY,bool rgbOtherwiseDepth)
{
	if ( (posX<0)||(posY<0)||(sizeX<1)||(sizeY<1)||(posX+sizeX>_resolutionX)||(posY+sizeY>_resolutionY) )
		return(NULL);
	float* buff;
	if (rgbOtherwiseDepth)
		buff=new float[sizeX*sizeY*3];
	else
		buff=new float[sizeX*sizeY];
	int p=0;
	for (int j=posY;j<posY+sizeY;j++)
	{
		for (int i=posX;i<posX+sizeX;i++)
		{
			if (rgbOtherwiseDepth)
			{
				buff[3*p+0]=_imageBuffer[3*(j*_resolutionX+i)+0];
				buff[3*p+1]=_imageBuffer[3*(j*_resolutionX+i)+1];
				buff[3*p+2]=_imageBuffer[3*(j*_resolutionX+i)+2];
			}
			else
				buff[p]=_depthBuffer[j*_resolutionX+i];
			p++;
		}
	}
	return(buff);
}

bool CVisionSensor::getFullBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	minV(0)=-0.5f*_size(0);
	maxV(0)=0.5f*_size(0);
	minV(1)=-0.5f*_size(1);
	maxV(1)=0.5f*_size(1);
	minV(2)=-_size(2);
	maxV(2)=0.0f;

	C3Vector c,f,e;
	getSensingVolumeCorners(c,f);
	for (float fi=-1.0f;fi<2.0f;fi+=2.0f)
	{
		for (float fj=-1.0f;fj<2.0f;fj+=2.0f)
		{
			e(0)=fi*c(0);
			e(1)=fj*c(1);
			e(2)=c(2);
			minV.keepMin(e);
			maxV.keepMax(e);
			e(0)=fi*f(0);
			e(1)=fj*f(1);
			e(2)=f(2);
			minV.keepMin(e);
			maxV.keepMax(e);
		}
	}
	return(true);
}

bool CVisionSensor::getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	return(getFullBoundingBox(minV,maxV));
}

void CVisionSensor::commonInit()
{
	setObjectType(sim_object_visionsensor_type);
	_nearClippingPlane=0.01f;
	_farClippingPlane=0.2f;
	_viewAngle=45.0f*degToRad;
	_orthoViewSize=0.01f;
	_showFogIfAvailable=true;
	_useLocalLights=false;

	layer=VISION_SENSOR_LAYER;
	_rgbBuffer=NULL;
	_detectableEntityID=-1;
	_mainPropertyLocal=0;
	_explicitHandling=false;
	_showVolumeWhenNotDetecting=true;
	_showVolumeWhenDetecting=true;

	_ignoreRGBInfo=false;
	_ignoreDepthInfo=false;
	_colorCodeId=false;

	_initialValuesInitialized=false;

	_useExternalImage=false;
	_useSameBackgroundAsEnvironment=true;

	_composedFilter=new CComposedFilter();
	CSimpleFilter* it=new CSimpleFilter();
	it->setFilterType(simple_filter_originalimage);
	_composedFilter->insertSimpleFilter(it);

	it=new CSimpleFilter();
	it->setFilterType(simple_filter_tooutput);
	_composedFilter->insertSimpleFilter(it);

	_contextFboAndTexture=NULL;

	_defaultBufferValues[0]=0.0f;
	_defaultBufferValues[1]=0.0f;
	_defaultBufferValues[2]=0.0f;

	sensorResult.sensorWasTriggered=false;
	sensorResult.sensorResultIsValid=false;
	sensorResult.calcTimeInMs=0;
	sensorAuxiliaryResult.clear();
	bodyColor.setDefaultValues();
	bodyColor.setColor(0.0f,0.25f,1.0f,AMBIENT_MODE);
	bodyActiveColor.setColorsAllBlack();
	bodyActiveColor.setColor(1.0f,0.0f,0.0f,EMISSION_MODE);
	bodyActiveColor.setFlash(true);
	detectionVolumeColor.setColorsAllBlack();
	detectionVolumeColor.setColor(0.0f,0.3f,0.6f,AMBIENT_MODE);
	detectionVolumeActiveColor.setColorsAllBlack();
	detectionVolumeActiveColor.setColor(1.0f,1.0f,0.0f,EMISSION_MODE);
	detectionVolumeActiveColor.setFlash(true);

	_resolutionX=32;
	_resolutionY=32;
	_desiredResolution[0]=32;
	_desiredResolution[1]=32;
	_size(0)=_orthoViewSize;
	_size(1)=_size(0);
	_size(2)=_size(0)*3.0f;
	_perspectiveOperation=false;

	_imageBuffer=NULL;
	_previousImageBuffer=NULL;
	_depthBuffer=NULL;
	_reserveBuffers();

	_objectName=IDSOGL_VISION_U_SENSOR;
}

void CVisionSensor::setUseExternalImage(bool u)
{
	_useExternalImage=u;
}

bool CVisionSensor::getUseExternalImage()
{
	return(_useExternalImage);
}


CComposedFilter* CVisionSensor::getComposedFilter()
{
	return(_composedFilter);
}

void CVisionSensor::setComposedFilter(CComposedFilter* newFilter)
{
	delete _composedFilter;
	_composedFilter=newFilter;
}

CVisionSensor::~CVisionSensor()
{
	_removeGlContextAndFboAndTextureObjectIfNeeded();

	delete[] _imageBuffer;
	delete[] _previousImageBuffer;
	delete[] _depthBuffer;
	delete[] _rgbBuffer;

	delete _composedFilter;
}

float CVisionSensor::getCalculationTime()
{
	return(float(sensorResult.calcTimeInMs)*0.001f);
}

void CVisionSensor::_reserveBuffers()
{
	delete[] _imageBuffer;
	delete[] _previousImageBuffer;
	delete[] _depthBuffer;
	delete[] _rgbBuffer;
	_imageBuffer=new float[3*_resolutionX*_resolutionY];
	_previousImageBuffer=new float[3*_resolutionX*_resolutionY];
	_depthBuffer=new float[_resolutionX*_resolutionY];
	_rgbBuffer=new unsigned char[3*_resolutionX*_resolutionY];
	_clearBuffers();

	_removeGlContextAndFboAndTextureObjectIfNeeded();
}

void CVisionSensor::_clearBuffers()
{
	for (int i=0;i<_resolutionX*_resolutionY;i++)
	{
		if (_useSameBackgroundAsEnvironment)
		{
			_imageBuffer[3*i+0]=App::ct->environment->fogBackgroundColor[0];
			_imageBuffer[3*i+1]=App::ct->environment->fogBackgroundColor[1];
			_imageBuffer[3*i+2]=App::ct->environment->fogBackgroundColor[2];
			_previousImageBuffer[3*i+0]=App::ct->environment->fogBackgroundColor[0];
			_previousImageBuffer[3*i+1]=App::ct->environment->fogBackgroundColor[1];
			_previousImageBuffer[3*i+2]=App::ct->environment->fogBackgroundColor[2];
		}
		else
		{
			_imageBuffer[3*i+0]=_defaultBufferValues[0];
			_imageBuffer[3*i+1]=_defaultBufferValues[1];
			_imageBuffer[3*i+2]=_defaultBufferValues[2];
			_previousImageBuffer[3*i+0]=_defaultBufferValues[0];
			_previousImageBuffer[3*i+1]=_defaultBufferValues[1];
			_previousImageBuffer[3*i+2]=_defaultBufferValues[2];
		}
		_rgbBuffer[3*i+0]=0;
		_rgbBuffer[3*i+1]=0;
		_rgbBuffer[3*i+2]=0;
	}
	for (int i=0;i<_resolutionX*_resolutionY;i++)
		_depthBuffer[i]=1.0f;
}

bool CVisionSensor::getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices)
{
	vertices.clear();
	indices.clear();
	return(false); // for now
}

void CVisionSensor::display(CViewableBase* renderingObject,int displayAttrib)
{
	FUNCTION_INSIDE_DEBUG("CVisionSensor::display");
	EASYLOCK(_objectMutex);
	// At the beginning of every 3DObject display routine:
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

		glPushMatrix();

		if ((sensorResult.sensorWasTriggered&&_showVolumeWhenDetecting)||((!sensorResult.sensorWasTriggered)&&_showVolumeWhenNotDetecting))
		{
			if (sensorResult.sensorWasTriggered)
				detectionVolumeActiveColor.makeCurrentColor();
			else
				detectionVolumeColor.makeCurrentColor();
			C3Vector c,f;
			getSensingVolumeCorners(c,f);

			glBegin(GL_LINE_STRIP);
			glVertex3f(-f(0),-f(1),f(2));
			glVertex3f(-f(0),+f(1),f(2));
			glVertex3f(+f(0),+f(1),f(2));
			glVertex3f(+f(0),-f(1),f(2));
			glVertex3f(-f(0),-f(1),f(2));
			glEnd();
			glBegin(GL_LINE_STRIP);
			glVertex3f(-c(0),-c(1),c(2));
			glVertex3f(-c(0),+c(1),c(2));
			glVertex3f(+c(0),+c(1),c(2));
			glVertex3f(+c(0),-c(1),c(2));
			glVertex3f(-c(0),-c(1),c(2));
			glEnd();
			glBegin(GL_LINES);
			glVertex3f(-c(0),-c(1),c(2));
			glVertex3f(-f(0),-f(1),f(2));
			glVertex3f(-c(0),+c(1),c(2));
			glVertex3f(-f(0),+f(1),f(2));
			glVertex3f(+c(0),-c(1),c(2));
			glVertex3f(+f(0),-f(1),f(2));
			glVertex3f(+c(0),+c(1),c(2));
			glVertex3f(+f(0),+f(1),f(2));
			glEnd();

		}

		if (sensorResult.sensorWasTriggered)
			bodyActiveColor.makeCurrentColor();
		else
			bodyColor.makeCurrentColor();
		float dx=_size(0)*0.5f;
		float dy=_size(1)*0.5f;
		glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glVertex3f(-dx,-dy,0.0f);
		glVertex3f(-dx,dy,0.0f);
		glVertex3f(-dx,dy,-_size(2));
		glVertex3f(-dx,-dy,-_size(2));
		glNormal3f(0.0f,0.0f,-1.0f);
		glVertex3f(-dx,-dy,-_size(2));
		glVertex3f(-dx,dy,-_size(2));
		glVertex3f(dx,dy,-_size(2));
		glVertex3f(dx,-dy,-_size(2));
		glNormal3f(1.0f,0.0f,0.0f);
		glVertex3f(dx,-dy,-_size(2));
		glVertex3f(dx,dy,-_size(2));
		glVertex3f(dx,dy,0.0f);
		glVertex3f(dx,-dy,0.0f);
		glNormal3f(0.0f,-1.0f,0.0f);
		glVertex3f(-dx,-dy,0.0f);
		glVertex3f(-dx,-dy,-_size(2));
		glVertex3f(dx,-dy,-_size(2));
		glVertex3f(dx,-dy,0.0f);
		glNormal3f(0.0f,1.0f,0.0f);
		glVertex3f(-dx,dy,0.0f);
		glVertex3f(dx,dy,0.0f);
		glVertex3f(dx,dy,-_size(2));
		glVertex3f(-dx,dy,-_size(2));
		ogl::setColorsAllBlack();
		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3f(dx,-dy,0.0f);
		glVertex3f(dx,dy,0.0f);
		glVertex3f(-dx,dy,0.0f);
		glVertex3f(-dx,-dy,0.0f);
		glEnd();
		glPopMatrix();

		App::ct->objCont->disableAuxClippingPlanes();
	}

	// At the end of every 3DObject display routine:
	commonFinish();
}

void CVisionSensor::getSensingVolumeCorners(C3Vector& sizeAndPosClose,C3Vector& sizeAndPosFar)
{
	float r=float(_resolutionY)/float(_resolutionX);
	if (_perspectiveOperation)
	{
		if (r<=1.0f)
		{ // x is bigger
			sizeAndPosFar(0)=tan(_viewAngle/2.0f)*_farClippingPlane;
			sizeAndPosFar(1)=sizeAndPosFar(0)*r;
			sizeAndPosFar(2)=_farClippingPlane;

			sizeAndPosClose(0)=tan(_viewAngle/2.0f)*_nearClippingPlane;
			sizeAndPosClose(1)=sizeAndPosClose(0)*r;
			sizeAndPosClose(2)=_nearClippingPlane;
		}
		else
		{ // y is bigger
			sizeAndPosFar(1)=tan(_viewAngle/2.0f)*_farClippingPlane;
			sizeAndPosFar(0)=sizeAndPosFar(1)/r;
			sizeAndPosFar(2)=_farClippingPlane;

			sizeAndPosClose(1)=tan(_viewAngle/2.0f)*_nearClippingPlane;
			sizeAndPosClose(0)=sizeAndPosClose(1)/r;
			sizeAndPosClose(2)=_nearClippingPlane;
		}
	}
	else
	{
		if (r<=1.0f)
			sizeAndPosFar(0)=_orthoViewSize/2.0f; // x is bigger
		else
			sizeAndPosFar(0)=_orthoViewSize/(2.0f*r); // y is bigger
		sizeAndPosFar(1)=sizeAndPosFar(0)*r;
		sizeAndPosFar(2)=_farClippingPlane;

		sizeAndPosClose(0)=sizeAndPosFar(0);
		sizeAndPosClose(1)=sizeAndPosFar(1);
		sizeAndPosClose(2)=_nearClippingPlane;
	}
}

void CVisionSensor::getRealResolution(int r[2])
{
	r[0]=_resolutionX;
	r[1]=_resolutionY;
}

void CVisionSensor::setDesiredResolution(int r[2])
{
	tt::limitValue(1,2048,r[0]);
	tt::limitValue(1,2048,r[1]);
	_desiredResolution[0]=r[0];
	_desiredResolution[1]=r[1];
	_resolutionX=_desiredResolution[0];
	_resolutionY=_desiredResolution[1];
	_reserveBuffers();
	if (_composedFilter!=NULL)
		_composedFilter->removeBuffers();
}

void CVisionSensor::getDesiredResolution(int r[2])
{
	r[0]=_desiredResolution[0];
	r[1]=_desiredResolution[1];
}

void CVisionSensor::setUseEnvironmentBackgroundColor(bool s)
{
	_useSameBackgroundAsEnvironment=s;
}

bool CVisionSensor::getUseEnvironmentBackgroundColor()
{
	return(_useSameBackgroundAsEnvironment);
}

void CVisionSensor::setSize(const C3Vector& s)
{
	_size=s;
	for (int i=0;i<3;i++)
		tt::limitValue(0.0001f,100.0f,_size(i));
}

C3Vector CVisionSensor::getSize()
{
	return(_size);
}

void CVisionSensor::setPerspectiveOperation(bool p)
{
	_perspectiveOperation=p;
}

bool CVisionSensor::getPerspectiveOperation()
{
	return(_perspectiveOperation);
}

void CVisionSensor::setExplicitHandling(bool explicitHandl)
{
	_explicitHandling=explicitHandl;
}

bool CVisionSensor::getExplicitHandling()
{
	return (_explicitHandling);
}

void CVisionSensor::setShowVolumeWhenNotDetecting(bool s)
{
	_showVolumeWhenNotDetecting=s;
}
bool CVisionSensor::getShowVolumeWhenNotDetecting()
{
	return(_showVolumeWhenNotDetecting);
}
void CVisionSensor::setShowVolumeWhenDetecting(bool s)
{
	_showVolumeWhenDetecting=s;
}
bool CVisionSensor::getShowVolumeWhenDetecting()
{
	return(_showVolumeWhenDetecting);
}

void CVisionSensor::setIgnoreRGBInfo(bool ignore)
{
	_ignoreRGBInfo=ignore;
}
bool CVisionSensor::getIgnoreRGBInfo()
{
	return(_ignoreRGBInfo);
}
void CVisionSensor::setIgnoreDepthInfo(bool ignore)
{
	_ignoreDepthInfo=ignore;
}
bool CVisionSensor::getIgnoreDepthInfo()
{
	return(_ignoreDepthInfo);
}

void CVisionSensor::setColorCodeId(bool codeId)
{
	_colorCodeId=codeId;
}

bool CVisionSensor::getColorCodeId()
{
	return(_colorCodeId);
}


void CVisionSensor::setDetectableEntityID(int objID)
{
	_detectableEntityID=objID;
}

int CVisionSensor::getDetectableEntityID()
{
	return(_detectableEntityID);
}

void CVisionSensor::setDefaultBufferValues(const float v[3])
{
	for (int i=0;i<3;i++)
		_defaultBufferValues[i]=v[i];
}

void CVisionSensor::getDefaultBufferValues(float v[3])
{
	for (int i=0;i<3;i++)
		v[i]=_defaultBufferValues[i];
}

void CVisionSensor::resetSensor()
{
	sensorAuxiliaryResult.clear();
	sensorResult.sensorWasTriggered=false;
	sensorResult.sensorResultIsValid=false;
	sensorResult.calcTimeInMs=0;
	for (int i=0;i<3;i++)
	{
		sensorResult.sensorDataRed[i]=0.0f;
		sensorResult.sensorDataGreen[i]=0.0f;
		sensorResult.sensorDataBlue[i]=0.0f;
		sensorResult.sensorDataIntensity[i]=0.0f;
		sensorResult.sensorDataDepth[i]=0.0f;
	}
	if (!_useExternalImage) // condition added on 2010/12/21
		_clearBuffers();
}

bool CVisionSensor::setExternalImage(const float* img)
{
	swapImageBuffers();
	for (int i=0;i<_resolutionX*_resolutionY*3;i++)
		_imageBuffer[i]=img[i];
	_getDefaultReturnValues();
	bool returnValue=applyFilter(); // this might overwrite the default return values
	for (int i=0;i<3*_resolutionX*_resolutionY;i++)
		_rgbBuffer[i]=(unsigned char)(_imageBuffer[i]*255.0f);


	if (_contextFboAndTexture==NULL)
		App::uiThread->createGlContextAndFboAndTextureObjectIfNeeded(this,false); // We need to have this executed by the GUI thread!!

	if (_contextFboAndTexture!=NULL)
		_contextFboAndTexture->textureObject->setImage(false,false,true,_rgbBuffer); // Update the texture

	return(returnValue);
}

bool CVisionSensor::handleSensor()
{
	FUNCTION_DEBUG;
	sensorAuxiliaryResult.clear();
	sensorResult.sensorWasTriggered=false;
	sensorResult.sensorResultIsValid=false;
	sensorResult.calcTimeInMs=0;
	for (int i=0;i<3;i++)
	{
		sensorResult.sensorDataRed[i]=0.0f;
		sensorResult.sensorDataGreen[i]=0.0f;
		sensorResult.sensorDataBlue[i]=0.0f;
		sensorResult.sensorDataIntensity[i]=0.0f;
		sensorResult.sensorDataDepth[i]=0.0f;
	}
	if (!App::ct->mainSettings->visionSensorsEnabled)
		return(false);
	if (_useExternalImage) // those 2 lines added on 2010/12/12
		return(false);
	int stTime=VDateTime::getTimeInMs();
	detectEntity(_detectableEntityID,_detectableEntityID==-1,false,false,false,false);
	for (int i=0;i<3*_resolutionX*_resolutionY;i++)
		_rgbBuffer[i]=(unsigned char)(_imageBuffer[i]*255.0f);
	if (_contextFboAndTexture!=NULL)
		_contextFboAndTexture->textureObject->setImage(false,false,true,_rgbBuffer); // Update the texture
	sensorResult.calcTimeInMs=VDateTime::getTimeDiffInMs(stTime);
	return(sensorResult.sensorWasTriggered);
}

bool CVisionSensor::checkSensor(int entityID,bool overrideRenderableFlagsForNonCollections)
{ // This function should only be used by simCheckVisionSensor(Ex) functions! It will temporarily buffer current result
	if (_useExternalImage) // added those 2 lines on 2010/12/21
		return(false);

	// 1. We save current state:
	SHandlingResult cop;
	std::vector<std::vector<float> > sensorAuxiliaryResultCop(sensorAuxiliaryResult);
	sensorAuxiliaryResult.clear();
	cop.sensorWasTriggered=sensorResult.sensorWasTriggered;
	cop.sensorResultIsValid=sensorResult.sensorResultIsValid;
	cop.calcTimeInMs=sensorResult.calcTimeInMs;
	for (int i=0;i<3;i++)
	{
		cop.sensorDataRed[i]=sensorResult.sensorDataRed[i];
		cop.sensorDataGreen[i]=sensorResult.sensorDataGreen[i];
		cop.sensorDataBlue[i]=sensorResult.sensorDataBlue[i];
		cop.sensorDataIntensity[i]=sensorResult.sensorDataIntensity[i];
		cop.sensorDataDepth[i]=sensorResult.sensorDataDepth[i];
	}
	float* copIm=new float[_resolutionX*_resolutionY*3];
	float* copDep=new float[_resolutionX*_resolutionY];
	for (int i=0;i<_resolutionX*_resolutionY;i++)
	{
		copIm[3*i+0]=_imageBuffer[3*i+0];
		copIm[3*i+1]=_imageBuffer[3*i+1];
		copIm[3*i+2]=_imageBuffer[3*i+2];
		copDep[i]=_depthBuffer[i];
	}
	// 2. Do the detection:
	bool all=(entityID==-1);
	bool retVal=detectEntity(entityID,all,true,false,false,overrideRenderableFlagsForNonCollections); // We don't swap image buffers!
	// 3. Restore previous state:
	sensorResult.sensorWasTriggered=cop.sensorWasTriggered;
	sensorResult.sensorResultIsValid=cop.sensorResultIsValid;
	sensorResult.calcTimeInMs=cop.calcTimeInMs;
	sensorAuxiliaryResult.assign(sensorAuxiliaryResultCop.begin(),sensorAuxiliaryResultCop.end());
	for (int i=0;i<3;i++)
	{
		sensorResult.sensorDataRed[i]=cop.sensorDataRed[i];
		sensorResult.sensorDataGreen[i]=cop.sensorDataGreen[i];
		sensorResult.sensorDataBlue[i]=cop.sensorDataBlue[i];
		sensorResult.sensorDataIntensity[i]=cop.sensorDataIntensity[i];
		sensorResult.sensorDataDepth[i]=cop.sensorDataDepth[i];
	}
	for (int i=0;i<_resolutionX*_resolutionY;i++)
	{
		_imageBuffer[3*i+0]=copIm[3*i+0];
		_imageBuffer[3*i+1]=copIm[3*i+1];
		_imageBuffer[3*i+2]=copIm[3*i+2];
		_depthBuffer[i]=copDep[i];
	}
	// 4. Release memory
	delete[] copIm;
	delete[] copDep;

	return(retVal);
}

float* CVisionSensor::checkSensorEx(int entityID,bool imageBuffer,bool entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,bool hideEdgesIfModel,bool overrideRenderableFlagsForNonCollections)
{ // This function should only be used by simCheckVisionSensor(Ex) functions! It will temporarily buffer current result
	if (_useExternalImage) // added those 2 lines on 2010/12/21
		return(NULL);

	// 1. We save current state:
	SHandlingResult cop;
	std::vector<std::vector<float> > sensorAuxiliaryResultCop(sensorAuxiliaryResult);
	sensorAuxiliaryResult.clear();
	cop.sensorWasTriggered=sensorResult.sensorWasTriggered;
	cop.sensorResultIsValid=sensorResult.sensorResultIsValid;
	cop.calcTimeInMs=sensorResult.calcTimeInMs;
	for (int i=0;i<3;i++)
	{
		cop.sensorDataRed[i]=sensorResult.sensorDataRed[i];
		cop.sensorDataGreen[i]=sensorResult.sensorDataGreen[i];
		cop.sensorDataBlue[i]=sensorResult.sensorDataBlue[i];
		cop.sensorDataIntensity[i]=sensorResult.sensorDataIntensity[i];
		cop.sensorDataDepth[i]=sensorResult.sensorDataDepth[i];
	}
	float* copIm=new float[_resolutionX*_resolutionY*3];
	float* copDep=new float[_resolutionX*_resolutionY];
	for (int i=0;i<_resolutionX*_resolutionY;i++)
	{
		copIm[3*i+0]=_imageBuffer[3*i+0];
		copIm[3*i+1]=_imageBuffer[3*i+1];
		copIm[3*i+2]=_imageBuffer[3*i+2];
		copDep[i]=_depthBuffer[i];
	}
	// 2. Do the detection:
	bool all=(entityID==-1);
	detectEntity(entityID,all,true,entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,hideEdgesIfModel,overrideRenderableFlagsForNonCollections); // we don't swap image buffers!
	// 3. Prepare return buffer:
	float* retBuffer=NULL;
	int l=_resolutionX*_resolutionY;
	if (imageBuffer)
		l*=3;
	retBuffer=new float[l];
	if (imageBuffer)
	{
		for (int i=0;i<l;i++)
			retBuffer[i]=_imageBuffer[i];
	}
	else
	{
		for (int i=0;i<l;i++)
			retBuffer[i]=_depthBuffer[i];
	}
	// 4. Restore previous state:
	sensorAuxiliaryResult.assign(sensorAuxiliaryResultCop.begin(),sensorAuxiliaryResultCop.end());
	sensorResult.sensorWasTriggered=cop.sensorWasTriggered;
	sensorResult.sensorResultIsValid=cop.sensorResultIsValid;
	sensorResult.calcTimeInMs=cop.calcTimeInMs;
	for (int i=0;i<3;i++)
	{
		sensorResult.sensorDataRed[i]=cop.sensorDataRed[i];
		sensorResult.sensorDataGreen[i]=cop.sensorDataGreen[i];
		sensorResult.sensorDataBlue[i]=cop.sensorDataBlue[i];
		sensorResult.sensorDataIntensity[i]=cop.sensorDataIntensity[i];
		sensorResult.sensorDataDepth[i]=cop.sensorDataDepth[i];
	}
	for (int i=0;i<_resolutionX*_resolutionY;i++)
	{
		_imageBuffer[3*i+0]=copIm[3*i+0];
		_imageBuffer[3*i+1]=copIm[3*i+1];
		_imageBuffer[3*i+2]=copIm[3*i+2];
		_depthBuffer[i]=copDep[i];
	}
	// 5. Release memory
	delete[] copIm;
	delete[] copDep;

	return(retBuffer);
}

void CVisionSensor::createGlContextAndFboAndTextureObjectIfNeeded(bool useStencilBuffer)
{
	FUNCTION_DEBUG;
	if ((_contextFboAndTexture!=NULL)&&(_contextFboAndTexture->frameBufferObject->getUsingStencilBuffer()!=useStencilBuffer))
		_removeGlContextAndFboAndTextureObjectIfNeeded(); // if settings have changed (e.g. mirror was added), remove everything

	if (_contextFboAndTexture==NULL)
	{ // our objects are not yet there. Build them:

		QGLWidget* otherWidgetToShareResourcesWith=NULL;
		if (App::mainWindow!=NULL)
			otherWidgetToShareResourcesWith=App::mainWindow->openglWidget;

		// By default, we use QT_WINDOW_HIDE_TP, since
		// QT_OFFSCREEN_TP causes problems on certain GPUs, e.g.:
		// Intel Graphics Media Accelerator 3150
		int offscreenContextType=COffscreenGlContext::QT_WINDOW_HIDE_TP;
		if (App::userSettings->offscreenContextType!=-1)
		{
			if (App::userSettings->offscreenContextType==0)
				offscreenContextType=COffscreenGlContext::QT_OFFSCREEN_TP;
			if (App::userSettings->offscreenContextType==1)
				offscreenContextType=COffscreenGlContext::QT_WINDOW_SHOW_TP;
			if (App::userSettings->offscreenContextType==2)
				offscreenContextType=COffscreenGlContext::QT_WINDOW_HIDE_TP;
		}

		// By default:
		// - we use Qt FBOs on Mac (the non-Qt FBOs always caused problems there)
		// - we use non-Qt FBOs on Windows and Linux (Qt FBOs on Linux cause a lot of problems, e.g.: NVIDIA Geforce 9600M GS
#ifdef WIN_VREP
		bool nativeFbo=true;
#endif
#ifdef MAC_VREP
		bool nativeFbo=false;
#endif
#ifdef LIN_VREP
		bool nativeFbo=true;
#endif
		if (App::userSettings->fboType!=-1)
		{
			if (App::userSettings->fboType==0)
				nativeFbo=true;
			if (App::userSettings->fboType==1)
				nativeFbo=false;
		}

		_contextFboAndTexture=new CVisionSensorGlStuff(_resolutionX,_resolutionY,offscreenContextType,!nativeFbo,otherWidgetToShareResourcesWith,useStencilBuffer,!App::userSettings->oglCompatibilityTweak1,App::userSettings->desiredOpenGlMajor,App::userSettings->desiredOpenGlMinor);
	}
}

void CVisionSensor::_removeGlContextAndFboAndTextureObjectIfNeeded()
{
	FUNCTION_DEBUG;
	if (_contextFboAndTexture!=NULL)
	{
		if (_contextFboAndTexture->canDeleteNow())
			delete _contextFboAndTexture;
		else
			_contextFboAndTexture->deleteLater(); // We are in the wrong thread to delete it here
		_contextFboAndTexture=NULL;
	}
}

bool CVisionSensor::detectEntity(int entityID,bool detectAll,bool dontSwapImageBuffers,bool entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,bool hideEdgesIfModel,bool overrideRenderableFlagsForNonCollections)
{ // if entityID is -1, all detectable objects are rendered!
	FUNCTION_DEBUG;
	bool returnValue=false;
	App::ct->infoDisplay->visionSensorSimulationStart();

	// Following strange construction needed so that we can
	// do all the initialization/rendering in the UI thread:
	// - if not using an offscreen type (otherwise big problems and crashes)
	// - if this is not called from the main simulation thread or the UI thread (otherwise, hangs eventually (linked to QOpenGLContext that requires event handling somehow))

	// On Linux, depending on the openGl version, drivers and GPU, there can be many crashes if we do not handle
	// vision sensors in the UI thread. So, we handle everything in the UI thread by default:
	bool onlyGuiThread=true;
	if (App::mainWindow==NULL)
	{ // headless
		if (App::userSettings->visionSensorsUseGuiThread_headless==0)
			onlyGuiThread=false;
	}
	else
	{ // windowed
		if (App::userSettings->visionSensorsUseGuiThread_windowed==0)
			onlyGuiThread=false;
	}

	bool ui=VThread::isCurrentThreadTheUiThread();
	bool noAuxThread=VThread::isCurrentThreadTheUiThread()||VThread::isCurrentThreadTheMainThread();
	bool offscreen=(App::userSettings->offscreenContextType<1);
	if ( ui || ((noAuxThread&&offscreen)&&(!onlyGuiThread)) )
	{
		returnValue=detectEntity2(entityID,detectAll,dontSwapImageBuffers,entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,hideEdgesIfModel,overrideRenderableFlagsForNonCollections);
	}
	else
		App::uiThread->detectVisionSensorEntity(returnValue,this,entityID,detectAll,dontSwapImageBuffers,entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,hideEdgesIfModel,overrideRenderableFlagsForNonCollections);


	App::ct->infoDisplay->visionSensorSimulationEnd(returnValue);
	return(returnValue);
}

bool CVisionSensor::detectEntity2(int entityID,bool detectAll,bool dontSwapImageBuffers,bool entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,bool hideEdgesIfModel,bool overrideRenderableFlagsForNonCollections)
{ // if entityID is -1, all detectable objects are rendered!
	FUNCTION_DEBUG;
	bool returnValue=false;
	std::vector<int> activeMirrors;
	int activeMirrorCnt=_getActiveMirrors(entityID,detectAll,entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,overrideRenderableFlagsForNonCollections,activeMirrors);
	createGlContextAndFboAndTextureObjectIfNeeded(activeMirrorCnt>0);
	if (!_contextFboAndTexture->offscreenContext->makeCurrent())
	{ // we could not make current in the current thread: we erase the context and create a new one:
		_removeGlContextAndFboAndTextureObjectIfNeeded();
		createGlContextAndFboAndTextureObjectIfNeeded(activeMirrorCnt>0);
		_contextFboAndTexture->offscreenContext->makeCurrent();
	}
	_contextFboAndTexture->frameBufferObject->switchToFbo();

	if (!_useExternalImage)
		renderForDetection(entityID,detectAll,entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,hideEdgesIfModel,overrideRenderableFlagsForNonCollections,activeMirrors);

	if (!dontSwapImageBuffers)
		swapImageBuffers();

	if (!_useExternalImage)
	{
		if (!_ignoreRGBInfo)
			glReadPixels(0,0,_resolutionX,_resolutionY,GL_RGB,GL_FLOAT,_imageBuffer);
		if (!_ignoreDepthInfo)
		{
			glReadPixels(0,0,_resolutionX,_resolutionY,GL_DEPTH_COMPONENT,GL_FLOAT,_depthBuffer);
			// Convert this depth info into values corresponding to linear depths (if perspective mode):
			if (_perspectiveOperation)
			{
				float farMinusNear= _farClippingPlane-_nearClippingPlane;
				float farDivFarMinusNear=_farClippingPlane/farMinusNear;
				float nearTimesFar=_nearClippingPlane*_farClippingPlane;
				for (int i=0;i<_resolutionX*_resolutionY;i++)
					_depthBuffer[i]=((nearTimesFar/(farMinusNear*(farDivFarMinusNear-_depthBuffer[i])))-_nearClippingPlane)/farMinusNear;
			}
		}
	}

	_getDefaultReturnValues();
	returnValue=applyFilter(); // this might overwrite the default return values
	sensorResult.sensorWasTriggered=returnValue;

	if (App::userSettings->useGlFinish_visionSensors) // false by default!
		glFinish();	// Might be important later (synchronization problems)
			// removed on 2009/12/09 upon recomendation of gamedev community
			// re-put on 2010/01/11 because it slows down some graphic cards in a non-proportional way (e.g. 1 object=x ms, 5 objects=20x ms)
			// re-removed again (by default) on 31/01/2013. Thanks a lot to Cedric Pradalier for pointing problems appearing with the NVidia drivers

	_contextFboAndTexture->frameBufferObject->switchToNonFbo();
	_contextFboAndTexture->offscreenContext->doneCurrent(); // Important to free it if we want to use this context from a different thread!

	return(returnValue);
}

void CVisionSensor::renderForDetection(int entityID,bool detectAll,bool entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,bool hideEdgesIfModel,bool overrideRenderableFlagsForNonCollections,const std::vector<int>& activeMirrors)
{ // if entityID==-1, all objects that can be detected are rendered. 
	FUNCTION_DEBUG;
	glViewport(0,0,_resolutionX,_resolutionY);
	int currentWinSize[2]={_resolutionX,_resolutionY};

	if (_useSameBackgroundAsEnvironment)
		glClearColor(App::ct->environment->fogBackgroundColor[0],App::ct->environment->fogBackgroundColor[1],App::ct->environment->fogBackgroundColor[2],0.0f);
	else
		glClearColor(_defaultBufferValues[0],_defaultBufferValues[1],_defaultBufferValues[2],0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
//	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glRenderMode(GL_RENDER);

	float ratio=(float)(currentWinSize[0]/(float)currentWinSize[1]);
	if (_perspectiveOperation)
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
			glOrtho(-_orthoViewSize*0.5f,_orthoViewSize*0.5f,-_orthoViewSize*0.5f/ratio,_orthoViewSize*0.5f/ratio,_nearClippingPlane,_farClippingPlane);
		else
			glOrtho(-_orthoViewSize*0.5f*ratio,_orthoViewSize*0.5f*ratio,-_orthoViewSize*0.5f,_orthoViewSize*0.5f,_nearClippingPlane,_farClippingPlane);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	
	C4X4Matrix m4(getCumulativeTransformation().getMatrix());
	// The following 6 instructions have the same effect as gluLookAt()
	m4.inverse();
	m4.rotateAroundY(piValue);
	float m4_[4][4];
	m4.copyTo(m4_);
	CMeshManip::transposeMatrix_4x4Array(m4_);
	glLoadMatrixf((float*)m4_);
	

	if (!_colorCodeId)
		App::ct->objCont->actualizeLights(this,false);
	else
	{
		App::ct->environment->activateAmbientLight(false);
		for (int i=0;i<CIloIlo::maximumOpenGlLights;i++)
			glDisable(GL_LIGHT0+i);
	}

	glInitNames();
	glPushName(-1);
	glLoadName(-1);

	glShadeModel(GL_SMOOTH);

	if (!_colorCodeId)
	{
		glEnable(GL_DITHER);
		glEnable(GL_LIGHTING);
	}
	else
	{
		glDisable(GL_DITHER);
		glDisable(GL_LIGHTING);
	}
	ogl::lockColors(false);

	// Set data for view frustum culling
	_planesCalculated=false;
	_currentViewSize[0]=_resolutionX;
	_currentViewSize[1]=_resolutionY;
	_currentPerspective=_perspectiveOperation;

	C7Vector camTr(getCumulativeTransformation());
	C7Vector camTri(camTr.getInverse());

	// Handle mirrors here (not very elegant!)
	if (activeMirrors.size()!=0)
	{
		setFrustumCullingTemporarilyDisabled(true);
		// Prep stencil buffer:
		glEnable(GL_STENCIL_TEST);
		glClearStencil(0);
		glClear (GL_STENCIL_BUFFER_BIT);
		int drawOk=1;

		std::vector<int> allMirrors;
		std::vector<float> allMirrorDist;
		for (int mir=0;mir<int(activeMirrors.size());mir++)
		{
			CMirror* myMirror=App::ct->objCont->getMirror(activeMirrors[mir]);
			C7Vector mmtr(myMirror->getCumulativeTransformation());
			mmtr=camTri*mmtr;
			allMirrors.push_back(activeMirrors[mir]);
			allMirrorDist.push_back(mmtr.X(2));
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
			bool inFrontOfMirror=(((MirrCam*mtrN)>0.0f)&&myMirror->getActive());

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
				_drawObjects(entityID,detectAll,entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,hideEdgesIfModel,overrideRenderableFlagsForNonCollections);
				CMirror::currentMirrorContentBeingRendered=-1;
				glFrontFace (GL_CCW);
				glPopMatrix();
				glDisable(GL_CLIP_PLANE0);
			}

			// Now draw the mirror overlay:
			glPushAttrib (0xffffffff);
			glDisable (GL_LIGHTING);
			glDepthFunc(GL_ALWAYS);
			if (inFrontOfMirror)
			{
				glEnable (GL_BLEND);
				glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			glColor4f (myMirror->mirrorColor[0],myMirror->mirrorColor[1],myMirror->mirrorColor[2],1.0f-myMirror->getReflectance());
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
	_drawObjects(entityID,detectAll,entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,hideEdgesIfModel,overrideRenderableFlagsForNonCollections);

	glDisable(GL_FOG);
	glRenderMode(GL_RENDER);	
}

void CVisionSensor::_drawObjects(int entityID,bool detectAll,bool entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,bool hideEdgesIfModel,bool overrideRenderableFlagsForNonCollections)
{ // if entityID==-1, all objects that can be detected are rendered. 
	FUNCTION_DEBUG;
	// Very unelegant routine. Needs badly refactoring!

	int rendAttrib=attributesForVisionSensorRendering;

	if (_colorCodeId)
		rendAttrib|=sim_displayattribute_colorcoded;

	C3DObject* object=App::ct->objCont->getObject(entityID);
	CRegGroup* collection=NULL;
	std::vector<C3DObject*> toRender;
	std::vector<int> transparentObjects;
	std::vector<float> transparentObjectsDist;
	C7Vector camTrInv(getCumulativeTransformationPart1().getInverse());
	C3DObject* viewBoxObject=NULL;
	if (object==NULL)
	{
		collection=App::ct->collections->getGroup(entityID);
		if (collection!=NULL)
		{
			bool overridePropertyFlag=collection->getOverridesObjectMainProperties();
			for (int i=0;i<int(collection->groupObjects.size());i++)
			{
				C3DObject* it=App::ct->objCont->getObject(collection->groupObjects[i]);
				if (it!=NULL)
				{
					if  ( ((it->getMainProperty()&sim_objectspecialproperty_renderable)!=0)||overridePropertyFlag )
					{ // supposed to be rendered
						if (it->getObjectType()==sim_object_shape_type)
						{
							CShape* sh=(CShape*)it;
							if (sh->getContainsTransparentComponent())
							{
								C7Vector obj(it->getCumulativeTransformationPart1());
								transparentObjectsDist.push_back(-(camTrInv*obj).X(2));
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
									transparentObjectsDist.push_back(-(camTrInv*obj).X(2));
									transparentObjects.push_back(it->getID());
								}
								else
									toRender.push_back(it);
							}
							else
								toRender.push_back(it);
						}
						if (it->getParent()!=NULL)
						{ // We need this because the dummy that is the base of the skybox is not renderable!
							if (it->getParent()->getName()==IDSOGL_SKYBOX_DO_NOT_RENAME)
								viewBoxObject=it->getParent();
						}
					}
				}
			}
		}
		else
		{ // Here we want to detect all detectable objects maybe:
			if (detectAll)
			{
				for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
				{
					C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
					if (it!=NULL)
					{
						if  ( ( (it->getMainProperty()&sim_objectspecialproperty_renderable)!=0 )||overrideRenderableFlagsForNonCollections ) //overrideRenderableFlagsForNonCollections new since 17/6/2011
						{ // supposed to be rendered
							if (it->getObjectType()==sim_object_shape_type)
							{
								CShape* sh=(CShape*)it;
								if (sh->getContainsTransparentComponent())
								{
									C7Vector obj(it->getCumulativeTransformationPart1());
									transparentObjectsDist.push_back(-(camTrInv*obj).X(2));
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
										transparentObjectsDist.push_back(-(camTrInv*obj).X(2));
										transparentObjects.push_back(it->getID());
									}
									else
										toRender.push_back(it);
								}
								else
									toRender.push_back(it);
							}
							if (it->getParent()!=NULL)
							{ // We need this because the dummy that is the base of the skybox is not renderable!
								if (it->getParent()->getName()==IDSOGL_SKYBOX_DO_NOT_RENAME)
									viewBoxObject=it->getParent();
							}
						}
					}
				}
			}
		}
	}
	else
	{ // We want to detect a single object (no collection not all objects in the scene)
		if (!entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects)
		{
			if  ( ( (object->getMainProperty()&sim_objectspecialproperty_renderable)!=0 )||overrideRenderableFlagsForNonCollections ) //overrideRenderableFlagsForNonCollections added on 17/6/2011
			{
				toRender.push_back(object);
				if (object->getParent()!=NULL)
				{ // We need this because the dummy that is the base of the skybox is not renderable!
					if (object->getParent()->getName()==IDSOGL_SKYBOX_DO_NOT_RENAME)
						viewBoxObject=object->getParent();
				}
			}
		}
		else
		{ // we have a model here that we want to render. We render also non-renderable object. And only those currently visible:
			std::vector<int> rootSel;
			rootSel.push_back(object->getID());
			CObjectEdition::addRootObjectChildrenToSelection(rootSel);
			for (int i=0;i<int(rootSel.size());i++)
			{
				C3DObject* it=App::ct->objCont->getObject(rootSel[i]);
				if (App::ct->mainSettings->activeLayers&it->layer)
				{ // ok, currently visible
					if (it->getObjectType()==sim_object_shape_type)
					{
						CShape* sh=(CShape*)it;
						if (sh->getContainsTransparentComponent())
						{
							C7Vector obj(it->getCumulativeTransformationPart1());
							transparentObjectsDist.push_back(-(camTrInv*obj).X(2));
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
								transparentObjectsDist.push_back(-(camTrInv*obj).X(2));
								transparentObjects.push_back(it->getID());
							}
							else
								toRender.push_back(it);
						}
						else
							toRender.push_back(it);
					}
				}
			}
		}
	}
	tt::orderAscending(transparentObjectsDist,transparentObjects);
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
		{ // This doesn't work!!
			C3Vector minV,maxV;
			bool first=true;
			viewBoxObject->getGlobalMarkingBoundingBox(getCumulativeTransformation().getInverse(),minV,maxV,first,false);
			float shift=getFarClippingPlane()-0.505f*(maxV(2)-minV(2)); // just a bit more than half!
			cam.X+=cam.Q.getMatrix().axis[2]*shift;
		}
		C7Vector newLocal(viewBoxObject->getParentCumulativeTransformation().getInverse()*cam);
		newLocal.Q=rel;
		viewBoxObject->setLocalTransformation(newLocal);
	}
	//***************************************

	App::ct->objCont->prepareAuxClippingPlanes();
	App::ct->objCont->enableAuxClippingPlanes(-1);
	// first non-transparent attached drawing objects:
	for (int i=0;i<int(toRender.size());i++)
		App::ct->drawingCont->drawObjectsParentedWith(false,false,toRender[i]->getID(),rendAttrib,getCumulativeTransformation().getMatrix());
	// Now the same as above but for non-attached drawing objects:
	App::ct->drawingCont->drawObjectsParentedWith(false,false,-1,rendAttrib,getCumulativeTransformation().getMatrix());
	App::ct->objCont->disableAuxClippingPlanes();

	for (int i=0;i<int(toRender.size());i++)
	{
		if (!entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects)
		{
			toRender[i]->display(this,rendAttrib); // attached non-transparent objects were rendered before
		}
		else
		{ // we render the scene twice when we have edges, since antialiasing might not be beautiful otherwise
			int atr=sim_displayattribute_renderpass;
			toRender[i]->display(this,atr|sim_displayattribute_forbidedges); // attached non-transparent objects were rendered before
			if (!hideEdgesIfModel)
				toRender[i]->display(this,atr); // attached object were rendered before
		}
	}

	App::ct->objCont->enableAuxClippingPlanes(-1);
	// Transparent attached drawing objects:
	for (int i=0;i<int(toRender.size());i++)
		App::ct->drawingCont->drawObjectsParentedWith(false,true,toRender[i]->getID(),rendAttrib,getCumulativeTransformation().getMatrix());
	// Now the same as above but for non-attached drawing objects:
	App::ct->drawingCont->drawObjectsParentedWith(false,true,-1,rendAttrib,getCumulativeTransformation().getMatrix());

	// Point clouds:
	App::ct->pointCloudCont->renderYour3DStuff_nonTransparent(this,rendAttrib);

	// overlay attached drawing objects:
	for (int i=0;i<int(toRender.size());i++)
		App::ct->drawingCont->drawObjectsParentedWith(true,true,toRender[i]->getID(),rendAttrib,getCumulativeTransformation().getMatrix());
	// Now the same as above but for non-attached drawing objects:
	App::ct->drawingCont->drawObjectsParentedWith(true,true,-1,rendAttrib,getCumulativeTransformation().getMatrix());

	// particles:
	App::ct->dynamicsContainer->renderYour3DStuff(this,rendAttrib);
	App::ct->objCont->disableAuxClippingPlanes();
}

int CVisionSensor::_getActiveMirrors(int entityID,bool detectAll,bool entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects,bool overrideRenderableFlagsForNonCollections,std::vector<int>& activeMirrors)
{
	if (App::ct->objCont->mirrorList.size()==0)
		return(0);
	if (App::ct->mainSettings->mirrorsDisabled)
		return(0);
	C3DObject* object=App::ct->objCont->getObject(entityID);
	CRegGroup* collection=NULL;
	std::vector<C3DObject*> toRender;
	if (object==NULL)
	{
		collection=App::ct->collections->getGroup(entityID);
		if (collection!=NULL)
		{
			bool overridePropertyFlag=collection->getOverridesObjectMainProperties();
			for (int i=0;i<int(collection->groupObjects.size());i++)
			{
				C3DObject* it=App::ct->objCont->getObject(collection->groupObjects[i]);
				if (it!=NULL)
				{
					if  ( ((it->getMainProperty()&sim_objectspecialproperty_renderable)!=0)||overridePropertyFlag )
					{ // supposed to be rendered
						toRender.push_back(it);
					}
				}
			}
		}
		else
		{ // Here we want to detect all detectable objects maybe:
			if (detectAll)
			{
				for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
				{
					C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
					if (it!=NULL)
					{
						if  ( ( (it->getMainProperty()&sim_objectspecialproperty_renderable)!=0 )||overrideRenderableFlagsForNonCollections ) //overrideRenderableFlagsForNonCollections new since 17/6/2011
						{ // supposed to be rendered
							toRender.push_back(it);
						}
					}
				}
			}
		}
	}
	else
	{ // We want to detect a single object (no collection not all objects in the scene)
		if (!entityIsModelAndRenderAllVisibleModelAlsoNonRenderableObjects)
		{
			if  ( ( (object->getMainProperty()&sim_objectspecialproperty_renderable)!=0 )||overrideRenderableFlagsForNonCollections ) //overrideRenderableFlagsForNonCollections added on 17/6/2011
			{
				toRender.push_back(object);
			}
		}
		else
		{ // we have a model here that we want to render. We render also non-renderable object. And only those currently visible:
			std::vector<int> rootSel;
			rootSel.push_back(object->getID());
			CObjectEdition::addRootObjectChildrenToSelection(rootSel);
			for (int i=0;i<int(rootSel.size());i++)
			{
				C3DObject* it=App::ct->objCont->getObject(rootSel[i]);
				if (App::ct->mainSettings->activeLayers&it->layer)
				{ // ok, currently visible
					toRender.push_back(it);
				}
			}
		}
	}
	int retVal=0;
	for (int i=0;i<int(toRender.size());i++)
	{
		C3DObject* it=toRender[i];
		if (it->getObjectType()==sim_object_mirror_type)
		{
			CMirror* mi=(CMirror*)it;
			if (mi->getActive()&&mi->getIsMirror())
			{
				activeMirrors.push_back(it->getID());
				retVal++;
			}
		}
	}
	return(retVal);
}

void CVisionSensor::_getDefaultReturnValues()
{
	sensorAuxiliaryResult.clear();
	sensorResult.sensorResultIsValid=true;
	for (int i=0;i<3;i++)
	{
		sensorResult.sensorDataRed[i]=0.0f;
		sensorResult.sensorDataGreen[i]=0.0f;
		sensorResult.sensorDataBlue[i]=0.0f;
		sensorResult.sensorDataIntensity[i]=0.0f;
		sensorResult.sensorDataDepth[i]=0.0f;
	}
	float cumulRed=0.0f;
	float cumulGreen=0.0f;
	float cumulBlue=0.0f;
	float cumulIntensity=0.0f;
	float cumulDepth=0.0f;
	for (int i=0;i<_resolutionX*_resolutionY;i++)
	{
		float intens=(_imageBuffer[3*i+0]+_imageBuffer[3*i+1]+_imageBuffer[3*i+2])/3.0f;
		cumulRed+=_imageBuffer[3*i+0];
		cumulGreen+=_imageBuffer[3*i+1];
		cumulBlue+=_imageBuffer[3*i+2];
		cumulDepth+=_depthBuffer[i];
		cumulIntensity+=intens;
		if (i==0)
		{ // We initialize min/max values:
			for (int j=0;j<2;j++)
			{
				sensorResult.sensorDataRed[j]=_imageBuffer[3*i+0];
				sensorResult.sensorDataGreen[j]=_imageBuffer[3*i+1];
				sensorResult.sensorDataBlue[j]=_imageBuffer[3*i+2];
				sensorResult.sensorDataIntensity[j]=intens;
				sensorResult.sensorDataDepth[j]=_depthBuffer[i];
			}
		}
		else
		{ // we actualize min/max values:
			// Red
			if (_imageBuffer[3*i+0]<sensorResult.sensorDataRed[0])
				sensorResult.sensorDataRed[0]=_imageBuffer[3*i+0];
			if (_imageBuffer[3*i+0]>sensorResult.sensorDataRed[1])
				sensorResult.sensorDataRed[1]=_imageBuffer[3*i+0];
			// Green
			if (_imageBuffer[3*i+1]<sensorResult.sensorDataGreen[0])
				sensorResult.sensorDataGreen[0]=_imageBuffer[3*i+1];
			if (_imageBuffer[3*i+1]>sensorResult.sensorDataGreen[1])
				sensorResult.sensorDataGreen[1]=_imageBuffer[3*i+1];
			// Blue
			if (_imageBuffer[3*i+2]<sensorResult.sensorDataBlue[0])
				sensorResult.sensorDataBlue[0]=_imageBuffer[3*i+2];
			if (_imageBuffer[3*i+2]>sensorResult.sensorDataBlue[1])
				sensorResult.sensorDataBlue[1]=_imageBuffer[3*i+2];
			// Intensity
			if (intens<sensorResult.sensorDataIntensity[0])
				sensorResult.sensorDataIntensity[0]=intens;
			if (intens>sensorResult.sensorDataIntensity[1])
				sensorResult.sensorDataIntensity[1]=intens;
			// Depth
			if (_depthBuffer[i]<sensorResult.sensorDataDepth[0])
				sensorResult.sensorDataDepth[0]=_depthBuffer[i];
			if (_depthBuffer[i]>sensorResult.sensorDataDepth[1])
				sensorResult.sensorDataDepth[1]=_depthBuffer[i];
		}
	}
	float averageRed=cumulRed/float(_resolutionX*_resolutionY);
	float averageGreen=cumulGreen/float(_resolutionX*_resolutionY);
	float averageBlue=cumulBlue/float(_resolutionX*_resolutionY);
	float averageIntensity=cumulIntensity/float(_resolutionX*_resolutionY);
	float averageDepth=cumulDepth/float(_resolutionX*_resolutionY);
	// We set-up average values:
	sensorResult.sensorDataRed[2]=averageRed;
	sensorResult.sensorDataGreen[2]=averageGreen;
	sensorResult.sensorDataBlue[2]=averageBlue;
	sensorResult.sensorDataIntensity[2]=averageIntensity;
	sensorResult.sensorDataDepth[2]=averageDepth;

	// We prepare the auxiliary values:
	std::vector<float> defaultResults;
	defaultResults.push_back(sensorResult.sensorDataIntensity[0]);
	defaultResults.push_back(sensorResult.sensorDataRed[0]);
	defaultResults.push_back(sensorResult.sensorDataGreen[0]);
	defaultResults.push_back(sensorResult.sensorDataBlue[0]);
	defaultResults.push_back(sensorResult.sensorDataDepth[0]);

	defaultResults.push_back(sensorResult.sensorDataIntensity[1]);
	defaultResults.push_back(sensorResult.sensorDataRed[1]);
	defaultResults.push_back(sensorResult.sensorDataGreen[1]);
	defaultResults.push_back(sensorResult.sensorDataBlue[1]);
	defaultResults.push_back(sensorResult.sensorDataDepth[1]);

	defaultResults.push_back(sensorResult.sensorDataIntensity[2]);
	defaultResults.push_back(sensorResult.sensorDataRed[2]);
	defaultResults.push_back(sensorResult.sensorDataGreen[2]);
	defaultResults.push_back(sensorResult.sensorDataBlue[2]);
	defaultResults.push_back(sensorResult.sensorDataDepth[2]);
	sensorAuxiliaryResult.push_back(defaultResults);
}

void CVisionSensor::lookAt(CSView* viewObject,int viewPos[2],int viewSize[2])
{	// viewPos and viewSize can be NULL.
	FUNCTION_DEBUG;
	int currentWinSize[2];
	int currentWinPos[2];
	if (viewObject!=NULL)
	{
		viewObject->getViewSize(currentWinSize);
		viewObject->getViewPosition(currentWinPos);
	}
	else
	{
		currentWinSize[0]=viewSize[0];
		currentWinSize[1]=viewSize[1];
		currentWinPos[0]=viewPos[0];
		currentWinPos[1]=viewPos[1];
	}
	
	ogl::setColorsAllBlack();
	glEnable(GL_SCISSOR_TEST);	
	glViewport(currentWinPos[0],currentWinPos[1],currentWinSize[0],currentWinSize[1]);
	ogl::allLightsOffAmbientFull();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f,currentWinSize[0],0.0f,currentWinSize[1],-1.0f,1.0f);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glDisable(GL_DEPTH_TEST);


	glClearColor(0.3f,0.3f,0.3f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	if (_contextFboAndTexture!=NULL)
	{
		float r0=float(currentWinSize[1])/float(currentWinSize[0]);
		float r1=float(_resolutionY)/float(_resolutionX);
		int c0[2];
		int c1[2];
		if (r1>=r0)
		{
			c0[1]=0;
			c1[1]=currentWinSize[1];
			int d=int(float(currentWinSize[1])/r1);
			c0[0]=(currentWinSize[0]-d)/2;
			c1[0]=c0[0]+d;

		}
		else
		{
			c0[0]=0;
			c1[0]=currentWinSize[0];
			int d=int(float(currentWinSize[0])*r1);
			c0[1]=(currentWinSize[1]-d)/2;
			c1[1]=c0[1]+d;
		}

		ogl::setColor(1.0f,1.0f,1.0f,AMBIENT_MODE);
		float texCorners[4]={0.0f,0.0f,1.0f,1.0f};
		_contextFboAndTexture->textureObject->startTextureDisplay(false,false,false,false);

		glBegin(GL_QUADS);
		glTexCoord2f(texCorners[0],texCorners[1]);
		glVertex3i(c0[0],c0[1],0);
		glTexCoord2f(texCorners[0],texCorners[3]);
		glVertex3i(c0[0],c1[1],0);
		glTexCoord2f(texCorners[2],texCorners[3]);
		glVertex3i(c1[0],c1[1],0);
		glTexCoord2f(texCorners[2],texCorners[1]);
		glVertex3i(c1[0],c0[1],0);
		glEnd();
		_contextFboAndTexture->textureObject->endTextureDisplay();

		if (_composedFilter!=NULL)
			_composedFilter->displayOverlay(c0,c1);
	}


	glDisable(GL_SCISSOR_TEST);	
}

CTextureObject* CVisionSensor::getTextureObject()
{
	if (_contextFboAndTexture!=NULL)
		return(_contextFboAndTexture->textureObject);
	return(NULL);
}

void CVisionSensor::scaleObject(float scalingFactor)
{
	_nearClippingPlane*=scalingFactor;
	_farClippingPlane*=scalingFactor;

	_orthoViewSize*=scalingFactor;
	_size=_size*scalingFactor;
	scaleObjectMain(scalingFactor);
}

C3DObject* CVisionSensor::copyYourself()
{	
	CVisionSensor* newVisionSensor=(CVisionSensor*)copyYourselfMain();

	delete newVisionSensor->_composedFilter;
	newVisionSensor->_composedFilter=_composedFilter->copyYourself();

	newVisionSensor->_viewAngle=_viewAngle;
	newVisionSensor->_orthoViewSize=_orthoViewSize;
	newVisionSensor->_nearClippingPlane=_nearClippingPlane;
	newVisionSensor->_farClippingPlane=_farClippingPlane;
	newVisionSensor->_showFogIfAvailable=_showFogIfAvailable;
	newVisionSensor->_useLocalLights=_useLocalLights;

	newVisionSensor->_resolutionX=_resolutionX;
	newVisionSensor->_resolutionY=_resolutionY;
	newVisionSensor->_desiredResolution[0]=_desiredResolution[0];
	newVisionSensor->_desiredResolution[1]=_desiredResolution[1];
	for (int i=0;i<3;i++) // Important to do it before setting the sensor type
		newVisionSensor->_defaultBufferValues[i]=_defaultBufferValues[i];
	newVisionSensor->_perspectiveOperation=_perspectiveOperation;
	newVisionSensor->_size=_size;
	newVisionSensor->_detectableEntityID=_detectableEntityID;
	newVisionSensor->_useExternalImage=_useExternalImage;
	newVisionSensor->_useSameBackgroundAsEnvironment=_useSameBackgroundAsEnvironment;

	newVisionSensor->_explicitHandling=_explicitHandling;
	newVisionSensor->_showVolumeWhenNotDetecting=_showVolumeWhenNotDetecting;
	newVisionSensor->_showVolumeWhenDetecting=_showVolumeWhenDetecting;

	newVisionSensor->_ignoreRGBInfo=_ignoreRGBInfo;
	newVisionSensor->_ignoreDepthInfo=_ignoreDepthInfo;
	newVisionSensor->_colorCodeId=_colorCodeId;

	for (int i=0;i<3;i++)
	{
		newVisionSensor->sensorResult.sensorDataRed[i]=0.0f;
		newVisionSensor->sensorResult.sensorDataGreen[i]=0.0f;
		newVisionSensor->sensorResult.sensorDataBlue[i]=0.0f;
		newVisionSensor->sensorResult.sensorDataIntensity[i]=0.0f;
		newVisionSensor->sensorResult.sensorDataDepth[i]=0.0f;
	}
	newVisionSensor->sensorAuxiliaryResult.assign(sensorAuxiliaryResult.begin(),sensorAuxiliaryResult.end());
	newVisionSensor->sensorResult.sensorWasTriggered=false;
	newVisionSensor->sensorResult.sensorResultIsValid=false;


	bodyColor.copyYourselfInto(&newVisionSensor->bodyColor);
	bodyActiveColor.copyYourselfInto(&newVisionSensor->bodyActiveColor);
	detectionVolumeColor.copyYourselfInto(&newVisionSensor->detectionVolumeColor);
	detectionVolumeActiveColor.copyYourselfInto(&newVisionSensor->detectionVolumeActiveColor);
	newVisionSensor->_reserveBuffers(); // important!

	newVisionSensor->_initialValuesInitialized=_initialValuesInitialized;
	newVisionSensor->_initialExplicitHandling=_initialExplicitHandling;

	return(newVisionSensor);
}

bool CVisionSensor::announceObjectWillBeErased(int objID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	// This routine can be called for objCont-objects, but also for objects
	// in the copy-buffer!! So never make use of any 
	// 'ct::objCont->getObject(id)'-call or similar
	// Return value true means 'this' has to be erased too!
	if (_detectableEntityID==objID)
		_detectableEntityID=-1;
	announceObjectWillBeErasedMain(objID,copyBuffer);
	return(false);
}

void CVisionSensor::announceGroupWillBeErased(int groupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	if (_detectableEntityID==groupID)
		_detectableEntityID=-1;
	announceGroupWillBeErasedMain(groupID,copyBuffer);
}
void CVisionSensor::announceCollisionWillBeErased(int collisionID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceCollisionWillBeErasedMain(collisionID,copyBuffer);
}
void CVisionSensor::announceDistanceWillBeErased(int distanceID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceDistanceWillBeErasedMain(distanceID,copyBuffer);
}
void CVisionSensor::announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceIkObjectWillBeErasedMain(ikGroupID,copyBuffer);
}
void CVisionSensor::announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGcsObjectWillBeErasedMain(gcsObjectID,copyBuffer);
}
void CVisionSensor::performObjectLoadingMapping(std::vector<int>* map)
{ // New_Object_ID=map[Old_Object_ID]
	performObjectLoadingMappingMain(map);
	if (_detectableEntityID<SIM_IDSTART_COLLECTION)
		_detectableEntityID=App::ct->objCont->getLoadingMapping(map,_detectableEntityID);
}
void CVisionSensor::performGroupLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performGroupLoadingMappingMain(map);
	if (_detectableEntityID>=SIM_IDSTART_COLLECTION)
		_detectableEntityID=App::ct->objCont->getLoadingMapping(map,_detectableEntityID);
}
void CVisionSensor::performCollisionLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performCollisionLoadingMappingMain(map);
}
void CVisionSensor::performDistanceLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performDistanceLoadingMappingMain(map);
}
void CVisionSensor::performIkLoadingMapping(std::vector<int>* map)
{
	performIkLoadingMappingMain(map);
}
void CVisionSensor::performGcsLoadingMapping(std::vector<int>* map)
{
	performGcsLoadingMappingMain(map);
}
void CVisionSensor::performTextureObjectLoadingMapping(std::vector<int>* map)
{
	performTextureObjectLoadingMappingMain(map);
}
void CVisionSensor::performDynMaterialObjectLoadingMapping(std::vector<int>* map)
{
	performDynMaterialObjectLoadingMappingMain(map);
}
void CVisionSensor::bufferMainDisplayStateVariables()
{
	bufferMainDisplayStateVariablesMain();
}
void CVisionSensor::bufferedMainDisplayStateVariablesToDisplay()
{
	bufferedMainDisplayStateVariablesToDisplayMain();
}

void CVisionSensor::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	initializeInitialValuesMain(simulationIsRunning);
	_initialValuesInitialized=simulationIsRunning;
	for (int i=0;i<3;i++)
	{
		sensorResult.sensorDataRed[i]=0.0f;
		sensorResult.sensorDataGreen[i]=0.0f;
		sensorResult.sensorDataBlue[i]=0.0f;
		sensorResult.sensorDataIntensity[i]=0.0f;
		sensorResult.sensorDataDepth[i]=0.0f;
	}
	sensorAuxiliaryResult.clear();
	sensorResult.sensorWasTriggered=false;
	sensorResult.sensorResultIsValid=false;
	sensorResult.calcTimeInMs=0;
	if (_composedFilter!=NULL)
		_composedFilter->initializeInitialValues(simulationIsRunning);
	if (simulationIsRunning)
	{
		_initialExplicitHandling=_explicitHandling;
	}
	else
	{
		_removeGlContextAndFboAndTextureObjectIfNeeded();
		if (_composedFilter!=NULL)
		{
			_composedFilter->drawingContainer.removeAllObjects();
			_composedFilter->simulationEnded();
		}
	}
}


void CVisionSensor::simulationAboutToStart()
{
	initializeInitialValues(true);
}

void CVisionSensor::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	_removeGlContextAndFboAndTextureObjectIfNeeded();
	if (_composedFilter!=NULL)
	{
		_composedFilter->drawingContainer.removeAllObjects();
		_composedFilter->simulationEnded();
	}
	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd()&&((getCumulativeMainPropertyOverride()&sim_modelproperty_not_reset)==0))
	{
		_explicitHandling=_initialExplicitHandling;
	}
	_initialValuesInitialized=false;
	simulationEndedMain();
}

void CVisionSensor::swapImageBuffers()
{
	float* tmp=_imageBuffer;
	_imageBuffer=_previousImageBuffer;
	_previousImageBuffer=tmp;
}

bool CVisionSensor::applyFilter()
{
	float* outputImage=CImageProcess::createRGBImage(_resolutionX,_resolutionY);
	bool trigg=_composedFilter->processAndTrigger(this,_resolutionX,_resolutionY,_imageBuffer,_depthBuffer,outputImage,sensorAuxiliaryResult);
	int s=_resolutionX*_resolutionY*3;
	for (int i=0;i<s;i++)
		_imageBuffer[i]=outputImage[i];
	CImageProcess::deleteImage(outputImage);
	return(trigg);
}

void CVisionSensor::serialize(CSer& ar)
{
	serializeMain(ar);
	if (ar.isStoring())
	{ // Storing
		ar.storeDataName("Ccp");
		ar << _orthoViewSize << _viewAngle;
		ar << _nearClippingPlane << _farClippingPlane;
		ar.flush();

		ar.storeDataName("Res");
		ar << _desiredResolution[0] << _desiredResolution[1];
		ar.flush();

		ar.storeDataName("Dox");
		ar << _detectableEntityID;
		ar.flush();

		ar.storeDataName("Db2");
		ar << _defaultBufferValues[0] << _defaultBufferValues[1] << _defaultBufferValues[2];
		ar.flush();

		ar.storeDataName("Siz");
		ar << _size(0) << _size(1) << _size(2);
		ar.flush();

		ar.storeDataName("Va3");
		BYTE nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,_perspectiveOperation);
		SIM_SET_CLEAR_BIT(nothing,1,_explicitHandling);
		SIM_SET_CLEAR_BIT(nothing,2,!_showFogIfAvailable);
//12/12/2011		SIM_SET_CLEAR_BIT(nothing,3,_detectAllDetectable);
		SIM_SET_CLEAR_BIT(nothing,4,_showVolumeWhenNotDetecting);
		SIM_SET_CLEAR_BIT(nothing,5,_showVolumeWhenDetecting);
		SIM_SET_CLEAR_BIT(nothing,6,_useLocalLights);
		SIM_SET_CLEAR_BIT(nothing,7,_useSameBackgroundAsEnvironment);
		ar << nothing;
		ar.flush();

		ar.storeDataName("Va2");
		nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,_useExternalImage);
		SIM_SET_CLEAR_BIT(nothing,1,_ignoreRGBInfo);
		SIM_SET_CLEAR_BIT(nothing,2,_ignoreDepthInfo);
		SIM_SET_CLEAR_BIT(nothing,3,_colorCodeId);
		ar << nothing;
		ar.flush();

		ar.storeDataName("Cl1");
		ar.setCountingMode();
		bodyColor.serialize(ar);
		if (ar.setWritingMode())
			bodyColor.serialize(ar);

		ar.storeDataName("Cl2");
		ar.setCountingMode();
		bodyActiveColor.serialize(ar);
		if (ar.setWritingMode())
			bodyActiveColor.serialize(ar);

		ar.storeDataName("Cl3");
		ar.setCountingMode();
		detectionVolumeColor.serialize(ar);
		if (ar.setWritingMode())
			detectionVolumeColor.serialize(ar);

		ar.storeDataName("Cl4");
		ar.setCountingMode();
		detectionVolumeActiveColor.serialize(ar);
		if (ar.setWritingMode())
			detectionVolumeActiveColor.serialize(ar);

		ar.storeDataName("Cfr");
		ar.setCountingMode();
		_composedFilter->serialize(ar);
		if (ar.setWritingMode())
			_composedFilter->serialize(ar);

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
				if (theName.compare("Ccp")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _orthoViewSize >> _viewAngle;
					ar >> _nearClippingPlane >> _farClippingPlane;
				}
				if (theName.compare("Res")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _desiredResolution[0] >> _desiredResolution[1];
					setDesiredResolution(_desiredResolution); // we need to set the real resolution!
				}
				if (theName.compare("Dox")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _detectableEntityID;
				}
				if (theName.compare("Db2")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _defaultBufferValues[0] >> _defaultBufferValues[1] >> _defaultBufferValues[2];
				}
				if (theName.compare("Siz")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _size(0) >> _size(1) >> _size(2);
				}
				if (theName=="Var")
				{ // keep a while for backward compatibility (2010/07/17)
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_perspectiveOperation=SIM_IS_BIT_SET(nothing,0);
					_explicitHandling=SIM_IS_BIT_SET(nothing,1);
					bool hideDetectionVolume=SIM_IS_BIT_SET(nothing,2);
					_useSameBackgroundAsEnvironment=SIM_IS_BIT_SET(nothing,7);
					if (hideDetectionVolume)
					{
						_showVolumeWhenNotDetecting=false;
						_showVolumeWhenDetecting=false;
					}
				}
				if (theName=="Va3")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_perspectiveOperation=SIM_IS_BIT_SET(nothing,0);
					_explicitHandling=SIM_IS_BIT_SET(nothing,1);
					_showFogIfAvailable=!SIM_IS_BIT_SET(nothing,2);
					_showVolumeWhenNotDetecting=SIM_IS_BIT_SET(nothing,4);
					_showVolumeWhenDetecting=SIM_IS_BIT_SET(nothing,5);
					_useLocalLights=SIM_IS_BIT_SET(nothing,6);
					_useSameBackgroundAsEnvironment=SIM_IS_BIT_SET(nothing,7);
				}
				if (theName=="Va2")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					_useExternalImage=SIM_IS_BIT_SET(nothing,0);
					_ignoreRGBInfo=SIM_IS_BIT_SET(nothing,1);
					_ignoreDepthInfo=SIM_IS_BIT_SET(nothing,2);
					_colorCodeId=SIM_IS_BIT_SET(nothing,3);
				}
				if (theName.compare("Cl1")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					bodyColor.serialize(ar);
				}
				if (theName.compare("Cl2")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					bodyActiveColor.serialize(ar);
				}
				if (theName.compare("Cl3")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					detectionVolumeColor.serialize(ar);
				}
				if (theName.compare("Cl4")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					detectionVolumeActiveColor.serialize(ar);
				}
				if (theName.compare("Cfr")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					delete _composedFilter;
					_composedFilter=new CComposedFilter();
					_composedFilter->serialize(ar);
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
		_reserveBuffers();

		if ((ar.getSerializationVersionThatWroteThisFile()<17)&&App::userSettings->correctColorsAndLights)
		{ // on 29/08/2013 we corrected all default lights. So we need to correct for that change:
			CTTUtil::scaleColorUp_(bodyColor.colors);
			CTTUtil::scaleColorUp_(bodyActiveColor.colors);
			CTTUtil::scaleColorUp_(detectionVolumeColor.colors);
			CTTUtil::scaleColorUp_(detectionVolumeActiveColor.colors);
		}
	}
}

void CVisionSensor::serializeWExtIk(CExtIkSer& ar)
{
	serializeWExtIkMain(ar);
	CDummy::serializeWExtIkStatic(ar);
}
