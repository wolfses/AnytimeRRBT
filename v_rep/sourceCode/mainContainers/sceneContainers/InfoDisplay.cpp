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
#include "InfoDisplay.h"
#include "App.h"
#include "GV.h"
#include "Tt.h"
#include "IloIlo.h"
#include "ThreadPool.h"
#include <boost/lexical_cast.hpp>
#include "v_repStrings.h"
#include "VDateTime.h"
#include "sensingThreadPool.h"
#include "collDistInterface.h"
#include "dynInterface.h"

CInfoDisplay::CInfoDisplay()
{
	newSceneProcedure();
}

CInfoDisplay::~CInfoDisplay()
{
}

void CInfoDisplay::newSceneProcedure()
{
	resetInfo();
	_threadedScriptCount=0;

	scriptTxt[0]="";
	scriptTxt[1]="";
	collTxt[0]="";
	collTxt[1]="";
	distTxt[0]="";
	distTxt[1]="";
	sensTxt[0]="";
	sensTxt[1]="";
	workThreadsTxt[0]="";
	workThreadsTxt[1]="";
	rendSensTxt[0]="";
	rendSensTxt[1]="";
	ikTxt[0]="";
	ikTxt[1]="";
	gcsTxt[0]="";
	gcsTxt[1]="";
	millTxt[0]="";
	millTxt[1]="";
	dynamicsTxt[0]="";
	dynamicsTxt[1]="";
}

void CInfoDisplay::simulationAboutToStart()
{

}

void CInfoDisplay::simulationEnded()
{

}

void CInfoDisplay::renderYour3DStuff(CViewableBase* renderingObject,int displayAttrib)
{

}

