#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#include <functional>
#include "TradeCalculator.h"
using namespace std;

//#define _DEBUG

string TradeCalculator::trimString(string szString)
{
    int s = szString.find_first_not_of(" \t");
    int e = szString.find_last_not_of(" \t\r\n");
    if (s != string::npos && e != string::npos )
    {
        string sz1 = szString.substr(s, e-s+1);
        return sz1;
    }
    return string("");
}

bool TradeCalculator::validateNum(string& numString)
{
  size_t pos = numString.find_first_not_of("0123456789");
  if (pos != string::npos)
  {
    return false;
  }
  return true;
}

bool TradeCalculator::validateAndFormatMoney(string& moneyString)
{
  size_t pos = moneyString.find_first_not_of("0123456789. ,-");
  if (pos != string::npos)
  {
    return false;
  }
  pos = moneyString.find(" ");
  while (pos != string::npos)
  {
    moneyString.erase(pos, 1);
    pos = moneyString.find(" ");
  }

  pos = moneyString.find(",");
  while (pos != string::npos)
  {
    moneyString.erase(pos, 1);
    pos = moneyString.find(",");
  }
  return true;
}

vector<string> TradeCalculator::splitTradeType(string& szLine)
{
  vector<string> vType;
  szLine = trimString(szLine);
  size_t pos = szLine.find(string(","));
  while (pos != string::npos)
  {
    string subStr = szLine.substr(0, pos);
    subStr = trimString(subStr);
    vType.push_back(subStr);
    szLine = szLine.substr(pos+1);
    szLine = trimString(szLine);
    pos = szLine.find(string(","));
  }
  if (szLine.size() > 0)
  {
    vType.push_back(szLine);
  }
  return vType;
}

void TradeCalculator::getTradeTypeConfigured(string cfgFileName)
{
    ifstream rfile(cfgFileName.c_str());
    string oneLine;
    bool bTradeTypeSec = false;
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
        if (pureLine.find(string("[trade-type]")) != string::npos)
        {
            bTradeTypeSec = true;
        }
        else if (pureLine.find(string("[")) != string::npos)
        {
            bTradeTypeSec = false;
        }
        else if (bTradeTypeSec)
        {
            if (pureLine.find(string("in=")) != string::npos)
            {
                string szLine = pureLine.substr(pureLine.find(string("in="))+string("in=").size());
                vector<string> vTradeType_in = splitTradeType(szLine);
                for (int i = 0; i < vTradeType_in.size(); i++)
                {
                    tradeTypeMap.insert(make_pair(vTradeType_in[i], TRADE_IN));
                }
            }
            else if (pureLine.find(string("out=")) != string::npos)
            {
                string szLine = pureLine.substr(pureLine.find(string("out="))+string("out=").size());
                vector<string> vTradeType_out = splitTradeType(szLine);
                for (int i = 0; i < vTradeType_out.size(); i++)
                {
                    tradeTypeMap.insert(make_pair(vTradeType_out[i], TRADE_OUT));
                }
            }
            else if (pureLine.find(string("useless=")) != string::npos)
            {
                string szLine = pureLine.substr(pureLine.find(string("useless="))+string("useless=").size());
                vector<string> vTradeType_useless = splitTradeType(szLine);
                for (int i = 0; i < vTradeType_useless.size(); i++)
                {
                    tradeTypeMap.insert(make_pair(vTradeType_useless[i], TRADE_USELESS));
                }
            }
            else
            {
                cerr << "ERROR, trade type: " << pureLine << endl;
            }
        }
    }
#ifdef _DEBUG
    map<string, TRADE_TYPE>::iterator iter = tradeTypeMap.begin();
    for (; iter != tradeTypeMap.end(); iter++)
    {
        cout << (string)(iter->first) << " : " << (TRADE_TYPE)(iter->second) << endl;
    }
#endif
}

void TradeCalculator::getCurrentHolder(string cfgFileName)
{
    ifstream rfile(cfgFileName.c_str());
    string oneLine;
    bool bHolderSec = false;
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
        if (pureLine.find(string("[current-hold]")) != string::npos)
        {
            bHolderSec = true;
        }
        else if (pureLine.find(string("[")) != string::npos)
        {
            bHolderSec = false;
        }
        else if (bHolderSec)
        {
            pos = pureLine.find(string("id_money="));
            string idMoney = pureLine.substr(pos+string("id_money=").size());
            pos = idMoney.find(string(","));
            string fundId = idMoney.substr(0,pos);
            string money = idMoney.substr(pos+1);
            fundId = trimString(fundId);
            money = trimString(money);
            float moneyHold = atof(money.c_str());
            fundHolder.insert(make_pair(fundId, moneyHold));
        }
    }
