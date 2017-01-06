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

#include "qvrep_openglwidget.h"
#include "IloIlo.h"
#include <QMouseEvent>
#include "VDateTime.h"
#include "ttUtil.h"
#include "App.h"

//COpenglWidget::COpenglWidget(QWidget *parent) : QGLWidget(parent)
COpenglWidget::COpenglWidget(QWidget *parent) : QGLWidget(QGLFormat((App::userSettings->stereoDist<=0.0f) ? (QGL::DoubleBuffer) : (QGL::StereoBuffers)),parent)
{
	_openGlDisplayEnabled=true;
	setMouseTracking(true);
	setFocusPolicy(Qt::WheelFocus);
}

COpenglWidget::~COpenglWidget()
{
}

void COpenglWidget::setOpenGlDisplayEnabled(bool e)
{
	_openGlDisplayEnabled=e;
}

bool COpenglWidget::getOpenGlDisplayEnabled()
{
	return(_openGlDisplayEnabled);
}

void COpenglWidget::initializeGL()
{
	setAutoBufferSwap(false);
}

void COpenglWidget::paintEvent(QPaintEvent* event)
{
//	QGLWidget::paintEvent(event);
}

void COpenglWidget::resizeEvent(QResizeEvent* event)
{
//	QGLWidget::resizeEvent(event);

	float sx=1.0f;
	float sy=1.0f;
	if (App::userSettings->retinaScaling2X>0.01f)
		sx=App::userSettings->retinaScaling2X;
	if (App::userSettings->retinaScaling2Y>0.01f)
		sy=App::userSettings->retinaScaling2Y;
	int width=int(float(event->size().width())*sx+0.5f);
	int height=int(float(event->size().height())*sy+0.5f);

	resizeGL(width,height);
//	resizeGL(event->size().width(),event->size().height());
}

void COpenglWidget::paintGL()
{
}

void COpenglWidget::resizeGL(int width, int height)
{
	float sx=windowHandle()->devicePixelRatio();
	float sy=windowHandle()->devicePixelRatio();
	if (App::userSettings->retinaScaling1X>0.01f)
		sx=App::userSettings->retinaScaling1X;
	if (App::userSettings->retinaScaling1Y>0.01f)
		sy=App::userSettings->retinaScaling1Y;
	width=int(float(width)*sx+0.5f);
	height=int(float(height)*sy+0.5f);

	App::mainWindow->simulationRecorder->setRecordingSizeChanged(width,height);
	App::mainWindow->clientArea.x=width;
	App::mainWindow->clientArea.y=height;
	App::mainWindow->clientPos.x=0;
	App::mainWindow->clientPos.y=0;
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::mainWindow->recomputeClientSizeAndPos();
		App::mainWindow->renderOpenGlContent_callFromRenderingThreadOnly();
	//	App::mainWindow->dlgCont->refresh(); // Removed on 7/6/2014 because of some crashes, and this is anyway not needed
	}
}