void CInfoDisplay::printInformation()
{
	if (App::ct->buttonBlockContainer==NULL)
		return;
	if (App::ct->objCont==NULL)
		return;
	for (int i=0;i<INFO_BOX_ROW_COUNT;i++)
	{
		if (App::ct->buttonBlockContainer->getInfoBoxButton(i,0)==NULL)
			return;
		if (App::ct->buttonBlockContainer->getInfoBoxButton(i,1)==NULL)
			return;
		App::ct->buttonBlockContainer->getInfoBoxButton(i,0)->label="";
		App::ct->buttonBlockContainer->getInfoBoxButton(i,1)->label="";
	}
	if (App::ct->buttonBlockContainer!=NULL)
	{
		int pos=0;
		std::string tmp;
		std::string txt;
		if ((App::ct->objCont->getEditModeType()&SHAPE_OR_PATH_EDIT_MODE)==0)
		{
			C3DObject* it=App::ct->objCont->getLastSelection();
			if (it!=NULL)
			{

				App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label="Selected objects:";
				if (App::ct->objCont->getSelSize()!=2)
					tmp=boost::lexical_cast<std::string>(App::ct->objCont->getSelSize());
				else
				{
					C3DObject* it2=App::ct->objCont->getObject(App::ct->objCont->getSelID(0));
					C7Vector v0(it->getCumulativeTransformation());
					C7Vector v1(it2->getCumulativeTransformation());
					tmp="2 (frame-frame distance="+gv::getSizeStr(false,(v0.X-v1.X).getLength(),0)+")";
				}
				App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=tmp;

				App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label="Last selected object name:";
				tmp=it->getName();
				App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=tmp;
				
				App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label="Last selected object type:";
				tmp=it->getObjectTypeInfoExtended();
				App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=tmp;
				
				C7Vector m(it->getCumulativeTransformationPart1());
				C3Vector euler(m.Q.getEulerAngles());
				App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label="Last selected object position:";
				txt="x: "+gv::getSizeStr(true,m.X(0))+"    y: "+gv::getSizeStr(true,m.X(1))+"    z: "+gv::getSizeStr(true,m.X(2));
				App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=txt;
				App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label="Last selected object orientation:";
				txt="a: "+gv::getAngleStr(true,euler(0))+"    b: "+gv::getAngleStr(true,euler(1))+"    g: "+gv::getAngleStr(true,euler(2));
				App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=txt;
			}
			else
			{
				App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label="Selected objects:";
				App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label="0";
			}

			int selSize=App::ct->collections->selectedGroups.size();

			if (selSize==1)
			{
				CRegGroup* aGroup=App::ct->collections->getGroup(App::ct->collections->selectedGroups[0]);
				if (aGroup!=NULL)
				{
					App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label="Selected collection:";
					tmp=aGroup->getGroupName()+" (containing ";
					tmp+=boost::lexical_cast<std::string>(aGroup->groupObjects.size())+" objects)";
					App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=tmp;
				}
			}
		}
		if (App::ct->objCont->getEditModeType()&VERTEX_EDIT_MODE)
		{
			if (App::ct->objCont->editModeBuffer.size()!=2)
			{
				App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label="Selected vertices:";
				tmp=boost::lexical_cast<std::string>(App::ct->objCont->editModeBuffer.size());
				App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=tmp;
			}
			else
			{
				C3Vector p1(App::ct->objCont->getRelativeVertexPosition(App::ct->objCont->editModeBuffer[0]));
				C3Vector p2(App::ct->objCont->getRelativeVertexPosition(App::ct->objCont->editModeBuffer[1]));
				float dist=(p2-p1).getLength();
				App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label="Selected vertices:";
				txt="2 (distance="+gv::getSizeStr(false,dist,0)+")";
				App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=txt;
			}
			if (App::ct->objCont->editModeBuffer.size()!=0)
			{
				C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->getEditModeObjectID());
				if (it!=NULL)
				{
					C7Vector m(it->getCumulativeTransformationPart1());
					C3Vector v;
					v(0)=App::ct->objCont->_editionVertices[3*App::ct->objCont->editModeBuffer[App::ct->objCont->editModeBuffer.size()-1]+0];
					v(1)=App::ct->objCont->_editionVertices[3*App::ct->objCont->editModeBuffer[App::ct->objCont->editModeBuffer.size()-1]+1];
					v(2)=App::ct->objCont->_editionVertices[3*App::ct->objCont->editModeBuffer[App::ct->objCont->editModeBuffer.size()-1]+2];
					v*=m;
					App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label="Last selected vertex position:";
					txt="x: "+gv::getSizeStr(true,v(0))+"    y: "+gv::getSizeStr(true,v(1))+"    z: "+gv::getSizeStr(true,v(2));
					App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=txt;
				}
			}
		}
		if (App::ct->objCont->getEditModeType()&TRIANGLE_EDIT_MODE)
		{
			App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label="Selected triangles:";
			tmp=boost::lexical_cast<std::string>(App::ct->objCont->editModeBuffer.size());
			App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=tmp;
		}
		if (App::ct->objCont->getEditModeType()&EDGE_EDIT_MODE)
		{
			if (App::ct->objCont->editModeBuffer.size()==0)
			{
				App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label="Selected edges:";
				tmp="0";
				App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=tmp;
			}
			else
			{
				float totLength=0.0f;
				for (int i=0;i<int(App::ct->objCont->editModeBuffer.size());i++)
				{
					int ind[2]={App::ct->objCont->_edgeCont.allEdges[2*App::ct->objCont->editModeBuffer[i]+0],
								App::ct->objCont->_edgeCont.allEdges[2*App::ct->objCont->editModeBuffer[i]+1]};
					C3Vector p1(App::ct->objCont->getRelativeVertexPosition(ind[0]));
					C3Vector p2(App::ct->objCont->getRelativeVertexPosition(ind[1]));
					float dist=(p2-p1).getLength();
					totLength+=dist;
				}
				App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label="Selected edges:";
				txt=boost::lexical_cast<std::string>(App::ct->objCont->editModeBuffer.size())+" (total edge length="+gv::getSizeStr(false,totLength,0)+")";
				App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=txt;
			}
		}



		if (App::ct->objCont->getEditModeType()&PATH_EDIT_MODE)
		{
			CPathCont* pc=App::ct->objCont->_editionPath;
			CPath* path=(CPath*)App::ct->objCont->getObject(App::ct->objCont->getEditModeObjectID());
			if ( (pc!=NULL)&&(path!=NULL) )
			{
				if (App::ct->objCont->editModeBuffer.size()!=2)
				{
					App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label="Selected path points:";
					txt=boost::lexical_cast<std::string>(App::ct->objCont->editModeBuffer.size());
					txt+=" (Bezier path length="+gv::getSizeStr(false,pc->getBezierNormalPathLength(),0)+")";
					App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=txt;
				}
				else
				{
					CSimplePathPoint* pt1(pc->getSimplePathPoint(App::ct->objCont->editModeBuffer[0]));
					CSimplePathPoint* pt2(pc->getSimplePathPoint(App::ct->objCont->editModeBuffer[1]));
					float dist=(pt2->getTransformation().X-pt1->getTransformation().X).getLength();
					App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label="Selected path points:";
					txt="2 (distance="+gv::getSizeStr(false,dist,0)+")";
					App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=txt;
				}
				if (App::ct->objCont->editModeBuffer.size()!=0)
				{
					CSimplePathPoint* pt(pc->getSimplePathPoint(App::ct->objCont->editModeBuffer[App::ct->objCont->editModeBuffer.size()-1]));
					C7Vector tr(path->getCumulativeTransformationPart1());
					C3Vector v(tr*pt->getTransformation().X);
					App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label="Last selected path point position:";
					txt="x: "+gv::getSizeStr(true,v(0))+"    y: "+gv::getSizeStr(true,v(1))+"    z: "+gv::getSizeStr(true,v(2));
					App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=txt;
					C3Vector euler((tr.Q*pt->getTransformation().Q).getEulerAngles());
					App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label="Last selected path point orientation:";
					txt="a: "+gv::getAngleStr(true,euler(0))+"    b: "+gv::getAngleStr(true,euler(1))+"    g: "+gv::getAngleStr(true,euler(2));
					App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=txt;
				}
			}
		}




		std::string txtLeft,txtRight;
		int index=0;
		while (App::ct->simulation->getInfo(txtLeft,txtRight,index))
		{
			App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label=txtLeft;
			App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=txtRight;
		}
		if (App::ct->simulation->isSimulationRunning())
		{
			// Script functionality:
			App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label=scriptTxt[0];
			App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=scriptTxt[1];
			// Collision detection:
			App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label=collTxt[0];
			App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=collTxt[1];
			// Distance calculation:
			App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label=distTxt[0];
			App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=distTxt[1];
			// Proximity sensor simulation:
			App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label=sensTxt[0];
			App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=sensTxt[1];
			// Work threads:
			if (CSensingThreadPool::getThreadCount()>0)
			{
				App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label=workThreadsTxt[0];
				App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=workThreadsTxt[1];
			}
			// Vision sensor simulation:
			App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label=rendSensTxt[0];
			App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=rendSensTxt[1];
			// IK calculation:
			App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label=ikTxt[0];
			App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=ikTxt[1];
			// Geometric constraint solver calculation:
			App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label=gcsTxt[0];
			App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=gcsTxt[1];
			// Dynamics calculation:
			App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label=dynamicsTxt[0];
			App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=dynamicsTxt[1];
			// Milling calculation:
			App::ct->buttonBlockContainer->getInfoBoxButton(pos,0)->label=millTxt[0];
			App::ct->buttonBlockContainer->getInfoBoxButton(pos++,1)->label=millTxt[1];
		}	
	}
}

