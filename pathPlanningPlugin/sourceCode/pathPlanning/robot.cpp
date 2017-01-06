#include "robot.h"

std::vector<SRobotParam> CRobot::robots;

CRobot::CRobot()
{

}

CRobot::~CRobot()
{

}

void CRobot::allocRobot(int robot_id)
{
    int len = robots.size();
    int i;
    for(i=0;i<len;i++)
    {
        if(robots[i].robot_id == robot_id)
            break;
    }
    if(i==len)
    {
        SRobotParam newBot;
        newBot.robot_id = robot_id;
        robots.push_back(newBot);
    }
}

void CRobot::setRobotMPType(int robot_id, int mp_type)
{

    for (std::vector<SRobotParam>::iterator it = this->robots.begin() ; it != this->robots.end(); ++it)
    {
        if((*it).robot_id == robot_id)
        {
            (*it).mp_type = mp_type;
            break;
        }
    }
}

void CRobot::setRobotLocation(int robot_id, float x, float y)
{
    for (std::vector<SRobotParam>::iterator it = this->robots.begin() ; it != this->robots.end(); ++it)
    {
        if((*it).robot_id == robot_id)
        {
            (*it).cur_x = x;
            (*it).cur_y = y;
            break;
        }
    }
}

void CRobot::setRobotGoal(int robot_id,float x, float y)
{
    for (std::vector<SRobotParam>::iterator it = this->robots.begin() ; it != this->robots.end(); ++it)
    {
        if((*it).robot_id == robot_id)
        {
            (*it).goal_x = x;
            (*it).goal_y = y;
            break;
        }
    }
}

void CRobot::setRobotUncertainty(int robot_id, float uncertainty)
{
    for (std::vector<SRobotParam>::iterator it = this->robots.begin() ; it != this->robots.end(); ++it)
    {
        if((*it).robot_id == robot_id)
        {
            (*it).uncertainty = uncertainty;
            break;
        }
    }
}

void CRobot::setRobotDirection(int robot_id, float dir)
{
    for (std::vector<SRobotParam>::iterator it = this->robots.begin() ; it != this->robots.end(); ++it)
    {
        if((*it).robot_id == robot_id)
        {
            (*it).direction = dir;
            break;
        }
    }
}

void CRobot::setRobotVelocity(int robot_id, float vel)
{
    for (std::vector<SRobotParam>::iterator it = this->robots.begin() ; it != this->robots.end(); ++it)
    {
        if((*it).robot_id == robot_id)
        {
            (*it).velocity = vel;
            break;
        }
    }
}

void CRobot::setRobotPrefVelocity(int robot_id, float vel)
{
    for (std::vector<SRobotParam>::iterator it = this->robots.begin() ; it != this->robots.end(); ++it)
    {
        if((*it).robot_id == robot_id)
        {
            (*it).pref_velocity = vel;
            break;
        }
    }
}

float CRobot::getRobotPrefVelByID(int robot_id)
{
    for (std::vector<SRobotParam>::iterator it = this->robots.begin() ; it != this->robots.end(); ++it)
    {
        if((*it).robot_id == robot_id)
        {
            return (*it).pref_velocity;
            break;
        }
    }
    return 0;
}

int CRobot::getNumOfRobot(){
    return robots.size();
}

SRobotParam CRobot::getRobotParamAt(int idx){
    return robots.at(idx);
}