void COpenglWidget::mouseMoveEvent(QMouseEvent* mEvent)
{
	float sx=windowHandle()->devicePixelRatio();
	float sy=windowHandle()->devicePixelRatio();
	if (App::userSettings->retinaScaling3X>0.01f)
		sx=App::userSettings->retinaScaling3X;
	if (App::userSettings->retinaScaling3Y>0.01f)
		sy=App::userSettings->retinaScaling3Y;
	int x=int(float(mEvent->x())*sx+0.5f);
	int y=int(float(mEvent->y())*sy+0.5f);

	App::mainWindow->ctrlKeyDown=((mEvent->modifiers()&Qt::ControlModifier)!=0);
	App::mainWindow->shiftKeyDown=((mEvent->modifiers()&Qt::ShiftModifier)!=0);
	int cur=-1;
	IF_UI_EVENT_CAN_READ_DATA
	{
		App::mainWindow->onMouseMoveTT(x,y); // This will also call recomputeMousePos
		if (_openGlDisplayEnabled)
			cur=App::ct->oglSurface->getCursor(x,App::mainWindow->clientArea.y-y);
	}
	if (cur!=-1)
		App::mainWindow->_currentCursor=cur;
	else
		App::mainWindow->_currentCursor=sim_cursor_arrow;
	static int previousCursor=-1;
	if (previousCursor!=App::mainWindow->_currentCursor)
	{
		setCursor(Qt::PointingHandCursor);
		if (App::mainWindow->_currentCursor==sim_cursor_arrow)
			setCursor(Qt::ArrowCursor);
		if (App::mainWindow->_currentCursor==sim_cursor_finger)
			setCursor(Qt::PointingHandCursor);
		if (App::mainWindow->_currentCursor==sim_cursor_all_directions)
			setCursor(Qt::SizeAllCursor);
		if (App::mainWindow->_currentCursor==sim_cursor_horizontal_directions)
			setCursor(Qt::SizeHorCursor);
		if (App::mainWindow->_currentCursor==sim_cursor_vertical_directions)
			setCursor(Qt::SizeVerCursor);
		if (App::mainWindow->_currentCursor==sim_cursor_slash_directions)
			setCursor(Qt::SizeBDiagCursor);
		if (App::mainWindow->_currentCursor==sim_cursor_backslash_directions)
			setCursor(Qt::SizeFDiagCursor);
	}
}

void COpenglWidget::mousePressEvent(QMouseEvent* mEvent)
{
	float sx=windowHandle()->devicePixelRatio();
	float sy=windowHandle()->devicePixelRatio();
	if (App::userSettings->retinaScaling3X>0.01f)
		sx=App::userSettings->retinaScaling3X;
	if (App::userSettings->retinaScaling3Y>0.01f)
		sy=App::userSettings->retinaScaling3Y;
	int x=int(float(mEvent->x())*sx+0.5f);
	int y=int(float(mEvent->y())*sy+0.5f);

	App::mainWindow->ctrlKeyDown=((mEvent->modifiers()&Qt::ControlModifier)!=0);
	App::mainWindow->shiftKeyDown=((mEvent->modifiers()&Qt::ShiftModifier)!=0);
	if (mEvent->button()==Qt::LeftButton)
	{
		CMainWindow::leftMouseDown=true;
		IF_UI_EVENT_CAN_READ_DATA
		{
			App::mainWindow->onLeftMouseButtonDownTT(x,y);
		}
		App::mainWindow->_mouseButtonsState|=1;
	}
	if (!(App::isFullScreen()))
	{
		if (mEvent->button()==Qt::RightButton)
		{
			CMainWindow::rightMouseDown=true;
			App::mainWindow->_savedMouseMode=App::ct->getMouseMode();
//			int lowerMouseMode=App::ct->getMouseMode()&0x00ff;
			int upperMouseMode=((App::ct->getMouseMode()&0xff00)|sim_navigation_clickselection)-sim_navigation_clickselection; // sim_navigation_clickselection because otherwise we have a problem (12/06/2011)
			if (App::ct->getMouseMode()&sim_navigation_camerarotaterightbutton)
				App::ct->setMouseMode(upperMouseMode|sim_navigation_camerarotate); // default
			else
				App::ct->setMouseMode(upperMouseMode|sim_navigation_passive);
			App::mainWindow->onRightMouseButtonDownTT(x,y);
			App::mainWindow->_mouseButtonsState|=4;
		}
		if (mEvent->button()==Qt::MidButton)
		{ // similar thing further down (in mouse double-click)
			CMainWindow::middleMouseDown=true;

			bool noSelector=true;
			IF_UI_EVENT_CAN_READ_DATA
			{
				if (App::ct->oglSurface->isSceneSelectionActive()||App::ct->oglSurface->isPageSelectionActive()||App::ct->oglSurface->isViewSelectionActive())
					noSelector=false;
			}
			if (noSelector)
			{
				if ((App::ct->getMouseMode()&0x00ff)==sim_navigation_camerashift)
					App::ct->setMouseMode((App::ct->getMouseMode()&0xff00)|sim_navigation_objectshift);
				else
				{
					if ((App::ct->getMouseMode()&0x00ff)==sim_navigation_objectshift)
						App::ct->setMouseMode((App::ct->getMouseMode()&0xff00)|sim_navigation_objectrotate);
					else
						App::ct->setMouseMode((App::ct->getMouseMode()&0xff00)|sim_navigation_camerashift);
				}
			}
			App::mainWindow->_mouseButtonsState|=8;
		}
	}
	CDlgEx::doTransparencyCounter++;
}

