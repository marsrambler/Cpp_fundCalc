#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#include <functional>
#include "IcbcTradeCalculator.h"
#include "TradeItemIcbc.h"
#include "FundTradeStatistics.h"
using namespace std;

//#define _DEBUG

IcbcTradeCalculator::IcbcTradeCalculator()
{
    STATUS = 0;
}

void IcbcTradeCalculator::dispVTradeItem(vector<TradeItemIcbc>& vItem, bool bTitle)
{
    if (bTitle)
    {
      char buffer[2048] = {0};
      sprintf(buffer, "%17s %8s %20s %5s %12s %18s %12s \
%20s %10s %15s %10s %8s", 
                        "FromId", "fundId", "fundName", "stat", "Adate", "Cdate", "Fdate",
                        "tradeType", "uPrice", "money", "units", "fee");
      cout << buffer << endl;
    }

  for (int i = 0; i < vItem.size(); i++)
  {
    TradeItemIcbc& item = vItem[i];



    char buffer[2048] = {0};
    sprintf(buffer, "%17s %8s %20s %5s %12s %18s %12s \
%20s %10s %15s %10s %8s", 
                    item.applyFormId.c_str(), item.fundId.c_str(), item.fundName.c_str(),
                    item.tradeStatus.c_str(), item.applyDate.c_str(), item.confirmDate.c_str(),
                    item.finishDate.c_str(), item.tradeType.c_str(), item.unitPrice.c_str(),
                    item.tradeMoney.c_str(), item.tradeUnits.c_str(), item.tradeFee.c_str());

    cout << buffer << endl;
  }
}

