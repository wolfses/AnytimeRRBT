/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2010, Rice University
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Rice University nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Author: Mark Moll */

#include <algorithm>
#include "ShootingPath.h"
#include "v_repLib.h"
#include <math.h>



CShootingPath::CShootingPath()
{
	
}


CShootingPath::CShootingPath(double theta1,double _x1, double _y1, double _x2, double _y2)
{
	int try_cnt = 100;
	double theta;
	double _min_r=min_r,_max_r=max_r;
	x1 = _x1;
	x2 = _x2;
	y1 = _y1;
	y2 = _y2;
	double theta2 = atan2(y2-y1,x2-x1);
	double dist = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
	//3사분면 -> 2사분면으로 이동  //4사분면 -> 1사분면으로 이동
	if(fmod(twopi + theta2 - theta1 + pi/2,twopi) > pi)
	{
		double theta = -fmod(twopi + theta2 - theta1 + pi/2,twopi);
		
		//printf("move1 %f\t%f\t%f\t | %f\t%f\t",x1,y1,theta1,x2,y2);
		x2 = x1 + dist * cos(theta1 + theta - (pi / 2));
		y2 = y1 + dist * sin(theta1 + theta - (pi / 2));
		theta2 = atan2(y2-y1,x2-x1);
		//printf("to %f\t%f\n",x2,y2);
	}
	//Min radius보다 작으면
	if(fmod(twopi + theta2 - theta1 + pi/2,twopi) > pi/2)//2사분면
	{
		coor = 2;
		x0 = x1 - r * sin(theta1);
		y0 = y1 + r * cos(theta1);
		if( (x2 - x0)*(x2 - x0) + (y2 - y0)*(y2 - y0) > min_r * min_r )
		{
			for(int i=0;i<try_cnt;i++)
			{
				r = (_min_r + _max_r) / 2;
				x0 = x1 - r * sin(theta1);
				y0 = y1 + r * cos(theta1);

				if(fabs((x2 - x0)*(x2 - x0) + (y2 - y0)*(y2 - y0) - r * r) < 1e-4)
					break;
				else if( (x2 - x0)*(x2 - x0) + (y2 - y0)*(y2 - y0) > r * r)
					_min_r = r;
				else if( (x2 - x0)*(x2 - x0) + (y2 - y0)*(y2 - y0) < r * r)
					_max_r = r;
			}
		}
	}
	else
	{
		coor = 1;
		x0 = x1 + r * sin(theta1);
		y0 = y1 - r * cos(theta1);
		if( (x2 - x0)*(x2 - x0) + (y2 - y0)*(y2 - y0) > min_r * min_r )
		{
			for(int i=0;i<try_cnt;i++)
			{
				r = -(_min_r + _max_r) / 2;
				x0 = x1 - r * sin(theta1);
				y0 = y1 + r * cos(theta1);

				if(fabs( (x2 - x0)*(x2 - x0) + (y2 - y0)*(y2 - y0) - r * r) < 1e-4)
					break;
				else if( (x2 - x0)*(x2 - x0) + (y2 - y0)*(y2 - y0) > r * r)
					_min_r = -r;
				else if( (x2 - x0)*(x2 - x0) + (y2 - y0)*(y2 - y0) < r * r)
					_max_r = -r;
			}
			r = -r;
		}
	}
	theta = atan2(y2-y0,x2-x0);
	x2 = x0 + r * cos(theta);
	y2 = y0 + r * sin(theta);
	t1 = theta1;
	if(coor == 1)
	{
		t2 = theta - pi/2;		
		d_theta = mod2pi(t1-t2);
	}
	else
	{
		t2 = theta + pi/2;
		d_theta = mod2pi(t2-t1);
	}		
	

	//printf("here %f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",x1,y1,t1,x2,y2,t2);
	//printf("to %f\t%f\n",x2,y2);
}


void CShootingPath::interpolate(double dt, double *result)
{
//	;d_theta * dt;
	double theta;

	if(coor == 1)
	{
		theta = atan2(y1-y0,x1-x0);
		result[0] = x0 + r * cos(theta - d_theta * dt);
		result[1] = y0 + r * sin(theta - d_theta * dt);
		result[2] = theta - d_theta * dt - pi/2;
	}
	else
	{
		theta = atan2(y1-y0,x1-x0);
		result[0] = x0 + r * cos(theta + d_theta * dt);
		result[1] = y0 + r * sin(theta + d_theta * dt);
		result[2] = theta + d_theta * dt + pi/2;
	}
}