void COpenglWidget::mouseReleaseEvent(QMouseEvent* mEvent)
{
	float sx=windowHandle()->devicePixelRatio();
	float sy=windowHandle()->devicePixelRatio();
	if (App::userSettings->retinaScaling3X>0.01f)
		sx=App::userSettings->retinaScaling3X;
	if (App::userSettings->retinaScaling3Y>0.01f)
		sy=App::userSettings->retinaScaling3Y;
	int x=int(float(mEvent->x())*sx+0.5f);
	int y=int(float(mEvent->y())*sy+0.5f);

	App::mainWindow->ctrlKeyDown=((mEvent->modifiers()&Qt::ControlModifier)!=0);
	App::mainWindow->shiftKeyDown=((mEvent->modifiers()&Qt::ShiftModifier)!=0);

	if (mEvent->button()==Qt::LeftButton)
	{
		CMainWindow::leftMouseDown=false;
		IF_UI_EVENT_CAN_READ_DATA
		{
			App::mainWindow->onLeftMouseButtonUpTT(x,y);
		}
		App::mainWindow->_mouseButtonsState&=0xffff-1;
	}
	if (!(App::isFullScreen()))
	{
		if (mEvent->button()==Qt::RightButton)
		{
			CMainWindow::rightMouseDown=false;
			IF_UI_EVENT_CAN_READ_DATA
			{
				App::mainWindow->onRightMouseButtonUpTT(x,y);
			}
			App::ct->setMouseMode(App::mainWindow->_savedMouseMode);
			App::mainWindow->_mouseButtonsState&=0xffff-4;
		}
		if (mEvent->button()==Qt::MidButton)
		{
			CMainWindow::middleMouseDown=false;
			App::mainWindow->_mouseButtonsState&=0xffff-8;
		}
	}
}

void COpenglWidget::mouseDoubleClickEvent(QMouseEvent* mEvent)
{
	float sx=windowHandle()->devicePixelRatio();
	float sy=windowHandle()->devicePixelRatio();
	if (App::userSettings->retinaScaling3X>0.01f)
		sx=App::userSettings->retinaScaling3X;
	if (App::userSettings->retinaScaling3Y>0.01f)
		sy=App::userSettings->retinaScaling3Y;
	int x=int(float(mEvent->x())*sx+0.5f);
	int y=int(float(mEvent->y())*sy+0.5f);

	App::mainWindow->ctrlKeyDown=((mEvent->modifiers()&Qt::ControlModifier)!=0);
	App::mainWindow->shiftKeyDown=((mEvent->modifiers()&Qt::ShiftModifier)!=0);

	if (!(App::isFullScreen()))
	{
		if (mEvent->button()==Qt::LeftButton)
		{
			IF_UI_EVENT_CAN_READ_DATA
			{
				App::mainWindow->onLeftMouseButtonDoubleClickTT(x,y);
			}
		}
		if (mEvent->button()==Qt::MidButton)
		{ // similar thing further up (in mouse press)
			CMainWindow::middleMouseDown=true;
			bool noSelector=true;
			IF_UI_EVENT_CAN_READ_DATA
			{
				if (App::ct->oglSurface->isSceneSelectionActive()||App::ct->oglSurface->isPageSelectionActive()||App::ct->oglSurface->isViewSelectionActive())
					noSelector=false;
			}
			if (noSelector)
			{
				if ((App::ct->getMouseMode()&0x00ff)==sim_navigation_camerashift)
					App::ct->setMouseMode((App::ct->getMouseMode()&0xff00)|sim_navigation_objectshift);
				else
				{
					if ((App::ct->getMouseMode()&0x00ff)==sim_navigation_objectshift)
						App::ct->setMouseMode((App::ct->getMouseMode()&0xff00)|sim_navigation_objectrotate);
					else
						App::ct->setMouseMode((App::ct->getMouseMode()&0xff00)|sim_navigation_camerashift);
				}
			}
		}
	}
}

