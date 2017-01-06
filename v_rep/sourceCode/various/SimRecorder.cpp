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
#include "SimRecorder.h"
#include "OGL.h"
#include "Tt.h"

#include "IloIlo.h"
#include "imgLoader.h"
#include "auxLibVideo.h"
#include "v_repStrings.h"
#include "VDateTime.h"
#include "App.h"

CSimRecorder::CSimRecorder(const char* initialPathAndFilename)
{
	_isRecording=false;
	_recorderEnabled=false;
	_showCursor=false;
	_showButtonStates=false;
	_hideInfoTextAndStatusBar=true;
	_frameCount=0;
	_frameRate=20; // was 30 until 18/1/2013. With 20, people get less confused about how to record a real-time video ;)
	_autoFrameRate=true;
	_simulationFrameCount=0;
	_recordEveryXRenderedFrame=1;
	_aviGenInitialized=false;
	_buffer=NULL;
	_tempBuffer=NULL;
	_resolution[0]=640;
	_resolution[1]=480;
	_position[0]=0;
	_position[1]=0;
	_showSavedMessage=true;
	_filenameAndPath=initialPathAndFilename;
	_startedInInstance=-1; // Means manually started (not through the simulation start button)
	_encoderIndex=0;
	_simulationTimeOfLastFrame=-1.0f;
	_vrepMessageBitmap=NULL;
}

CSimRecorder::~CSimRecorder()
{
	stopRecording(true);
	stopRecording(false);
	delete[] _vrepMessageBitmap;
}

void CSimRecorder::setShowSavedMessage(bool s)
{
	_showSavedMessage=s;
}

void CSimRecorder::setShowCursor(bool show)
{
	_showCursor=show;
}

bool CSimRecorder::getShowCursor()
{
	return(_showCursor);
}

void CSimRecorder::setAutoFrameRate(bool a)
{
	_autoFrameRate=a;
}

bool CSimRecorder::getAutoFrameRate()
{
	return(_autoFrameRate);
}

void CSimRecorder::setShowButtonStates(bool show)
{
	_showButtonStates=show;
}

bool CSimRecorder::getShowButtonStates()
{
	return(_showButtonStates);
}

bool CSimRecorder::getHideInfoTextAndStatusBar()
{
	return(_hideInfoTextAndStatusBar);
}

void CSimRecorder::setHideInfoTextAndStatusBar(bool hide)
{
	_hideInfoTextAndStatusBar=hide;
}

void CSimRecorder::setEncoderIndex(int index)
{
	_encoderIndex=index;
}

int CSimRecorder::getEncoderIndex()
{
	return(_encoderIndex);
}

void CSimRecorder::setRecordingSizeChanged(int newXsize,int newYsize)
{
	_resolutionInfo[0]=newXsize;
	_resolutionInfo[1]=newYsize;
}

void CSimRecorder::getRecordingSize(int& x, int&y)
{
	x=_resolutionInfo[0];
	y=_resolutionInfo[1];
}

std::string CSimRecorder::getEncoderString(int index)
{
	std::string retStr;
	char txt[201];
	if ('\0'==CAuxLibVideo::video_recorderGetEncoderString(index,txt))
		retStr=txt;
	return(retStr);
}

