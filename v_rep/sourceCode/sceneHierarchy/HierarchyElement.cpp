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

// This file requires some serious refactoring!!

#include "vrepPrecompiledHeader.h"
#include "v_rep_internal.h"
#include "HierarchyElement.h"
#include "OGL.h"
#include "IloIlo.h"
#include "imgLoader.h"
#include "App.h"
#include "Tt.h"
#include "geometric.h"
#include <boost/lexical_cast.hpp>

const int CONST_VAL_6=6;

CHierarchyElement::CHierarchyElement(int theObjectID)
{
	children.clear();
	objectID=theObjectID;
}

CHierarchyElement::~CHierarchyElement()
{
	for (int i=0;i<int(children.size());i++)
		delete children[i];
	children.clear();
}

void CHierarchyElement::setSceneName(const std::string& sn)
{
	if (sn=="")
		_sceneName="new scene";
	else
		_sceneName=sn;
}

CHierarchyElement* CHierarchyElement::getElementLinkedWithObject(int objID)
{
	if (objectID==objID)
		return(this);
	CHierarchyElement* retVal=NULL;
	for (int i=0;i<int(children.size());i++)
	{
		retVal=children[i]->getElementLinkedWithObject(objID);
		if (retVal!=NULL)
			return(retVal);
	}
	return(NULL);
}

int CHierarchyElement::addYourChildren()
{ // returns the total number of children
	int retVal=0;
	if (App::ct->objCont->getEditModeType()==NO_EDIT_MODE)
	{
		if ((objectID<0)&&isLocalWorld())
		{ // this is the world!
			for (int i=0;i<int(App::ct->objCont->objectList.size());i++)
			{
				C3DObject* it=App::ct->objCont->getObject(App::ct->objCont->objectList[i]);
				if (it->getParent()==NULL)
				{
					retVal++;
					CHierarchyElement* aKid=new CHierarchyElement(App::ct->objCont->objectList[i]);
					retVal+=aKid->addYourChildren();
					children.push_back(aKid);
				}
			}
		}
		else
		{ // this is a 3DObject!
			C3DObject* it=App::ct->objCont->getObject(objectID);
			if (it!=NULL)
			{
				retVal+=int(it->childList.size());
				for (int i=0;i<int(it->childList.size());i++)
				{
					if (it->childList[i]!=NULL)
					{
						CHierarchyElement* aKid=new CHierarchyElement(it->childList[i]->getID());
						retVal+=aKid->addYourChildren();
						children.push_back(aKid);
					}
				}
			}
		}
	}
	// Nothing needed for the various edit modes (for now!)
	return(retVal);
}

int CHierarchyElement::getLinkedObjectID()
{
	return(objectID);
}

