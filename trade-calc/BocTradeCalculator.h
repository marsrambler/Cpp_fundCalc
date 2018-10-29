#ifndef FUND_TRADE_BOC_H
#define FUND_TRADE_BOC_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#include <functional>
#include "TradeItemBoc.h"
#include "FundTradeStatistics.h"
#include "TradeCalculator.h"
using namespace std;

class BocTradeCalculator : public TradeCalculator
{
public:
    class OneFundInAndOut
    {
    public:
        vector<float> vIn_Money;
        vector<string> vIn_Date;
        vector<float> vOut_Money;
        vector<string> vOut_Date;
        float totalInvest;
        float totalReturn;
        OneFundInAndOut()
        {
            totalInvest = 0;
            totalReturn = 0;
        }
    };

public:
    int STATUS; // 0 - fundId, 1 - fundName, ..., 6 - tradeNo
public:
    vector<TradeItemBoc> vTradeHistory;
    map<string, vector<TradeItemBoc> > tradeHistoryMap;
    map<string, float> fundDateValue; //funId_date & value
    map<string, string> fundTrans;

public:
    virtual int loadFundHistoryTradeRecords();
    virtual void distinguishFundTradeRecords();
    virtual void calculateFundTradeStatistics();
    virtual void dispFundHistoryTrade();

public:
    map<string, OneFundInAndOut> oneFundInAndOutMap;

public:
    void translateTradeRecords();
    void calcOneFundInAndOut();
    bool getOneFundInAndOut(string fundId, OneFundInAndOut& oneFundInAndOut, bool erase=true);
    vector<FundTradeStatistics> calcOrphaFund();

public:
    void dispVTradeItem(vector<TradeItemBoc>& vItem);
    bool validateTradeType();
    void getFundDateValue(string cfgFileName);
    void getFundTrans(string cfgFileName);

public:
    BocTradeCalculator();
};
#endif