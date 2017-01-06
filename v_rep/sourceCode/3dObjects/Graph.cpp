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
#include "Graph.h"
#include "OGL.h"
#include "Tt.h"

#include "GV.h"
#include "GraphingRoutines.h"
#include "IloIlo.h"
#include "v_repStrings.h"
#include <boost/lexical_cast.hpp>
#include "VVarious.h"
#include "ttUtil.h"
#include "easyLock.h"
#include "App.h"

CGraph::CGraph()
{
	setObjectType(sim_object_graph_type);
	justDrawCurves=false;
	_explicitHandling=false;
	bufferSize=1000;
	numberOfPoints=0;
	startingPoint=0;
	threeDPartners.reserve(4);
	threeDPartners.clear();
	twoDPartners.reserve(4);
	twoDPartners.clear();
	daten.reserve(16);
	daten.clear();
	times.reserve(bufferSize);
	times.clear();
	for (int i=0;i<bufferSize;i++)
		times.push_back(0.0f);

	_initialValuesInitialized=false;

	_mainPropertyLocal=0; // actually also renderable, but turned off by default!
	cyclic=true;
	xYZPlanesDisplay=true;
	graphGrid=true;
	graphValues=true;
	size=0.1f;
	color.setDefaultValues();
	color.setColor(0.15f,0.15f,0.15f,AMBIENT_MODE);

	backgroundColor[0]=0.1f;
	backgroundColor[1]=0.1f;
	backgroundColor[2]=0.1f;

	textColor[0]=0.8f;
	textColor[1]=0.8f;
	textColor[2]=0.8f;
	layer=GRAPH_LAYER;
	_objectName=IDSOGL_GRAPH;
}

CGraph::~CGraph()
{
	for (int i=0;i<int(threeDPartners.size());i++)
		delete threeDPartners[i];
	threeDPartners.clear();
	for (int i=0;i<int(twoDPartners.size());i++)
		delete twoDPartners[i];
	twoDPartners.clear();
	for (int i=0;i<int(daten.size());i++)
		delete daten[i];
	daten.clear();

	removeAllStatics();
}

std::string CGraph::getObjectTypeInfo()
{
	return(IDSOGL_GRAPH);
}
std::string CGraph::getObjectTypeInfoExtended()
{
	return(IDSOGL_GRAPH);
}

bool CGraph::getFullBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	getMarkingBoundingBox(minV,maxV);
	C7Vector thisInv(getCumulativeTransformation().getInverse());
	for (int i=0;i<int(threeDPartners.size());i++)
	{
		CGraphData* part0=getGraphData(threeDPartners[i]->data[0]);
		CGraphData* part1=getGraphData(threeDPartners[i]->data[1]);
		CGraphData* part2=getGraphData(threeDPartners[i]->data[2]);
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
		while (getAbsIndexOfPosition(pos++,absIndex))
		{
			bool dataIsValid=true;
			if (part0!=NULL)
			{
				if(!getData(part0,absIndex,point[0],cyclic0,range0,true))
					dataIsValid=false;
			}
			else
				dataIsValid=false;
			if (part1!=NULL)
			{
				if(!getData(part1,absIndex,point[1],cyclic1,range1,true))
					dataIsValid=false;
			}
			else
				dataIsValid=false;
			if (part2!=NULL)
			{
				if(!getData(part2,absIndex,point[2],cyclic2,range2,true))
					dataIsValid=false;
			}
			else
				dataIsValid=false;
			if (dataIsValid)
			{
				C3Vector pp(point);
				if (threeDPartners[i]->getCurveRelativeToWorld())
					pp=thisInv*pp;
				minV.keepMin(pp);
				maxV.keepMax(pp);
			}
		}
	}

	// Static 3D curves now:
	for (int i=0;i<int(_staticCurves.size());i++)
	{
		CStaticGraphCurve* it=_staticCurves[i];
		if (it->getCurveType()==2)
		{
			for (int j=0;j<int(it->values.size()/3);j++)
			{
				C3Vector pp(it->values[3*j+0],it->values[3*j+1],it->values[3*j+2]);
				if (it->getRelativeToWorld())
					pp=thisInv*pp;
				minV.keepMin(pp);
				maxV.keepMax(pp);
			}
		}
	}

	return(true);
}

bool CGraph::getMarkingBoundingBox(C3Vector& minV,C3Vector& maxV)
{
	maxV(0)=maxV(1)=maxV(2)=size/2.0f;
	minV=maxV*-1.0f;
	return(true);
}

bool CGraph::getExportableMeshAtIndex(int index,std::vector<float>& vertices,std::vector<int>& indices)
{
	vertices.clear();
	indices.clear();
	return(false); // for now
}

void CGraph::display(CViewableBase* renderingObject,int displayAttrib)
{ // This is a quite ugly routine which requires refactoring!
	FUNCTION_INSIDE_DEBUG("CGraph::display");
	EASYLOCK(_objectMutex);
	// At the beginning of every 3DObject display routine:
	commonStart(displayAttrib);

	// Bounding box display:
	if ( (displayAttrib&sim_displayattribute_renderpass)&&(!justDrawCurves) ) 
		displayBoundingBox(displayAttrib,false);

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
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		App::ct->objCont->enableAuxClippingPlanes(_objectID);
		color.makeCurrentColor();
		if (xYZPlanesDisplay&&(!justDrawCurves)&&((displayAttrib&sim_displayattribute_forvisionsensor)==0))
		{
			int subdiv=5;
			float step=size/(float)subdiv;
			glBegin(GL_LINES);
			for (int i=0;i<subdiv+1;i++)
			{
				glVertex3f(-size/2.0f+i*step,-size/2.0f,0.0f);
				glVertex3f(-size/2.0f+i*step,+size/2.0f,0.0f);
				glVertex3f(-size/2.0f,-size/2.0f+i*step,0.0f);
				glVertex3f(+size/2.0f,-size/2.0f+i*step,0.0f);

				glVertex3f(0.0f,-size/2.0f+i*step,-size/2.0f);
				glVertex3f(0.0f,-size/2.0f+i*step,+size/2.0f);
				glVertex3f(0.0f,-size/2.0f,-size/2.0f+i*step);
				glVertex3f(0.0f,+size/2.0f,-size/2.0f+i*step);

				glVertex3f(-size/2.0f+i*step,0.0f,-size/2.0f);
				glVertex3f(-size/2.0f+i*step,0.0f,+size/2.0f);
				glVertex3f(-size/2.0f,0.0f,-size/2.0f+i*step);
				glVertex3f(+size/2.0f,0.0f,-size/2.0f+i*step);
			}
			glEnd();
		}
		if ( (!justDrawCurves)&&((displayAttrib&sim_displayattribute_forvisionsensor)==0) )
			ogl::displaySphere(size/32.0f);

		// Display the trajectories..
		ogl::setColorsAllBlack();
		C7Vector thisInv(getCumulativeTransformation_forDisplay((displayAttrib&sim_displayattribute_forvisionsensor)==0).getInverse());

		for (int i=0;i<int(threeDPartners.size());i++)
		{
			if (threeDPartners[i]->getVisible())
			{
				if ( (justDrawCurves&&threeDPartners[i]->getVisibleOnTopOfEverything()) ||
					((!justDrawCurves)&&(!threeDPartners[i]->getVisibleOnTopOfEverything())) )
				{
					ogl::setColor(threeDPartners[i]->ambientColor,NULL,NULL,threeDPartners[i]->emissiveColor,12);
					CGraphData* part0=getGraphData(threeDPartners[i]->data[0]);
					CGraphData* part1=getGraphData(threeDPartners[i]->data[1]);
					CGraphData* part2=getGraphData(threeDPartners[i]->data[2]);
					int pos=0;
					int absIndex;
					float point[3];
					bool firstPass=true;
					glLineWidth(threeDPartners[i]->get3DCurveWidth());
					glPointSize(threeDPartners[i]->get3DCurveWidth());
					if (threeDPartners[i]->getVisibleOnTopOfEverything())
					{
						glDepthMask(GL_FALSE);
						glDisable(GL_DEPTH_TEST);
					}
					bool cyclic0,cyclic1,cyclic2;
					float range0,range1,range2;
					if (part0!=NULL)	
						CGraphingRoutines::getCyclicAndRangeValues(part0,cyclic0,range0);
					if (part1!=NULL)	
						CGraphingRoutines::getCyclicAndRangeValues(part1,cyclic1,range1);
					if (part2!=NULL)	
						CGraphingRoutines::getCyclicAndRangeValues(part2,cyclic2,range2);
					while (getAbsIndexOfPosition(pos++,absIndex))
					{
						bool dataIsValid=true;
						if (part0!=NULL)
						{
							if(!getData(part0,absIndex,point[0],cyclic0,range0,false))
								dataIsValid=false;
						}
						else
							dataIsValid=false;
						if (part1!=NULL)
						{
							if(!getData(part1,absIndex,point[1],cyclic1,range1,false))
								dataIsValid=false;
						}
						else
							dataIsValid=false;
						if (part2!=NULL)
						{
							if(!getData(part2,absIndex,point[2],cyclic2,range2,false))
								dataIsValid=false;
						}
						else
							dataIsValid=false;

						if (dataIsValid)
						{
							C3Vector pp(point);
							if (threeDPartners[i]->getCurveRelativeToWorld())
								pp=thisInv*pp;
							if (firstPass&&threeDPartners[i]->getLabel()&&(displayAttrib&sim_displayattribute_renderpass)&&((displayAttrib&sim_displayattribute_forvisionsensor)==0))
								ogl::drawBitmapTextIntoScene(pp(0),pp(1),pp(2),threeDPartners[i]->getName());
							if (firstPass)
							{
								if (threeDPartners[i]->getLinkPoints())
									glBegin(GL_LINE_STRIP);
								else
									glBegin(GL_POINTS);
							}
							glVertex3f(pp(0),pp(1),pp(2));
							firstPass=false;
						}
					}
					glEnd();
					glDepthMask(GL_TRUE);
					glEnable(GL_DEPTH_TEST);
					glLineWidth(1.0f);
					glPointSize(1.0f);
				}
			}
		}

		// Static curves now:
		if ((displayAttrib&sim_displayattribute_forvisionsensor)==0)
		{
			for (int i=0;i<int(_staticCurves.size());i++)
			{
				if (_staticCurves[i]->getCurveType()==2)
				{
					CStaticGraphCurve* it=_staticCurves[i];
					ogl::setColor(it->color,EMISSION_MODE);
					glLineWidth(it->getCurveWidth());
					glPointSize(it->getCurveWidth());
					glLineStipple(1,0xE187);
					glEnable(GL_LINE_STIPPLE);
					for (int j=0;j<int(it->values.size()/3);j++)
					{
						C3Vector pp(it->values[3*j+0],it->values[3*j+1],it->values[3*j+2]);
						if (it->getRelativeToWorld())
							pp=thisInv*pp;
						if ((j==0)&&it->getLabel()&&(displayAttrib&sim_displayattribute_renderpass))
							ogl::drawBitmapTextIntoScene(pp(0),pp(1),pp(2),it->getName()+" [STATIC]");
						if (j==0)
						{
							if (it->getLinkPoints())
								glBegin(GL_LINE_STRIP);
							else
								glBegin(GL_POINTS);
						}
						glVertex3f(pp(0),pp(1),pp(2));
					}
					glEnd();
					glLineWidth(1.0f);
					glPointSize(1.0f);
					glDisable(GL_LINE_STIPPLE);
				}
			}
		}

		if ((displayAttrib&sim_displayattribute_forvisionsensor)==0)
			ogl::drawReference(size/2.0f);
		App::ct->objCont->disableAuxClippingPlanes();
	}

	// At the end of every 3DObject display routine:
	commonFinish();
}

