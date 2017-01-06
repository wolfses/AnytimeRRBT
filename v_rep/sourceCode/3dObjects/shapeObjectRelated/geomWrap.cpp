// This file is part of V-REP, the Virtual Robot Experimentation Platform.
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// V-REP is dual-licensed, under the terms of EITHER (at your option):
//   1. V-REP commercial license (contact us for details)
//   2. GNU GPL (see below)
// 
// GNU GPL license:
// -------------------------------------------------------------------
// V-REP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// V-REP IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with V-REP.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.1.2 on June 16th 2014

#include "vrepPrecompiledHeader.h"
#include "v_rep_internal.h"
#include "geomWrap.h"
#include "geometric.h"
#include "Tt.h"
#include "Global.h"
#include "OGL.h"

#include "ViewableBase.h"
#include "IloIlo.h"
#include "App.h"

CGeomWrap::CGeomWrap()
{
	_mass=1.0f;
	_name="sub__0";

	_dynMaterialId=SIM_IDSTART_DYNMATERIAL; // that means: default material from this V-REP release! Happens when shapes are newly created

	__bulletStickyContact=false;
	__bulletRestitution=0.0f;
	__bulletFriction=1.0f;
	__bulletLinearDamping=0.0f;
	__bulletAngularDamping=0.0f;
	__bulletNonDefaultCollisionMarginFactor=0.1f;
	__bulletNonDefaultCollisionMargin=false;
	__bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape=0.002f;
	__bulletNonDefaultCollisionMargin_forConvexAndNonPureShape=true;
	__bulletAutoShrinkConvexMesh=false;

	__odeMaxContacts=8;
	__odeFriction=1.0f;
	__odeSoftERP=0.2f;
	__odeSoftCFM=0.0f;
	__odeLinearDamping=0.0f;
	__odeAngularDamping=0.0f;

	/*
	// Vortex parameters:
	// ----------------------------------------------------
	_vortexFloatParams.push_back(0.5f); // friction coeff., linear axis, primary
	_vortexFloatParams.push_back(0.5f); // friction coeff., linear axis, secondary
	_vortexFloatParams.push_back(0.0f); // friction coeff., angular axis, primary
	_vortexFloatParams.push_back(0.0f); // friction coeff., angular axis, secondary
	_vortexFloatParams.push_back(0.0f); // friction coeff., angular axis, normal

	_vortexFloatParams.push_back(1.1f); // static friction scale, linear axis, primary (was 1.0f)
	_vortexFloatParams.push_back(1.1f); // static friction scale, linear axis, secondary (was 1.0f)
	_vortexFloatParams.push_back(1.0f); // static friction scale, angular axis, primary
	_vortexFloatParams.push_back(1.0f); // static friction scale, angular axis, secondary
	_vortexFloatParams.push_back(1.0f); // static friction scale, angular axis, normal

	_vortexFloatParams.push_back(0.00000001f); // compliance (was 0.000005f)
	_vortexFloatParams.push_back(10000000.0f); // damping (was 0.0f)
	_vortexFloatParams.push_back(0.0f); // restitution
	_vortexFloatParams.push_back(0.5f); // restitution threshold
	_vortexFloatParams.push_back(0.0f); // adhesive force
	_vortexFloatParams.push_back(0.0f); // linear velocity damping
	_vortexFloatParams.push_back(0.0f); // angular velocity damping

	_vortexFloatParams.push_back(0.0f); // slide, linear axis, primary
	_vortexFloatParams.push_back(0.0f); // slide, linear axis, secondary
	_vortexFloatParams.push_back(0.0f); // slide, angular axis, primary
	_vortexFloatParams.push_back(0.0f); // slide, angular axis, secondary
	_vortexFloatParams.push_back(0.0f); // slide, angular axis, normal

	_vortexFloatParams.push_back(0.0f); // slip, linear axis, primary (was 0.0001f)
	_vortexFloatParams.push_back(0.0f); // slip, linear axis, secondary (was 0.0001f)
	_vortexFloatParams.push_back(0.0f); // slip, angular axis, primary
	_vortexFloatParams.push_back(0.0f); // slip, angular axis, secondary
	_vortexFloatParams.push_back(0.0f); // slip, angular axis, normal

	_vortexFloatParams.push_back(0.14f); // autosleep linear speed threshold
	_vortexFloatParams.push_back(0.045f); // autosleep linear accel threshold
	_vortexFloatParams.push_back(0.03f); // autosleep angular speed threshold
	_vortexFloatParams.push_back(0.045f); // autosleep angular accel threshold

	_vortexFloatParams.push_back(0.002); // skin thickness

	_vortexIntParams.push_back(4); // friction model, linear axis, primary (0=box, 1=scaledBox, 2=proportionalLow, 3=proportionalHigh, 4=scaledBoxFast, 5=neutral, 6=none)
	_vortexIntParams.push_back(4); // friction model, linear axis, secondary
	_vortexIntParams.push_back(6); // friction model, angular axis, primary
	_vortexIntParams.push_back(6); // friction model, angular axis, secondary
	_vortexIntParams.push_back(6); // friction model, angular axis, none

	_vortexIntParams.push_back(8+16+32+64+128); // bit coded: 0set=treat pure shapes as convex shapes, 1set=treat convex shapes as random shapes, 2set=threat random shapes as terrain, 3set=fast moving, 4set=auto-slip, 
												// 5set=sec. lin. axis uses same values as prim. lin. axis, 6set=sec. ang. axis uses same values as prim. ang. axis, 7set=norm. ang. axis uses same values as prim. ang. axis (bits 5-7 are not taken into account on the plugin side: they just serve as setting memory
	_vortexIntParams.push_back(10); // autosleep step live threshold
	// ----------------------------------------------------
*/

	_convex=false;

	setDefaultInertiaParams();

	_transformationsSinceGrouping.setIdentity();
}

