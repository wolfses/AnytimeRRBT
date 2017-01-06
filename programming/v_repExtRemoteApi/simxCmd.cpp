// This file is part of the REMOTE API
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The REMOTE API is licensed under the terms of GNU GPL:
// 
// -------------------------------------------------------------------
// The REMOTE API is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// THE REMOTE API IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with the REMOTE API.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

#include "simxCmd.h"
#include "simxUtils.h"
#include "v_repLib.h"
#include "simxSocket.h"
#include "extApiCustomConst.h"
#include <stdio.h>

CSimxCmd::CSimxCmd(int commandID,WORD delayOrSplit,int dataSize,const char* dataPointer)
{
	_pureDataSize=dataSize;
	_rawCmdID=commandID&simx_cmdmask;
	_status=0;
	_processingDelayOrMaxDataSize=delayOrSplit;
	_lastTimeProcessed=0;
	_dataSizeLeftToBeSent=0;
	_executionTime=0;
	_memorizedSplitCmd=NULL;
	if ((_rawCmdID>simx_cmd4bytes_start)&&(_rawCmdID<simx_cmd8bytes_start))
	{
		for (int i=0;i<4;i++)
			_cmdData[i]=dataPointer[i];
		_pureDataSize-=4;
	}
	if ((_rawCmdID>simx_cmd8bytes_start)&&(_rawCmdID<simx_cmd1string_start))
	{
		for (int i=0;i<8;i++)
			_cmdData[i]=dataPointer[i];
		_pureDataSize-=8;
	}
	if ((_rawCmdID>simx_cmd1string_start)&&(_rawCmdID<simx_cmdreserved_start))
	{
		_cmdString=std::string(dataPointer);
		_pureDataSize-=(int)strlen(dataPointer)+1; // with terminal zero
	}
	_opMode=commandID-_rawCmdID;
	if (_opMode==simx_opmode_continuous_split)
	{
		if (_processingDelayOrMaxDataSize<50)
			_processingDelayOrMaxDataSize=50;
	}

	if (_pureDataSize>0)
	{
		_pureData=new char[_pureDataSize];
		for (int i=0;i<_pureDataSize;i++)
			_pureData[i]=dataPointer[dataSize-_pureDataSize+i];
	}
	else
		_pureData=NULL;
}

CSimxCmd::CSimxCmd()
{
	// used during copy operations
}

CSimxCmd::~CSimxCmd()
{
	delete[] _pureData;
	delete _memorizedSplitCmd;
}

int CSimxCmd::getRawCommand()
{
	return(_rawCmdID);
}

int CSimxCmd::getOperationMode()
{
	return(_opMode);
}

bool CSimxCmd::areCommandAndCommandDataSame(const CSimxCmd* otherCmd)
{
	if (otherCmd->_rawCmdID!=_rawCmdID)
		return(false);
	if ((_rawCmdID>simx_cmd4bytes_start)&&(_rawCmdID<simx_cmd8bytes_start))
	{
		for (int i=0;i<4;i++)
		{
			if (_cmdData[i]!=otherCmd->_cmdData[i])
				return(false);
		}
		return(true);
	}
	if ((_rawCmdID>simx_cmd8bytes_start)&&(_rawCmdID<simx_cmd1string_start))
	{
		for (int i=0;i<8;i++)
		{
			if (_cmdData[i]!=otherCmd->_cmdData[i])
				return(false);
		}
		return(true);
	}
	if ((_rawCmdID>simx_cmd1string_start)&&(_rawCmdID<simx_cmdreserved_start))
	{
		return(_cmdString.compare(otherCmd->_cmdString)==0);
	}
	return(true);
}

void CSimxCmd::appendYourData(std::vector<char>& dataString,bool otherSideIsBigEndian)
{
	//1. Prepare the sub-header:
	char header[SIMX_SUBHEADER_SIZE];
	((int*)(header+simx_cmdheaderoffset_cmd))[0]=littleEndianIntConversion(_rawCmdID+_opMode,otherSideIsBigEndian); // return also the opmode, we need to detect cont. cmds on the client side!

	header[simx_cmdheaderoffset_status]=_status;

	//2. Check the data to append:
	int commandByteDataSize=0;
	int commandStringDataSize=0;
	if ((_rawCmdID>simx_cmd4bytes_start)&&(_rawCmdID<simx_cmd8bytes_start))
		commandByteDataSize=4;
	if ((_rawCmdID>simx_cmd8bytes_start)&&(_rawCmdID<simx_cmd1string_start))
		commandByteDataSize=8;
	if ((_rawCmdID>simx_cmd1string_start)&&(_rawCmdID<simx_cmdreserved_start))
		commandStringDataSize=(int)_cmdString.length()+1;
	
	//3. We have the total length of this command:
	((int*)(header+simx_cmdheaderoffset_mem_size))[0]=littleEndianIntConversion(SIMX_SUBHEADER_SIZE+commandByteDataSize+commandStringDataSize+_pureDataSize,otherSideIsBigEndian);
	((int*)(header+simx_cmdheaderoffset_full_mem_size))[0]=littleEndianIntConversion(SIMX_SUBHEADER_SIZE+commandByteDataSize+commandStringDataSize+_pureDataSize,otherSideIsBigEndian);
	((WORD*)(header+simx_cmdheaderoffset_pdata_offset0))[0]=littleEndianWordConversion(commandByteDataSize+commandStringDataSize,otherSideIsBigEndian);
	((int*)(header+simx_cmdheaderoffset_pdata_offset1))[0]=littleEndianIntConversion(0,otherSideIsBigEndian);
	((int*)(header+simx_cmdheaderoffset_sim_time))[0]=littleEndianIntConversion(_executionTime,otherSideIsBigEndian);


	//4. Append the sub-header:
	for (int i=0;i<SIMX_SUBHEADER_SIZE;i++)
		dataString.push_back(header[i]);
	//5. Append the command data:
	for (int i=0;i<commandByteDataSize;i++)
		dataString.push_back(_cmdData[i]);
	if (commandStringDataSize>0)
	{
		for (int i=0;i<commandStringDataSize-1;i++)
			dataString.push_back(_cmdString[i]);
		dataString.push_back(0); // terminal zero
	}
	//6. Append the auxiliary data:
	for (int i=0;i<_pureDataSize;i++)
		dataString.push_back(_pureData[i]);
}