void CGraph::setJustDrawCurves(bool justCurves)
{
	justDrawCurves=justCurves;
}

int CGraph::getDataStreamCount()
{
	return(daten.size());
}
int CGraph::get2DCurveCount()
{
	return(twoDPartners.size());
}
int CGraph::get3DCurveCount()
{
	return(threeDPartners.size());
}

CGraphData* CGraph::getGraphData(int id)
{
	for (int i=0;i<int(daten.size());i++)
	{
		if (daten[i]->getIdentifier()==id)
			return(daten[i]);
	}
	return(NULL);
}
CGraphData* CGraph::getGraphData(std::string theName)
{
	for (int i=0;i<int(daten.size());i++)
	{
		if (daten[i]->getName().compare(theName)==0)
			return(daten[i]);
	}
	return(NULL);
}
CGraphDataComb* CGraph::getGraphData3D(int id)
{
	for (int i=0;i<int(threeDPartners.size());i++)
	{
		if (threeDPartners[i]->getIdentifier()==id)
		return(threeDPartners[i]);
	}
	return(NULL);
}
CGraphDataComb* CGraph::getGraphData3D(std::string theName)
{
	for (int i=0;i<int(threeDPartners.size());i++)
	{
		if (threeDPartners[i]->getName().compare(theName)==0)
		return(threeDPartners[i]);
	}
	return(NULL);
}
CGraphDataComb* CGraph::getGraphData2D(int id)
{
	for (int i=0;i<int(twoDPartners.size());i++)
	{
		if (twoDPartners[i]->getIdentifier()==id)
		return(twoDPartners[i]);
	}
	return(NULL);
}
CGraphDataComb* CGraph::getGraphData2D(std::string theName)
{
	for (int i=0;i<int(twoDPartners.size());i++)
	{
		if (twoDPartners[i]->getName().compare(theName)==0)
		return(twoDPartners[i]);
	}
	return(NULL);
}

int CGraph::addNewGraphData(CGraphData* graphData)
{	// Returns the graphData identifier
	// We don't care if already present, because we could scale one but not the other
	// for instance.
	std::string theName=graphData->getName();
	while (getGraphData(theName)!=NULL)
		theName=tt::generateNewName_noDash(theName);
	graphData->setName(theName);
	int id=0;
	while (getGraphData(id)!=NULL)
		id++;
	graphData->setIdentifier(id);
	daten.push_back(graphData);
	graphData->resetData(bufferSize);
	return(graphData->getIdentifier());
}
void CGraph::removeGraphData(int id)
{
	announceGraphDataObjectWillBeDestroyed(id);
	for (int i=0;i<int(daten.size());i++)
	{
		if (daten[i]->getIdentifier()==id)
		{
			delete daten[i];
			daten.erase(daten.begin()+i);
			break;
		}
	}
}
void CGraph::remove2DPartners(int id)
{
	for (int i=0;i<int(twoDPartners.size());i++)
	{
		if (twoDPartners[i]->getIdentifier()==id)
		{
			delete twoDPartners[i];
			twoDPartners.erase(twoDPartners.begin()+i);
			break;
		}
	}
}
void CGraph::remove3DPartners(int id)
{
	for (int i=0;i<int(threeDPartners.size());i++)
	{
		if (threeDPartners[i]->getIdentifier()==id)
		{
			delete threeDPartners[i];
			threeDPartners.erase(threeDPartners.begin()+i);
			break;
		}
	}
}
void CGraph::add2DPartners(CGraphDataComb* it)
{
	std::string tmp=it->getName();
	while (getGraphData2D(tmp)!=NULL)
		tmp=tt::generateNewName_noDash(tmp);
	it->setName(tmp);
	int id=0;
	while (getGraphData2D(id)!=NULL)
		id++;
	it->setIdentifier(id);
	twoDPartners.push_back(it);
}
void CGraph::add3DPartners(CGraphDataComb* it)
{
	std::string tmp=it->getName();
	while (getGraphData3D(tmp)!=NULL)
		tmp=tt::generateNewName_noDash(tmp);
	it->setName(tmp);
	int id=0;
	while (getGraphData3D(id)!=NULL)
		id++;
	it->setIdentifier(id);
	threeDPartners.push_back(it);
}