CGeomWrap::~CGeomWrap()
{
	for (int i=0;i<int(childList.size());i++)
		delete childList[i];
}

C7Vector CGeomWrap::getTransformationsSinceGrouping()
{
	return(_transformationsSinceGrouping);
}

void CGeomWrap::setTransformationsSinceGrouping(const C7Vector& tr)
{
	_transformationsSinceGrouping=tr;
}

void CGeomWrap::display(CGeomProxy* geomData,int displayAttrib,CVisualParam* collisionColor,int dynObjFlag_forVisualization,int transparencyHandling)
{ // function has virtual/non-virtual counterpart!
	for (int i=0;i<int(childList.size());i++)
		childList[i]->display(geomData,displayAttrib,collisionColor,dynObjFlag_forVisualization,transparencyHandling);
}

void CGeomWrap::display_colorCoded(CGeomProxy* geomData,int objectId)
{ // function has virtual/non-virtual counterpart!
	for (int i=0;i<int(childList.size());i++)
		childList[i]->display_colorCoded(geomData,objectId);
}


void CGeomWrap::displayForCutting(CGeomProxy* geomData,int displayAttrib,CVisualParam* collisionColor)
{ // function has virtual/non-virtual counterpart!
	for (int i=0;i<int(childList.size());i++)
		childList[i]->displayForCutting(geomData,displayAttrib,collisionColor);
}

void CGeomWrap::displayGhost(CGeomProxy* geomData,int displayAttrib,bool originalColors,bool backfaceCulling,float transparency,const float* newColors)
{ // function has virtual/non-virtual counterpart!
	for (int i=0;i<int(childList.size());i++)
		childList[i]->displayGhost(geomData,displayAttrib,originalColors,backfaceCulling,transparency,newColors);
}

void CGeomWrap::perform3DObjectLoadingMapping(std::vector<int>* map)
{ // function has virtual/non-virtual counterpart!
	for (int i=0;i<int(childList.size());i++)
		childList[i]->perform3DObjectLoadingMapping(map);
}

void CGeomWrap::performTextureObjectLoadingMapping(std::vector<int>* map)
{ // function has virtual/non-virtual counterpart!
	for (int i=0;i<int(childList.size());i++)
		childList[i]->performTextureObjectLoadingMapping(map);
}

void CGeomWrap::performDynMaterialObjectLoadingMapping(std::vector<int>* map)
{
	_dynMaterialId=App::ct->objCont->getLoadingMapping(map,_dynMaterialId);
}

void CGeomWrap::announce3DObjectWillBeErased(int objectID)
{ // function has virtual/non-virtual counterpart!
	for (int i=0;i<int(childList.size());i++)
		childList[i]->announce3DObjectWillBeErased(objectID);
}

void CGeomWrap::setTextureDependencies(int shapeID)
{ // function has virtual/non-virtual counterpart!
	for (int i=0;i<int(childList.size());i++)
		childList[i]->setTextureDependencies(shapeID);
}

int CGeomWrap::getTextureCount()
{ // function has virtual/non-virtual counterpart!
	int retVal=0;
	for (int i=0;i<int(childList.size());i++)
		retVal+=childList[i]->getTextureCount();
	return(retVal);
}

bool CGeomWrap::hasTextureThatUsesFixedTextureCoordinates()
{ // function has virtual/non-virtual counterpart!
	for (int i=0;i<int(childList.size());i++)
	{
		if (childList[i]->hasTextureThatUsesFixedTextureCoordinates())
			return(true);
	}
	return(false);
}

void CGeomWrap::removeAllTextures()
{ // function has virtual/non-virtual counterpart!
	for (int i=0;i<int(childList.size());i++)
		childList[i]->removeAllTextures();
}

bool CGeomWrap::getContainsTransparentComponents()
{ // function has virtual/non-virtual counterpart!
	for (int i=0;i<int(childList.size());i++)
	{
		if (childList[i]->getContainsTransparentComponents())
			return(true);
	}
	return(false);
}

float CGeomWrap::getGouraudShadingAngle()
{ // function has virtual/non-virtual counterpart!
	return(childList[0]->getGouraudShadingAngle()); // we just return the first angle we encounter! Normally never used
}

void CGeomWrap::setGouraudShadingAngle(float angle)
{ // function has virtual/non-virtual counterpart!
	for (int i=0;i<int(childList.size());i++)
		childList[i]->setGouraudShadingAngle(angle);
}

float CGeomWrap::getEdgeThresholdAngle()
{ // function has virtual/non-virtual counterpart!
	return(childList[0]->getEdgeThresholdAngle()); // we just return the first angle we encounter! Normally never used
}

void CGeomWrap::setEdgeThresholdAngle(float angle)
{ // function has virtual/non-virtual counterpart!
	for (int i=0;i<int(childList.size());i++)
		childList[i]->setEdgeThresholdAngle(angle);
}

CGeomWrap* CGeomWrap::copyYourself()
{ // function has virtual/non-virtual counterpart!
	CGeomWrap* newIt=new CGeomWrap();
	copyWrapperInfos(newIt);
	return(newIt);
}

