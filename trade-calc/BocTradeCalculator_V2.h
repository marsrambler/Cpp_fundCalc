#ifndef FUND_TRADE_BOC_V2_H
#define FUND_TRADE_BOC_V2_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#include <functional>
#include "TradeItemBoc_V2.h"
#include "FundTradeStatistics.h"
#include "TradeCalculator.h"
#include "BocTradeCalculator.h"

using namespace std;

class BocTradeCalculator_V2 : public TradeCalculator
{
public:
    int STATUS; // 0 - date, 1 - fundId, ..., 9 - tradeFlag
public:
    vector<TradeItemBoc_V2> vTradeHistory;
    map<string, vector<TradeItemBoc_V2> > tradeHistoryMap;
    // map<string, float> fundDateValue; //funId_date & value
    // map<string, string> fundTrans;

public:
    virtual int loadFundHistoryTradeRecords();
    virtual void distinguishFundTradeRecords();
    virtual void calculateFundTradeStatistics();
    virtual void dispFundHistoryTrade();

public:
    void dispVTradeItem(vector<TradeItemBoc_V2>& vItem);
    bool validateTradeType();
    //void getFundDateValue(string cfgFileName);
    //void getFundTrans(string cfgFileName);
    void translateDate();

public:
    BocTradeCalculator* bocProvCalculator;

public:
    BocTradeCalculator_V2();
    void setPrevCalculator(BocTradeCalculator* prevCalculator);
};
#endif