void CGraph::addNextPoint(float time)
{
	int nextEntryPosition;
	if (numberOfPoints>=bufferSize)
	{ // We reached the maximum of points
		if (!cyclic)
			return; // The buffer is not cyclic, we leave here
		else
		{ // The buffer is cyclic
			nextEntryPosition=startingPoint;
			startingPoint++;
			if (startingPoint>=bufferSize)
				startingPoint=startingPoint-bufferSize;
		}
	}
	else
	{
		nextEntryPosition=startingPoint+numberOfPoints;
		if (nextEntryPosition>=bufferSize)
			nextEntryPosition=nextEntryPosition-bufferSize;
		numberOfPoints++;
	}
	times[nextEntryPosition]=time;
	C7Vector m(getCumulativeTransformationPart1());
	for (int i=0;i<int(daten.size());i++)
	{
		bool cyclic;
		float range;
		CGraphingRoutines::getCyclicAndRangeValues(daten[i],cyclic,range);
		daten[i]->setValue(&m,nextEntryPosition,nextEntryPosition==startingPoint,cyclic,range,times);
		// Here we have to handle a special case: GRAPH_VARIOUS_TIME
		if (daten[i]->getDataType()==GRAPH_NOOBJECT_TIME)
			daten[i]->setValueDirect(nextEntryPosition,time,nextEntryPosition==startingPoint,cyclic,range,times);
		daten[i]->clearUserData();
	}
}
bool CGraph::getAbsIndexOfPosition(int pos,int& absIndex)
{	// Use this function to loop through all absolute indices until it returns false.
	if (pos>=numberOfPoints)
		return(false);
	absIndex=startingPoint+pos;
	if (absIndex>=bufferSize)
		absIndex=absIndex-bufferSize;
	return(true);
}
int CGraph::getNumberOfPoints()
{
	return(numberOfPoints);
}
void CGraph::lookAt(int windowSize[2],CSView* subView,bool timeGraph,bool drawText,bool passiveSubView,bool oneOneProportionForXYGraph)
{ // drawText is false and passiveSubView is true by default
	// Default values (used for instance in view selection mode)
	int currentWinSize[2];
	int mouseRelativePosition[2];
	int mouseDownRelativePosition[2];
	float graphPos[2]={-2.0f,-2.0f};
	float graphSize[2]={4.0f,4.0f};
	int selectionStatus=NOSELECTION;
	bool autoMode=true;
	bool timeGraphYaxisAutoMode=true;
	bool mouseIsDown=false;
	int mouseMode=sim_navigation_passive;
	if (windowSize!=NULL)
	{
		currentWinSize[0]=windowSize[0];
		currentWinSize[1]=windowSize[1];
	}
	if (subView!=NULL)
	{
		subView->getViewSize(currentWinSize);
		subView->getMouseRelativePosition(mouseRelativePosition);
		subView->getMouseDownRelativePosition(mouseDownRelativePosition);
		if (timeGraph)
		{
			autoMode=subView->getTimeGraphXAutoModeDuringSimulation()&&App::ct->simulation->isSimulationRunning();
			timeGraphYaxisAutoMode=subView->getTimeGraphYAutoModeDuringSimulation()&&App::ct->simulation->isSimulationRunning();
			oneOneProportionForXYGraph=false;
		}
		else
		{
			autoMode=subView->getXYGraphAutoModeDuringSimulation()&&App::ct->simulation->isSimulationRunning();
			oneOneProportionForXYGraph=subView->getXYGraphIsOneOneProportional();
			timeGraphYaxisAutoMode=true;
		}
		subView->getGraphPosition(graphPos);
		subView->getGraphSize(graphSize);
		timeGraph=subView->getTimeGraph();
		if (!passiveSubView)
		{
			selectionStatus=subView->getSelectionStatus();
			mouseIsDown=subView->isMouseDown();
		}
	}

	// We go a first time through the rendering process without displaying anything (we check the view size etc.)
	drawValues(currentWinSize,graphPos,graphSize,mouseRelativePosition,false,true,autoMode,timeGraphYaxisAutoMode,drawText,passiveSubView,timeGraph,subView);

	if ((!timeGraph)&&oneOneProportionForXYGraph)
		validateViewValues(currentWinSize,graphPos,graphSize,false,false,true,autoMode);

	// We set up the view:	
	glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
	glDisable(GL_CULL_FACE);
	glClearColor(backgroundColor[0],backgroundColor[1],backgroundColor[2],1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(graphPos[0],graphPos[0]+graphSize[0],graphPos[1],graphPos[1]+graphSize[1],-1.0f,1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRenderMode(GL_RENDER);
	glDisable(GL_DEPTH_TEST);
	ogl::allLightsOffAmbientFull();
	ogl::setColorsAllBlack();
	// Now we render the graph:
	drawValues(currentWinSize,graphPos,graphSize,mouseRelativePosition,mouseIsDown,false,autoMode,timeGraphYaxisAutoMode,drawText,passiveSubView,timeGraph,subView);
	drawGrid(currentWinSize,graphPos,graphSize);
	if (!passiveSubView)
	{
		if (!App::ct->simulation->isSimulationRunning())
			drawOverlay(currentWinSize,graphPos,graphSize,mouseMode,subView,passiveSubView);
	}
		
	// Once the scene is rendered, we have to report changes of some values:
	if (subView!=NULL)
	{
		subView->setGraphPosition(graphPos[0],graphPos[1]);
		subView->setGraphSize(graphSize[0],graphSize[1]);
	}
}

void CGraph::drawGrid(int windowSize[2],float graphPosition[2],float graphSize[2])
{
	float interline=((float)ogl::getInterline())*graphSize[1]/(float)windowSize[1];
	float labelPos[2];
	labelPos[0]=graphPosition[0]+3.0f*graphSize[0]/(float)windowSize[0];
	labelPos[1]=graphPosition[1]+0.3f*interline;

	// First compute the x- and y-grid start and the x- and y-grid spacing
	int minNbOfLines[2]={8*windowSize[0]/1024,8*windowSize[1]/768};
	int maxNbOfLines[2]={2*minNbOfLines[0],2*minNbOfLines[1]};

	float gridStartX=graphPosition[0];
	float a=(float)((int)log10f(graphSize[0]));
	if (graphSize[0]<1.0f) 
		a=a-1.0f;
	float gridSpacingX=(float)pow(10.0f,a);
	while((graphSize[0]/gridSpacingX)<minNbOfLines[0]) 
		gridSpacingX=gridSpacingX/2.0f;
	while((graphSize[0]/gridSpacingX)>maxNbOfLines[0]) 
		gridSpacingX=gridSpacingX*2.0f;
	gridStartX=(((int)(gridStartX/gridSpacingX))*gridSpacingX)-gridSpacingX;

	float gridStartY=graphPosition[1];
	a=(float)((int)log10f(graphSize[1]));
	if (graphSize[1]<1.0f) 
		a=a-1.0f;
	float gridSpacingY=(float)pow(10.0f,a);
	while ((graphSize[1]/gridSpacingY)<minNbOfLines[1]) 
		gridSpacingY=gridSpacingY/2.0f;
	while ((graphSize[1]/gridSpacingY)>maxNbOfLines[1]) 
		gridSpacingY=gridSpacingY*2.0f;
	gridStartY=(((int)(gridStartY/gridSpacingY))*gridSpacingY)-gridSpacingY;
	
	glLineStipple(1,0x1111);
	glEnable(GL_LINE_STIPPLE);
	ogl::setColorsAllBlack(); 
	ogl::setColor(textColor,AMBIENT_MODE);
		
	for (int i=0;i<60;i++)
	{
		std::string tmp;
		if (graphGrid)
		{
			glBegin(GL_LINES);
			glVertex3f(gridStartX+gridSpacingX*i,gridStartY,0.0f);
			glVertex3f(gridStartX+gridSpacingX*i,gridStartY+60.0f*gridSpacingY,0.0f);
			glEnd();
		}
		if (graphValues)
		{
			tmp=gv::getTimeStr(gridStartX+gridSpacingX*i);
			ogl::drawBitmapTextIntoScene(gridStartX+gridSpacingX*i,labelPos[1],0.0f,tmp.c_str());
		}
		if (graphGrid)
		{
			glBegin(GL_LINES);
			glVertex3f(gridStartX,gridStartY+gridSpacingY*i,0.0f);
			glVertex3f(gridStartX+60.0f*gridSpacingX,gridStartY+gridSpacingY*i,0.0f);
			glEnd();
		}
		if (graphValues)
		{
			int d=0;
			if (fabs(gridSpacingY)>0.0f)
			{
				float l=log10f(fabs(gridSpacingY));
				d=int(1.5f-l);
				if (d<0)
					d=0;
				tmp=tt::FNb(0,gridStartY+gridSpacingY*i,d,false);
				ogl::drawBitmapTextIntoScene(labelPos[0],gridStartY+gridSpacingY*i,0.0f,tmp.c_str());
			}
		}
	}
	glDisable(GL_LINE_STIPPLE);
}

void CGraph::drawOverlay(int windowSize[2],float graphPosition[2],float graphSize[2],int mouseMode,CSView* subView,bool passiveSubView)
{
	// Draw the selection square
	if ( (subView!=NULL)&&(!passiveSubView)&&subView->isMouseDown()&&(subView->getSelectionStatus()==SHIFTSELECTION) )
	{
		float downRelPos[2];
		float relPos[2];
		int aux[2];
		subView->getMouseDownRelativePosition(aux);
		downRelPos[0]=graphPosition[0]+((float)aux[0]/(float)windowSize[0])*graphSize[0];
		downRelPos[1]=graphPosition[1]+((float)aux[1]/(float)windowSize[1])*graphSize[1];
		subView->getMouseRelativePosition(aux);
		relPos[0]=graphPosition[0]+((float)aux[0]/(float)windowSize[0])*graphSize[0];
		relPos[1]=graphPosition[1]+((float)aux[1]/(float)windowSize[1])*graphSize[1];
		ogl::setColorsAllBlack();
		ogl::setColor(textColor,AMBIENT_MODE);
		if ((relPos[0]>downRelPos[0])&&(downRelPos[1]>relPos[1]))
		{
			float black[3]={0.0f,0.0f,0.0f};
			float yellow[3]={1.0f,1.0f,0.0f};
			ogl::setColor(yellow,black,black,black,12,0.2f);
			ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glBegin(GL_QUADS);
				glVertex3f(downRelPos[0],downRelPos[1],0);
				glVertex3f(downRelPos[0],relPos[1],0);
				glVertex3f(relPos[0],relPos[1],0);
				glVertex3f(relPos[0],downRelPos[1],0);
				glVertex3f(downRelPos[0],downRelPos[1],0);
			glEnd();
			ogl::setBlending(false);			
			ogl::setColorsAllBlack();
			ogl::setColor(textColor,AMBIENT_MODE);
			glBegin(GL_LINE_STRIP);
				glVertex3f(downRelPos[0],downRelPos[1],0);
				glVertex3f(downRelPos[0],relPos[1],0);
				glVertex3f(relPos[0],relPos[1],0);
				glVertex3f(relPos[0],downRelPos[1],0);
				glVertex3f(downRelPos[0],downRelPos[1],0);
			glEnd();
			ogl::drawBitmapTextIntoScene((relPos[0]+downRelPos[0])/2.0f,(relPos[1]+downRelPos[1])/2.0f,0.0f,IDSOGL_ZOOMING_IN);
		}
		if ((relPos[0]>downRelPos[0])&&(relPos[1]>downRelPos[1]))
			ogl::drawBitmapTextIntoScene((relPos[0]+downRelPos[0])/2.0f,(relPos[1]+downRelPos[1])/2.0f,0.0f,IDSOGL_SETTING_PROPORTIONS_TO_1_1);
		if ((downRelPos[0]>relPos[0])&&(relPos[1]>downRelPos[1]))
			ogl::drawBitmapTextIntoScene((relPos[0]+downRelPos[0])/2.0f,(relPos[1]+downRelPos[1])/2.0f,0.0f,IDSOGL_ZOOMING_OUT);
		if ((downRelPos[0]>relPos[0])&&(relPos[1]<downRelPos[1]))
			ogl::drawBitmapTextIntoScene((relPos[0]+downRelPos[0])/2.0f,(relPos[1]+downRelPos[1])/2.0f,0.0f,IDSOGL_ZOOMING_IN);
	}  
}




void CGraph::drawValues(int windowSize[2],float graphPosition[2],float graphSize[2],
						int mousePosition[2],bool mouseIsDown,bool dontRender,
						bool autoMode,bool timeGraphYaxisAutoMode,bool drawText,bool passiveSubView,bool timeGraph,CSView* subView)
{ // This is a quite ugly routine which requires refactoring!
	static bool markSelectedStream=false;
	float interline=((float)ogl::getInterline())*graphSize[1]/(float)windowSize[1];
	float labelPos[2];
	float pixelSizeCoeff=graphSize[0]/(float)windowSize[0];
	labelPos[0]=graphPosition[0]+graphSize[0]-3.0f*pixelSizeCoeff;
	labelPos[1]=graphPosition[1]+graphSize[1]-interline;

	float maxVal[2]={-SIM_MAX_FLOAT,-SIM_MAX_FLOAT};
	float minVal[2]={+SIM_MAX_FLOAT,+SIM_MAX_FLOAT};
	if (dontRender)
		trackingValueIndex=-1;
	float ratio=graphSize[1]/graphSize[0];
	float relMousePos[2];
	relMousePos[0]=graphPosition[0]+graphSize[0]*((float)mousePosition[0]/(float)windowSize[0]);
	relMousePos[1]=graphPosition[1]+graphSize[1]*((float)mousePosition[1]/(float)windowSize[1]);
	ogl::setColorsAllBlack();
	if (App::userSettings->antiAliasing)
	{
		glEnable (GL_LINE_SMOOTH);
		glEnable (GL_POINT_SMOOTH);
		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint (GL_POINT_SMOOTH_HINT, GL_NICEST);
	}
	if (timeGraph)
	{ // Display of time graph curves here:
		std::string tmp(IDSOGL_TIME_GRAPH_CURVES_);
		float tl=float(ogl::getTextLengthInPixels(tmp))*pixelSizeCoeff;
		ogl::setColor(textColor,EMISSION_MODE);
		ogl::drawBitmapTextIntoScene(labelPos[0]-tl,labelPos[1],0.0f,tmp);
		labelPos[1]=labelPos[1]-interline;
		for (int i=0;i<int(daten.size());i++)
		{
			if (daten[i]->getVisible())
			{
				CGraphData* it=daten[i];
				ogl::setColor(it->ambientColor,EMISSION_MODE);
				if (!dontRender)
				{ // We display that curve
					if (it->getLinkPoints())
					{
						if ((trackingValueIndex==i)&&markSelectedStream&&(!trackingValueIsStatic))
							glLineWidth(3.0f); // we are tracking that curve!
						glBegin(GL_LINE_STRIP);
					}
					else
					{
						if ((trackingValueIndex==i)&&markSelectedStream&&(!trackingValueIsStatic))
							glPointSize(6.0f); // we are tracking that curve!
						else
							glPointSize(4.0f);
						glBegin(GL_POINTS);
					}
					int pos=0;
					int absIndex;
					float yVal,xVal;

					bool cyclic;
					float range;
					CGraphingRoutines::getCyclicAndRangeValues(it,cyclic,range);

					while (getAbsIndexOfPosition(pos++,absIndex))
					{
						xVal=times[absIndex];
						bool dataIsValid=getData(it,absIndex,yVal,cyclic,range,true);
						if (dataIsValid)
							glVertex3f(xVal,yVal,0.0f);					
					}
					glEnd();
					glPointSize(1.0f);
					glLineWidth(1.0f);
					if (it->getLabel())
					{
						tmp=it->getName()+" ("+CGraphingRoutines::getDataUnit(it)+")";
						float tl=float(ogl::getTextLengthInPixels(tmp))*pixelSizeCoeff;
						ogl::drawBitmapTextIntoScene(labelPos[0]-tl,labelPos[1],0.0f,tmp);
						labelPos[1]=labelPos[1]-interline;
					}

				}
				else
				{ // We don't display the curve, we just get its min-max values
					int pos=0;
					int absIndex;
					float yVal,xVal;
					bool cyclic;
					float range;
					CGraphingRoutines::getCyclicAndRangeValues(it,cyclic,range);
					while (getAbsIndexOfPosition(pos++,absIndex))
					{
						xVal=times[absIndex];
						bool dataIsValid=getData(it,absIndex,yVal,cyclic,range,true);
						if (xVal<minVal[0]) // keep those two outside of dataIsValid check!
							minVal[0]=xVal;
						if (xVal>maxVal[0]) 
							maxVal[0]=xVal;
						if (dataIsValid)
						{
							if (yVal<minVal[1]) 
								minVal[1]=yVal;
							if (yVal>maxVal[1]) 
								maxVal[1]=yVal;
							if ( (!passiveSubView)&&(!mouseIsDown) )
							{ // Here we have the value tracking part:
								if (trackingValueIndex!=-1)
								{
									float d1=(relMousePos[0]-xVal)*ratio;
									float d2=relMousePos[1]-yVal;
									float dist=d1*d1+d2*d2;
									if (dist<squareDistFromTrackingValue)
									{
										trackingValue[0]=xVal;
										trackingValue[1]=yVal;
										squareDistFromTrackingValue=dist;
										trackingValueIndex=i;
										trackingValueIsStatic=false;
									}
								}
								else
								{
									trackingValue[0]=xVal;
									trackingValue[1]=yVal;
									float d1=(relMousePos[0]-xVal)*ratio;
									float d2=relMousePos[1]-yVal;
									squareDistFromTrackingValue=d1*d1+d2*d2;
									trackingValueIndex=i;
									trackingValueIsStatic=false;
								}
							}
						}
					}
				}
			}
		}

		// We have the static curves here:
		float vOffset=2.0f*graphSize[1]/float(windowSize[1]);
		if (dontRender)
			vOffset=0.0f;
		for (int i=0;i<int(_staticCurves.size());i++)
		{
			if (_staticCurves[i]->getCurveType()==0)
			{
				CStaticGraphCurve* it=_staticCurves[i];
				if (!dontRender)
				{
					ogl::setColor(it->color,EMISSION_MODE);
					if (it->getLinkPoints())
					{
						if ((trackingValueIndex==i)&&markSelectedStream&&trackingValueIsStatic)
							glLineWidth(3.0f); // we are tracking that curve!
						glLineStipple(1,0xE187);
						glEnable(GL_LINE_STIPPLE);
						glBegin(GL_LINE_STRIP);
					}
					else
					{
						if ((trackingValueIndex==i)&&markSelectedStream&&trackingValueIsStatic)
							glPointSize(6.0f); // we are tracking that curve!
						else
							glPointSize(4.0f);
						glBegin(GL_POINTS);
					}
				}
				for (int ka=0;ka<int(it->values.size()/2);ka++)
				{
					float xVal=it->values[2*ka+0]*gv::secondsToUser;
					float yVal=it->values[2*ka+1]+vOffset;
					glVertex3f(xVal,yVal,0.0f);
					if (dontRender)
					{ // min/max and value tracking
						if ((yVal<minVal[1])&&(xVal>=minVal[0])&&(xVal<=maxVal[0])) // only in the active time slice!
							minVal[1]=yVal;
						if ((yVal>maxVal[1])&&(xVal>=minVal[0])&&(xVal<=maxVal[0])) // only in the active time slice!
							maxVal[1]=yVal;
						if ( (!passiveSubView)&&(!mouseIsDown) )
						{ // Here we have the value tracking part:
							if (trackingValueIndex!=-1)
							{
								if ((xVal>=minVal[0])&&(xVal<=maxVal[0])) // only in the active time slice!
								{
									float d1=(relMousePos[0]-xVal)*ratio;
									float d2=relMousePos[1]-yVal;
									float dist=d1*d1+d2*d2;
									if (dist<squareDistFromTrackingValue)
									{
										trackingValue[0]=xVal;
										trackingValue[1]=yVal-vOffset;
										squareDistFromTrackingValue=dist;
										trackingValueIndex=i;
										trackingValueIsStatic=true;
									}
								}
							}
							else
							{
								if ((xVal>=minVal[0])&&(xVal<=maxVal[0])) // only in the active time slice!
								{
									trackingValue[0]=xVal;
									trackingValue[1]=yVal-vOffset;
									float d1=(relMousePos[0]-xVal)*ratio;
									float d2=relMousePos[1]-yVal;
									squareDistFromTrackingValue=d1*d1+d2*d2;
									trackingValueIndex=i;
									trackingValueIsStatic=true;
								}
							}
						}
					}
				}
				if (!dontRender)
				{
					glEnd();
					glPointSize(1.0f);
					glLineWidth(1.0f);
					glDisable(GL_LINE_STIPPLE);
					if (it->getLabel())
					{
						tmp=it->getName()+" [STATIC]";
						float tl=float(ogl::getTextLengthInPixels(tmp))*pixelSizeCoeff;
						ogl::drawBitmapTextIntoScene(labelPos[0]-tl,labelPos[1],0.0f,tmp);
						labelPos[1]=labelPos[1]-interline;
					}
				}
			}
		}
	}
	else
	{ // Display of xy graph curves here:
		std::string tmp(IDSOGL_X_Y_GRAPH_CURVES_);
		float tl=float(ogl::getTextLengthInPixels(tmp))*pixelSizeCoeff;
		ogl::setColor(textColor,EMISSION_MODE);
		ogl::drawBitmapTextIntoScene(labelPos[0]-tl,labelPos[1],0.0f,tmp);
		labelPos[1]=labelPos[1]-interline;
		for (int i=0;i<int(twoDPartners.size());i++)
		{
			if (twoDPartners[i]->getVisible())
			{
				CGraphDataComb* it=twoDPartners[i];
				ogl::setColor(it->ambientColor,EMISSION_MODE);
				if (!dontRender)
				{
					if (it->getLinkPoints())
					{
						if ((trackingValueIndex==i)&&markSelectedStream&&(!trackingValueIsStatic))
							glLineWidth(3.0f); // we are tracking that curve!
						glBegin(GL_LINE_STRIP);
					}
					else
					{
						if ((trackingValueIndex==i)&&markSelectedStream&&(!trackingValueIsStatic))
							glPointSize(6.0f); // we are tracking that curve!
						else
							glPointSize(4.0f);
						glBegin(GL_POINTS);
					}
					int pos=0;
					int absIndex;
					float val[3];
					CGraphData* number1=getGraphData(it->data[0]);
					CGraphData* number2=getGraphData(it->data[1]);

					bool cyclic1,cyclic2;
					float range1,range2;
					if (number1!=NULL)	
						CGraphingRoutines::getCyclicAndRangeValues(number1,cyclic1,range1);
					if (number2!=NULL)	
						CGraphingRoutines::getCyclicAndRangeValues(number2,cyclic2,range2);

					while (getAbsIndexOfPosition(pos++,absIndex))
					{
						bool dataIsValid=true;
						if (number1!=NULL)
						{
							if(!getData(number1,absIndex,val[0],cyclic1,range1,true))
								dataIsValid=false;
						}
						else
							dataIsValid=false;
						if (number2!=NULL)
						{
							if(!getData(number2,absIndex,val[1],cyclic2,range2,true))
								dataIsValid=false;
						}
						else
							dataIsValid=false;
						if (dataIsValid)
							glVertex3f(val[0],val[1],0.0f);	
					}
					glEnd();
					glPointSize(1.0f);
					glLineWidth(1.0f);
					if ( it->getLabel() )
					{
						if ( (number1!=NULL)&&(number2!=NULL) )
						{
							tmp=it->getName()+" (x: "+CGraphingRoutines::getDataUnit(number1)+") (y: "+CGraphingRoutines::getDataUnit(number2)+")";
							float tl=float(ogl::getTextLengthInPixels(tmp))*pixelSizeCoeff;
							ogl::drawBitmapTextIntoScene(labelPos[0]-tl,labelPos[1],0.0f,tmp);
							labelPos[1]=labelPos[1]-interline;
						}
					}
				}
				else
				{ // We don't display the curve, we just take the max-min values:
					int pos=0;
					int absIndex;
					float val[3];
					CGraphData* number1=getGraphData(it->data[0]);
					CGraphData* number2=getGraphData(it->data[1]);
					bool cyclic1,cyclic2;
					float range1,range2;
					if (number1!=NULL)	
						CGraphingRoutines::getCyclicAndRangeValues(number1,cyclic1,range1);
					if (number2!=NULL)	
						CGraphingRoutines::getCyclicAndRangeValues(number2,cyclic2,range2);
					while (getAbsIndexOfPosition(pos++,absIndex))
					{
						bool dataIsValid=true;
						if (number1!=NULL)
						{
							if(!getData(number1,absIndex,val[0],cyclic1,range1,true))
								dataIsValid=false;
						}
						else
							dataIsValid=false;
						if (number2!=NULL)
						{
							if(!getData(number2,absIndex,val[1],cyclic2,range2,true))
								dataIsValid=false;
						}
						else
							dataIsValid=false;
						if (dataIsValid)
						{
							if (val[0]<minVal[0]) 
								minVal[0]=val[0];
							if (val[0]>maxVal[0]) 
								maxVal[0]=val[0];
							if (val[1]<minVal[1]) 
								minVal[1]=val[1];
							if (val[1]>maxVal[1]) 
								maxVal[1]=val[1];

							if ( (!passiveSubView)&&(!mouseIsDown) )
							{ // Here we have the value tracking part:
								if (trackingValueIndex!=-1)
								{
									float d1=(relMousePos[0]-val[0])*ratio;
									float d2=relMousePos[1]-val[1];
									float dist=d1*d1+d2*d2;
									if (dist<squareDistFromTrackingValue)
									{
										trackingValue[0]=val[0];
										trackingValue[1]=val[1];
										squareDistFromTrackingValue=dist;
										trackingValueIndex=i;
										trackingValueIsStatic=false;
									}
								}
								else
								{
									trackingValue[0]=val[0];
									trackingValue[1]=val[1];
									float d1=(relMousePos[0]-val[0])*ratio;
									float d2=relMousePos[1]-val[1];
									squareDistFromTrackingValue=d1*d1+d2*d2;
									trackingValueIndex=i;
									trackingValueIsStatic=false;
								}
							}
						}
					}
				}
			}
		}

		// We have the static curves here:
		float vOffset=2.0f*graphSize[1]/float(windowSize[1]);
		if (dontRender)
			vOffset=0.0f;
		for (int i=0;i<int(_staticCurves.size());i++)
		{
			if (_staticCurves[i]->getCurveType()==1)
			{
				CStaticGraphCurve* it=_staticCurves[i];
				if (!dontRender)
				{
					ogl::setColor(it->color,EMISSION_MODE);
					if (it->getLinkPoints())
					{
						if ((trackingValueIndex==i)&&markSelectedStream&&trackingValueIsStatic)
							glLineWidth(3.0f); // we are tracking that curve!
						glLineStipple(1,0xE187);
						glEnable(GL_LINE_STIPPLE);
						glBegin(GL_LINE_STRIP);
					}
					else
					{
						if ((trackingValueIndex==i)&&markSelectedStream&&trackingValueIsStatic)
							glPointSize(6.0f); // we are tracking that curve!
						else
							glPointSize(4.0f);
						glBegin(GL_POINTS);
					}
				}
				for (int ka=0;ka<int(it->values.size()/2);ka++)
				{
					float xVal=it->values[2*ka+0];
					float yVal=it->values[2*ka+1]+vOffset;
					glVertex3f(xVal,yVal,0.0f);


					if (dontRender)
					{ // Min/max and value tracking!
						if (xVal<minVal[0]) 
							minVal[0]=xVal;
						if (xVal>maxVal[0]) 
							maxVal[0]=xVal;
						if (yVal<minVal[1]) 
							minVal[1]=yVal;
						if (yVal>maxVal[1]) 
							maxVal[1]=yVal;
						if ( (!passiveSubView)&&(!mouseIsDown) )
						{ // Here we have the value tracking part:
							if (trackingValueIndex!=-1)
							{
								float d1=(relMousePos[0]-xVal)*ratio;
								float d2=relMousePos[1]-yVal;
								float dist=d1*d1+d2*d2;
								if (dist<squareDistFromTrackingValue)
								{
									trackingValue[0]=xVal;
									trackingValue[1]=yVal-vOffset;
									squareDistFromTrackingValue=dist;
									trackingValueIndex=i;
									trackingValueIsStatic=true;
								}
							}
							else
							{
								trackingValue[0]=xVal;
								trackingValue[1]=yVal-vOffset;
								float d1=(relMousePos[0]-xVal)*ratio;
								float d2=relMousePos[1]-yVal;
								squareDistFromTrackingValue=d1*d1+d2*d2;
								trackingValueIndex=i;
								trackingValueIsStatic=true;
							}
						}
					}
				}
				if (!dontRender)
				{
					glEnd();
					glPointSize(1.0f);
					glLineWidth(1.0f);
					glDisable(GL_LINE_STIPPLE);
					if (it->getLabel())
					{
						tmp=it->getName()+" [STATIC]";
						float tl=float(ogl::getTextLengthInPixels(tmp))*pixelSizeCoeff;
						ogl::drawBitmapTextIntoScene(labelPos[0]-tl,labelPos[1],0.0f,tmp);
						labelPos[1]=labelPos[1]-interline;
					}
				}
			}
		}
	}

	// following 3 to reset antialiasing:
	glDisable (GL_LINE_SMOOTH);
	glDisable (GL_POINT_SMOOTH);
	glDisable (GL_BLEND);

	// Data tracking:
	bool markSelectedStreamSaved=markSelectedStream;
	markSelectedStream=false;
	float sensitivity=30.0f;
	if ( (!passiveSubView)&&(!mouseIsDown)&&dontRender )
	{
		if ( (trackingValueIndex!=-1)&&((sqrtf(squareDistFromTrackingValue)*windowSize[1]/graphSize[1])<sensitivity) )
			markSelectedStream=true;
	}
	if (subView!=NULL)
		subView->setTrackedGraphCurveIndex(-1);
	if ( (!passiveSubView)&&(!mouseIsDown)&&(!dontRender) )
	{
		if ( (trackingValueIndex!=-1)&&((sqrtf(squareDistFromTrackingValue)*windowSize[1]/graphSize[1])<sensitivity) )
		{
			float squareSize=5.0f;
			float r[2];
			r[0]=squareSize*graphSize[0]/(float)windowSize[0];
			r[1]=squareSize*graphSize[1]/(float)windowSize[1];
			ogl::setColorsAllBlack();
			ogl::setColor(textColor,EMISSION_MODE);
			std::string tmp;
			if (subView!=NULL)
				subView->setTrackedGraphCurveIndex(-1);
			if (timeGraph)
			{
				if (!trackingValueIsStatic)
				{
					CGraphData* it=daten[trackingValueIndex];
					tmp=" ("+boost::lexical_cast<std::string>(trackingValue[0]*gv::secondsToUser)+";";
					tmp+=boost::lexical_cast<std::string>(trackingValue[1])+")";
					tmp=it->getName()+tmp;
					ogl::drawBitmapTextIntoScene(trackingValue[0]+r[0]*1.5f,trackingValue[1]+r[1]*1.0f,0.0f,tmp);
					ogl::setColor(it->ambientColor,EMISSION_MODE);
					if (markSelectedStreamSaved&&(subView!=NULL))
						subView->setTrackedGraphCurveIndex(trackingValueIndex);
				}
				else
				{ // Tracking a static curve here!
					CStaticGraphCurve* it=_staticCurves[trackingValueIndex];
					tmp=" [STATIC] ("+boost::lexical_cast<std::string>(trackingValue[0]*gv::secondsToUser)+";";
					tmp+=boost::lexical_cast<std::string>(trackingValue[1])+")";
					tmp=it->getName()+tmp;
					ogl::drawBitmapTextIntoScene(trackingValue[0]+r[0]*1.5f,trackingValue[1]-r[1]*0.9f,0.0f,tmp);
					ogl::setColor(it->color,EMISSION_MODE);
				}
			}
			else
			{
				if (!trackingValueIsStatic)
				{
					CGraphDataComb* it=twoDPartners[trackingValueIndex];
					tmp=" ("+boost::lexical_cast<std::string>(trackingValue[0])+";";
					tmp+=boost::lexical_cast<std::string>(trackingValue[1])+")";
					tmp=it->getName()+tmp;
					ogl::drawBitmapTextIntoScene(trackingValue[0]+r[0]*1.5f,trackingValue[1]-r[1]*0.9f,0.0f,tmp);
					ogl::setColor(it->ambientColor,EMISSION_MODE);
					if (markSelectedStreamSaved&&(subView!=NULL))
						subView->setTrackedGraphCurveIndex(trackingValueIndex);
				}
				else
				{ // tracking a static curve here!
					CStaticGraphCurve* it=_staticCurves[trackingValueIndex];
					tmp=" [STATIC] ("+boost::lexical_cast<std::string>(trackingValue[0])+";";
					tmp+=boost::lexical_cast<std::string>(trackingValue[1])+")";
					tmp=it->getName()+tmp;
					ogl::drawBitmapTextIntoScene(trackingValue[0]+r[0]*1.5f,trackingValue[1]-r[1]*0.9f,0.0f,tmp);
					ogl::setColor(it->color,EMISSION_MODE);
				}
			}
			glBegin(GL_LINE_STRIP);
			glVertex3f(trackingValue[0]-r[0],trackingValue[1]-r[1],0.0f);
			glVertex3f(trackingValue[0]+r[0],trackingValue[1]-r[1],0.0f);
			glVertex3f(trackingValue[0]+r[0],trackingValue[1]+r[1],0.0f);
			glVertex3f(trackingValue[0]-r[0],trackingValue[1]+r[1],0.0f);
			glVertex3f(trackingValue[0]-r[0],trackingValue[1]-r[1],0.0f);
			glEnd();
		}
	}
	
	if (dontRender) // Recently modified to avoid strange effects when zooming
	{ // into a running time graph!
		float size[2]={maxVal[0]-minVal[0],maxVal[1]-minVal[1]};
		if (timeGraph)
		{
			if (autoMode)
			{ // for the x axis:
				graphPosition[0]=minVal[0]-0.05f*size[0];
				graphSize[0]=1.1f*size[0];
			}
			if (timeGraphYaxisAutoMode)
			{ // for the y axis:
				graphPosition[1]=minVal[1]-0.05f*size[1];
				graphSize[1]=1.1f*size[1];
			}
		}
		else
		{
			if (autoMode)
			{
				graphPosition[0]=minVal[0]-0.05f*size[0];
				graphPosition[1]=minVal[1]-0.05f*size[1];
				graphSize[0]=1.1f*size[0];
				graphSize[1]=1.1f*size[1];
			}
		}
		validateViewValues(windowSize,graphPosition,graphSize,timeGraph,false,false,autoMode);
	}
}

void CGraph::scaleObject(float scalingFactor)
{
	size=size*scalingFactor;
	scaleObjectMain(scalingFactor);
}

void CGraph::validateViewValues(int windowSize[2],float graphPosition[2],float graphSize[2],
			bool timeGraph,bool shiftOnly,bool keepProp,bool autoModeForTimeGraphXaxis)
{ // keepProp is false by default, shiftOnly also
	float minValues[2]={-SIM_MAX_FLOAT,-SIM_MAX_FLOAT};
	float maxValues[2]={+SIM_MAX_FLOAT,+SIM_MAX_FLOAT};
	float minGraphSize[2]={0.00001f,0.00001f};
	if (timeGraph)
	{
		if (autoModeForTimeGraphXaxis)
		{
			if (numberOfPoints<2)
			{
				minValues[0]=0.0f;
				maxValues[0]=1.0f;
				minGraphSize[0]=1.0f;
			}
			else
			{
				int absIndex;
				getAbsIndexOfPosition(0,absIndex);
				minValues[0]=times[absIndex];
				getAbsIndexOfPosition(numberOfPoints-1,absIndex);
				maxValues[0]=times[absIndex];
			}
		}
	}
	float maxGraphSize[2]={maxValues[0]-minValues[0],maxValues[1]-minValues[1]};
	if (shiftOnly)
	{
		if (graphPosition[0]<minValues[0]) 
			graphPosition[0]=minValues[0];
		if (graphPosition[1]<minValues[1]) 
			graphPosition[1]=minValues[1];
		if ((graphPosition[0]+graphSize[0])>maxValues[0]) 
			graphPosition[0]=graphPosition[0]-(graphPosition[0]+graphSize[0]-maxValues[0]);
		if ((graphPosition[1]+graphSize[1])>maxValues[1]) 
			graphPosition[1]=graphPosition[1]-(graphPosition[1]+graphSize[1]-maxValues[1]);
	}
	else
	{
		if (graphSize[0]<minGraphSize[0]) 
			graphPosition[0]=graphPosition[0]+0.5f*(graphSize[0]-minGraphSize[0]);
		if (graphSize[1]<minGraphSize[1]) 
			graphPosition[1]=graphPosition[1]+0.5f*(graphSize[1]-minGraphSize[1]);
	
		if (graphPosition[0]<minValues[0]) 
			graphPosition[0]=minValues[0];
		if (graphPosition[1]<minValues[1]) 
			graphPosition[1]=minValues[1];
		if (graphPosition[0]>(maxValues[0]-minGraphSize[0])) 
			graphPosition[0]=maxValues[0]-minGraphSize[0];
		if (graphPosition[1]>(maxValues[1]-minGraphSize[1])) 
			graphPosition[1]=maxValues[1]-minGraphSize[1];
		if (graphSize[0]<minGraphSize[0]) 
			graphSize[0]=minGraphSize[0];
		if (graphSize[1]<minGraphSize[1]) 
			graphSize[1]=minGraphSize[1];
		if ((graphPosition[0]+graphSize[0])>maxValues[0]) 
			graphSize[0]=maxValues[0]-graphPosition[0];
		if ((graphPosition[1]+graphSize[1])>maxValues[1]) 
			graphSize[1]=maxValues[1]-graphPosition[1];
	}

	if (keepProp)
	{
		float prop[2]={((float)windowSize[0])/graphSize[0],((float)windowSize[1])/graphSize[1]};
		float coeff=prop[1]/prop[0];
		float nys=graphSize[1]*coeff;
		float nxs=graphSize[0]/coeff;
		if (coeff>1.0f)
		{	// We have to zoom out on the y-axis
			if (nys<maxGraphSize[1])
			{	// We zoom out on the y-axis
				graphPosition[1]=graphPosition[1]-(nys-graphSize[1])/2.0f;
				if (graphPosition[1]<minValues[1]) 
					graphPosition[1]=minValues[1];
				if ((graphPosition[1]+nys)>maxValues[1]) 
					graphPosition[1]=graphPosition[1]+maxValues[1]-(graphPosition[1]+nys);
				graphSize[1]=nys;	
			}
			else
			{	// We zoom in on the x-axis
				graphPosition[0]=graphPosition[0]-(nxs-graphSize[0])/2.0f;
				graphSize[0]=nxs;	
			}
		}
		else
		{	// We have to zoom in on the y-axis
			if (nxs<maxGraphSize[0])
			{	// We zoom out on the x-axis
				graphPosition[0]=graphPosition[0]-(nxs-graphSize[0])/2.0f;
				if (graphPosition[0]<minValues[0]) 
					graphPosition[0]=minValues[0];
				if ((graphPosition[0]+nxs)>maxValues[0]) 
					graphPosition[0]=graphPosition[0]+maxValues[0]-(graphPosition[0]+nxs);
				graphSize[0]=nxs;	
			}
			else
			{	// We zoom in on the y-axis
				graphPosition[1]=graphPosition[1]-(nys-graphSize[1])/2.0f;
				graphSize[1]=nys;	
			}
		}
	}
}

C3DObject* CGraph::copyYourself()
{	
	CGraph* newGraph=(CGraph*)copyYourselfMain();
	color.copyYourselfInto(&newGraph->color);
	newGraph->setSize(getSize());
	newGraph->setCyclic(getCyclic());
	newGraph->setBufferSize(getBufferSize());
	newGraph->numberOfPoints=numberOfPoints;
	newGraph->startingPoint=startingPoint;
	newGraph->xYZPlanesDisplay=xYZPlanesDisplay;
	newGraph->times.reserve(times.size());
	newGraph->times.clear();
	for (int i=0;i<int(times.size());i++)
		newGraph->times.push_back(times[i]);
	newGraph->daten.reserve(daten.size());
	newGraph->daten.clear();
	for (int i=0;i<int(daten.size());i++)
		newGraph->daten.push_back(daten[i]->copyYourself());
	newGraph->threeDPartners.reserve(threeDPartners.size());
	newGraph->threeDPartners.clear();
	for (int i=0;i<int(threeDPartners.size());i++)
		newGraph->threeDPartners.push_back(threeDPartners[i]->copyYourself());
	newGraph->twoDPartners.reserve(twoDPartners.size());
	newGraph->twoDPartners.clear();
	for (int i=0;i<int(twoDPartners.size());i++)
		newGraph->twoDPartners.push_back(twoDPartners[i]->copyYourself());

	for (int i=0;i<int(_staticCurves.size());i++)
		newGraph->_staticCurves.push_back(_staticCurves[i]->copyYourself());

	newGraph->backgroundColor[0]=backgroundColor[0];
	newGraph->backgroundColor[1]=backgroundColor[1];
	newGraph->backgroundColor[2]=backgroundColor[2];
	newGraph->textColor[0]=textColor[0];
	newGraph->textColor[1]=textColor[1];
	newGraph->textColor[2]=textColor[2];
	newGraph->graphGrid=graphGrid;
	newGraph->graphValues=graphValues;
	newGraph->setExplicitHandling(getExplicitHandling());

	newGraph->_initialValuesInitialized=_initialValuesInitialized;
	newGraph->_initialExplicitHandling=_initialExplicitHandling;

	return(newGraph);
}

int CGraph::getTrackingValueIndex()
{
	return(trackingValueIndex);
}

void CGraph::removeAllStatics()
{
	for (int i=0;i<int(_staticCurves.size());i++)
		delete _staticCurves[i];
	_staticCurves.clear();
}

void CGraph::copyCurveToClipboard(int curveIndex,int dimensionIndex)
{
	std::string txt;
	if (dimensionIndex==0)
	{ // time graph curves:
		if (curveIndex<int(daten.size()))
		{
			CGraphData* it=daten[curveIndex];
			int pos=0;
			int absIndex;
			float yVal,xVal;
			bool cyclic;
			float range;
			CGraphingRoutines::getCyclicAndRangeValues(it,cyclic,range);
			while (getAbsIndexOfPosition(pos++,absIndex))
			{
				xVal=times[absIndex];
				bool dataIsValid=getData(it,absIndex,yVal,cyclic,range,true);
				if (dataIsValid)
				{
					txt+=boost::lexical_cast<std::string>(xVal)+char(9);
					txt+=boost::lexical_cast<std::string>(yVal)+char(13);
					txt+=char(10);
				}
			}
		}
	}
	if (dimensionIndex==1)
	{ // x/y graph curves:
		if (curveIndex<int(twoDPartners.size()))
		{
			CGraphDataComb* it=twoDPartners[curveIndex];
			int pos=0;
			int absIndex;
			float val[3];
			CGraphData* number1=getGraphData(it->data[0]);
			CGraphData* number2=getGraphData(it->data[1]);
			bool cyclic1,cyclic2;
			float range1,range2;
			if (number1!=NULL)	
				CGraphingRoutines::getCyclicAndRangeValues(number1,cyclic1,range1);
			if (number2!=NULL)	
				CGraphingRoutines::getCyclicAndRangeValues(number2,cyclic2,range2);

			while (getAbsIndexOfPosition(pos++,absIndex))
			{
				bool dataIsValid=true;
				if (number1!=NULL)
				{
					if(!getData(number1,absIndex,val[0],cyclic1,range1,true))
						dataIsValid=false;
				}
				else
					dataIsValid=false;
				if (number2!=NULL)
				{
					if(!getData(number2,absIndex,val[1],cyclic2,range2,true))
						dataIsValid=false;
				}
				else
					dataIsValid=false;
				if (dataIsValid)
				{
					txt+=boost::lexical_cast<std::string>(val[0])+char(9);
					txt+=boost::lexical_cast<std::string>(val[1])+char(13);
					txt+=char(10);
				}
			}
		}
	}

	VVarious::copyTextToClipboard(txt);
}

void CGraph::makeCurveStatic(int curveIndex,int dimensionIndex)
{
	if (dimensionIndex==0)
	{ // time graph curves:
		if (curveIndex<int(daten.size()))
		{
			std::vector<float> timeValues;
			std::vector<float> staticValues;
			CGraphData* it=daten[curveIndex];
			int pos=0;
			int absIndex;
			float yVal,xVal;
			bool cyclic;
			float range;
			CGraphingRoutines::getCyclicAndRangeValues(it,cyclic,range);
			while (getAbsIndexOfPosition(pos++,absIndex))
			{
				xVal=times[absIndex];
				bool dataIsValid=getData(it,absIndex,yVal,cyclic,range,true);
				if (dataIsValid)
				{
					timeValues.push_back(xVal);
					staticValues.push_back(yVal);
				}
			}
			if (timeValues.size()!=0)
			{
				CStaticGraphCurve* curve=new CStaticGraphCurve(0,&timeValues,&staticValues,NULL);
				_staticCurves.push_back(curve);
				curve->setName(it->getName());
				curve->setLabel(it->getLabel());
				curve->setLinkPoints(it->getLinkPoints());
				curve->color[0]=it->ambientColor[0];
				curve->color[1]=it->ambientColor[1];
				curve->color[2]=it->ambientColor[2];
			}
		}
	}
	if (dimensionIndex==1)
	{ // x/y graph curves:
		if (curveIndex<int(twoDPartners.size()))
		{
			std::vector<float> values0;
			std::vector<float> values1;
			CGraphDataComb* it=twoDPartners[curveIndex];
			int pos=0;
			int absIndex;
			float val[3];
			CGraphData* number1=getGraphData(it->data[0]);
			CGraphData* number2=getGraphData(it->data[1]);
			bool cyclic1,cyclic2;
			float range1,range2;
			if (number1!=NULL)	
				CGraphingRoutines::getCyclicAndRangeValues(number1,cyclic1,range1);
			if (number2!=NULL)	
				CGraphingRoutines::getCyclicAndRangeValues(number2,cyclic2,range2);

			while (getAbsIndexOfPosition(pos++,absIndex))
			{
				bool dataIsValid=true;
				if (number1!=NULL)
				{
					if(!getData(number1,absIndex,val[0],cyclic1,range1,true))
						dataIsValid=false;
				}
				else
					dataIsValid=false;
				if (number2!=NULL)
				{
					if(!getData(number2,absIndex,val[1],cyclic2,range2,true))
						dataIsValid=false;
				}
				else
					dataIsValid=false;
				if (dataIsValid)
				{
					values0.push_back(val[0]);
					values1.push_back(val[1]);
				}
			}
			if (values0.size()!=0)
			{
				CStaticGraphCurve* curve=new CStaticGraphCurve(1,&values0,&values1,NULL);
				_staticCurves.push_back(curve);
				curve->setName(it->getName());
				curve->setLabel(it->getLabel());
				curve->setLinkPoints(it->getLinkPoints());
				curve->color[0]=it->ambientColor[0];
				curve->color[1]=it->ambientColor[1];
				curve->color[2]=it->ambientColor[2];
			}
		}
	}
	if (dimensionIndex==2)
	{ // 3D graph curves:
		if (curveIndex<int(threeDPartners.size()))
		{
			std::vector<float> values0;
			std::vector<float> values1;
			std::vector<float> values2;
			CGraphDataComb* it=threeDPartners[curveIndex];
			int pos=0;
			int absIndex;
			float val[3];
			CGraphData* number1=getGraphData(it->data[0]);
			CGraphData* number2=getGraphData(it->data[1]);
			CGraphData* number3=getGraphData(it->data[2]);
			bool cyclic1,cyclic2,cyclic3;
			float range1,range2,range3;
			if (number1!=NULL)	
				CGraphingRoutines::getCyclicAndRangeValues(number1,cyclic1,range1);
			if (number2!=NULL)	
				CGraphingRoutines::getCyclicAndRangeValues(number2,cyclic2,range2);
			if (number3!=NULL)	
				CGraphingRoutines::getCyclicAndRangeValues(number3,cyclic3,range3);

			while (getAbsIndexOfPosition(pos++,absIndex))
			{
				bool dataIsValid=true;
				if (number1!=NULL)
				{
					if(!getData(number1,absIndex,val[0],cyclic1,range1,true))
						dataIsValid=false;
				}
				else
					dataIsValid=false;
				if (number2!=NULL)
				{
					if(!getData(number2,absIndex,val[1],cyclic2,range2,true))
						dataIsValid=false;
				}
				else
					dataIsValid=false;
				if (number3!=NULL)
				{
					if(!getData(number3,absIndex,val[2],cyclic3,range3,true))
						dataIsValid=false;
				}
				else
					dataIsValid=false;
				if (dataIsValid)
				{
					C3Vector pp(val[0],val[1],val[2]);
					values0.push_back(val[0]);
					values1.push_back(val[1]);
					values2.push_back(val[2]);
				}
			}
			if (values0.size()!=0)
			{
				CStaticGraphCurve* curve=new CStaticGraphCurve(2,&values0,&values1,&values2);
				_staticCurves.push_back(curve);
				curve->setName(it->getName());
				curve->setLabel(it->getLabel());
				curve->setLinkPoints(it->getLinkPoints());
				curve->color[0]=it->ambientColor[0];
				curve->color[1]=it->ambientColor[1];
				curve->color[2]=it->ambientColor[2];
				curve->setCurveWidth(it->get3DCurveWidth());
				curve->setRelativeToWorld(threeDPartners[curveIndex]->getCurveRelativeToWorld());
			}
		}
	}
}

void CGraph::setSize(float theNewSize)
{
	tt::limitValue(0.001f,10.0f,theNewSize);
	size=theNewSize;
}

float CGraph::getSize()
{
	return(size);
}

void CGraph::setExplicitHandling(bool explicitHandl)
{
	_explicitHandling=explicitHandl;
}

bool CGraph::getExplicitHandling()
{
	return(_explicitHandling);
}

void CGraph::announceGraphDataObjectWillBeDestroyed(int graphDataID)
{
// We first erase all the data entries from threeDPartners and twoDPartners
// of graph graphID which contain graphDataID.
	// We first set all invalid entries to -1:	
	for (int j=0;j<3;j++)
	{
		for (int i=0;i<int(threeDPartners.size());i++)
		{
			if (threeDPartners[i]->data[j]==graphDataID)
				threeDPartners[i]->data[j]=-1;
		}
		for (int i=0;i<int(twoDPartners.size());i++)
		{
			if (twoDPartners[i]->data[j]==graphDataID)
				twoDPartners[i]->data[j]=-1;
		}
	}
	// Now we remove all threeDPartners and twoDPartners which have 0 valid
	// entries:
	int i=0;
	while (i<int(threeDPartners.size()))
	{
		int objID=threeDPartners[i]->getIdentifier();
		if ( (threeDPartners[i]->data[0]==-1)&&
			(threeDPartners[i]->data[1]==-1)&&
			(threeDPartners[i]->data[2]==-1) )
		{
			remove3DPartners(objID);
			i=0; // We start at 0 again: ordering may have changed
		}
		else
			i++;
	}
	i=0;
	while (i<int(twoDPartners.size()))
	{
		int objID=twoDPartners[i]->getIdentifier();
		if ( (twoDPartners[i]->data[0]==-1)&&
			(twoDPartners[i]->data[1]==-1)&&
			(twoDPartners[i]->data[2]==-1) )
		{
			remove2DPartners(objID);
			i=0; // We start at 0 again: ordering may have changed
		}
		else
			i++;
	}
}

void CGraph::exportGraphData(VArchive &ar)
{ // STATIC streams are not exported!! (they might have a different time interval, etc.)
	// The graph name:
	ar.writeString(getName());
	ar << (BYTE)13;
	ar << (BYTE)10;
	// The first line:
	std::string tmp("Time ("+gv::getTimeUnitStr()+")");
	if (daten.size()!=0)
		tmp+=",";
	ar.writeString(tmp);
	for (int k=0;k<int(daten.size());k++)
	{
		CGraphData* gr=daten[k];
		tmp=gr->getName()+" ("+CGraphingRoutines::getDataUnit(gr)+")";
		if (k<(int(daten.size())-1))
			tmp+=",";
		ar.writeString(tmp);
	}
	ar << (BYTE)13;
	ar << (BYTE)10;

	// Now the data:
	int pos=0;
	int absIndex;
	while (getAbsIndexOfPosition(pos++,absIndex))
	{
		float value=times[absIndex];
		tmp=tt::FNb(0,gv::secondsToUser*value,6,false);
		if (daten.size()!=0)
			tmp+=",";
		ar.writeString(tmp);
		for (int k=0;k<int(daten.size());k++)
		{
			CGraphData* gr=daten[k];
			bool cyclic;
			float range;
			CGraphingRoutines::getCyclicAndRangeValues(gr,cyclic,range);
			bool dataIsValid=getData(gr,absIndex,value,cyclic,range,true);
			if (dataIsValid)
				tmp=tt::FNb(0,value,6,false);
			else
				tmp="Null";
			if (k<(int(daten.size())-1))
				tmp+=",";
			ar.writeString(tmp);
		}
		ar << (BYTE)13;
		ar << (BYTE)10;
	}
	ar << (BYTE)13;
	ar << (BYTE)10;
}

bool CGraph::announceObjectWillBeErased(int objID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	// This routine can be called for objCont-objects, but also for objects
	// in the copy-buffer!! So never make use of any 
	// 'ct::objCont->getObject(id)'-call or similar
	// Return value true means 'this' has to be erased too!
	announceObjectWillBeErasedMain(objID,copyBuffer);
	// Remove the object which will be erased from the daten's list
	int i=0;
	while (i<int(daten.size()))
	{
		CGraphData* gr=daten[i];
		if (gr->announceObjectWillBeErased(objID,copyBuffer))
		{ // We have to remove this graphData:
			removeGraphData(gr->getIdentifier());
			i=0; // We start at 0 again
		}
		else
			i++;
	}
	return(false);
}

void CGraph::announceGroupWillBeErased(int groupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGroupWillBeErasedMain(groupID,copyBuffer);
}
void CGraph::announceCollisionWillBeErased(int collisionID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceCollisionWillBeErasedMain(collisionID,copyBuffer);
	// Remove the object which will be erased from the daten's list
	int i=0;
	while (i<int(daten.size()))
	{
		CGraphData* gr=daten[i];
		if (gr->announceCollisionWillBeErased(collisionID,copyBuffer))
		{ // We have to remove this graphData:
			removeGraphData(gr->getIdentifier());
			i=0; // We start at 0 again
		}
		else
			i++;
	}
}
void CGraph::announceDistanceWillBeErased(int distanceID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceDistanceWillBeErasedMain(distanceID,copyBuffer);
	// Remove the object which will be erased from the daten's list
	int i=0;
	while (i<int(daten.size()))
	{
		CGraphData* gr=daten[i];
		if (gr->announceDistanceWillBeErased(distanceID,copyBuffer))
		{ // We have to remove this graphData:
			removeGraphData(gr->getIdentifier());
			i=0; // We start at 0 again
		}
		else
			i++;
	}
}
void CGraph::announceIkObjectWillBeErased(int ikGroupID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceIkObjectWillBeErasedMain(ikGroupID,copyBuffer);
	// Remove the object which will be erased from the daten's list
	int i=0;
	while (i<int(daten.size()))
	{
		CGraphData* gr=daten[i];
		if (gr->announceIkObjectWillBeErased(ikGroupID,copyBuffer))
		{ // We have to remove this graphData:
			removeGraphData(gr->getIdentifier());
			i=0; // We start at 0 again
		}
		else
			i++;
	}
}
void CGraph::announceGcsObjectWillBeErased(int gcsObjectID,bool copyBuffer)
{	// copyBuffer is false by default (if true, we are 'talking' to objects
	// in the copyBuffer)
	announceGcsObjectWillBeErasedMain(gcsObjectID,copyBuffer);
	// Remove the object which will be erased from the daten's list
	int i=0;
	while (i<int(daten.size()))
	{
		CGraphData* gr=daten[i];
		if (gr->announceGcsObjectWillBeErased(gcsObjectID,copyBuffer))
		{ // We have to remove this graphData:
			removeGraphData(gr->getIdentifier());
			i=0; // We start at 0 again
		}
		else
			i++;
	}
}
void CGraph::performObjectLoadingMapping(std::vector<int>* map)
{ // New_Object_ID=map[Old_Object_ID]
	performObjectLoadingMappingMain(map);
	for (int i=0;i<int(daten.size());i++)
		daten[i]->performObjectLoadingMapping(map);
}
void CGraph::performGroupLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performGroupLoadingMappingMain(map);
}
void CGraph::performCollisionLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performCollisionLoadingMappingMain(map);
	for (int i=0;i<int(daten.size());i++)
		daten[i]->performCollisionLoadingMapping(map);
}
void CGraph::performDistanceLoadingMapping(std::vector<int>* map)
{ // If (map[2*i]==Old_Group_ID) then New_Group_ID=map[2*i+1]
	performDistanceLoadingMappingMain(map);
	for (int i=0;i<int(daten.size());i++)
		daten[i]->performDistanceLoadingMapping(map);
}
void CGraph::performIkLoadingMapping(std::vector<int>* map)
{
	performIkLoadingMappingMain(map);
	for (int i=0;i<int(daten.size());i++)
		daten[i]->performIkLoadingMapping(map);
}
void CGraph::performGcsLoadingMapping(std::vector<int>* map)
{
	performGcsLoadingMappingMain(map);
	for (int i=0;i<int(daten.size());i++)
		daten[i]->performGcsLoadingMapping(map);
}