bool CSimRecorder::recordFrameIfNeeded(int resX,int resY,int posX,int posY)
{ // return value false indicates a failed initialization
	static bool inside=false; // this function is not re-entrant!
	if (inside)
		return(true);
	inside=true;
	if (_isRecording)
	{
		bool validFrame=true;
		if (!getManualStart())
		{
			float simTime=App::ct->simulation->getSimulationTime();
			if (_simulationTimeOfLastFrame!=simTime)
				_simulationTimeOfLastFrame=simTime;
			else
				validFrame=false;
		}

		if (validFrame)
		{
			_simulationFrameCount++;
			if (_simulationFrameCount>=_recordEveryXRenderedFrame)
			{ // we record this frame
				if ( (!_aviGenInitialized)&&(!_initFailed) )
				{ // Initialize the recorder
					_resolution[0]=resX;//&0x0ffc; // must be multiple of 4!
					_resolution[1]=resY;//&0x0ffc; // must be multiple of 4!
					_position[0]=0;//(resX&0x0003)/2;
					_position[1]=0;//(resY&0x0003)/2;

					char txt[201];
					CAuxLibVideo::video_recorderGetEncoderString(_encoderIndex,txt);
					_filenameAndPathAndExtension=_filenameAndPath;
					if (strncmp(txt,"AVI/",4)==0)
						_filenameAndPathAndExtension+=".avi";
					else
					{
						if (strncmp(txt,"MP4/",4)==0)
							_filenameAndPathAndExtension+=".mp4";
						else
							_filenameAndPathAndExtension+=".bin";
					}
					char res=CAuxLibVideo::video_recorderInitialize(_resolution[0],_resolution[1],_filenameAndPathAndExtension.c_str(),getFrameRate(),_encoderIndex);
					_aviGenInitialized=(res!='e');
					if (_aviGenInitialized)
					{
						App::uiThread->addStatusLine(IDSNS_VIDEO_COMPRESSOR_INITIALIZED);
						if (res=='w')
							App::uiThread->addStatusLine(IDSNS_VIDEO_USING_PADDING);
					}
					else
						App::uiThread->addStatusLine(IDSNS_VIDEO_COMPRESSOR_FAILED_TO_INITIALIZE);
					_initFailed=(!_aviGenInitialized);
					if (!_initFailed)
					{
						_buffer=new BYTE[_resolution[0]*_resolution[1]*3];
						_tempBuffer=new BYTE[_resolution[0]*_resolution[1]*3];
					}
				}

				if (_aviGenInitialized)
				{ // Ok to record:
					glReadPixels(_position[0]+posX,_position[1]+posY,_resolution[0],_resolution[1],GL_RGB,GL_UNSIGNED_BYTE,_tempBuffer);
//					int r=_resolution[0]*_resolution[1];
					for (int j=0;j<_resolution[1];j++)
					{
						int yp=j*_resolution[0];
						int yq=(_resolution[1]-j-1)*_resolution[0];
						for (int i=0;i<_resolution[0];i++)
						{
							_buffer[3*(yp+i)+0]=_tempBuffer[3*(yq+i)+0];
							_buffer[3*(yp+i)+1]=_tempBuffer[3*(yq+i)+1];
							_buffer[3*(yp+i)+2]=_tempBuffer[3*(yq+i)+2];
						}
					}

					_blendVrepMessageIn();
					CAuxLibVideo::video_recorderAddFrame(_buffer);
					_simulationFrameCount=0;
					_frameCount++;
				}
			}
		}
	}
	inside=false;
	return((!_initFailed)||(!_isRecording));
}

bool CSimRecorder::getIsRecording()
{
	return(_isRecording);
}

bool CSimRecorder::startRecording(bool manualStart)
{
	if (_recorderEnabled)
	{
		_simulationTimeOfLastFrame=-1.0f;
		if (!manualStart)
			_startedInInstance=App::ct->getCurrentInstanceIndex();
		else
			_startedInInstance=-1;
		_isRecording=true;
		_frameCount=0;
		_simulationFrameCount=0;
		_initFailed=false;
		_recorderEnabled=false;
		return(true);
	}
	return(false);
}

bool CSimRecorder::getManualStart()
{
	return(_startedInInstance==-1);
}

