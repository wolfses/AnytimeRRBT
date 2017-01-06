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
#include "ikGroup.h"
#include "ikRoutine.h"
#include "Ct.h"
#include "Tt.h"
#include <algorithm>


CikGroup::CikGroup()
{
	objectID=SIM_IDSTART_IKGROUP;
	maxIterations=3;
	active=true;
	ignoreMaxStepSizes=true;


	_lastJacobian=NULL;
	_explicitHandling=false;
	dlsFactor=0.1f;
	calculationMethod=sim_ik_pseudo_inverse_method;
	restoreIfPositionNotReached=false;
	restoreIfOrientationNotReached=false;
	doOnFailOrSuccessOf=-1;
	doOnFail=true;
	doOnPerformed=true;
	constraints=0;
	jointLimitWeight=1.0f;
	jointTreshholdAngular=2.0f*degToRad;
	jointTreshholdLinear=0.001f;
	_calculationResult=sim_ikresult_not_performed;
	_correctJointLimits=false;
}

CikGroup::~CikGroup()
{
	while (ikElements.size()!=0)
		removeIkElement(ikElements[0]->getObjectID());

}



void CikGroup::performObjectLoadingMapping(std::vector<int>* map)
{
	for (int i=0;i<int(ikElements.size());i++)
		ikElements[i]->performObjectLoadingMapping(map);
}

void CikGroup::setExplicitHandling(bool explicitHandl)
{
	_explicitHandling=explicitHandl;
}

bool CikGroup::getExplicitHandling()
{
	return(_explicitHandling);
}

void CikGroup::setObjectName(std::string newName)
{
	objectName=newName;
}

void CikGroup::setActive(bool isActive)
{
	active=isActive;
}

void CikGroup::resetCalculationResult()
{
	_calculationResult=sim_ikresult_not_performed;
}

void CikGroup::setCorrectJointLimits(bool c)
{
	_correctJointLimits=c;
}

bool CikGroup::getCorrectJointLimits()
{
	return(_correctJointLimits);
}

void CikGroup::setCalculationResult(int res,int calcTimeInMs)
{
	_calculationResult=res;
}

int CikGroup::getCalculationResult()
{
	return(_calculationResult);
}

void CikGroup::setJointLimitWeight(float weight)
{
	tt::limitValue(0.001f,1.0f,weight);
	jointLimitWeight=weight;
}
float CikGroup::getJointLimitWeight()
{
	return(jointLimitWeight);
}

float CikGroup::getJointTreshholdAngular()
{
	return(jointTreshholdAngular);
}
float CikGroup::getJointTreshholdLinear()
{
	return(jointTreshholdLinear);
}

void CikGroup::setConstraints(int constr)
{
	constraints=constr;
}

int CikGroup::getConstraints()
{
	return(constraints);
}

void CikGroup::setMaxIterations(int maxIter)
{
	tt::limitValue(1,400,maxIter);
	maxIterations=maxIter;
}
int CikGroup::getMaxIterations()
{
	return(maxIterations);
}

bool CikGroup::getActive()
{
	return(active);
}
void CikGroup::setDlsFactor(float theFactor)
{
	tt::limitValue(0.000001f,10.0f,theFactor);
	dlsFactor=theFactor;
}
float CikGroup::getDlsFactor()
{
	return(dlsFactor);
}
void CikGroup::setCalculationMethod(int theMethod)
{
	if ( (theMethod==sim_ik_pseudo_inverse_method)||(theMethod==sim_ik_damped_least_squares_method)||
		(theMethod==sim_ik_jacobian_transpose_method) )
	{
		calculationMethod=theMethod;
	}
}
int CikGroup::getCalculationMethod()
{
	return(calculationMethod);
}

bool CikGroup::getRestoreIfPositionNotReached()
{
	return(restoreIfPositionNotReached);
}
bool CikGroup::getRestoreIfOrientationNotReached()
{
	return(restoreIfOrientationNotReached);
}
int CikGroup::getDoOnFailOrSuccessOf()
{
	return(doOnFailOrSuccessOf);
}
bool CikGroup::getDoOnFail()
{
	return(doOnFail);
}
bool CikGroup::getDoOnPerformed()
{
	return(doOnPerformed);
}
void CikGroup::removeIkElement(int elementID)
{
	for (int i=0;i<int(ikElements.size());i++)
	{
		if (ikElements[i]->getObjectID()==elementID)
		{

			delete ikElements[i];
			ikElements.erase(ikElements.begin()+i);
			break;
		}
	}
}

CikEl* CikGroup::getIkElement(int ikElementID)
{
	for (int i=0;i<int(ikElements.size());i++)
	{
		if (ikElements[i]->getObjectID()==ikElementID)
			return(ikElements[i]);
	}
	return(NULL);
}

