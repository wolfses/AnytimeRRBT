
#pragma once

#include <list>
#include <utility>
class C3Vector;

typedef std::pair<float,float> RPoint; //Region Point

class CRegion
{
public:
    CRegion();
    ~CRegion();

    std::list< RPoint > point;
};

