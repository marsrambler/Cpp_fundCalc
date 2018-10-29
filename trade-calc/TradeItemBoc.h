#ifndef TRADE_ITEM_BOC_H
#define TRADE_ITEM_BOC_H

#include <string>
using namespace std;

class TradeItemBoc
{
public:
  string fundId;
  string fundName;
  string tradeDate;
  string tradeType;
  string tradeMoney;
  string tradeFlag;
  string tradeNo;
  bool operator < (const TradeItemBoc& m) const;
};
#endif