bool CSimxCmd::appendYourMemorizedSplitData(bool calledFromContainer,std::vector<char>& dataString,bool& removeCommand,bool otherSideIsBigEndian)
{
	if (calledFromContainer)
	{
		if ((_opMode!=simx_opmode_continuous_split)&&(_opMode!=simx_opmode_oneshot_split))
			return(false); 
		if (_memorizedSplitCmd==NULL)
			return(false); // not yet "first" processed
		_memorizedSplitCmd->_dataSizeLeftToBeSent=_dataSizeLeftToBeSent;
		bool retVal=_memorizedSplitCmd->appendYourMemorizedSplitData(false,dataString,removeCommand,otherSideIsBigEndian);
		_dataSizeLeftToBeSent=_memorizedSplitCmd->_dataSizeLeftToBeSent;
		if (_opMode==simx_opmode_oneshot_split)
			removeCommand=(_dataSizeLeftToBeSent==0);
		return(retVal);
	}

	if (_dataSizeLeftToBeSent<=0)
		return(false);

	//1. Prepare the sub-header:
	char header[SIMX_SUBHEADER_SIZE];
	((int*)(header+simx_cmdheaderoffset_cmd))[0]=littleEndianIntConversion(_rawCmdID+_opMode,otherSideIsBigEndian); // return also the opmode, we need to detect cont. cmds on the client side!
	header[simx_cmdheaderoffset_status]=_status;

	//2. Check the data to append:
	int commandByteDataSize=0;
	int commandStringDataSize=0;
	if ((_rawCmdID>simx_cmd4bytes_start)&&(_rawCmdID<simx_cmd8bytes_start))
		commandByteDataSize=4;
	if ((_rawCmdID>simx_cmd8bytes_start)&&(_rawCmdID<simx_cmd1string_start))
		commandByteDataSize=8;
	if ((_rawCmdID>simx_cmd1string_start)&&(_rawCmdID<simx_cmdreserved_start))
		commandStringDataSize=(int)_cmdString.length()+1;
	
	//3. We have the total length of this command, and can decide how much we wanna send each time:
	int totExceptHeaderAndPureData=commandByteDataSize+commandStringDataSize;
	int pureDataPart=_pureDataSize;
	int pureDataOffset=0;
	if (_pureDataSize>_processingDelayOrMaxDataSize)
	{ // we can't send everything at once
		if (_processingDelayOrMaxDataSize<_dataSizeLeftToBeSent)
		{
			pureDataPart=_processingDelayOrMaxDataSize;
		}
		else
		{
			pureDataPart=_dataSizeLeftToBeSent;
		}
		pureDataOffset=_pureDataSize-_dataSizeLeftToBeSent;
		_dataSizeLeftToBeSent-=pureDataPart;
		if (_dataSizeLeftToBeSent<0)
			_dataSizeLeftToBeSent=0;
	}
	else
		_dataSizeLeftToBeSent=0;

	((int*)(header+simx_cmdheaderoffset_mem_size))[0]=littleEndianIntConversion(SIMX_SUBHEADER_SIZE+totExceptHeaderAndPureData+pureDataPart,otherSideIsBigEndian);
	((int*)(header+simx_cmdheaderoffset_full_mem_size))[0]=littleEndianIntConversion(SIMX_SUBHEADER_SIZE+totExceptHeaderAndPureData+_pureDataSize,otherSideIsBigEndian);
	((WORD*)(header+simx_cmdheaderoffset_pdata_offset0))[0]=littleEndianWordConversion(commandByteDataSize+commandStringDataSize,otherSideIsBigEndian);
	((int*)(header+simx_cmdheaderoffset_pdata_offset1))[0]=littleEndianIntConversion(pureDataOffset,otherSideIsBigEndian);
	((int*)(header+simx_cmdheaderoffset_sim_time))[0]=littleEndianIntConversion(_executionTime,otherSideIsBigEndian);


	//4. Append the sub-header:
	for (int i=0;i<SIMX_SUBHEADER_SIZE;i++)
		dataString.push_back(header[i]);
	//5. Append the command data:
	for (int i=0;i<commandByteDataSize;i++)
		dataString.push_back(_cmdData[i]);
	if (commandStringDataSize>0)
	{
		for (int i=0;i<commandStringDataSize-1;i++)
			dataString.push_back(_cmdString[i]);
		dataString.push_back(0); // terminal zero
	}
	//6. Append the auxiliary data, but only the part we need to send now:
	for (int i=0;i<pureDataPart;i++)
		dataString.push_back(_pureData[pureDataOffset+i]);
	return(true);
}

void CSimxCmd::appendIntToString(std::string& str,int v,bool doConversion,bool otherSideIsBigEndian)
{
	char* vp=(char*)(&v);
	if ((!doConversion)||(!otherSideIsBigEndian))
	{
		str.push_back(vp[0]);
		str.push_back(vp[1]);
		str.push_back(vp[2]);
		str.push_back(vp[3]);
	}
	else
	{
		str.push_back(vp[3]);
		str.push_back(vp[2]);
		str.push_back(vp[1]);
		str.push_back(vp[0]);
	}
}

void CSimxCmd::appendFloatToString(std::string& str,float v,bool doConversion,bool otherSideIsBigEndian)
{
	char* vp=(char*)(&v);
	if ((!doConversion)||(!otherSideIsBigEndian))
	{
		str.push_back(vp[0]);
		str.push_back(vp[1]);
		str.push_back(vp[2]);
		str.push_back(vp[3]);
	}
	else
	{
		str.push_back(vp[3]);
		str.push_back(vp[2]);
		str.push_back(vp[1]);
		str.push_back(vp[0]);
	}
}

void CSimxCmd::setDataReply_nothing(bool success)
{
	_status=0;
	if (!success)
		_status|=1;
	delete[] _pureData;
	_pureData=NULL;
	_pureDataSize=0;
}

void CSimxCmd::setDataReply_custom_transferBuffer(char* customData,int customDataSize,bool success)
{
	_status=0;
	if (!success)
		_status|=1;
	delete[] _pureData;
	_pureData=customData;
	_pureDataSize=customDataSize;
}

void CSimxCmd::setDataReply_custom_copyBuffer(char* customData,int customDataSize,bool success)
{
	_status=0;
	if (!success)
		_status|=1;
	delete[] _pureData;
	_pureData=new char[customDataSize];
	for (int i=0;i<customDataSize;i++)
		_pureData[i]=customData[i];
	_pureDataSize=customDataSize;
}

void CSimxCmd::setDataReply_1float(float floatVal,bool success,bool otherSideIsBigEndian)
{
	_status=0;
	if (!success)
		_status|=1;
	if (_pureDataSize!=4)
	{
		delete[] _pureData;
		_pureData=new char[4];
		_pureDataSize=4;
		((float*)_pureData)[0]=littleEndianFloatConversion(floatVal,otherSideIsBigEndian);
	}
}

void CSimxCmd::setDataReply_1int(int intVal,bool success,bool otherSideIsBigEndian)
{
	_status=0;
	if (!success)
		_status|=1;
	if (_pureDataSize!=4)
	{
		delete[] _pureData;
		_pureData=new char[4];
		_pureDataSize=4;
		((int*)_pureData)[0]=littleEndianIntConversion(intVal,otherSideIsBigEndian);
	}
}

void CSimxCmd::setDataReply_2int(int intVal1,int intVal2,bool success,bool otherSideIsBigEndian)
{
	_status=0;
	if (!success)
		_status|=1;
	if (_pureDataSize!=8)
	{
		delete[] _pureData;
		_pureData=new char[8];
		_pureDataSize=8;
		((int*)_pureData)[0]=littleEndianIntConversion(intVal1,otherSideIsBigEndian);
		((int*)_pureData)[1]=littleEndianIntConversion(intVal2,otherSideIsBigEndian);
	}
}

void CSimxCmd::setDataReply_3int(int intVal1,int intVal2,int intVal3,bool success,bool otherSideIsBigEndian)
{
	_status=0;
	if (!success)
		_status|=1;
	if (_pureDataSize!=12)
	{
		delete[] _pureData;
		_pureData=new char[12];
		_pureDataSize=12;
		((int*)_pureData)[0]=littleEndianIntConversion(intVal1,otherSideIsBigEndian);
		((int*)_pureData)[1]=littleEndianIntConversion(intVal2,otherSideIsBigEndian);
		((int*)_pureData)[2]=littleEndianIntConversion(intVal3,otherSideIsBigEndian);
	}
}

CSimxCmd* CSimxCmd::copyYourself()
{
	CSimxCmd* newCmd=new CSimxCmd();
	
	newCmd->_rawCmdID=_rawCmdID;
	newCmd->_opMode=_opMode;
	newCmd->_status=_status;
	newCmd->_processingDelayOrMaxDataSize=_processingDelayOrMaxDataSize;
	newCmd->_lastTimeProcessed=_lastTimeProcessed;
	newCmd->_dataSizeLeftToBeSent=_dataSizeLeftToBeSent;
	newCmd->_cmdString=_cmdString;
	newCmd->_executionTime=_executionTime;
	if (_memorizedSplitCmd!=NULL)
		newCmd->_memorizedSplitCmd=_memorizedSplitCmd->copyYourself();
	else
		newCmd->_memorizedSplitCmd=NULL;
	for (int i=0;i<8;i++)
		newCmd->_cmdData[i]=_cmdData[i];
	newCmd->_pureDataSize=_pureDataSize;
	if (_pureData!=NULL)
	{
		newCmd->_pureData=new char[_pureDataSize];
		for (int i=0;i<_pureDataSize;i++)
			newCmd->_pureData[i]=_pureData[i];
	}
	else
		newCmd->_pureData=NULL;

	return(newCmd);
}

