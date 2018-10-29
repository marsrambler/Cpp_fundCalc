#ifndef FUND_TRADE_STATISTICS_H
#define FUND_TRADE_STATISTICS_H

#include <string>
using namespace std;

enum FUND_STATUS
{
  ON_GOING = 0,
  GIVEUP = 1,
};

class FundTradeStatistics
{
public:
  string fundId;
  string fundName;
  FUND_STATUS status; // 0 - ongoing, 1 - give up
  float totalEarnMoney;
  float totalInvestAndDays; // 10000Yuan & month
  float profitRate; // 10000Yuan & year
  string dateRange; // from date to end date.

public:
  float totalInvest;
  float totalReturned;
  float totalHolding;

public:
  FundTradeStatistics();
  static int diffDays(unsigned ty1, unsigned ty2);
  bool operator < (const FundTradeStatistics& m) const;
};

#endif