void CInfoDisplay::prepareInfo()
{
	// Script functionality:
	if (_regularScriptCount==0)
		scriptTxt[0]="&&fg930Main script not called";
	else
		scriptTxt[0]="Script(s) executed";
	scriptTxt[1]="Non-threaded: ";
	scriptTxt[1]+=boost::lexical_cast<std::string>(_regularScriptCount)+" (";
	scriptTxt[1]+=boost::lexical_cast<std::string>(_mainScriptDuration)+" ms), running threads: ";
	scriptTxt[1]+=boost::lexical_cast<std::string>(_threadedScriptCount);
	scriptTxt[1]+=" (";
	scriptTxt[1]+=boost::lexical_cast<std::string>(_threadedScriptDuration)+" ms) ";
	scriptTxt[1]+=_mainScriptMessage;

	// Collision detection:
	if (CCollDistInterface::initializeFunctionsIfNeeded())
	{
		if (!App::ct->mainSettings->collisionDetectionEnabled)
			collTxt[0]="&&fg930Collision handling disabled";
		else
			collTxt[0]="Collision handling enabled";
	}
	else
		collTxt[0]="&&fg930'MeshCalc' plugin not found";
	collTxt[1]="Calculations: ";
	collTxt[1]+=boost::lexical_cast<std::string>(_collCalcCount)+", detections: ";
	collTxt[1]+=boost::lexical_cast<std::string>(_collDetectCount)+" (";
	collTxt[1]+=boost::lexical_cast<std::string>(_collCalcDuration)+" ms)";
	
	// Distance calculation:
	if (CCollDistInterface::initializeFunctionsIfNeeded())
	{
		if (!App::ct->mainSettings->distanceCalculationEnabled)
			distTxt[0]="&&fg930Distance handling disabled";
		else
			distTxt[0]="Distance handling enabled";
	}
	else
		distTxt[0]="&&fg930'MeshCalc' plugin not found";
	distTxt[1]="Calculations: ";
	distTxt[1]+=boost::lexical_cast<std::string>(_distCalcCount)+" (";
	distTxt[1]+=boost::lexical_cast<std::string>(_distCalcDuration)+" ms)";


	// Proximity sensor calculation:
	if (CCollDistInterface::initializeFunctionsIfNeeded())
	{
		if (!App::ct->mainSettings->proximitySensorsEnabled)
			sensTxt[0]="&&fg930Proximity sensor handling disabled";
		else
			sensTxt[0]="Proximity sensor handling enabled";
	}
	else
		sensTxt[0]="&&fg930'MeshCalc' plugin not found";
	sensTxt[1]="Calculations: ";
	sensTxt[1]+=boost::lexical_cast<std::string>(_sensCalcCount)+", detections: ";
	sensTxt[1]+=boost::lexical_cast<std::string>(_sensDetectCount)+" (";
	sensTxt[1]+=boost::lexical_cast<std::string>(_sensCalcDuration)+" ms)";

	// Work threads:
	workThreadsTxt[0]="Work threads enabled";
	workThreadsTxt[1]=boost::lexical_cast<std::string>(CSensingThreadPool::getThreadCount())+" threads running (";
	workThreadsTxt[1]+=boost::lexical_cast<std::string>(_workThreadExecutionDuration)+" ms)";

	// Vision sensor calculation:
	if (!App::ct->mainSettings->visionSensorsEnabled)
		rendSensTxt[0]="&&fg930Vision sensor handling disabled";
	else
		rendSensTxt[0]="Vision sensor handling enabled (FBO)";
	rendSensTxt[1]="Calculations: ";
	rendSensTxt[1]+=boost::lexical_cast<std::string>(_rendSensCalcCount)+", detections: ";
	rendSensTxt[1]+=boost::lexical_cast<std::string>(_rendSensDetectCount)+" (";
	rendSensTxt[1]+=boost::lexical_cast<std::string>(_rendSensCalcDuration)+" ms)";

	// IK calculation:
	if (!App::ct->mainSettings->ikCalculationEnabled)
		ikTxt[0]="&&fg930IK group handling disabled";
	else
		ikTxt[0]="IK group handling enabled";
	ikTxt[1]="Calculations: ";
	ikTxt[1]+=boost::lexical_cast<std::string>(_ikCalcCount)+" (";
	ikTxt[1]+=boost::lexical_cast<std::string>(_ikCalcDuration)+" ms)";

	// GCS calculation:
	if (!App::ct->mainSettings->gcsCalculationEnabled)
		gcsTxt[0]="&&fg930Mechanism handling disabled";
	else
		gcsTxt[0]="Mechanism handling enabled";
	gcsTxt[1]="Calculations: ";
	gcsTxt[1]+=boost::lexical_cast<std::string>(_gcsCalcCount)+" (";
	gcsTxt[1]+=boost::lexical_cast<std::string>(_gcsCalcDuration)+" ms)";

	// Dynamics calculation:
	if (CDynInterface::initializeFunctionsIfNeeded())
	{
		if (!App::ct->dynamicsContainer->getDynamicsEnabled())
			dynamicsTxt[0]="&&fg930Dynamics handling disabled";
		else
		{
			dynamicsTxt[0]="Dynamics handling enabled (";
			if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_ode)
				dynamicsTxt[0]+=IDS_ODE;
			if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_bullet)
				dynamicsTxt[0]+=IDS_BULLET;
			if (App::ct->dynamicsContainer->getDynamicEngineType()==sim_physics_vortex)
				dynamicsTxt[0]+=IDS_VORTEX;
			dynamicsTxt[0]+=")";
		}
	}
	else
		dynamicsTxt[0]="&&fg930'Dynamics' plugin not found";
	dynamicsTxt[1]="Calculation passes: ";
	if (_dynamicsContentAvailable)
	{
		dynamicsTxt[1]+=boost::lexical_cast<std::string>(_dynamicsCalcPasses)+" (";
		dynamicsTxt[1]+=boost::lexical_cast<std::string>(_dynamicsCalcDuration)+" ms)";
	}
	else
		dynamicsTxt[1]+="0 (no dynamic content)";

	// Milling calculation:
	if (CCollDistInterface::initializeFunctionsIfNeeded())
	{
		if (!App::ct->mainSettings->millsEnabled)
			millTxt[0]="&&fg930Mill handling disabled";
		else
			millTxt[0]="Mill handling enabled";
	}
	else
		millTxt[0]="&&fg930'MeshCalc' plugin not found";
	millTxt[1]="Calculations: ";
	millTxt[1]+=boost::lexical_cast<std::string>(_millCalcCount)+", surface cut: ";
	millTxt[1]+=boost::lexical_cast<std::string>(_millRemovedSurface*1000000.0f)+" mm^2 (";
	millTxt[1]+=boost::lexical_cast<std::string>(_millCalcDuration)+" ms)";
}

