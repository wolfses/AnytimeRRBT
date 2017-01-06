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

#include "HolonomicPathPlanning.h"
#include "pathPlanningInterface.h"
#include "v_repLib.h"
#include "Obstacles.h"
#include "measurementregion.h"
#include "StaticData.h"
#include "Belief.h"
#include <Eigen/Dense>


#include <iostream>

#define SIM_MIN(a,b) (((a)<(b)) ? (a) : (b))
#define SIM_MAX(a,b) (((a)>(b)) ? (a) : (b))

int CHolonomicPathPlanning::maxBeliefNode;

CHolonomicPathPlanning::CHolonomicPathPlanning(int theStartDummyID,int theGoalDummyID,
						int theRobotCollectionID,int theObstacleCollectionID,int ikGroupID,
						int thePlanningType,float theAngularCoeff,
						float theStepSize,
						const float theSearchMinVal[4],const float theSearchRange[4],
						const int theDirectionConstraints[4],const float clearanceAndMaxDistance[2],const C3Vector& gammaAxis)
{
    //yhc//printf("CHolonomicPathPlanning %d\n",theStartDummyID);
    //CMeasurementRegion temp;

    //MatrixXd ccd = MatrixXd::Identity(2,2);
    //std::cout << "norm = ";
    //std::cout << ccd.norm() << std::endl;

    isHolonomic=true;
	float angle=C3Vector::unitZVector.getAngle(gammaAxis);
	if (angle<0.1f*degToRad)
		_gammaAxisRotation.setIdentity();
	else
	{
        if (angle>179.9f*degToRad)
			_gammaAxisRotation.setEulerAngles(piValue,0.0f,0.0f);
		else
		{
			C3Vector r((C3Vector::unitZVector^gammaAxis).getNormalized());
			_gammaAxisRotation.setAngleAndAxis(angle,r);
		}
	}
	_gammaAxisRotationInv=_gammaAxisRotation.getInverse();

	_allIsObstacle=(theObstacleCollectionID==-1);
	firstPass=true;
	invalidData=true;
	obstacleClearanceAndMaxDistance[0]=clearanceAndMaxDistance[0];
	obstacleClearanceAndMaxDistance[1]=clearanceAndMaxDistance[1];
	planningType=thePlanningType;
	startDummyID=theStartDummyID;
	goalDummyID=theGoalDummyID;
	CDummyDummy* startDummy=(CDummyDummy*)_simGetObject(startDummyID);
	CDummyDummy* goalDummy=(CDummyDummy*)_simGetObject(goalDummyID);
	if ( (startDummy==NULL)||(goalDummy==NULL) )
		return;



    ///////////////////////////
    /// \brief uncertainty
    /// Read uncertainty parameter
    ///////////////////////////
    simChar *param;
    int ret_size;



    param = simReadCustomDataBlock(startDummyID,"MPType",&ret_size);
    if(param)
    {
        custom_function = atoi(param);
        robot.setRobotMPType(startDummyID,custom_function);
        //yhc//printf("Use RRBT : %d\n",custom_function);
        simReleaseBuffer(param);
    }
    else custom_function = 0;

    if(custom_function == MPTYPE_RRBT)
    {        
        numoffail = 0;
        //simRVO = new RVO::RVOSimulator();
        //simRVO->setAgentDefaults(10,10,5,5,0.25,1);
        robot.allocRobot(startDummyID);
        param = simReadCustomDataBlock(startDummyID,"init_uncert",&ret_size);
        initialUncertainty = atof(param);
        robot.setRobotUncertainty(startDummyID,initialUncertainty);
        //yhc//printf("initial uncertainty : %f\n",initialUncertainty);
        simReleaseBuffer(param);

        simFloat orientation[3];
        simGetObjectOrientation(startDummyID,-1,orientation);
        robot.setRobotDirection(startDummyID,orientation[2]);

        float leftV=0,rightV=0,prefV=0;

        param = simReadCustomDataBlock(startDummyID,"leftV",&ret_size);
        if(param){
            leftV = atof(param);
            simReleaseBuffer(param);
        }
        param = simReadCustomDataBlock(startDummyID,"rightV",&ret_size);
        if(param)
        {
            rightV = atof(param);
            simReleaseBuffer(param);
        }
        robot.setRobotVelocity(startDummyID,(rightV + leftV) / 2);

        param = simReadCustomDataBlock(startDummyID,"prefV",&ret_size);
        if(param)
        {
            prefV = atof(param);
            simReleaseBuffer(param);
        }
        prefSpeed = prefV;
        robot.setRobotPrefVelocity(startDummyID,prefV);
        //yhc//printf("initial uncertainty : %f\n",initialUncertainty);
        if(CStaticData::savedGoal && CStaticData::savedGoal->belief_node.size())
            CHolonomicPathPlanning::maxBeliefNode = 1000;
        else
            CHolonomicPathPlanning::maxBeliefNode = 10;
    }

    if(custom_function == MPTYPE_RRBT )
    {
        int obs_id;
        int obs_num = 1;
        char obs_name[10];
        simFloat orientation[3];
        CDummyDummy* obs_dummy;

        float *vertices;
        int verticesSize;
        int *indices;
        int indicesSize;
        float vert[4][3];

        C7Vector ObstacleVec;
        while(1)
        {
            sprintf(obs_name,"Obs%d",obs_num);
            obs_id=simGetObjectHandle(obs_name);
            if(obs_id == -1)    break;
            obs_dummy = (CDummyDummy*)_simGetObject(obs_id);
            _simGetObjectCumulativeTransformation(obs_dummy,ObstacleVec.X.data,ObstacleVec.Q.data,false);
            simGetObjectOrientation(obs_id,-1,orientation);

            if(simGetShapeMesh(obs_id,&vertices,&verticesSize,&indices,&indicesSize,NULL)!=-1)
            {
                for(int i=0;i<4;i++)
                {
                    for(int j=0;j<3;j++)
                    {
                        if(i > 1)
                            vert[i-2][j] = vertices[i*3+j]; //make clock wise
                        else
                            vert[3-i][j] = vertices[i*3+j]; //make clock wise
                    }
                }
                simReleaseBuffer((char*)vertices);
                simReleaseBuffer((char*)indices);
            }
            obstacles.addObstacleType1(vert,ObstacleVec.X.data[0],ObstacleVec.X.data[1],orientation[2]);
            obs_num++;
        }
    }

    if(custom_function == MPTYPE_RRBT)
    {
        int mea_id;
        int mea_num = 1;
        char mea_name[15];

        simFloat orientation[3];
        CDummyDummy* mea_dummy;
        C7Vector MeaRegionVec;

        float *vertices;
        int verticesSize;
        int *indices;
        int indicesSize;
        float vert[4][3];

        while(1)
        {
            sprintf(mea_name,"MeaRegion%d",mea_num);
            mea_id = simGetObjectHandle(mea_name);
            if(mea_id == -1)    break;

            mea_dummy = (CDummyDummy*)_simGetObject(mea_id);
            _simGetObjectCumulativeTransformation(mea_dummy,MeaRegionVec.X.data,MeaRegionVec.Q.data,false);
            simGetObjectOrientation(mea_id,-1,orientation);

            if(simGetShapeMesh(mea_id,&vertices,&verticesSize,&indices,&indicesSize,NULL)!=-1)
            {
                for(int i=0;i<4;i++)
                {
                    for(int j=0;j<3;j++)
                    {
                        vert[i][j] = vertices[i*3+j];
                    }
                }
                simReleaseBuffer((char*)vertices);
                simReleaseBuffer((char*)indices);
            }
            measurement.addMeasurementType1(vert,MeaRegionVec.X.data[0],MeaRegionVec.X.data[1],orientation[2]);
            mea_num++;
        }
        //simInt shapeHandle,simFloat** vertices,simInt* verticesSize,simInt** indices,simInt* indicesSize,simFloat** normals);
    }

    if(custom_function == MPTYPE_RRBT)
    {
        int beacon_id;
        int beacon_num = 1;
        char beacon_name[15];

        CDummyDummy* beacon_dummy;
        C7Vector BeaconVec;

        while(1)
        {
            sprintf(beacon_name,"Beacon%d",beacon_num);
            beacon_id = simGetObjectHandle(beacon_name);
            if(beacon_id == -1)    break;

            beacon_dummy = (CDummyDummy*)_simGetObject(beacon_id);
            _simGetObjectCumulativeTransformation(beacon_dummy,BeaconVec.X.data,BeaconVec.Q.data,false);

            beacon.addBeacon(BeaconVec.X.data[0],BeaconVec.X.data[1]);
            beacon_num++;
        }
    }


    if(custom_function == MPTYPE_RRBT) // Calculate other robot position
    {
        int robot_id;
        int robot_num = 0;
        char robot_name[20];
        simFloat orientation[3];
        CDummyDummy* robot_dummy;
        C7Vector RobotVec;

        while(1)
        {
            sprintf(robot_name,"Bill#%d",robot_num++);
            robot_id = simGetObjectHandle(robot_name);
            if((robot_id == -1) || (robot_num > 2))    break;
            if(robot_id == startDummyID)    continue;   // current robot id
            robot.allocRobot(robot_id);
            robot_dummy = (CDummyDummy*)_simGetObject(robot_id);
            _simGetObjectCumulativeTransformation(robot_dummy,RobotVec.X.data,RobotVec.Q.data,false);
            robot.setRobotLocation(robot_id,RobotVec.X.data[0],RobotVec.X.data[1]);

            simGetObjectOrientation(robot_id,-1,orientation);
            robot.setRobotDirection(robot_id,orientation[2]);
            float leftV = 0,rightV = 0,prefV = 0;

            param = simReadCustomDataBlock(robot_id,"leftV",&ret_size);
            if(param){
                leftV = atof(param);
                simReleaseBuffer(param);
            }
            else leftV = 20;
            param = simReadCustomDataBlock(robot_id,"rightV",&ret_size);
            if(param){
                rightV = atof(param);
                simReleaseBuffer(param);
            }
            else rightV = 20;
            robot.setRobotVelocity(robot_id,(rightV + leftV) / 2);

            param = simReadCustomDataBlock(robot_id,"prefV",&ret_size);
            if(param){
                prefV = atof(param);
                simReleaseBuffer(param);                
            }
            else
                prefV = 1;
            robot.setRobotPrefVelocity(robot_id,prefV);
            //simRVO->addAgent(RVO::Vector2(RobotVec.X.data[0],RobotVec.X.data[1]));
        }
    }

	_simGetObjectCumulativeTransformation(startDummy,_startDummyCTM.X.data,_startDummyCTM.Q.data,false);
	_simGetObjectLocalTransformation(startDummy,_startDummyLTM.X.data,_startDummyLTM.Q.data,false);

	C7Vector goalDummyCumulTransf;
	_simGetObjectCumulativeTransformation(goalDummy,goalDummyCumulTransf.X.data,goalDummyCumulTransf.Q.data,false);
	C7Vector goalDummyLocalConf(_startDummyCTM.getInverse()*goalDummyCumulTransf);

    //temp.checkMeasurement(_startDummyCTM.X.data[0],_startDummyCTM.X.data[1]);

    CHolonomicPathNode *startNode;
    CHolonomicPathNode *goalNode;


    if(custom_function == MPTYPE_RRBT)
    {
        startNode = new CHolonomicPathNode(planningType,_startDummyCTM,_gammaAxisRotation,_gammaAxisRotationInv);
        goalNode = new CHolonomicPathNode(planningType,goalDummyCumulTransf,_gammaAxisRotation,_gammaAxisRotationInv);

        robot.setRobotLocation(startDummyID,startNode->values[0],startNode->values[1]);
        robot.setRobotGoal(startDummyID,goalNode->values[0],goalNode->values[1]);
    }
    else
    {
        C7Vector sConf;
        sConf.setIdentity();
        startNode = new CHolonomicPathNode(planningType,sConf,_gammaAxisRotation,_gammaAxisRotationInv);
        goalNode = new CHolonomicPathNode(planningType,goalDummyLocalConf,_gammaAxisRotation,_gammaAxisRotationInv);
    }

    if(custom_function == MPTYPE_RRBT)
    {
        clock_t begin = clock();
        minGoalBelief = NULL;
        use_vocheck = true;
        YHC_RRBT_InitializeTree(startNode,goalNode);
        //YHC_RRBT_CompactInitializeTree(startNode,goalNode);
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        //printf("initialize time %lf\n",elapsed_secs);
        //printf("%lf\n",elapsed_secs);

        /*int numofBelief = 0;
        for(std::vector<CHolonomicPathNode*>::iterator it=fromStart.begin();it!=fromStart.end();++it)
        {
            numofBelief += (*it)->belief_node.size();
            if((*it)->belief_node.size() > maxBeliefNode)
            {
                printf("??\n");
            }
        }
        printf("Num of Belief = %d\n",numofBelief);*/
    }    
    else
    {
        fromStart.push_back(startNode);
        //CStaticData::push_back(startDummyID,tempNode->values[0],tempNode->values[1],(void*)tempNode);
        fromGoal.push_back(goalNode);
    }
    robotCollectionID=theRobotCollectionID;
	obstacleCollectionID=theObstacleCollectionID;
	// Following sets the _startConfInterferenceState to SIM_MAX_FLOAT if not colliding or above distance threshold.
	// Otherwise it is set to 0.0 (colliding) or the distance with the obstacle
	// This is used to allow path planning also when in an initial collision state. In that case the initial path segment
	// will follow a distance to obstacle gradient that is increasing until above the collision distance
	doCollide(&_startConfInterferenceState);

	angularCoeff=theAngularCoeff;
	stepSize=theStepSize;
	_directionConstraintsOn=false;

	for (int i=0;i<4;i++)
	{
		_searchMinVal[i]=theSearchMinVal[i];
		_searchRange[i]=theSearchRange[i];
		_directionConstraints[i]=theDirectionConstraints[i];
        if (_directionConstraints[i]!=0)
            _directionConstraintsOn=true;
	}



	buffer[0]=-1;
	invalidData=false;

}