void CSimRecorder::stopRecording(bool manualStop)
{
	bool doIt=false;
	if (!manualStop)
	{
		if (_startedInInstance==App::ct->getCurrentInstanceIndex())
			doIt=true;
	}
	else
	{
		if (_startedInInstance==-1)
			doIt=true;
	}
	if (doIt)
	{
		
		if (_aviGenInitialized)
		{ // deinitialize the recorder:
			CAuxLibVideo::video_recorderEnd();
			_aviGenInitialized=false;
			delete[] _tempBuffer;
			delete[] _buffer;
			_buffer=NULL;
			std::string tmp(IDS_AVI_FILE_WAS_SAVED);
			tmp+=_filenameAndPathAndExtension+")";


			App::uiThread->addStatusLine(tmp.c_str());
			if (_showSavedMessage)
				App::uiThread->messageBox_information(App::mainWindow,strTranslate(IDSN_AVI_RECORDER),tmp,VMessageBox::OKELI);
			_showSavedMessage=true; // reset this flag
			App::ct->objCont->setFullDialogRefreshFlag();
		}
		_isRecording=false;
		_frameCount=0;
		_simulationFrameCount=0;
		_startedInInstance=-1;
	}
}

int CSimRecorder::getFrameCount()
{
	return(_frameCount);
}

void CSimRecorder::setRecordEveryXRenderedFrame(int x)
{
	if (x<1)
		x=1;
	if (x>10000)
		x=10000;
	_recordEveryXRenderedFrame=x;
	_simulationFrameCount=0;
}

int CSimRecorder::getRecordEveryXRenderedFrame()
{
	return(_recordEveryXRenderedFrame);
}

void CSimRecorder::setFrameRate(int fps)
{
	if (fps>30)
		fps=30;
	if (fps<1)
		fps=1;
	_frameRate=fps;
}

int CSimRecorder::getFrameRate()
{
	if (_autoFrameRate)
	{
		int frate=int((1.0f/App::ct->simulation->getSimulationTimeStep_raw())+0.5f);
		return(tt::getLimitedInt(1,120,frate)); // the recorder probably doesn't support that high (120)
	}
	return(_frameRate);
}

void CSimRecorder::setFilenameAndPath(const char* filenameAndPath)
{
	_filenameAndPath=std::string(filenameAndPath);
}

std::string CSimRecorder::getFilenameAndPath()
{
	return(_filenameAndPath);
}

void CSimRecorder::setRecorderEnabled(bool e)
{
	_recorderEnabled=e;
}

bool CSimRecorder::getRecorderEnabled()
{
	return(_recorderEnabled);
}

void CSimRecorder::_blendVrepMessageIn()
{
	if (_vrepMessageBitmap!=NULL)
	{
		int off=(_resolution[0]-_vrepMsgRes[0])/2;
		for (int i=0;i<_vrepMsgRes[1];i++)
		{
			if (i<_resolution[1])
			{
				for (int j=0;j<_vrepMsgRes[0];j++)
				{
					if ((j+off>=0)&&(j+off<_resolution[0]))
					{
						float t=float(_vrepMessageBitmap[4*(i*_vrepMsgRes[0]+j)+3])/255.0f;
						_buffer[3*((_resolution[1]-i-1)*_resolution[0]+j+off)+0]=int(float(_buffer[3*((_resolution[1]-i-1)*_resolution[0]+j+off)+0])*(1.0f-t)+float(_vrepMessageBitmap[4*(i*_vrepMsgRes[0]+j)+0])*t);
						_buffer[3*((_resolution[1]-i-1)*_resolution[0]+j+off)+1]=int(float(_buffer[3*((_resolution[1]-i-1)*_resolution[0]+j+off)+1])*(1.0f-t)+float(_vrepMessageBitmap[4*(i*_vrepMsgRes[0]+j)+0])*t);
						_buffer[3*((_resolution[1]-i-1)*_resolution[0]+j+off)+2]=int(float(_buffer[3*((_resolution[1]-i-1)*_resolution[0]+j+off)+2])*(1.0f-t)+float(_vrepMessageBitmap[4*(i*_vrepMsgRes[0]+j)+0])*t);
					}
				}
			}
		}
	}
}




