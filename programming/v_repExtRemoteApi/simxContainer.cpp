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

#include "simxContainer.h"
#include "simxUtils.h"
#include "v_repLib.h"

CSimxContainer::CSimxContainer(bool isInputContainer)
{
	_isInputContainer=isInputContainer;
	_messageID=-1;
	_dataTimeStamp=0;
	_dataServerTime=0;
}

CSimxContainer::~CSimxContainer()
{
	clearAll();
}

int CSimxContainer::getMessageID()
{
	return(_messageID);
}

void CSimxContainer::setMessageID(int id)
{
	_messageID=id;
}

void CSimxContainer::setDataTimeStamp(int ts)
{
	_dataTimeStamp=ts;
}

void CSimxContainer::setDataServerTimeStamp(int st)
{
	_dataServerTime=st;
}

void CSimxContainer::setSceneID(WORD id)
{
	_sceneID=id;
}

void CSimxContainer::setServerState(BYTE serverState)
{
	_serverState=serverState;
}


void CSimxContainer::clearAll()
{
	for (unsigned int i=0;i<_allCommands.size();i++)
		delete _allCommands[i];
	_allCommands.clear();
	for (unsigned int i=0;i<_partialCommands.size();i++)
		delete[] _partialCommands[i];
	_partialCommands.clear();

	_messageID=-1;
}

int CSimxContainer::_arePartialCommandsSame(const char* buff1,const char* buff2,bool otherSideIsBigEndian)
{ // ret value: 0=not same, 1=same cmds, buff1 has same size than buff2, 2=buffers have different sizes
	int cmd1=littleEndianIntConversion(((int*)(buff1+simx_cmdheaderoffset_cmd))[0],otherSideIsBigEndian)&simx_cmdmask;
	int cmd2=littleEndianIntConversion(((int*)(buff2+simx_cmdheaderoffset_cmd))[0],otherSideIsBigEndian)&simx_cmdmask;
	if (cmd1==cmd2)
	{ // commands are same. Check the command data now:
		if ((cmd1>simx_cmd4bytes_start)&&(cmd1<simx_cmd8bytes_start))
		{
			if ( ((int*)(buff1+SIMX_SUBHEADER_SIZE))[0]!=((int*)(buff2+SIMX_SUBHEADER_SIZE))[0] )
				return(0);
		}
		if ((cmd1>simx_cmd8bytes_start)&&(cmd1<simx_cmd1string_start))
		{
			if ( ((int*)(buff1+SIMX_SUBHEADER_SIZE))[0]!=((int*)(buff2+SIMX_SUBHEADER_SIZE))[0] )
				return(0);
			if ( ((int*)(buff1+SIMX_SUBHEADER_SIZE+4))[0]!=((int*)(buff2+SIMX_SUBHEADER_SIZE+4))[0] )
				return(0);
		}
		if ((cmd1>simx_cmd1string_start)&&(cmd1<simx_cmdreserved_start))
		{
			if (strcmp(buff1+SIMX_SUBHEADER_SIZE,buff2+SIMX_SUBHEADER_SIZE)!=0)
				return(0);
		}
		int size1=littleEndianIntConversion(((int*)(buff1+simx_cmdheaderoffset_full_mem_size))[0],otherSideIsBigEndian);
		int size2=littleEndianIntConversion(((int*)(buff2+simx_cmdheaderoffset_full_mem_size))[0],otherSideIsBigEndian);
		if (size1==size2)
			return(1);
		return(2);
	}
	return(0);
}

