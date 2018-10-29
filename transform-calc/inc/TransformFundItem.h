#ifndef TRANSFORM_FUND_ITEM_H
#define TRANSFORM_FUND_ITEM_H

#include <vector>
#include <string>
#include "MergedFundItemGen.h"
using namespace std;

class TransformFundItem : public MergedFundItemGen
{
public:
  string id;
  string stockTypeName;
  int totalFundNum;
  string pearsonFactor;
  TransformFundItem();
  void assignMergedFundItemGen(MergedFundItemGen& mergedFundItemGen);
  virtual bool operator < (const TransformFundItem& m) const;
};

#endif

