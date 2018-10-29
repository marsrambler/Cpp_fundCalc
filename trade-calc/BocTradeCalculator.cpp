#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#include <functional>
#include "BocTradeCalculator.h"
#include "TradeItemBoc.h"
#include "FundTradeStatistics.h"
using namespace std;

//#define _DEBUG

BocTradeCalculator::BocTradeCalculator()
{
    STATUS = 0;
}

void BocTradeCalculator::dispVTradeItem(vector<TradeItemBoc>& vItem)
{
  char buffer[2048] = {0};
  sprintf(buffer, "%10s %40s %10s %20s %10s %5s %15s", "fundId", "fundName", "date", "type", "money", "flag", "tradeNo");
  cout << buffer << endl;

  for (int i = 0; i < vItem.size(); i++)
  {
    TradeItemBoc& item = vItem[i];
    char buffer[2048] = {0};
    sprintf(buffer, "%10s %40s %10s %20s %10s %5s %15s", item.fundId.c_str(), item.fundName.c_str(),
                          item.tradeDate.c_str(), item.tradeType.c_str(), item.tradeMoney.c_str(),
                          item.tradeFlag.c_str(), item.tradeNo.c_str());
    cout << buffer << endl;
  }
}

bool BocTradeCalculator::validateTradeType()
{
  if (vTradeHistory.size() == 0)
  {
    cerr << "ERROR, the trade history records is 0!" << endl;
    return false;
  }

  getTradeTypeConfigured(tradeCfgFileName);

#ifdef _DEBUG
  map<string, int> tradeTypeMap_debug;
  for (int i = 0; i < vTradeHistory.size(); i++)
  {
    TradeItemBoc& item = vTradeHistory[i];
    map<string, int>::iterator iter = tradeTypeMap_debug.find(item.tradeType);
    if (iter != tradeTypeMap_debug.end())
    {
      tradeTypeMap_debug[item.tradeType] += 1;
    }
    else
    {
      tradeTypeMap_debug.insert(make_pair(item.tradeType, 1));
    }
  }

  map<string, int>::iterator iter = tradeTypeMap_debug.begin();
  for (; iter != tradeTypeMap_debug.end(); iter++)
  {
    cout << (string)(iter->first) << " : " << (int)(iter->second) << endl;
  }
#endif

  for (int i = 0; i < vTradeHistory.size(); i++)
  {
    TradeItemBoc& item = vTradeHistory[i];
    if (tradeTypeMap.find(item.tradeType) == tradeTypeMap.end())
    {
        cerr << "trade type error: " << item.tradeType << endl;
        return false;
    }
  }

  return true;
}

void BocTradeCalculator::getFundDateValue(string cfgFileName)
{
    ifstream rfile(cfgFileName.c_str());
    string oneLine;
    bool bFundValueSec = false;
    while (getline(rfile, oneLine))
    {
        size_t pos = oneLine.find(string("\r"));
        if (pos == string::npos)
        {
          pos = oneLine.find(string("\n"));
          if (pos == string::npos) break;
        }

        string pureLine = trimString(oneLine);
        if (pureLine.find(string("#")) != string::npos) continue;
        if (pureLine.size() == 0) continue;
        if (pureLine.find(string("[history-value]")) != string::npos)
        {
            bFundValueSec = true;
        }
        else if (pureLine.find(string("[")) != string::npos)
        {
            bFundValueSec = false;
        }
        else if (bFundValueSec)
        {
            pos = pureLine.find(string("="));
            string fundId_date = pureLine.substr(0, pos);
            string value = pureLine.substr(pos+1);
            fundId_date = trimString(fundId_date);
            value = trimString(value);
            if (fundId_date.size() > 0 && value.size() > 0)
            {
                float fundValue = atof(value.c_str());
                fundDateValue.insert(make_pair(fundId_date, fundValue));
            }
        }
    }

#ifdef _DEBUG
    cout << endl << "debug fund value" << endl;
    map<string, float>::iterator iter = fundDateValue.begin();
    for (; iter != fundDateValue.end(); iter++)
    {
        cout << (string)(iter->first) << " : " << (float)(iter->second) << endl;
    }
#endif
}

