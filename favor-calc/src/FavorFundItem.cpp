#include "FavorFundItem.h"

FavorFundItem::FavorFundItem()
{
    id = string("");
    priority = string("");
    priComments = string("");
    stockTypeName = string("");
    totalFundNum = 0;
}

void FavorFundItem::assignMergedFundItemGen(MergedFundItemGen& mergedFundItemGen)
{
    this->name = mergedFundItemGen.name;
    this->vRankOfGens   = mergedFundItemGen.vRankOfGens;
    this->rankOfMerged  = mergedFundItemGen.rankOfMerged;
    this->rankVariance  = mergedFundItemGen.rankVariance;
    this->vScoreOfGens  = mergedFundItemGen.vScoreOfGens;
    this->scoreOfMerged = mergedFundItemGen.scoreOfMerged;
    this->scoreVariance = mergedFundItemGen.scoreVariance;
    this->vTickOfGens   = mergedFundItemGen.vTickOfGens;
    this->vTickOfGens   = mergedFundItemGen.vTickOfGens;
    this->rankOfRankVar             = mergedFundItemGen.rankOfRankVar;
    this->rankOfScoreVar            = mergedFundItemGen.rankOfScoreVar;
    this->rankOfUnionRankAndScore   = mergedFundItemGen.rankOfUnionRankAndScore;

}

bool FavorFundItem::operator < (const FavorFundItem& m) const
{
    if (rankOfUnionRankAndScore < m.rankOfUnionRankAndScore)
    {
        return true;
    }
    return false;
}
