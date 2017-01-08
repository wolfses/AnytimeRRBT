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

#include <stdio.h>
#include <algorithm>
#include "DubinPath.h"
#include "v_repLib.h"
#include <math.h>


#define dubin_assert(x) {if(!(x)) return DUBINS_ERR;}

CDubinPath::CDubinPath()
{
}

CDubinPath::CDubinPath(double x1,double y1,double t1,double x2,double y2,double t2)
{
	double d,alpha,beta;
	double dx = (x2 - x1),
		   dy = (y2 - y1),
		   offset = atan2(dy,dx);

	t1=mod2pi(t1);
	t2=mod2pi(t2);

	xyt[0] = x1;
	xyt[1] = y1;
	xyt[2] = t1;
	d = sqrt(dx*dx+dy*dy) / rho_;
	alpha = mod2pi(t1 - offset);
	beta = mod2pi(t2 - offset);

	CDubinPath_(d,alpha,beta);
}

void CDubinPath::CDubinPath_(double d, double alpha, double beta)
{
	if(d<DUBINS_EPS && fabs(alpha-beta)<DUBINS_EPS)
	{
		setValue(0,d,0);
		type[0] = DUBINS_LEFT;
		type[1] = DUBINS_STRAIGHT;
		type[2] = DUBINS_LEFT;
		return;
	}

	double len,len_tmp;
	double st,sp,sq;
	int dubin_type;

	len = dubinsLSL(d,alpha,beta);
	st = value[0],sp = value[1],sq = value[2];
	dubin_type = 0;
	
	len_tmp = dubinsRSR(d,alpha,beta);
	if(len_tmp < len)
	{
		len = len_tmp;
		st = value[0],sp = value[1],sq = value[2];
		dubin_type = 1;
	}
	len_tmp = dubinsRSL(d,alpha,beta);
	if(len_tmp < len)
	{
		len = len_tmp;
		st = value[0],sp = value[1],sq = value[2];
		dubin_type = 2;
	}
	len_tmp = dubinsLSR(d,alpha,beta);
	if(len_tmp < len)
	{
		len = len_tmp;
		st = value[0],sp = value[1],sq = value[2];
		dubin_type = 3;
	}
	len_tmp = dubinsRLR(d,alpha,beta);
	if(len_tmp < len)
	{
		len = len_tmp;
		st = value[0],sp = value[1],sq = value[2];
		dubin_type = 4;
	}
	len_tmp = dubinsLRL(d,alpha,beta);
	if(len_tmp < len)
	{
		len = len_tmp;
		st = value[0],sp = value[1],sq = value[2];
		dubin_type = 5;
	}
	value[0] = st,value[1] = sp,value[2] = sq;
	switch(dubin_type)
	{
	case 0: type[0] = DUBINS_LEFT;
			type[1] = DUBINS_STRAIGHT;
			type[2] = DUBINS_LEFT;break;
	case 1: type[0] = DUBINS_RIGHT;
			type[1] = DUBINS_STRAIGHT;
			type[2] = DUBINS_RIGHT;break;
	case 2: type[0] = DUBINS_RIGHT;
			type[1] = DUBINS_STRAIGHT;
			type[2] = DUBINS_LEFT;break;
	case 3: type[0] = DUBINS_LEFT;
			type[1] = DUBINS_STRAIGHT;
			type[2] = DUBINS_RIGHT;break;
	case 4: type[0] = DUBINS_RIGHT;
			type[1] = DUBINS_LEFT;
			type[2] = DUBINS_RIGHT;break;
	case 5: type[0] = DUBINS_LEFT;
			type[1] = DUBINS_RIGHT;
			type[2] = DUBINS_LEFT;break;
	default:type[0] = type[1] = type[2] = DUBINS_TYPE_ERR;
			printf("DUBINS_TYPE_ERR\n");
	
	}
}


