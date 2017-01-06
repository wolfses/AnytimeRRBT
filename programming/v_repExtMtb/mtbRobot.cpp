// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// -------------------------------------------------------------------
// THIS FILE IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// You are free to use/modify/distribute this file for whatever purpose!
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

//**************************************************
// This class represents a MTB robot inside this
// plugin. Each MTB model in V-REP
// should have its corresponding CMtbRobot object
// in this plugin. If a MTB model
// in V-REP is removed (i.e. erased), then its
// corresponding CMtbRobot object should also be
// removed. Same when a new MTB model is added
//**************************************************

#include "mtbRobot.h"
#include "v_repLib.h"
#include "Access.h"
#include "mtbGlobal.h"
#include <QString>
#include <QProcess>
#include <math.h>
#if defined (__linux) || defined (__APPLE__)
	#include <unistd.h>
#endif

#define DEFAULT_JOINT_VELOCITY (45.0f*piValue/180.0f)

CMtbRobot::CMtbRobot()
{
	_id=-1;
	_associatedObjectID=-1;
	_associatedObjectUniqueID=-1;
	_robotLanguageProgram="";
	_options=1; // bit 1= automatic execution
	_connectionAttemptPerformed=false;

	for (int i=0;i<4;i++)
	{
		jointPosition_localCopy[i]=0.0f;
		robotOutput_localCopy[i]=0;
		robotInput[i]=0;
	}

	_connection=NULL;
	_compiled=false;
}

CMtbRobot::~CMtbRobot()
{
	_disconnect();
}

void CMtbRobot::setID(int newID)
{
	_id=newID;
}

int CMtbRobot::getID()
{
	return(_id);
}

void CMtbRobot::setAssociatedObject(int objID,int objUniqueID)
{
	_associatedObjectID=objID;
	_associatedObjectUniqueID=objUniqueID;
}

int CMtbRobot::getAssociatedObject()
{
	return(_associatedObjectID);
}

int CMtbRobot::getAssociatedObjectUniqueId()
{
	return(_associatedObjectUniqueID);
}

void CMtbRobot::setProgram(const std::string& prg)
{
	_robotLanguageProgram=prg;
	attachCustomDataToSceneObject();
}

std::string CMtbRobot::getProgram()
{
	return(_robotLanguageProgram);
}

void CMtbRobot::setOptions(char e)
{
	_options=e;
	attachCustomDataToSceneObject();
}

char CMtbRobot::getOptions()
{
	return(_options);
}

void CMtbRobot::readCCustomDataFromSceneObject()
{ // We update CMtbRobot's data from its associated scene object custom data (for now only contains the robot language program):
	// 1. Get all the developer data attached to the associated scene object (this is custom data added by the developer):
	int buffSize=simGetObjectCustomDataLength(_associatedObjectID,DEVELOPER_DATA_HEADER);
	char* datBuff=new char[buffSize];
	simGetObjectCustomData(_associatedObjectID,DEVELOPER_DATA_HEADER,datBuff);
	std::vector<unsigned char> developerCustomData(datBuff,datBuff+buffSize);
	delete[] datBuff;
	// 2. From that retrieved data, try to extract sub-data with the MTB_PROGRAM and MTB_OPTIONS tag:
	std::vector<unsigned char> mtbData;
	_robotLanguageProgram.clear(); 
	if (CAccess::extractSerializationData(developerCustomData,MTB_PROGRAM,mtbData)) // extract the data stored under 'MTB_PROGRAM'-tag
	{ // the data was present!
		for (unsigned int i=0;i<mtbData.size();i++)
			_robotLanguageProgram.push_back(mtbData[i]);
	}
	if (CAccess::extractSerializationData(developerCustomData,MTB_OPTIONS,mtbData)) // extract the data stored under 'MTB_OPTIONS'-tag
	{ // the data was present!
		_options=mtbData[0];
	}
}