double CShootingPath::mod2pi(double x)
{
    if (x<0 && x>SHOOTING_ZERO) return 0;
    return x - twopi * floor(x / twopi);
}
/*
DubinsStateSpace::DubinsPath dubins(double d, double alpha, double beta)
{
    if (d<DUBINS_EPS && fabs(alpha-beta)<DUBINS_EPS)
        return DubinsStateSpace::DubinsPath(DubinsStateSpace::dubinsPathType[0], 0, d, 0);

    DubinsStateSpace::DubinsPath path(dubinsLSL(d, alpha, beta)), tmp(dubinsRSR(d, alpha, beta));
    double len, minLength = path.length();

    if ((len = tmp.length()) < minLength)
    {
        minLength = len;
        path = tmp;
    }
    tmp = dubinsRSL(d, alpha, beta);
    if ((len = tmp.length()) < minLength)
    {
        minLength = len;
        path = tmp;
    }
    tmp = dubinsLSR(d, alpha, beta);
    if ((len = tmp.length()) < minLength)
    {
        minLength = len;
        path = tmp;
    }
    tmp = dubinsRLR(d, alpha, beta);
    if ((len = tmp.length()) < minLength)
    {
        minLength = len;
        path = tmp;
    }
    tmp = dubinsLRL(d, alpha, beta);
    if ((len = tmp.length()) < minLength)
        path = tmp;
    return path;
}

	

const ompl::base::DubinsStateSpace::DubinsPathSegmentType
ompl::base::DubinsStateSpace::dubinsPathType[6][3] = {
    { DUBINS_LEFT, DUBINS_STRAIGHT, DUBINS_LEFT },
    { DUBINS_RIGHT, DUBINS_STRAIGHT, DUBINS_RIGHT },
    { DUBINS_RIGHT, DUBINS_STRAIGHT, DUBINS_LEFT },
    { DUBINS_LEFT, DUBINS_STRAIGHT, DUBINS_RIGHT },
    { DUBINS_RIGHT, DUBINS_LEFT, DUBINS_RIGHT },
    { DUBINS_LEFT, DUBINS_RIGHT, DUBINS_LEFT }
};

double ompl::base::DubinsStateSpace::distance(const State *state1, const State *state2) const
{
    if (isSymmetric_)
        return rho_ * std::min(dubins(state1, state2).length(), dubins(state2, state1).length());
    else
        return rho_ * dubins(state1, state2).length();
}

void ompl::base::DubinsStateSpace::interpolate(const State *from, const State *to, const double t, State *state) const
{
    bool firstTime = true;
    DubinsPath path;
    interpolate(from, to, t, firstTime, path, state);
}

void ompl::base::DubinsStateSpace::interpolate(const State *from, const State *to, const double t,
    bool &firstTime, DubinsPath &path, State *state) const
{
    if (firstTime)
    {
        if (t>=1.)
        {
            if (to != state)
                copyState(state, to);
            return;
        }
        if (t<=0.)
        {
            if (from != state)
                copyState(state, from);
            return;
        }

        path = dubins(from, to);
        if (isSymmetric_)
        {
            DubinsPath path2(dubins(to, from));
            if (path2.length() < path.length())
            {
                path2.reverse_ = true;
                path = path2;
            }
        }
        firstTime = false;
    }
    interpolate(from, path, t, state);
}

void ompl::base::DubinsStateSpace::interpolate(const State *from, const DubinsPath &path, double t, State *state) const
{
    StateType *s  = allocState()->as<StateType>();
    double seg = t * path.length(), phi, v;

    s->setXY(0., 0.);
    s->setYaw(from->as<StateType>()->getYaw());
    if (!path.reverse_)
    {
        for (unsigned int i=0; i<3 && seg>0; ++i)
        {
            v = std::min(seg, path.length_[i]);
            phi = s->getYaw();
            seg -= v;
            switch(path.type_[i])
            {
                case DUBINS_LEFT:
                    s->setXY(s->getX() + sin(phi+v) - sin(phi), s->getY() - cos(phi+v) + cos(phi));
                    s->setYaw(phi+v);
                    break;
                case DUBINS_RIGHT:
                    s->setXY(s->getX() - sin(phi-v) + sin(phi), s->getY() + cos(phi-v) - cos(phi));
                    s->setYaw(phi-v);
                    break;
                case DUBINS_STRAIGHT:
                    s->setXY(s->getX() + v * cos(phi), s->getY() + v * sin(phi));
                    break;
            }
        }
    }
    else
    {
        for (unsigned int i=0; i<3 && seg>0; ++i)
        {
            v = std::min(seg, path.length_[2-i]);
            phi = s->getYaw();
            seg -= v;
            switch(path.type_[2-i])
            {
                case DUBINS_LEFT:
                    s->setXY(s->getX() + sin(phi-v) - sin(phi), s->getY() - cos(phi-v) + cos(phi));
                    s->setYaw(phi-v);
                    break;
                case DUBINS_RIGHT:
                    s->setXY(s->getX() - sin(phi+v) + sin(phi), s->getY() + cos(phi+v) - cos(phi));
                    s->setYaw(phi+v);
                    break;
                case DUBINS_STRAIGHT:
                    s->setXY(s->getX() - v * cos(phi), s->getY() - v * sin(phi));
                    break;
            }
        }
    }
    state->as<StateType>()->setX(s->getX() * rho_ + from->as<StateType>()->getX());
    state->as<StateType>()->setY(s->getY() * rho_ + from->as<StateType>()->getY());
    getSubspace(1)->enforceBounds(s->as<SO2StateSpace::StateType>(1));
    state->as<StateType>()->setYaw(s->getYaw());
    freeState(s);
}

ompl::base::DubinsStateSpace::DubinsPath ompl::base::DubinsStateSpace::dubins(const State *state1, const State *state2) const
{
    const StateType *s1 = static_cast<const StateType*>(state1);
    const StateType *s2 = static_cast<const StateType*>(state2);
    double x1 = s1->getX(), y1 = s1->getY(), th1 = s1->getYaw();
    double x2 = s2->getX(), y2 = s2->getY(), th2 = s2->getYaw();
    double dx = x2 - x1, dy = y2 - y1, d = sqrt(dx*dx + dy*dy) / rho_, th = atan2(dy, dx);
    double alpha = mod2pi(th1 - th), beta = mod2pi(th2 - th);
    return ::dubins(d, alpha, beta);
}


void ompl::base::DubinsMotionValidator::defaultSettings()
{
    stateSpace_ = dynamic_cast<DubinsStateSpace*>(si_->getStateSpace().get());
    if (!stateSpace_)
        throw Exception("No state space for motion validator");
}

bool ompl::base::DubinsMotionValidator::checkMotion(const State *s1, const State *s2, std::pair<State*, double> &lastValid) const
{
    // assume motion starts in a valid configuration so s1 is valid 

    bool result = true, firstTime = true;
    DubinsStateSpace::DubinsPath path;
    int nd = stateSpace_->validSegmentCount(s1, s2);

    if (nd > 1)
    {
        // temporary storage for the checked state 
        State *test = si_->allocState();

        for (int j = 1 ; j < nd ; ++j)
        {
            stateSpace_->interpolate(s1, s2, (double)j / (double)nd, firstTime, path, test);
            if (!si_->isValid(test))
            {
                lastValid.second = (double)(j - 1) / (double)nd;
                if (lastValid.first)
                    stateSpace_->interpolate(s1, s2, lastValid.second, firstTime, path, lastValid.first);
                result = false;
                break;
            }
        }
        si_->freeState(test);
    }

    if (result)
        if (!si_->isValid(s2))
        {
            lastValid.second = (double)(nd - 1) / (double)nd;
            if (lastValid.first)
                stateSpace_->interpolate(s1, s2, lastValid.second, firstTime, path, lastValid.first);
            result = false;
        }

    if (result)
        valid_++;
    else
        invalid_++;

    return result;
}

bool ompl::base::DubinsMotionValidator::checkMotion(const State *s1, const State *s2) const
{
    // assume motion starts in a valid configuration so s1 is valid 
    if (!si_->isValid(s2))
        return false;

    bool result = true, firstTime = true;
    DubinsStateSpace::DubinsPath path;
    int nd = stateSpace_->validSegmentCount(s1, s2);

    // initialize the queue of test positions 
    std::queue< std::pair<int, int> > pos;
    if (nd >= 2)
    {
        pos.push(std::make_pair(1, nd - 1));

        // temporary storage for the checked state 
        State *test = si_->allocState();

        // repeatedly subdivide the path segment in the middle (and check the middle) 
        while (!pos.empty())
        {
            std::pair<int, int> x = pos.front();

            int mid = (x.first + x.second) / 2;
            stateSpace_->interpolate(s1, s2, (double)mid / (double)nd, firstTime, path, test);

            if (!si_->isValid(test))
            {
                result = false;
                break;
            }

            pos.pop();

            if (x.first < mid)
                pos.push(std::make_pair(x.first, mid - 1));
            if (x.second > mid)
                pos.push(std::make_pair(mid + 1, x.second));
        }

        si_->freeState(test);
    }

    if (result)
        valid_++;
    else
        invalid_++;

    return result;
}
*/