char* CSimxContainer::addPartialCommand(const char* buffer,bool otherSideIsBigEndian)
{
	for (unsigned int i=0;i<_partialCommands.size();i++)
	{
		int res=_arePartialCommandsSame(buffer,_partialCommands[i],otherSideIsBigEndian);
		if (res==2)
		{ // same commands, but somehow different memory footprints. Erase the one we have stored here and break from the loop
			delete[] _partialCommands[i];
			_partialCommands.erase(_partialCommands.begin()+i);
			break;
		}
		else
		{
			if (res==1)
			{ // same commands and same footprints. Merge the new command with the old:
				int fullSize=littleEndianIntConversion(((int*)(buffer+simx_cmdheaderoffset_full_mem_size))[0],otherSideIsBigEndian);
				int memSize=littleEndianIntConversion(((int*)(buffer+simx_cmdheaderoffset_mem_size))[0],otherSideIsBigEndian);
				WORD pdataOffset0=littleEndianWordConversion(((WORD*)(buffer+simx_cmdheaderoffset_pdata_offset0))[0],otherSideIsBigEndian);
				int pdataOffset1=littleEndianIntConversion(((int*)(buffer+simx_cmdheaderoffset_pdata_offset1))[0],otherSideIsBigEndian);
				int pdataSize=memSize-SIMX_SUBHEADER_SIZE-pdataOffset0;
				for (int cnt=0;cnt<pdataSize;cnt++)
					_partialCommands[i][SIMX_SUBHEADER_SIZE+pdataOffset0+pdataOffset1+cnt]=buffer[SIMX_SUBHEADER_SIZE+pdataOffset0+cnt];
				// Is that merged command complete yet?
				if (SIMX_SUBHEADER_SIZE+pdataOffset0+pdataOffset1+pdataSize>=fullSize)
				{ // yes!
					char* retVal=_partialCommands[i];
					_partialCommands.erase(_partialCommands.begin()+i);
					return(retVal);
				}
				else
					return(NULL);
			}
		}
	}
	// the partial command is not yet present! We simply add it:
	int memSize=littleEndianIntConversion(((int*)(buffer+simx_cmdheaderoffset_mem_size))[0],otherSideIsBigEndian);
	int fullSize=littleEndianIntConversion(((int*)(buffer+simx_cmdheaderoffset_full_mem_size))[0],otherSideIsBigEndian);
	char* dat=new char[fullSize];
	WORD pdataOffset0=littleEndianWordConversion(((WORD*)(buffer+simx_cmdheaderoffset_pdata_offset0))[0],otherSideIsBigEndian);
	int pdataOffset1=littleEndianIntConversion(((int*)(buffer+simx_cmdheaderoffset_pdata_offset1))[0],otherSideIsBigEndian);
	int pdataSize=memSize-SIMX_SUBHEADER_SIZE-pdataOffset0;
	for (int cnt=0;cnt<SIMX_SUBHEADER_SIZE+pdataOffset0;cnt++)
		dat[cnt]=buffer[cnt];
	// We need to correct for the correct memory footprint here:
	((int*)(dat+simx_cmdheaderoffset_mem_size))[0]=littleEndianIntConversion(fullSize,otherSideIsBigEndian);
	for (int cnt=0;cnt<pdataSize;cnt++)
		dat[SIMX_SUBHEADER_SIZE+pdataOffset0+pdataOffset1+cnt]=buffer[SIMX_SUBHEADER_SIZE+pdataOffset0+cnt];
	_partialCommands.push_back(dat);
	return(NULL);
}

void CSimxContainer::addCommand(CSimxCmd* cmd,bool doNotOverwriteSameCommand)
{
	if (_isInputContainer)
	{
		int sci=_getIndexOfSimilarCommand(cmd);
		if ((sci==-1)||doNotOverwriteSameCommand)
			_allCommands.push_back(cmd); // command not yet there. just append it
		else
		{ // Command already there
			if ((cmd->getOperationMode()==simx_opmode_oneshot_split)&&(_allCommands[sci]->getOperationMode()==simx_opmode_oneshot_split))
			{ // let the command finish, and only in above case (don't do this with continuous commands!!). Special case!
				delete cmd;
			}
			else
			{
				delete _allCommands[sci]; // we remove the older command
				_allCommands[sci]=cmd; // we replace the old command with the new. Discontinue commands have to be relayed back to the client (which might have a continuous command to clear there too)
			}
		}
	}
	else
		_allCommands.push_back(cmd);
}

int CSimxContainer::_getIndexOfSimilarCommand(CSimxCmd* cmd)
{
	for (unsigned int i=0;i<_allCommands.size();i++)
	{
		if (_allCommands[i]->areCommandAndCommandDataSame(cmd))
			return((int)i);
	}
	return(-1);
}