void IcbcTradeCalculator::dispAllTradeTypes()
{
#ifdef _DEBUG
      map<string, int> tradeTypeMap_debug;
      for (int i = 0; i < vTradeHistory.size(); i++)
      {
        TradeItemIcbc& item = vTradeHistory[i];
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
}

bool IcbcTradeCalculator::validateTradeType()
{
  if (vTradeHistory.size() == 0)
  {
    cerr << "ERROR, the trade history records is 0!" << endl;
    return false;
  }

  getTradeTypeConfigured(tradeCfgFileName);

  for (int i = 0; i < vTradeHistory.size(); i++)
  {
    TradeItemIcbc& item = vTradeHistory[i];
    if (tradeTypeMap.find(item.tradeType) == tradeTypeMap.end())
    {
        cerr << "trade type error: " << item.tradeType << endl;
        return false;
    }
  }

  return true;
}

void IcbcTradeCalculator::transDate()
{
    for (int i = 0; i < vTradeHistory.size(); i++)
    {
      TradeItemIcbc& item = vTradeHistory[i];
      if (item.applyDate.compare(string("-")) == 0)
      {
        item.applyDate = string("0");
      }
      if (item.confirmDate.compare(string("-")) == 0)
      {
        item.confirmDate = string("0");
      }
      if (item.finishDate.compare(string("-")) == 0)
      {
        item.finishDate = string("0");
      }
      size_t pos = item.applyDate.find("-");
      while (pos != string::npos)
      {
        item.applyDate.erase(pos, 1);
        pos = item.applyDate.find("-");
      }
      pos = item.confirmDate.find("-");
      while (pos != string::npos)
      {
        item.confirmDate.erase(pos, 1);
        pos = item.confirmDate.find("-");
      }
      pos = item.finishDate.find("-");
      while (pos != string::npos)
      {
        item.finishDate.erase(pos, 1);
        pos = item.finishDate.find("-");
      }
    }
}

void IcbcTradeCalculator::transApplyNum()
{
    for (int i = 0; i < vTradeHistory.size(); i++)
    {
      TradeItemIcbc& item = vTradeHistory[i];
      if (item.applyFormId.compare(string("-")) == 0)
      {
        item.applyFormId = string("0");
      }
    }
}

int IcbcTradeCalculator::loadFundHistoryTradeRecords()
{
    if (vTradeFiles.size() == 0)
    {
      cerr << "Trade file is null, should load configuration files firstly" << endl;
      return -1;
    }

    for (int i = 0; i < vTradeFiles.size(); i++)
    {
      vector<TradeItemIcbc> vSubTrade;
      STATUS = 0;
      int subIdx = 0; 
      bool bTradeStatus = false;
    
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
          if (subIdx < vSubTrade.size() && STATUS != 3)
          {
            cerr << "ERROR file<0>: " << vTradeFiles[i] << " size:idx:stat " 
                 << vSubTrade.size() << " " << subIdx << " " << STATUS << endl;
            return -2;
          }
          STATUS++;
          subIdx = 0;
          continue;
        }
        else if (STATUS == 3 && !bTradeStatus)
        {
          int idx = 0;
          string subLine = pureLine;
          while (subLine.size() > 0)
          {
            if (idx >= vSubTrade.size())
            {
                cerr << "tradeStatus error! idx : subsize " << idx << " " << vSubTrade.size() << endl;
                return -3;
            }

            string word = subLine.substr(0,2);
            if (word.compare(string("功")) == 0)
            {
                vSubTrade[idx].tradeStatus = string("succ");
            }
            else
            {
                vSubTrade[idx].tradeStatus = string("fail");
                //cerr << "trade status word" << endl;
            }
            idx++;
            subLine = subLine.substr(2);
          }
          bTradeStatus = true;
        }
    
        TradeItemIcbc item;
        TradeItemIcbc* item_p = &item;
    
        if (STATUS == 0)
        {
      
        }
        else
        {
          if (subIdx >= vSubTrade.size())
          {
              cerr << "ERROR: pureLine is " << pureLine << " file:" << vTradeFiles[i] << endl;
              return -4;
          }
          item = vSubTrade[subIdx];
        }
       
        switch (STATUS)
        {
          case 0: { item_p->applyFormId = pureLine; break;}
          case 1: { item_p->fundId      = pureLine; break;}
          case 2: { item_p->fundName    = pureLine; break;}
          case 3: { break;}
          case 4: { item_p->applyDate   = pureLine; break;}
          case 5: { item_p->confirmDate = pureLine; break;}
          case 6: { item_p->finishDate  = pureLine; break;}
          case 7: { item_p->tradeType   = pureLine; break;}
          case 8: { item_p->unitPrice   = pureLine; break;}
          case 9: { item_p->tradeMoney  = pureLine; break;}
          case 10: {item_p->tradeUnits  = pureLine; break;}
          case 11: {item_p->tradeFee    = pureLine; break;}
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
        cerr << "ERROR file<1>: " << vTradeFiles[i] << " size:idx:stat " 
             << vSubTrade.size() << " " << subIdx << " " << STATUS << endl;
        return -5;
      }

      //dispVTradeItem(vSubTrade);
      //cout << "new sub: " << i << endl;    
      vTradeHistory.insert(vTradeHistory.end(), vSubTrade.begin(), vSubTrade.end());
      vSubTrade.clear();
    }

    transDate();
    transApplyNum();

    for (int i = 0; i < vTradeHistory.size(); i++)
    {
      TradeItemIcbc& item = vTradeHistory[i];
      if (!validateNum(item.applyFormId))
      {
        cerr << "i: " << i << " fundId: " << item.fundId << endl;
        return -6;
      }
      if (!validateNum(item.fundId))
      {
        cerr << "i: " << i << " fundId: " << item.fundId << endl;
        return -7;
      }
      if (!validateNum(item.applyDate) && !validateNum(item.confirmDate) && !validateNum(item.confirmDate))
      {
        cerr << "i: " << i << " date: " << item.applyDate 
                           << " " << item.confirmDate
                           << " " << item.finishDate << endl;
        return -8;
      }
      
      bool bval1 = validateAndFormatMoney(item.unitPrice);
      bool bval2 = validateAndFormatMoney(item.tradeMoney);
      bool bval3 = validateAndFormatMoney(item.tradeUnits);
      bool bval4 = validateAndFormatMoney(item.tradeFee);
      if (!bval1 || !bval2 || !bval3 || !bval4)
      {
        cerr << "i: " << i << " unitPrice: " << item.unitPrice 
                           << " tradeMoney: " << item.tradeMoney
                           << " tradeUnits: " << item.tradeUnits
                           << " tradeFee:   " << item.tradeFee << endl;
        return -9;
      }
    }

    //dispVTradeItem(vTradeHistory);
    dispAllTradeTypes();

    if (!validateTradeType())
    {
        cerr << "validate trade type failed" << endl;
    }

    //getFundDateValue(tradeCfgFileName);
    getCurrentHolder(tradeCfgFileName);
    //getFundTrans(tradeCfgFileName);

    return 0;
}