void BocTradeCalculator::getFundTrans(string cfgFileName)
{
    ifstream rfile(cfgFileName.c_str());
    string oneLine;
    bool bTransSec = false;
    while (getline(rfile, oneLine))
    {
        size_t pos = oneLine.find(string("\r"));
        if (pos == string::npos)
        {
          pos = oneLine.find(string("\n"));
          if (pos == string::npos) break;
        }

        string pureLine = trimString(oneLine);
        if (pureLine.find(string("#")) != string::npos) continue;
        if (pureLine.size() == 0) continue;
        if (pureLine.find(string("[fund-trans]")) != string::npos)
        {
            bTransSec = true;
        }
        else if (pureLine.find(string("[")) != string::npos)
        {
            bTransSec = false;
        }
        else if (bTransSec)
        {
            pos = pureLine.find(string("->"));
            string fundOrg = pureLine.substr(0, pos);
            string fundNew = pureLine.substr(pos+string("->").size());
            fundOrg = trimString(fundOrg);
            fundNew = trimString(fundNew);
            fundTrans.insert(make_pair(fundOrg, fundNew));
        }
    }
#ifdef _DEBUG
    cout << endl << "debug fund trans" << endl;
    map<string, string>::iterator iter = fundTrans.begin();
    for (; iter != fundTrans.end(); iter++)
    {
        cout << (string)(iter->first) << " -> " << (string)(iter->second) << endl;
    }
#endif
}

int BocTradeCalculator::loadFundHistoryTradeRecords()
{
    if (vTradeFiles.size() == 0)
    {
      cerr << "Trade file is null, should load configuration files firstly" << endl;
      return -1;
    }

    for (int i = 0; i < vTradeFiles.size(); i++)
    {
      vector<TradeItemBoc> vSubTrade;
      STATUS = 0;
      int subIdx = 0; 
    
      ifstream rfile(vTradeFiles[i].c_str());
      string oneLine;
      while (getline(rfile, oneLine))
      {
        size_t pos = oneLine.find(string("\r"));
        if (pos == string::npos)
        {
          pos = oneLine.find(string("\n"));
          if (pos == string::npos) break;
        }
        
        string pureLine = trimString(oneLine);
        if (pureLine.size() == 0)
        {
          if (subIdx < vSubTrade.size())
          {
            cerr << "ERROR file<0>: " << vTradeFiles[i] << " size:idx:stat " 
                 << vSubTrade.size() << " " << subIdx << " " << STATUS << endl;
            return -2;
          }

          STATUS++;
          subIdx = 0;
          continue;
        }
    
        TradeItemBoc item;
        TradeItemBoc* item_p = &item;
        bool bReplaceFlag = false;
    
        if (STATUS == 0)
        {
      
        }
        else
        {
          bReplaceFlag = true;
          if (subIdx >= vSubTrade.size())
          {
              cerr << "ERROR: " << pureLine << endl;
              return -3;
          }
          item = vSubTrade[subIdx];
        }
       
        switch (STATUS)
        {
          case 0: { item_p->fundId = pureLine;     break;}
          case 1: { item_p->fundName = pureLine;   break;}
          case 2: { item_p->tradeDate = pureLine;  break;}
          case 3: { item_p->tradeType = pureLine;  break;}
          case 4: { item_p->tradeMoney = pureLine; break;}
          case 5: { item_p->tradeFlag  = pureLine; break;}
          case 6: { item_p->tradeNo = pureLine;    break;}
          default: { cout << "error, status: " << STATUS << " line: " << pureLine << endl; break;}
        }
    
        if (STATUS == 0)
        {
          vSubTrade.push_back(item);
        }
        else
        {
          vSubTrade[subIdx] = item;
        }
    
        subIdx++;
      }

      if (subIdx < vSubTrade.size())
      {
        cerr << "ERROR file<0>: " << vTradeFiles[i] << " size:idx:stat " 
             << vSubTrade.size() << " " << subIdx << " " << STATUS << endl;
        return -4;
      }

      //dispVTradeItem(vSubTrade);
      //cout << "new sub: " << i << endl;    
      vTradeHistory.insert(vTradeHistory.end(), vSubTrade.begin(), vSubTrade.end());
      vSubTrade.clear();
    }

    for (int i = 0; i < vTradeHistory.size(); i++)
    {
      TradeItemBoc& item = vTradeHistory[i];
      if (!validateNum(item.fundId))
      {
        cerr << "i: " << i << " fundId: " << item.fundId << endl;
        return -5;
      }
      if (!validateNum(item.tradeDate))
      {
        cerr << "i: " << i << " tradeDate: " << item.tradeDate << endl;
        return -6;
      }
      if (!validateAndFormatMoney(item.tradeMoney))
      {
        cerr << "i: " << i << " tradeMoney: " << item.tradeMoney << endl;
        return -7;
      }
    }

    if (!validateTradeType())
    {
        cerr << "validate trade type failed" << endl;
        return -8;
    }

    getFundDateValue(tradeCfgFileName);
    getCurrentHolder(tradeCfgFileName);
    getFundTrans(tradeCfgFileName);
    return 0;
}

