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


#include "v_repConst.h"
#include "Global.h"
#include "ikEl.h"
#include "ikRoutine.h"
#include "Ct.h"
#include "Tt.h"


CikEl::CikEl()
{
	tooltip=-1;
	commonInit();
}

CikEl::CikEl(int theTooltip)
{
	tooltip=theTooltip;
	commonInit();
}

CikEl::~CikEl()
{
}



void CikEl::setMinAngularPrecision(float prec)
{
	minAngularPrecision=prec;
}
void CikEl::setMinLinearPrecision(float prec)
{
	minLinearPrecision=prec;
}

void CikEl::performObjectLoadingMapping(std::vector<int>* map)
{
	tooltip=ct::objCont->getLoadingMapping(map,tooltip);
	base=ct::objCont->getLoadingMapping(map,base);
	alternativeBaseForConstraints=ct::objCont->getLoadingMapping(map,alternativeBaseForConstraints);
}

void CikEl::commonInit()
{
	base=-1;
	alternativeBaseForConstraints=-1; // -1 means use the regular base for constraints!

	minAngularPrecision=0.1f*degToRad;
	minLinearPrecision=0.0005f;
	active=true;
	constraints=(sim_ik_x_constraint|sim_ik_y_constraint|sim_ik_z_constraint);
	positionWeight=1.0f;
	orientationWeight=1.0f;
	matrix=NULL;
	matrix_correctJacobian=NULL; // same as matrix, but the orientation parts are multiplied by IK_DIVISION_FACTOR
	errorVector=NULL;
	rowJointIDs=NULL;
	rowJointStages=NULL;
}

void CikEl::setBase(int newBase)
{
	base=newBase;
}

void CikEl::setActive(bool isActive)
{
	active=isActive;
}

bool CikEl::announceObjectWillBeErased(int objID,bool copyBuffer)
{ // Return value true means that this IK el object should be destroyed
	if ((base==objID)||(alternativeBaseForConstraints==objID)||(tooltip==objID))
		return(true); // This element has to be erased!
	return(false);
}

int CikEl::getObjectID()
{
	return(objectID);
}

int CikEl::getTooltip()
{
	return(tooltip);
}

int CikEl::getBase()
{
	return(base);
}

int CikEl::getAlternativeBaseForConstraints()
{ 
	return(alternativeBaseForConstraints);
}

int CikEl::getTarget()
{ 
	CDummy* tip=ct::objCont->getDummy(tooltip);
	if (tip==NULL)
		return(-1);
	int linkedDummyID=tip->getLinkedDummyID();
	if (linkedDummyID==-1)
		return(-1);
	if (tip->getLinkType()!=sim_dummy_linktype_ik_tip_target)
		return(-1);
	return(linkedDummyID); // this should be the target!
}

float CikEl::getMinAngularPrecision()
{
	return(minAngularPrecision);
}

float CikEl::getMinLinearPrecision()
{
	return(minLinearPrecision);
}

bool CikEl::getActive()
{
	return(active);
}
void CikEl::setConstraints(int constr)
{
	constraints=constr;
}
int CikEl::getConstraints()
{
	return(constraints); 
}
void CikEl::setPositionWeight(float weight)
{
	tt::limitValue(0.001f,1.0f,weight);
	positionWeight=weight;
}
float CikEl::getPositionWeight()
{
	return(positionWeight);
}
void CikEl::setOrientationWeight(float weight)
{
	tt::limitValue(0.001f,1.0f,weight);
	orientationWeight=weight;
}
float CikEl::getOrientationWeight()
{
	return(orientationWeight);
}

void CikEl::checkIfWithinTolerance(bool& position,bool& orientation,bool useTempValues)
{
	position=true;
	orientation=true;
	CDummy* targetObj=ct::objCont->getDummy(getTarget());
	if (targetObj==NULL)
		return; // The tooltip is not constrained!
	CDummy* tooltipObj=ct::objCont->getDummy(tooltip);
	C7Vector targetM(targetObj->getCumulativeTransformationPart1(useTempValues));
	C7Vector tooltipM(tooltipObj->getCumulativeTransformationPart1(useTempValues));
 
	// Since everything is relative to the base
	C7Vector baseM;
	baseM.setIdentity();
	CDummy* baseObj=ct::objCont->getDummy(base);
	if (baseObj!=NULL)
		baseM=baseObj->getCumulativeTransformationPart1(useTempValues).getInverse();

	baseObj=ct::objCont->getDummy(alternativeBaseForConstraints);
	if (baseObj!=NULL)
		baseM=baseObj->getCumulativeTransformationPart1(useTempValues).getInverse();

	targetM=baseM*targetM;
	tooltipM=baseM*tooltipM;

	float err[2];
	getError(targetM.getMatrix(),tooltipM.getMatrix(),err,(constraints&sim_ik_x_constraint)!=0,
		(constraints&sim_ik_y_constraint)!=0,(constraints&sim_ik_z_constraint)!=0,
		(constraints&sim_ik_alpha_beta_constraint)!=0,(constraints&sim_ik_gamma_constraint)!=0);
	if (constraints&(sim_ik_x_constraint|sim_ik_y_constraint|sim_ik_z_constraint))
	{
		if (minLinearPrecision<err[0])
			position=false;
	}
	if (constraints&(sim_ik_alpha_beta_constraint|sim_ik_gamma_constraint))
	{
		if (minAngularPrecision<err[1])
			orientation=false;
	}
}

