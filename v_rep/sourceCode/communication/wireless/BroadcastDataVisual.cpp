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
#include "v_rep_internal.h"
#include "BroadcastDataVisual.h"
#include "OGL.h"
#include "App.h"
#include "VDateTime.h"

CBroadcastDataVisual::CBroadcastDataVisual(float timeOutSimulationTime,float actionRadius,const C7Vector& emitterConf,float emissionAngle1,float emissionAngle2)
{ // displayed if not timed out
	_timeOutSimulationTime=timeOutSimulationTime;
	_actionRadius=actionRadius;
	_emitterConf=emitterConf;
	_emissionAngle1=emissionAngle1;
	_emissionAngle2=emissionAngle2;
	_emitter=true;
}

CBroadcastDataVisual::CBroadcastDataVisual(const C3Vector& emitterPos,const C3Vector& receiverPos)
{ // receiving. Displayed exactly once
	_timeOutSimulationTime=0.0f;
	_emitterConf.X=emitterPos;
	_receiverPos=receiverPos;
	_emitter=false;
}

bool CBroadcastDataVisual::doesRequireDestruction(float simulationTime)
{
	if (_emitter)
		return(simulationTime>_timeOutSimulationTime);
	return(_timeOutSimulationTime!=0.0f);
}

bool CBroadcastDataVisual::isReceiver()
{
	return(!_emitter);
}

CBroadcastDataVisual::~CBroadcastDataVisual()
{
}

void CBroadcastDataVisual::visualize()
{
	if (_emitter)
	{
		glPushMatrix();
		glTranslatef(_emitterConf.X(0),_emitterConf.X(1),_emitterConf.X(2));
		C4Vector axis=_emitterConf.Q.getAngleAndAxis();
		glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));
		float distances[11]={0.0f,0.001f*_actionRadius,0.00215f*_actionRadius,0.00462f*_actionRadius,0.01f*_actionRadius,0.0215f*_actionRadius,0.0462f*_actionRadius,0.1f*_actionRadius,0.215f*_actionRadius,0.462f*_actionRadius,_actionRadius};

		App::ct->environment->wirelessEmissionVolumeColor.makeCurrentColor();

		int vertSubdiv=int(_emissionAngle1*24.0f/piValue);
		if (vertSubdiv>12)
			vertSubdiv=12;
		int horizSubdiv=int(_emissionAngle2*48.0f/piValTimes2);
		if (horizSubdiv>24)
			horizSubdiv=24;
		for (int i=0;i<10;i++)
		{
			float d0=distances[i];
			float d1=distances[i+1];
			float b=-_emissionAngle2*0.5f;
			float dB=_emissionAngle2/float(horizSubdiv);
			for (int j=0;j<horizSubdiv;j++)
			{
				float a=-_emissionAngle1*0.5f;
				float dA=_emissionAngle1/float(vertSubdiv);
				glBegin(GL_LINES);
				for (int k=0;k<vertSubdiv;k++)
				{
					glVertex3f(d0*cos(a)*cos(b),d0*cos(a)*sin(b),d0*sin(a));
					glVertex3f(d1*cos(a)*cos(b),d1*cos(a)*sin(b),d1*sin(a));
					glVertex3f(d1*cos(a)*cos(b),d1*cos(a)*sin(b),d1*sin(a));
					glVertex3f(d1*cos(a+dA)*cos(b),d1*cos(a+dA)*sin(b),d1*sin(a+dA));
					glVertex3f(d1*cos(a)*cos(b),d1*cos(a)*sin(b),d1*sin(a));
					glVertex3f(d1*cos(a)*cos(b+dB),d1*cos(a)*sin(b+dB),d1*sin(a));
					if ( (j==horizSubdiv-1)&&(_emissionAngle2<piValTimes2*0.99f) )
					{ // Not 360 degrees. We have to close the edges:
						glVertex3f(d0*cos(a)*cos(b+dB),d0*cos(a)*sin(b+dB),d0*sin(a));
						glVertex3f(d1*cos(a)*cos(b+dB),d1*cos(a)*sin(b+dB),d1*sin(a));
						glVertex3f(d1*cos(a)*cos(b+dB),d1*cos(a)*sin(b+dB),d1*sin(a));
						glVertex3f(d1*cos(a+dA)*cos(b+dB),d1*cos(a+dA)*sin(b+dB),d1*sin(a+dA));
					}
					a+=dA;
				}
				if (_emissionAngle1<piValue*0.99f)
				{ // Not 180 degrees. We have to close the edges:
					glVertex3f(d0*cos(a)*cos(b),d0*cos(a)*sin(b),d0*sin(a));
					glVertex3f(d1*cos(a)*cos(b),d1*cos(a)*sin(b),d1*sin(a));
					glVertex3f(d1*cos(a)*cos(b),d1*cos(a)*sin(b),d1*sin(a));
					glVertex3f(d1*cos(a)*cos(b+dB),d1*cos(a)*sin(b+dB),d1*sin(a));
					if ( (j==horizSubdiv-1)&&(_emissionAngle2<piValTimes2*0.99f) )
					{ // Not 360 degrees. We have to close one edge:
						glVertex3f(d0*cos(a)*cos(b+dB),d0*cos(a)*sin(b+dB),d0*sin(a));
						glVertex3f(d1*cos(a)*cos(b+dB),d1*cos(a)*sin(b+dB),d1*sin(a));
					}
				}
				glEnd();
				b+=dB;
			}
		}
		ogl::setBlending(false);
		glPopMatrix();
	}
	else
	{
		ogl::setColorsAllBlack();
		App::ct->environment->wirelessTransmissionColor.makeCurrentColor();
		WORD stipple=255;//SIM_ROTATE_LEFT_16(0x000f,(pcTimeInMs/50)&15));
		glLineStipple(1,stipple);
		glLineWidth(4.0f);
		glEnable(GL_LINE_STIPPLE);
		glBegin(GL_LINES);
		glVertex3f(_emitterConf.X(0),_emitterConf.X(1),_emitterConf.X(2));
		glVertex3f(_receiverPos(0),_receiverPos(1),_receiverPos(2));
		glEnd();
		glDisable(GL_LINE_STIPPLE);
		glLineWidth(1.0f);
		_timeOutSimulationTime=1.0f; // indicates that this object will be destroyed
	}
}