void CMtbRobot::attachCustomDataToSceneObject()
{ // We update CMtbRobot's associated scene object custom data (for now only contains the robot language program):
	// 1. Get all the developer data attached to the associated scene object (this is custom data added by the developer):
	int buffSize=simGetObjectCustomDataLength(_associatedObjectID,DEVELOPER_DATA_HEADER);
	char* datBuff=new char[buffSize];
	simGetObjectCustomData(_associatedObjectID,DEVELOPER_DATA_HEADER,datBuff);
	std::vector<unsigned char> developerCustomData(datBuff,datBuff+buffSize);
	delete[] datBuff;
	// 2. From that retrieved data, extract sub-data with the MTB_PROGRAM and MTB_OPTIONS tags, update them, and add them back to the retrieved data:
	std::vector<unsigned char> mtbData;

	CAccess::extractSerializationData(developerCustomData,MTB_PROGRAM,mtbData); // extract the data stored under 'MTB_PROGRAM'-tag
	mtbData.clear(); // we discard the old value (if present)
	// we replace it with the new value:
	for (unsigned int i=0;i<_robotLanguageProgram.length();i++)
		mtbData.push_back(_robotLanguageProgram[i]);
	if (mtbData.size()==0)
		mtbData.push_back(' '); // put an empty char if we don't have a prg yet
	CAccess::insertSerializationData(developerCustomData,MTB_PROGRAM,mtbData); // insert the new data under 'MTB_PROGRAM'-tag

	CAccess::extractSerializationData(developerCustomData,MTB_OPTIONS,mtbData); // extract the data stored under 'MTB_OPTIONS'-tag
	mtbData.clear(); // we discard the old value (if present)
	// we replace it with the new value:
	mtbData.push_back(_options);
	CAccess::insertSerializationData(developerCustomData,MTB_OPTIONS,mtbData); // insert the new data under 'MTB_OPTIONS'-tag

	// 3. We add/update the scene object with the updated custom data:
	simAddObjectCustomData(_associatedObjectID,DEVELOPER_DATA_HEADER,(simChar*)&developerCustomData[0],int(developerCustomData.size()));
}

bool CMtbRobot::_connect()
{
	if (!_connectionAttemptPerformed)
	{
		int portNb=20000;
		std::string argumentString;

		// Find a free port where we want to perform the connection:
		int portStart;
		int portRange;
		simGetIntegerParameter(sim_intparam_server_port_start,&portStart);
		simGetIntegerParameter(sim_intparam_server_port_range,&portRange);
		simGetIntegerParameter(sim_intparam_server_port_next,&portNb); // this is the port to use.

		// now update the next port to use:
		int nextPortNb=portNb+1;
		if (nextPortNb>=portStart+portRange)
			nextPortNb=portStart;
		simSetIntegerParameter(sim_intparam_server_port_next,nextPortNb);

		argumentString=QString::number(portNb).toStdString();

		// We have the port (portNb). Now launch the server application with appropriate argument:

		QString cmd("./");
		cmd+=QString::fromLocal8Bit("mtbServer");
		QStringList strList;
		strList << QString(argumentString.c_str());
		QProcess::startDetached(cmd,strList,QString::fromLocal8Bit(CAccess::currentDirAndPath.c_str()),NULL);


#ifdef _WIN32
		Sleep(500); // Leave enough time for the server to start
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
		usleep(500*1000); // Leave enough time for the server to start
#endif /* __linux || __APPLE__ */

		_connection=new COutConnection("127.0.0.1",portNb); // socket communication
		if (_connection->connectToServer()!=1)
		{ // we could not connect!
			delete _connection;
			_connection=NULL;
		}

		// We reset the inout buffer only here, since it is not really part of the robot
		for (int i=0;i<4;i++)
			robotInput[i]=0;
	}
	_connectionAttemptPerformed=true;
	return(_connection!=NULL);
}

void CMtbRobot::_disconnect()
{
	delete _connection;
	_connection=NULL;
}

void CMtbRobot::startOfSimulation()
{ // called at start of simulation
	removeOutputInputConnection(0); // clear all connections
	_connectionAttemptPerformed=false;
	_connect();
}

void CMtbRobot::endOfSimulation()
{ // called at end of simulation
	_disconnect();
	removeOutputInputConnection(0); // clear all connections
}