void CInfoDisplay::resetInfo()
{
	_collCalcCount=0;
	_collDetectCount=0;
	_collCalcDuration=0;
	_distCalcCount=0;
	_distCalcDuration=0;
	_sensCalcCount=0;
	_sensDetectCount=0;
	_sensCalcDuration=0;
	_rendSensCalcCount=0;
	_rendSensDetectCount=0;
	_rendSensCalcDuration=0;
	_ikCalcCount=0;
	_ikCalcDuration=0;
	_gcsCalcCount=0;
	_gcsCalcDuration=0;
	_regularScriptCount=0;
	_mainScriptMessage="";
	_mainScriptDuration=0;
	_threadedScriptDuration=0;
	_threadedScriptCount=0;

	_simulationPassDuration=0;

	_dynamicsCalcPasses=0;
	_dynamicsCalcDuration=0;

	_workThreadExecutionDuration=0;
	_renderingDuration=0;

	_millCalcCount=0;
	_millRemovedSurface=0.0f;
	_millRemovedVolume=0.0f;
	_millCalcDuration=0;
}

float CInfoDisplay::getCollisionCalculationTime()
{
	return(float(_collCalcDuration)*0.001f);
}

float CInfoDisplay::getDistanceCalculationTime()
{
	return(float(_distCalcDuration)*0.001f);
}