void CGeomWrap::copyWrapperInfos(CGeomWrap* target)
{
	target->_mass=_mass;
	target->_name=_name;
	target->_convex=_convex;
/*
	target->_bulletStickyContact=_bulletStickyContact;
	target->_bulletRestitution=_bulletRestitution;
	target->_bulletFriction=_bulletFriction;
	target->_bulletLinearDamping=_bulletLinearDamping;
	target->_bulletAngularDamping=_bulletAngularDamping;
	target->_bulletNonDefaultCollisionMarginFactor=_bulletNonDefaultCollisionMarginFactor;
	target->_bulletNonDefaultCollisionMargin=_bulletNonDefaultCollisionMargin;

	target->_bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape=_bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape;
	target->_bulletNonDefaultCollisionMargin_forConvexAndNonPureShape=_bulletNonDefaultCollisionMargin_forConvexAndNonPureShape;

	target->_bulletAutoShrinkConvexMesh=_bulletAutoShrinkConvexMesh;

	target->_odeMaxContacts=_odeMaxContacts;
	target->_odeFriction=_odeFriction;
	target->_odeSoftERP=_odeSoftERP;
	target->_odeSoftCFM=_odeSoftCFM;
	target->_odeLinearDamping=_odeLinearDamping;
	target->_odeAngularDamping=_odeAngularDamping;
	*/

/*	target->_vortexFloatParams.assign(_vortexFloatParams.begin(),_vortexFloatParams.end());
	target->_vortexIntParams.assign(_vortexIntParams.begin(),_vortexIntParams.end()); */

	target->_localInertiaFrame=_localInertiaFrame;
	target->_principalMomentsOfInertia=_principalMomentsOfInertia;

	target->_transformationsSinceGrouping=_transformationsSinceGrouping;

	target->_dynMaterialId=_dynMaterialId;

	for (int i=0;i<int(childList.size());i++)
		target->childList.push_back(childList[i]->copyYourself());
}

void CGeomWrap::setMass(float m)
{
	_mass=tt::getLimitedFloat(0.0001f,100000.0f,m);
}

float CGeomWrap::getMass()
{
	return(_mass);
}

void CGeomWrap::setDefaultInertiaParams()
{
	_localInertiaFrame.setIdentity();
	_principalMomentsOfInertia(0)=0.001f;
	_principalMomentsOfInertia(1)=0.001f;
	_principalMomentsOfInertia(2)=0.001f;
}

void CGeomWrap::setName(std::string newName)
{
	_name=newName;
}

std::string CGeomWrap::getName()
{
	return(_name);
}

void CGeomWrap::displayInertia(float bboxDiagonal)
{
	C7Vector tr(getLocalInertiaFrame());
	glPushMatrix();
	glTranslatef(tr.X(0),tr.X(1),tr.X(2));
	C4Vector axis=tr.Q.getAngleAndAxisNoChecking();
	glRotatef(axis(0)*radToDeg,axis(1),axis(2),axis(3));	

	ogl::setColorsAllBlack();
	ogl::setColor(1.0f,0.0f,0.0f,AMBIENT_MODE);
	ogl::setColor(0.2f,0.2f,0.2f,DIFFUSE_MODE);
	ogl::setColor(0.2f,0.2f,0.2f,SPECULAR_MODE);

	C3Vector me(getPrincipalMomentsOfInertia());
	C3Vector ma;
	ma(0)=(me(1)+me(2))*(me(1)+me(2));
	ma(1)=(me(0)+me(2))*(me(0)+me(2));
	ma(2)=(me(1)+me(0))*(me(1)+me(0));
	ma.normalize();
	float mf=bboxDiagonal*0.6f;
	glBegin(GL_LINE_STRIP);
	glVertex3f(-ma(0)*mf,-ma(1)*mf,+ma(2)*mf);
	glVertex3f(-ma(0)*mf,-ma(1)*mf,-ma(2)*mf);
	glVertex3f(+ma(0)*mf,-ma(1)*mf,-ma(2)*mf);
	glVertex3f(+ma(0)*mf,-ma(1)*mf,+ma(2)*mf);
	glVertex3f(-ma(0)*mf,-ma(1)*mf,+ma(2)*mf);
	glVertex3f(-ma(0)*mf,+ma(1)*mf,+ma(2)*mf);
	glVertex3f(+ma(0)*mf,+ma(1)*mf,+ma(2)*mf);
	glVertex3f(+ma(0)*mf,+ma(1)*mf,-ma(2)*mf);
	glVertex3f(-ma(0)*mf,+ma(1)*mf,-ma(2)*mf);
	glVertex3f(-ma(0)*mf,+ma(1)*mf,+ma(2)*mf);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-ma(0)*mf,-ma(1)*mf,-ma(2)*mf);
	glVertex3f(-ma(0)*mf,+ma(1)*mf,-ma(2)*mf);
	glVertex3f(+ma(0)*mf,-ma(1)*mf,-ma(2)*mf);
	glVertex3f(+ma(0)*mf,+ma(1)*mf,-ma(2)*mf);
	glVertex3f(+ma(0)*mf,-ma(1)*mf,+ma(2)*mf);
	glVertex3f(+ma(0)*mf,+ma(1)*mf,+ma(2)*mf);
	glEnd();

	glLineWidth(3.0f);
	ogl::drawReference(bboxDiagonal*0.5f,0.8f,true,false);

	ogl::setColor(0.4f,0.0f,1.0f,AMBIENT_MODE);
	float l=pow(_mass/1000.0f,0.3333f); // Cubic root and mass density of 1000
	glLineWidth(6.0f);
	ogl::displayBox(l,l,l,false);
	glLineWidth(1.0f);

	glPopMatrix();
}