CHolonomicPathPlanning::~CHolonomicPathPlanning()
{
    //yhc//printf("~CHolonomicPathPlanning\n");
    //CStaticData::free(startDummyID);
    if(custom_function == MPTYPE_RRBT)
    {
        //민철: 여기도 카운트 해보세요

        CStaticData::savedTree.clear();
        CStaticData::savedTree.assign(fromStart.begin(),fromStart.end());
        int numofbef = 0;
        for(int i=0;i<CStaticData::savedTree.size();i++)
        {
            numofbef += CStaticData::savedTree[i]->belief_node.size();
        }
        clock_t endClock = clock();
        if((double(endClock - startClock) / CLOCKS_PER_SEC) > 0.5)
        {
            printf("%f\n",CStaticData::numof_tested_belief / (double(endClock - startClock) / CLOCKS_PER_SEC));
        }

        for(int i=0;i<fromStart[0]->belief_node.size();i++)
        {
            if(YHC_RRBT_nodetest_recur(fromStart[0]->belief_node[i]) == false)
            {
                printf("error l;dafszcxv\n");
            }
        }
        fromStart.clear();
        //for (int i=0;i<int(fromGoal.size());i++)
        //    delete fromGoal[i];
        fromGoal.clear();
        for (int i=0;i<int(foundPath.size());i++)
            delete foundPath[i];
        foundPath.clear();

        //delete simRVO;
    }
    else
    {
        for (int i=0;i<int(fromStart.size());i++)
            delete fromStart[i];
        fromStart.clear();
        for (int i=0;i<int(fromGoal.size());i++)
            delete fromGoal[i];
        fromGoal.clear();
        for (int i=0;i<int(foundPath.size());i++)
            delete foundPath[i];
        foundPath.clear();
    }


}

void CHolonomicPathPlanning::YHC_RRBT_CompactInitializeTree(CHolonomicPathNode *startNode, CHolonomicPathNode *goalNode)
{


}

void CHolonomicPathPlanning::YHC_RRBT_ClearSavedTree(){
    for(std::vector<CHolonomicPathNode*>::iterator it = CStaticData::savedTree.begin();
        it != CStaticData::savedTree.end();++it)
    {
        for(std::vector<CBelief*>::iterator bf = (*it)->belief_node.begin(); bf != (*it)->belief_node.begin();)
        {
            delete *bf;
        }
        (*it)->belief_node.clear();
        delete (*it);
    }
    CStaticData::savedTree.clear();
    CStaticData::savedGoal = NULL;
}

void CHolonomicPathPlanning::YHC_RRBT_InitializeTree(CHolonomicPathNode *startNode, CHolonomicPathNode *goalNode)
{
    CBelief *startBelief = new CBelief();
    std::vector<CBelief*> goalPathBelief;

    startBelief->cost = 0;
    startBelief->time = 0;
    startBelief->lamda = MatrixXd::Zero(2,2);
    startBelief->sigma = initialUncertainty * MatrixXd::Identity(2,2);
    startBelief->parent = 0;
    startBelief->node = startNode;
    startNode->appendBelief(startBelief,NULL);

    ompnn.reset(new ompl::NearestNeighborsLinear<CHolonomicPathNode*>());
    ompnn->setDistanceFunction(boost::bind(&CHolonomicPathPlanning::distance, this, _1, _2));




    simChar *param;
    int ret_size;
    int clear_tree = false;
    param = simReadCustomDataBlock(startDummyID,"clear_tree",&ret_size);
    if(param)
    {
        clear_tree = atoi(param);
        //yhc//printf("Clear Tree : %d\n",clear_tree);
        simReleaseBuffer(param);
    }
    //clear_tree = true;
    sampleCount = 0;
    if(clear_tree || ((CStaticData::savedGoal)&&(CStaticData::savedGoal->belief_node.size()==0)))
    {

        CStaticData::try_count=0;
        CStaticData::numof_tested_vertex=0;
        CStaticData::numof_tested_belief=0;

        YHC_RRBT_ClearSavedTree();
    }
    if(CStaticData::savedTree.size())
    {
        if(CStaticData::savedGoal->belief_node.size())
        {
            CHolonomicPathNode *pre_goalNode = CStaticData::savedGoal;
            CBelief *min_belief;

            min_belief = pre_goalNode->belief_node[0];
            for(int i=1;i<pre_goalNode->belief_node.size();i++)
            {
                int cmp = min_belief->compareCostWith(pre_goalNode->belief_node[i]);
                if(cmp == 1)
                    min_belief = pre_goalNode->belief_node[i];
            }
            minGoalBelief = min_belief;
            CBelief *bef = min_belief;
            while(bef)
            {
                goalPathBelief.push_back(bef);
                bef = bef->parent;
            }
            CStaticData::path_length += distance(goalPathBelief[goalPathBelief.size()-1]->node,startNode);

            if(!YHC_RRBT_BranchBound(&goalPathBelief))
            {
                YHC_RRBT_ClearSavedTree();
            }
            else
            {
                //if(goalPathBelief.size() == 2)
                //    printf("goalPathBelief.size() %d \n",goalPathBelief.size());

                int pre_belief_idx = goalPathBelief.size()-1;
                int next_belief_idx = goalPathBelief.size()-2;
                CBelief* next_belief = goalPathBelief[next_belief_idx];
                float cur_x = startNode->values[0];
                float cur_y = startNode->values[1];
                float ex = next_belief->node->values[0];
                float ey = next_belief->node->values[1];
                float dist = distance(startNode,next_belief->node);

                if(dist < 0.1 && next_belief_idx > 0)
                {
                    next_belief_idx--;
                }

                //CBelief *new_n = CBelief();
                if(YHC_RRBT_propagate(goalPathBelief[pre_belief_idx]->node,startNode,goalPathBelief[pre_belief_idx],startBelief,false) == false)
                    printf("fatal error T_T!!!\n");
                startBelief->cost = 0;
                startBelief->parent = 0;

                bef = goalPathBelief[next_belief_idx];
                for(std::vector<CBelief*>::iterator it = bef->parent->children.begin();it != bef->parent->children.end();++it)
                {
                    if(*it == bef)
                    {
                        bef->parent->children.erase(it);
                        break;
                    }
                }
                bef->parent = startBelief;
                startBelief->children.push_back(bef);

                YHC_RRBT_DeleteNode(goalPathBelief[pre_belief_idx]->node,goalPathBelief[next_belief_idx]);


                /*for(std::vector<CBelief*>::iterator it=bef->node->belief_node.begin();
                    it != bef->node->belief_node.end(); ++it)
                {
                    if((*it)->parent)
                        //yhc//printf("invalid parent\n");
                    //(*it)->parent = startBelief;
                    //startBelief->children.push_back(*it);
                }*/
                ompnn->add(startNode);
                fromStart.push_back(startNode);

                /*
                for(int i=0;i<(int)fromStart.size();i++){
                    for(int j=0;j<(int)fromStart[i]->belief_node.size();j++)
                    {
                        if(fromStart[i]->belief_node[j]->node == 0)
                        {
                            //yhc//printf("error on YHC_InitializeTree\n");
                        }
                    }
                }*/

                //clock_t begin = clock();
                //YHC_RRBT_repropagate(startBelief);
                //clock_t end = clock();
                //double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
                //printf("Repropagate time %lf\n",elapsed_secs);
            }
        }
        else
        {
            delete startNode;
            startNode = NULL;
        }
        for(int i=0;i<CStaticData::savedTree.size();i++)
        {
            if(CStaticData::savedTree[i]->belief_node.size())
            {
                ompnn->add(CStaticData::savedTree[i]);
                fromStart.push_back(CStaticData::savedTree[i]);
            }
            else
                delete CStaticData::savedTree[i];
        }
        if(CStaticData::savedGoal)
        {
            delete goalNode;
            goalNode = NULL;
            fromGoal.push_back(CStaticData::savedGoal);
            ompnn->add(CStaticData::savedGoal);
        }
    }
    else
    {
        CStaticData::path_length = 0;
    }
    if(!CStaticData::savedTree.size())
    {
        ompnn->add(startNode);
        ompnn->add(goalNode);
        CStaticData::savedGoal = goalNode;
        fromStart.push_back(startNode);
        fromGoal.push_back(goalNode);
    }


    CHolonomicPathNode* front = fromStart.front();
    //simRVO->addAgent(RVO::Vector2(front->values[0],front->values[1]));

    startClock = clock();
}

bool CHolonomicPathPlanning::YHC_RRBT_nodetest(CBelief *belief)
{
    int flag = 0;
    for(std::vector<CBelief*>::iterator it = belief->node->belief_node.begin();it != belief->node->belief_node.end();++it)
    {
        if(*it == belief)
        {
            flag = 1;
            break;
        }
    }

    if(flag == 0)
    {
        printf("test errror\n");
        return false;
    }
    return true;
}
bool CHolonomicPathPlanning::YHC_RRBT_nodetest_recur(CBelief *belief)
{
    int flag = 0;
    for(std::vector<CBelief*>::iterator it = belief->node->belief_node.begin();it != belief->node->belief_node.end();++it)
    {
        if(*it == belief)
        {
            flag = 1;
            break;
        }
    }


    for(std::vector<CBelief*>::iterator it = belief->children.begin();it != belief->children.end();)
    {
        if(!YHC_RRBT_nodetest_recur(*it))
        {
            if((*it)->children.size())
            {
                printf("what should I do?\n");
            }
            delete *it;
            belief->children.erase(it);
        }
        else
            ++it;
    }
    if(flag == 0)
    {
        return false;
    }
    return true;
}


bool CHolonomicPathPlanning::YHC_RRBT_DeleteBelief(CBelief *tobeDeleted,CBelief *exceptBef)
{
    if(!YHC_RRBT_nodetest(tobeDeleted))
        printf("error here\n");
    if(!YHC_RRBT_DeleteChildBelief(tobeDeleted,exceptBef))
        return false;
    if(tobeDeleted->parent)
    {
        CBelief *parent = tobeDeleted->parent;
        for(std::vector<CBelief*>::iterator chd = parent->children.begin();chd != parent->children.end();++chd)
        {
            if(*chd == tobeDeleted)
            {
                parent->children.erase(chd);
                break;
            }
        }
    }
    int flag = 0;
    for(std::vector<CBelief*>::iterator it = tobeDeleted->node->belief_node.begin();it != tobeDeleted->node->belief_node.end();++it)
    {
        if(*it == tobeDeleted)
        {
            tobeDeleted->node->belief_node.erase(it);
            flag = 1;
            break;
        }
    }

    if(flag == 0)
    {
        printf("debug point\n");
    }
    tobeDeleted->parent = 0;
    tobeDeleted->node = 0;
    delete tobeDeleted;

    return true;
}