void IcbcTradeCalculator::distinguishFundTradeRecords()
{
    while (vTradeHistory.size() > 0)
    {
      vector<TradeItemIcbc> vSubFundTrade;
      vSubFundTrade.clear();
      int eraseTimes = 0;
    
      TradeItemIcbc item = vTradeHistory[0];
      vSubFundTrade.push_back(item);
    
      vector<TradeItemIcbc>::iterator iter = vTradeHistory.begin();
      vTradeHistory.erase(iter);
      while (iter != vTradeHistory.end())
      {
        TradeItemIcbc iterItem = (TradeItemIcbc)(*iter);
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
        cerr << "ERROR, fundId alread existed: " << item.fundId << endl;
      }
      else
      {
        tradeHistoryMap.insert(make_pair(item.fundId, vSubFundTrade));
      }
    }
}

void IcbcTradeCalculator::distinguishFundTradeRecordsByApplyId()
{
    tradeApplyIdMap.clear();

    map<string, vector<TradeItemIcbc> >::iterator iter = tradeHistoryMap.begin();
    for (; iter != tradeHistoryMap.end(); iter++)
    {
        string fundId = (string)(iter->first);
        vector<TradeItemIcbc> vItem = (vector<TradeItemIcbc>)(iter->second);

        vector<vector<TradeItemIcbc> > vvTrades;

        while (vItem.size() > 0)
        {
            vector<TradeItemIcbc> vTrades;

            vector<TradeItemIcbc>::iterator iterV = vItem.begin();
            vTrades.push_back(vItem[0]);
            string applyId = vItem[0].applyFormId;
            vItem.erase(iterV);

            while (iterV != vItem.end())
            {
                TradeItemIcbc item = (TradeItemIcbc)(*iterV);

                if (applyId.compare(item.applyFormId) == 0)
                {
                    vTrades.push_back(item);
                    vItem.erase(iterV);
                }
                else
                {
                    iterV++;
                }
            }
            vvTrades.push_back(vTrades);
        }
        tradeApplyIdMap.insert(make_pair(fundId, vvTrades));
    }
}

int IcbcTradeCalculator::mergeTradeDateForSameApplyId()
{
    tradeHistoryMap.clear();
    map<string, vector<vector<TradeItemIcbc> > >::iterator iter = tradeApplyIdMap.begin();
    for (; iter != tradeApplyIdMap.end(); iter++)
    {
        string fundId = (string)(iter->first);
        vector<TradeItemIcbc> vMergedTrades;
        vector<vector<TradeItemIcbc> > vvTrades = (vector<vector<TradeItemIcbc> >)(iter->second);
        for (int i = 0; i < vvTrades.size(); i++)
        {
            vector<TradeItemIcbc> vTrades = vvTrades[i];

            string tradeTypeName = vTrades[0].tradeType;
            TRADE_TYPE tradeType;
            map<string, TRADE_TYPE>::iterator iterType = tradeTypeMap.find(tradeTypeName);
            if (iterType == tradeTypeMap.end())
            {
                cerr << "ERROR, tradeType is " << tradeTypeName << endl;
                return -1;
            }
            else
            {
                tradeType = (TRADE_TYPE)(iterType->second);
            }

            string tradeStat = vTrades[0].tradeStatus;
            if (tradeStat.compare(string("fail")) == 0)
            {
                continue;
            }
            else if (tradeType == TRADE_USELESS)
            {
                continue;
            }
            else
            {
                if (vTrades.size() == 2)
                {
                    TradeItemIcbc item1 = vTrades[0];
                    TradeItemIcbc item2 = vTrades[1];
                    TradeItemIcbc newItem;
                    newItem.applyFormId     = item1.applyFormId;
                    newItem.fundId          = item1.fundId;
                    newItem.fundName        = item1.fundName;
                    newItem.tradeStatus     = item1.tradeStatus;
                    newItem.tradeType       = item1.tradeType;
                    
                    if (item1.applyDate.compare(string("0")) != 0)
                    {
                        newItem.applyDate   = item1.applyDate;
                    }
                    else
                    {
                        newItem.applyDate   = item2.applyDate;
                    }
                    if (item1.confirmDate.find(string("等待基金公司")) == string::npos)
                    {
                        newItem.confirmDate = item1.confirmDate;
                    }
                    else
                    {
                        newItem.confirmDate = item2.confirmDate;
                    }
                    if (item1.finishDate.compare(string("0")) != 0)
                    {
                        newItem.finishDate  = item1.finishDate;
                    }
                    else
                    {
                        newItem.finishDate  = item2.finishDate;
                    }
                    if (item1.unitPrice.compare(string("-")) != 0)
                    {
                        newItem.unitPrice   = item1.unitPrice;
                    }
                    else
                    {
                        newItem.unitPrice   = item2.unitPrice;
                    }
                    if (item1.tradeMoney.compare(string("-")) != 0)
                    {
                        newItem.tradeMoney  = item1.tradeMoney;
                    }
                    else
                    {
                        newItem.tradeMoney  = item2.tradeMoney;
                    }
                    if (item1.tradeUnits.compare(string("-")) != 0)
                    {
                        newItem.tradeUnits  = item1.tradeUnits;
                    }
                    else
                    {
                        newItem.tradeUnits  = item2.tradeUnits;
                    }
                    if (item1.tradeFee.compare(string("-")) != 0)
                    {
                        newItem.tradeFee    = item2.tradeFee;
                    }
                    else
                    {
                        newItem.tradeFee    = item2.tradeFee;
                    }
                    vMergedTrades.push_back(newItem);
                }
                else if (vTrades.size() == 1)
                {
                    if (vTrades[0].confirmDate.compare(string("等待基金公司答复")) == 0)
                    {
                        cerr << "warning, confirmDate not align with the size!" << endl;
                        continue;
                    }
                    else
                    {
                        vMergedTrades.push_back(vTrades[0]);
                    }
                }
                else
                {
                    cerr << "error, trades size is: " << vTrades.size() << endl;
                    return -2;
                }
            }
        }

        if (vMergedTrades.size() > 0)
        {
            tradeHistoryMap.insert(make_pair(fundId, vMergedTrades));
        }
    }
    return 0;
}