void BocTradeCalculator::distinguishFundTradeRecords()
{
    while (vTradeHistory.size() > 0)
    {
      vector<TradeItemBoc> vSubFundTrade;
      vSubFundTrade.clear();
      int eraseTimes = 0;
    
      TradeItemBoc item = vTradeHistory[0];
      vSubFundTrade.push_back(item);
    
      vector<TradeItemBoc>::iterator iter = vTradeHistory.begin();
      vTradeHistory.erase(iter);
      while (iter != vTradeHistory.end())
      {
        TradeItemBoc iterItem = (TradeItemBoc)(*iter);
        if (item.fundId.compare(iterItem.fundId) == 0)
        { 
          eraseTimes++;
          vSubFundTrade.push_back(iterItem);
          vTradeHistory.erase(iter);
        }
        else
        {
          iter++;
        }
      }
      if (tradeHistoryMap.find(item.fundId) != tradeHistoryMap.end())
      {
        cerr << "fundId alread existed: " << item.fundId << endl;
      }
      else
      {
        tradeHistoryMap.insert(make_pair(item.fundId, vSubFundTrade));
      }
    }
}

void BocTradeCalculator::translateTradeRecords() 
{
    if (tradeHistoryMap.size() <= 0 || fundTrans.size() <= 0)
    {
        cerr << "should call this function after getFundTrans() and distinguishTrades()" << endl;
        return;
    }

    //Fund trade translating replacement
    map<string, string>:: iterator iter = fundTrans.begin();
    for (; iter != fundTrans.end(); iter++)
    {
        string orgFundId = (string)(iter->first);
        string newFundId = (string)(iter->second);
        map<string, vector<TradeItemBoc> >::iterator iterOldFund = tradeHistoryMap.find(orgFundId);
        map<string, vector<TradeItemBoc> >::iterator iterNewFund = tradeHistoryMap.find(newFundId);
        if (iterOldFund == tradeHistoryMap.end() || iterNewFund == tradeHistoryMap.end())
        {
            cerr << "Error for the fund translating trade: " << orgFundId << " -> " << newFundId << endl;
            return;
        }
        vector<TradeItemBoc>& vOldFundTrade = (iterOldFund->second);
        vector<TradeItemBoc>& vNewFundTrade = (iterNewFund->second);
        for (int i = 0; i < vOldFundTrade.size(); i++)
        {
            vOldFundTrade[i].fundId = vNewFundTrade[0].fundId;
            vOldFundTrade[i].fundName = vNewFundTrade[0].fundName;
        }
        vNewFundTrade.insert(vNewFundTrade.begin(), vOldFundTrade.begin(), vOldFundTrade.end());
        tradeHistoryMap.erase(iterOldFund);
    }    
}

