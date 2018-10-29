#ifndef FAVOR_FUND_ITEM_H
#define FAVOR_FUND_ITEM_H

#include <vector>
#include <string>
#include "MergedFundItemGen.h"
using namespace std;

class FavorFundItem : public MergedFundItemGen
{
public:
  string id;
  string priority;
  string priComments;
  string stockTypeName;
  int totalFundNum;
  FavorFundItem();
  void assignMergedFundItemGen(MergedFundItemGen& mergedFundItemGen);
  virtual bool operator < (const FavorFundItem& m) const;
};

#endif