void CGraph::performTextureObjectLoadingMapping(std::vector<int>* map)
{
	performTextureObjectLoadingMappingMain(map);
}

void CGraph::performDynMaterialObjectLoadingMapping(std::vector<int>* map)
{
	performDynMaterialObjectLoadingMappingMain(map);
}

void CGraph::bufferMainDisplayStateVariables()
{
	bufferMainDisplayStateVariablesMain();
}

void CGraph::bufferedMainDisplayStateVariablesToDisplay()
{
	bufferedMainDisplayStateVariablesToDisplayMain();
}

void CGraph::setBufferSize(int buffSize)
{
	tt::limitValue(10,1000000,buffSize);
	bufferSize=buffSize;
	resetGraph();
}
int CGraph::getBufferSize()
{
	return(bufferSize);
}
void CGraph::setCyclic(bool isCyclic)
{
	if (isCyclic!=cyclic)
	{
		cyclic=isCyclic;
		resetGraph();
	}
}
bool CGraph::getCyclic()
{
	return(cyclic);
}

bool CGraph::getData(CGraphData* it,int pos,float& outputValue,bool cyclic,float range,bool doUnitConversion)
{
	float cumulativeValue=0.0f;
	int cumulativeValueCount=0;
	int movingAverageCount=it->getMovingAverageCount();
	for (int i=0;i<movingAverageCount;i++)
	{
		float tmpVal;
		if (it->getValue(pos,tmpVal))
		{
			if (doUnitConversion)
				CGraphingRoutines::adjustDataToUserMetric(it,tmpVal,it->getDerivativeIntegralAndCumulative());
			cumulativeValue+=tmpVal;
			cumulativeValueCount++;
		}
		if ((i==0)&&(cumulativeValueCount==0))
			return(false); // the current point is not valid, we leave here
		if (i!=movingAverageCount-1)
		{ // we are not finished looping!
			if (pos==startingPoint)
				return(false); // we reached the starting pos but didn't finish looping! (not enough values from current point!)
			pos--;
			if (pos<0)
				pos+=bufferSize;
		}
	}

	if (cumulativeValueCount>0)
		outputValue=cumulativeValue/float(cumulativeValueCount);
	return(cumulativeValueCount>0);
}