void BocTradeCalculator::calcOneFundInAndOut()
{
    vIn_Money_total.clear();
    vIn_Date_total.clear();
    vOut_Money_total.clear();
    vOut_Date_total.clear();

    // calculate all of the statistics
    map<string, vector<TradeItemBoc> >::iterator iterTrade = tradeHistoryMap.begin();
    for (; iterTrade != tradeHistoryMap.end(); iterTrade++)
    {
        vector<TradeItemBoc>& vTrade = (iterTrade->second);
        sort(vTrade.begin(), vTrade.end());

        string fundId = vTrade[0].fundId;

        //FundTradeStatistics oneStat;
        //oneStat.fundId   = vTrade[0].fundId;
        //oneStat.fundName = vTrade[0].fundName;
        /*
        map<string, float>::iterator iterHolder = fundHolder.find(oneStat.fundId);
        if (iterHolder != fundHolder.end())
        {
            oneStat.totalHolding = iterHolder->second;
            oneStat.status = ON_GOING;
        }
        else
        {
            oneStat.status = GIVEUP;
        }
        */

        //
        vector<float> vIn_Money;
        vector<string> vIn_Date;
        vector<float> vOut_Money;
        vector<string> vOut_Date;
        float totalInvest = 0;
        float totalReturn = 0;

        for (int i = 0; i < vTrade.size(); i++)
        {
            map<string, TRADE_TYPE>::iterator iter = tradeTypeMap.find(vTrade[i].tradeType);
            if (iter == tradeTypeMap.end())
            {
                cerr << "error, trade type error: " << vTrade[i].tradeType << endl;
                return;
            }
            TRADE_TYPE tradeType = iter->second;
            if (tradeType == TRADE_USELESS)
            {
                continue;
            }
            else if (tradeType == TRADE_IN)
            {
                //oneStat.totalInvest += atof(vTrade[i].tradeMoney.c_str());
                totalInvest += atof(vTrade[i].tradeMoney.c_str());
                vIn_Money.push_back(atof(vTrade[i].tradeMoney.c_str()));
                vIn_Money_total.push_back(atof(vTrade[i].tradeMoney.c_str()));
                vIn_Date.push_back(vTrade[i].tradeDate);
                vIn_Date_total.push_back(vTrade[i].tradeDate);
            }
            else if (tradeType == TRADE_OUT)
            {
                string fundId_date = fundId + string("_") + vTrade[i].tradeDate;
                map<string, float>::iterator iterVal = fundDateValue.find(fundId_date);
                if (iterVal == fundDateValue.end())
                {
                    cerr << "fundDateValue look up failed" << fundId_date << endl;
                    return;
                }
                float fVal = iterVal->second;
                float out_money = atof(vTrade[i].tradeMoney.c_str()) * fVal;
                //oneStat.totalReturned += out_money;
                totalReturn += out_money;
                vOut_Money.push_back(out_money);
                vOut_Money_total.push_back(out_money);
                vOut_Date.push_back(vTrade[i].tradeDate);
                vOut_Date_total.push_back(vTrade[i].tradeDate);
            }
        }
        
        /*
        string lastDate("");
        if (oneStat.status == ON_GOING)
        {
            oneStat.totalReturned += oneStat.totalHolding;
            time_t now;
            struct tm *timenow;
            time(&now);
            timenow = localtime(&now);
            char buffer[20] = {0};
            sprintf(buffer, "%04d%02d%02d", 1900+timenow->tm_year, 1+timenow->tm_mon, timenow->tm_mday);
            //cout << "current day: " << buffer << endl;
            lastDate = string(buffer);
            oneStat.dateRange = vTrade[0].tradeDate+string("-")+string("today");
        }
        else
        {
            lastDate = vTrade[vTrade.size()-1].tradeDate;
            oneStat.dateRange = vTrade[0].tradeDate+string("-")+lastDate;
        }
        */

        /*
        oneStat.totalInvestAndDays = calculateInvestAndDays(vIn_Money, vIn_Date, vOut_Money, vOut_Date, lastDate);        
        oneStat.totalEarnMoney = oneStat.totalReturned - oneStat.totalInvest;
        oneStat.profitRate = oneStat.totalEarnMoney / oneStat.totalInvestAndDays / 100;

        fundStatistics.push_back(oneStat);
        */

        OneFundInAndOut oneFundInAndOut;
        oneFundInAndOut.vIn_Money   = vIn_Money;
        oneFundInAndOut.vIn_Date    = vIn_Date;
        oneFundInAndOut.vOut_Money  = vOut_Money;
        oneFundInAndOut.vOut_Date   = vOut_Date;
        oneFundInAndOut.totalInvest = totalInvest;
        oneFundInAndOut.totalReturn = totalReturn;

        map<string, OneFundInAndOut>::iterator iterInAndOut = oneFundInAndOutMap.find(fundId);
        if (iterInAndOut != oneFundInAndOutMap.end())
        {
            cerr << "Error, calc oneFund in and out FAILED, fundId = " << fundId << endl;
            return;
        }
        else
        {
            oneFundInAndOutMap.insert(make_pair(fundId, oneFundInAndOut)); 
        }
    }
    
    //cout << endl << "after trade translating replacement......................" << endl;
    //dispFundHistoryTrade();
    
    //sort(fundStatistics.begin(), fundStatistics.end());
    //cout << endl << "after calculate statistics..............................." << endl;
    //dispFundStatistics();
}

