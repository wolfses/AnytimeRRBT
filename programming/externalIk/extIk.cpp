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

#include "extIk.h"
#include "Ct.h"
#include "v_repConst.h"

bool hasLaunched()
{
	return(ct::getInstanceNumber()>0);
}

bool simEmbLaunch()
{
	if (hasLaunched())
		return(false);
	ct::createSingletonsAndOneInstance(false); // This has to be done at the very beginning!
	return(true);
}
bool simEmbShutDown()
{
	if (!hasLaunched())
		return(false);
	ct::destroySingletonsAndAllInstances(); // This has to be done at the very end
	return(true);
}

int simEmbStart(unsigned char* data,int dataLength)
{
	if (!hasLaunched())
		return(-1);
	if ((data==NULL)||(dataLength==0))
		return(-1);
	CExtIkSer ar(data,dataLength);
	ct::objCont->importIkContent(ar);
	return(int(ct::objCont->objectList.size()));
}

int simEmbGetObjectHandle(const char* objectName)
{
	if (!hasLaunched())
		return(-1);
	C3DObject* it=ct::objCont->getObject(objectName);
	if (it==NULL)
		return(-1);
	return(it->getID());
}

int simEmbGetObjectTransformation(int objectHandle,int relativeToObjectHandle,float* position,float* quaternion)
{
	if (!hasLaunched())
		return(-1);
	// V-REP quaternion, internally: w x y z
	// V-REP quaternion, at interfaces: x y z w (like ROS)
	C3DObject* it=ct::objCont->getObject(objectHandle);
	if (it==NULL)
		return(-1);
	if (relativeToObjectHandle==sim_handle_parent)
	{
		relativeToObjectHandle=-1;
		C3DObject* parent=it->getParent();
		if (parent!=NULL)
			relativeToObjectHandle=parent->getID();
	}
	C3DObject* relObj=ct::objCont->getObject(relativeToObjectHandle);
	if (relativeToObjectHandle!=-1)
	{
		if (relObj==NULL)
			return(-1);
	}
	C7Vector tr;
	if (relativeToObjectHandle==-1)
		tr=it->getCumulativeTransformationPart1();
	else
	{
		C7Vector relTr(relObj->getCumulativeTransformationPart1()); // added ..Part1 on 2010/06/14
		tr=relTr.getInverse()*it->getCumulativeTransformationPart1(); // Corrected bug on 2011/01/22: was getLocalTransformationPart1 before!!!
	}
	quaternion[0]=tr.Q(1);
	quaternion[1]=tr.Q(2);
	quaternion[2]=tr.Q(3);
	quaternion[3]=tr.Q(0);
	position[0]=tr.X(0);
	position[1]=tr.X(1);
	position[2]=tr.X(2);
	return(1);
}

int simEmbSetObjectTransformation(int objectHandle,int relativeToObjectHandle,const float* position,const float* quaternion)
{
	if (!hasLaunched())
		return(-1);
	// V-REP quaternion, internally: w x y z
	// V-REP quaternion, at interfaces: x y z w (like ROS)
	C3DObject* it=ct::objCont->getObject(objectHandle);
	if (it==NULL)
		return(-1);
	if (relativeToObjectHandle==sim_handle_parent)
	{
		relativeToObjectHandle=-1;
		C3DObject* parent=it->getParent();
		if (parent!=NULL)
			relativeToObjectHandle=parent->getID();
	}
	C3DObject* relObj=ct::objCont->getObject(relativeToObjectHandle);
	if (relativeToObjectHandle!=-1)
	{
		if (relObj==NULL)
			return(-1);
	}
	if (relativeToObjectHandle==-1)
	{
		C7Vector tr;
		tr.Q(0)=quaternion[3];
		tr.Q(1)=quaternion[0];
		tr.Q(2)=quaternion[1];
		tr.Q(3)=quaternion[2];
		tr.X(0)=position[0];
		tr.X(1)=position[1];
		tr.X(2)=position[2];
		ct::objCont->setAbsoluteConfiguration(it->getID(),tr,false);
	}
	else
	{
		C7Vector absTr(it->getCumulativeTransformationPart1());
		C7Vector relTr(relObj->getCumulativeTransformationPart1()); // added ..Part1 on 2010/06/14
		C7Vector x(relTr.getInverse()*absTr);
		x.Q(0)=quaternion[3];
		x.Q(1)=quaternion[0];
		x.Q(2)=quaternion[1];
		x.Q(3)=quaternion[2];
		x.X(0)=position[0];
		x.X(1)=position[1];
		x.X(2)=position[2];
		absTr=relTr*x;
		ct::objCont->setAbsoluteConfiguration(it->getID(),absTr,false);
	}
	return(1);
}

