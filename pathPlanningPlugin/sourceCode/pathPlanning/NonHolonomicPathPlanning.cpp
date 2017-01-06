// This file is part of the PATH PLANNING PLUGIN for V-REP
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The PATH PLANNING PLUGIN is licensed under the terms of EITHER:
//   1. PATH PLANNING PLUGIN commercial license (contact us for details)
//   2. PATH PLANNING PLUGIN educational license (see below)
// 
// PATH PLANNING PLUGIN educational license:
// -------------------------------------------------------------------
// The PATH PLANNING PLUGIN educational license applies only to EDUCATIONAL
// ENTITIES composed by following people and institutions:
// 
// 1. Hobbyists, students, teachers and professors
// 2. Schools and universities
// 
// EDUCATIONAL ENTITIES do NOT include companies, research institutions,
// non-profit organisations, foundations, etc.
// 
// An EDUCATIONAL ENTITY may use, modify, compile and distribute the
// modified/unmodified PATH PLANNING PLUGIN under following conditions:
//  
// 1. Distribution should be free of charge.
// 2. Distribution should be to EDUCATIONAL ENTITIES only.
// 3. Usage should be non-commercial.
// 4. Altered source versions must be plainly marked as such and distributed
//    along with any compiled code.
// 5. When using the PATH PLANNING PLUGIN in conjunction with V-REP, the "EDU"
//    watermark in the V-REP scene view should not be removed.
// 6. The origin of the PATH PLANNING PLUGIN must not be misrepresented. you must
//    not claim that you wrote the original software.
// 
// THE PATH PLANNING PLUGIN IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR
// IMPLIED WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.1.2 on June 16th 2014

#include <stdio.h>
#include "NonHolonomicPathPlanning.h"
#include "pathPlanningInterface.h"
#include "v_repLib.h"


#define SIM_MIN(a,b) (((a)<(b)) ? (a) : (b))
#define SIM_MAX(a,b) (((a)>(b)) ? (a) : (b))