void CSimxCmd::setLastTimeProcessed(DWORD t)
{
	_lastTimeProcessed=t;
}

DWORD CSimxCmd::getLastTimeProcessed()
{
	return(_lastTimeProcessed);
}


CSimxCmd* CSimxCmd::execute(CSimxSocket* sock,bool otherSideIsBigEndian)
{ // Even "set-" commands should return a value, so the client can check if an error occured here
	int ct=int(simGetSystemTime()*1000.1f);

	// Continuous commands: we might not want to always process (and send) this command. Maybe only after a certain delay
	if ((_opMode==simx_opmode_continuous)&&(_processingDelayOrMaxDataSize!=0)&&(_lastTimeProcessed!=0))
	{ 
		if (ct-_lastTimeProcessed<_processingDelayOrMaxDataSize)
			return(NULL); // not now!
	}
	_lastTimeProcessed=ct;
	
	CSimxCmd* retCmd=NULL;

	if (_opMode==simx_opmode_discontinue)
	{ // we send back the discontinue command, without executing it here!
		retCmd=copyYourself();
		delete retCmd->_memorizedSplitCmd; // probably not necessary
		retCmd->_memorizedSplitCmd=NULL;
		retCmd->setDataReply_nothing(true);
		return(retCmd);
	}

	if ( ((_opMode!=simx_opmode_continuous_split)&&(_opMode!=simx_opmode_oneshot_split))||(_dataSizeLeftToBeSent==0) )
		retCmd=_executeCommand(sock,otherSideIsBigEndian);

	// Prepare to concatenate the data here (if needed). 
	if ((_opMode==simx_opmode_continuous_split)||(_opMode==simx_opmode_oneshot_split))
	{ // in this mode we reprocess the command only once all parts were sent. These commands will not automatically be put into the output container like other commands
		if (_dataSizeLeftToBeSent==0)
		{
			delete _memorizedSplitCmd;
			delete retCmd->_memorizedSplitCmd;
			retCmd->_memorizedSplitCmd=NULL;
			_memorizedSplitCmd=retCmd;
			_dataSizeLeftToBeSent=_memorizedSplitCmd->_pureDataSize;
		}
		retCmd=NULL;
	}

	return(retCmd);
}

