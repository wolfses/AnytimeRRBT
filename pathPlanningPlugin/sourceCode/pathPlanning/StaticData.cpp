#include <stdio.h>
#include <stdlib.h>
#include "StaticData.h"
#include <vector>
#include "HolonomicPathNode.h"


std::vector<CHolonomicPathNode*> CStaticData::savedTree;
CHolonomicPathNode* CStaticData::savedGoal;
float CStaticData::path_length;
int CStaticData::try_count;
int CStaticData::numof_tested_vertex;
int CStaticData::numof_tested_belief;
CStaticData::CStaticData()
{

}

CStaticData::~CStaticData()
{

}