int simEmbGetJointPosition(int jointHandle,float* position)
{
	if (!hasLaunched())
		return(-1);
	CJoint* it=ct::objCont->getJoint(jointHandle);
	if (it==NULL)
		return(-1);
	if (it->getJointType()==sim_joint_spherical_subtype)
		return(-1);
	position[0]=it->getPosition();
	return(1);
}

int simEmbSetJointPosition(int jointHandle,float position)
{
	if (!hasLaunched())
		return(-1);
	CJoint* it=ct::objCont->getJoint(jointHandle);
	if (it==NULL)
		return(-1);
	if (it->getJointType()==sim_joint_spherical_subtype)
		return(-1);
	it->setPosition(position);
	return(1);
}

int simEmbGetIkGroupHandle(const char* ikGroupName)
{
	if (!hasLaunched())
		return(-1);
	CikGroup* it=ct::ikGroups->getIkGroup(ikGroupName);
	if (it==NULL)
		return(-1);
	return(it->getObjectID());
}

int simEmbHandleIkGroup(int ikGroupHandle)
{
	if (!hasLaunched())
		return(-1);
	CikGroup* it=ct::ikGroups->getIkGroup(ikGroupHandle);
	if ( (ikGroupHandle!=sim_handle_all)&&(ikGroupHandle!=sim_handle_all_except_explicit) )
	{
		if (it==NULL)
			return(-1);
	}
	int returnValue=0;
	if (ikGroupHandle<0)
		returnValue=ct::ikGroups->computeAllIkGroups(ikGroupHandle==sim_handle_all_except_explicit);
	else
	{ // explicit handling
		if (!it->getExplicitHandling())
			return(-1);
		returnValue=it->computeGroupIk(false);
	}
	return(returnValue);
}

int simEmbQuaternionToEulerAngles(const float* quaternion,float* euler)
{
	if (!hasLaunched())
		return(-1);
	// V-REP quaternion, internally: w x y z
	// V-REP quaternion, at interfaces: x y z w (like ROS)
	C4Vector q(quaternion[3],quaternion[0],quaternion[1],quaternion[2]);
	C3Vector e(q.getEulerAngles());
	euler[0]=e(0);
	euler[1]=e(1);
	euler[2]=e(2);
	return(1);
}

int simEmbEulerAnglesToQuaternion(const float* euler,float* quaternion)
{
	if (!hasLaunched())
		return(-1);
	// V-REP quaternion, internally: w x y z
	// V-REP quaternion, at interfaces: x y z w (like ROS)
	C4Vector q(euler[0],euler[1],euler[2]);
	quaternion[0]=q(1);
	quaternion[1]=q(2);
	quaternion[2]=q(3);
	quaternion[3]=q(0);
	return(1);
}

int simEmbGetJointTransformation(int jointHandle,float* position,float* quaternion)
{
	if (!hasLaunched())
		return(-1);
	// V-REP quaternion, internally: w x y z
	// V-REP quaternion, at interfaces: x y z w (like ROS)
	CJoint* it=ct::objCont->getJoint(jointHandle);
	if (it==NULL)
		return(-1);
	C7Vector trFull(it->getLocalTransformation());
	C7Vector trPart1(it->getLocalTransformationPart1());
	C7Vector tr(trPart1.getInverse()*trFull);
	quaternion[0]=tr.Q(1);
	quaternion[1]=tr.Q(2);
	quaternion[2]=tr.Q(3);
	quaternion[3]=tr.Q(0);
	position[0]=tr.X(0);
	position[1]=tr.X(1);
	position[2]=tr.X(2);
	return(1);
}