void CikEl::prepareIkEquations(float interpolFact)
{	// Before calling this function, make sure that joint's temp. param. are initialized!
	// Make also sure the tooltip is built on a joint before 'base' and that base
	// is parent of 'tooltip'.
	// interpolFact is the interpolation factor we use to compute the target pose:
	// interpolPose=tooltipPose*(1-interpolFact)+targetPose*interpolFact

	// We first take care of dummies linked to path objects in a "sliding" manner (not fixed but assigned to the path):
	// Case 1. Target is the free sliding dummy:
	CDummy* dummyObj=ct::objCont->getDummy(getTarget());
	CDummy* tipObj=ct::objCont->getDummy(tooltip);

	// We get the jacobian and the rowJointIDs:
	rowJointIDs=new std::vector<int>;
	rowJointStages=new std::vector<int>;
	C4X4Matrix oldMatr;
	CMatrix* Ja=CIkRoutine::getJacobian(this,oldMatr,rowJointIDs,rowJointStages);

	// oldMatr now contains the cumulative transf. matr. of tooltip relative to base
	C4X4Matrix oldMatrInv(oldMatr.getInverse());
	int doF=Ja->cols;
	int equationNumber=0;

	C4X4Matrix dummyCumul;
	C4X4Matrix m;
	if (dummyObj!=NULL)
	{
		C3DObject* baseObj=ct::objCont->getObject(base);
		C4X4Matrix baseCumul;
		baseCumul.setIdentity();
		if (baseObj!=NULL)
			baseCumul=baseObj->getCumulativeTransformation(true).getMatrix();

		baseObj=ct::objCont->getObject(alternativeBaseForConstraints);
		if (baseObj!=NULL)
			baseCumul=baseObj->getCumulativeTransformation(true).getMatrix();

		baseCumul.inverse();

		dummyCumul=dummyObj->getCumulativeTransformationPart1(true).getMatrix();
		dummyCumul=baseCumul*dummyCumul; // target is relative to the base (or the alternative base)!
		C7Vector tr;
		tr.buildInterpolation(oldMatr.getTransformation(),dummyCumul.getTransformation(),interpolFact);
		m=tr;

		// We prepare matrix and errorVector and their respective sizes:
		if (constraints&sim_ik_x_constraint)
			equationNumber++;
		if (constraints&sim_ik_y_constraint)
			equationNumber++;
		if (constraints&sim_ik_z_constraint)
			equationNumber++;
		if (constraints&sim_ik_alpha_beta_constraint)
			equationNumber+=2;
		if (constraints&sim_ik_gamma_constraint)
			equationNumber++;
	}

	matrix=new CMatrix(equationNumber,doF);
	matrix_correctJacobian=new CMatrix(equationNumber,doF);
	errorVector=new CMatrix(equationNumber,1);
	if (dummyObj!=NULL)
	{
		// We set up the position/orientation errorVector and the matrix:
		int pos=0;
		if (constraints&sim_ik_x_constraint)
		{
			for (int i=0;i<doF;i++)
			{
				(*matrix)(pos,i)=(*Ja)(0,i);
				(*matrix_correctJacobian)(pos,i)=(*Ja)(0,i);
			}
			(*errorVector)(pos,0)=(m.X(0)-oldMatr.X(0))*positionWeight;
			pos++;
		}
		if (constraints&sim_ik_y_constraint)
		{
			for (int i=0;i<doF;i++)
			{
				(*matrix)(pos,i)=(*Ja)(1,i);
				(*matrix_correctJacobian)(pos,i)=(*Ja)(1,i);
			}
			(*errorVector)(pos,0)=(m.X(1)-oldMatr.X(1))*positionWeight;
			pos++;
		}
		if (constraints&sim_ik_z_constraint)
		{
			for (int i=0;i<doF;i++)
			{
				(*matrix)(pos,i)=(*Ja)(2,i);
				(*matrix_correctJacobian)(pos,i)=(*Ja)(2,i);
			}
			(*errorVector)(pos,0)=(m.X(2)-oldMatr.X(2))*positionWeight;
			pos++;
		}
		if ( (constraints&sim_ik_alpha_beta_constraint)&&(constraints&sim_ik_gamma_constraint) )
		{
			for (int i=0;i<doF;i++)
			{
				(*matrix)(pos,i)=(*Ja)(3,i);
				(*matrix)(pos+1,i)=(*Ja)(4,i);
				(*matrix)(pos+2,i)=(*Ja)(5,i);
				(*matrix_correctJacobian)(pos,i)=(*Ja)(3,i)*IK_DIVISION_FACTOR;
				(*matrix_correctJacobian)(pos+1,i)=(*Ja)(4,i)*IK_DIVISION_FACTOR;
				(*matrix_correctJacobian)(pos+2,i)=(*Ja)(5,i)*IK_DIVISION_FACTOR;
			}
			C4X4Matrix diff(oldMatrInv*m);
			C3Vector euler(diff.M.getEulerAngles());
			(*errorVector)(pos,0)=euler(0)*orientationWeight/IK_DIVISION_FACTOR;
			(*errorVector)(pos+1,0)=euler(1)*orientationWeight/IK_DIVISION_FACTOR;
			(*errorVector)(pos+2,0)=euler(2)*orientationWeight/IK_DIVISION_FACTOR;
			pos=pos+3;
		}
		else
		{
			if (constraints&sim_ik_alpha_beta_constraint)
			{
				for (int i=0;i<doF;i++)
				{
					(*matrix)(pos,i)=(*Ja)(3,i);
					(*matrix)(pos+1,i)=(*Ja)(4,i);
					(*matrix_correctJacobian)(pos,i)=(*Ja)(3,i)*IK_DIVISION_FACTOR;
					(*matrix_correctJacobian)(pos+1,i)=(*Ja)(4,i)*IK_DIVISION_FACTOR;
				}
				C4X4Matrix diff(oldMatrInv*m);
				C3Vector euler(diff.M.getEulerAngles());
				(*errorVector)(pos,0)=euler(0)*orientationWeight/IK_DIVISION_FACTOR;
				(*errorVector)(pos+1,0)=euler(1)*orientationWeight/IK_DIVISION_FACTOR;
				pos=pos+2;
			}
			if (constraints&sim_ik_gamma_constraint)
			{ // sim_gamma_constraint can't exist without sim_alpha_beta_constraint!
				for (int i=0;i<doF;i++)
				{
					(*matrix)(pos,i)=(*Ja)(5,i);
					(*matrix_correctJacobian)(pos,i)=(*Ja)(5,i)*IK_DIVISION_FACTOR;
				}
				C4X4Matrix diff(oldMatrInv*m);
				C3Vector euler(diff.M.getEulerAngles());
				(*errorVector)(pos,0)=euler(2)*orientationWeight/IK_DIVISION_FACTOR;
				pos++;
			}
		}
	}
	delete Ja; // We delete the jacobian!
}