CikEl* CikGroup::getIkElementWithTooltipID(int tooltipID)
{ 
	if (tooltipID==-1)
		return(NULL);
	for (int i=0;i<int(ikElements.size());i++)
	{
		if (ikElements[i]->getTooltip()==tooltipID)
			return(ikElements[i]);
	}
	return(NULL);
}

int CikGroup::getObjectID()
{
	return(objectID);
}

std::string CikGroup::getObjectName()
{
	return(objectName);
}

bool CikGroup::announceObjectWillBeErased(int objID,bool copyBuffer)
{ // Return value true means that this object should be destroyed
	int i=0;
	while (i<int(ikElements.size()))
	{
		if (ikElements[i]->announceObjectWillBeErased(objID,copyBuffer))
		{
			removeIkElement(ikElements[i]->getObjectID());
			i=0; // ordering may have changed
		}
		else
			i++;
	}
	return(ikElements.size()==0);
}

bool CikGroup::announceIkGroupWillBeErased(int ikGroupID,bool copyBuffer)
{ // Return value true means that this avoidance object should be destroyed
	if (doOnFailOrSuccessOf==ikGroupID)
	{
		doOnFailOrSuccessOf=-1;
		active=false;
	}
	return(false);
}

bool CikGroup::getIgnoreMaxStepSizes()
{
	return(ignoreMaxStepSizes);
}

void CikGroup::getAllActiveJoints(std::vector<CJoint*>& jointList)
{ // Retrieves all active joints in this group. Ordering is random!
	for (int elementNumber=0;elementNumber<int(ikElements.size());elementNumber++)
	{
		CikEl* element=ikElements[elementNumber];
		CDummy* tooltip=ct::objCont->getDummy(element->getTooltip());
		C3DObject* base=ct::objCont->getObject(element->getBase());
		bool valid=true;
		if (!element->getActive())
			valid=false;
		if (tooltip==NULL)
			valid=false;
		// We check that tooltip is parented with base and has at least one joint in-between:
		if (valid)
		{
			valid=false;
			bool jointPresent=false;
			bool baseOk=false;
			C3DObject* iterat=tooltip;
			while ( (iterat!=base)&&(iterat!=NULL) )
			{
				iterat=iterat->getParent();
				if (iterat==base)
				{
					baseOk=true;
					if (jointPresent)
						valid=true;
				}
				if ( (iterat!=base)&&(iterat!=NULL)&&(iterat->getObjectType()==sim_object_joint_type) )
					jointPresent=true;
			}
			if (!valid)
			{
				element->setActive(false); // This element has an error
				if (!baseOk)
					element->setBase(-1); // The base was illegal!
			}
		}
		if (valid)
		{ // We add all joint between tooltip and base which are not yet present:
			C3DObject* iterat=tooltip;
			while ( (iterat!=base)&&(iterat!=NULL) )
			{
				if (iterat->getObjectType()==sim_object_joint_type)
				{

					// FOLLOWING JUST FOR EXT IK (SLOWER, BUT NOT USING BOOST LIB):
					bool abcdefound=false;
					for (int abcde=0;abcde<int(jointList.size());abcde++)
					{
						if (jointList[abcde]==iterat)
						{
							abcdefound=true;
							break;
						}
					}
					if (!abcdefound)
						jointList.push_back((CJoint*)iterat);
				}
				iterat=iterat->getParent();
			}
		}
	}
}

void CikGroup::getTipAndTargetLists(std::vector<CDummy*>& tipList,std::vector<CDummy*>& targetList)
{
	for (int elementNumber=0;elementNumber<int(ikElements.size());elementNumber++)
	{
		CikEl* element=ikElements[elementNumber];
		CDummy* tooltip=ct::objCont->getDummy(element->getTooltip());
		CDummy* target=ct::objCont->getDummy(element->getTarget());
		C3DObject* base=ct::objCont->getObject(element->getBase());
		bool valid=true;
		if (!element->getActive())
			valid=false;
		if (tooltip==NULL)
			valid=false;
		if (target==NULL)
			valid=false;
		// We check that tooltip is parented with base and has at least one joint in-between:
		if (valid)
		{
			valid=false;
			bool jointPresent=false;
			bool baseOk=false;
			C3DObject* iterat=tooltip;
			while ( (iterat!=base)&&(iterat!=NULL) )
			{
				iterat=iterat->getParent();
				if (iterat==base)
				{
					baseOk=true;
					if (jointPresent)
						valid=true;
				}
				if ( (iterat!=base)&&(iterat!=NULL)&&(iterat->getObjectType()==sim_object_joint_type) )
					jointPresent=true;
			}
			if (!valid)
			{
				element->setActive(false); // This element has an error
				if (!baseOk)
					element->setBase(-1); // The base was illegal!
			}
		}
		if (valid)
		{ 
			tipList.push_back(tooltip);
			targetList.push_back(target);
		}
	}
}

