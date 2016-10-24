#ifndef CROBOT_H
#define CROBOT_H

#include <vector>
#include <iterator>

typedef struct _SRobotParam
{
    int robot_id;
    int mp_type;
    float cur_x, cur_y;
    float goal_x, goal_y;
    float uncertainty;
    float direction;
    float velocity;
    float pref_velocity;
}SRobotParam;

class CRobot
{
public:
    static std::vector<SRobotParam> robots;

public:
    CRobot();
    ~CRobot();

public:
    void allocRobot(int robot_id);
    void setRobotMPType(int robot_id, int mp_type);
    void setRobotLocation(int robot_id, float x, float y);
    void setRobotGoal(int robot_id,float x, float y);
    void setRobotUncertainty(int robot_id, float uncertainty);
    void setRobotDirection(int robot_id, float dir);
    void setRobotVelocity(int robot_id, float vel);
    void setRobotPrefVelocity(int robot_id, float vel);

    float getRobotPrefVelByID(int robot_id);
    int getNumOfRobot();
    SRobotParam getRobotParamAt(int idx);

};

#endif // CROBOT_H