float CInfoDisplay::getProximitySensorCalculationTime()
{
	return(float(_sensCalcDuration)*0.001f);
}

float CInfoDisplay::getMillingCalculationTime()
{
	return(float(_millCalcDuration)*0.001f);
}

float CInfoDisplay::getVisionSensorCalculationTime()
{
	return(float(_rendSensCalcDuration)*0.001f);
}

float CInfoDisplay::getIkCalculationTime()
{
	return(float(_ikCalcDuration)*0.001f);
}

float CInfoDisplay::getGcsCalculationTime()
{
	return(float(_gcsCalcDuration)*0.001f);
}

float CInfoDisplay::getDynamicsCalculationTime()
{
	return(float(_dynamicsCalcDuration)*0.001f);
}

float CInfoDisplay::getMainScriptExecutionTime()
{
	return(float(VDateTime::getTimeDiffInMs(_mainScriptStartTime))*0.001f);
}

float CInfoDisplay::getSimulationPassExecutionTime()
{
	return(float(VDateTime::getTimeDiffInMs(_simulationPassStartTime))*0.001f);
}

float CInfoDisplay::getRenderingDuration()
{
	return(float(_renderingDuration)*0.001f);
}

float CInfoDisplay::getWorkThreadExecutionDuration()
{
	return(float(_workThreadExecutionDuration)*0.001f);
}

int CInfoDisplay::getWorkThreadExecutionDuration_inMs()
{
	return(_workThreadExecutionDuration);
}

void CInfoDisplay::scriptStart(bool threaded,bool mainScript)
{
	if (!threaded)
		_regularScriptCount++;
	if (mainScript)
		_mainScriptStartTime=VDateTime::getTimeInMs();
}

void CInfoDisplay::mainScriptPaused(int pauseTimeInMs)
{
	_mainScriptStartTime+=pauseTimeInMs;
}

void CInfoDisplay::scriptEnd(bool threaded,bool mainScript)
{
	if (mainScript)
		_mainScriptDuration+=VDateTime::getTimeDiffInMs(_mainScriptStartTime);
}

