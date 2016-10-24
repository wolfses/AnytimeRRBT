#include "Obstacles.h"
#include "Region.h"
#include <math.h>

CObstacles::CObstacles()
{
    /*CRegion r1;
    RPoint p;

    //ordered by clock wise
    p.first = 2.5;  p.second = -1.45;
    r1.point.push_back(p);
    p.first = 2.5;  p.second = -1.55;
    r1.point.push_back(p);
    p.first = 0.25;  p.second = -1.55;
    r1.point.push_back(p);
    p.first = 0.25;  p.second = -1.45;
    r1.point.push_back(p);
    obstacleSet1.push_back(r1);

    r1.point.clear();
    p.first = -2.5;  p.second = -1.45;
    r1.point.push_back(p);
    p.first = -0.25;  p.second = -1.45;
    r1.point.push_back(p);
    p.first = -0.25;  p.second = -1.55;
    r1.point.push_back(p);
    p.first = -2.5;  p.second = -1.55;
    r1.point.push_back(p);
    obstacleSet1.push_back(r1);


    obstacles = &obstacleSet1;*/

    /*
    CRegion temp = obstacles->at(0);
    for(std::list< RPoint >::iterator iter=temp.point.begin();iter!=temp.point.end();iter++)
    {
        RPoint t = *iter;
        float x = t.first;
        float y = t.second;
        printf("%f %f\n",x,y);
    }

    temp = obstacles->at(1);
    for(std::list< RPoint >::iterator iter=temp.point.begin();iter!=temp.point.end();iter++)
    {
        RPoint t = *iter;
        float x = t.first;
        float y = t.second;
        printf("%f %f\n",x,y);
    }*/
    obstacles = &obstacleSet1;
}

CObstacles::~CObstacles()
{

}

void CObstacles::addObstacleType1(float rect[4][3],float x0,float y0, float deg0)
{
    float x,y;
    float c = cos(deg0),s = sin(deg0);
    CRegion r1;
    RPoint p;
    for(int i=0;i<4;i++)
    {
        x = c * rect[i][0] - s * rect[i][1] + x0;
        y = s * rect[i][0] + c * rect[i][1] + y0;
        p.first = x;  p.second = y;
        r1.point.push_back(p);
    }
    obstacleSet1.push_back(r1);

    /*float width = 0.375;
    float height = 0.05;
    float length = sqrt(width * width + height * height);
    float offset_deg = atan2(height,width);
    float x1, y1, deg1;
    float x2, y2, deg2;
    float x3, y3, deg3;
    float x4, y4, deg4;

    deg1 = M_PI - offset_deg + deg0;
    x1 = x0 + length * cos(deg1);
    y1 = y0 + length * sin(deg1);

    deg2 = offset_deg + deg0;
    x2 = x0 + length * cos(deg2);
    y2 = y0 + length * sin(deg2);

    deg3 = -offset_deg + deg0;
    x3 = x0 + length * cos(deg3);
    y3 = y0 + length * sin(deg3);

    deg4 = M_PI + offset_deg + deg0;
    x4 = x0 + length * cos(deg4);
    y4 = y0 + length * sin(deg4);

    CRegion r1;
    RPoint p;

    //ordered by clock wise

    p.first = x1;  p.second = y1;
    r1.point.push_back(p);
    p.first = x2;  p.second = y2;
    r1.point.push_back(p);
    p.first = x3;  p.second = y3;
    r1.point.push_back(p);
    p.first = x4;  p.second = y4;
    r1.point.push_back(p);
    obstacleSet1.push_back(r1);

    obstacles = &obstacleSet1;*/
}

bool CObstacles::checkCollide(float x0,float y0,Eigen::MatrixXd *p)
{
    float x1 = 10, y1 = 10;
    int cross;
    int temp;
    //printf("%f %f ",x0,y0);
    //p.norm();
    for(std::vector<CRegion>::iterator iter_region=obstacles->begin();iter_region!=obstacles->end();++iter_region)
    {
        CRegion region = *iter_region;
        cross = 0;
        bool cc = 1;
        temp = 0;
        for(std::list< RPoint >::iterator iter_point=region.point.begin();iter_point!=region.point.end();)
        {
            RPoint p1 = *   iter_point;
            RPoint p2 = *(++iter_point);
            if(iter_point == region.point.end())
                p2 = *region.point.begin();
            float x2 = p1.first;
            float y2 = p1.second;
            float x3 = p2.first;
            float y3 = p2.second;

            //printf("%f %f %f %f %f %f\n",x,y,x1,y1,x2,y2);

            float sign1 = (x1 * y2) + (x2 * y3) + (x3 * y1) - (x1 * y3) - (x2 * y1) - (x3 * y2);
            float sign2 = (x0 * y2) + (x2 * y3) + (x3 * y0) - (x0 * y3) - (x2 * y0) - (x3 * y2);

            float sign3 = (x0 * y1) + (x1 * y2) + (x2 * y0) - (x0 * y2) - (x1 * y0) - (x2 * y1);
            float sign4 = (x0 * y1) + (x1 * y3) + (x3 * y0) - (x0 * y3) - (x1 * y0) - (x3 * y1);
            if(sign1 * sign2 < 0 && sign3 * sign4 < 0)
                cross = cross + 1;

            float dist = sign2 / sqrt( (x2-x3)*(x2-x3) + (y2-y3)*(y2-y3) );
            //printf("collide %d] %f %f\n",temp++,dist,p.norm());
            if((dist > p->norm() * 60) && (dist > 0.1))
            {// dist - robot.radius값이 x일때  0.5 - (  inf->x까지  norm(0,p-norm())을 적분한값) 이 delta보다 작아야함
             // delta는 최대 0.5, 0.5보다 작을수록 보수적으로 계산함
                cc = 0;
            }
        }
        if(cross % 2 == 1)
        {
            //printf("collide\n");
            return true;
        }
        if(cc == 1)
        {
            return true;
        }
    }
    //printf("off measurement\n");
    return false;
}

