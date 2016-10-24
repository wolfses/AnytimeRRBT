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
#include "HolonomicPathNode.h"
#include "pathPlanningInterface.h"
#include "Belief.h"
#include "v_repLib.h"

CHolonomicPathNode::CHolonomicPathNode(const C4Vector& rotAxisRot,const C4Vector& rotAxisRotInv)
{
	_rotAxisRot=rotAxisRot;
	_rotAxisRotInv=rotAxisRotInv;
	parent=NULL;
	_nodeType=-1;
	values=NULL;
}

CHolonomicPathNode::CHolonomicPathNode(int theType,const C7Vector& conf,const C4Vector& rotAxisRot,const C4Vector& rotAxisRotInv)
{
    _rotAxisRot=rotAxisRot;
    _rotAxisRotInv=rotAxisRotInv;
    parent=NULL;
    _nodeType=theType;
    values=NULL;
    int s=getSize();
    values=new float[s];
    setAllValues(conf.X,conf.Q);
}

CHolonomicPathNode::CHolonomicPathNode(int theType,const C7Vector& conf,const C4Vector& rotAxisRot,const C4Vector& rotAxisRotInv,const float time)
{
    _rotAxisRot=rotAxisRot;
    _rotAxisRotInv=rotAxisRotInv;
    parent=NULL;
    _nodeType=theType;
    values=NULL;
    int s=getSize();
    values=new float[s];
    setAllValues(conf.X,conf.Q);
    this->time = time;
}

CHolonomicPathNode::CHolonomicPathNode(int theType,float searchMin[4],float searchRange[4],const C4Vector& rotAxisRot,const C4Vector& rotAxisRotInv)
{
	_rotAxisRot=rotAxisRot;
	_rotAxisRotInv=rotAxisRotInv;
	parent=NULL;
	_nodeType=theType;
	values=NULL;
	if (theType==sim_holonomicpathplanning_xy)
	{
		values=new float[2];
		values[0]=searchMin[0]+searchRange[0]*SIM_RAND_FLOAT;
		values[1]=searchMin[1]+searchRange[1]*SIM_RAND_FLOAT;
	}
	if (theType==sim_holonomicpathplanning_xg)
	{
		values=new float[2];
		values[0]=searchMin[0]+searchRange[0]*SIM_RAND_FLOAT;
		values[1]=CPathPlanningInterface::getNormalizedAngle(searchMin[3]+searchRange[3]*SIM_RAND_FLOAT);
	}
	if (theType==sim_holonomicpathplanning_xyz)
	{
		values=new float[3];
		values[0]=searchMin[0]+searchRange[0]*SIM_RAND_FLOAT;
		values[1]=searchMin[1]+searchRange[1]*SIM_RAND_FLOAT;
		values[2]=searchMin[2]+searchRange[2]*SIM_RAND_FLOAT;
	}
	if (theType==sim_holonomicpathplanning_xyg)
	{
		values=new float[3];
		values[0]=searchMin[0]+searchRange[0]*SIM_RAND_FLOAT;
		values[1]=searchMin[1]+searchRange[1]*SIM_RAND_FLOAT;
		values[2]=CPathPlanningInterface::getNormalizedAngle(searchMin[3]+searchRange[3]*SIM_RAND_FLOAT);
	}
	if (theType==sim_holonomicpathplanning_abg)
	{
		values=new float[4];
		C4Vector d;
		d.buildRandomOrientation();
		values[0]=d(0);
		values[1]=d(1);
		values[2]=d(2);
		values[3]=d(3);
	}
	if (theType==sim_holonomicpathplanning_xyzg)
	{
		values=new float[4];
		values[0]=searchMin[0]+searchRange[0]*SIM_RAND_FLOAT;
		values[1]=searchMin[1]+searchRange[1]*SIM_RAND_FLOAT;
		values[2]=searchMin[2]+searchRange[2]*SIM_RAND_FLOAT;
		values[3]=CPathPlanningInterface::getNormalizedAngle(searchMin[3]+searchRange[3]*SIM_RAND_FLOAT);
	}
	if (theType==sim_holonomicpathplanning_xabg)
	{
		values=new float[5];
		values[0]=searchMin[0]+searchRange[0]*SIM_RAND_FLOAT;
		C4Vector d;
		d.buildRandomOrientation();
		values[1]=d(0);
		values[2]=d(1);
		values[3]=d(2);
		values[4]=d(3);
	}
	if (theType==sim_holonomicpathplanning_xyabg)
	{
		values=new float[6];
		values[0]=searchMin[0]+searchRange[0]*SIM_RAND_FLOAT;
		values[1]=searchMin[1]+searchRange[1]*SIM_RAND_FLOAT;
		C4Vector d;
		d.buildRandomOrientation();
		values[2]=d(0);
		values[3]=d(1);
		values[4]=d(2);
		values[5]=d(3);
	}
	if (theType==sim_holonomicpathplanning_xyzabg)
	{
		values=new float[7];
		values[0]=searchMin[0]+searchRange[0]*SIM_RAND_FLOAT;
		values[1]=searchMin[1]+searchRange[1]*SIM_RAND_FLOAT;
		values[2]=searchMin[2]+searchRange[2]*SIM_RAND_FLOAT;
		C4Vector d;
		d.buildRandomOrientation();
		values[3]=d(0);
		values[4]=d(1);
		values[5]=d(2);
		values[6]=d(3);
	}
}