int CikGroup::computeGroupIk(bool forMotionPlanning)
{ // Return value is one of following: sim_ikresult_not_performed, sim_ikresult_success, sim_ikresult_fail
	if (!active)
		return(sim_ikresult_not_performed); // That group is not active!
	if (!forMotionPlanning)
	{
		if (doOnFailOrSuccessOf!=-1)
		{ // Conditional execution part:
			CikGroup* it=ct::ikGroups->getIkGroup(doOnFailOrSuccessOf);
			if (it!=NULL)
			{
				if (doOnPerformed)
				{
					if (it->getCalculationResult()==sim_ikresult_not_performed)
						return(sim_ikresult_not_performed);
					if (it->getCalculationResult()==sim_ikresult_success)
					{
						if (doOnFail)
							return(sim_ikresult_not_performed);
					}
					else
					{
						if (!doOnFail)
							return(sim_ikresult_not_performed);
					}
				}
				else
				{
					if (it->getCalculationResult()!=sim_ikresult_not_performed)
						return(sim_ikresult_not_performed);
				}
			}
		}
	}

	// Now we prepare a vector with all valid and active elements:
	std::vector<CikEl*> validElements;
	validElements.reserve(ikElements.size());
	validElements.clear();

	for (int elementNumber=0;elementNumber<int(ikElements.size());elementNumber++)
	{
		CikEl* element=ikElements[elementNumber];
		CDummy* tooltip=ct::objCont->getDummy(element->getTooltip());
		C3DObject* base=ct::objCont->getObject(element->getBase());
		bool valid=true;
		if (!element->getActive())
			valid=false;
		if (tooltip==NULL)
			valid=false; // should normally never happen!
		// We check that tooltip is parented with base and has at least one joint in-between:
		if (valid)
		{
			valid=false;
			bool jointPresent=false;
			bool baseOk=false;
			C3DObject* iterat=tooltip;
			while ( (iterat!=base)&&(iterat!=NULL) )
			{
				iterat=iterat->getParent();
				if (iterat==base)
				{
					baseOk=true;
					if (jointPresent)
						valid=true;
				}
				if ( (iterat!=base)&&(iterat!=NULL)&&(iterat->getObjectType()==sim_object_joint_type) )
				{ 
					if ( (((CJoint*)iterat)->getJointMode()==sim_jointmode_ik)||(((CJoint*)iterat)->getJointMode()==sim_jointmode_reserved_previously_ikdependent)||(((CJoint*)iterat)->getJointMode()==sim_jointmode_dependent) )
						jointPresent=true;
				}
			}
			if (!valid)
			{
				element->setActive(false); // This element has an error
				if (!baseOk)
					element->setBase(-1); // The base was illegal!
			}
		}
		if (valid)
			validElements.push_back(element);
	}
	// Now validElements contains all valid elements we have to use in the following computation!
	if (validElements.size()==0)
	{
		return(sim_ikresult_fail); // Error!
	}

	_resetTemporaryParameters();

	// Here we have the main iteration loop:
	float interpolFact=1.0f; // We first try to solve in one step
	int successNumber=0;
	bool limitOrAvoidanceNeedMoreCalculation;
	bool leaveNow=false;
	bool errorOccured=false;
	for (int iterationNb=0;iterationNb<maxIterations;iterationNb++)
	{
		// Here we prepare all element equations:
		for (int elementNumber=0;elementNumber<int(validElements.size());elementNumber++)
		{
			CikEl* element=validElements[elementNumber];
			element->prepareIkEquations(interpolFact);
		}

		int res=performOnePass(&validElements,limitOrAvoidanceNeedMoreCalculation,interpolFact,forMotionPlanning);
		if (res==-1)
		{
			errorOccured=true;
			break;
		}
		if (res==1)
		{ // Joint variations within tolerance
			successNumber++;
			//MARC_PRIVATEif (doubleInterpolFactAfter<=successNumber)
			//MARC_PRIVATE}

			// We check if all IK elements are under the required precision and
			// that there are not active joint limitation or avoidance equations
			bool posAndOrAreOk=true;
			for (int elementNumber=0;elementNumber<int(validElements.size());elementNumber++)
			{
				CikEl* element=validElements[elementNumber];
				bool posit,orient;
				element->checkIfWithinTolerance(posit,orient,true);
				if (!(posit&&orient))
				{
					posAndOrAreOk=false;
					break;
				}
			}
			if (posAndOrAreOk&&(!limitOrAvoidanceNeedMoreCalculation))
				leaveNow=true; // Everything is fine, we can leave here
		}
		else
		{ // Joint variations not within tolerance
			successNumber=0;
			interpolFact=interpolFact/2.0f;
			_resetTemporaryParameters();
		}

		// Here we remove all element equations (free memory)
		for (int elementNumber=0;elementNumber<int(validElements.size());elementNumber++)
		{
			CikEl* element=validElements[elementNumber];
			element->removeIkEquations();
		}
		if (leaveNow)
			break;
	}
	int returnValue=sim_ikresult_success;
	if (errorOccured)
		returnValue=sim_ikresult_fail;
	bool setNewValues=(!errorOccured);
	for (int elementNumber=0;elementNumber<int(validElements.size());elementNumber++)
	{
		CikEl* element=validElements[elementNumber];
		bool posit,orient;
		element->checkIfWithinTolerance(posit,orient,true);
		if ( (!posit)||(!orient) )
		{
			returnValue=sim_ikresult_fail;
			if ( (restoreIfPositionNotReached&&(!posit))||
				(restoreIfOrientationNotReached&&(!orient)) )
				setNewValues=false;
		}
	}

	// We set all joint parameters:
	if (setNewValues)
		_applyTemporaryParameters();
	return(returnValue);
}