void CHierarchyElement::renderElement_3DObject(CHierarchy* hier,int labelEditObjectID,bool& bright,bool dontDisplay,
		int renderingSize[2],int textPos[2],
		int indentNb,std::vector<int>* vertLines,int minRenderedPos[2],int maxRenderedPos[2],bool forDragAndDrop/*=false*/,int transparentForTreeObjects/*=-1*/,int dropID/*=-1*/,int worldClick/*=-9999*/)
{ // transparentForTreeObjects==-1: normal, transparentForTreeObjects==-2: transparent, otherwise transparent only for objID=transparentForTreeObjects
	const BYTE horizontalShift=13;
	float transparencyFactor=0.0f;
	if ((transparentForTreeObjects>=0)&&(transparentForTreeObjects==objectID))
		transparentForTreeObjects=-2; // from here on, everything is transparent
	if (transparentForTreeObjects==-2)
		transparencyFactor=0.875f;

	bool isOtherWorld=((objectID<0)&&(!isLocalWorld()));

	if (!forDragAndDrop)
	{
		bright=!bright;
		if (textPos[0]<minRenderedPos[0])
			minRenderedPos[0]=textPos[0];
		if (textPos[1]<minRenderedPos[1])
			minRenderedPos[1]=textPos[1];
		if (textPos[0]>maxRenderedPos[0])
			maxRenderedPos[0]=textPos[0];
		if (textPos[1]>maxRenderedPos[1])
			maxRenderedPos[1]=textPos[1];

		if (textPos[1]<-HIERARCHY_INTER_LINE_SPACE*App::sc)
			dontDisplay=true; // We don't display what is outside of the view!
	}

	bool textInside=(textPos[1]<renderingSize[1]+HIERARCHY_INTER_LINE_SPACE*App::sc);
	C3DObject* it=App::ct->objCont->getObject(objectID);
	std::string theText;
	if (it!=NULL)
		theText=it->getName();
	else
	{
		theText+=_sceneName;
		theText+=tt::decorateString(" (scene ",tt::FNb(-objectID),")");
	}


	bool inSelection=false;
	float dummyCol[3]={0.0f,0.0f,0.0f};
	float* bgCol=dummyCol;
	if (!forDragAndDrop)
	{
		bool hasAColor=false;
		if (dropID!=-9999)
		{
			if ((transparentForTreeObjects!=-2)&&(dropID==objectID))
			{
				bgCol=ogl::HIERARCHY_DROP_LOCATION_COLOR;
				hasAColor=true;
			}
		}
		else
		{
			if (App::ct->objCont->isObjectInSelection(objectID)&&textInside&&(!dontDisplay))
			{
				if (App::ct->objCont->getLastSelection()==it)
					bgCol=ogl::HIERARCHY_AND_BROWSER_LAST_SELECTION_COLOR;
				else
				{
					if (App::ct->collections->isObjectInMarkedGroup(objectID))
					{
						if (bright)
							bgCol=ogl::HIERARCHY_COLLECTION_SELECTION_COLOR_BRIGHT;
						else
							bgCol=ogl::HIERARCHY_COLLECTION_SELECTION_COLOR_DARK;
					}
					else
					{
						if (bright)
							bgCol=ogl::HIERARCHY_NOT_LAST_SELECTION_COLOR_BRIGHT;
						else
							bgCol=ogl::HIERARCHY_NOT_LAST_SELECTION_COLOR_DARK;
					}
				}
				inSelection=true;
				hasAColor=true;
			}
		}
		if (textInside&&(!dontDisplay)&&(!hasAColor))
		{
			if (it==NULL)
			{ // world
				if (worldClick==objectID)
					bgCol=ogl::HIERARCHY_WORLD_CLICK_COLOR;
				else
				{
					if (isLocalWorld())
					{
						if (!bright)
							bgCol=ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_DARK;
						else
							bgCol=ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_BRIGHT;
					}
					else
						bgCol=ogl::HIERARCHY_UNACTIVE_WORLD_COLOR;
				}
			}
			else
			{
				int colIndex=it->getHierarchyColorIndex();
				if (colIndex==-1)
				{
					if (!bright)
						bgCol=ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_DARK;
					else
						bgCol=ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_BRIGHT;
				}
				else
				{
					if (colIndex==0)
					{
						if (!bright)
							bgCol=ogl::HIERARCHY_NO_SELECTION_RED_DARK;
						else
							bgCol=ogl::HIERARCHY_NO_SELECTION_RED_BRIGHT;
					}
					if (colIndex==1)
					{
						if (!bright)
							bgCol=ogl::HIERARCHY_NO_SELECTION_GREEN_DARK;
						else
							bgCol=ogl::HIERARCHY_NO_SELECTION_GREEN_BRIGHT;
					}
					if (colIndex==2)
					{
						if (!bright)
							bgCol=ogl::HIERARCHY_NO_SELECTION_BLUE_DARK;
						else
							bgCol=ogl::HIERARCHY_NO_SELECTION_BLUE_BRIGHT;
					}
				}
			}
		}

		if (!forDragAndDrop)
		{
			ogl::setColor(bgCol,AMBIENT_MODE);
			glBegin(GL_QUADS);
			glVertex3i(0,textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET-HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
			glVertex3i(renderingSize[0],textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET-HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
			glVertex3i(renderingSize[0],textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET+HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
			glVertex3i(0,textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET+HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
			glEnd();
		}


		if (textInside&&(!dontDisplay))
		{
			hier->objectPosition.push_back(textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc);
			hier->objectPosition.push_back(objectID);
		}
	}
	else
	{ // for drag and drop only:
		inSelection=App::ct->objCont->isObjectInSelection(objectID);
	}

	int off=2;
	if (it!=NULL)
	{
		if (it->childList.size()!=0)
		{
			if (it->getObjectProperty()&sim_objectproperty_collapsed)
			{
				int picture=PLUS_SIGN_TREE_PICTURE;
				if (dontDisplay||(!textInside))
					picture=NO_TREE_PICTURE;
				_drawIcon_3DObject(hier,textPos[0]+off-5,textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,it,picture,true,transparencyFactor,forDragAndDrop);
			}
			else
			{
				int picture=MINUS_SIGN_TREE_PICTURE;
				if (dontDisplay||(!textInside))
					picture=NO_TREE_PICTURE;
				_drawIcon_3DObject(hier,textPos[0]+off-5,textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,it,picture,true,transparencyFactor,forDragAndDrop);
			}
		}

		off=off+_drawIcon_3DObject(hier,textPos[0]+off+8,textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,it,-1,!dontDisplay,transparencyFactor,forDragAndDrop);
	}

	if (objectID<0)
	{ // For the world!
		off=off+_drawIcon_3DObject(hier,textPos[0]+off-15,textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,it,objectID,!dontDisplay,transparencyFactor,forDragAndDrop);
		off-=24; // tweaking
	}

	int textEndPosition=textPos[0]+off+ogl::getTextLengthInPixels(theText);
	if ( (labelEditObjectID==objectID)&&(objectID>=0) )
		textEndPosition=textPos[0]+off+ogl::getTextLengthInPixels(hier->editionText)+16;

	if (!forDragAndDrop)
	{
		if (objectID>=0)
		{ // world text can never be edited anyways
			hier->textPosition.push_back(textPos[0]);
			hier->textPosition.push_back(textPos[1]);
			hier->textPosition.push_back(textPos[0]+off);
			hier->textPosition.push_back(textEndPosition);
			hier->textPosition.push_back(textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc);
			hier->textPosition.push_back(objectID);
		}
	}

	if (textInside&&(!dontDisplay))
	{
		float* tc;
		if (inSelection)
			tc=ogl::HIERARCHY_AND_BROWSER_TEXT_COLOR_VISIBLE;
		else
		{
			if ( (it!=NULL)&&( ((it->layer&App::ct->mainSettings->activeLayers)==0)||it->isObjectPartOfInvisibleModel() ) )
				tc=ogl::HIERARCHY_AND_BROWSER_TEXT_COLOR_INVISIBLE;
			else
				tc=ogl::HIERARCHY_AND_BROWSER_TEXT_COLOR_VISIBLE;
		}
		if (transparentForTreeObjects==-2)
			ogl::setColor((tc[0]+7.0f*bgCol[0])*0.125f,(tc[1]+7.0f*bgCol[1])*0.125f,(tc[2]+7.0f*bgCol[2])*0.125f,AMBIENT_MODE);
		else
		{
			if ((it==NULL)&&(!isLocalWorld()))
				ogl::setColor((tc[0]+3.0f*bgCol[0])*0.25f,(tc[1]+3.0f*bgCol[1])*0.25f,(tc[2]+3.0f*bgCol[2])*0.25f,AMBIENT_MODE); // other worlds
			else
				ogl::setColor(tc,AMBIENT_MODE);
		}

		if ( (labelEditObjectID!=objectID)||(objectID==-1) )
			ogl::drawText(textPos[0]+off,textPos[1],0,theText,false);
		else
			hier->drawEditionLabel(textPos[0]+off,textPos[1]);
		// Following is for vertical lines that originates from one of this object's parent
		ogl::setColor(ogl::HIERARCHY_AND_BROWSER_LINE_COLOR,AMBIENT_MODE);
		if (!forDragAndDrop)
		{
			for (int i=0;i<int(vertLines->size());i++)
			{
				if (vertLines->at(i)<indentNb-1)
				{
					glBegin(GL_LINES);
					glVertex3i(textPos[0]+3+horizontalShift/2-horizontalShift*(indentNb-vertLines->at(i)),textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET+HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
					glVertex3i(textPos[0]+3+horizontalShift/2-horizontalShift*(indentNb-vertLines->at(i)),textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET-HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
					glEnd();
				}
			}
		}
	}

	if (isOtherWorld)
	{
		textPos[1]=textPos[1]-HIERARCHY_INTER_LINE_SPACE*App::sc;
		return;
	}

	int lineLastPos=textEndPosition;

	// Show the script and script parameter icons:
	int tPosX=lineLastPos;
	int tPosY=textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc;
	int localOffset=16;
	if (it!=NULL)
	{
		CLuaScriptObject* script=App::ct->luaScriptContainer->getScriptFromObjectAttachedTo(it->getID());
		if (script!=NULL)
		{
			if (!dontDisplay)
			{
				if (script->getThreadedExecution())
					App::ct->globalTextureCont->startTextureDisplay(SCRIPT_THREADED_PICTURE);
				else
					App::ct->globalTextureCont->startTextureDisplay(SCRIPT_PICTURE);
				_drawTexturedIcon(tPosX+localOffset,tPosY,HIERARCHY_ICON_WIDTH*App::sc,HIERARCHY_ICON_HEIGHT*App::sc,transparencyFactor);
			}
			if (!forDragAndDrop)
			{
				hier->scriptIconPosition.push_back(tPosX+localOffset);
				hier->scriptIconPosition.push_back(tPosY);
				hier->scriptIconPosition.push_back(script->getScriptID());
			}
			localOffset+=HIERARCHY_ICON_WIDTH*App::sc;
			if (!dontDisplay)
			{
				
				if (script->getScriptParametersObject()->parameterValues.size()!=0)
					App::ct->globalTextureCont->startTextureDisplay(SCRIPT_PARAMETERS_ACTIVE_PICTURE);
				else
					App::ct->globalTextureCont->startTextureDisplay(SCRIPT_PARAMETERS_PICTURE);
				_drawTexturedIcon(tPosX+localOffset,tPosY,HIERARCHY_ICON_WIDTH*App::sc,HIERARCHY_ICON_HEIGHT*App::sc,transparencyFactor);
			}
			if (!forDragAndDrop)
			{
				hier->scriptParametersIconPosition.push_back(tPosX+localOffset);
				hier->scriptParametersIconPosition.push_back(tPosY);
				hier->scriptParametersIconPosition.push_back(script->getScriptID());
			}
			localOffset+=(HIERARCHY_ICON_WIDTH+HIERARCHY_INTER_ICON_SPACING)*App::sc;
		}
	}
	else
	{ // This is for the main script (pseudo object "world"):
		CLuaScriptObject* script=App::ct->luaScriptContainer->getMainScript();
		if (script!=NULL)
		{
			if (!dontDisplay)
			{
				if (script->isDefaultMainScript())
					App::ct->globalTextureCont->startTextureDisplay(MAIN_SCRIPT_PICTURE);
				else
					App::ct->globalTextureCont->startTextureDisplay(MAIN_SCRIPT_CUSTOMIZED_PICTURE);
				_drawTexturedIcon(tPosX+localOffset,tPosY,HIERARCHY_ICON_WIDTH*App::sc,HIERARCHY_ICON_HEIGHT*App::sc,transparencyFactor);
			}
			if (!forDragAndDrop)
			{
				hier->scriptIconPosition.push_back(tPosX+localOffset);
				hier->scriptIconPosition.push_back(tPosY);
				hier->scriptIconPosition.push_back(script->getScriptID());
			}
			localOffset+=HIERARCHY_ICON_WIDTH*App::sc;
			if (!dontDisplay)
			{
				
				if (script->getScriptParametersObject()->parameterValues.size()!=0)
					App::ct->globalTextureCont->startTextureDisplay(SCRIPT_PARAMETERS_ACTIVE_PICTURE);
				else
					App::ct->globalTextureCont->startTextureDisplay(SCRIPT_PARAMETERS_PICTURE);
				_drawTexturedIcon(tPosX+localOffset,tPosY,HIERARCHY_ICON_WIDTH*App::sc,HIERARCHY_ICON_HEIGHT*App::sc,transparencyFactor);
			}
			if (!forDragAndDrop)
			{
				hier->scriptParametersIconPosition.push_back(tPosX+localOffset);
				hier->scriptParametersIconPosition.push_back(tPosY);
				hier->scriptParametersIconPosition.push_back(script->getScriptID());
			}
			localOffset+=(HIERARCHY_ICON_WIDTH+HIERARCHY_INTER_ICON_SPACING)*App::sc;
		}
	}
	lineLastPos+=localOffset;

	if (lineLastPos>maxRenderedPos[0])
		maxRenderedPos[0]=lineLastPos;


	// Show the dynamic icons:
	if ((it!=NULL)&&(!forDragAndDrop))
	{ 
		int simIconCode=it->getDynamicSimulationIconCode();
		if (simIconCode!=DYNAMIC_SIMULATION_ICON_NONE)
		{
			int ic;
			if (simIconCode==DYNAMIC_SIMULATION_ICON_OBJECT_IS_DYNAMICALLY_SIMULATED)
				ic=DYNAMICS_PICTURE;
			if (simIconCode==DYNAMIC_SIMULATION_ICON_OBJECT_IS_NOT_DYNAMICALLY_ENABLED)
				ic=WARNING_PICTURE;
			lineLastPos+=8+8+_drawIcon_3DObject(hier,lineLastPos+8+8+8,textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,it,ic,!dontDisplay,transparencyFactor,forDragAndDrop);
			if (textEndPosition+lineLastPos>maxRenderedPos[0])
				maxRenderedPos[0]=textEndPosition+lineLastPos;
		}
	}

	if (!forDragAndDrop)
	{
		hier->lineLastPosition.push_back(lineLastPos);
		hier->lineLastPosition.push_back(textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc);
		if (it!=NULL)
			hier->lineLastPosition.push_back(it->getID());
		else
			hier->lineLastPosition.push_back(-1);
	}



	if ((!dontDisplay)&&(!forDragAndDrop)&&(it!=NULL))
	{ // Following for the line element(s) just left of scene object icons:
		glBegin(GL_LINES);
		if (it->getModelBase())
		{
			glVertex3i(textPos[0]+16,textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,0);
			glVertex3i(textPos[0]+20,textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,0);
			if ((it->childList.size()!=0)&&((it->getObjectProperty()&sim_objectproperty_collapsed)==0))
			{
				glVertex3i(textPos[0]+10,textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET-CONST_VAL_6)*App::sc,0);
				glVertex3i(textPos[0]+10,textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET-HIERARCHY_INTER_LINE_SPACE)*App::sc,0);
			}
		}
		else
		{
			glVertex3i(textPos[0]+4,textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,0);
			glVertex3i(textPos[0]+20,textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,0);
			if ((it->childList.size()!=0)&&((it->getObjectProperty()&sim_objectproperty_collapsed)==0))
			{
				glVertex3i(textPos[0]+10,textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,0);
				glVertex3i(textPos[0]+10,textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET-HIERARCHY_INTER_LINE_SPACE)*App::sc,0);
			}
		}
		glEnd();
	}

	textPos[1]=textPos[1]-HIERARCHY_INTER_LINE_SPACE*App::sc;


	if ( ( (it!=NULL)&&((it->getObjectProperty()&sim_objectproperty_collapsed)==0) )||(objectID<0) )
	{
		int xPosCopy=textPos[0];
		textPos[0]=textPos[0]+horizontalShift;
		int indentCopy=indentNb+1;

		ogl::setColor(ogl::HIERARCHY_AND_BROWSER_LINE_COLOR,AMBIENT_MODE);


		std::vector<CHierarchyElement*> el;
		std::vector<int> elNb;
		for (int i=0;i<int(children.size());i++)
		{ // This is needed to order the elements from least sub-elements to most sub-elements
			int els=children[i]->getNumberOfElements();
			int j;
			for (j=0;j<int(elNb.size());j++)
			{
				if (els<elNb[j])
					break;
			}
			elNb.insert(elNb.begin()+j,els);
			el.insert(el.begin()+j,children[i]);
		}	
		for (int i=0;i<int(el.size());i++)
		{
			if (i!=int(el.size())-1)
			{ // Vertical line going through (a T turned counter-clockwise)
				vertLines->push_back(indentNb);
				int txtYTmp=textPos[1];
				el[i]->renderElement_3DObject(hier,labelEditObjectID,bright,dontDisplay,
											renderingSize,textPos,indentCopy,
											vertLines,minRenderedPos,maxRenderedPos,forDragAndDrop,transparentForTreeObjects,dropID);
				vertLines->erase(vertLines->end()-1);
				if ((!dontDisplay)&&(!forDragAndDrop))
				{
					C3DObject* childIt=App::ct->objCont->getObject(el[i]->getLinkedObjectID());
					if (childIt->childList.size()==0)
					{
						glBegin(GL_LINES);
						glVertex3i(xPosCopy+10,txtYTmp+(HIERARCHY_TEXT_CENTER_OFFSET+HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
						glVertex3i(xPosCopy+10,txtYTmp+(HIERARCHY_TEXT_CENTER_OFFSET-HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
						glVertex3i(xPosCopy+10,txtYTmp+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,0);
						glVertex3i(xPosCopy+17,txtYTmp+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,0);
						glEnd();
					}
				}
			}
			else
			{ // Vertical line stopping in the middle (L)
				int txtYTmp=textPos[1];
				el[i]->renderElement_3DObject(hier,labelEditObjectID,bright,dontDisplay,
											renderingSize,textPos,indentCopy,
											vertLines,minRenderedPos,maxRenderedPos,forDragAndDrop,transparentForTreeObjects,dropID);
				if ((!dontDisplay)&&(!forDragAndDrop))
				{
					C3DObject* childIt=App::ct->objCont->getObject(el[i]->getLinkedObjectID());
					if (childIt->childList.size()==0)
					{
						glBegin(GL_LINES);
						glVertex3i(xPosCopy+10,txtYTmp+(HIERARCHY_TEXT_CENTER_OFFSET+HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
						glVertex3i(xPosCopy+10,txtYTmp+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,0);
						glVertex3i(xPosCopy+10,txtYTmp+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,0);
						glVertex3i(xPosCopy+17,txtYTmp+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,0);
						glEnd();
					}
				}
			}
		}
		textPos[0]=xPosCopy;
	}
}



int CHierarchyElement::_drawIcon_3DObject(CHierarchy* hier,int tPosX,int tPosY,C3DObject* it,int pictureID,bool drawIt,float transparencyFactor,bool forDragAndDrop)
{ // pictureID is -1 by default. It is then ignored. The size of the icon is 16x16
	int retVal=0;
	if (pictureID!=NO_TREE_PICTURE)
	{
		if (pictureID>=0)
		{
			if (drawIt)
			{
				App::ct->globalTextureCont->startTextureDisplay(pictureID);
				if ((it!=NULL)&&(!forDragAndDrop))
				{
					if (((pictureID==PLUS_SIGN_TREE_PICTURE)||(pictureID==MINUS_SIGN_TREE_PICTURE)))
					{
						hier->inflateIconPosition.push_back(tPosX);
						hier->inflateIconPosition.push_back(tPosY);
						hier->inflateIconPosition.push_back(it->getID());
					}
					else
					{
						if (it->getDynamicSimulationIconCode()!=DYNAMIC_SIMULATION_ICON_NONE)
						{
							hier->simulationIconPosition.push_back(tPosX);
							hier->simulationIconPosition.push_back(tPosY);
							hier->simulationIconPosition.push_back(it->getID());
						}
					}
				}
				_drawTexturedIcon(tPosX,tPosY,HIERARCHY_ICON_WIDTH*App::sc,HIERARCHY_ICON_HEIGHT*App::sc,transparencyFactor);
			}
			retVal+=(HIERARCHY_ICON_WIDTH+HIERARCHY_INTER_ICON_SPACING)*App::sc;
		}
		else
		{
			int type=-1; // World
			if (it!=NULL)
				type=it->getObjectType();
			int objectOrWorldIconID=-1;
			if (drawIt)
			{
				if (type==-1)
				{
					objectOrWorldIconID=WORLD_TREE_PICTURE;
					if (!isLocalWorld())
						transparencyFactor=0.6f;
				}
				if (type==sim_object_shape_type)
				{
					if (((CShape*)it)->isGrouping())
					{
						if (((CShape*)it)->geomData->geomInfo->isPure())
							objectOrWorldIconID=PURE_MULTISHAPE_TREE_PICTURE;
						else if (((CShape*)it)->geomData->geomInfo->isConvex())
							objectOrWorldIconID=CONVEX_MULTISHAPE_TREE_PICTURE;
						else
							objectOrWorldIconID=MULTI_SHAPE_TREE_PICTURE;
					}
					else
					{
						if (((CShape*)it)->geomData->geomInfo->isPure())
						{
							if (((CGeometric*)((CShape*)it)->geomData->geomInfo)->getPurePrimitiveType()==sim_pure_primitive_heightfield)
								objectOrWorldIconID=HEIGHTFIELD_SHAPE_TREE_PICTURE;
							else
								objectOrWorldIconID=PURE_SHAPE_TREE_PICTURE;
						}
						else if (((CShape*)it)->geomData->geomInfo->isConvex())
							objectOrWorldIconID=CONVEX_SHAPE_TREE_PICTURE;
						else
							objectOrWorldIconID=SIMPLE_SHAPE_TREE_PICTURE;
					}
				}
				if (type==sim_object_joint_type)
				{
					if (((CJoint*)it)->getJointType()==sim_joint_revolute_subtype)
						objectOrWorldIconID=REVOLUTE_JOINT_TREE_PICTURE;
					if (((CJoint*)it)->getJointType()==sim_joint_prismatic_subtype)
						objectOrWorldIconID=PRISMATIC_JOINT_TREE_PICTURE;
					if (((CJoint*)it)->getJointType()==sim_joint_spherical_subtype)
						objectOrWorldIconID=SPHERICAL_JOINT_TREE_PICTURE;
				}
				if (type==sim_object_camera_type)
					objectOrWorldIconID=CAMERA_TREE_PICTURE;
				if (type==sim_object_mirror_type)
				{
					if (((CMirror*)it)->getIsMirror())
						objectOrWorldIconID=MIRROR_TREE_PICTURE;
					else
						objectOrWorldIconID=CLIPPLANE_TREE_PICTURE;
				}
				if (type==sim_object_light_type)
				{
					int subType=((CLight*)it)->getLightType();
					if (subType==sim_light_omnidirectional_subtype)
					{
						if (((CLight*)it)->getLightActive())
							objectOrWorldIconID=OMNI_LIGHT_ON_TREE_PICTURE;
						else
							objectOrWorldIconID=OMNI_LIGHT_OFF_TREE_PICTURE;
					}
					if (subType==sim_light_spot_subtype)
					{
						if (((CLight*)it)->getLightActive())
							objectOrWorldIconID=SPOT_LIGHT_ON_TREE_PICTURE;
						else
							objectOrWorldIconID=SPOT_LIGHT_OFF_TREE_PICTURE;
					}
					if (subType==sim_light_directional_subtype)
					{
						if (((CLight*)it)->getLightActive())
							objectOrWorldIconID=DIR_LIGHT_ON_TREE_PICTURE;
						else
							objectOrWorldIconID=DIR_LIGHT_OFF_TREE_PICTURE;
					}
				}
				if (type==sim_object_graph_type)
					objectOrWorldIconID=GRAPH_TREE_PICTURE;
				if (type==sim_object_dummy_type)
					objectOrWorldIconID=DUMMY_TREE_PICTURE;
				if (type==sim_object_proximitysensor_type)
					objectOrWorldIconID=PROXIMITYSENSOR_TREE_PICTURE;
				if (type==sim_object_visionsensor_type)
					objectOrWorldIconID=VISIONSENSOR_TREE_PICTURE;
				if (type==sim_object_path_type)
					objectOrWorldIconID=PATH_TREE_PICTURE;
				if (type==sim_object_mill_type)
					objectOrWorldIconID=MILL_TREE_PICTURE;
				if (type==sim_object_forcesensor_type)
					objectOrWorldIconID=FORCE_SENSOR_TREE_PICTURE;
			}
			if (drawIt)
			{
				if ((it!=NULL)&&it->getModelBase())
				{ // We have to draw a model icon before the object icon:
					App::ct->globalTextureCont->startTextureDisplay(MODEL_TREE_PICTURE);
					_drawTexturedIcon(tPosX+retVal,tPosY,HIERARCHY_ICON_WIDTH*App::sc,HIERARCHY_ICON_HEIGHT*App::sc,transparencyFactor);
					if (!forDragAndDrop)
					{
						hier->modelIconPosition.push_back(tPosX+retVal);
						hier->modelIconPosition.push_back(tPosY);
						hier->modelIconPosition.push_back(it->getID());
					}
				}
				retVal+=(HIERARCHY_ICON_WIDTH+HIERARCHY_INTER_ICON_SPACING)*App::sc;

				App::ct->globalTextureCont->startTextureDisplay(objectOrWorldIconID);
				_drawTexturedIcon(tPosX+retVal,tPosY,HIERARCHY_ICON_WIDTH*App::sc,HIERARCHY_ICON_HEIGHT*App::sc,transparencyFactor);
				if (!forDragAndDrop)
				{
					if ( (it!=NULL)||((pictureID<0) ))
					{
						hier->objectIconPosition.push_back(tPosX+retVal);
						hier->objectIconPosition.push_back(tPosY);
						if (it==NULL)
							hier->objectIconPosition.push_back(objectID);
						else
							hier->objectIconPosition.push_back(it->getID());
					}
				}
				retVal+=(HIERARCHY_ICON_WIDTH+HIERARCHY_INTER_ICON_SPACING)*App::sc;
			}
			else
			{
				if ((it!=NULL)&&it->getModelBase())
				{ // We have to consider a model icon before the object icon:
					if (!forDragAndDrop)
					{
						hier->modelIconPosition.push_back(tPosX+retVal);
						hier->modelIconPosition.push_back(tPosY);
						hier->modelIconPosition.push_back(it->getID());
					}
					retVal+=(HIERARCHY_ICON_WIDTH+HIERARCHY_INTER_ICON_SPACING)*App::sc;
				}
				if (!forDragAndDrop)
				{
					hier->objectIconPosition.push_back(tPosX+retVal);
					hier->objectIconPosition.push_back(tPosY);
					if (it==NULL)
						hier->objectIconPosition.push_back(-1); // World icon
					else
						hier->objectIconPosition.push_back(it->getID());
				}
				retVal+=(HIERARCHY_ICON_WIDTH+HIERARCHY_INTER_ICON_SPACING)*App::sc;
			}

		}
	}
	else
		retVal+=(HIERARCHY_ICON_WIDTH+HIERARCHY_INTER_ICON_SPACING)*App::sc;
	return(retVal);
}

void CHierarchyElement::renderElement_editModeList(CHierarchy* hier,int labelEditObjectID,bool& bright,bool dontDisplay,
								int renderingSize[2],int textPos[2],int indentNb,int minRenderedPos[2],int maxRenderedPos[2],
								int selectedState,int editModeType)
{
	bright=!bright;
	if (textPos[0]<minRenderedPos[0])
		minRenderedPos[0]=textPos[0];
	if (textPos[1]<minRenderedPos[1])
		minRenderedPos[1]=textPos[1];
	if (textPos[1]>maxRenderedPos[1])
		maxRenderedPos[1]=textPos[1];

	if (textPos[1]<-HIERARCHY_INTER_LINE_SPACE*App::sc)
		dontDisplay=true; // We don't display what is outside of the view!
	bool textInside=(textPos[1]<renderingSize[1]+HIERARCHY_INTER_LINE_SPACE*App::sc);
	std::string theText;
	if (editModeType==VERTEX_EDIT_MODE)
	{
		theText="Vertex ";
		theText+=boost::lexical_cast<std::string>(objectID+1);
	}
	if (editModeType==TRIANGLE_EDIT_MODE)
	{
		theText="Triangle ";
		theText+=boost::lexical_cast<std::string>(objectID+1);
	}
	if (editModeType==EDGE_EDIT_MODE)
	{
		theText="Edge ";
		theText+=boost::lexical_cast<std::string>(objectID+1);
	}
	if (editModeType==PATH_EDIT_MODE)
	{
		theText="Path point ";
		theText+=boost::lexical_cast<std::string>(objectID+1);
	}
	if (editModeType==BUTTON_EDIT_MODE)
	{
		if (objectID==App::ct->buttonBlockContainer->getBlockInEdition())
			selectedState=2;
		CButtonBlock* blk=App::ct->buttonBlockContainer->getBlockWithID(objectID);
		if (blk!=NULL)
			theText=blk->getBlockName();
		else
			theText="ERROR";
	}
	if ((selectedState!=0)&&textInside&&(!dontDisplay))
	{
		if (selectedState==2)
			ogl::setColor(ogl::HIERARCHY_AND_BROWSER_LAST_SELECTION_COLOR,AMBIENT_MODE);
		else
		{
			if (bright)
				ogl::setColor(ogl::HIERARCHY_NOT_LAST_SELECTION_COLOR_BRIGHT,AMBIENT_MODE);
			else
				ogl::setColor(ogl::HIERARCHY_NOT_LAST_SELECTION_COLOR_DARK,AMBIENT_MODE);
		}
		glBegin(GL_QUADS);
		glVertex3i(0,textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET-HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
		glVertex3i(renderingSize[0],textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET-HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
		glVertex3i(renderingSize[0],textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET+HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
		glVertex3i(0,textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET+HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
		glEnd();
	}
	else if (textInside&&(!dontDisplay))
	{
		if (!bright)
			ogl::setColor(ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_DARK,AMBIENT_MODE);
		else
			ogl::setColor(ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_BRIGHT,AMBIENT_MODE);
		glBegin(GL_QUADS);
		glVertex3i(0,textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET-HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
		glVertex3i(renderingSize[0],textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET-HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
		glVertex3i(renderingSize[0],textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET+HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
		glVertex3i(0,textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET+HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
		glEnd();
	}

	if (textInside&&(!dontDisplay))
	{
		hier->objectPosition.push_back(textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc);
		hier->objectPosition.push_back(objectID);
	}
	int off=2;

	if (editModeType==VERTEX_EDIT_MODE)
		off=off+_drawIcon_editModeList(hier,textPos[0]+off+8,textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,VERTEX_PICTURE,!dontDisplay);
	if (editModeType==TRIANGLE_EDIT_MODE)
		off=off+_drawIcon_editModeList(hier,textPos[0]+off+8,textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,TRIANGLE_PICTURE,!dontDisplay);
	if (editModeType==EDGE_EDIT_MODE)
		off=off+_drawIcon_editModeList(hier,textPos[0]+off+8,textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,EDGE_PICTURE,!dontDisplay);
	if (editModeType==PATH_EDIT_MODE)
		off=off+_drawIcon_editModeList(hier,textPos[0]+off+8,textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,PATH_POINT_PICTURE,!dontDisplay);
	if (editModeType==BUTTON_EDIT_MODE)
		off=off+_drawIcon_editModeList(hier,textPos[0]+off+8,textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc,BUTTON_BLOCK_PICTURE,!dontDisplay);

		
	int textEndPosition=textPos[0]+off+ogl::getTextLengthInPixels(theText);
	hier->textPosition.push_back(textPos[0]);
	hier->textPosition.push_back(textPos[1]);
	hier->textPosition.push_back(textPos[0]+off);
	hier->textPosition.push_back(textEndPosition);
	hier->textPosition.push_back(textPos[1]+HIERARCHY_TEXT_CENTER_OFFSET*App::sc);
	hier->textPosition.push_back(objectID);

	if (textEndPosition>maxRenderedPos[0])
		maxRenderedPos[0]=textEndPosition;

	if (textInside&&(!dontDisplay))
	{
		ogl::setColor(0.0f,0.0f,0.0f,AMBIENT_MODE);
		if (labelEditObjectID!=objectID)
			ogl::drawText(textPos[0]+off,textPos[1],0,theText);
		else
			hier->drawEditionLabel(textPos[0]+off,textPos[1]);
		ogl::setColor(0.0f,0.0f,0.0f,AMBIENT_MODE);
	}
	textPos[1]=textPos[1]-HIERARCHY_INTER_LINE_SPACE*App::sc;
}

int CHierarchyElement::_drawIcon_editModeList(CHierarchy* hier,int tPosX,int tPosY,int pictureID,bool drawIt)
{ // The size of the icon is 16x16
	const int sizeX=16;
	const int sizeY=16;
	const int interIconSpacing=4;
	int retVal=0;
	if (pictureID!=NO_TREE_PICTURE)
	{
		if (pictureID!=-1)
		{
			if (drawIt)
			{
				App::ct->globalTextureCont->startTextureDisplay(pictureID);
				_drawTexturedIcon(tPosX,tPosY,sizeX,sizeY,0.0f);
			}
			hier->objectIconPosition.push_back(tPosX);
			hier->objectIconPosition.push_back(tPosY);
			hier->objectIconPosition.push_back(objectID);
		}
	}
	retVal+=sizeX+interIconSpacing;
	return(retVal);
}

void CHierarchyElement::_drawTexturedIcon(int tPosX,int tPosY,int sizeX,int sizeY,float transparencyFactor)
{
	ogl::setBlending(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // We turn blending on!

	if (transparencyFactor>0.01f)
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f-transparencyFactor);
	else
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // opaque

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	glTexEnvf (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f);
	glVertex3i(tPosX-sizeX/2,tPosY-sizeY/2,0);
	glTexCoord2f(0.625f,0.0f); // icons themselves are 20x16, but since some gfx cards don't support that non-power of 2 res, we save them as 32x16!
	glVertex3i(tPosX+sizeX/2,tPosY-sizeY/2,0);
	glTexCoord2f(0.625f,1.0f); // icons themselves are 20x16, but since some gfx cards don't support that non-power of 2 res, we save them as 32x16!
	glVertex3i(tPosX+sizeX/2,tPosY+sizeY/2,0);
	glTexCoord2f(0.0f,1.0f);
	glVertex3i(tPosX-sizeX/2,tPosY+sizeY/2,0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	ogl::setBlending(false);
	glDisable(GL_ALPHA_TEST);
	App::ct->globalTextureCont->endTextureDisplay();
}

bool CHierarchyElement::renderDummyElement(bool& bright,int renderingSize[2],int textPos[2])
{ // Just renders the background and returns true when inside the visible zone
	bright=!bright;
	if (textPos[1]<-HIERARCHY_INTER_LINE_SPACE*App::sc)
		return(false);
	if (textPos[1]<renderingSize[1]+HIERARCHY_INTER_LINE_SPACE*App::sc)
	{
		if (!bright)
			ogl::setColor(ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_DARK,AMBIENT_MODE);
		else
			ogl::setColor(ogl::HIERARCHY_AND_BROWSER_NO_SELECTION_COLOR_BRIGHT,AMBIENT_MODE);
		glBegin(GL_QUADS);
		glVertex3i(0,textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET-HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
		glVertex3i(renderingSize[0],textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET-HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
		glVertex3i(renderingSize[0],textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET+HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
		glVertex3i(0,textPos[1]+(HIERARCHY_TEXT_CENTER_OFFSET+HIERARCHY_HALF_INTER_LINE_SPACE)*App::sc,0);
		glEnd();
		textPos[1]=textPos[1]-HIERARCHY_INTER_LINE_SPACE*App::sc;
		return(true);
	}
	return(false);
}


int CHierarchyElement::getNumberOfElements()
{
	if (objectID<0)
	{
		if (!isLocalWorld())
			return(999999); // for the other instances!
	}
	return(numberOfElements);
}

bool CHierarchyElement::isLocalWorld()
{
	if (objectID<0)
		return(-App::ct->getCurrentInstanceIndex()-1==objectID);
	return(false);
}

int CHierarchyElement::computeNumberOfElements()
{
	numberOfElements=1;
	for (int i=0;i<int(children.size());i++)
	{
		children[i]->computeNumberOfElements();
		numberOfElements=numberOfElements+children[i]->getNumberOfElements();
	}
	return(numberOfElements);
}