/*
// Engine setters/getters:
// ************************
void CGeomWrap::setBulletRestitution(float r)
{
	_bulletRestitution=tt::getLimitedFloat(0.0f,10.0f,r);
}

float CGeomWrap::getBulletRestitution()
{
	return(_bulletRestitution);
}

void CGeomWrap::setBulletFriction(float f)
{
	if (f<0.0f)
		f=0.0f;
	_bulletFriction=f;
}

float CGeomWrap::getBulletFriction()
{
	return(_bulletFriction);
}

void CGeomWrap::setBulletStickyContact(bool sticky)
{
	_bulletStickyContact=sticky;
}

bool CGeomWrap::getBulletStickyContact()
{
	return(_bulletStickyContact);
}

void CGeomWrap::setBulletLinearDamping(float d)
{
	if (d<0.0f)
		d=0.0f;
	_bulletLinearDamping=d;
}

float CGeomWrap::getBulletLinearDamping()
{
	return(_bulletLinearDamping);
}

void CGeomWrap::setBulletAngularDamping(float d)
{
	if (d<0.0f)
		d=0.0f;
	_bulletAngularDamping=d;
}

float CGeomWrap::getBulletAngularDamping()
{
	return(_bulletAngularDamping);
}
*/
/*
void CGeomWrap::setBulletNonDefaultCollisionMargin(bool b)
{
	setBulletNonDefaultCollisionMargin_(_convex&&(!isPure()),b);
}
*/
/*
void CGeomWrap::setBulletNonDefaultCollisionMargin_(bool for_convex_and_not_pure,bool b)
{
	if (for_convex_and_not_pure)
		_bulletNonDefaultCollisionMargin_forConvexAndNonPureShape=b;
	else
		_bulletNonDefaultCollisionMargin=b;
}
*/
/*
bool CGeomWrap::getBulletNonDefaultCollisionMargin()
{
	return(getBulletNonDefaultCollisionMargin_(_convex&&(!isPure())));
}
*/
/*
bool CGeomWrap::getBulletNonDefaultCollisionMargin_(bool for_convex_and_not_pure)
{
	if (for_convex_and_not_pure)
		return(_bulletNonDefaultCollisionMargin_forConvexAndNonPureShape);
	else
		return(_bulletNonDefaultCollisionMargin);
}
*/
/*
void CGeomWrap::setBulletNonDefaultCollisionMarginFactor(float m)
{
	setBulletNonDefaultCollisionMarginFactor_(_convex&&(!isPure()),m);
}
*/
/*
void CGeomWrap::setBulletNonDefaultCollisionMarginFactor_(bool for_convex_and_not_pure,float m)
{
	m=tt::getLimitedFloat(0.0001f,1000.0f,m);
	if (for_convex_and_not_pure)
		_bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape=m;
	else
		_bulletNonDefaultCollisionMarginFactor=m;
}
*/
/*
float CGeomWrap::getBulletNonDefaultCollisionMarginFactor()
{
	return(getBulletNonDefaultCollisionMarginFactor_(_convex&&(!isPure())));
}
*/
/*
float CGeomWrap::getBulletNonDefaultCollisionMarginFactor_(bool for_convex_and_not_pure)
{
	if (for_convex_and_not_pure)
		return(_bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape);
	else
		return(_bulletNonDefaultCollisionMarginFactor);
}

void CGeomWrap::setBulletAutoShrinkConvexMesh(bool s)
{
	_bulletAutoShrinkConvexMesh=s;
}

bool CGeomWrap::getBulletAutoShrinkConvexMesh()
{
	return(_bulletAutoShrinkConvexMesh);
}

void CGeomWrap::setOdeMaxContacts(int m)
{
	_odeMaxContacts=tt::getLimitedInt(-64,64,m);
}

int CGeomWrap::getOdeMaxContacts()
{
	return(_odeMaxContacts);
}

void CGeomWrap::setOdeFriction(float f)
{
	if (f<0.0f)
		f=0.0f;
	_odeFriction=f;
}

float CGeomWrap::getOdeFriction()
{
	return(_odeFriction);
}

void CGeomWrap::setOdeSoftERP(float e)
{
	_odeSoftERP=tt::getLimitedFloat(0.0f,1.0f,e);
}

float CGeomWrap::getOdeSoftERP()
{
	return(_odeSoftERP);
}

void CGeomWrap::setOdeSoftCFM(float e)
{
	_odeSoftCFM=tt::getLimitedFloat(0.0f,1.0f,e);
}

float CGeomWrap::getOdeSoftCFM()
{
	return(_odeSoftCFM);
}

void CGeomWrap::setOdeLinearDamping(float d)
{
	if (d<0.0f)
		d=0.0f;
	_odeLinearDamping=d;
}

float CGeomWrap::getOdeLinearDamping()
{
	return(_odeLinearDamping);
}

void CGeomWrap::setOdeAngularDamping(float d)
{
	if (d<0.0f)
		d=0.0f;
	_odeAngularDamping=d;
}

float CGeomWrap::getOdeAngularDamping()
{
	return(_odeAngularDamping);
}
*/

/*
void CGeomWrap::getVortexFloatParams(std::vector<float>& p)
{
	p.assign(_vortexFloatParams.begin(),_vortexFloatParams.end());
}

void CGeomWrap::setVortexFloatParams(const std::vector<float>& p)
{
	_vortexFloatParams.assign(p.begin(),p.end());
	_vortexFloatParams[31]=tt::getLimitedFloat(0.0f,10.0f,_vortexFloatParams[31]); // skin thickness
}

void CGeomWrap::getVortexIntParams(std::vector<int>& p)
{
	p.assign(_vortexIntParams.begin(),_vortexIntParams.end());
}

void CGeomWrap::setVortexIntParams(const std::vector<int>& p)
{
	_vortexIntParams.assign(p.begin(),p.end());
	_vortexIntParams[6]=tt::getLimitedInt(1,10000,_vortexIntParams[6]); // auto-sleep steps
}
*/
int CGeomWrap::getDynMaterialId()
{
	return(_dynMaterialId);
}

void CGeomWrap::setDynMaterialId(int id)
{
	_dynMaterialId=id;
}

