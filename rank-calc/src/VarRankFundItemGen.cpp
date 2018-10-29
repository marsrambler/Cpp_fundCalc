#include "VarRankFundItemGen.h"

VarRankFundItemGen::VarRankFundItemGen(VarFundItemGen& m)
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

VarRankFundItemGen::VarRankFundItemGen(MergedFundItemGen& m)
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

VarRankFundItemGen::~VarRankFundItemGen()
{
}

bool VarRankFundItemGen::operator < (const VarRankFundItemGen& m) const
{
    if (rankVariance < m.rankVariance)
    {
        return true;
    }
/*
    else if (rankVariance == m.rankVariance)
    {
        if (scoreVariance < m.scoreVariance)
        {
            return true;
        }
    }
*/
    return false;
}

MergedFundItemGen VarRankFundItemGen::getBaseItem()
{
    return *this;
}