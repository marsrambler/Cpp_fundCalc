#ifndef TRADE_ITEM_ICBC_H
#define TRADE_ITEM_ICBC_H

#include <string>
using namespace std;

class TradeItemIcbc
{
public:
  string applyFormId;
  string fundId;
  string fundName;
  string tradeStatus;
  string applyDate;
  string confirmDate;
  string finishDate;
  string tradeType;
  string unitPrice;
  string tradeMoney;
  string tradeUnits;
  string tradeFee;
  bool operator < (const TradeItemIcbc& m) const;
};

#endif