std::vector<CNonHolonomicPathNode*> CNonHolonomicPathPlanning::tree;
double CDubinPath::rho_;
CNonHolonomicPathPlanning::CNonHolonomicPathPlanning(int theStartDummyID,int theGoalDummyID,
						int theRobotCollectionID,int theObstacleCollectionID,int ikGroupID,float theAngularCoeff,
						float theSteeringAngleCoeff,float theMaxSteeringAngleVariation,float theMaxSteeringAngle,
						float theStepSize,const float theSearchMinVal[2],const float theSearchRange[2],
						const int theDirectionConstraints[2],const float clearanceAndMaxDistance[2])
{

	
	/*printf("theStartDummyID : %d\n",theStartDummyID);
	printf("theGoalDummyID : %d\n",theGoalDummyID);
	printf("theRobotCollectionID : %d\n",theRobotCollectionID);
	printf("theObstacleCollectionID : %d\n",theObstacleCollectionID);
	printf("ikGroupID : %d\n",ikGroupID);
	printf("theAngularCoeff : %f\n",theAngularCoeff);
	printf("theSteeringAngleCoeff : %f\n",theSteeringAngleCoeff);
	printf("theMaxSteeringAngleVariation : %f\n",theMaxSteeringAngleVariation);
	printf("theMaxSteeringAngle : %f\n",theMaxSteeringAngle);
	printf("theStepSize : %f\n",theStepSize);
	printf("theSearchMinVal : %f, %f\n",theSearchMinVal[0],theSearchMinVal[1]);
	printf("theSearchRange : %f, %f\n",theSearchRange[0],theSearchRange[1]);
	printf("theDirectionConstraints : %d, %d\n",theDirectionConstraints[0],theDirectionConstraints[1]);
	printf("clearanceAndMaxDistance : %f, %f\n",clearanceAndMaxDistance[0],clearanceAndMaxDistance[1]);*/

	isHolonomic=false;
	_allIsObstacle=(theObstacleCollectionID==-1);
	firstPass=true;
	invalidData=true;
	obstacleClearanceAndMaxDistance[0]=clearanceAndMaxDistance[0];
	obstacleClearanceAndMaxDistance[1]=clearanceAndMaxDistance[1];

	//printf("first tree size: %d\n",tree.size());
	tree.reserve(300000);
	stepSize=theStepSize;
	angularCoeff=theAngularCoeff;
	steeringAngleCoeff=theSteeringAngleCoeff;
	maxSteeringAngleVariation=theMaxSteeringAngleVariation;
	maxSteeringAngle=theMaxSteeringAngle;
	//minTurningRadius=stepSize/(2.0f*sin(maxSteeringAngle/2.0f));
	minTurningRadius = 0.2 / theMaxSteeringAngle;
	CDubinPath::rho_ = 0.2 / theMaxSteeringAngle;
	//printf("dubin rho_(minTurningRadius) : %f\n",CDubinPath::rho_);
	_startDummyID=theStartDummyID;
	startDummy=(CDummyDummy*)_simGetObject(_startDummyID);
	goalDummy=(CDummyDummy*)_simGetObject(theGoalDummyID);
	
	if ( (startDummy==NULL)||(goalDummy==NULL) )
		return;
	_simGetObjectCumulativeTransformation(startDummy,_startDummyCTM.X.data,_startDummyCTM.Q.data,false);
	_simGetObjectLocalTransformation(startDummy,_startDummyLTM.X.data,_startDummyLTM.Q.data,false);
    /*printf("_startDummyCTM.X : %f %f %f\n",_startDummyCTM.X.data[0],_startDummyCTM.X.data[1],_startDummyCTM.X.data[2]);
    printf("_startDummyCTM.Q : %f %f %f %f\n",_startDummyCTM.Q.data[0],_startDummyCTM.Q.data[1],_startDummyCTM.Q.data[2],_startDummyCTM.Q.data[3]);
    
    printf("_startDummyLTM.X : %f %f %f\n",_startDummyLTM.X.data[0],_startDummyLTM.X.data[1],_startDummyLTM.X.data[2]);
    printf("_startDummyLTM.Q : %f %f %f %f\n",_startDummyLTM.Q.data[0],_startDummyLTM.Q.data[1],_startDummyLTM.Q.data[2],_startDummyLTM.Q.data[3]);
    */
    //Q is maybe Quaternion
    
    
    C7Vector startEuler(_startDummyLTM.Q.getEulerAngles());
    //printf("start euler %f\n",startEuler(2)/piValue*180);
    //tmpEuler(2) = Target Degree
    
	C7Vector goalTr;
	_simGetObjectCumulativeTransformation(goalDummy,goalTr.X.data,goalTr.Q.data,false);
    //printf("_goalDummyTr.X : %f %f %f\n",goalTr.X.data[0],goalTr.X.data[1],goalTr.X.data[2]);
    //printf("_goalDummyTr.Q : %f %f %f %f\n",goalTr.Q.data[0],goalTr.Q.data[1],goalTr.Q.data[2],goalTr.Q.data[3]);
    C7Vector goalEuler(goalTr.Q.getEulerAngles());
    //printf("goal euler %f\n",goalEuler(2)/piValue*180);


	
	
    //printf("sx:%f sy:%f st:%f\n",rootNode->values[0],rootNode->values[1],rootNode->values[2]);
	//printf("fx:%f fy:%f ft:%f\n",goalNode->values[0],goalNode->values[1],goalNode->values[2]);
	//printf("goal length: %f\n",goalNode->length_from_root);
    
    
	robotCollectionID=theRobotCollectionID;
	obstacleCollectionID=theObstacleCollectionID;
    
    // Following sets the _startConfInterferenceState to SIM_MAX_FLOAT if not colliding or above distance threshold.
	// Otherwise it is set to 0.0 (colliding) or the distance with the obstacle
	// This is used to allow path planning also when in an initial collision state. In that case the initial path segment
	// will follow a distance to obstacle gradient that is increasing until above the collision distance
	doCollide(&_startConfInterferenceState);

	for (int i=0;i<2;i++)
	{
		searchMinVal[i]=theSearchMinVal[i];
		searchRange[i]=theSearchRange[i];
		directionConstraints[i]=theDirectionConstraints[i];
	}
	buffer[0]=-1;
	invalidData=false;


	/*if(tree.size() != 0)
	{//When object is moved, clear data
		if(!(tree[0]->values[0] == (float)_startDummyCTM.X.data[0] && tree[0]->values[1] == (float)_startDummyCTM.X.data[1] && tree[0]->values[2] == (float)startEuler(2)))
		{
			printf("Clear previous data\n");
			for (int i=0;i<int(tree.size());i++)
				delete tree[i];
			tree.clear();
		}
	}*/

	rootNode = new CNonHolonomicPathNode((float)_startDummyCTM.X.data[0],(float)_startDummyCTM.X.data[1],(float)startEuler(2));
    goalNode = new CNonHolonomicPathNode((float)goalTr.X.data[0],(float)goalTr.X.data[1],(float)goalEuler(2));
	goalNode->length_from_root = SIM_MAX_FLOAT;

	if(tree.size()==0)
	{	    
    	tree.push_back(rootNode);
		pathWasFound = false;
	}
	else
		Recalculation();	
}