bool CHolonomicPathPlanning::YHC_RRBT_DeleteChildBelief(CBelief *tobeDeleted,CBelief *exceptBef)
{
    CBelief *chk = tobeDeleted;
    while(chk && exceptBef)
    {
        if(chk == exceptBef)
            return false;
        chk = chk->parent;
    }
    for(std::vector<CBelief*>::iterator it=tobeDeleted->children.begin();
        it != tobeDeleted->children.end();)
    {
        if(!YHC_RRBT_nodetest(*it))
        {
            printf("error here\n");
        }
        if((*it)->children.size())
        {
            YHC_RRBT_DeleteChildBelief(*it,exceptBef);
        }
        {
            CBelief *del = (CBelief*)*it;
            if((del)->children.size() && (del) != exceptBef)
            {
                //yhc//
                printf("fatal error on YHC_DeleteBelief\n");
            }
            if((del) == exceptBef)
            {
                del->parent = NULL;
            }
            else
            {
                int flag = 0;
                for(std::vector<CBelief*>::iterator it2=del->node->belief_node.begin();
                    it2 != del->node->belief_node.end();)
                {
                    if(*it2 == del)
                    {
                        del->node->belief_node.erase(it2);
                        flag = 1;
                        break;
                    }
                    else
                        ++it2;
                }
                if(flag == 0)
                {
                    //yhc//
                    printf("fatal error on YHC_DeleteBelief\n");
                }
                del->node = 0;
                del->parent = 0;
                delete del;
            }
            tobeDeleted->children.erase(it);
        }
    }
    return true;
}

bool CHolonomicPathPlanning::YHC_RRBT_DeleteNode(CHolonomicPathNode *tobeDeleted,CBelief *exceptBef)
{
    for(std::vector<CBelief*>::iterator it=tobeDeleted->belief_node.begin();
        it != tobeDeleted->belief_node.end();)
    {
        if(YHC_RRBT_DeleteChildBelief(*it,exceptBef))
        {
            if((*it)->children.size())
            {
                //yhc//printf("fatal error on YHC_DeleteNode\n");
            }
            if((*it)->parent)
            {
                CBelief *parent = (*it)->parent;
                for(std::vector<CBelief*>::iterator chd = parent->children.begin();chd != parent->children.end();++chd)
                {
                    if(*chd == *it)
                    {
                        parent->children.erase(chd);
                        break;
                    }
                }
            }
            delete *it;
            tobeDeleted->belief_node.erase(it);
        }
        else
            ++it;
    }
    if(exceptBef->parent)
    {
        //yhc//printf("Unexpected operation\n");
    }
    /*for(std::vector<CHolonomicPathNode*>::iterator it=CStaticData::savedTree.begin();
        it != CStaticData::savedTree.end();)
    {
        if((*it)->belief_node.size())
            ++it;
        else
        {
            delete (*it);
            CStaticData::savedTree.erase(it);
        }
    }*/
}

void CHolonomicPathPlanning::setAngularCoefficient(float coeff)
{
	angularCoeff=coeff;
}

void CHolonomicPathPlanning::setStepSize(float size)
{
	stepSize=size;
}

void CHolonomicPathPlanning::YHC_RRBT_getSearchTreeData(std::vector<float>& data,bool fromTheStart)
{
    if (!fromTheStart)
    {

        CHolonomicPathNode *goalNode = CStaticData::savedGoal;
        CBelief *cur_belief;
        if(!goalNode->belief_node.size())   return;

        minGoalBelief = cur_belief = goalNode->belief_node[0];
        for(int i=1;i<goalNode->belief_node.size();i++)
        {
            int cmp = cur_belief->compareCostWith(goalNode->belief_node[i]);
            if(cmp == 1)
                minGoalBelief = cur_belief = goalNode->belief_node[i];
        }
        //printf("goal_length = %f\n",cur_belief->cost);
        CBelief *last_bef;
        while(cur_belief->parent)
        {
            C3Vector goal(cur_belief->node->values[0]         ,cur_belief->node->values[1]        ,0.1f);
            C3Vector start(cur_belief->parent->node->values[0],cur_belief->parent->node->values[1],0.1f);
            float d[6];
            start.copyTo(d);
            goal.copyTo(d+3);
            for (int k=0;k<6;k++)
                data.push_back(d[k]);
            last_bef = cur_belief;
            cur_belief = cur_belief->parent;
        }
        char output_speed[20];
        sprintf(output_speed,"%f",last_bef->speed);
        simWriteCustomDataBlock(startDummyID,"OutSpeed",output_speed,strlen(output_speed));
        //printf("speed %f to %f\n",cur_belief->speed,last_bef->speed);
    }
    else
    {
        //std::vector<CHolonomicPathNode*>* cont = &fromStart;
        for (int i=1;i<(int)fromStart.size();i++)
        {
            C3Vector start(fromStart[i]->values[0],fromStart[i]->values[1],0.0f);
            for(int j=0;j<(int)(fromStart[i]->belief_node).size();j++)
            {
                CBelief *belief = (fromStart[i]->belief_node)[j];
                if(belief->parent == 0)
                    continue;
                ////yhc//printf("parent %d, node %d\n",(long)belief->parent,(long)belief->parent->node);
                C3Vector goal(belief->parent->node->values[0],belief->parent->node->values[1],0.1f);
                ////yhc//printf("%f %f %f %f\n",start.data[0],start.data[1],goal.data[0],goal.data[1]);
                float d[6];
                start.copyTo(d);
                goal.copyTo(d+3);
                for (int k=0;k<6;k++)
                    data.push_back(d[k]);
            }
        }
    }
}

void CHolonomicPathPlanning::getSearchTreeData(std::vector<float>& data,bool fromTheStart)
{
    //yhc//printf("getSearchTreeData\n");
    if(custom_function == MPTYPE_RRBT)
    {
        YHC_RRBT_getSearchTreeData(data,fromTheStart);
        return;
    }
    std::vector<CHolonomicPathNode*>* cont;
	if (fromTheStart)
		cont=&fromStart;
	else
		cont=&fromGoal;
	if ( (planningType==sim_holonomicpathplanning_xy)||(planningType==sim_holonomicpathplanning_xyg)||(planningType==sim_holonomicpathplanning_xyabg) )
	{
		for (int i=1;i<int(cont->size());i++)
		{
			C3Vector start((*cont)[i]->values[0],(*cont)[i]->values[1],0.0f);
			C3Vector goal((*cont)[i]->parent->values[0],(*cont)[i]->parent->values[1],0.0f);
			start=_startDummyCTM*start;
			goal=_startDummyCTM*goal;
			float d[6];
			start.copyTo(d);
			goal.copyTo(d+3);
			for (int j=0;j<6;j++)
				data.push_back(d[j]);
		}
	}
	else if ( (planningType==sim_holonomicpathplanning_xg)||(planningType==sim_holonomicpathplanning_xabg) )
	{
		for (int i=1;i<int(cont->size());i++)
		{
			C3Vector start((*cont)[i]->values[0],0.0f,0.0f);
			C3Vector goal((*cont)[i]->parent->values[0],0.0f,0.0f);
			start=_startDummyCTM*start;
			goal=_startDummyCTM*goal;
			float d[6];
			start.copyTo(d);
			goal.copyTo(d+3);
			for (int j=0;j<6;j++)
				data.push_back(d[j]);
		}
	}
	else if ( (planningType==sim_holonomicpathplanning_xyz)||(planningType==sim_holonomicpathplanning_xyzg)||(planningType==sim_holonomicpathplanning_xyzabg) )
	{
		for (int i=1;i<int(cont->size());i++)
		{
			C3Vector start((*cont)[i]->values[0],(*cont)[i]->values[1],(*cont)[i]->values[2]);
			C3Vector goal((*cont)[i]->parent->values[0],(*cont)[i]->parent->values[1],(*cont)[i]->parent->values[2]);
			start=_startDummyCTM*start;
			goal=_startDummyCTM*goal;
			float d[6];
			start.copyTo(d);
			goal.copyTo(d+3);
			for (int j=0;j<6;j++)
				data.push_back(d[j]);
		}
	}
}

float CHolonomicPathPlanning::YHC_nearRange(void)
{
    int n = fromStart.size();
    float alpha = 3;
    //float dimen = 2;

    if(n < 2)  return alpha;
    //return alpha * pow(log(n) / n,1.f/dimen);
    //return alpha * pow(log(n) / n,0.5);
    return alpha * pow(log(n) / n,0.5);
    //return alpha;
}

void CHolonomicPathPlanning::YHC_pullNode(CHolonomicPathNode* closest,CHolonomicPathNode* randNode,float rn)
{
    float dx = randNode->values[0] - closest->values[0];
    float dy = randNode->values[1] - closest->values[1];
    float dist = (float)sqrt(dx * dx + dy * dy);

    if(dist > rn)
    {
        float nx,ny;
        nx = dx * rn / dist + closest->values[0];
        ny = dy * rn / dist + closest->values[1];

        randNode->values[0] = nx;
        randNode->values[1] = ny;
    }
}

int CHolonomicPathPlanning::YHC_RRBT_ParentTest(CHolonomicPathNode* testNode)
{
    CBelief *gparentBelief;
    CBelief *parentBelief;
    CBelief *testBelief;
    CHolonomicPathNode *parentNode;
    CHolonomicPathNode *gparentNode;
    bool replaced = false;

    for(int i=0;i<testNode->belief_node.size();i++)
    {
        testBelief = testNode->belief_node[i];
        parentBelief = testBelief->parent;
        gparentBelief = parentBelief->parent;
        if(gparentBelief == NULL)   continue;
        CBelief *test_n = new CBelief();
        if(YHC_RRBT_propagate(gparentBelief->node,testBelief->node,gparentBelief,test_n))
        {
            CBelief *new_n = new CBelief();
            if(YHC_RRBT_propagate(testBelief->node,parentBelief->node,test_n,new_n))
            {
                int cmp = parentBelief->compareWith(new_n);
                if( cmp == 1 ) // if new belief is lower than current belief
                {
                    testNode->replaceBelief(testBelief,test_n,NULL);
                    test_n->node->belief_node.push_back(test_n);
                    gparentBelief->children.push_back(test_n);

                    parentNode = parentBelief->node;
                    parentNode->replaceBelief(parentBelief,new_n,NULL);
                    new_n->node->belief_node.push_back(new_n);
                    gparentBelief->children.push_back(new_n);
                    replaced = true;
                    break;
                }
            }
            else
                delete new_n;
        }
        else
        {
            delete test_n;
        }
    }

    return replaced;
}

void CHolonomicPathPlanning::YHC_RRBT_BoundPrune(CBelief *sb,std::vector<CBelief*>* goalPathBelief,int goalDepth)
{
    CBelief *eb;
    CBelief *tb = new CBelief();
    int depth_count=0;

    for(std::vector<CBelief*>::iterator chd = sb->children.begin();chd != sb->children.end();)
    {
        eb = *chd;
        if((goalDepth >= 0) && (eb != goalPathBelief->at(goalDepth)))
        {
            if(YHC_RRBT_propagate(sb->node,eb->node,sb,tb,false) == false)
            {
                //printf("Fatal error T_T_T_T_T\n");
            }

            eb->cost = tb->cost;
            eb->sigma = tb->sigma;
            eb->time = tb->time;
            int use_branchbound = true;
            if(use_branchbound)
            {
                float dist = distance(eb->node,minGoalBelief->node);
                eb->cost += dist;
                if(eb->compareWith(minGoalBelief) > 0 || eb->compareWithLM_Marginal(minGoalBelief) > 0)
                {
                    // This function is a core part of Branch - and - bound
                    YHC_RRBT_DeleteBelief(eb,NULL);
                    continue;
                }
                eb->cost -= dist;
            }
            depth_count=0;
        }
        else
            depth_count=1;
        YHC_RRBT_BoundPrune(eb,goalPathBelief,goalDepth-depth_count);
        ++chd;
    }

    delete tb;
}