CHolonomicPathNode::~CHolonomicPathNode()
{
	delete[] values;
}

void CHolonomicPathNode::deleteRecur(CBelief *belief)
{
    for(std::vector<CBelief*>::iterator it = belief->children.begin();
        it != belief->children.end();)
    {
        deleteRecur(*it);
    }
    for(std::vector<CBelief*>::iterator it = belief->parent->children.begin();
        it != belief->parent->children.end();++it)
    {
        if(*it == belief)
        {
            belief->parent->children.erase(it);
            break;
        }
    }

    for(std::vector<CBelief*>::iterator it = belief->node->belief_node.begin();
        it != belief->node->belief_node.end();++it)
    {
        if(*it == belief)
        {
            belief->node->belief_node.erase(it);
            break;
        }
    }
    belief->children.clear();
    belief->parent = 0;
    belief->node = 0;
    delete belief;
}

void CHolonomicPathNode::replaceBelief(CBelief *belief,CBelief *rep, std::vector<CBelief*> *extend_belief)
{
    if(belief->node != rep->node)
    {
        printf("error not same node\n");
        return;
    }
    for(int i=0;i<(int)belief->children.size();i++)
    {
        belief->children[i]->parent = rep;
        rep->children.push_back(belief->children[i]);
    }
    /*int flag = 0;
    for(std::vector<CBelief*>::iterator it = belief->node->belief_node.begin();
        it != belief->node->belief_node.end();++it)
    {
        if(*it == belief)
        {
            belief->node->belief_node.erase(it);
            flag = 1;
            //belief->node->belief_node.push_back(rep);
            break;
        }
    }
    if(flag == 0)
    {
        printf("error here replaceBelief \n");
    }*/
    if(belief->parent)
    {
        for(std::vector<CBelief*>::iterator it = belief->parent->children.begin();
            it != belief->parent->children.end();++it)
        {
            if(*it == belief)
            {
                belief->parent->children.erase(it);
                break;
            }
        }
        if(extend_belief != NULL)
        {
            for(std::vector<CBelief*>::iterator it = extend_belief->begin();
                it != extend_belief->end();++it)
            {
                if(*it == belief)
                {
                    extend_belief->erase(it);
                    break;
                }
            }
        }
    }
    //printf("size of belief nodes = %ld\n",rep->children.size() );
    //printf("bef = %ld, rep = %ld\n",(long)belief,(long)rep);
    belief->children.clear();
    belief->parent = 0;
    belief->node = 0;
    delete belief;
}