CNonHolonomicPathPlanning::~CNonHolonomicPathPlanning()
{
	//for (int i=0;i<int(tree.size());i++)
	{
		//printf("%f\t%f\t%f\n",tree[i]->values[0],tree[i]->values[1],tree[i]->values[2]);
		//delete tree[i];
	}
	//tree.clear();
	
	tree.push_back(goalNode);
	
	for (int i=0;i<int(foundPath.size());i++)
		delete foundPath[i];
	foundPath.clear();

	//printf("~CNonHolonomicPathPlanning()\n");
}

void CNonHolonomicPathPlanning::FindNearestPath(CNonHolonomicPathNode *prevGoalNode)
{
	CNonHolonomicPathNode *iter = prevGoalNode;
	CNonHolonomicPathNode *second;
	CDubinPath oldPath;
	double x0,y0,t0;
	double x1,y1,t1;
	double x2,y2,t2;
	double result[3];
	double min_dist = 1e+10;
	double mx,my,mt;
	x0 = rootNode->values[0];
	y0 = rootNode->values[1];
	t0 = rootNode->values[2];
	while(iter->parent != NULL)
	{
		x1 = iter->parent->values[0];
		y1 = iter->parent->values[1];
		t1 = iter->parent->values[2];

		x2 = iter->values[0];
		y2 = iter->values[1];
		t2 = iter->values[2];

		CDubinPath tmpPath(x1,y1,t1,x2,y2,t2);
		
		for(double dist=0.0,slice;(slice = dist / tmpPath.length())<=1;dist+=0.05)
		{
			tmpPath.interpolate(slice,result);
			if(min_dist > (x0 - result[0])*(x0 - result[0]) + (y0 - result[1])*(y0 - result[1]))
			{
				min_dist = (x0 - result[0])*(x0 - result[0]) + (y0 - result[1])*(y0 - result[1]);
				mx = result[0];
				my = result[1];
				mt = result[2];
			}
		}
		second = iter;
		iter = iter->parent;
		if(iter->parent == NULL)
			oldPath = tmpPath;
	}
	CDubinPath newPath(x0,y0,t0,second->values[0],second->values[1],second->values[2]);
	if(newPath.length() > oldPath.length())
	{
		rootNode->values[0] = mx;
		rootNode->values[1] = my;
		rootNode->values[2] = mt;
	}
}


