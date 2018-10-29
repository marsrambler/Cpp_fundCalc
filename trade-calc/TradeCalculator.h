#ifndef TRADE_CALC_H
#define TRADE_CALC_H

#include <vector>
#include <string>
#include <map>
#include "FundTradeStatistics.h"
using namespace std;

enum TRADE_TYPE
{
TRADE_IN=0,
TRADE_OUT=1,
TRADE_USELESS=2
};

class TradeCalculator
{
public:
    vector<string> vTradeFiles;
    map<string, TRADE_TYPE> tradeTypeMap;
    map<string, float> fundHolder;
    vector<FundTradeStatistics> fundStatistics;
    string tradeCfgFileName;

public:
    vector<float> vIn_Money_total;
    vector<string> vIn_Date_total;
    vector<float> vOut_Money_total;
    vector<string> vOut_Date_total;

public:
    void configureHistoryTradeFiles(string cfgFileName);
    virtual int loadFundHistoryTradeRecords() = 0;
    virtual void distinguishFundTradeRecords() = 0;
    virtual void calculateFundTradeStatistics() = 0;
    virtual void dispFundHistoryTrade() = 0;
    void dispFundStatistics();
    void calcAndDispTotalProfit();

public:
    string trimString(string szString);
    bool validateNum(string& numString);
    bool validateAndFormatMoney(string& moneyString);
    float calculateInvestAndDays(vector<float>& vIn_Money, vector<string>& vIn_date, 
                                 vector<float>&vOut_Money, vector<string>& vOut_date, 
                                 string last_date);
    vector<string> splitTradeType(string& szLine);
    void getTradeTypeConfigured(string cfgFileName);
    void getCurrentHolder(string cfgFileName);
};

#endif