void CDubinPath::interpolate(double dt, double *result)
{
    //StateType *s  = allocState()->as<StateType>();
    double x,y,t;
    double seg = dt * length(), phi, v;

    //s->setXY(0., 0.);
	x = 0,y = 0,t = xyt[2];
    //s->setYaw(from->as<StateType>()->getYaw());

    for (unsigned int i=0; i<3 && seg>0; ++i)
    {
        v = std::min(seg, value[i]);
        phi = t;
        seg -= v;
        switch(type[i])
        {
            case DUBINS_LEFT:
                //s->setXY(s->getX() + sin(phi+v) - sin(phi), s->getY() - cos(phi+v) + cos(phi));                
                //s->setYaw(phi+v);
                x = x + sin(phi+v) - sin(phi);
				y = y - cos(phi+v) + cos(phi);
                t = phi + v;
                break;
            case DUBINS_RIGHT:
                //s->setXY(s->getX() - sin(phi-v) + sin(phi), s->getY() + cos(phi-v) - cos(phi));
                //s->setYaw(phi-v);
                x = x - sin(phi-v) + sin(phi);
                y = y + cos(phi-v) - cos(phi);
                t = phi - v;
                break;
            case DUBINS_STRAIGHT:
                //s->setXY(s->getX() + v * cos(phi), s->getY() + v * sin(phi));
                x = x + v * cos(phi);
                y = y + v * sin(phi);
                break;
        }
    }
	
	result[0] = x * rho_ + xyt[0];
	result[1] = y * rho_ + xyt[1];
	result[2] = mod2pi(t);
}

double CDubinPath::mod2pi(double x)
{
    if (x<0 && x>DUBINS_ZERO) return 0;
    return x - twopi * floor(x / twopi);
}
void CDubinPath::setValue(double t,double p,double q)
{
	value[0] = t;
	value[1] = p;
	value[2] = q;
}
double CDubinPath::dubinsLSL(double d, double alpha, double beta)
{
    double ca = cos(alpha), sa = sin(alpha), cb = cos(beta), sb = sin(beta);
    double tmp = 2. + d*d - 2.*(ca*cb +sa*sb - d*(sa - sb));
    if (tmp >= DUBINS_ZERO)
    {
        double theta = atan2(cb - ca, d + sa - sb);
        double t = mod2pi(-alpha + theta);
        double p = sqrt(std::max(tmp, 0.));
        double q = mod2pi(beta - theta);

		dubin_assert(fabs(p*cos(alpha + t) - sa + sb - d) < DUBINS_EPS);
		dubin_assert(fabs(p*sin(alpha + t) + ca - cb) < DUBINS_EPS);
		dubin_assert(mod2pi(alpha + t + q - beta + .5 * DUBINS_EPS) < DUBINS_EPS);

		setValue(t,p,q);		
		type[0] = DUBINS_LEFT;
		type[1] = DUBINS_STRAIGHT;
		type[2] = DUBINS_LEFT;
		return length();
    }
	else
		return DUBINS_ERR;
}

double CDubinPath::dubinsRSR(double d, double alpha, double beta)
{
    double ca = cos(alpha), sa = sin(alpha), cb = cos(beta), sb = sin(beta);
    double tmp = 2. + d*d - 2.*(ca*cb + sa*sb - d*(sb - sa));
    if (tmp >= DUBINS_ZERO)
    {
        double theta = atan2(ca - cb, d - sa + sb);
        double t = mod2pi(alpha - theta);
        double p = sqrt(std::max(tmp, 0.));
        double q = mod2pi(-beta + theta);
		
		dubin_assert(fabs(p*cos(alpha - t) + sa - sb - d) < DUBINS_EPS);		
		dubin_assert(fabs(p*sin(alpha - t) - ca + cb) < DUBINS_EPS);
		dubin_assert(mod2pi(alpha - t - q - beta + .5 * DUBINS_EPS) < DUBINS_EPS);
		
        setValue(t,p,q);		
		type[0] = DUBINS_RIGHT;
		type[1] = DUBINS_STRAIGHT;
		type[2] = DUBINS_RIGHT;
		return length();
    }   
	else
		return DUBINS_ERR;
}

double CDubinPath::dubinsRSL(double d, double alpha, double beta)
{
    double ca = cos(alpha), sa = sin(alpha), cb = cos(beta), sb = sin(beta);
    double tmp = d * d - 2. + 2. * (ca*cb + sa*sb - d * (sa + sb));
    if (tmp >= DUBINS_ZERO)
    {
        double p = sqrt(std::max(tmp, 0.));
        double theta = atan2(ca + cb, d - sa - sb) - atan2(2., p);
        double t = mod2pi(alpha - theta);
        double q = mod2pi(beta - theta);
        
		dubin_assert(fabs(p*cos(alpha - t) - 2. * sin(alpha - t) + sa + sb - d) < DUBINS_EPS);
		dubin_assert(fabs(p*sin(alpha - t) + 2. * cos(alpha - t) - ca - cb) < DUBINS_EPS);
		dubin_assert(mod2pi(alpha - t + q - beta + .5 * DUBINS_EPS) < DUBINS_EPS);

        setValue(t,p,q);		
		type[0] = DUBINS_RIGHT;
		type[1] = DUBINS_STRAIGHT;
		type[2] = DUBINS_LEFT;
		return length();
    }
	else
		return DUBINS_ERR;
}