void CNonHolonomicPathPlanning::Recalculation()
{
	double x1,y1,t1;
	double x2,y2,t2;
	CNonHolonomicPathNode *prevRootNode = tree[0];
	CNonHolonomicPathNode *prevGoalNode = tree.back();	

	

	if(prevGoalNode->parent)
	{
		tree.pop_back();		
		
		std::vector<CNonHolonomicPathNode*> tmp_tree;
		tmp_tree.reserve(1000);

		C7Vector dumSavedConf;
	    _simGetObjectLocalTransformation(startDummy,dumSavedConf.X.data,dumSavedConf.Q.data,false);
		
		// Goal Reconnect
		CNonHolonomicPathNode *iter = prevGoalNode;

		printf("from %f\t%f\t%f\n",rootNode->values[0],rootNode->values[1],rootNode->values[2]);
		FindNearestPath(prevGoalNode);
		printf("  to %f\t%f\t%f\n",rootNode->values[0],rootNode->values[1],rootNode->values[2]);	
		
		x1 = rootNode->values[0];
		y1 = rootNode->values[1];
		t1 = rootNode->values[2];

		while(iter->parent != NULL)
		{
			x2 = iter->values[0];
			y2 = iter->values[1];
			t2 = iter->values[2];
			//printf("%f\t%f\t%f\n",x2,y2,t2);
			tmp_tree.insert(tmp_tree.begin(),new CNonHolonomicPathNode(iter));
			
			CDubinPath tmpPath(x1,y1,t1,x2,y2,t2);
			if( checkCollisionFree(&tmpPath) )
			{
				iter->parent = rootNode;
				iter->length_from_parent = 
				iter->length_from_root = tmpPath.length();
				tmp_tree.insert(tmp_tree.begin(),rootNode);
				break;
			}
			iter = iter->parent;
		}

		_simSetObjectLocalTransformation(startDummy,dumSavedConf.X.data,dumSavedConf.Q.data);
		
		
		delete prevGoalNode;
		for (int i=0;i<int(tree.size());i++)
			delete tree[i];
		tree.clear();

		if(iter->parent != NULL)
		{		
			pathWasFound = 1;
			for (int i=1;i<int(tmp_tree.size());i++)
			{
				tmp_tree[i]->parent = tmp_tree[i-1];
				tmp_tree[i]->length_from_root = tmp_tree[i-1]->length_from_root + tmp_tree[i]->length_from_parent;
				//printf("%f 0x%x\n",tmp_tree[i]->length_from_root,(unsigned long)tmp_tree[i]->parent);
			}
			for (int i=0;i<int(tmp_tree.size());i++)
				tree.push_back(tmp_tree[i]);
			//printf("new tree size %d\n",tree.size());
			tmp_tree.clear();

			delete goalNode;
			goalNode = tree.back();
			tree.pop_back();
		}
		else
		{
			tree.push_back(rootNode);
			pathWasFound = false;
		}
			
		//printf("0x%x\n",(unsigned long)goalNode->parent);
	}
	else
	{
		for (int i=0;i<int(tree.size());i++)
			delete tree[i];
		tree.clear();
		tree.push_back(rootNode);
		pathWasFound = false;
	}
}

void CNonHolonomicPathPlanning::setStepSize(float size)
{
	stepSize=size;
}

void CNonHolonomicPathPlanning::getSearchTreeData(std::vector<float>& data,bool fromTheStart)
{
	//printf("getSearchTreeData\n");
	
}

int CNonHolonomicPathPlanning::expandTree()
{
	
    CNonHolonomicPathNode* randNode;
	
	randNode = new CNonHolonomicPathNode(searchMinVal[0]+searchRange[0]*SIM_RAND_FLOAT,
    	                			      searchMinVal[1]+searchRange[1]*SIM_RAND_FLOAT,
        				                  maxSteeringAngle*2*SIM_RAND_FLOAT - maxSteeringAngle);
    CNonHolonomicPathNode* closest=NULL;
    
    closest=getClosestNode(tree,randNode);

	pullNode(closest,randNode);
	
    extend(closest,randNode);   // Extend Tree toword Rand Node

	delete randNode;
    return 0;
}