void CSimxContainer::executeCommands(CSimxContainer* outputContainer,CSimxSocket* sock)
{
	if (outputContainer->getCommandCount()==0)
	{ // Make sure the output buffer was already sent and is empty
		outputContainer->setMessageID(_messageID);
		outputContainer->setDataTimeStamp(_dataTimeStamp);
		int simState=simGetSimulationState();
		BYTE serverState=0;
		outputContainer->setDataServerTimeStamp(int(simGetSystemTime()*1000.1f));
		if (simState!=sim_simulation_stopped)
		{
			serverState|=1; // simulation is not stopped
			if (simState==sim_simulation_paused)
				serverState|=2; // simulation is paused
		}
		if (simGetRealTimeSimulation()>0)
			serverState|=4;
		int editModeType;
		simGetIntegerParameter(sim_intparam_edit_mode_type,&editModeType);
		serverState|=(editModeType<<3);
		int sceneUniqueID;
		simGetIntegerParameter(sim_intparam_scene_unique_id,&sceneUniqueID);
		outputContainer->setSceneID(WORD(sceneUniqueID));
		outputContainer->setServerState(serverState);

		// Prepare for correct error reporting:
		int errorModeSaved;
		simGetIntegerParameter(sim_intparam_error_report_mode,&errorModeSaved);
		simSetIntegerParameter(sim_intparam_error_report_mode,sim_api_errormessage_report);
		char* err=simGetLastError(); // just clear the last error
		if (err!=NULL)
			simReleaseBuffer(err);

		// Execute pending commands:
		for (unsigned int i=0;i<_allCommands.size();i++)
		{
			CSimxCmd* outputCmd=_allCommands[i]->execute(sock,_otherSideIsBigEndian);
			if (outputCmd!=NULL) // gradual or split commands are not added here!
				outputContainer->addCommand(outputCmd,false);
		}

		// Restore previous error report mode:
		simSetIntegerParameter(sim_intparam_error_report_mode,errorModeSaved); 

		_removeNonContinuousCommands(); // simx_opmode_oneshot_split commands are also kept!
	}
}

void CSimxContainer::_removeNonContinuousCommands()
{ // this also keeps simx_opmode_oneshot_split commands!!!
	for (unsigned int i=0;i<_allCommands.size();i++)
	{
		if ( (_allCommands[i]->getOperationMode()!=simx_opmode_continuous)&&(_allCommands[i]->getOperationMode()!=simx_opmode_continuous_split)&&(_allCommands[i]->getOperationMode()!=simx_opmode_oneshot_split) )
		{
			delete _allCommands[i];
			_allCommands.erase(_allCommands.begin()+i);
			i--; // reprocess this position
		}
	}
}

int CSimxContainer::getStreamCommandCount()
{
	int retVal=0;
	if (_isInputContainer)
		return(0); // apply only on output containers
	for (unsigned int i=0;i<_allCommands.size();i++)
	{
		if (_allCommands[i]->getOperationMode()==simx_opmode_streaming)
			retVal++;
	}
	return(retVal);
}


int CSimxContainer::getDataString(std::vector<char>& dataString,bool otherSideIsBigEndian)
{ // returns the number of commands fetched
	if (_isInputContainer)
		return(0); // apply only on output containers
	dataString.clear();
	// 1. Prepare the header:
	char header[SIMX_HEADER_SIZE];
	// The CRC is added later
	header[simx_headeroffset_version]=SIMX_VERSION;
	((int*)(header+simx_headeroffset_message_id))[0]=littleEndianIntConversion(_messageID,otherSideIsBigEndian);
	((int*)(header+simx_headeroffset_client_time))[0]=littleEndianIntConversion(_dataTimeStamp,otherSideIsBigEndian);
	((int*)(header+simx_headeroffset_server_time))[0]=littleEndianIntConversion(_dataServerTime,otherSideIsBigEndian);
	((WORD*)(header+simx_headeroffset_scene_id))[0]=littleEndianWordConversion(_sceneID,otherSideIsBigEndian);
	header[simx_headeroffset_server_state]=_serverState;

	for (int i=0;i<SIMX_HEADER_SIZE;i++)
		dataString.push_back(header[i]);
	// 2. Prepare the individual commands (or command replies). Only non-split and non-gradual commands are taken here:
	for (unsigned int i=0;i<_allCommands.size();i++)
		_allCommands[i]->appendYourData(dataString,_otherSideIsBigEndian);
	return(int(_allCommands.size()));
}

int CSimxContainer::getDataStringOfSplitOrGradualCommands(std::vector<char>& dataString,bool otherSideIsBigEndian)
{ // returns the number of commands fetched
	if (!_isInputContainer)
		return(0); // apply only on input containers

	// Take care only of split or gradual commands:
	int fetchedCnt=0;
	for (unsigned int i=0;i<_allCommands.size();i++)
	{
		bool removeCommand=false;
		if (_allCommands[i]->appendYourMemorizedSplitData(true,dataString,removeCommand,_otherSideIsBigEndian))
			fetchedCnt++;
		if (removeCommand)
		{
			delete _allCommands[i];
			_allCommands.erase(_allCommands.begin()+i);
			i--; // reprocess this position
		}
	}
	return(fetchedCnt);
}


int CSimxContainer::getCommandCount()
{
	return((int)_allCommands.size());
}

void CSimxContainer::setOtherSideIsBigEndian(bool bigEndian)
{
	_otherSideIsBigEndian=bigEndian;
}