int simEmbSetSphericalJointQuaternion(int jointHandle,const float* quaternion)
{
	if (!hasLaunched())
		return(-1);
	// V-REP quaternion, internally: w x y z
	// V-REP quaternion, at interfaces: x y z w (like ROS)
	CJoint* it=ct::objCont->getJoint(jointHandle);
	if (it==NULL)
		return(-1);
	if (it->getJointType()!=sim_joint_spherical_subtype)
		return(-1);
	C4Vector q;
	q(0)=quaternion[3];
	q(1)=quaternion[0];
	q(2)=quaternion[1];
	q(3)=quaternion[2];
	it->setSphericalTransformation(q);
	return(1);
}

int simEmbGetObjectParent(int objectHandle)
{
	if (!hasLaunched())
		return(-2);
	C3DObject* it=ct::objCont->getObject(objectHandle);
	if (it==NULL)
		return(-2);
	int retVal=-1;
	if (it->getParent()!=NULL)
		retVal=it->getParent()->getID();
	return(retVal);
}

int simEmbSetObjectParent(int objectHandle,int parentObjectHandle,bool keepInPlace)
{
	if (!hasLaunched())
		return(-1);
	C3DObject* it=ct::objCont->getObject(objectHandle);
	C3DObject* parentIt=ct::objCont->getObject(parentObjectHandle);
	if (it==NULL)
		return(-1);
	if ((parentIt==NULL)&&(parentObjectHandle!=-1))
		return(-1);
	if (keepInPlace)
		ct::objCont->makeObjectChildOf(it,parentIt);
	else
		it->setParent(parentIt);
	return(1);
}

int simEmbInvertTransformation(float* position,float* quaternion)
{
	if (!hasLaunched())
		return(-1);
	// V-REP quaternion, internally: w x y z
	// V-REP quaternion, at interfaces: x y z w (like ROS)
	C7Vector tr;
	tr.Q(0)=quaternion[3];
	tr.Q(1)=quaternion[0];
	tr.Q(2)=quaternion[1];
	tr.Q(3)=quaternion[2];
	tr.X(0)=position[0];
	tr.X(1)=position[1];
	tr.X(2)=position[2];
	tr.inverse();
	quaternion[0]=tr.Q(1);
	quaternion[1]=tr.Q(2);
	quaternion[2]=tr.Q(3);
	quaternion[3]=tr.Q(0);
	position[0]=tr.X(0);
	position[1]=tr.X(1);
	position[2]=tr.X(2);
	return(1);
}

int simEmbMultiplyTransformations(const float* position1,const float* quaternion1,const float* position2,const float* quaternion2,float* positionOut,float* quaternionOut)
{
	if (!hasLaunched())
		return(-1);
	// V-REP quaternion, internally: w x y z
	// V-REP quaternion, at interfaces: x y z w (like ROS)
	C7Vector tr1;
	tr1.Q(0)=quaternion1[3];
	tr1.Q(1)=quaternion1[0];
	tr1.Q(2)=quaternion1[1];
	tr1.Q(3)=quaternion1[2];
	tr1.X(0)=position1[0];
	tr1.X(1)=position1[1];
	tr1.X(2)=position1[2];

	C7Vector tr2;
	tr2.Q(0)=quaternion2[3];
	tr2.Q(1)=quaternion2[0];
	tr2.Q(2)=quaternion2[1];
	tr2.Q(3)=quaternion2[2];
	tr2.X(0)=position2[0];
	tr2.X(1)=position2[1];
	tr2.X(2)=position2[2];

	C7Vector trOut(tr1*tr2);

	quaternionOut[0]=trOut.Q(1);
	quaternionOut[1]=trOut.Q(2);
	quaternionOut[2]=trOut.Q(3);
	quaternionOut[3]=trOut.Q(0);
	positionOut[0]=trOut.X(0);
	positionOut[1]=trOut.X(1);
	positionOut[2]=trOut.X(2);
	return(1);
}

