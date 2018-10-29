#ifndef TRANSFORM_PROCESSOR
#define TRANSFORM_PROCESSOR

#include <vector>
#include <map>
#include "TransformFundItem.h"
#include "FavorFundsProcessor.h"
using namespace std;

class TransformProcessor : public FavorFundsProcessor
{
public:
    void buildFavorTransformRelation(string cfgFilename);

private:
    vector<string> getTransFunds(string fileName);

public:
    map<string, vector<string> > favorTransformMap;
    map<MergedFundItemGen, vector<TransformFundItem> > mainSubFundsMap;

public:
    int getMaxRankGenDispLen(vector<vector<TransformFundItem> >& vItem);
    int getMaxScoreGenDispLen(vector<vector<TransformFundItem> >& vItem);

public:
  void dumpTransformFund2File(string rootPath); 
};

#endif