// This file is part of the EXTERNAL IK for V-REP
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The EXTERNAL IK is licensed under the terms of EITHER:
//   1. EXTERNAL IK commercial license (contact us for details)
//   2. EXTERNAL IK educational license (see below)
// 
// EXTERNAL IK educational license:
// -------------------------------------------------------------------
// The EXTERNAL IK educational license applies only to EDUCATIONAL
// ENTITIES composed by following people and institutions:
// 
// 1. Hobbyists, students, teachers and professors
// 2. Schools and universities
// 
// EDUCATIONAL ENTITIES do NOT include companies, research institutions,
// non-profit organisations, foundations, etc.
// 
// An EDUCATIONAL ENTITY may use, modify, compile and distribute the
// modified/unmodified EXTERNAL IK under following conditions:
//  
// 1. Distribution should be free of charge.
// 2. Distribution should be to EDUCATIONAL ENTITIES only.
// 3. Usage should be non-commercial.
// 4. Altered source versions must be plainly marked as such and distributed
//    along with any compiled code.
// 5. The origin of the EXTERNAL IK must not be misrepresented. you must
//    not claim that you wrote the original software.
// 
// THE EXTERNAL IK IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#include "4Vector.h"
#include "MyMath.h"


C4Vector::C4Vector()
{
}

C4Vector::C4Vector(float v0,float v1,float v2,float v3)
{
	data[0]=v0;
	data[1]=v1;
	data[2]=v2;
	data[3]=v3;
	// We don't normalize here
}

C4Vector::C4Vector(const float v[4])
{
	data[0]=v[0];
	data[1]=v[1];
	data[2]=v[2];
	data[3]=v[3];
	// We don't normalize here
}

C4Vector::C4Vector(const C4Vector& q)
{
	data[0]=q(0);
	data[1]=q(1);
	data[2]=q(2);
	data[3]=q(3);
	// We don't normalize here
}

C4Vector::C4Vector(const C3Vector& v)
{ // Alpha, beta and gamma are in radians!
	setEulerAngles(v);
}

C4Vector::C4Vector(float a,float b,float g)
{ // Alpha, beta and gamma are in radians!
	setEulerAngles(a,b,g);
}

C4Vector::C4Vector(float angle,const C3Vector& axis)
{ // Builds a rotation quaternion around axis (angle in radian!)
	setAngleAndAxis(angle,axis);
}

C4Vector::C4Vector(const C3Vector& startV,const C3Vector& endV)
{
	setVectorMapping(startV,endV);
}

C4Vector::~C4Vector()
{

}

void C4Vector::setEulerAngles(float a,float b,float g)
{ // a,b anf g are in radian!
	C4Vector vx(a,C3Vector(1.0f,0.0f,0.0f));
	C4Vector vy(b,C3Vector(0.0f,1.0f,0.0f));
	C4Vector vz(g,C3Vector(0.0f,0.0f,1.0f));
	(*this)=vx*vy*vz;
}

void C4Vector::setEulerAngles(const C3Vector& v)
{ // v(0), v(1) and v(2) are in radian!
	setEulerAngles(v(0),v(1),v(2));
}

void C4Vector::setAngleAndAxis(float angle,const C3Vector& axis)
{ // angle in radian!
	C3Vector axisTmp=axis;
	axisTmp.normalize();
    float sinA=(float)sin(angle/2.0f);
    data[1]=axisTmp(0)*sinA;
    data[2]=axisTmp(1)*sinA;
    data[3]=axisTmp(2)*sinA;
    data[0]=(float)cos(angle/2.0f);
}

void C4Vector::setVectorMapping(const C3Vector& startV,const C3Vector& endV)
{
	C3Vector v0(startV.getNormalized());
	C3Vector v1(endV.getNormalized());
	C3Vector cross(v0^v1);
	float cosAngle=v0*v1;
	if (cosAngle>1.0f)
		setIdentity();
	else
		setAngleAndAxis(CMath::robustAcos(cosAngle),cross);
}

C4Vector C4Vector::getAngleAndAxis() const
{ // Returned vector is (angle,x,y,z) (angle is in radians)
	C4Vector retV;
	C4Vector d(*this);
	if (d(0)<0.0f)	// Condition added on 2009/02/26
		d=d*-1.0f;
	float l=sqrtf(d(0)*d(0)+d(1)*d(1)+d(2)*d(2)+d(3)*d(3));
	float cosA=d(0)/l; // Quaternion needs to be normalized
	if (cosA>1.0f) // Just make sure..
		cosA=1.0f;
	retV(0)=CMath::robustAcos(cosA)*2.0f;
	float sinA=sqrtf(1.0f-cosA*cosA); 
	if (fabs(sinA)<0.00005f)
		sinA=1.0f;
	else
		sinA*=l; // Quaternion needs to be normalized
	retV(1)=d(1)/sinA;
	retV(2)=d(2)/sinA;	
	retV(3)=d(3)/sinA;
	return(retV);
}

C4Vector C4Vector::getAngleAndAxisNoChecking() const
{ // Returned vector is (angle,x,y,z) (angle is in radians)
	C4Vector retV;
	C4Vector d(*this);
	if (d(0)<0.0f)	// Condition added on 2009/02/26
		d=d*-1.0f;
	float l=sqrtf(d(0)*d(0)+d(1)*d(1)+d(2)*d(2)+d(3)*d(3));
	float cosA=d(0)/l; // Quaternion needs to be normalized
	if (cosA>1.0f) // Just make sure..
		cosA=1.0f;
	retV(0)=acos(cosA)*2.0f;
	float sinA=sqrtf(1.0f-cosA*cosA); 
	if (fabs(sinA)<0.00005f)
		sinA=1.0f;
	else
		sinA*=l; // Quaternion needs to be normalized
	retV(1)=d(1)/sinA;
	retV(2)=d(2)/sinA;	
	retV(3)=d(3)/sinA;
	return(retV);
}


C3Vector C4Vector::getEulerAngles() const
{ // angles are in radians!
	return(getMatrix().getEulerAngles());
}


float C4Vector::getAngleBetweenQuaternions(const C4Vector& q) const
{
	float angle=fabs(data[0]*q(0)+data[1]*q(1)+data[2]*q(2)+data[3]*q(3));
	return(CMath::robustAcos(angle)*2.0f);
}

void C4Vector::buildInterpolation(const C4Vector& fromThis,const C4Vector& toThat,float t)
{
	C4Vector AA(fromThis);
	C4Vector BB(toThat);
	if (AA(0)*BB(0)+AA(1)*BB(1)+AA(2)*BB(2)+AA(3)*BB(3)<0.0f)
		AA=AA*-1.0f;
	C4Vector r((AA.getInverse()*BB).getAngleAndAxis());
	(*this)=(AA*C4Vector(r(0)*t,C3Vector(r(1),r(2),r(3))));
	// Already normalized through * operator
}


const C4Vector C4Vector::identityRotation(1.0f,0.0f,0.0f,0.0f);
