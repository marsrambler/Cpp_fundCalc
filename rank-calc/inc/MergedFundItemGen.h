#ifndef MERGED_FUND_ITEM_GEN_H
#define MERGED_FUND_ITEM_GEN_H

#include <vector>
#include <string>
using namespace std;

class MergedFundItemGen
{
public:
  string id;
  string name;

  vector<int> vRankOfGens;
  int rankOfMerged;
  double rankVariance;

  vector<double> vScoreOfGens; 
  double scoreOfMerged;
  double scoreVariance;

  int rankOfRankVar;
  int rankOfScoreVar;
  int rankOfUnionRankAndScore;

  vector<int> vTickOfGens;
  int tickOfMerged; 

  MergedFundItemGen(); 
  ~MergedFundItemGen();
  virtual bool operator < (const MergedFundItemGen& m) const; 
};

#endif