int CNonHolonomicPathPlanning::searchPath(int maxTimePerPass)
{ // maxTimePerPass is in miliseconds

    if (invalidData)
        return(0);    
    
    // Update the current vehicle states x0 and environmental constraints X_free(t)
    // Propagate the states by the computation time and obtain x(t_0+t_delta)
    if (startDummy==NULL)
        return(0);

	
    C7Vector dumSavedConf;
    _simGetObjectLocalTransformation(startDummy,dumSavedConf.X.data,dumSavedConf.Q.data,false);
	
    int initTime=simGetSystemTimeInMs(-1);
    while (_simGetTimeDiffInMs(initTime)<maxTimePerPass)
    {
        expandTree();
    }	
	_simSetObjectLocalTransformation(startDummy,dumSavedConf.X.data,dumSavedConf.Q.data);
	
	printf("tree size %d\n",(int)CNonHolonomicPathPlanning::tree.size());
	
    return pathWasFound;
    //return 0;
}
CShootingPath CNonHolonomicPathPlanning::Steer(CNonHolonomicPathNode* closest,CNonHolonomicPathNode* randNode)
{
	
	double min_r = minTurningRadius,max_r=1e+5;
	double x1 = closest->values[0], y1 = closest->values[1], theta1 = closest->values[2];
	double x2 = randNode->values[0],y2 = randNode->values[1];


	//printf("here\n");
	CShootingPath retPath(theta1,x1,y1,x2,y2);

	randNode->values[0] = retPath.x2;
	randNode->values[1] = retPath.y2;
	randNode->values[2] = retPath.t2;

	return retPath;
}

void CNonHolonomicPathPlanning::pullNode(CNonHolonomicPathNode* closest,CNonHolonomicPathNode* randNode)
{
	double dist = (closest->values[0] - randNode->values[0])*(closest->values[0] - randNode->values[0]) + (closest->values[1] - randNode->values[1])*(closest->values[1] - randNode->values[1]);
	double max_dist = 1.5*1.5;
	if(dist < max_dist)	return;

	double theta = randNode->values[2];
	randNode->values[0] = closest->values[0] + max_dist * cos(theta);
	randNode->values[1] = closest->values[1] + max_dist * sin(theta);
}


bool CNonHolonomicPathPlanning::setPartialPath()
{	
	return pathWasFound;
}

void CNonHolonomicPathPlanning::getNearNodes(std::vector<CNonHolonomicPathNode*>& nears,std::vector<CNonHolonomicPathNode*>& nodes,CNonHolonomicPathNode* sample)
{
	float threshold=50;
	
	for(int i=0;i<int(nodes.size());i++)
	{
		float vect[2];
		vect[0]=sample->values[0]-nodes[i]->values[0];
		vect[1]=sample->values[1]-nodes[i]->values[1];
		//vect[2]=nodes[i]->values[2] - atan2(vect[1],vect[0]);
		float d = vect[0]*vect[0]+vect[1]*vect[1];
		if(d < threshold)
		{
			nears.push_back(nodes[i]);
		}
	}
}


CNonHolonomicPathNode* CNonHolonomicPathPlanning::getClosestNode(std::vector<CNonHolonomicPathNode*>& nodes,CNonHolonomicPathNode* sample)
{
	float minD=SIM_MAX_FLOAT;
	int index=-1;
	const float alpha=0.1;
	
	for (int i=0;i<int(nodes.size());i++)
	{
        float vect[3];
		vect[0]=sample->values[0]-nodes[i]->values[0];
		vect[1]=sample->values[1]-nodes[i]->values[1];
		//vect[2]=atan2(vect[1],vect[0]) - nodes[i]->values[2];
		//vect[2]=sample->values[2]-nodes[i]->values[2];
		CDubinPath testPath(nodes[i]->values[0],nodes[i]->values[1],nodes[i]->values[2],sample->values[0],sample->values[1],atan2(vect[1],vect[0]));
		//CDubinPath testPath(nodes[i]->values[0],nodes[i]->values[1],nodes[i]->values[2],sample->values[0],sample->values[1],sample->values[2]);
		float d = testPath.length();
		//float d = vect[0]*vect[0]+vect[1]*vect[1]+alpha*vect[2]*vect[2];
		if (d<minD)
		{
			minD=d;
			index=i;
		}
	}
	if (index!=-1)
	{
		sample->values[2] = atan2(sample->values[1]-nodes[index]->values[1],sample->values[0]-nodes[index]->values[0]);
		//printf("start %f %f %f\n",nodes[index]->values[0],nodes[index]->values[1],nodes[index]->values[2]/piValue*180);
		//printf("finis %f %f %f\n\n",sample->values[0],sample->values[1],sample->values[2]/piValue*180);
		return(nodes[index]);
	}
	return(NULL);
}