bool CHolonomicPathPlanning::YHC_RRBT_BranchBound(std::vector<CBelief*>* goalPathBelief)
{
    int path_idx = goalPathBelief->size()-1;
    CBelief *sb = goalPathBelief->at(path_idx);
    CBelief *eb;
    CBelief *tb = new CBelief();


    for(int i=path_idx-1;i>=0;i--)
    {
        eb = goalPathBelief->at(i);

        if(YHC_RRBT_propagate(sb->node,eb->node,sb,tb,false) == false)
        {
            //printf("FATAL ERRORRORORORORO!!!\n");
        }
        eb->cost = tb->cost;
        eb->sigma = tb->sigma;
        eb->time = tb->time;

        if(!use_vocheck && YHC_RRBT_VOCheck(sb->node,eb->node,sb,eb) == false)
        {
            delete tb;
            return false;
        }
        sb = eb;
    }

    {
        int numofbef = 0;
        for(int i=0;i<CStaticData::savedTree.size();i++)
        {
            numofbef += CStaticData::savedTree[i]->belief_node.size();
        }
        //printf("%d\t",numofbef);
    }
        //요거 완성할 것.
    sb = goalPathBelief->at(path_idx);
    YHC_RRBT_BoundPrune(sb,goalPathBelief,path_idx-1);
    delete tb;
    {
        int numofbef = 0;
        for(int i=0;i<CStaticData::savedTree.size();i++)
        {
            numofbef += CStaticData::savedTree[i]->belief_node.size();
        }
        //printf("%d\n",numofbef);
    }
    return true;
}

int CHolonomicPathPlanning::YHC_RRBT_searchPath(int maxTimePerPass)
{
    if (invalidData)
        return(0);
    //if ( (fromStart.size()==0)||(fromGoal.size()==0)||(foundPath.size()!=0) )
        //return(0);
    CDummyDummy* startDummy=(CDummyDummy*)_simGetObject(startDummyID);
    if (startDummy==NULL)
        return(0);

    if(CStaticData::savedGoal && CStaticData::savedGoal->belief_node.size())
        CStaticData::try_count++;

    int initTime=simGetSystemTimeInMs(-1);

    std::vector<CHolonomicPathNode*>* current=&fromStart;

    MatrixXd zero = MatrixXd::Identity(2,2)*1e-10;
    while (_simGetTimeDiffInMs(initTime)<maxTimePerPass)
    {
        CHolonomicPathNode* randNode;
        std::vector<CHolonomicPathNode*> n_range;

        while(n_range.size() == 0)
        {
            randNode = new CHolonomicPathNode(planningType,_searchMinVal,_searchRange,_gammaAxisRotation,_gammaAxisRotationInv);

            if(obstacles.checkCollide(randNode->values[0],randNode->values[1],&zero))
            {
                delete randNode;
                continue;
            }
            CHolonomicPathNode *nearest = ompnn->nearest(randNode);
            if(distance(nearest,randNode) > YHC_nearRange())
                YHC_pullNode(nearest,randNode,YHC_nearRange()*0.99f);
            ompnn->nearestR(randNode,YHC_nearRange(),n_range);
        }
        //민철 : 카운트를 하세요

        std::vector<CBelief*> extend_ns;
        bool propagate_succ = false;
        for(int i=0;i<(int)n_range.size();i++)
        {
            for(int j=0;j<(int)n_range[i]->belief_node.size();j++)
            {
                CBelief *new_n = new CBelief();

                if(YHC_RRBT_propagate(n_range[i],randNode,n_range[i]->belief_node[j],new_n))
                {
                    if(randNode->appendBelief(new_n,&extend_ns))
                    {
                        propagate_succ = true;
                        extend_ns.push_back(new_n);
                        //if(minGoalBelief)
                            CStaticData::numof_tested_belief++;
                    }
                    else
                        numoffail++;
                }
                else
                    delete new_n;
            }
        }

        if(!propagate_succ)
        {
            delete randNode;    //rand node 재생성
            continue; //Propagation이 안되면 재시도
        }
        //민철 : 여기를 카운트를 하세요


        if(minGoalBelief)
            CStaticData::numof_tested_vertex++;
        fromStart.push_back(randNode);
        ompnn->add(randNode);


        //if(!YHC_RRBT_ParentTest(randNode)) // Parent와 Child사이에 Node를 넣어
        {
            for(int i=0;i<(int)extend_ns.size();i++)
            {
                for(int j=0;j<(int)n_range.size();j++)
                {
                    CBelief *new_n = new CBelief();
                    if(YHC_RRBT_propagate(randNode,n_range[j],extend_ns[i],new_n))
                    {
                        if(n_range[j]->appendBelief(new_n,NULL) && new_n->children.size())
                        {
                            if(minGoalBelief)
                                CStaticData::numof_tested_belief++;
                        }
                    }
                    else
                        delete new_n;
                }
            }
        }
    }
    for(int i=0;i<(int)fromStart.size();i++){
        for(int j=0;j<(int)fromStart[i]->belief_node.size();j++)
        {
            if(fromStart[i]->belief_node[j]->node == 0)
            {
                //yhc//printf("errorrrrrr\n");
            }
        }
    }

    if(CStaticData::savedGoal->belief_node.size())
        return 1;
    return 0;
    //return 1;//fake
}

int CHolonomicPathPlanning::searchPath(int maxTimePerPass)
{ // maxTimePerPass is in miliseconds

    // RRBT brach
    if(custom_function == MPTYPE_RRBT)
        return YHC_RRBT_searchPath(maxTimePerPass);

	if (invalidData)
		return(0);
	if ( (fromStart.size()==0)||(fromGoal.size()==0)||(foundPath.size()!=0) )
        return(0);
	CDummyDummy* startDummy=(CDummyDummy*)_simGetObject(startDummyID);
	if (startDummy==NULL)
		return(0);
	// Following since 2010/08/19 so that we can move the "robot" while we search:
	C7Vector dumSavedConf;
    _simGetObjectLocalTransformation(startDummy,dumSavedConf.X.data,dumSavedConf.Q.data,false);

    //yhc//printf("searchPath %d\n",maxTimePerPass);
	int dirConstrSave[4];
	for (int constr=0;constr<4;constr++)
		dirConstrSave[constr]=_directionConstraints[constr];

	if (maxTimePerPass==131183)
		return(61855195);
	
	std::vector<CHolonomicPathNode*>* current=&fromStart;
	std::vector<CHolonomicPathNode*>* nextCurrent=&fromGoal;
	std::vector<CHolonomicPathNode*>* tmpCurrent=NULL;

	int foundAPath=0;
	int initTime=simGetSystemTimeInMs(-1);
	while (_simGetTimeDiffInMs(initTime)<maxTimePerPass)
    {
        for (int i=0;i<2;i++)
		{
			CHolonomicPathNode* randNode;
			if (firstPass) // If first pass, we try to link start to goal directly!
			{
				randNode=(*nextCurrent)[0]->copyYourself();
				firstPass=false;
			}
			else
				randNode=new CHolonomicPathNode(planningType,_searchMinVal,_searchRange,_gammaAxisRotation,_gammaAxisRotationInv);

			CHolonomicPathNode* closest=getClosestNode(*current,randNode);
			if (closest!=NULL)
			{
				closest=extend(current,closest,randNode,false,startDummy);
				if (closest!=NULL)
				{
					for (int constr=0;constr<4;constr++) // We have to inverse the constraints!!
						_directionConstraints[constr]*=-1;
					CHolonomicPathNode* closestConnect=getClosestNode(*nextCurrent,closest);
					if (closestConnect!=NULL)
					{
						closestConnect=extend(nextCurrent,closestConnect,closest,true,startDummy);
						if (closestConnect!=NULL)
						{
							if (current==&fromStart)
							{
								CHolonomicPathNode* iterat=closest;
								while (iterat!=NULL)
								{
									foundPath.insert(foundPath.begin(),iterat->copyYourself());
                                    iterat=iterat->parent;
								}
								iterat=closestConnect;
								while (iterat!=NULL)
								{
									foundPath.push_back(iterat->copyYourself());
									iterat=iterat->parent;	
								}
							}
							else
							{
								CHolonomicPathNode* iterat=closest;
								while (iterat!=NULL)
								{
									foundPath.push_back(iterat->copyYourself());
									iterat=iterat->parent;
								}
								iterat=closestConnect;
								while (iterat!=NULL)
								{
									foundPath.insert(foundPath.begin(),iterat->copyYourself());
									iterat=iterat->parent;
								}
							}
                            foundAPath=1;
						}
					}
					for (int constr=0;constr<4;constr++) // We have to inverse the constraints!!
                        _directionConstraints[constr]*=-1;
				}
			}
			delete randNode;
			tmpCurrent=nextCurrent;
			nextCurrent=current;
			current=tmpCurrent;

			for (int constr=0;constr<4;constr++) // We have to inverse the constraints!!
				_directionConstraints[constr]*=-1;

			if (foundAPath!=0)
				break;
        }
		if (foundAPath!=0)
			break;
	}
	// We restore the dummy local config and the constraints
    _simSetObjectLocalTransformation(startDummy,dumSavedConf.X.data,dumSavedConf.Q.data);
	for (int constr=0;constr<4;constr++)
		_directionConstraints[constr]=dirConstrSave[constr];

	return(foundAPath);
}

bool CHolonomicPathPlanning::setPartialPath()
{
    //yhc//printf("setPartialPath()\n");
    if(custom_function == MPTYPE_RRBT)
    {
        //yhc//printf("custom_function\n");
        return true;
    }

    CHolonomicPathNode* it=getClosestNode(fromStart,fromGoal[0]);
    while (it!=NULL)
    {
        foundPath.insert(foundPath.begin(),it->copyYourself());
        it=it->parent;
    }
    return(true);

}

