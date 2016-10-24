#include "MeasurementRegion.h"
#include "Region.h"
#include <iterator>
#include <vector>
#include <iostream>
#include <math.h>

CMeasurementRegion::CMeasurementRegion()
{
    CRegion r1;
    RPoint p;

    /*
    p.first = 2.5;  p.second = 2.5;
    r1.point.push_back(p);
    p.first = 2.5;  p.second = 1.5;
    r1.point.push_back(p);
    p.first = -2.5;  p.second = 1.5;
    r1.point.push_back(p);
    p.first = -2.5;  p.second = 2.5;
    r1.point.push_back(p);
    regionSet1.push_back(r1);
    */
    m_regions = &regionSet1;
}

CMeasurementRegion::~CMeasurementRegion()
{

}

bool CMeasurementRegion::checkMeasurement(float x0,float y0)
{
    float x1 = 10, y1 = 10;
    int cross;
    //printf("%f %f ",x0,y0);
    for(std::vector<CRegion>::iterator iter_region=m_regions->begin();iter_region!=m_regions->end();++iter_region)
    {
        CRegion region = *iter_region;
        cross = 0;
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
        }
        if(cross % 2 == 1)
        {
            //printf("on measurement\n");
            return true;
        }
    }
    //printf("off measurement\n");
    return false;
}

void CMeasurementRegion::addMeasurementType1(float rect[4][3],float x0,float y0,float deg0)
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
    regionSet1.push_back(r1);
}
