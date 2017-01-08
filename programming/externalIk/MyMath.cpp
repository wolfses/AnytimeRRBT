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

#include "MyMath.h"

CMath::CMath()
{
}

CMath::~CMath()
{
}

void CMath::limitValue(float minValue,float maxValue,float &value)
{
	if (value>maxValue)
		value=maxValue;
	if (value<minValue) 
		value=minValue;
}

void CMath::limitValue(int minValue,int maxValue,int &value)
{
	if (value>maxValue) 
		value=maxValue;
	if (value<minValue) 
		value=minValue;
}

float CMath::robustAsin(float v)
{
	if (!isFloatNumberOk(v))
	{
		// GENERATE AN ERROR MESSAGE HERE: IDSNOTR_NO_NUMBER_ERROR1
		return(0.0f);
	}
	if (v>=1.0f)
		return(piValue*0.5f);
	if (v<=-1.0f)
		return(-piValue*0.5f);
	return(asinf(v));
}

float CMath::robustAcos(float v)
{
	if (!isFloatNumberOk(v))
	{
		// GENERATE AN ERROR MESSAGE HERE: IDSNOTR_NO_NUMBER_ERROR2
		return(0.0f);
	}
	if (v>=1.0f)
		return(0.0f);
	if (v<=-1.0f)
		return(piValue);
	return(acosf(v));
}

float CMath::robustFmod(float v,float w)
{
	if ( (!isFloatNumberOk(v))||(!isFloatNumberOk(w)) )
	{
		// GENERATE AN ERROR MESSAGE HERE: IDSNOTR_NO_NUMBER_ERROR3
		return(0.0f);
	}
	if (w==0.0f)
		return(0.0f);
	return(fmod(v,w));
}

double CMath::robustmod(double v,double w)
{
	if ( (!isDoubleNumberOk(v))||(!isDoubleNumberOk(w)) )
	{
		// GENERATE AN ERROR MESSAGE HERE: IDSNOTR_NO_NUMBER_ERROR4
		return(0.0);
	}
	if (w==0.0)
		return(0.0);
	return(fmod(v,w));
}

bool CMath::isFloatNumberOk(float v)
{
	return ( (!VREP_IS_NAN(v))&&(fabs(v)!=std::numeric_limits<float>::infinity()) );	
}

bool CMath::isDoubleNumberOk(double v)
{
	return ( (!VREP_IS_NAN(v))&&(fabs(v)!=std::numeric_limits<double>::infinity()) );	
}
