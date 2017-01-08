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
#include "frameBufferObject.h"
#include "oglExt.h"

CFrameBufferObject::CFrameBufferObject(bool native,int resX,int resY,bool useStencilBuffer) : QObject()
{
	FUNCTION_DEBUG;

	if (!oglExt::isFboAvailable())
		native=false;

	_native=native;
	_usingStencilBuffer=useStencilBuffer;

	_initialThread=QThread::currentThread();

	if (_native)
	{
		unsigned int theMode=GL_DEPTH_COMPONENT;
		unsigned int attachment=oglExt::DEPTH_ATTACHMENT;
		if (_usingStencilBuffer)
		{
			theMode=oglExt::DEPTH24_STENCIL8;
			attachment=oglExt::DEPTH_STENCIL_ATTACHMENT;
		}

		oglExt::GenFramebuffers(1,&_fbo);
		oglExt::BindFramebuffer(oglExt::FRAMEBUFFER,_fbo);

		oglExt::GenRenderbuffers(1,&_fboDepthBuffer);
		oglExt::BindRenderbuffer(oglExt::RENDERBUFFER,_fboDepthBuffer);

		oglExt::RenderbufferStorage(oglExt::RENDERBUFFER,theMode,resX,resY);
		oglExt::FramebufferRenderbuffer(oglExt::FRAMEBUFFER,attachment,oglExt::RENDERBUFFER,_fboDepthBuffer);

		oglExt::GenRenderbuffers(1,&_fboPictureBuffer);
		oglExt::BindRenderbuffer(oglExt::RENDERBUFFER,_fboPictureBuffer);

		oglExt::RenderbufferStorage(oglExt::RENDERBUFFER,GL_RGB,resX,resY);
		oglExt::FramebufferRenderbuffer(oglExt::FRAMEBUFFER,oglExt::COLOR_ATTACHMENT0,oglExt::RENDERBUFFER,_fboPictureBuffer);
	}
	else
	{
		QOpenGLFramebufferObject::Attachment attachment=QOpenGLFramebufferObject::Depth;
		if (_usingStencilBuffer)
			attachment=QOpenGLFramebufferObject::CombinedDepthStencil;
		_frameBufferObject = new QOpenGLFramebufferObject(resX,resY,attachment,GL_TEXTURE_2D,GL_RGBA8); // GL_RGB);
	}
}

CFrameBufferObject::~CFrameBufferObject()
{
	FUNCTION_DEBUG;
	switchToNonFbo();
	if (_native)
	{
		oglExt::DeleteRenderbuffers(1,&_fboPictureBuffer);
		oglExt::DeleteRenderbuffers(1,&_fboDepthBuffer);
		oglExt::DeleteFramebuffers(1,&_fbo);
	}
	else
		delete _frameBufferObject;
}

bool CFrameBufferObject::canBeDeleted()
{
	return(_initialThread==QThread::currentThread());
}

void CFrameBufferObject::switchToFbo()
{
	FUNCTION_DEBUG;
	if (_native)
		oglExt::BindFramebuffer(oglExt::FRAMEBUFFER,_fbo);
	else
		_frameBufferObject->bind();
}

void CFrameBufferObject::switchToNonFbo()
{
	FUNCTION_DEBUG;
	if (_native)
		oglExt::BindFramebuffer(oglExt::FRAMEBUFFER,0);
	else
		_frameBufferObject->release();
}

bool CFrameBufferObject::getUsingStencilBuffer()
{
	return(_usingStencilBuffer);
}
