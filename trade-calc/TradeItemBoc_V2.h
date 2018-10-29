#ifndef TRADE_ITEM_BOC_V2_H
#define TRADE_ITEM_BOC_V2_H

#include <string>
using namespace std;

class TradeItemBoc_V2
{
public:
  string tradeDate;
  string fundId;
  string fundName;
  string tradeType;
  string tradeMoney;
  string tradeUnits;
  string unitPrice;
  string tradeFee;
  string tradeFlag;
  bool operator < (const TradeItemBoc_V2& m) const;
};
#endif

