#include "FundTopNItemGen.h"

FundTopNItemGen::FundTopNItemGen() 
{   
  score = 0;
  ticks = 0;
}

bool FundTopNItemGen::operator < (const FundTopNItemGen& m) const
{
  if (score > m.score) return true;
  else return false;
}
