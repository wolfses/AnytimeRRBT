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

#include "robotLanguageInterpreter.h"
#include "simpleInConnection.h"
#include <stdio.h>
#include <cstdlib>


int main(int argc, char* argv[])
{
	int portNb=20000;
	if (argv[1]!=NULL)
		portNb=atoi(argv[1]);
	printf("Trying socket communication on port %i\n",portNb);
	CSimpleInConnection connection(portNb);
	printf("Connecting to client...\n");
	if (connection.connectToClient())
	{
		printf("Connected to client.\n");
		while (true)
		{
			int dataSize;
			char* data=connection.receiveData(dataSize);
			if (dataSize>0)
			{ // We received data. The server ALWAYS replies!
				printf("."); // Indicate when data is received
				std::vector<char> returnData;
				unsigned char command=data[0];
				if (command==0)
				{ // compile command
					// Read the arguments
					float initJoints[4];
					initJoints[0]=((float*)(data+1))[0];
					initJoints[1]=((float*)(data+1))[1];
					initJoints[2]=((float*)(data+1))[2];
					initJoints[3]=((float*)(data+1))[3];
					float initVelocity[2];
					initVelocity[0]=((float*)(data+1))[4];
					initVelocity[1]=((float*)(data+1))[5];
					std::string prg;
					for (int i=25;i<dataSize;i++)
						prg+=data[i];
					// Now execute the command:
					std::string message(compileCode(prg,initJoints,initVelocity));
					// Now prepare the return data:
					returnData.push_back(command);
					for (int i=0;i<int(message.length());i++)
						returnData.push_back(message[i]);
				}
				if (command==1)
				{ // run command
					// Read the arguments
					unsigned char inputData[4];
					inputData[0]=data[1];
					inputData[1]=data[2];
					inputData[2]=data[3];
					inputData[3]=data[4];
					float deltaTime=((float*)(data+5))[0];				
					// Now execute the command:
					std::string message(runProgram(inputData,deltaTime));
					// And read the return values:
					float jointPosition[4];
					unsigned char outputData[4];
					getJointsAndOutputData(jointPosition,outputData);
					// Now prepare the return data:
					returnData.push_back(command);
					for (int i=0;i<4;i++)
					{
						for (int j=0;j<4;j++)
							returnData.push_back(((char*)jointPosition)[i*4+j]);
					}
					for (int i=0;i<4;i++)
						returnData.push_back(outputData[i]);
					for (int i=0;i<int(message.length());i++)
						returnData.push_back(message[i]);
				}
				delete[] data;
				if (!connection.replyToReceivedData(&returnData[0],int(returnData.size())))
				{
					printf("Failed to send reply.\n");
					break;
				}
			}
			if (dataSize==-1)
				break; // error
		}
	}
	else
		printf("Failed connecting to client.\n");
	Sleep(5000);
	return 0;
}

