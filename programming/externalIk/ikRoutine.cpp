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
#include "ikRoutine.h"
#include "Ct.h"
#include "Tt.h"


void CIkRoutine::multiply(C4X4FullMatrix& d0,C4X4FullMatrix& dp,int index,
					   std::vector<C4X4FullMatrix*>& allMatrices)
{
// Input transformation matrices:
// Right part:
// R=m0+m1*deltaQ1+m2*deltaQ2+m3*deltaQ3+...+m6*deltaQ6
// Left part:
// L=d0+dp*deltaQindex
//Output transformation matrices:
// R=L*R
// index should be between 1 and n (indication which deltaQ dp has to be multiplied with
// If index is different from that, d0+dp*deltQindex=d0=normal transformation matrix
// If index==1, it concerns the first joint in the chain (from the tooltip), etc.
	C4X4FullMatrix& m0=*allMatrices[0];
	C4X4FullMatrix m0Saved(m0);
	m0=d0*m0Saved;
	for (int i=1;i<int(allMatrices.size());i++)
		(*allMatrices[i])=d0*(*allMatrices[i]);
	if ((index>0)&&(index<int(allMatrices.size())))
	{
		C4X4FullMatrix w(dp*m0Saved);
		(*allMatrices[index])+=w;
	}
}

void CIkRoutine::buildDeltaZRotation(C4X4FullMatrix& d0,C4X4FullMatrix& dp,float screwCoeff)
{
	d0.setIdentity();
	dp.clear();
	dp(0,1)=-1.0f;
	dp(1,0)=1.0f;
	dp(2,3)=screwCoeff;
}

void CIkRoutine::buildDeltaZTranslation(C4X4FullMatrix& d0,C4X4FullMatrix& dp)
{
	d0.setIdentity();
	dp.clear();
	dp(2,3)=1.0f;
}