int simEmbInterpolateTransformations(const float* position1,const float* quaternion1,const float* position2,const float* quaternion2,float interpolFactor,float* positionOut,float* quaternionOut)
{
	if (!hasLaunched())
		return(-1);
	// V-REP quaternion, internally: w x y z
	// V-REP quaternion, at interfaces: x y z w (like ROS)
	C7Vector tr1;
	tr1.Q(0)=quaternion1[3];
	tr1.Q(1)=quaternion1[0];
	tr1.Q(2)=quaternion1[1];
	tr1.Q(3)=quaternion1[2];
	tr1.X(0)=position1[0];
	tr1.X(1)=position1[1];
	tr1.X(2)=position1[2];

	C7Vector tr2;
	tr2.Q(0)=quaternion2[3];
	tr2.Q(1)=quaternion2[0];
	tr2.Q(2)=quaternion2[1];
	tr2.Q(3)=quaternion2[2];
	tr2.X(0)=position2[0];
	tr2.X(1)=position2[1];
	tr2.X(2)=position2[2];

	C7Vector trOut;
	trOut.buildInterpolation(tr1,tr2,interpolFactor);

	quaternionOut[0]=trOut.Q(1);
	quaternionOut[1]=trOut.Q(2);
	quaternionOut[2]=trOut.Q(3);
	quaternionOut[3]=trOut.Q(0);
	positionOut[0]=trOut.X(0);
	positionOut[1]=trOut.X(1);
	positionOut[2]=trOut.X(2);
	return(1);
}

int simEmbMultTransformationWithVector(const float* position,const float* quaternion,float* vect)
{
	if (!hasLaunched())
		return(-1);
	// V-REP quaternion, internally: w x y z
	// V-REP quaternion, at interfaces: x y z w (like ROS)
	C7Vector tr;
	tr.Q(0)=quaternion[3];
	tr.Q(1)=quaternion[0];
	tr.Q(2)=quaternion[1];
	tr.Q(3)=quaternion[2];
	tr.X(0)=position[0];
	tr.X(1)=position[1];
	tr.X(2)=position[2];

	C3Vector v1(vect);

	C3Vector v2(tr*v1);

	vect[0]=v2(0);
	vect[1]=v2(1);
	vect[2]=v2(2);
	return(1);
}

int simEmbGetRotationAxis(const float* quaternionStart,const float* quaternionGoal,float* axis,float* angle)
{
	if (!hasLaunched())
		return(-1);
	// V-REP quaternion, internally: w x y z
	// V-REP quaternion, at interfaces: x y z w (like ROS)
	C4Vector qStart;
	qStart(0)=quaternionStart[3];
	qStart(1)=quaternionStart[0];
	qStart(2)=quaternionStart[1];
	qStart(3)=quaternionStart[2];

	C4Vector qGoal;
	qGoal(0)=quaternionGoal[3];
	qGoal(1)=quaternionGoal[0];
	qGoal(2)=quaternionGoal[1];
	qGoal(3)=quaternionGoal[2];

	// Following few lines taken from the quaternion interpolation part:
	C4Vector AA(qStart);
	C4Vector BB(qGoal);
	if (AA(0)*BB(0)+AA(1)*BB(1)+AA(2)*BB(2)+AA(3)*BB(3)<0.0f)
		AA=AA*-1.0f;
	C4Vector r((AA.getInverse()*BB).getAngleAndAxis());

	C3Vector v(r(1),r(2),r(3));
	v=AA*v;

	axis[0]=v(0);
	axis[1]=v(1);
	axis[2]=v(2);
	float l=sqrt(v(0)*v(0)+v(1)*v(1)+v(2)*v(2));
	if (l!=0.0f)
	{
		axis[0]/=l;
		axis[1]/=l;
		axis[2]/=l;
	}
	angle[0]=r(0);

	return(1);
}