bool CHolonomicPathNode::appendBelief(CBelief *belief, std::vector<CBelief*> *extend_belief)
{
    std::vector<CBelief*> new_nodes;
    int i;
    bool ret_value = false;
    bool push_ok = true;
    bool must_push = false;
    belief->node = this;

    for(i=0;i<belief_node.size();i++)
    {
        int cmp = belief_node[i]->compareWith(belief);
        if( cmp == -1 )
        {
            //printf("belief_node[i] < belief\n");
            //printf("cost %f < %f\n",belief_node[i]->cost,belief->cost);
            //printf("sigma %f < %f\n",belief_node[i]->sigma.norm(),belief->sigma.norm());
            push_ok = false;
            new_nodes.push_back(belief_node[i]);
        }
        else if( cmp == 0 )
        {
            //printf("same same\n");
            push_ok = false;
            new_nodes.push_back(belief_node[i]);
        }
        else if( cmp == 1 ) // if new belief is lower than current belief
        {
            //printf("belief_node[i] > belief\n");
            //printf("cost %f > %f\n",belief_node[i]->cost,belief->cost);
            //printf("sigma %f > %f\n",belief_node[i]->sigma.norm(),belief->sigma.norm());

            //new_nodes.push_back(belief_node[i]);
            //del_flag.push_back(i);
            if(!belief->isParent(belief_node[i]))
            {
                replaceBelief(belief_node[i],belief,extend_belief);
                must_push = true;
            }
            else
                new_nodes.push_back(belief_node[i]);
        }
        else if( cmp == -2 )
        {
            int cmp_lm = belief_node[i]->compareWithLM(belief);

            if(new_nodes.size()+1<CHolonomicPathPlanning::maxBeliefNode)
            {
                new_nodes.push_back(belief_node[i]);
            }
            else if( cmp_lm == -1 )
            {
                push_ok = false;
                new_nodes.push_back(belief_node[i]);
            }
            else if( cmp_lm == 0 )
            {
                push_ok = false;
                new_nodes.push_back(belief_node[i]);
            }
            else if( cmp_lm == 1 ) // if new belief is lower than current belief
            {
                if(!belief->isParent(belief_node[i]))
                {
                    replaceBelief(belief_node[i],belief,extend_belief);
                    must_push = true;
                }
                else
                    new_nodes.push_back(belief_node[i]);
            }


            //new_nodes.push_back(belief_node[i]);
        }
    }

    for(int i=0;i<new_nodes.size();i++)
    {
        if(new_nodes[i]->node == 0)
        {
            printf("impossible\n");
        }
    }
    if(push_ok == false && must_push == true)
    {
        //printf("important error\n");
        //for(i=0;i<n;i++)
        //{
            //int cmp = belief_node[i]->compareWith(*belief);
            //printf("cmp = %d\n",cmp);
        //}
    }
    /*while(new_nodes.size() > CHolonomicPathPlanning::maxBeliefNode)
    {
        new_nodes.pop_back();
    }*/
    if(push_ok || must_push )//||(new_nodes.size()<CHolonomicPathPlanning::maxBeliefNode))
    {
        //printf("new belief is added\n");
        belief_node.clear();
        belief_node.assign(new_nodes.begin(),new_nodes.end());
        if(must_push || belief_node.size()<CHolonomicPathPlanning::maxBeliefNode)
        {
            ret_value = true;
            belief_node.push_back(belief);
            if(belief->parent != 0)
                belief->parent->children.push_back(belief);

            if(belief->node != this)
            {
                printf("debug point ckvsa\n");
            }
        }
        if(belief_node.size()>CHolonomicPathPlanning::maxBeliefNode)
        {
            printf("debug point\n");
        }
    }
    else
    {
        if(belief->children.size())
            printf("maybe error. size of belief nodes = %ld\n",belief->children.size() );
        //printf("new belief will be deleted\n");
        delete belief;
    }

    //printf("A");
    //belief_node = new_nodes;

    if(belief_node.size() == 0)
    {
        printf("error. size of belief nodes = %ld\n",belief_node.size() );
    }
    return ret_value;
}

void CHolonomicPathNode::setAllValues(const C3Vector& pos,const C4Vector& orient)
{
	if (_nodeType==sim_holonomicpathplanning_xy)
	{
		values[0]=pos(0);
		values[1]=pos(1);
	}
	if (_nodeType==sim_holonomicpathplanning_xg)
	{
		values[0]=pos(0);
		C4Vector o(_rotAxisRotInv*orient*_rotAxisRot);
		values[1]=o.getEulerAngles()(2);
	}
	if (_nodeType==sim_holonomicpathplanning_xyz)
	{
		values[0]=pos(0);
		values[1]=pos(1);
		values[2]=pos(2);
	}
	if (_nodeType==sim_holonomicpathplanning_xyg)
	{
		values[0]=pos(0);
		values[1]=pos(1);
		C4Vector o(_rotAxisRotInv*orient*_rotAxisRot);
		values[2]=o.getEulerAngles()(2);
	}
	if (_nodeType==sim_holonomicpathplanning_abg)
	{
		values[0]=orient(0);
		values[1]=orient(1);
		values[2]=orient(2);
		values[3]=orient(3);
	}
	if (_nodeType==sim_holonomicpathplanning_xyzg)
	{
		values[0]=pos(0);
		values[1]=pos(1);
		values[2]=pos(2);
		C4Vector o(_rotAxisRotInv*orient*_rotAxisRot);
		values[3]=o.getEulerAngles()(2);
	}
	if (_nodeType==sim_holonomicpathplanning_xabg)
	{
		values[0]=pos(0);
		values[1]=orient(0);
		values[2]=orient(1);
		values[3]=orient(2);
		values[4]=orient(3);
	}
	if (_nodeType==sim_holonomicpathplanning_xyabg)
	{
		values[0]=pos(0);
		values[1]=pos(1);
		values[2]=orient(0);
		values[3]=orient(1);
		values[4]=orient(2);
		values[5]=orient(3);
	}
	if (_nodeType==sim_holonomicpathplanning_xyzabg)
	{
		values[0]=pos(0);
		values[1]=pos(1);
		values[2]=pos(2);
		values[3]=orient(0);
		values[4]=orient(1);
		values[5]=orient(2);
		values[6]=orient(3);
	}
}