void CikGroup::_resetTemporaryParameters()
{
	// We prepare all joint temporary parameters:
	for (int jc=0;jc<int(ct::objCont->jointList.size());jc++)
	{
		CJoint* it=ct::objCont->getJoint(ct::objCont->jointList[jc]);

		it->setPosition(it->getPosition(),true);
		it->initializeParametersForIK(getJointTreshholdAngular());
	}
	// We prepare all dummy temporary parameters (needed for rail-type mechanisms):
	for (int jc=0;jc<int(ct::objCont->dummyList.size());jc++)
	{ // It doesn't matter if the dummy is free or not or if its parent is a path. All parameters must be set
		CDummy* it=ct::objCont->getDummy(ct::objCont->dummyList[jc]);
		it->setTempLocalTransformation(it->getLocalTransformation());
	}
}

void CikGroup::_applyTemporaryParameters()
{
	// Joints:
	for (int jc=0;jc<int(ct::objCont->jointList.size());jc++)
	{
		CJoint* it=ct::objCont->getJoint(ct::objCont->jointList[jc]);

		it->setPosition(it->getPosition(true),false);
		it->applyTempParametersEx();
	}
	// Dummies: (for rail-type mechanisms)
	for (int jc=0;jc<int(ct::objCont->dummyList.size());jc++)
	{
		CDummy* it=ct::objCont->getDummy(ct::objCont->dummyList[jc]);
		it->setLocalTransformation(it->getTempLocalTransformation());
	}
}