int simEmbRotateAroundAxis(const float* positionIn,const float* quaternionIn,const float* axisVector,const float* axisPosition,float angle,float* positionOut,float* quaternionOut)
{
	if (!hasLaunched())
		return(-1);
	// V-REP quaternion, internally: w x y z
	// V-REP quaternion, at interfaces: x y z w (like ROS)
	C7Vector m;
	m.Q(0)=quaternionIn[3];
	m.Q(1)=quaternionIn[0];
	m.Q(2)=quaternionIn[1];
	m.Q(3)=quaternionIn[2];
	m.X(0)=positionIn[0];
	m.X(1)=positionIn[1];
	m.X(2)=positionIn[2];

	C3Vector ax(axisVector);
	C3Vector pos(axisPosition);

	float alpha=-atan2(ax(1),ax(0));
	float beta=atan2(-sqrt(ax(0)*ax(0)+ax(1)*ax(1)),ax(2));
	m.X-=pos;
	C7Vector r;
	r.X.clear();
	r.Q.setEulerAngles(0.0f,0.0f,alpha);
	m=r*m;
	r.Q.setEulerAngles(0.0f,beta,0.0f);
	m=r*m;
	r.Q.setEulerAngles(0.0f,0.0f,angle);
	m=r*m;
	r.Q.setEulerAngles(0.0f,-beta,0.0f);
	m=r*m;
	r.Q.setEulerAngles(0.0f,0.0f,-alpha);
	m=r*m;
	m.X+=pos;

	quaternionOut[0]=m.Q(1);
	quaternionOut[1]=m.Q(2);
	quaternionOut[2]=m.Q(3);
	quaternionOut[3]=m.Q(0);
	positionOut[0]=m.X(0);
	positionOut[1]=m.X(1);
	positionOut[2]=m.X(2);
	return(1);
}

int simEmbSetJointMode(int jointHandle,int jointMode)
{
	if (!hasLaunched())
		return(-1);
	CJoint* it=ct::objCont->getJoint(jointHandle);
	if (it==NULL)
		return(-1);
	it->setJointMode(jointMode);
	return(1);
}

int simEmbSetIkGroupExplicitHandling(int ikGroupHandle,bool explicitHandling)
{
	if (!hasLaunched())
		return(-1);
	CikGroup* it=ct::ikGroups->getIkGroup(ikGroupHandle);
	if (it==NULL)
		return(-1);
	it->setExplicitHandling(explicitHandling);
	return(1);
}

int simEmbSetIkGroupProperties(int ikGroupHandle,int resolutionMethod,int maxIterations,float damping)
{
	if (!hasLaunched())
		return(-1);
	CikGroup* it=ct::ikGroups->getIkGroup(ikGroupHandle);
	if (it==NULL)
		return(-1);
	it->setCalculationMethod(resolutionMethod);
	it->setMaxIterations(maxIterations);
	it->setDlsFactor(damping);
	return(1);
}

int simEmbSetIkElementProperties(int ikGroupHandle,int tipDummyHandle,int constraints,const float* precision,const float* weight)
{
	if (!hasLaunched())
		return(-1);
	CikGroup* it=ct::ikGroups->getIkGroup(ikGroupHandle);
	if (it==NULL)
		return(-1);
	CDummy* dum=ct::objCont->getDummy(tipDummyHandle);
	if (dum==NULL)
		return(-1);
	CikEl* el=it->getIkElementWithTooltipID(tipDummyHandle);
	if (el==NULL)
		return(-1);
	if (constraints!=-1)
		el->setConstraints(constraints);
	if (precision!=NULL)
	{
		el->setMinLinearPrecision(precision[0]);
		el->setMinAngularPrecision(precision[1]);
	}
	if (weight!=NULL)
	{
		el->setPositionWeight(weight[0]);
		el->setOrientationWeight(weight[1]);
	}
	return(1);
}