void IcbcTradeCalculator::calculateFundTradeStatistics()
{
    vIn_Money_total.clear();
    vIn_Date_total.clear();
    vOut_Money_total.clear();
    vOut_Date_total.clear();

    // calculate all of the statistics
    map<string, vector<TradeItemIcbc> >::iterator iterTrade = tradeHistoryMap.begin();
    for (; iterTrade != tradeHistoryMap.end(); iterTrade++)
    {
        vector<TradeItemIcbc>& vTrade = (iterTrade->second);
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
            if (tradeType == TRADE_USELESS || vTrade[i].tradeMoney.compare(string("-")) == 0)
            {
                continue;
            }
            else if (tradeType == TRADE_IN)
            {
                oneStat.totalInvest += atof(vTrade[i].tradeMoney.c_str());
                vIn_Money.push_back(atof(vTrade[i].tradeMoney.c_str()));
                vIn_Money_total.push_back(atof(vTrade[i].tradeMoney.c_str()));
                vIn_Date.push_back(vTrade[i].finishDate);
                vIn_Date_total.push_back(vTrade[i].finishDate);
            }
            else if (tradeType == TRADE_OUT)
            {
                float out_money = atof(vTrade[i].tradeMoney.c_str());
                oneStat.totalReturned += out_money;
                vOut_Money.push_back(out_money);
                vOut_Money_total.push_back(out_money);
                vOut_Date.push_back(vTrade[i].finishDate);
                vOut_Date_total.push_back(vTrade[i].finishDate);
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
            oneStat.dateRange = vTrade[0].finishDate+string("-")+string("today");
        }
        else
        {
            lastDate = vTrade[vTrade.size()-1].finishDate;
            oneStat.dateRange = vTrade[0].finishDate+string("-")+lastDate;
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

void IcbcTradeCalculator::dispFundHistoryTrade()
{
    map<string, vector<TradeItemIcbc> >::iterator iter = tradeHistoryMap.begin();
    for (; iter != tradeHistoryMap.end(); iter++)
    {
      string fundId = (string)(iter->first);
      vector<TradeItemIcbc> vItem = (vector<TradeItemIcbc>)(iter->second);
      cout << "fundId: " << fundId << endl;
      cout << "**********************************************************" << endl;
      dispVTradeItem(vItem);
      cout << endl;
    }
}

void IcbcTradeCalculator::dispFundHistoryTradeByAppId()
{
    map<string, vector<vector<TradeItemIcbc> > >::iterator iter = tradeApplyIdMap.begin();
    for (; iter != tradeApplyIdMap.end(); iter++)
    {
      string fundId = (string)(iter->first);
      vector<vector<TradeItemIcbc> > vvItem = (vector<vector<TradeItemIcbc> >)(iter->second);
      cout << "fundId: " << fundId << endl;
      cout << "**********************************************************" << endl;
      dispVTradeItem(vvItem[0]);
      for (int i = 1; i < vvItem.size(); i++)
      {
        dispVTradeItem(vvItem[i], false);
      }
      cout << endl;
    }
}
