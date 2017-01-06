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
#include "visionSensorGlStuff.h"
#include "glShader.h"

CVisionSensorGlStuff::CVisionSensorGlStuff(int resX,int resY,int offscreenType,bool qtFbo,QGLWidget* otherWidgetToShareResourcesWith,bool useStencilBuffer,bool destroyOffscreenContext,int majorOpenGl,int minorOpenGl) : QObject()
{
	_destroyOffscreenContext=destroyOffscreenContext;

	// 1. We need an offscreen context:
	offscreenContext=new COffscreenGlContext(offscreenType,resX,resY,otherWidgetToShareResourcesWith,majorOpenGl,minorOpenGl);

	// 2. We need an FBO:
	frameBufferObject=new CFrameBufferObject(!qtFbo,resX,resY,useStencilBuffer);

	// 3. We need a texture object:
	textureObject=new CTextureObject(resX,resY);

//	CGlShader* a=new CGlShader();

	offscreenContext->doneCurrent();
}

CVisionSensorGlStuff::~CVisionSensorGlStuff()
{
	FUNCTION_DEBUG;
	offscreenContext->makeCurrent();
	delete textureObject;
	delete frameBufferObject;
	offscreenContext->doneCurrent();

	// On some buggy GPU drivers, destroying the context here might hinder further renderings in other contexts:
	if (_destroyOffscreenContext)
		delete offscreenContext;
}

bool CVisionSensorGlStuff::canDeleteNow()
{
	return(offscreenContext->canBeDeleted());
}