CHolonomicPathNode* CHolonomicPathPlanning::getClosestNode(std::vector<CHolonomicPathNode*>& nodes,CHolonomicPathNode* sample)
{
    ////yhc//printf("getClosestNode()\n");
	float minD=SIM_MAX_FLOAT;
	int index=-1;
	if (planningType==sim_holonomicpathplanning_xy)
	{
		for (int i=0;i<int(nodes.size());i++)
		{
			float vect[2];
			vect[0]=sample->values[0]-nodes[i]->values[0];
			vect[1]=sample->values[1]-nodes[i]->values[1];
			if (areDirectionConstraintsRespected(vect))
			{
				float d=vect[0]*vect[0]+vect[1]*vect[1];
				if (d<minD)
				{
					minD=d;
					index=i;
				}
			}
		}
	}
	else if (planningType==sim_holonomicpathplanning_xg)
	{
		for (int i=0;i<int(nodes.size());i++)
		{
			float vect[2];
			vect[0]=sample->values[0]-nodes[i]->values[0];
			vect[1]=CPathPlanningInterface::getNormalizedAngle(sample->values[1]-nodes[i]->values[1]);
			if (areDirectionConstraintsRespected(vect))
			{
				vect[1]*=angularCoeff;
				float d=vect[0]*vect[0]+vect[1]*vect[1];
				if (d<minD)
				{
					minD=d;
					index=i;
				}
			}
		}
	}
	else if (planningType==sim_holonomicpathplanning_xyz)
	{
		for (int i=0;i<int(nodes.size());i++)
		{
			float vect[3];
			vect[0]=sample->values[0]-nodes[i]->values[0];
			vect[1]=sample->values[1]-nodes[i]->values[1];
			vect[2]=sample->values[2]-nodes[i]->values[2];
			if (areDirectionConstraintsRespected(vect))
			{
				float d=vect[0]*vect[0]+vect[1]*vect[1]+vect[2]*vect[2];
				if (d<minD)
				{
					minD=d;
					index=i;
				}
			}
		}
	}
	else if (planningType==sim_holonomicpathplanning_xyg)
	{
		for (int i=0;i<int(nodes.size());i++)
		{
			float vect[3];
			vect[0]=sample->values[0]-nodes[i]->values[0];
			vect[1]=sample->values[1]-nodes[i]->values[1];
			vect[2]=CPathPlanningInterface::getNormalizedAngle(sample->values[2]-nodes[i]->values[2]);
			if (areDirectionConstraintsRespected(vect))
			{
				vect[2]*=angularCoeff;
				float d=vect[0]*vect[0]+vect[1]*vect[1]+vect[2]*vect[2];
				if (d<minD)
				{
					minD=d;
					index=i;
				}
			}
		}
	}
	else if (planningType==sim_holonomicpathplanning_abg)
	{
		for (int i=0;i<int(nodes.size());i++)
		{
			float vect[4];
			C4Vector toP,fromP;
			C3Vector dum;
			sample->getAllValues(dum,toP);
			nodes[i]->getAllValues(dum,fromP);
			C4Vector diff(fromP.getInverse()*toP);
			vect[0]=diff(0);
			vect[1]=diff(1);
			vect[2]=diff(2);
			vect[3]=diff(3);
			if (areDirectionConstraintsRespected(vect))
			{
				float d=angularCoeff*fromP.getAngleBetweenQuaternions(toP);
				d*=d;
				if (d<minD)
				{
					minD=d;
					index=i;
				}
			}
		}
	}
	else if (planningType==sim_holonomicpathplanning_xyzg)
	{
		for (int i=0;i<int(nodes.size());i++)
		{
			float vect[4];
			vect[0]=sample->values[0]-nodes[i]->values[0];
			vect[1]=sample->values[1]-nodes[i]->values[1];
			vect[2]=sample->values[2]-nodes[i]->values[2];
			vect[3]=CPathPlanningInterface::getNormalizedAngle(sample->values[3]-nodes[i]->values[3]);
			if (areDirectionConstraintsRespected(vect))
			{
				vect[3]*=angularCoeff;
				float d=vect[0]*vect[0]+vect[1]*vect[1]+vect[2]*vect[2]+vect[3]*vect[3];
				if (d<minD)
				{
					minD=d;
					index=i;
				}
			}
		}
	}
	else if (planningType==sim_holonomicpathplanning_xabg)
	{
		for (int i=0;i<int(nodes.size());i++)
		{
			float vect[5];
			vect[0]=sample->values[0]-nodes[i]->values[0];
			C4Vector toP,fromP;
			C3Vector dum;
			sample->getAllValues(dum,toP);
			nodes[i]->getAllValues(dum,fromP);
			C4Vector diff(fromP.getInverse()*toP);
			vect[1]=diff(0);
			vect[2]=diff(1);
			vect[3]=diff(2);
			vect[4]=diff(3);
			if (areDirectionConstraintsRespected(vect))
			{
				float ad=angularCoeff*fromP.getAngleBetweenQuaternions(toP);
				float d=vect[0]*vect[0]+ad*ad;
				if (d<minD)
				{
					minD=d;
					index=i;
				}
			}
		}
	}
	else if (planningType==sim_holonomicpathplanning_xyabg)
	{
		for (int i=0;i<int(nodes.size());i++)
		{
			float vect[6];
			vect[0]=sample->values[0]-nodes[i]->values[0];
			vect[1]=sample->values[1]-nodes[i]->values[1];
			C4Vector toP,fromP;
			C3Vector dum;
			sample->getAllValues(dum,toP);
			nodes[i]->getAllValues(dum,fromP);
			C4Vector diff(fromP.getInverse()*toP);
			vect[2]=diff(0);
			vect[3]=diff(1);
			vect[4]=diff(2);
			vect[5]=diff(3);
			if (areDirectionConstraintsRespected(vect))
			{
				float ad=angularCoeff*fromP.getAngleBetweenQuaternions(toP);
				float d=vect[0]*vect[0]+vect[1]*vect[1]+ad*ad;
				if (d<minD)
				{
					minD=d;
					index=i;
				}
			}
		}
	}
	else // (planningType==sim_holonomicpathplanning_xyzabg)
	{
		for (int i=0;i<int(nodes.size());i++)
		{
			float vect[7];
			vect[0]=sample->values[0]-nodes[i]->values[0];
			vect[1]=sample->values[1]-nodes[i]->values[1];
			vect[2]=sample->values[2]-nodes[i]->values[2];
			C4Vector toP,fromP;
			C3Vector dum;
			sample->getAllValues(dum,toP);
			nodes[i]->getAllValues(dum,fromP);
			C4Vector diff(fromP.getInverse()*toP);
			vect[3]=diff(0);
			vect[4]=diff(1);
			vect[5]=diff(2);
			vect[6]=diff(3);
			if (areDirectionConstraintsRespected(vect))
			{
				float ad=angularCoeff*fromP.getAngleBetweenQuaternions(toP);
				float d=vect[0]*vect[0]+vect[1]*vect[1]+vect[2]*vect[2]+ad*ad;
				if (d<minD)
				{
					minD=d;
					index=i;
				}
			}
		}
	}
	if (index!=-1)
		return(nodes[index]);
	return(NULL);
}

void CHolonomicPathPlanning::YHC_RRBT_changeBelief(CBelief *belief, CBelief *rep)
{
    for(int i=0;i<(int)belief->children.size();i++)
    {
        belief->children[i]->parent = rep;
        rep->children.push_back(belief->children[i]);
    }
    for(std::vector<CBelief*>::iterator it = belief->node->belief_node.begin();
        it != belief->node->belief_node.end(); ++it)
    {
        if((*it) == belief)
        {
            belief->node->belief_node.erase(it);
            belief->node->belief_node.push_back(rep);
            break;
        }
    }
    delete belief;
}

bool CHolonomicPathPlanning::YHC_RRBT_repropagate(CBelief *belief)
{
    for(std::vector<CBelief*>::iterator it = belief->children.begin();
        it != belief->children.end();)
    {
        CBelief *new_n = new CBelief();
        if(YHC_RRBT_propagate(belief->node,(*it)->node,belief,new_n))
        {
            if(new_n->compareWith(*it) < 0)
            {
                YHC_RRBT_changeBelief(*it, new_n);
                *it = new_n;
                //(*it)->node->replaceBelief((*it),new_n,NULL);
                YHC_RRBT_repropagate(new_n);
            }
        }
        else
        {
            //printf("unexpected error, repropagate should be always success\n");
            delete new_n;
        }
        ++it;
    }
}

bool CHolonomicPathPlanning::YHC_RRBT_VOCheck(CHolonomicPathNode* toBeExtended,CHolonomicPathNode* extention,CBelief *n,CBelief *new_n)
{
    if(robot.getNumOfRobot() == 1)
        return true;    
    float T = n->time;
    float total_dist = new_n->cost - n->cost;
    // Step1. Calculate the position at Time T

    // Step2. Calculate the velocity when robot reach toBeExtended node

    RVO::RVOSimulator *simRVO = new RVO::RVOSimulator();

    simRVO->setTimeStep(0.25f); //set default value
    simRVO->setAgentDefaults(1,10,1,1,0.3,2);
    //(float neighborDist, size_t maxNeighbors, float timeHorizon, float timeHorizonObst, float radius, float maxSpeed, const Vector2 &velocity)

    float robot_x = toBeExtended->values[0],robot_y = toBeExtended->values[1];
    simRVO->addAgent(RVO::Vector2(robot_x,robot_y));
    RVO::Vector2 delta = RVO::Vector2(extention->values[0],extention->values[1]) - simRVO->getAgentPosition(0);
    float direction = atan2(delta.y(),delta.x());
    float velocity = robot.getRobotPrefVelByID(startDummyID);
    RVO::Vector2 vecVel(cos(direction)*velocity,sin(direction)*velocity);
    //if(RVO::absSq(vecVel)>1.f)
    //    vecVel = RVO::normalize(vecVel);
    simRVO->setAgentPrefVelocity(0,vecVel);
    simRVO->setAgentVelocity(0,vecVel);


    int numofRobot = robot.getNumOfRobot();
    int robotCount = 1;
    for(int i=0;i<numofRobot;i++)
    {
        float obs_x,obs_y;
        float vel_x,vel_y;
        SRobotParam param = robot.getRobotParamAt(i);
        if(param.robot_id == startDummyID)  continue;
        vel_x = cos(param.direction) * param.velocity;
        vel_y = sin(param.direction) * param.velocity;
        obs_x = param.cur_x + vel_x * T;
        obs_y = param.cur_y + vel_y * T;
        RVO::Vector2 obsVel(vel_x,vel_y);
        if(RVO::absSq(obsVel)>1.f)
            obsVel = RVO::normalize(obsVel);
        simRVO->addAgent(RVO::Vector2(obs_x,obs_y));
        simRVO->setAgentPrefVelocity(robotCount,obsVel);
        simRVO->setAgentVelocity(robotCount,obsVel);
        robotCount++;
    }

    simRVO->doStep();

    RVO::Vector2 newVel = simRVO->getAgentVelocity(0);
    delete simRVO;

    float abs_vel = RVO::abs(vecVel - newVel);
    if(abs_vel > 0.0001)
    {

        if(use_vocheck && fabs(vecVel.y()/vecVel.x() - newVel.y()/newVel.x()) < 0.3)
        {
            /*printf("A %f %f\n",toBeExtended->values[0],toBeExtended->values[1]);
            printf("B %f %f\n",extention->values[0],extention->values[1]);
            printf("Ori_V %.3f %.3f mag %.3f\n",vecVel.x(),vecVel.y(),RVO::abs(vecVel));
            printf("New_V %.3f %.3f mag %.3f\n",newVel.x(),newVel.y(),RVO::abs(newVel));
            printf("new velocity adjusted\n");
            printf("angle diff %f \n",fabs(vecVel.y()/vecVel.x() - newVel.y()/newVel.x()));*/
            new_n->speed = RVO::abs(newVel)*0.6;//prefSpeed
            new_n->time = n->time + total_dist / new_n->speed;
            //printf("new velocity adjusted %f -> %f\n",RVO::abs(vecVel),RVO::abs(newVel));
        }
        else
            return false;
    }

    return true;
}

bool CHolonomicPathPlanning::YHC_RRBT_propagate(CHolonomicPathNode* toBeExtended,CHolonomicPathNode* extention,CBelief *n,CBelief *new_n,bool checkCollision)
{
    //CBelief new_n = new CBelief();
    new_n->parent = n;
    new_n->node = extention;
    new_n->sigma = MatrixXd::Identity(2,2);
    float sx = toBeExtended->values[0];
    float sy = toBeExtended->values[1];

    if(extention->values == NULL)
    {
        printf("fatal error in YHC_RRBT_propagate\n");
        return false;
    }
    float ex = extention->values[0];
    float ey = extention->values[1];

    float cx,cy;
    float prev_x,prev_y;
    float total_dist = (float)sqrt((sx - ex)*(sx - ex) + (sy - ey)*(sy - ey));
    float left_dist = total_dist;

    new_n->cost = n->cost + total_dist;
    new_n->speed = prefSpeed;
    new_n->time = n->time + total_dist / new_n->speed;

    float min_dist = fmin(0.1,total_dist);

    MatrixXd cur_P = n->sigma;
    MatrixXd H = MatrixXd::Identity(2,2);
    MatrixXd F = MatrixXd::Identity(2,2);
    MatrixXd Q;  // Control uncertainty;
    MatrixXd R,S,K,Y;
    MatrixXd Z(2,1);
    MatrixXd u(2,1);
    MatrixXd cur_state(2,1);
    MatrixXd next_state,next_P;
    //MatrixXd virtual_state,virtual_P;
    cur_state << sx, cy;

    /*//yhc//printf("P\n");
    std::cout << P << std::endl;
    //yhc//printf("H\n");
    std::cout << H << std::endl;
    //yhc//printf("F\n");
    std::cout << F << std::endl;
    //yhc//printf("Q\n");
    std::cout << Q << std::endl;
    //yhc//printf("u\n");
    std::cout << u << std::endl;*/
    prev_x = sx;
    prev_y = sy;
    //while(left_dist > min_dist)
    while(left_dist >= 0)
    {
        cx = ex - (ex - sx) * left_dist / total_dist;
        cy = ey - (ey - sy) * left_dist / total_dist;

        if(checkCollision && (obstacles.checkCollide(cx,cy,&cur_P) == true))
        {
            ////yhc//printf("collide %f,%f,%f\n",cx,cy,cur_P.norm());
            return false;
        }
        if(left_dist == 0) break;
        u << (cx - prev_x) , (cy - prev_y) ;

        // Measurement update
        if(measurement.checkMeasurement(cx,cy))
        {
            R = MatrixXd::Identity(2,2) * 0.007;
        }
        else
        {
            R = MatrixXd::Identity(2,2) * 1e+10;
        }
        S = (H * cur_P * H.transpose() + R);
        K = cur_P * H.transpose() * S.inverse();

        Z << cx, cy;
        Y = Z - H * cur_state;
        next_state = cur_state + K * Y;
        next_P = (MatrixXd::Identity(2,2) - K*H) * cur_P;

        //virtual_state = next_state;
        //virtual_P = next_P;

#define CONTROL_UNCERTAINTY 0.00001
        //% Prediction
        if(left_dist >= min_dist)
            Q = MatrixXd::Identity(2,2) * CONTROL_UNCERTAINTY;
        else
            Q = MatrixXd::Identity(2,2) * CONTROL_UNCERTAINTY * left_dist / min_dist;
        cur_state = F * next_state + u;
        cur_P = F * next_P * F.transpose() + Q;        
        left_dist -= min_dist;
        if(left_dist < 0)   left_dist = 0;
        prev_x = cx;
        prev_y = cy;


        //std::cout << prev_x << ' ' << prev_y << std::endl;
    }
    new_n->sigma = cur_P;


    if(!YHC_RRBT_VOCheck(toBeExtended,extention,n,new_n))
    {
        //printf("VO Collision\n");
        return false;
    }


    return true;
}

