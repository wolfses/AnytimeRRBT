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

#ifdef WIN_VREP
//	#include <qgl.h>
//	#include <GL/gl.h>
//	#include "glext.h"
//	#define GL_DEPTH24_STENCIL8_EXT 0x88F0
#endif

#ifdef LIN_VREP
	#include <GL/glext.h>
	#include <GL/gl.h>
	#include <GL/glx.h>
	#include <GL/glxext.h>
#endif

#ifdef MAC_VREP
//	#define GL_GLEXT_FUNCTION_POINTERS 1
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>
	#include <OpenGL/gl3.h>
	#include <OpenGL/glext.h>
	#include <OpenGL/gl3ext.h>
#endif

#ifndef MAC_VREP
	extern PFNGLGENFRAMEBUFFERSEXTPROC _glGenFramebuffers;
	extern PFNGLDELETEFRAMEBUFFERSEXTPROC _glDeleteFramebuffers;
	extern PFNGLBINDFRAMEBUFFEREXTPROC _glBindFramebuffer;
	extern PFNGLGENRENDERBUFFERSEXTPROC _glGenRenderbuffers;
	extern PFNGLDELETERENDERBUFFERSEXTPROC _glDeleteRenderbuffers;
	extern PFNGLBINDRENDERBUFFEREXTPROC _glBindRenderbuffer;
	extern PFNGLRENDERBUFFERSTORAGEEXTPROC _glRenderbufferStorage;
	extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC _glFramebufferRenderbuffer;
	extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC _glCheckFramebufferStatus;
	extern PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC _glGetRenderbufferParameteriv;
#endif

//FULLY STATIC CLASS
class oglExt 
{
public:
	static void prepareExtensionFunctions(bool forceFboToUseExt);
	static void turnOffVSync(int vsync);
	static bool isFboAvailable();
	static bool _isFboAvailable(bool &viaExt);
	static bool areNonPowerOfTwoTexturesAvailable();
	static void initDefaultGlValues();


	static void GenFramebuffers(GLsizei a,GLuint* b);
	static void DeleteFramebuffers(GLsizei a,const GLuint* b);
	static void BindFramebuffer(GLenum a,GLuint b);
	static void GenRenderbuffers(GLsizei a,GLuint* b);
	static void DeleteRenderbuffers(GLsizei a,const GLuint* b);
	static void BindRenderbuffer(GLenum a,GLuint b);
	static void RenderbufferStorage(GLenum a,GLenum b,GLsizei c,GLsizei d);
	static void FramebufferRenderbuffer(GLenum a,GLenum b,GLenum c,GLuint d);
	static void CheckFramebufferStatus(GLenum a);
	static void GetRenderbufferParameteriv(GLenum a,GLenum b,GLint* c);

#ifndef MAC_VREP
	static PFNGLGENFRAMEBUFFERSEXTPROC _glGenFramebuffers;
	static PFNGLDELETEFRAMEBUFFERSEXTPROC _glDeleteFramebuffers;
	static PFNGLBINDFRAMEBUFFEREXTPROC _glBindFramebuffer;
	static PFNGLGENRENDERBUFFERSEXTPROC _glGenRenderbuffers;
	static PFNGLDELETERENDERBUFFERSEXTPROC _glDeleteRenderbuffers;
	static PFNGLBINDRENDERBUFFEREXTPROC _glBindRenderbuffer;
	static PFNGLRENDERBUFFERSTORAGEEXTPROC _glRenderbufferStorage;
	static PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC _glFramebufferRenderbuffer;
	static PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC _glCheckFramebufferStatus;
	static PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC _glGetRenderbufferParameteriv;
#endif

	static GLenum DEPTH24_STENCIL8;
	static GLenum FRAMEBUFFER;
	static GLenum RENDERBUFFER;
	static GLenum DEPTH_ATTACHMENT;
	static GLenum DEPTH_STENCIL_ATTACHMENT;
	static GLenum COLOR_ATTACHMENT0;

	static bool _usingExt;
};
