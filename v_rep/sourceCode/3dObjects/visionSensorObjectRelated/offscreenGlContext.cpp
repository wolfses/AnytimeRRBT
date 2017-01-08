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
#include "offscreenGlContext.h"
#include "App.h"

std::vector<QOpenGLContext*> COffscreenGlContext::_allQtContexts;
std::vector<QGLWidget*> COffscreenGlContext::_allQtWidgets;

#ifdef WIN_VREP
std::vector<HGLRC> COffscreenGlContext::_allNativeContexts;

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, Message, wParam, lParam);
}
#endif

COffscreenGlContext::COffscreenGlContext(int offscreenType,int resX,int resY,QGLWidget *otherWidgetToShareResourcesWith,int majorOpenGl,int minorOpenGl) : QObject()
{
	// 1. QT_OFFSCREEN_TP can be started from any thread, also when running in headless mode. However:
	// QOpenGLContext somehow requires that the thread that created it also processes its events on
	// a regular basis, otherwise the UI thread will hang out of the blue somehow.
	// The UI thread has its own App::qtApp->processEvents(), same for the SIM thread.
	// Auxiliary SIM threads (started via threaded scripts) will ask the UI thread to do
	// QOpenGLContext initialization and rendering.
	//
	// 2. QT_WINDOW_SHOW_TP and QT_WINDOW_HIDE_TP can only be started from the UI thread
	// (also when running in headless mode), otherwise we have crashes. So in that situation
	// if we notice we are not running the UI thread, we ask the UI thread to do the
	// QGLWidget initialization and rendering.

	FUNCTION_DEBUG;
	_offscreenType=offscreenType;
	_initialThread=QThread::currentThread();

	if (_offscreenType==QT_OFFSCREEN_TP)
	{
		_qOffscreenSurface=new QOffscreenSurface();
		QSurfaceFormat f;
		f.setSwapBehavior(QSurfaceFormat::SingleBuffer);
		f.setRenderableType(QSurfaceFormat::OpenGL);
		f.setRedBufferSize(8);
		f.setGreenBufferSize(8);
		f.setBlueBufferSize(8);
		f.setAlphaBufferSize(0);
		f.setStencilBufferSize(8);
		f.setDepthBufferSize(24);
		if (majorOpenGl!=-1)
			f.setMajorVersion(majorOpenGl);
		if (minorOpenGl!=-1)
			f.setMinorVersion(minorOpenGl);
		_qOffscreenSurface->setFormat(f);
		_qOffscreenSurface->create();
		if (_qOffscreenSurface->isValid())
		{
			_qContext=new QOpenGLContext();
			_qContext->setFormat(_qOffscreenSurface->format());
			if (otherWidgetToShareResourcesWith!=NULL)
				_qContext->setShareContext(otherWidgetToShareResourcesWith->context()->contextHandle());
			else
			{
				if (_allQtContexts.size()!=0)
					_qContext->setShareContext(_allQtContexts[0]);
			}
			_qContext->create();
			if (_qContext->isValid())
				_allQtContexts.push_back(_qContext);
			else
			{ // context creation failed. Use an invisible window instead:
				delete _qContext;
				_qOffscreenSurface->destroy();
				delete _qOffscreenSurface;
				_offscreenType=QT_WINDOW_HIDE_TP;
				FUNCTION_INSIDE_DEBUG("ERROR: Failed creating an offscreen QOpenGLContext. Switching to a hidden QGLWidget.");
				printf("ERROR: Failed creating an offscreen QOpenGLContext. Switching to a hidden QGLWidget.\n");
			}
		}
		else
		{ // surface creation failed. Use an invisible window instead:
			delete _qOffscreenSurface;
			_offscreenType=QT_WINDOW_HIDE_TP;
			FUNCTION_INSIDE_DEBUG("ERROR: Failed creating a QOffscreenSurface. Switching to a hidden QGLWidget.");
			printf("ERROR: Failed creating a QOffscreenSurface. Switching to a hidden QGLWidget.\n");
		}
	}

	if ((_offscreenType==QT_WINDOW_SHOW_TP)||(_offscreenType==QT_WINDOW_HIDE_TP))
	{
		if ((otherWidgetToShareResourcesWith==NULL)&&(_allQtWidgets.size()!=0))
			otherWidgetToShareResourcesWith=_allQtWidgets[0];

		if ((majorOpenGl!=-1)||(minorOpenGl!=-1))
		{
			QGLFormat fmt;
			fmt.setDoubleBuffer(false);
			// fmt.setDirectRendering(false);   // default is true
			fmt.setRedBufferSize(8);
			fmt.setGreenBufferSize(8);
			fmt.setBlueBufferSize(8);
			fmt.setAlphaBufferSize(0);
			fmt.setStencilBufferSize(8);
			fmt.setDepthBufferSize(24);
			if ((majorOpenGl!=-1)||(minorOpenGl!=-1))
			{
				if (majorOpenGl==-1)
					majorOpenGl=2;
				if (minorOpenGl==-1)
					minorOpenGl=0;
				fmt.setVersion(majorOpenGl,minorOpenGl);
			}
			_hiddenWindow=new QGLWidget(fmt,App::mainWindow,otherWidgetToShareResourcesWith,Qt::Tool);
		}
		else
			_hiddenWindow=new QGLWidget(App::mainWindow,otherWidgetToShareResourcesWith,Qt::Tool);

		_hiddenWindow->setFixedWidth(resX);
		_hiddenWindow->setFixedHeight(resY);

		if (_offscreenType==QT_WINDOW_SHOW_TP)
			_hiddenWindow->show();
		_allQtWidgets.push_back(_hiddenWindow);
	}

	if ((_offscreenType==NATIVE_WINDOW_SHOW_TP)||(_offscreenType==NATIVE_WINDOW_HIDE_TP))
	{
#ifdef WIN_VREP
		WNDCLASSEX wc;
		ZeroMemory( &wc, sizeof( wc ) );
		wc.cbSize = sizeof( wc );
		wc.lpfnWndProc = WndProc;
		wc.hInstance = 0;
		wc.hIcon = 0;
		wc.hIconSm = 0;
		wc.hCursor  = LoadCursor( NULL, IDC_ARROW );
		wc.lpszClassName = TEXT("vrepOffscreen");
		RegisterClassEx( &wc );
		_windowHandle = CreateWindowEx( WS_EX_CLIENTEDGE, TEXT("vrepOffscreen"), TEXT("vrepOffscreen"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, resX, resY, NULL, NULL, GetModuleHandle( NULL ), NULL );
		if (_offscreenType==NATIVE_WINDOW_SHOW_TP)
			ShowWindow(_windowHandle,SW_SHOW); // showing the window makes it faster somehow!!!
//		_windowHandle=GetDesktopWindow();
		_hdc=GetDC(_windowHandle);
		const BYTE depthBufferResolution=16;
		int bits=24; // Color bits (excluding alpha) // changed from 16 to 24 on 2009/12/08
		static PIXELFORMATDESCRIPTOR pfd={sizeof(PIXELFORMATDESCRIPTOR),1,
			PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,bits,0,0,0,0,0,0,0,0,0,0,0,0,0,depthBufferResolution,0,0,
			PFD_MAIN_PLANE,0,0,0,0};
		unsigned int pixelFormat=ChoosePixelFormat(_hdc,&pfd);
		SetPixelFormat(_hdc,pixelFormat,&pfd);
		_nativeContext=wglCreateContext(_hdc);
		_allNativeContexts.push_back(_nativeContext);
		printf("COffscreenGlContext code missing (resource sharing between contexts).\n");
		VREP_BEEP;
#endif

#ifdef MAC_VREP
		// CGLCreateContext
		printf("COffscreenGlContext code missing.\n");
		VREP_BEEP;
#endif

#ifdef LIN_VREP
		// glXCreateContext
		printf("COffscreenGlContext code missing.\n");
		VREP_BEEP;
#endif
	}

	makeCurrent();
	App::initGl_ifNeeded();
}

COffscreenGlContext::~COffscreenGlContext()
{
	FUNCTION_DEBUG;
	if (_offscreenType==QT_OFFSCREEN_TP)
	{
		for (int i=0;i<int(_allQtContexts.size());i++)
		{
			if (_allQtContexts[i]==_qContext)
			{
				_allQtContexts.erase(_allQtContexts.begin()+i);
				break;
			}
		}
		delete _qContext;
		_qOffscreenSurface->destroy();
		delete _qOffscreenSurface;
	}

	if ((_offscreenType==QT_WINDOW_SHOW_TP)||(_offscreenType==QT_WINDOW_HIDE_TP))
	{
		for (int i=0;i<int(_allQtWidgets.size());i++)
		{
			if (_allQtWidgets[i]==_hiddenWindow)
			{
				_allQtWidgets.erase(_allQtWidgets.begin()+i);
				break;
			}
		}
		_hiddenWindow->hide(); // required, otherwise crash at simulator exit when in headless mode
		_hiddenWindow->deleteLater(); // delete _hiddenWindow works also (with hide first) under Windows
	}

	if ((_offscreenType==NATIVE_WINDOW_SHOW_TP)||(_offscreenType==NATIVE_WINDOW_HIDE_TP))
	{
#ifdef WIN_VREP
		for (int i=0;i<int(_allNativeContexts.size());i++)
		{
			if (_allNativeContexts[i]==_nativeContext)
			{
				_allNativeContexts.erase(_allNativeContexts.begin()+i);
				break;
			}
		}
		wglDeleteContext(_nativeContext);
		DestroyWindow(_windowHandle);
#endif

#ifdef MAC_VREP
		// CGLDeleteContext
		printf("COffscreenGlContext code missing.\n");
		VREP_BEEP;
#endif

#ifdef LIN_VREP
		// glXDeleteContext
		printf("COffscreenGlContext code missing.\n");
		VREP_BEEP;
#endif
	}
}

bool COffscreenGlContext::canBeDeleted()
{
	return(QThread::currentThread()==_initialThread);
}

bool COffscreenGlContext::makeCurrent()
{
	FUNCTION_DEBUG;
	if (_offscreenType==QT_OFFSCREEN_TP)
	{
		if (QThread::currentThread()==_initialThread)
		{
			_qContext->makeCurrent(_qOffscreenSurface);
			return(true);
		}
		else
			return(false); // we cannot pull a Qt context from another thread!
	}

	if ((_offscreenType==QT_WINDOW_SHOW_TP)||(_offscreenType==QT_WINDOW_HIDE_TP))
	{
		if (QThread::currentThread()==_initialThread)
		{
			_hiddenWindow->makeCurrent();
			return(true);
		}
		else
			return(false); // we cannot pull a Qt context from another thread!
	}

	if ((_offscreenType==NATIVE_WINDOW_SHOW_TP)||(_offscreenType==NATIVE_WINDOW_HIDE_TP))
	{
#ifdef WIN_VREP
		wglMakeCurrent(_hdc,_nativeContext);
#endif

#ifdef MAC_VREP
		// CGLMakeCurrent
		printf("COffscreenGlContext code missing.\n");
		VREP_BEEP;
#endif

#ifdef LIN_VREP
		// glXMakeCurrent
		printf("COffscreenGlContext code missing.\n");
		VREP_BEEP;
#endif
		return(true);
	}
	return(false);
}

bool COffscreenGlContext::doneCurrent()
{
	FUNCTION_DEBUG;
	if (_offscreenType==QT_OFFSCREEN_TP)
	{
		if (QThread::currentThread()==_initialThread)
		{
			_qContext->doneCurrent();
			return(true);
		}
	}

	if ((_offscreenType==QT_WINDOW_SHOW_TP)||(_offscreenType==QT_WINDOW_HIDE_TP))
	{
		if (QThread::currentThread()==_initialThread)
		{
			_hiddenWindow->doneCurrent();
			return(true);
		}
	}

	if ((_offscreenType==NATIVE_WINDOW_SHOW_TP)||(_offscreenType==NATIVE_WINDOW_HIDE_TP))
	{
#ifdef WIN_VREP
		wglMakeCurrent(NULL,NULL);
#endif

#ifdef MAC_VREP
		// CGLMakeCurrent
		printf("COffscreenGlContext code missing.\n");
		VREP_BEEP;
#endif

#ifdef LIN_VREP
		// glXMakeCurrent
		printf("COffscreenGlContext code missing.\n");
		VREP_BEEP;
#endif
		return(true);
	}

	if (QThread::currentThread()!=_initialThread)
	{
		printf("Do not call COffscreenGlContext::doneCurrent() from a non-initial thread.\n");
		VREP_BEEP;
	}
	return(false);
}

void COffscreenGlContext::moveGlContextToThread(QThread* otherThread)
{ // not used!
	FUNCTION_DEBUG;
	if (_offscreenType==QT_OFFSCREEN_TP)
		_qContext->moveToThread(otherThread);

	if ((_offscreenType==QT_WINDOW_SHOW_TP)||(_offscreenType==QT_WINDOW_HIDE_TP))
		_hiddenWindow->context()->moveToThread(otherThread);

	_initialThread=otherThread;
}