double CDubinPath::dubinsLSR(double d, double alpha, double beta)
{
    double ca = cos(alpha), sa = sin(alpha), cb = cos(beta), sb = sin(beta);
    double tmp = -2. + d * d + 2. * (ca*cb + sa*sb + d * (sa + sb));
    if (tmp >= DUBINS_ZERO)
    {
        double p = sqrt(std::max(tmp, 0.));
        double theta = atan2(-ca - cb, d + sa + sb) - atan2(-2., p);
        double t = mod2pi(-alpha + theta);
        double q = mod2pi(-beta + theta);
        
		dubin_assert(fabs(p*cos(alpha + t) + 2. * sin(alpha + t) - sa - sb - d) < DUBINS_EPS);
		dubin_assert(fabs(p*sin(alpha + t) - 2. * cos(alpha + t) + ca + cb) < DUBINS_EPS);
		dubin_assert(mod2pi(alpha + t - q - beta + .5 * DUBINS_EPS) < DUBINS_EPS);
		
        setValue(t,p,q);		
		type[0] = DUBINS_LEFT;
		type[1] = DUBINS_STRAIGHT;
		type[2] = DUBINS_RIGHT;
		return length();
    }
	else
		return DUBINS_ERR;
}

double CDubinPath::dubinsRLR(double d, double alpha, double beta)
{
    double ca = cos(alpha), sa = sin(alpha), cb = cos(beta), sb = sin(beta);
    double tmp = .125 * (6. - d * d  + 2. * (ca*cb + sa*sb + d * (sa - sb)));
    if (fabs(tmp) < 1.)
    {
        double p = twopi - acos(tmp);
        double theta = atan2(ca - cb, d - sa + sb);
        double t = mod2pi(alpha - theta + .5 * p);
        double q = mod2pi(alpha - beta - t + p);
        
		dubin_assert(fabs( 2.*sin(alpha - t + p) - 2. * sin(alpha - t) - d + sa - sb) < DUBINS_EPS);
		dubin_assert(fabs(-2.*cos(alpha - t + p) + 2. * cos(alpha - t) - ca + cb) < DUBINS_EPS);
		dubin_assert(mod2pi(alpha - t + p - q - beta + .5 * DUBINS_EPS) < DUBINS_EPS);
        setValue(t,p,q);		
		type[0] = DUBINS_RIGHT;
		type[1] = DUBINS_LEFT;
		type[2] = DUBINS_RIGHT;
		return length();
    }
	else
		return DUBINS_ERR;
}

double CDubinPath::dubinsLRL(double d, double alpha, double beta)
{
    double ca = cos(alpha), sa = sin(alpha), cb = cos(beta), sb = sin(beta);
    double tmp = .125 * (6. - d * d  + 2. * (ca*cb + sa*sb - d * (sa - sb)));
    if (fabs(tmp) < 1.)
    {
        double p = twopi - acos(tmp);
        double theta = atan2(-ca + cb, d + sa - sb);
        double t = mod2pi(-alpha + theta + .5 * p);
        double q = mod2pi(beta - alpha - t + p);
        
		dubin_assert(fabs(-2.*sin(alpha + t - p) + 2. * sin(alpha + t) - d - sa + sb) < DUBINS_EPS);
		dubin_assert(fabs( 2.*cos(alpha + t - p) - 2. * cos(alpha + t) + ca - cb) < DUBINS_EPS);
		dubin_assert(mod2pi(alpha + t - p + q - beta + .5 * DUBINS_EPS) < DUBINS_EPS);
		
        setValue(t,p,q);		
		type[0] = DUBINS_LEFT;
		type[1] = DUBINS_RIGHT;
		type[2] = DUBINS_LEFT;
		return length();
    }
	else
		return DUBINS_ERR;
}