bool BocTradeCalculator::getOneFundInAndOut(string fundId, OneFundInAndOut& oneFundInAndOut, bool erase)
{
    map<string, OneFundInAndOut>::iterator iterInAndOut = oneFundInAndOutMap.find(fundId);
    if (iterInAndOut != oneFundInAndOutMap.end())
    {
        oneFundInAndOut = iterInAndOut->second;
        if (erase)
        {
            oneFundInAndOutMap.erase(iterInAndOut);
        }
        return true;
    }
    return false;
}

vector<FundTradeStatistics> BocTradeCalculator::calcOrphaFund()
{
    vector<FundTradeStatistics> vTradeStats;
    vTradeStats.clear();

    map<string, OneFundInAndOut>::iterator iterInAndOut = oneFundInAndOutMap.begin();
    for (; iterInAndOut != oneFundInAndOutMap.end(); iterInAndOut++)
    {
        string fundId = (string)(iterInAndOut->first);
        map<string, vector<TradeItemBoc> >::iterator iterTrade = tradeHistoryMap.find(fundId);

        if (iterTrade == tradeHistoryMap.end())
        {
            cerr << "calc prhpa fund trade FAILED, fundId: " << fundId << endl;
            return vTradeStats;
        }

        vector<TradeItemBoc>& vTrade = (iterTrade->second);
        sort(vTrade.begin(), vTrade.end());

        FundTradeStatistics oneStat;
        oneStat.fundId   = vTrade[0].fundId;
        oneStat.fundName = vTrade[0].fundName;

        map<string, float>::iterator iterHolder = fundHolder.find(oneStat.fundId);
        if (iterHolder != fundHolder.end())
        {
            oneStat.totalHolding = iterHolder->second;
            oneStat.status = ON_GOING;
        }
        else
        {
            oneStat.status = GIVEUP;
        }

        //
        vector<float> vIn_Money;
        vector<string> vIn_Date;
        vector<float> vOut_Money;
        vector<string> vOut_Date;

        for (int i = 0; i < vTrade.size(); i++)
        {
            map<string, TRADE_TYPE>::iterator iter = tradeTypeMap.find(vTrade[i].tradeType);
            if (iter == tradeTypeMap.end())
            {
                cerr << "error, trade type error: " << vTrade[i].tradeType << endl;
                return vTradeStats;
            }
            TRADE_TYPE tradeType = iter->second;
            if (tradeType == TRADE_USELESS)
            {
                continue;
            }
            else if (tradeType == TRADE_IN)
            {
                oneStat.totalInvest += atof(vTrade[i].tradeMoney.c_str());
                vIn_Money.push_back(atof(vTrade[i].tradeMoney.c_str()));
                vIn_Money_total.push_back(atof(vTrade[i].tradeMoney.c_str()));
                vIn_Date.push_back(vTrade[i].tradeDate);
                vIn_Date_total.push_back(vTrade[i].tradeDate);
            }
            else if (tradeType == TRADE_OUT)
            {
                string fundId_date = oneStat.fundId + string("_") + vTrade[i].tradeDate;
                map<string, float>::iterator iterVal = fundDateValue.find(fundId_date);
                if (iterVal == fundDateValue.end())
                {
                    cerr << "fundDateValue look up failed" << fundId_date << endl;
                    return vTradeStats;
                }
                float fVal = iterVal->second;
                float out_money = atof(vTrade[i].tradeMoney.c_str()) * fVal;
                oneStat.totalReturned += out_money;
                vOut_Money.push_back(out_money);
                vOut_Money_total.push_back(out_money);
                vOut_Date.push_back(vTrade[i].tradeDate);
                vOut_Date_total.push_back(vTrade[i].tradeDate);
            }
        }

        string lastDate("");
        if (oneStat.status == ON_GOING)
        {
            oneStat.totalReturned += oneStat.totalHolding;
            time_t now;
            struct tm *timenow;
            time(&now);
            timenow = localtime(&now);
            char buffer[20] = {0};
            sprintf(buffer, "%04d%02d%02d", 1900+timenow->tm_year, 1+timenow->tm_mon, timenow->tm_mday);
            //cout << "current day: " << buffer << endl;
            lastDate = string(buffer);
            oneStat.dateRange = vTrade[0].tradeDate+string("-")+string("today");
        }
        else
        {
            lastDate = vTrade[vTrade.size()-1].tradeDate;
            oneStat.dateRange = vTrade[0].tradeDate+string("-")+lastDate;
        }

        oneStat.totalInvestAndDays = calculateInvestAndDays(vIn_Money, vIn_Date, vOut_Money, vOut_Date, lastDate);        
        oneStat.totalEarnMoney = oneStat.totalReturned - oneStat.totalInvest;
        oneStat.profitRate = oneStat.totalEarnMoney / oneStat.totalInvestAndDays / 100;

        vTradeStats.push_back(oneStat);        
    }

    sort(vTradeStats.begin(), vTradeStats.end());

    return vTradeStats;
}

