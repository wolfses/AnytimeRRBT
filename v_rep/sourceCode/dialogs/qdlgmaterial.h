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

// Not very elegant. Needs refactoring!!

#ifndef QDLGMATERIAL_H
#define QDLGMATERIAL_H

#include "DlgEx.h"
#include "qdlgcolor.h"
#include "VisualParam.h"

namespace Ui {
	class CQDlgMaterial;
}

class CQDlgMaterial : public CDlgEx
{
    Q_OBJECT

public:
    explicit CQDlgMaterial(QWidget *parent = 0);
    ~CQDlgMaterial();

	void refresh();

	void cancelEvent();

	bool needsDestruction();
	bool isLinkedDataValid();
	bool doesInstanceSwitchRequireDestruction();

	CQDlgColor* color;
	int _identification[4]; //{lifeID,itemType (0=3DObject),itemID (if itemType is 0, then objectID, if itemType is 1 or 2 then buttonBlockID), itemSubID (if ItemType is 2, then button uniqueID}
	float* ambient;
	float* diffuse;
	float* specular;
	float* emissive;
	int* shininess;
	bool* transparency;
	bool* flashing;
	bool* simulationTimeFlashing;
	float* flashingFrequency;
	float* flashingPhase;
	float* flashingRatio;
	float* transparencyFactor;
	std::string* colorName;
	bool isModal;

	void colorAdjust(float* col);
	void _initialize(int identification[4],CVisualParam* it,bool amb,bool diff,bool spec,
									bool emiss,bool shin,bool transp,bool flashing,bool colorNameEnabled);
	void _initialize(int identification[4],float ambientP[3],float diffuseP[3],float specularP[3],float emissiveP[3],bool* flashingP,int* shininessP,bool* transparencyP,float* transparencyFactorP,bool* flashSimTimeP,std::string* theColorName);//,float flashParams[3]);

	static void display(int identification[4],CVisualParam* it,QWidget* theParentWindow,bool amb,bool diff,bool spec,
									bool emiss,bool shin,bool transp,bool allowFlashing,bool colorNameEnabled);
	static void display(int identification[4],QWidget* theParentWindow,float ambientP[3],float diffuseP[3],float specularP[3],float emissiveP[3],bool* flashingP,int* shininessP,bool* transparency,float* transparencyFactor,std::string* theColorName);
	static void displayModal(int identification[4],QWidget* theParentWindow,float ambientP[3],float diffuseP[3],float specularP[3],float emissiveP[3],bool* flashingP,int* shininessP,bool* transparency,float* transparencyFactor,std::string* theColorName);

private slots:
	void on_qqAmbientAdjust_clicked();

	void on_qqDiffuseAdjust_clicked();

	void on_qqSpecularAdjust_clicked();

	void on_qqEmissiveAdjust_clicked();

	void on_qqPulsationAdjust_clicked();

	void on_qqShininess_editingFinished();

	void on_qqOpacityEnable_clicked();

	void on_qqOpacity_editingFinished();

	void on_qqColorName_editingFinished();

private:
    Ui::CQDlgMaterial *ui;
};

#endif // QDLGMATERIAL_H