CHolonomicPathNode* CHolonomicPathPlanning::extend(std::vector<CHolonomicPathNode*>* nodeList,CHolonomicPathNode* toBeExtended,CHolonomicPathNode* extention,bool connect,CDummyDummy* dummy)
{	// Return value is !=NULL if extention was performed and connect is false
	// If connect is true, then return value indicates that connection can be performed!
	bool specialCase=( (fromStart==nodeList[0])&&(toBeExtended==fromStart[0])&&(_startConfInterferenceState!=SIM_MAX_FLOAT) );
	float lastClosest_specialCase=_startConfInterferenceState;
	float theVect[7];
	float totalVect[7];
	int insertedPts=0;
	float artificialVectorLength;
	int passes=getVector(toBeExtended,extention,theVect,stepSize,artificialVectorLength,false);
	bool leave=false;
    ////yhc//printf("extend\n");
	for (int currentPass=0;currentPass<passes;currentPass++)
	{
		if (currentPass==passes-1)
		{
			if (connect)
				return(toBeExtended);
			leave=true;
		}
		C3Vector pos;
		pos.clear();
		C4Vector orient;
		orient.setIdentity();
		bool forbiddenValues;
		if (planningType==sim_holonomicpathplanning_xy)
		{
			totalVect[0]=pos(0)=toBeExtended->values[0]+theVect[0];
			totalVect[1]=pos(1)=toBeExtended->values[1]+theVect[1];
			forbiddenValues=areSomeValuesForbidden(totalVect);
		}
		if (planningType==sim_holonomicpathplanning_xg)
		{
			totalVect[0]=pos(0)=toBeExtended->values[0]+theVect[0];
			totalVect[1]=toBeExtended->values[1]+theVect[1];
			orient=_gammaAxisRotation*(C4Vector(C3Vector(0.0f,0.0f,totalVect[1]))*_gammaAxisRotationInv);
			forbiddenValues=areSomeValuesForbidden(totalVect);
		}
		if (planningType==sim_holonomicpathplanning_xyz)
		{
			totalVect[0]=pos(0)=toBeExtended->values[0]+theVect[0];
			totalVect[1]=pos(1)=toBeExtended->values[1]+theVect[1];
			totalVect[2]=pos(2)=toBeExtended->values[2]+theVect[2];
			forbiddenValues=areSomeValuesForbidden(totalVect);
		}
		if (planningType==sim_holonomicpathplanning_xyg)
		{
			totalVect[0]=pos(0)=toBeExtended->values[0]+theVect[0];
			totalVect[1]=pos(1)=toBeExtended->values[1]+theVect[1];
			totalVect[2]=toBeExtended->values[2]+theVect[2];
			orient=_gammaAxisRotation*(C4Vector(C3Vector(0.0f,0.0f,totalVect[2]))*_gammaAxisRotationInv);
			forbiddenValues=areSomeValuesForbidden(totalVect);
		}
		if (planningType==sim_holonomicpathplanning_abg)
		{
			toBeExtended->getAllValues(pos,orient);
			C4Vector q(theVect);
			orient=orient*q;
			totalVect[0]=orient(0);
			totalVect[1]=orient(1);
			totalVect[2]=orient(2);
			totalVect[3]=orient(3);
			forbiddenValues=false;
		}
		if (planningType==sim_holonomicpathplanning_xyzg)
		{
			totalVect[0]=pos(0)=toBeExtended->values[0]+theVect[0];
			totalVect[1]=pos(1)=toBeExtended->values[1]+theVect[1];
			totalVect[2]=pos(2)=toBeExtended->values[2]+theVect[2];
			totalVect[3]=toBeExtended->values[3]+theVect[3];
			orient=_gammaAxisRotation*(C4Vector(C3Vector(0.0f,0.0f,totalVect[3]))*_gammaAxisRotationInv);
			forbiddenValues=areSomeValuesForbidden(totalVect);
		}
		if (planningType==sim_holonomicpathplanning_xabg)
		{
			toBeExtended->getAllValues(pos,orient);
			totalVect[0]=pos(0)=toBeExtended->values[0]+theVect[0];
			C4Vector q(theVect+1);
			orient=orient*q;
			totalVect[1]=orient(0);
			totalVect[2]=orient(1);
			totalVect[3]=orient(2);
			totalVect[4]=orient(3);
			forbiddenValues=areSomeValuesForbidden(totalVect);
		}
		if (planningType==sim_holonomicpathplanning_xyabg)
		{
			toBeExtended->getAllValues(pos,orient);
			totalVect[0]=pos(0)=toBeExtended->values[0]+theVect[0];
			totalVect[1]=pos(1)=toBeExtended->values[1]+theVect[1];
			C4Vector q(theVect+2);
			orient=orient*q;
			totalVect[2]=orient(0);
			totalVect[3]=orient(1);
			totalVect[4]=orient(2);
			totalVect[5]=orient(3);
			forbiddenValues=areSomeValuesForbidden(totalVect);
		}
		if (planningType==sim_holonomicpathplanning_xyzabg)
		{
			toBeExtended->getAllValues(pos,orient);
			totalVect[0]=pos(0)=toBeExtended->values[0]+theVect[0];
			totalVect[1]=pos(1)=toBeExtended->values[1]+theVect[1];
			totalVect[2]=pos(2)=toBeExtended->values[2]+theVect[2];
			C4Vector q(theVect+3);
			orient=orient*q;
			totalVect[3]=orient(0);
			totalVect[4]=orient(1);
			totalVect[5]=orient(2);
			totalVect[6]=orient(3);
			forbiddenValues=areSomeValuesForbidden(totalVect);
		}
		if (forbiddenValues)
		{ // We hit on forbidden values!
			if (connect)
				return(NULL);
			if (insertedPts!=0)
				return(toBeExtended);
			return(NULL);
		}
		C7Vector transf(C4Vector(orient),pos);
		C7Vector tmpTr(_startDummyLTM*transf);
		_simSetObjectLocalTransformation(dummy,tmpTr.X.data,tmpTr.Q.data);
		if (specialCase)
		{
			float d;
			doCollide(&d);
			if (d>=lastClosest_specialCase)
			{ // This is acceptable (we extend a colliding state, but slowly moving away from the collision)
				lastClosest_specialCase=d;
			}
			else
			{ // Here we are again coming closer to the initial colliding state --> we leave
				if (connect)
					return(NULL);
				if (insertedPts!=0)
					return(toBeExtended);
				return(NULL);
			}
		}
		else
		{
			if (doCollide(NULL))
			{ // We collided!
				if (connect)
					return(NULL);
				if (insertedPts!=0)
					return(toBeExtended);
				return(NULL);
			}
		}

		// No collision at this configuration! We extend it
		CHolonomicPathNode* save=toBeExtended;
		toBeExtended=save->copyYourself();
		toBeExtended->setAllValues(totalVect);

		toBeExtended->parent=save;
		nodeList->push_back(toBeExtended);
		if (leave)
			return(toBeExtended);
		insertedPts++;
	}
	return(NULL);
}

int CHolonomicPathPlanning::getVector(CHolonomicPathNode* fromPoint,CHolonomicPathNode* toPoint,float vect[7],float e,float& artificialLength,bool dontDivide)
{ // if direction constraints are not respected, return value is -1 and vect does not contain anything
	// Otherwise return value is the number of times we have to add 'vect' to 'fromPoint' to reach 'toPoint'
    ////yhc//printf("getVector()\n");
	int retVal=-1;
	if (planningType==sim_holonomicpathplanning_xy)
	{
		vect[0]=toPoint->values[0]-fromPoint->values[0];
		vect[1]=toPoint->values[1]-fromPoint->values[1];
		if (areDirectionConstraintsRespected(vect))
		{
			artificialLength=sqrtf(vect[0]*vect[0]+vect[1]*vect[1]);
			if (dontDivide)
				return(1);
			retVal=(int)(artificialLength/e)+1;
			float l=(float)retVal;
			vect[0]/=l;
			vect[1]/=l;
		}
	}
	if (planningType==sim_holonomicpathplanning_xg)
	{
		vect[0]=toPoint->values[0]-fromPoint->values[0];
		vect[1]=CPathPlanningInterface::getNormalizedAngle(toPoint->values[1]-fromPoint->values[1]);
		if (areDirectionConstraintsRespected(vect))
		{
			artificialLength=vect[0]*vect[0];
			artificialLength=sqrtf(artificialLength+vect[1]*angularCoeff*vect[1]*angularCoeff);
			if (dontDivide)
				return(1);
			retVal=(int)(artificialLength/e)+1;
			float l=(float)retVal;
			vect[0]/=l;
			vect[1]/=l;
		}
	}
	if (planningType==sim_holonomicpathplanning_xyz)
	{
		vect[0]=toPoint->values[0]-fromPoint->values[0];
		vect[1]=toPoint->values[1]-fromPoint->values[1];
		vect[2]=toPoint->values[2]-fromPoint->values[2];
		if (areDirectionConstraintsRespected(vect))
		{
			artificialLength=sqrtf(vect[0]*vect[0]+vect[1]*vect[1]+vect[2]*vect[2]);
			if (dontDivide)
				return(1);
			retVal=(int)(artificialLength/e)+1;
			float l=(float)retVal;
			vect[0]/=l;
			vect[1]/=l;
			vect[2]/=l;
		}
	}
	if (planningType==sim_holonomicpathplanning_xyg)
	{
		vect[0]=toPoint->values[0]-fromPoint->values[0];
		vect[1]=toPoint->values[1]-fromPoint->values[1];
		vect[2]=CPathPlanningInterface::getNormalizedAngle(toPoint->values[2]-fromPoint->values[2]);
		if (areDirectionConstraintsRespected(vect))
		{
			artificialLength=vect[0]*vect[0]+vect[1]*vect[1];
			artificialLength=sqrtf(artificialLength+vect[2]*angularCoeff*vect[2]*angularCoeff);
			if (dontDivide)
				return(1);
			retVal=(int)(artificialLength/e)+1;
			float l=(float)retVal;
			vect[0]/=l;
			vect[1]/=l;
			vect[2]/=l;
		}
	}
	if (planningType==sim_holonomicpathplanning_abg)
	{
		C4Vector toP,fromP;
		C3Vector dum;
		toPoint->getAllValues(dum,toP);
		fromPoint->getAllValues(dum,fromP);
		C4Vector diff(fromP.getInverse()*toP);
		vect[0]=diff(0);
		vect[1]=diff(1);
		vect[2]=diff(2);
		vect[3]=diff(3);
		if (areDirectionConstraintsRespected(vect))
		{
			artificialLength=angularCoeff*fromP.getAngleBetweenQuaternions(toP);
			if (dontDivide)
				return(1);
			retVal=(int)(artificialLength/e)+1;
			float l=(float)retVal;
			C4Vector q;
			q.setIdentity();
			fromP.buildInterpolation(q,diff,1.0f/l);
			vect[0]=fromP(0);
			vect[1]=fromP(1);
			vect[2]=fromP(2);
			vect[3]=fromP(3);
		}
	}
	if (planningType==sim_holonomicpathplanning_xyzg)
	{
		vect[0]=toPoint->values[0]-fromPoint->values[0];
		vect[1]=toPoint->values[1]-fromPoint->values[1];
		vect[2]=toPoint->values[2]-fromPoint->values[2];
		vect[3]=CPathPlanningInterface::getNormalizedAngle(toPoint->values[3]-fromPoint->values[3]);
		if (areDirectionConstraintsRespected(vect))
		{
			artificialLength=vect[0]*vect[0]+vect[1]*vect[1]+vect[2]*vect[2];
			artificialLength=sqrtf(artificialLength+vect[3]*angularCoeff*vect[3]*angularCoeff);
			if (dontDivide)
				return(1);
			retVal=(int)(artificialLength/e)+1;
			float l=(float)retVal;
			vect[0]/=l;
			vect[1]/=l;
			vect[2]/=l;
			vect[3]/=l;
		}
	}
	if (planningType==sim_holonomicpathplanning_xabg)
	{
		vect[0]=toPoint->values[0]-fromPoint->values[0];
		C4Vector toP,fromP;
		C3Vector dum;
		toPoint->getAllValues(dum,toP);
		fromPoint->getAllValues(dum,fromP);
		C4Vector diff(fromP.getInverse()*toP);
		vect[1]=diff(0);
		vect[2]=diff(1);
		vect[3]=diff(2);
		vect[4]=diff(3);
		if (areDirectionConstraintsRespected(vect))
		{
			float ap=angularCoeff*fromP.getAngleBetweenQuaternions(toP);
			artificialLength=vect[0]*vect[0];
			artificialLength=sqrtf(artificialLength+ap*ap);

			if (dontDivide)
				return(1);
			retVal=(int)(artificialLength/e)+1;
			float l=(float)retVal;
			vect[0]/=l;
			C4Vector q;
			q.setIdentity();
			fromP.buildInterpolation(q,diff,1.0f/l);
			vect[1]=fromP(0);
			vect[2]=fromP(1);
			vect[3]=fromP(2);
			vect[4]=fromP(3);
		}
	}
	if (planningType==sim_holonomicpathplanning_xyabg)
	{
		vect[0]=toPoint->values[0]-fromPoint->values[0];
		vect[1]=toPoint->values[1]-fromPoint->values[1];
		C4Vector toP,fromP;
		C3Vector dum;
		toPoint->getAllValues(dum,toP);
		fromPoint->getAllValues(dum,fromP);
		C4Vector diff(fromP.getInverse()*toP);
		vect[2]=diff(0);
		vect[3]=diff(1);
		vect[4]=diff(2);
		vect[5]=diff(3);
		if (areDirectionConstraintsRespected(vect))
		{
			float ap=angularCoeff*fromP.getAngleBetweenQuaternions(toP);
			artificialLength=vect[0]*vect[0]+vect[1]*vect[1];
			artificialLength=sqrtf(artificialLength+ap*ap);

			if (dontDivide)
				return(1);
			retVal=(int)(artificialLength/e)+1;
			float l=(float)retVal;
			vect[0]/=l;
			vect[1]/=l;
			C4Vector q;
			q.setIdentity();
			fromP.buildInterpolation(q,diff,1.0f/l);
			vect[2]=fromP(0);
			vect[3]=fromP(1);
			vect[4]=fromP(2);
			vect[5]=fromP(3);
		}
	}
	if (planningType==sim_holonomicpathplanning_xyzabg)
	{
		vect[0]=toPoint->values[0]-fromPoint->values[0];
		vect[1]=toPoint->values[1]-fromPoint->values[1];
		vect[2]=toPoint->values[2]-fromPoint->values[2];
		C4Vector toP,fromP;
		C3Vector dum;
		toPoint->getAllValues(dum,toP);
		fromPoint->getAllValues(dum,fromP);
		C4Vector diff(fromP.getInverse()*toP);
		vect[3]=diff(0);
		vect[4]=diff(1);
		vect[5]=diff(2);
		vect[6]=diff(3);
		if (areDirectionConstraintsRespected(vect))
		{
			float ap=angularCoeff*fromP.getAngleBetweenQuaternions(toP);
			artificialLength=vect[0]*vect[0]+vect[1]*vect[1]+vect[2]*vect[2];
			artificialLength=sqrtf(artificialLength+ap*ap);

			if (dontDivide)
				return(1);
			retVal=(int)(artificialLength/e)+1;
			float l=(float)retVal;
			vect[0]/=l;
			vect[1]/=l;
			vect[2]/=l;
			C4Vector q;
			q.setIdentity();
			fromP.buildInterpolation(q,diff,1.0f/l);
			vect[3]=fromP(0);
			vect[4]=fromP(1);
			vect[5]=fromP(2);
			vect[6]=fromP(3);
		}
	}
	return(retVal);
}