CDynMaterialObject* CGeomWrap::createDynMaterialObject()
{ // needed for backward compatibility (12/9/2013)
	CDynMaterialObject* mat=new CDynMaterialObject();

	mat->setBulletAngularDamping(__bulletAngularDamping);
	mat->setBulletAutoShrinkConvexMesh(__bulletAutoShrinkConvexMesh);
	mat->setBulletFriction(__bulletFriction);
	mat->setBulletLinearDamping(__bulletLinearDamping);
	mat->setBulletNonDefaultCollisionMargin(true,__bulletNonDefaultCollisionMargin_forConvexAndNonPureShape);
	mat->setBulletNonDefaultCollisionMarginFactor(true,__bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape);
	mat->setBulletNonDefaultCollisionMargin(false,__bulletNonDefaultCollisionMargin);
	mat->setBulletNonDefaultCollisionMarginFactor(false,__bulletNonDefaultCollisionMarginFactor);
	mat->setBulletRestitution(__bulletRestitution);
	mat->setBulletStickyContact(__bulletStickyContact);

	mat->setOdeAngularDamping(__odeAngularDamping);
	mat->setOdeFriction(__odeFriction);
	mat->setOdeLinearDamping(__odeLinearDamping);
	mat->setOdeMaxContacts(__odeMaxContacts);
	mat->setOdeSoftCFM(__odeSoftCFM);
	mat->setOdeSoftERP(__odeSoftERP);

	// Try to guess the friction we would need for Vortex:
	float averageFriction=0.0f;
	if (__bulletFriction>1.0f)
		averageFriction+=1.0f;
	else
		averageFriction+=__bulletFriction;
	if (__odeFriction>1.0f)
		averageFriction+=1.0f;
	else
		averageFriction+=__odeFriction;
	averageFriction*=0.5f;
	if (averageFriction<0.01f)
	{
		std::vector<int> ip;
		mat->getVortexIntParams(ip);
		ip[0]=6;
		ip[1]=6;
		mat->setVortexIntParams(ip);
	}
	else
	{
		std::vector<float> fp;
		mat->getVortexFloatParams(fp);
		fp[0]=averageFriction;
		fp[1]=averageFriction;
		mat->setVortexFloatParams(fp);
	}

	return(mat);
}

CDynMaterialObject* CGeomWrap::getDynMaterialObject()
{
	CDynMaterialObject* obj=App::ct->dynMaterialCont->getObject(_dynMaterialId);
	if (obj==NULL)
		obj=App::ct->dynMaterialCont->getDefaultObject(); // should normally never happen!
	return(obj);
}


/*
void CGeomWrap::copyEnginePropertiesTo(CGeomWrap* target)
{
	target->_dynMaterialId=_dynMaterialId;

	// Bullet:
	target->_bulletStickyContact=_bulletStickyContact;
	target->_bulletRestitution=_bulletRestitution;
	target->_bulletFriction=_bulletFriction;
	target->_bulletLinearDamping=_bulletLinearDamping;
	target->_bulletAngularDamping=_bulletAngularDamping;
	target->_bulletNonDefaultCollisionMarginFactor=_bulletNonDefaultCollisionMarginFactor;
	target->_bulletNonDefaultCollisionMargin=_bulletNonDefaultCollisionMargin;
	target->_bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape=_bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape;
	target->_bulletNonDefaultCollisionMargin_forConvexAndNonPureShape=_bulletNonDefaultCollisionMargin_forConvexAndNonPureShape;
	target->_bulletAutoShrinkConvexMesh=_bulletAutoShrinkConvexMesh;

	// ODE:
	target->_odeMaxContacts=_odeMaxContacts;
	target->_odeFriction=_odeFriction;
	target->_odeSoftERP=_odeSoftERP;
	target->_odeSoftCFM=_odeSoftCFM;
	target->_odeLinearDamping=_odeLinearDamping;
	target->_odeAngularDamping=_odeAngularDamping;

	// Vortex:
	target->_vortexFloatParams.assign(_vortexFloatParams.begin(),_vortexFloatParams.end());
	target->_vortexIntParams.assign(_vortexIntParams.begin(),_vortexIntParams.end());
}
*/

C7Vector CGeomWrap::getLocalInertiaFrame()
{
	return (_localInertiaFrame);
}

void CGeomWrap::setLocalInertiaFrame(const C7Vector& li)
{
	_localInertiaFrame=li;
}

C3Vector CGeomWrap::getPrincipalMomentsOfInertia()
{
	return (_principalMomentsOfInertia);
}

void CGeomWrap::setPrincipalMomentsOfInertia(const C3Vector& inertia)
{
	_principalMomentsOfInertia=inertia;
	_principalMomentsOfInertia(0)=tt::getLimitedFloat(0.0f,10000.0f,_principalMomentsOfInertia(0));
	_principalMomentsOfInertia(1)=tt::getLimitedFloat(0.0f,10000.0f,_principalMomentsOfInertia(1));
	_principalMomentsOfInertia(2)=tt::getLimitedFloat(0.0f,10000.0f,_principalMomentsOfInertia(2));
	if (_principalMomentsOfInertia.getLength()==0.0f)
		_principalMomentsOfInertia(0)=0.001f; // make sure we don't have a zero vector (problems with Bullet? and V-REP!)
}

void CGeomWrap::scale(float xVal,float yVal,float zVal)
{ // function has virtual/non-virtual counterpart!
	// iso-scaling for grouped shapes!! (should normally already be xVal=yVal=zVal)
	scaleWrapperInfos(xVal,xVal,xVal);
}

void CGeomWrap::scaleWrapperInfos(float xVal,float yVal,float zVal)
{
	scaleMassAndInertia(xVal,yVal,zVal);

	_localInertiaFrame.X(0)*=xVal;
	_localInertiaFrame.X(1)*=yVal;
	_localInertiaFrame.X(2)*=zVal;

	_transformationsSinceGrouping.X(0)*=xVal;
	_transformationsSinceGrouping.X(1)*=yVal;
	_transformationsSinceGrouping.X(2)*=zVal;

	for (int i=0;i<int(childList.size());i++)
		childList[i]->scale(xVal,yVal,zVal);
	if ((xVal<0.0f)||(yVal<0.0f)||(zVal<0.0f)) // that effectively flips faces!
		checkIfConvex();
}