void CHolonomicPathNode::getAllValues(C3Vector& pos,C4Vector& orient)
{
	pos.clear();
	orient.setIdentity();
	if (_nodeType==sim_holonomicpathplanning_xy)
	{
		pos(0)=values[0];
		pos(1)=values[1];
	}
	if (_nodeType==sim_holonomicpathplanning_xg)
	{
		pos(0)=values[0];
		orient=_rotAxisRot*(C4Vector(C3Vector(0.0f,0.0f,values[1]))*_rotAxisRotInv);
	}
	if (_nodeType==sim_holonomicpathplanning_xyz)
	{
		pos(0)=values[0];
		pos(1)=values[1];
		pos(2)=values[2];
	}
	if (_nodeType==sim_holonomicpathplanning_xyg)
	{
		pos(0)=values[0];
		pos(1)=values[1];
		orient=_rotAxisRot*(C4Vector(C3Vector(0.0f,0.0f,values[2]))*_rotAxisRotInv);
	}
	if (_nodeType==sim_holonomicpathplanning_abg)
	{
		orient(0)=values[0];
		orient(1)=values[1];
		orient(2)=values[2];
		orient(3)=values[3];
	}
	if (_nodeType==sim_holonomicpathplanning_xyzg)
	{
		pos(0)=values[0];
		pos(1)=values[1];
		pos(2)=values[2];
		orient=_rotAxisRot*(C4Vector(C3Vector(0.0f,0.0f,values[3]))*_rotAxisRotInv);
	}
	if (_nodeType==sim_holonomicpathplanning_xabg)
	{
		pos(0)=values[0];
		orient(0)=values[1];
		orient(1)=values[2];
		orient(2)=values[3];
		orient(3)=values[4];
	}
	if (_nodeType==sim_holonomicpathplanning_xyabg)
	{
		pos(0)=values[0];
		pos(1)=values[1];
		orient(0)=values[2];
		orient(1)=values[3];
		orient(2)=values[4];
		orient(3)=values[5];
	}
	if (_nodeType==sim_holonomicpathplanning_xyzabg)
	{
		pos(0)=values[0];
		pos(1)=values[1];
		pos(2)=values[2];
		orient(0)=values[3];
		orient(1)=values[4];
		orient(2)=values[5];
		orient(3)=values[6];
	}
}

CHolonomicPathNode* CHolonomicPathNode::copyYourself()
{
	CHolonomicPathNode* newNode=new CHolonomicPathNode(_rotAxisRot,_rotAxisRotInv);
	newNode->_nodeType=_nodeType;
    newNode->belief_node = belief_node;
	int s=getSize();
	newNode->values=new float[s];
	for (int i=0;i<s;i++)
		newNode->values[i]=values[i];
	return(newNode);
}

void CHolonomicPathNode::setAllValues(float* v)
{
	int s=getSize();
	for (int i=0;i<s;i++)
		values[i]=v[i];
}

int CHolonomicPathNode::getSize()
{
	if (_nodeType==sim_holonomicpathplanning_xy)
		return(2);
	if (_nodeType==sim_holonomicpathplanning_xg)
		return(2);
	if (_nodeType==sim_holonomicpathplanning_xyz)
		return(3);
	if (_nodeType==sim_holonomicpathplanning_xyg)
		return(3);
	if (_nodeType==sim_holonomicpathplanning_abg)
		return(4);
	if (_nodeType==sim_holonomicpathplanning_xyzg)
		return(4);
	if (_nodeType==sim_holonomicpathplanning_xabg)
		return(5);
	if (_nodeType==sim_holonomicpathplanning_xyabg)
		return(6);
	if (_nodeType==sim_holonomicpathplanning_xyzabg)
		return(7);
	return(0);
}