CSimxCmd* CSimxCmd::_executeCommand(CSimxSocket* sock,bool otherSideIsBigEndian)
{
//	if (_rawCmdID==simx_cmd_get_vision_sensor_image_rgb)
//		printf("*");

	if (simGetSimulationState()==sim_simulation_stopped)
		_executionTime=0;
	else
		_executionTime=int(simGetSimulationTime()*1000.01f);

	CSimxCmd* retCmd=copyYourself();
	retCmd->_status|=1; // this means error on the server side. The flag will be cleared if the execution was successful


// *************************************************************************
// Here we have the custom remote API functions (i.e. designed by the user)
// Not very elegant and considered bad practice to do this kind of inclusion, 
// but here we go anyway:
#include "simxCustomCmd.cpp"
// *************************************************************************

	// From here on we have the regular remote API functions:
	if (_rawCmdID==simx_cmd_get_joint_position)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		float pos=0.0f;
		bool success=(simGetJointPosition(handle,&pos)!=-1);
		retCmd->setDataReply_1float(pos,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_get_joint_matrix)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		float matrix[12];
		bool success=(simGetJointMatrix(handle,matrix)!=-1);
		if (success)
		{
			for (int i=0;i<12;i++)
				matrix[i]=littleEndianFloatConversion(matrix[i],otherSideIsBigEndian);
		}
		retCmd->setDataReply_custom_copyBuffer((char*)matrix,12*4,success);
	}

	if (_rawCmdID==simx_cmd_read_proximity_sensor)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		float detectedPoint[4];
		int detectedObjectHandle;
		float detectedSurfaceNormalVector[3];
		int res=simReadProximitySensor(handle,detectedPoint,&detectedObjectHandle,detectedSurfaceNormalVector);
		bool success=(res!=-1);
		char data[29];
		data[0]=char(res);
		((float*)(data+1))[0]=littleEndianFloatConversion(detectedPoint[0],otherSideIsBigEndian);
		((float*)(data+5))[0]=littleEndianFloatConversion(detectedPoint[1],otherSideIsBigEndian);
		((float*)(data+9))[0]=littleEndianFloatConversion(detectedPoint[2],otherSideIsBigEndian);
		((int*)(data+13))[0]=littleEndianIntConversion(detectedObjectHandle,otherSideIsBigEndian);
		((float*)(data+17))[0]=littleEndianFloatConversion(detectedSurfaceNormalVector[0],otherSideIsBigEndian);
		((float*)(data+21))[0]=littleEndianFloatConversion(detectedSurfaceNormalVector[1],otherSideIsBigEndian);
		((float*)(data+25))[0]=littleEndianFloatConversion(detectedSurfaceNormalVector[2],otherSideIsBigEndian);
		retCmd->setDataReply_custom_copyBuffer(data,29,success);
	}

	if (_rawCmdID==simx_cmd_get_object_handle)
	{
		int handle=simGetObjectHandle(_cmdString.c_str());
		bool success=(handle!=-1);
		retCmd->setDataReply_1int(handle,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_get_ui_handle)
	{
		int handle=simGetUIHandle(_cmdString.c_str());
		bool success=(handle!=-1);
		retCmd->setDataReply_1int(handle,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_load_model)
	{
		simRemoveObjectFromSelection(sim_handle_all,-1);
		int initValue=simGetBooleanParameter(sim_boolparam_scene_and_model_load_messages);
		simSetBooleanParameter(sim_boolparam_scene_and_model_load_messages,0);
		bool success=(simLoadModel(_cmdString.c_str())!=-1);
		simSetBooleanParameter(sim_boolparam_scene_and_model_load_messages,initValue);
		int handle=simGetObjectLastSelection();
		retCmd->setDataReply_1int(handle,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_load_scene)
	{
		int initValue=simGetBooleanParameter(sim_boolparam_scene_and_model_load_messages);
		simSetBooleanParameter(sim_boolparam_scene_and_model_load_messages,0);
		bool success=(simLoadScene(_cmdString.c_str())!=-1);
		simSetBooleanParameter(sim_boolparam_scene_and_model_load_messages,initValue);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_set_joint_position)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		float pos=littleEndianFloatConversion(((float*)_pureData)[0],otherSideIsBigEndian);
		bool success=(simSetJointPosition(handle,pos)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_set_spherical_joint_matrix)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		float matrix[12];
		for (int i=0;i<12;i++)
			matrix[i]=littleEndianFloatConversion(((float*)_pureData)[i],otherSideIsBigEndian);
		bool success=(simSetSphericalJointMatrix(handle,matrix)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_set_joint_target_velocity)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		float vel=littleEndianFloatConversion(((float*)_pureData)[0],otherSideIsBigEndian);
		bool success=(simSetJointTargetVelocity(handle,vel)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_set_joint_target_position)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		float pos=littleEndianFloatConversion(((float*)_pureData)[0],otherSideIsBigEndian);
		bool success=(simSetJointTargetPosition(handle,pos)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_start_pause_stop_simulation)
	{
		int v=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		bool success=false;
		if (v==0)
			success=(simStartSimulation()!=-1);
		if (v==1)
			success=(simPauseSimulation()!=-1);
		if (v==2)
			success=(simStopSimulation()!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_synchronous_next)
	{
		bool success=false;
		if (sock->getWaitForTriggerEnabled())
		{
			if ((simGetSimulationState()&sim_simulation_advancing)!=0)
			{
				sock->setWaitForTrigger(false);
				success=true;
			}
		}
		retCmd->setDataReply_nothing(success);
	}
	if (_rawCmdID==simx_cmd_synchronous_enable)
	{ 
		bool success=false;
		if (sock->getWaitForTriggerAuthorized())
		{
			sock->setWaitForTriggerEnabled(true);
			success=true;
		}
		retCmd->setDataReply_nothing(success);
	}
	if (_rawCmdID==simx_cmd_synchronous_disable)
	{ 
		bool success=false;
		if (sock->getWaitForTriggerAuthorized())
		{
			sock->setWaitForTriggerEnabled(false);
			success=true;
		}
		retCmd->setDataReply_nothing(success);
	}

	if ( (_rawCmdID==simx_cmd_get_vision_sensor_image_bw)||(_rawCmdID==simx_cmd_get_vision_sensor_image_rgb) )
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int res[2];
		bool success=false;
		if (simGetVisionSensorResolution(handle,res)!=-1)
		{
			int bytesPerPixel;
			if (_rawCmdID==simx_cmd_get_vision_sensor_image_bw)
				bytesPerPixel=1;
			if (_rawCmdID==simx_cmd_get_vision_sensor_image_rgb)
				bytesPerPixel=3;

			char* dat=new char[4+4+res[0]*res[1]*bytesPerPixel];
			unsigned char* img=simGetVisionSensorCharImage(handle,NULL,NULL);
			if (img!=NULL)
			{
				success=true;
				((int*)(dat+0))[0]=littleEndianIntConversion(res[0],otherSideIsBigEndian);
				((int*)(dat+0))[1]=littleEndianIntConversion(res[1],otherSideIsBigEndian);
				if (bytesPerPixel==1)
				{
					for (int i=0;i<res[0]*res[1];i++)
						dat[8+i]=char((img[3*i+0]+img[3*i+1]+img[3*i+2])/3);
				}
				if (bytesPerPixel==3)
				{
					for (int i=0;i<res[0]*res[1]*3;i++)
						dat[8+i]=img[i];
				}
				simReleaseBuffer((simChar*)img);
				retCmd->setDataReply_custom_transferBuffer(dat,4+4+res[0]*res[1]*bytesPerPixel,success);
			}
			else
				delete[] dat;
		}
		if (!success)
			retCmd->setDataReply_nothing(success);
	}

	if ( (_rawCmdID==simx_cmd_set_vision_sensor_image_bw)||(_rawCmdID==simx_cmd_set_vision_sensor_image_rgb) )
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int res[2];
		bool success=false;
		if (simGetVisionSensorResolution(handle,res)!=-1)
		{
			int bytesPerPixel;
			if (_rawCmdID==simx_cmd_set_vision_sensor_image_bw)
				bytesPerPixel=1;
			if (_rawCmdID==simx_cmd_set_vision_sensor_image_rgb)
				bytesPerPixel=3;
			if (res[0]*res[1]*bytesPerPixel==_pureDataSize)
			{
				unsigned char* img=new unsigned char[res[0]*res[1]*3];
				if (bytesPerPixel==1)
				{
					for (int i=0;i<_pureDataSize;i++)
					{
						img[3*i+0]=_pureData[i];
						img[3*i+1]=_pureData[i];
						img[3*i+2]=_pureData[i];
					}
				}
				if (bytesPerPixel==3)
				{
					for (int i=0;i<_pureDataSize;i++)
						img[i]=_pureData[i];
				}
				success=(simSetVisionSensorCharImage(handle,img)!=-1);
				delete[] img;
			}
		}
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_get_joint_force)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		float f=0.0f;
		bool success=(simJointGetForce(handle,&f)!=-1);
		retCmd->setDataReply_1float(f,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_set_joint_force)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		float f=littleEndianFloatConversion(((float*)_pureData)[0],otherSideIsBigEndian);
		bool success=(simSetJointForce(handle,f)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_read_force_sensor)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		float forceV[3];
		float torqueV[3];
		int res=simReadForceSensor(handle,forceV,torqueV);
		char dat[25];
		dat[0]=0;
		if (res>=0)
			dat[0]=BYTE(res);
		for (int i=0;i<3;i++)
			((float*)(dat+1))[i]=littleEndianFloatConversion(forceV[i],otherSideIsBigEndian);
		for (int i=0;i<3;i++)
			((float*)(dat+13))[i]=littleEndianFloatConversion(torqueV[i],otherSideIsBigEndian);
		bool success=(res!=-1);
		retCmd->setDataReply_custom_copyBuffer(dat,25,success);
	}

	if (_rawCmdID==simx_cmd_break_force_sensor)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		bool success=(simBreakForceSensor(handle)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_read_vision_sensor)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		float* auxValues;
		int* auxValuesCount;
		int res=simReadVisionSensor(handle,&auxValues,&auxValuesCount);
		if (res>=0)
		{
			int packetCnt=auxValuesCount[0];
			int auxValCnt=0;
			for (int i=0;i<packetCnt;i++)
				auxValCnt+=auxValuesCount[1+i];
			char* buff=new char[1+4*(1+packetCnt+auxValCnt)];
			buff[0]=BYTE(res);
			for (int i=0;i<packetCnt+1;i++)
				((int*)(buff+1))[i]=littleEndianIntConversion(auxValuesCount[i],otherSideIsBigEndian);
			for (int i=0;i<auxValCnt;i++) // was for (int i=0;i<auxValCnt+1;i++), thanks to Billy Newman for noticing the bug
				((float*)(buff+1))[packetCnt+1+i]=littleEndianFloatConversion(auxValues[i],otherSideIsBigEndian);
			retCmd->setDataReply_custom_transferBuffer(buff,1+4*(1+packetCnt+auxValCnt),true);
			simReleaseBuffer((char*)auxValues);
			simReleaseBuffer((char*)auxValuesCount);
		}
		else
			retCmd->setDataReply_1int(0,false,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_get_object_parent)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int parent=simGetObjectParent(handle);
//		bool success=(parent!=-1);
		retCmd->setDataReply_1int(parent,true,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_get_object_child)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int index=littleEndianIntConversion(((int*)(_cmdData+0))[1],otherSideIsBigEndian);
		int child=simGetObjectChild(handle,index);
//		bool success=(child!=-1);
		retCmd->setDataReply_1int(child,true,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_transfer_file)
	{
		FILE* file=fopen(_cmdString.c_str(),"wb");
		bool success=(file!=NULL);
		if (success)
		{
			fwrite(_pureData,1,_pureDataSize,file);
			fclose(file);
		}
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_erase_file)
	{
		bool success=(remove(_cmdString.c_str())==0);
		retCmd->setDataReply_nothing(success);
	}
	
	if (_rawCmdID==simx_cmd_load_ui)
	{
		int handles[1000];
		int cnt=simLoadUI(_cmdString.c_str(),1000,handles);
		bool success=(cnt!=-1);
		if (!success)
			cnt=0;
		int* dat=new int[cnt+1];
		dat[0]=littleEndianIntConversion(cnt,otherSideIsBigEndian);
		for (int i=0;i<cnt;i++)
			dat[1+i]=littleEndianIntConversion(handles[i],otherSideIsBigEndian);
		retCmd->setDataReply_custom_transferBuffer((char*)dat,4*(cnt+1),success);
	}

	if (_rawCmdID==simx_cmd_get_ui_slider)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int buttonID=littleEndianIntConversion(((int*)(_cmdData+0))[1],otherSideIsBigEndian);
		int pos=simGetUISlider(handle,buttonID);
		bool success=(pos!=-1);
		retCmd->setDataReply_1int(pos,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_set_ui_slider)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int buttonID=littleEndianIntConversion(((int*)(_cmdData+0))[1],otherSideIsBigEndian);
		int pos=littleEndianIntConversion(((int*)_pureData)[0],otherSideIsBigEndian);
		bool success=(simSetUISlider(handle,buttonID,pos)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_get_ui_event_button)
	{ 
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int auxVals[2];
		int buttonID=simGetUIEventButton(handle,auxVals);
		if (buttonID!=-1)
			_opMode=simx_opmode_oneshot; // an event was received. We need to remove this command! (this is a special case, only for this command. See the doc!)
//		bool success=(buttonID!=-1);
		retCmd->setDataReply_3int(buttonID,auxVals[0],auxVals[1],true,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_get_ui_button_property)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int buttonID=littleEndianIntConversion(((int*)(_cmdData+0))[1],otherSideIsBigEndian);
		int prop=simGetUIButtonProperty(handle,buttonID);
		bool success=(prop!=-1);
		retCmd->setDataReply_1int(prop,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_set_ui_button_property)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int buttonID=littleEndianIntConversion(((int*)(_cmdData+0))[1],otherSideIsBigEndian);
		int prop=littleEndianIntConversion(((int*)_pureData)[0],otherSideIsBigEndian);
		bool success=(simSetUIButtonProperty(handle,buttonID,prop)!=-1);
		retCmd->setDataReply_nothing(success);
	}
	
	if (_rawCmdID==simx_cmd_add_statusbar_message)
	{
		int res=simAddStatusbarMessage(_cmdString.c_str());
		bool success=(res!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_aux_console_open)
	{
		int maxLines=littleEndianIntConversion(((int*)(_pureData+0))[0],otherSideIsBigEndian);
		int mode=littleEndianIntConversion(((int*)(_pureData+0))[1],otherSideIsBigEndian);

		int* pos=NULL;
		int _pos[2];
		_pos[0]=littleEndianIntConversion(((int*)(_pureData+0))[2],otherSideIsBigEndian);
		_pos[1]=littleEndianIntConversion(((int*)(_pureData+0))[3],otherSideIsBigEndian);
		if (_pos[0]!=98765)
			pos=_pos; // arg is not NULL!

		int* size=NULL;
		int _size[2];
		_size[0]=littleEndianIntConversion(((int*)(_pureData+0))[4],otherSideIsBigEndian);
		_size[1]=littleEndianIntConversion(((int*)(_pureData+0))[5],otherSideIsBigEndian);
		if (_size[0]!=98765)
			size=_size; // arg is not NULL!

		float* tcol=NULL;
		float _tcol[3];
		_tcol[0]=littleEndianFloatConversion(((float*)(_pureData+0))[6],otherSideIsBigEndian);
		_tcol[1]=littleEndianFloatConversion(((float*)(_pureData+0))[7],otherSideIsBigEndian);
		_tcol[2]=littleEndianFloatConversion(((float*)(_pureData+0))[8],otherSideIsBigEndian);
		if (_tcol[0]>-5.0f)
			tcol=_tcol; // arg is not NULL!

		float* bcol=NULL;
		float _bcol[3];
		_bcol[0]=littleEndianFloatConversion(((float*)(_pureData+0))[9],otherSideIsBigEndian);
		_bcol[1]=littleEndianFloatConversion(((float*)(_pureData+0))[10],otherSideIsBigEndian);
		_bcol[2]=littleEndianFloatConversion(((float*)(_pureData+0))[11],otherSideIsBigEndian);
		if (_bcol[0]>-5.0f)
			bcol=_bcol; // arg is not NULL!

		int handle=simAuxiliaryConsoleOpen(_cmdString.c_str(),maxLines,mode,pos,size,tcol,bcol);
		bool success=(handle!=-1);
		retCmd->setDataReply_1int(handle,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_create_dummy)
	{
		float size=littleEndianFloatConversion(((float*)(_pureData+0))[0],otherSideIsBigEndian);
		float cols[12];
		for (int i=0;i<12;i++)
			cols[i]=float(((unsigned char*)_pureData)[4+1+i])/255.0f;
		float* c=NULL;
		if (_pureData[4+0]!=0)
			c=cols;
		int handle=simCreateDummy(size,c);
		bool success=(handle!=-1);
		retCmd->setDataReply_1int(handle,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_aux_console_close)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		bool success=(simAuxiliaryConsoleClose(handle)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_aux_console_print)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		bool success;
		if (_pureDataSize==0)
			success=(simAuxiliaryConsolePrint(handle,NULL)!=-1);
		else
			success=(simAuxiliaryConsolePrint(handle,_pureData)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_aux_console_show)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int showState=littleEndianIntConversion(((int*)(_pureData+0))[0],otherSideIsBigEndian);
		bool success=(simAuxiliaryConsoleShow(handle,showState)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_get_vision_sensor_depth_buffer)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int res[2];
		bool success=false;
		if (simGetVisionSensorResolution(handle,res)!=-1)
		{
			char* dat=new char[4+4+res[0]*res[1]*4];
			float* img=simGetVisionSensorDepthBuffer(handle);
			if (img!=NULL)
			{
				success=true;
				((int*)(dat+0))[0]=littleEndianIntConversion(res[0],otherSideIsBigEndian);
				((int*)(dat+0))[1]=littleEndianIntConversion(res[1],otherSideIsBigEndian);
				for (int i=0;i<res[0]*res[1];i++)
					((float*)dat)[2+i]=littleEndianFloatConversion(img[i],otherSideIsBigEndian);
				simReleaseBuffer((simChar*)img);
				retCmd->setDataReply_custom_transferBuffer(dat,4+4+res[0]*res[1]*4,success);
			}
			else
				delete[] dat;
		}
		if (!success)
			retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_get_object_orientation)
	{ // should not be used anymore, but kept for backward compatibility (10/6/2014)
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int relativeToObject=littleEndianIntConversion(((int*)(_pureData+0))[0],otherSideIsBigEndian);
		float euler[3];
		bool success=(simGetObjectOrientation(handle,relativeToObject,euler)!=-1);
		// Endian conversion on the client side!
		retCmd->setDataReply_custom_copyBuffer((char*)euler,4*3,success);
	}

	if (_rawCmdID==simx_cmd_get_object_position)
	{ // should not be used anymore, but kept for backward compatibility (10/6/2014)
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int relativeToObject=littleEndianIntConversion(((int*)(_pureData+0))[0],otherSideIsBigEndian);
		float pos[3];
		bool success=(simGetObjectPosition(handle,relativeToObject,pos)!=-1);
		// Endian conversion on the client side!
		retCmd->setDataReply_custom_copyBuffer((char*)pos,4*3,success);
	}

	if (_rawCmdID==simx_cmd_get_object_orientation2)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int relativeToObject=littleEndianIntConversion(((int*)(_cmdData+0))[1],otherSideIsBigEndian);
		float euler[3];
		bool success=(simGetObjectOrientation(handle,relativeToObject,euler)!=-1);
		// Endian conversion on the client side!
		retCmd->setDataReply_custom_copyBuffer((char*)euler,4*3,success);
	}

	if (_rawCmdID==simx_cmd_get_object_position2)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int relativeToObject=littleEndianIntConversion(((int*)(_cmdData+0))[1],otherSideIsBigEndian);
		float pos[3];
		bool success=(simGetObjectPosition(handle,relativeToObject,pos)!=-1);
		// Endian conversion on the client side!
		retCmd->setDataReply_custom_copyBuffer((char*)pos,4*3,success);
	}

	if (_rawCmdID==simx_cmd_get_object_velocity)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		float data[6];
		bool success=(simGetObjectVelocity(handle,data,data+3)!=-1);
		// Endian conversion on the client side!
		retCmd->setDataReply_custom_copyBuffer((char*)data,4*6,success);
	}

	if (_rawCmdID==simx_cmd_set_object_orientation)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int relativeToObject=littleEndianIntConversion(((int*)(_pureData+0))[0],otherSideIsBigEndian);
		float euler[3];
		euler[0]=littleEndianFloatConversion(((float*)(_pureData+0))[1],otherSideIsBigEndian);
		euler[1]=littleEndianFloatConversion(((float*)(_pureData+0))[2],otherSideIsBigEndian);
		euler[2]=littleEndianFloatConversion(((float*)(_pureData+0))[3],otherSideIsBigEndian);
		bool success=(simSetObjectOrientation(handle,relativeToObject,euler)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_set_object_position)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int relativeToObject=littleEndianIntConversion(((int*)(_pureData+0))[0],otherSideIsBigEndian);
		float pos[3];
		pos[0]=littleEndianFloatConversion(((float*)(_pureData+0))[1],otherSideIsBigEndian);
		pos[1]=littleEndianFloatConversion(((float*)(_pureData+0))[2],otherSideIsBigEndian);
		pos[2]=littleEndianFloatConversion(((float*)(_pureData+0))[3],otherSideIsBigEndian);
		bool success=(simSetObjectPosition(handle,relativeToObject,pos)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_set_object_parent)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int parentObject=littleEndianIntConversion(((int*)(_pureData+0))[0],otherSideIsBigEndian);
		bool success=(simSetObjectParent(handle,parentObject,_pureData[4])!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_set_ui_button_label)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int buttonID=littleEndianIntConversion(((int*)(_cmdData+0))[1],otherSideIsBigEndian);
		const char* str1=_pureData;
		const char* str2=_pureData+strlen(_pureData)+1;
		bool success=(simSetUIButtonLabel(handle,buttonID,str1,str2)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_get_last_errors)
	{
		int s;
		char* errString=sock->getAndClearFullErrorStringBuffer(s);
		// the first 4 values represent an int that is the error string count.
		((int*)errString)[0]=littleEndianIntConversion(((int*)errString)[0],otherSideIsBigEndian);
		retCmd->setDataReply_custom_transferBuffer(errString,s,true);
	}

	if (_rawCmdID==simx_cmd_get_object_group_data)
	{
		int objectType=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int dataType=littleEndianIntConversion(((int*)(_cmdData+0))[1],otherSideIsBigEndian);
		std::string retData;
		std::vector<int> retHandles;
		std::vector<int> retInt;
		std::vector<float> retFloat;
		std::string retString;
		int retStringCount=0;
		int i=0;
		bool success=false;
		if ((objectType==sim_appobj_object_type)||((objectType>=sim_object_shape_type)&&(objectType<sim_object_type_end)) )
		{
			if (objectType==sim_appobj_object_type)
				objectType=sim_handle_all;
			success=true;
			while (true)
			{
				int handle=simGetObjects(i++,objectType);
				if (handle<0)
					break;
				retHandles.push_back(handle);
				if (dataType==0)
				{ // object name
					char* name=simGetObjectName(handle);
					retString+=name;
					retString+='\0';
					simReleaseBuffer(name);
					retStringCount++;
				}
				if (dataType==1)
				{ // object type
					retInt.push_back(simGetObjectType(handle));
				}
				if (dataType==2)
				{ // object parent
					retInt.push_back(simGetObjectParent(handle));
				}
				if ((dataType==3)||(dataType==4))
				{ // object position
					int w=-1; // abs
					if (dataType==4)
						w=sim_handle_parent; // rel
					float p[3];
					simGetObjectPosition(handle,w,p);
					retFloat.push_back(p[0]);
					retFloat.push_back(p[1]);
					retFloat.push_back(p[2]);
				}
				if ((dataType==5)||(dataType==6))
				{ // object orientation (Euler angles)
					int w=-1; // abs
					if (dataType==6)
						w=sim_handle_parent; // rel
					float o[3];
					simGetObjectOrientation(handle,w,o);
					retFloat.push_back(o[0]);
					retFloat.push_back(o[1]);
					retFloat.push_back(o[2]);
				}
				if ((dataType==7)||(dataType==8))
				{ // object orientation (Quaternions)
					int w=-1; // abs
					if (dataType==8)
						w=sim_handle_parent; // rel
					float q[4];
					simGetObjectQuaternion(handle,w,q);
					retFloat.push_back(q[0]);
					retFloat.push_back(q[1]);
					retFloat.push_back(q[2]);
					retFloat.push_back(q[3]);
				}
				if ((dataType==9)||(dataType==10))
				{ // object pose (position+orientation (Euler angles))
					int w=-1; // abs
					if (dataType==10)
						w=sim_handle_parent; // rel
					float p[3];
					simGetObjectPosition(handle,w,p);
					retFloat.push_back(p[0]);
					retFloat.push_back(p[1]);
					retFloat.push_back(p[2]);
					float o[3];
					simGetObjectOrientation(handle,w,o);
					retFloat.push_back(o[0]);
					retFloat.push_back(o[1]);
					retFloat.push_back(o[2]);
				}
				if ((dataType==11)||(dataType==12))
				{ // object pose (position+orientation (Quaternion))
					int w=-1; // abs
					if (dataType==12)
						w=sim_handle_parent; // rel
					float p[3];
					simGetObjectPosition(handle,w,p);
					retFloat.push_back(p[0]);
					retFloat.push_back(p[1]);
					retFloat.push_back(p[2]);
					float q[4];
					simGetObjectOrientation(handle,w,q);
					retFloat.push_back(q[0]);
					retFloat.push_back(q[1]);
					retFloat.push_back(q[2]);
					retFloat.push_back(q[3]);
				}
				if (dataType==13)
				{ // prox sensor data
					if (simGetObjectType(handle)==sim_object_proximitysensor_type)
					{
						float pt[4];
						int obj;
						float normal[3];
						int res=simReadProximitySensor(handle,pt,&obj,normal);
						retInt.push_back(res);
						retInt.push_back(obj);
						retFloat.push_back(pt[0]);
						retFloat.push_back(pt[1]);
						retFloat.push_back(pt[2]);
						retFloat.push_back(normal[0]);
						retFloat.push_back(normal[1]);
						retFloat.push_back(normal[2]);
					}
					else
					{ // this is not a proximity sensor!
						retInt.push_back(-1);
						retInt.push_back(-1);
						retFloat.push_back(0.0f);
						retFloat.push_back(0.0f);
						retFloat.push_back(0.0f);
						retFloat.push_back(0.0f);
						retFloat.push_back(0.0f);
						retFloat.push_back(0.0f);
					}
				}
				if (dataType==14)
				{ // force sensor data
					if (simGetObjectType(handle)==sim_object_forcesensor_type)
					{
						float force[3];
						float torque[3];
						int res=simReadForceSensor(handle,force,torque);
						retInt.push_back(res);
						retFloat.push_back(force[0]);
						retFloat.push_back(force[1]);
						retFloat.push_back(force[2]);
						retFloat.push_back(torque[0]);
						retFloat.push_back(torque[1]);
						retFloat.push_back(torque[2]);
					}
					else
					{ // this is not a force sensor!
						retInt.push_back(-1);
						retFloat.push_back(0.0f);
						retFloat.push_back(0.0f);
						retFloat.push_back(0.0f);
						retFloat.push_back(0.0f);
						retFloat.push_back(0.0f);
						retFloat.push_back(0.0f);
					}
				}
				if (dataType==15)
				{ // joint data
					if (simGetObjectType(handle)==sim_object_joint_type)
					{
						float pos=0.0f;
						float force=0.0f;
						simGetJointPosition(handle,&pos);
						simJointGetForce(handle,&force);
						retFloat.push_back(pos);
						retFloat.push_back(force);
					}
					else
					{ // this is not a joint!
						retFloat.push_back(0.0f);
						retFloat.push_back(0.0f);
					}
				}
				if (dataType==16)
				{ // joint type, mode and limit data
					if (simGetObjectType(handle)==sim_object_joint_type)
					{
						float range[2];
						simBool cyclic;
						simGetJointInterval(handle,&cyclic,range);
						if (cyclic)
							range[1]=-1.0f;
						int jointType=simGetJointType(handle);
						int options;
						int jointMode=simGetJointMode(handle,&options);
						if (options&1)
							jointMode|=65536;
						retInt.push_back(jointType);
						retInt.push_back(jointMode);
						retFloat.push_back(range[0]);
						retFloat.push_back(range[1]);
					}
					else
					{ // this is not a joint!
						retInt.push_back(-1);
						retInt.push_back(-1);
						retFloat.push_back(0.0f);
						retFloat.push_back(0.0f);
					}
				}
				if (dataType==17)
				{ // object linear velocity
					float linVel[3];
					simGetObjectVelocity(handle,linVel,NULL);
					retFloat.push_back(linVel[0]);
					retFloat.push_back(linVel[1]);
					retFloat.push_back(linVel[2]);
				}
				if (dataType==18)
				{ // object angular velocity
					float angVel[3];
					simGetObjectVelocity(handle,NULL,angVel);
					retFloat.push_back(angVel[0]);
					retFloat.push_back(angVel[1]);
					retFloat.push_back(angVel[2]);
				}
				if (dataType==19)
				{ // object linear and angular velocity (twist data)
					float linVel[3];
					float angVel[3];
					simGetObjectVelocity(handle,linVel,angVel);
					retFloat.push_back(linVel[0]);
					retFloat.push_back(linVel[1]);
					retFloat.push_back(linVel[2]);
					retFloat.push_back(angVel[0]);
					retFloat.push_back(angVel[1]);
					retFloat.push_back(angVel[2]);
				}
			}
		}

		if (success)
		{
			appendIntToString(retData,int(retHandles.size()),false,otherSideIsBigEndian);
			appendIntToString(retData,int(retInt.size()),false,otherSideIsBigEndian);
			appendIntToString(retData,int(retFloat.size()),false,otherSideIsBigEndian);	
			appendIntToString(retData,retStringCount,false,otherSideIsBigEndian);
			for (int i=0;i<int(retHandles.size());i++)
				appendIntToString(retData,retHandles[i],true,otherSideIsBigEndian);
			for (int i=0;i<int(retInt.size());i++)
				appendIntToString(retData,retInt[i],true,otherSideIsBigEndian);
			for (int i=0;i<int(retFloat.size());i++)
				appendFloatToString(retData,retFloat[i],true,otherSideIsBigEndian);
			retData+=retString;
			retCmd->setDataReply_custom_copyBuffer(&retData[0],int(retData.length()),success);
		}
		else
			retCmd->setDataReply_nothing(false);
	}


	if (_rawCmdID==simx_cmd_get_array_parameter)
	{
		int parameterID=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		float p[3];
		bool success=(simGetArrayParameter(parameterID,p)!=-1);
		if (success)
		{
			p[0]=littleEndianFloatConversion(p[0],otherSideIsBigEndian);
			p[1]=littleEndianFloatConversion(p[1],otherSideIsBigEndian);
			p[2]=littleEndianFloatConversion(p[2],otherSideIsBigEndian);
		}
		retCmd->setDataReply_custom_copyBuffer((char*)p,3*4,success);
	}

	if (_rawCmdID==simx_cmd_set_array_parameter)
	{
		int parameterID=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		float p[3];
		p[0]=littleEndianFloatConversion(((float*)_pureData)[0],otherSideIsBigEndian);
		p[1]=littleEndianFloatConversion(((float*)_pureData)[1],otherSideIsBigEndian);
		p[2]=littleEndianFloatConversion(((float*)_pureData)[2],otherSideIsBigEndian);
		bool success=(simSetArrayParameter(parameterID,p)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_get_boolean_parameter)
	{
		int parameterID=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int p=simGetBooleanParameter(parameterID);
		bool success=(p!=-1);
		retCmd->setDataReply_1int(p,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_set_boolean_parameter)
	{
		int parameterID=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int p=littleEndianIntConversion(((int*)_pureData)[0],otherSideIsBigEndian);
		bool success=(simSetBooleanParameter(parameterID,p)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_get_integer_parameter)
	{
		int parameterID=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int p;
		bool success=(simGetIntegerParameter(parameterID,&p)!=-1);
		retCmd->setDataReply_1int(p,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_set_integer_parameter)
	{
		int parameterID=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int p=littleEndianIntConversion(((int*)_pureData)[0],otherSideIsBigEndian);
		bool success=(simSetIntegerParameter(parameterID,p)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_get_floating_parameter)
	{
		int parameterID=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		float p;
		bool success=(simGetFloatingParameter(parameterID,&p)!=-1);
		retCmd->setDataReply_1float(p,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_set_floating_parameter)
	{
		int parameterID=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		float p=littleEndianFloatConversion(((float*)_pureData)[0],otherSideIsBigEndian);
		bool success=(simSetFloatingParameter(parameterID,p)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_get_string_parameter)
	{
		int parameterID=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		char* str=simGetStringParameter(parameterID);
		if (str!=NULL)
		{
			retCmd->setDataReply_custom_copyBuffer(str,int(strlen(str)+1),true);
			simReleaseBuffer(str);
		}
		else
			retCmd->setDataReply_nothing(false);
	}

	if (_rawCmdID==simx_cmd_get_collision_handle)
	{
		int handle=simGetCollisionHandle(_cmdString.c_str());
		bool success=(handle!=-1);
		retCmd->setDataReply_1int(handle,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_get_distance_handle)
	{
		int handle=simGetDistanceHandle(_cmdString.c_str());
		bool success=(handle!=-1);
		retCmd->setDataReply_1int(handle,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_read_collision)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int res=simReadCollision(handle);
		bool success=(res!=-1);
		retCmd->setDataReply_1int(res,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_read_distance)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		float dist=0.0f;
		bool success=(simReadDistance(handle,&dist)!=-1);
		retCmd->setDataReply_1float(dist,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_remove_object)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		bool success=(simRemoveObject(handle)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_remove_model)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		bool success=(simRemoveModel(handle)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_remove_ui)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		bool success=(simRemoveUI(handle)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_close_scene)
	{
		int res=simCloseScene();
		retCmd->setDataReply_nothing(res!=-1);
	}

	if (_rawCmdID==simx_cmd_get_objects)
	{
		int objType=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int index=0;
		std::vector<int> handles;
		while (true)
		{
			int h=simGetObjects(index++,objType);
			if (h==-1)
				break;
			handles.push_back(h);
		}
		char* buff=new char[4+handles.size()*4];
		((int*)buff)[0]=littleEndianIntConversion(int(handles.size()),otherSideIsBigEndian);
		for (unsigned int i=0;i<handles.size();i++)
			((int*)buff)[1+i]=littleEndianIntConversion(handles[i],otherSideIsBigEndian);
		retCmd->setDataReply_custom_transferBuffer(buff,4+int(handles.size())*4,true);
	}

	if (_rawCmdID==simx_cmd_display_dialog)
	{
		char* mainText=_pureData+0;
		int off=int(strlen(mainText))+1;
		int dlgType=littleEndianIntConversion(((int*)(_pureData+off))[0],otherSideIsBigEndian);
		off+=4;
		char* initialText=_pureData+off;
		off+=int(strlen(initialText))+1;

		float* col1=NULL;
		float _col1[6];
		for (int i=0;i<6;i++)
			_col1[i]=littleEndianFloatConversion(((float*)(_pureData+off))[i],otherSideIsBigEndian);
		if (_col1[0]>-5.0f)
			col1=_col1; // arg is not NULL!
		off+=6*4;

		float* col2=NULL;
		float _col2[6];
		for (int i=0;i<6;i++)
			_col2[i]=littleEndianFloatConversion(((float*)(_pureData+off))[i],otherSideIsBigEndian);
		if (_col2[0]>-5.0f)
			col2=_col2; // arg is not NULL!
		off+=6*4;

		int uiHandle;
		int handle=simDisplayDialog(_cmdString.c_str(),mainText,dlgType,initialText,col1,col2,&uiHandle);
		bool success=(handle!=-1);
		retCmd->setDataReply_2int(handle,uiHandle,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_end_dialog)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		bool success=(simEndDialog(handle)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_get_dialog_result)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int result=simGetDialogResult(handle);
		retCmd->setDataReply_1int(result,result!=-1,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_get_dialog_input)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		char* input=simGetDialogInput(handle);
		if (input!=NULL)
		{
			retCmd->setDataReply_custom_copyBuffer(input,int(strlen(input))+1,true);
			simReleaseBuffer(input);
		}
		else
			retCmd->setDataReply_nothing(false);
	}

	if (_rawCmdID==simx_cmd_copy_paste_objects)
	{
		// 1. Save current selection state:
		int initialSelSize=simGetObjectSelectionSize();
		int* initialSelection=new int[initialSelSize];
		simGetObjectSelection(initialSelection);
		simRemoveObjectFromSelection(sim_handle_all,0);

		// 2. Select objects we wanna copy and paste:
		int cnt=_pureDataSize/4;
		for (int i=0;i<cnt;i++)
			simAddObjectToSelection(sim_handle_single,littleEndianIntConversion(((int*)(_pureData+0))[i],otherSideIsBigEndian));

		// 3. Copy and paste the selection:
		simCopyPasteSelectedObjects();

		// 4. Send back the handles of the new objects:
		int newSelSize=simGetObjectSelectionSize();
		int* newSelection=new int[newSelSize+1];
		simGetObjectSelection(newSelection+1);
		newSelection[0]=littleEndianIntConversion(newSelSize,otherSideIsBigEndian);
		for (int i=0;i<newSelSize;i++)
			newSelection[1+i]=littleEndianIntConversion(newSelection[1+i],otherSideIsBigEndian);
		retCmd->setDataReply_custom_transferBuffer((char*)newSelection,(newSelSize+1)*4,true);

		// 4. Restore previous selection state
		simRemoveObjectFromSelection(sim_handle_all,0);
		for (int i=0;i<initialSelSize;i++)
			simAddObjectToSelection(sim_handle_single,initialSelection[i]);
		delete[] initialSelection;
	}

	if (_rawCmdID==simx_cmd_get_object_selection)
	{
		int newSelSize=simGetObjectSelectionSize();
		int* newSelection=new int[newSelSize+1];
		simGetObjectSelection(newSelection+1);
		newSelection[0]=littleEndianIntConversion(newSelSize,otherSideIsBigEndian);
		for (int i=0;i<newSelSize;i++)
			newSelection[1+i]=littleEndianIntConversion(newSelection[1+i],otherSideIsBigEndian);
		retCmd->setDataReply_custom_transferBuffer((char*)newSelection,(newSelSize+1)*4,true);
	}

	if (_rawCmdID==simx_cmd_set_object_selection)
	{
		simRemoveObjectFromSelection(sim_handle_all,0);
		int cnt=_pureDataSize/4;
		for (int i=0;i<cnt;i++)
			simAddObjectToSelection(sim_handle_single,littleEndianIntConversion(((int*)(_pureData+0))[i],otherSideIsBigEndian));
		retCmd->setDataReply_nothing(true);
	}

	if (_rawCmdID==simx_cmd_clear_float_signal)
	{
		int res;
		if (_cmdString.length()==0)
			res=simClearFloatSignal(NULL);
		else
			res=simClearFloatSignal(_cmdString.c_str());
		retCmd->setDataReply_nothing(res!=-1);
	}

	if (_rawCmdID==simx_cmd_clear_integer_signal)
	{
		int res;
		if (_cmdString.length()==0)
			res=simClearIntegerSignal(NULL);
		else
			res=simClearIntegerSignal(_cmdString.c_str());
		retCmd->setDataReply_nothing(res!=-1);
	}

	if (_rawCmdID==simx_cmd_clear_string_signal)
	{
		int res;
		if (_cmdString.length()==0)
			res=simClearStringSignal(NULL);
		else
			res=simClearStringSignal(_cmdString.c_str());
		retCmd->setDataReply_nothing(res!=-1);
	}

	if (_rawCmdID==simx_cmd_get_float_signal)
	{
		float signalValue;
		int res=simGetFloatSignal(_cmdString.c_str(),&signalValue);
		retCmd->setDataReply_1float(signalValue,res>0,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_get_integer_signal)
	{
		int signalValue;
		int res=simGetIntegerSignal(_cmdString.c_str(),&signalValue);
		retCmd->setDataReply_1int(signalValue,res>0,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_get_string_signal)
	{
		int signalLength;
		char* signalValue=simGetStringSignal(_cmdString.c_str(),&signalLength);
		if (signalValue!=NULL)
		{
			retCmd->setDataReply_custom_copyBuffer(signalValue,signalLength,true);
			simReleaseBuffer(signalValue);
		}
		else
			retCmd->setDataReply_nothing(false);
	}

	if (_rawCmdID==simx_cmd_get_and_clear_string_signal)
	{
		int signalLength;
		char* signalValue=simGetStringSignal(_cmdString.c_str(),&signalLength);
		if (signalValue!=NULL)
		{
			retCmd->setDataReply_custom_copyBuffer(signalValue,signalLength,true);
			simReleaseBuffer(signalValue);
			simClearStringSignal(_cmdString.c_str());
			_opMode=simx_opmode_oneshot; // We need to remove this command! (this is a special case, only for this command. See the doc!)
		}
		else
			retCmd->setDataReply_nothing(false);
	}

	if (_rawCmdID==simx_cmd_read_string_stream)
	{
		int signalLength;
		char* signalValue=simGetStringSignal(_cmdString.c_str(),&signalLength);
		if (signalValue!=NULL)
		{
			retCmd->setDataReply_custom_copyBuffer(signalValue,signalLength,true);
			simReleaseBuffer(signalValue);
			simClearStringSignal(_cmdString.c_str());
		}
		else
			retCmd->setDataReply_nothing(true);
	}

	if (_rawCmdID==simx_cmd_set_float_signal)
	{
		float signalValue=littleEndianFloatConversion(((float*)(_pureData+0))[0],otherSideIsBigEndian);
		bool success=(simSetFloatSignal(_cmdString.c_str(),signalValue)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_set_integer_signal)
	{
		int signalValue=littleEndianIntConversion(((int*)(_pureData+0))[0],otherSideIsBigEndian);
		bool success=(simSetIntegerSignal(_cmdString.c_str(),signalValue)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_set_string_signal)
	{
		bool success=(simSetStringSignal(_cmdString.c_str(),_pureData,_pureDataSize)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_append_string_signal)
	{ // new since 31/1/2013
		std::string theNewString;
		int stringLength;
		char* stringSignal=simGetStringSignal(_cmdString.c_str(),&stringLength);
		if (stringSignal!=NULL)
		{
			theNewString=std::string(stringSignal,stringLength);
			simReleaseBuffer(stringSignal);
		}
		theNewString+=std::string(_pureData,_pureDataSize);
		bool success=(simSetStringSignal(_cmdString.c_str(),theNewString.c_str(),int(theNewString.length()))!=-1);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_get_object_float_parameter)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int paramID=littleEndianIntConversion(((int*)(_cmdData+0))[1],otherSideIsBigEndian);
		float param;
		bool success=(simGetObjectFloatParameter(handle,paramID,&param)>0);
		retCmd->setDataReply_1float(param,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_get_object_int_parameter)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int paramID=littleEndianIntConversion(((int*)(_cmdData+0))[1],otherSideIsBigEndian);
		int param;
		bool success=(simGetObjectIntParameter(handle,paramID,&param)>0);
		retCmd->setDataReply_1int(param,success,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_set_object_float_parameter)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int paramID=littleEndianIntConversion(((int*)(_cmdData+0))[1],otherSideIsBigEndian);
		float param=littleEndianFloatConversion(((float*)(_pureData+0))[0],otherSideIsBigEndian);
		bool success=(simSetObjectFloatParameter(handle,paramID,param)>0);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_set_object_int_parameter)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int paramID=littleEndianIntConversion(((int*)(_cmdData+0))[1],otherSideIsBigEndian);
		int param=littleEndianIntConversion(((int*)(_pureData+0))[0],otherSideIsBigEndian);
		bool success=(simSetObjectIntParameter(handle,paramID,param)>0);
		retCmd->setDataReply_nothing(success);
	}

	if (_rawCmdID==simx_cmd_get_model_property)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int prop=simGetModelProperty(handle);
		retCmd->setDataReply_1int(prop,prop!=-1,otherSideIsBigEndian);
	}

	if (_rawCmdID==simx_cmd_set_model_property)
	{
		int handle=littleEndianIntConversion(((int*)(_cmdData+0))[0],otherSideIsBigEndian);
		int prop=littleEndianIntConversion(((int*)(_pureData+0))[0],otherSideIsBigEndian);
		bool success=(simSetModelProperty(handle,prop)!=-1);
		retCmd->setDataReply_nothing(success);
	}

	// ******************************* error reporting **********************************
	char* err=simGetLastError(); // this also clears the last error
	if (err!=NULL)
	{
		sock->addErrorString(err);
		simReleaseBuffer(err);
	}
	return(retCmd);
}