void CGraph::resetGraph()
{
	numberOfPoints=0;
	startingPoint=0;
	times.reserve(bufferSize);
	times.clear();
	for (int i=0;i<bufferSize;i++)
		times.push_back(0.0f);
	for (int i=0;i<int(daten.size());i++)
		daten[i]->resetData(bufferSize);
}

void CGraph::initializeInitialValues(bool simulationIsRunning)
{ // is called at simulation start, but also after object(s) have been copied into a scene!
	initializeInitialValuesMain(simulationIsRunning);
	_initialValuesInitialized=simulationIsRunning;
	if (simulationIsRunning)
	{
		_initialExplicitHandling=_explicitHandling;
		resetGraph();
	}
}

void CGraph::simulationAboutToStart()
{
	initializeInitialValues(true);
}

void CGraph::simulationEnded()
{ // Remember, this is not guaranteed to be run! (the object can be copied during simulation, and pasted after it ended). For thoses situations there is the initializeInitialValues routine!
	if (_initialValuesInitialized&&App::ct->simulation->getResetSceneAtSimulationEnd()&&((getCumulativeMainPropertyOverride()&sim_modelproperty_not_reset)==0))
	{
		_explicitHandling=_initialExplicitHandling;
	}
	_initialValuesInitialized=false;
	simulationEndedMain();
}

