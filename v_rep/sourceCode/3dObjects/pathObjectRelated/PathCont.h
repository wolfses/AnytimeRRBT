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

#pragma once

#include "SimplePathPoint.h"
#include "BezierPathPoint.h"
#include "VisualParam.h"

class CPath;

class CPathCont  
{
public:
	CPathCont();
	virtual ~CPathCont(); 

	int getLifeID();
	void render(bool pathEditMode,int displayAttrib,bool pathIsOnlySelectedObject,int objectID);
	void serialize(CSer& ar);
	CPathCont* copyYourself();
	void initializeInitialValues(bool simulationIsRunning);
	void simulationEnded();


	// Simple path and bezier path point routines:
	void removeAllSimplePathPoints();								// Triggers an actualizePath() call
	void addSimplePathPoint(CSimplePathPoint* it);					// Triggers an actualizePath() call
	void insertSimplePathPoint(CSimplePathPoint* it,int position);	// Triggers an actualizePath() call
	void removeSimplePathPoint(int position);						// Triggers an actualizePath() call
	void rollPathPoints(bool forward);								// Triggers an actualizePath() call

	CSimplePathPoint* getSimplePathPoint(int position);
	CBezierPathPoint* getBezierPathPoint(int position);
	int getSimplePathPointCount();
	int getBezierPathPointCount();

	void enableActualization(bool enable);
	void actualizePath();

	void setAttributes(int attrib);									// Triggers an actualizePath() call
	int getAttributes();

	// Visual aspect:
	void setLineSize(int size);
	int getLineSize();
	void setSquareSize(float size);
	float getSquareSize();

	void setPosition_ratio(double p);
	float getPosition_ratio();
	
	void setPosition(double p);
	double getPosition();

	double getPosition_linearComponentOnly();
	double getPosition_angularComponentOnly();


	void setNominalVelocity(float v);
	float getNominalVelocity();
	void setTargetNominalVelocity(float v);
	float getTargetNominalVelocity();
	void setAngleVarToDistanceCoeff(float f);				// Triggers an actualizePath call!
	float getAngleVarToDistanceCoeff();
	void setOnSpotDistanceToDistanceCoeff(float f);				// Triggers an actualizePath call!
	float getOnSpotDistanceToDistanceCoeff();

	void setMaxAcceleration(float maxAccel);
	float getMaxAcceleration();

//****************************

	void scaleObject(float scalingFactor);
	void resetPath(CPath* it);
	void handlePath(CPath* it,float deltaTime);
	void handlePath_keepObjectUnchanged(float deltaTime,double& pos_,float& nomVel_,float& targetNomVel_,float maxAccel,bool invertVelocity,bool infiniteAccel); // so that the path can be handled by simFollowPath functions or such!
	float getBezierVirtualPathLength();
	float getBezierNormalPathLength();
	float getBezierAngularPathLength();
	bool getConfigurationOnBezierCurveClosestTo(C3Vector& pt,C7Vector& conf);
	
	bool getPositionOnPathClosestTo(const C3Vector& pt,float& distOnPath);

	WORD getPathModifID();

	bool getPointOnBezierCurveAtNormalDistance(float& l,int& index0,float& t);
	C7Vector _getInterpolatedBezierCurvePoint(int index0,float t);
	void _getInterpolatedBezierCurveData(int index0,float t,int& auxFlags,float auxChannels[4]);

	bool getTransformationOnBezierCurveAtNormalizedVirtualDistance(float l,C7Vector& tr);
	bool getAuxDataOnBezierCurveAtNormalizedVirtualDistance(float l,int& auxFlags,float auxChannels[4]);



	void setAvpTurningCircleRadiusForHalfVelocity(float radius);
	float getAvpTurningCircleRadiusForHalfVelocity();
	void setAvpRelativeVelocityAtRotationAxisChange(float relVel);
	float getAvpRelativeVelocityAtRotationAxisChange();
	void setAvpRelativeAcceleration(float relAccel);
	float getAvpRelativeAcceleration();

	void setPathLengthCalculationMethod(int m);								// Triggers an actualizePath()!
	int getPathLengthCalculationMethod();


	CVisualParam _lineColor;

protected:
	int _removeDoubleBezierPoints(float linTol,float angTol);				// Called from actualizePath()-routine
	void _computeAutomaticOrientation(const std::vector<CPathPoint*>& pc);	// Called from actualizePath()-routine
	void _recomputeBezierPathElementLengths();								// Called from actualizePath()-routine
	void _recomputeBezierPathMaxVelocities();								// Called from actualizePath()-routine

	bool _getNextBezierPathPointIndex(int currentIndex,int& nextIndex);
	bool _getPointOnBezierCurveAtVirtualDistance(float& l,int& index0,float& t);

	bool _getBezierPointsForVirtualDistance(double& l,int& index0,int& index1,bool forwardDirection);


	void _handleAttachedDummies(CPath* it);

	C3Vector _getPointOnBezierCubic(const C3Vector& ptBefore,const C3Vector& ptMiddle,const C3Vector& ptAfter,C3Vector& dir,float t);
	C4Vector _getOrientationOnBezierCubic(const C4Vector& orBefore,const C4Vector& orMiddle,const C4Vector& orAfter,float t);
	void _getDistinctConsecutivePoints(const std::vector<CPathPoint*>& ptCont,std::vector<C4X4Matrix>& tr,std::vector<std::vector<int> >& ptIndices);
	void _applyDistinctConsecutivePoints(const std::vector<CPathPoint*>& ptCont,std::vector<C4X4Matrix>& tr,std::vector<std::vector<int> >& ptIndices);
	void _draw(std::vector<CPathPoint*>& ptCont,bool pathEditMode,bool isPath,bool showLine,bool stripplePts,bool showSquare,bool showOrientation,int lineSize,float squareSize,bool pathIsOnlySelectedObject,int objectID);
	void _recomputeBezierPoints();
	void _removeAllBezierPathPoints();
	CBezierPathPoint* _addBezierPathPoint(const C7Vector& transf,float maxRelAbsVelocity,float onSpotDistance,WORD auxFlags,const float auxChannels[4]);

	std::vector<CSimplePathPoint*> _simplePathPoints;
	std::vector<CBezierPathPoint*> _bezierPathPoints;

//***********************************

	// Variables needed for the reset procedure of the path. Do not serialize
	bool _initialValuesInitialized;
	float _initialNominalVelocity;
	float _initialPosition;
	float _initialTargetNominalVelocity;

	// Path state and other:
	double _position; // This one is double!!!!
	float _nominalVelocity;
	float _targetNominalVelocity;

	float _maxAcceleration;
	int _pathLengthCalculationMethod;

	// Variables for the automatic velocity profile calculation:
	float _avp_turningCircleRadiusForHalfVelocity;
	float _avp_relativeVelocityAtRotationAxisChange;
	float _avp_relativeAcceleration;
//***********************************

	float _angleVarToDistanceCoeff;
	float _onSpotDistanceToDistanceCoeff;

	int _attributes;
	int _lineSize;
	float _squareSize;


	bool _actualizationEnabled;

	// following is calculated:
	C3Vector _startPosition;
	WORD _pathModifID;
	int _lifeID;
};