int CMtbRobot::reset(float jointsInitial[4],float jointVelocityInitial[2])
{
	_connect();
	_compiled=false;
	for (int i=0;i<4;i++)
	{
		jointPosition_localCopy[i]=jointsInitial[i];
		_initialJointPosition[i]=jointsInitial[i];
	}

	_initialJointVelocity[0]=jointVelocityInitial[0];
	_initialJointVelocity[1]=jointVelocityInitial[1];

	for (int i=0;i<4;i++)
		robotOutput_localCopy[i]=0;

	return(1);
}

int CMtbRobot::run(float timeStep,std::string& message)
{
	_connect();
	if (_connection==NULL)
		return(-1);
	if (!_compiled)
	{
		if (!_SEND_compile(_robotLanguageProgram,_initialJointPosition,_initialJointVelocity,message))
			return(-1); // connection error
		if (message.length()!=0)
			return(0); // Compilation error
		_compiled=true;
	}

	// The prog. is supposed to have been compiled here.
	// Run the interpreter for timeStep:
	if (!_SEND_run(robotInput,timeStep,message)) // this also automatically actualizes jointPosition_localCopy and robotOutput_localCopy
		return(-1); // connection error
	if (message.length()!=0)
		return(1); // Program executes normally
	else
		return(2); // program end!
}

bool CMtbRobot::readOutput(int data[4])
{
	_connect();
	if (_connection==NULL)
		return(false);
	for (int i=0;i<4;i++)
		data[i]=int(robotOutput_localCopy[i]);
	return(true);
}

bool CMtbRobot::readInput(int data[4])
{
	_connect();
	if (_connection==NULL)
		return(false);
	_updateInputsFromConnections();
	for (int i=0;i<4;i++)
		data[i]=int(robotInput[i]);
	return(true);
}

bool CMtbRobot::writeInput(int data[4])
{
	_connect();
	if (_connection==NULL)
		return(false);
	for (int i=0;i<4;i++)
		robotInput[i]=(unsigned char)data[i];
	_updateInputsFromConnections();
	return(true);
}

bool CMtbRobot::getJoints(float j[4])
{
	_connect();
	if (_connection==NULL)
		return(false);
	for (int i=0;i<4;i++)
		j[i]=jointPosition_localCopy[i];
	return(true);
}

bool CMtbRobot::_SEND_compile(const std::string& inputCode,float initialJointPosition[4],float initialJointVelocityWhenMoving[2],std::string& retString)
{
	if (_connection==NULL)
		return(false);
	std::vector<char> dataToSend;

	//Byte 1: command
	dataToSend.push_back(0); // command is "compile"
	//Byte 2-17: initial joint positions
	for (int i=0;i<4;i++)
	{
		for (int j=0;j<4;j++)
			dataToSend.push_back(((char*)initialJointPosition)[i*4+j]);
	}
	//Byte 18-25: initial joint velocity
	for (int i=0;i<2;i++)
	{
		for (int j=0;j<4;j++)
			dataToSend.push_back(((char*)initialJointVelocityWhenMoving)[i*4+j]);
	}
	//The rest: the program
	for (int j=0;j<int(inputCode.length());j++)
		dataToSend.push_back(inputCode[j]);

	// Now send the data to the server:
	if (_connection->sendData(&dataToSend[0],int(dataToSend.size())))
	{ // send was successful
		int dataSize;
		char* data=_connection->receiveReplyData(dataSize);
		if (dataSize>0)
		{ // data reception was ok!
			// The first byte is always the command that was sent
			retString.clear();
			for (int i=1;i<dataSize;i++)
				retString+=data[i];
			delete[] data;
			return(true);
		}
		else
		{ // error while receiving
			_disconnect();
			return(false);
		}
	}
	else
	{ // error while sending
		_disconnect();
		return(false);
	}
}