void CGraph::serialize(CSer& ar)
{
	serializeMain(ar);
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Ghg");
		ar << size;
		ar.flush();

		ar.storeDataName("Cl0"); // Colors
		ar.setCountingMode();
		color.serialize(ar);
		if (ar.setWritingMode())
			color.serialize(ar);

		ar.storeDataName("Cl1");
		ar << backgroundColor[0] << backgroundColor[1] << backgroundColor[2];
		ar << textColor[0] << textColor[1] << textColor[2];
		ar.flush();

		ar.storeDataName("Gbv");
		ar << bufferSize << numberOfPoints;
		ar.flush();

		ar.storeDataName("Gtd"); // Should always come after bufferSize!!!
		for (int i=0;i<numberOfPoints;i++)
		{
			int absIndex;
			getAbsIndexOfPosition(i,absIndex);
			ar << times[absIndex];
		}
		ar.flush();

		for (int i=0;i<int(daten.size());i++)
		{
			ar.storeDataName("Ghd");
			ar.setCountingMode();
			daten[i]->serialize(ar,this);
			if (ar.setWritingMode())
				daten[i]->serialize(ar,this);
		}
		for (int i=0;i<int(threeDPartners.size());i++)
		{
			ar.storeDataName("Gh3");
			ar.setCountingMode();
			threeDPartners[i]->serialize(ar);
			if (ar.setWritingMode())
				threeDPartners[i]->serialize(ar);
		}
		for (int i=0;i<int(twoDPartners.size());i++)
		{
			ar.storeDataName("Gh2");
			ar.setCountingMode();
			twoDPartners[i]->serialize(ar);
			if (ar.setWritingMode())
				twoDPartners[i]->serialize(ar);
		}

		for (int i=0;i<int(_staticCurves.size());i++)
		{
			ar.storeDataName("Sta");
			ar.setCountingMode();
			_staticCurves[i]->serialize(ar);
			if (ar.setWritingMode())
				_staticCurves[i]->serialize(ar);
		}

		ar.storeDataName("Gps");
		BYTE nothing=0;
		SIM_SET_CLEAR_BIT(nothing,0,cyclic);
		SIM_SET_CLEAR_BIT(nothing,1,xYZPlanesDisplay);
		SIM_SET_CLEAR_BIT(nothing,2,graphGrid);
		SIM_SET_CLEAR_BIT(nothing,3,graphValues);
		SIM_SET_CLEAR_BIT(nothing,4,_explicitHandling);
		ar << nothing;
		ar.flush();

		ar.storeDataName(SER_END_OF_OBJECT);
	}
	else
	{		// Loading
		startingPoint=0;
		int byteQuantity;
		std::string theName="";
		while (theName.compare(SER_END_OF_OBJECT)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_END_OF_OBJECT)!=0)
			{
				bool noHit=true;
				if (theName.compare("Ghg")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> size;
				}
				if (theName.compare("Cl0")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					color.serialize(ar);
				}
				if (theName.compare("Cl1")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> backgroundColor[0] >> backgroundColor[1] >> backgroundColor[2];
					ar >> textColor[0] >> textColor[1] >> textColor[2];
				}
				if (theName.compare("Gbv")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> bufferSize >> numberOfPoints;
				}
				if (theName.compare("Gtd")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					times.reserve(bufferSize);
					times.clear();
					for (int i=0;i<bufferSize;i++)
						times.push_back(0.0f);
					for (int i=0;i<byteQuantity/int(sizeof(float));i++)
					{
						float aVal;
						ar >> aVal;
						times[i]=aVal;
					}
				}
				if (theName.compare("Ghd")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					CGraphData* it=new CGraphData();
					it->serialize(ar,this);
					daten.push_back(it);
				}
				if (theName.compare("Gh3")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					CGraphDataComb* it=new CGraphDataComb();
					it->serialize(ar);
					threeDPartners.push_back(it);
				}
				if (theName.compare("Gh2")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					CGraphDataComb* it=new CGraphDataComb();
					it->serialize(ar);
					twoDPartners.push_back(it);
				}
				if (theName.compare("Sta")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					CStaticGraphCurve* it=new CStaticGraphCurve();
					it->serialize(ar);
					_staticCurves.push_back(it);
				}
				if (theName=="Gps")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					cyclic=SIM_IS_BIT_SET(nothing,0);
					xYZPlanesDisplay=SIM_IS_BIT_SET(nothing,1);
					graphGrid=SIM_IS_BIT_SET(nothing,2);
					graphValues=SIM_IS_BIT_SET(nothing,3);
					_explicitHandling=SIM_IS_BIT_SET(nothing,4);
				}

				if (noHit)
					ar.loadUnknownData();
			}
		}
		if ((ar.getSerializationVersionThatWroteThisFile()<17)&&App::userSettings->correctColorsAndLights)
		{ // on 29/08/2013 we corrected all default lights. So we need to correct for that change:
			CTTUtil::scaleColorUp_(color.colors);
		}
	}
}

void CGraph::serializeWExtIk(CExtIkSer& ar)
{
	serializeWExtIkMain(ar);
	CDummy::serializeWExtIkStatic(ar);
}

VPoint CGraph::currentWinSize;