int CikGroup::performOnePass(std::vector<CikEl*>* validElements,bool& limitOrAvoidanceNeedMoreCalculation,float interpolFact,bool forMotionPlanning)
{	// Return value -1 means that an error occured --> keep old configuration
	// Return value 0 means that the max. angular or linear variation were overpassed.
	// Return value 1 means everything went ok
	// In that case the joints temp. values are not actualized. Another pass is needed
	// Here we have the multi-ik solving algorithm:
	//********************************************************************************
	limitOrAvoidanceNeedMoreCalculation=false;
	// We prepare a vector of all used joints and a counter for the number of rows:
	std::vector<CJoint*> allJoints;
	std::vector<int> allJointStages;
	int numberOfRows=0;
	for (int elementNumber=0;elementNumber<int(validElements->size());elementNumber++)
	{
		CikEl* element=validElements->at(elementNumber);
		numberOfRows=numberOfRows+element->matrix->rows;
		for (int i=0;i<int(element->rowJointIDs->size());i++)
		{
			int current=element->rowJointIDs->at(i);
			int currentStage=element->rowJointStages->at(i);
			// We check if that joint is already present:
			bool present=false;
			for (int j=0;j<int(allJoints.size());j++)
			{
				if ( (allJoints[j]->getID()==current)&&(allJointStages[j]==currentStage) )
				{
					present=true;
					break;
				}
			}
			if (!present)
			{
				allJoints.push_back(ct::objCont->getJoint(current));
				allJointStages.push_back(currentStage);
			}
		}
	}

	// Now we prepare the obstacle avoidance part:
	//---------------------------------------------------------------------------
	std::vector<float> avoidanceError;
	std::vector<CMatrix*> avoidanceMatrix;
	std::vector<std::vector<int>*> avoidanceRowIDs;

	numberOfRows=numberOfRows+int(avoidanceError.size());

	//---------------------------------------------------------------------------

	// Now we prepare the joint limitation part:
	//---------------------------------------------------------------------------
	std::vector<float> limitationError;
	std::vector<int> limitationIndex;
	std::vector<float> limitationValue;
	if (_correctJointLimits)
	{
		for (int jointCounter=0;jointCounter<int(allJoints.size());jointCounter++)
		{
			CJoint* it=allJoints[jointCounter];
			int stage=allJointStages[jointCounter];
			float minVal=it->getPositionIntervalMin();
			float range=it->getPositionIntervalRange();
			float value=it->getPosition(true);
			if (it->getJointType()==sim_joint_revolute_subtype)
			{ // We have to handle a revolute joint
				if (!it->getPositionIsCyclic())
				{ // Limitation applies only if joint is not cyclic!
					float distFromMin=value-jointTreshholdAngular-minVal;
					float distFromMax=value+jointTreshholdAngular-(minVal+range);
					float eq=0.0f;
					float activate=-10.0f;
					if (distFromMin<0.0f)
					{
						activate=1.0f; // We correct in the positive direction
						eq=-distFromMin;
					}
					if (distFromMax>0.0f)
					{
						activate=-1.0f; // We correct in the negative direction
						eq=distFromMax;
					}
					if (activate>-5.0f)
					{ // We have to activate a joint limitation equation!
						// If we are over the treshhold of more than 5%:
						// (important in case target and tooltip are within tolerance)
						if (eq>(jointTreshholdAngular*0.05f))
							limitOrAvoidanceNeedMoreCalculation=true;
						// First the error part:
						limitationError.push_back(eq*jointLimitWeight);
						// Now the matrix part:
						limitationIndex.push_back(jointCounter);
						limitationValue.push_back(activate);
					}
				}
			}
			if (it->getJointType()==sim_joint_prismatic_subtype)
			{ // We have to handle a prismatic joint:
				float distFromMin=value-(minVal+jointTreshholdLinear);
				float distFromMax=value-(minVal+range-jointTreshholdLinear);
				float eq=0.0f;
				float activate=-10.0f;
				if ( (distFromMin<0.0f)&&(fabs(distFromMin)<fabs(distFromMax)) )
				{
					activate=1.0f; // We correct in the positive direction
					eq=-distFromMin;
				}
				if ( (distFromMax>0.0f)&&(fabs(distFromMax)<fabs(distFromMin)) )
				{
					activate=-1.0f; // We correct in the negative direction
					eq=distFromMax;
				}
				if (activate>-5.0f)
				{ // We have to activate a joint limitation equation!
					// If we are over the treshhold of more than 5%:
					// (important in case target and tooltip are within tolerance)
					if (eq>(jointTreshholdLinear*0.05f))
						limitOrAvoidanceNeedMoreCalculation=true;
					// First the error part:
					limitationError.push_back(eq*jointLimitWeight);
					// Now the matrix part:
					limitationIndex.push_back(jointCounter);
					limitationValue.push_back(activate);
				}
			}
			if (it->getJointType()==sim_joint_spherical_subtype)
			{ // We have to handle a spherical joint
				if ( (it->getTempSphericalJointLimitations()!=0)&&(stage==1) ) // Joint limitation configuration was activated!
				{
					float v=it->getTempParameterEx(stage);
					float distFromMax=v+jointTreshholdAngular-(minVal+range);
					float eq;
					float activate=-10.0f;
					if (distFromMax>0.0f)
					{
						activate=-1.0f; // We correct in the negative direction
						eq=distFromMax;
					}
					if (activate>-5.0f)
					{ // We have to activate a joint limitation equation!
						// If we are over the treshhold of more than 5%:
						// (important in case target and tooltip are within tolerance)
						if (eq>(jointTreshholdAngular*0.05f))
							limitOrAvoidanceNeedMoreCalculation=true;
						// First the error part:
						limitationError.push_back(eq*jointLimitWeight);
						// Now the matrix part:
						limitationIndex.push_back(jointCounter);
						limitationValue.push_back(activate);
					}
				}
			}
		}
	}
	numberOfRows=numberOfRows+int(limitationError.size());
	//---------------------------------------------------------------------------

	// Now we prepare the individual joint constraints part:
	//---------------------------------------------------------------------------
	for (int i=0;i<int(allJoints.size());i++)
	{
		if (allJoints[i]->getJointType()!=sim_joint_spherical_subtype)
		{
			if ( (allJoints[i]->getJointMode()==sim_jointmode_reserved_previously_ikdependent)||(allJoints[i]->getJointMode()==sim_jointmode_dependent) )
				numberOfRows++;
		}
	}
	//---------------------------------------------------------------------------

	// We prepare the main matrix and the main error vector.
	CMatrix mainMatrix(numberOfRows,int(allJoints.size()));
	CMatrix mainMatrix_correctJacobian(numberOfRows,int(allJoints.size()));
	// We have to zero it first:
	mainMatrix.clear();
	mainMatrix_correctJacobian.clear();
	CMatrix mainErrorVector(numberOfRows,1);
	
	// Now we fill in the main matrix and the main error vector:
	int currentRow=0;
	for (int elementNumber=0;elementNumber<int(validElements->size());elementNumber++)
	{
		CikEl* element=validElements->at(elementNumber);
		for (int i=0;i<element->errorVector->rows;i++)
		{ // We go through the rows:
			// We first set the error part:
			mainErrorVector(currentRow,0)=(*element->errorVector)(i,0);
			// Now we set the delta-parts:
			for (int j=0;j<element->matrix->cols;j++)
			{ // We go through the columns:
				// We search for the right entry
				int jointID=element->rowJointIDs->at(j);
				int stage=element->rowJointStages->at(j);
				int index=0;
				while ( (allJoints[index]->getID()!=jointID)||(allJointStages[index]!=stage) )
					index++;
				mainMatrix(currentRow,index)=(*element->matrix)(i,j);
				mainMatrix_correctJacobian(currentRow,index)=(*element->matrix_correctJacobian)(i,j);
			}
			currentRow++;
		}
	}
	// Now we add the joint limitation equations:
	for (int i=0;i<int(limitationError.size());i++)
	{ // We go through the rows:
		mainErrorVector(currentRow,0)=limitationError[i];
		// Now we set the delta-part:
		mainMatrix(currentRow,limitationIndex[i])=limitationValue[i];
		mainMatrix_correctJacobian(currentRow,limitationIndex[i])=limitationValue[i];
		currentRow++;
	}

	// Now we add the avoidance equations:
	for (int avNumber=0;avNumber<int(avoidanceError.size());avNumber++)
	{
		CMatrix* avMatr=avoidanceMatrix[avNumber];
		float avErr=avoidanceError[avNumber];
		std::vector<int>* avIDs=avoidanceRowIDs[avNumber];

		mainErrorVector(currentRow,0)=avErr;
		// Now we set the delta-parts:
		for (int j=0;j<avMatr->cols;j++)
		{ // We go through the columns:
			// We search for the right entry
			int jointID=avIDs->at(j);
			int index=0;
			while (allJoints[index]->getID()!=jointID)
				index++;
			mainMatrix(currentRow,index)=(*avMatr)(0,j);
			mainMatrix_correctJacobian(currentRow,index)=(*avMatr)(0,j);
		}
		currentRow++;
	}
	// We free the avoidance equations:
	for (int i=0;i<int(avoidanceError.size());i++)
	{
		delete avoidanceMatrix[i];
		avoidanceRowIDs[i]->clear();
		delete avoidanceRowIDs[i];
	}
	avoidanceError.clear();
	avoidanceMatrix.clear();
	avoidanceRowIDs.clear();	

	// Now we prepare the individual joint constraints part:
	//---------------------------------------------------------------------------
	for (int i=0;i<int(allJoints.size());i++)
	{
		if ( ((allJoints[i]->getJointMode()==sim_jointmode_dependent)||(allJoints[i]->getJointMode()==sim_jointmode_reserved_previously_ikdependent))&&(allJoints[i]->getJointType()!=sim_joint_spherical_subtype) )
		{
			int dependenceID=allJoints[i]->getDependencyJointID();
			if (dependenceID!=-1)
			{
				bool found=false;
				int j;
				for (j=0;j<int(allJoints.size());j++)
				{
					if (allJoints[j]->getID()==dependenceID)
					{
						found=true;
						break;
					}
				}
				if (found)
				{
					dependenceID=j;
					float coeff=allJoints[i]->getDependencyJointCoeff();
					float fact=allJoints[i]->getDependencyJointFact();
					mainErrorVector(currentRow,0)=((allJoints[i]->getPosition(true)-fact)-
									coeff*allJoints[dependenceID]->getPosition(true))*interpolFact;
					mainMatrix(currentRow,i)=-1.0f;
					mainMatrix(currentRow,dependenceID)=coeff;
					mainMatrix_correctJacobian(currentRow,i)=-1.0f;
					mainMatrix_correctJacobian(currentRow,dependenceID)=coeff;
				}
				else
				{	// joint of dependenceID is not part of this group calculation:
					// therefore we take its current value --> WRONG! Since all temp params are initialized!
					CJoint* dependentJoint=ct::objCont->getJoint(dependenceID);
					if (dependentJoint!=NULL)
					{
						float coeff=allJoints[i]->getDependencyJointCoeff();
						float fact=allJoints[i]->getDependencyJointFact();
						mainErrorVector(currentRow,0)=((allJoints[i]->getPosition(true)-fact)-
										coeff*dependentJoint->getPosition(true))*interpolFact;
						mainMatrix(currentRow,i)=-1.0f;
						mainMatrix_correctJacobian(currentRow,i)=-1.0f;
					}
				}
			}
			else
			{				
				mainErrorVector(currentRow,0)=interpolFact*(allJoints[i]->getPosition(true)-allJoints[i]->getDependencyJointFact());
				mainMatrix(currentRow,i)=-1.0f;
				mainMatrix_correctJacobian(currentRow,i)=-1.0f;
			}
			currentRow++;
		}
	}
	//---------------------------------------------------------------------------

	// We take the joint weights into account here (part1):
	for (int i=0;i<mainMatrix.rows;i++)
	{
		for (int j=0;j<int(allJoints.size());j++)
		{
			float coeff=allJoints[j]->getIKWeight();
			if (coeff>=0.0f)
				coeff=sqrtf(coeff);
			else
				coeff=-sqrtf(-coeff);
			mainMatrix(i,j)=mainMatrix(i,j)*coeff;
			mainMatrix_correctJacobian(i,j)=mainMatrix_correctJacobian(i,j)*coeff;
		}
	}
	// Now we just have to solve:
	int doF=mainMatrix.cols;
	int eqNumb=mainMatrix.rows;
	CMatrix solution(doF,1);

	if (!forMotionPlanning)
	{
		delete _lastJacobian;
		_lastJacobian=new CMatrix(mainMatrix_correctJacobian); //mainMatrix);
	}

	if (calculationMethod==sim_ik_pseudo_inverse_method)
	{
		CMatrix JT(mainMatrix);
		JT.transpose();
		CMatrix pseudoJ(doF,eqNumb);
		CMatrix JJTInv(mainMatrix*JT);
		if (!JJTInv.inverse())
			return(-1);
		pseudoJ=JT*JJTInv;
		solution=pseudoJ*mainErrorVector;
	}
	if (calculationMethod==sim_ik_damped_least_squares_method)
	{
		CMatrix JT(mainMatrix);
		JT.transpose();
		CMatrix DLSJ(doF,eqNumb);
		CMatrix JJTInv(mainMatrix*JT);
		CMatrix ID(mainMatrix.rows,mainMatrix.rows);
		ID.setIdentity();
		ID/=1.0f/(dlsFactor*dlsFactor);
		JJTInv+=ID;
		if (!JJTInv.inverse())
			return(-1);
		DLSJ=JT*JJTInv;
		solution=DLSJ*mainErrorVector;
	}
	if (calculationMethod==sim_ik_jacobian_transpose_method)
	{
		CMatrix JT(mainMatrix);
		JT.transpose();
		solution=JT*mainErrorVector;
	}

	// We take the joint weights into account here (part2):
	for (int i=0;i<doF;i++)
	{
		CJoint* it=allJoints[i];
		float coeff=sqrtf(fabs(it->getIKWeight()));
		solution(i,0)=solution(i,0)*coeff;
	}

	// We check if some variations are too big:
	if (!ignoreMaxStepSizes)
	{
		for (int i=0;i<doF;i++)
		{
			CJoint* it=allJoints[i];
			if (it->getJointType()!=sim_joint_prismatic_subtype)
				solution(i,0)=tt::getNormalizedAngle(solution(i,0));
			if (fabs(solution(i,0))>it->getMaxStepSize())
				return(0);
		}
	}
	// Now we set the computed values
	for (int i=0;i<doF;i++)
	{
		CJoint* it=allJoints[i];
		int stage=allJointStages[i];
		if (it->getJointType()!=sim_joint_spherical_subtype)
			it->setPosition(it->getPosition(true)+solution(i,0),true);
		else
			it->setTempParameterEx(it->getTempParameterEx(stage)+solution(i,0),stage);
	}
	return(1);
}