void COpenglWidget::wheelEvent(QWheelEvent* wEvent)
{
	float sx=1.0f;
	float sy=1.0f;
//	if (App::userSettings->retinaScaling3X>0.01f)
//		sx=App::userSettings->retinaScaling3X;
//	if (App::userSettings->retinaScaling3Y>0.01f)
//		sy=App::userSettings->retinaScaling3Y;
	int x=int(float(wEvent->x())*sx+0.5f);
	int y=int(float(wEvent->y())*sy+0.5f);

	App::mainWindow->ctrlKeyDown=((wEvent->modifiers()&Qt::ControlModifier)!=0);
	App::mainWindow->shiftKeyDown=((wEvent->modifiers()&Qt::ShiftModifier)!=0);

	if (App::ct->getMouseMode()&sim_navigation_camerazoomwheel)
	{
		IF_UI_EVENT_CAN_READ_DATA
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->mouseWheel(wEvent->delta(),x,App::mainWindow->clientArea.y-y);
		}
	}
	App::mainWindow->_mouseButtonsState|=2;
	App::mainWindow->_mouseWheelEventTime=VDateTime::getTimeInMs();
}

void COpenglWidget::keyPressEvent(QKeyEvent* kEvent)
{
	int key=kEvent->key();
	bool processed=false;
	if (key==Qt::Key_Control)
	{
		App::mainWindow->ctrlKeyDown=true;
		processed=true;
	}
	if (key==Qt::Key_Shift)
	{
		App::mainWindow->shiftKeyDown=true;
		processed=true;
	}
	if (key==Qt::Key_Delete)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("Qt::Key_Delete")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(DELETE_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (key==Qt::Key_Left)
	{
		App::mainWindow->leftKeyDown=true;
		IF_UI_EVENT_CAN_READ_DATA_CMD("Qt::Key_Left")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(LEFT_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (key==Qt::Key_Right)
	{
		App::mainWindow->rightKeyDown=true;
		IF_UI_EVENT_CAN_READ_DATA_CMD("Qt::Key_Right")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(RIGHT_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (key==Qt::Key_Up)
	{
		App::mainWindow->upKeyDown=true;
		IF_UI_EVENT_CAN_READ_DATA_CMD("Qt::Key_Up")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(UP_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (key==Qt::Key_Down)
	{
		App::mainWindow->downKeyDown=true;
		IF_UI_EVENT_CAN_READ_DATA_CMD("Qt::Key_Down")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(DOWN_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (key==Qt::Key_Escape)
	{
		if (!_openGlDisplayEnabled)
			_openGlDisplayEnabled=true;	// Esc enables the display again
		IF_UI_EVENT_CAN_READ_DATA_CMD("Qt::Key_Escape")
		{
			App::ct->oglSurface->setFocusObject(FOCUS_ON_PAGE);
			App::mainWindow->focusObject=App::ct->oglSurface->getFocusObject();
			App::ct->oglSurface->keyPress(ESC_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (key==Qt::Key_Tab)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("Qt::Key_Tab")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(TAB_KEY,App::mainWindow);
		}
		processed=true;
	}
	if ((key==Qt::Key_Enter)||(key==Qt::Key_Return))
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("Qt::Key_Enter")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(ENTER_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (key==Qt::Key_Backspace)
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("Qt::Key_Backspace")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(BACKSPACE_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (App::mainWindow->ctrlKeyDown)
	{ // Very specific to V-REP, except for ctrl+Q (which doesn't exist by default on Windows)
		if (key==81)
		{
			IF_UI_EVENT_CAN_READ_DATA_CMD("key==81")
			{
				App::ct->oglSurface->keyPress(CTRL_Q_KEY,App::mainWindow);
			}
			processed=true;
		}
		if (key==32)
		{
			IF_UI_EVENT_CAN_READ_DATA_CMD("key==32")
			{
				App::ct->oglSurface->keyPress(CTRL_SPACE_KEY,App::mainWindow);
			}
			processed=true;
		}
		if (key==68)
		{
			IF_UI_EVENT_CAN_READ_DATA_CMD("key==68")
			{
				App::ct->oglSurface->keyPress(CTRL_D_KEY,App::mainWindow);
			}
			processed=true;
		}
		if (key==71)
		{
			IF_UI_EVENT_CAN_READ_DATA_CMD("key==71")
			{
				App::ct->oglSurface->keyPress(CTRL_G_KEY,App::mainWindow);
			}
			processed=true;
		}
		if (key==69)
		{
			IF_UI_EVENT_CAN_READ_DATA_CMD("key==69")
			{
				App::ct->oglSurface->keyPress(CTRL_E_KEY,App::mainWindow);
			}
			processed=true;
		}
	}

	if (kEvent->matches(QKeySequence::Cut))
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("QKeySequence::Cut")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(CTRL_X_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (kEvent->matches(QKeySequence::Copy))
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("QKeySequence::Copy")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(CTRL_C_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (kEvent->matches(QKeySequence::Paste))
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("QKeySequence::Paste")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(CTRL_V_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (kEvent->matches(QKeySequence::Undo))
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("QKeySequence::Undo")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(CTRL_Z_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (kEvent->matches(QKeySequence::Redo))
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("QKeySequence::Redo")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(CTRL_Y_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (kEvent->matches(QKeySequence::Save))
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("QKeySequence::Save")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(CTRL_S_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (kEvent->matches(QKeySequence::SelectAll))
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("QKeySequence::SelectAll")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(CTRL_A_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (kEvent->matches(QKeySequence::Open))
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("QKeySequence::Open")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(CTRL_O_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (kEvent->matches(QKeySequence::Close))
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("QKeySequence::Close")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(CTRL_W_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (kEvent->matches(QKeySequence::New))
	{
		IF_UI_EVENT_CAN_READ_DATA_CMD("QKeySequence::New")
		{
			if (_openGlDisplayEnabled)
				App::ct->oglSurface->keyPress(CTRL_N_KEY,App::mainWindow);
		}
		processed=true;
	}
	if (!processed)
	{
		QByteArray ba(kEvent->text().toLatin1());
		if (ba.length()>=1)
		{
			IF_UI_EVENT_CAN_READ_DATA_CMD("otherKey")
			{
				if (_openGlDisplayEnabled)
					App::ct->oglSurface->keyPress(int(ba.at(0)),App::mainWindow);
			}
		}
	}
}

void COpenglWidget::keyReleaseEvent(QKeyEvent* kEvent)
{
	int key=kEvent->key();
	if (key==Qt::Key_Control)
		App::mainWindow->ctrlKeyDown=false;
	if (key==Qt::Key_Shift)
		App::mainWindow->shiftKeyDown=false;
	if (key==Qt::Key_Left)
		App::mainWindow->leftKeyDown=false;
	if (key==Qt::Key_Right)
		App::mainWindow->rightKeyDown=false;
	if (key==Qt::Key_Up)
		App::mainWindow->upKeyDown=false;
	if (key==Qt::Key_Down)
		App::mainWindow->downKeyDown=false;
}
