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
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#pragma once

#include "PathPlanning.h"
#include "HolonomicPathNode.h"
#include "dummyClasses.h"
#include "MeasurementRegion.h"
#include "Obstacles.h"
#include "Beacon.h"
#include "Robot.h"
#include <vector>
#include "4Vector.h"
#include "7Vector.h"

#include "ompl/datastructures/NearestNeighborsLinear.h"

#include "RVO.h"

#define MPTYPE_DEFAULT      0
#define MPTYPE_RRBT         1
#define MPTYPE_RRBT_DYNAMIC 2

class CBelief;
class CHolonomicPathPlanning : public CPathPlanning  
{
public:
	CHolonomicPathPlanning(int theStartDummyID,int theGoalDummyID,
							int theRobotCollectionID,int theObstacleCollectionID,int ikGroupID,
							int thePlanningType,float theAngularCoeff,
							float theStepSize,
							const float theSearchMinVal[4],const float theSearchRange[4],
							const int theDirectionConstraints[4],const float clearanceAndMaxDistance[2],const C3Vector& gammaAxis);
	virtual ~CHolonomicPathPlanning();


	// Following functions are inherited from CPathPlanning:
	int searchPath(int maxTimePerPass);
	bool setPartialPath();
	int smoothFoundPath(int steps,int maxTimePerPass);
	void getPathData(std::vector<float>& data);

	void getSearchTreeData(std::vector<float>& data,bool fromStart);

	void setAngularCoefficient(float coeff);
	void setStepSize(float size);

	std::vector<CHolonomicPathNode*> fromStart;
	std::vector<CHolonomicPathNode*> fromGoal;
    std::vector<CHolonomicPathNode*> foundPath;

    boost::shared_ptr<ompl::NearestNeighborsLinear<CHolonomicPathNode*> > ompnn;

    // Insert YHC RRBT functions
    bool YHC_RRBT_nodetest(CBelief *belief);
    bool YHC_RRBT_nodetest_recur(CBelief *belief);
    bool YHC_RRBT_DeleteNode(CHolonomicPathNode *tobeDeleted,CBelief *exceptBef);
    bool YHC_RRBT_DeleteBelief(CBelief *tobeDeleted,CBelief *exceptBef);
    bool YHC_RRBT_DeleteChildBelief(CBelief *tobeDeleted,CBelief *exceptBef);
    void YHC_RRBT_CompactInitializeTree(CHolonomicPathNode *startNode,CHolonomicPathNode *goalNode);
    void YHC_RRBT_InitializeTree(CHolonomicPathNode *startNode,CHolonomicPathNode *goalNode);
    int YHC_RRBT_ParentTest(CHolonomicPathNode* testNode);
    void YHC_RRBT_BoundPrune(CBelief *sb,std::vector<CBelief*>* goalPathBelief,int goalDepth);
    bool YHC_RRBT_BranchBound(std::vector<CBelief*>* goalPathBelief);
    int YHC_RRBT_searchPath(int maxTimePerPass);
    bool YHC_RRBT_propagate(CHolonomicPathNode* toBeExtended,CHolonomicPathNode* extention,CBelief *n,CBelief *new_n,bool checkCollision=1);
    bool YHC_RRBT_VOCheck(CHolonomicPathNode* toBeExtended,CHolonomicPathNode* extention,CBelief *n,CBelief *new_n);
    bool YHC_RRBT_repropagate(CBelief *belief);
    void YHC_RRBT_changeBelief(CBelief *current, CBelief *rep);
    void YHC_RRBT_getSearchTreeData(std::vector<float>& data,bool fromStart);
    void YHC_RRBT_getPathData(std::vector<float>& data);
    void YHC_RRBT_ClearSavedTree();
    bool use_vocheck;
    // YHC MT-RRT Function

    // YHC common functions
    void YHC_pullNode(CHolonomicPathNode* closest,CHolonomicPathNode* randNode,float rn);    
    float YHC_nearRange(void);

    float distance(CHolonomicPathNode* from, CHolonomicPathNode* to);

private:
	bool doCollide(float* dist);

	CHolonomicPathNode* getClosestNode(std::vector<CHolonomicPathNode*>& nodes,CHolonomicPathNode* sample);
	CHolonomicPathNode* extend(std::vector<CHolonomicPathNode*>* nodeList,CHolonomicPathNode* toBeExtended,CHolonomicPathNode* extention,bool connect,CDummyDummy* dummy);
	int getVector(CHolonomicPathNode* fromPoint,CHolonomicPathNode* toPoint,float vect[7],float e,float& artificialLength,bool dontDivide);
	bool addVector(C3Vector& pos,C4Vector& orient,float vect[7]);
	bool areDirectionConstraintsRespected(float vect[7]);
	bool areSomeValuesForbidden(float values[7]);


    int custom_function;   // 0 Default, 1 RRBT, 2 MT-RRT
    int numoffail;
	int startDummyID;
	int goalDummyID;
	int planningType;
	float angularCoeff;
	float stepSize;
	float _searchMinVal[4];
	float _searchRange[4];
    float initialUncertainty;
    float prefSpeed;
	int _directionConstraints[4];
	bool _directionConstraintsOn;

	C4Vector _gammaAxisRotation;
	C4Vector _gammaAxisRotationInv;

	float _startConfInterferenceState;

	C7Vector _startDummyCTM;
	C7Vector _startDummyLTM;

	int numberOfRandomConnectionTries_forSteppedSmoothing;
	int numberOfRandomConnectionTriesLeft_forSteppedSmoothing;
	std::vector<int> foundPathSameStraightLineID_forSteppedSmoothing;
	int sameStraightLineNextID_forSteppedSmoothing;
	int nextIteration_forSteppedSmoothing;

    CRobot robot;
    CBeacon beacon;
    CMeasurementRegion measurement;
    CObstacles obstacles;
    RVO::RVOSimulator *simRVO;
    CBelief *minGoalBelief;
    clock_t startClock;
    int sampleCount;
public:
    static int maxBeliefNode;
};