void CNonHolonomicPathPlanning::ReWire(std::vector<CNonHolonomicPathNode*>& nears,CNonHolonomicPathNode* toBeExtended)
{
	float x1,y1,t1;
	float x2,y2,t2;

	x1 = toBeExtended->values[0];
	y1 = toBeExtended->values[1];
	t1 = toBeExtended->values[2];
	for(int i=0;i<int(nears.size());i++)
	{
		x2 = nears[i]->values[0];
		y2 = nears[i]->values[1];
		t2 = nears[i]->values[2];
		
		CDubinPath temp(x1,y1,t1,x2,y2,t2);
		if(checkCollisionFree(&temp) && (nears[i]->length_from_root > toBeExtended->length_from_root + temp.length()))
		{
			nears[i]->length_from_parent = temp.length();
			nears[i]->length_from_root = toBeExtended->length_from_root + nears[i]->length_from_parent;
			nears[i]->parent = toBeExtended;
			if(nears[i] == goalNode)
			{
				pathWasFound = true;
				printf("pathWasFound goal length: %f\n",goalNode->length_from_root);
				//printf("pathWasFound\n");
			}
		}
	}
}

void CNonHolonomicPathPlanning::ChooseParent(std::vector<CNonHolonomicPathNode*>& nears, CNonHolonomicPathNode* extention)
{
	//float minD=SIM_MAX_FLOAT;
	float x1,y1,t1;
	float x2,y2,t2;
	x2 = extention->values[0];
	y2 = extention->values[1];
	t2 = extention->values[2];
	for(int i=0;i<int(nears.size());i++)
	{
		if(extention->parent == nears[i])	continue;
		x1 = nears[i]->values[0];
		y1 = nears[i]->values[1];
		t1 = nears[i]->values[2];

		CDubinPath temp(x1,y1,t1,x2,y2,t2);
		if(checkCollisionFree(&temp) && (extention->length_from_root > nears[i]->length_from_root + temp.length()))
		{
			extention->length_from_parent = temp.length();
			extention->length_from_root = nears[i]->length_from_root + extention->length_from_parent;
			extention->parent = nears[i];
		}
	}
}

//extend(current,closest,randNode,i==0,startDummy)
void CNonHolonomicPathPlanning::extend(CNonHolonomicPathNode* toBeExtended,CNonHolonomicPathNode* extention)
{	
	CDubinPath newPath(toBeExtended->values[0],toBeExtended->values[1],toBeExtended->values[2],extention->values[0],extention->values[1],extention->values[2]);
	
	
	if(checkCollisionFree(&newPath) == false)	return;
	
	CNonHolonomicPathNode* newNode=new CNonHolonomicPathNode(extention);
	newNode->parent=toBeExtended;
	newNode->length_from_parent = newPath.length();
	newNode->length_from_root=toBeExtended->length_from_root + newNode->length_from_parent;
	toBeExtended=newNode;
	
	std::vector<CNonHolonomicPathNode*> nears;
	getNearNodes(nears,tree,toBeExtended);
	ChooseParent(nears,toBeExtended);
	// Try to connect Goal node
	nears.push_back(goalNode);	
	ReWire(nears,toBeExtended);
	nears.clear();
	tree.push_back(toBeExtended);
}

//CNonHolonomicPathNode* CNonHolonomicPathPlanning::connect(std::vector<CNonHolonomicPathNode*>* currentList,std::vector<CNonHolonomicPathNode*>* nextList,CNonHolonomicPathNode* toBeExtended,CNonHolonomicPathNode* extention,bool forward,bool connect,CDummyDummy* startDummy)
//{
//	return NULL;
//}


int CNonHolonomicPathPlanning::smoothFoundPath(int steps,int maxTimePerPass)
{ 
	char test_test[100]="0.0";
	int mantaHandle = simGetObjectHandle("Manta");
    simWriteCustomDataBlock(mantaHandle,"cusdata",test_test,sizeof(test_test));
	return(1);
}