bool CHolonomicPathPlanning::addVector(C3Vector& pos,C4Vector& orient,float vect[7])
{ // return value true means values are not forbidden!
    ////yhc//printf("addVector()\n");
	float auxVect[7];
	if (planningType==sim_holonomicpathplanning_xy)
	{
		pos(0)+=vect[0];
		pos(1)+=vect[1];
		auxVect[0]=pos(0);
		auxVect[1]=pos(1);
	}
	else if (planningType==sim_holonomicpathplanning_xg)
	{
		pos(0)+=vect[0];
		orient=orient*_gammaAxisRotation*(C4Vector(C3Vector(0.0f,0.0f,vect[1]))*_gammaAxisRotationInv);
		auxVect[0]=pos(0);
		auxVect[1]=(_gammaAxisRotationInv*orient*_gammaAxisRotation).getEulerAngles()(2);
	}
	else if (planningType==sim_holonomicpathplanning_xyz)
	{
		pos(0)+=vect[0];
		pos(1)+=vect[1];
		pos(2)+=vect[2];
		auxVect[0]=pos(0);
		auxVect[1]=pos(1);
		auxVect[2]=pos(2);
	}
	else if (planningType==sim_holonomicpathplanning_xyg)
	{
		pos(0)+=vect[0];
		pos(1)+=vect[1];
		orient=orient*_gammaAxisRotation*(C4Vector(C3Vector(0.0f,0.0f,vect[2]))*_gammaAxisRotationInv);
		auxVect[0]=pos(0);
		auxVect[1]=pos(1);
		auxVect[2]=(_gammaAxisRotationInv*orient*_gammaAxisRotation).getEulerAngles()(2);
	}
	else if (planningType==sim_holonomicpathplanning_abg)
	{
		orient=orient*C4Vector(vect);
		auxVect[0]=orient(0);
		auxVect[1]=orient(1);
		auxVect[2]=orient(2);
		auxVect[3]=orient(3);
	}
	else if (planningType==sim_holonomicpathplanning_xyzg)
	{
		pos(0)+=vect[0];
		pos(1)+=vect[1];
		pos(2)+=vect[2];
		orient=orient*_gammaAxisRotation*(C4Vector(C3Vector(0.0f,0.0f,vect[3]))*_gammaAxisRotationInv);
		auxVect[0]=pos(0);
		auxVect[1]=pos(1);
		auxVect[2]=pos(2);
		auxVect[3]=(_gammaAxisRotationInv*orient*_gammaAxisRotation).getEulerAngles()(2);
	}
	else if (planningType==sim_holonomicpathplanning_xabg)
	{
		pos(0)+=vect[0];
		orient=orient*C4Vector(vect+1);
		auxVect[0]=pos(0);
		auxVect[1]=orient(0);
		auxVect[2]=orient(1);
		auxVect[3]=orient(2);
		auxVect[4]=orient(3);
	}
	else if (planningType==sim_holonomicpathplanning_xyabg)
	{
		pos(0)+=vect[0];
		pos(1)+=vect[1];
		orient=orient*C4Vector(vect+2);
		auxVect[0]=pos(0);
		auxVect[1]=pos(1);
		auxVect[2]=orient(0);
		auxVect[3]=orient(1);
		auxVect[4]=orient(2);
		auxVect[5]=orient(3);
	}
	else // (planningType==sim_holonomicpathplanning_xyzabg)
	{
		pos(0)+=vect[0];
		pos(1)+=vect[1];
		pos(2)+=vect[2];
		orient=orient*C4Vector(vect+3);
		auxVect[0]=pos(0);
		auxVect[1]=pos(1);
		auxVect[2]=pos(2);
		auxVect[3]=orient(0);
		auxVect[4]=orient(1);
		auxVect[5]=orient(2);
		auxVect[6]=orient(3);
	}
	return(!areSomeValuesForbidden(auxVect));
}

int CHolonomicPathPlanning::smoothFoundPath(int steps,int maxTimePerPass)
{ // step specifies the number of passes (each pass is composed by a calculated sub-pass, and some random sub-pass)
	// We first copy foundPath:
    //yhc//printf("smoothFoundPath\n");
    if(custom_function == MPTYPE_RRBT)
    {
        return 1;
    }
	if (steps<2)
		return(1);
	if (invalidData)
		return(0);
	CDummyDummy* startDummy=(CDummyDummy*)_simGetObject(startDummyID);
	if (startDummy==NULL)
		return(0);

	if (foundPath.size()<3)
		return(0);

	if (foundPathSameStraightLineID_forSteppedSmoothing.size()==0)
	{ // the first time we call this routine!
		sameStraightLineNextID_forSteppedSmoothing=0;
		for (int i=0;i<int(foundPath.size());i++)
			foundPathSameStraightLineID_forSteppedSmoothing.push_back(sameStraightLineNextID_forSteppedSmoothing++);
		numberOfRandomConnectionTries_forSteppedSmoothing=steps;
		numberOfRandomConnectionTriesLeft_forSteppedSmoothing=steps;
	}
	int startTime=simGetSystemTimeInMs(-1);
	while (true)
	{
		if (numberOfRandomConnectionTriesLeft_forSteppedSmoothing<=0)
		{ // we finished smoothing!
			return(1); 
		}
		if (_simGetTimeDiffInMs(startTime)>maxTimePerPass)
			return(-1); // we are not yet finished, but we did enough for the time we had
		numberOfRandomConnectionTriesLeft_forSteppedSmoothing--;
		int lowIndex,highIndex;
		CHolonomicPathNode* startP;
		CHolonomicPathNode* endP;
		for (int randomPass=0;randomPass<5;randomPass++)
		{ // If randomPass==0, the pass is not random, i.e. the low and high indices are calculated
			startP=NULL; // added on 2010/09/09
			if (randomPass==0)
			{ // We calculate lowIndex and highIndex!
				float span=float(foundPath.size())/float(numberOfRandomConnectionTries_forSteppedSmoothing);
				while ( (span<5)&&(numberOfRandomConnectionTries_forSteppedSmoothing>1) )
				{
					numberOfRandomConnectionTries_forSteppedSmoothing--;
					if (numberOfRandomConnectionTriesLeft_forSteppedSmoothing>=numberOfRandomConnectionTries_forSteppedSmoothing)
						numberOfRandomConnectionTriesLeft_forSteppedSmoothing=numberOfRandomConnectionTries_forSteppedSmoothing-1;
					span=float(foundPath.size())/float(numberOfRandomConnectionTries_forSteppedSmoothing);
				}
				if (numberOfRandomConnectionTries_forSteppedSmoothing<=1)
					return(1); // finished!
				lowIndex=int(span*float(numberOfRandomConnectionTriesLeft_forSteppedSmoothing+0));
				highIndex=int(span*float(numberOfRandomConnectionTriesLeft_forSteppedSmoothing+1));
				while (highIndex>=int(foundPath.size())) // probably not needed
					highIndex--;
				if (foundPathSameStraightLineID_forSteppedSmoothing[lowIndex]!=foundPathSameStraightLineID_forSteppedSmoothing[highIndex])
				{ // otherwise this pass is skipped!
					startP=foundPath[lowIndex];
					endP=foundPath[highIndex];
				}
			}
			else
			{ // We randomly chose lowIndex and highIndex!
				for (int i=0;i<5;i++)
				{ // so that if we have only very few ids, we don't stay stuck here
					int ra=int((SIM_RAND_FLOAT*float(foundPath.size()))-0.5f);
					int rb=int((SIM_RAND_FLOAT*float(foundPath.size()))-0.5f);
					if ( (ra!=rb)&&(abs(ra-rb)>1)&&(foundPathSameStraightLineID_forSteppedSmoothing[ra]!=foundPathSameStraightLineID_forSteppedSmoothing[rb]) )
					{
						lowIndex=SIM_MIN(ra,rb);
						highIndex=SIM_MAX(ra,rb);
						startP=foundPath[lowIndex];
						endP=foundPath[highIndex];
						break;
					}
				}
			}
			if (startP!=NULL)
			{ // Now let's try to link highIndex from lowIndex with a "straight" line:
				float vect[7];
				float artificialVectorLength;
				int passes=getVector(startP,endP,vect,stepSize,artificialVectorLength,false);
				if ( (passes!=-1)&&(highIndex-(lowIndex+1)>passes-1) )
				{ // no forbidden direction, and the number of nodes is reduced!
					C3Vector pos;
					C4Vector orient;
					startP->getAllValues(pos,orient);

					C3Vector posCop(pos);
					C4Vector orientCop(orient);
					bool impossible=false;
					C7Vector originalLocal;
					_simGetObjectLocalTransformation(startDummy,originalLocal.X.data,originalLocal.Q.data,true);
					for (int currentPass=0;currentPass<passes-1;currentPass++)
					{
						if (!addVector(pos,orient,vect))
						{
							impossible=true;
							break; // We are in forbidden values!
						}
						C7Vector transf(orient,pos);
						C7Vector tmpTr(_startDummyLTM*transf);
						_simSetObjectLocalTransformation(startDummy,tmpTr.X.data,tmpTr.Q.data);
						if (doCollide(NULL))
						{
							impossible=true;
							break;
						}
					}
					_simSetObjectLocalTransformation(startDummy,originalLocal.X.data,originalLocal.Q.data);
					pos=posCop;
					orient=orientCop;
					if (!impossible)
					{ // Path was collision free:
						// We delete the nodes between low and high, then remove some, to have to correct desired length:
						for (int i=lowIndex+1;i<highIndex;i++)
							delete foundPath[i];
						int a=lowIndex+1;
						int b=lowIndex+1+highIndex-(lowIndex+1)-(passes-1)-0;
						foundPath.erase(foundPath.begin()+a,foundPath.begin()+b);
						foundPathSameStraightLineID_forSteppedSmoothing.erase(foundPathSameStraightLineID_forSteppedSmoothing.begin()+a,foundPathSameStraightLineID_forSteppedSmoothing.begin()+b);

						for (int currentPass=0;currentPass<passes-1;currentPass++)
						{
							addVector(pos,orient,vect);
							CHolonomicPathNode* it=endP->copyYourself(); // just to have the right size!
							it->setAllValues(pos,orient);
							foundPath[lowIndex+1+currentPass]=it;
							foundPathSameStraightLineID_forSteppedSmoothing[lowIndex+1+currentPass]=sameStraightLineNextID_forSteppedSmoothing;
						}
						sameStraightLineNextID_forSteppedSmoothing++;
					}
				}
			}
		}
	}
	return(0); // will never pass here!
}