#ifdef _DEBUG
    cout << endl << "get holder" << endl;
    map<string,float>::iterator iter = fundHolder.begin();
    for (; iter != fundHolder.end(); iter++)
    {
        cout << (string)(iter->first) << " : " << (float)(iter->second) << endl;
    }
#endif
}

float TradeCalculator::calculateInvestAndDays(vector<float>& vIn_Money, vector<string>& vIn_date, 
                             vector<float>&vOut_Money, vector<string>& vOut_date, string last_date)
{
    float moneyAndDays = 0; // 10K.Y
    for (int i = 0; i < vIn_Money.size(); i++)
    {
        unsigned int dateFrom = atoi(vIn_date[i].c_str());
        unsigned int dateTo   = atoi(last_date.c_str());
        int diffDays = FundTradeStatistics::diffDays(dateFrom, dateTo);
        moneyAndDays += vIn_Money[i] / 10000 * diffDays / 365;
    }
    for (int i = 0; i < vOut_Money.size(); i++)
    {
        unsigned int dateFrom = atoi(vOut_date[i].c_str());
        unsigned int dateTo   = atoi(last_date.c_str());
        int diffDays = FundTradeStatistics::diffDays(dateFrom, dateTo);
        moneyAndDays -= vOut_Money[i] / 10000 * diffDays / 365;
    }
    return moneyAndDays;
}


void TradeCalculator::configureHistoryTradeFiles(string cfgFileName)
{
    tradeCfgFileName = cfgFileName;
    vTradeFiles.clear();

    ifstream rfile(cfgFileName.c_str());
    string oneLine;
    bool bRecordsSec = false;
    while (getline(rfile, oneLine))
    {
      size_t pos = oneLine.find(string("\r"));
      if (pos == string::npos)
      {
        pos = oneLine.find(string("\n"));
        if (pos == string::npos) break;
      }
        
      string pureLine = trimString(oneLine);
      if (pureLine.size() == 0 || pureLine.find(string("#")) != string::npos)
      {
        continue;
      }
      
//cout << "pureLine: " << pureLine << endl;

      if (pureLine.compare(string("[trade-records]")) == 0)
      {
//cout << endl << "enter 1: " << bRecordsSec << endl;
        bRecordsSec = true;
      }
      else if (pureLine.find(string("[")) != string::npos)
      {
//cout << endl << "enter 2: " << bRecordsSec << endl;
        bRecordsSec = false;
      }
      else if (bRecordsSec)
      {
//        cout << "push: " << pureLine << endl;
        vTradeFiles.push_back(pureLine);
      }
    }
}

/*
int TradeCalculator::loadFundHistoryTradeRecords()
{

}

void TradeCalculator::distinguishFundTradeRecords()
{

}

void TradeCalculator::dispFundHistoryTrade()
{

}
*/

void TradeCalculator::dispFundStatistics()
{
    char buffer[2048] = {0};
    sprintf(buffer, "%10s %40s %8s %8s %15s %15s %20s", "fundId", "fundName", "STATUS", "Earn", "Invest(10K.Y)", "profit(10K.Y)", "dateRange");
    cout << buffer << endl;
    for (int i = 0; i < fundStatistics.size(); i++)
    {
        char buffer[2048] = {0};
        sprintf(buffer, "%10s %40s %8d %8.1f %15.3f %14.2f%% %20s",
            fundStatistics[i].fundId.c_str(), fundStatistics[i].fundName.c_str(),
            fundStatistics[i].status, fundStatistics[i].totalEarnMoney, fundStatistics[i].totalInvestAndDays,
            fundStatistics[i].profitRate,fundStatistics[i].dateRange.c_str());
        cout << buffer << endl;
    }
}

void TradeCalculator::calcAndDispTotalProfit()
{
    string lastDate;
    time_t now;
    struct tm *timenow;
    time(&now);
    timenow = localtime(&now);
    char buffer[20] = {0};
    sprintf(buffer, "%04d%02d%02d", 1900+timenow->tm_year, 1+timenow->tm_mon, timenow->tm_mday);
    lastDate = string(buffer);
    float totalInvestAndDays = calculateInvestAndDays(vIn_Money_total, vIn_Date_total, vOut_Money_total, vOut_Date_total, lastDate);
    float totalEarn = 0;
    for (int i = 0; i < fundStatistics.size(); i++)
    {
        totalEarn += fundStatistics[i].totalEarnMoney;
    }
    float profitRate = totalEarn / totalInvestAndDays / 100;
    cout << endl << "total statitics" << endl;
    cout << "*****************************************************************" << endl;
    char bufferOut[2048] = {0};
    sprintf(bufferOut, "total invest&days (10K.Y): %.4f, total earn: %.4f, profit: %.2f%%", totalInvestAndDays, totalEarn, profitRate);
    cout << bufferOut << endl << endl;;
}