void CGeomWrap::scaleMassAndInertia(float xVal,float yVal,float zVal)
{
	_mass*=xVal*yVal*zVal;
	_principalMomentsOfInertia(0)*=yVal*zVal;
	_principalMomentsOfInertia(1)*=xVal*zVal;
	_principalMomentsOfInertia(2)*=xVal*yVal;
}

void CGeomWrap::setPurePrimitiveType(int theType,float xOrDiameter,float y,float zOrHeight)
{ // function has virtual/non-virtual counterpart!
	// Following added on 14/03/2011 because a grouped shape composed by pure and non pure shapes would decompose as pure shapes with wrong orientation!
	if (theType==sim_pure_primitive_none)
	{
		for (int i=0;i<int(childList.size());i++)
			childList[i]->setPurePrimitiveType(theType,xOrDiameter,y,zOrHeight);
		// _convex=false;  NO!!!
	}
}

int CGeomWrap::getPurePrimitiveType()
{ // function has virtual/non-virtual counterpart!
	return(childList[0]->getPurePrimitiveType()); // we just return the first type we encounter! Normally never used
}

bool CGeomWrap::isGeometric()
{ // function has virtual/non-virtual counterpart!
	return(false);
}

bool CGeomWrap::isPure()
{ // function has virtual/non-virtual counterpart!
	return(childList[0]->isPure());
}

bool CGeomWrap::isConvex()
{ // function has virtual/non-virtual counterpart!
	return(_convex);
}

bool CGeomWrap::checkIfConvex()
{ // function has virtual/non-virtual counterpart!
	_convex=true;
	for (int i=0;i<int(childList.size());i++)
		_convex&=childList[i]->checkIfConvex();
	setConvex(_convex);
	return(_convex);
}

void CGeomWrap::setConvex(bool convex)
{ // function has virtual/non-virtual counterpart!
	_convex=convex; // This is just for the wrapper!
	/* removed on 24/3/2013
	if (_convex)
	{ // convex shape handling in Bullet includes a very large margin. We can:
		// 1. shrink the dynamic model using _bulletAutoShrinkConvexMesh. This adds some initial preprocessing time, can lead to crashes, and edges and points appear shifted inwards. Faces react correctly.
		// 2. reduce the margin (what we do here). Erwin from Bullet doesn't recommend it (doesn't say why), but I got the best (still not good!!) results with it
		_bulletNonDefaultCollisionMarginFactor=0.002f;
		_bulletNonDefaultCollisionMargin=true;
	}
	else
		_bulletNonDefaultCollisionMargin=false;
		*/
}

void CGeomWrap::getCumulativeMeshes(std::vector<float>& vertices,std::vector<int>* indices,std::vector<float>* normals)
{ // function has virtual/non-virtual counterpart!
	for (int i=0;i<int(childList.size());i++)
		childList[i]->getCumulativeMeshes(vertices,indices,normals);
}

void CGeomWrap::setColor(const char* colorName,int colorComponent,const float* rgbData)
{ // function has virtual/non-virtual counterpart!
	for (int i=0;i<int(childList.size());i++)
		childList[i]->setColor(colorName,colorComponent,rgbData);
}

bool CGeomWrap::getColor(const char* colorName,int colorComponent,float* rgbData)
{ // function has virtual/non-virtual counterpart!
	bool retVal=false;
	for (int i=0;i<int(childList.size());i++)
		retVal|=childList[i]->getColor(colorName,colorComponent,rgbData);
	return(retVal);
}

void CGeomWrap::getAllShapeComponentsCumulative(std::vector<CGeometric*>& shapeComponentList)
{	// function has virtual/non-virtual counterpart!
	// needed by the dynamics routine. We return ALL shape components!
	for (int i=0;i<int(childList.size());i++)
		childList[i]->getAllShapeComponentsCumulative(shapeComponentList);
}

CGeometric* CGeomWrap::getShapeComponentAtIndex(int& index)
{ // function has virtual/non-virtual counterpart!
	if (index<0)
		return(NULL);
	for (int i=0;i<int(childList.size());i++)
	{
		CGeometric* retVal=childList[i]->getShapeComponentAtIndex(index);
		if (retVal!=NULL)
			return(retVal);
		if (index<0)
			return(NULL);
	}
	return(NULL);
}

void CGeomWrap::preMultiplyAllVerticeLocalFrames(const C7Vector& preTr)
{ // function has virtual/non-virtual counterpart!

	_transformationsSinceGrouping=preTr*_transformationsSinceGrouping;
	_localInertiaFrame=preTr*_localInertiaFrame;

	for (int i=0;i<int(childList.size());i++)
		childList[i]->preMultiplyAllVerticeLocalFrames(preTr);
}

void CGeomWrap::flipFaces()
{ // function has virtual/non-virtual counterpart!
	for (int i=0;i<int(childList.size());i++)
		childList[i]->flipFaces();
	checkIfConvex();
}

void CGeomWrap::serialize(CSer& ar)
{ // function has virtual/non-virtual counterpart!
	serializeWrapperInfos(ar);
}