void CNonHolonomicPathPlanning::getPathData(std::vector<float>& data)
{
    float send_euler;
	data.clear();
	if (invalidData)
		return;
	if (pathWasFound == false)
		return;
	printf("getPathData %d\n",pathWasFound);

	//printf("goal length: %f\n",goalNode->length_from_root);
	
	CNonHolonomicPathNode *iterat=goalNode;	
	while(iterat->parent != NULL)
	{
		foundPath.insert(foundPath.begin(),new CNonHolonomicPathNode(iterat));
		iterat=iterat->parent;
		//printf("%f\t%f\t%f\n",iterat->values[0],iterat->values[1],iterat->values[2]/piValue*180);
	}
	foundPath.insert(foundPath.begin(),new CNonHolonomicPathNode(iterat));

	CNonHolonomicPathNode* prevNode;
	CNonHolonomicPathNode* nextNode;
	double result[3];
	double slice,dist;
	C3Vector p,euler;
	for (int i=1;i<int(foundPath.size());i++)
	{
		prevNode = foundPath[i-1];
		nextNode = foundPath[i];
		CDubinPath tmpPath(prevNode->values[0],prevNode->values[1],prevNode->values[2],
						   nextNode->values[0],nextNode->values[1],nextNode->values[2]);
		//if(!checkCollisionFree(tmpPath))
		//	printf("Collision %f\t%f\t%f\t%f\t%f\t%f\n",prevNode->values[0],prevNode->values[1],prevNode->values[2],nextNode->values[0],nextNode->values[1],nextNode->values[2]);
		for(dist=0.1;(slice = dist / tmpPath.length())<1;dist+=0.1)
		{
			tmpPath.interpolate(slice,result);
			
			p.clear();
			euler.clear();
			p(0)=result[0];
			p(1)=result[1];
			p(2)=_startDummyCTM.X.data[2];
			euler(2)=result[2];
			C7Vector conf(C4Vector(euler),p);
			
			data.push_back(conf(0));
			data.push_back(conf(1));
			data.push_back(conf(2));
			data.push_back(conf(3));
			data.push_back(conf(4));
			data.push_back(conf(5));
			data.push_back(conf(6));	        
		}
        
	}

	C7Vector startVector(_startDummyLTM.Q.getEulerAngles());
	double startEuler = startVector(2);
	double pathEuler = startVector(2);	//\C3ʱⰪ

	double next_dist = 0.35;	
	int idx=0;
	CDubinPath *tmpPath;
	while(1)
	{
		if(idx + 1 == foundPath.size())	break;
		tmpPath = new CDubinPath(foundPath[idx  ]->values[0],foundPath[idx  ]->values[1],foundPath[idx  ]->values[2],
						   		 foundPath[idx+1]->values[0],foundPath[idx+1]->values[1],foundPath[idx+1]->values[2]);
		//printf("tmpPath Length : %f\n",tmpPath->length());
		if(tmpPath->length() > next_dist)
		{
			tmpPath->interpolate(fmin(next_dist/tmpPath->length(),(double)1.f),result);
			pathEuler = result[2];

			break;
		}
		next_dist -= tmpPath->length();
		idx++;
		delete tmpPath;
	}
	printf("%d\t%f\t%f  %d %d %d %d %f\n",idx,startEuler/piValue*180,pathEuler/piValue*180,foundPath.size(), tmpPath->type[0],tmpPath->type[1],tmpPath->type[2],tmpPath->length());
	printf("%f %f %f\n",tmpPath->value[0],tmpPath->value[1],tmpPath->value[2]);
	//printf("%f %f %f\t %f %f %f\n",foundPath[idx]->values[0],foundPath[idx]->values[1],foundPath[idx]->values[2],foundPath[idx+1]->values[0],foundPath[idx+1]->values[1],foundPath[idx+1]->values[2]);
	delete tmpPath;

	double origin_x = _startDummyCTM.X.data[0];
	double origin_y = _startDummyCTM.X.data[1];


    send_euler = fmod((pathEuler - startEuler)+piValue,piValTimes2)-piValue;// / piValue * 180;
	char test_test[100];
	sprintf(test_test,"%.3f",send_euler);
    //printf("%f\n",send_euler/piValue*180);
    
    int mantaHandle = simGetObjectHandle("Manta");
    simWriteCustomDataBlock(mantaHandle,"cusdata",test_test,sizeof(test_test));
}