float*  CikGroup::getLastJacobianData(int matrixSize[2])
{
	if (_lastJacobian==NULL)
		return(NULL);
	matrixSize[0]=_lastJacobian->cols;
	matrixSize[1]=_lastJacobian->rows;
	return(_lastJacobian->data);
}


float*  CikGroup::getLastManipulabilityValue(int matrixSize[2])
{
	static float v;
	if (_lastJacobian==NULL)
		return(NULL);
	matrixSize[0]=1;
	matrixSize[1]=1;

	CMatrix JT(_lastJacobian[0]);
	JT.transpose();
	CMatrix JJT(_lastJacobian[0]*JT);

	v=sqrt(getDeterminant(JJT,NULL,NULL));

	return(&v);
}

float CikGroup::getDeterminant(const CMatrix& m,const std::vector<int>* activeRows,const std::vector<int>* activeColumns)
{ // activeRows and activeColumns are NULL by default (--> all rows and columns are active)
	// Routine is recursive! (i.e. Laplace expansion, which is not efficient for large matrices!)
	if (activeRows==NULL)
	{ // First call goes here:
		std::vector<int> actR;
		std::vector<int> actC;
		for (int i=0;i<m.cols;i++)
		{
			actR.push_back(i);
			actC.push_back(i);
		}
		return(getDeterminant(m,&actR,&actC));
	}
	// If we arrived here, we have to compute the determinant of the sub-matrix obtained
	// by removing all rows and columns not listed in activeRows, respectively activeColumns
	if (activeRows->size()==2)
	{ // We compute this directly, we have a two-by-two matrix:
		float retVal=0.0f;
		retVal+=m(activeRows->at(0),activeColumns->at(0))*m(activeRows->at(1),activeColumns->at(1));
		retVal-=m(activeRows->at(0),activeColumns->at(1))*m(activeRows->at(1),activeColumns->at(0));
		return(retVal);
	}

	if (activeRows->size()==3)
	{ // We compute this directly, we have a three-by-three matrix:
		float retVal=0.0f;
		retVal+=m(activeRows->at(0),activeColumns->at(0)) * ( (m(activeRows->at(1),activeColumns->at(1))*m(activeRows->at(2),activeColumns->at(2))) - (m(activeRows->at(1),activeColumns->at(2))*m(activeRows->at(2),activeColumns->at(1))) );
		retVal-=m(activeRows->at(0),activeColumns->at(1)) * ( (m(activeRows->at(1),activeColumns->at(0))*m(activeRows->at(2),activeColumns->at(2))) - (m(activeRows->at(1),activeColumns->at(2))*m(activeRows->at(2),activeColumns->at(0))) );
		retVal+=m(activeRows->at(0),activeColumns->at(2)) * ( (m(activeRows->at(1),activeColumns->at(0))*m(activeRows->at(2),activeColumns->at(1))) - (m(activeRows->at(1),activeColumns->at(1))*m(activeRows->at(2),activeColumns->at(0))) );
		return(retVal);
	}

	// The general routine
	std::vector<int> actR;
	std::vector<int> actC;
	float retVal=0.0f;

	for (int colInd=1;colInd<int(activeColumns->size());colInd++)
		actC.push_back(activeColumns->at(colInd));
	for (int rowInd=0;rowInd<int(activeRows->size());rowInd++)
	{
		actR.clear();
		int i=activeRows->at(rowInd);
		for (int rowInd2=0;rowInd2<int(activeRows->size());rowInd2++)
		{
			int j=activeRows->at(rowInd2);
			if (j!=i)
				actR.push_back(j);
		}
	retVal+=m(i,activeColumns->at(0))*getDeterminant(m,&actR,&actC)*pow(float(-1.0f),float(rowInd+2)); // was rowInd+1 until 3.1.3 rev2.
	}
	return(retVal);
}



