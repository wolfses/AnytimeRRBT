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

/*****************************************************
 This file represents the robot language interpreter
 which is only provided as a very very simple
 example. Following commands/instructions are
 recognized (one command per line):
 ------------------------------------
 REM this is a comment section
 SETLINVEL v
 SETROTVEL v
 MOVE p1 p2 p3 p4
 WAIT x
 SETBIT y
 CLEARBIT y
 IFBITGOTO y label
 IFNBITGOTO y label
 GOTO label
 label
 ------------------------------------
 SETLINVEL: sets the linear joint velocities for next movement to v (m/sec)
 SETROTVEL: sets the angular joint velocities for next movement to v (deg/sec)
 MOVE: moves the 4 robot joints to positions (p1 (deg), p2 (deg), p3 (m), p4 (deg)) (at constant velocity v)
 WAIT: waits x miliseconds
 SETBIT: set the bit at position y in the robot output (1-32)
 CLEARBIT: clears the bit at position y in the robot output (1-32)
 IFBITGOTO: if bit y in the robot input (1-32) is set, then jump to label
 IFNBITGOTO: if bit y in the robot input (1-32) is NOT set, then jump to label
 GOTO: jumps to label. A label is any string different from REM, SETVEL, MOVE, WAIT or GOTO
**************************************************/

#include "robotLanguageInterpreter.h"
#include <boost/lexical_cast.hpp>
#include <math.h>

const float piValue=3.14159265f;
SMtbRobotState robotState;
std::vector<SCompiledProgramLine> _compiledRobotLanguageProgram;

bool _extractOneLine(std::string& inputString,std::string& extractedLine)
{ // Extracts one line from a multi-line string
	extractedLine="";
	while (inputString.length()!=0)
	{
		char c=inputString[0];
		inputString.erase(inputString.begin());
		if ( (c==char(13))||(c==char(10)) )
		{
			if (c==char(10))
				break;
		}
		else
			extractedLine+=c;
	}
	return(extractedLine.length()!=0);
}

bool _extractOneWord(std::string& line,std::string& extractedWord)
{ // Extracts one word from a string (words are delimited by spaces)
	extractedWord="";
	while (line.length()!=0)
	{
		char c=line[0];
		line.erase(line.begin());
		if (c==' ')
		{
			if (extractedWord.length()!=0)
				return(extractedWord.length()!=0);
		}
		else
			extractedWord+=c;
	}
	return(extractedWord.length()!=0);
}

bool _getCommandFromWord(const std::string& word,int& command)
{ // returns the command index for a string command
	command=-1; // label
	if (word.compare("MOVE")==0)
		command=0;
	if (word.compare("WAIT")==0)
		command=1;
	if (word.compare("GOTO")==0)
		command=2;
	if (word.compare("SETROTVEL")==0)
		command=3;
	if (word.compare("REM")==0)
		command=4;
	if (word.compare("SETBIT")==0)
		command=5;
	if (word.compare("CLEARBIT")==0)
		command=6;
	if (word.compare("IFBITGOTO")==0)
		command=7;
	if (word.compare("IFNBITGOTO")==0)
		command=8;
	if (word.compare("SETLINVEL")==0)
		command=9;
	return(command!=-1);
}

bool _getIntegerFromWord(const std::string& word,int& theInteger)
{ // returns the integer value corresponding to a string
	try
	{
		theInteger=boost::lexical_cast<int>(word);
		return(true);
	}
	catch (boost::bad_lexical_cast &)
	{
		return(false);
	}
}

bool _getFloatFromWord(const std::string& word,float& theFloat)
{ // returns the float value corresponding to a string
	try
	{
		theFloat=boost::lexical_cast<float>(word);
		return(true);
	}
	catch (boost::bad_lexical_cast &)
	{
		return(false);
	}
}