void CInfoDisplay::runningThreadedScriptStart(int threadedScriptCount)
{
	_threadedScriptCount+=threadedScriptCount;
	_threadedScriptStartTime=VDateTime::getTimeInMs();
}

void CInfoDisplay::runningThreadedScriptEnd()
{
	_threadedScriptDuration+=VDateTime::getTimeDiffInMs(_threadedScriptStartTime);
}

void CInfoDisplay::setMainScriptMessage(const char* msg)
{
	_mainScriptMessage=msg;
}

void CInfoDisplay::simulationPassStart()
{
	_simulationPassStartTime=VDateTime::getTimeInMs();
}

void CInfoDisplay::simulationPassEnd()
{
	_simulationPassDuration+=VDateTime::getTimeDiffInMs(_simulationPassStartTime);
}

void CInfoDisplay::collisionDetectionStart()
{
	_collStartTime=VDateTime::getTimeInMs();
}

void CInfoDisplay::collisionDetectionEnd(bool detected)
{
	_collCalcCount++;
	if (detected)
		_collDetectCount++;
	_collCalcDuration+=VDateTime::getTimeDiffInMs(_collStartTime);
}

void CInfoDisplay::distanceCalculationStart()
{
	_distStartTime=VDateTime::getTimeInMs();
}

void CInfoDisplay::distanceCalculationEnd()
{
	_distCalcCount++;
	_distCalcDuration+=VDateTime::getTimeDiffInMs(_distStartTime);
}

void CInfoDisplay::proximitySensorSimulationStart()
{
	_sensStartTime=VDateTime::getTimeInMs();
}

void CInfoDisplay::proximitySensorSimulationEnd(bool detected)
{
	_sensCalcCount++;
	if (detected)
		_sensDetectCount++;
	_sensCalcDuration+=VDateTime::getTimeDiffInMs(_sensStartTime);
}

void CInfoDisplay::visionSensorSimulationStart()
{
	_rendSensStartTime=VDateTime::getTimeInMs();
}

void CInfoDisplay::visionSensorSimulationEnd(bool detected)
{
	_rendSensCalcCount++;
	if (detected)
		_rendSensDetectCount++;
	_rendSensCalcDuration+=VDateTime::getTimeDiffInMs(_rendSensStartTime);
}

void CInfoDisplay::inverseKinematicsStart()
{
	_ikStartTime=VDateTime::getTimeInMs();
}

void CInfoDisplay::inverseKinematicsEnd()
{
	_ikCalcCount++;
	_ikCalcDuration+=VDateTime::getTimeDiffInMs(_ikStartTime);
}

void CInfoDisplay::renderingStart()
{
	_renderingStartTime=VDateTime::getTimeInMs();
}

void CInfoDisplay::renderingEnd()
{
	_renderingDuration+=VDateTime::getTimeDiffInMs(_renderingStartTime);
}

void CInfoDisplay::clearRenderingTime()
{
	_renderingDuration=0;
}

void CInfoDisplay::setWorkThreadExecutionDuration_inMs(int d)
{
	_workThreadExecutionDuration=d;
}

void CInfoDisplay::dynamicsStart()
{
	_dynamicsStartTime=VDateTime::getTimeInMs();
}

void CInfoDisplay::dynamicsEnd(int calcPasses,bool dynamicContent)
{
	_dynamicsCalcPasses=calcPasses;
	_dynamicsCalcDuration+=VDateTime::getTimeDiffInMs(_dynamicsStartTime);
	_dynamicsContentAvailable=dynamicContent;
}

void CInfoDisplay::gcsStart()
{
	_gcsStartTime=VDateTime::getTimeInMs();
}

void CInfoDisplay::gcsEnd()
{
	_gcsCalcCount++;
	_gcsCalcDuration+=VDateTime::getTimeDiffInMs(_gcsStartTime);
}

void CInfoDisplay::millSimulationStart()
{
	_millStartTime=VDateTime::getTimeInMs();
}

void CInfoDisplay::millSimulationEnd(float surfaceRemoved,float volumeRemoved)
{
	_millCalcCount++;
	_millRemovedSurface+=surfaceRemoved;
	_millRemovedVolume+=volumeRemoved;
	_millCalcDuration+=VDateTime::getTimeDiffInMs(_millStartTime);
}