void CikGroup::serializeRExtIk(CExtIkSer &ar)
{
	while (ikElements.size()!=0)
		removeIkElement(ikElements[0]->getObjectID());

	objectID=ar.readInt();

	objectName=ar.readString();

	maxIterations=ar.readInt();

	constraints=ar.readInt();
	
	jointLimitWeight=ar.readFloat();

	jointTreshholdAngular=ar.readFloat();
	jointTreshholdLinear=ar.readFloat();

	dlsFactor=ar.readFloat();

	calculationMethod=ar.readInt();

	doOnFailOrSuccessOf=ar.readInt();

	unsigned char nothing=ar.readByte();
	active=SIM_IS_BIT_SET(nothing,0);
	restoreIfPositionNotReached=SIM_IS_BIT_SET(nothing,1);
	restoreIfOrientationNotReached=SIM_IS_BIT_SET(nothing,2);
	doOnFail=SIM_IS_BIT_SET(nothing,3);
	doOnPerformed=SIM_IS_BIT_SET(nothing,4);
	ignoreMaxStepSizes=!SIM_IS_BIT_SET(nothing,5);
	_explicitHandling=SIM_IS_BIT_SET(nothing,6);

	nothing=ar.readByte();
	_correctJointLimits=SIM_IS_BIT_SET(nothing,0);

	int el=ar.readInt();
	for (int i=0;i<el;i++)
	{
		CikEl* it=new CikEl();
		it->serializeRExtIk(ar);
		ikElements.push_back(it);
	}
}