void _removeFrontAndBackSpaces(std::string& word)
{ // removes spaces at the front and at the back of a string
	while ( (word.length()!=0)&&(word[0]==' ') )
		word.erase(word.begin());
	while ( (word.length()!=0)&&(word[word.length()-1]==' ') )
		word.erase(word.begin()+word.length()-1);
}

std::string compileCode(const std::string& inputCode,float initialJointPosition[4],float initialJointVelocityWhenMoving[2])
{	// This function "compiles" the robot language code
	// A return value different from "" indicates a compilation error

	for (int i=0;i<4;i++)
		robotState.currentJointPosition[i]=initialJointPosition[i];
	robotState.jointVelocityWhenMoving[0]=initialJointVelocityWhenMoving[0];
	robotState.jointVelocityWhenMoving[1]=initialJointVelocityWhenMoving[1];
	robotState.currentProgramLine=0;

	std::vector<std::string> labels;
	std::vector<int> labelLocations;
	_compiledRobotLanguageProgram.clear();
	std::string code(inputCode);
	int errorOnLineNumber=-1; // no error for now
	std::string codeLine;
	int currentCodeLineNb=0;
	while (_extractOneLine(code,codeLine))
	{ //  get one code line
		currentCodeLineNb++;
		_removeFrontAndBackSpaces(codeLine);
		if (codeLine.length()!=0)
		{
			std::string originalLine(codeLine);
			std::string codeWord;
			if (_extractOneWord(codeLine,codeWord))
			{ // get the first word in the code line
				int cmd;
				_getCommandFromWord(codeWord,cmd); // get the equivalent command index for the word
				if (cmd==-1)
				{ // we have a label here
					_removeFrontAndBackSpaces(codeLine);
					if (codeLine.length()==0)
					{ // the line is ok.
						labels.push_back(codeWord);
						labelLocations.push_back(int(_compiledRobotLanguageProgram.size()));
					}
					else
					{ // there shouldn't be more text on this line!
						errorOnLineNumber=currentCodeLineNb;
						break;
					}
				}
				if (cmd==0)
				{ // we have a MOVE here
					bool error=true;
					if (_extractOneWord(codeLine,codeWord))
					{
						float p1;
						if (_getFloatFromWord(codeWord,p1))
						{
							if (_extractOneWord(codeLine,codeWord))
							{
								float p2;
								if (_getFloatFromWord(codeWord,p2))
								{
									if (_extractOneWord(codeLine,codeWord))
									{
										float p3;
										if (_getFloatFromWord(codeWord,p3))
										{
											if (_extractOneWord(codeLine,codeWord))
											{
												float p4;
												if (_getFloatFromWord(codeWord,p4))
												{
													if (p1>160.0f)
														p1=160.0f;
													if (p1<-160.0f)
														p1=-160.0f;
													if (p2>160.0f)
														p2=160.0f;
													if (p2<-160.0f)
														p2=-160.0f;
													if (p3>0.2f)
														p3=0.2f;
													if (p3<0.0f)
														p3=0.0f;
													if (!_extractOneWord(codeLine,codeWord))
													{
														error=false;
														SCompiledProgramLine a;
														a.command=cmd;
														a.correspondingUncompiledCode=originalLine;
														a.floatParameter[0]=p1*piValue/180.0f; // convert from deg to rad
														a.floatParameter[1]=p2*piValue/180.0f; // convert from deg to rad
														a.floatParameter[2]=p3; // prismatic actuator
														a.floatParameter[3]=p4*piValue/180.0f; // convert from deg to rad
														_compiledRobotLanguageProgram.push_back(a);
													}
												}
											}
										}
									}
								}
							}
						}
					}
					if (error)
					{
						errorOnLineNumber=currentCodeLineNb;
						break;
					}
				}
				if (cmd==1)
				{ // we have a WAIT here
					bool error=true;
					if (_extractOneWord(codeLine,codeWord))
					{
						int t;
						if (_getIntegerFromWord(codeWord,t))
						{
							if (t>0)
							{
								if (!_extractOneWord(codeLine,codeWord))
								{
									error=false;
									SCompiledProgramLine a;
									a.command=cmd;
									a.correspondingUncompiledCode=originalLine;
									a.floatParameter[0]=float(t)/1000.0f; // convert from ms to s
									_compiledRobotLanguageProgram.push_back(a);
								}
							}
						}
					}
					if (error)
					{
						errorOnLineNumber=currentCodeLineNb;
						break;
					}
				}
				if (cmd==2)
				{ // we have a GOTO here
					bool error=true;
					if (_extractOneWord(codeLine,codeWord))
					{
						std::string label(codeWord);
						if (!_extractOneWord(codeLine,codeWord))
						{
							error=false;
							SCompiledProgramLine a;
							a.command=cmd;
							a.correspondingUncompiledCode=originalLine;
							a.tmpLabel=label;
							a.intParameter[0]=currentCodeLineNb; // the line number to jumo to is set in the second compilation pass
							_compiledRobotLanguageProgram.push_back(a); 
						}
					}
					if (error)
					{
						errorOnLineNumber=currentCodeLineNb;
						break;
					}
				}
				if (cmd==3)
				{ // we have a SETROTVEL here
					bool error=true;
					if (_extractOneWord(codeLine,codeWord))
					{
						float v;
						if (_getFloatFromWord(codeWord,v))
						{
							if (v>0.0001f)
							{
								if (!_extractOneWord(codeLine,codeWord))
								{
									error=false;
									SCompiledProgramLine a;
									a.command=cmd;
									a.correspondingUncompiledCode=originalLine;
									a.floatParameter[0]=v*piValue/180.0f; // convert from deg/s to rad/s
									_compiledRobotLanguageProgram.push_back(a);
								}
							}
						}
					}
					if (error)
					{
						errorOnLineNumber=currentCodeLineNb;
						break;
					}
				}
				if (cmd==9)
				{ // we have a SETLINVEL here
					bool error=true;
					if (_extractOneWord(codeLine,codeWord))
					{
						float v;
						if (_getFloatFromWord(codeWord,v))
						{
							if (v>0.0001f)
							{
								if (!_extractOneWord(codeLine,codeWord))
								{
									error=false;
									SCompiledProgramLine a;
									a.command=cmd;
									a.correspondingUncompiledCode=originalLine;
									a.floatParameter[0]=v;
									_compiledRobotLanguageProgram.push_back(a);
								}
							}
						}
					}
					if (error)
					{
						errorOnLineNumber=currentCodeLineNb;
						break;
					}
				}
				if (cmd==4)
				{ // we have a comment here
				}
				if ((cmd==5)||(cmd==6))
				{ // we have a SETBIT or CLEARBIT here
					bool error=true;
					if (_extractOneWord(codeLine,codeWord))
					{
						int bitPos;
						if (_getIntegerFromWord(codeWord,bitPos))
						{
							if ((bitPos>0)&&(bitPos<33))
							{
								if (!_extractOneWord(codeLine,codeWord))
								{
									error=false;
									SCompiledProgramLine a;
									a.command=cmd;
									a.correspondingUncompiledCode=originalLine;
									a.intParameter[0]=bitPos; 
									_compiledRobotLanguageProgram.push_back(a);
								}
							}
						}
					}
					if (error)
					{
						errorOnLineNumber=currentCodeLineNb;
						break;
					}
				}
				if ((cmd==7)||(cmd==8))
				{ // we have a IFBITGOTO or IFNBITGOTO here
					bool error=true;
					if (_extractOneWord(codeLine,codeWord))
					{
						int bitPos;
						if (_getIntegerFromWord(codeWord,bitPos))
						{
							if ((bitPos>0)&&(bitPos<33))
							{
								if (_extractOneWord(codeLine,codeWord))
								{
									std::string label=codeWord;
									if (!_extractOneWord(codeLine,codeWord))
									{
										error=false;
										SCompiledProgramLine a;
										a.command=cmd;
										a.correspondingUncompiledCode=originalLine;
										a.tmpLabel=label;
										a.intParameter[0]=bitPos;
										a.intParameter[1]=currentCodeLineNb; // the line number to jumo to is set in the second compilation pass
										_compiledRobotLanguageProgram.push_back(a);
									}
								}
							}
						}
					}
					if (error)
					{
						errorOnLineNumber=currentCodeLineNb;
						break;
					}
				}
			}
		}
	}
	if (errorOnLineNumber!=-1)
	{ // we return an error message
		std::string retString("Error on line ");
		retString+=boost::lexical_cast<std::string>(currentCodeLineNb);
		return(retString);
	}
	else
	{ // we have a second pass where we need to set the line number where to jump to!
		for (unsigned int i=0;i<_compiledRobotLanguageProgram.size();i++)
		{
			if (_compiledRobotLanguageProgram[i].command==2)
			{ // this is a GOTO command.
				bool found=false;
				for (unsigned int j=0;j<labels.size();j++)
				{
					if (labels[j].compare(_compiledRobotLanguageProgram[i].tmpLabel)==0)
					{
						_compiledRobotLanguageProgram[i].intParameter[0]=labelLocations[j];
						found=true;
					}
				}
				if (!found)
				{
					std::string retString("Error on line ");
					retString+=boost::lexical_cast<std::string>(_compiledRobotLanguageProgram[i].intParameter[0]);
					return(retString);
				}
			}
			if ((_compiledRobotLanguageProgram[i].command==7)||(_compiledRobotLanguageProgram[i].command==8))
			{ // this is a IFBITGOTO or IFNBITGOTO command.
				bool found=false;
				for (unsigned int j=0;j<labels.size();j++)
				{
					if (labels[j].compare(_compiledRobotLanguageProgram[i].tmpLabel)==0)
					{
						_compiledRobotLanguageProgram[i].intParameter[1]=labelLocations[j];
						found=true;
					}
				}
				if (!found)
				{
					std::string retString("Error on line ");
					retString+=boost::lexical_cast<std::string>(_compiledRobotLanguageProgram[i].intParameter[1]);
					return(retString);
				}
			}
		}
		return(""); // no error!
	}
}

