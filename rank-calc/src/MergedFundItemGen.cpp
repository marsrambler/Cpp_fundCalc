#include "MergedFundItemGen.h"

MergedFundItemGen::MergedFundItemGen()
{
    vRankOfGens.clear();
    rankOfMerged = 0;
    rankVariance = 0;

    vScoreOfGens.clear(); 
    scoreOfMerged = 0;
    scoreVariance = 0;
 
    rankOfRankVar = 0;
    rankOfScoreVar = 0;
    rankOfUnionRankAndScore = 0;
   
    vTickOfGens.clear();
    tickOfMerged = 0; 
}

MergedFundItemGen::~MergedFundItemGen()
{
}

bool MergedFundItemGen::operator < (const MergedFundItemGen& m) const
{
  if (scoreOfMerged > m.scoreOfMerged) return true;
  else return false;
}