void CGeomWrap::serializeWrapperInfos(CSer& ar)
{
	if (ar.isStoring())
	{		// Storing
		ar.storeDataName("Nme");
		ar << _name;
		ar.flush();

		ar.storeDataName("Mas");
		ar << _mass;
		ar.flush();

		ar.storeDataName("Dmi");
		ar << _dynMaterialId;
		ar.flush();
/*
		ar.storeDataName("Bu2");
		ar << _bulletRestitution << _bulletFriction << _bulletLinearDamping << _bulletAngularDamping << _bulletNonDefaultCollisionMarginFactor << _bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape;
		ar.flush();

		ar.storeDataName("Ode");
		ar << _odeMaxContacts << _odeFriction << _odeSoftERP << _odeSoftCFM << _odeLinearDamping << _odeAngularDamping;
		ar.flush();

		ar.storeDataName("Vo2"); // vortex params:
		ar << int(_vortexFloatParams.size()) << int(_vortexIntParams.size());
		for (int i=0;i<int(_vortexFloatParams.size());i++)
			ar << _vortexFloatParams[i];
		for (int i=0;i<int(_vortexIntParams.size());i++)
			ar << _vortexIntParams[i];
		ar.flush();
*/
		ar.storeDataName("Ine");
		ar << _localInertiaFrame(0) << _localInertiaFrame(1) << _localInertiaFrame(2) << _localInertiaFrame(3);
		ar << _localInertiaFrame(4) << _localInertiaFrame(5) << _localInertiaFrame(6);
		ar << _principalMomentsOfInertia(0) << _principalMomentsOfInertia(1) << _principalMomentsOfInertia(2);
		ar.flush();

		ar.storeDataName("Vtb");
		ar << _transformationsSinceGrouping(0) << _transformationsSinceGrouping(1) << _transformationsSinceGrouping(2) << _transformationsSinceGrouping(3);
		ar << _transformationsSinceGrouping(4) << _transformationsSinceGrouping(5) << _transformationsSinceGrouping(6);
		ar.flush();

		ar.storeDataName("Var");
		BYTE nothing=0;
		// reserved (12/9/2013) SIM_SET_CLEAR_BIT(nothing,0,_bulletNonDefaultCollisionMargin);
		// reserved (12/9/2013) SIM_SET_CLEAR_BIT(nothing,1,_bulletStickyContact);
		SIM_SET_CLEAR_BIT(nothing,2,_convex);
		// reserved (12/9/2013) SIM_SET_CLEAR_BIT(nothing,3,!_bulletAutoShrinkConvexMesh);
		// reserved (12/9/2013) SIM_SET_CLEAR_BIT(nothing,4,_bulletNonDefaultCollisionMargin_forConvexAndNonPureShape); // since 24/3/2013
		SIM_SET_CLEAR_BIT(nothing,5,true); // means: we do not have to make the convectivity test for this shape (was already done). Added this on 28/1/2013
		ar << nothing;
		ar.flush();

		for (int i=0;i<int(childList.size());i++)
		{
			if (childList[i]->isGeometric())
				ar.storeDataName("Geo");
			else
				ar.storeDataName("Wrp");
			ar.setCountingMode();
			childList[i]->serialize(ar);
			if (ar.setWritingMode())
				childList[i]->serialize(ar);
		}

		ar.storeDataName(SER_END_OF_OBJECT);
	}
	else
	{		// Loading
		int byteQuantity;
		std::string theName="";
		bool doBugCorrection_untilVrep3_0_2=false;
		while (theName.compare(SER_END_OF_OBJECT)!=0)
		{
			theName=ar.readDataName();
			if (theName.compare(SER_END_OF_OBJECT)!=0)
			{
				bool noHit=true;
				if (theName.compare("Nme")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _name;
				}
				if (theName.compare("Mas")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _mass;
					if (_mass==0.0f) // to catch an old bug
						_mass=0.001f;
				}
				if (theName.compare("Dmi")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _dynMaterialId;
				}
				if (theName.compare("Bul")==0)
				{ // keep for backward compatibilitiy (24/3/2013)
					noHit=false;
					ar >> byteQuantity;
					ar >> __bulletRestitution >> __bulletFriction >> __bulletLinearDamping >> __bulletAngularDamping >> __bulletNonDefaultCollisionMarginFactor;
					doBugCorrection_untilVrep3_0_2=true; // until V-REP 3.0.2
					_dynMaterialId=-1; // -1 means: we have to insert this material
				}
				if (theName.compare("Bu2")==0)
				{ // keep for backward compatibilitiy (12/9/2013)
					noHit=false;
					ar >> byteQuantity;
					ar >> __bulletRestitution >> __bulletFriction >> __bulletLinearDamping >> __bulletAngularDamping >> __bulletNonDefaultCollisionMarginFactor >> __bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape;
					_dynMaterialId=-1; // -1 means: we have to insert this material
				}
				if (theName.compare("Ode")==0)
				{ // keep for backward compatibilitiy (12/9/2013)
					noHit=false;
					ar >> byteQuantity;
					ar >> __odeMaxContacts >> __odeFriction >> __odeSoftERP >> __odeSoftCFM >> __odeLinearDamping >> __odeAngularDamping;
					_dynMaterialId=-1; // -1 means: we have to insert this material
				}
				/*
				if (theName.compare("Vo2")==0)
				{ // keep for backward compatibilitiy (12/9/2013)
					noHit=false;
					ar >> byteQuantity;
					int cnt1,cnt2;
					ar >> cnt1 >> cnt2;
					_vortexFloatParams.clear();
					_vortexIntParams.clear();
					float vf;
					int vi;
					for (int i=0;i<cnt1;i++)
					{
						ar >> vf;
						_vortexFloatParams.push_back(vf);
					}
					for (int i=0;i<cnt2;i++)
					{
						ar >> vi;
						_vortexIntParams.push_back(vi);
					}
					_dynMaterialId=-1; // -1 means: we have to insert this material
				}
				*/
				if (theName.compare("Ine")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _localInertiaFrame(0) >> _localInertiaFrame(1) >> _localInertiaFrame(2) >> _localInertiaFrame(3);
					ar >> _localInertiaFrame(4) >> _localInertiaFrame(5) >> _localInertiaFrame(6);
					ar >> _principalMomentsOfInertia(0) >> _principalMomentsOfInertia(1) >> _principalMomentsOfInertia(2);
				}

				if (theName.compare("Vtb")==0)
				{
					noHit=false;
					ar >> byteQuantity;
					ar >> _transformationsSinceGrouping(0) >> _transformationsSinceGrouping(1) >> _transformationsSinceGrouping(2) >> _transformationsSinceGrouping(3);
					ar >> _transformationsSinceGrouping(4) >> _transformationsSinceGrouping(5) >> _transformationsSinceGrouping(6);
				}

				if (theName=="Var")
				{
					noHit=false;
					ar >> byteQuantity;
					BYTE nothing;
					ar >> nothing;
					__bulletNonDefaultCollisionMargin=SIM_IS_BIT_SET(nothing,0); // keep for backward compatibility (12/9/2013)
					__bulletStickyContact=SIM_IS_BIT_SET(nothing,1); // keep for backward compatibility (12/9/2013)
					_convex=SIM_IS_BIT_SET(nothing,2);
					__bulletAutoShrinkConvexMesh=!SIM_IS_BIT_SET(nothing,3); // keep for backward compatibility (12/9/2013)
					__bulletNonDefaultCollisionMargin_forConvexAndNonPureShape=SIM_IS_BIT_SET(nothing,4); // keep for backward compatibility (12/9/2013)
					// reserved doTheConvectivityTest=!SIM_IS_BIT_SET(nothing,5); // since version 3.0.2 (version 3.0.1 was buggy)
				}
				if (theName.compare("Geo")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					CGeometric* it=new CGeometric();
					it->serialize(ar);
					childList.push_back(it);
				}
				if (theName.compare("Wrp")==0)
				{
					noHit=false;
					ar >> byteQuantity; // never use that info, unless loading unknown data!!!! (undo/redo stores dummy info in there)
					CGeomWrap* it=new CGeomWrap();
					it->serialize(ar);
					childList.push_back(it);
				}
				if (noHit)
					ar.loadUnknownData();
			}
		}
//**************************************
		if (!isGeometric())
		{
			if ((!App::ct->undoBufferContainer->isUndoSavingOrRestoringUnderWay())&&doBugCorrection_untilVrep3_0_2)
			{
				checkIfConvex();
				if ( (fabs(__bulletNonDefaultCollisionMarginFactor-0.002f)<0.0001f) && ( (ar.getVrepVersionThatWroteThisFile()==30002)||(ar.getVrepVersionThatWroteThisFile()==30001) ) )
				{ // Here we very probably correct a bug that affected version 3.0.2! (some people might have reverted to version 3.0.1, but the bug then was saved in 3.0.1, so we also check that version)
					__bulletNonDefaultCollisionMarginFactor=0.1f;
					__bulletNonDefaultCollisionMargin=false; // !!!!
					__bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape=0.002f;
					__bulletNonDefaultCollisionMargin_forConvexAndNonPureShape=true;
				}
			}
		}
		if (App::userSettings->forceBugFix_rel30002)
		{
			__bulletNonDefaultCollisionMarginFactor=0.1f;
			__bulletNonDefaultCollisionMargin=false;
			__bulletNonDefaultCollisionMarginFactor_forConvexAndNonPureShape=0.002f;
			__bulletNonDefaultCollisionMargin_forConvexAndNonPureShape=true;
		}
//**************************************
	}
}