std::string runProgram(unsigned char inputData[4],float deltaTime)
{	// This function runs the compiled robot language program for "deltaTime" seconds
	// return value "" means program ended!
	if (int(_compiledRobotLanguageProgram.size())<=robotState.currentProgramLine)
		return(""); // program end
	int cmd=_compiledRobotLanguageProgram[robotState.currentProgramLine].command; // get the current command
	int loopCnt=0;
	while (true)
	{
		if (cmd==0)
		{ // MOVE 
			// We arrive at the same time at the desired positions for each joint
			float deltaTimeLeft;

			float biggestT=0.0f;
			for (int i=0;i<4;i++)
			{
				float dj=_compiledRobotLanguageProgram[robotState.currentProgramLine].floatParameter[i]-robotState.currentJointPosition[i];
				float vv=robotState.jointVelocityWhenMoving[1];
				if (i==2)
					vv=robotState.jointVelocityWhenMoving[0]; // This is a linear joint
				float t=fabs(dj)/vv;
				if (t>biggestT)
					biggestT=t;
			}
			for (int i=0;i<4;i++)
			{
				if (biggestT>deltaTime)
				{
					float dj=_compiledRobotLanguageProgram[robotState.currentProgramLine].floatParameter[i]-robotState.currentJointPosition[i];
					if (dj!=0.0f)
					{
						float vv=fabs(dj)/biggestT;
						robotState.currentJointPosition[i]+=vv*deltaTime*dj/fabs(dj);
					}
					deltaTimeLeft=0.0f;
				}
				else
				{
					robotState.currentJointPosition[i]=_compiledRobotLanguageProgram[robotState.currentProgramLine].floatParameter[i];
					deltaTimeLeft=deltaTime-biggestT;
				}
			}
			deltaTime=deltaTimeLeft;
			if (deltaTime>0.0f)
				robotState.currentProgramLine++;
			else
				break;
		}
		if (cmd==1)
		{ // WAIT
			float timeToWait=_compiledRobotLanguageProgram[robotState.currentProgramLine].floatParameter[0];
			timeToWait-=robotState.timeAlreadySpentAtCurrentProgramLine;
			if (timeToWait>deltaTime)
			{
				robotState.timeAlreadySpentAtCurrentProgramLine+=deltaTime;
				break;
			}
			else
			{
				deltaTime-=timeToWait;
				robotState.currentProgramLine++;
			}
		}
		if (cmd==2)
		{ // GOTO
			robotState.currentProgramLine=_compiledRobotLanguageProgram[robotState.currentProgramLine].intParameter[0]; // we jump
		}
		if (cmd==3)
		{ // SETROTVEL
			robotState.jointVelocityWhenMoving[1]=_compiledRobotLanguageProgram[robotState.currentProgramLine].floatParameter[0];
			robotState.currentProgramLine++;
		}
		if (cmd==9)
		{ // SETLINVEL
			robotState.jointVelocityWhenMoving[0]=_compiledRobotLanguageProgram[robotState.currentProgramLine].floatParameter[0];
			robotState.currentProgramLine++;
		}
		if (cmd==5)
		{ // SETBIT
			int pos=_compiledRobotLanguageProgram[robotState.currentProgramLine].intParameter[0];
			int bytePos=(pos-1)/8; // 0-3
			int bitPos=(pos-1)%8; //0-7
			robotState.outputData[bytePos]|=1<<bitPos;
			robotState.currentProgramLine++;
		}
		if (cmd==6)
		{ // CLEARBIT
			int pos=_compiledRobotLanguageProgram[robotState.currentProgramLine].intParameter[0];
			int bytePos=(pos-1)/8; // 0-3
			int bitPos=(pos-1)%8; //0-7
			robotState.outputData[bytePos]&=255-(1<<bitPos);
			robotState.currentProgramLine++;
		}
		if (cmd==7)
		{ // IFBITGOTO
			int pos=_compiledRobotLanguageProgram[robotState.currentProgramLine].intParameter[0];
			int bytePos=(pos-1)/8; // 0-3
			int bitPos=(pos-1)%8; //0-7
			if (inputData[bytePos]&(1<<bitPos))
			{ // we have to jump
				robotState.currentProgramLine=_compiledRobotLanguageProgram[robotState.currentProgramLine].intParameter[1]; // we jump
			}
			else
				robotState.currentProgramLine++;
		}
		if (cmd==8)
		{ // IFNBITGOTO
			int pos=_compiledRobotLanguageProgram[robotState.currentProgramLine].intParameter[0];
			int bytePos=(pos-1)/8; // 0-3
			int bitPos=(pos-1)%8; //0-7
			if ((inputData[bytePos]&(1<<bitPos))==0)
			{ // we have to jump
				robotState.currentProgramLine=_compiledRobotLanguageProgram[robotState.currentProgramLine].intParameter[1]; // we jump
			}
			else
				robotState.currentProgramLine++;
		}
		robotState.timeAlreadySpentAtCurrentProgramLine=0.0f;
		if (int(_compiledRobotLanguageProgram.size())<=robotState.currentProgramLine)
			return(""); // program end
		loopCnt++;
		if (loopCnt>1000)
			break; // We looped too often... maybe waiting for an input signal or simply infinite loop! we leave here
		cmd=_compiledRobotLanguageProgram[robotState.currentProgramLine].command;
	}
	return(_compiledRobotLanguageProgram[robotState.currentProgramLine].correspondingUncompiledCode); // return the string command that is being executed now
}

void getJointsAndOutputData(float jointPosition[4],unsigned char outputData[4])
{
	for (int i=0;i<4;i++)
		jointPosition[i]=robotState.currentJointPosition[i];
	for (int i=0;i<4;i++)
		outputData[i]=robotState.outputData[i];
}