bool CNonHolonomicPathPlanning::checkCollisionFree(CShootingPath* path)
{

	double result[3];
	double dist;
	double slice;
	double x,y,t;

	for(dist=0.1;(slice = dist / path->length())<=1;dist+=0.1)
	{
		path->interpolate(slice,result);

		x = result[0],y = result[1],t = result[2];
		
		if (!( (x>searchMinVal[0])&&(x<searchMinVal[0]+searchRange[0])&&
				(y>searchMinVal[1])&&(y<searchMinVal[1]+searchRange[1]) ))
			return false;
	
		C3Vector trEuler(0.0f,0.0f,t);
		C7Vector tr(C4Vector(trEuler),C3Vector(0.0f,0.0f,0.0f));
		tr.X(0)=x;
		tr.X(1)=y;
		tr.X(2)=_startDummyCTM.X.data[2];
		_simSetObjectLocalTransformation(startDummy,tr.X.data,tr.Q.data);

		if (doCollide(NULL))
		{
			return false;
		}
	}
	return true;
}


bool CNonHolonomicPathPlanning::checkCollisionFree(CDubinPath* path)
{

	double result[3];
	double dist;
	double slice;
	double x,y,t;

	for(dist=0.1;(slice = dist / path->length())<=1;dist+=0.1)
	{
		path->interpolate(slice,result);

		x = result[0],y = result[1],t = result[2];
		
		if (!( (x>searchMinVal[0])&&(x<searchMinVal[0]+searchRange[0])&&
				(y>searchMinVal[1])&&(y<searchMinVal[1]+searchRange[1]) ))
			return false;
	
		C3Vector trEuler(0.0f,0.0f,t);
		C7Vector tr(C4Vector(trEuler),C3Vector(0.0f,0.0f,0.0f));
		tr.X(0)=x;
		tr.X(1)=y;
		tr.X(2)=_startDummyCTM.X.data[2];
		_simSetObjectLocalTransformation(startDummy,tr.X.data,tr.Q.data);

		if (doCollide(NULL))
		{
			return false;
		}
	}
	return true;
}

bool CNonHolonomicPathPlanning::doCollide(float* dist)
{// dist can be NULL. Dist returns the actual distance only when return value is true!! otherwise it is SIM_MAX_FLOAT!!
	//printf("doCollide %f\n",dist);
	if (dist!=NULL)
		dist[0]=SIM_MAX_FLOAT;
	if (obstacleClearanceAndMaxDistance[0]<=0.0f)
	{
		if ( (obstacleCollectionID==-1)&&(!_allIsObstacle) )
			return(false);
		if (_simDoEntitiesCollide(robotCollectionID,obstacleCollectionID,buffer,false,false,true)!=0)
		{
			if (dist!=NULL)
				dist[0]=0.0f;
			return(true);
		}
		return(false);
	}
	else
	{
		if ( (obstacleCollectionID==-1)&&(!_allIsObstacle) )
			return(false);
		if (obstacleClearanceAndMaxDistance[1]<=0.0f)
		{ // no max distance, only min. distance:
			float ray[7];
			float theDist=obstacleClearanceAndMaxDistance[0];
			if (_simGetDistanceBetweenEntitiesIfSmaller(robotCollectionID,obstacleCollectionID,&theDist,ray,buffer,false,false,true)!=0)
			{
				if (dist!=NULL)
					dist[0]=theDist;
				return(true);
			}
			return(false);
		}
		else
		{ // min. distance and max. distance:
			float ray[7];
			float theDist=obstacleClearanceAndMaxDistance[1];
			if (_simGetDistanceBetweenEntitiesIfSmaller(robotCollectionID,obstacleCollectionID,&theDist,ray,buffer,false,false,true)!=0)
			{
				if (theDist>=obstacleClearanceAndMaxDistance[0])
					return(false);
				if (dist!=NULL)
					dist[0]=theDist;
				return(true);
			}
			if (dist!=NULL)
				dist[0]=theDist;
			return(true);
		}
	}
}