void CikEl::removeIkEquations()
{
	if (matrix==NULL)
		return;
	delete matrix;
	delete matrix_correctJacobian;
	matrix=NULL;
	matrix_correctJacobian=NULL;
	delete errorVector;
	errorVector=NULL;
	rowJointIDs->clear();
	delete rowJointIDs;
	rowJointIDs=NULL;

	rowJointStages->clear();
	delete rowJointStages;
	rowJointStages=NULL;
}

void CikEl::getError(const C4X4Matrix& m1,const C4X4Matrix& m2,float err[2],bool xC,bool yC,bool zC,bool abC,bool gC)
{	// err[0] is position error, err[1] is orientation error
	// xC, yC and zC represents the position component we care about
	// Similar with abC and gB for the orientation: if abC and gC are false, orientation error is 0
	// All in rad or meter!!!
	C3Vector p(m1.X-m2.X);

	float xConstr=0.0f;
	if (xC) 
		xConstr=1.0f;
	float yConstr=0.0f;
	if (yC) 
		yConstr=1.0f;
	float zConstr=0.0f;
	if (zC) 
		zConstr=1.0f;
	err[0]=sqrtf(p(0)*p(0)*xConstr+p(1)*p(1)*yConstr+p(2)*p(2)*zConstr);

	float x=m1.M.axis[0]*m2.M.axis[0];
	float z=m1.M.axis[2]*m2.M.axis[2];
	x=tt::getLimitedFloat(-1.0f,1.0f,x);
	z=tt::getLimitedFloat(-1.0f,1.0f,z);
	if (gC)
	{ // Full orientation constraints
		err[1]=fabs(CMath::robustAcos(x));
		float v=fabs(CMath::robustAcos(z));
		if (v>err[1])
			err[1]=v;
	}
	else if (abC)
	{ // Orientation constraints, but free around z axis
		err[1]=fabs(CMath::robustAcos(z));
	}
	else
		err[1]=0.0f; // No orientation constraints!
}


void CikEl::serializeRExtIk(CExtIkSer& ar)
{
	objectID=ar.readInt();

	tooltip=ar.readInt();
	base=ar.readInt();

	alternativeBaseForConstraints=ar.readInt();

	minAngularPrecision=ar.readFloat();
	minLinearPrecision=ar.readFloat();

	constraints=ar.readInt();

	positionWeight=ar.readFloat();
	orientationWeight=ar.readFloat();

	unsigned char nothing=ar.readByte();
	active=(nothing&1);
}