CMatrix* CIkRoutine::getJacobian(CikEl* ikElement,C4X4Matrix& tooltipTransf,std::vector<int>* rowJointIDs,std::vector<int>* rowJointStages)
{	// rowJointIDs is NULL by default. If not null, it will contain the ids of the joints
	// corresponding to the rows of the jacobian.
	// Return value NULL means that is ikElement is either inactive, either invalid
	// tooltipTransf is the cumulative transformation matrix of the tooltip,
	// computed relative to the base!
	// The temporary joint parameters need to be initialized before calling this function!
	// We check if the ikElement's base is in the chain and that tooltip is valid!
	CDummy* tooltip=ct::objCont->getDummy(ikElement->getTooltip());
	if (tooltip==NULL)
	{ // Should normally never happen!
		ikElement->setActive(false);
		return(NULL);
	}
	C3DObject* base=ct::objCont->getObject(ikElement->getBase());
	if ( (base!=NULL)&&(!tooltip->isObjectParentedWith(base)) )
	{ // This case can happen (when the base's parenting was changed for instance)
		ikElement->setBase(-1);
		ikElement->setActive(false);
		return(NULL);
	}

	// We check the number of degrees of freedom and prepare the rowJointIDs vector:
	C3DObject* iterat=tooltip;
	int doF=0;
	while (iterat!=base)
	{
		iterat=iterat->getParent();
		if ( (iterat!=NULL)&&(iterat!=base) )
		{
			if (iterat->getObjectType()==sim_object_joint_type)
			{
				if ( (((CJoint*)iterat)->getJointMode()==sim_jointmode_ik)||(((CJoint*)iterat)->getJointMode()==sim_jointmode_reserved_previously_ikdependent)||(((CJoint*)iterat)->getJointMode()==sim_jointmode_dependent) )
				{
					int d=((CJoint*)iterat)->getDoFs();
					for (int i=d-1;i>=0;i--)
					{
						if (rowJointIDs!=NULL)
						{
							rowJointIDs->push_back(iterat->getID());
							rowJointStages->push_back(i);
						}
					}
					doF+=d;
				}
			}
		}
	}
	CMatrix* J=new CMatrix(6,(unsigned char)doF);
	std::vector<C4X4FullMatrix*> jMatrices;
	for (int i=0;i<(doF+1);i++)
	{
		C4X4FullMatrix* matr=new C4X4FullMatrix();
		if (i==0)
			(*matr).setIdentity();
		else
			(*matr).clear();
		jMatrices.push_back(matr);
	}

	// Now we go from tip to base:
	iterat=tooltip;
	C4X4FullMatrix buff;
	buff.setIdentity();
	int positionCounter=0;
	C4X4FullMatrix d0;
	C4X4FullMatrix dp;
	C4X4FullMatrix paramPart;
	CJoint* lastJoint=NULL;
	int indexCnt=-1;
	int indexCntLast=-1;
	while (iterat!=base)
	{
		C3DObject* nextIterat=iterat->getParent();
		C7Vector local;
		if (iterat->getObjectType()==sim_object_joint_type)
		{
			if ( (((CJoint*)iterat)->getJointMode()!=sim_jointmode_ik)&&(((CJoint*)iterat)->getJointMode()!=sim_jointmode_reserved_previously_ikdependent)&&(((CJoint*)iterat)->getJointMode()!=sim_jointmode_dependent) )
				local=iterat->getLocalTransformation(true);
			else
			{
				CJoint* it=(CJoint*)iterat;
				if (it->getJointType()==sim_joint_spherical_subtype)
				{
					if (indexCnt==-1)
						indexCnt=it->getDoFs()-1;
					it->getLocalTransformationExPart1(local,indexCnt--,true);
					if (indexCnt!=-1)
						nextIterat=iterat; // We keep the same object! (but indexCnt has decreased)
				}
				else
					local=iterat->getLocalTransformationPart1(true);
			}
		}
		else
			local=iterat->getLocalTransformation(true); 

		buff=C4X4FullMatrix(local.getMatrix())*buff;
		iterat=nextIterat;
		bool activeJoint=false;
		if (iterat!=NULL) // Following lines recently changed!
		{
			if (iterat->getObjectType()==sim_object_joint_type) 
				activeJoint=( (((CJoint*)iterat)->getJointMode()==sim_jointmode_ik)||(((CJoint*)iterat)->getJointMode()==sim_jointmode_reserved_previously_ikdependent)||(((CJoint*)iterat)->getJointMode()==sim_jointmode_dependent) );
		}
		if ( (iterat==base)||activeJoint )
		{	// If base is NULL then the second part is not evaluated (iterat->getObjectType())
			if (positionCounter==0)
			{	// Here we have the first part (from tooltip to first joint)
				d0=buff;
				dp.clear();
				multiply(d0,dp,0,jMatrices);
			}
			else
			{	// Here we have a joint:
				if (lastJoint->getJointType()==sim_joint_revolute_subtype)
				{
					buildDeltaZRotation(d0,dp,lastJoint->getScrewPitch());
					multiply(d0,dp,positionCounter,jMatrices);
					paramPart.buildZRotation(lastJoint->getPosition(true));
				}
				else if (lastJoint->getJointType()==sim_joint_prismatic_subtype)
				{
					buildDeltaZTranslation(d0,dp);
					multiply(d0,dp,positionCounter,jMatrices);
					paramPart.buildTranslation(0.0f,0.0f,lastJoint->getPosition(true));
				}
				else 
				{ // Spherical joint part!
					buildDeltaZRotation(d0,dp,0.0f);
					multiply(d0,dp,positionCounter,jMatrices);
					if (indexCntLast==-1)
						indexCntLast=lastJoint->getDoFs()-1;
					paramPart.buildZRotation(lastJoint->getTempParameterEx(indexCntLast--));
				}
				d0=buff*paramPart;
				dp.clear();
				multiply(d0,dp,0,jMatrices);
			}
			buff.setIdentity();
			lastJoint=(CJoint*)iterat;
			positionCounter++;
		}
	}

	int alternativeBaseForConstraints=ikElement->getAlternativeBaseForConstraints();
	if (alternativeBaseForConstraints!=-1)
	{
		CDummy* alb=ct::objCont->getDummy(alternativeBaseForConstraints);
		if (alb!=NULL)
		{ // We want everything relative to the alternativeBaseForConstraints dummy orientation!
			C7Vector alternativeBase(alb->getCumulativeTransformationPart1(true));
			C7Vector currentBase;
			currentBase.setIdentity();
			if (base!=NULL)
				currentBase=base->getCumulativeTransformation(true); // could be a joint, we want also the joint intrinsic transformation part!
			C4X4FullMatrix correction((alternativeBase.getInverse()*currentBase).getMatrix());
			dp.clear();
			multiply(correction,dp,0,jMatrices);
		}
	}

	// The x-, y- and z-component:
	for (int i=0;i<doF;i++)
	{
		(*J)(0,i)=(*jMatrices[1+i])(0,3);
		(*J)(1,i)=(*jMatrices[1+i])(1,3);
		(*J)(2,i)=(*jMatrices[1+i])(2,3);
	}
	// We divide all delta components (to avoid distorsions)...
	for (int i=0;i<doF;i++)
		(*jMatrices[1+i])/=IK_DIVISION_FACTOR;
	// ...and add the cumulative transform to the delta-components:
	for (int i=0;i<doF;i++)
		(*jMatrices[1+i])+=(*jMatrices[0]);
	// We also copy the cumulative transform to 'tooltipTransf':
	tooltipTransf=(*jMatrices[0]);
	// Now we extract the delta Euler components:
	C4X4FullMatrix mainInverse(*jMatrices[0]);
	mainInverse.invert();
	C4X4FullMatrix tmp;
	// Alpha-, Beta- and Gamma-components:
	for (int i=0;i<doF;i++)
	{
		tmp=mainInverse*(*jMatrices[1+i]);
		C3Vector euler(tmp.getEulerAngles());
		(*J)(3,i)=euler(0); // here we would have to multiply the euler angle with IK_DIVISION_FACTOR to get the "correct" Jacobian
		(*J)(4,i)=euler(1); // here we would have to multiply the euler angle with IK_DIVISION_FACTOR to get the "correct" Jacobian
		(*J)(5,i)=euler(2); // here we would have to multiply the euler angle with IK_DIVISION_FACTOR to get the "correct" Jacobian
	}


	// We free the memory allocated for each joint variable:
	for (int i=0;i<int(jMatrices.size());i++)
		delete jMatrices[i];
	return(J);
}



