#include "VarScoreFundItemGen.h"

VarScoreFundItemGen::VarScoreFundItemGen(VarFundItemGen& m)
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

VarScoreFundItemGen::VarScoreFundItemGen(MergedFundItemGen& m)
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

VarScoreFundItemGen::~VarScoreFundItemGen()
{
}

bool VarScoreFundItemGen::operator < (const VarScoreFundItemGen& m) const
{
    if (scoreVariance < m.scoreVariance)
    {
        return true;
    }
/*
    else if (scoreVariance == m.scoreVariance)
    {
        if (rankVariance < m.rankVariance)
        {
            return true;
        }
    }
*/
    return false;
}

MergedFundItemGen VarScoreFundItemGen::getBaseItem()
{
    return *this;
}