void CHolonomicPathPlanning::YHC_RRBT_getPathData(std::vector<float>& data)
{
    std::vector<CBelief*> stackPath;
    CHolonomicPathNode *goalNode = CStaticData::savedGoal;
    CBelief *cur_belief;
    CBelief *min_belief;



    if(goalNode->belief_node.size() == 0)
    {
        return;
    }
    min_belief = goalNode->belief_node[0];
    for(int i=1;i<goalNode->belief_node.size();i++)
    {
        int cmp = min_belief->compareCostWith(goalNode->belief_node[i]);
        if(cmp == 1)
            min_belief = goalNode->belief_node[i];
    }

    cur_belief = min_belief;
    while(cur_belief->parent)
    {
        stackPath.push_back(cur_belief);
        cur_belief = cur_belief->parent;
    }
    stackPath.push_back(cur_belief);

    for(int i=stackPath.size()-1;i>=0;i--)
    {
        C3Vector p;
        C4Vector o;
        stackPath[i]->node->getAllValues(p,o);

        C7Vector conf(o,p);
        data.push_back(conf(0));
        data.push_back(conf(1));
        data.push_back(conf(2));
        data.push_back(conf(3));
        data.push_back(conf(4));
        data.push_back(conf(5));
        data.push_back(conf(6));
    }
}

void CHolonomicPathPlanning::getPathData(std::vector<float>& data)
{
    //yhc//printf("getPathData\n");

    if(custom_function == MPTYPE_RRBT)
    {
        YHC_RRBT_getPathData(data);
        return;
    }

	data.clear();
	if (invalidData)
		return;
	for (int i=0;i<int(foundPath.size());i++)
	{
		CHolonomicPathNode* theNode=foundPath[i];
		C3Vector p;
		C4Vector o;
		theNode->getAllValues(p,o);

		C7Vector conf(o,p);
		conf=_startDummyCTM*conf;
		data.push_back(conf(0));
		data.push_back(conf(1));
		data.push_back(conf(2));
		data.push_back(conf(3));
		data.push_back(conf(4));
        data.push_back(conf(5));
		data.push_back(conf(6));
	}
}

bool CHolonomicPathPlanning::areDirectionConstraintsRespected(float vect[7])
{
    ////yhc//printf("areDirectionConstraintsRespected()\n");
	if (planningType==sim_holonomicpathplanning_xy)
	{
		if (!_directionConstraintsOn)
			return(true);
		for (int i=0;i<2;i++)
		{
			if ( (_directionConstraints[i]==-1)&&(vect[i]>0.0f) )
				return(false);
			if ( (_directionConstraints[i]==+1)&&(vect[i]<0.0f) )
				return(false);
		}
		return(true);
	}
	if (planningType==sim_holonomicpathplanning_xg)
	{
		CPathPlanningInterface::getNormalizedAngle(vect[1]);
		if (!_directionConstraintsOn)
			return(true);
		if ( (_directionConstraints[0]==-1)&&(vect[0]>0.0f) )
			return(false);
		if ( (_directionConstraints[0]==+1)&&(vect[0]<0.0f) )
			return(false);
		if ( (_directionConstraints[3]==-1)&&(vect[1]>0.0f) )
			return(false);
		if ( (_directionConstraints[3]==+1)&&(vect[1]<0.0f) )
			return(false);
		return(true);
	}
	if (planningType==sim_holonomicpathplanning_xyz)
	{
		if (!_directionConstraintsOn)
			return(true);
		for (int i=0;i<3;i++)
		{
			if ( (_directionConstraints[i]==-1)&&(vect[i]>0.0f) )
				return(false);
			if ( (_directionConstraints[i]==+1)&&(vect[i]<0.0f) )
				return(false);
		}
		return(true);
	}
	if (planningType==sim_holonomicpathplanning_xyg)
	{
		CPathPlanningInterface::getNormalizedAngle(vect[2]);
		if (!_directionConstraintsOn)
			return(true);
		for (int i=0;i<2;i++)
		{
			if ( (_directionConstraints[i]==-1)&&(vect[i]>0.0f) )
				return(false);
			if ( (_directionConstraints[i]==+1)&&(vect[i]<0.0f) )
				return(false);
		}
		if ( (_directionConstraints[3]==-1)&&(vect[2]>0.0f) )
			return(false);
		if ( (_directionConstraints[3]==+1)&&(vect[2]<0.0f) )
			return(false);
		return(true);
	}
	if (planningType==sim_holonomicpathplanning_abg)
	{ // No direction constraints for A,B,G here!
		return(true);
	}
	if (planningType==sim_holonomicpathplanning_xyzg)
	{
		CPathPlanningInterface::getNormalizedAngle(vect[3]);
		if (!_directionConstraintsOn)
			return(true);
		for (int i=0;i<3;i++)
		{
			if ( (_directionConstraints[i]==-1)&&(vect[i]>0.0f) )
				return(false);
			if ( (_directionConstraints[i]==+1)&&(vect[i]<0.0f) )
				return(false);
		}
		if ( (_directionConstraints[3]==-1)&&(vect[3]>0.0f) )
			return(false);
		if ( (_directionConstraints[3]==+1)&&(vect[3]<0.0f) )
			return(false);
		return(true);
	}
	if (planningType==sim_holonomicpathplanning_xabg)
	{ // No direction constraints for A,B,G here!
		if (!_directionConstraintsOn)
			return(true);
		if ( (_directionConstraints[0]==-1)&&(vect[0]>0.0f) )
			return(false);
		if ( (_directionConstraints[0]==+1)&&(vect[0]<0.0f) )
			return(false);
		return(true);
	}
	if (planningType==sim_holonomicpathplanning_xyabg)
	{ // No direction constraints for A,B,G here!
		if (!_directionConstraintsOn)
			return(true);
		for (int i=0;i<2;i++)
		{
			if ( (_directionConstraints[i]==-1)&&(vect[i]>0.0f) )
				return(false);
			if ( (_directionConstraints[i]==+1)&&(vect[i]<0.0f) )
				return(false);
		}
		return(true);
	}
	if (planningType==sim_holonomicpathplanning_xyzabg)
	{ // No direction constraints for A,B,G here!
		if (!_directionConstraintsOn)
			return(true);
		for (int i=0;i<3;i++)
		{
			if ( (_directionConstraints[i]==-1)&&(vect[i]>0.0f) )
				return(false);
			if ( (_directionConstraints[i]==+1)&&(vect[i]<0.0f) )
				return(false);
		}
		return(true);
	}
	return(true);
}

bool CHolonomicPathPlanning::areSomeValuesForbidden(float values[7])
{
    ////yhc//printf("areSomeValuesForbidden()\n");
	float gamma=0.0f; 
	if (planningType==sim_holonomicpathplanning_xy)
	{
		if ((values[0]<_searchMinVal[0])||(values[0]>_searchMinVal[0]+_searchRange[0]))
			return(true);
		if ((values[1]<_searchMinVal[1])||(values[1]>_searchMinVal[1]+_searchRange[1]))
			return(true);
		return(false);
	}
	if (planningType==sim_holonomicpathplanning_xg)
	{
		if ((values[0]<_searchMinVal[0])||(values[0]>_searchMinVal[0]+_searchRange[0]))
			return(true);
		gamma=values[1];
	}
	if (planningType==sim_holonomicpathplanning_xyz)
	{
		if ((values[0]<_searchMinVal[0])||(values[0]>_searchMinVal[0]+_searchRange[0]))
			return(true);
		if ((values[1]<_searchMinVal[1])||(values[1]>_searchMinVal[1]+_searchRange[1]))
			return(true);
		if ((values[2]<_searchMinVal[2])||(values[2]>_searchMinVal[2]+_searchRange[2]))
			return(true);
		return(false);
	}
	if (planningType==sim_holonomicpathplanning_xyg)
	{
		if ((values[0]<_searchMinVal[0])||(values[0]>_searchMinVal[0]+_searchRange[0]))
			return(true);
		if ((values[1]<_searchMinVal[1])||(values[1]>_searchMinVal[1]+_searchRange[1]))
			return(true);
		gamma=values[2];
	}
	if (planningType==sim_holonomicpathplanning_abg)
	{
		return(false);
	}
	if (planningType==sim_holonomicpathplanning_xyzg)
	{
		if ((values[0]<_searchMinVal[0])||(values[0]>_searchMinVal[0]+_searchRange[0]))
			return(true);
		if ((values[1]<_searchMinVal[1])||(values[1]>_searchMinVal[1]+_searchRange[1]))
			return(true);
		if ((values[2]<_searchMinVal[2])||(values[2]>_searchMinVal[2]+_searchRange[2]))
			return(true);
		gamma=values[3];
	}
	if (planningType==sim_holonomicpathplanning_xabg)
	{
		if ((values[0]<_searchMinVal[0])||(values[0]>_searchMinVal[0]+_searchRange[0]))
			return(true);
		return(false);
	}
	if (planningType==sim_holonomicpathplanning_xyabg)
	{
		if ((values[0]<_searchMinVal[0])||(values[0]>_searchMinVal[0]+_searchRange[0]))
			return(true);
		if ((values[1]<_searchMinVal[1])||(values[1]>_searchMinVal[1]+_searchRange[1]))
			return(true);
		return(false);
	}
	if (planningType==sim_holonomicpathplanning_xyzabg)
	{
		if ((values[0]<_searchMinVal[0])||(values[0]>_searchMinVal[0]+_searchRange[0]))
			return(true);
		if ((values[1]<_searchMinVal[1])||(values[1]>_searchMinVal[1]+_searchRange[1]))
			return(true);
		if ((values[2]<_searchMinVal[2])||(values[2]>_searchMinVal[2]+_searchRange[2]))
			return(true);
		return(false);
	}
	// We check the gamma value here:
	if (_searchRange[3]>(359.0f*degToRad))
		return(false);
	// Search range is smaller than 360 degrees:
	while (gamma>_searchMinVal[3])
		gamma-=piValTimes2;
	while (gamma<_searchMinVal[3])
		gamma+=piValTimes2;
	return(gamma>(_searchMinVal[3]+_searchRange[3]));
}

bool CHolonomicPathPlanning::doCollide(float* dist)
{// dist can be NULL. Dist returns the actual distance only when return value is true!! otherwise it is SIM_MAX_FLOAT!!
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

float CHolonomicPathPlanning::distance(CHolonomicPathNode* from, CHolonomicPathNode* to) {
    float dist = 0.0;
    float vect[3];

    if (planningType==sim_holonomicpathplanning_xy)
    {
            float vect[2];
            vect[0]=from->values[0] - to->values[0];
            vect[1]=from->values[1] - to->values[1];
            float d=vect[0]*vect[0]+vect[1]*vect[1];

            return sqrt(d);
    }
}
