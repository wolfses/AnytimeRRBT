
#pragma once
#include <Eigen/Dense>
#include <vector>
class CRegion;

class CObstacles
{
public:
    CObstacles();
    ~CObstacles();

    bool checkCollide(float x,float y,Eigen::MatrixXd *r);

    void addObstacleType1(float rect[4][3],float x,float y, float gamma);
    std::vector<CRegion> *obstacles;
    std::vector<CRegion> obstacleSet1;
    std::vector<CRegion> obstacleSet2;
};

