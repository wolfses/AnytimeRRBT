#pragma once

#include <vector>
class CRegion;

class CMeasurementRegion
{
public:
    CMeasurementRegion();
    ~CMeasurementRegion();

    bool checkMeasurement(float x,float y);
    void addMeasurementType1(float rect[4][3],float x0,float y0,float deg0);
    std::vector<CRegion> *m_regions;
    std::vector<CRegion> regionSet1;
    std::vector<CRegion> regionSet2;
};

