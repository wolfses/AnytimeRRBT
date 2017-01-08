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
#include "LuaObjectGroup.h"
#include "LuaTableObject.h"

CLuaObjectGroup::CLuaObjectGroup()
{

}

CLuaObjectGroup::CLuaObjectGroup(CLuaObject* lObject)
{
	addObject(lObject);
}

CLuaObjectGroup::CLuaObjectGroup(CLuaObject* lObject1,CLuaObject* lObject2)
{
	addObject(lObject1);
	addObject(lObject2);
}

CLuaObjectGroup::CLuaObjectGroup(CLuaObject* lObject1,CLuaObject* lObject2,CLuaObject* lObject3)
{
	addObject(lObject1);
	addObject(lObject2);
	addObject(lObject3);
}

CLuaObjectGroup::~CLuaObjectGroup()
{
	removeAllObjects();
}

void CLuaObjectGroup::addObject(CLuaObject* lObject)
{
	objects.push_back(lObject);
}
void CLuaObjectGroup::removeAllObjects()
{
	for (int i=0;i<int(objects.size());i++)
		delete objects[i];
	objects.clear();
}

int CLuaObjectGroup::getObjectNumber()
{
	return(objects.size());
}

bool CLuaObjectGroup::areAllObjectsNumbers(int range)
{ // range is -1 by default
	int top=objects.size();
	if ( (range>0)&&(range<=int(objects.size())) )
		top=range;
	for (int i=0;i<top;i++)
	{
		if (!objects[i]->isNumber())
			return(false);
	}
	return(true);
}

CLuaObject* CLuaObjectGroup::builObjectFromStackAtPosition(lua_State* L,int pos)
{ // 1, 2, etc., 3.4, 5.6, etc. are recognized as strings! NONONO!!
	// ABOVE THING CHANGED ON 2009/07/31
	CLuaObject* obj=NULL;
	if (lua_istable(L,pos))
	{ // We have a table here:
		CLuaTableObject* it=new CLuaTableObject();
		int tableSize=VREP_LUA_OBJLEN(L,pos);
		for (int i=0;i<tableSize;i++)
		{
			lua_rawgeti(L,pos,i+1); // we push an element onto the stack
			CLuaObject* child=builObjectFromStackAtPosition(L,lua_gettop(L));
			lua_settop(L,-2); // We remove the last element
			it->addElement(child);
		}
		obj=new CLuaObject(it);
	}
	else if (lua_isnumber(L,pos))
	{ // We have a number here:
		obj=new CLuaObject(lua_tonumber(L,pos));
	}
	else if (lua_isboolean(L,pos))
	{ // We have a boolean here:
		obj=new CLuaObject(lua_toboolean(L,pos)!=0);
	}
	else if (lua_isstring(L,pos))
	{ // We have a string here:
		// On 2011/01/23 considered the fact that lua strings can have embedded zeros!
		std::string a;
		int l;
		const char* c=lua_tolstring(L,pos,(size_t*)&l);
		a.assign(c,l);
		obj=new CLuaObject(a);
	}
	if (obj==NULL)
	{ // All other values are considered as nil:
		obj=new CLuaObject();
	}
	return(obj);
}

void CLuaObjectGroup::buildObjectsFromStack(lua_State* L,int stackPositionStart)
{
	removeAllObjects();
	int numberOfArgs=lua_gettop(L);
	for (int i=stackPositionStart;i<=numberOfArgs;i++)
		objects.push_back(builObjectFromStackAtPosition(L,i));
}

void CLuaObjectGroup::pushObjectsOnStack(lua_State* L)
{
	for (int i=0;i<int(objects.size());i++)
		objects[i]->pushObjectOnStack(L);
}

CLuaObjectGroup* CLuaObjectGroup::copyYourself()
{
	CLuaObjectGroup* it=new CLuaObjectGroup();
	for (int i=0;i<int(objects.size());i++)
		it->addObject(objects[i]->copyYourself());
	return(it);
}
