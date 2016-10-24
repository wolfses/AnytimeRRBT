#pragma once

#include <vector>
#include "HolonomicPathNode.h"
class CStaticData
{
public:
    static std::vector<CHolonomicPathNode*> savedTree;
    static CHolonomicPathNode* savedGoal;
    static float path_length;
    static int try_count;
    static int numof_tested_vertex;
    static int numof_tested_belief;

    CStaticData();
    ~CStaticData();
};