void CGeomWrap::findPrincipalMomentOfInertia(const C3X3Matrix& tensor,C4Vector& rotation,C3Vector& principalMoments)
{ // This routine is iterative and not elegant. But we do not need speed here anyway ;)
	C3X3Matrix rot;
	C3X3Matrix tens(tensor);
	rot.setIdentity();
	float w=_getTensorNonDiagonalMeasure(tensor);

	// With below settings, we will get an orientational precision of about 0.000128 degrees
	float stepSize=10.0f*degToRad;
	const float stepSizeMultiplier=0.2f;
	const int resolutionScalePasses=8;

	C3X3Matrix drot;
	for (int scale=0;scale<resolutionScalePasses;scale++)
	{ // resolution loop
		for (int axis=0;axis<3;axis++)
		{ // axis loop (x, y, z)
			if (axis==0)
				drot.buildXRotation(stepSize);
			if (axis==1)
				drot.buildYRotation(stepSize);
			if (axis==2)
				drot.buildZRotation(stepSize);
			int dirChangeCnt=0;
			while (dirChangeCnt<2)
			{ // error reduction loop for a given axis
				rot=rot*drot;
				tens=drot.getTranspose()*tens*drot;
				float w2=_getTensorNonDiagonalMeasure(tens);
				if (w2>=w)
				{ // it got worse
					dirChangeCnt++;
					drot.transpose();
					rot=rot*drot; // reverse change
					tens=drot.getTranspose()*tens*drot;
				}
				else
					w=w2; // it got better
			}
		}
		stepSize*=stepSizeMultiplier;
	}

	principalMoments(0)=tens.axis[0](0);
	principalMoments(1)=tens.axis[1](1);
	principalMoments(2)=tens.axis[2](2);
	rotation=rot.getQuaternion();
}

float CGeomWrap::_getTensorNonDiagonalMeasure(const C3X3Matrix& tensor)
{
	C3Vector v(tensor.axis[1](0),tensor.axis[2](0),tensor.axis[2](1));
	return(v*v);
}

C3X3Matrix CGeomWrap::getNewTensor(const C3Vector& principalMoments,const C7Vector& newFrame)
{ // remember that we always work with a massless tensor. The tensor is multiplied with the mass in the dynamics module!
	C3X3Matrix tensor;
	tensor.clear();
	tensor.axis[0](0)=principalMoments(0);
	tensor.axis[1](1)=principalMoments(1);
	tensor.axis[2](2)=principalMoments(2);
	// 1. reorient the frame:
	tensor=newFrame.Q.getMatrix()*tensor*newFrame.Q.getInverse().getMatrix();
	// 2. shift the frame:
	C3X3Matrix D;
	D.setIdentity();
	D*=(newFrame.X*newFrame.X);
	D.axis[0]-=newFrame.X*newFrame.X(0);
	D.axis[1]-=newFrame.X*newFrame.X(1);
	D.axis[2]-=newFrame.X*newFrame.X(2);
	tensor+=D;
	return(tensor);
}
