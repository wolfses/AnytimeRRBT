#pragma once

#include <Eigen/Dense>
#include <vector>

using Eigen::MatrixXd;

#define YPSILON_THRESHOLD 0.00001
#define ALPHA 0.9
#define BETA 0.1
class CHolonomicPathNode;
class CBelief
{
public:

    CBelief(){}
    ~CBelief(){}

    MatrixXd sigma;
    MatrixXd lamda;
    //float sigma;
    //float lamda;
    float speed;
    float time;
    float cost;
    CHolonomicPathNode *node;
    CBelief *parent;
    std::vector<CBelief*> children;

    /*void DeleteChild(CBelief* target)
    {
        for(std::vector<CBelief*>::iterator it = children->begin();
            it != children->end();++it)
        {
            if(*it == target)
            {
                children->erase(it);
                break;
            }
        }
    }*/

    bool isParent(CBelief *bef)
    {
        CBelief *tar = this;
        while(tar)
        {
            if(tar == bef)
                return true;
            tar = tar->parent;
        }
        return false;
    }

    int compareCostWith(CBelief *c)
    {
        if((cost == c->cost))
            return 0;

        if((cost >= c->cost))
            return 1;

        if((cost <= c->cost))
            return -1;

        return -2;
    }

    int compareWithLM(CBelief *c)
    {
        float cur_cost = (ALPHA * sigma.norm() + BETA * cost);
        float tar_cost = (ALPHA * (c->sigma.norm()+YPSILON_THRESHOLD) + BETA * c->cost);
        if(cur_cost == tar_cost)
            return 0;
        if(cur_cost >= tar_cost)
            return 1;
        if(cur_cost < tar_cost)
            return -1;
        return -2;
    }

    int compareWithLM_Marginal(CBelief *c)
    {
        float cur_cost = (ALPHA * sigma.norm() + BETA * cost);
        float tar_cost = (ALPHA * (c->sigma.norm()+YPSILON_THRESHOLD) + BETA * c->cost);
        if(cur_cost == tar_cost + 0.01)
            return 0;
        if(cur_cost >= tar_cost + 0.01)
            return 1;
        if(cur_cost < tar_cost + 0.01)
            return -1;
        return -2;
    }

    int compareWith(CBelief *c)
    {

        if((sigma.norm() == c->sigma.norm()) &&
           //(lamda.norm() == c->lamda.norm()) &&
           (cost == c->cost))
            return 0;

        if((sigma.norm() + YPSILON_THRESHOLD >= c->sigma.norm()) &&
           //(lamda.norm() + YPSILON_THRESHOLD >= c->lamda.norm()) &&
           (cost >= c->cost))
            return 1;

        if((sigma.norm() <= c->sigma.norm() + YPSILON_THRESHOLD) &&
           //(lamda.norm() <= c->lamda.norm() + YPSILON_THRESHOLD) &&
           (cost <= c->cost))
            return -1;

        return -2;
    }

    bool operator == (CBelief &c)
    {
        /*if(sigma[0][0] != c.sigma[0][0])    return false;
        if(sigma[0][1] != c.sigma[0][1])    return false;
        if(sigma[1][0] != c.sigma[1][0])    return false;
        if(sigma[1][1] != c.sigma[1][1])    return false;

        if(lamda[0][0] != c.lamda[0][0])    return false;
        if(lamda[0][1] != c.lamda[0][1])    return false;
        if(lamda[1][0] != c.lamda[1][0])    return false;
        if(lamda[1][1] != c.lamda[1][1])    return false;*/

        if(sigma.norm() != c.sigma.norm())                return false;
        if(lamda.norm() != c.lamda.norm())                return false;
        if(cost != c.cost)                  return false;
        return true;
    }

    bool operator != (CBelief &c)
    {
        /*if(sigma[0][0] != c.sigma[0][0])    return true;
        if(sigma[0][1] != c.sigma[0][1])    return true;
        if(sigma[1][0] != c.sigma[1][0])    return true;
        if(sigma[1][1] != c.sigma[1][1])    return true;

        if(lamda[0][0] != c.lamda[0][0])    return true;
        if(lamda[0][1] != c.lamda[0][1])    return true;
        if(lamda[1][0] != c.lamda[1][0])    return true;
        if(lamda[1][1] != c.lamda[1][1])    return true;*/
        if(sigma.norm() != c.sigma.norm())                return true;
        if(lamda.norm() != c.lamda.norm())                return true;
        if(cost != c.cost)                  return true;
        return false;
    }

    bool operator >  (CBelief &c)
    {
        /*if(sigma[0][0] < c->sigma[0][0])    return false;
        if(sigma[0][1] < c->sigma[0][1])    return false;
        if(sigma[1][0] < c->sigma[1][0])    return false;
        if(sigma[1][1] < c->sigma[1][1])    return false;

        if(lamda[0][0] < c->lamda[0][0])    return false;
        if(lamda[0][1] < c->lamda[0][1])    return false;
        if(lamda[1][0] < c->lamda[1][0])    return false;
        if(lamda[1][1] < c->lamda[1][1])    return false;*/
        if(sigma.norm() < c.sigma.norm())                return false;
        if(lamda.norm() < c.lamda.norm())                return false;
        if(cost < c.cost)                  return false;

        printf("belief_node[i] > belief\n");
        printf("cost %f > %f\n",cost,c.cost);
        printf("sigma %f > %f\n",sigma.norm(),c.sigma.norm());
        return true;
    }

    //bool operator >= (CBelief &c);
    bool operator <  (CBelief &c)
    {
        /*if(sigma(0,0) > c.sigma(0,0))    return false;
        if(sigma(0,1) > c.sigma(0,1))    return false;
        if(sigma(1,0) > c.sigma(1,0))    return false;
        if(sigma(1,1) > c.sigma(1,1))    return false;

        if(lamda(0,0) > c.lamda(0,0))    return false;
        if(lamda(0,1) > c.lamda(0,1))    return false;
        if(lamda(1,0) > c.lamda(1,0))    return false;
        if(lamda(1,1) > c.lamda(1,1))    return false;*/
        if(sigma.norm() > c.sigma.norm())  return false;
        if(lamda.norm() > c.lamda.norm())  return false;
        if(cost > c.cost)                  return false;
        return true;
    }

    //bool operator <= (CBelief &c);

    //float sigma[2][2];
    //float lamda[2][2];
};

