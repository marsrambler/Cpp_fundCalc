#include "VarFundItemGen.h"

#define _DEBUG_UNION_SORT

VarFundItemGen::VarFundItemGen()
{
    unionVarRankAndRank = -1;
}

VarFundItemGen::VarFundItemGen(MergedFundItemGen& m)
{
    this->id     = m.id;
    this->name   = m.name;
    this->vRankOfGens    = m.vRankOfGens;
    this->rankOfMerged   = m.rankOfMerged;
    this->rankVariance   = m.rankVariance;
    this->vScoreOfGens   = m.vScoreOfGens;
    this->scoreOfMerged  = m.scoreOfMerged;
    this->scoreVariance  = m.scoreVariance;
    this->vTickOfGens    = m.vTickOfGens;
    this->vTickOfGens    = m.vTickOfGens;
}

VarFundItemGen::~VarFundItemGen()
{
}

bool VarFundItemGen::operator < (const VarFundItemGen& m) const
{
    static int times = 0;

    if (unionVarRankAndRank > m.unionVarRankAndRank)
    {
        return true;
    }
    else if (unionVarRankAndRank == m.unionVarRankAndRank)
    {
#ifdef _DEBUG_UNION_SORT
        printf("times %3d: unionVarRankAndRank %d\n", times++, unionVarRankAndRank);
#endif
        if (rankVariance < m.rankVariance)
        {
            return true;
        }
        else if (rankVariance == m.rankVariance)
        {
            if (scoreVariance < m.scoreVariance)
            {
                return true;
            }
        }
    }
    return false;
}

MergedFundItemGen VarFundItemGen::getBaseItem()
{
    return *this;
}