void BocTradeCalculator::calculateFundTradeStatistics()
{
    //Fund trade translating replacement
    map<string, string>:: iterator iter = fundTrans.begin();
    for (; iter != fundTrans.end(); iter++)
    {
        string orgFundId = (string)(iter->first);
        string newFundId = (string)(iter->second);
        map<string, vector<TradeItemBoc> >::iterator iterOldFund = tradeHistoryMap.find(orgFundId);
        map<string, vector<TradeItemBoc> >::iterator iterNewFund = tradeHistoryMap.find(newFundId);
        if (iterOldFund == tradeHistoryMap.end() || iterNewFund == tradeHistoryMap.end())
        {
            cerr << "Error for the fund translating trade: " << orgFundId << " -> " << newFundId << endl;
            return;
        }
        vector<TradeItemBoc>& vOldFundTrade = (iterOldFund->second);
        vector<TradeItemBoc>& vNewFundTrade = (iterNewFund->second);
        for (int i = 0; i < vOldFundTrade.size(); i++)
        {
            vOldFundTrade[i].fundId = vNewFundTrade[0].fundId;
            vOldFundTrade[i].fundName = vNewFundTrade[0].fundName;
        }
        vNewFundTrade.insert(vNewFundTrade.begin(), vOldFundTrade.begin(), vOldFundTrade.end());
        tradeHistoryMap.erase(iterOldFund);
    }

    vIn_Money_total.clear();
    vIn_Date_total.clear();
    vOut_Money_total.clear();
    vOut_Date_total.clear();

    // calculate all of the statistics
    map<string, vector<TradeItemBoc> >::iterator iterTrade = tradeHistoryMap.begin();
    for (; iterTrade != tradeHistoryMap.end(); iterTrade++)
    {
        vector<TradeItemBoc>& vTrade = (iterTrade->second);
        sort(vTrade.begin(), vTrade.end());

        FundTradeStatistics oneStat;
        oneStat.fundId   = vTrade[0].fundId;
        oneStat.fundName = vTrade[0].fundName;

        map<string, float>::iterator iterHolder = fundHolder.find(oneStat.fundId);
        if (iterHolder != fundHolder.end())
        {
            oneStat.totalHolding = iterHolder->second;
            oneStat.status = ON_GOING;
        }
        else
        {
            oneStat.status = GIVEUP;
        }

        //
        vector<float> vIn_Money;
        vector<string> vIn_Date;
        vector<float> vOut_Money;
        vector<string> vOut_Date;

        for (int i = 0; i < vTrade.size(); i++)
        {
            map<string, TRADE_TYPE>::iterator iter = tradeTypeMap.find(vTrade[i].tradeType);
            if (iter == tradeTypeMap.end())
            {
                cerr << "error, trade type error: " << vTrade[i].tradeType << endl;
                return;
            }
            TRADE_TYPE tradeType = iter->second;
            if (tradeType == TRADE_USELESS)
            {
                continue;
            }
            else if (tradeType == TRADE_IN)
            {
                oneStat.totalInvest += atof(vTrade[i].tradeMoney.c_str());
                vIn_Money.push_back(atof(vTrade[i].tradeMoney.c_str()));
                vIn_Money_total.push_back(atof(vTrade[i].tradeMoney.c_str()));
                vIn_Date.push_back(vTrade[i].tradeDate);
                vIn_Date_total.push_back(vTrade[i].tradeDate);
            }
            else if (tradeType == TRADE_OUT)
            {
                string fundId_date = oneStat.fundId + string("_") + vTrade[i].tradeDate;
                map<string, float>::iterator iterVal = fundDateValue.find(fundId_date);
                if (iterVal == fundDateValue.end())
                {
                    cerr << "fundDateValue look up failed" << fundId_date << endl;
                    return;
                }
                float fVal = iterVal->second;
                float out_money = atof(vTrade[i].tradeMoney.c_str()) * fVal;
                oneStat.totalReturned += out_money;
                vOut_Money.push_back(out_money);
                vOut_Money_total.push_back(out_money);
                vOut_Date.push_back(vTrade[i].tradeDate);
                vOut_Date_total.push_back(vTrade[i].tradeDate);
            }
        }

        string lastDate("");
        if (oneStat.status == ON_GOING)
        {
            oneStat.totalReturned += oneStat.totalHolding;
            time_t now;
            struct tm *timenow;
            time(&now);
            timenow = localtime(&now);
            char buffer[20] = {0};
            sprintf(buffer, "%04d%02d%02d", 1900+timenow->tm_year, 1+timenow->tm_mon, timenow->tm_mday);
            //cout << "current day: " << buffer << endl;
            lastDate = string(buffer);
            oneStat.dateRange = vTrade[0].tradeDate+string("-")+string("today");
        }
        else
        {
            lastDate = vTrade[vTrade.size()-1].tradeDate;
            oneStat.dateRange = vTrade[0].tradeDate+string("-")+lastDate;
        }

        oneStat.totalInvestAndDays = calculateInvestAndDays(vIn_Money, vIn_Date, vOut_Money, vOut_Date, lastDate);        
        oneStat.totalEarnMoney = oneStat.totalReturned - oneStat.totalInvest;
        oneStat.profitRate = oneStat.totalEarnMoney / oneStat.totalInvestAndDays / 100;

        fundStatistics.push_back(oneStat);
    }

    
    //cout << endl << "after trade translating replacement......................" << endl;
    //dispFundHistoryTrade();
    
    sort(fundStatistics.begin(), fundStatistics.end());
    cout << endl << "after calculate statistics..............................." << endl;
    dispFundStatistics();
}

void BocTradeCalculator::dispFundHistoryTrade()
{
    map<string, vector<TradeItemBoc> >::iterator iter = tradeHistoryMap.begin();
    for (; iter != tradeHistoryMap.end(); iter++)
    {
      string fundId = (string)(iter->first);
      vector<TradeItemBoc> vItem = (vector<TradeItemBoc>)(iter->second);
      //sort(vItem.begin(), vItem.end());
      cout << "fundId: " << fundId << endl;
      cout << "**********************************************************" << endl;
      dispVTradeItem(vItem);
    }
}
