#ifndef FUND_TRADE_ICBC_H
#define FUND_TRADE_ICBC_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#include <functional>
#include "TradeItemIcbc.h"
#include "FundTradeStatistics.h"
#include "TradeCalculator.h"
using namespace std;

class IcbcTradeCalculator : public TradeCalculator
{
public:
    int STATUS; // 0 - applyFormNo, 1 - fundId, ..., 12 - fee
public:
    vector<TradeItemIcbc> vTradeHistory;
    map<string, vector<TradeItemIcbc> > tradeHistoryMap; // fundId <-> all trade records of this fundId

    //map<string, float> fundDateValue; //funId_date & value
    //map<string, string> fundTrans;

public:
    virtual int loadFundHistoryTradeRecords();
    virtual void distinguishFundTradeRecords();
    virtual void calculateFundTradeStatistics();
    virtual void dispFundHistoryTrade();
    virtual void dispFundHistoryTradeByAppId();

public:
    map<string, vector<vector<TradeItemIcbc> > > tradeApplyIdMap;
    // fundId -> applyFormId -> tradeRecords
    virtual void distinguishFundTradeRecordsByApplyId();
    virtual int mergeTradeDateForSameApplyId();

public:
    void dispVTradeItem(vector<TradeItemIcbc>& vItem, bool bTitle=true);
    bool validateTradeType();
    //void getFundDateValue(string cfgFileName);
    //void getFundTrans(string cfgFileName);

public:
    IcbcTradeCalculator();
    void transDate();
    void transApplyNum();
    void dispAllTradeTypes();
};
#endif