bool CMtbRobot::_SEND_run(unsigned char inputData[4],float deltaTime,std::string& retString)
{
	if (_connection==NULL)
		return(false);
	std::vector<char> dataToSend;

	//Byte 1: command
	dataToSend.push_back(1); // command is "run"
	//Byte 2-5: inputData
	for (int i=0;i<4;i++)
		dataToSend.push_back(inputData[i]);
	//Byte 6-9: deltaTime
	for (int i=0;i<4;i++)
		dataToSend.push_back(((char*)&deltaTime)[i]);

	// Now send the data to the server:
	if (_connection->sendData(&dataToSend[0],int(dataToSend.size())))
	{ // send was successful
		int dataSize;
		char* data=_connection->receiveReplyData(dataSize);
		if (dataSize>0)
		{ // data reception was ok!
			// The first byte is always the command that was sent
			// Byte 2-17: joint positions
			for (int i=0;i<4;i++)
				jointPosition_localCopy[i]=((float*)(data+1))[i];
			// Byte 18-21: outputs
			for (int i=0;i<4;i++)
				robotOutput_localCopy[i]=data[17+i];
			// The rest is the return string:
			retString.clear();
			for (int i=21;i<dataSize;i++)
				retString+=data[i];
			delete[] data;
			return(true);
		}
		else
		{ // error while receiving
			_disconnect();
			return(false);
		}
	}
	else
	{ // error while sending
		_disconnect();
		return(false);
	}
}

bool CMtbRobot::addOutputInputConnection(int inputBit,int outputRobotHandle,int outputBit,int connectionType)
{
	//1. Check first parameters:
	if ( (inputBit<1)||(inputBit>32)||(outputBit<1)||(outputBit>32)||(connectionType<0)||(connectionType>1) )
		return(false); // error
	//2. Does the other robot exist?
	CMtbRobot* otherRobot=CAccess::mtbRobotContainer->getFromAssociatedObject(outputRobotHandle);
	if (otherRobot==NULL)
		return(false); // error
	//3. Remove any existing connection to that input:
	removeOutputInputConnection(inputBit);
	//4. Add the connection:
	_inputOutputConnections.push_back(inputBit);
	_inputOutputConnections.push_back(outputRobotHandle);
	_inputOutputConnections.push_back(outputBit);
	_inputOutputConnections.push_back(connectionType);
	return(true);
}

void CMtbRobot::removeOutputInputConnection(int inputBit)
{
	if (inputBit==0)
		_inputOutputConnections.clear(); // reset
	else
	{
		for (unsigned int i=0;i<_inputOutputConnections.size()/4;i++)
		{
			if (_inputOutputConnections[4*i+0]==inputBit)
			{
				_inputOutputConnections.erase(_inputOutputConnections.begin()+4*i+0,_inputOutputConnections.begin()+4*i+4);
				break;
			}
		}
	}
}

void CMtbRobot::_updateInputsFromConnections()
{
	for (unsigned int i=0;i<_inputOutputConnections.size()/4;i++)
	{
		CMtbRobot* otherRobot=CAccess::mtbRobotContainer->getFromAssociatedObject(_inputOutputConnections[4*i+1]);
		if (otherRobot!=NULL)
		{
			int tmp[4];
			unsigned char otherRobotOutputData[4];
			if (!otherRobot->readOutput(tmp))
			{ // We couldn't read it somehow (not yet connected?). We put everything to zero:
				for (int j=0;j<4;j++)
					otherRobotOutputData[j]=0;
			}
			else
			{
				for (int j=0;j<4;j++)
					otherRobotOutputData[j]=int(tmp[j]);
			}

			int inBytePos=(_inputOutputConnections[4*i+0]-1)/8; // 0-3
			int inBitPos=(_inputOutputConnections[4*i+0]-1)%8; //0-7

			int outBytePos=(_inputOutputConnections[4*i+2]-1)/8; // 0-3
			int outBitPos=(_inputOutputConnections[4*i+2]-1)%8; //0-7

			bool setInputBit=false;
			if (otherRobotOutputData[outBytePos]&(1<<outBitPos))
				setInputBit=((1-_inputOutputConnections[4*i+3])!=0);
			else
				setInputBit=(_inputOutputConnections[4*i+3]!=0);

			if (setInputBit)
				robotInput[inBytePos]|=(1<<inBitPos);
			else
				robotInput[inBytePos]&=255-(1<<inBitPos);
		}
		else
		{ // the other robot was removed. We have to remove this output-input connection:
			_inputOutputConnections.erase(_inputOutputConnections.begin()+4*i+0,_inputOutputConnections.begin()+4*i+4);
			i--; // we have to reprocess this position in the for-loop!
		}
	}
}
